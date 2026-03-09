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

#include <filesystem>

#include "bumblebee/function/predicate/WriteParquet.hpp"

#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{

static void parseColumns(const string& partitionString, vector<string>& partitions) {

	partitions.clear();
	std::stringstream ss(partitionString);
	string token;

	while (getline(ss, token, ',')) { // Split by ';'
		if (token.empty()) continue;
		partitions.push_back(token);
	}
}

string WriteParquetData::getFileToWrite() {
	auto separator = fs.getFileSeparator();

	// return a new file based on thread id
	auto id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
	lock_guard lock(mutex_);

	string filename = StringUtils::getUUID() + "_" + id + tmpExtension_;
	auto file = folder_ + separator + filename;
	if (isSingleFile_) {
		// single file change the id and the file pointing to the same file
		id = singleFile_;
		file = singleFile_ + tmpExtension_;
	}

	if (threadFiles_.contains(id))
		return threadFiles_[id];

	threadFiles_[id] = file;

	// if the file exist delete it
	// we are safe to delete it as is tmp file with uuid as name
	if (fs.fileExists(file)) {
		fs.removeFile(file);
	}

	// create the lock and file handle
	lockFiles_[file] = std::make_unique<std::mutex>();
	filesWriter_[file] = parquet_writer_ptr_t(new ParquetWriter(fs, file, colTypes_, colNames_, codec_));

	return file;
}



static function_data_ptr_t writeParquetBind(ClientContext &context,
                                        vector<Value> &inputs,
                                        vector<LogicalType> & inputTypes,
                                        std::unordered_map<string, Value> &parameters,
                                        std::unordered_map<string, idx_t> &bindVarName,
                                        vector<LogicalType> &returnTypes, vector<string> &names,
                                        TableFilterSet& filters) {

	auto result = std::make_unique<WriteParquetData>(*context.fileSystem_);

	BB_ASSERT(inputs.size() == 1);
	BB_ASSERT(!names.empty());
	BB_ASSERT(!returnTypes.empty());
	string filePattern = inputs[0].toString();

	result->folder_ = filePattern;

	result->codec_ = format::CompressionCodec::UNCOMPRESSED;

	for (auto &kv : parameters) {
		if (kv.first == "mode") {
			if (StringUtils::lower( kv.second.toString()) == "overwrite")
				result->overwrite_ = true;
			else if (StringUtils::lower( kv.second.toString()) == "append")
				result->overwrite_ = false;
			else
				LOG_WARNING("Warning value: %s is not a valid mode. Available mode: [overwrite, append]", kv.second.toString().c_str());

		} if (kv.first == "columns") {
			parseColumns(kv.second.toString(), result->colNames_);
		}else if (kv.first == "single_file") {
			result->isSingleFile_ = kv.second.getValueUnsafe<uint8_t>();
			result->singleFile_ = filePattern;
			// if ends with the extension remove the extensions as we will add it
			if (result->singleFile_.ends_with(result->extension_))
				result->singleFile_.erase(result->singleFile_.size() - result->extension_.size());
		}else if (kv.first == "codec") {
			if (kv.second.toString() == "UNCOMPRESSED") {
				result->codec_ = format::CompressionCodec::UNCOMPRESSED;
			}else if (kv.second.toString() == "SNAPPY") {
				result->codec_ = format::CompressionCodec::SNAPPY;
			}else if (kv.second.toString() == "GZIP") {
				result->codec_ = format::CompressionCodec::GZIP;
			}else if (kv.second.toString() == "ZSTD") {
				result->codec_ = format::CompressionCodec::ZSTD;
			}else {
				string warning = "Parquet codec "+kv.second.toString()+" not supported, using uncompressed";
				LOG_WARNING("Parquet codec %s not supported, using uncompressed. Available codec: [UNCOMPRESSED, SNAPPY, GZIP, ZSTD]", kv.second.toString().c_str());
				result->codec_ = format::CompressionCodec::UNCOMPRESSED;
			}
		}
	}

	for (auto& var: names) {
		BB_ASSERT(bindVarName.contains(var));
		result->colsIdx_.push_back(bindVarName.at(var));
	}

	if (!result->isSingleFile_ && !context.fileSystem_->directoryExists(result->folder_))
		context.fileSystem_->createDirectory(result->folder_);

	result->colTypes_ = returnTypes;

	if (result->colNames_.empty())
		//set the col names the variables
		result->colNames_ = names;

	if (result->colNames_.size() != names.size()) {
		ErrorHandler::errorParsing("Error, names from columns configuration does not match number of variables in external atom!");
	}

	return std::move(result);
}


