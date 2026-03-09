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
#include "bumblebee/common/types/RowDataCollection.hpp"

#include "bumblebee/common/Helper.hpp"

namespace bumblebee{


RowDataCollection::RowDataCollection(BufferManager &buffer_manager, idx_t block_capacity, idx_t entry_size,
                                     bool keep_pinned)
    : bufferManager_(buffer_manager), count_(0), blockCapacity_(block_capacity), entrySize_(entry_size),
      keepPinned_(keep_pinned) {
	BB_ASSERT(block_capacity * entry_size >= Storage::BLOCK_SIZE);
}

idx_t RowDataCollection::appendToBlock(RowDataBlock &block, BufferHandle &handle,
                                       vector<BlockAppendEntry> &append_entries, idx_t remaining, idx_t entry_sizes[]) {
	idx_t append_count = 0;
	data_ptr_t dataptr;
	if (entry_sizes) {
		BB_ASSERT(entrySize_ == 1);
		// compute how many entries fit if entry size is variable
		dataptr = handle.ptr() + block.byteOffset_;
		for (idx_t i = 0; i < remaining; i++) {
			if (block.byteOffset_ + entry_sizes[i] > block.capacity_) {
				if (block.count_ == 0 && append_count == 0 && entry_sizes[i] > block.capacity_) {
					// special case: single entry is bigger than block capacity
					// resize current block to fit the entry, append it, and move to the next block
					block.capacity_ = entry_sizes[i];
					bufferManager_.reAllocate(block.block_, block.capacity_);
					dataptr = handle.ptr();
					append_count++;
					block.byteOffset_ += entry_sizes[i];
				}
				break;
			}
			append_count++;
			block.byteOffset_ += entry_sizes[i];
		}
	} else {
		append_count = minValue<idx_t>(remaining, block.capacity_ - block.count_);
		dataptr = handle.ptr() + block.count_ * entrySize_;
	}
	append_entries.emplace_back(dataptr, append_count);
	block.count_ += append_count;
	return append_count;
}

vector<buffer_handle_ptr_t> RowDataCollection::build(idx_t added_count, data_ptr_t key_locations[],
                                                          idx_t entry_sizes[], const SelectionVector *sel) {
	vector<buffer_handle_ptr_t> handles;
	vector<BlockAppendEntry> append_entries;

	// first allocate space of where to serialize the keys and payload columns
	idx_t remaining = added_count;
	{
		// first append to the last block (if any)
		lock_guard append_lock(rdcLock_);
		count_ += added_count;

		if (!blocks_.empty()) {
			auto &last_block = blocks_.back();
			if (last_block.count_ < last_block.capacity_) {
				// last block has space: pin the buffer of this block
				auto handle = bufferManager_.pin(last_block.block_);
				// now append to the block
				idx_t append_count = appendToBlock(last_block, *handle, append_entries, remaining, entry_sizes);
				remaining -= append_count;
				handles.push_back(std::move(handle));
			}
		}
		while (remaining > 0) {
			// now for the remaining data, allocate new buffers to store the data and append there
			RowDataBlock new_block(bufferManager_, blockCapacity_, entrySize_);
			auto handle = bufferManager_.pin(new_block.block_);

			// offset the entry sizes array if we have added entries already
			idx_t *offset_entry_sizes = entry_sizes ? entry_sizes + added_count - remaining : nullptr;

			idx_t append_count = appendToBlock(new_block, *handle, append_entries, remaining, offset_entry_sizes);
			BB_ASSERT(new_block.count_ > 0);
			remaining -= append_count;

			blocks_.push_back(std::move(new_block));
			if (keepPinned_) {
				pinnedBlocks_.push_back(std::move(handle));
			} else {
				handles.push_back(std::move(handle));
			}
		}
	}
	// now set up the key_locations based on the append entries
	idx_t append_idx = 0;
	for (auto &append_entry : append_entries) {
		idx_t next = append_idx + append_entry.count_;
		if (entry_sizes) {
			for (; append_idx < next; append_idx++) {
				key_locations[append_idx] = append_entry.baseptr_;
				append_entry.baseptr_ += entry_sizes[append_idx];
			}
		} else {
			for (; append_idx < next; append_idx++) {
				auto idx = sel->getIndex(append_idx);
				key_locations[idx] = append_entry.baseptr_;
				append_entry.baseptr_ += entrySize_;
			}
		}
	}
	// return the unique pointers to the handles because they must stay pinned
	return handles;
}

void RowDataCollection::merge(RowDataCollection &other) {
	RowDataCollection temp(bufferManager_, Storage::BLOCK_SIZE, 1);
	{
		//	One lock at a time to avoid deadlocks
		lock_guard read_lock(other.rdcLock_);
		temp.count_ = other.count_;
		temp.blockCapacity_ = other.blockCapacity_;
		temp.entrySize_ = other.entrySize_;
		temp.blocks_ = std::move(other.blocks_);
		other.count_ = 0;
	}

	lock_guard write_lock(rdcLock_);
	count_ += temp.count_;
	blockCapacity_ = maxValue(blockCapacity_, temp.blockCapacity_);
	entrySize_ = maxValue(entrySize_, temp.entrySize_);
	for (auto &block : temp.blocks_) {
		blocks_.emplace_back(std::move(block));
	}
	for (auto &handle : temp.pinnedBlocks_) {
		pinnedBlocks_.emplace_back(std::move(handle));
	}
}
}
