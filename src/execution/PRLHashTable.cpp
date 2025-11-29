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
#include "bumblebee//execution/PRLHashTable.hpp"

#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/function/AggregateFunction.hpp"

namespace bumblebee{
PRLHashTable::PRLHashTable(BufferManager &manager, const vector<ConstantType> &types, idx_t capacity,
    bool resizable) : bufferManager_(manager){
    initialize( types,  capacity,resizable);
}

PRLHashTable::PRLHashTable(BufferManager &manager): bufferManager_(manager) {
}

void PRLHashTable::initialize(const vector<ConstantType> &types, idx_t capacity,
                              bool resizable) {
    capacity_ = 0;
    types_ = types;
    resizable_ = resizable;
    entries_ = 0;
    payloadPageOffset_ = 0;
    bitmask_ = capacity -1;
    BB_ASSERT(capacity != 0 && (capacity & (capacity - 1)) == 0); // capacity should be power of 2

    layout_.initialize(types);
    tupleSize_ = layout_.getRowWidth();
    BB_ASSERT(tupleSize_ < Storage::BLOCK_SIZE);
    tuplesPerBlock_ = Storage::BLOCK_SIZE / tupleSize_;

    hashes_ = bufferManager_.allocate(Storage::BLOCK_SIZE);
    hashesPtr_ = hashes_->ptr();

    PRLHashTable::resize(capacity, true);
    stringHeap_ = std::make_unique<RowDataCollection>(bufferManager_, (idx_t)Storage::BLOCK_SIZE, 1, true);

}

void PRLHashTable::addChunk(Vector &hash, DataChunk &chunk) {
    Vector addresses(UBIGINT, chunk.getSize());

    findOrCreateGroups(hash, chunk, addresses);
}

void PRLHashTable::addChunk(DataChunk &chunk) {
    Vector hash(UBIGINT, chunk.getSize());
    chunk.hash(hash);
   addChunk(hash, chunk);
}

idx_t PRLHashTable::scan(idx_t offset, DataChunk &result, idx_t size) {
    BB_ASSERT(offset < entries_);
    BB_ASSERT(result.getCapacity() >= size);
    BB_ASSERT(result.columnCount() == types_.size());
    // find the addresses

    auto remaining = entries_ - offset;
    auto toScan = minValue(size, remaining);
    Vector addresses(UBIGINT, toScan);
    auto addrsPtr = FlatVector::getData<data_ptr_t>(addresses);
    auto chunkIdx = offset / tuplesPerBlock_;
    auto chunkOffset = (offset % tuplesPerBlock_) * tupleSize_;
    BB_ASSERT(chunkOffset + tupleSize_ <= Storage::BLOCK_SIZE);

    auto readPtr = payloadPtrs_[chunkIdx++];
    for (idx_t i = 0; i < toScan; i++) {
        BB_ASSERT(readPtr);
        addrsPtr[i] = readPtr + chunkOffset;
        chunkOffset += tupleSize_;
        if (chunkOffset >= tuplesPerBlock_ * tupleSize_) {
            readPtr = (chunkIdx < payloadPtrs_.size())? payloadPtrs_[chunkIdx++]: nullptr;
            chunkOffset = 0;
        }
    }

    // now fetch the vectors
    for (id_t i = 0;i<types_.size();++i) {
        BB_ASSERT(result.data_[i].getType() == types_[i]);
        BB_ASSERT(result.data_[i].getVectorType() == VectorType::FLAT_VECTOR);
        RowOperations::gather(addresses, FlatVector::INCREMENTAL_SELECTION_VECTOR,
            result.data_[i], FlatVector::INCREMENTAL_SELECTION_VECTOR, toScan,
            layout_.getOffsets()[i]);
    }

    result.setCardinality(toScan);
    return result.getSize();
}

void PRLHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses) {
    idx_t matchedCount = 0;
    idx_t newGroupsCount = 0;
    findOrCreateGroupsInternal(hash, groups, addresses, matchedCount, newGroupsCount, true, nullptr, nullptr );
}

void PRLHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses,
    idx_t &matchedCount, bool createGroups, SelectionVector &matchedSel) {
    idx_t newGroupsCount = 0;
    findOrCreateGroupsInternal(hash, groups, addresses, matchedCount, newGroupsCount, createGroups, &matchedSel, nullptr );
}

void PRLHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses,
    idx_t &newGroupsCount, SelectionVector &newGroupSel) {
    idx_t matchedCount = 0;
    findOrCreateGroupsInternal(hash, groups, addresses, matchedCount, newGroupsCount, true, nullptr, &newGroupSel );
}

