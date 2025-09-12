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
#include "BlockHandle.hpp"
#include "BlockManager.hpp"
#include "BufferHandle.hpp"
#include "StorageInfo.hpp"
#include "ManagedBuffer.hpp"
#include "bumblebee/common/Mutex.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "concurrentqueue/concurrentqueue.h"

namespace bumblebee{

class ClientContext;


struct BufferEvictionNode {

	BufferEvictionNode(std::weak_ptr<BlockHandle> handle_p, idx_t timestamp_p)
		: handle_(std::move(handle_p)), timestamp_(timestamp_p) {
		BB_ASSERT(!handle_.expired());
	}

	std::weak_ptr<BlockHandle> handle_;
	idx_t timestamp_;

	bool canUnload(BlockHandle &handle_p) {
		if (timestamp_ != handle_p.evictionTimestamp_) {
			// handle was used in between
			return false;
		}
		return handle_p.canUnload();
	}

	std::shared_ptr<BlockHandle> tryGetBlockHandle() {
		auto handle_p = handle_.lock();
		if (!handle_p) {
			// BlockHandle has been destroyed
			return nullptr;
		}
		if (!canUnload(*handle_p)) {
			// handle was used in between
			return nullptr;
		}
		// this is the latest node in the queue with this handle
		return handle_p;
	}
};


using eviction_queue_t = moodycamel::ConcurrentQueue<std::unique_ptr<BufferEvictionNode>>;

struct EvictionQueue {
	eviction_queue_t q;
};

class TemporaryDirectoryHandle {
public:
	TemporaryDirectoryHandle(ClientContext &context, string path_p);

	~TemporaryDirectoryHandle();

private:
	ClientContext &context_;
	string temp_directory;
};

// The buffer manager is in charge of handling memory management for the database. It hands out memory buffers that can
// be used by the database internally.
class BufferManager {
	friend class BufferHandle;
	friend class BlockHandle;

public:
	BufferManager(ClientContext &context, string temp_directory, idx_t maximum_memory);
	~BufferManager();

	// Register a block with the given block id in the base file
	block_handle_shared_ptr_t registerBlock(block_id_t block_id);

	// Register an in-memory buffer of arbitrary size, as long as it is >= BLOCK_SIZE. can_destroy signifies whether or
	// not the buffer can be destroyed when unpinned, or whether or not it needs to be written to a temporary file so
	// it can be reloaded. The resulting buffer will already be allocated, but needs to be pinned in order to be used.
	block_handle_shared_ptr_t registerMemory(idx_t block_size, bool can_destroy);

	// Convert an existing in-memory buffer into a persistent disk-backed block
	block_handle_shared_ptr_t convertToPersistent(BlockManager &block_manager, block_id_t block_id,
	                                            block_handle_shared_ptr_t old_block);

	// Allocate an in-memory buffer with a single pin.
	// The allocated memory is released when the buffer handle is destroyed.
	buffer_handle_ptr_t allocate(idx_t block_size);

	// Reallocate an in-memory buffer that is pinned.
	void reAllocate(block_handle_shared_ptr_t &handle, idx_t block_size);

	buffer_handle_ptr_t pin(block_handle_shared_ptr_t &handle);
	void unpin(block_handle_shared_ptr_t &handle);

	void unregisterBlock(block_id_t block_id, bool can_destroy);

	// Set a new memory limit to the buffer manager, throws an exception if the new limit is too low and not enough
	// blocks can be evicted
	void setLimit(idx_t limit = (idx_t)-1);


	idx_t getUsedMemory() {
		return currentMemory_;
	}
	idx_t getMaxMemory() {
		return maximumMemory_;
	}

	const string &getTemporaryDirectory() {
		return tempDirectory_;
	}

	void setTemporaryDirectory(string new_dir);

private:
	// Evict blocks until the currently used memory + extra_memory fit, returns false if this was not possible
	// (i.e. not enough blocks could be evicted)
	bool evictBlocks(idx_t extra_memory, idx_t memory_limit);

	// Garbage collect eviction queue
	void purgeQueue();

	// Write a temporary buffer to disk
	void writeTemporaryBuffer(ManagedBuffer &buffer);
	// Read a temporary buffer from disk
	file_buffer_ptr_t readTemporaryBuffer(block_id_t id);
	// Get the path of the temporary buffer
	string getTemporaryPath(block_id_t id);

	void deleteTemporaryFile(block_id_t id);

	void requireTemporaryDirectory();

	void addToEvictionQueue(block_handle_shared_ptr_t &handle);


private:
	ClientContext &context_;
	// The lock for changing the memory limit
	mutex limitLock_;
	// The current amount of memory that is occupied by the buffer manager (in bytes)
	atomic<idx_t> currentMemory_;
	// The maximum amount of memory that the buffer manager can keep (in bytes)
	atomic<idx_t> maximumMemory_;
	// The directory name where temporary files are stored
	string tempDirectory_;
	// Lock for creating the temp handle
	mutex tempHandleLock_;
	// Handle for the temporary directory
	std::unique_ptr<TemporaryDirectoryHandle> tempDirectoryHandle_;
	// The lock for the set of blocks
	mutex blocksLock_;
	// A mapping of block id -> BlockHandle
	std::unordered_map<block_id_t, std::weak_ptr<BlockHandle>> blocks_;
	// Eviction queue
	std::unique_ptr<EvictionQueue> queue_;
	// The temporary id used for managed buffers
	atomic<block_id_t> temporaryId_;
};

using buffer_mngr_ptr_ptr_t = std::unique_ptr<BufferManager>;

}
