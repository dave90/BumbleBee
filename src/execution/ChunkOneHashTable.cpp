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
#include "../include/bumblebee/execution/ChunkOneHashTable.h"

#include "bumblebee/common/vector_operations/VectorOperations.h"

namespace bumblebee{
ChunkOneHashTable::ChunkOneHashTable(const vector<ConstantType>& types, idx_t capacity, bool resizable) :
    entries_(0), capacity_(capacity), resizable_(resizable), hash_(UBIGINT,true, true, capacity) {
    BB_ASSERT(capacity_ != 0 && (capacity_ & (capacity_ - 1)) == 0); // capacity should be power of 2
    // mask is for masking the last bit of the hash
    mask_ = (hash_t(1) << (sizeof(hash_t) * 8 - 1));

    // init chunk
    chunkone_.initialize(types);
    chunkone_.setCapacity(capacity_);
    chunkone_.resize(capacity_);
    chunkone_.setCardinality(capacity_);




    auto maxValue = (1ULL << (sizeof(capacity_) * 8 - 1));
    // check the capacity can fit in a uint of 63 (last bit is used to flag if a bucket is empty or not)
    BB_ASSERT(capacity_ < maxValue);
}

void ChunkOneHashTable::addChunk(Vector &hash, DataChunk &chunk) {
    // creates the buckets for each row in the groups
    SelectionVector groupSel(chunk.getSize());
    findOrCreateGroups(hash, chunk, groupSel);
}

idx_t ChunkOneHashTable::scan(idx_t position, DataChunk &result, idx_t size) {
    BB_ASSERT(position < entries_);
    // find the non empty slots starting from position
    SelectionVector sel(size);
    idx_t counter = getGroups(position, sel, size);
    result.slice( chunkone_, sel, counter);
    return counter;
}

void ChunkOneHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel) {
    idx_t mc, ng;
    return findOrCreateGroupsInternal(hash, groups, groupSel,mc,ng, true, nullptr, nullptr );
}

void ChunkOneHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel,
    idx_t &matchedCount, bool createGroups, SelectionVector &matchedSel) {
    idx_t ng;
    return findOrCreateGroupsInternal(hash, groups, groupSel,matchedCount,ng, createGroups, &matchedSel, nullptr );
    }

void ChunkOneHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel,
    idx_t &newGroupsCount, SelectionVector &newGroupSel) {
    idx_t mc;
    return findOrCreateGroupsInternal(hash, groups, groupSel,mc,newGroupsCount, true, nullptr, &newGroupSel );
}

void ChunkOneHashTable::combine(ChunkOneHashTable &other) {
    BB_ASSERT(chunkone_.columnCount() == other.chunkone_.columnCount());
    BB_ASSERT(chunkone_.getTypes() == other.chunkone_.getTypes());

    // find the groups of the other table and remove the empty group
    SelectionVector sel(other.entries_);
    other.getGroups(0, sel, other.entries_);
    other.chunkone_.slice(sel, other.entries_);

    Vector otherHash(UBIGINT, other.chunkone_.getSize());
    other.chunkone_.hash(otherHash);

    // add the chunk in the current ht
    addChunk(otherHash, other.chunkone_);
}

struct PartitionInfo {
    explicit PartitionInfo (idx_t capacity): sel_(capacity), size_(0){};
    SelectionVector sel_;
    idx_t size_;
};

void ChunkOneHashTable::partition(vector<distinct_ht_ptr_t> &partitions, idx_t shift) {
    vector<PartitionInfo> pInfo;
    pInfo.reserve(partitions.size());
    for (idx_t i = 0; i < partitions.size(); i++) pInfo.emplace_back(capacity_);
    // fill the partition informations
    // for each partitions create a sel vector with the groups that belong to the partition
    BB_ASSERT(hash_.getVectorType() == VectorType::FLAT_VECTOR);
    auto ht_hash_data = FlatVector::getData<uint64_t>(hash_);
    for (id_t i = 0;i<capacity_;++i) {
        idx_t p = ht_hash_data[i] >> shift; // partition
        pInfo[p].sel_.setIndex(pInfo[p].size_, i);
        pInfo[p].size_ += (ht_hash_data[i] && mask_)?1:0; // increment size if slot is non empty
    }

    for (id_t i = 0;i<pInfo.size();++i) {
        auto& pi = pInfo[i];
        if (pi.size_ == 0)continue; // empty partition
        auto& partition = partitions[i];
        // create partition if null
        if (!partition) partition = distinct_ht_ptr_t(new ChunkOneHashTable(chunkone_.getTypes(),MORSEL_SIZE, true));

        DataChunk chunk;
        chunk.initializeEmpty(chunkone_.getTypes());
        chunk.slice(chunkone_, pi.sel_, pi.size_);
        Vector hash(UBIGINT,pi.size_);
        chunk.hash(hash);

        partition->addChunk(hash, chunk);
    }
}

