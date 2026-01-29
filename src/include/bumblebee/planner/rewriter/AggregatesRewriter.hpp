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
#include "bumblebee/common/Vector.hpp"
#include "bumblebee/parser/statement/Atom.hpp"
#include "bumblebee/parser/statement/Rule.hpp"

namespace bumblebee{

struct AggInfo {
    AggInfo(vector<Atom> &atoms, set_term_variable_t &groups, set_term_variable_t  &terms, bool distinct = true);

    string createAggPredicateName(idx_t& suffixCounter, const vector<Term>& terms);

    // atoms in the body of the aggregate
    vector<Atom>& atoms_;
    // aggregation terms (group variables + distinct aggregation variable)
    set_term_variable_t terms_;
    // group variable in terms
    set_term_variable_t groups_;
    // map of payload variable and agg function
    std::unordered_map<string, std::unordered_set<string>> payloadMap_;
    // name of the predicate
    string predName_;

    bool distinct_;
};



// Transform aggregate bodies (which may contain multiple atoms) into a single atom.
// Example transformation:
//
//   a(X), #sum{Y : b(X), c(X,Y)} = T.
//     -> a(X), #sum{Y : #AGG1_SUM_GROUP_0_PAYLOADS_1(X,Y)} = T.
//        #SUM_GROUP_0_PAYLOADS_1(X,Y) :- b(X), c(X,Y).
// The new predicate name encodes:
//   - the aggregates function (e.g., #SUM),
//   - the grouping variables,
//   - and the payload
// Details:
// - Groups are the variables shared between the aggregate and the rest of the rule body.
// - Payloads are the columns being aggregated. Note: payloads.size() == functions.size().
// - Since aggregate tables can apply multiple functions on the same groups, we can
//   reuse aggregates across rules when possible.
//
// Two aggregates can be reused if they share:
//   1. the same groups,
//   2. the same body atoms,
//   3. the same aggregation terms (group variables + variables defined inside the aggregate).
class AggregatesRewriter {
public:
    explicit AggregatesRewriter(ClientContext &clientContext);
    void rewrite(rules_vector_t& program);

private:
    void findAggregates(rules_vector_t& program);

    ClientContext& clientContext_;
    vector<idx_t> rulesWithAggregates_;
    idx_t newPredCounter_{0};
};


}
