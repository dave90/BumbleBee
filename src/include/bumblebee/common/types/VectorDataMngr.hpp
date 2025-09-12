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
#include <memory>

#include "SelectionVector.hpp"
#include "StringHeap.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{

enum class VectorDataMngrType : uint8_t {
    STANDARD_DATA_MNGR,     // holds a single array of data
    DICTIONARY_DATA_MNGR,   // holds a selection vector
    STRING_BUFFER,          // holds a string heap
    VECTOR_CHILD_BUFFER     // holds a child vector (for vector dictionary)
};

// This class manage the data of a vector
// Critical class -> inlined functions
class VectorDataMngr {
public:
    using vector_data_mngr_ptr_t = std::shared_ptr<VectorDataMngr>;
    using vector_vdm_ptr_t = vector<vector_data_mngr_ptr_t>;

    explicit VectorDataMngr(VectorDataMngrType type): type_(type) {}
    explicit VectorDataMngr(std::unique_ptr<data_t[]> data): data_(std::move(data)), type_(VectorDataMngrType::STANDARD_DATA_MNGR) {}
    explicit VectorDataMngr(idx_t size): data_(new data_t[size]),type_(VectorDataMngrType::STANDARD_DATA_MNGR) {}

    inline VectorDataMngrType getType() {
        return type_;
    }
    inline data_ptr_t getData() {
        return data_.get();
    }
    inline void setData(std::unique_ptr<data_t[]> data) {
        data_ = std::move(data);
    }

protected:
    VectorDataMngrType type_;
    std::unique_ptr<data_t[]> data_;

public:
    // static method
    static inline vector_data_mngr_ptr_t createStandardVector(ConstantType type, idx_t capacity = STANDARD_VECTOR_SIZE) {
        return vector_data_mngr_ptr_t(new VectorDataMngr(capacity * getCTypeSize(type)));
    }
    static inline vector_data_mngr_ptr_t createConstantVector(ConstantType type) {
        return vector_data_mngr_ptr_t(new VectorDataMngr(getCTypeSize(type)));
    }

};

class DictionaryDataMngr : public VectorDataMngr {
public:

    explicit DictionaryDataMngr(const SelectionVector &sel):VectorDataMngr(VectorDataMngrType::DICTIONARY_DATA_MNGR), sel_(sel) {}
    explicit DictionaryDataMngr(sel_ptr_t sel):VectorDataMngr(VectorDataMngrType::DICTIONARY_DATA_MNGR), sel_(sel) {}
    explicit DictionaryDataMngr(idx_t size):VectorDataMngr(VectorDataMngrType::DICTIONARY_DATA_MNGR), sel_(size) {}

    inline const SelectionVector & getSelection() const {
        return sel_;
    }
    inline SelectionVector & getSelection() {
        return sel_;
    }
    inline void setSelection(const SelectionVector &sel) {
        sel_.initialize(sel);
    }

private:
    SelectionVector sel_;
};

class StringDataMngr : public VectorDataMngr {
public:
    StringDataMngr():VectorDataMngr(VectorDataMngrType::STRING_BUFFER) {}

    inline string_t addString(const char *data, idx_t len) {
        return heap_.addString(data, len);
    }
    inline string_t addString(string_t data) {
        return heap_.addString(data);
    }
    inline string_t addBlob(string_t data) {
        return heap_.addBlob(data.getDataWriteable(), data.size());
    }
    // take ownership of the heap
    inline void addHeapReference(vector_data_mngr_ptr_t heap) {
        references_.push_back(std::move(heap));
    }
    inline string_t addEmptyString(idx_t len) {
        return heap_.addEmptyString(len);
    }

private:
    StringHeap heap_;
    // reference to other data mngr and heap (usefully when merge 2 string vector)
    vector_vdm_ptr_t references_;
};

}
