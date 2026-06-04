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
#include "bumblebee/common/vector_operations/CreateSortKey.hpp"

#include "bumblebee/common/SortKeyEncoding.hpp"

namespace bumblebee{

struct SortKeyVectorData {
    SortKeyVectorData(Vector &vector, idx_t size): vector_(vector),size_(size) {
    }

    Vector& vector_;
    idx_t size_;
};

using sort_key_data_ptr_t = std::unique_ptr<SortKeyVectorData>;

struct SortKeyLengthInfo {
    explicit SortKeyLengthInfo(idx_t size) : constant_(0) {
        variable_.resize(size, 0);
    }

    idx_t constant_;
    vector<idx_t> variable_;
};



template <class T>
struct SortKeyConstantOperator {
    using TYPE = T;

    static idx_t getEncodeLength(T input) {
        return sizeof(T);
    }

    static idx_t encode(data_ptr_t result, T input) {
        SortKeyEncoding::encodeData<T>(result, input);
        return sizeof(T);
    }

    // NULL encoding: all 0xFF bytes of the natural width. Sorts after any real value
    // when unflipped (NULLS LAST in ASC); the existing DESC flip turns it into all
    // 0x00 (NULLS FIRST in DESC) — the conventional SQL default.
    static idx_t encodeNull(data_ptr_t result) {
        for (idx_t i = 0; i < sizeof(T); i++) result[i] = 0xFF;
        return sizeof(T);
    }
};

struct SortKeyStringOperator {
    static constexpr data_t STRING_DELIMITER = 0;
    // Every STRING key starts with a small prefix byte: NON_NULL_PREFIX for a real
    // value, NON_NULL_PREFIX+1 for a NULL. Both sit above STRING_DELIMITER (0x00)
    // and the +1 gap means a non-null key always sorts strictly before a NULL key
    // under memcmp (NULLS LAST in ASC); the existing whole-key flip for DESC turns
    // them into 0xFE / 0xFD, putting NULLs first there.
    static constexpr data_t NON_NULL_PREFIX = 0x01;
    using TYPE = string_t;

    // +2 = 1 prefix byte + the trailing delimiter.
    static idx_t getEncodeLength(TYPE& input) {
        return input.size() + 2;
    }

    static idx_t encode(data_ptr_t result, TYPE& input) {
        auto input_data = (const_data_ptr_t)input.getDataUnsafe();
        auto input_size = input.size();
        result[0] = NON_NULL_PREFIX;
        for (idx_t r = 0; r < input_size; r++) {
            result[1 + r] = input_data[r] + 1;
        }
        result[1 + input_size] = STRING_DELIMITER;
        return input_size + 2;
    }

    // NULL key is the single prefix-plus-1 byte; nothing follows.
    static idx_t getNullEncodeLength() { return 1; }
    static idx_t encodeNull(data_ptr_t result) {
        result[0] = NON_NULL_PREFIX + 1;
        return 1;
    }
};


struct SortKeyConstructInfo {
    SortKeyConstructInfo(OrderModifiers modifiers_p,vector<idx_t>& offsets, data_ptr_t *result)
        : modifiers(modifiers_p), result_(result), offsets_(offsets) {
        flip_bytes = modifiers.order_type == OrderType::DESCENDING;
    }

