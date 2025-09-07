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
#include "bumblebee//execution/ProbeRLHashTable.hpp"

#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{
ProbeRLHashTable::ProbeRLHashTable(BufferManager &manager, const vector<ConstantType> &types, idx_t capacity,
    bool resizable) : bufferManager_(manager), types_(types), capacity_(capacity), resizable_(resizable),
    entries_(0), payloadPageOffset_(0), bitmask_(capacity-1){
    BB_ASSERT(capacity_ != 0 && (capacity_ & (capacity_ - 1)) == 0); // capacity should be power of 2

    layout_.initialize(types);
    tupleSize_ = layout_.getRowWidth();
    BB_ASSERT(tupleSize_ < Storage::BLOCK_SIZE);
    tuplesPerBlock_ = Storage::BLOCK_SIZE / tupleSize_;

    hashes_ = bufferManager_.allocate(Storage::BLOCK_SIZE);
    hashesPtr_ = hashes_->ptr();

    ProbeRLHashTable::resize(STANDARD_VECTOR_SIZE);
    stringHeap_ = std::make_unique<RowDataCollection>(bufferManager_, (idx_t)Storage::BLOCK_SIZE, 1, true);
}

void ProbeRLHashTable::addChunk(Vector &hash, DataChunk &chunk) {
    Vector addresses(UBIGINT, chunk.getSize());

    findOrCreateGroups(hash, chunk, addresses);
}

idx_t ProbeRLHashTable::scan(idx_t offset, DataChunk &result, idx_t size) {
    BB_ASSERT(offset < entries_);
    BB_ASSERT(result.getCapacity() >= size);
    BB_ASSERT(result.columnCount() == types_.size());
    // find the addresses

    auto remaining = entries_ - offset;
    auto toScan = minValue(size, remaining);
    Vector addresses(UBIGINT, toScan);
    auto addrsPtr = FlatVector::getData<data_ptr_t>(addresses);
    auto chunkIdx = offset / tuplesPerBlock_;
    auto chunkOffset = (offset % tuplesPerBlock_) * tupleSize_;
    BB_ASSERT(chunkOffset + tupleSize_ <= Storage::BLOCK_SIZE);

    auto readPtr = payloadPtrs_[chunkIdx++];
    for (idx_t i = 0; i < toScan; i++) {
        addrsPtr[i] = readPtr + chunkOffset;
        chunkOffset += tupleSize_;
        if (chunkOffset >= tuplesPerBlock_ * tupleSize_) {
            readPtr = payloadPtrs_[chunkIdx++];
            chunkOffset = 0;
        }
    }

    // now fetch the vectors
    for (id_t i = 0;i<types_.size();++i) {
        BB_ASSERT(result.data_[i].getType() == types_[i]);
        BB_ASSERT(result.data_[i].getVectorType() == VectorType::FLAT_VECTOR);
        RowOperations::gather(addresses, FlatVector::INCREMENTAL_SELECTION_VECTOR,
            result.data_[i], FlatVector::INCREMENTAL_SELECTION_VECTOR, toScan,
            layout_.getOffsets()[i]);
    }

    result.setCapacity(toScan);
    result.setCardinality(toScan);
    return result.getSize();
}

void ProbeRLHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses) {
    idx_t matchedCount = 0;
    idx_t newGroupsCount = 0;
    findOrCreateGroupsInternal(hash, groups, addresses, matchedCount, newGroupsCount, true, nullptr, nullptr );
}

void ProbeRLHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses,
    idx_t &matchedCount, bool createGroups, SelectionVector &matchedSel) {
    idx_t newGroupsCount = 0;
    findOrCreateGroupsInternal(hash, groups, addresses, matchedCount, newGroupsCount, createGroups, &matchedSel, nullptr );

}

void ProbeRLHashTable::findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses,
    idx_t &newGroupsCount, SelectionVector &newGroupSel) {
    idx_t matchedCount = 0;
    findOrCreateGroupsInternal(hash, groups, addresses, matchedCount, newGroupsCount, true, nullptr, &newGroupSel );
}

