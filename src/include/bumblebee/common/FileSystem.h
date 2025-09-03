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
#include <functional>

#include "TypeDefs.h"

namespace bumblebee{
class FileSystem;
class ClientContext;

enum class FileCompressionType : std::uint8_t { AUTO_DETECT = 0, UNCOMPRESSED = 1, GZIP = 2 };
enum class FileLockType : uint8_t { NO_LOCK = 0, READ_LOCK = 1, WRITE_LOCK = 2 };

enum class FileType {
	// Regular file
	FILE_TYPE_REGULAR,
	// Directory
	FILE_TYPE_DIR,
	// FIFO named pipe
	FILE_TYPE_FIFO,
	// Socket
	FILE_TYPE_SOCKET,
	// Symbolic link
	FILE_TYPE_LINK,
	// Block device
	FILE_TYPE_BLOCKDEV,
	// Character device
	FILE_TYPE_CHARDEV,
	// Unknown or invalid file handle
	FILE_TYPE_INVALID,
};


class FileFlags {
public:
	// Open file with read access
	static constexpr uint8_t FILE_FLAGS_READ = 1 << 0;
	// Open file with write access
	static constexpr uint8_t FILE_FLAGS_WRITE = 1 << 1;
	// Use direct IO when reading/writing to the file
	static constexpr uint8_t FILE_FLAGS_DIRECT_IO = 1 << 2;
	// Create file if not exists, can only be used together with WRITE
	static constexpr uint8_t FILE_FLAGS_FILE_CREATE = 1 << 3;
	// Always create a new file. If a file exists, the file is truncated. Cannot be used together with CREATE.
	static constexpr uint8_t FILE_FLAGS_FILE_CREATE_NEW = 1 << 4;
	// Open file in append mode
	static constexpr uint8_t FILE_FLAGS_APPEND = 1 << 5;
};

// Handle the file operations
struct FileHandle {
public:
	FileHandle(FileSystem &fileSystem, string path) : fileSystem_(fileSystem), path_(path) {
	}
	FileHandle(const FileHandle &) = delete;
	virtual ~FileHandle() {
	}

	int64_t read(void *buffer, idx_t nr_bytes);
	int64_t write(void *buffer, idx_t nr_bytes);
	void read(void *buffer, idx_t nr_bytes, idx_t location);
	void write(void *buffer, idx_t nr_bytes, idx_t location);
	void seek(idx_t location);
	void reset();
	idx_t seekPosition();
	void sync();
	void truncate(int64_t new_size);
	string readLine();

	bool canSeek();
	bool onDiskFile();
	idx_t getFileSize();
	FileType getType();

protected:
	virtual void closeFile() = 0;

public:
	FileSystem &fileSystem_;
	string path_;
};

// generic File System class
class FileSystem {
public:
	virtual ~FileSystem() {
	}

public:
	static constexpr FileCompressionType DEFAULT_COMPRESSION = FileCompressionType::UNCOMPRESSED;
	static constexpr FileLockType DEFAULT_LOCK = FileLockType::NO_LOCK;

	virtual std::unique_ptr<FileHandle> openFile(const string &path, uint8_t flags, FileLockType lock = DEFAULT_LOCK,
	                                        FileCompressionType compression = DEFAULT_COMPRESSION);

	// Read exactly nr_bytes from the specified location in the file
	virtual void read(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location);
	// Write exactly nr_bytes to the specified location in the file.
	virtual void write(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location);
	// Read nr_bytes from the specified file into the buffer, moving the file pointer forward by nr_bytes.
	virtual int64_t read(FileHandle &handle, void *buffer, int64_t nr_bytes);
	// Write nr_bytes from the buffer into the file, moving the file pointer forward by nr_bytes.
	virtual int64_t write(FileHandle &handle, void *buffer, int64_t nr_bytes);

	// Returns the file size of a file handle, returns -1 on error
	virtual int64_t getFileSize(FileHandle &handle);
	// Returns the file last modified time of a file handle, returns timespec with zero on all attributes on error
	virtual time_t getLastModifiedTime(FileHandle &handle);
	// Returns the file last modified time of a file handle, returns timespec with zero on all attributes on error
	virtual FileType getFileType(FileHandle &handle);
	// Truncate a file to a maximum size of new_size, new_size should be smaller than or equal to the current size of
	// the file
	virtual void truncate(FileHandle &handle, int64_t new_size);

	// Check if a directory exists
	virtual bool directoryExists(const string &directory);
	// Create a directory if it does not exist
	virtual void createDirectory(const string &directory);
	// Recursively remove a directory and all files in it
	virtual void removeDirectory(const string &directory);
	// List files in a directory, invoking the callback method for each one with (filename, is_dir)
	virtual bool listFiles(const string &directory, const std::function<void(string, bool)> &callback);
	// Move a file from source path to the target, StorageManager relies on this being an atomic action for ACID
	// properties
	virtual void moveFile(const string &source, const string &target);
	// Check if a file exists
	virtual bool fileExists(const string &filename);
	// Remove a file from disk
	virtual void removeFile(const string &filename);
	// Sync a file handle to disk
	virtual void fileSync(FileHandle &handle);

	// Runs a glob on the file system, returning a list of matching files
	virtual vector<string> glob(const string &path);

	// Set the file pointer of a file handle to a specified location. Reads and writes will happen from this location
	virtual void seek(FileHandle &handle, idx_t location);
	// Reset a file to the beginning (equivalent to Seek(handle, 0) for simple files)
	virtual void reset(FileHandle &handle);
	virtual idx_t seekPosition(FileHandle &handle);

	// Whether or not we can seek into the file
	virtual bool canSeek();
	// Whether or not the FS handles plain files on disk. This is relevant for certain optimizations, as random reads
	// in a file on-disk are much cheaper than e.g. random reads in a file over the network
	virtual bool onDiskFile(FileHandle &handle);

protected:
	// Return the name of the filesytem.
	virtual std::string getName() const = 0;

public:
	// Sets the working directory
	static void setWorkingDirectory(const string &path);
	// Gets the working directory
	static string getWorkingDirectory();
	// Gets the users home directory
	static string getHomeDirectory();
	// Returns the system-available memory in bytes
	static idx_t getAvailableMemory();
	// Path separator for the current file system
	static string pathSeparator();
	// Join two paths together
	static string joinPath(const string &p1, const string &p2);
	// Convert separators in a path to the local separators (e.g. convert "/" into \\ on windows)
	static string convertSeparators(const string &path);
	// Extract the base name of a file (e.g. if the input is lib/example.dll the base name is example)
	static string extractBaseName(const string &path);

	// Return file system from context
	static FileSystem &getFileSystem(ClientContext &context);
};

using fs_ptr_t = std::unique_ptr<FileSystem>;

}
