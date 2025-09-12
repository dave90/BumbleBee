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
#include "bumblebee/execution/PhysicalRule.hpp"
#include "bumblebee/parser/statement/Rule.hpp"

namespace bumblebee{


class Planner {
public:
    explicit Planner(ClientContext &context): context_(context) {}
    virtual ~Planner() = default;

    virtual PhysicalRulesBucket plan(RulesBucket& rules);

private:
    void executeRewriters(RulesBucket& rules);
    void executeOptimizer(RulesBucket&, PhysicalRulesBucket& );

    ClientContext& context_;
};



}
