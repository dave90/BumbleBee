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
#include "bumblebee/catalog/PredicateTables.h"

#include "bumblebee/parser/statement/Atom.h"

namespace bumblebee{
PredicateTables::PredicateTables(const char* name, unsigned arity): predicate_(new Predicate(name, arity)), types_(arity, UNKNOWN) {
}

bool operator==(const PredicateTables &lhs, const PredicateTables &rhs) {
    return lhs.predicate_ == rhs.predicate_;
}

bool operator!=(const PredicateTables &lhs, const PredicateTables &rhs) {
    return !(lhs == rhs);
}

void PredicateTables::updateTypes(std::vector<ConstantType>& newTypes) {
    BB_ASSERT(newTypes.size() == types_.size() && "Wrong number of terms for Fact");
    for (idx_t i = 0; i < newTypes.size(); i++) {
        if (newTypes[i] == types_[i]) continue;
        // if types is UNKNOWN
        // or different types take the one with the greatest size
        if (types_[i] == UNKNOWN || getCTypeSize( types_[i]) < getCTypeSize(newTypes[i]) ) {
            types_[i] = newTypes[i];
            continue;
        }
        // different types but same sizeof take the not unsigned
        // and bump up
        auto signedType = (!isUnsigned(types_[i]))?types_[i]:newTypes[i];
        types_[i] = getBumpedType(signedType);
    }
}

void PredicateTables::addFact(Atom &atom) {
    // track the types for each column
    if (!atom.containsRange()) {
        auto types = atom.getTermsCType();
        updateTypes(types);
        facts_.push_back(std::move(atom));
    }
    // fact atom is a sequence convert to sequence vector

}

std::vector<ConstantType> PredicateTables::getTypes() {
    return types_;
}

void PredicateTables::initializeChunks() {
    if (facts_.empty()) return;
    auto types = getTypes();
    BB_ASSERT(types.size() == predicate_->getArity());
    DataChunk chunk;
    chunk.initialize(types);
    auto columns = chunk.columnCount();
    idx_t idx = 0;
    auto chunkCapacity = chunk.getCapacity();
    auto factsSize = facts_.size();
    // while until we add all the facts
    while (idx < factsSize) {
        // fill the data chunk or we finish the facts
        for (idx_t i=0;i< chunkCapacity && idx < factsSize;++i) {
            for (auto col = 0;col < columns;++col) {
                chunk.setValue(col, idx, facts_[idx].getValue(col) );
            }
        }

    }
}

void PredicateTables::append(DataChunk &chunk) {
    // TODO mutex
    atoms_.append(chunk);
    if (types_.size() > 0 && types_[0] == UNKNOWN) {
        // set the types as the chunk types
        types_ = chunk.getTypes();
    }
}
}
