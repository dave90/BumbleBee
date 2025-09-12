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
#include "bumblebee/catalog/Catalog.hpp"

namespace bumblebee{
std::unique_ptr<Catalog> Catalog::instance_ = nullptr;

Catalog::Catalog(): default_schema_(default_schema_name) {}

Schema & Catalog::getDefaultSchema() {
    return default_schema_;
}

void Catalog::dropCatalog() {
    // delete the catalog instance
    instance_ = nullptr;
}
}