void ProbeRLHashTable::combine(ProbeRLHashTable &other) {
    if (other.entries_ == 0)
        return;

    BB_ASSERT(types_ == other.types_);

    idx_t position = 0;
    while (position < other.entries_) {
        DataChunk group;
        group.initialize(other.types_);
        other.scan(position, group);
        Vector hash(UBIGINT);
        group.hash(hash);
        addChunk(hash, group);

        position += minValue<idx_t>(STANDARD_VECTOR_SIZE, other.entries_ - position);
    }
}

struct PartitionInfo {
    explicit PartitionInfo (): groupAddresses_(UBIGINT), hashes_(UBIGINT), size_(0) {
        addressesPtr_ = FlatVector::getData<data_ptr_t>(groupAddresses_);
        hashesPtr_ = FlatVector::getData<uint64_t>(hashes_);
    };
    Vector groupAddresses_;
    data_ptr_t *addressesPtr_;
    Vector hashes_;
    uint64_t * hashesPtr_;
    idx_t size_;
};

void ProbeRLHashTable::partition(vector<distinct_ht_ptr_t> &partitions, idx_t shift) {
    vector<PartitionInfo> partitionsInfo;
    partitionsInfo.resize(partitions.size());
    for (idx_t i = 0; i < capacity_; ++i) {
        auto hashEntryPtr = (HTEntry64*)hashesPtr_ + i;
        if (hashEntryPtr->pageNum_ == 0 ) continue;
        auto hash = hashEntryPtr->hash_;
        auto p = hash >> shift;
        BB_ASSERT(p < partitionsInfo.size());
        auto &info = partitionsInfo[p];
        info.hashesPtr_[info.size_] = hash;
        info.addressesPtr_[info.size_] = payloadPtrs_[ hashEntryPtr->pageNum_ -1] + (hashEntryPtr->pageOffset_ * tupleSize_);;
        info.size_++;
        if (info.size_ >= STANDARD_VECTOR_SIZE) {
            // merge with the partition table
            if (!partitions[p])
                partitions[p] = distinct_ht_ptr_t(new ProbeRLHashTable(bufferManager_, types_));
            partitions[p]->move(info.groupAddresses_, info.hashes_, info.size_);
            info.size_ = 0;
        }
    }


    idx_t infoIdx = 0;
    idx_t totalCount = 0;
    for (auto &pInfo : partitionsInfo) {
        if (!partitions[infoIdx])
            partitions[infoIdx] = distinct_ht_ptr_t(new ProbeRLHashTable(bufferManager_, types_));
        partitions[infoIdx]->move(pInfo.groupAddresses_, pInfo.hashes_, info.pInfo);
        pInfo.size_ = 0;

        partitions[infoIdx]->stringHeap_->merge(*stringHeap_);
        totalCount += partitions[infoIdx]->getSize();
        infoIdx++;
    }
    BB_ASSERT(totalCount == entries_);

}

idx_t ProbeRLHashTable::getSize() const {
    return entries_;
}

idx_t ProbeRLHashTable::getCapacity() const {
    return capacity_;
}

string ProbeRLHashTable::toString(bool compact) {
    string result = "ProbeRLHashTable - "
        +std::to_string(capacity_)+" - "
        +std::to_string(entries_)+" - "
        +std::to_string(resizable_) +"\n";

    result +="Hash :\n";
    for (idx_t i=0;i<capacity_;++i) {
        auto entry = (HTEntry64*)hashesPtr_ + i;
        if ((entry->pageNum_ == 0 && !compact) || entry->pageNum_ > 0) {
            result += std::to_string(entry->pageNum_)+" - "+std::to_string(entry->pageOffset_)
            +" - "+std::to_string(entry->hash_)+"\n";
        }
    }
    result += "\nPayload : \n";
    payloadApply([&](idx_t page_nr, idx_t page_offset, data_ptr_t ptr) {
        result += std::to_string(page_nr)+" - "+std::to_string(page_offset)+" - ";
        for (idx_t i=0;i<types_.size();++i) {
            auto colOffset = layout_.getOffsets()[i];
            auto col = ptr +colOffset;
            result += " - "+Value::cast(types_[i], col).toString();
        }
        result += "\n";
    });

    return result;
}

