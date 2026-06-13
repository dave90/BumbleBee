/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "bumblebee/execution/AggregatePRLHashTable.hpp"

#include "bumblebee/common/Log.hpp"
#include "bumblebee/function/AggregateFunction.hpp"

namespace bumblebee{

void AggregatePRLHashTable::internalizeStrings(Vector &payload, idx_t count) {
    if (count == 0 || payload.getType() != PhysicalType::STRING)
        return;
    // flatten so we can rewrite the string_t entries in place
    payload.normalify(count);
    auto data = FlatVector::getData<string_t>(payload);

    idx_t entry_sizes[STANDARD_VECTOR_SIZE];
    data_ptr_t locations[STANDARD_VECTOR_SIZE];
    bool any = false;
    for (idx_t i = 0; i < count; i++) {
        entry_sizes[i] = 0;
        // NULL and inlined (<= prefix) strings carry their bytes in the string_t
        // itself, so they need no external storage and stay valid as-is.
        if (payload.rowIsValid(i) && !data[i].isInlined()) {
            entry_sizes[i] = data[i].size();
            any = true;
        }
    }
    if (!any) return;

    stringHeap_->build(count, locations, entry_sizes);
    for (idx_t i = 0; i < count; i++) {
        if (entry_sizes[i] == 0) continue;
        const idx_t sz = data[i].size();
        memcpy(locations[i], data[i].getDataUnsafe(), sz);
        data[i] = string_t((const char *)locations[i], (uint32_t)sz);
    }
}

AggregatePRLHashTable::AggregatePRLHashTable(BufferManager &manager, const vector<LogicalType> &types,
    idx_t capacity, bool resizable, const vector<AggregateFunction *> &functions): PRLHashTable(manager, types, capacity, resizable), functions_(functions) {
    layout_.initialize(types, functions);
    tupleSize_ = layout_.getRowWidth();
    BB_ASSERT(tupleSize_ < Storage::BLOCK_SIZE);
    tuplesPerBlock_ = Storage::BLOCK_SIZE / tupleSize_;
}

AggregatePRLHashTable::~AggregatePRLHashTable() {}

void AggregatePRLHashTable::addChunk(Vector &hash, DataChunk &groups, DataChunk &payload) {
    if (groups.columnCount() == 0) {
        addChunk(payload);
        return;
    }

    // now insert or find the groups
    // find the buckets for each row in the group
    Vector addresses(LogicalTypeId::ADDRESS, groups.getSize());
    SelectionVector newGroupsSel(groups.getSize());
    idx_t newGroupCount = 0;
    findOrCreateGroups(hash, groups, addresses, newGroupCount, newGroupsSel);


    // init the states (initStates already iterates all aggregate functions via layout)
    AggregateFunction::initStates(layout_, addresses, newGroupsSel, newGroupCount);

    // now update the states. Internalize string payloads first so string
    // aggregate states reference heap memory owned by this table, not the
    // transient scan/decode buffer the payload points into.
    for (id_t i = 0;i<functions_.size();++i) {
        internalizeStrings(payload.data_[i], payload.getSize());
        AggregateFunction::updateStates(layout_, addresses,payload.data_[i],payload.getSize(), i );
    }

}

void AggregatePRLHashTable::addChunk(DataChunk &payload) {
    if (payload.getSize()==0)return;
    // set the address of the first state
    Vector addresses(PhysicalType::UBIGINT, 1);
    auto addrPtr = FlatVector::getData<data_ptr_t>(addresses);

    SelectionVector sel;
    if (entries_ == 0) {
        newBlock();
        addrPtr[0] = payloadPtrs_.back();

        // init the first state (initStates already iterates all aggregate functions via layout)
        AggregateFunction::initStates(layout_, addresses, sel, 1);
        entries_ = 1;
    }
    addrPtr[0] = payloadPtrs_.back();

    // reinterpret addresses as const vector
    auto val = Value((uint64_t)payloadPtrs_.back());
    addresses.reference(val);
    BB_ASSERT(addresses.getVectorType() == VectorType::CONSTANT_VECTOR);
    // update the payload with the first state (internalize strings first, see above)
    for (id_t i = 0;i<functions_.size();++i) {
        internalizeStrings(payload.data_[i], payload.getSize());
        AggregateFunction::updateStates(layout_, addresses,payload.data_[i],payload.getSize(), i );
    }
}

void AggregatePRLHashTable::moveAndMergeStates(idx_t count, Vector &addresses, Vector &hashes) {
    if (count == 0) return;

    // Fast path: fixed-width group keys -> merge at the tuple level. A new group is
    // a single whole-tuple memcpy; a match combines states. The validity prefix sits
    // at offset 0 (inside the compared key region) and NULL keys carry a canonical
    // sentinel value, so memcmp of the key region is exact. Avoids the generic
    // gather/scatter/init round trip that dominates high-cardinality combines.
    if (layout_.allConstant() && !types_.empty()) {
        moveAndMergeStatesFixed(count, addresses, hashes);
        return;
    }

    SelectionVector newGroupsSel(count);
    idx_t newGroupsCount = 0;
    auto groupAddresses = move(addresses, hashes, count, &newGroupsSel, newGroupsCount);

    // init new states (initStates already iterates all aggregate functions via layout)
    AggregateFunction::initStates(layout_, groupAddresses, newGroupsSel, newGroupsCount);


    AggregateFunction::combineStates(layout_, addresses, groupAddresses, FlatVector::INCREMENTAL_SELECTION_VECTOR, count);

}

void AggregatePRLHashTable::moveAndMergeStatesFixed(idx_t count, Vector &addresses, Vector &hashes) {
    // Ensure capacity for up to `count` brand-new groups so the probe below never
    // needs to resize mid-loop (which would invalidate the directory pointer).
    while (entries_ + count >= capacity_ ||
           (float)(entries_ + count) / (float)capacity_ > LOAD_FACTOR) {
        resize(capacity_ * 2);
    }

    auto srcPtrs = FlatVector::getData<data_ptr_t>(addresses);
    auto hashPtrs = FlatVector::getData<hash_t>(hashes);
    auto htEntries = (HTEntry64 *)hashesPtr_;
    auto &offsets = layout_.getOffsets();
    auto &aggregates = layout_.getAggregates();
    const idx_t aggBase = layout_.columnCount();
    // Key region = validity prefix (offset 0) + packed fixed-width group columns.
    const idx_t keyEnd = offsets[types_.size() - 1] +
                         getPhysicalTypeSize(types_[types_.size() - 1].getPhysicalType());

    for (idx_t i = 0; i < count; ++i) {
        // Software prefetch: the directory read below is a random access keyed by
        // hash and is the dominant cache miss when merging large HTs. Pull the
        // bucket for a later iteration into cache while we work on this one. The
        // pre-resize above keeps bitmask_/htEntries stable for the whole loop.
        constexpr idx_t PREFETCH_DIST = 32;
        if (i + PREFETCH_DIST < count)
            __builtin_prefetch(&htEntries[hashPtrs[i + PREFETCH_DIST] & bitmask_], 1, 0);
        auto src = srcPtrs[i];
        const auto h = hashPtrs[i];
        idx_t bucket = h & bitmask_;
        while (true) {
            auto &e = htEntries[bucket];
            if (e.pageNum_ == 0) {
                // empty bucket -> new group: copy the whole source tuple
                if (payloadPageOffset_ == tuplesPerBlock_ || payload_.empty())
                    newBlock();
                auto dest = payloadPtrs_.back() + payloadPageOffset_ * tupleSize_;
                memcpy(dest, src, tupleSize_);
                e.pageNum_ = payload_.size();
                e.pageOffset_ = payloadPageOffset_++;
                e.hash_ = h;
                entries_++;
                break;
            }
            if (e.hash_ == h) {
                auto dest = payloadPtrs_[e.pageNum_ - 1] + e.pageOffset_ * tupleSize_;
                if (memcmp(src, dest, keyEnd) == 0) {
                    // existing group: combine source state into the kept one
                    idx_t a = aggBase;
                    for (auto *aggr : aggregates) {
                        aggr->combine_(src + offsets[a], dest + offsets[a]);
                        ++a;
                    }
                    break;
                }
            }
            bucket = (bucket + 1) & bitmask_;  // capacity is a power of two
        }
    }
}

void AggregatePRLHashTable::combine(AggregatePRLHashTable &other) {
    BB_ASSERT(types_ == other.types_);
    BB_ASSERT(functions_ == other.functions_);
    BB_ASSERT(layout_.getRowWidth() == other.layout_.getRowWidth());
    BB_ASSERT(layout_.getOffsets() == other.layout_.getOffsets());
    BB_ASSERT(tupleSize_ == other.tupleSize_);

    if (other.entries_ == 0)return;

    Vector addresses(LogicalTypeId::ADDRESS);
    Vector hashes(LogicalTypeId::HASH);
    if (types_.size() == 0) {
        // no groups, so merge the first state
        BB_ASSERT(entries_ == 1);
        Vector groupAddresses(LogicalTypeId::ADDRESS, 1);
        auto groupAddressesPtr = FlatVector::getData<data_ptr_t>(groupAddresses);
        groupAddressesPtr[0] = payloadPtrs_.back();
        auto addrPtr = FlatVector::getData<data_ptr_t>(addresses);
        addrPtr[0] = other.payloadPtrs_.back();

        AggregateFunction::combineStates(layout_, addresses, groupAddresses, FlatVector::INCREMENTAL_SELECTION_VECTOR, 1);

        return ;
    }

    idx_t offset = 0;
    while (true) {
        idx_t count = other.scanRawEntries(offset, addresses, hashes);
        if (count == 0) break;
        moveAndMergeStates(count, addresses, hashes);
    }

}

void AggregatePRLHashTable::combineUnsafe(AggregatePRLHashTable &other) {
    BB_ASSERT(types_ == other.types_);
    BB_ASSERT(functions_ == other.functions_);
    BB_ASSERT(layout_.getRowWidth() == other.layout_.getRowWidth());
    BB_ASSERT(layout_.getOffsets() == other.layout_.getOffsets());
    BB_ASSERT(tupleSize_ == other.tupleSize_);

    if (other.entries_ == 0) return;

    // Total aggregation: no groups, merge the single running state
    if (types_.size() == 0) {
        BB_ASSERT(entries_ == 1);
        Vector groupAddresses(LogicalTypeId::ADDRESS, 1);
        auto groupAddressesPtr = FlatVector::getData<data_ptr_t>(groupAddresses);
        groupAddressesPtr[0] = payloadPtrs_.back();
        Vector addresses(LogicalTypeId::ADDRESS, 1);
        auto addrPtr = FlatVector::getData<data_ptr_t>(addresses);
        addrPtr[0] = other.payloadPtrs_.back();

        AggregateFunction::combineStates(layout_, addresses, groupAddresses, FlatVector::INCREMENTAL_SELECTION_VECTOR, 1);
        return;
    }

    auto destBlockCount = payload_.size();
    auto originalPayloadPageOffset = payloadPageOffset_;
    auto totalEntries = entries_ + other.entries_;

    // Resize hash directory to fit totalEntries under LOAD_FACTOR
    while (totalEntries >= capacity_ || (float)totalEntries / (float)capacity_ > LOAD_FACTOR) {
        resize(capacity_ * 2);
    }

    // Gap filling: fill the dest's last block gap with rows from the end of the source
    auto gap = tuplesPerBlock_ - payloadPageOffset_;
    auto fillCount = minValue(gap, other.entries_);
    auto remainingInOther = other.entries_ - fillCount;

    if (fillCount > 0) {
        auto destPtr = payloadPtrs_.back() + payloadPageOffset_ * tupleSize_;

        // Source rows to fill come from the end of the source's payload.
        // They span at most 2 blocks: the source's last block and the one before it.
        auto rowsFromLastBlock = minValue(fillCount, other.payloadPageOffset_);
        auto rowsFromPrevBlock = fillCount - rowsFromLastBlock;

        if (rowsFromPrevBlock > 0) {
            // Copy from source's second-to-last block (the last rowsFromPrevBlock rows of that block)
            auto srcBlock = other.payloadPtrs_[other.payload_.size() - 2];
            auto srcOffset = (tuplesPerBlock_ - rowsFromPrevBlock) * tupleSize_;
            memcpy(destPtr, srcBlock + srcOffset, rowsFromPrevBlock * tupleSize_);
            destPtr += rowsFromPrevBlock * tupleSize_;
        }
        if (rowsFromLastBlock > 0) {
            // Copy from source's last block (first rowsFromLastBlock rows starting from
            // offset other.payloadPageOffset_ - rowsFromLastBlock)
            auto srcBlock = other.payloadPtrs_.back();
            auto srcOffset = (other.payloadPageOffset_ - rowsFromLastBlock) * tupleSize_;
            memcpy(destPtr, srcBlock + srcOffset, rowsFromLastBlock * tupleSize_);
        }
        payloadPageOffset_ += fillCount;
    }

    // Append source's remaining blocks
    if (remainingInOther > 0) {
        // Number of full blocks to append: all blocks that hold the first 'remainingInOther' rows
        auto blocksToAppend = (remainingInOther + tuplesPerBlock_ - 1) / tuplesPerBlock_;
        for (idx_t i = 0; i < blocksToAppend; ++i) {
            payload_.push_back(std::move(other.payload_[i]));
            payloadPtrs_.push_back(payload_.back()->ptr());
        }
        payloadPageOffset_ = remainingInOther - (blocksToAppend - 1) * tuplesPerBlock_;
    }

    // Transfer hash entries from source's hash directory
    auto htPtr = (HTEntry64*)hashesPtr_;
    auto otherHtPtr = (HTEntry64*)other.hashesPtr_;
    for (idx_t i = 0; i < other.capacity_; ++i) {
        auto& entry = otherHtPtr[i];
        if (entry.pageNum_ == 0) continue;

        // Compute globalPos: the 0-based row index in the source
        auto pageNum = entry.pageNum_ - 1;  // 0-based block index
        auto globalPos = pageNum * tuplesPerBlock_ + entry.pageOffset_;

        uint32_t newPageNum;
        uint32_t newPageOffset;
        if (globalPos >= remainingInOther) {
            // This row was gap-filled into dest's original last block
            newPageNum = destBlockCount;  // 1-based
            newPageOffset = originalPayloadPageOffset + (globalPos - remainingInOther);
        } else {
            // This row stays in an appended block
            newPageNum = destBlockCount + pageNum + 1;  // 1-based, offset by dest's original block count
            newPageOffset = entry.pageOffset_;
        }

        // Insert into dest's hash directory via linear probing
        auto bucket = entry.hash_ & bitmask_;
        while (htPtr[bucket].pageNum_ != 0) {
            bucket = (bucket + 1) % capacity_;
        }
        htPtr[bucket].hash_ = entry.hash_;
        htPtr[bucket].pageNum_ = newPageNum;
        htPtr[bucket].pageOffset_ = newPageOffset;
    }

    entries_ = totalEntries;

    // Merge string heaps
    stringHeap_->merge(*other.stringHeap_);
}

void AggregatePRLHashTable::findAddresses(Vector &hash, DataChunk &groups, SelectionVector &sel, Vector &addresses, idx_t &matchedGroups) {
    matchedGroups = 0;
    findOrCreateGroups(hash, groups, addresses,matchedGroups, false, sel);

    if (matchedGroups < groups.getSize()) {
        // not all groups are presents, so filter out the missing groups
        addresses.slice(sel, matchedGroups);
        groups.slice(sel, matchedGroups);
    }
}

void AggregatePRLHashTable::fetchAggregates(Vector &hash, DataChunk &groups, DataChunk &result, SelectionVector &sel) {
    BB_ASSERT(result.columnCount() == functions_.size());
    BB_ASSERT(result.getCapacity() >= groups.getSize());
    if (entries_ == 0 || groups.getSize() == 0) {
        result.setCardinality(0);
        return;
    }

    Vector addresses(LogicalTypeId::ADDRESS, groups.getSize());
    idx_t matchedGroups;
    findAddresses(hash, groups, sel, addresses, matchedGroups);
    result.setCardinality(matchedGroups);
    if (matchedGroups == 0) {
        result.setCardinality(0);
        groups.setCardinality(0);
        return;
    }
    // copy the agg results value in the result chunk (finalizeStates already iterates all aggregate functions via layout)
    AggregateFunction::finalizeStates(layout_, addresses, result, matchedGroups);

}

void AggregatePRLHashTable::fetchAggregates(Vector &hash, DataChunk &groups, Vector &result, idx_t aggIndex,
    SelectionVector &sel) {
    if (groups.getSize() == 0) {
        return;
    }

    Vector addresses(LogicalTypeId::ADDRESS, groups.getSize());
    idx_t matchedGroups;
    findAddresses(hash, groups, sel, addresses, matchedGroups);
    if (matchedGroups == 0) {
        groups.setCardinality(0);
        return;
    }
    // copy the agg results value in the result chunk
    AggregateFunction::finalizeStates(layout_, addresses, result, aggIndex, matchedGroups);
}

void AggregatePRLHashTable::fetchAggregates(DataChunk &result) {
    // fetch the aggregates from the first state
    BB_ASSERT(result.columnCount() == functions_.size());
    if (entries_ == 0) {
        result.setCardinality(0);
        return;
    }
    Vector addresses(LogicalTypeId::ADDRESS, 1);
    auto addrPtr = FlatVector::getData<data_ptr_t>(addresses);
    addrPtr[0] = payloadPtrs_.back();

    // finalizeStates already iterates all aggregate functions via layout
    AggregateFunction::finalizeStates(layout_, addresses, result, 1);

    result.setCardinality(1);
    // transform the vectors in the results as constant vector
    // as the result is always one without groups
    for (idx_t i = 0;i<result.columnCount();++i) {
        auto val = result.getValue(i, 0);
        result.data_[i].reference(val);
    }

}

void AggregatePRLHashTable::fetchAggregates(Vector &result, idx_t aggIndex) {
    Vector addresses(LogicalTypeId::ADDRESS, 1);
    auto addrPtr = FlatVector::getData<data_ptr_t>(addresses);
    addrPtr[0] = payloadPtrs_.back();

    AggregateFunction::finalizeStates(layout_, addresses,result, aggIndex, 1);

    // transform the vectors in the results as constant vector
    // as the result is always one without groups
    auto val = result.getValue(0);
    result.reference(val);
}

idx_t AggregatePRLHashTable::scanEntries(idx_t offset, DataChunk &groups, Vector &addresses, idx_t size) {
    // Use parent's scan which builds addresses and gathers group values
    return PRLHashTable::scan(offset, groups, addresses, size);
}

idx_t AggregatePRLHashTable::scanWithAggregates(idx_t offset, DataChunk &groups, Vector &aggResult, idx_t aggIndex, idx_t size) {
    Vector addresses(LogicalTypeId::ADDRESS, size);
    idx_t toScan = scanEntries(offset, groups, addresses, size);
    if (toScan == 0) return 0;

    // Finalize aggregate states from addresses
    AggregateFunction::finalizeStates(layout_, addresses, aggResult, aggIndex, toScan);
    return toScan;
}

idx_t AggregatePRLHashTable::scanWithAggregates(idx_t offset, DataChunk &groups, DataChunk &aggResults, idx_t size) {
    BB_ASSERT(functions_.size() == aggResults.columnCount());
    Vector addresses(LogicalTypeId::ADDRESS, size);
    idx_t toScan = scanEntries(offset, groups, addresses, size);
    if (toScan == 0) {
        aggResults.setCardinality(0);
        return 0;
    }

    // Finalize all aggregate states from addresses
    for (idx_t i = 0; i < functions_.size(); ++i) {
        AggregateFunction::finalizeStates(layout_, addresses, aggResults.data_[i], i, toScan);
    }
    aggResults.setCardinality(toScan);
    return toScan;
}

vector<LogicalType> AggregatePRLHashTable::getPayloadsTypes() {
    vector<LogicalType> types;
    for (auto& func: functions_) {
        types.push_back(func->result_);
    }
    return types;
}
}
