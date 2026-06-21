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
#include "bumblebee/common/types/Vector.hpp"

#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/Profiler.hpp"
#include "bumblebee/common/types/NullValue.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{
Vector::Vector(Vector &other):type_(other.type_), vtype_(other.vtype_) {
    reference(other);
}

Vector::Vector(Vector &other, const SelectionVector &sel, idx_t count):type_(other.type_) {
    slice(other, sel, count);
}

Vector::Vector(Vector &other, idx_t offset):type_(other.type_), vtype_(other.vtype_) {
    slice(other, offset);
}

Vector::Vector(const Value &other):type_(other.ctype_) {
    reference(other);
}

Vector::Vector(LogicalType type, idx_t capacity):type_(type) {
    initialize(capacity);
}

Vector::Vector(LogicalType type, data_ptr_t dataptr):type_(type), data_(dataptr) {

}

Vector::Vector(LogicalType type, bool create_data, bool zero_data, idx_t capacity):type_(type), data_(nullptr) {
    if (create_data) {
        initialize(zero_data, capacity);
    }
}

Vector::Vector(Vector &&other) noexcept : type_(other.type_), vtype_(other.vtype_), data_(other.data_), dataMngr_(std::move(other.dataMngr_)) , auxDataMngr_(std::move(other.auxDataMngr_)), validity_(std::move(other.validity_)) {

}

void Vector::reference(const Value &value) {
    vtype_ = VectorType::CONSTANT_VECTOR;
    dataMngr_ = VectorDataMngr::createConstantVector(value.isNull_ ? type_.getPhysicalType() : value.ctype_);
    data_ = dataMngr_->getData();
    // free aux data mngr we do not need it
    auxDataMngr_.reset();
    // fresh all-valid mask; setValue handles a NULL value by clearing the bit
    validity_.reset();
    // set the constant value at position 0
    setValue(0, value);
}

void Vector::reference(const Vector &other) {
    reinterpret(other);
}

void Vector::reinterpret(const Vector &other) {
    type_ = other.type_;
    vtype_ = other.vtype_;
    data_ = other.data_;
    assignSharedPointer(dataMngr_, other.dataMngr_);
    assignSharedPointer(auxDataMngr_, other.auxDataMngr_);
    // share the validity buffer (shallow): referenced vectors share their nulls
    validity_ = other.validity_;
}

void Vector::referenceAndSetType(Vector &other) {
    type_ = other.type_;
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
        data_ = data_ + getPhysicalTypeSize(internalType) * offset;
        // the mask must shift with the data; produce an independent shifted copy
        // (only when nulls are actually present — all-valid stays zero-copy)
        if (!other.validity_.allValid()) {
            validity_.slice(other.validity_, offset);
        }
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
    // the authoritative mask now lives on the child; read through the selection
    validity_.reset();
}

void Vector::slice(const SelectionVector &sel, idx_t count, SelCache &cache) {
    if (getVectorType() == VectorType::DICTIONARY_VECTOR) {
        // check if we have a cached entry
        auto &current_sel = DictionaryVector::selVector(*this);
        auto target_data = current_sel.getData();
        auto entry = cache.cache.find(target_data);
        if (entry != cache.cache.end()) {
            // cached entry exists: use that do not calculate the slice
            auto selCached = ((DictionaryDataMngr &)*entry->second).getSelection();
            dataMngr_ = vector_data_mngr_ptr_t( new DictionaryDataMngr(selCached) );
            vtype_ = VectorType::DICTIONARY_VECTOR;
        } else {
            // calculate the slice and store the result in the cache
            slice(sel, count);
            cache.cache[target_data] = this->dataMngr_;
        }
    } else {
        // if is not a dictionary call the normal slice
        slice(sel, count);
    }
}

void Vector::swap(Vector &other) {
    Vector tmp(other);
    other.reference(*this);
    reference(tmp);
}

void Vector::initialize(bool zeroData, idx_t capacity) {
    auxDataMngr_.reset();
    dataMngr_ = VectorDataMngr::createStandardVector(type_.getPhysicalType(), capacity);
    data_ = dataMngr_->getData();
    // fresh vector is all-valid
    validity_.reset();
    if (zeroData)
        memset(data_, 0, getPhysicalTypeSize(type_.getPhysicalType()) * capacity);
}

