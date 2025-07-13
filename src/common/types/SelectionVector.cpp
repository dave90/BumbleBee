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
#include "bumblebee/common/types/SelectionVector.h"

#include <string>

namespace bumblebee{

sel_ptr_t SelectionVector::slice(const SelectionVector &sel, idx_t count) const {
    sel_ptr_t data = sel_ptr_t(new sel_t[count]);
    for (unsigned i = 0; i < count; i++) {
        auto idx = sel.getIndex(i);
        data[i] = this->getIndex(idx);
    }
    return data;
}

std::string SelectionVector::toString(idx_t count) {
    if (!sel_data_) return "";
    std::string r = "";
    for (unsigned i = 0; i < count; i++) {
        r += std::to_string(sel_vector_[i]) + ", ";
    }
    return r;
}

}
