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
#include "Rewriter.hpp"

namespace bumblebee{

// Order the Built in atoms and execute it as soon as possible
class FilterPushDownRewriter : public Rewriter{
public:
    ~FilterPushDownRewriter() override;
    void rewrite(Rule &rule) override;

private:
    // return true if the assignment is possible to evaluate with the current variables
    bool isAssignmePossibleToEvaluate(set_term_variable_t& currentVariables, set_term_variable_t& builtinVars, Atom& assignment);
};


}
