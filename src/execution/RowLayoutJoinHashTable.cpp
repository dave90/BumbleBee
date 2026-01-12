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
#include "bumblebee/execution/RowLayoutJoinHashTable.hpp"

#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{
RowLayoutJoinHashTable::RowLayoutJoinHashTable(BufferManager &manager, const vector<LogicalType> &types, const vector<idx_t> &key_columns, const vector<idx_t> &payload_columns,idx_t capacity,
        bool resizable): PRLHashTable(manager),
    keyColumns_(key_columns),
    payloadColumns_(payload_columns) {
    // create the types where keys are in the beginning and after the payloads
    cols_ = keyColumns_;
    cols_.insert(cols_.end(), payloadColumns_.begin(), payloadColumns_.end());
    vector<LogicalType> orderedTypes;
    for (auto& idx: key_columns)
        orderedTypes.push_back(types[idx]);
    keyLayout_.initialize(orderedTypes); // init with keys cols
    for (auto& idx: payload_columns)
        orderedTypes.push_back(types[idx]);
    // last column contains hash
    orderedTypes.push_back(LogicalTypeId::HASH);

    initialize(orderedTypes, capacity, resizable);
}

bool RowLayoutJoinHashTable::isReady() {
    return ready_;
}

vector<idx_t> RowLayoutJoinHashTable::getKeys() {
    return keyColumns_;
}

vector<idx_t> RowLayoutJoinHashTable::getPayloads() {
    return payloadColumns_;
}

void RowLayoutJoinHashTable::newBlocks(idx_t size, Vector& addresses) {
    vector<BufferHandle*> handles;
    idx_t startingOffset;
    {
        lock_guard lock(mutex_);

        // ensure we have a block with space
        if (payload_.empty() || payloadPageOffset_ == tuplesPerBlock_) {
            newBlock();
        }

        startingOffset = payloadPageOffset_;
        BB_ASSERT(tuplesPerBlock_ * payload_.size() > entries_);
        auto freeEntries = tuplesPerBlock_ - payloadPageOffset_;;

        auto remaining = size;
        while (remaining > 0) {
            handles.push_back(payload_.back().get());
            if (remaining <= freeEntries) {
                payloadPageOffset_ += remaining;
                remaining = 0;
            }else {
                remaining -= freeEntries;
                newBlock();
                freeEntries = tuplesPerBlock_;
            }
        }

        entries_ += size;
        BB_ASSERT(tuplesPerBlock_ * payload_.size() >= entries_);
    }

    // now fill the addresses
    addresses.normalify(size);
    auto addressesPtr = FlatVector::getData<data_ptr_t>(addresses);
    idx_t idx = 0;
    for (auto& handle: handles) {
        for (idx_t offset = startingOffset; offset < tuplesPerBlock_ && idx < size; ++offset) {
            addressesPtr[idx++] = handle->ptr() + (offset * tupleSize_);
        }
        startingOffset = 0;
    }
    BB_ASSERT(size == idx);
}

bool RowLayoutJoinHashTable::checksTypes(const vector<LogicalType>& predicateTypes) {
    for (idx_t i = 0; i < cols_.size(); ++i) {
        if (types_[i] != predicateTypes[cols_[i]])
            return false;
    }
    return true;
}

void RowLayoutJoinHashTable::addChunk(DataChunk &chunk) {
    if (!chunk.getSize()) return;
    // because we are adding new data set ready to false
    ready_ = false;
    // only a chunk x block
    auto size = chunk.getSize();
    BB_ASSERT(size <= STANDARD_VECTOR_SIZE);
    Vector hash(LogicalTypeId::HASH, size);
    chunk.hash(hash, keyColumns_);
    Vector addresses(LogicalTypeId::ADDRESS, size);
    DataChunk orderedChunk;
    orderedChunk.initializeEmpty(types_);
    // first the keys and after the payload cols
    orderedChunk.reference(chunk, cols_);
    // last column is the hash
    orderedChunk.data_[orderedChunk.columnCount() -1].reference(hash);

    // generate a new block
    auto data = orderedChunk.orrify();
    newBlocks(size, addresses);

    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(orderedChunk, data.get(), layout_, addresses, *stringHeap_, sel, size);
    int x = 10;
}


