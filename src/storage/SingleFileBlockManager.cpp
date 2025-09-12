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
#include "bumblebee/storage/SingleFileBlockManager.hpp"

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/serializer/BufferedDeserializer.hpp"
#include "bumblebee/common/serializer/BufferedSerializer.hpp"
#include "bumblebee/storage/MetaBlockReader.hpp"
#include "bumblebee/storage/MetaBlockWriter.hpp"


namespace bumblebee{

template <class T>
void serializeHeaderStructure(T header, data_ptr_t ptr) {
	BufferedSerializer ser(ptr, Storage::FILE_HEADER_SIZE);
	header.serialize(ser);
}

template <class T>
T deserializeHeaderStructure(data_ptr_t ptr) {
	BufferedDeserializer source(ptr, Storage::FILE_HEADER_SIZE);
	return T::deserialize(source);
}

SingleFileBlockManager::SingleFileBlockManager(ClientContext &context, string path_p, bool read_only, bool create_new,
                                               bool use_direct_io)
    : context_(context), path_(std::move(path_p)),
      headerBuffer_(context_.allocator_, FileBufferType::MANAGED_BUFFER, Storage::FILE_HEADER_SIZE), iterationCount_(0),
      readOnly_(read_only), useDirectIO_(use_direct_io) {
	uint8_t flags;
	FileLockType lock;
	if (read_only) {
		BB_ASSERT(!create_new);
		flags = FileFlags::FILE_FLAGS_READ;
		lock = FileLockType::READ_LOCK;
	} else {
		flags = FileFlags::FILE_FLAGS_WRITE | FileFlags::FILE_FLAGS_READ;
		lock = FileLockType::WRITE_LOCK;
		if (create_new) {
			flags |= FileFlags::FILE_FLAGS_FILE_CREATE;
		}
	}
	if (use_direct_io) {
		flags |= FileFlags::FILE_FLAGS_DIRECT_IO;
	}
	// open the RDBMS handle
	auto &fs = *context_.fileSystem_;
	handle_ = fs.openFile(path_, flags, lock);
	if (create_new) {
		// if we create a new file, we fill the metadata of the file
		// first fill in the new header
		headerBuffer_.clear();

		MainHeader main_header;
		main_header.version_number = VERSION_NUMBER;
		memset(main_header.flags, 0, sizeof(uint64_t) * 4);

		serializeHeaderStructure<MainHeader>(main_header, headerBuffer_.buffer_);
		// now write the header to the file
		headerBuffer_.checksumAndWrite(*handle_, 0);
		headerBuffer_.clear();

		// write the database headers
		// initialize meta_block and free_list to INVALID_BLOCK because the database file does not contain any actual
		// content yet
		DatabaseHeader h1,h2;
		// header 1
		h1.iteration = 0;
		h1.meta_block = INVALID_BLOCK;
		h1.free_list = INVALID_BLOCK;
		h1.block_count = 0;
		serializeHeaderStructure<DatabaseHeader>(h1, headerBuffer_.buffer_);
		headerBuffer_.checksumAndWrite(*handle_, Storage::FILE_HEADER_SIZE);
		// header 2
		h2.iteration = 0;
		h2.meta_block = INVALID_BLOCK;
		h2.free_list = INVALID_BLOCK;
		h2.block_count = 0;
		serializeHeaderStructure<DatabaseHeader>(h2, headerBuffer_.buffer_);
		headerBuffer_.checksumAndWrite(*handle_, Storage::FILE_HEADER_SIZE * 2);
		// ensure that writing to disk is completed before returning
		handle_->sync();
		iterationCount_ = 0;
		// we start with h2 as active_header, this way our initial write will be in h1
		activeHeader_ = 1;
		maxBlock_ = 0;
	} else {
		MainHeader::checkMagicBytes(*handle_);
		// otherwise, we check the metadata of the file
		headerBuffer_.readAndChecksum(*handle_, 0);
		MainHeader header = deserializeHeaderStructure<MainHeader>(headerBuffer_.buffer_);
		// check the version number
		if (header.version_number != VERSION_NUMBER) {
			ErrorHandler::errorParsing("Trying to read a database file but the file is corrupted :(\n");

		}

		// read the database headers from disk
		DatabaseHeader h1, h2;
		headerBuffer_.readAndChecksum(*handle_, Storage::FILE_HEADER_SIZE);
		h1 = deserializeHeaderStructure<DatabaseHeader>(headerBuffer_.buffer_);
		headerBuffer_.readAndChecksum(*handle_, Storage::FILE_HEADER_SIZE * 2);
		h2 = deserializeHeaderStructure<DatabaseHeader>(headerBuffer_.buffer_);
		// check the header with the highest iteration count
		if (h1.iteration > h2.iteration) {
			// h1 is active header
			activeHeader_ = 0;
			initialize(h1);
		} else {
			// h2 is active header
			activeHeader_ = 1;
			initialize(h2);
		}
	}
}

void SingleFileBlockManager::initialize(DatabaseHeader &header) {
	freeListId_ = header.free_list;
	metaBlock_ = header.meta_block;
	iterationCount_ = header.iteration;
	maxBlock_ = header.block_count;
}

void SingleFileBlockManager::loadFreeList() {
	if (readOnly_) {
		// no need to load free list for read only db
		return;
	}
	if (freeListId_ == INVALID_BLOCK) {
		// no free list
		return;
	}
	MetaBlockReader reader(context_, freeListId_);
	auto free_list_count = reader.read<uint64_t>();
	freeList_.clear();
	for (idx_t i = 0; i < free_list_count; i++) {
		freeList_.insert(reader.read<block_id_t>());
	}
	auto multi_use_blocks_count = reader.read<uint64_t>();
	multiUseBlocks_.clear();
	for (idx_t i = 0; i < multi_use_blocks_count; i++) {
		auto block_id = reader.read<block_id_t>();
		auto usage_count = reader.read<uint32_t>();
		multiUseBlocks_[block_id] = usage_count;
	}
}

void SingleFileBlockManager::startCheckpoint() {
}

bool SingleFileBlockManager::isRootBlock(block_id_t root) {
	return root == metaBlock_;
}

block_id_t SingleFileBlockManager::getFreeBlockId() {
	block_id_t block;
	if (!freeList_.empty()) {
		// free list is non empty
		// take an entry from the free list
		block = *freeList_.begin();
		// erase the entry from the free list again
		freeList_.erase(freeList_.begin());
	} else {
		block = maxBlock_++;
	}
	return block;
}

void SingleFileBlockManager::markBlockAsModified(block_id_t block_id) {
	BB_ASSERT(block_id >= 0);

	// check if the block is a multi-use block
	auto entry = multiUseBlocks_.find(block_id);
	if (entry != multiUseBlocks_.end()) {
		// it is! reduce the reference count of the block
		entry->second--;
		// check the reference count: is the block still a multi-use block?
		if (entry->second <= 1) {
			// no longer a multi-use block!
			multiUseBlocks_.erase(entry);
		}
		return;
	}
	modifiedBlocks_.insert(block_id);
}

void SingleFileBlockManager::increaseBlockReferenceCount(block_id_t block_id) {
	BB_ASSERT(freeList_.find(block_id) == freeList_.end());
	auto entry = multiUseBlocks_.find(block_id);
	if (entry != multiUseBlocks_.end()) {
		entry->second++;
	} else {
		multiUseBlocks_[block_id] = 2;
	}
}

block_id_t SingleFileBlockManager::getMetaBlock() {
	return metaBlock_;
}

block_ptr_t SingleFileBlockManager::createBlock(block_id_t block_id) {
	return std::make_unique<Block>(context_.allocator_, block_id);
}

void SingleFileBlockManager::read(Block &block) {
	BB_ASSERT(block.id >= 0);
	BB_ASSERT(std::find(freeList_.begin(), freeList_.end(), block.id) == freeList_.end());
	block.readAndChecksum(*handle_, BLOCK_START + block.id * Storage::BLOCK_ALLOC_SIZE);
}

void SingleFileBlockManager::write(FileBuffer &buffer, block_id_t block_id) {
	BB_ASSERT(block_id >= 0);
	buffer.checksumAndWrite(*handle_, BLOCK_START + block_id * Storage::BLOCK_ALLOC_SIZE);
}

vector<block_id_t> SingleFileBlockManager::getFreeListBlocks() {
	vector<block_id_t> free_list_blocks;

	if (!freeList_.empty() || !multiUseBlocks_.empty() || !modifiedBlocks_.empty()) {
		// there are blocks in the free list or multi_use_blocks
		// figure out how many blocks we need to write these to the file
		auto free_list_size = sizeof(uint64_t) + sizeof(block_id_t) * (freeList_.size() + modifiedBlocks_.size());
		auto multi_use_blocks_size =
		    sizeof(uint64_t) + (sizeof(block_id_t) + sizeof(uint32_t)) * multiUseBlocks_.size();
		auto total_size = free_list_size + multi_use_blocks_size;
		// because of potential alignment issues and needing to store a next pointer in a block we subtract
		// a bit from the max block size
		auto space_in_block = Storage::BLOCK_SIZE - 4 * sizeof(block_id_t);
		auto total_blocks = (total_size + space_in_block - 1) / space_in_block;
		BB_ASSERT(total_size > 0);
		BB_ASSERT(total_blocks > 0);

		// reserve the blocks that we are going to write
		// since these blocks are no longer free we cannot just include them in the free list!
		for (idx_t i = 0; i < total_blocks; i++) {
			auto block_id = getFreeBlockId();
			free_list_blocks.push_back(block_id);
		}
	}

	return free_list_blocks;
}

class FreeListBlockWriter : public MetaBlockWriter {
public:
	FreeListBlockWriter(ClientContext& context, vector<block_id_t> &free_list_blocks_p)
	    : MetaBlockWriter(context, free_list_blocks_p[0]), freeList_(free_list_blocks_p), index(1) {
	}

