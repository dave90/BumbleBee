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

#include "bumblebee/common/parquet/ParquetReader.hpp"

#include "bumblebee/ClientContext.hpp"
#include "thrift/protocol/TCompactProtocol.h"
#include "bumblebee/common/parquet/StructColumnReader.hpp"
#include "bumblebee/common/types/Decimal.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"

namespace bumblebee {


static std::unique_ptr<thrift::protocol::TProtocol> createThriftProtocol(Allocator &allocator,
                                                                                   FileHandle &file_handle) {
	auto transport = std::make_shared<ThriftFileTransport>(allocator, file_handle);
	return make_unique<thrift::protocol::TCompactProtocolT<ThriftFileTransport>>(std::move(transport));
}

static std::shared_ptr<ParquetFileMetadataCache> loadMetadata(Allocator &allocator, FileHandle &file_handle) {
	auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	auto proto = createThriftProtocol(allocator, file_handle);
	auto &transport = ((ThriftFileTransport &)*proto->getTransport());
	auto file_size = transport.getSize();
	if (file_size < 12) {
		ErrorHandler::errorGeneric(StringUtils::format("File '%s' too small to be a Parquet file", file_handle.path_));
	}

	ResizeableBuffer buf;
	buf.resize(allocator, 8);
	buf.zero();

	transport.setLocation(file_size - 8);
	transport.read((uint8_t *)buf.ptr_, 8);

	if (strncmp(buf.ptr_ + 4, "PAR1", 4) != 0) {
		ErrorHandler::errorGeneric(StringUtils::format("No magic bytes found at end of file '%s'", file_handle.path_));
	}
	// read four-byte footer length from just before the end magic bytes
	auto footer_len = *(uint32_t *)buf.ptr_;
	if (footer_len <= 0 || file_size < 12 + footer_len) {
		ErrorHandler::errorGeneric(StringUtils::format("Footer length error in file '%s'", file_handle.path_));
	}
	auto metadata_pos = file_size - (footer_len + 8);
	transport.setLocation(metadata_pos);
	transport.prefetch(metadata_pos, footer_len);

	auto metadata = std::make_unique<format::FileMetaData>();
	metadata->read(proto.get());
	return make_shared<ParquetFileMetadataCache>(std::move(metadata), current_time);
}

LogicalType ParquetReader::deriveLogicalType(const SchemaElement &s_ele) {
	// inner node
	BB_ASSERT(s_ele.__isset.type && s_ele.num_children == 0);
	switch (s_ele.type) {
		case Type::BOOLEAN:
			return LogicalTypeId::BOOLEAN;
		case Type::INT32:
			if (s_ele.__isset.converted_type) {
				switch (s_ele.converted_type) {
					case format::ConvertedType::DATE:
						return LogicalType::createDate();
					case format::ConvertedType::DECIMAL:
						return LogicalType::createDecimal(9, s_ele.scale);
					case format::ConvertedType::UINT_8:
						return LogicalTypeId::UTINYINT;
					case format::ConvertedType::UINT_16:
						return LogicalTypeId::USMALLINT;
					default:
						return LogicalTypeId::INTEGER;
				}
			}
			return LogicalTypeId::INTEGER;
		case Type::INT64:
			if (s_ele.__isset.converted_type) {
				switch (s_ele.converted_type) {
					case format::ConvertedType::DECIMAL:
						return LogicalType::createDecimal(18, s_ele.scale);
					case format::ConvertedType::TIMESTAMP_MICROS:
					case format::ConvertedType::TIMESTAMP_MILLIS:
						return LogicalType::createTimestamp();
					case format::ConvertedType::UINT_32:
						return LogicalTypeId::UINTEGER;
					case format::ConvertedType::UINT_64:
						return LogicalTypeId::UBIGINT;
					default:
						return LogicalTypeId::BIGINT;
				}
			}
			return LogicalTypeId::BIGINT;
		case Type::INT96:
			// it is a timestamp
			return LogicalType::createTimestamp();
		case Type::FLOAT:
			return LogicalTypeId::FLOAT;
		case Type::DOUBLE:
			return LogicalTypeId::DOUBLE;
		case Type::BYTE_ARRAY:
		case Type::FIXED_LEN_BYTE_ARRAY:
			if (s_ele.type == Type::FIXED_LEN_BYTE_ARRAY && !s_ele.__isset.type_length) {
				formatException("ARRAY parquet type is not supported.");
				return LogicalTypeId::UNKNOWN;
			}
			if (s_ele.__isset.converted_type) {
				switch (s_ele.converted_type) {
				case format::ConvertedType::DECIMAL: {
					if (s_ele.type == Type::FIXED_LEN_BYTE_ARRAY && s_ele.__isset.scale && s_ele.__isset.type_length) {
						return LogicalType::createDecimal(s_ele.precision, s_ele.scale);
					}
					else {
						formatException("DECIMAL parquet type is not supported without scale and precision.");
						return LogicalTypeId::UNKNOWN;
					}
				}
				case format::ConvertedType::UTF8:
					return LogicalTypeId::STRING;
				default:
					return LogicalTypeId::STRING;
				}
			}
			if (parquetOptions_.binaryAsString_) {
				return LogicalTypeId::STRING;
			}
			return LogicalTypeId::STRING;
		default:
			return LogicalTypeId::UNKNOWN;
	}
}


std::unique_ptr<ColumnReader> ParquetReader::createReaderRecursive(const format::FileMetaData *file_meta_data, idx_t depth,
                                                              idx_t max_define, idx_t max_repeat,
                                                              idx_t &next_schema_idx, idx_t &next_file_idx) {
	BB_ASSERT(file_meta_data);
	BB_ASSERT(next_schema_idx < file_meta_data->schema.size());
	auto &s_ele = file_meta_data->schema[next_schema_idx];
	auto this_idx = next_schema_idx;

	if (s_ele.__isset.repetition_type) {
		if (s_ele.repetition_type != FieldRepetitionType::REQUIRED) {
			max_define++;
		}
		if (s_ele.repetition_type == FieldRepetitionType::REPEATED) {
			max_repeat++;
		}
	}

	if (!s_ele.__isset.type) { // inner node
		if (depth > 0) {
			// we support only struct as root node, not in inner nodes
			formatException("Struct parquet type not supported!");
		}
		if (s_ele.num_children == 0) {
			throw std::runtime_error("Node has no children but should");
		}
		child_list_t<LogicalType> child_types;
		vector<std::unique_ptr<ColumnReader>> child_readers;

		idx_t c_idx = 0;
		while (c_idx < (idx_t)s_ele.num_children) {
			next_schema_idx++;

			auto &child_ele = file_meta_data->schema[next_schema_idx];

			auto child_reader = createReaderRecursive(file_meta_data, depth + 1, max_define, max_repeat,
			                                          next_schema_idx, next_file_idx);
			child_types.push_back(make_pair(child_ele.name, child_reader->logicalType()));
			child_readers.push_back(std::move(child_reader));

			c_idx++;
		}
		BB_ASSERT(!child_types.empty());
		std::unique_ptr<ColumnReader> result;
		LogicalType result_type_l{LogicalTypeId::UNKNOWN};
		// if we only have a single child no reason to create a struct
		if (child_types.size() > 1 || depth == 0) {
			result_type_l = {LogicalTypeId::STRUCT};
			result = make_unique<StructColumnReader>(*this, result_type_l, s_ele, this_idx, max_define, max_repeat,
			                                         std::move(child_readers), std::move(child_types));
		} else {
			// if we have a struct with only a single type, pull up
			result_type_l = child_types[0].second;
			result = std::move(child_readers[0]);
		}
		if (s_ele.repetition_type == FieldRepetitionType::REPEATED) {
			formatException("List parquet type not supported!");
			return nullptr;
		}
		return result;
	} else { // leaf node
		return ColumnReader::createReader(*this, deriveLogicalType(s_ele), s_ele, next_file_idx++, max_define,
		                                  max_repeat);
	}
}

std::unique_ptr<ColumnReader> ParquetReader::createReader(const format::FileMetaData *file_meta_data) {
	idx_t next_schema_idx = 0;
	idx_t next_file_idx = 0;

	auto ret = createReaderRecursive(file_meta_data, 0, 0, 0, next_schema_idx, next_file_idx);
	BB_ASSERT(next_schema_idx == file_meta_data->schema.size() - 1);
	BB_ASSERT(file_meta_data->row_groups.empty() || next_file_idx == file_meta_data->row_groups[0].columns.size());
	return ret;
}

void ParquetReader::initializeSchema(const vector<PhysicalType> &expected_types_p, const string &initial_filename_p) {
	auto file_meta_data = getFileMetadata();

	if (file_meta_data->__isset.encryption_algorithm) {
		formatException("Encrypted Parquet files are not supported");
	}
	// check if we like this schema
	if (file_meta_data->schema.size() < 2) {
		formatException("Need at least one non-root column in the file");
	}

	bool has_expected_types = !expected_types_p.empty();
	auto root_reader = createReader(file_meta_data);

	// first type should be a struct containing all the columns types
	auto &root_type = root_reader->logicalType();
	BB_ASSERT(root_type.type() == LogicalTypeId::STRUCT);
	auto& child_types = ((StructColumnReader&)*root_reader).childTypes_;

	if (has_expected_types && child_types.size() != expected_types_p.size()) {
		formatException("column count mismatch");
	}
	idx_t col_idx = 0;
	for (auto &type_pair : child_types) {
		if (has_expected_types && expected_types_p[col_idx] != type_pair.second.getPhysicalType()) {
			if (initial_filename_p.empty()) {
				formatException("column \"%d\" in parquet file is of type %s, could not auto cast to "
				                      "expected type %s for this column",
				                      col_idx, physicalTypeToString(type_pair.second.getPhysicalType()),  physicalTypeToString(expected_types_p[col_idx]));
			} else {
				formatException("schema mismatch in Parquet glob: column \"%d\" in parquet file is of type "
				                      "%s, but in the original file \"%s\" this column is of type \"%s\"",
				                      col_idx, physicalTypeToString(type_pair.second.getPhysicalType()), initial_filename_p,
				                      physicalTypeToString(expected_types_p[col_idx]));
			}
		} else {
			names_.push_back(type_pair.first);
			colNormalizedIdx_[StringUtils::normalizeColumnName(type_pair.first)] = col_idx;
			returnTypes_.push_back(type_pair.second);
		}
		col_idx++;
	}
	BB_ASSERT(!names_.empty());
	BB_ASSERT(!returnTypes_.empty());
}

ParquetReader::ParquetReader(Allocator &allocator_p, std::unique_ptr<FileHandle> file_handle_p,
                             const vector<PhysicalType> &expected_types_p, const string &initial_filename_p)
    : allocator_(allocator_p) {
	fileName_ = file_handle_p->path_;
	fileHandle_ = std::move(file_handle_p);
	metadata_ = loadMetadata(allocator_, *fileHandle_);
	initializeSchema(expected_types_p, initial_filename_p);
}

ParquetReader::ParquetReader(ClientContext &context_p, string file_name_p, const vector<PhysicalType> &expected_types_p,
                             ParquetOptions parquet_options_p, const string &initial_filename_p)
    : allocator_(context_p.allocator_),
      parquetOptions_(parquet_options_p) {
	auto &fs = *context_p.fileSystem_;
	fileName_ = std::move(file_name_p);
	fileHandle_ = fs.openFile(fileName_, FileFlags::FILE_FLAGS_READ, FileSystem::DEFAULT_LOCK,
	                          FileSystem::DEFAULT_COMPRESSION);
	// If object cached is disabled
	// or if this file has cached metadata
	// or if the cached version already expired
	auto last_modify_time = fs.getLastModifiedTime(*fileHandle_);

	metadata_ = loadMetadata(allocator_, *fileHandle_);

	initializeSchema(expected_types_p, initial_filename_p);
}

ParquetReader::~ParquetReader() {
}

const format::FileMetaData *ParquetReader::getFileMetadata() {
	BB_ASSERT(metadata_);
	BB_ASSERT(metadata_->metadata_);
	return metadata_->metadata_.get();
}

std::unique_ptr<BaseStatistics> ParquetReader::readStatistics(ParquetReader &reader, PhysicalType &type,
                                                              idx_t file_col_idx, const format::FileMetaData *file_meta_data) {
	std::unique_ptr<BaseStatistics> column_stats;

	auto root_reader = reader.createReader(file_meta_data);
	auto column_reader = ((StructColumnReader *)root_reader.get())->getChildReader(file_col_idx);

	for (auto &row_group : file_meta_data->row_groups) {
		auto chunk_stats = column_reader->stats(row_group.columns);
		if (!chunk_stats) {
			return nullptr;
		}
		if (!column_stats) {
			column_stats = std::move(chunk_stats);
		} else {
			column_stats->merge(*chunk_stats);
		}
	}
	return column_stats;
}

const format::RowGroup & ParquetReader::getGroup(ParquetReaderScanState &state) {
	auto file_meta_data = getFileMetadata();
	BB_ASSERT(state.currentGroup_ >= 0 && (idx_t)state.currentGroup_ < state.groupIdxList_.size());
	BB_ASSERT(state.groupIdxList_[state.currentGroup_] >= 0 &&
	         state.groupIdxList_[state.currentGroup_] < file_meta_data->row_groups.size());
	return file_meta_data->row_groups[state.groupIdxList_[state.currentGroup_]];
}

void ParquetReader::prepareRowGroupBuffer(ParquetReaderScanState &state, idx_t out_col_idx) {
	auto &group = getGroup(state);

	auto column_reader = ((StructColumnReader *)state.rootReader_.get())->getChildReader(state.columnIds_[out_col_idx]);

	if (state.filters_) {
		auto stats = column_reader->stats(group.columns);
		// filters contain output chunk index, not file col idx!
		auto filter_entry = state.filters_->filters_.find(out_col_idx);
		if (stats && filter_entry != state.filters_->filters_.end()) {
			bool skip_chunk = false;
			auto &filter = *filter_entry->second;
			auto prune_result = filter.checkStatistics(*stats);
			if (prune_result == FilterPropagateResult::FILTER_ALWAYS_FALSE) {
				skip_chunk = true;
			}
			if (skip_chunk) {
				state.groupOffset_ = group.num_rows;
				return;
				// this effectively will skip this chunk
			}
		}
	}

	state.rootReader_->initializeRead(group.columns, *state.thriftFileProto_);
}

idx_t ParquetReader::numRows() {
	return getFileMetadata()->num_rows;
}

idx_t ParquetReader::numRowGroups() {
	return getFileMetadata()->row_groups.size();
}

void ParquetReader::initializeScan(ParquetReaderScanState &state, vector<idx_t> column_ids,
                                   vector<idx_t> groups_to_read, TableFilterSet *filters) {
	state.currentGroup_ = -1;
	state.finished_ = false;
	state.columnIds_ = std::move(column_ids);
	state.groupOffset_ = 0;
	state.groupIdxList_ = std::move(groups_to_read);
	state.filters_ = filters;
	state.sel_.initialize(STANDARD_VECTOR_SIZE);
	state.fileHandle_ =
	    fileHandle_->fileSystem_.openFile(fileHandle_->path_, FileFlags::FILE_FLAGS_READ, FileSystem::DEFAULT_LOCK,
	                                      FileSystem::DEFAULT_COMPRESSION);
	state.thriftFileProto_ = createThriftProtocol(allocator_, *state.fileHandle_);
	state.rootReader_ = createReader(getFileMetadata());

	state.defineBuf_.resize(allocator_, STANDARD_VECTOR_SIZE);
	state.repeatBuf_.resize(allocator_, STANDARD_VECTOR_SIZE);
}



void ParquetReader::scan(ParquetReaderScanState &state, DataChunk &result) {
	while (scanInternal(state, result)) {
		if (result.getSize() > 0) {
			break;
		}
		result.reset();
	}
}

string ParquetReader::getAvailableColumns() const {
	string result = "";
	for (auto& [col,_]:colNormalizedIdx_)
		result +=  col + ", ";
	result.pop_back();
	result.pop_back();
	return result;
}

bool ParquetReader::scanInternal(ParquetReaderScanState &state, DataChunk &result) {
	if (state.finished_) {
		return false;
	}

	// see if we have to switch to the next row group in the parquet file
	if (state.currentGroup_ < 0 || (int64_t)state.groupOffset_ >= getGroup(state).num_rows) {
		// first group scan or groupOffset_ > num_rows
		state.currentGroup_++;
		state.groupOffset_ = 0;

		if ((idx_t)state.currentGroup_ == state.groupIdxList_.size()) {
			state.finished_ = true;
			return false;
		}

		// prepare the scan for each column
		for (idx_t out_col_idx = 0; out_col_idx < result.columnCount(); out_col_idx++) {
			// this is a special case where we are not interested in the actual contents of the file
			if (state.columnIds_[out_col_idx] == COLUMN_IDENTIFIER_ROW_ID) {
				continue;
			}

			prepareRowGroupBuffer(state, out_col_idx);
		}
		return true;
	}

	auto this_output_chunk_rows = minValue<idx_t>(STANDARD_VECTOR_SIZE, getGroup(state).num_rows - state.groupOffset_);
	result.setCardinality(this_output_chunk_rows);

	if (this_output_chunk_rows == 0) {
		state.finished_ = true;
		return false; // end of last group, we are done
	}

	// we evaluate simple table filters directly in this scan so we can skip decoding column data that's never going to
	// be relevant
	parquet_filter_t filter_mask;
	filter_mask.set();

	state.defineBuf_.zero();
	state.repeatBuf_.zero();

	auto define_ptr = (uint8_t *)state.defineBuf_.ptr_;
	auto repeat_ptr = (uint8_t *)state.repeatBuf_.ptr_;

	auto root_reader = ((StructColumnReader *)state.rootReader_.get());

	if (state.filters_) {
		vector<bool> need_to_read(result.columnCount(), true);

		// first load the columns that are used in filters
		for (auto &filter_col : state.filters_->filters_) {
			auto file_col_idx = state.columnIds_[filter_col.first];

			if (filter_mask.none()) { // if no rows are left we can stop checking filters
				break;
			}

			root_reader->getChildReader(file_col_idx)
			    ->read(result.getSize(), filter_mask, define_ptr, repeat_ptr, result.data_[filter_col.first]);

			need_to_read[filter_col.first] = false;

		}

		// we still may have to read some cols
		for (idx_t out_col_idx = 0; out_col_idx < result.columnCount(); out_col_idx++) {
			if (!need_to_read[out_col_idx]) {
				continue;
			}
			auto file_col_idx = state.columnIds_[out_col_idx];

			if (filter_mask.none()) {
				root_reader->getChildReader(file_col_idx)->skip(result.getSize());
				continue;
			}
			root_reader->getChildReader(file_col_idx)
			    ->read(result.getSize(), filter_mask, define_ptr, repeat_ptr, result.data_[out_col_idx]);
		}

		idx_t sel_size = 0;
		for (idx_t i = 0; i < this_output_chunk_rows; i++) {
			if (filter_mask[i]) {
				state.sel_.setIndex(sel_size++, i);
			}
		}

		result.slice(state.sel_, sel_size);

	} else { // #no filter, just  load the data
		for (idx_t out_col_idx = 0; out_col_idx < result.columnCount(); out_col_idx++) {
			auto file_col_idx = state.columnIds_[out_col_idx];

			if (file_col_idx == COLUMN_IDENTIFIER_ROW_ID) {
				continue;
			}

			root_reader->getChildReader(file_col_idx)
			    ->read(result.getSize(), filter_mask, define_ptr, repeat_ptr, result.data_[out_col_idx]);
		}
	}

	state.groupOffset_ += this_output_chunk_rows;
	return true;
}

}