void addNewEntriesInHash(idx_t idx, vector<buffer_handle_ptr_t>& payload, idx_t entries ,idx_t tupleSize, idx_t tuplesPerBlock, idx_t hashOffset, idx_t bitmask, HTEntry64* __restrict hashEntries,  idx_t capacity) {
    BB_ASSERT(idx < entries);
    idx_t apply_entries = entries - idx;
    idx_t page_nr = idx / tuplesPerBlock;
    idx_t page_offset = idx % tuplesPerBlock;

    for (; page_nr < payload.size(); ++page_nr) {
        auto payload_chunk_ptr = payload[page_nr]->ptr() + page_offset * tupleSize;
        auto this_entries = minValue(tuplesPerBlock - page_offset, apply_entries);
        auto end = payload_chunk_ptr + this_entries * tupleSize;
        for (data_ptr_t ptr = payload_chunk_ptr; ptr < end; ptr += tupleSize) {
            auto hash = load<hash_t>(ptr+hashOffset);
            auto bucket = hash & bitmask;
            while (hashEntries[bucket].pageNum_ != 0) {
                ++bucket;
                if (bucket >= capacity) bucket = 0;
            }
            hashEntries[bucket].pageNum_    = page_nr + 1; // as 0 is for page empty
            hashEntries[bucket].pageOffset_ = page_offset++;
            hashEntries[bucket].hash_       = hash;
        }

        page_offset = 0;
        apply_entries -= this_entries;
    }
    BB_ASSERT(apply_entries == 0);
}

void RowLayoutJoinHashTable::finalize() {
    // build the hash table
    lock_guard lock(mutex_);
    if (ready_) return;
    BB_ASSERT(entries_ > idxEntries_);
    if(entries_ >= capacity_  || (float)(entries_) / (float)capacity_ > LOAD_FACTOR  ) {
        auto newsSize  = nextPowerOfTwo(entries_*2);
        resize(newsSize);
    }
    auto hashOffset = layout_.getOffsets()[types_.size() -1];
    addNewEntriesInHash(idxEntries_, payload_, entries_, tupleSize_, tuplesPerBlock_, hashOffset, bitmask_, (HTEntry64 *)hashesPtr_, capacity_);

    ready_ = true;
    idxEntries_ = entries_;
}

bool RowLayoutJoinHashTable::checkKeysAndPayloads(const vector<idx_t>& keys, const vector<idx_t>& payloads) {
    return compareVectors(keys, keyColumns_) && compareVectors(payloads, payloadColumns_);
}

