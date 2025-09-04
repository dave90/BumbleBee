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
#include "bumblebee/storage/BufferManager.h"

#include "bumblebee/ClientContext.h"
#include "bumblebee/common/ErrorHandler.h"

namespace bumblebee{

void BufferManager::setTemporaryDirectory(string new_dir) {
	if (tempDirectoryHandle_) {
		ErrorHandler::errorGeneric("Cannot switch temporary directory after the current one has been used");
	}
	this->tempDirectory_ = std::move(new_dir);
}

TemporaryDirectoryHandle::TemporaryDirectoryHandle(ClientContext &context, string path_p): context_(context), temp_directory(std::move(path_p)) {
	auto &fs = context_.fileSystem_;
	if (!temp_directory.empty()) {
		fs->createDirectory(temp_directory);
	}
}

TemporaryDirectoryHandle::~TemporaryDirectoryHandle() {
	auto &fs = context_.fileSystem_;
	if (!temp_directory.empty()) {
		fs->removeDirectory(temp_directory);
	}
}

BufferManager::BufferManager(ClientContext &context, string tmp, idx_t maximum_memory)
	: context_(context), currentMemory_(0), maximumMemory_(maximum_memory), tempDirectory_(std::move(tmp)),
	  queue_(std::make_unique<EvictionQueue>()), temporaryId_(MAXIMUM_BLOCK) {
}

BufferManager::~BufferManager() {
}



block_shared_ptr_t BufferManager::registerBlock(block_id_t block_id) {
	lock_guard lock(blocksLock_);
	// check if the block already exists
	auto entry = blocks_.find(block_id);
	if (entry != blocks_.end()) {
		// already exists: check if it hasn't expired yet
		auto existing_ptr = entry->second.lock();
		if (existing_ptr) {
			//! it hasn't! return it
			return existing_ptr;
		}
	}
	// create a new block pointer for this block
	auto result = block_shared_ptr_t(new BlockHandle(context_, block_id));
	// register the block pointer in the set of blocks as a weak pointer
	blocks_[block_id] = std::weak_ptr<BlockHandle>(result);
	return result;
}



block_shared_ptr_t BufferManager::registerMemory(idx_t block_size, bool can_destroy) {
	auto alloc_size = block_size + Storage::BLOCK_HEADER_SIZE;
	// first evict blocks until we have enough memory to store this buffer
	if (!evictBlocks(alloc_size, maximumMemory_)) {
		string error = "Could not allocate block of %lld bytes" + std::to_string(alloc_size);
		ErrorHandler::errorGeneric(error);
	}

	// allocate the buffer
	auto temp_id = ++temporaryId_;
	auto buffer = std::make_unique<ManagedBuffer>(context_, block_size, can_destroy, temp_id);

	// create a new block pointer for this block
	return make_shared<BlockHandle>(context_, temp_id, std::move(buffer), can_destroy, block_size);
}

buffer_handle_ptr_t BufferManager::allocate(idx_t block_size) {
	auto block = registerMemory(block_size, true);
	return pin(block);
}


block_shared_ptr_t BufferManager::convertToPersistent(BlockManager &block_manager, block_id_t block_id,
															   block_shared_ptr_t old_block) {

	// pin the old block to ensure we have it loaded in memory
	auto old_handle = pin(old_block);
	BB_ASSERT(old_block->state_ == BlockState::BLOCK_LOADED);
	BB_ASSERT(old_block->buffer_);

	// register a block with the new block id
	auto new_block = registerBlock(block_id);
	BB_ASSERT(new_block->state_ == BlockState::BLOCK_UNLOADED);
	BB_ASSERT(new_block->readers_ == 0);

	// move the data from the old block into data for the new block
	new_block->state_ = BlockState::BLOCK_LOADED;
	new_block->buffer_ = block_ptr_t( new Block(*old_block->buffer_, block_id));

	// clear the old buffer and unload it
	old_handle.reset();
	old_block->buffer_.reset();
	old_block->state_ = BlockState::BLOCK_UNLOADED;
	old_block->memoryUsage_ = 0;
	old_block.reset();

	// persist the new block to disk
	block_manager.write(*new_block->buffer_, block_id);

	addToEvictionQueue(new_block);

	return new_block;
}

void BufferManager::reAllocate(block_shared_ptr_t &handle, idx_t block_size) {
	BB_ASSERT(block_size >= Storage::BLOCK_SIZE);
	lock_guard lock(handle->lock_);
	BB_ASSERT(handle->state_ == BlockState::BLOCK_LOADED);
	auto alloc_size = block_size + Storage::BLOCK_HEADER_SIZE;
	int64_t required_memory = alloc_size - handle->memoryUsage_;
	if (required_memory == 0) {
		return;
	}
	if (required_memory > 0) {
		// evict blocks until we have space to resize this block
		if (!evictBlocks(required_memory, maximumMemory_)) {
			string error = "failed to resize block from" + std::to_string(handle->memoryUsage_) + " to " +std::to_string(alloc_size)  ;
			ErrorHandler::outOfMemory(error);
		}
	} else {
		// no need to evict blocks
		currentMemory_ -= idx_t(-required_memory);
	}

	// resize and adjust current memory
	handle->buffer_->resize(block_size);
	handle->memoryUsage_ = alloc_size;
}

buffer_handle_ptr_t BufferManager::pin(block_shared_ptr_t &handle) {
	idx_t required_memory;
	{
		// lock the block
		lock_guard lock(handle->lock_);
		// check if the block is already loaded
		if (handle->state_ == BlockState::BLOCK_LOADED) {
			// the block is loaded, increment the reader count and return a pointer to the handle
			handle->readers_++;
			return handle->load();
		}
		required_memory = handle->memoryUsage_;
	}
	// evict blocks until we have space for the current block
	if (!evictBlocks(required_memory, maximumMemory_)) {
			string error = "failed to resize block from" + std::to_string(handle->memoryUsage_) + " to " +std::to_string(required_memory)  ;
			ErrorHandler::outOfMemory(error);
	}
	// lock the handle again and repeat the check (in case anybody loaded in the meantime)
	lock_guard lock(handle->lock_);
	// check if the block is already loaded
	if (handle->state_ == BlockState::BLOCK_LOADED) {
		// the block is loaded, increment the reader count and return a pointer to the handle
		handle->readers_++;
		currentMemory_ -= required_memory;
		return handle->load();
	}
	// now we can actually load the current block
	BB_ASSERT(handle->readers_ == 0);
	handle->readers_ = 1;
	return handle->load();
}

void BufferManager::addToEvictionQueue(block_shared_ptr_t &handle) {
	BB_ASSERT(handle->readers_ == 0);
	handle->evictionTimestamp_++;
	queue_->q.enqueue(make_unique<BufferEvictionNode>(std::weak_ptr(handle), handle->evictionTimestamp_));
}

void BufferManager::unpin(block_shared_ptr_t &handle) {
	lock_guard lock(handle->lock_);
	BB_ASSERT(handle->readers_ > 0);
	handle->readers_--;
	if (handle->readers_ == 0) {
		addToEvictionQueue(handle);
	}
}

bool BufferManager::evictBlocks(idx_t extra_memory, idx_t memory_limit) {
	purgeQueue();

	std::unique_ptr<BufferEvictionNode> node;
	currentMemory_ += extra_memory;
	while (currentMemory_ > memory_limit) {
		// get a block to unpin from the queue
		if (!queue_->q.try_dequeue(node)) {
			currentMemory_ -= extra_memory;
			return false;
		}
		// get a reference to the underlying block pointer
		auto handle = node->tryGetBlockHandle();
		if (!handle) {
			continue;
		}
		// we might be able to free this block: grab the mutex and check if we can free it
		lock_guard lock(handle->lock_);
		if (!node->canUnload(*handle)) {
			// something changed in the mean-time, bail out
			continue;
		}
		// yeee, we can unload the block
		// release the memory and mark the block as unloaded
		handle->unload();
	}
	return true;
}

void BufferManager::purgeQueue() {
	std::unique_ptr<BufferEvictionNode> node;
	while (true) {
		if (!queue_->q.try_dequeue(node)) {
			break;
		}
		auto handle = node->tryGetBlockHandle();
		if (!handle) {
			continue;
		}
		queue_->q.enqueue(std::move(node));
		break;
	}
}

void BufferManager::unregisterBlock(block_id_t block_id, bool can_destroy) {
	if (block_id >= MAXIMUM_BLOCK) {
		// in-memory buffer: destroy the buffer
		if (!can_destroy) {
			// buffer could have been offloaded to disk: remove the file
			deleteTemporaryFile(block_id);
		}
	} else {
		lock_guard lock(blocksLock_);
		// on-disk block: erase from list of blocks in manager
		blocks_.erase(block_id);
	}
}
void BufferManager::setLimit(idx_t limit) {
	lock_guard l_lock(limitLock_);
	// try to evict until the limit is reached
	if (!evictBlocks(0, limit)) {
		ErrorHandler::outOfMemory("Failed to change memory limit.");
	}
	idx_t old_limit = maximumMemory_;
	// set the global maximum memory to the new limit if successful
	maximumMemory_ = limit;
	// evict again
	if (!evictBlocks(0, limit)) {
		// failed: go back to old limit
		maximumMemory_ = old_limit;
		ErrorHandler::outOfMemory("Failed to change memory limit.");
	}
}

string BufferManager::getTemporaryPath(block_id_t id) {
	auto &fs = context_.fileSystem_;
	return fs->joinPath(tempDirectory_, std::to_string(id) + ".block");
}

void BufferManager::requireTemporaryDirectory() {
	if (tempDirectory_.empty()) {
		ErrorHandler::outOfMemory("Failed to create temporary directory. Cannot write buffer because no temporary directory is specified!");
	}
	lock_guard temp_handle_guard(tempHandleLock_);
	if (!tempDirectoryHandle_) {
		// temp directory has not been created yet: initialize it
		tempDirectoryHandle_ = std::make_unique<TemporaryDirectoryHandle>(context_, tempDirectory_);
	}
}

void BufferManager::writeTemporaryBuffer(ManagedBuffer &buffer) {
	requireTemporaryDirectory();

	BB_ASSERT(buffer.size_ >= Storage::BLOCK_SIZE);
	// get the path to write to
	auto path = getTemporaryPath(buffer.id_);
	// create the file and write the size followed by the buffer contents
	auto &fs = context_.fileSystem_;
	auto handle = fs->openFile(path, FileFlags::FILE_FLAGS_WRITE | FileFlags::FILE_FLAGS_FILE_CREATE);
	handle->write(&buffer.size_, sizeof(idx_t), 0);
	buffer.write(*handle, sizeof(idx_t));
}

file_buffer_ptr_t BufferManager::readTemporaryBuffer(block_id_t id) {
	BB_ASSERT(!tempDirectory_.empty());
	BB_ASSERT(tempDirectoryHandle_.get());
	idx_t block_size;
	// open the temporary file and read the size
	auto path = getTemporaryPath(id);
	auto &fs = context_.fileSystem_;
	auto handle = fs->openFile(path, FileFlags::FILE_FLAGS_READ);
	handle->read(&block_size, sizeof(idx_t), 0);

	// now allocate a buffer of this size and read the data into that buffer
	auto buffer = std::make_unique<ManagedBuffer>(context_, block_size, false, id);
	buffer->read(*handle, sizeof(idx_t));

	handle.reset();
	deleteTemporaryFile(id);
	return std::move(buffer);
}

void BufferManager::deleteTemporaryFile(block_id_t id) {
	if (tempDirectory_.empty() || !tempDirectoryHandle_) {
		return;
	}
	auto &fs = context_.fileSystem_;
	auto path = getTemporaryPath(id);
	if (fs->fileExists(path)) {
		fs->removeFile(path);
	}
}

}