	vector<block_id_t> &freeList_;
	idx_t index;

protected:
	block_id_t getNextBlockId() override {
		if (index >= freeList_.size()) {
			ErrorHandler::outOfMemory(
			    "Free List Block Writer ran out of blocks, this means not enough blocks were allocated up front");
		}
		return freeList_[index++];
	}
};

void SingleFileBlockManager::writeHeader(DatabaseHeader header) {
	// set the iteration count
	header.iteration = ++iterationCount_;

	vector<block_id_t> free_list_blocks = getFreeListBlocks();

	// now handle_ the free list
	// add all modified blocks to the free list: they can now be written to again
	for (auto &block : modifiedBlocks_) {
		freeList_.insert(block);
	}
	modifiedBlocks_.clear();

	if (!free_list_blocks.empty()) {
		// there are blocks to write, either in the free_list or in the modified_blocks
		// we write these blocks specifically to the free_list_blocks
		// a normal MetaBlockWriter will fetch blocks to use from the free_list
		// but since we are WRITING the free_list, this behavior is sub-optimal

		FreeListBlockWriter writer(context_, free_list_blocks);

		BB_ASSERT(writer.block_->id == free_list_blocks[0]);
		header.free_list = writer.block_->id;
		for (auto &block_id : free_list_blocks) {
			modifiedBlocks_.insert(block_id);
		}

		writer.write<uint64_t>(freeList_.size());
		for (auto &block_id : freeList_) {
			writer.write<block_id_t>(block_id);
		}
		writer.write<uint64_t>(multiUseBlocks_.size());
		for (auto &entry : multiUseBlocks_) {
			writer.write<block_id_t>(entry.first);
			writer.write<uint32_t>(entry.second);
		}
		writer.flush();
	} else {
		// no blocks in the free list
		header.free_list = INVALID_BLOCK;
	}
	header.block_count = maxBlock_;

	if (!useDirectIO_) {
		// if we are not using Direct IO we need to fsync BEFORE we write the header to ensure that all the previous
		// blocks are written as well
		handle_->sync();
	}
	// set the header inside the buffer
	headerBuffer_.clear();
	store<DatabaseHeader>(header, headerBuffer_.buffer_);
	// now write the header to the file, active_header determines whether we write to h1 or h2
	// note that if active_header is h1 we write to h2, and vice versa
	headerBuffer_.checksumAndWrite(*handle_,
	                               activeHeader_ == 1 ? Storage::FILE_HEADER_SIZE : Storage::FILE_HEADER_SIZE * 2);
	// switch active header to the other header
	activeHeader_ = 1 - activeHeader_;
	//! Ensure the header write ends up on disk
	handle_->sync();
}

}
