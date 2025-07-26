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

#include "ClientContext.h"
#include "parallel/Scheduler.h"
#include "planner/StatementDependency.h"

namespace bumblebee {

class BumbleBeeDB {
public:
    BumbleBeeDB() = default;
    ~BumbleBeeDB() = default;

    int parseArgs(int, char **);
    void printArgs();

    void parseProgram(rules_vector_t &program);
    void processProgram(rules_vector_t& program, Scheduler& scheduler);
    void run();
    void printIDB();

private:
    void processBucketRules( RulesBucket & bucket, Scheduler& scheduler);

    ClientContext context_;


};
} // bumblebee
