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
#include "bumblebee/common/Allocator.hpp"
#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/Helper.hpp"

namespace bumblebee{



class ByteBuffer {
public:
    ByteBuffer() {};
    ByteBuffer(char *ptr, uint64_t len) : ptr_(ptr), len_(len) {};

    char *ptr_ = nullptr;
    uint64_t len_ = 0;

public:
    void inc(uint64_t increment) {
        available(increment);
        len_ -= increment;
        ptr_ += increment;
    }

    template <class T>
    T read() {
        T val = get<T>();
        inc(sizeof(T));
        return val;
    }

    template <class T>
    T get() {
        available(sizeof(T));
        T val = load<T>((data_ptr_t)ptr_);
        return val;
    }

    void copy_to(char *dest, uint64_t len) {
        available(len);
        std::memcpy(dest, ptr_, len);
    }

    void zero() {
        std::memset(ptr_, 0, len_);
    }

    void available(uint64_t req_len) {
        if (req_len > len_) {
            ErrorHandler::outOfMemory("Out of buffer");
        }
    }
};

class ResizeableBuffer : public ByteBuffer {
public:
    ResizeableBuffer() {
    }
    ResizeableBuffer(Allocator &allocator, uint64_t new_size) {
        resize(allocator, new_size);
    }
    void resize(Allocator &allocator, uint64_t new_size) {
        len_ = new_size;
        if (new_size == 0) {
            return;
        }
        if (new_size > allocLen_) {
            allocLen_ = new_size;
            allocatedData_ = allocator.allocate(allocLen_);
            ptr_ = (char *)allocatedData_->get();
        }
    }

private:
    std::unique_ptr<AllocatedData> allocatedData_;
    idx_t allocLen_ = 0;
};


}
