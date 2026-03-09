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
#include "parquet/parquet_types.h"

namespace bumblebee{



class ParquetFileMetadataCache {
public:
    ParquetFileMetadataCache() : metadata_(nullptr) {
    }
    ParquetFileMetadataCache(std::unique_ptr<format::FileMetaData> file_metadata, time_t r_time)
        : metadata_(std::move(file_metadata)), readTime_(r_time) {
    }

    ~ParquetFileMetadataCache() = default;

    // file metadata
    std::unique_ptr<const format::FileMetaData> metadata_;

    // read time
    time_t readTime_;
};


}
