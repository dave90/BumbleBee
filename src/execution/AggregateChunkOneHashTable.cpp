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
#include "bumblebee/execution/AggregateChunkOneHashTable.hpp"

#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{

AggregateChunkOneHashTable::AggregateChunkOneHashTable(const vector<ConstantType> &types, idx_t capacity, bool resizable,
    const vector<AggregateFunction *> &aggFunctions)
    : ChunkOneHashTable(types, capacity, resizable),
      functions_(aggFunctions), ready_(false){

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
}

void AggregateChunkOneHashTable::addChunk(Vector& hash, DataChunk& groups, DataChunk& payload) {
    if (groups.columnCount() == 0) {
        addChunk(payload);
        return;
    }

    // now insert or find the groups
    // find the buckets for each row in the group
    SelectionVector groupSel(groups.getSize());
    SelectionVector newGroupsSel(groups.getSize());
    idx_t newGroupCount = 0;
    findOrCreateGroups(hash, groups, groupSel, newGroupCount, newGroupsSel);

    // select the buckets of the new groups
    SelectionVector newGroupsBucketSel(groupSel.slice(newGroupsSel, newGroupCount));

    // init the states
    for (idx_t i = 0; i < functions_.size(); i++)
        AggregateFunction::initStates(states_[i].get(),newGroupsBucketSel, *functions_[i], newGroupCount);


    // now update the states
    for (id_t i = 0;i<functions_.size();++i) {
        AggregateFunction::updateState(payload.data_[i], states_[i].get(), groupSel, *functions_[i], payload.getSize());
    }

}

void AggregateChunkOneHashTable::addChunk(DataChunk &payload) {
    // check if init state is needed
    BB_ASSERT(hash_.getVectorType() == VectorType::FLAT_VECTOR);
    auto hash_data = FlatVector::getData<uint64_t>(hash_);
    if (!(hash_data[0] & mask_)) {
        entries_++;
        hash_data[0] = hash_data[0] | mask_;
        SelectionVector sel;
        for (idx_t i = 0; i < functions_.size(); i++)
            AggregateFunction::initStates(states_[i].get(),sel, *functions_[i], 1);
    }

    // update the first state

    for (id_t i = 0;i<functions_.size();++i) {
        AggregateFunction::updateState(payload.data_[i], states_[i].get(), ConstantVector::ZERO_SELECTION_VECTOR, *functions_[i], payload.getSize());
    }
}

void AggregateChunkOneHashTable::combine(AggregateChunkOneHashTable &other) {
    BB_ASSERT(chunkone_.columnCount() == other.chunkone_.columnCount());
    BB_ASSERT(chunkone_.getTypes() == other.chunkone_.getTypes());
    BB_ASSERT(payload_.columnCount() == other.payload_.columnCount());
    BB_ASSERT(payload_.getTypes() == other.payload_.getTypes());
    for (idx_t i = 0; i < functions_.size(); i++)
        BB_ASSERT(functions_[i] == other.functions_[i]);

    if (other.entries_ == 0)return;

    if (other.chunkone_.columnCount() == 0) {
        // aggregation with no groups, so combine the first state
        BB_ASSERT(other.entries_ == 1);
        SelectionVector sel;
        for (idx_t i = 0; i < functions_.size(); i++)
            AggregateFunction::combineStates(other.states_[i].get(), states_[i].get(), sel, sel, *functions_[i], other.entries_);
        return;
    }

    // find the groups of the other table and remove the empty group
    SelectionVector sel(other.entries_);
    other.getGroups(0, sel, other.entries_);
    other.chunkone_.slice(sel, other.entries_);

    Vector otherHash(UBIGINT, other.chunkone_.getSize());
    other.chunkone_.hash(otherHash);

    // add and find the groups
    SelectionVector groupSel(other.chunkone_.getSize());
    SelectionVector newGroupsSel(other.chunkone_.getSize());
    idx_t newGroupCount = 0;
    findOrCreateGroups(otherHash, other.chunkone_, groupSel, newGroupCount, newGroupsSel);

    // select the buckets of the new groups
    SelectionVector newGroupsBucketSel(groupSel.slice(newGroupsSel, newGroupCount));

    // init the states
    for (idx_t i = 0; i < functions_.size(); i++)
        AggregateFunction::initStates(states_[i].get(),newGroupsBucketSel, *functions_[i], newGroupCount);


    // now combine the states
    // now merge the states
    for (idx_t i = 0; i < functions_.size(); i++)
        AggregateFunction::combineStates(other.states_[i].get(), states_[i].get(), sel, groupSel, *functions_[i], other.entries_);

}

