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
#include <cstdint>

#include "Allocator.hpp"
#include "FileSystem.hpp"

namespace bumblebee{


enum class FileBufferType : uint8_t { BLOCK = 1, MANAGED_BUFFER = 2 };

// The FileBuffer represents a buffer that can be read or written to a Direct IO FileHandle.
class FileBuffer {
public:
	// Allocates a buffer of the specified size that is sector-aligned. bufsiz must be a multiple of
	// FileSystemConstants::FILE_BUFFER_BLOCK_SIZE. The content in this buffer can be written to FileHandles that have
	// been opened with DIRECT_IO on all operating systems, however, the entire buffer must be written to the file.
	// Note that the returned size is 8 bytes less than the allocation size to account for the checksum.
	// Note: the header contains the checksum
	FileBuffer(Allocator &allocator, FileBufferType type, uint64_t bufsiz);
	FileBuffer(FileBuffer &source, FileBufferType type);

	virtual ~FileBuffer();

	Allocator &allocator_;
	// The type of the buffer
	FileBufferType type_;
	// The buffer that users can write to
	data_ptr_t buffer_;
	// The size of the portion that users can write to, this is equivalent to internal_size - BLOCK_HEADER_SIZE
	uint64_t size_;

public:
	// Read into the FileBuffer from the specified location.
	void read(FileHandle &handle, uint64_t location);
	// Read into the FileBuffer from the specified location. Automatically verifies the checksum, and throws an
	// exception if the checksum does not match correctly.
	void readAndChecksum(FileHandle &handle, uint64_t location);
	// Write the contents of the FileBuffer to the specified location.
	void write(FileHandle &handle, uint64_t location);
	// Write the contents of the FileBuffer to the specified location. Automatically adds a checksum of the contents of
	// the filebuffer in front of the written data.
	void checksumAndWrite(FileHandle &handle, uint64_t location);

	void clear();

	void resize(uint64_t bufsiz);

	uint64_t allocSize() {
		return internalSize_;
	}

protected:
	// The pointer to the internal buffer that will be read or written, including the buffer header
	data_ptr_t internalBuffer_;
	// This is the size that is read or written to disk.
	uint64_t internalSize_;

private:
	// The buffer that was actually malloc'd, i.e. the pointer that must be freed when the FileBuffer is destroyed
	data_ptr_t mallocedBuffer_;
	uint64_t mallocedSize_;

protected:
	uint64_t getMallocedSize() {
		return mallocedSize_;
	}
	// Sets malloced size given the requested buffer size
	void setMallocedSize(uint64_t &bufsiz);
	// Constructs the Filebuffer object
	void construct(uint64_t bufsiz);
};

using file_buffer_ptr_t = std::unique_ptr<FileBuffer>;

}
