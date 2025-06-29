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
#include "bumblebee/catalog/Schema.h"

namespace bumblebee{
Schema::Schema(const std::string &name): name_(name) {}

Predicate * Schema::createPredicate(char *predicateName, unsigned arity) {
    PredicateMapEntry entry{.name_ = predicateName, .arity_ = arity};
    auto it = ptables_.find(entry);
    if (it != ptables_.end()) {
        return it->second.get()->predicate_.get();
    }
    // create new predicate table
    // auto pt = make_predicate_tables_uptr(static_cast<const char*>(predicateName), arity);
    auto pt = PredicateTablesUPtr(new PredicateTables(predicateName, arity));
    Predicate *p = pt.get()->predicate_.get();
    entry.name_ = p->getName();
    ptables_.emplace(entry, std::move(pt));
    return p;
}

}
