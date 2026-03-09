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
#include "Serializer.hpp"
#include "BufferedSerializer.hpp"
#include "bumblebee/common/ErrorHandler.hpp"

namespace bumblebee{


class BufferedDeserializer : public Deserializer {
public:
    BufferedDeserializer(data_ptr_t ptr, idx_t data_size):ptr_(ptr), endptr_(ptr + data_size) {

    }
    explicit BufferedDeserializer(BufferedSerializer &serializer): BufferedDeserializer(serializer.data_, serializer.maximumSize_) {
    }

    data_ptr_t ptr_;
    data_ptr_t endptr_;

public:
    void readData(data_ptr_t buffer, uint64_t read_size) override {
        if (ptr_ + read_size > endptr_) {
            ErrorHandler::outOfMemory("Failed to deserialize: not enough data in buffer to fulfill read request");
        }
        memcpy(buffer, ptr_, read_size);
        ptr_ += read_size;
    }
};


}
