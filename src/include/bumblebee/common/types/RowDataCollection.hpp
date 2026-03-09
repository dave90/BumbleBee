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
#include "SelectionVector.hpp"
#include "Vector.hpp"
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/storage/BufferManager.hpp"

namespace bumblebee{


struct RowDataBlock {
	RowDataBlock(BufferManager &buffer_manager, idx_t capacity, idx_t entry_size)
	    : capacity_(capacity), entrySize_(entry_size), count_(0), byteOffset_(0) {
		block_ = buffer_manager.registerMemory(capacity * entry_size, false);
	}
	// The buffer block handle
	block_handle_shared_ptr_t block_;
	// Capacity (number of entries) and entry size that fit in this block
	idx_t capacity_;
	const idx_t entrySize_;
	// Number of entries currently in this block
	idx_t count_;
	// Write offset (if variable size entries)
	idx_t byteOffset_;
};

struct BlockAppendEntry {
	BlockAppendEntry(data_ptr_t baseptr, idx_t count) : baseptr_(baseptr), count_(count) {
	}
	data_ptr_t baseptr_;
	idx_t count_;
};

class RowDataCollection {
public:
	RowDataCollection(BufferManager &buffer_manager, idx_t block_capacity, idx_t entry_size, bool keep_pinned = false);

	// BufferManager
	BufferManager &bufferManager_;
	// The total number of stored entries
	idx_t count_;
	// The number of entries per block
	idx_t blockCapacity_;
	// Size of entries in the blocks
	idx_t entrySize_;
	// The blocks holding the main data
	vector<RowDataBlock> blocks_;
	// The blocks that this collection currently has pinned
	vector<buffer_handle_ptr_t> pinnedBlocks_;

public:
	vector<buffer_handle_ptr_t> build(idx_t added_count, data_ptr_t key_locations[], idx_t entry_sizes[],
	                                       const SelectionVector *sel = &FlatVector::INCREMENTAL_SELECTION_VECTOR);

	void merge(RowDataCollection &other);

	// The size (in bytes) of this RowDataCollection if it were stored in a single block
	idx_t sizeInBytes() const {
		idx_t bytes = 0;
		if (entrySize_ == 1) {
			for (auto &block : blocks_) {
				bytes += block.byteOffset_;
			}
		} else {
			bytes = count_ * entrySize_;
		}
		return maxValue(bytes, (idx_t)Storage::BLOCK_SIZE);
	}

private:
	idx_t appendToBlock(RowDataBlock &block, BufferHandle &handle, vector<BlockAppendEntry> &append_entries,
	                    idx_t remaining, idx_t entry_sizes[]);

	mutex rdcLock_;

	// Whether the blocks should stay pinned (necessary for e.g. a heap)
	const bool keepPinned_;
};


}
