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

TopNHeap::TopNHeap(const vector<LogicalType> &payloadTypes,const vector<ColModifier> &modifiers, idx_t limit): payloadTypes_(payloadTypes),
    heapSize_(limit),
    dataToInsert_(STANDARD_VECTOR_SIZE){
    BB_ASSERT(limit <= STANDARD_VECTOR_SIZE);
    vector<PhysicalType> types {PhysicalType::STRING};
    heapData_.initialize(types);
    keyStrings_.initialize(types);
    heapPayload_.initialize(payloadTypes);

    for (auto& colModifier: modifiers) {
        BB_ASSERT(colModifier.col_ < payloadTypes.size());
        modifiers_.push_back(colModifier.modifier_);
        sortCols_.push_back(colModifier.col_);
        sortColTypes_.push_back(payloadTypes_[colModifier.col_]);
    }

    // Enable the first-column prefilter only for integer first sort columns,
    // where a monotonic order-code can be derived cheaply (covers counts,
    // dates/timestamps and integer keys). Other types use the full path.
    if (!sortColTypes_.empty()) {
        firstColPhysType_ = sortColTypes_[0].getPhysicalType();
        desc0_ = modifiers_[0].order_type == OrderType::DESCENDING;
        switch (firstColPhysType_) {
            case PhysicalType::TINYINT:  case PhysicalType::SMALLINT:
            case PhysicalType::INTEGER:  case PhysicalType::BIGINT:
            case PhysicalType::UTINYINT: case PhysicalType::USMALLINT:
            case PhysicalType::UINTEGER: case PhysicalType::UBIGINT:
                prefilterEnabled_ = true;
                break;
            default:
                prefilterEnabled_ = false;
        }
    }
}

uint64_t TopNHeap::orderCodeAt(Vector &v, idx_t i) const {
    // Map the value to a uint64 that preserves the column's sort order, so that
    // a smaller code means "sorts earlier" (better). Signed types are offset by
    // the sign bit; DESC inverts the code.
    static constexpr uint64_t SIGN = 0x8000000000000000ull;
    uint64_t code;
    switch (firstColPhysType_) {
        case PhysicalType::TINYINT:  code = (uint64_t)(int64_t)FlatVector::getData<int8_t>(v)[i] ^ SIGN; break;
        case PhysicalType::SMALLINT: code = (uint64_t)(int64_t)FlatVector::getData<int16_t>(v)[i] ^ SIGN; break;
        case PhysicalType::INTEGER:  code = (uint64_t)(int64_t)FlatVector::getData<int32_t>(v)[i] ^ SIGN; break;
        case PhysicalType::BIGINT:   code = (uint64_t)FlatVector::getData<int64_t>(v)[i] ^ SIGN; break;
        case PhysicalType::UTINYINT: code = (uint64_t)FlatVector::getData<uint8_t>(v)[i]; break;
        case PhysicalType::USMALLINT:code = (uint64_t)FlatVector::getData<uint16_t>(v)[i]; break;
        case PhysicalType::UINTEGER: code = (uint64_t)FlatVector::getData<uint32_t>(v)[i]; break;
        case PhysicalType::UBIGINT:  code = (uint64_t)FlatVector::getData<uint64_t>(v)[i]; break;
        default: return 0; // unreachable when prefilterEnabled_
    }
    return desc0_ ? ~code : code;
}

bool TopNHeap::chunkCanContribute(DataChunk &input) const {
    // Only meaningful once the heap is full and we can derive a threshold.
    if (!prefilterEnabled_ || heap_.size() < heapSize_) return true;
    const TopNEntry &front = heap_.front();
    if (!front.firstValid_) return true;            // NULL threshold: be conservative
    const uint64_t thresh = front.firstCode_;
    Vector &col = input.data_[sortCols_[0]];
    const idx_t n = input.getSize();
    for (idx_t i = 0; i < n; ++i) {
        if (!col.rowIsValid(i)) return true;        // NULL row: cannot rule out
        // A row can only enter the heap if its first-column code is <= the
        // threshold's (a strictly larger code sorts strictly after the worst
        // kept entry on the dominant column, so it can never qualify).
        if (orderCodeAt(col, i) <= thresh) return true;
    }
    return false;
}

void TopNHeap::sink(DataChunk &input) {
    BB_ASSERT(keyStrings_.columnCount() == 1 && keyStrings_.data_[0].getType() == PhysicalType::STRING);
    BB_ASSERT(input.getSize() <= STANDARD_VECTOR_SIZE);
    // we need to normalify as we will copy only a subset of rows
    input.normalify();

    // Fast path: when the heap is full, skip building sort keys for an entire
    // chunk whose rows are all provably worse than the current threshold.
    if (!chunkCanContribute(input)) return;

    BB_ASSERT(keyStrings_.getCapacity() >= STANDARD_VECTOR_SIZE);
    DataChunk sortChunk;
    sortChunk.initializeEmpty(sortColTypes_);
    sortChunk.reference(input, sortCols_);
    CreateSortKey::createSortKey(sortChunk, modifiers_, keyStrings_.data_[0]);
    keyStrings_.setCardinality(input.getSize());

    auto dataPtr = FlatVector::getData<string_t>(keyStrings_.data_[0]);
    Vector &firstCol = input.data_[sortCols_[0]];
    idx_t count = 0;
    idx_t idx = heapData_.getSize();
    for (idx_t i = 0; i < input.getSize(); ++i) {
        auto& key = dataPtr[i];
        if (!shouldAddToHeap(key))
            continue;
        TopNEntry entry{.sortKey_ = key, .index_ = idx++};
        if (prefilterEnabled_) {
            entry.firstValid_ = firstCol.rowIsValid(i);
            if (entry.firstValid_) entry.firstCode_ = orderCodeAt(firstCol, i);
        }
        dataToInsert_.setIndex(count++, i);
        addEntryToHeap(entry);
    }

    if (!count) return;
    // for all the entry added we need to copy the strings and the payload
    heapData_.append(keyStrings_, true, &dataToInsert_, count);
    heapPayload_.append(input, true, &dataToInsert_, count);
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

void TopNHeap::getData(DataChunk &input, idx_t pos) {
    BB_ASSERT(input.getTypes() == heapPayload_.getTypes());
    if (heap_.empty())return;
    if (pos >= heap_.size()) return;

    // we need to sort the keys, so copy the heap and sort it
    SelectionVector sel(heap_.size());
    idx_t idx = pos;
    for (; idx < heap_.size() && pos < STANDARD_VECTOR_SIZE; ++idx)
        sel.setIndex(idx, heap_[idx].index_);

    input.reference(heapPayload_);
    input.slice(sel, idx - pos);
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

        TopNEntry entry{.sortKey_ = key, .index_ = idx++,
                        .firstCode_ = other.heap_[i].firstCode_,
                        .firstValid_ = other.heap_[i].firstValid_};
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
