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
#include "bumblebee/execution/AggregateHashTable.h"

#include "CLI11.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.h"

namespace bumblebee{
AggregateHashTable::AggregateHashTable(const vector<AggregateFunction *> &aggFunctions,
    const vector<ConstantType> &types, idx_t capacity,bool resizable ): functions_(aggFunctions), capacity_(capacity), entries_(0),
                                                             hash_(UBIGINT, true, true, capacity), ready_(false), resizable_(resizable) {
    BB_ASSERT(capacity_ != 0 && (capacity_ & (capacity_ - 1)) == 0); // capacity should be power of 2
    // mask is for masking the last bit of the hash
    mask_ = (hash_t(1) << (sizeof(hash_t) * 8 - 1));

    // init chunk
    chunkone_.initialize(types);
    chunkone_.setCapacity(capacity_);
    chunkone_.resize(capacity_);
    chunkone_.setCardinality(capacity_);

    // init states
    vector<ConstantType> payloadTypes;
    for (auto& fun: functions_) {
        idx_t size = fun->stateSize_() * capacity_;
        states_.emplace_back(new data_t[size]);
        memset(states_.back().get(), 0, size);
        payloadTypes.push_back(fun->result_);
    }

    // init payloads
    payload_.initialize(payloadTypes);
    payload_.setCapacity(capacity_);
    payload_.resize(capacity_);
    payload_.setCardinality(capacity_);


    auto maxValue = (1ULL << (sizeof(capacity_) * 8 - 1));
    // check the capacity can fit in a uint of 63 (last bit is used to flag if a bucket is empty or not)
    BB_ASSERT(capacity_ < maxValue);
}

void AggregateHashTable::addChunk(Vector &hash, DataChunk &group, DataChunk &payload) {
    BB_ASSERT(payload.columnCount() == functions_.size());
    BB_ASSERT(payload.getSize() == group.getSize());
    // find the buckets for each row in the group
    SelectionVector groupSel(group.getSize());
    findOrCreateGroups(hash, group, groupSel);
    // now apply the agg function
    for (id_t i = 0;i<functions_.size();++i) {
        AggregateFunction::updateState(payload.data_[i], states_[i].get(), groupSel, *functions_[i], group.getSize());
    }
}

idx_t AggregateHashTable::getGroups(idx_t position, SelectionVector &sel, idx_t size) {
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

idx_t AggregateHashTable::scan(idx_t position, DataChunk &result) {
    BB_ASSERT(position < entries_);
    // find the non empty slots starting from position
    SelectionVector sel(STANDARD_VECTOR_SIZE);
    idx_t counter = getGroups(position, sel, STANDARD_VECTOR_SIZE);
    result.slice( chunkone_, sel, counter);
    return counter;
}

void AggregateHashTable::fetchAggregates(Vector &hash, DataChunk &group, DataChunk &result) {
    BB_ASSERT(result.columnCount() == payload_.columnCount());
    BB_ASSERT(result.getSize() == group.getSize());

    // find the buckets for each row in the group
    SelectionVector groupSel(group.getSize());
    SelectionVector matchSel(group.getSize());
    auto matchedGroups = findOrCreateGroups(hash, group, groupSel, false, &matchSel);
    // copy the agg results value in the result chunk
    for (id_t i = 0;i<payload_.columnCount();++i) {
        VectorOperations::copy(payload_.data_[i], result.data_[i], groupSel, group.getSize(), 0, 0);
    }
    // filter only the matched groups
    group.slice(matchSel, matchedGroups);
    result.slice(matchSel, matchedGroups);
}

void AggregateHashTable::finalize() {
    SelectionVector sel(entries_);
    idx_t counter = getGroups(0, sel, entries_);
    BB_ASSERT(entries_ == counter);
    for (idx_t i = 0; i < functions_.size(); i++)
        AggregateFunction::finalizeState(payload_.data_[i], states_[i].get(), sel, *functions_[i], counter);
    setReady();
}

void AggregateHashTable::initStates(DataChunk &groups,SelectionVector& emptyBucketSel, SelectionVector& emptySel, idx_t new_entry_count) {
    // copy data
    idx_t size = groups.getSize();
    for (idx_t i = 0; i < chunkone_.columnCount(); i++) {
        Vector target(chunkone_.data_[i]);
        Vector source(groups.data_[i]);
        VectorOperations::copy(source, target, emptySel, emptyBucketSel, new_entry_count, 0,0);
    }

    // init states
    for (idx_t i = 0; i < functions_.size(); i++)
        AggregateFunction::initStates(states_[i].get(),emptyBucketSel, *functions_[i], new_entry_count);
}

void AggregateHashTable::matchChunks(DataChunk &groups, SelectionVector& compareSel, SelectionVector& compareBucketSel, SelectionVector& notMatchSel, idx_t& need_compare_count, idx_t &no_match_count) {
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
        // and also the compareSel is used in the probe HT
        compareSel.initialize(compareSel.slice(equalSel, equalCount));
        compareBucketSel.initialize(compareBucketSel.slice(equalSel, equalCount));
        need_compare_count = equalCount;
        if (!need_compare_count)return;
    }
}

idx_t AggregateHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel, bool createGroups, SelectionVector* matchedSel) {

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

    SelectionVector emptySel(size);
    SelectionVector emptyBucketSel(size);

    SelectionVector compareSel(size);
    SelectionVector compareBucketSel(size);

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
        created_groups += new_entry_count;

        // init the new states
        initStates(groups, emptyBucketSel, emptySel, new_entry_count);

        // now compare the possible match
        matchChunks(groups, compareSel, compareBucketSel, notMatchSel, need_compare_count, no_match_count);

        // set the matched sel if not null
        if (matchedSel) {
            // compareSel and need_compare_count will store the information of the matched index
            for (idx_t j = 0; j< need_compare_count; j++) {
                matchedSel->setIndex(matched_groups++, compareSel.getIndex(j));
            }
        }

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

    return matched_groups;

}