string Vector::toString(idx_t count) const {
    string s = "Vector type("+std::to_string(static_cast<int>(vtype_))+", " +type_.toString() + "[";
    if (!data_) {
        return s+" NULL ]";
    }
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
    if (vtype_ == VectorType::SEQUENCE_CIRCULAR_VECTOR) {
        int64_t start, offset, stride , end;
        CircularSequenceVector::getSequence(*this, start, offset, stride, end);
        int64_t size = end - start + 1;
        for (unsigned int i = 0; i < count; i++) {
            int64_t val = start + (i+offset) / stride % size;
            s += std::to_string(val) + ", ";
        }
    }

    return s + "]";

}

string Vector::toString() const {
    string s = "Vector type("+std::to_string(static_cast<int>(vtype_))+"), " + type_.toString() +" [";
    switch (vtype_) {
        case VectorType::SEQUENCE_VECTOR:
        case VectorType::SEQUENCE_CIRCULAR_VECTOR:
        case VectorType::FLAT_VECTOR:
        case VectorType::DICTIONARY_VECTOR:
            break;
        case VectorType::CONSTANT_VECTOR:
            s += getValue(0).toString();

    }
    return s + "]";
}

// from constant data stored in position 0 in old data populate the data
template <class T>
static void templatedFlattenConstantVector(data_ptr_t data, data_ptr_t oldData, idx_t count) {
    auto constant = load<T>(oldData);
    auto output = (T *)data;
    for (idx_t i = 0; i < count; i++) {
        output[i] = constant;
    }
}

void Vector::normalify(idx_t count) {
    if (vtype_ == VectorType::FLAT_VECTOR) {
        // vector is already flat
        return;
    }
    if (vtype_ == VectorType::DICTIONARY_VECTOR) {
        idx_t newSize = (count < STANDARD_VECTOR_SIZE)? STANDARD_VECTOR_SIZE : count;
        Vector normVec(getType(), newSize);
        VectorOperations::copy(*this, normVec, count ,0, 0);
        reference(normVec);
        return;
    }
    if (vtype_ == VectorType::SEQUENCE_VECTOR) {
        int64_t start, increment;
        SequenceVector::getSequence(*this, start, increment);
        dataMngr_ = VectorDataMngr::createStandardVector(getType());
        data_ = dataMngr_->getData();
        VectorOperations::generateSequence(*this, count, start, increment);
        return;
    }
    if (vtype_ == VectorType::SEQUENCE_CIRCULAR_VECTOR) {
        int64_t start, stride, end, offset;
        CircularSequenceVector::getSequence(*this, start,offset, stride, end);
        dataMngr_ = VectorDataMngr::createStandardVector(getType());
        data_ = dataMngr_->getData();
        VectorOperations::generateSequence(*this, count, start, offset, stride, end);
        return;
    }
    if (vtype_ == VectorType::CONSTANT_VECTOR) {
        // take old data manager otherwise will remove the old data
        auto oldDataMngr = dataMngr_;
        auto oldData = oldDataMngr->getData();
        // a constant carries a single validity bit (row 0) -> replicate it to every row
        bool constantIsNull = !validity_.rowIsValid(0);
        validity_.reset();
        dataMngr_ = VectorDataMngr::createStandardVector(getType());
        data_ = dataMngr_->getData();
        vtype_ = VectorType::FLAT_VECTOR;
        switch (type_.getPhysicalType()) {
            case PhysicalType::TINYINT:
                templatedFlattenConstantVector<int8_t>(data_, oldData, count);
                break;
            case PhysicalType::SMALLINT:
                templatedFlattenConstantVector<int16_t>(data_, oldData, count);
                break;
            case PhysicalType::INTEGER:
                templatedFlattenConstantVector<int32_t>(data_, oldData, count);
                break;
            case PhysicalType::BIGINT:
                templatedFlattenConstantVector<int64_t>(data_, oldData, count);
                break;
            case PhysicalType::UTINYINT:
                templatedFlattenConstantVector<uint8_t>(data_, oldData, count);
                break;
            case PhysicalType::USMALLINT:
                templatedFlattenConstantVector<uint16_t>(data_, oldData, count);
                break;
            case PhysicalType::UINTEGER:
                templatedFlattenConstantVector<uint32_t>(data_, oldData, count);
                break;
            case PhysicalType::UBIGINT:
                templatedFlattenConstantVector<uint64_t>(data_, oldData, count);
                break;
            case PhysicalType::FLOAT:
                templatedFlattenConstantVector<float>(data_, oldData, count);
                break;
            case PhysicalType::DOUBLE:
                templatedFlattenConstantVector<double>(data_, oldData, count);
                break;
            case PhysicalType::STRING:
                templatedFlattenConstantVector<string_t>(data_, oldData, count);
                break;
            default:
                ;
        }
        if (constantIsNull)
            validity_.setAllInvalid(count);
        return;
    }
    ErrorHandler::errorNotImplemented("Vector::normalify not implemented");
}

