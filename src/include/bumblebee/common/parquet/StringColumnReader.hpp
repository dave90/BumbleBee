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
#include "bumblebee/common/types/BumbleString.hpp"

namespace bumblebee{


struct StringParquetValueConversion {
    static string_t dictRead(ByteBuffer &dict, uint32_t &offset, ColumnReader &reader);

    static string_t plainRead(ByteBuffer &plain_data, ColumnReader &reader);

    static void plainSkip(ByteBuffer &plain_data, ColumnReader &reader);

    static string_t null();
};

class StringColumnReader : public TemplatedColumnReader<string_t, StringParquetValueConversion> {
public:
    StringColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t schema_idx_p,
                       idx_t max_define_p, idx_t max_repeat_p)
        : TemplatedColumnReader<string_t, StringParquetValueConversion>(reader,type_l, schema_p, schema_idx_p,
                                                                        max_define_p, max_repeat_p) {
        fixedWidthStringLength_ = 0;
        if (schema_p.type == Type::FIXED_LEN_BYTE_ARRAY) {
            BB_ASSERT(schema_p.__isset.type_length);
            fixedWidthStringLength_ = schema_p.type_length;
        }
    };

    std::unique_ptr<string_t[]> dictStrings_;
    idx_t fixedWidthStringLength_;

public:
    void dictionary(std::shared_ptr<ByteBuffer> dictionary_data, idx_t num_entries) override;

    uint32_t verifyString(const char *str_data, uint32_t str_len);

protected:
    void dictReference(Vector &result) override;
    void plainReference(std::shared_ptr<ByteBuffer> plain_data, Vector &result) override;
};

}