void PRLHashTable::combine(PRLHashTable &other) {
    if (other.entries_ == 0)
        return;

    BB_ASSERT(types_ == other.types_);

    idx_t position = 0;
    DataChunk group;
    group.initialize(other.types_);
    while (position < other.entries_) {
        other.scan(position, group);
        addChunk( group);
        position += minValue<idx_t>(STANDARD_VECTOR_SIZE, other.entries_ - position);
    }
}

struct PRLPartitionInfo {
    explicit PRLPartitionInfo (): groupAddresses_(UBIGINT, STANDARD_VECTOR_SIZE), hashes_(UBIGINT, STANDARD_VECTOR_SIZE), size_(0) {
        addressesPtr_ = FlatVector::getData<data_ptr_t>(groupAddresses_);
        hashesPtr_ = FlatVector::getData<uint64_t>(hashes_);
    };

    Vector groupAddresses_;
    data_ptr_t *addressesPtr_;
    Vector hashes_;
    uint64_t * hashesPtr_;
    idx_t size_;
};

void PRLHashTable::partition(vector<distinct_ht_ptr_t> &partitions, idx_t shift) {
    vector<PRLPartitionInfo> partitionsInfo(partitions.size());
    for (idx_t i = 0; i < capacity_; ++i) {
        auto hashEntryPtr = (HTEntry64*)hashesPtr_ + i;
        if (hashEntryPtr->pageNum_ == 0 ) continue;
        auto hash = hashEntryPtr->hash_;
        auto p = hash >> shift;
        BB_ASSERT(p < partitionsInfo.size());
        auto &info = partitionsInfo[p];
        info.hashesPtr_[info.size_] = hash;
        info.addressesPtr_[info.size_] = payloadPtrs_[ hashEntryPtr->pageNum_ -1] + (hashEntryPtr->pageOffset_ * tupleSize_);;
        info.size_++;
        if (info.size_ >= STANDARD_VECTOR_SIZE) {
            // merge with the partition table
            if (!partitions[p])
                partitions[p] = distinct_ht_ptr_t(new PRLHashTable(bufferManager_, types_));
            partitions[p]->move(info.groupAddresses_, info.hashes_, info.size_);
            info.size_ = 0;
        }
    }


    idx_t infoIdx = 0;
    idx_t totalCount = 0;
    for (auto &pInfo : partitionsInfo) {
        if (pInfo.size_ == 0 && !partitions[infoIdx]) {
            infoIdx++;
            continue;
        }
        if (!partitions[infoIdx])
            partitions[infoIdx] = distinct_ht_ptr_t(new PRLHashTable(bufferManager_, types_));
        partitions[infoIdx]->move(pInfo.groupAddresses_, pInfo.hashes_, pInfo.size_);
        pInfo.size_ = 0;

        partitions[infoIdx]->stringHeap_->merge(*stringHeap_);
        totalCount += partitions[infoIdx]->getSize();
        infoIdx++;
    }
    BB_ASSERT(totalCount == entries_);

}

idx_t PRLHashTable::getSize() const {
    return entries_;
}

idx_t PRLHashTable::getCapacity() const {
    return capacity_;
}

string PRLHashTable::toString(bool compact) {
    string result = "ProbeRLHashTable - "
        +std::to_string(capacity_)+" - "
        +std::to_string(entries_)+" - "
        +std::to_string(resizable_) +"\n";

    result +="Hash :\n";
    for (idx_t i=0;i<capacity_;++i) {
        auto entry = (HTEntry64*)hashesPtr_ + i;
        if ((entry->pageNum_ == 0 && !compact) || entry->pageNum_ > 0) {
            result += std::to_string(entry->pageNum_)+" - "+std::to_string(entry->pageOffset_)
            +" - "+std::to_string(entry->hash_)+"\n";
        }
    }
    result += "\nPayload : \n";
    payloadApply([&](idx_t page_nr, idx_t page_offset, data_ptr_t ptr) {
        //std::string address = std::format("{}", static_cast<uint64_t>(ptr));
        std::string address = std::format("{}", (uint64_t)(ptr));
        result += std::to_string(page_nr)+" - "+std::to_string(page_offset)+" - "+address;
        for (idx_t i=0;i<layout_.getTypes().size();++i) {
            auto colOffset = layout_.getOffsets()[i];
            auto col = ptr +colOffset;
            result += " - "+Value::cast(types_[i], col).toString();
        }
        for (idx_t i=0;i<layout_.getAggregates().size();++i) {
            auto colOffset = layout_.getOffsets()[layout_.columnCount()+i];
            auto col = ptr +colOffset;
            auto sizeResult = getCTypeSize(layout_.getAggregates()[i]->result_);
            auto resultData = std::unique_ptr<data_t[]>(new data_t[sizeResult]);
            layout_.getAggregates()[i]->finalize_(col, resultData.get());
            result += " - "+Value::cast(layout_.getAggregates()[i]->result_, resultData.get()).toString();
            // address = std::format("{}", static_cast<void*>(col));
            // result += " - "+address;
        }
        result += "\n";
    });

    return result;
}

