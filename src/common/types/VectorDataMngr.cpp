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
#include "bumblebee/common/types/VectorDataMngr.h"

namespace bumblebee{


VectorDataMngr::VectorDataMngr(VectorDataMngrType type): type_(type) {}

VectorDataMngr::VectorDataMngr(std::unique_ptr<data_t[]> data): data_(std::move(data)), type_(VectorDataMngrType::STANDARD_DATA_MNGR) {
}

VectorDataMngr::VectorDataMngr(idx_t size): data_(new data_t[size]),type_(VectorDataMngrType::STANDARD_DATA_MNGR) {}

VectorDataMngrType VectorDataMngr::getType() {
    return type_;
}

data_ptr_t VectorDataMngr::getData() {
    return data_.get();
}

void VectorDataMngr::setData(std::unique_ptr<data_t[]> data) {
    data_ = std::move(data);
}

VectorDataMngr::vector_data_mngr_ptr_t VectorDataMngr::createStandardVector(ConstantType type, idx_t capacity) {
    return vector_data_mngr_ptr_t(new VectorDataMngr(capacity * getCTypeSize(type)));
}

VectorDataMngr::vector_data_mngr_ptr_t VectorDataMngr::createConstantVector(ConstantType type) {
    return vector_data_mngr_ptr_t(new VectorDataMngr(getCTypeSize(type)));
}

const SelectionVector & DictionaryDataMngr::getSelection() const {
    return sel_;
}

SelectionVector & DictionaryDataMngr::getSelection() {
    return sel_;
}

DictionaryDataMngr::DictionaryDataMngr(const SelectionVector &sel):VectorDataMngr(VectorDataMngrType::DICTIONARY_DATA_MNGR), sel_(sel) {
}

DictionaryDataMngr::DictionaryDataMngr(sel_ptr_t sel):VectorDataMngr(VectorDataMngrType::DICTIONARY_DATA_MNGR), sel_(sel) {
}

DictionaryDataMngr::DictionaryDataMngr(idx_t size):VectorDataMngr(VectorDataMngrType::DICTIONARY_DATA_MNGR), sel_(size) {
}

void DictionaryDataMngr::setSelection(const SelectionVector &sel) {
    sel_.initialize(sel);
}

StringDataMngr::StringDataMngr():VectorDataMngr(VectorDataMngrType::STRING_BUFFER) {}

string_t StringDataMngr::addString(const char *data, idx_t len) {
    return heap_.addString(data, len);
}

string_t StringDataMngr::addString(string_t data) {
    return heap_.addString(data);
}

string_t StringDataMngr::addBlob(string_t data) {
    return heap_.addBlob(data.getDataWriteable(), data.size());
}

void StringDataMngr::addHeapReference(vector_data_mngr_ptr_t heap) {
    references_.push_back(std::move(heap));
}

string_t StringDataMngr::addEmptyString(idx_t len) {
    return heap_.addEmptyString(len);
}
}
