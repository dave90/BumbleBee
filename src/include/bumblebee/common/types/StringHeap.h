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
#include <memory>

#include "BumbleString.h"
#include "bumblebee/common/TypeDefs.h"

namespace bumblebee{

// This class manage the heap for a set of strings
class StringHeap {
public:
    StringHeap() = default;
    StringHeap(StringHeap &&other) : chunk_(std::move(other.chunk_)) {}
    ~StringHeap() = default;

    void destroy() {
        chunk_ = nullptr;
    }
    inline string_t addString(const char *data, idx_t len) {
        return addBlob(data, len);
    }
    inline string_t addString(const char *data) {
        return addString(data, strlen(data));
    }
    inline string_t addString(const string &data) {
        return addString(data.c_str(), data.length());
    }
    inline string_t addString(const string_t &data) {
        return addString(data.c_str(), data.length());
    }
    // Add a string in the heap (note: the add string do not copy the null termination)
    string_t addBlob(const char *data, idx_t len);
    string_t addEmptyString(idx_t len);

private:
    // chunk where to store the strings in a unique block
    struct StringChunk {
        explicit StringChunk(idx_t size) : current_position_(0), maximum_size_(size) {
            data_ = std::unique_ptr<char[]>(new char[maximum_size_]);
        }

        std::unique_ptr<char[]> data_;
        idx_t current_position_;
        idx_t maximum_size_;
        // previous chunk
        std::unique_ptr<StringChunk> prev_;
    };
    std::unique_ptr<StringChunk> chunk_{nullptr};
};


}
