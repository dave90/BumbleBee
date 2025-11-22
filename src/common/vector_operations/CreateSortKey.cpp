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

};

struct SortKeyStringOperator {
    static constexpr data_t STRING_DELIMITER = 0;
    using TYPE = string_t;

    static idx_t getEncodeLength(TYPE& input) {
        return input.size() + 1; // +1 for the delimiter
    }

    static idx_t encode(data_ptr_t result, TYPE& input) {
        auto input_data = (const_data_ptr_t)input.getDataUnsafe();
        auto input_size = input.size();
        for (idx_t r = 0; r < input_size; r++) {
            result[r] = input_data[r] + 1;
        }
        result[input_size] = STRING_DELIMITER; // null-byte delimiter
        return input_size + 1;
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
template <class OP>
void templatedConstructSortKeyConstant(Vector &vector, idx_t size, SortKeyConstructInfo &info) {
    BB_ASSERT(vector.getVectorType() == VectorType::CONSTANT_VECTOR);
    auto data = ConstantVector::getData<typename OP::TYPE>(vector);
    auto& offsets = info.offsets_;
    for (idx_t r = 0; r < size; r++) {
        auto result_ptr = info.result_[r];
        auto& offset = offsets[r];

        idx_t encode_len = OP::encode(result_ptr + offset, data[0]);
        if (info.flip_bytes) {
            // descending order - so flip bytes
            for (idx_t b = offset; b < offset + encode_len; b++) {
                result_ptr[b] = ~(result_ptr[b]);
            }
        }
        offset += encode_len;
    }
}

template <class OP, class T>
void templatedConstructSortKeyFlat(T* __restrict data, idx_t size, data_ptr_t* __restrict result,
    idx_t* __restrict offsets,  bool flip) {
    for (idx_t r = 0; r < size; r++) {
        auto result_ptr = result[r];
        auto& offset = offsets[r];

        idx_t encode_len = OP::encode(result_ptr + offset, data[r]);
        if (flip) {
            // descending order - so flip bytes
            for (idx_t b = offset; b < offset + encode_len; b++) {
                result_ptr[b] = ~(result_ptr[b]);
            }
        }
        offset += encode_len;
    }
}

template <class OP, class T>
void templatedConstructSortKeyGeneric(T* __restrict data, idx_t size, const SelectionVector& sel,
    data_ptr_t* __restrict result, idx_t* __restrict offsets,  bool flip) {
    for (idx_t r = 0; r < size; r++) {
        idx_t idx = sel.getIndex(r);
        auto result_ptr = result[r];
        auto& offset = offsets[r];

        idx_t encode_len = OP::encode(result_ptr + offset, data[idx]);
        if (flip) {
            // descending order - so flip bytes
            for (idx_t b = offset; b < offset + encode_len; b++) {
                result_ptr[b] = ~(result_ptr[b]);
            }
        }
        offset += encode_len;
    }
}


template <class OP>
void templatedConstructSortKey(SortKeyVectorData &vector_data, SortKeyConstructInfo &info) {
    switch (vector_data.vector_.getVectorType()) {
        case VectorType::CONSTANT_VECTOR:
            templatedConstructSortKeyConstant<OP>(vector_data.vector_, vector_data.size_, info);
            break;
        case VectorType::FLAT_VECTOR: {
            auto dataPtr = FlatVector::getData<typename OP::TYPE>(vector_data.vector_);
            templatedConstructSortKeyFlat<OP, typename OP::TYPE>(dataPtr, vector_data.size_, info.result_, info.offsets_.data(), info.flip_bytes);
            break;
        }
        default: {
            VectorData vd;
            vector_data.vector_.orrify(vector_data.size_,vd);
            templatedConstructSortKeyGeneric<OP, typename OP::TYPE>((typename OP::TYPE*)vd.data_, vector_data.size_, *vd.sel_, info.result_,info.offsets_.data(), info.flip_bytes);
        }
    }
}


static void constructSortKey(SortKeyVectorData &vectorData, SortKeyConstructInfo &info) {
    auto& vector = vectorData.vector_;
    switch (vector.getType()) {
        case ConstantType::TINYINT:
            templatedConstructSortKey<SortKeyConstantOperator<int8_t>>(vectorData, info);
            break;
        case ConstantType::SMALLINT:
            templatedConstructSortKey<SortKeyConstantOperator<int16_t>>(vectorData, info);
            break;
        case ConstantType::INTEGER:
            templatedConstructSortKey<SortKeyConstantOperator<int32_t>>(vectorData, info);
            break;
        case ConstantType::BIGINT:
            templatedConstructSortKey<SortKeyConstantOperator<int64_t>>(vectorData, info);
            break;
        case ConstantType::UTINYINT:
            templatedConstructSortKey<SortKeyConstantOperator<uint8_t>>(vectorData, info);
            break;
        case ConstantType::USMALLINT:
            templatedConstructSortKey<SortKeyConstantOperator<uint16_t>>(vectorData, info);
            break;
        case ConstantType::UINTEGER:
            templatedConstructSortKey<SortKeyConstantOperator<uint32_t>>(vectorData, info);
            break;
        case ConstantType::UBIGINT:
            templatedConstructSortKey<SortKeyConstantOperator<uint64_t>>(vectorData, info);
            break;
        case ConstantType::FLOAT:
            templatedConstructSortKey<SortKeyConstantOperator<float>>(vectorData, info);
            break;
        case ConstantType::DOUBLE:
            templatedConstructSortKey<SortKeyConstantOperator<double>>(vectorData, info);
            break;
        case ConstantType::STRING:	{
            templatedConstructSortKey<SortKeyStringOperator>(vectorData, info);
            break;
        }
        default:
            ErrorHandler::errorNotImplemented("Unsupported type in ConstructSortKey");
    }
}



static void getSortKeyVariableLengthGeneric(string_t* __restrict data, idx_t size, const SelectionVector& sel, idx_t* __restrict result) {
    for (idx_t i=0; i< size; ++i) {
        auto idx = sel.getIndex(i);
        result[i] += SortKeyStringOperator::getEncodeLength(data[idx]);
    }
}

static void getSortKeyVariableLengthConstant(Vector& data, idx_t size,  idx_t* __restrict result) {
    BB_ASSERT(data.getVectorType() == VectorType::CONSTANT_VECTOR);
    auto s = ConstantVector::getData<string_t>(data);
    auto length = SortKeyStringOperator::getEncodeLength(*s);
    for (idx_t i=0; i< size; ++i) {
        result[i] += length;
    }
}

static void getSortKeyVariableLengthFlat(string_t* __restrict data, idx_t size,  idx_t* __restrict result) {
    for (idx_t i=0; i< size; ++i) {
        result[i] += SortKeyStringOperator::getEncodeLength(data[i]);
    }
}


static void getSortKeyLength(SortKeyVectorData &data, SortKeyLengthInfo &result) {
    // top-level method
    auto type = data.vector_.getType();
    if (typeIsConstantSize(type)) {
        result.constant_ += getCTypeSize(type);
        return;
    }

    BB_ASSERT(data.vector_.getType() == ConstantType::STRING);
    switch (data.vector_.getVectorType()) {
        case VectorType::CONSTANT_VECTOR:
            getSortKeyVariableLengthConstant(data.vector_, data.size_, result.variable_.data());
            break;
        case VectorType::FLAT_VECTOR: {
            auto dataPtr = FlatVector::getData<string_t>(data.vector_);
            getSortKeyVariableLengthFlat(dataPtr, data.size_, result.variable_.data());
            break;
        }
        default: {
            VectorData vd;
            data.vector_.orrify(data.size_,vd);
            getSortKeyVariableLengthGeneric((string_t*)vd.data_, data.size_, *vd.sel_, result.variable_.data());
        }
    }
}

static void prepareSortData(Vector &result, idx_t size, SortKeyLengthInfo &keyLengths, data_ptr_t *dataPtr) {
    BB_ASSERT(result.getType() == STRING);

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

}
