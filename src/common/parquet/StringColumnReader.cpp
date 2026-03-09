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

#include "bumblebee/common/parquet/StringColumnReader.hpp"

#include "utf8proc/utf8proc_wrapper.hpp"

namespace bumblebee {

class ParquetStringVectorBuffer : public VectorDataMngr {
public:
    explicit ParquetStringVectorBuffer(std::shared_ptr<ByteBuffer> buffer_p)
        : VectorDataMngr(VectorDataMngrType::STRING_BUFFER), buffer_(std::move(buffer_p)) {
    }

private:
    std::shared_ptr<ByteBuffer> buffer_;
};

string_t StringParquetValueConversion::dictRead(ByteBuffer &dict, uint32_t &offset, ColumnReader &reader) {
    auto &dict_strings = ((StringColumnReader &)reader).dictStrings_;
    return dict_strings[offset];
}

string_t StringParquetValueConversion::plainRead(ByteBuffer &plain_data, ColumnReader &reader) {
    auto &scr = ((StringColumnReader &)reader);
    uint32_t str_len = scr.fixedWidthStringLength_ == 0 ? plain_data.read<uint32_t>() : scr.fixedWidthStringLength_;
    plain_data.available(str_len);
    auto actual_str_len = ((StringColumnReader &)reader).verifyString(plain_data.ptr_, str_len);
    auto ret_str = string_t(plain_data.ptr_, actual_str_len);
    plain_data.inc(str_len);
    return ret_str;
}

void StringParquetValueConversion::plainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
    auto &scr = ((StringColumnReader &)reader);
    uint32_t str_len = scr.fixedWidthStringLength_ == 0 ? plain_data.read<uint32_t>() : scr.fixedWidthStringLength_;
    plain_data.available(str_len);
    plain_data.inc(str_len);
}

uint32_t StringColumnReader::verifyString(const char *str_data, uint32_t str_len) {
    if (logicalType_.getPhysicalType() != PhysicalType::STRING) {
        return str_len;
    }
    // verify if a string is actually UTF8, and if there are no null bytes in the middle of the string
    UnicodeInvalidReason reason;
    size_t pos;
    auto utf_type = Utf8Proc::Analyze(str_data, str_len, &reason, &pos);
    if (utf_type == UnicodeType::INVALID) {
        if (reason == UnicodeInvalidReason::NULL_BYTE) {
            // for null bytes we just truncate the string
            return pos;
        }
        ErrorHandler::errorParsing("Invalid string encoding found in Parquet file: value is not UTF8!");
    }
    return str_len;
}

void StringColumnReader::dictionary(std::shared_ptr<ByteBuffer> data, idx_t num_entries) {
    dict_ = std::move(data);
    dictStrings_ = std::unique_ptr<string_t[]>(new string_t[num_entries]);
    for (idx_t dict_idx = 0; dict_idx < num_entries; dict_idx++) {
        uint32_t str_len = dict_->read<uint32_t>();
        dict_->available(str_len);

        auto actual_str_len = verifyString(dict_->ptr_, str_len);
        dictStrings_[dict_idx] = string_t(dict_->ptr_, actual_str_len);
        dict_->inc(str_len);
    }
}


void StringColumnReader::dictReference(Vector &result) {
    StringVector::addBuffer(result, vector_data_mngr_ptr_t(new ParquetStringVectorBuffer(dict_)));
}

void StringColumnReader::plainReference(std::shared_ptr<ByteBuffer> plain_data, Vector &result) {
    StringVector::addBuffer(result, vector_data_mngr_ptr_t(new ParquetStringVectorBuffer(std::move(plain_data))));
}

string_t StringParquetValueConversion::null() {
    return string_t("");
}
}
