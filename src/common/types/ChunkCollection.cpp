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
#include "bumblebee/common/types/ChunkCollection.h"

#include "bumblebee/common/vector_operations/VectorOperations.h"

namespace bumblebee{
void ChunkCollection::append(DataChunk &chunk) {
    if (chunk.getSize() == 0)return;
    count_ += chunk.getSize();

    idx_t dataToAppend = chunk.getSize();
    idx_t offset = 0;
    if (chunks_.size() == 0) {
        // first chunk
        types_ = chunk.getTypes();
    }else {
        // check types
        auto types =  chunk.getTypes();
        BB_ASSERT(types.size() == types_.size());
        for (idx_t i = 0; i < types_.size(); ++i) {
            if (types[i] != types_[i])
                ErrorHandler::errorNotImplemented ("Type mismatch: cannot combine rows containing both strings and numbers.");
        }
        // check available space in the last chunk
        auto& lastChunk = chunks_.back();
        auto avaliableSize = std::min(dataToAppend, STANDARD_VECTOR_SIZE - lastChunk->getSize());
        if (avaliableSize > 0) {
            // normalify the chunk
            chunk.normalify();
            auto oldCount = chunk.getSize();
            // set the cardinality to the avaliable size in the last chunk
            chunk.setCardinality(avaliableSize);
            lastChunk->append(chunk);
            dataToAppend -= avaliableSize;
            // restore the size and set the offset
            chunk.setCardinality(oldCount);
            offset = avaliableSize;
        }
    }

    if (dataToAppend > 0) {
        // append the remaining data
        auto chunkToAdd = data_chunk_ptr_t( new DataChunk());
        chunkToAdd->initialize(types_);
        // copy the data in the new chunk and push in the collection
        chunk.copy( *chunkToAdd, offset );
        chunks_.push_back(std::move(chunkToAdd));
    }
}

void ChunkCollection::append(data_chunk_ptr_t chunk) {
    if (chunks_.size() == 0) {
        // first chunk
        types_ = chunk->getTypes();
    }
    count_ += chunk->getSize();
    chunks_.push_back(std::move(chunk));
}

void ChunkCollection::append(ChunkCollection &other) {
    for (auto& chunk : other.chunks_) {
        append(*chunk);
    }
}

void ChunkCollection::merge(ChunkCollection &other) {
    if (other.count_ == 0)return;
    if (count_ == 0) {
        count_ = other.count_;
        types_ = std::move(other.types_);
        chunks_ = std::move(other.chunks_);
        return;
    }
    BB_ASSERT(types_ == other.types_);
    data_chunk_ptr_t lastChunk;
    if (chunks_.back()->getSize() != STANDARD_VECTOR_SIZE) {
        // last chunk is not full
        lastChunk = std::move(chunks_.back());
        count_ -= lastChunk->getSize();
        chunks_.pop_back();
    }
    // merge the chunks
    for (auto& chunk : other.chunks_) {
        append(std::move(chunk));
    }
    if (lastChunk) {
        // append last chunk filling the gaps
        append(*lastChunk);
    }
}

void ChunkCollection::fuse(ChunkCollection &other) {
    types_.insert(types_.end(), other.types_.begin(), other.types_.end());
    if (count_ == 0) {
        // current collection empty so create empty data chunks and take the vectors from other
        chunks_.reserve(other.chunkCount());
        for (idx_t i = 0; i < other.chunkCount(); ++i) {
            auto lhs = data_chunk_ptr_t( new DataChunk());
            auto &rhs = other.getChunk(i);
            lhs->data_.reserve(rhs.data_.size());
            for (auto &v : rhs.data_) {
                lhs->data_.emplace_back(Vector(v));
            }
            lhs->setCardinality(rhs.getSize());
            chunks_.push_back(std::move(lhs));
        }
        count_ = other.count_;
        return;
    }
    // number of chunks should match
    BB_ASSERT(count_ == other.count_);
    for (idx_t i = 0; i < other.chunkCount(); ++i) {
        auto& lhs = chunks_[i];
        auto &rhs = other.getChunk(i);
        // fuse the data chunks
        BB_ASSERT(lhs->getSize() == rhs.getSize());
        for (auto &v : rhs.data_) {
            lhs->data_.emplace_back(Vector(v));
        }
    }

}

void ChunkCollection::verify() {
    //TODO
}

Value ChunkCollection::getValue(idx_t column, idx_t index) {
    return getChunkForRow(index).getValue(column, index % STANDARD_VECTOR_SIZE);
}

void ChunkCollection::setValue(idx_t column, idx_t index, const Value &value) {
    getChunkForRow(index).setValue(column, index % STANDARD_VECTOR_SIZE, value);
}

void ChunkCollection::copyCell(idx_t column, idx_t index, Vector &target, idx_t targetOffset) {
    auto &source = getChunkForRow(index).data_[column];
    auto sourceOffset = index % STANDARD_VECTOR_SIZE;
    // copy 1 cell
    VectorOperations::copy(source, target, sourceOffset+1, sourceOffset, targetOffset);
}

string ChunkCollection::toString() const {
    auto result = chunks_.size() == 0 ? "ChunkCollection [ 0 ]"
               : "ChunkCollection [ " + std::to_string(count_) + " ]: \n";
    for (auto& chunk : chunks_) {
        result += chunk->toString() + "\n";
    }
    return result;
}

bool ChunkCollection::equals(ChunkCollection &other) {
    if (count_ != other.count_) {
        return false;
    }
    if (columnCount() != other.columnCount()) {
        return false;
    }
    if (types_ != other.types_) {
        return false;
    }
    // if count_ is equal amount of chunks should be equal
    for (idx_t row = 0; row < count_; row++) {
        for (idx_t col = 0; col < columnCount(); col++) {
            auto lvalue = getValue(col, row);
            auto rvalue = other.getValue(col, row);
            if (lvalue != rvalue) {
                return false;
            }
        }
    }
    return true;
}
}
