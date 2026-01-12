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
#include <uuid/uuid.h>

#include "bumblebee/function/predicate/WriteCsv.hpp"

#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{

static string getUUID() {
	uuid_t uuid;
	uuid_generate(uuid); // Generate UUID

	char uuid_str[37]; // 36 chars + null terminator
	uuid_unparse(uuid, uuid_str);

	string uuid_string(uuid_str);
	return uuid_string;
}

static bool requiresQuotes(WriteCSVData &data, const char *str, idx_t len) {

	// simple CSV: check for newlines, quotes and delimiter all at once
	for (idx_t i = 0; i < len; i++) {
		if (str[i] == '\n' || str[i] == '\r' || str[i] == data.quote_[0] || str[i] == data.delimiter_[0]) {
			// newline, write a quoted string
			return true;
		}
	}
	// no newline, quote or delimiter in the string
	// no quoting or escaping necessary
	return false;
}

static string addEscapes(string &to_be_escaped, const string &escape, const string &val) {
	idx_t i = 0;
	string new_val = "";
	idx_t found = val.find(to_be_escaped);

	while (found != string::npos) {
		while (i < found) {
			new_val += val[i];
			i++;
		}
		new_val += escape;
		found = val.find(to_be_escaped, found + escape.length());
	}
	while (i < val.length()) {
		new_val += val[i];
		i++;
	}
	return new_val;
}


static void writeQuotedString(Serializer &serializer, WriteCSVData &data, const char *str, idx_t len) {
	bool force_quote = requiresQuotes(data, str, len);
	if (!force_quote) {
		serializer.writeData((const_data_ptr_t)str, len);
		return;
	}

	// quoting is enabled: we might need to escape things in the string
	bool requires_escape = false;
	// simple CSV
	// do a single loop to check for a quote or escape value
	for (idx_t i = 0; i < len; i++) {
		if (str[i] == data.quote_[0] || str[i] == data.escape_[0]) {
			requires_escape = true;
			break;
		}
	}

	if (!requires_escape) {
		// fast path: no need to escape anything
		serializer.writeBufferData(data.quote_);
		serializer.writeData((const_data_ptr_t)str, len);
		serializer.writeBufferData(data.quote_);
		return;
	}

	// slow path: need to add escapes
	string new_val(str, len);
	new_val = addEscapes(data.escape_, data.escape_, new_val);
	if (data.escape_ != data.quote_) {
		// need to escape quotes separately
		new_val = addEscapes(data.quote_, data.escape_, new_val);
	}
	serializer.writeBufferData(data.quote_);
	serializer.writeBufferData(new_val);
	serializer.writeBufferData(data.quote_);
}


static void parseColumns(const string& partitionString, vector<string>& partitions) {

	partitions.clear();
	std::stringstream ss(partitionString);
	string token;

	while (getline(ss, token, ',')) { // Split by ';'
		if (token.empty()) continue;
		partitions.push_back(token);
	}
}

string WriteCSVData::getHeader() {
	string header = "";
	for (idx_t i =0;i < colNames_.size();i++) {
		if (i > 0) header += delimiter_;
		header += colNames_[i];
	}
	return header;
}

