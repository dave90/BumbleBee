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
#include "bumblebee/common/FileSystem.hpp"


#include <cstdint>
#include <cstdio>

#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/StringUtils.hpp"


namespace bumblebee{

int64_t FileHandle::read(void *buffer, idx_t nr_bytes) {
    return fileSystem_.read(*this, buffer, nr_bytes);
}

int64_t FileHandle::write(void *buffer, idx_t nr_bytes) {
    return fileSystem_.write(*this, buffer, nr_bytes);
}

void FileHandle::read(void *buffer, idx_t nr_bytes, idx_t location) {
    fileSystem_.read(*this, buffer, nr_bytes, location);
}

void FileHandle::write(void *buffer, idx_t nr_bytes, idx_t location) {
    fileSystem_.write(*this, buffer, nr_bytes, location);
}

void FileHandle::seek(idx_t location) {
    fileSystem_.seek(*this, location);
}

void FileHandle::reset() {
    fileSystem_.reset(*this);
}

idx_t FileHandle::seekPosition() {
    return fileSystem_.seekPosition(*this);
}

void FileHandle::sync() {
    fileSystem_.fileSync(*this);
}

void FileHandle::truncate(int64_t new_size) {
    fileSystem_.truncate(*this, new_size);
}

string FileHandle::readLine() {
    string result;
    char buffer[1];
    while (true) {
        idx_t tuples_read = read(buffer, 1);
        if (tuples_read == 0 || buffer[0] == '\n') {
            return result;
        }
        if (buffer[0] != '\r') {
            result += buffer[0];
        }
    }
}

bool FileHandle::canSeek() {
    return fileSystem_.canSeek();
}

bool FileHandle::onDiskFile() {
    return fileSystem_.onDiskFile(*this);
}

idx_t FileHandle::getFileSize() {
    return fileSystem_.getFileSize(*this);
}

FileType FileHandle::getType() {
    return fileSystem_.getFileType(*this);
}


// Current implementation works on unix machine
// TODO implement also for windows

std::unique_ptr<FileHandle> FileSystem::openFile(const string &path, uint8_t flags, FileLockType lock,
    FileCompressionType compression) {
    string error = "Open File is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return nullptr;
}

void FileSystem::read(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) {
    string error = "Read is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}
void FileSystem::write(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) {
    string error = "Write (with location) is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

int64_t FileSystem::read(FileHandle &handle, void *buffer, int64_t nr_bytes) {
    string error = "Read (no location) is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return 0;
}

int64_t FileSystem::write(FileHandle &handle, void *buffer, int64_t nr_bytes) {
    string error = "Write (no location) is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return 0;
}

int64_t FileSystem::getFileSize(FileHandle &handle) {
    string error = "GetFileSize is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return 0;
}

time_t FileSystem::getLastModifiedTime(FileHandle &handle) {
    string error = "GetLastModifiedTime is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return 0;
}

FileType FileSystem::getFileType(FileHandle &handle) {
    string error = "GetFileType is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return FileType::FILE_TYPE_INVALID;
}

void FileSystem::truncate(FileHandle &handle, int64_t new_size) {
    string error = "Truncate is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

bool FileSystem::directoryExists(const string &directory) {
    string error = "DirectoryExists is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return false;
}

void FileSystem::createDirectory(const string &directory) {
    string error = "CreateDirectory is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

void FileSystem::removeDirectory(const string &directory) {
    string error = "RemoveDirectory is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

bool FileSystem::listFiles(const string &directory, const std::function<void(string, bool)> &callback) {
    string error = "ListFiles is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return false;
}

void FileSystem::moveFile(const string &source, const string &target) {
    string error = "MoveFile is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

bool FileSystem::fileExists(const string &filename) {
    string error = "FileExists is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return false;
}

void FileSystem::removeFile(const string &filename) {
    string error = "RemoveFile is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

void FileSystem::fileSync(FileHandle &handle) {
    string error = "FileSync is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}



vector<string> FileSystem::glob(const string &path) {
    string error = "Glob is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return {};
}

void FileSystem::seek(FileHandle &handle, idx_t location) {
    string error = "Seek is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

void FileSystem::reset(FileHandle &handle) {
    string error = "Reset is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
}

idx_t FileSystem::seekPosition(FileHandle &handle) {
    string error = "SeekPosition is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return 0;
}

bool FileSystem::canSeek() {
    string error = "CanSeek is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return false;
}

bool FileSystem::onDiskFile(FileHandle &handle) {
    string error = "OnDiskFile is not implemented in filesystem: " + getName();
    ErrorHandler::errorNotImplemented(error.c_str());
    return false;
}


string FileSystem::getWorkingDirectory() {
    auto buffer = std::unique_ptr<char[]>(new char[PATH_MAX]);
    char *ret = getcwd(buffer.get(), PATH_MAX);
    if (!ret) {
        ErrorHandler::errorGeneric("Could not get the working directory!");
    }
    return string(buffer.get());
}

string FileSystem::getHomeDirectory() {
    const char *homedir = getenv("HOME");
    if (!homedir) {
        return string();
    }
    return homedir;
}

idx_t FileSystem::getAvailableMemory() {
    errno = 0;
    idx_t max_memory = (idx_t)sysconf(_SC_PHYS_PAGES) * (idx_t)sysconf(_SC_PAGESIZE);
    max_memory = std::min(max_memory, (idx_t)UINTPTR_MAX);
    if (errno != 0) {
        ErrorHandler::errorGeneric("Could not fetch available system memory!");
    }
    return max_memory;
}

string FileSystem::pathSeparator() {
    return "/";
}

string FileSystem::joinPath(const string &p1, const string &p2) {
    return p1 + pathSeparator() + p2;
}

string FileSystem::convertSeparators(const string &path) {
    auto separator_str = pathSeparator();
    char separator = separator_str[0];
    if (separator == '/') {
        // on unix-based systems we only accept / as a separator
        return path;
    }
    // on windows-based systems we accept both
    string result = path;
    for (idx_t i = 0; i < result.size(); i++) {
        if (result[i] == '/') {
            result[i] = separator;
        }
    }
    return result;
}

string FileSystem::extractBaseName(const string &path) {
    auto sep = pathSeparator();

    auto vec = StringUtils::split(StringUtils::split(path, sep).back(), ".");
    return vec[0];
}


void FileSystem::setWorkingDirectory(const string &path) {
    if (chdir(path.c_str()) != 0) {
        ErrorHandler::errorGeneric("Could not change working directory!");
    }
}

}
