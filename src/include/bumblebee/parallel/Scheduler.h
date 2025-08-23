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
#include "Task.h"

namespace bumblebee{

// Responsible for scheduling and managing the execution of physical rules.
// - Groups rules by priority.
// - Generates execution tasks for each rule based on the source size and morsel size.
// - Tracks completion of tasks using atomic counters.
// - Schedules a finalization task for each rule once all its execution tasks are completed.
// The scheduler enqueues all tasks into the shared concurrent queue.
class Scheduler {
public:
	ClientContext& context_;
	ConcurrentQueue queue_;


	constexpr static int64_t WAIT_TIMEOUT_USECS = 100000; // 100ms
    Scheduler(ClientContext& context);

	PhysicalAtomProfiler getAtomProfiler() const;
	void clearThreadContexts();

    // Schedule all the rules
    void scheduleRules(PhysicalRulesBucket& bucket);
    // Schedule the rules for one priority
    void schedulePriorityRules(prule_ptr_vector_t& bucket);
    // Schedule a single rule, return the number of the tasks generated
    idx_t scheduleRule(prule_ptr_t& rule);
    // Schedule a finalize task for single rule
    void scheduleFinalize(prule_ptr_t& rule);
    // Groups rules into separate vectors based on their priority.
    // Returns a vector of vectors, where each inner vector contains rules with the same priority.
    // The outer vector is indexed by priority, i.e., index 0 holds rules with priority 0, index 1 with priority 1, etc.
    vector<prule_ptr_vector_t> bucketByPriority(prule_ptr_vector_t& rules);
	// List of thread context of all the tasks
	// contains the execution time of the rules
	vector<thread_context_ptr_t> tcontexts_;


};


}
