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

#include <thread>

#include "concurrentqueue/blockingconcurrentqueue.h"
#include "concurrentqueue/lightweightsemaphore.h"
#include <gtest/gtest.h>

using namespace moodycamel;
using namespace std;

TEST(ConcurrentQueueTest, SimpleQueueTest) {
    BlockingConcurrentQueue<int> q;
    std::thread producer([&]() {
        for (int i = 0; i != 100; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(i % 10));
            q.enqueue(i);
        }
    });
    std::thread consumer([&]() {
        for (int i = 0; i != 100; ++i) {
            int item;
            q.wait_dequeue(item);
            assert(item == i);

            if (q.wait_dequeue_timed(item, std::chrono::milliseconds(5))) {
                ++i;
                assert(item == i);
            }
        }
    });
    producer.join();
    consumer.join();

    assert(q.size_approx() == 0);

}