void RowLayoutJoinHashTable::probe(idx_t &ltuple, idx_t &rtuple, DataChunk &lchunk, Vector &lhash,SelectionVector &lsel, SelectionVector &rsel, DataChunk &result) {
    BB_ASSERT(keyColumns_.size() == lchunk.columnCount());
    auto size = lchunk.getSize();
    Vector addresses(LogicalTypeId::ADDRESS, STANDARD_VECTOR_SIZE);
    auto addressesPtr = FlatVector::getData<data_ptr_t>(addresses);

    auto hashPtr = FlatVector::getData<uint64_t>(lhash);

    idx_t matchCount = 0;
    while (ltuple < size && matchCount < STANDARD_VECTOR_SIZE) {
        auto hash = hashPtr[ltuple];
        auto bucket = hash & bitmask_;
        while (true) {
            // scan the ht until we found an empty bucket
            auto idx = bucket + rtuple;
            // circular scan
            if (idx  >= capacity_)
                idx -= capacity_;
            BB_ASSERT(idx < capacity_);
            auto htEntry = ((HTEntry64*)hashesPtr_) +idx;
            if (htEntry->pageNum_ == 0) break;
            if (hash == htEntry->hash_) {
                // same hash put in the address vector
                auto entry = payloadPtrs_[ htEntry->pageNum_ -1] + (htEntry->pageOffset_ * tupleSize_);
                lsel.setIndex(matchCount, ltuple);
                rsel.setIndex(matchCount, matchCount);
                addressesPtr[matchCount++] = entry;
            }
            ++rtuple; // next bucket
            if (matchCount >= STANDARD_VECTOR_SIZE) {
                // stop as the chunk is full
                break;
            }
        }
        if (matchCount < STANDARD_VECTOR_SIZE) {
            rtuple = 0;
            ++ltuple;
        }
    }

    if (matchCount == 0) {
        // no data found
        result.setCapacity(0);
        result.setCardinality(0);
        return;
    }

    auto vec_data = lchunk.orrify();
    idx_t noMatchCount;
    auto finalCount = RowOperations::equal(lchunk, vec_data.get(), keyLayout_, addresses, lsel,rsel,matchCount,nullptr, noMatchCount );
    // construct the result chunk
    vector<LogicalType> payloadTypes;
    for (idx_t i=0;i<payloadColumns_.size();++i)
        payloadTypes.push_back(types_[payloadColumns_[i]]);
    result.initializeEmpty(payloadTypes);
    // fetch the payload columns
    for (idx_t idx=0; idx < payloadTypes.size(); ++idx) {
        result.data_[idx].initialize(false, finalCount);
        idx_t offset = layout_.getOffsets()[keyColumns_.size() + idx];
        RowOperations::gather(addresses, rsel, result.data_[idx], FlatVector::INCREMENTAL_SELECTION_VECTOR,finalCount, offset);
    }
    result.setCapacity(finalCount);
    result.setCardinality(finalCount);
}

void findAddresses(idx_t capacity, idx_t tupleSize, data_ptr_t hashesPtr, vector<data_ptr_t>& payloadPtrs,
    const SelectionVector* sel, idx_t size,Vector &buckets, Vector &lhash, SelectionVector &nonMatchSel, idx_t& noMatchCount,SelectionVector& toMatch , idx_t& toMatchCount, Vector& addresses) {
    if (size == 0) {
        toMatchCount = 0;
        return;
    }
    auto addressesPtr = FlatVector::getData<data_ptr_t>(addresses);
    auto hashPtr = FlatVector::getData<uint64_t>(lhash);
    auto bucketPtr = FlatVector::getData<uint64_t>(buckets);
    if (!sel)
        sel = &FlatVector::INCREMENTAL_SELECTION_VECTOR;

    for (idx_t i=0; i < size; ++i) {
        auto idx = sel->getIndex(i);
        auto hash = hashPtr[idx];
        auto bucket = bucketPtr[idx];
        while (true) {
            // scan the ht until we found an empty bucket
            // circular scan
            if (bucket  >= capacity)
                bucket = 0;
            BB_ASSERT(idx < capacity);
            auto htEntry = ((HTEntry64*)hashesPtr) + bucket;
            if (htEntry->pageNum_ == 0) {
                // no match for idx row
                nonMatchSel.setIndex(noMatchCount++, idx);
                break;
            }
            if (hash == htEntry->hash_) {
                // same hash put in the address vector
                auto entry = payloadPtrs[ htEntry->pageNum_ -1] + (htEntry->pageOffset_ * tupleSize);
                toMatch.setIndex(toMatchCount++, idx);
                addressesPtr[idx] = entry;
                break;
            }
            ++bucket;
        }
    }
}

void incrementBuckets(idx_t capacity, uint64_t* __restrict bucketPtr, SelectionVector& notEqual, idx_t notEqualCount) {
    for (idx_t i=0;i<notEqualCount;++i) {
        auto idx = notEqual.getIndex(i);
        bucketPtr[idx]++;
        if (bucketPtr[idx] > capacity)
            bucketPtr[idx] = 0;
    }
}


