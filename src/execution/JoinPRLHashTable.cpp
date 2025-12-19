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
#include "bumblebee/execution/JoinPRLHashTable.hpp"

#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{
JoinPRLHashTable::JoinPRLHashTable(BufferManager &manager, const vector<ConstantType> &types, const vector<idx_t> &key_columns, const vector<idx_t> &payload_columns,idx_t capacity,
        bool resizable): PRLHashTable(manager),
    keyColumns_(key_columns),
    payloadColumns_(payload_columns) {
    // create the types where keys are in the beginning and after the payloads
    cols_ = keyColumns_;
    cols_.insert(cols_.end(), payloadColumns_.begin(), payloadColumns_.end());
    vector<ConstantType> orderedTypes;
    for (auto& idx: key_columns)
        orderedTypes.push_back(types[idx]);
    keyLayout_.initialize(orderedTypes); // init with keys cols
    for (auto& idx: payload_columns)
        orderedTypes.push_back(types[idx]);

    initialize(orderedTypes, capacity, resizable);
}

bool JoinPRLHashTable::isReady() {
    return ready_;
}

void JoinPRLHashTable::setReady() {
    ready_ = true;
}

vector<idx_t> JoinPRLHashTable::getKeys() {
    return keyColumns_;
}

vector<idx_t> JoinPRLHashTable::getPayloads() {
    return payloadColumns_;
}

void JoinPRLHashTable::addChunk(DataChunk &chunk) {
    Vector hash(UBIGINT, chunk.getSize());
    chunk.hash(hash, keyColumns_);
    Vector addresses(UBIGINT, chunk.getSize());
    DataChunk orderedChunk;
    orderedChunk.initializeEmpty(types_);
    // first the keys and after the payload cols
    orderedChunk.reference(chunk, cols_);
    findOrCreateGroups(hash, orderedChunk, addresses);
}

bool JoinPRLHashTable::checkKeysAndPayloads(const vector<idx_t>& keys, const vector<idx_t>& payloads) {
    return compareVectors(keys, keyColumns_) && compareVectors(payloads, payloadColumns_);
}

void JoinPRLHashTable::probe(idx_t &ltuple, idx_t &rtuple, DataChunk &lchunk, Vector &lhash,SelectionVector &lsel, SelectionVector &rsel, DataChunk &result) {
    BB_ASSERT(keyColumns_.size() == lchunk.columnCount());
    auto size = lchunk.getSize();
    Vector addresses(UBIGINT, STANDARD_VECTOR_SIZE);
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
    vector<ConstantType> payloadTypes;
    for (idx_t i=0;i<payloadColumns_.size();++i)
        payloadTypes.push_back(types_[i]);
    result.initializeEmpty(payloadTypes);
    // fetch the payload columns
    for (idx_t idx=0; idx < payloadTypes.size(); ++idx) {
        result.data_[idx].initialize(payloadTypes[idx], finalCount);
        idx_t offset = layout_.getOffsets()[keyColumns_.size() + idx];
        RowOperations::gather(addresses, rsel, result.data_[idx], FlatVector::INCREMENTAL_SELECTION_VECTOR,finalCount, offset);
    }
    result.setCapacity(finalCount);
    result.setCardinality(finalCount);
}



void JoinPRLHashTable::match(DataChunk &lchunk, Vector &lhash, SelectionVector &matchSel, idx_t& matchCount,
                             SelectionVector &nonMatchSel, idx_t& noMatchCount) {
    BB_ASSERT(keyColumns_.size() == lchunk.columnCount());

    auto size = lchunk.getSize();
    auto vec_data = lchunk.orrify();

    Vector addresses(UBIGINT, size);
    Vector buckets(UBIGINT, size);
    Vector mask{Value(bitmask_)};
    VectorOperations::lAnd(lhash, mask, buckets, size);
    // row to compare
    SelectionVector toMatch (size);
    idx_t toMatchCount = 0;
    // row layout of the keys
    // first find the addresses for each row
    findAddresses(nullptr, size, buckets, lhash, nonMatchSel, noMatchCount, toMatch, toMatchCount, addresses);

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
        incrementBuckets(buckets, notEqual, notEqualCount);
        // find the addresses
        toMatchCount = 0;
        findAddresses(&notEqual, notEqualCount, buckets, lhash, nonMatchSel, noMatchCount, toMatch, toMatchCount, addresses);
    }
}