idx_t ChunkOneHashTable::getSize() const {
    return entries_;
}

idx_t ChunkOneHashTable::getCapacity() const {
    return capacity_;
}

string ChunkOneHashTable::toString(bool compact) {
    string result = "DistincteHashTable - "
            +std::to_string(capacity_)+" - "
            +std::to_string(entries_)+" - "
            +std::to_string(resizable_);

    DataChunk chunkone;
    Vector hash(UBIGINT, capacity_);
    chunkone.initialize(chunkone_.getTypes());
    chunkone.reference(chunkone_);
    hash.reference(hash_);

    if (compact) {
        SelectionVector sel(entries_);
        getGroups(0, sel, entries_);
        chunkone.slice(sel, entries_);
        hash.slice(sel, entries_);
    }

    result += "]\n";
    result += "Chunkone:\n" + chunkone.toString();
    result += "Hash:\n" + hash.toString((compact)?entries_:capacity_);

    return result;
}

vector<ConstantType> ChunkOneHashTable::getTypes() const{
    return chunkone_.getTypes();
}

void ChunkOneHashTable::resize(idx_t size) {
    if (capacity_ > size) return;
    BB_ASSERT(resizable_);
    // create a new HT with the capacity = size and combine with the current one
    ChunkOneHashTable newTable(chunkone_.getTypes(), size, false);
    newTable.combine(*this);
    // now take the results from the new table
    capacity_ = newTable.capacity_;
    entries_ = newTable.entries_;
    chunkone_.reference(newTable.chunkone_);
    hash_.reference(newTable.hash_);
    mask_ = newTable.mask_;
}

void ChunkOneHashTable::copyNewGroups(DataChunk &groups, SelectionVector &emptyBucketSel, SelectionVector &emptySel,
    idx_t new_entry_count) {
    // copy data
    for (idx_t i = 0; i < chunkone_.columnCount(); i++) {
        Vector target(chunkone_.data_[i]);
        Vector source(groups.data_[i]);
        VectorOperations::copy(source, target, emptySel, emptyBucketSel, new_entry_count, 0,0);
    }
}

void ChunkOneHashTable::matchChunks(DataChunk &groups, SelectionVector &compareSel,
    SelectionVector &compareBucketSel, SelectionVector &notMatchSel, idx_t &need_compare_count,
    idx_t &no_match_count) {

    if (!need_compare_count)return;
    idx_t size = groups.getSize();
    SelectionVector notEqualSel(size);
    SelectionVector equalSel(size);
    for (idx_t i = 0; i < chunkone_.columnCount(); i++) {
        Vector left(chunkone_.data_[i]);
        left.slice(compareBucketSel, need_compare_count);
        Vector right(groups.data_[i]);
        right.slice(compareSel, need_compare_count);
        idx_t notEqualCount;
        auto equalCount = VectorOperations::equals(left, right, nullptr, need_compare_count, &equalSel, &notEqualSel, notEqualCount);
        // add the not matched to the notMatchSel
        for (idx_t j = 0; j < notEqualCount; j++)
            notMatchSel.setIndex(no_match_count++, notEqualSel.getIndex(j));
        // set the compare sel with the matched for the next vector comparison ( avoid matching again the not equal index)
        compareSel.initialize(compareSel.slice(equalSel, equalCount));
        compareBucketSel.initialize(compareBucketSel.slice(equalSel, equalCount));
        need_compare_count = equalCount;
        if (!need_compare_count)return;
    }
}

idx_t ChunkOneHashTable::getGroups(idx_t position, SelectionVector &sel, idx_t size) {
    BB_ASSERT(hash_.getVectorType() == VectorType::FLAT_VECTOR);
    auto ht_hash_data = FlatVector::getData<uint64_t>(hash_);
    idx_t slotCounter = 0;
    idx_t counter = 0;
    for (id_t i = 0;i<capacity_ && counter < size;++i) {
        sel.setIndex(counter, i);
        counter += (ht_hash_data[i] && mask_ && slotCounter >= position)?1:0; // increment if we surpassed the position
        slotCounter += (ht_hash_data[i] && mask_)?1:0; // increment if entry is not empty
    }
    return counter;
}

