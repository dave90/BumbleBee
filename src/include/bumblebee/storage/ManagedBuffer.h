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
#include "StorageInfo.h"
#include "bumblebee/common/FileBuffer.h"
#include "bumblebee/common/TypeDefs.h"

namespace bumblebee{

// Managed buffer is an arbitrarily-sized buffer that is at least of size >= BLOCK_SIZE
class ManagedBuffer : public FileBuffer{
public:
    ManagedBuffer(ClientContext& context, idx_t size, bool can_destroy, block_id_t id);

    ClientContext& context_;
    // Whether or not the managed buffer can be freely destroyed when unpinned.
    // - If can_destroy is true, the buffer can be destroyed when unpinned and hence be unrecoverable. After being
    // destroyed, Pin() will return false.
    // - If can_destroy is false, the buffer will instead be written to a temporary file on disk when unloaded from
    // memory, and read back into memory when Pin() is called.
    bool canDestroy_;
    // The internal id of the buffer
    block_id_t id_;
};


}
