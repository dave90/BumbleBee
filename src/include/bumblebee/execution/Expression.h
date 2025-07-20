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
#include "bumblebee/common/types/DataChunk.h"
#include "bumblebee/parser/statement/Atom.h"

namespace bumblebee{

struct Operands {
    std::vector<idx_t> cols_;
    std::vector<Operator> operators_;
};

class Expression {
public:
    Expression(Binop op, const Operands &left, const Operands &right);
    std::string toString();
    // execute the expression
    void execute(DataChunk &input, DataChunk &output);

    // Execute a operands and set the result in result vector
    Vector executeOperands(DataChunk &input, Operands& op);
    // Execute the binop operation and set the results in the selection vector
    // and returns the numbers of rows filtered
    idx_t executeBinop(Vector& left,Vector& right, SelectionVector& sel);
    // verify the expression
    bool verify();

private:
    Binop op_;
    // if is assignment the column to set in the datachunk
    Operands left_;
    Operands right_;

};


}