void RowLayoutJoinHashTable::match(DataChunk &lchunk, Vector &lhash, SelectionVector &matchSel, idx_t& matchCount,
                                 SelectionVector &nonMatchSel, idx_t& noMatchCount) {
    BB_ASSERT(keyColumns_.size() == lchunk.columnCount());

    auto size = lchunk.getSize();
    auto vec_data = lchunk.orrify();

    Vector addresses(LogicalTypeId::ADDRESS, size);
    Vector buckets(PhysicalType::UBIGINT, size);
    Vector mask{Value(bitmask_)};
    VectorOperations::lAnd(lhash, mask, buckets, size);
    // row to compare
    SelectionVector toMatch (size);
    idx_t toMatchCount = 0;
    // row layout of the keys
    // first find the addresses for each row
    findAddresses(capacity_,  tupleSize_, hashesPtr_, payloadPtrs_, nullptr, size, buckets, lhash, nonMatchSel, noMatchCount, toMatch, toMatchCount, addresses);

    // sel vector of the rows that failed the comparison and more iterations needed
    SelectionVector notEqual(toMatchCount);
    idx_t notEqualCount = 0;
    while (toMatchCount > 0) {
        notEqualCount = 0;
        // execute the equal
        auto matched = RowOperations::equal(lchunk, vec_data.get(), keyLayout_,addresses, toMatch, toMatchCount, &notEqual, notEqualCount );
        // populate the matched rows
        for (idx_t i=0;i<matched;++i)
            matchSel.setIndex(matchCount++, toMatch.getIndex(i));

        // increment the addresses of not equal
        auto bucketPtr = FlatVector::getData<hash_t>(buckets);
        incrementBuckets(capacity_, bucketPtr, notEqual, notEqualCount);
        // find the addresses
        toMatchCount = 0;
        findAddresses(capacity_,  tupleSize_, hashesPtr_, payloadPtrs_, &notEqual, notEqualCount, buckets, lhash, nonMatchSel, noMatchCount, toMatch, toMatchCount, addresses);
    }
}


void insert(RowLayoutJoinHashTable &h1, RowLayoutJoinHashTable &h2) {
    BB_ASSERT(h1.getTypes() != h2.getTypes());
    BB_ASSERT(h1.getTypes().size() == h2.getTypes().size());

    id_t offset = 0;
    DataChunk chunk;
    chunk.initialize(h1.getTypes());
    DataChunk castChunk;
    castChunk.initialize(h2.getTypes());
    while (offset < h1.getSize()) {
        h1.scan(offset, chunk);
        chunk.cast(castChunk);
        castChunk.setCardinality(chunk.getSize());
        h2.addChunk(castChunk);
        offset += STANDARD_VECTOR_SIZE;
    }
}

void RowLayoutJoinHashTable::cast(BufferManager &manager, std::unique_ptr<RowLayoutJoinHashTable> &h1, vector<LogicalType> predicateTypes) {
    if (h1->checksTypes(predicateTypes)) return;
    vector<LogicalType> newTypes;
    for (idx_t i = 0; i < h1->cols_.size(); ++i) {
        newTypes.push_back(predicateTypes[h1->cols_[i]]);
    }
    newTypes.push_back(LogicalTypeId::HASH); // last type is for the hash
    auto newDht = rl_join_ht_ptr_t(new RowLayoutJoinHashTable(manager, newTypes, h1->keyColumns_, h1->payloadColumns_, h1->getCapacity(), h1->resizable_));
    insert( *h1, *newDht);
    h1 = std::move(newDht);
}


void RowLayoutJoinHashTable::castAndCombine(BufferManager &manager, std::unique_ptr<RowLayoutJoinHashTable> &h1, RowLayoutJoinHashTable &h2) {
    auto t1 = h1->getTypes();
    auto t2 = h2.getTypes();
    BB_ASSERT(t1.size() == t2.size());
    vector<LogicalType> commonTypes;
    for (idx_t i =0;i<t1.size();++i)
        commonTypes.push_back(getCommonType(t1[i], t2[i]));

    if (commonTypes!= h1->getTypes()) {
        // we need to cast the h1
        auto newDht = rl_join_ht_ptr_t(new RowLayoutJoinHashTable(manager, commonTypes, h1->keyColumns_, {}, h1->getCapacity(), true));
        insert( *h1, *newDht);
        h1 = std::move(newDht);
    }
    if (t2 != h1->getTypes())
        insert(h2, *h1);
    else
        h1->combine(h2);

}



}



