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

#include "bumblebee/function/AggregateFunction.hpp"

namespace bumblebee{
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

    // now update the states
    for (id_t i = 0;i<functions_.size();++i)
        AggregateFunction::updateStates(layout_, addresses,payload.data_[i],payload.getSize(), i );

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
    // update the payload with the first state
    for (id_t i = 0;i<functions_.size();++i)
        AggregateFunction::updateStates(layout_, addresses,payload.data_[i],payload.getSize(), i );
}

void AggregatePRLHashTable::moveAndMergeStates(idx_t count, Vector &addresses, Vector &hashes) {
    SelectionVector newGroupsSel(count);
    idx_t newGroupsCount = 0;
    auto groupAddresses = move(addresses, hashes, count, &newGroupsSel, newGroupsCount);

    // init new states (initStates already iterates all aggregate functions via layout)
    AggregateFunction::initStates(layout_, groupAddresses, newGroupsSel, newGroupsCount);


    AggregateFunction::combineStates(layout_, addresses, groupAddresses, FlatVector::INCREMENTAL_SELECTION_VECTOR, count);

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