static function_op_data_ptr_t writeParquetInit(ClientContext &context, const FunctionData *bind_data_p) {

	auto &bind_data = (WriteParquetData &)*bind_data_p;

	auto result = std::make_unique<WriteParquetOperatorData>();

	result->file_ = bind_data.getFileToWrite();

	result->writer_ = bind_data.filesWriter_[result->file_].get();

	return std::move(result);
}


static void writeParquetFunction(ClientContext &context, const FunctionData *bind_data_p,
                             FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (WriteParquetData &)*bind_data_p;
	auto &data = (WriteParquetOperatorData &)*operator_state;

	DataChunk poutput;
	poutput.initAndReference(output, bind_data.colsIdx_);
	data.chunkCollection_.append(poutput);
}


void writeParquetCombine(ClientContext &context, const FunctionData *bind_data_p, FunctionOperatorData *operator_state) {
	auto &bind_data = (WriteParquetData &)*bind_data_p;
	auto &data = (WriteParquetOperatorData &)*operator_state;

	data.writer_->flush(data.chunkCollection_);
}

void writeParquetFinalize(ClientContext &context, const FunctionData *bind_data_p) {
	auto &bind_data = (WriteParquetData &)*bind_data_p;
	auto separator = bind_data.fs.getFileSeparator();

	// finalize all the writers
	for (auto& [file, writer]: bind_data.filesWriter_)
		writer->finalize();


	if (bind_data.overwrite_) {
		// find and remove all the parquet files before writing the new one
		vector<string> oldFiles;
		if (!bind_data.isSingleFile_)
			oldFiles = bind_data.fs.glob(bind_data.folder_ + separator +"**"+ separator+"*"+bind_data.extension_);
		else
			oldFiles.push_back(bind_data.singleFile_+bind_data.extension_);

		for (auto& file : oldFiles)
			if (bind_data.fs.fileExists(file))
				bind_data.fs.removeFile(file);

	}
	// move the tmp data to the final extension
	for (auto& [key, _]: bind_data.filesWriter_) {
		string target(key.c_str(), key.size() - bind_data.tmpExtension_.size());
		target += bind_data.extension_;
		bind_data.fs.moveFile(key, target);
	}
}


static void writeParquetAddNamedParameters(PredFunction &table_function) {
	table_function.namedParameters_["columns"] = PhysicalType::STRING;
	table_function.namedParameters_["codec"] = PhysicalType::STRING;
	table_function.namedParameters_["mode"] = PhysicalType::STRING;
	table_function.namedParameters_["single_file"] = PhysicalType::UTINYINT;
}

string WriteParquetFunc::getName() {
	return "&write_parquet";
}

function_ptr_t WriteParquetFunc::createFunction(const vector<LogicalType> &type) {
	string name =getName();
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {PhysicalType::STRING}, writeParquetFunction, writeParquetBind, writeParquetInit, nullptr, writeParquetFinalize, writeParquetCombine));
	writeParquetAddNamedParameters((PredFunction&)*fun);
	return fun;
}

void WriteParquetFunc::registerFunction(FunctionRegister &funcRegister) {
	std::unique_ptr<FunctionGenerator> fg = std::make_unique<WriteParquetFunc>();
	funcRegister.registerFunctionGen(fg);
}

}
