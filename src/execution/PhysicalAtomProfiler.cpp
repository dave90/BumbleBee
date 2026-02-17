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
#include "bumblebee/execution/PhysicalAtomProfiler.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/execution/PhysicalAtom.hpp"
#include <iomanip>
#include <sstream>

namespace bumblebee{

#if PROFILING == 1
void PhysicalAtomProfiler::startPhysicalAtom(const PhysicalAtom *patom) {
    active_patom_ = patom;
    profiler_.start();
}

void PhysicalAtomProfiler::endPhysicalAtom() {
    profiler_.end();
    auto elapsed = profiler_.elapsed();
    auto find = profilingInfo_.find(active_patom_);
    if (find == profilingInfo_.end()) {
        profilingInfo_.insert(std::make_pair(active_patom_, ProfilingInformation{elapsed, 0}));

    }else {
        find->second.elements_ += 0;
        find->second.time_ += elapsed;
    }
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

void PhysicalAtomProfiler::endPhysicalAtomCombine() {
    profiler_.end();
    auto elapsed = profiler_.elapsed();
    auto find = profilingInfo_.find(active_patom_);
    if (find == profilingInfo_.end()) {
        profilingInfo_.insert(std::make_pair(active_patom_, ProfilingInformation{0, 0, 0, elapsed}));
    }else {
        find->second.combineTime_ += elapsed;
    }
}

static string formatNumber(idx_t n) {
    if (n == 0) return "0";
    string s = std::to_string(n);
    string result;
    int count = 0;
    for (int i = (int)s.size() - 1; i >= 0; --i) {
        if (count > 0 && count % 3 == 0) result = "," + result;
        result = s[i] + result;
        count++;
    }
    return result;
}

static string formatTime(double t) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << t;
    return oss.str() + "s";
}

static string padRight(const string& s, idx_t width) {
    if (s.size() >= width) return s.substr(0, width);
    return s + string(width - s.size(), ' ');
}

static string padLeft(const string& s, idx_t width) {
    if (s.size() >= width) return s;
    return string(width - s.size(), ' ') + s;
}

static string repeatStr(const string& s, idx_t count) {
    string result;
    result.reserve(s.size() * count);
    for (idx_t i = 0; i < count; ++i) result += s;
    return result;
}

static string buildTable(const vector<std::pair<const PhysicalAtom*, ProfilingInformation>>& entries) {
    // Column widths
    constexpr idx_t COL_OP = 34;
    constexpr idx_t COL_ROWS = 14;
    constexpr idx_t COL_TIME = 10;
    constexpr idx_t COL_FIN = 10;
    constexpr idx_t COL_COMB = 10;
    // Inner width = sum of segment widths + 4 inner column separators (┼)
    // Each segment: COL_X + 1 (for leading space)
    const idx_t totalWidth = (COL_OP + 1) + (COL_ROWS + 1) + (COL_TIME + 1) + (COL_FIN + 1) + (COL_COMB + 1) + 4;

    const string H = "\u2500"; // ─
    std::ostringstream ss;

    // Top border
    ss << "\u250c" << repeatStr(H, totalWidth) << "\u2510\n";

    // Header
    ss << "\u2502 " << padRight("Operator", COL_OP)
       << "\u2502" << padLeft("Rows", COL_ROWS)
       << " \u2502" << padLeft("Time", COL_TIME)
       << " \u2502" << padLeft("Finalize", COL_FIN)
       << " \u2502" << padLeft("Combine", COL_COMB)
       << " \u2502\n";

    // Header separator
    ss << "\u251c" << repeatStr(H, COL_OP + 1) << "\u253c"
       << repeatStr(H, COL_ROWS + 1) << "\u253c"
       << repeatStr(H, COL_TIME + 1) << "\u253c"
       << repeatStr(H, COL_FIN + 1) << "\u253c"
       << repeatStr(H, COL_COMB + 1) << "\u2524\n";

    // Data rows
    double totalTime = 0;
    for (auto& [atom, info] : entries) {
        auto name = atom->getName();
        auto rowTime = info.time_ + info.finalizeTime_ + info.combineTime_;
        totalTime += rowTime;

        ss << "\u2502 " << padRight(name, COL_OP)
           << "\u2502" << padLeft(formatNumber(info.elements_), COL_ROWS)
           << " \u2502" << padLeft(formatTime(info.time_), COL_TIME)
           << " \u2502" << padLeft(formatTime(info.finalizeTime_), COL_FIN)
           << " \u2502" << padLeft(formatTime(info.combineTime_), COL_COMB)
           << " \u2502\n";
    }

    // Bottom separator
    ss << "\u251c" << repeatStr(H, totalWidth) << "\u2524\n";

    // Total row
    ss << "\u2502 " << padRight("Total CPU time: " + formatTime(totalTime), totalWidth - 1) << "\u2502\n";

    // Bottom border
    ss << "\u2514" << repeatStr(H, totalWidth) << "\u2518\n";

    return ss.str();
}

string PhysicalAtomProfiler::toString() const {
    vector<std::pair<const PhysicalAtom*, ProfilingInformation>> entries;
    for (auto& [key, value] : profilingInfo_)
        entries.emplace_back(key, value);
    return buildTable(entries);
}

string PhysicalAtomProfiler::toString(vector<PhysicalAtom*>& patoms) const {
    vector<std::pair<const PhysicalAtom*, ProfilingInformation>> entries;
    for (auto& patom : patoms) {
        BB_ASSERT(profilingInfo_.contains(patom));
        entries.emplace_back(patom, profilingInfo_.find(patom)->second);
    }
    return buildTable(entries);
}

void PhysicalAtomProfiler::append(const PhysicalAtomProfiler &other) {
    for (auto& [key, value]:other.profilingInfo_) {
        if (!profilingInfo_.contains(key)) {
            profilingInfo_[key] = ProfilingInformation();
        }
        profilingInfo_[key].elements_ += value.elements_;
        profilingInfo_[key].time_ += value.time_;
        profilingInfo_[key].finalizeTime_ += value.finalizeTime_;
        profilingInfo_[key].combineTime_ += value.combineTime_;
    }
}

#else
void PhysicalAtomProfiler::startPhysicalAtom(PhysicalAtom *patom) {}

void PhysicalAtomProfiler::endPhysicalAtom(DataChunk &chunk, bool isFinalize) {}


string PhysicalAtomProfiler::toString() const {}
#endif

}
