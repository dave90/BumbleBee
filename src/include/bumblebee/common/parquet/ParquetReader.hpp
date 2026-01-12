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
#include "ParquetFileMetadataCache.hpp"
#include "parquet/parquet_types.h"


#include "bumblebee/common/FileSystem.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/Vector.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/SelectionVector.hpp"
#include "thrift/protocol/TProtocol.h"

namespace bumblebee {
namespace format {
class FileMetaData;
}
} // namespace duckdb_parquet

namespace bumblebee {
class Allocator;
class ClientContext;
class ChunkCollection;
class BaseStatistics;
class TableFilterSet;

const idx_t COLUMN_IDENTIFIER_ROW_ID = (idx_t)-1; // ID of the columns to skip

struct ParquetReaderScanState {
	vector<idx_t> groupIdxList_; // groups to read
	int64_t currentGroup_;
	vector<idx_t> columnIds_; // columns to read
	idx_t groupOffset_; // row offset inside the group
	std::unique_ptr<FileHandle> fileHandle_;
	std::unique_ptr<ColumnReader> rootReader_;
	std::unique_ptr<TProtocol> thriftFileProto_;

	bool finished_;
	TableFilterSet *filters_;
	SelectionVector sel_;

	ResizeableBuffer defineBuf_;
	ResizeableBuffer repeatBuf_;
};

struct ParquetOptions {
	bool binaryAsString_ = false;
};

class ParquetReader {
public:
	ParquetReader(Allocator &allocator, std::unique_ptr<FileHandle> file_handle_p,
	              const vector<PhysicalType> &expected_types_p, const string &initial_filename_p = string());
	ParquetReader(Allocator &allocator, std::unique_ptr<FileHandle> file_handle_p)
	    : ParquetReader(allocator, std::move(file_handle_p), vector<PhysicalType>(), string()) {
	}

	ParquetReader(ClientContext &context, string file_name, const vector<PhysicalType> &expected_types_p,
	              ParquetOptions parquet_options, const string &initial_filename = string());
	ParquetReader(ClientContext &context, string file_name, ParquetOptions parquet_options)
	    : ParquetReader(context, std::move(file_name), vector<PhysicalType>(), parquet_options, string()) {
	}
	~ParquetReader();

	Allocator &allocator_;
	string fileName_;
	vector<LogicalType> returnTypes_;
	vector<string> names_;
	std::shared_ptr<ParquetFileMetadataCache> metadata_;
	ParquetOptions parquetOptions_;

public:
	void initializeScan(ParquetReaderScanState &state, vector<idx_t> column_ids, vector<idx_t> groups_to_read,
	                    TableFilterSet *table_filters);
	void scan(ParquetReaderScanState &state, DataChunk &output);

	idx_t numRows();
	idx_t numRowGroups();

	const bumblebee::format::FileMetaData *getFileMetadata();

	static std::unique_ptr<BaseStatistics> readStatistics(ParquetReader &reader, PhysicalType &type, idx_t column_index,
	                                                 const bumblebee::format::FileMetaData *file_meta_data);

private:
	void initializeSchema(const vector<PhysicalType> &expected_types_p, const string &initial_filename_p);
	bool scanInternal(ParquetReaderScanState &state, DataChunk &output);
	std::unique_ptr<ColumnReader> createReader(const bumblebee::format::FileMetaData *file_meta_data);

	std::unique_ptr<ColumnReader> createReaderRecursive(const bumblebee::format::FileMetaData *file_meta_data,
	                                               idx_t depth, idx_t max_define, idx_t max_repeat,
	                                               idx_t &next_schema_idx, idx_t &next_file_idx);
	const bumblebee::format::RowGroup &getGroup(ParquetReaderScanState &state);
	void prepareRowGroupBuffer(ParquetReaderScanState &state, idx_t out_col_idx);
	LogicalType deriveLogicalType(const SchemaElement &s_ele);
	PhysicalType derivePhysicalType(const SchemaElement &s_ele);

	template <typename... Args>
	void formatException(const string fmt_str, Args... params) {
		ErrorHandler::errorParsing("Failed to read Parquet file \"" + fileName_ +
		                          "\": " + StringUtils::format(fmt_str, params...));
	}

private:
	std::unique_ptr<FileHandle> fileHandle_;
};


template <class T>
using child_list_t = std::vector<std::pair<std::string, T>>;



}
