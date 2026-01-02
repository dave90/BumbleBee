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
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{

struct TextSearchShiftArray {
    TextSearchShiftArray();
    explicit TextSearchShiftArray(string search_term);

    inline bool match(uint8_t &position, uint8_t byte_value) {
        if (position >= length_) {
            return false;
        }
        position = shifts_[position * 255 + byte_value];
        return position == length_;
    }

    idx_t length_;
    std::unique_ptr<uint8_t[]> shifts_;
};


struct BufferedCSVReaderOptions {
	// The file path of the CSV file to read
	string filePath_;
	// Whether file is compressed or not, and if so which compression type
	// ("infer" (default; infer from file extention), "gzip", "none")
	string compression_ = "none";
	// Whether or not to automatically detect dialect and datatypes
	bool autoDetect_ = true;
	// Whether or not a delimiter was defined by the user
	bool hasDelimiter_ = false;
	// Delimiter to separate columns within each line
	string delimiter_ = ",";
	// Whether or not a quote sign was defined by the user
	bool hasQuote_ = false;
	// Quote used for columns that contain reserved characters, e.g., delimiter
	string quote_ = "\"";
	// Whether or not an escape character was defined by the user
	bool hasEscape_ = false;
	// Escape character to escape quote character
	string escape_;
	// Whether or not a header information was given by the user
	bool hasHeader_ = false;
	// Whether or not the file has a header line
	bool header_ = false;
	// How many leading rows to skip
	idx_t skipRows_ = 0;
	// Expected number of columns
	idx_t numCols_ = 0;
	// Size of sample chunk used for dialect and type detection
	idx_t sampleChunkSize_ = STANDARD_VECTOR_SIZE;
	// Number of sample chunks used for type detection
	idx_t sampleChunks_ = 75;
	// Number of samples to buffer
	idx_t bufferSize_ = STANDARD_VECTOR_SIZE * 100;
	// Consider all columns to be of type varchar
	bool allVarchar_ = false;

	std::string toString() const {
		return "DELIMITER='" + delimiter_ + (hasDelimiter_ ? "'" : (autoDetect_ ? "' (auto detected)" : "' (default)")) +
		       ", QUOTE='" + quote_ + (hasQuote_ ? "'" : (autoDetect_ ? "' (auto detected)" : "' (default)")) +
		       ", ESCAPE='" + escape_ + (hasEscape_ ? "'" : (autoDetect_ ? "' (auto detected)" : "' (default)")) +
		       ", HEADER=" + std::to_string(header_) +
		       (hasHeader_ ? "" : (autoDetect_ ? " (auto detected)" : "' (default)")) +
		       ", SAMPLE_SIZE=" + std::to_string(sampleChunkSize_ * sampleChunks_) +
		       ", ALL_VARCHAR=" + std::to_string(allVarchar_);
	}
};

enum class ParserMode : uint8_t { PARSING = 0, SNIFFING_DIALECT = 1, SNIFFING_DATATYPES = 2, PARSING_HEADER = 3 };


class BufferedCSVReader {
	// Initial buffer read size; can be extended for long lines
	static constexpr idx_t INITIAL_BUFFER_SIZE = 16384;
	// Maximum CSV line size: specified because if we reach this amount, we likely have the wrong delimiters
	static constexpr idx_t MAXIMUM_CSV_LINE_SIZE = 1048576;
	ParserMode mode_;

public:
	BufferedCSVReader(ClientContext &context, BufferedCSVReaderOptions options,
	                  const vector<ConstantType> &requested_types = vector<ConstantType>(), const std::vector<idx_t> &select_cols = vector<idx_t>() );
	BufferedCSVReader(FileSystem &fs, BufferedCSVReaderOptions options,
					  const vector<ConstantType> &requested_types = vector<ConstantType>(), const std::vector<idx_t> &select_cols = vector<idx_t>());

	FileSystem &fs_;
	BufferedCSVReaderOptions options_;
	vector<ConstantType> types_;
	vector<string> colNames_;
	file_handler_ptr_t fileHandle_;
	bool plainFileSource_ = false;
	idx_t fileSize_ = 0;
	FileCompressionType compression_ = FileCompressionType::UNCOMPRESSED;

	std::unique_ptr<char[]> buffer_;
	idx_t bufferSize_;
	idx_t position_;
	idx_t start_ = 0;

	idx_t linenr_ = 0;
	bool linenrEstimated_ = false;

	vector<idx_t> sniffedColumnCounts_;
	bool rowEmpty_ = false;
	idx_t sampleChunkIdx_ = 0;
	bool jumpingSamples_ = false;
	bool endOfFileReached_ = false;
	bool bomChecked_ = false;

	idx_t bytesInChunk_ = 0;
	idx_t avgBytesInChunk_ = 0;
	double bytesPerLineAvg_ = 0;

	vector<std::unique_ptr<char[]>> cachedBuffers_;

	TextSearchShiftArray delimiterSearch_, escapeSearch_, quoteSearch_;

	DataChunk parseChunk_;

	std::queue<data_chunk_ptr_t> cached_chunks;
	idx_t chunkByteSizes_; // bytes in a data chunk
	idx_t bytesToRead_ = 0; // limit the bytes to read (0 is no limits)
	idx_t bytesRead_ = 0; // amount of bytes read

	// if not empty load and parse only the columns present in selectCols_
	std::unordered_set<id_t> selectCols_;

public:
	// Extract a single DataChunk from the CSV file and stores it in insert_chunk
	void parseCSV(DataChunk &insertChunk);

private:
	// Initialize Parser
	void initialize(const vector<ConstantType> &requested_types);
	// Initializes the parse_chunk with varchar columns and aligns info with new number of cols
	void initParseChunk(idx_t num_cols);
	// Initializes the TextSearchShiftArrays for complex parser
	void prepareComplexParser();
	// Try to parse a single datachunk from the file. Throws an exception if anything goes wrong.
	void parseCSV(ParserMode mode);
	// Try to parse a single datachunk from the file. Returns whether or not the parsing is successful
	bool tryParseCSV(ParserMode mode);
	// Extract a single DataChunk from the CSV file and stores it in insert_chunk
	bool tryParseCSV(ParserMode mode, DataChunk &insert_chunk, string &error_message);
	// Sniffs CSV dialect and determines skip rows, header row, column types and column names
	vector<ConstantType> sniffCSV(const vector<ConstantType> &requested_types);
	// Try to cast a string value to the specified sql type
	bool tryCastValue(const Value &value, const ConstantType &sql_type);
	// Try to cast a vector of values to the specified sql type
	bool tryCastVector(Vector &parse_chunk_col, idx_t size, const ConstantType &sql_type);
	// Skips skip_rows, reads header row from input stream
	void skipRowsAndReadHeader(idx_t skip_rows, bool skip_header);
	// Jumps back to the beginning of input stream and resets necessary internal states
	void jumpToBeginning(idx_t skip_rows, bool skip_header = false);
	// Jumps back to the beginning of input stream and resets necessary internal states
	bool jumpToNextSample();
	// Resets the buffer
	void resetBuffer();
	// Resets the steam
	void resetStream();

	// Parses a CSV file with a one-byte delimiter, escape and quote character
	bool tryParseSimpleCSV(DataChunk &insert_chunk, string &error_message);
	// Parses more complex CSV files with multi-byte delimiters, escapes or quotes
	bool tryParseComplexCSV(DataChunk &insert_chunk, string &error_message);

	// Adds a value to the current row
	void addValue(char *str_val, idx_t length, idx_t &column, vector<idx_t> &escape_positions);
	// Adds a row to the insert_chunk, returns true if the chunk is filled as a result of this row being added
	bool addRow(DataChunk &insert_chunk, idx_t &column);
	// Finalizes a chunk, parsing all values that have been added so far and adding them to the insert_chunk
	void flush(DataChunk &insert_chunk);
	// Reads a new buffer from the CSV file if the current one has been exhausted
	bool readBuffer(idx_t &start);

	file_handler_ptr_t openCSV(const BufferedCSVReaderOptions &options);

	// First phase of auto detection: detect CSV dialect (i.e. delimiter, quote rules, etc)
	void detectDialect(const vector<ConstantType> &requested_types, BufferedCSVReaderOptions &original_options,
	                   vector<BufferedCSVReaderOptions> &info_candidates, idx_t &best_num_cols);
	// Second phase of auto detection: detect candidate types for each column
	void detectCandidateTypes(const vector<ConstantType> &type_candidates,
	                          const vector<BufferedCSVReaderOptions> &info_candidates,
	                          BufferedCSVReaderOptions &original_options, idx_t best_num_cols,
	                          vector<vector<ConstantType>> &best_sql_types_candidates,
	                          DataChunk &best_header_row);
	// Third phase of auto detection: detect header of CSV file
	void detectHeader(const vector<vector<ConstantType>> &best_sql_types_candidates, const DataChunk &best_header_row);
	// Fourth phase of auto detection: refine the types of each column and select which types to use for each column
	vector<ConstantType> refineTypeDetection(const vector<ConstantType> &type_candidates,
	                                        const vector<ConstantType> &requested_types,
	                                        vector<vector<ConstantType>> &best_sql_types_candidates);
};

using buffered_csv_reader_ptr_t = std::unique_ptr<BufferedCSVReader>;
}
