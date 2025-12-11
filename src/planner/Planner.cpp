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

#include "bumblebee/planner/Planner.hpp"

#include "bumblebee/common/Log.hpp"
#include "bumblebee/planner/optimizer/PhysicalOptimizer.hpp"
#include "bumblebee/planner/rewriter/ArithRewriter.hpp"
#include "bumblebee/planner/rewriter/BuiltinsRewriter.hpp"
#include "bumblebee/planner/rewriter/ExtAtomRewriter.hpp"
#include "bumblebee/planner/rewriter/FilterPushDownRewriter.hpp"
#include "bumblebee/planner/rewriter/VariablesRewriter.hpp"

namespace bumblebee {


prule_ptr_vector_t Planner::plan(rules_vector_t &rules) {
    executeRewriters(rules);

    prule_ptr_vector_t physicalRules;
    executeOptimizer(rules, physicalRules);

    LOG_DEBUG("Physical rules: ");
    for (auto& prule: physicalRules)
        LOG_DEBUG("%s", prule->toString().c_str());

    return physicalRules;
}

void Planner::executeOptimizer(rules_vector_t & rules, prule_ptr_vector_t & prules) {
    PhysicalOptimizer optimizer(context_, recursiveRules_);
    for (auto& rule: rules) {
        for (auto& prule: optimizer.optimize(rule))
            prules.emplace_back(prule);
        optimizer.clear();
    }

}

void Planner::executeRewriters(rules_vector_t &rules) {
    // logical -> logical rewrite

    /*TODO:
     *- constant evaluate , bot X = 2 +2 and 10 > 20
     *- rule ordering
     *- common subexpression , i.e. X = Y +1, Z = Y +1
     *- arith simplication , X * 1 > Y -> X > Y
     */

    LOG_DEBUG("Rewriting rules...");
    vector<rewriter_ptr_t> rewriters;
    rewriters.emplace_back(new BinopRewriter());
    rewriters.emplace_back(new VariablesRewriter(context_));
    rewriters.emplace_back(new ExtAtomRewriter(context_));
    rewriters.emplace_back(new ArithRewriter());
    rewriters.emplace_back(new FilterPushDownRewriter());

    for (auto& rule: rules) {
        for (auto& rewriter : rewriters) {
            rewriter->rewrite(rule);
        }
    }

    for (auto& rule: rules) {
        LOG_DEBUG("Rule rewrote: %s", rule.toString().c_str());
    }
}
}