void Vector::normalify(const SelectionVector &sel, idx_t count) {
    if (vtype_ == VectorType::FLAT_VECTOR) {
        // vector is already flat
        return;
    }
    if (vtype_ == VectorType::SEQUENCE_VECTOR) {
        int64_t start, increment;
        SequenceVector::getSequence(*this, start, increment);
        dataMngr_ = VectorDataMngr::createStandardVector(getType());
        data_ = dataMngr_->getData();
        VectorOperations::generateSequence(*this, count, sel, start, increment );
        return;
    }
    if (vtype_ == VectorType::SEQUENCE_CIRCULAR_VECTOR) {
        int64_t start, stride, end,offset;
        CircularSequenceVector::getSequence(*this, start,offset, stride, end);
        dataMngr_ = VectorDataMngr::createStandardVector(getType());
        data_ = dataMngr_->getData();
        VectorOperations::generateSequence(*this, count, sel, start, offset, stride, end );
        return;
    }
    ErrorHandler::errorNotImplemented("Vector::normalify not implemented");
}

void Vector::orrify(idx_t count, VectorData &data) {
    if (vtype_ == VectorType::DICTIONARY_VECTOR) {
        auto & selVector = DictionaryVector::selVector(*this);
        auto & child = DictionaryVector::child(*this);
        if (child.getVectorType() == VectorType::FLAT_VECTOR) {
            data.data_ = child.getData();
            data.sel_ = &selVector;
            // validity lives on the child, indexed (like data_) through the selection
            data.validity_ = &child.validity_;
            return ;
        }
        // dictionary with no flat vectory as child, so normalify the child
        Vector childFlat(child);
        childFlat.normalify(selVector, count);
        auxDataMngr_ = vector_data_mngr_ptr_t(new VectorChildDataMngr(std::move(childFlat)));
        data.data_ = FlatVector::getData(DictionaryVector::child(*this));
        data.sel_ = &selVector;
        data.validity_ = &DictionaryVector::child(*this).validity_;
        return;
    }
    if (vtype_ == VectorType::CONSTANT_VECTOR) {
        data.sel_ = ConstantVector::zeroSelectionVector(count, data.owned_sel_);
        data.data_ = ConstantVector::getData(*this);
        // single validity bit (row 0); zero selection makes every read hit bit 0
        data.validity_ = &validity_;
        return;
    }
    // other vector type
    normalify(count);
    data.sel_ = &FlatVector::INCREMENTAL_SELECTION_VECTOR;
    data.data_ = FlatVector::getData(*this);
    data.validity_ = &validity_;
}

void Vector::sequence(int64_t start, int64_t increment) {
    vtype_ = VectorType::SEQUENCE_VECTOR;
    auxDataMngr_.reset();
    dataMngr_  = vector_data_mngr_ptr_t(new VectorDataMngr(2 * sizeof(int64_t)));
    auto data = (int64_t*) dataMngr_->getData();
    data[0] = start;
    data[1] = increment;
}

