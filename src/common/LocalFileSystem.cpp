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
#include "bumblebee/common/LocalFileSystem.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{

struct UnixFileHandle : public FileHandle {
public:
    UnixFileHandle(FileSystem &fs, string path, int fd) : FileHandle(fs, std::move(path)), fd(fd) {
    }
    ~UnixFileHandle() override {
        closeFile();
    }

protected:
    void closeFile() override {
        if (fd != -1) {
            close(fd);
        }
    };

public:
    int fd;
};



static FileType getFileTypeInternal(int fd) { // LCOV_EXCL_START
	struct stat s;
	if (fstat(fd, &s) == -1) {
		return FileType::FILE_TYPE_INVALID;
	}
	switch (s.st_mode & S_IFMT) {
		case S_IFBLK:
			return FileType::FILE_TYPE_BLOCKDEV;
		case S_IFCHR:
			return FileType::FILE_TYPE_CHARDEV;
		case S_IFIFO:
			return FileType::FILE_TYPE_FIFO;
		case S_IFDIR:
			return FileType::FILE_TYPE_DIR;
		case S_IFLNK:
			return FileType::FILE_TYPE_LINK;
		case S_IFREG:
			return FileType::FILE_TYPE_REGULAR;
		case S_IFSOCK:
			return FileType::FILE_TYPE_SOCKET;
		default:
			return FileType::FILE_TYPE_INVALID;
	}
} // LCOV_EXCL_STOP



LocalFileSystem::~LocalFileSystem() {}


string LocalFileSystem::getName() const {
	return "LocalFileSystem";
}

