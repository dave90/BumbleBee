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
#include "bumblebee/ClientContext.h"

#include "bumblebee/catalog/Catalog.h"
#include "bumblebee/common/LocalFileSystem.h"
#include "bumblebee/function/aggregate/Sum.h"
#include "bumblebee/storage/InMemoryBlockManager.h"

namespace bumblebee{
ClientContext::ClientContext():
    defaultSchema_(Catalog::instance().getDefaultSchema()),
    printLog_(false),
    threads_(1),
    printAll_(false),
    printProfiling_(false),
    singleShot_(true),
    maxMemory_((idx_t)-1),
    tempDirectory_(DEFAULT_TMP_DIR),
    bufferManager_(*this,tempDirectory_,maxMemory_),
    blockManager_(new InMemoryBlockManager()){

    registerFunctions();
    initFileSystem();
}

void ClientContext::registerFunctions() {
    SumFunc::registerFunction(functionRegister_);
}

void ClientContext::initFileSystem() {
    fileSystem_ = fs_ptr_t(new LocalFileSystem());
}

}
