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
#include "Task.hpp"

namespace bumblebee{

// Manages a pool of worker threads that consume and execute tasks from the queue.
// - Spawns a fixed number of threads.
// - Each thread waits for tasks and executes them in a loop.
// - Uses atomic markers to gracefully shut down threads.
// - Notifies the scheduler via a semaphore when tasks are completed.
class TaskExecutor {
    constexpr static int64_t WAIT_TIMEOUT_USECS = 100000; // 100ms
    using thread_ptr_t = std::unique_ptr<std::thread>;
    using vector_thread_ptr_t = vector<thread_ptr_t>;

public:

    explicit TaskExecutor(ConcurrentQueue &queue, idx_t threads);
    void startThreads();
    void stopThreadsAndJoin();

    static void executeForeverTask(ConcurrentQueue* queue_, std::atomic<bool> *marker);

private:
    // numbers of threads to spin
    idx_t threadsNumber_;
    // queue
    ConcurrentQueue& queue_;
    // list of internal threads
    vector_thread_ptr_t threads_;
    // vector of markers, usefull to stop the threads
    vector<std::unique_ptr<std::atomic<bool>>> markers_;
};

}
