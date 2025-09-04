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
#include "bumblebee/common/types/Graph.h"
#include "bumblebee/parser/statement/Rule.h"

namespace bumblebee{


// This class is responsible to find the correct order of the rules
class StatementDependency {
    using predicate_rule_map_t = std::unordered_map<std::string, vector<idx_t>>;
    using predicate_component_map_t = std::unordered_map<std::string, idx_t> ;
    using unstrat_predicates_t = std::unordered_set<std::string> ;
    using label_predicate_map_t = std::unordered_map<std::string,Predicate*>;
    using comp_graph_orders_t = vector<vector<idx_t>>;
public:
    StatementDependency(rules_vector_t &&program);

    bool isExitRule(idx_t component, Rule & rule);

    // order the rules in the program
    // each vector contains the set of the rules that can be executed
    // in the same iteration
    rules_bucket_vector_t orderRules();

private:

    void createDependencyGraph();
    void addEdgeDepGraph(Predicate*p1,Predicate*p2,edge_weight_t w);
    void addRuleMapping(idx_t ruleIndex);

    void createAndOrderComponentGraph();
    void recursiveOrderComponentGraph();

    rules_vector_t program_;
    label_predicate_map_t label_map_;
    // Dependency graph info
    Graph depGraph_{true};
    Graph stratDepGraph_{false};
    predicate_rule_map_t predicate_head_mapping_;
    predicate_rule_map_t predicate_body_mapping_;
    predicate_component_map_t  predComponentsMap_;
    unstrat_predicates_t unstratPredicates_;
    // component graph
    Graph compGraph_{false};
    comp_graph_orders_t orderedCompGraph_;
};


}