void Vector::sequence(int64_t start, int64_t offset, int64_t stride, int64_t end) {
    BB_ASSERT(start < end );
    vtype_ = VectorType::SEQUENCE_CIRCULAR_VECTOR;
    auxDataMngr_.reset();
    dataMngr_  = vector_data_mngr_ptr_t(new VectorDataMngr(4 * sizeof(int64_t)));
    auto data = (int64_t*) dataMngr_->getData();
    data[0] = start;
    data[1] = offset;
    data[2] = stride;
    data[3] = end;
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
    switch (getVectorType()){
        case VectorType::CONSTANT_VECTOR:
            // for constant vector value is in index 0
            index = 0;
            break;
        case VectorType::DICTIONARY_VECTOR: {
            // get the rela index from the sel vector
            index = DictionaryVector::selVector(*this).getIndex(index);
            // call the get value from child
            auto &child = DictionaryVector::child(*this);
            return child.getValue(index);
        }
        case VectorType::SEQUENCE_VECTOR: {
            int64_t start, increment;
            SequenceVector::getSequence(*this, start, increment);
            return Value( start + index * increment);
        }
        case VectorType::SEQUENCE_CIRCULAR_VECTOR: {
            int64_t start, stride, end, offset;
            CircularSequenceVector::getSequence(*this, start, offset, stride, end);
            int64_t size = end - start + 1;
            int64_t val = start + ((int64_t)index + offset) / stride % size;
            return Value( val);
        }
        case VectorType::FLAT_VECTOR:
            ;
    }
    // flat / constant vector: index is resolved (0 for constant)
    if (!validity_.rowIsValid(index)) {
        return Value::null();
    }
    switch (type_.getPhysicalType()) {
        case PhysicalType::TINYINT:
            return Value( ((int8_t*)data_)[index] );
        case PhysicalType::SMALLINT:
            return Value( ((int16_t*)data_)[index] );
        case PhysicalType::INTEGER:
            return Value( ((int32_t*)data_)[index] );
        case PhysicalType::BIGINT:
            return Value( ((int64_t*)data_)[index] );
        case PhysicalType::UTINYINT:
            return Value( ((uint8_t*)data_)[index] );
        case PhysicalType::USMALLINT:
            return Value( ((uint16_t*)data_)[index] );
        case PhysicalType::UINTEGER:
            return Value( ((uint32_t*)data_)[index] );
        case PhysicalType::UBIGINT:
            return Value( ((uint64_t*)data_)[index] );
        case PhysicalType::FLOAT:
            return Value( ((float*)data_)[index] );
        case PhysicalType::DOUBLE:
            return Value( ((double*)data_)[index] );
        case PhysicalType::STRING:
            return Value( ((string_t*)data_)[index].getString() );
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type access");
    }
    return Value();
}

void Vector::setValue(idx_t index, const Value &val) {
    if (getVectorType() == VectorType::DICTIONARY_VECTOR) {
        // resolve the real index in the child and delegate
        index = DictionaryVector::selVector(*this).getIndex(index);
        return DictionaryVector::child(*this).setValue(index, val);
    }
    if (val.isNull_) {
        // clear the validity bit; write a defensive NULL fill so raw-data readers
        // never observe garbage (the fill is never used to *detect* nullness).
        writeNullFill(index);
        validity_.setInvalid(index);
        return;
    }

    BB_ASSERT(val.ctype_ == type_.getPhysicalType() && "Error during set value on vector: different types");

    if (getVectorType() == VectorType::SEQUENCE_VECTOR ||
        getVectorType() == VectorType::SEQUENCE_CIRCULAR_VECTOR) {
        ErrorHandler::errorNotImplemented("Unimplemented set type on constant");
    }
    // a previously-null slot becomes valid again (no-op when all-valid)
    validity_.setValid(index);

    // FLAT or CONSTANT vector
    switch (type_.getPhysicalType()) {
        case PhysicalType::TINYINT:
            ((int8_t*)data_)[index] = val.value_.tinyint ;
            break;
        case PhysicalType::SMALLINT:
            ((int16_t*)data_)[index] = val.value_.smallint ;
            break;
        case PhysicalType::INTEGER:
            ((int32_t*)data_)[index] = val.value_.integer ;
            break;
        case PhysicalType::BIGINT:
            ((int64_t*)data_)[index] = val.value_.bigint ;
            break;
        case PhysicalType::UTINYINT:
            ((uint8_t*)data_)[index] = val.value_.utinyint ;
            break;
        case PhysicalType::USMALLINT:
            ((uint16_t*)data_)[index] = val.value_.usmallint ;
            break;
        case PhysicalType::UINTEGER:
            ((uint32_t*)data_)[index] = val.value_.uinteger ;
            break;
        case PhysicalType::UBIGINT:
            ((uint64_t*)data_)[index] = val.value_.ubigint ;
            break;
        case PhysicalType::FLOAT:
            ((float*)data_)[index] = val.value_.float_ ;
            break;
        case PhysicalType::DOUBLE:
            ((double*)data_)[index] = val.value_.double_ ;
            break;
        case PhysicalType::STRING:
            ((string_t*)data_)[index] = StringVector::addString(*this, val.stringValue_);
            break;

        default:
            ErrorHandler::errorNotImplemented("Unimplemented type access");
    }
}


