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
#include "bumblebee/function/predicate/ReadCsv.hpp"
#include <sstream>

#include "bumblebee/common/Limits.hpp"

namespace bumblebee{

void parseColumns(string& columns,vector<string>& colNames,vector<ConstantType>& colTypes) {
	colNames.clear();
	colTypes.clear();

	std::stringstream ss(columns);
	string token;

	while (getline(ss, token, ';')) { // Split by ';'
		if (token.empty()) continue;

		auto pos = token.find(':');
		if (pos != string::npos) {
			string name = token.substr(0, pos);
			string type = token.substr(pos + 1);

			// Trim whitespace (if any)
			auto trim = [](string& s) {
				s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
					return !isspace(ch);
				}));
				s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
					return !isspace(ch);
				}).base(), s.end());
			};

			trim(name);
			trim(type);

			colNames.push_back(name);
			auto t = ctypeFromString(type);
			if (t==UNKNOWN) {
				ErrorHandler::errorParsing("Error type "+ type+" is UNKNOWN.");
			}
			colTypes.push_back(ctypeFromString(type));
		}
	}
}



idx_t ReadCSVData::getMaxThread() {
    if (maxThreads_)
        return maxThreads_;

    BB_ASSERT(initialReader_);
    BB_ASSERT(initialReader_->avgBytesInChunk_);
    idx_t bytesPerLineAvg = initialReader_->avgBytesInChunk_ / STANDARD_VECTOR_SIZE;

    // calculate the total size
    idx_t totalSize = 0;
    for (auto fp : files_) {
        auto result = initialReader_->fs_.openFile(fp.c_str(), FileFlags::FILE_FLAGS_READ, FileLockType::NO_LOCK, initialReader_->compression_);
        filesSize_[fp] = result->getFileSize();
        totalSize += result->getFileSize();
    }

    auto bytesMorsel = bytesPerLineAvg * MORSEL_SIZE;

    maxThreads_ =  totalSize / bytesMorsel + 1;
    return maxThreads_;
}

vector<ReadCSVDataChunk> ReadCSVData::getNextChunksToRead() {
    BB_ASSERT(initialReader_->avgBytesInChunk_);
	idx_t bytesPerLineAvg = initialReader_->avgBytesInChunk_ / STANDARD_VECTOR_SIZE;

    lock_guard lock(mutex_);

    idx_t currentFile = 0;
    idx_t currentOffset = 0;
    if (chunks_.size() > 0) {
        // check if the last files was completed
        auto& last = chunks_.back().back();
        idx_t totalEstimatedLines = filesSize_[files_[last.file_]] / bytesPerLineAvg;
        if (last.end_ >= totalEstimatedLines) {
            // file completed read the next file
            currentFile = last.file_ +1 ;
        }else {
            currentFile = last.file_;
            currentOffset = last.end_ +1;
        }
    }
	// all data was assigned to other threads, return empty chunks
	if (currentFile >= files_.size()) {
		return {};
	}
    // insert the chunks for this thread
    chunks_.emplace_back();
    idx_t currentLines = 0;
    while (currentLines < MORSEL_SIZE) {
        if (currentFile >= files_.size()) {
            // all files completed
            break;
        }
        idx_t totalEstimatedLines = filesSize_[files_[currentFile]] / bytesPerLineAvg;
        idx_t start = currentOffset;
        idx_t end = start + minValue<idx_t>(MORSEL_SIZE, totalEstimatedLines - start) - 1;
        currentLines = end - start + 1;
    	chunks_.back().emplace_back();
        auto& last = chunks_.back().back();
        last.file_ = currentFile;
        last.start_ = start;
        if (end >= totalEstimatedLines - 1) {
            // set end to large integer to ensure that the thread read all the file content
            end = NumericLimits<uint64_t>::maximum();
            // go to the next file
            currentFile += 1;
            currentOffset = 0;
        }
        last.end_ = end;
    }

    return chunks_.back();
}


