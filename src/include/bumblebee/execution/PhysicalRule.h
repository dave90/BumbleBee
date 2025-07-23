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

// Physical Rule contains the source sink and atoms
// Contains also the global states shared by all the threads
class PhysicalRule {

    friend class PhysicalRuleExecutor;

public:
    PhysicalRule(patom_ptr_t &source, patom_ptr_t &sink, patom_ptr_vector_t &patoms, idx_t priority );

    idx_t getSourceSize() const;
    idx_t getPriority() const;
    void setPriority(idx_t priority);
    idx_t getPartitions() const;
    void setpartitions(idx_t partition);


private:
    patom_ptr_t source_;
    patom_ptr_t sink_;
    patom_ptr_vector_t patoms_;

    gpstate_ptr_t sourceGlobalState_;
    gpstate_ptr_t sinkGlobalState_;

    // decreasing priority, 0 "highest priority"
    idx_t priority_;
};

using prule_ptr_t = std::shared_ptr<PhysicalRule>;

}