void Vector::writeNullFill(idx_t index) {
    // Defensive physical fill for a NULL slot (never used to detect nullness).
    switch (type_.getPhysicalType()) {
        case PhysicalType::TINYINT:   ((int8_t*)data_)[index]   = NullValue<int8_t>();   break;
        case PhysicalType::SMALLINT:  ((int16_t*)data_)[index]  = NullValue<int16_t>();  break;
        case PhysicalType::INTEGER:   ((int32_t*)data_)[index]  = NullValue<int32_t>();  break;
        case PhysicalType::BIGINT:    ((int64_t*)data_)[index]  = NullValue<int64_t>();  break;
        case PhysicalType::UTINYINT:  ((uint8_t*)data_)[index]  = NullValue<uint8_t>();  break;
        case PhysicalType::USMALLINT: ((uint16_t*)data_)[index] = NullValue<uint16_t>(); break;
        case PhysicalType::UINTEGER:  ((uint32_t*)data_)[index] = NullValue<uint32_t>(); break;
        case PhysicalType::UBIGINT:   ((uint64_t*)data_)[index] = NullValue<uint64_t>(); break;
        case PhysicalType::FLOAT:     ((float*)data_)[index]    = NullValue<float>();    break;
        case PhysicalType::DOUBLE:    ((double*)data_)[index]   = NullValue<double>();   break;
        case PhysicalType::STRING:    ((string_t*)data_)[index] = NullValue<string_t>(); break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for null fill");
    }
}

ValidityMask &Vector::validity() {
    if (vtype_ == VectorType::DICTIONARY_VECTOR)
        return DictionaryVector::child(*this).validity();
    return validity_;
}

const ValidityMask &Vector::validity() const {
    if (vtype_ == VectorType::DICTIONARY_VECTOR)
        return DictionaryVector::child(*this).validity();
    return validity_;
}

bool Vector::rowIsValid(idx_t idx) const {
    switch (vtype_) {
        case VectorType::CONSTANT_VECTOR:
            return validity_.rowIsValid(0);
        case VectorType::DICTIONARY_VECTOR: {
            auto realIdx = DictionaryVector::selVector(*this).getIndex(idx);
            return DictionaryVector::child(*this).rowIsValid(realIdx);
        }
        case VectorType::SEQUENCE_VECTOR:
        case VectorType::SEQUENCE_CIRCULAR_VECTOR:
            return true;
        case VectorType::FLAT_VECTOR:
            ;
    }
    return validity_.rowIsValid(idx);
}

void Vector::setValid(idx_t idx) {
    switch (vtype_) {
        case VectorType::CONSTANT_VECTOR:
            validity_.setValid(0);
            return;
        case VectorType::DICTIONARY_VECTOR: {
            auto realIdx = DictionaryVector::selVector(*this).getIndex(idx);
            DictionaryVector::child(*this).setValid(realIdx);
            return;
        }
        default:
            ;
    }
    validity_.setValid(idx);
}

void Vector::setInvalid(idx_t idx) {
    switch (vtype_) {
        case VectorType::CONSTANT_VECTOR:
            validity_.setInvalid(0);
            return;
        case VectorType::DICTIONARY_VECTOR: {
            auto realIdx = DictionaryVector::selVector(*this).getIndex(idx);
            DictionaryVector::child(*this).setInvalid(realIdx);
            return;
        }
        default:
            ;
    }
    validity_.setInvalid(idx);
}

void Vector::resize(idx_t curSize, idx_t newSize) {
    if (!dataMngr_)
        dataMngr_ = vector_data_mngr_ptr_t(new VectorDataMngr(0));
    BB_ASSERT(data_ && "Data empty unexpected");
    // create a new array with the new size and copy the old data into the new data
    auto newData = std::unique_ptr<data_t[]>(new data_t[newSize * getPhysicalTypeSize(type_.getPhysicalType())]);
    memcpy(newData.get(), data_, curSize * getPhysicalTypeSize(type_.getPhysicalType()));
    dataMngr_->setData(std::move(newData));
    data_ = dataMngr_->getData();
    // grow the validity buffer to match (preserves existing bits, extends as all-valid)
    if (!validity_.allValid())
        validity_.ensureWritable(newSize);
}


void Vector::setVectorType(VectorType vectorType) {
    vtype_ = vectorType;
}

