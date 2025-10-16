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
#include "bumblebee/common/BufferedCSVReader.hpp"

#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include  "utf8proc/utf8proc.hpp"
#include  "utf8proc/utf8proc_wrapper.hpp"

namespace bumblebee{


static string getLineNumberStr(idx_t linenr, bool linenr_estimated) {
	string estimated = (linenr_estimated ? string(" (estimated)") : string(""));
	return std::to_string(linenr + 1) + estimated;
}

// Helper function to generate column names
static string generateColumnName(const idx_t total_cols, const idx_t col_number, const string &prefix = "column") {
	string value = std::to_string(col_number);
	return string(prefix + value);
}


// Helper function for UTF-8 aware space trimming
static string trimWhitespace(const string &col_name) {
	utf8proc_int32_t codepoint;
	auto str = reinterpret_cast<const utf8proc_uint8_t *>(col_name.c_str());
	idx_t size = col_name.size();
	// Find the first character that is not left trimmed
	idx_t begin = 0;
	while (begin < size) {
		auto bytes = utf8proc_iterate(str + begin, size - begin, &codepoint);
		BB_ASSERT(bytes > 0);
		if (utf8proc_category(codepoint) != UTF8PROC_CATEGORY_ZS) {
			break;
		}
		begin += bytes;
	}

	// Find the last character that is not right trimmed
	idx_t end;
	end = begin;
	for (auto next = begin; next < col_name.size();) {
		auto bytes = utf8proc_iterate(str + next, size - next, &codepoint);
		BB_ASSERT(bytes > 0);
		next += bytes;
		if (utf8proc_category(codepoint) != UTF8PROC_CATEGORY_ZS) {
			end = next;
		}
	}

	// return the trimmed string
	return col_name.substr(begin, end - begin);
}



static string normalizeColumnName(const string &col_name) {
	// normalize UTF8 characters to NFKD
	auto nfkd = utf8proc_NFKD((const utf8proc_uint8_t *)col_name.c_str(), col_name.size());
	const string col_name_nfkd = string((const char *)nfkd, strlen((const char *)nfkd));
	free(nfkd);

	// only keep ASCII characters 0-9 a-z A-Z and replace spaces with regular whitespace
	string col_name_ascii = "";
	for (idx_t i = 0; i < col_name_nfkd.size(); i++) {
		if (col_name_nfkd[i] == '_' || (col_name_nfkd[i] >= '0' && col_name_nfkd[i] <= '9') ||
			(col_name_nfkd[i] >= 'A' && col_name_nfkd[i] <= 'Z') ||
			(col_name_nfkd[i] >= 'a' && col_name_nfkd[i] <= 'z')) {
			col_name_ascii += col_name_nfkd[i];
			} else if (StringUtils::characterIsSpace(col_name_nfkd[i])) {
				col_name_ascii += " ";
			}
	}

	// trim whitespace and replace remaining whitespace by _
	string col_name_trimmed = trimWhitespace(col_name_ascii);
	string col_name_cleaned = "";
	bool in_whitespace = false;
	for (idx_t i = 0; i < col_name_trimmed.size(); i++) {
		if (col_name_trimmed[i] == ' ') {
			if (!in_whitespace) {
				col_name_cleaned += "_";
				in_whitespace = true;
			}
		} else {
			col_name_cleaned += col_name_trimmed[i];
			in_whitespace = false;
		}
	}

	// don't leave string empty; if not empty, make lowercase
	if (col_name_cleaned.empty()) {
		col_name_cleaned = "_";
	}
	// make upper case
	col_name_cleaned = StringUtils::upper(col_name_cleaned);

	return col_name_cleaned;
}


TextSearchShiftArray::TextSearchShiftArray() : length_(0){
}

TextSearchShiftArray::TextSearchShiftArray(string search_term) : length_(search_term.size()) {
    if (length_ > 255) {
        ErrorHandler::errorNotImplemented("Size of delimiter/quote/escape in CSV reader is limited to 255 bytes");
    }
    // initialize the shifts array
    shifts_ = std::unique_ptr<uint8_t[]>(new uint8_t[length_ * 255]);
    memset(shifts_.get(), 0, length_ * 255 * sizeof(uint8_t));
    // iterate over each of the characters in the array
    for (idx_t main_idx = 0; main_idx < length_; main_idx++) {
        uint8_t current_char = (uint8_t)search_term[main_idx];
        // now move over all the remaining positions
        for (idx_t i = main_idx; i < length_; i++) {
            bool is_match = true;
            // check if the prefix matches at this position
            // if it does, we move to this position after encountering the current character
            for (idx_t j = 0; j < main_idx; j++) {
                if (search_term[i - main_idx + j] != search_term[j]) {
                    is_match = false;
                }
            }
            if (!is_match) {
                continue;
            }
            shifts_[i * 255 + current_char] = main_idx + 1;
        }
    }
}

BufferedCSVReader::BufferedCSVReader(ClientContext &context, BufferedCSVReaderOptions options,
    const vector<ConstantType> &requested_types): BufferedCSVReader(*context.fileSystem_, options, requested_types){}

BufferedCSVReader::BufferedCSVReader(FileSystem &fs, BufferedCSVReaderOptions options,
const vector<ConstantType> &requested_types) : fs_(fs), options_(options), bufferSize_(0), position_(0) {
	fileHandle_ = openCSV(options_);
	initialize(requested_types);
}

void BufferedCSVReader::parseCSV(DataChunk &insertChunk) {
	// if no auto-detect or auto-detect with jumping samples, we have nothing cached and start from the beginning
	if (cached_chunks.empty()) {
		cachedBuffers_.clear();
	} else {
		auto &chunk = cached_chunks.front();
		parseChunk_.destroy();
		parseChunk_.initializeEmpty(chunk->getTypes());
		parseChunk_.reference(*chunk);
		chunk->destroy();
		cached_chunks.pop();
		flush(insertChunk);
		return;
	}

	string error_message;
	if (!tryParseCSV(ParserMode::PARSING, insertChunk, error_message)) {
		ErrorHandler::errorParsing(error_message);
	}
}

void BufferedCSVReader::initialize(const vector<ConstantType> &requested_types) {
    prepareComplexParser();
    if (options_.autoDetect_) {
        types_ = sniffCSV(requested_types);
        if (types_.empty()) {
            ErrorHandler::errorNotImplemented("Failed to detect column types from CSV: is the file a valid CSV file?");
        }
        if (cached_chunks.empty()) {
            jumpToBeginning(options_.skipRows_, options_.header_);
        }
    } else {
        types_ = requested_types;
        resetBuffer();
        skipRowsAndReadHeader(options_.skipRows_, options_.header_);
    }
    initParseChunk(types_.size());
}

void BufferedCSVReader::initParseChunk(idx_t num_cols) {

	if (num_cols == parseChunk_.columnCount()) {
		parseChunk_.reset();
	} else {
		parseChunk_.destroy();

		// initialize the parse_chunk with a set of VARCHAR types
		vector<ConstantType> varchar_types(num_cols, ConstantType::STRING);
		parseChunk_.initialize(varchar_types);
	}

}

void BufferedCSVReader::prepareComplexParser() {
    delimiterSearch_ = TextSearchShiftArray(options_.delimiter_);
    escapeSearch_ = TextSearchShiftArray(options_.escape_);
    quoteSearch_ = TextSearchShiftArray(options_.quote_);

}

void BufferedCSVReader::parseCSV(ParserMode mode) {
	DataChunk dummy_chunk;
	string error_message;
	if (!tryParseCSV(mode, dummy_chunk, error_message)) {
		ErrorHandler::errorParsing(error_message);
	}
}

bool BufferedCSVReader::tryParseCSV(ParserMode mode) {
	DataChunk dummy_chunk;
	string error_message;
	return tryParseCSV(mode, dummy_chunk, error_message);
}

bool BufferedCSVReader::tryParseCSV(ParserMode parser_mode, DataChunk &insert_chunk, string &error_message) {
	mode_ = parser_mode;

	if (options_.quote_.size() <= 1 && options_.escape_.size() <= 1 && options_.delimiter_.size() == 1) {
		return tryParseSimpleCSV(insert_chunk, error_message);
	} else {
		return tryParseComplexCSV(insert_chunk, error_message);
	}
}

vector<ConstantType> BufferedCSVReader::sniffCSV(const vector<ConstantType> &requested_types) {

	// #######
	// ### dialect detection
	// #######
	BufferedCSVReaderOptions original_options = options_;
	vector<BufferedCSVReaderOptions> info_candidates;
	idx_t best_num_cols = 0;

	detectDialect(requested_types, original_options, info_candidates, best_num_cols);

	// if no dialect candidate was found, then file was most likely empty and we throw an exception
	if (info_candidates.empty()) {
		ErrorHandler::errorNotImplemented("Error in file"+options_.filePath_+": CSV options could not be auto-detected. Consider setting parser options manually.");
	}

	// #######
	// ### type detection (initial)
	// #######
	// type candidates, ordered by descending specificity (~ from high to low)
	vector<ConstantType> type_candidates = {
	    ConstantType::STRING,
	    ConstantType::DOUBLE,
	    ConstantType::FLOAT,
	    ConstantType::BIGINT,
	    ConstantType::INTEGER
	};

	vector<vector<ConstantType>> best_sql_types_candidates;
	DataChunk best_header_row;
	detectCandidateTypes(type_candidates, info_candidates, original_options, best_num_cols,
	                     best_sql_types_candidates, best_header_row);

	// #######
	// ### header detection
	// #######
	detectHeader(best_sql_types_candidates, best_header_row);

	// #######
	// ### type detection (refining)
	// #######
	return refineTypeDetection(type_candidates, requested_types, best_sql_types_candidates);

}

bool BufferedCSVReader::tryCastValue(const Value &value, const ConstantType &sql_type) {
	Value new_value;
	string error_message;
	return value.tryCastAs(sql_type, new_value, &error_message);
}

bool BufferedCSVReader::tryCastVector(Vector &parse_chunk_col, idx_t size, const ConstantType &sql_type) {
	if (parse_chunk_col.getType() == sql_type) {
		return true;
	}
	// try vector-cast from string to sql_type
	Vector dummy_result(sql_type);

	// target type is not varchar: perform a cast
	string error_message;
	return VectorOperations::tryCast(parse_chunk_col, dummy_result, size, &error_message);

}

void BufferedCSVReader::skipRowsAndReadHeader(idx_t skip_rows, bool skip_header) {
	for (idx_t i = 0; i < skip_rows; i++) {
		// ignore skip rows
		string read_line = fileHandle_->readLine();
		linenr_++;
	}

	if (skip_header) {
		// ignore the first line as a header line
		initParseChunk(types_.size());
		parseCSV(ParserMode::PARSING_HEADER);
	}
}

void BufferedCSVReader::jumpToBeginning(idx_t skip_rows, bool skip_header) {
	resetBuffer();
	resetStream();
	skipRowsAndReadHeader(skip_rows, skip_header);
	sampleChunkIdx_ = 0;
	avgBytesInChunk_ = (avgBytesInChunk_ +bytesInChunk_)/2;
	bytesInChunk_ = 0;
	endOfFileReached_ = false;
	bomChecked_ = false;
}

bool BufferedCSVReader::jumpToNextSample() {
	// get bytes contained in the previously read chunk
	idx_t remaining_bytes_in_buffer = bufferSize_ - start_;
	bytesInChunk_ -= remaining_bytes_in_buffer;
	if (remaining_bytes_in_buffer == 0) {
		return false;
	}

	// assess if it makes sense to jump, based on size of the first chunk relative to size of the entire file
	if (sampleChunkIdx_ == 0) {
		idx_t bytes_first_chunk = bytesInChunk_;
		double chunks_fit = ((double)fileSize_ / (double)bytes_first_chunk);
		jumpingSamples_ = chunks_fit >= options_.sampleChunks_;

		// jump back to the beginning
		jumpToBeginning(options_.skipRows_, options_.header_);
		sampleChunkIdx_++;
		return true;
	}

	if (endOfFileReached_ || sampleChunkIdx_ >= options_.sampleChunks_) {
		return false;
	}

	// if we deal with any other sources than plaintext files, jumping_samples can be tricky. In that case
	// we just read x continuous chunks from the stream TODO: make jumps possible for zipfiles.
	if (!plainFileSource_ || !jumpingSamples_) {
		sampleChunkIdx_++;
		return true;
	}

	// update average bytes per line
	double bytes_per_line = (double)bytesInChunk_ / (double)options_.sampleChunkSize_;
	bytesPerLineAvg_ = ((bytesPerLineAvg_ * (sampleChunkIdx_)) + bytes_per_line) / (sampleChunkIdx_ + 1);

	// if none of the previous conditions were met, we can jump
	auto partition_size = (idx_t)round(fileSize_ / (double)options_.sampleChunks_);

	// calculate offset to end of the current partition
	int64_t offset = partition_size - bytesInChunk_ - remaining_bytes_in_buffer;
	auto current_pos = fileHandle_->seekPosition();

	if (current_pos + offset < fileSize_) {
		// set position in stream and clear failure bits
		fileHandle_->seek(current_pos + offset);

		// estimate linenr
		linenr_ += (idx_t)round((offset + remaining_bytes_in_buffer) / bytesPerLineAvg_);
		linenrEstimated_ = true;
	} else {
		// seek backwards from the end in last chunk and hope to catch the end of the file
		// TODO: actually it would be good to make sure that the end of file is being reached, because
		// messy end-lines are quite common. For this case, however, we first need a skip_end detection anyways.
		fileHandle_->seek(fileSize_ - bytesInChunk_);

		// estimate linenr
		linenr_ = (idx_t)round((fileSize_ - bytesInChunk_) / bytesPerLineAvg_);
		linenrEstimated_ = true;
	}

	// reset buffers and parse chunk
	resetBuffer();

	// seek beginning of next line
	// FIXME: if this jump ends up in a quoted linebreak, we will have a problem
	string read_line = fileHandle_->readLine();
	linenr_++;

	sampleChunkIdx_++;

	return true;
}

void BufferedCSVReader::resetBuffer() {
	buffer_.reset();
	bufferSize_ = 0;
	position_ = 0;
	start_ = 0;
	cachedBuffers_.clear();
}

void BufferedCSVReader::resetStream() {
	if (!fileHandle_->canSeek()) {
		// seeking to the beginning appears to not be supported in all compiler/os-scenarios,
		// so we have to create a new stream source here for now
		fileHandle_->reset();
	} else {
		fileHandle_->seek(0);
	}
	linenr_ = 0;
	linenrEstimated_ = false;
	bytesPerLineAvg_ = 0;
	sampleChunkIdx_ = 0;
	jumpingSamples_ = false;
}

bool BufferedCSVReader::tryParseSimpleCSV(DataChunk &insert_chunk, string &error_message) {
	// used for parsing algorithm
	bool finished_chunk = false;
	idx_t column = 0;
	idx_t offset = 0;
	vector<idx_t> escape_positions;

	// read values into the buffer (if any)
	if (position_ >= bufferSize_) {
		if (!readBuffer(start_)) {
			return true;
		}
	}
	// start parsing the first value
	goto value_start;
value_start:
	offset = 0;
	/* state: value_start */
	// this state parses the first character of a value
	if (buffer_[position_] == options_.quote_[0]) {
		// quote: actual value starts in the next position
		// move to in_quotes state
		start_ = position_ + 1;
		goto in_quotes;
	} else {
		// no quote, move to normal parsing state
		start_ = position_;
		goto normal;
	}
normal:
	/* state: normal parsing state */
	// this state parses the remainder of a non-quoted value until we reach a delimiter or newline
	do {
		for (; position_ < bufferSize_; position_++) {
			if (buffer_[position_] == options_.delimiter_[0]) {
				// delimiter: end the value and add it to the chunk
				goto add_value;
			} else if (StringUtils::characterIsNewline(buffer_[position_])) {
				// newline: add row
				goto add_row;
			}
		}
	} while (readBuffer(start_));
	// file ends during normal scan: go to end state
	goto final_state;
add_value:
	addValue(buffer_.get() + start_, position_ - start_ - offset, column, escape_positions);
	// increase position by 1 and move start to the new position
	offset = 0;
	start_ = ++position_;
	if (position_ >= bufferSize_ && !readBuffer(start_)) {
		// file ends right after delimiter, go to final state
		goto final_state;
	}
	goto value_start;
add_row : {
	// check type of newline (\r or \n)
	bool carriage_return = buffer_[position_] == '\r';
	addValue(buffer_.get() + start_, position_ - start_ - offset, column, escape_positions);
	finished_chunk = addRow(insert_chunk, column);
	// increase position by 1 and move start to the new position
	offset = 0;
	start_ = ++position_;
	if (position_ >= bufferSize_ && !readBuffer(start_)) {
		// file ends right after delimiter, go to final state
		goto final_state;
	}
	if (carriage_return) {
		// \r newline, go to special state that parses an optional \n afterwards
		goto carriage_return;
	} else {
		// \n newline, move to value start
		if (finished_chunk) {
			return true;
		}
		goto value_start;
	}
}
in_quotes:
	/* state: in_quotes */
	// this state parses the remainder of a quoted value
	position_++;
	do {
		for (; position_ < bufferSize_; position_++) {
			if (buffer_[position_] == options_.quote_[0]) {
				// quote: move to unquoted state
				goto unquote;
			} else if (buffer_[position_] == options_.escape_[0]) {
				// escape: store the escaped position and move to handle_escape state
				escape_positions.push_back(position_ - start_);
				goto handle_escape;
			}
		}
	} while (readBuffer(start_));
	// still in quoted state at the end of the file, error:
	error_message = "Error in file"+options_.filePath_+ " on line" + getLineNumberStr(linenr_, linenrEstimated_) +" : unterminated quotes. ";
	return false;

unquote:
	/* state: unquote */
	// this state handles the state directly after we unquote
	// in this state we expect either another quote (entering the quoted state again, and escaping the quote)
	// or a delimiter/newline, ending the current value and moving on to the next value
	position_++;
	if (position_ >= bufferSize_ && !readBuffer(start_)) {
		// file ends right after unquote, go to final state
		offset = 1;
		goto final_state;
	}
	if (buffer_[position_] == options_.quote_[0] && (options_.escape_.empty() || options_.escape_[0] == options_.quote_[0])) {
		// escaped quote, return to quoted state and store escape position
		escape_positions.push_back(position_ - start_);
		goto in_quotes;
	} else if (buffer_[position_] == options_.delimiter_[0]) {
		// delimiter, add value
		offset = 1;
		goto add_value;
	} else if (StringUtils::characterIsNewline(buffer_[position_])) {
		offset = 1;
		goto add_row;
	} else {
		error_message = "Error in file"+options_.filePath_+ " on line" +
			getLineNumberStr(linenr_, linenrEstimated_) +" quote should be followed by end of value, end of "
		    "row or another quote.";
		return false;
	}
handle_escape:
	/* state: handle_escape */
	// escape should be followed by a quote or another escape character
	position_++;
	if (position_ >= bufferSize_ && !readBuffer(start_)) {
		error_message = "Error in file"+options_.filePath_+ " on line" +
			getLineNumberStr(linenr_, linenrEstimated_) +
				": neither QUOTE nor ESCAPE is proceeded by ESCAPE.";
		return false;
	}
	if (buffer_[position_] != options_.quote_[0] && buffer_[position_] != options_.escape_[0]) {
		error_message = "Error in file"+options_.filePath_+ " on line" +
						getLineNumberStr(linenr_, linenrEstimated_) +
						" neither QUOTE nor ESCAPE is proceeded by ESCAPE.";
		return false;
	}
	// escape was followed by quote or escape, go back to quoted state
	goto in_quotes;
carriage_return:
	/* state: carriage_return */
	// this stage optionally skips a newline (\n) character, which allows \r\n to be interpreted as a single line
	if (buffer_[position_] == '\n') {
		// newline after carriage return: skip
		// increase position by 1 and move start to the new position
		start_ = ++position_;
		if (position_ >= bufferSize_ && !readBuffer(start_)) {
			// file ends right after delimiter, go to final state
			goto final_state;
		}
	}
	if (finished_chunk) {
		return true;
	}
	goto value_start;
final_state:
	if (finished_chunk) {
		return true;
	}

	if (column > 0 || position_ > start_) {
		// remaining values to be added to the chunk
		addValue(buffer_.get() + start_, position_ - start_ - offset, column, escape_positions);
		finished_chunk = addRow(insert_chunk, column);
	}

	// final stage, only reached after parsing the file is finished
	// flush the parsed chunk and finalize parsing
	if (mode_ == ParserMode::PARSING) {
		flush(insert_chunk);
	}

	endOfFileReached_ = true;
	return true;
}

bool BufferedCSVReader::tryParseComplexCSV(DataChunk &insert_chunk, string &error_message) {
	// TODO Implement complex csv reader
	ErrorHandler::errorNotImplemented("Complex CSV reader not implemented! Please use CSV with one byte delimiter and escape.");
	return false;
}

void BufferedCSVReader::addValue(char *str_val, idx_t length, idx_t &column, vector<idx_t> &escape_positions) {
	if (length == 0 && column == 0) {
		rowEmpty_ = true;
	} else {
		rowEmpty_ = false;
	}

	if (!types_.empty() && column == types_.size() && length == 0) {
		// skip a single trailing delimiter in last column
		return;
	}
	if (mode_ == ParserMode::SNIFFING_DIALECT) {
		column++;
		return;
	}
	if (column >= types_.size()) {
		ErrorHandler::errorParsing("Error on line"+ getLineNumberStr(linenr_, linenrEstimated_) + " : expected" + std::to_string(types_.size()) +" values per row, but got more.");
	}

	// insert the line number into the chunk
	idx_t row_entry = parseChunk_.getSize();

	str_val[length] = '\0';


	auto &v = parseChunk_.data_[column];
	auto parse_data = FlatVector::getData<string_t>(v);
	if (!escape_positions.empty()) {
		// remove escape characters (if any)
		string old_val = str_val;
		string new_val = "";
		idx_t prev_pos = 0;
		for (idx_t i = 0; i < escape_positions.size(); i++) {
			idx_t next_pos = escape_positions[i];
			new_val += old_val.substr(prev_pos, next_pos - prev_pos);

			if (options_.escape_.empty() || options_.escape_ == options_.quote_) {
				prev_pos = next_pos + options_.quote_.size();
			} else {
				prev_pos = next_pos + options_.escape_.size();
			}
		}
		new_val += old_val.substr(prev_pos, old_val.size() - prev_pos);
		escape_positions.clear();
		parse_data[row_entry] = StringVector::addString(v,  string_t(new_val.c_str()));
	} else {
		parse_data[row_entry] = StringVector::addString(v,  string_t(str_val));
	}

	// move to the next column
	column++;
}

bool BufferedCSVReader::addRow(DataChunk &insert_chunk, idx_t &column) {
	linenr_++;

	if (rowEmpty_) {
		rowEmpty_ = false;
		if (types_.size() != 1) {
			column = 0;
			return false;
		}
	}

	if (column < types_.size() && mode_ != ParserMode::SNIFFING_DIALECT) {
		ErrorHandler::errorParsing("Error on line"+ getLineNumberStr(linenr_, linenrEstimated_) + " : expected" + std::to_string(types_.size()) +" values per row, but got more.");
	}

	if (mode_ == ParserMode::SNIFFING_DIALECT) {
		sniffedColumnCounts_.push_back(column);

		if (sniffedColumnCounts_.size() == options_.sampleChunkSize_) {
			return true;
		}
	} else {
		parseChunk_.setCardinality(parseChunk_.getSize() + 1);
	}

	if (mode_ == ParserMode::PARSING_HEADER) {
		return true;
	}

	if (mode_ == ParserMode::SNIFFING_DATATYPES && parseChunk_.getSize() == options_.sampleChunkSize_) {
		return true;
	}

	if (mode_ == ParserMode::PARSING && parseChunk_.getSize() == STANDARD_VECTOR_SIZE) {
		flush(insert_chunk);
		return true;
	}

	column = 0;
	return false;
}

void BufferedCSVReader::flush(DataChunk &insert_chunk) {
	if (parseChunk_.getSize() == 0) {
		return;
	}
	// convert the columns in the parsed chunk to the types of the table
	insert_chunk.setCardinality(parseChunk_);
	for (idx_t col_idx = 0; col_idx < types_.size(); col_idx++) {

		if (types_[col_idx] == ConstantType::STRING) {
			// target type is varchar: no need to convert
			// just test that all strings are valid utf-8 strings
			auto parse_data = FlatVector::getData<string_t>(parseChunk_.data_[col_idx]);
			for (idx_t i = 0; i < parseChunk_.getSize(); i++) {
				auto& s = parse_data[i];
				auto utf_type = Utf8Proc::Analyze(s.getDataUnsafe(), strlen(s.getDataUnsafe()));
				if (utf_type == UnicodeType::INVALID) {
					string col_name = std::to_string(col_idx);
					if (col_idx < colNames_.size()) {
						col_name = "\"" + colNames_[col_idx] + "\"";
					}
					ErrorHandler::errorParsing("Error in file " + options_.filePath_+" between line " + std::to_string(linenr_ - parseChunk_.getSize()) +
						" and "+ std::to_string(linenr_) +" in column "+ col_name+": "
												"file is not valid UTF8.");
				}
			}
			insert_chunk.data_[col_idx].reference(parseChunk_.data_[col_idx]);
		} else {
			string error_message;
			bool success = true;
			if (parseChunk_.data_[col_idx].getType() != insert_chunk.data_[col_idx].getType())
				success = VectorOperations::tryCast(parseChunk_.data_[col_idx], insert_chunk.data_[col_idx],
				                                    parseChunk_.getSize(), &error_message);
			else
				insert_chunk.data_[col_idx].reference(parseChunk_.data_[col_idx]);

			if (!success) {
				string col_name = std::to_string(col_idx);
				if (col_idx < colNames_.size()) {
					col_name = "\"" + colNames_[col_idx] + "\"";
				}

				ErrorHandler::errorParsing("Error: "+error_message+ "; in column "+col_name+" between line "+std::to_string(linenr_ - parseChunk_.getSize() + 1) + " and "+std::to_string(linenr_));
			}
		}
	}
	parseChunk_.reset();
}

bool BufferedCSVReader::readBuffer(idx_t &start) {
	auto old_buffer = std::move(buffer_);

	// the remaining part of the last buffer
	idx_t remaining = bufferSize_ - start;
	idx_t buffer_read_size = INITIAL_BUFFER_SIZE;
	while (remaining > buffer_read_size) {
		buffer_read_size *= 2;
	}
	if (remaining + buffer_read_size > MAXIMUM_CSV_LINE_SIZE) {
		ErrorHandler::errorParsing("Maximum line size of "+ std::to_string(MAXIMUM_CSV_LINE_SIZE)+ " bytes exceeded!");
	}
	buffer_ = std::unique_ptr<char[]>(new char[buffer_read_size + remaining + 1]);
	bufferSize_ = remaining + buffer_read_size;
	if (remaining > 0) {
		// remaining from last buffer: copy it here
		memcpy(buffer_.get(), old_buffer.get() + start, remaining);
	}
	idx_t read_count = fileHandle_->read(buffer_.get() + remaining, buffer_read_size);

	bytesInChunk_ += read_count;
	bufferSize_ = remaining + read_count;
	buffer_[bufferSize_] = '\0';
	if (old_buffer) {
		cachedBuffers_.push_back(std::move(old_buffer));
	}
	start = 0;
	position_ = remaining;
	if (!bomChecked_) {
		bomChecked_ = true;
		if (read_count >= 3 && buffer_[0] == '\xEF' && buffer_[1] == '\xBB' && buffer_[2] == '\xBF') {
			position_ += 3;
		}
	}

	return read_count > 0;
}

file_handler_ptr_t BufferedCSVReader::openCSV(const BufferedCSVReaderOptions &options) {
    compression_ = FileCompressionType::UNCOMPRESSED;
    if (options.compression_ == "infer" || options.compression_ == "auto") {
        compression_ = FileCompressionType::AUTO_DETECT;
    } else if (options.compression_ == "gzip") {
        compression_ = FileCompressionType::GZIP;
    }

    auto result = fs_.openFile(options.filePath_.c_str(), FileFlags::FILE_FLAGS_READ, FileLockType::NO_LOCK,
                              compression_);
    plainFileSource_ = result->onDiskFile() && result->canSeek();
    fileSize_ = result->getFileSize();
    return result;
}

enum class QuoteRule : uint8_t { QUOTES_RFC = 0, QUOTES_OTHER = 1, NO_QUOTES = 2 };

void BufferedCSVReader::detectDialect(const vector<ConstantType> &requested_types,
    BufferedCSVReaderOptions &original_options, vector<BufferedCSVReaderOptions> &info_candidates,
    idx_t &best_num_cols) {
    // set up the candidates we consider for delimiter and quote rules based on user input
	vector<string> delim_candidates;
	vector<QuoteRule> quoterule_candidates;
	vector<vector<string>> quote_candidates_map;
	vector<vector<string>> escape_candidates_map = {{""}, {"\\"}, {""}};

	if (options_.hasDelimiter_) {
		// user provided a delimiter: use that delimiter
		delim_candidates = {options_.delimiter_};
	} else {
		// no delimiter provided: try standard/common delimiters
		delim_candidates = {",", "|", ";", "\t"};
	}
	if (options_.hasQuote_) {
		// user provided quote: use that quote rule
		quote_candidates_map = {{options_.quote_}, {options_.quote_}, {options_.quote_}};
	} else {
		// no quote rule provided: use standard/common quotes
		quote_candidates_map = {{"\""}, {"\"", "'"}, {""}};
	}
	if (options_.hasEscape_) {
		// user provided escape: use that escape rule
		if (options_.escape_.empty()) {
			quoterule_candidates = {QuoteRule::QUOTES_RFC};
		} else {
			quoterule_candidates = {QuoteRule::QUOTES_OTHER};
		}
		escape_candidates_map[static_cast<uint8_t>(quoterule_candidates[0])] = {options_.escape_};
	} else {
		// no escape provided: try standard/common escapes
		quoterule_candidates = {QuoteRule::QUOTES_RFC, QuoteRule::QUOTES_OTHER, QuoteRule::NO_QUOTES};
	}

	idx_t best_consistent_rows = 0;
	for (auto quoterule : quoterule_candidates) {
		const auto &quote_candidates = quote_candidates_map[static_cast<uint8_t>(quoterule)];
		for (const auto &quote : quote_candidates) {
			for (const auto &delim : delim_candidates) {
				const auto &escape_candidates = escape_candidates_map[static_cast<uint8_t>(quoterule)];
				for (const auto &escape : escape_candidates) {
					BufferedCSVReaderOptions sniff_info = original_options;
					sniff_info.delimiter_ = delim;
					sniff_info.quote_ = quote;
					sniff_info.escape_ = escape;

					options_ = sniff_info;
					prepareComplexParser();

					jumpToBeginning(original_options.skipRows_);
					sniffedColumnCounts_.clear();
					if (!tryParseCSV(ParserMode::SNIFFING_DIALECT)) {
						continue;
					}

					idx_t start_row = original_options.skipRows_;
					idx_t consistent_rows = 0;
					idx_t num_cols = 0;

					for (idx_t row = 0; row < sniffedColumnCounts_.size(); row++) {
						if (sniffedColumnCounts_[row] == num_cols) {
							consistent_rows++;
						} else {
							num_cols = sniffedColumnCounts_[row];
							start_row = row + original_options.skipRows_;
							consistent_rows = 1;
						}
					}

					// some logic
					bool more_values = (consistent_rows > best_consistent_rows && num_cols >= best_num_cols);
					bool single_column_before = best_num_cols < 2 && num_cols > best_num_cols;
					bool rows_consistent =
					    start_row + consistent_rows - original_options.skipRows_ == sniffedColumnCounts_.size();
					bool more_than_one_row = (consistent_rows > 1);
					bool more_than_one_column = (num_cols > 1);
					bool start_good = !info_candidates.empty() && (start_row <= info_candidates.front().skipRows_);

					if (!requested_types.empty() && requested_types.size() != num_cols) {
						continue;
					} else if ((more_values || single_column_before) && rows_consistent) {
						sniff_info.skipRows_ = start_row;
						sniff_info.numCols_ = num_cols;
						best_consistent_rows = consistent_rows;
						best_num_cols = num_cols;

						info_candidates.clear();
						info_candidates.push_back(sniff_info);
					} else if (more_than_one_row && more_than_one_column && start_good && rows_consistent) {
						bool same_quote_is_candidate = false;
						for (auto &info_candidate : info_candidates) {
							if (quote.compare(info_candidate.quote_) == 0) {
								same_quote_is_candidate = true;
							}
						}
						if (!same_quote_is_candidate) {
							sniff_info.skipRows_ = start_row;
							sniff_info.numCols_ = num_cols;
							info_candidates.push_back(sniff_info);
						}
					}
				}
			}
		}
	}
}

void BufferedCSVReader::detectCandidateTypes(const vector<ConstantType> &type_candidates,
    const vector<BufferedCSVReaderOptions> &info_candidates, BufferedCSVReaderOptions &original_options,
    idx_t best_num_cols, vector<vector<ConstantType>> &best_sql_types_candidates,
    DataChunk &best_header_row) {
	BufferedCSVReaderOptions best_options;
	idx_t min_varchar_cols = best_num_cols + 1;

	for (auto &info_candidate : info_candidates) {
		options_ = info_candidate;
		vector<vector<ConstantType>> info_sql_types_candidates(options_.numCols_, type_candidates);

		// set all sql_types to VARCHAR so we can do datatype detection based on VARCHAR values
		types_.clear();
		types_.assign(options_.numCols_, ConstantType::STRING);

		// jump to beginning and skip potential header
		jumpToBeginning(options_.skipRows_, true);
		DataChunk header_row;
		header_row.initialize(types_);
		parseChunk_.copy(header_row);

		if (header_row.getSize() == 0) {
			continue;
		}

		// init parse chunk and read csv with info candidate
		initParseChunk(types_.size());
		parseCSV(ParserMode::SNIFFING_DATATYPES);
		for (idx_t row_idx = 0; row_idx <= parseChunk_.getSize(); row_idx++) {
			bool is_header_row = row_idx == 0;
			idx_t row = row_idx - 1;
			for (idx_t col = 0; col < parseChunk_.columnCount(); col++) {
				auto &col_type_candidates = info_sql_types_candidates[col];
				while (col_type_candidates.size() > 1) {
					const auto &sql_type = col_type_candidates.back();
					// try cast from string to sql_type
					Value dummy_val;
					if (is_header_row) {
						dummy_val = header_row.getValue(col, 0);
					} else {
						dummy_val = parseChunk_.getValue(col, row);
					}
					// try formatting for date types if the user did not specify one and it starts with numeric values.
					string separator;

					// try cast from string to sql_type
					if (tryCastValue(dummy_val, sql_type)) {
						break;
					} else {
						col_type_candidates.pop_back();
					}
				}
			}
			// reset type detection, because first row could be header,
			// but only do it if csv has more than one line (including header)
			if (parseChunk_.getSize() > 0 && is_header_row) {
				info_sql_types_candidates = vector<vector<ConstantType>>(options_.numCols_, type_candidates);
			}
		}

		idx_t varchar_cols = 0;
		for (idx_t col = 0; col < parseChunk_.columnCount(); col++) {
			auto &col_type_candidates = info_sql_types_candidates[col];
			// check number of varchar columns
			const auto &col_type = col_type_candidates.back();
			if (col_type == ConstantType::STRING) {
				varchar_cols++;
			}
		}

		// it's good if the dialect creates more non-varchar columns, but only if we sacrifice < 30% of best_num_cols.
		if (varchar_cols < min_varchar_cols && parseChunk_.columnCount() > (best_num_cols * 0.7)) {
			// we have a new best_options candidate
			best_options = info_candidate;
			min_varchar_cols = varchar_cols;
			best_sql_types_candidates = info_sql_types_candidates;
			best_header_row.destroy();
			auto header_row_types = header_row.getTypes();
			best_header_row.initialize(header_row_types);
			header_row.copy(best_header_row);
		}
	}

	options_ = best_options;
}

void BufferedCSVReader::detectHeader(const vector<vector<ConstantType>> &best_sql_types_candidates,
    const DataChunk &best_header_row) {
	// information for header detection
	bool first_row_consistent = true;

	// check if header row is all null and/or consistent with detected column data types
	for (idx_t col = 0; col < best_sql_types_candidates.size(); col++) {
		auto dummy_val = best_header_row.getValue(col, 0);

		// try cast to sql_type of column
		const auto &sql_type = best_sql_types_candidates[col].back();
		if (!tryCastValue(dummy_val, sql_type)) {
			first_row_consistent = false;
		}
	}

	// update parser info, and read, generate & set col_names based on previous findings
	if ((!first_row_consistent && !options_.hasHeader_) || (options_.hasHeader_ && options_.header_)) {
		options_.header_ = true;
		std::unordered_map<string, idx_t> name_collision_count;
		// get header names from CSV
		for (idx_t col = 0; col < options_.numCols_; col++) {
			const auto &val = best_header_row.getValue(col, 0);
			string col_name = val.toString();

			// generate name if field is empty
			if (col_name.empty()) {
				col_name = generateColumnName(options_.numCols_, col);
			}

			col_name = normalizeColumnName(col_name);
			// col_name = trimWhitespace(col_name);

			// avoid duplicate header names
			const string col_name_raw = col_name;
			while (name_collision_count.find(col_name) != name_collision_count.end()) {
				name_collision_count[col_name] += 1;
				col_name = col_name + "_" + std::to_string(name_collision_count[col_name]);
			}

			colNames_.push_back(col_name);
			name_collision_count[col_name] = 0;
		}

	} else {
		options_.header_ = false;
		idx_t total_columns = parseChunk_.columnCount();
		for (idx_t col = 0; col < total_columns; col++) {
			string column_name = generateColumnName(total_columns, col);
			colNames_.push_back(column_name);
		}
	}
}

vector<ConstantType> BufferedCSVReader::refineTypeDetection(const vector<ConstantType> &type_candidates,
    const vector<ConstantType> &requested_types, vector<vector<ConstantType>> &best_sql_types_candidates) {
	// for the type refine we set the SQL types to STRING for all columns
	types_.clear();
	types_.assign(options_.numCols_, ConstantType::STRING);

	vector<ConstantType> detected_types;

	// if data types were provided, exit here if number of columns does not match
	if (!requested_types.empty()) {
		if (requested_types.size() != options_.numCols_) {
			ErrorHandler::errorParsing(
			    "Error while determining column types: found "+ std::to_string(options_.numCols_) +" columns but expected "+ std::to_string(requested_types.size())+".");
		} else {
			detected_types = requested_types;
		}
	} else if (options_.allVarchar_) {
		// return all types varchar
		detected_types = types_;
	} else {
		// jump through the rest of the file and continue to refine the sql type guess
		while (jumpToNextSample()) {
			initParseChunk(types_.size());
			// if jump ends up a bad line, we just skip this chunk
			if (!tryParseCSV(ParserMode::SNIFFING_DATATYPES)) {
				continue;
			}
			for (idx_t col = 0; col < parseChunk_.columnCount(); col++) {
				vector<ConstantType> &col_type_candidates = best_sql_types_candidates[col];
				while (col_type_candidates.size() > 1) {
					const auto &sql_type = col_type_candidates.back();
					if (tryCastVector(parseChunk_.data_[col], parseChunk_.getSize(), sql_type)) {
						break;
					} else {
						col_type_candidates.pop_back();
					}
				}
			}

			if (!jumpingSamples_) {
				if ((sampleChunkIdx_)*options_.sampleChunkSize_ <= options_.bufferSize_) {
					// cache parse chunk
					// create a new chunk and fill it with the remainder
					auto chunk = data_chunk_ptr_t(new DataChunk);
					auto parse_chunk_types = parseChunk_.getTypes();
					chunk->initializeEmpty(parse_chunk_types);
					chunk->reference(parseChunk_);
					parseChunk_.destroy();
					cached_chunks.push(std::move(chunk));
				} else {
					while (!cached_chunks.empty()) {
						cached_chunks.pop();
					}
				}
			}
		}

		// set sql types
		for (auto &best_sql_types_candidate : best_sql_types_candidates) {
			ConstantType d_type = best_sql_types_candidate.back();
			// if (best_sql_types_candidate.size() == type_candidates.size()) {
			// 	d_type = ConstantType::STRING;
			// }
			detected_types.push_back(d_type);
		}
	}

	return detected_types;
}
}
