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
#include "bumblebee/catalog/PredicateTables.hpp"

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Log.hpp"
#include "bumblebee/parser/statement/Atom.hpp"
#include "bumblebee/execution/JoinHashTable.hpp"
#include "bumblebee/execution/PartitionedAggHT.hpp"

namespace bumblebee{
PredicateTables::PredicateTables(ClientContext* context_, const char* name, unsigned arity): context_(context_), predicate_(new Predicate(name, arity)), types_(arity, UNKNOWN) {
}

bool operator==(const PredicateTables &lhs, const PredicateTables &rhs) {
    return lhs.predicate_ == rhs.predicate_;
}

bool operator!=(const PredicateTables &lhs, const PredicateTables &rhs) {
    return !(lhs == rhs);
}

void PredicateTables::updateTypes(vector<ConstantType>& newTypes) {
    BB_ASSERT(newTypes.size() == types_.size() && "Wrong number of terms for Fact");
    for (idx_t i = 0; i < newTypes.size(); i++) {
        if (newTypes[i] == types_[i]) continue;
        types_[i] = getCommonType(types_[i], newTypes[i]);
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

vector<ConstantType> PredicateTables::getTypes() {
    return types_;
}

joinht_ptr_t & PredicateTables::getJoinHashTable(const vector<idx_t>& keys) {
    for (auto&ht: jhtables_)
        if (ht->checkKeys(keys))
            return ht;

    jhtables_.emplace_back(new JoinHashTable(predicate_.get(), keys, getCount()));
    return jhtables_.back();
}

bool PredicateTables::existJoinHashTable(const vector<idx_t>& keys) {
    for (auto&ht: jhtables_)
        if (ht->checkKeys(keys))
            return true;
    return false;
}


join_prl_ht_ptr_t & PredicateTables::getJoinPRLHashTable( const vector<idx_t> &keys, const vector<idx_t> &payload) {
    BB_ASSERT(existJoinPRLHashTable(keys, payload));
    for (auto&ht: prlHTables_)
        if (ht->checkKeysAndPayloads(keys, payload))
            return ht;

    return prlHTables_.back();
}

void PredicateTables::createJoinPRLHashTable( const vector<ConstantType>& types, const vector<idx_t>& keys,const vector<idx_t>& payload ) {
    if (existJoinPRLHashTable(keys, payload)) return;
    auto ht = join_prl_ht_ptr_t(new JoinPRLHashTable(*context_->bufferManager_, types, keys, payload));
    prlHTables_.push_back(std::move(ht));

}


bool PredicateTables::existJoinPRLHashTable(const vector<idx_t> &keys, const vector<idx_t> &payload) const{
    for (auto&ht: prlHTables_)
        if (ht->checkKeysAndPayloads(keys, payload))
            return true;

    return false;
}

partitioned_agg_ht_ptr_t&  PredicateTables::createPartitionedAggHashTable( const vector<idx_t> &groups, const vector<idx_t> &payloads,
                                                                          const vector<AggregateFunction *> &aggregateFunctions) {
    if (partitionedAggHT_) return partitionedAggHT_;
    partitionedAggHT_ = partitioned_agg_ht_ptr_t(new PartitionedAggHT(*context_, groups, payloads, aggregateFunctions));
    return partitionedAggHT_;
}


bool PredicateTables::existPartitionedAggHashTable() {
    return partitionedAggHT_ != nullptr;
}

void PredicateTables::mergeIntoPRLHT(JoinPRLHashTable &ht) {
    lock_guard lock(mutex_);
    auto &dht = getJoinPRLHashTable(ht.getKeys(), ht.getPayloads());
    if (dht->getTypes() != ht.getTypes()) {
        JoinPRLHashTable::castAndCombine(*context_->bufferManager_ ,dht, ht);
        dht->setReady();
        if (ht.getKeys() == getKeys())
            types_ = dht->getTypes();
        return;
    }
    if (ht.getKeys() == getKeys())
        types_ = dht->getTypes();
    dht->combine(ht);
    dht->setReady();

}

void PredicateTables::initializeChunks() {
    // sync the functions as multiple source operators can call the init
    if (facts_.empty() && ranges_.empty())return;
    lock_guard lock(mutex_);
    LOG_DEBUG("Initializing PredicateTables %s...", predicate_->toString().c_str());
    if (!facts_.empty()) loadFacts();
    if (!ranges_.empty()) loadRanges();
    facts_.clear();
    ranges_.clear();
    if (isDistinct()) {
        // is distinct predicate move the chunks in the HT
        moveChunksToHT();
    }
    LOG_DEBUG("Initializing PredicateTables %s completed", predicate_->toString().c_str());
}

JoinPRLHashTable * PredicateTables::getDistinctHT() const {
    vector<idx_t> keys;
    for (idx_t i=0;i<predicate_->getArity();++i)
        keys.push_back(i);
    JoinPRLHashTable* htPtr = nullptr;
    for (auto&ht: prlHTables_)
        if (ht->checkKeysAndPayloads(keys, {})) {
            htPtr = ht.get();
            break;
        }
    BB_ASSERT(htPtr != nullptr);
    return htPtr;
}

join_prl_ht_ptr_t & PredicateTables::getDistinctUHT() {
    vector<idx_t> keys;
    for (idx_t i=0;i<predicate_->getArity();++i)
        keys.push_back(i);
    BB_ASSERT(existJoinPRLHashTable(getKeys(), {}));
    for (auto&ht: prlHTables_)
        if (ht->checkKeysAndPayloads(keys, {})) {
            return ht;
        }
    // fallback
    return *prlHTables_.end();
}

void PredicateTables::moveChunksToHT(){
    vector<idx_t> keys;
    for (idx_t i=0;i<predicate_->getArity();++i)keys.push_back(i);
    if (!existJoinPRLHashTable(keys, {})) {
        createJoinPRLHashTable(getTypes(), getKeys(), {});
    }
    JoinPRLHashTable* htPtr = getDistinctHT();
    for (auto& chunk:chunks_.chunks()) {
        htPtr->addChunk(*chunk);
    }
}

idx_t PredicateTables::getCount() const {
    if (facts_.empty() && ranges_.empty()) {
        if (!isDistinct())
            return chunks_.getCount();
        if (isRecursive() && delta_.delta_)
            return delta_.delta_->getSize();
        if (existJoinPRLHashTable(getKeys(), {}))
            return getDistinctHT()->getSize();
        return 0;
    }
    auto estimateCount = facts_.size();
    for (auto& atom: ranges_) {
        auto rangeSie = 1;
        BB_ASSERT(atom.getType() == CLASSICAL);
        for (auto& term : atom.getTerms()) {
            if (term.getType() != RANGE)continue;
            rangeSie *= (term.getInterval().to -term.getInterval().from +1);
        }
        estimateCount += rangeSie;
    }
    return estimateCount;
}

void PredicateTables::loadFacts() {
    LOG_DEBUG("Loading facts of %s ...", predicate_->toString().c_str());
    auto types = getTypes();
    BB_ASSERT(types.size() == predicate_->getArity());
    data_chunk_ptr_t chunk = data_chunk_ptr_t(new DataChunk());
    chunk->initialize(types);

    idx_t idx = 0;
    auto chunkCapacity = chunk->getCapacity();
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


vector<data_chunk_ptr_t> getChunksFromRange(Atom &atom, const vector<ConstantType> &types) {
    vector<data_chunk_ptr_t>  chunks;
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
            Vector v(term.getValue().cast(types[i]));
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
    LOG_DEBUG("Loading ranges of %s ...", predicate_->toString().c_str());

    // process the range atoms
    for (auto& atom: ranges_) {
        auto chunks = getChunksFromRange(atom, types_);


        for (auto& chunk: chunks) {
            chunks_.append(*chunk);
        }

    }
}

Value PredicateTables::getValue(idx_t column, idx_t index) {
    return chunks_.getValue(column, index);
}


void PredicateTables::castEntirePredicateTable(const vector<ConstantType> &newTypes) {
    LOG_WARNING("Casting predicate tables of type %s. For better optimization, set the table types explicitly using directives.", predicate_->getName());
    BB_ASSERT(newTypes.size() == types_.size());
    chunks_.cast(newTypes);
    types_ = newTypes;
}

void PredicateTables::append(DataChunk& chunk) {
    BB_ASSERT(chunk.columnCount() == predicate_->getArity());
    lock_guard guard(mutex_);
    BB_ASSERT(chunk.columnCount() == types_.size());
    if (types_.size() > 0 && types_[0] != UNKNOWN && chunk.getTypes() != types_) {
        // different types, check if we need to cast the entire predicate tables
        // or just the chunk
        bool castPredicateTable = false;
        auto ctype = chunk.getTypes();
        vector<ConstantType> commonTypes;
        for (idx_t i =0;i<ctype.size()&&!castPredicateTable;++i) {
            commonTypes.push_back(getCommonType(ctype[i], types_[i]));
            if (commonTypes.back() != types_[i])
                castPredicateTable = true; // common type is not the type that we have in pt
        }

        chunk.cast(commonTypes);
        if (castPredicateTable)
            castEntirePredicateTable(chunk.getTypes());
    }

    if (!types_.empty() && types_[0] == UNKNOWN) {
        // set the types as the chunk types
        types_ = chunk.getTypes();
    }
    if (chunk.getSize() < STANDARD_VECTOR_SIZE) {
        chunks_.append(chunk);
        return;
    }
    // chunk is full so we can just push the pointer
    data_chunk_ptr_t cptr = chunk.clone();
    chunks_.append(std::move(cptr));

    // but check if the second last is full otherwise we need to swap it
    idx_t size = chunks_.chunkCount();
    if (size > 1 && chunks_.getChunk(size - 2).getCapacity() != STANDARD_VECTOR_SIZE) {
        // the second last is not full so swap with the last
        // we want to keep the not full chunks at the end
        chunks_.swapChunks(size - 1 , size - 2);
    }

}


void PredicateTables::setRecursive(bool recursive) {
    recursive_ = recursive;
}

bool PredicateTables::isRecursive() const{
    return recursive_;
}

void PredicateTables::initializeDelta(const vector<ConstantType>& types) {
    BB_ASSERT(isDistinct());
    delta_.delta_ = nullptr;
    delta_.nextDelta_ = join_prl_ht_ptr_t(new JoinPRLHashTable(*context_->bufferManager_, types, getKeys(), {}));
    delta_.deltaCount_ = 0;
}

idx_t PredicateTables::getDeltaCount() const {
    return delta_.deltaCount_;
}

JoinPRLHashTable& PredicateTables::getDelta() {
    if (delta_.delta_)
        return *delta_.delta_;
    // delta is null so return the distinct HT
    return *getDistinctHT();
}

void PredicateTables::mergeIntoNextDelta(JoinPRLHashTable &delta) {
    lock_guard lock(mutex_);
    if (delta_.nextDelta_ == nullptr) {
        // init delta
        initializeDelta(delta.getTypes());
    }
    auto& dht = delta_.nextDelta_;
    if (dht->getTypes() != delta.getTypes()) {
        JoinPRLHashTable::castAndCombine(*context_->bufferManager_, dht, delta);
        return;
    }
    dht->combine(delta);
}

void PredicateTables::mergeDelta() {
    // if dht and next delta are null this recursive predicate was not processed so skip the merge delta phase
    if (!existJoinPRLHashTable(getKeys(), {}) && !delta_.nextDelta_)
        return;
    lock_guard lock(mutex_);
    auto& dht = getDistinctUHT();
    idx_t prevCounter = dht->getSize();
    // merge next delta into distinct ht
    if (delta_.nextDelta_) {
        if (dht->getTypes() == delta_.nextDelta_->getTypes())
            dht->combine(*delta_.nextDelta_);
        else {
            JoinPRLHashTable::castAndCombine(*context_->bufferManager_, dht, *delta_.nextDelta_);
        }

        // swap delta and next delta
        delta_.delta_ = std::move(delta_.nextDelta_);
        delta_.nextDelta_ = join_prl_ht_ptr_t(new JoinPRLHashTable(*context_->bufferManager_, delta_.delta_->getTypes(), getKeys(), {}));
    }
    delta_.deltaCount_ = dht->getSize() - prevCounter;
    types_ = dht->getTypes();
}
}
