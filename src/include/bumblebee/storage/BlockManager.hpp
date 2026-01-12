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
#include "Block.hpp"

namespace bumblebee{

// BlockManager is an abstract representation to manage blocks. When writing or reading blocks, the
// BlockManager creates and accesses blocks. The concrete types implements how blocks are stored.
class BlockManager {
public:
    virtual ~BlockManager() = default;

    virtual void startCheckpoint() = 0;
    // Creates a new block inside the block manager
    virtual block_ptr_t createBlock(block_id_t block_id) = 0;
    // Return the next free block id
    virtual block_id_t getFreeBlockId() = 0;
    // Returns whether or not a specified block is the root block
    virtual bool isRootBlock(block_id_t root) = 0;
    // Mark a block as "modified"; modified blocks are added to the free list after a checkpoint (i.e. their data is
    // assumed to be rewritten)
    virtual void markBlockAsModified(block_id_t block_id) = 0;
    // Increase the reference count of a block. The block should hold at least one reference before this method is
    // called.
    virtual void increaseBlockReferenceCount(block_id_t block_id) = 0;
    // Get the first meta block id
    virtual block_id_t getMetaBlock() = 0;
    // Read the content of the block from disk
    virtual void read(Block &block) = 0;
    // Writes the block to disk
    virtual void write(FileBuffer &block, block_id_t block_id) = 0;
    // Writes the block to disk
    void write(Block &block) {
        write(block, block.id);
    }
    // Write the header; should be the final step of a checkpoint
    virtual void writeHeader(DatabaseHeader header) = 0;

    // Returns the number of total blocks
    virtual idx_t totalBlocks() = 0;
    // Returns the number of free blocks
    virtual idx_t freeBlocks() = 0;

};

using block_manager_ptr_t = std::unique_ptr<BlockManager>;


}
