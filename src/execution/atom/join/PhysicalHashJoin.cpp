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
#include "bumblebee/execution/atom/join/PhysicalHashJoin.h"

namespace bumblebee{

class HTJoinAtomState : public PhysicalAtomState {
public:
    HTJoinAtomState(JoinHashTable& ht):ht_(ht){};

    void reset() {
        rpos_ = 0;
        lpos_ = 0;
    }

    JoinHashTable& ht_;
    idx_t rpos_{0};
    idx_t lpos_{0};
};


PhysicalHashJoin::PhysicalHashJoin(const std::vector<ConstantType> &types, std::vector<idx_t> &dcCols,
    std::vector<idx_t> &selectedCols, idx_t estimated_cardinality,
    PredicateTables *pt, std::vector<idx_t> keys, std::vector<idx_t> lkeys, std::vector<Expression>& conditions)
        :PhysicalAtom(types, dcCols, selectedCols, estimated_cardinality),
        pt_(pt),
        conditions_(std::move(conditions)),
        keys_(keys),
        lkeys_(lkeys){
}

PhysicalHashJoin::~PhysicalHashJoin() {}

string PhysicalHashJoin::getName() const {
    return "PHYSICAL_HASH_JOIN";
}

string PhysicalHashJoin::toString() const {
    string result = getName() + "( "+pt_->predicate_->getName()+", ";
    for (auto k:keys_)
        result += std::to_string(k) + ", ";

    return result + " )";}

pstate_ptr_t PhysicalHashJoin::getState() const {
    return pstate_ptr_t(new HTJoinAtomState(pt_->getJoinHashTable(keys_)));
}

DataChunk PhysicalHashJoin::selectColumns(DataChunk &chunk) const{
    DataChunk newChunk;
    newChunk.initializeEmpty(colsType_);
    newChunk.reference(chunk, selectCols_);
    return newChunk;
}

AtomResultType PhysicalHashJoin::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    auto& cstate = (HTJoinAtomState&)state;
    context.profiler_.startPhysicalAtom(this);

    BB_ASSERT(cstate.ht_.isReady());

    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(chunk);
        cstate.reset();
        chunk.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }

    auto& lchunk = input;
    Vector hash(UBIGINT);
    lchunk.hash(hash, lkeys_);

    auto& rchunk = cstate.ht_.getDataChunk();

    auto count = 0;
    // loop until we produce output data chunk, or we complete the join
    while (!count && cstate.lpos_ < lchunk.getSize()) {

        SelectionVector lsel(STANDARD_VECTOR_SIZE);
        SelectionVector rsel(STANDARD_VECTOR_SIZE);

        count = cstate.ht_.probe(cstate.lpos_, cstate.rpos_, lchunk, hash,lsel, rsel, conditions_ );

        if (count > 0) {
            // we have output chunk to construct
            chunk.slice(input, lsel, count);
            // remove the keys (same columns between left and right) before the slice
            auto rSelChunk = selectColumns(rchunk);
            chunk.slice(rSelChunk, rsel, count, dcCols_);
        }
    }
    context.profiler_.endPhysicalAtom(chunk);

    if (count == 0) {
        // no output generated, ask new input
        cstate.reset();
        chunk.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }
    return AtomResultType::HAVE_MORE_OUTPUT;

}
}
