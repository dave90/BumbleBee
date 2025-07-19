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
#include "PhysicalRule.h"

namespace bumblebee{

// Executor of a Physical Rule.
// Each execution runs in its own thread.
// Manages the local state of the physical atom (patom).
//
// Execution flow:
// - Initialize patom states
// - Retrieve data from the source patom
// - Process data through the chain
// - Send output to the sink
//
// The `finalize` method of the patom is called by the scheduler
// once all threads have completed execution.
class PhysicalRuleExecutor {
private:
    prule_ptr_t prule_;

    pstate_ptr_vector_t states_;
    // intermediate chunks
    data_chunk_vector_t chunks_;

    pstate_ptr_t sourceState_;
    pstate_ptr_t sinkState_;

    // final chunk to send into the sink
    DataChunk finalChunk_;
    // result type for each intermediate physical atom
    std::vector<AtomResultType> atomResults_;

    // TODO Execution context with a  profiler
};


}
