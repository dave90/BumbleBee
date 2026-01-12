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

#include <bitset>

#include "parquet/parquet_types.h"
#include "ThriftTools.hpp"

#include "ParquetStatistics.hpp"
#include "ResizeableBuffer.hpp"
#include "RleBpDecoder.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/Assert.hpp"
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee {

class ParquetReader;

using bumblebee::thrift::protocol::TProtocol;

using bumblebee::format::ColumnChunk;
using bumblebee::format::FieldRepetitionType;
using bumblebee::format::PageHeader;
using bumblebee::format::SchemaElement;
using bumblebee::format::Type;

typedef std::bitset<STANDARD_VECTOR_SIZE> parquet_filter_t;

class ColumnReader {
public:

	ColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t file_idx_p,
	             idx_t max_define_p, idx_t max_repeat_p);

	virtual void initializeRead(const std::vector<ColumnChunk> &columns, TProtocol &protocol_p) {
		BB_ASSERT(fileIdx_ < columns.size());
		chunk_ = &columns[fileIdx_];
		protocol_ = &protocol_p;
		BB_ASSERT(chunk_);
		BB_ASSERT(chunk_->__isset.meta_data);

		if (chunk_->__isset.file_path) {
			ErrorHandler::errorNotImplemented("Only inlined data files are supported (no references)");
		}

		// ugh. sometimes there is an extra offset for the dict. sometimes it's wrong.
		chunkReadOffset_ = chunk_->meta_data.data_page_offset;
		if (chunk_->meta_data.__isset.dictionary_page_offset && chunk_->meta_data.dictionary_page_offset >= 4) {
			// this assumes the data pages follow the dict pages directly.
			chunkReadOffset_ = chunk_->meta_data.dictionary_page_offset;
		}
		groupRowsAvailable_ = chunk_->meta_data.num_values;
	}
	virtual ~ColumnReader();

	virtual idx_t read(uint64_t num_values, parquet_filter_t &filter, uint8_t *define_out, uint8_t *repeat_out,
	                   Vector &result_out);

	virtual void skip(idx_t num_values);

	const LogicalType &logicalType() {
		return logicalType_;
	}

	const SchemaElement &schema() {
		return schema_;
	}

	virtual idx_t groupRowsAvailable() {
		return groupRowsAvailable_;
	}

	std::unique_ptr<BaseStatistics> stats(const std::vector<ColumnChunk> &columns) {
		return parquetTransformColumnStatistics(schema(), logicalType_.getPhysicalType(), columns[fileIdx_]);
	}

protected:
	// readers that use the default Read() need to implement those
	virtual void plain(std::shared_ptr<ByteBuffer> plain_data, uint8_t *defines, idx_t num_values, parquet_filter_t &filter,
	                   idx_t result_offset, Vector &result) {
		ErrorHandler::errorNotImplemented("Parquet reader error: Plain");
	}

	virtual void dictionary(std::shared_ptr<ByteBuffer> dictionary_data, idx_t num_entries) {
		ErrorHandler::errorNotImplemented("Parquet reader error: Dictionary");

	}

	virtual void offsets(uint32_t *offsets, uint8_t *defines, idx_t num_values, parquet_filter_t &filter,
	                     idx_t result_offset, Vector &result) {
		ErrorHandler::errorNotImplemented("Parquet reader error: Offsets");
	}

	// these are nops for most types, but not for strings
	virtual void dictReference(Vector &result) {
	}
	virtual void plainReference(std::shared_ptr<ByteBuffer>, Vector &result) {
	}

	bool hasDefines() {
		return maxDefine_ > 0;
	}

	bool hasRepeats() {
		return maxRepeat_ > 0;
	}

protected:
	const SchemaElement &schema_;

	idx_t fileIdx_;
	idx_t maxDefine_;
	idx_t maxRepeat_;

	ParquetReader &reader_;
	LogicalType logicalType_;

private:
	void prepareRead(parquet_filter_t &filter);
	void preparePage(idx_t compressed_page_size, idx_t uncompressed_page_size);
	void prepareDataPage(PageHeader &page_hdr);

	const bumblebee::format::ColumnChunk *chunk_;

	bumblebee::thrift::protocol::TProtocol *protocol_;
	idx_t pageRowsAvailable_;
	idx_t groupRowsAvailable_;
	idx_t chunkReadOffset_;

	std::shared_ptr<ResizeableBuffer> block_;

	ResizeableBuffer offsetBuffer_;

	std::unique_ptr<RleBpDecoder> dictDecoder_;
	std::unique_ptr<RleBpDecoder> definedDecoder_;
	std::unique_ptr<RleBpDecoder> repeatedDecoder_;

	// dummies for Skip()
	parquet_filter_t noneFilter_;
	ResizeableBuffer dummyDefine_;
	ResizeableBuffer dummyRepeat_;

public:
	static std::unique_ptr<ColumnReader> createReader(ParquetReader &reader, const LogicalType &type_l,
											 const SchemaElement &schema_p, idx_t schema_idx_p, idx_t max_define,
											 idx_t max_repeat);
};

} // namespace duckdb