vector<ConstantType> ProbeRLHashTable::getTypes() const {
    return types_;
}

void rehash(const HTEntry64* __restrict hashes, HTEntry64* __restrict newHashes, idx_t capacity, idx_t bitmask)
{
    for (idx_t i = 0; i < capacity; ++i) {
        const auto& entry = hashes[i];
        const auto bucket = entry.hash_ & bitmask;
        newHashes[bucket].pageNum_    = entry.pageNum_;
        newHashes[bucket].pageOffset_ = entry.pageOffset_;
        newHashes[bucket].hash_       = entry.hash_;
    }
}

void ProbeRLHashTable::resize(idx_t size) {
    BB_ASSERT(size > capacity_);
    BB_ASSERT(size != 0 && (size & (size - 1)) == 0); // new size should be power of 2
    BB_ASSERT(resizable_);

    auto byteSize =  size * sizeof(HTEntry64);
    auto hashes = bufferManager_.allocate(byteSize);
    auto hashesPtr = hashes->ptr();
    memset(hashesPtr, 0, byteSize);

    rehash((HTEntry64 *)hashesPtr_, (HTEntry64 *)hashesPtr, capacity_, size-1);

    hashes_ = std::move(hashes);
    hashesPtr_ = hashesPtr;
    hashesEndPtr_ = hashesPtr_ + byteSize;
    capacity_ = size;
    bitmask_ = size -1;
}



template <class FUNC>
void ProbeRLHashTable::payloadApply(FUNC fun) {
    if (entries_ == 0) {
        return;
    }
    idx_t apply_entries = entries_;
    idx_t page_nr = 0;
    idx_t page_offset = 0;

    for (auto &payload_chunk_ptr : payloadPtrs_) {
        auto this_entries = minValue(tuplesPerBlock_, apply_entries);
        page_offset = 0;
        auto end = payload_chunk_ptr + this_entries * tupleSize_;
        for (data_ptr_t ptr = payload_chunk_ptr; ptr < end; ptr += tupleSize_)
            fun(page_nr, page_offset++, ptr);

        apply_entries -= this_entries;
        page_nr++;
    }
    BB_ASSERT(apply_entries == 0);
}


void ProbeRLHashTable::newBlock() {
    auto pin = bufferManager_.allocate(Storage::BLOCK_SIZE);
    payload_.push_back(std::move(pin));
    payloadPtrs_.push_back(payload_.back()->ptr());
    payloadPageOffset_ = 0;
}

void ProbeRLHashTable::move(Vector &addresses, Vector &hashes, idx_t count) {
    BB_ASSERT(addresses.getType() == UBIGINT);
    BB_ASSERT(hashes.getType() == UBIGINT);

    DataChunk groups;
    groups.initialize(types_);
    groups.setCardinality(count);
    for (idx_t i = 0; i < groups.columnCount(); i++) {
        auto &column = groups.data_[i];
        const auto colOffset = layout_.getOffsets()[i];
        RowOperations::gather(addresses, FlatVector::INCREMENTAL_SELECTION_VECTOR, column,
                              FlatVector::INCREMENTAL_SELECTION_VECTOR, count, colOffset);
    }

    Vector groupAddresses(UBIGINT);
    Vector hash(UBIGINT);
    groups.hash(hash);

    findOrCreateGroups(hash, groups, groupAddresses);
}

