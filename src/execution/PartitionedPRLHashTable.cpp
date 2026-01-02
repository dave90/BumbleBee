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

#include "bumblebee/execution/PartitionedPRLHashTable.hpp"

#include "bumblebee/common/Profiler.hpp"

namespace bumblebee {
PartitionedPRLHashTable::PartitionedPRLHashTable(BufferManager &manager, const vector<ConstantType> &types, idx_t partitions): bufferManager_(manager), partitionMutex_(partitions){
    auto t = types;
    initialize(t);
}

PartitionedPRLHashTable::PartitionedPRLHashTable(BufferManager &manager, idx_t partitions): bufferManager_(manager), partitionMutex_(partitions), shift_(0) {
}

void PartitionedPRLHashTable::initialize(const vector<ConstantType> &types) {
    types_ = types;
    entries_ = 0;
    shift_ = (sizeof(hash_t)*8) - std::bit_width(partitionMutex_.size()) + 1;

    partitions_.resize(partitionMutex_.size());
    partitionHashIndex_.resize(partitionMutex_.size(), 0);
}

bool PartitionedPRLHashTable::equalTypes(const vector<ConstantType>& types) const {
    for (idx_t i =0;i<types.size();++i) {
        if (types[i] != types_[i])
            return false;
    }
    return true;
}

void PartitionedPRLHashTable::addChunk(DataChunk &chunk) {
    BB_ASSERT(chunk.getTypes() == getTypes());
    Vector hash(UBIGINT, chunk.getSize());
    chunk.hash(hash);
    addChunk(hash, chunk);
}


void PartitionedPRLHashTable::addChunk(Vector &hash, DataChunk &chunk) {
    idx_t newGroupsCount = 0;
    addChunkInternal(hash, chunk, newGroupsCount, nullptr );
}

void PartitionedPRLHashTable::addChunk(DataChunk &chunk, idx_t& newCount, SelectionVector &newGroupSel) {
    Vector hash(UBIGINT, chunk.getSize());
    chunk.hash(hash);
    addChunkInternal(hash, chunk, newCount, &newGroupSel );
}

struct PartitionInfo {
    PartitionInfo (idx_t size): sel_(size), size_(0){
    };

