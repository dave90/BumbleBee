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

#include "bumblebee/parser/statement/Rule.hpp"

namespace bumblebee {

class ClientContext;

// Program-level rewrite that folds a plain COUNT(*) over an unfiltered
// &read_parquet scan into a constant fact, reading the row count from the
// Parquet file metadata instead of scanning the data.
//
// It is intentionally very conservative: it only fires when the row count is
// provably COUNT(*) (the aggregated term is a constant in the scan rule head)
// and there is no filter (single-atom bodies). Any query that does not match
// the exact pattern is left untouched, so it cannot regress other queries.
class MetadataAggRewriter {
public:
    explicit MetadataAggRewriter(ClientContext& context) : context_(context) {}

    void rewrite(rules_vector_t& program);

private:
    ClientContext& context_;

    // Resolve the parquet path(s) referenced by a &read_parquet atom and sum
    // their row counts from metadata. Returns false (and leaves total unset)
    // if the path cannot be resolved or any reader fails.
    bool countRowsFromMetadata(Atom& readParquetAtom, idx_t& total);
};

} // namespace bumblebee