const SelectionVector * ConstantVector::zeroSelectionVector(idx_t count, SelectionVector &ownedSel) {
    if (count <= STANDARD_VECTOR_SIZE)
        return &ConstantVector::ZERO_SELECTION_VECTOR;
    ownedSel.initialize(count);
    for (idx_t i = 0; i < count; i++) {
        ownedSel.setIndex(i, 0);
    }
    return &ownedSel;
}

void ConstantVector::reference(Vector &vector, Vector &source, idx_t position, idx_t count) {
    BB_ASSERT(position < count);
    // get the value and reference to the value
    auto val = source.getValue(position);
    vector.reference(val);
}

string_t StringVector::addString(Vector &vector, const char *data, idx_t len) {
    BB_ASSERT(vector.getType() == PhysicalType::STRING && "Add string on non string vector");
    if (string_t::isInlined(len)) {
        // string is inline no need to store in a heap
        return data;
    }
    if (!vector.auxDataMngr_)
        // init string heap
        vector.auxDataMngr_ = vector_data_mngr_ptr_t(new StringDataMngr());
    BB_ASSERT(vector.auxDataMngr_->getType() == VectorDataMngrType::STRING_BUFFER);
    auto stringDataMngr = (StringDataMngr*)vector.auxDataMngr_.get();
    return stringDataMngr->addString(data, len);
}

string_t StringVector::addString(Vector &vector, const char *data) {
    return addString(vector, string_t(data, strlen(data)));
}

string_t StringVector::addString(Vector &vector, string_t data) {
    return addString(vector, data.c_str(), data.size());
}

string_t StringVector::addString(Vector &vector, const string &data) {
    return addString(vector, string_t(data.c_str(), data.length()));
}

string_t StringVector::emptyString(Vector &vector, idx_t len) {
    BB_ASSERT(vector.getType() == PhysicalType::STRING && "Add string on non string vector");
    if (string_t::isInlined(len)) {
        // string is inline no need to store in a heap
        return string_t(len);
    }
    if (!vector.auxDataMngr_)
        // init string heap
        vector.auxDataMngr_ = vector_data_mngr_ptr_t(new StringDataMngr());
    BB_ASSERT(vector.auxDataMngr_->getType() == VectorDataMngrType::STRING_BUFFER);
    auto stringDataMngr = (StringDataMngr*)vector.auxDataMngr_.get();
    return stringDataMngr->addEmptyString(len);
}

void StringVector::addBuffer(Vector &vector, vector_data_mngr_ptr_t buffer) {
    BB_ASSERT(vector.getType() == PhysicalType::STRING && "Add string on non string vector");
    if (!vector.auxDataMngr_)
        // init string heap
        vector.auxDataMngr_ = vector_data_mngr_ptr_t(new StringDataMngr());
    BB_ASSERT(vector.auxDataMngr_->getType() == VectorDataMngrType::STRING_BUFFER);
    auto stringDataMngr = (StringDataMngr*)vector.auxDataMngr_.get();
    stringDataMngr->addHeapReference(buffer);
}

void StringVector::addHeapReference(Vector &vector, Vector &other) {
    BB_ASSERT(vector.getType() == PhysicalType::STRING && "Add string on non string vector");
 	if (other.getVectorType() == VectorType::DICTIONARY_VECTOR) {
 	    // call the add reference to the child
 	    addHeapReference(vector, DictionaryVector::child(other));
 	}
   if (!other.auxDataMngr_)
        // no heap to append
        return;
    BB_ASSERT(vector.auxDataMngr_->getType() == VectorDataMngrType::STRING_BUFFER);
    BB_ASSERT(other.auxDataMngr_->getType() == VectorDataMngrType::STRING_BUFFER);
    addBuffer(vector, other.auxDataMngr_);
}

void SequenceVector::getSequence(const Vector &vector, int64_t &start, int64_t &increment) {
    BB_ASSERT(vector.getVectorType() == VectorType::SEQUENCE_VECTOR);
    auto data = (int64_t *)vector.dataMngr_->getData();
    start = data[0];
    increment = data[1];
}

void CircularSequenceVector::getSequence(const Vector &vector, int64_t &start, int64_t &offset, int64_t &stride, int64_t &end) {
    BB_ASSERT(vector.getVectorType() == VectorType::SEQUENCE_CIRCULAR_VECTOR);
    auto data = (int64_t *)vector.dataMngr_->getData();
    start = data[0];
    offset = data[1];
    stride = data[2];
    end = data[3];
}
}
