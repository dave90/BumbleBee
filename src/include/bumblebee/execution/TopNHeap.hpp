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
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/types/BumbleString.hpp"
#include "bumblebee/common/vector_operations/CreateSortKey.hpp"

namespace bumblebee{

struct ColModifier {
    OrderModifiers modifier_;
    idx_t col_;
};

struct TopNEntry {
    string_t sortKey_;
    idx_t index_;

    string toString() const {
        string result = "STRING: ";
        auto ptr = sortKey_.getDataUnsafe();
        for (idx_t i = 0; i < sortKey_.size(); ++i) {
            auto byte = (uint8_t)(ptr[i]);
            result += std::to_string(byte)+" - ";
        }
        return result;
    }

    bool operator<(const TopNEntry &other) const {

        return sortKey_ < other.sortKey_;
    }
};

class TopNHeap {
public:

    TopNHeap(const vector<ConstantType> &payloadTypes,const vector<ColModifier> &modifiers, idx_t limit);

    // add the chunk in the heap
    void sink(DataChunk& input);
    // clean the payload and head data not used
    void reduce(bool reduce = false);
    // get data from heap
    void getData(DataChunk& input, idx_t pos = 0);
    // combine with another heap
    void combine(TopNHeap &other);
    // sort the heap
    void finalize();

    vector<OrderModifiers> & getModifiers() {
        return modifiers_;
    }

    vector<idx_t> & getSortCols() {
        return sortCols_;
    }

    vector<ConstantType> & getPayloadTypes() {
        return payloadTypes_;
    }

    idx_t getHeapSize() const {
        return heapSize_;
    }

    idx_t getSize() const {
        return heap_.size();
    }

    string toString() const;

private:

    idx_t getReduceThreshold() const {
        return maxValue<idx_t>(MORSEL_SIZE, 2ULL * heapSize_);
    }

    // return true if we should add the entry
    inline bool shouldAddToHeap(const string_t &sortKey) {
        if (heap_.size() < heapSize_) {
            // heap is not full, insert the key
            return true;
        }
        if (sortKey < heap_.front().sortKey_) {
            // sort key is smaller than current max value, insert it
            return true;
        }
        // heap is full and is > of the max, skip to add it
        return false;
    }

    inline void addEntryToHeap(const TopNEntry &entry) {
        if (heap_.size() >= heapSize_) {
            std::pop_heap(heap_.begin(), heap_.end());
            heap_.pop_back();
        }
        heap_.push_back(entry);
        std::push_heap(heap_.begin(), heap_.end());
    }

    vector<TopNEntry> heap_;
    vector<ConstantType> payloadTypes_;
    vector<OrderModifiers> modifiers_;
    vector<idx_t> sortCols_;
    vector<ConstantType> sortColTypes_;
    idx_t heapSize_;

    DataChunk heapData_;
    DataChunk heapPayload_;

    // Cached objects
    DataChunk keyStrings_;
    SelectionVector dataToInsert_;

    bool finalized_{false};

};


}
