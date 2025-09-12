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
#include "bumblebee/planner/StatementDependency.hpp"

#include "bumblebee/common/Log.hpp"

#include <format>

namespace bumblebee{

StatementDependency::StatementDependency(rules_vector_t &&program): program_(std::move(program)) {}

void StatementDependency::createDependencyGraph() {
    LOG_DEBUG("Calculating dependency graph...");

    // Determine which predicates are EDB (extensional database) and IDB (intensional database).
    // - Predicates that appear in rule bodies are considered EDB.
    // - Predicates that appear in rule heads are considered IDB (not EDB).
    // Important: Body predicates must be marked as EDB *before* marking head predicates as not EDB,
    //            because a predicate may appear in both, and head status should override body status.

    for (unsigned int i = 0; i < program_.size(); ++i) {
        Rule &r = program_[i];
        predicates_ptr_set_t preds;
        for (auto& a:r.getBody()) a.getPredicates(preds);
        for (auto p:  preds) p->setEdb(true);
    }
    for (unsigned int i = 0; i < program_.size(); ++i) {
        Rule &r = program_[i];
        predicates_ptr_set_t preds;
        for (auto& a:r.getHead()) a.getPredicates(preds);
        for (auto p:  preds) p->setEdb(false);
    }
    for (unsigned int i = 0; i < program_.size(); ++i)
        addRuleMapping(i);


    // calculate strong components of dep graph
    auto components = depGraph_.calculateStrongComponent();
    // store the component id for each predicate
    for (unsigned int i = 0; i < components.size(); ++i) {
        for (auto& v : components[i])
            predComponentsMap_[v] = i;
    }

    LOG_DEBUG("%s",depGraph_.getDotFormat("DepGraph").c_str());
    for (const auto& [key, value] : predComponentsMap_) {
        LOG_DEBUG("(Predicate, Component): (%s, %d)", key.c_str(), value);
    }


    // calculate the unstrat predicates
    if (stratDepGraph_.getNumVertices() == 0)return;
    // recursion same rule, predicates are unstra
    auto& edges = stratDepGraph_.getEdges();
    for (unsigned i=0;i<edges.size();++i) {
        // check edge with the same vertex and weight is negative is unstrat
        if (edges[i].contains(i) && edges[i][i] < 0) {
            auto p = stratDepGraph_.getValue(i);
            unstratPredicates_.insert(stratDepGraph_.getValue(i));
            label_map_[p]->setSolved(false);
        }
    }


    LOG_DEBUG("Calculating unstrat predicates...");

    // calculate the component if the predicates in the strat dep graph
    auto stratComponents = stratDepGraph_.calculateStrongComponent();
    predicate_component_map_t stratComponentsMap;
    for (unsigned i=0;i<stratComponents.size();++i) {
        for (auto v : stratComponents[i])
            stratComponentsMap[v] = i;
    }
    // if a negative edge (negation) exist within the same components the predicates are all unstratified
    std::unordered_set<idx_t> componentVisited;
    for (unsigned i=0;i<edges.size();++i) {
        for (const auto& [key, value] : edges[i]) {
            if (value > 0) continue;
            auto c1 = stratComponentsMap[stratDepGraph_.getValue(i)];
            auto c2 = stratComponentsMap[stratDepGraph_.getValue(key)];
            if (c1 == c2 && !componentVisited.contains(c1)) {
                componentVisited.insert(c1);
                for (auto &p : stratComponents[c1]) {
                    // set all the predicates in the component unstrat
                    unstratPredicates_.insert(p);
                    label_map_[p]->setSolved(false);
                }


            }
        }
    }
    LOG_DEBUG("%s",stratDepGraph_.getDotFormat("StratGraph").c_str());
    for (auto& p : unstratPredicates_)
        LOG_DEBUG("Unstratified predicate: %s",p.c_str());

    if (!unstratPredicates_.empty()) {
        // TODO support unstrat predicates
        ErrorHandler::errorNotImplemented("Unstratified predicates currently not supported :(");
    }


}



void StatementDependency::addEdgeDepGraph(Predicate *p1, Predicate *p2, edge_weight_t w) {
    if (p1 != p2 || w <= 0) {
        stratDepGraph_.addEdge(p1->getLabel(), p2->getLabel(), w);
    }
    if (w > 0)
        depGraph_.addEdge(p1->getLabel(), p2->getLabel(), w);

}

void StatementDependency::addRuleMapping(idx_t ruleIndex) {
    predicates_ptr_set_t head_predicates, body_predicates;
    Rule& rule = program_[ruleIndex];
    rule.getPredicatesInHead(head_predicates);
    rule.getPredicatesInBody(body_predicates);
    for (auto& p: head_predicates) {
        predicate_head_mapping_[p->getLabel()].push_back(ruleIndex);
        label_map_[p->getLabel()] = p;
    }
    for (auto& p: body_predicates) {
        predicate_body_mapping_[p->getLabel()].push_back(ruleIndex);
        label_map_[p->getLabel()] = p;
    }

    predicates_ptr_set_t bodyPosPredVisited, bodyNegPredVisited;
    // for each predicate in the head
    for (auto& hp: head_predicates) {
        // for each atom in the body
        bool positiveEdgeFound=false;
        for (auto& ba: rule.getBody()) {
            predicates_ptr_set_t bps;
            ba.getPredicates(bps);
            // for each predicates in the atom in the body
            for (auto& bp: bps) {
                if (!ba.isNegative() && !bp->isEdb()  && !bodyPosPredVisited.contains(bp)) {
                    // positive atom first time visited and edb
                    bodyPosPredVisited.insert(bp);
                    addEdgeDepGraph(bp,hp,1);
                    positiveEdgeFound = true;
                } else {
                    if (ba.isNegative() && !bp->isEdb()  && !bodyNegPredVisited.contains(bp)) {
                        bodyNegPredVisited.insert(bp);
                        addEdgeDepGraph(bp, hp, -1);
                    }
                }
            }
        }
        // if no positive edge was added add a vertex in dependency graph
        if (!positiveEdgeFound)
            depGraph_.addVertex(hp->getLabel());

        bodyPosPredVisited.clear();
        bodyNegPredVisited.clear();
    }
}

void StatementDependency::createAndOrderComponentGraph() {
    LOG_DEBUG("Creating ComponentGraph...");
    // create the component graph
    for (const auto& [key, value] : predComponentsMap_) {
        // add a vertex in case of rules composed by facts in body
        compGraph_.addVertex(std::to_string(value));
        if (!predicate_head_mapping_.contains(key))continue;
        for (auto& ruleIndex: predicate_head_mapping_[key]) {
            // fo each rule where the predicate is in head iterate the body atoms
            Rule& rule = program_[ruleIndex];
            for (auto& ba:rule.getBody()) {
                // for each predicates in the atom add and edge between the component of the predicate in the head
                // and the component of the predicate in the body (if it is negative add negative edge)
                predicates_ptr_set_t bps;
                ba.getPredicates(bps);
                for (auto& bp: bps) {
                    // if is a fact predicate skip it or if is same predicate
                    auto bodyPredLabel = bp->getLabel();
                    if (!predComponentsMap_.contains(bp->getLabel()))continue;
                    auto bpc = predComponentsMap_[bp->getLabel()];
                    if (!predicate_head_mapping_.contains(bp->getLabel()) || bpc == value)continue;
                    compGraph_.addEdge(std::to_string(bpc), std::to_string(value), ba.isNegative() ? -1 : 1);
                }
            }
        }
    }
    LOG_DEBUG("%s",compGraph_.getDotFormat("CompGraph").c_str());
    // order the component graph
    if (compGraph_.getNumEdges() == 0) {
        //if there are no edges put all components in the same bucket
        orderedCompGraph_.emplace_back();
        std::unordered_set<idx_t> components;
        for (const auto& [_, value] : predComponentsMap_)
            components.insert(value);
        orderedCompGraph_[0].insert(orderedCompGraph_[0].end(), components.begin(), components.end());
        return;
    }
    try {
        for (auto& bucket: compGraph_.calculateTopologicalSort()) {
            orderedCompGraph_.emplace_back();
            for (auto c : bucket)
                orderedCompGraph_.back().push_back(atoi(c.c_str()));
        }
    }
    catch (const std::runtime_error& e) {
        // we do not have a dag in comp graph, call the sortRecursive function
        recursiveOrderComponentGraph();
    }

}

void StatementDependency::recursiveOrderComponentGraph() {
    // Recursive approach:
    // Detect all cycles in the graph. For each cycle, identify and remove one negative edge
    // connecting two vertices within the same cycle.
    // Perform a topological sort — if successful, exit.
    // If a cycle is still detected, repeat the process until all cycles are resolved.
    // TODO Optimize it, without recursion
    LOG_DEBUG("Calculate recursive component order...");
    auto strongComp = compGraph_.calculateStrongComponent();
    predicate_component_map_t components;
    for (unsigned int i = 0; i < strongComp.size(); ++i) {
        for (auto& v: strongComp[i])
            components[v] = i;
    }

    auto& edges = compGraph_.getEdges();
    std::unordered_set<idx_t> componentVisited;
    vector<std::pair<string, string>> edgesToRemove;
    for (unsigned i=0; i<edges.size(); i++) {
        for (auto [key, value]: edges[i]) {
            auto v1 = compGraph_.getValue(i);
            auto v2 = compGraph_.getValue(key);
            auto c1 = components[v1];
            auto c2 = components[v2];
            if (  c1 == c2 && !componentVisited.contains(c1) && value < 0) {
                componentVisited.insert(c1);
                edgesToRemove.emplace_back(v1,v2);
            }
        }
    }
    for (auto& p: edgesToRemove) {
        LOG_DEBUG("Removing edge from component graph (%s, %s)",p.first.c_str(), p.second.c_str());
        compGraph_.removeEdge(p.first, p.second);
    }

    try {
        for (auto& bucket: compGraph_.calculateTopologicalSort()) {
            orderedCompGraph_.emplace_back();
            for (auto c : bucket)
                orderedCompGraph_.back().push_back(atoi(c.c_str()));
        }
    }catch (const std::runtime_error& e) {
        // we do not have a dag in comp graph, call the sortRecursive function
        recursiveOrderComponentGraph();
    }
}

rules_bucket_vector_t StatementDependency::orderRules() {
    createDependencyGraph();
    createAndOrderComponentGraph();

    rules_bucket_vector_t orderedRules;

    // First, we store the predicates in the body of each constraint rule.
    // A constraint can only be processed once all its predicates have been evaluated.
    // To track this, we maintain a counter for each constraint rule representing
    // the number of pending predicates. Each time a predicate is processed,
    // we decrement the corresponding counter. When the counter reaches zero,
    // all predicates for that constraint have been evaluated, and the constraint can be processed.

    // Create the map of predicates -> set of rules
    // Create the counter map  constriants -> num of predicates
    std::unordered_map<Predicate*, vector<idx_t>> constraintsPredicateMap;
    std::unordered_map<idx_t, int> constraintsCounter;
    for (unsigned i=0; i<program_.size(); ++i) {
        Rule& rule = program_[i];
        // check if it is a contraint
        if (!rule.getHead().empty()) continue;
        predicates_ptr_set_t preds;
        for (auto& ba: rule.getBody()) ba.getPredicates(preds);
        for (auto p: preds) {
            constraintsPredicateMap[p].push_back(i);
            if (!constraintsCounter.contains(i))constraintsCounter[i]=0;
            ++constraintsCounter[i];
        }
    }

    // For each component store the vector of predicates
    std::unordered_map<idx_t, vector<Predicate*>> componentsPredicatesMap;
    for (auto& [predLabel, component]:predComponentsMap_)
        componentsPredicatesMap[component].push_back(label_map_[predLabel]);


    // create the order of the rules based on top. sort of components
    for (auto& components:orderedCompGraph_) {
        RulesBucket bucket;
        for (auto component : components) {
            for (auto predicate: componentsPredicatesMap[component]) {
                if (predicate_head_mapping_.contains(predicate->getLabel())) {
                    for (auto ruleIndex : predicate_head_mapping_[predicate->getLabel()]) {
                        Rule &rule = program_[ruleIndex];
                        if (isExitRule(component, rule ))
                            bucket.exit_.push_back(std::move(rule));
                        else
                            bucket.recursive_.push_back(std::move(rule));
                    }
                }
                // check if some constraints can be evaluated in this stage
                for (auto ruleIndex:constraintsPredicateMap[predicate]) {
                    if (--constraintsCounter[ruleIndex] == 0)
                        bucket.constraints_.push_back(std::move(program_[ruleIndex]));
                }
            }
        }
        if (!bucket.constraints_.empty()) {
            // TODO support constraints
            ErrorHandler::errorNotImplemented("Constraints rules currently not supported :(");
        }
        orderedRules.push_back(std::move(bucket));
    }


    return orderedRules;
}

bool StatementDependency::isExitRule(idx_t component, Rule &rule) {
    // is a recursive rule if the predicates in the body are predicates to be processed
    // of the same component and is present in the head of some rule
    predicates_ptr_set_t preds;
    for (auto& atom : rule.getBody()) {
        if (atom.isNegative())continue;
        atom.getPredicates(preds);
    }
    for (auto& p: preds)
        if (predicate_head_mapping_.contains(p->getLabel()) && predComponentsMap_[p->getLabel()] == component)
            return false;

    return true;
}

}
