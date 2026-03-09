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
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/execution/PhysicalAtomProfiler.hpp"

namespace bumblebee{

// Context information of the current thread execution
class ThreadContext {
public:
    explicit ThreadContext(ClientContext &context)
        : context_(context) {
    }

    ClientContext& context_;
    // profiler for each physical atoms
    PhysicalAtomProfiler profiler_;
};

using thread_context_ptr_t = std::unique_ptr<ThreadContext>;

}
