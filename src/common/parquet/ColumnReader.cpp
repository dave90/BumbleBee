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

#include "bumblebee/common/parquet/ColumnReader.hpp"

#include <filesystem>
#include <sstream>


#include "bumblebee/common/parquet/BooleanColumnReader.hpp"
#include "bumblebee/common/parquet/CallbackColumnReader.hpp"
#include "bumblebee/common/parquet/DecimalColumnReader.hpp"
#include "bumblebee/common/parquet/ParquetTimestamp.hpp"
#include "bumblebee/common/parquet/ParquetReader.hpp"
#include "bumblebee/common/parquet/RleBpDecoder.hpp"
#include "bumblebee/common/parquet/StringColumnReader.hpp"
#include "bumblebee/common/parquet/TemplatedColumnReader.hpp"
#include "parquet/parquet_types.h"
#include "snappy/snappy.h"
#include "zstd/include/zstd.h"
#include "miniz/miniz_wrapper.hpp"

namespace bumblebee {


using format::CompressionCodec;
using format::ConvertedType;
using format::Encoding;
using format::PageType;
using format::Type;

const uint32_t RleBpDecoder::BITPACK_MASKS[] = {
    0,       1,       3,        7,        15,       31,        63,        127,       255,        511,       1023,
    2047,    4095,    8191,     16383,    32767,    65535,     131071,    262143,    524287,     1048575,   2097151,
    4194303, 8388607, 16777215, 33554431, 67108863, 134217727, 268435455, 536870911, 1073741823, 2147483647};

const uint8_t RleBpDecoder::BITPACK_DLEN = 8;

ColumnReader::ColumnReader(ParquetReader &reader, LogicalType type_l, const SchemaElement &schema_p, idx_t file_idx_p,
                           idx_t max_define_p, idx_t max_repeat_p)
    : schema_(schema_p), fileIdx_(file_idx_p), maxDefine_(max_define_p), maxRepeat_(max_repeat_p), reader_(reader),
      logicalType_(type_l), pageRowsAvailable_(0) {

	// dummies for Skip()
	noneFilter_.none();
	dummyDefine_.resize(reader_.allocator_, STANDARD_VECTOR_SIZE);
	dummyRepeat_.resize(reader_.allocator_, STANDARD_VECTOR_SIZE);
}

ColumnReader::~ColumnReader() {
}

std::unique_ptr<ColumnReader> ColumnReader::createReader(ParquetReader &reader,const LogicalType &type_l,
                                                         const SchemaElement &schema_p, idx_t file_idx_p, idx_t max_define,
                                                         idx_t max_repeat) {
	switch (type_l.type()) {
		case LogicalTypeId::BOOLEAN:
			return std::make_unique<BooleanColumnReader>(reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::DECIMAL:
			switch (type_l.getPhysicalType()) {
				case PhysicalType::SMALLINT:
					return std::make_unique<DecimalColumnReader<int16_t>>(reader, type_l, schema_p, file_idx_p, max_define,
					                                                      max_repeat);
				case PhysicalType::INTEGER:
					return std::make_unique<DecimalColumnReader<int32_t>>(reader, type_l, schema_p, file_idx_p, max_define,
																		  max_repeat);
				case PhysicalType::BIGINT:
					return std::make_unique<DecimalColumnReader<int64_t>>(reader, type_l, schema_p, file_idx_p, max_define,
																		  max_repeat);
				default:
					ErrorHandler::errorNotImplemented("Error DECIMAL logical type does not have proper physical type.");
			}
		case LogicalTypeId::DATE:
			return std::make_unique<CallbackColumnReader<int32_t, date_t, parquetIntToDate>>(reader, type_l, schema_p,
																					file_idx_p, max_define, max_repeat);
		case LogicalTypeId::TIMESTAMP:
			switch (schema_p.type) {
				case Type::INT96:
						return std::make_unique<CallbackColumnReader<Int96, timestamp_t, impalaTimestampToTimestamp>>(
							reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
				case Type::INT64:
						switch (schema_p.converted_type) {
					case ConvertedType::TIMESTAMP_MICROS:
								return std::make_unique<CallbackColumnReader<int64_t, timestamp_t, parquetTimestampMicrosToTimestamp>>(
									reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
					case ConvertedType::TIMESTAMP_MILLIS:
								return std::make_unique<CallbackColumnReader<int64_t, timestamp_t, parquetTimestampMsToTimestamp>>(
									reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
					default:
								break;
						}
				default:
						break;
			}
			break;
		case LogicalTypeId::UTINYINT:
			return std::make_unique<TemplatedColumnReader<uint8_t, TemplatedParquetValueConversion<uint32_t>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::USMALLINT:
			return std::make_unique<TemplatedColumnReader<uint16_t, TemplatedParquetValueConversion<uint32_t>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::UINTEGER:
			return std::make_unique<TemplatedColumnReader<uint32_t, TemplatedParquetValueConversion<uint32_t>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::HASH:
		case LogicalTypeId::ADDRESS:
		case LogicalTypeId::UBIGINT:
			return std::make_unique<TemplatedColumnReader<uint64_t, TemplatedParquetValueConversion<uint64_t>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::TINYINT:
			return std::make_unique<TemplatedColumnReader<int8_t, TemplatedParquetValueConversion<int32_t>>>(
				reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::SMALLINT:
			return std::make_unique<TemplatedColumnReader<int16_t, TemplatedParquetValueConversion<int32_t>>>(
				reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::INTEGER:
			return std::make_unique<TemplatedColumnReader<int32_t, TemplatedParquetValueConversion<int32_t>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::BIGINT:
			return std::make_unique<TemplatedColumnReader<int64_t, TemplatedParquetValueConversion<int64_t>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::FLOAT:
			return std::make_unique<TemplatedColumnReader<float, TemplatedParquetValueConversion<float>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::DOUBLE:
			return std::make_unique<TemplatedColumnReader<double, TemplatedParquetValueConversion<double>>>(
			    reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::STRING:
			return std::make_unique<StringColumnReader>(reader, type_l, schema_p, file_idx_p, max_define, max_repeat);
		case LogicalTypeId::STRUCT:
			ErrorHandler::errorNotImplemented("Error STRUCT logical type does not have proper physical type.");
		default:
			;
	}
	ErrorHandler::errorNotImplemented("Error logical types does not have proper physical type.");
	return nullptr;
}

void ColumnReader::prepareRead(parquet_filter_t &filter) {
	dictDecoder_.reset();
	definedDecoder_.reset();
	block_.reset();

	PageHeader page_hdr;
	page_hdr.read(protocol_);

	preparePage(page_hdr.compressed_page_size, page_hdr.uncompressed_page_size);

	switch (page_hdr.type) {
		case PageType::DATA_PAGE_V2:
		case PageType::DATA_PAGE:
			prepareDataPage(page_hdr);
			break;
		case PageType::DICTIONARY_PAGE:
			dictionary(std::move(block_), page_hdr.dictionary_page_header.num_values);
			break;
		default:
			break; // ignore INDEX page type and any other custom extensions
	}
}

void ColumnReader::preparePage(idx_t compressed_page_size, idx_t uncompressed_page_size) {
	auto &trans = (ThriftFileTransport &)*protocol_->getTransport();

	block_ = std::make_shared<ResizeableBuffer>(reader_.allocator_, compressed_page_size + 1);
	trans.read((uint8_t *)block_->ptr_, compressed_page_size);

	std::shared_ptr<ResizeableBuffer> unpacked_block;
	if (chunk_->meta_data.codec != CompressionCodec::UNCOMPRESSED) {
		unpacked_block = std::make_shared<ResizeableBuffer>(reader_.allocator_, uncompressed_page_size + 1);
	}

	switch (chunk_->meta_data.codec) {
	case CompressionCodec::UNCOMPRESSED:
		break;
	case CompressionCodec::GZIP: {
		MiniZStream s;

		s.decompress((const char *) block_->ptr_, compressed_page_size, (char *)unpacked_block->ptr_,
		             uncompressed_page_size);
		block_ = std::move(unpacked_block);

		break;
	}
	case CompressionCodec::SNAPPY: {
		auto res = snappy::RawUncompress((const char *)block_->ptr_, compressed_page_size, (char *)unpacked_block->ptr_);
		if (!res) {
			ErrorHandler::errorGeneric("Decompression failure");
		}
		block_ = std::move(unpacked_block);
		break;
	}
	case CompressionCodec::ZSTD: {
		auto res = ZSTD_decompress((char *)unpacked_block->ptr_, uncompressed_page_size,
		                                        (const char *)block_->ptr_, compressed_page_size);
		if (ZSTD_isError(res) || res != (size_t)uncompressed_page_size) {
			ErrorHandler::errorGeneric("ZSTD Decompression failure");
		}
		block_ = std::move(unpacked_block);
		break;
	}

	default: {
		std::stringstream codec_name;
		codec_name << chunk_->meta_data.codec;
		ErrorHandler::errorNotImplemented("Unsupported compression codec \"" + codec_name.str() +
								 "\". Supported options are uncompressed, gzip or snappy");
		break;
	}
	}
}

static uint8_t computeBitWidth(idx_t val) {
	if (val == 0) {
		return 0;
	}
	uint8_t ret = 1;
	while (((idx_t)(1 << ret) - 1) < val) {
		ret++;
	}
	return ret;
}

void ColumnReader::prepareDataPage(PageHeader &page_hdr) {
	if (page_hdr.type == PageType::DATA_PAGE && !page_hdr.__isset.data_page_header) {
		ErrorHandler::errorGeneric("Missing data page header from data page");
	}
	if (page_hdr.type == PageType::DATA_PAGE_V2 && !page_hdr.__isset.data_page_header_v2) {
		ErrorHandler::errorGeneric("Missing data page header from data page v2");
	}

	pageRowsAvailable_ = page_hdr.type == PageType::DATA_PAGE ? page_hdr.data_page_header.num_values
	                                                           : page_hdr.data_page_header_v2.num_values;
	auto page_encoding = page_hdr.type == PageType::DATA_PAGE ? page_hdr.data_page_header.encoding
	                                                          : page_hdr.data_page_header_v2.encoding;

	if (hasRepeats()) {
		uint32_t rep_length = page_hdr.type == PageType::DATA_PAGE
		                          ? block_->read<uint32_t>()
		                          : page_hdr.data_page_header_v2.repetition_levels_byte_length;
		block_->available(rep_length);
		repeatedDecoder_ =
		    std::make_unique<RleBpDecoder>((const uint8_t *)block_->ptr_, rep_length, computeBitWidth(maxRepeat_));
		block_->inc(rep_length);
	}

	if (hasDefines()) {
		uint32_t def_length = page_hdr.type == PageType::DATA_PAGE
		                          ? block_->read<uint32_t>()
		                          : page_hdr.data_page_header_v2.definition_levels_byte_length;
		block_->available(def_length);
		definedDecoder_ =
		    std::make_unique<RleBpDecoder>((const uint8_t *)block_->ptr_, def_length, computeBitWidth(maxDefine_));
		block_->inc(def_length);
	}

	switch (page_encoding) {
		case Encoding::RLE_DICTIONARY:
		case Encoding::PLAIN_DICTIONARY: {
			// TODO there seems to be some confusion whether this is in the bytes for v2
			// where is it otherwise??
			auto dict_width = block_->read<uint8_t>();
			// TODO somehow dict_width can be 0 ?
			dictDecoder_ = std::make_unique<RleBpDecoder>((const uint8_t *)block_->ptr_, block_->len_, dict_width);
			block_->inc(block_->len_);
			break;
		}
		case Encoding::PLAIN:
			// nothing to do here, will be read directly below
			break;

		default:
			ErrorHandler::errorNotImplemented("Unsupported page type");
	}
}

idx_t ColumnReader::read(uint64_t num_values, parquet_filter_t &filter, uint8_t *define_out, uint8_t *repeat_out,
                         Vector &result) {
	// we need to reset the location because multiple column readers share the same protocol
	auto &trans = (ThriftFileTransport &)*protocol_->getTransport();
	trans.setLocation(chunkReadOffset_);

	idx_t result_offset = 0;
	auto to_read = num_values;

	while (to_read > 0) {
		while (pageRowsAvailable_ == 0) {
			prepareRead(filter);
		}

		BB_ASSERT(block_);
		auto read_now = minValue<idx_t>(to_read, pageRowsAvailable_);

		BB_ASSERT(read_now <= STANDARD_VECTOR_SIZE);

		if (hasRepeats()) {
			BB_ASSERT(repeatedDecoder_);
			repeatedDecoder_->getBatch<uint8_t>((char *)repeat_out + result_offset, read_now);
		}

		if (hasDefines()) {
			BB_ASSERT(definedDecoder_);
			definedDecoder_->getBatch<uint8_t>((char *)define_out + result_offset, read_now);
		}

		if (dictDecoder_) {
			// we need the null count because the offsets and plain values have no entries for nulls
			idx_t null_count = 0;
			if (hasDefines()) {
				for (idx_t i = 0; i < read_now; i++) {
					if (define_out[i + result_offset] != maxDefine_) {
						null_count++;
					}
				}
			}

			offsetBuffer_.resize(reader_.allocator_, sizeof(uint32_t) * (read_now - null_count));
			dictDecoder_->getBatch<uint32_t>(offsetBuffer_.ptr_, read_now - null_count);
			dictReference(result);
			offsets((uint32_t *)offsetBuffer_.ptr_, define_out, read_now, filter, result_offset, result);
		} else {
			plainReference(block_, result);
			plain(block_, define_out, read_now, filter, result_offset, result);
		}

		result_offset += read_now;
		pageRowsAvailable_ -= read_now;
		to_read -= read_now;
	}
	groupRowsAvailable_ -= num_values;
	chunkReadOffset_ = trans.getLocation();

	return num_values;
}

void ColumnReader::skip(idx_t num_values) {
	dummyDefine_.zero();
	dummyRepeat_.zero();

	// TODO this can be optimized, for example we dont actually have to bitunpack offsets
	Vector dummy_result(logicalType_.getPhysicalType(), nullptr);
	auto values_read =
	    read(num_values, noneFilter_, (uint8_t *)dummyDefine_.ptr_, (uint8_t *)dummyRepeat_.ptr_, dummy_result);
	if (values_read != num_values) {
		ErrorHandler::errorGeneric("Row count mismatch when skipping rows");
	}
}


}