vector<ConstantType> PRLHashTable::getTypes() const {
    return types_;
}

void rehash(const HTEntry64* __restrict hashes, HTEntry64* __restrict newHashes, idx_t capacity, idx_t bitmask)
{
    idx_t newCapacity = bitmask +1;

    for (idx_t i = 0; i < capacity; ++i) {
        const auto& entry = hashes[i];
        if (entry.pageNum_ == 0) continue;
        auto bucket = entry.hash_ & bitmask;
        while (newHashes[bucket].pageNum_ != 0) {
            ++bucket;
            if (bucket >= newCapacity)bucket = 0;
        }
        newHashes[bucket].pageNum_    = entry.pageNum_;
        newHashes[bucket].pageOffset_ = entry.pageOffset_;
        newHashes[bucket].hash_       = entry.hash_;
    }
}

void PRLHashTable::resize(idx_t size, bool initResize) {
    BB_ASSERT(size > capacity_);
    BB_ASSERT(size != 0 && (size & (size - 1)) == 0); // new size should be power of 2
    BB_ASSERT(resizable_ || initResize);

    auto byteSize =  (size * sizeof(HTEntry64) > Storage::BLOCK_SIZE)? size * sizeof(HTEntry64): Storage::BLOCK_SIZE;
    auto hashes = bufferManager_.allocate(byteSize);
    auto hashesPtr = hashes->ptr();
    memset(hashesPtr, 0, byteSize);

    rehash((HTEntry64 *)hashesPtr_, (HTEntry64 *)hashesPtr, capacity_, size-1);

    hashes_ = std::move(hashes);
    hashesPtr_ = hashesPtr;
    hashesEndPtr_ = hashesPtr_ + byteSize;
    capacity_ = size;
    bitmask_ = size -1;
}



template <class FUNC>
void PRLHashTable::payloadApply(FUNC fun) {
    if (entries_ == 0) {
        return;
    }
    idx_t apply_entries = entries_;
    idx_t page_nr = 0;
    idx_t page_offset = 0;

    for (auto &payload_chunk_ptr : payloadPtrs_) {
        auto this_entries = minValue(tuplesPerBlock_, apply_entries);
        page_offset = 0;
        auto end = payload_chunk_ptr + this_entries * tupleSize_;
        for (data_ptr_t ptr = payload_chunk_ptr; ptr < end; ptr += tupleSize_)
            fun(page_nr, page_offset++, ptr);

        apply_entries -= this_entries;
        page_nr++;
    }
    BB_ASSERT(apply_entries == 0);
}


void PRLHashTable::newBlock() {
    auto pin = bufferManager_.allocate(Storage::BLOCK_SIZE);
    payload_.push_back(std::move(pin));
    payloadPtrs_.push_back(payload_.back()->ptr());
    payloadPageOffset_ = 0;
}

Vector PRLHashTable::move(Vector &addresses, Vector &hashes, idx_t count, SelectionVector* newGroupSel, idx_t& newGroupsCount) {
    BB_ASSERT(addresses.getType() == UBIGINT);
    BB_ASSERT(hashes.getType() == UBIGINT);

    Vector groupAddresses(UBIGINT);
    if (count == 0) return groupAddresses;

    DataChunk groups;
    groups.initialize(types_);
    groups.setCardinality(count);
    for (idx_t i = 0; i < groups.columnCount(); i++) {
        auto &column = groups.data_[i];
        const auto colOffset = layout_.getOffsets()[i];
        RowOperations::gather(addresses, FlatVector::INCREMENTAL_SELECTION_VECTOR, column,
                              FlatVector::INCREMENTAL_SELECTION_VECTOR, count, colOffset);
    }

    Vector hash(UBIGINT);
    groups.hash(hash);


    idx_t matchedCount=0;
    findOrCreateGroupsInternal(hash, groups, groupAddresses, matchedCount, newGroupsCount, true, nullptr, newGroupSel );
    return groupAddresses;
}

Vector PRLHashTable::move(Vector &addresses, Vector &hashes, idx_t count) {
    idx_t newGroupsCount = 0;
    return move(addresses, hashes, count, nullptr, newGroupsCount);
}

