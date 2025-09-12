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
#include "StorageInfo.hpp"
#include "bumblebee/common/FileBuffer.hpp"
#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{


class Block : public FileBuffer {
public:
    Block(Allocator &allocator, block_id_t id)
        : FileBuffer(allocator, FileBufferType::BLOCK, Storage::BLOCK_ALLOC_SIZE), id(id) {
    }

    Block(FileBuffer &source, block_id_t id) : FileBuffer(source, FileBufferType::BLOCK), id(id) {
        BB_ASSERT(getMallocedSize() == Storage::BLOCK_ALLOC_SIZE);
        BB_ASSERT(size_ == Storage::BLOCK_SIZE);
    }
    block_id_t id;
};

struct BlockPointer {
    block_id_t blockId_;
    uint32_t offset_;
};

using block_ptr_t = std::unique_ptr<Block>;

}
