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
#include "bumblebee/storage/BlockHandle.h"

#include "bumblebee/ClientContext.h"
#include "bumblebee/common/types/Assert.h"

namespace bumblebee{

BlockHandle::BlockHandle(ClientContext &context, block_id_t block_id): canDestroy_(false), context_(context), blockId_(block_id), buffer_(nullptr) {
    evictionTimestamp_ = 0;
    state_ = BlockState::BLOCK_UNLOADED;
    memoryUsage_ = Storage::BLOCK_ALLOC_SIZE;
}

BlockHandle::BlockHandle(ClientContext &context, block_id_t block_id, file_buffer_ptr_t buffer, bool can_destroy,
    idx_t block_size) :canDestroy_(can_destroy), context_(context), blockId_(block_id), buffer_(std::move(buffer)){
    BB_ASSERT(block_size >= Storage::BLOCK_SIZE);
    state_ = BlockState::BLOCK_LOADED;
    memoryUsage_ = block_size + Storage::BLOCK_HEADER_SIZE;
}

BlockHandle::~BlockHandle() {
    auto &buffer_manager = context_.bufferManager_;
    // no references remain to this block: erase
    if (state_ == BlockState::BLOCK_LOADED) {
        // the block is still loaded in memory: erase it
        buffer_.reset();
        buffer_manager->currentMemory_ -= memoryUsage_;
    }
    buffer_manager->unregisterBlock(blockId_, canDestroy_);
}

buffer_handle_ptr_t BlockHandle::load() {
    auto this_shared = block_shared_ptr_t(this);
    if (state_ == BlockState::BLOCK_LOADED) {
        // already loaded
        BB_ASSERT(buffer_);
        return buffer_handle_ptr_t( new BufferHandle(this_shared, buffer_.get()));
    }

    auto &buffer_manager = context_.bufferManager_;
    auto &block_manager = context_.blockManager_;
    if (blockId_ < MAXIMUM_BLOCK) {
        auto block = block_ptr_t(new Block(context_.allocator_, blockId_));
        block_manager->read(*block);
        buffer_ = std::move(block);
    } else {
        if (canDestroy_) {
            return nullptr;
        } else {
            buffer_ = buffer_manager->readTemporaryBuffer(blockId_);
        }
    }
    state_ = BlockState::BLOCK_LOADED;
    return buffer_handle_ptr_t( new BufferHandle(this_shared, buffer_.get()));
}

void BlockHandle::unload() {
    auto &buffer_manager =context_.bufferManager_;
    if (state_ == BlockState::BLOCK_UNLOADED) {
        // already unloaded: nothing to do
        return;
    }
    BB_ASSERT(canUnload());
    BB_ASSERT(memoryUsage_ >= Storage::BLOCK_ALLOC_SIZE);

    if (blockId_ >= MAXIMUM_BLOCK && !canDestroy_) {
        // temporary block that cannot be destroyed: write to temporary file
        buffer_manager->writeTemporaryBuffer((ManagedBuffer &)*buffer_);
    }
    buffer_.reset();
    buffer_manager->currentMemory_ -= memoryUsage_;
    state_ = BlockState::BLOCK_UNLOADED;
}

bool BlockHandle::canUnload() {
    if (state_ == BlockState::BLOCK_UNLOADED) {
        // already unloaded
        return false;
    }
    if (readers_ > 0) {
        // there are active readers
        return false;
    }
    auto &buffer_manager = context_.bufferManager_;
    if (blockId_ >= MAXIMUM_BLOCK && !canDestroy_ && buffer_manager->tempDirectory_.empty()) {
        // in order to unload this block we need to write it to a temporary buffer
        // however, no temporary directory is specified!
        // hence we cannot unload the block
        return false;
    }
    return true;
}
}
