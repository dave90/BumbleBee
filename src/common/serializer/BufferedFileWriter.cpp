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
#include "bumblebee/common/serializer/BufferedFileWriter.hpp"

#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{


BufferedFileWriter::BufferedFileWriter(FileSystem &fs, const string &path, uint8_t open_flags)
    : fs_(fs), data_(std::unique_ptr<data_t[]>(new data_t[FILE_BUFFER_SIZE])), offset_(0), totalWritten_(0) {
    handle_ = fs.openFile(path, open_flags, FileLockType::WRITE_LOCK, FileSystem::DEFAULT_COMPRESSION);
}

int64_t BufferedFileWriter::getFileSize() {
    return fs_.getFileSize(*handle_);
}

idx_t BufferedFileWriter::getTotalWritten() {
    return totalWritten_ + offset_;
}

void BufferedFileWriter::writeData(const_data_ptr_t buffer, uint64_t write_size) {
    const_data_ptr_t end_ptr = buffer + write_size;
    while (buffer < end_ptr) {
        idx_t to_write = std::min( (idx_t)(end_ptr - buffer),(idx_t)( FILE_BUFFER_SIZE - offset_));
        BB_ASSERT(to_write > 0);
        memcpy(data_.get() + offset_, buffer, to_write);
        offset_ += to_write;
        buffer += to_write;
        if (offset_ == FILE_BUFFER_SIZE) {
            flush();
        }
    }
}

void BufferedFileWriter::flush() {
    if (offset_ == 0) {
        return;
    }
    fs_.write(*handle_, data_.get(), offset_);
    totalWritten_ += offset_;
    offset_ = 0;
}

void BufferedFileWriter::sync() {
    flush();
    handle_->sync();
}

void BufferedFileWriter::truncate(int64_t size) {
    // truncate the physical file on disk
    handle_->truncate(size);
    // reset anything written in the buffer
    offset_ = 0;
}

}
