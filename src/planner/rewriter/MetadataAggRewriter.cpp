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
#include "bumblebee/planner/rewriter/MetadataAggRewriter.hpp"

#include <set>
#include <unordered_map>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/parquet/ParquetReader.hpp"

namespace bumblebee {

static constexpr const char* READ_PARQUET = "&read_parquet";

bool MetadataAggRewriter::countRowsFromMetadata(Atom& atom, idx_t& total) {
    if (atom.getType() != EXTERNAL) return false;
    if (atom.getExternalFunctionName() != READ_PARQUET) return false;
    auto& inputs = atom.getInputValues();
    if (inputs.size() != 1) return false;

    string folder = inputs[0].toString();
    auto& fs = *context_.fileSystem_;

    vector<string> files;
    try {
        if (!StringUtils::hasGlob(folder)) {
            if (fs.fileExists(folder)) {
                files.push_back(folder);
            } else if (fs.directoryExists(folder)) {
                auto sep = fs.getFileSeparator();
                files = fs.glob(folder + sep + "**" + sep + "*.parquet");
            } else {
                return false;
            }
        } else {
            files = fs.glob(folder);
        }

        if (files.empty()) return false;

        idx_t sum = 0;
        for (auto& f : files) {
            ParquetReader reader(context_, f, ParquetOptions{});
            sum += reader.numRows();
        }
        total = sum;
        return true;
    } catch (...) {
        // Any failure resolving/reading metadata: skip the optimization safely.
        return false;
    }
}

void MetadataAggRewriter::rewrite(rules_vector_t& program) {
    // Index predicate definitions (in heads) and body uses (classical body
    // atoms and aggregate-body classical atoms). Used to verify the scan
    // predicate is defined once and referenced only by the count rule.
    std::unordered_map<string, vector<idx_t>> defs;
    std::unordered_map<string, int> bodyUses;

    auto recordBodyUse = [&](Atom& a) {
        if (a.getType() == CLASSICAL && a.getPredicate())
            bodyUses[a.getPredicate()->getName()]++;
        if (a.getType() == AGGREGATE)
            for (auto& sub : a.getAggsAtoms())
                if (sub.getType() == CLASSICAL && sub.getPredicate())
                    bodyUses[sub.getPredicate()->getName()]++;
    };

    for (idx_t i = 0; i < program.size(); ++i) {
        for (auto& h : program[i].getHead())
            if (h.getType() == CLASSICAL && h.getPredicate())
                defs[h.getPredicate()->getName()].push_back(i);
        for (auto& b : program[i].getBody())
            recordBodyUse(b);
    }

    std::set<idx_t> rulesToErase;

    for (idx_t i = 0; i < program.size(); ++i) {
        Rule& rcount = program[i];

        // R_count: head single atom, body a single COUNT aggregate, no groups.
        if (rcount.getHead().size() != 1 || rcount.getBody().size() != 1) continue;
        Atom& agg = rcount.getBody()[0];
        if (agg.getType() != AGGREGATE) continue;
        auto& funcs = agg.getAggregateFunctions();
        if (funcs.size() != 1 || funcs[0] != COUNT) continue;
        if (agg.hasExplicitGroups()) continue;

        // The aggregate must range over exactly one classical predicate atom
        // (the materialised scan). A filter would appear as an extra atom.
        auto& aggBody = agg.getAggsAtoms();
        if (aggBody.size() != 1) continue;
        Atom& scanRef = aggBody[0];
        if (scanRef.getType() != CLASSICAL || !scanRef.getPredicate()) continue;

        // Counted term must be a single variable.
        auto& aggTerms = agg.getAggTerms();
        if (aggTerms.empty() || aggTerms[0].getType() != VARIABLE) continue;
        const string countedVar = aggTerms[0].getVariable();

        // Find the position of the counted variable inside the scan atom.
        auto& scanRefTerms = scanRef.getTerms();
        idx_t pos = scanRefTerms.size();
        for (idx_t k = 0; k < scanRefTerms.size(); ++k) {
            if (scanRefTerms[k].getType() == VARIABLE && scanRefTerms[k].getVariable() == countedVar) {
                pos = k;
                break;
            }
        }
        if (pos == scanRefTerms.size()) continue;

        // The scan predicate must be defined by exactly one rule and used only
        // here, so that we can prove COUNT(*) and safely drop the scan rule.
        const string scanPred = scanRef.getPredicate()->getName();
        auto defIt = defs.find(scanPred);
        if (defIt == defs.end() || defIt->second.size() != 1) continue;
        idx_t scanRuleIdx = defIt->second[0];
        if (scanRuleIdx == i) continue;
        if (bodyUses[scanPred] != 1) continue;

        Rule& rscan = program[scanRuleIdx];
        // R_scan: head P(...), body a single &read_parquet atom (no filter).
        if (rscan.getHead().size() != 1 || rscan.getBody().size() != 1) continue;
        Atom& scanHead = rscan.getHead()[0];
        if (scanHead.getTerms().size() <= pos) continue;
        // COUNT(*) proof: the counted position holds a constant in the scan head
        // (i.e. each row contributes a constant marker, not a real column value
        // as COUNT(DISTINCT col) would).
        if (scanHead.getTerms()[pos].getType() != CONSTANT) continue;

        Atom& scanAtom = rscan.getBody()[0];
        idx_t total = 0;
        if (!countRowsFromMetadata(scanAtom, total)) continue;

        // The head must expose only the aggregate result variable; replace it
        // with the constant row count, turning the rule into a fact.
        auto assignTerms = agg.getAssignmentTerms();
        if (assignTerms.size() != 1 || assignTerms[0].getType() != VARIABLE) continue;
        const string resultVar = assignTerms[0].getVariable();

        Atom& head = rcount.getHead()[0];
        if (head.getType() != CLASSICAL || !head.getPredicate()) continue;
        bool ok = true;
        terms_vector_t newTerms;
        for (auto& t : head.getTerms()) {
            if (t.getType() == VARIABLE) {
                if (t.getVariable() != resultVar) { ok = false; break; }
                newTerms.push_back(Term::createSmallestConstantNumericTerm((unsigned long long) total));
            } else {
                newTerms.push_back(t);
            }
        }
        if (!ok) continue;

        Predicate* headPred = head.getPredicate();
        Atom newHead = Atom::createClassicalAtom(headPred, std::move(newTerms));

        auto& headVec = rcount.getHead();
        headVec.clear();
        headVec.push_back(std::move(newHead));
        rcount.getBody().clear();

        rulesToErase.insert(scanRuleIdx);

        LOG_DEBUG("MetadataAggRewriter: folded COUNT(*) over %s into constant %llu",
                  scanPred.c_str(), (unsigned long long) total);
    }

    // Erase the now-dead scan rules (descending order to keep indices valid).
    for (auto it = rulesToErase.rbegin(); it != rulesToErase.rend(); ++it)
        program.erase(program.begin() + *it);
}

} // namespace bumblebee
