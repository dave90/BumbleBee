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
#include <set>
#include <unordered_set>

#include "BlockManager.hpp"

namespace bumblebee{



// SingleFileBlockManager is an implementation for a BlockManager which manages blocks in a single file
class SingleFileBlockManager : public BlockManager {
	// The location in the file where the block writing starts
	static constexpr uint64_t BLOCK_START = Storage::FILE_HEADER_SIZE * 3;

public:
	SingleFileBlockManager(ClientContext &context, string path, bool read_only, bool create_new, bool use_direct_io);

	void startCheckpoint() override;
	block_ptr_t createBlock(block_id_t block_id) override;
	block_id_t getFreeBlockId() override;
	bool isRootBlock(block_id_t root) override;
	void markBlockAsModified(block_id_t block_id) override;
	void increaseBlockReferenceCount(block_id_t block_id) override;
	block_id_t getMetaBlock() override;
	void read(Block &block) override;
	// Write the given block to disk
	void write(FileBuffer &block, block_id_t block_id) override;
	// Write the header to disk, this is the final step of the checkpointing process
	void writeHeader(DatabaseHeader header) override;

	// Returns the number of total blocks
	idx_t totalBlocks() override {
		return maxBlock_;
	}
	// Returns the number of free blocks
	idx_t freeBlocks() override {
		return freeList_.size();
	}
	// Load the free list from the file
	void loadFreeList();

private:
	void initialize(DatabaseHeader &header);

	// Return the blocks to which we will write the free list and modified blocks
	vector<block_id_t> getFreeListBlocks();

private:
	ClientContext &context_;
	// The active DatabaseHeader, either 0 (h1) or 1 (h2)
	uint8_t activeHeader_;
	// The path where the file is stored
	string path_;
	// The file handle
	file_handler_ptr_t handle_;
	// The buffer used to read/write to the headers
	FileBuffer headerBuffer_;
	// The list of free blocks that can be written to currently
	std::set<block_id_t> freeList_;
	// The list of multi-use blocks (i.e. blocks that have >1 reference in the file)
	// When a multi-use block is marked as modified, the reference count is decreased by 1 instead of directly
	// Appending the block to the modified_blocks list
	std::unordered_map<block_id_t, uint32_t> multiUseBlocks_;
	// The list of blocks that will be added to the free list
	std::unordered_set<block_id_t> modifiedBlocks_;
	// The current meta block id
	block_id_t metaBlock_;
	// The current maximum block id, this id will be given away first after the free_list runs out
	block_id_t maxBlock_;
	// The block id where the free list can be found
	block_id_t freeListId_;
	// The current header iteration count
	uint64_t iterationCount_;
	// Whether or not the db is opened in read-only mode
	bool readOnly_;
	// Whether or not to use Direct IO to read the blocks
	bool useDirectIO_;
};


}
