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
#include "Serializer.hpp"
#include "bumblebee/common/FileSystem.hpp"

namespace bumblebee{

#define FILE_BUFFER_SIZE 4096


class BufferedFileWriter : public Serializer {
public:
    static constexpr uint8_t DEFAULT_OPEN_FLAGS = FileFlags::FILE_FLAGS_WRITE | FileFlags::FILE_FLAGS_FILE_CREATE;

    // Serializes to a buffer allocated by the serializer, will expand when
    // writing past the initial threshold
    BufferedFileWriter(FileSystem &fs, const string &path, uint8_t open_flags = DEFAULT_OPEN_FLAGS);

    FileSystem &fs_;
    std::unique_ptr<data_t[]> data_;
    idx_t offset_;
    idx_t totalWritten_;
    std::unique_ptr<FileHandle> handle_;

public:
    void writeData(const_data_ptr_t buffer, uint64_t write_size) override;
    // Flush the buffer to disk and sync the file to ensure writing is completed
    void sync();
    // Flush the buffer to the file (without sync)
    void flush();
    // Returns the current size of the file
    int64_t getFileSize();
    // Truncate the size to a previous size (given that size <= GetFileSize())
    void truncate(int64_t size);

    idx_t getTotalWritten();
};


}
