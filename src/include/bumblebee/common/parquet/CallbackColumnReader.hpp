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
#include "TemplatedColumnReader.hpp"

namespace bumblebee{

template <class PARQUET_PHYSICAL_TYPE, class PHYSICAL_TYPE,
          PHYSICAL_TYPE (*FUNC)(const PARQUET_PHYSICAL_TYPE &input)>
struct CallbackParquetValueConversion {
    static PHYSICAL_TYPE dictRead(ByteBuffer &dict, uint32_t &offset, ColumnReader &reader) {
        return TemplatedParquetValueConversion<PHYSICAL_TYPE>::dictRead(dict, offset, reader);
    }

    static PHYSICAL_TYPE plainRead(ByteBuffer &plain_data, ColumnReader &reader) {
        return FUNC(plain_data.read<PARQUET_PHYSICAL_TYPE>());
    }

    static void plainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
        plain_data.inc(sizeof(PARQUET_PHYSICAL_TYPE));
    }

    static PHYSICAL_TYPE null() {
        return NumericLimits<PHYSICAL_TYPE>::maximum();
    }
};


template <class PARQUET_PHYSICAL_TYPE, class PHYSICAL_TYPE,
          PHYSICAL_TYPE (*FUNC)(const PARQUET_PHYSICAL_TYPE &input)>
class CallbackColumnReader
    : public TemplatedColumnReader<PHYSICAL_TYPE,
        CallbackParquetValueConversion<PARQUET_PHYSICAL_TYPE, PHYSICAL_TYPE, FUNC>> {

public:
    CallbackColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t file_idx_p,
                         idx_t max_define_p, idx_t max_repeat_p)
        : TemplatedColumnReader<PHYSICAL_TYPE,
                                CallbackParquetValueConversion<PARQUET_PHYSICAL_TYPE, PHYSICAL_TYPE, FUNC>>(
              reader, type_l, schema_p, file_idx_p, max_define_p, max_repeat_p) {
    }

protected:
    void dictionary(std::shared_ptr<ByteBuffer> dictionary_data, idx_t num_entries) {
        this->dict_ = std::make_shared<ResizeableBuffer>(this->reader_.allocator_, num_entries * sizeof(PHYSICAL_TYPE));
        auto dict_ptr = (PHYSICAL_TYPE *)this->dict_->ptr_;
        for (idx_t i = 0; i < num_entries; i++) {
            dict_ptr[i] = FUNC(dictionary_data->read<PARQUET_PHYSICAL_TYPE>());
        }
    }
};



}
