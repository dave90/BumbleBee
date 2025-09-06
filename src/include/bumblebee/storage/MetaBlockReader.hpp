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
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/serializer/Serializer.hpp"

namespace bumblebee{


class MetaBlockReader : public Deserializer {
public:
    MetaBlockReader(ClientContext &context, block_id_t block):context_(context), handle_(nullptr), offset_(0), nextBlock_(-1) {
        readNewBlock(block);
    }
    ~MetaBlockReader() override {

    }

    ClientContext &context_;
    block_handle_shared_ptr_t block_;
    buffer_handle_ptr_t handle_;
    idx_t offset_;
    block_id_t nextBlock_;

public:
    // Read content of size read_size into the buffer
    void readData(data_ptr_t buffer, idx_t read_size) override {
        while (offset_ + read_size > handle_->node_->size_) {
            // cannot read entire entry from block
            // first read what we can from this block
            idx_t to_read = handle_->node_->size_ - offset_;
            if (to_read > 0) {
                memcpy(buffer, handle_->node_->buffer_ + offset_, to_read);
                read_size -= to_read;
                buffer += to_read;
            }
            // then move to the next block
            readNewBlock(nextBlock_);
        }
        // we have enough left in this block to read from the buffer
        memcpy(buffer, handle_->node_->buffer_ + offset_, read_size);
        offset_ += read_size;
    }

private:
    void readNewBlock(block_id_t id) {
        auto &block_manager = *context_.blockManager_;
        auto &buffer_manager = *context_.bufferManager_;

        block_manager.markBlockAsModified(id);
        block_ = buffer_manager.registerBlock(id);
        handle_ = buffer_manager.pin(block_);

        nextBlock_ = load<block_id_t>(handle_->node_->buffer_);
        BB_ASSERT(nextBlock_ >= -1);
        offset_ = sizeof(block_id_t);
    }
};


}