    SelectionVector sel_;
    idx_t size_;
};

void PartitionedPRLHashTable::addChunkInternal(Vector &hash, DataChunk &chunk, idx_t &newGroupsCount, SelectionVector *newGroupSel) {
    BB_ASSERT(chunk.getTypes() == getTypes());
    // first calculate the partition info

    vector<PartitionInfo> partitionsInfo;
    auto size = chunk.getSize();
    for (idx_t i=0;i<partitionMutex_.size();++i)
        partitionsInfo.emplace_back(size);

    BB_ASSERT(hash.getType() == UBIGINT);
    auto hashesPtr = FlatVector::getData<hash_t>(hash);
    for (idx_t i = 0; i < size; ++i) {
        auto h = hashesPtr[i];
        auto partition = h >> shift_;
        BB_ASSERT(partition < partitionsInfo.size());
        auto &info = partitionsInfo[partition];
        info.sel_.setIndex(info.size_++, i);
    }

    // now let's add the partitions to the ht
    auto added = 0;
    Vector pAddresses(UBIGINT, chunk.getSize());
    Vector pHash(hash);
    SelectionVector sel(chunk.getSize());

    for (idx_t partition = 0; partition < partitionsInfo.size(); ++partition) {
        auto & info = partitionsInfo[partition];
        if (!info.size_) continue;
        DataChunk pChunk;
        pChunk.initializeEmpty(types_);
        pChunk.reference(chunk);
        pChunk.slice(info.sel_, info.size_);
        pHash.reference(hash);
        pHash.slice(info.sel_, info.size_);
        {
            lock_guard lock(partitionMutex_[partition]);
            // create new ht is null
            if (!partitions_[partition])
                partitions_[partition] = distinct_ht_ptr_t(new PRLHashTable(bufferManager_, types_));
            idx_t newRows = 0;
            partitions_[partition]->findOrCreateGroups(pHash, pChunk, pAddresses, newRows, sel);
            added += newRows;
            if (newGroupSel)
                for (idx_t i = 0; i < newRows; ++i)
                    newGroupSel->setIndex(newGroupsCount++, info.sel_.getIndex( sel.getIndex(i) ));
        }
    }
    entries_ += added;
}

idx_t PartitionedPRLHashTable::scan(idx_t offset, DataChunk &result, idx_t size) {
    BB_ASSERT(offset < entries_);
    BB_ASSERT(result.getCapacity() >= size);
    BB_ASSERT(result.columnCount() <= types_.size());
    for (idx_t i = 0; i < result.columnCount(); ++i) {
        BB_ASSERT(result.data_[i].getType() == types_[i]);
        BB_ASSERT(result.data_[i].getVectorType() == VectorType::FLAT_VECTOR);
    }

    // Map global offset -> (partition, local offset)
    DataChunk pchunk;
    pchunk.initialize(result.getTypes());
    BB_ASSERT(pchunk.getCapacity() >= size); // usually STANDARD_VECTOR_SIZE

    // map global offset -> starting partition/local offset
    idx_t base = 0;
    idx_t start_partition = 0;
    idx_t local_offset = 0;

    for (idx_t p = 0; p < partitions_.size(); ++p) {
        if (!partitions_[p]) {
            continue;
        }
        auto psize = partitions_[p]->getSize();
        if (psize == 0) {
            continue;
        }
        if (offset < base + psize) {
            start_partition = p;
            local_offset = offset - base;
            break;
        }
        base += psize;
    }

    idx_t remaining = size;

    for (idx_t p = start_partition; p < partitions_.size() && remaining > 0; ++p) {
        if (!partitions_[p]) {
            local_offset = 0;
            continue;
        }

        auto psize = partitions_[p]->getSize();
        if (psize == 0 || local_offset >= psize) {
            local_offset = 0;
            continue;
        }

        auto to_fetch = minValue<idx_t>(remaining, psize - local_offset);

        partitions_[p]->scan(local_offset, pchunk, to_fetch);
        result.append(pchunk, false);

        remaining -= to_fetch;
        local_offset = 0; // only first partition uses offset
    }
    return result.getSize();
}



void PartitionedPRLHashTable::combine(PartitionedPRLHashTable &other) {
    // combine with other but do not erase data from other

    BB_ASSERT(types_ == other.types_);
    auto added = 0;
    for (idx_t p=0; p < partitions_.size(); ++p) {
        if (!other.partitions_[p] || !other.partitions_[p]->getSize())continue;
        lock_guard lock(partitionMutex_[p]);
        if (!partitions_[p])
            partitions_[p] = distinct_ht_ptr_t(new PRLHashTable(bufferManager_, types_));
        auto prevSize = partitions_[p]->getSize();
        partitions_[p]->combine(*other.partitions_[p]);
        added += partitions_[p]->getSize() - prevSize;
    }
    entries_ += added;
}

idx_t PartitionedPRLHashTable::getSize() const {
    return entries_;
}

vector<idx_t> PartitionedPRLHashTable::getPartitionsSize() const {
    vector<idx_t> sizes;
    for (idx_t p = 0; p < partitions_.size(); ++p) {
        if (partitions_[p])
            sizes.push_back(partitions_[p]->getSize());
        else
            sizes.push_back(0);
    }
    return sizes;
}

string PartitionedPRLHashTable::toString(bool compact) {
    string result = "";
    for (idx_t partition = 0; partition < partitions_.size(); ++partition) {
        auto &ht = partitions_[partition];
        if (!ht) continue;
        result += "Partition "+std::to_string(partition) + "\n";
        result += ht->toString(compact) + "\n";
    }
    return result;
}

vector<ConstantType> PartitionedPRLHashTable::getTypes() const {
    return types_;
}

void PartitionedPRLHashTable::setTypes(const std::vector<ConstantType> &types) {
    types_ = types;
}

void insert(PRLHashTable &h1, PRLHashTable &h2) {
    BB_ASSERT(h1.getTypes() != h2.getTypes());
    BB_ASSERT(h1.getTypes().size() == h2.getTypes().size());

    id_t offset = 0;
    DataChunk chunk;
    chunk.initialize(h1.getTypes());
    DataChunk castChunk;
    castChunk.initialize(h2.getTypes());
    while (offset < h1.getSize()) {
        h1.scan(offset, chunk);
        chunk.cast(castChunk);
        castChunk.setCardinality(chunk.getSize());
        h2.addChunk(castChunk);
        offset += STANDARD_VECTOR_SIZE;
    }
}


void PartitionedPRLHashTable::castAndCombine(BufferManager &manager, std::unique_ptr<PartitionedPRLHashTable> &h1, PartitionedPRLHashTable  &h2) {

    BB_ASSERT(h1->partitions_.size() == h2.partitions_.size());
    auto t1 = h1->types_;
    auto t2 = h2.types_;
    BB_ASSERT(t1.size() == t2.size());
    vector<ConstantType> commonTypes;
    for (idx_t i =0;i<t1.size();++i)
        commonTypes.push_back(getCommonType(t1[i], t2[i]));

    idx_t added = 0;
    for (idx_t p=0; p < h1->partitions_.size(); ++p) {
        lock_guard lock(h1->partitionMutex_[p]);

        auto& p1 = h1->partitions_[p];
        auto& p2 = h2.partitions_[p];
        if (!p1 && !p2) continue;

        if (commonTypes != t1) {
            // we need to cast the h1
            idx_t capacity = p1 ? p1->getCapacity() : HT_INIT_CAPACITY;
            auto newDht = distinct_ht_ptr_t(new PRLHashTable(manager, commonTypes, capacity, true));
            if (p1)
                insert( *p1, *newDht);
            p1 = std::move(newDht);
        }

        if (!p2) continue;
        if (p2->getTypes() != p1->getTypes())
            insert(*p2, *p1);
        else
            p1->combine(*p2);
    }

    h1->setTypes(commonTypes);
    h1->entries_ = 0;
    for (auto& p : h1->partitions_) {
        if (!p) continue;
        h1->entries_ += p->getSize();
    }
}

void PartitionedPRLHashTable::cast(BufferManager &manager, std::unique_ptr<PartitionedPRLHashTable> &h1,
    const vector<ConstantType> &types) {
    auto t2 = types;

    auto t1 = h1->types_;
    BB_ASSERT(t1.size() == t2.size());
    vector<ConstantType> commonTypes;
    for (idx_t i =0;i<t1.size();++i)
        commonTypes.push_back(getCommonType(t1[i], t2[i]));

    for (idx_t p=0; p < h1->partitions_.size(); ++p) {
        lock_guard lock(h1->partitionMutex_[p]);

        auto& p1 = h1->partitions_[p];
        if (!p1) continue;

        if (commonTypes != t1) {
            // we need to cast the h1
            idx_t capacity = p1 ? p1->getCapacity() : HT_INIT_CAPACITY;
            auto newDht = distinct_ht_ptr_t(new PRLHashTable(manager, commonTypes, capacity, true));
            if (p1)
                insert( *p1, *newDht);
            p1 = std::move(newDht);
        }
    }
    h1->setTypes(commonTypes);
}
}
