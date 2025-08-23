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
#pragma once

#include "PhysicalAtom.h"


namespace bumblebee{

// Represents a compiled physical rule consisting of source, sink, and intermediate atoms.
// - Holds shared global state and priority metadata.
// - Tracks the number of completed task executions.
// - Tracks whether the rule has been finalized.
// Used by the scheduler to manage rule execution lifecycle.
class PhysicalRule {

    friend class PhysicalRuleExecutor;

public:
    PhysicalRule(patom_ptr_t &source, patom_ptr_t &sink, patom_ptr_vector_t &patoms, idx_t priority );

    idx_t getSourceSize() const;
    idx_t getPriority() const;
    void setPriority(idx_t priority);
    void incrementCompleted();
    idx_t getCompletedCount() const;
    bool isFinalized() const;
    void setFinalized();

    std::string toString()const;
    idx_t getPhysicalAtomsSize() const;
    vector<PhysicalAtom*> getPhysicalAtoms() const;

private:
    // The patom that generates the chunks
    patom_ptr_t source_;
    // The patom that sink the output chunks
    patom_ptr_t sink_;
    // Intermediate patoms (filters, hash join etc.)
    patom_ptr_vector_t patoms_;

    // The global states shared among tasks
    gpstate_ptr_t sourceGlobalState_;
    gpstate_ptr_t sinkGlobalState_;

    // decreasing priority, 0 "highest priority"
    idx_t priority_;

    // counter of the completed execution
    std::atomic<idx_t> completed_{0};
    // if the finalze was called
    std::atomic<bool> finalized_{false};
};

using prule_ptr_t = std::shared_ptr<PhysicalRule>;
using prule_ptr_vector_t = vector<prule_ptr_t>;

struct PhysicalRulesBucket {
    prule_ptr_vector_t exit_;
    prule_ptr_vector_t constraints_;
    prule_ptr_vector_t recursive_;
};

}
