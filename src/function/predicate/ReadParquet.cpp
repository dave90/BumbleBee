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
#include "bumblebee/function/predicate/ReadParquet.hpp"

namespace bumblebee{


idx_t ReadParquetData::getMaxThread() {

	ParquetOptions options;

	filesToProcess_.clear();
	for (idx_t fileIdx = 0; fileIdx < files_.size(); ++fileIdx) {
		ParquetReader reader(context_, files_[fileIdx], options);
		auto metadata = reader.getFileMetadata();

		idx_t start = 0;
		idx_t currentRows = 0;

		for (idx_t rgIdx = 0; rgIdx < metadata->row_groups.size(); ++rgIdx) {
			const auto rows = metadata->row_groups[rgIdx].num_rows;

			// If adding this group would exceed the morsel and we already have something, flush.
			if (currentRows > 0 && currentRows + rows > MORSEL_SIZE) {
				// end is inclusive
				filesToProcess_.emplace_back(fileIdx, start, rgIdx-1);
				start = rgIdx;
				currentRows = 0;
			}

			currentRows += rows;
		}

		// Flush tail
		if (start < metadata->row_groups.size()) {
			filesToProcess_.emplace_back(fileIdx, start, metadata->row_groups.size()-1);
		}
	}

	return filesToProcess_.size();
}

ReadParquetDataChunk  ReadParquetData::getNextChunksToRead() {
	auto index = nextFileToProcess_.fetch_add(1);
	if (index >= filesToProcess_.size()) {
		return ReadParquetDataChunk(filesToProcess_.size(),0,0); // or a sentinel
	}
	return filesToProcess_[index];
}


static function_data_ptr_t readParquetBind(ClientContext &context,
                                       vector<Value> &inputs,
                                       vector<LogicalType> & inputTypes,
                                       std::unordered_map<string, Value> &parameters,
                                       vector<LogicalType> &returnTypes, vector<string> &names,
                                       TableFilterSet& filters) {
	auto result = std::make_unique<ReadParquetData>(context);

	BB_ASSERT(inputs.size() == 1);
	BB_ASSERT(returnTypes.empty());
	BB_ASSERT(!names.empty());
	string folder = inputs[0].toString();
	string path = folder;
	auto &fs = *context.fileSystem_;
	if (!StringUtils::hasGlob(folder)) {
		auto fileExist = fs.fileExists(folder);
		// is not a glob, check if is a directory
		if (!fileExist && !fs.directoryExists(folder))
			ErrorHandler::errorParsing("No files found in the folder: " + path);

		if (!fileExist) {
			// is a directory add the *.parquet to the file pattern
			auto separator = context.fileSystem_->getFileSeparator();
			path = folder + separator + "**" + separator + "*" + result->extension_;
		}
	}

	result->files_ = fs.glob(path);
	if (result->files_.empty()) {
		ErrorHandler::errorParsing("No files found that match the pattern: " + path);
	}


	for (auto &kv : parameters) {
		if (kv.first == "binary_as_string") {
			result->options_.binaryAsString_ = kv.second.getValueUnsafe<uint8_t>();
		}
	}

	ParquetReader reader(context, result->files_[0], result->options_);
	if (names.size() == 1 && names[0] == "*") {
		// select all the available columns
		names.clear();
		for (auto& name:reader.names_) {
			names.push_back(StringUtils::normalizeColumnName(name));
		}
	}
	returnTypes.clear();
	result->cols_.clear();
	// check all the names are in col name and build returnTypes
	for (auto& name: names) {
		if (!reader.colNormalizedIdx_.contains(name))
			ErrorHandler::errorParsing("Column " + name + " does not exist. Avaliable columns: ["+reader.getAvailableColumns()+"]");
		auto col = reader.colNormalizedIdx_[name];
		result->cols_.push_back(col);
		returnTypes.push_back(reader.returnTypes_[col]);
	}

	if (!filters.filters_.empty()) {
		// build the filters
		result->filters_ = table_filter_set_ptr_t(new TableFilterSet());
		*result->filters_ = std::move(filters);
	}


	return std::move(result);
}

static function_op_data_ptr_t readParquetInit(ClientContext &context, const FunctionData *bind_data_p) {

	auto &bind_data = (ReadParquetData &)*bind_data_p;

	auto result = std::make_unique<ReadParquetOperatorData>();

	result->filesToProcess_ = bind_data.getNextChunksToRead();
	if (result->filesToProcess_.file_ >= bind_data.files_.size())
		result->finished_ = true;

	return std::move(result);
}

static idx_t readParquetMaxThread(ClientContext &context, const FunctionData *bind_data_p) {
	auto &bind_data = (ReadParquetData &)*bind_data_p;

	return bind_data.getMaxThread();
}

static void readParquetFunction(ClientContext &context, const FunctionData *bind_data_p,
									 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (ReadParquetData &)*bind_data_p;
	auto &data = (ReadParquetOperatorData &)*operator_state;

	if (data.finished_) return;

	if (!data.reader_) {
		BB_ASSERT(data.filesToProcess_.file_ < bind_data.files_.size());
		auto file = bind_data.files_[data.filesToProcess_.file_];
		vector<idx_t> groups;
		for (idx_t i=data.filesToProcess_.groupStart_;i<=data.filesToProcess_.groupEnd_;++i)
			groups.push_back(i);

		data.reader_ = parquet_reader_ptr_t(new ParquetReader(context, file, bind_data.options_));
		// set the non-needed columns to COLUMN_IDENTIFIER_ROW_ID
		vector<idx_t> colIdx;
		std::unordered_set colsSet(bind_data.cols_.begin(), bind_data.cols_.end());
		for (idx_t i=0;i<data.reader_->names_.size();++i) {
			if (colsSet.contains(i))
				colIdx.emplace_back(i);
			else
				colIdx.push_back(COLUMN_IDENTIFIER_ROW_ID);
		}
		data.reader_->initializeScan(data.readerState_, colIdx, groups, bind_data.filters_.get());
		data.readChunk_.initialize(data.reader_->returnTypes_);
	}

	data.reader_->scan(data.readerState_, data.readChunk_);

	output.reference(data.readChunk_, bind_data.cols_);

	if (output.getSize() == 0 ) {
		// exhausted
		data.finished_ = true;
		data.reader_ = nullptr;
		data.readChunk_.data_.clear();
	}else {
		data.readChunk_.reset();
	}

}


static void readParquetAddNamedParameters(PredFunction &table_function) {
	table_function.namedParameters_["binary_as_string"] = PhysicalType::UTINYINT;
}

string ReadParquetFunc::getName() {
	return "&read_parquet";
}


function_ptr_t ReadParquetFunc::createFunction(const vector<LogicalType> &type) {
	string name = getName();
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {PhysicalType::STRING}, readParquetFunction, readParquetBind, readParquetInit, readParquetMaxThread, nullptr, nullptr));
	readParquetAddNamedParameters((PredFunction&)*fun);
	return fun;
}

void ReadParquetFunc::registerFunction(FunctionRegister &funcRegister) {
	std::unique_ptr<FunctionGenerator> fg = std::make_unique<ReadParquetFunc>();
	funcRegister.registerFunctionGen(fg);

}


}