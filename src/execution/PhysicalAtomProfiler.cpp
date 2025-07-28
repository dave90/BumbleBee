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
#include "bumblebee/execution/PhysicalAtomProfiler.h"
#include "bumblebee/common/Constants.h"
#include "bumblebee/common/types/DataChunk.h"
#include "bumblebee/execution/PhysicalAtom.h"

namespace bumblebee{

#if PROFILING == 1
void PhysicalAtomProfiler::startPhysicalAtom(const PhysicalAtom *patom) {
    active_patom_ = patom;
    profiler_.start();
}

void PhysicalAtomProfiler::endPhysicalAtom(DataChunk &chunk) {
    profiler_.end();
    auto elapsed = profiler_.elapsed();
    auto find = profilingInfo_.find(active_patom_);
    if (find == profilingInfo_.end()) {
            profilingInfo_.insert(std::make_pair(active_patom_, ProfilingInformation{elapsed, chunk.getSize()}));

    }else {
        find->second.elements_ += chunk.getSize();
        find->second.time_ += elapsed;
    }
}

void PhysicalAtomProfiler::endPhysicalAtomFinalize() {
    profiler_.end();
    auto elapsed = profiler_.elapsed();
    auto find = profilingInfo_.find(active_patom_);
    if (find == profilingInfo_.end()) {
        profilingInfo_.insert(std::make_pair(active_patom_, ProfilingInformation{0, 0, elapsed}));
    }else {
        find->second.finalizeTime_ += elapsed;
    }
}

string PhysicalAtomProfiler::toString() const {
    string result;
    for (auto& [key, value]:profilingInfo_) {
        result += key->toString() + "\t" + std::to_string(value.elements_) + "\t"  + std::to_string(value.time_) + "\t" + std::to_string(value.finalizeTime_) + "\n";
    }
    return result;
}

#else
void PhysicalAtomProfiler::startPhysicalAtom(PhysicalAtom *patom) {}

void PhysicalAtomProfiler::endPhysicalAtom(DataChunk &chunk, bool isFinalize) {}


string PhysicalAtomProfiler::toString() const {}
#endif

}
