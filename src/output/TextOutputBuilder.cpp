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

#include "bumblebee/output/OutputBuilder.h"

namespace bumblebee {


void TextOutputBuilder::outputAtoms(const DataChunk &chunk, Predicate *predicate) {
    auto arity = predicate->getArity();
    for (idx_t row = 0; row < chunk.getSize();++row) {
        std::cout << predicate->getName() << "( " << chunk.getValue(0, row).toString();
        for (idx_t col = 1; col < arity; ++col) {
            std::cout << ", " << chunk.getValue(row, col).toString();
        }
        std::cout << " )." <<std::endl;
    }

}

}