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
#include "bumblebee/common/FileBuffer.hpp"

#include "CLI11.hpp"
#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{


FileBuffer::FileBuffer(Allocator &allocator, FileBufferType type, uint64_t bufsiz)
    : allocator_(allocator), type_(type), mallocedBuffer_(nullptr) {
	setMallocedSize(bufsiz);
	mallocedBuffer_ = allocator.allocateData(mallocedSize_);
	construct(bufsiz);
}

FileBuffer::FileBuffer(FileBuffer &source, FileBufferType type_p) : allocator_(source.allocator_), type_(type_p) {
	// take over the structures of the source buffer
	buffer_ = source.buffer_;
	size_ = source.size_;
	internalBuffer_ = source.internalBuffer_;
	internalSize_ = source.internalSize_;
	mallocedBuffer_ = source.mallocedBuffer_;
	mallocedSize_ = source.mallocedSize_;

	source.buffer_ = nullptr;
	source.size_ = 0;
	source.internalBuffer_ = nullptr;
	source.internalSize_ = 0;
	source.mallocedBuffer_ = nullptr;
	source.mallocedSize_ = 0;
}

FileBuffer::~FileBuffer() {
	allocator_.freeData(mallocedBuffer_, mallocedSize_);
}

void FileBuffer::setMallocedSize(uint64_t &bufsiz) {
	// make room for the block header (if this is not the db file header)
	if (type_ == FileBufferType::MANAGED_BUFFER && bufsiz != Storage::FILE_HEADER_SIZE) {
		bufsiz += Storage::BLOCK_HEADER_SIZE;
	}
	if (type_ == FileBufferType::BLOCK) {
		const int sector_size = Storage::SECTOR_SIZE;
		// round up to the nearest sector_size
		if (bufsiz % sector_size != 0) {
			bufsiz += sector_size - (bufsiz % sector_size);
		}
		BB_ASSERT(bufsiz % sector_size == 0);
		BB_ASSERT(bufsiz >= sector_size);
		// we add (sector_size - 1) to ensure that we can align the buffer to sector_size
		mallocedSize_ = bufsiz + (sector_size - 1);
	} else {
		mallocedSize_ = bufsiz;
	}
}

void FileBuffer::construct(uint64_t bufsiz) {
	if (!mallocedBuffer_) {
		throw std::bad_alloc();
	}
	if (type_ == FileBufferType::BLOCK) {
		const int sector_size = Storage::SECTOR_SIZE;
		// round to multiple of sector_size
		uint64_t num = (uint64_t)mallocedBuffer_;
		uint64_t remainder = num % sector_size;
		if (remainder != 0) {
			num = num + sector_size - remainder;
		}
		BB_ASSERT(num % sector_size == 0);
		BB_ASSERT(num + bufsiz <= ((uint64_t)mallocedBuffer_ + bufsiz + (sector_size - 1)));
		BB_ASSERT(num >= (uint64_t)mallocedBuffer_);
		// construct the FileBuffer object
		internalBuffer_ = (data_ptr_t)num;
		internalSize_ = bufsiz;
	} else {
		internalBuffer_ = mallocedBuffer_;
		internalSize_ = mallocedSize_;
	}
	buffer_ = internalBuffer_ + Storage::BLOCK_HEADER_SIZE;
	size_ = internalSize_ - Storage::BLOCK_HEADER_SIZE;
}

void FileBuffer::resize(uint64_t bufsiz) {
	BB_ASSERT(type_ == FileBufferType::MANAGED_BUFFER);
	setMallocedSize(bufsiz);
	mallocedBuffer_ = allocator_.reallocateData(mallocedBuffer_, mallocedSize_);
	construct(bufsiz);
}

void FileBuffer::read(FileHandle &handle, uint64_t location) {
	handle.read(internalBuffer_, internalSize_, location);
}

void FileBuffer::readAndChecksum(FileHandle &handle, uint64_t location) {
	// read the buffer from disk
	read(handle, location);
	// compute the checksum
	auto stored_checksum = load<uint64_t>(internalBuffer_);
	uint64_t computed_checksum = calcChecksum(buffer_, size_);
	// verify the checksum
	if (stored_checksum != computed_checksum) {
		string error = "Corrupt database file: computed checksum" + std::to_string(stored_checksum) + " does not match stored checksum in block: "+std::to_string(computed_checksum);
		ErrorHandler::errorGeneric(error);
	}
}

void FileBuffer::write(FileHandle &handle, uint64_t location) {
	handle.write(internalBuffer_, internalSize_, location);
}

void FileBuffer::checksumAndWrite(FileHandle &handle, uint64_t location) {
	// compute the checksum and write it to the start of the buffer (if not temp buffer)
	uint64_t cs = calcChecksum(buffer_, size_);
	store<uint64_t>(cs, internalBuffer_);
	// now write the buffer
	write(handle, location);
}

void FileBuffer::clear() {
	memset(internalBuffer_, 0, internalSize_);
}

}
