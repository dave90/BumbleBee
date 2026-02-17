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
#include "bumblebee/common/Profiler.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{
class DataChunk;
class PhysicalAtom;

struct ProfilingInformation {
    double time_ {0};
    double finalizeTime_ {0};
    double combineTime_ {0};
    idx_t elements_ {0};
    explicit ProfilingInformation(double time = 0, idx_t elements = 0, double finalizeTime_ = 0, double combineTime = 0) : time_(time), elements_(elements), finalizeTime_(finalizeTime_), combineTime_(combineTime) {}
};

using patom_profiling_map_t = std::unordered_map<const PhysicalAtom*, ProfilingInformation>;

// Profiler of the physical atoms
class PhysicalAtomProfiler {

public:
    void startPhysicalAtom(const PhysicalAtom* patom);
    void endPhysicalAtom(DataChunk& chunk);
    void endPhysicalAtom();
    void endPhysicalAtomFinalize();
    void endPhysicalAtomCombine();
    patom_profiling_map_t& getProfilingMap() {
        return profilingInfo_;
    }
    string toString()const;
    string toString(vector<PhysicalAtom*>& patoms)const;
    // append the profilation of other in this profiler
    void append(const PhysicalAtomProfiler& other);

private:
    const PhysicalAtom *active_patom_{nullptr};

    // Profiler for time profiling
    Profiler profiler_;
    // Store all the profiling information for each patom
    patom_profiling_map_t profilingInfo_;
};


}
