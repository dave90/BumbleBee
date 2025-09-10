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

#include "Schema.hpp"
#include "bumblebee/parser/statement/Predicate.hpp"

namespace bumblebee{

using schema_map = std::unordered_map<std::string, PredicateMapEntry>;
constexpr const char* default_schema_name = "default";

class Catalog {
public:
    // Delete copy constructor and copy assignment
    Catalog(const Catalog&) = delete;
    Catalog& operator=(const Catalog&) = delete;

    // Access the singleton instance
    static Catalog& instance() {
        if (!instance_)
            instance_ = std::unique_ptr<Catalog>(new Catalog());
        return *instance_;
    }

    Schema& getDefaultSchema();

    static void dropCatalog();

    ~Catalog() = default;

private:

    static std::unique_ptr<Catalog> instance_;

    // Private constructor to prevent external instantiation
    Catalog();

    schema_map schemas_;
    Schema default_schema_;

};


}
