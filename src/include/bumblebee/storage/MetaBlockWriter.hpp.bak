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
#include "bumblebee/ClientContext.h"
#include "bumblebee/common/Helper.h"
#include "bumblebee/common/serializer/Serializer.h"

namespace bumblebee{


class MetaBlockWriter : public Serializer {
public:
    MetaBlockWriter(ClientContext &context, block_id_t initial_block_id = INVALID_BLOCK): context_(context) {
        if (initial_block_id == INVALID_BLOCK) {
            initial_block_id = MetaBlockWriter::getNextBlockId();
        }
        auto &block_manager = *context_.blockManager_;
        block_ = block_manager.createBlock(initial_block_id);
        store<block_id_t>(-1, block_->buffer_);
        offset_ = sizeof(block_id_t);
    }
    ~MetaBlockWriter() override {}

    ClientContext &context_;
    block_ptr_t block_;
    std::unordered_set<block_id_t> writtenBlocks_;
    idx_t offset_;

public:
    BlockPointer getBlockPointer() {
        BlockPointer pointer;
        pointer.blockId_ = block_->id;
        pointer.offset_ = offset_;
        return pointer;
    }
    void flush() {
        writtenBlocks_.insert(block_->id);
        if (offset_ > sizeof(block_id_t)) {
            auto &block_manager = *context_.blockManager_;
            block_manager.write(*block_);
            offset_ = sizeof(block_id_t);
        }
    }

    void writeData(const_data_ptr_t buffer, idx_t write_size) override {
        while (offset_ + write_size > block_->size_) {
            // we need to make a new block_
            // first copy what we can
            BB_ASSERT(offset_ <= block_->size_);
            idx_t copy_amount = block_->size_ - offset_;
            if (copy_amount > 0) {
                memcpy(block_->buffer_ + offset_, buffer, copy_amount);
                buffer += copy_amount;
                offset_ += copy_amount;
                write_size -= copy_amount;
            }
            // now we need to get a new block id
            block_id_t new_block_id = getNextBlockId();
            // write the block id of the new block to the start of the current block
            store<block_id_t>(new_block_id, block_->buffer_);
            // first flush the old block
            flush();
            // now update the block id of the lbock
            block_->id = new_block_id;
            store<block_id_t>(-1, block_->buffer_);
        }
        memcpy(block_->buffer_ + offset_, buffer, write_size);
        offset_ += write_size;
    }

protected:
    virtual block_id_t getNextBlockId() {
        auto &block_manager = *context_.blockManager_;
        return block_manager.getFreeBlockId();
    }
};


}
