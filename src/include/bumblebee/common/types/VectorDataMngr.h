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
#include <vector>

#include "SelectionVector.h"
#include "StringHeap.h"
#include "bumblebee/common/Constants.h"
#include "bumblebee/common/TypeDefs.h"

namespace bumblebee{

enum class VectorDataMngrType : uint8_t {
    STANDARD_DATA_MNGR,     // holds a single array of data
    DICTIONARY_DATA_MNGR,   // holds a selection vector
    STRING_BUFFER,          // holds a string heap
    VECTOR_CHILD_BUFFER     // holds a child vector (for vector dictionary)
};

// This class manage the data of a vector
class VectorDataMngr {
public:
    using vector_data_mngr_ptr_t = std::shared_ptr<VectorDataMngr>;
    using vector_vdm_ptr_t = std::vector<vector_data_mngr_ptr_t>;

    explicit VectorDataMngr(VectorDataMngrType type);
    explicit VectorDataMngr(std::unique_ptr<data_t[]> data);
    explicit VectorDataMngr(idx_t size);
    ~VectorDataMngr() = default;

    VectorDataMngrType getType() ;
    data_ptr_t getData();

    void setData(std::unique_ptr<data_t[]> data);

protected:
    VectorDataMngrType type_;
    std::unique_ptr<data_t[]> data_;

public:
    // static method
    static vector_data_mngr_ptr_t createStandardVector(ConstantType type, idx_t capacity = STANDARD_VECTOR_SIZE);
    static vector_data_mngr_ptr_t createConstantVector(ConstantType type);

};

class DictionaryDataMngr : public VectorDataMngr {
public:
    explicit DictionaryDataMngr(const SelectionVector& sel);
    explicit DictionaryDataMngr(sel_ptr_t sel);
    explicit DictionaryDataMngr(idx_t size);

    const SelectionVector& getSelection() const;
    SelectionVector& getSelection();
    void setSelection(const SelectionVector& sel);

private:
    SelectionVector sel_;
};

class StringDataMngr : public VectorDataMngr {
public:
    StringDataMngr();

public:
    string_t addString(const char *data, idx_t len);
    string_t addString(string_t data);
    string_t addBlob(string_t data);
    string_t addEmptyString(idx_t len);
    // take ownership of the heap
    void addHeapReference(vector_data_mngr_ptr_t heap);

private:
    StringHeap heap_;
    // reference to other data mngr and heap (usefully when merge 2 string vector)
    vector_vdm_ptr_t references_;
};

}