    OrderModifiers modifiers;
    data_ptr_t *result_;
    vector<idx_t>& offsets_;
    bool flip_bytes;
};


// ------------------------------------------------------------------------------------------------------------------
// Encode one row's value (or its NULL placeholder), then apply the DESC byte flip,
// and bump the per-row offset. Centralizing this here keeps the constant / flat /
// generic loops below one-liners.
template <class OP, class T>
static inline void encodeOneRow(data_ptr_t result_ptr, idx_t &offset,
                                T value, bool isNull, bool flip) {
    idx_t encode_len = isNull ? OP::encodeNull(result_ptr + offset)
                            : OP::encode(result_ptr + offset, value);

    if (flip) {
        for (idx_t b = offset; b < offset + encode_len; b++) {
            result_ptr[b] = ~result_ptr[b];
        }
    }
    offset += encode_len;
}

template <class OP>
void templatedConstructSortKeyConstant(Vector &vector, idx_t size, SortKeyConstructInfo &info) {
    BB_ASSERT(vector.getVectorType() == VectorType::CONSTANT_VECTOR);
    auto data = ConstantVector::getData<typename OP::TYPE>(vector);
    bool isNull = !vector.rowIsValid(0);
    for (idx_t r = 0; r < size; r++) {
        encodeOneRow<OP>(info.result_[r], info.offsets_[r], data[0], isNull, info.flip_bytes);
    }
}

template <class OP, class T, bool HAS_NULL>
void templatedConstructSortKeyFlat(T* __restrict data, const ValidityMask &validity,
                                   idx_t size, data_ptr_t* __restrict result,
                                   idx_t* __restrict offsets, bool flip) {
    if (HAS_NULL) {
        for (idx_t r = 0; r < size; r++) {
            bool isNull = !validity.rowIsValid(r);
            encodeOneRow<OP>(result[r], offsets[r], data[r], isNull, flip);
        }
    }else {
        for (idx_t r = 0; r < size; r++) {
            encodeOneRow<OP>(result[r], offsets[r], data[r], false, flip);
        }
    }
}

template <class OP, class T, bool HAS_NULL>
void templatedConstructSortKeyGeneric(T* __restrict data, const ValidityMask *validity,
                                      idx_t size, const SelectionVector& sel,
                                      data_ptr_t* __restrict result,
                                      idx_t* __restrict offsets, bool flip) {
    if (HAS_NULL) {
        bool noNulls = !validity || validity->allValid();
        for (idx_t r = 0; r < size; r++) {
            idx_t idx = sel.getIndex(r);
            bool isNull = !noNulls && !validity->rowIsValid(idx);
            encodeOneRow<OP>(result[r], offsets[r], data[idx], isNull, flip);
        }
    }else {
        for (idx_t r = 0; r < size; r++) {
            idx_t idx = sel.getIndex(r);
            encodeOneRow<OP>(result[r], offsets[r], data[idx], false, flip);
        }
    }
}


template <class OP>
void templatedConstructSortKey(SortKeyVectorData &vector_data, SortKeyConstructInfo &info) {
    auto &vector = vector_data.vector_;
    switch (vector.getVectorType()) {
        case VectorType::CONSTANT_VECTOR:
            templatedConstructSortKeyConstant<OP>(vector, vector_data.size_, info);
            break;
        case VectorType::FLAT_VECTOR: {
            auto dataPtr = FlatVector::getData<typename OP::TYPE>(vector);
            if (vector.validity().allValid())
                templatedConstructSortKeyFlat<OP, typename OP::TYPE, false>(
                    dataPtr, FlatVector::validity(vector), vector_data.size_,
                    info.result_, info.offsets_.data(), info.flip_bytes);
            else
                templatedConstructSortKeyFlat<OP, typename OP::TYPE, true>(
                    dataPtr, FlatVector::validity(vector), vector_data.size_,
                    info.result_, info.offsets_.data(), info.flip_bytes);
            break;
        }
        default: {
            VectorData vd;
            vector.orrify(vector_data.size_, vd);
            if (vd.validity_->allValid())
                templatedConstructSortKeyGeneric<OP, typename OP::TYPE, false>(
                    (typename OP::TYPE*)vd.data_, vd.validity_, vector_data.size_,
                    *vd.sel_, info.result_, info.offsets_.data(), info.flip_bytes);
            else
                templatedConstructSortKeyGeneric<OP, typename OP::TYPE, true>(
                    (typename OP::TYPE*)vd.data_, vd.validity_, vector_data.size_,
                    *vd.sel_, info.result_, info.offsets_.data(), info.flip_bytes);
        }
    }
}


static void constructSortKey(SortKeyVectorData &vectorData, SortKeyConstructInfo &info) {
    auto& vector = vectorData.vector_;
    switch (vector.getType()) {
        case PhysicalType::TINYINT:
            templatedConstructSortKey<SortKeyConstantOperator<int8_t>>(vectorData, info);
            break;
        case PhysicalType::SMALLINT:
            templatedConstructSortKey<SortKeyConstantOperator<int16_t>>(vectorData, info);
            break;
        case PhysicalType::INTEGER:
            templatedConstructSortKey<SortKeyConstantOperator<int32_t>>(vectorData, info);
            break;
        case PhysicalType::BIGINT:
            templatedConstructSortKey<SortKeyConstantOperator<int64_t>>(vectorData, info);
            break;
        case PhysicalType::UTINYINT:
            templatedConstructSortKey<SortKeyConstantOperator<uint8_t>>(vectorData, info);
            break;
        case PhysicalType::USMALLINT:
            templatedConstructSortKey<SortKeyConstantOperator<uint16_t>>(vectorData, info);
            break;
        case PhysicalType::UINTEGER:
            templatedConstructSortKey<SortKeyConstantOperator<uint32_t>>(vectorData, info);
            break;
        case PhysicalType::UBIGINT:
            templatedConstructSortKey<SortKeyConstantOperator<uint64_t>>(vectorData, info);
            break;
        case PhysicalType::FLOAT:
            templatedConstructSortKey<SortKeyConstantOperator<float>>(vectorData, info);
            break;
        case PhysicalType::DOUBLE:
            templatedConstructSortKey<SortKeyConstantOperator<double>>(vectorData, info);
            break;
        case PhysicalType::STRING:	{
            templatedConstructSortKey<SortKeyStringOperator>(vectorData, info);
            break;
        }
        default:
            ErrorHandler::errorNotImplemented("Unsupported type in ConstructSortKey");
    }
}



// Per-row length for the STRING encoding: getNullEncodeLength() (=1) for NULL
// rows, getEncodeLength(value) for non-null. Centralized so the constant / flat /
// generic length walkers below stay one-liners.
static inline idx_t stringRowEncodeLength(string_t &value, bool isNull) {
    return isNull ? SortKeyStringOperator::getNullEncodeLength()
                  : SortKeyStringOperator::getEncodeLength(value);
}

static void getSortKeyVariableLengthGeneric(string_t* __restrict data, const ValidityMask *validity,
                                            idx_t size, const SelectionVector& sel,
                                            idx_t* __restrict result) {
    bool noNulls = !validity || validity->allValid();
    for (idx_t i = 0; i < size; ++i) {
        auto idx = sel.getIndex(i);
        bool isNull = !noNulls && !validity->rowIsValid(idx);
        result[i] += stringRowEncodeLength(data[idx], isNull);
    }
}

static void getSortKeyVariableLengthConstant(Vector& data, idx_t size, idx_t* __restrict result) {
    BB_ASSERT(data.getVectorType() == VectorType::CONSTANT_VECTOR);
    auto s = ConstantVector::getData<string_t>(data);
    bool isNull = ConstantVector::isNull(data);
    auto length = stringRowEncodeLength(*s, isNull);
    for (idx_t i = 0; i < size; ++i) {
        result[i] += length;
    }
}

static void getSortKeyVariableLengthFlat(string_t* __restrict data, const ValidityMask &validity,
                                         idx_t size, idx_t* __restrict result) {
    bool allValid = validity.allValid();
    for (idx_t i = 0; i < size; ++i) {
        bool isNull = !allValid && !validity.rowIsValid(i);
        result[i] += stringRowEncodeLength(data[i], isNull);
    }
}


static void getSortKeyLength(SortKeyVectorData &data, SortKeyLengthInfo &result) {
    auto &vector = data.vector_;
    auto type = vector.getType();
    if (typeIsConstantSize(type)) {
        // Fixed-width column: same byte count whether NULL or not (encodeNull pads
        // to sizeof(T)). One constant addend covers every row in this column.
        result.constant_ += getPhysicalTypeSize(type);
        return;
    }

    BB_ASSERT(type == PhysicalType::STRING);
    switch (vector.getVectorType()) {
        case VectorType::CONSTANT_VECTOR:
            getSortKeyVariableLengthConstant(vector, data.size_, result.variable_.data());
            break;
        case VectorType::FLAT_VECTOR: {
            auto dataPtr = FlatVector::getData<string_t>(vector);
            getSortKeyVariableLengthFlat(dataPtr, FlatVector::validity(vector), data.size_, result.variable_.data());
            break;
        }
        default: {
            VectorData vd;
            vector.orrify(data.size_, vd);
            getSortKeyVariableLengthGeneric((string_t*)vd.data_, vd.validity_, data.size_, *vd.sel_, result.variable_.data());
        }
    }
}

static void prepareSortData(Vector &result, idx_t size, SortKeyLengthInfo &keyLengths, data_ptr_t *dataPtr) {
    BB_ASSERT(result.getType() == PhysicalType::STRING);

    auto result_data = FlatVector::getData<string_t>(result);
    for (idx_t r = 0; r < size; r++) {
        auto blob_size = keyLengths.variable_[r] + keyLengths.constant_;
        result_data[r] = StringVector::emptyString(result, blob_size);
        dataPtr[r] = (data_ptr_t)result_data[r].getDataWriteable();
    }
}


static void createSortKeyInternal(vector<sort_key_data_ptr_t> &sortKeyData,
                                  const vector<OrderModifiers> &modifiers, Vector &result, idx_t rowCount) {

    // First get the length of the keyes
    SortKeyLengthInfo keyLengths(rowCount);
    for (auto &vd : sortKeyData) {
        getSortKeyLength(*vd, keyLengths);
    }

    // allocate the empty sort keys
    auto data_pointers = std::unique_ptr<data_ptr_t[]>(new data_ptr_t[rowCount]);
    prepareSortData(result, rowCount, keyLengths, data_pointers.get());

    // now construct the sort keys
    vector<idx_t> offsets;
    offsets.resize(rowCount, 0);
    for (idx_t c = 0; c < sortKeyData.size(); c++) {
        SortKeyConstructInfo info(modifiers[c], offsets, data_pointers.get());
        constructSortKey(*sortKeyData[c], info);
    }
}



void CreateSortKey::createSortKey(DataChunk &input, const vector<OrderModifiers> &modifiers, Vector &result) {
    vector<sort_key_data_ptr_t> sortKeyData;
    BB_ASSERT(modifiers.size() == input.columnCount());
    for (idx_t r = 0; r < modifiers.size(); r++) {
        sortKeyData.push_back(sort_key_data_ptr_t(new SortKeyVectorData(input.data_[r], input.getSize())));
    }
    createSortKeyInternal(sortKeyData, modifiers, result, input.getSize());
}

void CreateSortKey::createSortKey(Vector &input, idx_t size, const OrderModifiers &modifiers, Vector &result) {
    vector<sort_key_data_ptr_t> sortKeyData;
    sortKeyData.push_back(sort_key_data_ptr_t(new SortKeyVectorData(input, size)));
    createSortKeyInternal(sortKeyData, {modifiers}, result, size);
}
}
