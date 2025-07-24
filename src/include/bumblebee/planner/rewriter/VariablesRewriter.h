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
#include "Rewriter.h"
#include "bumblebee/parser/statement/Rule.h"

namespace bumblebee{

// This rewriter rewrites the variables in the rule
class VariablesRewriter : public Rewriter {
public:

    void rewrite(Rule& rule) override;
private:
    // Try to rewrite the unused variable with anonymous
    void pushAnonymous(Rule &rule);
    // verify the rule is still correct after the rewrite
    // and prune atoms with all anonymous variables
    void verifyAndPruneAtoms(Rule& rule);
};


}
