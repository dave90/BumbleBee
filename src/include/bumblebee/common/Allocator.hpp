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
#include "TypeDefs.hpp"

namespace bumblebee{
class ClientContext;
class Allocator;

struct PrivateAllocatorData {
	virtual ~PrivateAllocatorData() {
	}
};

using private_alloc_data_ptr_t = std::unique_ptr<PrivateAllocatorData>;

typedef data_ptr_t (*allocate_function_ptr_t)(PrivateAllocatorData *private_data, idx_t size);
typedef void (*free_function_ptr_t)(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size);
typedef data_ptr_t (*reallocate_function_ptr_t)(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size);

class AllocatedData {
public:
	AllocatedData(Allocator &allocator, data_ptr_t pointer, idx_t allocated_size);
	~AllocatedData();

	data_ptr_t get() {
		return pointer_;
	}
	const_data_ptr_t get() const {
		return pointer_;
	}
	idx_t getSize() const {
		return allocated_size_;
	}
	void reset();

private:
	Allocator &allocator_;
	data_ptr_t pointer_;
	idx_t allocated_size_;
};

using alloc_data_ptr_t = std::unique_ptr<AllocatedData>;

class Allocator {
public:
	Allocator();
	Allocator(allocate_function_ptr_t allocate_function_p, free_function_ptr_t free_function_p,
	          reallocate_function_ptr_t reallocate_function_p, private_alloc_data_ptr_t private_data);

	data_ptr_t allocateData(idx_t size);
	void freeData(data_ptr_t pointer, idx_t size);
	data_ptr_t reallocateData(data_ptr_t pointer, idx_t size);

	alloc_data_ptr_t allocate(idx_t size) {
		return alloc_data_ptr_t( new AllocatedData( *this, allocateData(size), size));
	}

	static data_ptr_t defaultAllocate(PrivateAllocatorData *private_data, idx_t size) {
		return (data_ptr_t)malloc(size);
	}
	static void defaultFree(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size) {
		free(pointer);
	}
	static data_ptr_t defaultReallocate(PrivateAllocatorData *private_data, data_ptr_t pointer, idx_t size) {
		return (data_ptr_t)realloc(pointer, size);
	}

	PrivateAllocatorData *getPrivateData() {
		return privateData_.get();
	}

private:
	allocate_function_ptr_t allocFunction_;
	free_function_ptr_t freeFunction_;
	reallocate_function_ptr_t reallocFunction_;

	private_alloc_data_ptr_t privateData_;
};
};