void JoinPRLHashTable::findAddresses(const SelectionVector* sel, idx_t size,Vector &buckets, Vector &lhash, SelectionVector &nonMatchSel, idx_t& noMatchCount,SelectionVector& toMatch , idx_t& toMatchCount, Vector& addresses) {
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
            if (bucket  >= capacity_)
                bucket = 0;
            BB_ASSERT(idx < capacity_);
            auto htEntry = ((HTEntry64*)hashesPtr_) + bucket;
            if (htEntry->pageNum_ == 0) {
                // no match for idx row
                nonMatchSel.setIndex(noMatchCount++, idx);
                break;
            }
            if (hash == htEntry->hash_) {
                // same hash put in the address vector
                auto entry = payloadPtrs_[ htEntry->pageNum_ -1] + (htEntry->pageOffset_ * tupleSize_);
                toMatch.setIndex(toMatchCount++, idx);
                addressesPtr[idx] = entry;
                break;
            }
            ++bucket;
        }
    }
}

void JoinPRLHashTable::combine(PRLHashTable &other) {
   auto& joinOth = (JoinPRLHashTable&)other;

    if (joinOth.entries_ == 0)
        return;

    BB_ASSERT(types_ == joinOth.types_);
    BB_ASSERT(keyColumns_ == joinOth.keyColumns_);
    BB_ASSERT(payloadColumns_ == joinOth.payloadColumns_);

    idx_t position = 0;
    DataChunk group;
    group.initialize(types_);
    vector<idx_t> keys; // keys are the first N column
    for (idx_t i=0; i < keyColumns_.size(); ++i)
        keys.push_back(i);

    while (position < joinOth.entries_) {
        joinOth.scan(position, group);

        Vector hash(UBIGINT, group.getSize());
        group.hash(hash, keys);
        Vector addresses(UBIGINT, group.getSize());
        findOrCreateGroups(hash, group, addresses);

        position += minValue<idx_t>(STANDARD_VECTOR_SIZE, joinOth.entries_ - position);
    }
}


void JoinPRLHashTable::incrementBuckets(Vector& buckets, SelectionVector& notEqual, idx_t notEqualCount) {
    auto bucketPtr = FlatVector::getData<uint64_t>(buckets);
    for (idx_t i=0;i<notEqualCount;++i) {
        auto idx = notEqual.getIndex(i);
        bucketPtr[idx]++;
        if (bucketPtr[idx] > capacity_)
            bucketPtr[idx] = 0;
    }
}

void insert(JoinPRLHashTable &h1, JoinPRLHashTable &h2) {
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

void JoinPRLHashTable::castAndCombine(BufferManager &manager, std::unique_ptr<JoinPRLHashTable> &h1, JoinPRLHashTable &h2) {
    auto t1 = h1->getTypes();
    auto t2 = h2.getTypes();
    BB_ASSERT(t1.size() == t2.size());
    vector<ConstantType> commonTypes;
    for (idx_t i =0;i<t1.size();++i)
        commonTypes.push_back(getCommonType(t1[i], t2[i]));

    if (commonTypes!= h1->getTypes()) {
        // we need to cast the h1
        join_prl_ht_ptr_t newDht = join_prl_ht_ptr_t(new JoinPRLHashTable(manager, commonTypes, h1->keyColumns_, {}, h1->getCapacity(), true));
        insert( *h1, *newDht);
        h1 = std::move(newDht);
    }
    if (t2 != h1->getTypes())
        insert(h2, *h1);
    else
        h1->combine(h2);

}



}



