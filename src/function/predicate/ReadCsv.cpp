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
#include <filesystem>
#include <numeric>
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

static bool hasGlob(const string &str) {
	for (idx_t i = 0; i < str.size(); i++) {
		switch (str[i]) {
			case '*':
			case '?':
			case '[':
				return true;
			default:
				break;
		}
	}
	return false;
}

idx_t ReadCSVData::getMaxThread() {
	// if we have multi files each thread read a file

	if (files_.size() > 1) {
		for (idx_t i = 0; i < files_.size(); i++)
			filesToProcess_.emplace_back(i,0,0);

		return files_.size();
	}
	// we have only one file to read
	BB_ASSERT(initialReader_);
	auto fileSizeBytes = initialReader_->fileSize_;
	if (fileSizeBytes < CSV_MULTITHREAD_THRESHOLD_BYTES) {
		// for small files let's use only one thread
		filesToProcess_.emplace_back(0,0,0);
		return 1;
	}

	auto fh = initialReader_->fileHandle_.get();
	BB_ASSERT(fh);
	if (!fh->canSeek()) {
		// we cannot jump in the middle of the file
		// so only one thread is allowed
		filesToProcess_.emplace_back(0,0,0);
		return 1;
	}
	// calculate the bytes in STANDARD_VECTOR_SIZE rows
	auto line = fh->readLine(); // read first line (in case of header)
	idx_t bytesInChunk = 0;
	for (idx_t i = 0; i < STANDARD_VECTOR_SIZE; i++) {
		line = fh->readLine();
		bytesInChunk += line.size();
	}
	// idea is to read for each task 4 (MORSE_ / VECTOR SIZE) data chunks
	auto morselBytesSize = bytesInChunk * MORSEL_SIZE/STANDARD_VECTOR_SIZE;
	auto tasks = fileSizeBytes/morselBytesSize;
	idx_t start = 0;
	for (idx_t i=0; i< tasks; i++) {
		auto end = start+morselBytesSize;
		if (i == tasks - 1) {
			// read until end
			end = 0;
		}else {
			// we need to read until the end of the line
			fh->seek(end);
			auto string = fh->readLine();
			end += string.size();
		}

		filesToProcess_.emplace_back(0,start,end);
		start = end + 1;
	}
	return tasks;
}

ReadCSVDataChunk ReadCSVData::getNextFileToRead() {
	auto index = nextFileToProcess_.fetch_add(1);
	return filesToProcess_[index];
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
	string folder = inputs[0].toString();
	string path = folder;
	auto &fs = *context.fileSystem_;
	if (!hasGlob(folder)) {
		auto fileExist = fs.fileExists(folder);
		// is not a glob, check if is a directory
		if (!fileExist && !fs.directoryExists(folder))
			ErrorHandler::errorParsing("No files found in the folder: " + path);

		if (!fileExist) {
			// is a directory add the *.csv to the file pattern
			auto separator = context.fileSystem_->getFileSeparator();
			path = folder + separator + "**" + separator + "*.csv";
		}
	}

	result->files_ = fs.glob(path);
	if (result->files_.empty()) {
		ErrorHandler::errorParsing("No files found that match the pattern: " + path);
	}
	vector<string> colNames;
	vector<ConstantType> colTypes;

	for (auto &kv : parameters) {
		if (kv.first == "auto_detect") {
			options.autoDetect_ = kv.second.getValueUnsafe<uint8_t>();
		} else if (kv.first == "sep" || kv.first == "delim") {
			options.autoDetect_ = false;
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

	result->filesToProcess_ = bind_data.getNextFileToRead();

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

	if (data.finished_) return;

	if (!data.csvReader_) {
		auto option = (bind_data.initialReader_) ? bind_data.initialReader_->options_ : bind_data.options_;
		option.filePath_ = bind_data.files_[data.filesToProcess_.file_];
		option.autoDetect_ = false;
		if (data.filesToProcess_.byteStart_ > 0)
			// disable header if this task read in the middle of file
			option.header_ = false;
		data.csvReader_ = std::make_unique<BufferedCSVReader>(*context.fileSystem_, option, bind_data.types_);
		if (data.filesToProcess_.byteStart_ > 0)
			data.csvReader_->fileHandle_->seek(data.filesToProcess_.byteStart_);
		if (data.filesToProcess_.byteEnd_)
			data.csvReader_->bytesToRead_ = data.filesToProcess_.byteEnd_ - data.filesToProcess_.byteStart_;
	}
	if ( data.filesToProcess_.byteEnd_ > 0 && data.filesToProcess_.byteStart_ >= data.filesToProcess_.byteEnd_) {
		// we complete to read our data (if byteEnd_ == 0 we need to read until the end)
		data.finished_ = true;
		data.csvReader_ = nullptr;
		return;
	}
	data.csvReader_->parseCSV(data.readChunk_);
	output.reference(data.readChunk_, bind_data.cols_);
	auto& rb = data.csvReader_->chunkByteSizes_;
	auto bytesInChunk = data.csvReader_->chunkByteSizes_;
	data.filesToProcess_.byteStart_ += bytesInChunk;

	if (output.getSize() == 0 ) {
		// exhausted this file
		data.finished_ = true;
		data.csvReader_ = nullptr;
	}else {
		data.readChunk_.reset();
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