void AggregateHashTable::combine(AggregateHashTable &other) {
    BB_ASSERT(chunkone_.columnCount() == other.chunkone_.columnCount());
    BB_ASSERT(chunkone_.getTypes() == other.chunkone_.getTypes());
    BB_ASSERT(payload_.columnCount() == other.payload_.columnCount());
    BB_ASSERT(payload_.getTypes() == other.payload_.getTypes());
    for (idx_t i = 0; i < functions_.size(); i++)
        BB_ASSERT(functions_[i] == other.functions_[i]);

    SelectionVector sel(other.entries_);
    other.getGroups(0, sel, other.entries_);
    // find the groups of the other table
    Vector otherHash(UBIGINT, other.chunkone_.getSize());
    other.chunkone_.hash(otherHash);
    // slice the other chunk to keep only the non empty slots
    other.chunkone_.slice(sel, other.entries_);
    // creates the groups that are not present in this HT
    SelectionVector oBucketsel(other.entries_);
    findOrCreateGroups(otherHash, other.chunkone_, oBucketsel);
    // now merge the states
    for (idx_t i = 0; i < functions_.size(); i++)
        AggregateFunction::combineStates(other.states_[i].get(), states_[i].get(), sel, oBucketsel, *functions_[i], other.entries_);

    // if HT is ready recalculate the payloads
    if (ready_)
        finalize();
}

struct PartitionInfo {
    explicit PartitionInfo (idx_t capacity): sel_(capacity), size_(0){};
    SelectionVector sel_;
    idx_t size_;
};

void AggregateHashTable::partition(vector<agg_ht_ptr>& partitions, idx_t shift) {
    vector<PartitionInfo> pInfo;
    pInfo.reserve(partitions.size());
    for (idx_t i = 0; i < partitions.size(); i++) pInfo.emplace_back(capacity_);
    // fill the partition informations
    // for each partitions create a sel vector with the groups that belong to the partition
    auto ht_hash_data = FlatVector::getData<uint64_t>(hash_);
    idx_t slotCounter = 0;
    idx_t counter = 0;
    for (id_t i = 0;i<capacity_;++i) {
        if ( !(ht_hash_data[i] && mask_)) continue; // empty slot
        idx_t p = ht_hash_data[i] >> shift;
        pInfo[p].sel_.setIndex(pInfo[p].size_++, i);
    }

    for (id_t i = 0;i<pInfo.size();++i) {
        if (pInfo[i].size_ == 0)continue; // empty partition
        auto& partition = partitions[i];
        // create partition if null
        if (!partition) partition = agg_ht_ptr(new AggregateHashTable(functions_, chunkone_.getTypes(), true));

        DataChunk chunk;
        chunk.slice(chunkone_, pInfo[i].sel_, pInfo[i].size_);
        Vector hash(UBIGINT,pInfo[i].size_);
        chunk.hash(hash);

        // create the groups in the partition table
        SelectionVector groupSel(chunk.getSize());
        partition->findOrCreateGroups(hash, chunk, groupSel);

        // combine the states

        for (idx_t i = 0; i < functions_.size(); i++)
            AggregateFunction::combineStates(states_[i].get(), partition->states_[i].get(), pInfo[i].sel_, groupSel, *functions_[i], pInfo[i].size_);

    }

}


void AggregateHashTable::resize(idx_t size) {
    if (capacity_ > size) return;
    BB_ASSERT(resizable_);
    // create a new HT with the capacity = size and combine with the current one
    AggregateHashTable newTable(functions_, chunkone_.getTypes(), size);
    newTable.combine(*this);
    // now take the results from the new table
    capacity_ = newTable.capacity_;
    entries_ = newTable.entries_;
    chunkone_.reference(newTable.chunkone_);
    payload_.reference(newTable.payload_);
    states_ = std::move(newTable.states_);
    hash_.reference(newTable.hash_);
    mask_ = newTable.mask_;
}

idx_t AggregateHashTable::getSize() const {
    return entries_;
}

idx_t AggregateHashTable::getCapacity() const {
    return capacity_;
}

bool AggregateHashTable::isReady() const {
    return ready_;
}

void AggregateHashTable::setReady() {
    ready_ = true;
}

string AggregateHashTable::toString(bool compact) {
    string result = "AggregateHashTable - "
            +std::to_string(capacity_)+" - "
            +std::to_string(entries_)+" - "
            +std::to_string(resizable_);
    result += " - [";
    for (auto& f : functions_) {
        result += f->name_ + ",";
    }
    DataChunk chunkone, payload;
    Vector hash(UBIGINT, capacity_);
    chunkone.initialize(chunkone_.getTypes());
    payload.initialize(payload_.getTypes());
    chunkone.reference(chunkone_);
    payload.reference(payload_);
    hash.reference(hash_);

    if (compact) {
        SelectionVector sel(entries_);
        getGroups(0, sel, entries_);
        chunkone.slice(sel, entries_);
        payload.slice(sel, entries_);
        hash.slice(sel, entries_);
    }

    result += "]\n";
    result += "Chunkone:\n" + chunkone.toString();
    result += "Payload:\n" + payload.toString();
    result += "Hash:\n" + hash.toString((compact)?entries_:capacity_);

    return result;
}
}
