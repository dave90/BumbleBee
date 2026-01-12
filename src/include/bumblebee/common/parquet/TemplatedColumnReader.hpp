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
#include "bumblebee/common/Limits.hpp"

namespace bumblebee{



template <class VALUE_TYPE>
struct TemplatedParquetValueConversion {
	static VALUE_TYPE dictRead(ByteBuffer &dict, uint32_t &offset, ColumnReader &reader) {
		BB_ASSERT(offset < dict.len_ / sizeof(VALUE_TYPE));
		return ((VALUE_TYPE *)dict.ptr_)[offset];
	}

	static VALUE_TYPE plainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		return plain_data.read<VALUE_TYPE>();
	}

	static void plainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		plain_data.inc(sizeof(VALUE_TYPE));
	}

	static VALUE_TYPE null() {
		return NumericLimits<VALUE_TYPE>::maximum();
	}
};

template <class VALUE_TYPE, class VALUE_CONVERSION>
class TemplatedColumnReader : public ColumnReader {
public:
	TemplatedColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t schema_idx_p,
	                      idx_t max_define_p, idx_t max_repeat_p)
	    : ColumnReader(reader, type_l, schema_p, schema_idx_p, max_define_p, max_repeat_p) {};

	std::shared_ptr<ByteBuffer> dict_;

	void dictionary(std::shared_ptr<ByteBuffer> data, idx_t num_entries) override {
		dict_ = std::move(data);
	}

	void offsets(uint32_t *offsets, uint8_t *defines, uint64_t num_values, parquet_filter_t &filter,
	             idx_t result_offset, Vector &result) override {
		auto result_ptr = FlatVector::getData<VALUE_TYPE>(result);

		idx_t offset_idx = 0;
		for (idx_t row_idx = 0; row_idx < num_values; row_idx++) {
			if (hasDefines() && defines[row_idx + result_offset] != maxDefine_) {
				// invalid data
				VALUE_TYPE val = VALUE_CONVERSION::null();
				result_ptr[row_idx + result_offset] = val;
				continue;
			}
			if (filter[row_idx + result_offset]) {
				VALUE_TYPE val = VALUE_CONVERSION::dictRead(*dict_, offsets[offset_idx++], *this);
				result_ptr[row_idx + result_offset] = val;
			} else {
				offset_idx++;
			}
		}
	}

	void plain(std::shared_ptr<ByteBuffer> plain_data, uint8_t *defines, uint64_t num_values, parquet_filter_t &filter,
	           idx_t result_offset, Vector &result) override {
		auto result_ptr = FlatVector::getData<VALUE_TYPE>(result);
		for (idx_t row_idx = 0; row_idx < num_values; row_idx++) {
			if (hasDefines() && defines[row_idx + result_offset] != maxDefine_) {
				// invalid data
				VALUE_TYPE val = VALUE_CONVERSION::null();
				result_ptr[row_idx + result_offset] = val;
				continue;
			}
			if (filter[row_idx + result_offset]) {
				VALUE_TYPE val = VALUE_CONVERSION::plainRead(*plain_data, *this);
				result_ptr[row_idx + result_offset] = val;
			} else { // there is still some data there that we have to skip over
				VALUE_CONVERSION::plainSkip(*plain_data, *this);
			}
		}
	}
};



}
