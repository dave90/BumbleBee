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

#include <chrono>

namespace bumblebee{


class Profiler {
public:
    // Starts the timer
    void start() {
        finished_ = false;
        start_ = Tick();
    }
    // Finishes timing
    void end() {
        end_ = Tick();
        finished_ = true;
    }

    // Returns the elapsed time in seconds. If End() has been called, returns
    // the total elapsed time. Otherwise returns how far along the timer is
    // right now.
    double elapsed() const {
        auto end = finished_ ? end_ : Tick();
        return std::chrono::duration_cast<std::chrono::duration<double>>(end - start_).count();
    }

private:
    std::chrono::time_point<std::chrono::system_clock> Tick() const {
        return std::chrono::system_clock::now();
    }

    std::chrono::time_point<std::chrono::system_clock> start_;
    std::chrono::time_point<std::chrono::system_clock> end_;
    bool finished_ = false;

};


}
