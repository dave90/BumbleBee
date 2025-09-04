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
#include <utility>

#include "bumblebee/execution/PhysicalRuleExecutor.hpp"
#include "concurrentqueue/blockingconcurrentqueue.h"

namespace bumblebee{

// Base class representing a unit of work to be executed by a thread.
// Wraps a PhysicalRuleExecutor, which performs the rule's execution logic.
class Task {
public:
    virtual ~Task() = default;

    explicit Task(rule_executor_ptr_t rule)
        : rule_(std::move(rule)) {
    }

    virtual void execute() {
        rule_->execute();
    }

protected:
    rule_executor_ptr_t rule_;

};

// FinalizeTask is a special kind of Task that invokes finalize() instead of execute().
// Used by the scheduler to clean up rule state once all parallel executions complete.
class FinalizeTask : public Task {
public:
    explicit FinalizeTask(rule_executor_ptr_t rule)
        : Task(std::move(rule)) {
    }

    void execute() override {
        rule_->finalize();
    }
};

using task_ptr_t = std::unique_ptr<Task>;

// Shared queue structure for managing scheduled tasks.
struct ConcurrentQueue {
    moodycamel::BlockingConcurrentQueue<task_ptr_t> q;
    // semaphore notification for the ending task
    moodycamel::LightweightSemaphore semaphore;
};

}
