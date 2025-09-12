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
#include "bumblebee/common/serializer/BufferedFileReader.hpp"

#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/serializer/BufferedFileWriter.hpp"
#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{
BufferedFileReader::BufferedFileReader(FileSystem &fs, const char *path)
    : fs_(fs), data_(std::unique_ptr<data_t[]>(new data_t[FILE_BUFFER_SIZE])), offset_(0), readData_(0), totalRead_(0) {
    handle_ =
        fs.openFile(path, FileFlags::FILE_FLAGS_READ, FileLockType::READ_LOCK, FileSystem::DEFAULT_COMPRESSION);
    fileSize_ = fs.getFileSize(*handle_);
}

void BufferedFileReader::readData(data_ptr_t target_buffer, uint64_t read_size) {
    // first copy anything we can from the buffer
    data_ptr_t end_ptr = target_buffer + read_size;
    while (true) {
        idx_t to_read = std::min((idx_t)(end_ptr - target_buffer), (idx_t)(readData_ - offset_));
        if (to_read > 0) {
            memcpy(target_buffer, data_.get() + offset_, to_read);
            offset_ += to_read;
            target_buffer += to_read;
        }
        if (target_buffer < end_ptr) {
            BB_ASSERT(offset_ == readData_);
            totalRead_ += readData_;
            // did not finish reading yet but exhausted buffer
            // read data into buffer
            offset_ = 0;
            readData_ = fs_.read(*handle_, data_.get(), FILE_BUFFER_SIZE);
            if (readData_ == 0) {
                ErrorHandler::errorGeneric("not enough data in file to deserialize result");
            }
        } else {
            return;
        }
    }
}

bool BufferedFileReader::finished() {
    return totalRead_ + offset_ == fileSize_;
}
}
