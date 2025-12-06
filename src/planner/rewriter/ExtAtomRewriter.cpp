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

#include "bumblebee/planner/rewriter/ExtAtomRewriter.hpp"

#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee {
ExtAtomRewriter::ExtAtomRewriter(ClientContext &context): context_(context) {
}

ExtAtomRewriter::~ExtAtomRewriter() {
}

void ExtAtomRewriter::rewrite(Rule &rule) {
    // check if the head atom is ext atom and finalize function is defined
    for (auto& atom: rule.getHead()) {
        if (atom.getType() != EXTERNAL)continue;
        auto func = (PredFunction*) context_.functionRegister_.getFunction(atom.getExternalFunctionName(), atom.getInputValuesCType()).get();
        if (func->finalize_function_ == nullptr || func->combine_function_ == nullptr) {
            ErrorHandler::errorNotImplemented(
                " function '" + atom.getExternalFunctionName() +
                "' cannot be used in the head of a rule."
            );
        }
    }

    if (rule.getBody().empty())return;

    // check that the first atom in the rule if is an external support the data sourcing
    auto& source = rule.getBody().front();
    if (source.getType() != EXTERNAL) return;
    auto func = (PredFunction*) context_.functionRegister_.getFunction(source.getExternalFunctionName(), source.getInputValuesCType()).get();
    if (func->maxThreadFunction_ != nullptr) return;

    // first atom cannot be a source so move it

    idx_t idx = 0;
    for (auto& atom: rule.getBody()) {
        if (atom.getType() == CLASSICAL)break;
        ++idx;
    }

    if (idx == 0) {
        ErrorHandler::errorNotImplemented(
            "function '" + source.getExternalFunctionName() +
            "' cannot be used in this rule because it contains no classical atoms."
        );
    }

    Atom tmp = std::move(rule.getBody()[idx]);
    rule.getBody()[idx] = std::move(source);
    rule.getBody()[0] = std::move(tmp);
}
}
