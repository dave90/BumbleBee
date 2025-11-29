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
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/types/DataChunk.hpp"

namespace bumblebee{

enum class OrderType : uint8_t { INVALID = 0, ASCENDING = 2, DESCENDING = 3 };


struct OrderModifiers {
    OrderModifiers(OrderType order_type) : order_type(order_type) {
    }

    OrderType order_type;

    bool operator==(const OrderModifiers &other) const {
        return order_type == other.order_type;
    }

    static OrderModifiers parse(const string &val) {
        auto lcase = StringUtils::lower(val);
        OrderType order_type;
        if (StringUtils::startsWith(lcase, "asc")) {
            order_type = OrderType::ASCENDING;
        } else if (StringUtils::startsWith(lcase, "desc")) {
            order_type = OrderType::DESCENDING;
        } else {
            order_type = OrderType::INVALID;
            ErrorHandler::errorNotImplemented("create_sort_key modifier must start with either ASC or DESC");
        }

        return OrderModifiers(order_type);
    }

    string toString() const {
        switch (order_type) {
            case OrderType::ASCENDING:
                return "ASC";
            case OrderType::DESCENDING:
                return "DESC";
            default: ;
        }
        return "";
    }
};



struct CreateSortKey {
    static void createSortKey(DataChunk &input, const vector<OrderModifiers> &modifiers, Vector &result);
};


}