void PRLHashTable::findOrCreateGroupsInternal(Vector &hash, DataChunk &groups,
                                              Vector &addresses, idx_t &matchedCount, idx_t &newGroupsCount, bool createGroups, SelectionVector *matchedSel,
                                              SelectionVector *newGroupSel) {

    idx_t size = groups.getSize();
    while (createGroups &&
        (groups.getSize() + entries_ >= capacity_  || (float)(entries_ + size) / (float)capacity_ > LOAD_FACTOR  )) {
        resize(capacity_ * 2);
    }

    BB_ASSERT(entries_ + size < capacity_ );
    BB_ASSERT(groups.columnCount() == types_.size());
    BB_ASSERT(hash.getType() == UBIGINT);

    hash.normalify(size);
    auto groupsHashPtr = FlatVector::getData<uint64_t>(hash);
    // compute the buckets
    Vector buckets(UBIGINT, size);

    auto val = Value(bitmask_);
    Vector mask(val);
    BB_ASSERT(mask.getVectorType() == VectorType::CONSTANT_VECTOR);
    VectorOperations::lAnd(hash, mask, buckets, size);
    buckets.normalify(size);
    auto bucketsPtr = FlatVector::getData<uint64_t>(buckets);

    BB_ASSERT(addresses.getType() == UBIGINT);
    addresses.normalify(size);
    auto addressesPtr = FlatVector::getData<data_ptr_t>(addresses);

    auto groupsData = groups.orrify();

    SelectionVector selVector;
    // sel of index and buckets of new groups
    SelectionVector emptySel(size);
    // sel of index and buckets of the to be matched groups
    SelectionVector compareSel(size);
    // sel of the no match groups
    SelectionVector noMatchSel(size);
    SelectionVector nms(size);


    idx_t remainingEntries = size;
    matchedCount = 0;
    newGroupsCount = 0;
    while (remainingEntries > 0) {
        // counter for the current iteration
        idx_t newEntryCount = 0;
        idx_t newNeedCompareCount = 0;
        idx_t newNoMatchCount = 0;

        // first figure out if it belongs to a full or empty group
        for (idx_t i = 0; i < remainingEntries; i++) {

            idx_t index = selVector.getIndex(i);
            auto bucket = bucketsPtr[index];
            auto ghash = groupsHashPtr[index];
            auto htEntry = ((HTEntry64*)hashesPtr_) + bucket;
            if (htEntry->pageNum_ == 0) { // page 0 -> empty bucket
                if (!createGroups)continue;

                // create new block if full or no blocks
                if (payloadPageOffset_ == tuplesPerBlock_ || payload_.empty())
                    newBlock();

                auto entry = payloadPtrs_.back() + (payloadPageOffset_ * tupleSize_);
                BB_ASSERT(payloadPageOffset_ < tuplesPerBlock_);

                htEntry->pageNum_ = payload_.size(); // assign to size() as 0 is for empty buckets
                htEntry->pageOffset_ = payloadPageOffset_++;
                htEntry->hash_ = ghash;

                emptySel.setIndex(newEntryCount++, index);
                entries_++;
                addressesPtr[index] = entry;
                if (newGroupSel)
                    newGroupSel->setIndex(newGroupsCount++, index);
            }else {
                // bucket occupied, check the hash
                if (htEntry->hash_ == ghash) {
                    // hash is equal try to match in the next batch
                    compareSel.setIndex(newNeedCompareCount++, index);
                    auto entry = payloadPtrs_[ htEntry->pageNum_ -1] + (htEntry->pageOffset_ * tupleSize_);
                    addressesPtr[index] = entry;
                }else
                    noMatchSel.setIndex(newNoMatchCount++, index);
            }


        }


        // Scatter the new groups in the row storage
        RowOperations::scatter(groups, groupsData.get(), layout_, addresses, *stringHeap_, emptySel, newEntryCount);

        // Now let's try to match the groups with same hash of our ht
        idx_t noMatchCount = 0;
        idx_t matchCount = RowOperations::equal(groups, groupsData.get(), layout_, addresses, compareSel, newNeedCompareCount, &nms, noMatchCount);
        if (matchedSel) {
            // add the matched index
            for (idx_t j = 0; j< matchCount; j++) {
                matchedSel->setIndex(matchedCount++, compareSel.getIndex(j));
            }
        }else
            matchedCount += matchCount;

        // add the non match index to noMatchSel
        for (idx_t j = 0; j< noMatchCount; j++)
            noMatchSel.setIndex(newNoMatchCount++, nms.getIndex(j));


        // each of the entries that do not match we move them to the next bucket in the HT
        for (idx_t j = 0; j< newNoMatchCount; j++) {
            idx_t idx = noMatchSel.getIndex(j);
            bucketsPtr[idx]++;
            if (bucketsPtr[idx] >= capacity_) {
                bucketsPtr[idx] = 0;
            }
        }
        // next iteration take in consideration only the non-matched
        selVector.initialize(noMatchSel);
        remainingEntries = newNoMatchCount;

    }


}
}
