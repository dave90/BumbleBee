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
#include "bumblebee/parallel/TaskExecutor.h"

#include "bumblebee/common/Log.h"

namespace bumblebee{

TaskExecutor::TaskExecutor(ConcurrentQueue &queue, idx_t thread): queue_(queue), threadsNumber_(thread) {
    BB_ASSERT(threadsNumber_ > 0);
}

void TaskExecutor::executeForeverTask(ConcurrentQueue *queue_, std::atomic<bool> *marker) {
    task_ptr_t task;
    // loop until the marker is true
    while (*marker) {
        bool res = queue_->q.wait_dequeue_timed(task ,WAIT_TIMEOUT_USECS);
        // if res is false no data in the queue, continue to spin if marker is true
        if (!res) continue;
        // task to execute
        task->execute();
        // signal the scheduler that the task is completed
        queue_->semaphore.signal();
    }
}

void TaskExecutor::startThreads() {
    LOG_DEBUG("Starting %d threads...",threadsNumber_);
    for (idx_t i = 0; i < threadsNumber_; ++i) {
        auto marker = std::make_unique<std::atomic<bool>>(true);
        auto thread = make_unique<std::thread>(TaskExecutor::executeForeverTask, &queue_, marker.get());
        threads_.push_back(std::move(thread));
        markers_.push_back(std::move(marker));
    }
    LOG_DEBUG("Thread started");
    BB_ASSERT(threads_.size() == markers_.size());
}

void TaskExecutor::stopThreadsAndJoin() {
    LOG_DEBUG("Stopping %d threads...",threadsNumber_);

    for (auto& marker: markers_) {
        marker->store(false);
    }
    for (auto& thread: threads_) {
        thread->join();
    }
    LOG_DEBUG("Thread stopped");
}
}
