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

// Profiler for the internal functions
class FunctionProfiler {
public:
    struct Stats {
        std::uint64_t calls = 0;
        std::chrono::nanoseconds total{0};
    };

    void add(std::string_view name, std::chrono::nanoseconds elapsed) {
        std::lock_guard g(mu_);
        auto &s = data_[std::string(name)];
        s.calls += 1;
        s.total += elapsed;
    }

    // Example reporting function (customize as needed)
    std::string toString() const {
        std::string result;
        for (const auto& [name, s] : data_) {
            const double ms = std::chrono::duration<double, std::milli>(s.total).count();
            result += name + " | calls=" + std::to_string(s.calls)
               + " | total_ms=" + std::to_string(ms)
               + " | avg_us=" + std::to_string(ms * 1000.0 / (s.calls ? s.calls : 1)) + "\n";
        }
        return result;
    }

    static FunctionProfiler& instance() {
        // One global store.
        static FunctionProfiler p;
        return p;
    }

private:
    mutable std::mutex mu_;
    std::unordered_map<std::string, Stats> data_;
};

class ScopeTimer {
public:
    using clock = std::chrono::steady_clock;

    explicit ScopeTimer(std::string_view name, FunctionProfiler& prof = FunctionProfiler::instance())
        : name_(name), prof_(prof), start_(clock::now()) {}

    ~ScopeTimer() {
        const auto end = clock::now();
        prof_.add(name_, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_));
    }

    ScopeTimer(const ScopeTimer&) = delete;
    ScopeTimer& operator=(const ScopeTimer&) = delete;

private:
    std::string_view name_;
    FunctionProfiler& prof_;
    clock::time_point start_;
};

#if defined(_MSC_VER)
#define FUNC_NAME __FUNCSIG__
#else
#define FUNC_NAME __PRETTY_FUNCTION__
#endif

#define CONCAT_IMPL(a,b) a##b
#define CONCAT(a,b) CONCAT_IMPL(a,b)
#define PROFILE_SCOPE() ScopeTimer CONCAT(__scopeTimer, __LINE__)(FUNC_NAME)

}
