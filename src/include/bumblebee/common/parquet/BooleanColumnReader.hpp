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
#include "TemplatedColumnReader.hpp"

namespace bumblebee{



struct BooleanParquetValueConversion;

class BooleanColumnReader : public TemplatedColumnReader<bool, BooleanParquetValueConversion> {
public:
    BooleanColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t schema_idx_p,
                        idx_t max_define_p, idx_t max_repeat_p)
        : TemplatedColumnReader<bool, BooleanParquetValueConversion>(reader, type_l, schema_p, schema_idx_p,
                                                                     max_define_p, max_repeat_p),
          bytePos_(0) {};

    uint8_t bytePos_;

    void initializeRead(const std::vector<ColumnChunk> &columns, TProtocol &protocol_p) override {
        bytePos_ = 0;
        TemplatedColumnReader<bool, BooleanParquetValueConversion>::initializeRead(columns, protocol_p);
    }
};

struct BooleanParquetValueConversion {
    static uint8_t dictRead(ByteBuffer &dict, uint32_t &offset, ColumnReader &reader) {
        ErrorHandler::errorNotImplemented("Dicts for booleans make no sense");
        return 0;
    }

    static uint8_t plainRead(ByteBuffer &plain_data, ColumnReader &reader) {
        plain_data.available(1);
        auto &byte_pos = ((BooleanColumnReader &)reader).bytePos_;
        bool ret = (*plain_data.ptr_ >> byte_pos) & 1;
        byte_pos++;
        if (byte_pos == 8) {
            byte_pos = 0;
            plain_data.inc(1);
        }
        return (ret)? 1 : 0;
    }

    static void plainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
        plainRead(plain_data, reader);
    }

    static uint8_t null() {
        return NumericLimits<uint8_t>::maximum();
    }
};


}
