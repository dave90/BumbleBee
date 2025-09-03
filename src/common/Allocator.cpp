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
#include "../include/bumblebee/common/Allocator.h"

#include "bumblebee/ClientContext.h"

namespace bumblebee{
AllocatedData::AllocatedData(Allocator &allocator, data_ptr_t pointer, idx_t allocated_size):
    allocator_(allocator), allocated_size_(allocated_size), pointer_(pointer){
}

AllocatedData::~AllocatedData() {
}

void AllocatedData::reset() {
    if (!pointer_) return;
    allocator_.freeData(pointer_, allocated_size_);
    pointer_ = nullptr;
}

Allocator::Allocator() : allocFunction_(defaultAllocate), freeFunction_(defaultFree), reallocFunction_(defaultReallocate){
}

Allocator::Allocator(allocate_function_ptr_t allocate_function_p, free_function_ptr_t free_function_p,
    reallocate_function_ptr_t reallocate_function_p, private_alloc_data_ptr_t private_data)
        :allocFunction_(allocate_function_p), freeFunction_(free_function_p), reallocFunction_(reallocate_function_p),
    privateData_(std::move(private_data)){
}

data_ptr_t Allocator::allocateData(idx_t size) {
    return allocFunction_(privateData_.get(), size);
}

void Allocator::freeData(data_ptr_t pointer, idx_t size) {
    freeFunction_(privateData_.get(), pointer, size);
}

data_ptr_t Allocator::reallocateData(data_ptr_t pointer, idx_t size) {
    return reallocFunction_(privateData_.get(), pointer, size);
}

}
