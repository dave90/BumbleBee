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



template <class PHYSICAL_TYPE>
struct DecimalParquetValueConversion {

	static idx_t getByteLenght(ColumnReader &reader, bool& plain_decoder) {
		plain_decoder = false;
		switch (reader.schema().type) {
			case Type::INT32:
				plain_decoder = true;
				return sizeof(int32_t);
			case Type::INT64:
				plain_decoder = true;
				return sizeof(int64_t);
			default:
				auto len = (idx_t)reader.schema().type_length;
				if (!len)
					ErrorHandler::errorNotImplemented("Parquet Decimal type with byte lenght 0");
				return len;
		}
	}

	static PHYSICAL_TYPE dictRead(ByteBuffer &dict, uint32_t &offset, ColumnReader &reader) {
		auto dict_ptr = (PHYSICAL_TYPE *)dict.ptr_;
		return dict_ptr[offset];
	}

	static PHYSICAL_TYPE plainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		PHYSICAL_TYPE res = 0;
		bool plain_read;
		auto byte_len = getByteLenght(reader, plain_read); /* sure, type length needs to be a signed int */
		BB_ASSERT(byte_len <= sizeof(PHYSICAL_TYPE));
		plain_data.available(byte_len);
		if (plain_read) {
			// return without decoding
			PHYSICAL_TYPE* res = (PHYSICAL_TYPE*)plain_data.ptr_;
			plain_data.inc(byte_len);
			return *res;
		}
		auto res_ptr = (uint8_t *)&res;


		// numbers are stored as two's complement
		bool positive = (*plain_data.ptr_ & 0x80) == 0;

		for (idx_t i = 0; i < byte_len; i++) {
			auto byte = *(plain_data.ptr_ + (byte_len - i - 1));
			res_ptr[i] = positive ? byte : byte ^ 0xFF;
		}
		plain_data.inc(byte_len);
		if (!positive) {
			res += 1;
			return -res;
		}
		return res;
	}

	static void plainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		bool plain_read;
		auto len = getByteLenght(reader, plain_read);;
		plain_data.inc(len);
	}

	static PHYSICAL_TYPE null() {
		return NumericLimits<PHYSICAL_TYPE>::maximum();
	}
};

template <class PHYSICAL_TYPE>
class DecimalColumnReader
    : public TemplatedColumnReader<PHYSICAL_TYPE, DecimalParquetValueConversion<PHYSICAL_TYPE>> {

public:
	DecimalColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t file_idx_p,
	                    idx_t max_define_p, idx_t max_repeat_p)
	    : TemplatedColumnReader<PHYSICAL_TYPE, DecimalParquetValueConversion<PHYSICAL_TYPE>>(
	          reader, type_l, schema_p, file_idx_p, max_define_p, max_repeat_p) {};

protected:
	void dictionary(std::shared_ptr<ByteBuffer> dictionary_data, idx_t num_entries) {
		this->dict_ = std::make_shared<ResizeableBuffer>(this->reader_.allocator_, num_entries * sizeof(PHYSICAL_TYPE));
		auto dict_ptr = (PHYSICAL_TYPE *)this->dict_->ptr_;
		for (idx_t i = 0; i < num_entries; i++) {
			dict_ptr[i] = DecimalParquetValueConversion<PHYSICAL_TYPE>::plainRead(*dictionary_data, *this);
		}
	}
};



}
