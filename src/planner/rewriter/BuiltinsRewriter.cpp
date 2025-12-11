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
#include "bumblebee/planner/rewriter/BuiltinsRewriter.hpp"

namespace bumblebee{

void removeBuiltins(Atom& binop, std::unordered_set<idx_t>& toRemove) {
    if (toRemove.empty()) return;
    vector<Term> newTerms;
    vector<Binop> newBinops;
    auto binopTerms = binop.getBuiltinTerms();
    for (idx_t i=0;i<binopTerms.size();++i) {
        if (toRemove.contains(i))continue;
        newTerms.push_back(std::move(binopTerms[i].left));
        newTerms.push_back(std::move(binopTerms[i].right));
        newBinops.push_back(binop.getBinop(i));
    }
    binop.setTerms(newTerms);
    binop.setBinops(newBinops);
}

void removeDuplicates(Atom& binop) {
    // remove duplicates binop in binop OR list
    // X > 10 OR 10 < 10 -> X > 10
    BB_ASSERT(binop.isOrBuiltin());
    auto binopTerms = binop.getBuiltinTerms();
    std::unordered_set<idx_t> toRemove;
    for (idx_t i=0; i< binopTerms.size()-1; i++) {
        for (idx_t j= i+1;j< binopTerms.size(); j++) {
            if (Atom::isEqualBuiltins(binopTerms[i].right,binopTerms[i].left, binop.getBinop(i),
                binopTerms[j].right,binopTerms[j].left, binop.getBinop(j))) {
                toRemove.insert(i);
            }
        }
    }

    if (toRemove.empty())return;

    removeBuiltins(binop, toRemove);
}

bool removeDuplicatesBinops(Rule& rule) {
    // remove duplicates builtins and return true if
    // the body of the rule was changed
    // For example:
    // X > 10, X > 10 OR Y = 2, 10 < X --> X > 10, Y = 2

    // first remove the duplicates in single builtin (not or list)
    std::unordered_set<idx_t> toRemove;
    auto& body = rule.getBody();
    for (idx_t i=0;i<body.size();++i ) {
        if (toRemove.contains(i))continue;
        auto& atom1 = body[i];
        if (atom1.getType() != BUILTIN || atom1.isOrBuiltin())continue;
        for (idx_t j=i+1;j<body.size();++j) {
            auto& atom2 = body[j];
            if (atom2.getType() != BUILTIN || atom2.isOrBuiltin())continue;
            if (Atom::isEqualBuiltins(atom1.getTerms()[0],atom1.getTerms()[1], atom1.getBinop(),
                atom2.getTerms()[0],atom2.getTerms()[1], atom2.getBinop())) {
                toRemove.insert(i);
            }
        }
    }

    // now remove in builtin or list
    for (idx_t i=0;i<body.size();++i ) {
        if (toRemove.contains(i))continue;
        auto& atom1 = body[i];
        // loop over builtin not or list
        if (atom1.getType() != BUILTIN || atom1.isOrBuiltin())continue;
        for (idx_t j=0;j<body.size();++j) {
            auto& atom2 = body[j];
            // loop over builtin or list
            if (atom2.getType() != BUILTIN || !atom2.isOrBuiltin()) continue;
            // now loop the builtins inside the or list
            auto builtinTerms = atom2.getBuiltinTerms();
            for (idx_t k=0; k<builtinTerms.size();++k) {
                if (Atom::isEqualBuiltins(atom1.getTerms()[0],atom1.getTerms()[1], atom1.getBinop(),
                builtinTerms[k].left, builtinTerms[k].right, atom2.getBinop(k))) {
                    // we can remove this built in as one builtin in the or clause must be true
                    toRemove.insert(j);
                    break;
                }
            }
        }
    }

    if (toRemove.empty()) return false;

    // remove the duplicate binops
    vector<Atom> newBody;
    for (idx_t i=0;i<body.size();++i) {
        if (toRemove.contains(i)) continue;
        newBody.push_back(std::move(body[i]));
    }
    rule.setBody(newBody);

    return true;

}

BinopRewriter::~BinopRewriter() {
}

void BinopRewriter::rewrite(Rule &rule) {
    for (auto& atom:rule.getBody())
        if (atom.isOrBuiltin())
            removeDuplicates(atom);

    // remove duplicates until we do not remove any atom
    while (removeDuplicatesBinops(rule));
}
}
