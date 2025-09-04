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
#include "FileSystem.hpp"

namespace bumblebee{


class LocalFileSystem : public FileSystem{
public:
    ~LocalFileSystem() override;

    std::unique_ptr<FileHandle> openFile(const string &path, uint8_t flags, FileLockType lock,
        FileCompressionType compression) override;

    void read(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) override;

    void write(FileHandle &handle, void *buffer, int64_t nr_bytes, idx_t location) override;

    int64_t read(FileHandle &handle, void *buffer, int64_t nr_bytes) override;

    int64_t write(FileHandle &handle, void *buffer, int64_t nr_bytes) override;

    int64_t getFileSize(FileHandle &handle) override;

    time_t getLastModifiedTime(FileHandle &handle) override;

    FileType getFileType(FileHandle &handle) override;

    void truncate(FileHandle &handle, int64_t new_size) override;

    bool directoryExists(const string &directory) override;

    void createDirectory(const string &directory) override;

    void removeDirectory(const string &directory) override;

    bool listFiles(const string &directory, const std::function<void(string, bool)> &callback) override;

    void moveFile(const string &source, const string &target) override;

    bool fileExists(const string &filename) override;

    void removeFile(const string &filename) override;

    void fileSync(FileHandle &handle) override;

    vector<string> glob(const string &path) override;

    void seek(FileHandle &handle, idx_t location) override;

    idx_t seekPosition(FileHandle &handle) override;

    bool canSeek() override;

    bool onDiskFile(FileHandle &handle) override;


protected:
    string getName() const override;

};


}
