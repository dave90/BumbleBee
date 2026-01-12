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
#include "bumblebee/common/Allocator.hpp"
#include "bumblebee/common/FileSystem.hpp"
#include "thrift/transport/TVirtualTransport.h"

namespace bumblebee{

class ThriftFileTransport : public thrift::transport::TVirtualTransport<ThriftFileTransport> {
public:
    ThriftFileTransport(Allocator &allocator, FileHandle &handle_p)
        : allocator_(allocator), handle_(handle_p), location_(0) {
    }

    uint32_t read(uint8_t *buf, uint32_t len) {
        if (prefetchedData_ && location_ >= prefetchLocation_ &&
            location_ + len < prefetchLocation_ + prefetchedData_->getSize()) {
            memcpy(buf, prefetchedData_->get() + location_ - prefetchLocation_, len);
            } else {
                handle_.read(buf, len, location_);
            }
        location_ += len;
        return len;
    }

    void prefetch(idx_t pos, idx_t len) {
        prefetchLocation_ = pos;
        prefetchedData_ = allocator_.allocate(len);
        handle_.read(prefetchedData_->get(), len, prefetchLocation_);
    }

    void clearPrefetch() {
        prefetchedData_.reset();
    }

    void setLocation(idx_t location_p) {
        location_ = location_p;
    }

    idx_t getLocation() {
        return location_;
    }
    idx_t getSize() {
        return handle_.fileSystem_.getFileSize(handle_);
    }

private:
    Allocator &allocator_;
    FileHandle &handle_;
    idx_t location_;

    std::unique_ptr<AllocatedData> prefetchedData_;
    idx_t prefetchLocation_;
};

}
