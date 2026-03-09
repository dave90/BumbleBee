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

#include "StorageInfo.hpp"
#include "bumblebee/common/FileBuffer.hpp"
#include "bumblebee/common/Mutex.hpp"

namespace bumblebee{


class ClientContext;
class BufferHandle;

enum class BlockState : uint8_t { BLOCK_UNLOADED = 0, BLOCK_LOADED = 1 };

class BlockHandle {
    friend struct BufferEvictionNode;
    friend class BufferHandle;
    friend class BufferManager;

public:
    BlockHandle(ClientContext &context, block_id_t block_id);
    BlockHandle(ClientContext &context, block_id_t block_id, file_buffer_ptr_t buffer, bool can_destroy,
                idx_t block_size);
    ~BlockHandle();

    ClientContext &context_;

public:
    block_id_t blockId() {
        return blockId_;
    }

    int32_t readers() const {
        return readers_;
    }

private:
    static std::unique_ptr<BufferHandle> load(std::shared_ptr<BlockHandle>& handle);
    void unload();
    bool canUnload();

    // The block-level lock
    mutex lock_;
    // Whether or not the block is loaded/unloaded
    BlockState state_;
    // Amount of concurrent readers
    atomic<int32_t> readers_;
    // The block id of the block
    const block_id_t blockId_;
    // Pointer to loaded data (if any)
    std::unique_ptr<FileBuffer> buffer_;
    // Internal eviction timestamp
    atomic<idx_t> evictionTimestamp_;
    // Whether or not the buffer can be destroyed (only used for temporary buffers)
    const bool canDestroy_;
    // The memory usage of the block
    idx_t memoryUsage_;
};


using block_handle_shared_ptr_t = std::shared_ptr<BlockHandle>;

}