static function_data_ptr_t readCSVBind(ClientContext &context,
                                       vector<Value> &inputs,
                                       vector<ConstantType> & inputTypes,
                                       std::unordered_map<string, Value> &parameters,
                                       vector<ConstantType> &returnTypes,vector<string> &names,
                                       vector<Expression>& filters) {
	auto result = std::make_unique<ReadCSVData>();
	auto &options = result->options_;

	BB_ASSERT(inputs.size() == 1);
	BB_ASSERT(returnTypes.empty());
	BB_ASSERT(!names.empty());
	string filePattern = inputs[0].toString();

	auto &fs = *context.fileSystem_;
	result->files_ = fs.glob(filePattern);
	if (result->files_.empty()) {
		ErrorHandler::errorParsing("No files found that match the pattern: " + filePattern);
	}
	vector<string> colNames;
	vector<ConstantType> colTypes;

	for (auto &kv : parameters) {
		if (kv.first == "auto_detect") {
			options.autoDetect_ = kv.second.getValueUnsafe<uint8_t>();
		} else if (kv.first == "sep" || kv.first == "delim") {
			options.delimiter_ = kv.second.toString();
			options.hasDelimiter_ = true;
		} else if (kv.first == "header") {
			options.header_ = kv.second.getValueUnsafe<uint8_t>();
			options.hasHeader_ = true;
		} else if (kv.first == "quote") {
			options.quote_ = kv.second.toString();
			options.hasQuote_ = true;
		} else if (kv.first == "escape") {
			options.escape_ = kv.second.toString();
			options.hasEscape_ = true;
		} else if (kv.first == "sample_size") {
			int64_t sample_size = kv.second.getValueUnsafe<int64_t>();
			if (sample_size <= STANDARD_VECTOR_SIZE) {
				options.sampleChunkSize_ = sample_size;
				options.sampleChunks_ = 1;
			} else {
				options.sampleChunkSize_ = STANDARD_VECTOR_SIZE;
				options.sampleChunks_ = sample_size / STANDARD_VECTOR_SIZE;
			}
		} else if (kv.first == "sample_chunks") {
			options.sampleChunks_ = kv.second.getValueUnsafe<int64_t>();
		} else if (kv.first == "all_varchar") {
			options.allVarchar_ = kv.second.getValueUnsafe<uint8_t>();
		} else if (kv.first == "columns") {
			auto columns = kv.second.toString();
			// parse the columns
			parseColumns(columns, colNames, colTypes);
		} else if (kv.first == "compression") {
			options.compression_ = kv.second.toString();
		}
	}

	if (!options.autoDetect_ && colTypes.empty()) {
		ErrorHandler::errorParsing("read_csv requires columns to be specified or specify auto_detect=1.");
	}
	if (!(options.compression_ == "infer" || options.compression_ == "gzip" || options.compression_ == "none" ||
	      options.compression_.empty())) {
		ErrorHandler::errorParsing("read_csv currently only supports 'gzip' compression.");
	}
	options.filePath_ = result->files_[0];

	if (options.autoDetect_) {
		result->initialReader_ = std::make_unique<BufferedCSVReader>(context, options);
		// autodect on, fetch the names and types from the initial reader sniff
		colTypes = result->initialReader_->types_;
		result->types_ = result->initialReader_->types_;
		colNames = result->initialReader_->colNames_;
	} else {
		result->initialReader_ = std::make_unique<BufferedCSVReader>(context, options, colTypes);
		// set the types and names from user input
		result->types_ = colTypes;
		result->initialReader_->colNames_ = colNames;
	}
	if (names.size() == 1 && names[0] == "*") {
		// select all the avaliable columns
		names = result->initialReader_->colNames_;
	}

	// build the map of col name and type
	std::unordered_map<string, ConstantType> colMap;

	BB_ASSERT(colTypes.size() == colNames.size());
	for (idx_t i = 0;i<colNames.size();i++)
		colMap[colNames[i]] = colTypes[i];


	// check all the names are in col name and build returnTypes
	for (auto& name: names) {
		if (!colMap.contains(name))
			ErrorHandler::errorParsing("Column " + name + " does not exist.");
		returnTypes.push_back(colMap[name]);
	}


	result->cols_.clear();
	for (auto& name: names) {
		for (idx_t i = 0;i<colNames.size();i++)
			if (name == colNames[i])
				result->cols_.push_back(i);
	}

	return std::move(result);
}

static function_op_data_ptr_t readCSVInit(ClientContext &context, const FunctionData *bind_data_p) {

	auto &bind_data = (ReadCSVData &)*bind_data_p;

	auto result = std::make_unique<ReadCSVOperatorData>();

	result->chunks_ = bind_data.getNextChunksToRead();

	result->readChunk_.initialize(bind_data.types_);

	return std::move(result);
}

static idx_t readCSVMaxThread(ClientContext &context, const FunctionData *bind_data_p) {
	auto &bind_data = (ReadCSVData &)*bind_data_p;

	return bind_data.getMaxThread();
}

static void readCSVFunction(ClientContext &context, const FunctionData *bind_data_p,
									 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (ReadCSVData &)*bind_data_p;
	auto &data = (ReadCSVOperatorData &)*operator_state;


	while (data.chunkIndex_ < data.chunks_.size()) {
		auto& chunk = data.chunks_[data.chunkIndex_];

		if (!data.csvReader_) {
			auto option = bind_data.options_;
			option.filePath_ = bind_data.files_[chunk.file_];
			option.skipRows_ = chunk.start_;
			data.csvReader_ = std::make_unique<BufferedCSVReader>(*context.fileSystem_, option, bind_data.types_);
		}
		if (chunk.start_ < chunk.end_) {
			data.readChunk_.setCardinality(0);
			data.csvReader_->parseCSV(data.readChunk_);
			// select the interested cols
			output.reference(data.readChunk_, bind_data.cols_);
			chunk.start_ += data.readChunk_.getSize();
		}

		if (output.getSize() == 0 || chunk.start_ >= chunk.end_) {
			// exhausted this file, but we have more files we can read
			data.chunkIndex_++;
			data.csvReader_ = nullptr;
		} else
			break;
	}

}


static void readCSVAddNamedParameters(PredFunction &table_function) {
	table_function.namedParameters_["sep"] = ConstantType::STRING;
	table_function.namedParameters_["delim"] = ConstantType::STRING;
	table_function.namedParameters_["quote"] = ConstantType::STRING;
	table_function.namedParameters_["escape"] = ConstantType::STRING;
	table_function.namedParameters_["columns"] = ConstantType::STRING;
	table_function.namedParameters_["header"] = ConstantType::UTINYINT;
	table_function.namedParameters_["auto_detect"] = ConstantType::UTINYINT;
	table_function.namedParameters_["sample_size"] = ConstantType::BIGINT;
	table_function.namedParameters_["sample_chunks"] = ConstantType::BIGINT;
	table_function.namedParameters_["all_varchar"] = ConstantType::UTINYINT;
	table_function.namedParameters_["compression"] = ConstantType::STRING;
}


function_ptr_t ReadCsvFunc::getFunction() {
	string name = "&read_csv";
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {STRING}, readCSVFunction, readCSVBind, readCSVInit, readCSVMaxThread, nullptr, nullptr));
	readCSVAddNamedParameters((PredFunction&)*fun);
	return fun;
}

void ReadCsvFunc::registerFunction(FunctionRegister &funcRegister) {
	funcRegister.registerFunction(getFunction());
}


}