void ChunkOneHashTable::findOrCreateGroupsInternal(Vector &hash, DataChunk &groups, SelectionVector &groupSel,
    idx_t &matchedCount, idx_t &newGroupsCount, bool createGroups, SelectionVector *matchedSel,
    SelectionVector *newGroupSel) {

    auto size = groups.getSize();

    if (createGroups && // if new groups can be created
        (size + entries_ >= capacity_ // size + current size > capacity
            || ((float)entries_ + size) / (float)capacity_ > LOAD_FACTOR // size + current size > LOAD FACTOR
        )) {
        resize(capacity_ * 2);
    }

    BB_ASSERT(size + entries_ <= capacity_ );
    BB_ASSERT(groups.columnCount() == chunkone_.columnCount());
    BB_ASSERT(groups.getTypes() == chunkone_.getTypes());
    BB_ASSERT(hash.getType() == UBIGINT);

    // should be already normalized
    hash.normalify(size);
    hash_.normalify(capacity_);

    // now compute the buckets from the hashes
    Vector buckets(UBIGINT, size);
    Vector mask(Value(capacity_-1));
    BB_ASSERT(mask.getVectorType() == VectorType::CONSTANT_VECTOR);
    VectorOperations::lAnd(hash, mask, buckets, size);
    buckets.normalify(size); // should be already normalized

    SelectionVector sel_vector;
    // sel of index and buckets of new groups
    SelectionVector emptySel(size);
    SelectionVector emptyBucketSel(size);
    // sel of index and buckets of the to be matched groups
    SelectionVector compareSel(size);
    SelectionVector compareBucketSel(size);
    // sel of the no match groups
    SelectionVector notMatchSel(size);

    auto group_data = chunkone_.orrify();
    auto hash_data = FlatVector::getData<uint64_t>(hash);
    auto buckets_data = FlatVector::getData<uint64_t>(buckets);
    auto ht_hash_data = FlatVector::getData<uint64_t>(hash_);

    idx_t remaining_entries = size;
    idx_t created_groups = 0;
    idx_t matched_groups = 0;
    while (remaining_entries > 0) {

        idx_t new_entry_count = 0;
        idx_t need_compare_count = 0;
        idx_t no_match_count = 0;

        // first figure out if it belongs to a full or empty group
        for (idx_t i = 0; i < remaining_entries; i++) {

            idx_t index = sel_vector.getIndex(i);
            auto bucket = buckets_data[index];
            auto ht_entry = ht_hash_data[bucket];
            auto hash = hash_data[index];
            if (ht_entry && mask_) {
                // entry is not empty as last bit is 1
                // try to compare the hash except last bit
                if ( (ht_entry | mask_) == (hash | mask_)) {
                    // try to compare the data and set the bucket
                    compareSel.setIndex(need_compare_count, index);
                    compareBucketSel.setIndex(need_compare_count++, bucket);
                    groupSel.setIndex(index, bucket);
                }else {
                    // hash does not match so avoid to compare data
                    notMatchSel.setIndex(no_match_count++, index);
                }

            }else {
                // empty entry, set the bucket with the hash
                groupSel.setIndex(index, bucket);
                if (!createGroups) continue;
                ht_hash_data[bucket] = hash | mask_;

                emptySel.setIndex(new_entry_count, index);
                emptyBucketSel.setIndex(new_entry_count++, bucket);
                entries_++;
            }

        }
        // set the created groups index sel
        if (newGroupSel) {
            for (idx_t j = 0; j< new_entry_count; j++) {
                newGroupSel->setIndex(created_groups++, emptySel.getIndex(j));
            }
        }else
            created_groups += new_entry_count;

        // copy the data
        copyNewGroups(groups, emptyBucketSel, emptySel, new_entry_count);

        // now compare the possible match
        matchChunks(groups, compareSel, compareBucketSel, notMatchSel, need_compare_count, no_match_count);

        // set the matched sel if not null
        if (matchedSel) {
            // compareSel and need_compare_count will store the information of the matched index
            for (idx_t j = 0; j< need_compare_count; j++) {
                matchedSel->setIndex(matched_groups++, compareSel.getIndex(j));
            }
        }else
            matched_groups += need_compare_count;

        // the matched rows we do not need to update as bucket is correct
        // for the notMatchSel we need to try to compare with another bucket (linear probe)
        for (idx_t i = 0; i < no_match_count; i++) {
            idx_t index = notMatchSel.getIndex(i);
            buckets_data[index]++;
            if (buckets_data[index] >= capacity_) {
                // circular scan
                buckets_data[index] = 0;
            }
        }

        sel_vector.initialize(notMatchSel);
        remaining_entries = no_match_count;
    }

    matchedCount = matched_groups;
    newGroupsCount = created_groups;
}
}
