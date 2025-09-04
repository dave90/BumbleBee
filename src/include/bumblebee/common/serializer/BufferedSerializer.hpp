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
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{



#define SERIALIZER_DEFAULT_SIZE 1024

struct BinaryData {
    std::unique_ptr<data_t[]> data_;
    idx_t size_;
};

class BufferedSerializer : public Serializer {
public:
    BufferedSerializer(idx_t maximum_size = SERIALIZER_DEFAULT_SIZE)
        : BufferedSerializer(std::unique_ptr<data_t[]>(new data_t[maximum_size]), maximum_size) {
    }

    BufferedSerializer(std::unique_ptr<data_t[]> data, idx_t size) : maximumSize_(size), data_(data.get()) {
        blob_.size_ = 0;
        blob_.data_ = std::move(data);
    }

    BufferedSerializer(data_ptr_t data, idx_t size) : maximumSize_(size), data_(data) {
        blob_.size_ = 0;
    }

    idx_t maximumSize_;
    data_ptr_t data_;

    BinaryData blob_;

public:
    void writeData(const_data_ptr_t buffer, idx_t write_size) override {
        if (blob_.size_ + write_size >= maximumSize_) {
            do {
                maximumSize_ *= 2;
            } while (blob_.size_ + write_size > maximumSize_);
            auto new_data = new data_t[maximumSize_];
            memcpy(new_data, data_, blob_.size_);
            data_ = new_data;
            blob_.data_ = std::unique_ptr<data_t[]>(new_data);
        }

        memcpy(data_ + blob_.size_, buffer, write_size);
        blob_.size_ += write_size;
    }

    BinaryData getData() {
        return std::move(blob_);
    }

    void reset() {
        blob_.size_ = 0;
    }
};


}
