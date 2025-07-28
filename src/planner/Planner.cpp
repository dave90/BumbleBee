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

#include "bumblebee/planner/Planner.h"

#include "bumblebee/common/Log.h"
#include "bumblebee/planner/optimizer/PhysicalOptimizer.h"
#include "bumblebee/planner/rewriter/ArithRewriter.h"
#include "bumblebee/planner/rewriter/FilterPushDownRewriter.h"
#include "bumblebee/planner/rewriter/VariablesRewriter.h"

namespace bumblebee {


PhysicalRulesBucket Planner::plan(RulesBucket &rules) {
    executeRewriters(rules);

    PhysicalRulesBucket physicalRules;
    executeOptimizer(rules, physicalRules);

    LOG_DEBUG("Physical exit rules: ");
    for (auto& prule: physicalRules.exit_)
        LOG_DEBUG("%s", prule->toString().c_str());
    LOG_DEBUG("Physical constraints rules: ");
    for (auto& prule: physicalRules.constraints_)
        LOG_DEBUG("%s", prule->toString().c_str());
    LOG_DEBUG("Physical recursive rules: ");
    for (auto& prule: physicalRules.recursive_)
        LOG_DEBUG("%s", prule->toString().c_str());

    return physicalRules;
}

void Planner::executeOptimizer(RulesBucket & rules, PhysicalRulesBucket & prules) {
    PhysicalOptimizer optimizer(context);
    for (auto& rule: rules.exit_) {
        for (auto& prule: optimizer.optimize(rule))
            prules.exit_.emplace_back(prule);
        optimizer.clear();
    }
    for (auto& rule: rules.constraints_) {
        for (auto& prule: optimizer.optimize(rule))
            prules.constraints_.emplace_back(prule);
        optimizer.clear();
    }
    for (auto& rule: rules.recursive_) {
        for (auto& prule: optimizer.optimize(rule))
            prules.recursive_.emplace_back(prule);
        optimizer.clear();
    }
}

void Planner::executeRewriters(RulesBucket &rules) {
    // logical -> logical rewrite

    /*TODO:
     *- constant evaluate , bot X = 2 +2 and 10 > 20
     *- rule ordering
     *- common subexpression , i.e. X = Y +1, Z = Y +1
     *- arith simplication , X * 1 > Y -> X > Y
     */

    LOG_DEBUG("Rewriting rules...");
    std::vector<rewriter_ptr_t> rewriters;
    rewriters.emplace_back(new VariablesRewriter());
    rewriters.emplace_back(new ArithRewriter());
    rewriters.emplace_back(new FilterPushDownRewriter());

    for (auto& rule: rules.exit_) {
        for (auto& rewriter : rewriters) {
            rewriter->rewrite(rule);
        }
    }
    for (auto& rule: rules.constraints_) {
        for (auto& rewriter : rewriters) {
            rewriter->rewrite(rule);
        }
    }
    for (auto& rule: rules.recursive_) {
        for (auto& rewriter : rewriters) {
            rewriter->rewrite(rule);
        }
    }

    for (auto& rule: rules.exit_) {
        LOG_DEBUG("Rule rewrited: %s", rule.toString().c_str());
    }
    for (auto& rule: rules.constraints_) {
        LOG_DEBUG("Rule rewrited: %s", rule.toString().c_str());
    }
    for (auto& rule: rules.recursive_) {
        LOG_DEBUG("Rule rewrited: %s", rule.toString().c_str());
    }
}
}
