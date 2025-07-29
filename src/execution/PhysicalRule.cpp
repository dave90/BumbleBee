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
#include "bumblebee/execution/PhysicalRule.h"

namespace bumblebee{
PhysicalRule::PhysicalRule(patom_ptr_t &source, patom_ptr_t &sink, patom_ptr_vector_t &patoms, idx_t priority): source_(std::move(source)),
    sink_(std::move(sink)),
    patoms_(std::move(patoms)),
    priority_(priority){

    BB_ASSERT(source_->isSource());
    BB_ASSERT(sink_->isSink());
    // set global states
    sinkGlobalState_ = sink_->getGlobalState();
    sourceGlobalState_ = source_->getGlobalState();
}

idx_t PhysicalRule::getPriority() const {
        return priority_;
}

void PhysicalRule::setPriority(idx_t priority) {
    priority_ = priority;
}

idx_t PhysicalRule::getSourceSize() const {
    return source_->estimatedCardinality_;
}

void PhysicalRule::incrementCompleted() {
    ++completed_;
}

idx_t PhysicalRule::getCompletedCount() const {
    return completed_.load();
}

std::string PhysicalRule::toString() const {
    std::string result = sink_->toString() + " :- " + source_->toString();
    for (auto & patom: patoms_) {
        result += ", "+ patom->toString() ;
    }
    return result;
}

idx_t PhysicalRule::getPhysicalAtomsSize() const {
    return patoms_.size();
}

std::vector<PhysicalAtom*> PhysicalRule::getPhysicalAtoms() const {
    std::vector<PhysicalAtom*> patoms;
    patoms.push_back(source_.get());
    for (auto& patom: patoms_) {
        patoms.push_back(patom.get());
    }
    patoms.push_back(sink_.get());
    return patoms;
}

bool PhysicalRule::isFinalized() const {
    return finalized_.load();
}

void PhysicalRule::setFinalized() {
    finalized_.store(true);
}


}
