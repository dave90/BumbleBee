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

#include "bumblebee/common/Vector.hpp"
#include "catalog/Schema.hpp"
#include "common/Allocator.hpp"
#include "common/FileSystem.hpp"
#include "function/FunctionRegister.hpp"
#include "storage/BufferManager.hpp"

namespace bumblebee{

// The ClientContext holds information relevant to the current session
class ClientContext {
public:
    std::string logFilename_;
    vector<std::string> inputFiles_;
    bool printLog_;
    std::unique_ptr<Schema> ownedSchema_;
    Schema& defaultSchema_;
    idx_t threads_;
    bool printAll_;
    bool printProfiling_;
    bool distinct_;
    FunctionRegister functionRegister_;
    fs_ptr_t fileSystem_;
    Allocator allocator_;
    buffer_mngr_ptr_ptr_t bufferManager_;
    string tempDirectory_;
    idx_t maxMemory_;
    block_manager_ptr_t blockManager_;
    bool printProgram_;
    atomic<unsigned long long> counter_;

    ClientContext();
    ClientContext(std::unique_ptr<Schema> ownedSchema);

private:
    void init();
    void registerFunctions();
    void initFileSystem();
};


}
