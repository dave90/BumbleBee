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
#include "bumblebee/execution/Expression.h"
#include "bumblebee/execution/PhysicalAtom.h"

namespace bumblebee{

class PhysicalExpression : public PhysicalAtom {
public:
    PhysicalExpression(Expression& expr, std::vector<ConstantType>& types, idx_t estimated_cardinality);
    // constant assignment of the expression (X = 100)
    PhysicalExpression(idx_t col,Value& constantValue);
    ~PhysicalExpression() override;

    AtomResultType execute(ThreadContext& context, DataChunk &input, DataChunk &chunk, PhysicalAtomState &state) const override;
    string getName() const override;
    string toString() const override;

private:
    Expression expression_;
    bool constantAssignment_;
    Value constantValue_;
};


}
