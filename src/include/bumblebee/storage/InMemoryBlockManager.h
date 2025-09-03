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
#include "BlockManager.h"
#include "bumblebee/common/ErrorHandler.h"

namespace bumblebee{


class InMemoryBlockManager : public BlockManager {
public:
    ~InMemoryBlockManager() override {

    }

    void startCheckpoint() override;

    block_ptr_t createBlock(block_id_t block_id) override {
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
        return 0;
    }

    block_id_t getFreeBlockId() override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
        return 0;
    }

    bool isRootBlock(block_id_t root) override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
        return false;
    }

    void markBlockAsModified(block_id_t block_id) override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
    }

    void increaseBlockReferenceCount(block_id_t block_id) override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
    }

    block_id_t getMetaBlock() override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
        return 0;
    }

    void read(Block &block) override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
    }

    void write(FileBuffer &block, block_id_t block_id) override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
    }

    void writeHeader(DatabaseHeader header) override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
    }

    idx_t totalBlocks() override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
        return 0;
    }

    idx_t freeBlocks() override{
        ErrorHandler::errorNotImplemented("Cannot perform IO in in-memory database!");
        return 0;
    }
};


}
