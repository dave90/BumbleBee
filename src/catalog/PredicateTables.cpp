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
#include "bumblebee/catalog/PredicateTables.h"

#include "bumblebee/common/Log.h"
#include "bumblebee/parser/statement/Atom.h"
#include "bumblebee/parser/statement/Rule.h"

namespace bumblebee{
PredicateTables::PredicateTables(const char* name, unsigned arity): predicate_(new Predicate(name, arity)), types_(arity, UNKNOWN) {
}

bool operator==(const PredicateTables &lhs, const PredicateTables &rhs) {
    return lhs.predicate_ == rhs.predicate_;
}

bool operator!=(const PredicateTables &lhs, const PredicateTables &rhs) {
    return !(lhs == rhs);
}

void PredicateTables::updateTypes(std::vector<ConstantType>& newTypes) {
    BB_ASSERT(newTypes.size() == types_.size() && "Wrong number of terms for Fact");
    for (idx_t i = 0; i < newTypes.size(); i++) {
        if (newTypes[i] == types_[i]) continue;
        types_[i] = getBumpedType(types_[i], newTypes[i]);
    }
}

void PredicateTables::addFact(Atom &atom) {
    if (atom.containsArith())
        ErrorHandler::errorNotImplemented("Arith term in fact not implemented!");
    auto types = atom.getTermsCType();
    updateTypes(types);

    // track the types for each column
    if (!atom.containsRange()) {
        facts_.push_back(std::move(atom));
    }else {
        ranges_.push_back(std::move(atom));
    }
}

std::vector<ConstantType> PredicateTables::getTypes() {
    return types_;
}


void PredicateTables::initializeChunks() {
    // sync the functions as multiple source operators can call the init
    lock_guard lock(mutex_);
    LOG_DEBUG("Initializing PredicateTables %s", predicate_->toString().c_str());
    if (!facts_.empty()) loadFacts();
    if (!ranges_.empty()) loadRanges();
    facts_.clear();
    ranges_.clear();
}

void PredicateTables::loadFacts() {
    auto types = getTypes();
    BB_ASSERT(types.size() == predicate_->getArity());
    data_chunk_ptr_t chunk = data_chunk_ptr_t(new DataChunk());
    chunk->initialize(types);

    idx_t idx = 0;
    auto chunkCapacity = chunk->getCapacity();
    auto factsSize = facts_.size();
    for (auto& fact : facts_) {
        auto factTypes = fact.getTermsCType();
        // set all the columns
        for (auto col = 0;col < types.size();++col) {
            if (types[col] != factTypes[col])
                // different column type cast it
                chunk->setValue(col, idx, fact.getValue(col).cast(types[col]) );
            else
                chunk->setValue(col, idx, fact.getValue(col) );
        }
        ++idx;
        // check chunk capacity
        if (idx >= chunkCapacity ) {
            chunk->setCardinality(idx);
            // chunk is full add into atoms and create new chunk
            chunks_.append(std::move(chunk));
            chunk = data_chunk_ptr_t(new DataChunk());
            chunk->initialize(types);
            idx = 0;
        }
    }
    chunk->setCardinality(idx);
    if (chunk->getSize() > 0 ) {
        // insert last chunk
        chunks_.append(std::move(chunk));
    }
    LOG_DEBUG("Fact PredicateTables loaded, fact: %d,chunks: %d", chunks_.getCount(), chunks_.chunkCount());
}


std::vector<data_chunk_ptr_t> getChunksFromRange(Atom &atom, const std::vector<ConstantType> &types) {
    std::vector<data_chunk_ptr_t>  chunks;
    data_chunk_ptr_t chunk = data_chunk_ptr_t(new DataChunk());
    // we will set by us the vector data ;)
    chunk->initializeEmpty(types);
    chunk->data_.clear();

    idx_t capacity = chunk->getCapacity();
    auto &terms = atom.getTerms();
    auto cardinality = 1;
    // try to fit all in one chunk
    for (idx_t i = 0; i < terms.size(); ++i) {
        auto &term = terms[i];
        if (term.getType() != RANGE) {
            // constant column
            Vector v(term.getValue());
            chunk->data_.push_back(std::move(v));
            continue;
        }
        // sequence column
        auto& interval = term.getInterval();
        BB_ASSERT(interval.to > interval.from);
        // Set sequence type as BIGINT (64 bit) for handle big sequence
        Vector v(ConstantType::BIGINT, nullptr);
        v.sequence(interval.from, 0,  (int64_t) cardinality , interval.to);
        cardinality = cardinality * (interval.to - interval.from + 1);
        chunk->data_.push_back(std::move(v));
    }
    if (cardinality <= capacity) {
        chunk->setCardinality(cardinality);
        // the sequence is fit on one chunk
        if (cardinality < capacity) {
            // normalify and resize as does not fit an entire chunk
            // resize is for allocate the data as capacity otherwise sequence allocate memory only for the sequence size
            chunk->normalify();
            chunk->resize(capacity);
        }
        chunks.push_back(std::move(chunk));
        return chunks;
    }
    // chunk need to be split in multiple chunks
    idx_t idx = 0;
    auto& vectors = chunk->data_;
    while (idx < cardinality) {
        data_chunk_ptr_t schunk = data_chunk_ptr_t(new DataChunk());
        schunk->initializeEmpty(types);
        schunk->data_.clear();

        for (idx_t col = 0; col < vectors.size(); ++col) {
            if (vectors[col].getVectorType() == VectorType::CONSTANT_VECTOR) {
                Vector newVec(atom.getValue(col));
                schunk->data_.push_back(std::move(newVec));
                continue;
            }
            // sequence to split
            int64_t start, stride, end, offset;
            CircularSequenceVector::getSequence(vectors[col], start,offset, stride, end);
            BB_ASSERT(offset == 0);
            offset = (int64_t)idx;
            Vector newVec(ConstantType::BIGINT, nullptr);
            newVec.sequence(start, offset,  stride , end);
            schunk->data_.push_back(std::move(newVec));
        }
        auto schunkCapacity = std::min(cardinality-idx, capacity); // the last split chunk can be smaller than capacity
        schunk->setCardinality(schunkCapacity);
        chunks.push_back(std::move(schunk));
        idx += capacity;
    }

    // check last chunk size if fit the chunk
    if (chunks.back()->getSize() < capacity) {
        // resize and normalify as does not fit an entire chunk
        chunks.back()->normalify();
        chunks.back()->resize(capacity);
    }
    return chunks;
}

void PredicateTables::loadRanges() {

    // process the range atoms
    for (auto& atom: ranges_) {
        auto chunks = getChunksFromRange(atom, types_);
        auto chunksSize = chunks.size();

        // check if the last chunk is not totally full
        if (chunks_.chunkCount() > 0  ) {
            auto &lastChunk = chunks_.chunks().back();
            if (lastChunk->getSize() < lastChunk->getCapacity()) {
                // merge the last chunk created by this atom with the last chunk stored (copy needed :( )
                chunks_.append(*chunks.back());
                if (chunksSize == 1) return;
                // remove the chunk appended
                chunks.pop_back();
                --chunksSize;
            }
        }

        for (auto& chunk: chunks) {
            // do not copy the chunk :)
            chunks_.append(std::move(chunk));
        }

    }
}

Value PredicateTables::getValue(idx_t column, idx_t index) {
    return chunks_.getValue(column, index);
}

void PredicateTables::append(data_chunk_ptr_t chunk) {
    BB_ASSERT(chunk->columnCount() == predicate_->getArity());
    lock_guard guard(mutex_);
    if (types_.size() > 0 && types_[0] == UNKNOWN) {
        // set the types as the chunk types
        types_ = chunk->getTypes();
    }
    chunks_.append(std::move(chunk));
}

void PredicateTables::append(DataChunk& chunk) {
    BB_ASSERT(chunk.columnCount() == predicate_->getArity());
    lock_guard guard(mutex_);
    if (types_.size() > 0 && types_[0] == UNKNOWN) {
        // set the types as the chunk types
        types_ = chunk.getTypes();
    }
    chunks_.append(chunk);
}
}
