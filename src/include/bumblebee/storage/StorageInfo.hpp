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

#include "bumblebee/common/FileSystem.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/serializer/Serializer.hpp"

namespace bumblebee{

// The version number of the database storage format
const uint64_t VERSION_NUMBER = 1;

using block_id_t = int64_t;

#define INVALID_BLOCK (-1)

// maximum block id, 2^62
#define MAXIMUM_BLOCK 4611686018427388000LL

// The MainHeader is the first header in the storage file. The MainHeader is typically written only once for a database
// file.
struct MainHeader {
    static constexpr idx_t MAGIC_BYTE_SIZE = 4;
    static constexpr idx_t MAGIC_BYTE_OFFSET = sizeof(uint64_t);
    static constexpr idx_t FLAG_COUNT = 4;
    // the magic bytes in front of the file
    // should be "BEEE"
    static const char MAGIC_BYTES[];
    // The version of the database
    uint64_t version_number;
    // The set of flags used by the database
    uint64_t flags[FLAG_COUNT];

    static void checkMagicBytes(FileHandle &handle);

    void serialize(Serializer &ser);
    static MainHeader deserialize(Deserializer &source);
};

// The DatabaseHeader contains information about the current state of the database. Every storage file has two
// DatabaseHeaders. On startup, the DatabaseHeader with the highest iteration count is used as the active header. When
// a checkpoint is performed, the active DatabaseHeader is switched by increasing the iteration count of the
// DatabaseHeader.
struct DatabaseHeader {
    // The iteration count, increases by 1 every time the storage is checkpointed.
    uint64_t iteration;
    // A pointer to the initial meta block
    block_id_t meta_block;
    // A pointer to the block containing the free list
    block_id_t free_list;
    // The number of blocks that is in the file as of this database header. If the file is larger than BLOCK_SIZE *
    // block_count any blocks appearing AFTER block_count are implicitly part of the free_list.
    uint64_t block_count;

    void serialize(Serializer &ser);
    static DatabaseHeader deserialize(Deserializer &source);
};



}
