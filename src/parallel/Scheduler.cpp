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
#include "../include/bumblebee/parallel/Scheduler.h"

#include "bumblebee/common/Log.h"

enum TaskStatus : uint8_t {
    RUNNING = 0,
    FINALIZING = 1,
    COMPLETED = 2
};

namespace bumblebee{
Scheduler::Scheduler(ClientContext &context):context_(context) {
}


std::vector<prule_ptr_vector_t> Scheduler::bucketByPriority(prule_ptr_vector_t &rules) {
    std::vector<idx_t> priorities;
    std::unordered_map<idx_t, prule_ptr_vector_t> priorityMap;

    for (auto& rule : rules) {
        auto priority = rule->getPriority();
        if (!priorityMap.count(priority)) {
            // first time we see this priority
            priorities.push_back(priority);
        }
        priorityMap[priority].push_back(rule);
    }
    std::sort(priorities.begin(), priorities.end());
    std::vector<prule_ptr_vector_t> result;
    result.reserve(priorities.size());
    for (auto p: priorities) {
        result.push_back(std::move(priorityMap[p]));
    }
    return result;
}

void Scheduler::schedulePriorityRules(prule_ptr_vector_t &bucket) {
    // schedule all the rules and then wait the completion
    // a rule can generate multiple tasks so keep track of the tasks
    // when all the tasks for a rule are completed then call the finalize task
    if (bucket.empty())return;
    LOG_DEBUG("Process rules for priority %d.", bucket[0]->getPriority());

    // store for each rules the expected number of completed tasks
    std::vector<idx_t> rulesTaskExpected;
    rulesTaskExpected.reserve(bucket.size());
    for (auto& rule: bucket) {
        auto taskExpected = scheduleRule(rule);
        rulesTaskExpected.push_back(taskExpected);
        LOG_DEBUG("Generate %d tasks for rule: %s.", taskExpected, rule->toString().c_str());
    }

    std::vector<TaskStatus> taskStatus;
    taskStatus.resize(bucket.size(), RUNNING);
    bool allCompleted = false;
    while (!allCompleted) {
        allCompleted = true;
        // wait for a signal with a timeout; the timeout allows us to periodically check
        queue_.semaphore.wait(WAIT_TIMEOUT_USECS);
        for (idx_t idx = 0;idx < bucket.size();++idx) {
            if (taskStatus[idx] == COMPLETED) continue;
            if ( bucket[idx]->isFinalized() ) {
                BB_ASSERT(taskStatus[idx] == FINALIZING);
                // finalization task also completed
                taskStatus[idx] = COMPLETED;
                LOG_DEBUG("Rule %s completed.",bucket[idx]->toString().c_str() );
                continue;
            }
            // if is finalized we expect completed <= expected,
            allCompleted = false;
            // if FINALIZING waiting the finalize task
            if (taskStatus[idx] == FINALIZING) continue;
            BB_ASSERT(taskStatus[idx] == RUNNING);
            // if completed task are >= to the scheduled tasks then run the finalization task
            if ( bucket[idx]->getCompletedCount() >= rulesTaskExpected[idx]) {
                LOG_DEBUG("Schedule finalization for rule: %s.",bucket[idx]->toString().c_str() );
                scheduleFinalize(bucket[idx]);
                taskStatus[idx] = FINALIZING;
            }
        }
    }
    LOG_DEBUG("All rules completed for priority %d.", bucket[0]->getPriority());
}

idx_t Scheduler::scheduleRule(prule_ptr_t& rule) {

    // generate tasks based on morsel size and source cardinality
    auto partitions = rule->getSourceSize() / MORSEL_SIZE + 1;
    for (idx_t idx = 0; idx < partitions; ++idx) {
        tcontexts_.emplace_back(new ThreadContext(context_));
        rule_executor_ptr_t execRule = rule_executor_ptr_t(new PhysicalRuleExecutor(rule, tcontexts_.back().get()));
        task_ptr_t newTask = task_ptr_t( new Task(execRule));
        queue_.q.enqueue(std::move(newTask));
    }
    return partitions;
}

void Scheduler::scheduleFinalize(prule_ptr_t& rule) {
    tcontexts_.emplace_back(new ThreadContext(context_));
    rule_executor_ptr_t execRule = rule_executor_ptr_t(new PhysicalRuleExecutor(rule, tcontexts_.back().get()));
    task_ptr_t newTask = task_ptr_t( new FinalizeTask(execRule));
    queue_.q.enqueue(std::move(newTask));
}

void Scheduler::scheduleRules(PhysicalRulesBucket &bucket) {
    auto rulesByPriorities = bucketByPriority(bucket.rules_);
    for (auto& prules : rulesByPriorities) {
        schedulePriorityRules(prules);
    }

    // TODO schedule the recursive rules
}

}
