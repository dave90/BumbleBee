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
#include "bumblebee/common/types/Vector.h"

#include "bumblebee/common/Helper.h"

namespace bumblebee{
Vector::Vector(Vector &other):ctype_(other.ctype_), vtype_(other.vtype_) {
    reference(other);
}

Vector::Vector(Vector &other, const SelectionVector &sel, idx_t count):ctype_(other.ctype_), vtype_(other.vtype_) {
    slice(other, sel, count);
}

Vector::Vector(Vector &other, idx_t offset):ctype_(other.ctype_), vtype_(other.vtype_) {
    slice(other, offset);
}

Vector::Vector(const Value &other):ctype_(other.ctype_) {
    reference(other);
}

Vector::Vector(ConstantType type, idx_t capacity):ctype_(type) {
    initialize(capacity);
}

Vector::Vector(ConstantType type, data_ptr_t dataptr):ctype_(type), data_(dataptr) {

}

Vector::Vector(ConstantType type, bool create_data, bool zero_data, idx_t capacity):ctype_(type), data_(nullptr) {
    if (create_data) {
        initialize(zero_data, capacity);
    }
}

Vector::Vector(Vector &&other) noexcept : ctype_(other.ctype_), vtype_(other.vtype_), data_(other.data_), dataMngr_(std::move(other.dataMngr_)) , auxDataMngr_(std::move(other.auxDataMngr_)) {

}

void Vector::reference(const Value &value) {
    vtype_ = VectorType::CONSTANT_VECTOR;
    dataMngr_ = VectorDataMngr::createConstantVector(value.ctype_);
    data_ = dataMngr_->getData();
    // free aux data mngr we do not need it
    auxDataMngr_.reset();
    // set the constant value at position 0
    setValue(0, value);
}

void Vector::reference(Vector &other) {
    reinterpret(other);
}

void Vector::reinterpret(Vector &other) {
    data_ = other.data_;
    assignSharedPointer(dataMngr_, other.dataMngr_);
    assignSharedPointer(auxDataMngr_, other.auxDataMngr_);
}

void Vector::referenceAndSetType(Vector &other) {
    ctype_ = other.ctype_;
    reference(other);
}

void Vector::slice(Vector &other, idx_t offset) {
    if (other.vtype_ == VectorType::CONSTANT_VECTOR) {
        // is a constant vector just reference the constant value
        reference(other);
        return;
    }
    // other vector is a flat (no dictionary , for dictionary call slice with selection)
    BB_ASSERT(other.getVectorType() == VectorType::FLAT_VECTOR);
    auto internalType = getType();
    reference(other);
    if (offset > 0) {
        // move the data by offset
        data_ = data_ + getCTypeSize(internalType) * offset;
    }
}

void Vector::slice(Vector &other, const SelectionVector &sel, idx_t count) {
    reference(other);
    slice(sel, count);
}

void Vector::slice(const SelectionVector &sel, idx_t count) {
    if (vtype_ == VectorType::CONSTANT_VECTOR) {
        //  ignore selection vector with constant vector
        return;
    }
    if (vtype_ == VectorType::DICTIONARY_VECTOR) {
        auto currentSel = DictionaryVector::selVector(*this);
        auto newSel = currentSel.slice(sel, count);
        dataMngr_ = vector_data_mngr_ptr_t(new DictionaryDataMngr(std::move(newSel)));
        return;
    }
    // flat vector convert to dictionary
    Vector child(*this);
    vtype_ = VectorType::DICTIONARY_VECTOR;
    dataMngr_ = vector_data_mngr_ptr_t(new DictionaryDataMngr(sel));
    auxDataMngr_ = vector_data_mngr_ptr_t(new VectorChildDataMngr(std::move(child)));
}

void Vector::initialize(bool zeroData, idx_t capacity) {
    auxDataMngr_.reset();
    dataMngr_ = VectorDataMngr::createStandardVector(ctype_, capacity);
    data_ = dataMngr_->getData();
    if (zeroData)
        memset(data_, 0, getCTypeSize(ctype_) * capacity);
}

string Vector::toString(idx_t count) const {
    string s = "Vector type("+std::to_string(static_cast<int>(vtype_))+"), Type("+std::to_string(ctype_)+ ") [";
    if (vtype_ == VectorType::CONSTANT_VECTOR) {
        s += getValue(0).toString();
    }
    if (vtype_ == VectorType::DICTIONARY_VECTOR || vtype_ == VectorType::FLAT_VECTOR) {
        for (unsigned int i = 0; i < count; i++) {
            s += getValue(i).toString() + ", ";
        }
    }
    if (vtype_ == VectorType::SEQUENCE_VECTOR) {
        int64_t start, increment;
        SequenceVector::getSequence(*this, start, increment);
        for (unsigned int i = 0; i < count; i++) {
            s += std::to_string(start +increment * i) + ", ";
        }
    }

    return s + "]";

}

string Vector::toString() const {
    string s = "Vector type("+std::to_string(static_cast<int>(vtype_))+"), Type("+std::to_string(ctype_)+ ") [";
    switch (vtype_) {
        case VectorType::SEQUENCE_VECTOR:
        case VectorType::FLAT_VECTOR:
        case VectorType::DICTIONARY_VECTOR:
            break;
        case VectorType::CONSTANT_VECTOR:
            s += getValue(0).toString();

    }
    return s + "]";
}

void Vector::normalify(idx_t count) {
    if (vtype_ == VectorType::FLAT_VECTOR) {
        // vector is already flat
        return;
    }
    if (vtype_ == VectorType::DICTIONARY_VECTOR) {
        // flatten based on selection vector and child data

        return;
    }
    if (vtype_ == VectorType::CONSTANT_VECTOR) {

    }
}

void Vector::normalify(const SelectionVector &sel, idx_t count) {
}

void Vector::orrify(idx_t count, VectorData &data) {
}

void Vector::sequence(int64_t start, int64_t increment) {
}

void Vector::verify(idx_t count) {
    //TODO
}

void Vector::verify(const SelectionVector &sel, idx_t count) {
    //TODO
}

void Vector::utfVerify(idx_t count) {
    //TODO
}

void Vector::utfVerify(const SelectionVector &sel, idx_t count) {
    //TODO
}

Value Vector::getValue(idx_t index) const {
}

void Vector::setValue(idx_t index, const Value &val) {
}

void Vector::setAuxiliary(vector_data_mngr_ptr_t new_buffer) {
}

void Vector::resize(idx_t cur_size, idx_t new_size) {
}

VectorType Vector::getVectorType() const {
}

const ConstantType & Vector::getType() const {
}

data_ptr_t Vector::getData() {
}

vector_data_mngr_ptr_t Vector::getAuxiliary() {
}

vector_data_mngr_ptr_t Vector::getDataMngr() {
}

void Vector::setVectorType(VectorType vector_type) {
}

const SelectionVector * ConstantVector::zeroSelectionVector(idx_t count, SelectionVector &owned_sel) {
}

void ConstantVector::reference(Vector &vector, Vector &source, idx_t position, idx_t count) {
}

string_t StringVector::addString(Vector &vector, const char *data, idx_t len) {
}

string_t StringVector::addString(Vector &vector, const char *data) {
}

string_t StringVector::addString(Vector &vector, string_t data) {
}

string_t StringVector::addString(Vector &vector, const string &data) {
}

string_t StringVector::emptyString(Vector &vector, idx_t len) {
}

void StringVector::addBuffer(Vector &vector, vector_data_mngr_ptr_t buffer) {
}

void StringVector::addHeapReference(Vector &vector, Vector &other) {
}
}
