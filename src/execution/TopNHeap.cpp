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
#include "bumblebee/execution/TopNHeap.hpp"

namespace bumblebee{

TopNHeap::TopNHeap(const vector<ConstantType> &payloadTypes,const vector<ColModifier> &modifiers, idx_t limit): payloadTypes_(payloadTypes),
    heapSize_(limit),
    dataToInsert_(STANDARD_VECTOR_SIZE){
    heapData_.initialize({STRING});
    keyStrings_.initialize({STRING});
    heapPayload_.initialize(payloadTypes);

    for (auto& colModifier: modifiers) {
        BB_ASSERT(colModifier.col_ < payloadTypes.size());
        modifiers_.push_back(colModifier.modifier_);
        sortCols_.push_back(colModifier.col_);
        sortColTypes_.push_back(payloadTypes_[colModifier.col_]);
    }
}

void TopNHeap::sink(DataChunk &input) {
    BB_ASSERT(keyStrings_.columnCount() == 1 && keyStrings_.data_[0].getType() == STRING);
    BB_ASSERT(input.getSize() <= STANDARD_VECTOR_SIZE);

    BB_ASSERT(keyStrings_.getCapacity() >= STANDARD_VECTOR_SIZE);
    DataChunk sortChunk;
    sortChunk.initializeEmpty(sortColTypes_);
    sortChunk.reference(input, sortCols_);
    CreateSortKey::createSortKey(sortChunk, modifiers_, keyStrings_.data_[0]);
    keyStrings_.setCardinality(input.getSize());

    auto dataPtr = FlatVector::getData<string_t>(keyStrings_.data_[0]);
    idx_t count = 0;
    idx_t idx = heapData_.getSize();
    for (idx_t i = 0; i < input.getSize(); ++i) {
        auto& key = dataPtr[i];
        if (!shouldAddToHeap(key))
            continue;
        TopNEntry entry{.sortKey_ = key, .index_ = idx++};
        dataToInsert_.setIndex(count++, i);
        addEntryToHeap(entry);
    }

    if (!count) return;
    // for all the entry added we need to copy the strings and the payload
    heapData_.append(keyStrings_, true, &dataToInsert_, count);
    heapPayload_.append(input, true, &dataToInsert_, count);

    int x = 0;
}

void TopNHeap::reduce(bool force) {
    if (!force && heap_.size() < getReduceThreshold())
        return;
    // we need to drop the payload and strings unused data

    // first collect the used data and reset the index
    SelectionVector sel(heap_.size());
    idx_t idx = 0;
    for (auto& e: heap_) {
        sel.setIndex(idx, e.index_);
        e.index_ = idx++;
    }

    // now slice the data and normalize
    heapData_.slice(sel, idx);
    heapData_.normalify();
    heapData_.setCardinality(idx);
    heapData_.setCapacity(idx);
    heapPayload_.slice(sel, idx);
    heapPayload_.normalify();
    heapPayload_.setCardinality(idx);
    heapPayload_.setCapacity(idx);
}

void TopNHeap::getData(DataChunk &input) {
    BB_ASSERT(input.getTypes() == heapPayload_.getTypes());
    if (heap_.empty())return;

    // we need to sort the keys, so copy the heap and sort it
    SelectionVector sel(heap_.size());
    for (idx_t i = 0; i < heap_.size(); ++i)
        sel.setIndex(i, heap_[i].index_);

    input.reference(heapPayload_);
    input.slice(sel, heap_.size());
}

void TopNHeap::combine(TopNHeap &other) {
    BB_ASSERT(payloadTypes_ == other.payloadTypes_);
    BB_ASSERT(modifiers_ == other.modifiers_);
    BB_ASSERT(sortCols_ == other.sortCols_);
    if (other.heap_.empty()) return;

    // expected other is sorted (finalize)
    if (!other.finalized_)
        other.finalize();

    idx_t count = 0;
    idx_t idx = heapData_.getSize();
    for(idx_t i = 0; i < other.heap_.size(); ++i) {
        auto& key = other.heap_[i].sortKey_;
        if (!shouldAddToHeap(key))
            break;

        dataToInsert_.setIndex(count++, other.heap_[i].index_);

        TopNEntry entry{.sortKey_ = key, .index_ = idx++};
        addEntryToHeap(entry);
    }

    if (!count) return;
    // for all the entry added we need to copy the strings and the payload
    heapData_.append(other.heapData_, true, &dataToInsert_, count);
    heapPayload_.append(other.heapPayload_, true, &dataToInsert_, count);

    reduce();
}

void TopNHeap::finalize() {
    std::sort(heap_.begin(), heap_.end());
    finalized_ = true;
}


string TopNHeap::toString() const{
    string result = "HEAP: \n\t";
    for (auto &e : heap_) {
        result += "( ";
        for (idx_t col=0; col < heapPayload_.columnCount(); col++)
            result += heapPayload_.getValue(col, e.index_).toString()+" ,";
        result += "),";
    }

    auto sortHeap = heap_;
    std::sort(sortHeap.begin(), sortHeap.end());
    result += "\nORDERED HEAP: \n\t";
    for (auto &e : sortHeap) {
        result += "( ";
        for (idx_t col=0; col < heapPayload_.columnCount(); col++)
            result += heapPayload_.getValue(col, e.index_).toString()+" ,";
        result += "),";
    }

    return result;
}

}