void ProbeRLHashTable::findOrCreateGroupsInternal(Vector &hash, DataChunk &groups,
                                                  Vector &addresses, idx_t &matchedCount, idx_t &newGroupsCount, bool createGroups, SelectionVector *matchedSel,
                                                  SelectionVector *newGroupSel) {

    idx_t size = groups.getSize();
    if (groups.getSize() + entries_ >= capacity_  || (float)(entries_ + size) / (float)capacity_ > LOAD_FACTOR  ) {
        resize(capacity_ * 2);
    }

    BB_ASSERT(entries_ + size < capacity_ );
    BB_ASSERT(groups.columnCount() == types_.size());
    BB_ASSERT(hash.getType() == UBIGINT);

    hash.normalify(size);
    auto groupsHashPtr = FlatVector::getData<uint64_t>(hash);
    // compute the buckets
    Vector buckets(UBIGINT, size);

    auto val = Value(bitmask_);
    Vector mask(val);
    BB_ASSERT(mask.getVectorType() == VectorType::CONSTANT_VECTOR);
    VectorOperations::lAnd(hash, mask, buckets, size);
    buckets.normalify(size);
    auto bucketsPtr = FlatVector::getData<uint64_t>(buckets);

    BB_ASSERT(addresses.getType() == UBIGINT);
    addresses.normalify(size);
    auto addressesPtr = FlatVector::getData<data_ptr_t>(addresses);

    auto groupsData = groups.orrify().get();

    SelectionVector selVector;
    // sel of index and buckets of new groups
    SelectionVector emptySel(size);
    // sel of index and buckets of the to be matched groups
    SelectionVector compareSel(size);
    // sel of the no match groups
    SelectionVector notMatchSel(size);


    idx_t remainingEntries = size;
    matchedCount = 0;
    newGroupsCount = 0;
    while (remainingEntries > 0) {
        // counter for the current iteration
        idx_t newEntryCount = 0;
        idx_t newNeedCompareCount = 0;
        idx_t newNoMatchCount = 0;

        // first figure out if it belongs to a full or empty group
        for (idx_t i = 0; i < remainingEntries; i++) {

            idx_t index = selVector.getIndex(i);
            auto bucket = bucketsPtr[index];
            auto ghash = groupsHashPtr[index];
            auto htEntry = ((HTEntry64*)hashesPtr_) + bucket;
            if (htEntry->pageNum_ == 0) { // page 0 -> empty bucket
                if (!createGroups)continue;

                // create new block if full or no blocks
                if (payloadPageOffset_ == tuplesPerBlock_ || payload_.empty())
                    newBlock();

                auto entry = payloadPtrs_.back() + (payloadPageOffset_ * tupleSize_)
                BB_ASSERT(payloadPageOffset_ < tuplesPerBlock_);

                htEntry->pageNum_ = payload_.size(); // assign to size() as 0 is for empty buckets
                htEntry->pageOffset_ = payloadPageOffset_++;
                htEntry->hash_ = ghash;

                emptySel.setIndex(newEntryCount++, index);
                entries_++;
                addressesPtr[index] = entry;
                if (newGroupSel)
                    newGroupSel->setIndex(newGroupsCount++, index);
            }else {
                // bucket occupied, check the hash
                if (htEntry->hash_ == ghash) {
                    // hash is equal try to match in the next batch
                    compareSel.setIndex(newNeedCompareCount++, index);
                    auto entry = payloadPtrs_[ htEntry->pageNum_ -1] + (htEntry->pageOffset_ * tupleSize_);
                    addressesPtr[index] = entry;
                }else
                    notMatchSel.setIndex(newNoMatchCount++, index);
            }


            // Scatter the new groups in the row storage
            RowOperations::scatter(groups, groupsData, layout_, addresses, *stringHeap_, emptySel, newEntryCount);

            // Now let's try to match the groups with same hash of our ht
            SelectionVector noMatchSel;
            idx_t noMatchCount = 0;
            idx_t matchCount = RowOperations::equal(groups, groupsData, layout_, addresses, compareSel, newNeedCompareCount, &noMatchSel, noMatchCount);
            if (matchedSel) {
                // add the matched index
                for (idx_t j = 0; j< matchCount; j++) {
                    matchedSel->setIndex(matchedCount++, compareSel.getIndex(j));
                }
            }else
                matchedCount += matchCount;

            // each of the entries that do not match we move them to the next bucket in the HT
            for (idx_t j = 0; j< noMatchCount; j++) {
                idx_t idx = noMatchSel.getIndex(j);
                bucketsPtr[idx]++;
                if (bucketsPtr[idx] >= capacity_) {
                    bucketsPtr[idx] = 0;
                }
            }
            // next iteration take in consideration only the non-matched
            selVector.initialize(notMatchSel);
            remainingEntries = noMatchCount;
        }


    }


}
}