string WriteCSVData::getFileToWrite(const vector<string>& partitionValues) {
	auto separator = fs.getFileSeparator();

	// return a new file based on thread id
	auto id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
	lock_guard lock(mutex_);

	string filename = getUUID() + "_" + id + tmpExtension_;
	auto file = folder_ + separator + filename;
	if (isSingleFile_) {
		// single file change the id and the file pointing to the same file
		id = singleFile_;
		file = singleFile_ + tmpExtension_;
	}
	if (!partitions_.empty()) {
		file = folder_ + separator;
		for (idx_t i =0;i < partitionValues.size();i++) {
			file +=  colNames_[partitions_[i]] + "=" + partitionValues[i] + separator;
			if (!fs.directoryExists(file))
				fs.createDirectory(file);
		}
		file += filename;
		id = file;
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
	filesHandler_[file] = fs.openFile(file, FileFlags::FILE_FLAGS_WRITE | FileFlags::FILE_FLAGS_FILE_CREATE_NEW,
							 FileLockType::WRITE_LOCK, FileSystem::DEFAULT_COMPRESSION);

	// write the header if or overwrite is true or the mode is append but the file does not exist
	if (header_) {
		// add the header as first line
		string header = getHeader() + newline_;
		fs.write(*filesHandler_[file], (void *)header.c_str(), header.size());
	}
	return file;
}



void WriteCSVData::writeDataToFile(const string &file, const_data_ptr_t data, idx_t size) {
	BB_ASSERT(lockFiles_.contains(file));
	BB_ASSERT(filesHandler_.contains(file));
	BB_ASSERT(lockFiles_[file]);
	BB_ASSERT(filesHandler_[file]);
	lock_guard flock(*(lockFiles_[file]));

	fs.write(*(filesHandler_[file]), (void *)data, size);
}

static function_data_ptr_t writeCSVBind(ClientContext &context,
                                        vector<Value> &inputs,
                                        vector<LogicalType> & inputTypes,
                                        std::unordered_map<string, Value> &parameters,
                                        vector<LogicalType> &returnTypes,vector<string> &names,
                                        vector<Expression>& filters) {

	auto result = std::make_unique<WriteCSVData>(*context.fileSystem_);

	BB_ASSERT(inputs.size() == 1);
	BB_ASSERT(!names.empty());
	BB_ASSERT(!returnTypes.empty());
	string filePattern = inputs[0].toString();

	result->folder_ = filePattern;

	vector<string> partitions;
	for (auto &kv : parameters) {
		if (kv.first == "sep" || kv.first == "delim") {
			result->delimiter_ = kv.second.toString();
			result->hasDelimiter_ = true;
			if (result->delimiter_.size() > 1)
				ErrorHandler::errorNotImplemented("Error, multi bytes delimiter currently not supported :(.");
		} else if (kv.first == "header") {
			result->header_ = kv.second.getValueUnsafe<uint8_t>();
			result->hasHeader_ = true;
		} else if (kv.first == "quote") {
			result->quote_ = kv.second.toString();
			result->hasQuote_ = true;
			if (result->quote_.size() > 1)
				ErrorHandler::errorNotImplemented("Error, multi bytes quote currently not supported :(.");
		} else if (kv.first == "escape") {
			result->escape_ = kv.second.toString();
			result->hasEscape_ = true;
			if (result->escape_.size() > 1)
				ErrorHandler::errorNotImplemented("Error, multi bytes escape currently not supported :(.");

		} else if (kv.first == "mode") {
			if (StringUtils::lower( kv.second.toString()) == "overwrite")
				result->overwrite_ = true;
			else if (StringUtils::lower( kv.second.toString()) == "append")
				result->overwrite_ = false;
			else
				LOG_WARNING("Warning value: %s is not a valid mode. Available mode: [overwrite, append]", kv.second.toString().c_str());

		} else if (kv.first == "partitions") {
			parseColumns(kv.second.toString(), partitions);
		} else if (kv.first == "columns") {
			parseColumns(kv.second.toString(), result->colNames_);
		}else if (kv.first == "single_file") {
			result->isSingleFile_ = kv.second.getValueUnsafe<uint8_t>();
			result->singleFile_ = filePattern;
			// if ends with the extension remove the extensions as we will add it
			if (result->singleFile_.ends_with(result->extension_))
				result->singleFile_.erase(result->singleFile_.size() - result->extension_.size());
		}
	}

	if (result->isSingleFile_ && !result->partitions_.empty())
		ErrorHandler::errorParsing("Error, you cannot set single file and partitions :( .");


	if (!result->isSingleFile_ && !context.fileSystem_->directoryExists(result->folder_))
		context.fileSystem_->createDirectory(result->folder_);


	if (result->colNames_.empty())
		//set the col names the variables
		result->colNames_ = names;

	if (result->colNames_.size() != names.size()) {
		ErrorHandler::errorParsing("Error, names from columns configuration does not match number of variables in external atom!");
	}

	if (!partitions.empty()) {
		for (auto& col: partitions) {
			int index = -1;
			for (idx_t i = 0; i < names.size() && index < 0; i++) {
				if (names[i] == col)
					index = i;
			}
			if (index == -1) {
				ErrorHandler::errorParsing("Error partition column "+col+" does not exist.");
			}
			result->partitions_.push_back(index);
		}
	}

	return std::move(result);
}


static function_op_data_ptr_t writeCSVInit(ClientContext &context, const FunctionData *bind_data_p) {

	auto &bind_data = (WriteCSVData &)*bind_data_p;

	auto result = std::make_unique<WriteCSVOperatorData>();

	result->file_ = bind_data.getFileToWrite();

	// set all the cols as string
	vector<PhysicalType> types (bind_data.colNames_.size(), PhysicalType::STRING);
	result->chunk_.initialize(types);

	return std::move(result);
}

static void writeChunk(WriteCSVData &bind_data, WriteCSVOperatorData &data) {
	auto& writer = data.serializer_;
	// now loop over the vectors and output the values
	for (idx_t row_idx = 0; row_idx < data.chunk_.getSize(); row_idx++) {
		for (idx_t col_idx = 0; col_idx < data.chunk_.columnCount(); col_idx++) {
			if (col_idx != 0) {
				writer.writeBufferData(bind_data.delimiter_);
			}

			// non-null value, fetch the string value from the cast chunk
			auto str_data = FlatVector::getData<string_t>(data.chunk_.data_[col_idx]);
			auto& str_value = str_data[row_idx];
			writeQuotedString(writer, bind_data, str_value.getDataUnsafe(), str_value.size());
		}
		writer.writeBufferData(bind_data.newline_);
	}
	// check if we should flush what we have currently written
	if (writer.blob_.size_ >= data.flushSize_) {
		bind_data.writeDataToFile(data.file_, writer.blob_.data_.get(), writer.blob_.size_);
		writer.reset();
	}
}

static void writeChunkWithPartitions(WriteCSVData &bind_data, WriteCSVOperatorData &data) {
	// now loop over the vectors and output the values
	for (idx_t row_idx = 0; row_idx < data.chunk_.getSize(); row_idx++) {
		// first calculate the partition
		vector<string> partitionValues;
		for (idx_t p_idx = 0; p_idx < bind_data.partitions_.size(); p_idx++) {
			auto col_idx = bind_data.partitions_[p_idx];
			auto str_data = FlatVector::getData<string_t>(data.chunk_.data_[col_idx]);
			auto& str_value = str_data[row_idx];
			partitionValues.emplace_back(str_value.getDataUnsafe());
		}

		auto file = bind_data.getFileToWrite(partitionValues);
		auto& writer = data.partitionSerializers_[file];
		for (idx_t col_idx = 0; col_idx < data.chunk_.columnCount(); col_idx++) {
			if (col_idx != 0) {
				writer.writeBufferData(bind_data.delimiter_);
			}

			// non-null value, fetch the string value from the cast chunk
			auto str_data = FlatVector::getData<string_t>(data.chunk_.data_[col_idx]);
			auto& str_value = str_data[row_idx];
			writeQuotedString(writer, bind_data, str_value.getDataUnsafe(), str_value.size());
		}
		writer.writeBufferData(bind_data.newline_);
		if (writer.blob_.size_ >= data.flushSize_) {
			bind_data.writeDataToFile(data.file_, writer.blob_.data_.get(), writer.blob_.size_);
			writer.reset();
		}
	}

}


static void writeCSVFunction(ClientContext &context, const FunctionData *bind_data_p,
                             FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (WriteCSVData &)*bind_data_p;
	auto &data = (WriteCSVOperatorData &)*operator_state;


	data.chunk_.setCardinality(output.getSize());
	for (idx_t col_idx = 0; col_idx < output.columnCount(); col_idx++) {
		if (output.data_[col_idx].getType() == PhysicalType::STRING) {
			// STRING, just create a reference
			data.chunk_.data_[col_idx].reference(output.data_[col_idx]);
		} else {
			// non varchar column, perform the cast
			VectorOperations::cast( output.data_[col_idx], data.chunk_.data_[col_idx], output.getSize());
		}
	}
	data.chunk_.normalify();
	if (bind_data.partitions_.empty())
		writeChunk(bind_data, data);
	else
		writeChunkWithPartitions(bind_data, data);

}


void writeCSVCombine(ClientContext &context, const FunctionData *bind_data_p, FunctionOperatorData *operator_state) {
	auto &bind_data = (WriteCSVData &)*bind_data_p;
	auto &data = (WriteCSVOperatorData &)*operator_state;
	auto& writer = data.serializer_;

	// write data in the buffer
	// check if we should flush what we have currently written

	if (bind_data.partitions_.empty() && writer.blob_.size_ > 0) {
		bind_data.writeDataToFile(data.file_, writer.blob_.data_.get(), writer.blob_.size_);
		writer.reset();
		return;
	}
	// check the partitions buffer
	for (auto& [file, w]: data.partitionSerializers_)
		if (w.blob_.size_ > 0) {
			bind_data.writeDataToFile(file, w.blob_.data_.get(), w.blob_.size_);
			w.reset();
		}
}

void writeCSVFinalize(ClientContext &context, const FunctionData *bind_data_p) {
	auto &bind_data = (WriteCSVData &)*bind_data_p;
	auto& fs = bind_data.fs;
	auto separator = bind_data.fs.getFileSeparator();

	if (bind_data.overwrite_) {
		// find and remove all the csv files before writing the new one
		vector<string> oldFiles;
		if (!bind_data.isSingleFile_)
			oldFiles = bind_data.fs.glob(bind_data.folder_ + separator +"**"+ separator+"*"+bind_data.extension_);
		else
			oldFiles.push_back(bind_data.singleFile_+bind_data.extension_);

		for (auto& file : oldFiles)
			if (bind_data.fs.fileExists(file))
				bind_data.fs.removeFile(file);

		// move the tmp data to the final extension
		for (auto& [key, _]: bind_data.filesHandler_) {
			string target(key.c_str(), key.size() - bind_data.tmpExtension_.size());
			target += bind_data.extension_;
			fs.moveFile(key, target);
		}
		return;
	}

	// we need to append the data
	for (auto& [key, fh]: bind_data.filesHandler_) {
		string target(key.c_str(), key.size() - bind_data.tmpExtension_.size());
		target += bind_data.extension_;
		if (!fs.fileExists(target)) {
			// file does not exist so just rename it
			fs.moveFile(key, target);
			continue;
		}
		auto targetFh = fs.openFile(target, FileFlags::FILE_FLAGS_APPEND | FileFlags::FILE_FLAGS_WRITE,
							 FileLockType::WRITE_LOCK, FileSystem::DEFAULT_COMPRESSION);

		auto buffer = std::unique_ptr<char[]>(new char[bind_data.initial_buff_size]);
		// open the file as read
		auto readFh = fs.openFile(key, FileFlags::FILE_FLAGS_READ,
							 FileLockType::READ_LOCK, FileSystem::DEFAULT_COMPRESSION);
		if (bind_data.header_) {
			// we need to skip the first line
			idx_t read_count = readFh->read(buffer.get(), bind_data.initial_buff_size);
			if (read_count > 0) {
				auto pos = std::find(buffer.get(), buffer.get()+read_count, bind_data.newline_.c_str()[0]);
				BB_ASSERT(pos);
				idx_t index = pos - buffer.get() + 1;
				targetFh->write(buffer.get() + index  , read_count - index);
			}
		}
		while (true) {
			idx_t read_count = readFh->read(buffer.get(), bind_data.initial_buff_size);
			if (read_count == 0) break;
			targetFh->write(buffer.get(), read_count);
		}
		fs.removeFile(key);
	}
}


static void writeCSVAddNamedParameters(PredFunction &table_function) {
	table_function.namedParameters_["sep"] = PhysicalType::STRING;
	table_function.namedParameters_["delim"] = PhysicalType::STRING;
	table_function.namedParameters_["quote"] = PhysicalType::STRING;
	table_function.namedParameters_["escape"] = PhysicalType::STRING;
	table_function.namedParameters_["columns"] = PhysicalType::STRING;
	table_function.namedParameters_["header"] = PhysicalType::UTINYINT;
	table_function.namedParameters_["partitions"] = PhysicalType::STRING;
	table_function.namedParameters_["single_file"] = PhysicalType::UTINYINT;
}


function_ptr_t WriteCsvFunc::getFunction() {
	string name = "&write_csv";
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {PhysicalType::STRING}, writeCSVFunction, writeCSVBind, writeCSVInit, nullptr, writeCSVFinalize, writeCSVCombine));
	writeCSVAddNamedParameters((PredFunction&)*fun);
	return fun;
}

void WriteCsvFunc::registerFunction(FunctionRegister &funcRegister) {
	funcRegister.registerFunction(getFunction());
}

}
