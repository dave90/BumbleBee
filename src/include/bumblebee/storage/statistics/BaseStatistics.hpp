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
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/serializer/Serializer.hpp"
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee{

enum class FilterPropagateResult : uint8_t {
    NO_PRUNING_POSSIBLE = 0,
    FILTER_ALWAYS_TRUE = 1,
    FILTER_ALWAYS_FALSE = 2,
    FILTER_TRUE_OR_NULL = 3,
    FILTER_FALSE_OR_NULL = 4
};


class BaseStatistics {
public:
    explicit BaseStatistics(PhysicalType type);
    virtual ~BaseStatistics();

    PhysicalType type_;
    std::unique_ptr<BaseStatistics> validityStats_;


    bool canHaveNull();
    bool canHaveNoNull();

    virtual bool isConstant() {
        return false;
    }

    static std::unique_ptr<BaseStatistics> createEmpty(PhysicalType type);

    virtual void merge(const BaseStatistics &other);
    virtual std::unique_ptr<BaseStatistics> copy();
    virtual void serialize(Serializer &serializer);
    static std::unique_ptr<BaseStatistics> deserialize(Deserializer &source, PhysicalType type);
    virtual void verify(Vector &vector, const SelectionVector &sel, idx_t count);
    void verify(Vector &vector, idx_t count);

    virtual string toString();
};


}