void AggregateChunkOneHashTable::resize(idx_t size) {
    if (capacity_ > size) return;
    BB_ASSERT(resizable_);
    // create a new HT with the capacity = size and combine with the current one
    AggregateChunkOneHashTable newTable(chunkone_.getTypes(), size, false, functions_);
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

void AggregateChunkOneHashTable::finalize() {
    SelectionVector sel(entries_);
    idx_t counter = getGroups(0, sel, entries_);
    BB_ASSERT(entries_ == counter);
    for (idx_t i = 0; i < functions_.size(); i++)
        AggregateFunction::finalizeState(payload_.data_[i], states_[i].get(), sel, *functions_[i], counter);
    ready_ = true;
}

bool AggregateChunkOneHashTable::isReady() {
    return ready_;
}

void AggregateChunkOneHashTable::fetchAggregates(Vector &hash, DataChunk &group, DataChunk &result,
    SelectionVector &sel) {
    BB_ASSERT(result.columnCount() == payload_.columnCount());
    BB_ASSERT(result.getSize() == group.getSize());
    BB_ASSERT(ready_);

    // find the buckets for each row in the group
    SelectionVector groupSel(group.getSize());
    idx_t matchedGroups = 0;
    findOrCreateGroups(hash, group, groupSel,matchedGroups, false, sel);
    // copy the agg results value in the result chunk
    for (id_t i = 0;i<payload_.columnCount();++i) {
        VectorOperations::copy(payload_.data_[i], result.data_[i], groupSel, group.getSize(), 0, 0);
    }
    // filter only the matched groups
    group.slice(sel, matchedGroups);
    result.slice(sel, matchedGroups);
}


void AggregateChunkOneHashTable::fetchAggregates(Vector &hash, DataChunk &group, Vector &result, idx_t function,
    SelectionVector &sel) {
    BB_ASSERT(function < functions_.size());
    BB_ASSERT(ready_);
    BB_ASSERT(result.getType() == payload_.data_[function].getType());

    // find the buckets for each row in the group
    SelectionVector groupSel(group.getSize());
    idx_t matchedGroups = 0;
    findOrCreateGroups(hash, group, groupSel,matchedGroups, false, sel);

    // copy the agg results value in the result chunk
    VectorOperations::copy(payload_.data_[function], result, groupSel, group.getSize(), 0, 0);
    // filter only the matched groups
    group.slice(sel, matchedGroups);
    result.slice(sel, matchedGroups);
}

void AggregateChunkOneHashTable::fetchAggregates(DataChunk &result) {
    BB_ASSERT(ready_);

    // create constant vector that point to the first value of the payload
    for (idx_t i = 0; i < functions_.size(); i++) {
        result.data_[i].reference(Value(payload_.data_[i].getValue(0)));
    }
    result.setCardinality(1);
}

void AggregateChunkOneHashTable::fetchAggregates(Vector &result, idx_t function) {
    BB_ASSERT(ready_);
    BB_ASSERT(function < payload_.data_.size());

    result.reference(Value(payload_.data_[function].getValue(0)));
}

string AggregateChunkOneHashTable::toString(bool compact) {
    string result = "AggregateChunkOneHashTable - "
            +std::to_string(capacity_)+" - "
            +std::to_string(entries_)+" - "
            +std::to_string(resizable_);

    DataChunk chunkone, payload;
    Vector hash(UBIGINT, capacity_);
    chunkone.initialize(chunkone_.getTypes());
    chunkone.reference(chunkone_);
    payload.initialize(payload_.getTypes());
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