std::unique_ptr<FileHandle> LocalFileSystem::openFile(const string &path, uint8_t flags, FileLockType lock_type,
                                                      FileCompressionType compression) {
	if (compression != FileCompressionType::UNCOMPRESSED) {
		ErrorHandler::errorNotImplemented("Unsupported compression type for default file system");
		return nullptr;
	}

	int open_flags = 0;
	int rc;
	bool open_read = flags & FileFlags::FILE_FLAGS_READ;
	bool open_write = flags & FileFlags::FILE_FLAGS_WRITE;
	if (open_read && open_write) {
		open_flags = O_RDWR;
	} else if (open_read) {
		open_flags = O_RDONLY;
	} else if (open_write) {
		open_flags = O_WRONLY;
	} else {
		ErrorHandler::errorNotImplemented("READ, WRITE or both should be specified when opening a file");
	}
	if (open_write) {
		// need Read or Write
		BB_ASSERT(flags & FileFlags::FILE_FLAGS_WRITE);
		open_flags |= O_CLOEXEC;
		if (flags & FileFlags::FILE_FLAGS_FILE_CREATE) {
			open_flags |= O_CREAT;
		} else if (flags & FileFlags::FILE_FLAGS_FILE_CREATE_NEW) {
			open_flags |= O_CREAT | O_TRUNC;
		}
		if (flags & FileFlags::FILE_FLAGS_APPEND) {
			open_flags |= O_APPEND;
		}
	}
	if (flags & FileFlags::FILE_FLAGS_DIRECT_IO) {
#if defined(__DARWIN__) || defined(__APPLE__) || defined(__OpenBSD__)
		// OSX does not have O_DIRECT, instead we need to use fcntl afterwards to support direct IO
		open_flags |= O_SYNC;
#else
		open_flags |= O_DIRECT | O_SYNC;
#endif
	}

	int fd = open(path.c_str(), open_flags, 0666);
	if (fd == -1) {
		string error = "Cannot open file \"" + path +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
	// #if defined(__DARWIN__) || defined(__APPLE__)
	// 	if (flags & FileFlags::FILE_FLAGS_DIRECT_IO) {
	// 		// OSX requires fcntl for Direct IO
	// 		rc = fcntl(fd, F_NOCACHE, 1);
	// 		if (fd == -1) {
	// 			throw IOException("Could not enable direct IO for file \"%s\": %s", path, strerror(errno));
	// 		}
	// 	}
	// #endif
	if (lock_type != FileLockType::NO_LOCK) {
		// set lock on file
		// but only if it is not an input/output stream
		auto file_type = getFileTypeInternal(fd);
		if (file_type != FileType::FILE_TYPE_FIFO && file_type != FileType::FILE_TYPE_SOCKET) {
			struct flock fl;
			memset(&fl, 0, sizeof fl);
			fl.l_type = lock_type == FileLockType::READ_LOCK ? F_RDLCK : F_WRLCK;
			fl.l_whence = SEEK_SET;
			fl.l_start = 0;
			fl.l_len = 0;
			rc = fcntl(fd, F_SETLK, &fl);
			if (rc == -1) {
				string error = "Cannot open file \"" + path +"\":"+ strerror(errno);
				ErrorHandler::errorGeneric(error);
			}
		}
	}
	return make_unique<UnixFileHandle>(*this, path, fd);
}

void setFilePointer(FileHandle &handle, idx_t location) {
	int fd = ((UnixFileHandle &)handle).fd;
	off_t offset = lseek(fd, location, SEEK_SET);
	if (offset == (off_t)-1) {
		string error = "Could not seek file \"" + handle.path_ +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
}

idx_t getFilePointer(FileHandle &handle) {
	int fd = ((UnixFileHandle &)handle).fd;
	off_t position = lseek(fd, 0, SEEK_CUR);
	if (position == (off_t)-1) {
		string error = "Could not get file position file \"" + handle.path_ +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
	return position;
}

void LocalFileSystem::read(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) {
	int fd = ((UnixFileHandle &)handle).fd;
	int64_t bytes_read = pread(fd, buffer, nr_bytes, location);
	if (bytes_read == -1 || bytes_read != nr_bytes) {
		string error = "Could not read file \"" + handle.path_ +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
}

int64_t LocalFileSystem::read(FileHandle &handle, void *buffer, int64_t nr_bytes) {
	int fd = ((UnixFileHandle &)handle).fd;
	int64_t bytes_read = ::read(fd, buffer, nr_bytes);
	if (bytes_read == -1) {
		string error = "Could not read file \"" + handle.path_ +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
	return bytes_read;
}

void LocalFileSystem::write(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) {
	int fd = ((UnixFileHandle &)handle).fd;
	int64_t bytes_written = pwrite(fd, buffer, nr_bytes, location);
	if (bytes_written == -1 || bytes_written != nr_bytes) {
		string error = "Could not write file \"" + handle.path_ +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
}

int64_t LocalFileSystem::write(FileHandle &handle, void *buffer, int64_t nr_bytes) {
	int fd = ((UnixFileHandle &)handle).fd;
	int64_t bytes_written = ::write(fd, buffer, nr_bytes);
	if (bytes_written == -1) {
		string error = "Could not write file \"" + handle.path_ +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
	return bytes_written;
}

int64_t LocalFileSystem::getFileSize(FileHandle &handle) {
	int fd = ((UnixFileHandle &)handle).fd;
	struct stat s;
	if (fstat(fd, &s) == -1) {
		return -1;
	}
	return s.st_size;
}

time_t LocalFileSystem::getLastModifiedTime(FileHandle &handle) {
	int fd = ((UnixFileHandle &)handle).fd;
	struct stat s;
	if (fstat(fd, &s) == -1) {
		return -1;
	}
	return s.st_mtime;
}

FileType LocalFileSystem::getFileType(FileHandle &handle) {
	int fd = ((UnixFileHandle &)handle).fd;
	return getFileTypeInternal(fd);
}

void LocalFileSystem::truncate(FileHandle &handle, int64_t new_size) {
	int fd = ((UnixFileHandle &)handle).fd;
	if (ftruncate(fd, new_size) != 0) {
		string error = "Could not truncate file \"" + handle.path_ +"\":"+ strerror(errno);
		ErrorHandler::errorGeneric(error);
	}
}

bool LocalFileSystem::directoryExists(const string &directory) {
	if (!directory.empty()) {
		if (access(directory.c_str(), 0) == 0) {
			struct stat status;
			stat(directory.c_str(), &status);
			if (status.st_mode & S_IFDIR) {
				return true;
			}
		}
	}
	// if any condition fails
	return false;
}

bool LocalFileSystem::fileExists(const string &filename) {
	if (!filename.empty()) {
		if (access(filename.c_str(), 0) == 0) {
			struct stat status;
			stat(filename.c_str(), &status);
			if (!(status.st_mode & S_IFDIR)) {
				return true;
			}
		}
	}
	// if any condition fails
	return false;
}

void LocalFileSystem::createDirectory(const string &directory) {
	struct stat st;

	if (stat(directory.c_str(), &st) != 0) {
		/* Directory does not exist. EEXIST for race condition */
		if (mkdir(directory.c_str(), 0755) != 0 && errno != EEXIST) {
			string error = "Could not create directory \"" + directory +"\"";
			ErrorHandler::errorGeneric(error);
		}
	} else if (!S_ISDIR(st.st_mode)) {
		string error = "Could not create directory \"" + directory +"\":";
		ErrorHandler::errorGeneric(error);
	}
}

int removeDirectoryRecursive(const char *path) {
	DIR *d = opendir(path);
	idx_t path_len = (idx_t)strlen(path);
	int r = -1;

	if (d) {
		struct dirent *p;
		r = 0;
		while (!r && (p = readdir(d))) {
			int r2 = -1;
			char *buf;
			idx_t len;
			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
				continue;
			}
			len = path_len + (idx_t)strlen(p->d_name) + 2;
			buf = new char[len];
			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode)) {
						r2 = removeDirectoryRecursive(buf);
					} else {
						r2 = unlink(buf);
					}
				}
				delete[] buf;
			}
			r = r2;
		}
		closedir(d);
	}
	if (!r) {
		r = rmdir(path);
	}
	return r;
}

void LocalFileSystem::removeDirectory(const string &directory) {
	removeDirectoryRecursive(directory.c_str());
}

void LocalFileSystem::removeFile(const string &filename) {
	if (std::remove(filename.c_str()) != 0) {
		string error = "Could not create directory \"" + filename +"\":";
		ErrorHandler::errorGeneric(error);
	}
}

bool LocalFileSystem::listFiles(const string &directory, const std::function<void(string, bool)> &callback) {
	if (!directoryExists(directory)) {
		return false;
	}
	DIR *dir = opendir(directory.c_str());
	if (!dir) {
		return false;
	}
	struct dirent *ent;
	// loop over all files in the directory
	while ((ent = readdir(dir)) != nullptr) {
		string name = string(ent->d_name);
		// skip . .. and empty files
		if (name.empty() || name == "." || name == "..") {
			continue;
		}
		// now stat the file to figure out if it is a regular file or directory
		string full_path = joinPath(directory, name);
		if (access(full_path.c_str(), 0) != 0) {
			continue;
		}
		struct stat status;
		stat(full_path.c_str(), &status);
		if (!(status.st_mode & S_IFREG) && !(status.st_mode & S_IFDIR)) {
			// not a file or directory: skip
			continue;
		}
		// invoke callback
		callback(name, status.st_mode & S_IFDIR);
	}
	closedir(dir);
	return true;
}

void LocalFileSystem::fileSync(FileHandle &handle) {
	int fd = ((UnixFileHandle &)handle).fd;
	if (fsync(fd) != 0) {
		string error = "Could not sync  \"" + handle.path_ +"\":";
		ErrorHandler::errorGeneric(error);
	}
}

void LocalFileSystem::moveFile(const string &source, const string &target) {
	//! FIXME: rename does not guarantee atomicity or overwriting target file if it exists
	if (rename(source.c_str(), target.c_str()) != 0) {
		string error = "Could not move  \"" + source +"\":";
		ErrorHandler::errorGeneric(error);
	}
}


bool LocalFileSystem::canSeek() {
	return true;
}

bool LocalFileSystem::onDiskFile(FileHandle &handle) {
	return true;
}

string LocalFileSystem::getFileSeparator() {
#ifdef _WIN32
	return "\\";  // Windows
#else
	return "/";   // Linux, macOS, etc.
#endif
}

void LocalFileSystem::seek(FileHandle &handle, idx_t location) {
	if (!canSeek()) {
		ErrorHandler::errorNotImplemented("Cannot seek in files of this type");
	}
	setFilePointer(handle, location);
}

idx_t LocalFileSystem::seekPosition(FileHandle &handle) {
	if (!canSeek()) {
		ErrorHandler::errorNotImplemented("Cannot seek in files of this type");
	}
	return getFilePointer(handle);
}

static bool hasGlob(const string &str) {
	for (idx_t i = 0; i < str.size(); i++) {
		switch (str[i]) {
		case '*':
		case '?':
		case '[':
			return true;
		default:
			break;
		}
	}
	return false;
}

static void globFiles(FileSystem &fs, const string &path, const string &glob, bool match_directory,
                      vector<string> &result, bool join_path) {
	fs.listFiles(path, [&](const string &fname, bool is_directory) {
		if (is_directory != match_directory) {
			return;
		}
		if (StringUtils::glob(fname, glob)) {
			if (join_path) {
				result.push_back(fs.joinPath(path, fname));
			} else {
				result.push_back(fname);
			}
		}
	});
}


// Recursively collect all subdirectories under `path`.
// Does NOT include files; only directories.
static void collectDirectoriesRecursive(FileSystem &fs, const string &path, vector<string> &out) {
	fs.listFiles(path, [&](const string &fname, bool is_directory) {
		if (!is_directory) {
			return;
		}
		string full = fs.joinPath(path, fname);
		out.push_back(full);
		collectDirectoriesRecursive(fs, full, out);
	});
}

// Collect `path` itself (optional) and all its descendant directories.
static void collectAllDirectories(FileSystem &fs, const string &start, vector<string> &out, bool include_start) {
	if (include_start) {
		out.push_back(start);
	}
	collectDirectoriesRecursive(fs, start, out);
}

vector<string> LocalFileSystem::glob(const string &path) {
	if (path.empty()) {
		return vector<string>();
	}
	// first check if the path has a glob at all
	if (!hasGlob(path)) {
		// no glob: return only the file (if it exists)
		vector<string> result;
		if (fileExists(path)) {
			result.push_back(path);
		}
		return result;
	}
	// split up the path into separate chunks
	vector<string> splits;
	idx_t last_pos = 0;
	for (idx_t i = 0; i < path.size(); i++) {
		if (path[i] == '\\' || path[i] == '/') {
			if (i == last_pos) {
				// empty: skip this position
				last_pos = i + 1;
				continue;
			}
			if (splits.empty()) {
				splits.push_back(path.substr(0, i));
			} else {
				splits.push_back(path.substr(last_pos, i - last_pos));
			}
			last_pos = i + 1;
		}
	}
	splits.push_back(path.substr(last_pos, path.size() - last_pos));
	// handle absolute paths
	bool absolute_path = false;
	if (path[0] == '/') {
		// first character is a slash -  unix absolute path
		absolute_path = true;
	} else if (StringUtils::contains(splits[0], ":")) {
		// first split has a colon -  windows absolute path
		absolute_path = true;
	} else if (splits[0] == "~") {
		// starts with home directory
		auto home_directory = getHomeDirectory();
		if (!home_directory.empty()) {
			absolute_path = true;
			splits[0] = home_directory;
		}
	}
	vector<string> previous_directories;
	if (absolute_path) {
		// for absolute paths, we don't start by scanning the current directory
		previous_directories.push_back(splits[0]);
	}
	for (idx_t i = absolute_path ? 1 : 0; i < splits.size(); i++) {
		bool is_last_chunk = i + 1 == splits.size();
		bool has_glob = hasGlob(splits[i]);

		// Special case: "**" means "this directory and all subdirectories (recursively)"
		if (splits[i] == "**") {
			vector<string> result_dirs;

			if (previous_directories.empty()) {
				// Start from current directory when there is no base directory yet
				collectAllDirectories(*this, ".", result_dirs, true);
			} else {
				for (auto &base : previous_directories) {
					collectAllDirectories(*this, base, result_dirs, true);
				}
			}

			// If "**" is the last chunk, return all directories found (globstar typically matches dirs)
			if (is_last_chunk || result_dirs.empty()) {
				return result_dirs;
			}
			previous_directories = std::move(result_dirs);
			continue; // proceed with next path segment
		}

		// if it's the last chunk we need to find files, otherwise we find directories
		// not the last chunk: gather a list of all directories that match the glob pattern
		vector<string> result;
		if (!has_glob) {
			// no glob, just append as-is
			if (previous_directories.empty()) {
				result.push_back(splits[i]);
			} else {
				for (auto &prev_directory : previous_directories) {
					result.push_back(joinPath(prev_directory, splits[i]));
				}
			}
		} else {
			if (previous_directories.empty()) {
				// no previous directories: list in the current path
				globFiles(*this, ".", splits[i], !is_last_chunk, result, false);
			} else {
				// previous directories
				// we iterate over each of the previous directories, and apply the glob of the current directory
				for (auto &prev_directory : previous_directories) {
					globFiles(*this, prev_directory, splits[i], !is_last_chunk, result, true);
				}
			}
		}
		if (is_last_chunk || result.empty()) {
			return result;
		}
		previous_directories = std::move(result);
	}
	return vector<string>();
}

}
