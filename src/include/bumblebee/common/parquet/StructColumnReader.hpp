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
#include "ColumnReader.hpp"

namespace bumblebee{


class StructColumnReader : public ColumnReader {
public:
    StructColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t schema_idx_p,
                       idx_t max_define_p, idx_t max_repeat_p, vector<std::unique_ptr<ColumnReader>> child_readers_p, child_list_t<LogicalType> child_types)
        : ColumnReader(reader, type_l, schema_p, schema_idx_p, max_define_p, max_repeat_p),
          childReaders_(std::move(child_readers_p)), childTypes_(std::move(child_types)) {
        BB_ASSERT(!childReaders_.empty());
    };

    ColumnReader* getChildReader(idx_t child_idx) {
        return childReaders_[child_idx].get();
    }

    void initializeRead(const std::vector<ColumnChunk> &columns, TProtocol &protocol_p) override {
        for (auto &child : childReaders_) {
            child->initializeRead(columns, protocol_p);
        }
    }

    idx_t groupRowsAvailable() override {
        return childReaders_[0]->groupRowsAvailable();
    }


    vector<std::unique_ptr<ColumnReader>> childReaders_;
    child_list_t<LogicalType> childTypes_;
};


}
