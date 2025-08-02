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
#include "bumblebee/common/types/DataChunk.h"

#include "bumblebee/common/types/Vector.h"
#include "bumblebee/common/vector_operations/VectorOperations.h"

namespace bumblebee{
DataChunk::DataChunk(): count_(0), capacity_(STANDARD_VECTOR_SIZE) {}

DataChunk::DataChunk(DataChunk &&other) noexcept : count_(other.count_), capacity_(other.capacity_), data_(std::move(other.data_)) {
    other.destroy();
}

Value DataChunk::getValue(idx_t col, idx_t index) const {
    BB_ASSERT(col < columnCount());
    BB_ASSERT(index < getSize());
    return data_[col].getValue(index);
}

void DataChunk::setValue(idx_t col, idx_t index, const Value &val) {
    BB_ASSERT(col < columnCount());
    BB_ASSERT(index < getCapacity());
    return data_[col].setValue(index, val);
}

void DataChunk::reference(DataChunk &chunk) {
    BB_ASSERT(columnCount() >= chunk.columnCount());
    setCapacity(chunk);
    setCardinality(chunk);
    for (idx_t i = 0; i < chunk.columnCount(); ++i) {
        data_[i].reference(chunk.data_[i]);
    }
}

void DataChunk::reference(DataChunk &chunk, const std::vector<idx_t>& cols) {
    BB_ASSERT(columnCount() >= cols.size());
    setCapacity(chunk);
    setCardinality(chunk);
    for (idx_t i = 0; i < cols.size(); ++i) {
        auto idx = cols[i];
        BB_ASSERT(idx < chunk.columnCount());
        data_[i].reference(chunk.data_[idx]);
    }
}

std::unique_ptr<DataChunk> DataChunk::clone() {
    data_chunk_ptr_t chunk = std::make_unique<DataChunk>();
    chunk->initializeEmpty(getTypes());
    chunk->reference(*this);
    return chunk;
}

void DataChunk::initialize(const std::vector<ConstantType> &types) {
    BB_ASSERT(data_.empty());
    capacity_ = STANDARD_VECTOR_SIZE;
    for (idx_t i = 0; i < types.size(); ++i) {
        data_.emplace_back(types[i]);
    }
}

void DataChunk::initializeEmpty(const std::vector<ConstantType> &types) {
    BB_ASSERT(data_.empty());
    capacity_ = STANDARD_VECTOR_SIZE;
    for (idx_t i = 0; i < types.size(); ++i) {
        data_.emplace_back(types[i], nullptr);
    }
}

void DataChunk::append(const DataChunk &other, bool resize, SelectionVector *sel, idx_t count) {
    if (other.getSize() == 0) {
        return;
    }
    idx_t newSize = sel ? getSize() + count : getSize() + other.getSize();
    BB_ASSERT(columnCount() == other.columnCount() && "Column counts of appending chunk doesn't match!");

    // check if resize is needed
    if (newSize > capacity_) {
        BB_ASSERT(resize && "Can't append chunk to other chunk without resizing");

        for (idx_t i = 0; i < columnCount(); i++)
            data_[i].resize(getSize(), newSize);
        capacity_ = newSize;
    }

    for (idx_t i = 0; i < columnCount(); i++) {
        // expected flat vector, cannot copy in dictionary or other vector type
        BB_ASSERT(data_[i].getVectorType() == VectorType::FLAT_VECTOR);
        if (sel) {
            VectorOperations::copy(other.data_[i], data_[i], *sel, count, 0, getSize());
        } else {
            VectorOperations::copy(other.data_[i], data_[i], other.getSize(), 0, getSize());
        }
    }
    setCardinality(newSize);
}

void DataChunk::resize(idx_t size) {
    for (auto& v:data_)
        v.resize(count_, size);
}

void DataChunk::destroy() {
    data_.clear();
    capacity_ = 0;
    count_ = 0;
}

void DataChunk::copy(DataChunk &other, idx_t offset) const {
    BB_ASSERT(columnCount() == other.columnCount());
    BB_ASSERT(other.getSize() == 0); // other is the target chunk so size should be 0

    for (idx_t i = 0; i < columnCount(); i++) {
        BB_ASSERT(other.data_[i].getVectorType() == VectorType::FLAT_VECTOR);
        VectorOperations::copy(data_[i], other.data_[i], getSize(), offset, 0);
    }
    other.setCardinality(getSize() - offset);
}

void DataChunk::copy(DataChunk &other, const SelectionVector &sel, idx_t sourceCount, idx_t offset) const {
    BB_ASSERT(columnCount() == other.columnCount());

    for (idx_t i = 0; i < columnCount(); i++) {
        BB_ASSERT(other.data_[i].getVectorType() == VectorType::FLAT_VECTOR);
        VectorOperations::copy(data_[i], other.data_[i], sel, sourceCount, offset, 0);
    }
    other.setCardinality(sourceCount - offset);
}

void DataChunk::split(DataChunk &other, idx_t splitIndex) {
    BB_ASSERT(other.getSize() == 0);
    BB_ASSERT(other.data_.empty());
    BB_ASSERT(splitIndex < data_.size());

    const idx_t numCols = data_.size();
    for (idx_t col = splitIndex; col < numCols; col++) {
        other.data_.push_back(std::move(data_[col]));
    }
    // remove moved cols
    for (idx_t col = splitIndex; col < numCols; col++) {
        data_.pop_back();
    }
    other.setCardinality(*this);
}

void DataChunk::normalify() {
    for (idx_t i = 0; i < columnCount(); i++) {
        data_[i].normalify(count_);
    }
}

array_vector_data_t DataChunk::orrify() {
    array_vector_data_t result(new VectorData[columnCount()]);
    for (idx_t i = 0; i < columnCount(); i++) {
        data_[i].orrify(count_,result.get()[i] );
    }
    return result;
}

void DataChunk::slice(const SelectionVector &selVector, idx_t count) {
    count_ = count;
    for (idx_t i = 0; i < columnCount(); i++) {
        data_[i].slice(selVector, count);
    }
}

void DataChunk::slice(DataChunk &other, const SelectionVector &sel, idx_t count, idx_t colOffset) {
    BB_ASSERT(columnCount() >= other.columnCount() + colOffset);
    count_ = count;
    SelCache merge_cache;
    for (idx_t c = 0; c < other.columnCount(); c++) {
        if (other.data_[c].getVectorType() == VectorType::DICTIONARY_VECTOR) {
            // already a dictionary merge the dictionaries
            data_[colOffset + c].reference(other.data_[c]);
            data_[colOffset + c].slice(sel, count, merge_cache);
        } else {
            data_[colOffset + c].slice(other.data_[c], sel, count);
        }
    }
}

void DataChunk::slice(DataChunk &other, const SelectionVector &sel, idx_t count, const std::vector<idx_t>& colsMap) {
    BB_ASSERT(colsMap.size() == other.columnCount());
    count_ = count;
    SelCache merge_cache;
    for (idx_t c = 0; c < other.columnCount(); c++) {
        auto index = colsMap[c];
        BB_ASSERT(index < columnCount());
        if (other.data_[c].getVectorType() == VectorType::DICTIONARY_VECTOR) {
            // already a dictionary merge the dictionaries
            data_[index].reference(other.data_[c]);
            data_[index].slice(sel, count, merge_cache);
        } else {
            data_[index].slice(other.data_[c], sel, count);
        }
    }
}

void DataChunk::reset() {
    if (data_.empty()) {
        return;
    }
    auto types = getTypes();
    data_.clear();
    count_ = 0;
    initialize(types);
}

void DataChunk::hash(Vector &result) {
    BB_ASSERT(result.getType() == ConstantType::UBIGINT);
    VectorOperations::hash(data_[0], result, getSize());
    for (idx_t i = 1; i < columnCount(); i++) {
        ;
        VectorOperations::combineHash(result, data_[i], getSize());
    }
}

std::vector<ConstantType> DataChunk::getTypes() {
    std::vector<ConstantType> types;
    types.reserve(data_.size());
    for (auto&v : data_) {
        types.push_back(v.getType());
    }
    return types;
}

std::string DataChunk::toString() const {
    string result = "Chunk - [" + std::to_string(columnCount()) + " Columns]\n";
    for (idx_t i = 0; i < columnCount(); i++) {
        result += "- " + data_[i].toString(getSize()) + "\n";
    }
    return result;
}

void DataChunk::verify() {
    // TODO
}
}
