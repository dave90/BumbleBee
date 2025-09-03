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

#include "BlockHandle.h"
#include "bumblebee/ClientContext.h"
#include "bumblebee/common/FileBuffer.h"

namespace bumblebee{


class BufferHandle {
public:
    BufferHandle(block_shared_ptr_t handle, FileBuffer *node): handle_(handle), node_(node) {
    }
    ~BufferHandle() {
        auto &buffer_manager = handle_->context_.bufferManager_;
        buffer_manager.unpin(handle_);
    }

    // The block handle
    block_shared_ptr_t handle_;
    //! The managed buffer node
    FileBuffer *node_;
    data_ptr_t ptr() {
        return node_->buffer_;
    }
};

using buffer_hande_ptr_t = std::unique_ptr<BufferHandle>;

}
