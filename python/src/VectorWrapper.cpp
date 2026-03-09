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
#include "include/VectorWrapper.hpp"

#include "bumblebee/common/Limits.hpp"
#include "bumblebee/common/types/Date.hpp"
#include "bumblebee/common/types/Timestamp.hpp"
#include "parquet/parquet_types.h"
#include "utf8proc/utf8proc_wrapper.hpp"

namespace bumblebee::python {


struct RegularConvert {
	template <class BB_T, class NUMPY_T>
	static NUMPY_T convertValue(BB_T val) {
		return (NUMPY_T)val;
	}
};

struct TimestampConvert {
	template <class BB_T, class NUMPY_T>
	static int64_t convertValue(timestamp_t val) {
		return Timestamp::getEpochNanoSeconds(val);
	}
};


struct DateConvert {
	template <class BB_T, class NUMPY_T>
	static int64_t convertValue(date_t val) {
		return Date::epochNanoseconds(val);
	}
};


struct StringConvert {
	template <class T>
	static void convertUnicodeValueTemplated(T *result, int32_t *codepoints, idx_t codepoint_count, const char *data,
	                                         idx_t ascii_count) {
		// we first fill in the batch of ascii characters directly
		for (idx_t i = 0; i < ascii_count; i++) {
			result[i] = data[i];
		}
		// then we fill in the remaining codepoints from our codepoint array
		for (idx_t i = 0; i < codepoint_count; i++) {
			result[ascii_count + i] = codepoints[i];
		}
	}

	static PyObject * convertUnicodeValue(const char *data, idx_t len, idx_t start_pos) {
		// slow path: check the code points
		// we know that all characters before "start_pos" were ascii characters, so we don't need to check those

		// allocate an array of code points so we only have to convert the codepoints once
		// short-string optimization
		// we know that the max amount of codepoints is the length of the string
		// for short strings (less than 64 bytes) we simply statically allocate an array of 256 bytes (64x int32)
		// this avoids memory allocation for small strings (common case)
		idx_t remaining = len - start_pos;
		std::unique_ptr<int32_t[]> allocated_codepoints;
		int32_t static_codepoints[64];
		int32_t *codepoints;
		if (remaining > 64) {
			allocated_codepoints = std::unique_ptr<int32_t[]>(new int32_t[remaining]);
			codepoints = allocated_codepoints.get();
		} else {
			codepoints = static_codepoints;
		}
		// now we iterate over the remainder of the string to convert the UTF8 string into a sequence of codepoints
		// and to find the maximum codepoint
		int32_t max_codepoint = 127;
		int sz;
		idx_t pos = start_pos;
		idx_t codepoint_count = 0;
		while (pos < len) {
			codepoints[codepoint_count] = Utf8Proc::UTF8ToCodepoint(data + pos, sz);
			pos += sz;
			if (codepoints[codepoint_count] > max_codepoint) {
				max_codepoint = codepoints[codepoint_count];
			}
			codepoint_count++;
		}
		// based on the max codepoint, we construct the result string
		auto result = PyUnicode_New(start_pos + codepoint_count, max_codepoint);
		// based on the resulting unicode kind, we fill in the code points
		auto kind = PyUnicode_KIND(result);
		switch (kind) {
		case PyUnicode_1BYTE_KIND:
			convertUnicodeValueTemplated<Py_UCS1>(PyUnicode_1BYTE_DATA(result), codepoints, codepoint_count, data,
			                                      start_pos);
			break;
		case PyUnicode_2BYTE_KIND:
			convertUnicodeValueTemplated<Py_UCS2>(PyUnicode_2BYTE_DATA(result), codepoints, codepoint_count, data,
			                                      start_pos);
			break;
		case PyUnicode_4BYTE_KIND:
			convertUnicodeValueTemplated<Py_UCS4>(PyUnicode_4BYTE_DATA(result), codepoints, codepoint_count, data,
			                                      start_pos);
			break;
		default:
			throw std::runtime_error("Unsupported typekind for Python Unicode Compact decode");
		}
		return result;
	}

	template <class BB_T, class NUMPY_T>
	static PyObject * convertValue(string_t val) {
		// we could use PyUnicode_FromStringAndSize here, but it does a lot of verification that we don't need
		// because of that it is a lot slower than it needs to be
		auto data = (uint8_t *)val.getDataUnsafe();
		auto len = val.size();
		// check if there are any non-ascii characters in there
		for (idx_t i = 0; i < len; i++) {
			if (data[i] > 127) {
				// there are! fallback to slower case
				return convertUnicodeValue((const char *)data, len, i);
			}
		}
		// no unicode: fast path
		// directly construct the string and memcpy it
		auto result = PyUnicode_New(len, 127);
		auto target_data = PyUnicode_DATA(result);
		memcpy(target_data, data, len);
		return result;
	}
};


template <class BB_T, class NUMPY_T, class CONVERT>
static bool convertColumn(idx_t target_offset, data_ptr_t target_data, VectorData &idata, idx_t count) {
	auto src_ptr = (BB_T *)idata.data_;
	auto out_ptr = (NUMPY_T *)target_data;

	for (idx_t i = 0; i < count; i++) {
		idx_t src_idx = idata.sel_->getIndex(i);
		idx_t offset = target_offset + i;
		out_ptr[offset] = CONVERT::template convertValue<BB_T, NUMPY_T>(src_ptr[src_idx]);
	}
	return false;
}

template <class T>
static bool convertColumnRegular(idx_t target_offset, data_ptr_t target_data, VectorData &idata,
								 idx_t count) {
	return convertColumn<T, T, RegularConvert>(target_offset, target_data, idata, count);
}


struct IntegralConvert {
	template <class BB_T, class NUMPY_T>
	static NUMPY_T convertValue(BB_T val) {
		return NUMPY_T(val);
	}

};


template <class BB_T>
static bool convertDecimalInternal(idx_t target_offset, data_ptr_t target_data, VectorData &idata,
								   idx_t count, double division) {
	auto src_ptr = (BB_T *)idata.data_;
	auto out_ptr = (double *)target_data;

	for (idx_t i = 0; i < count; i++) {
		idx_t src_idx = idata.sel_->getIndex(i);
		idx_t offset = target_offset + i;
		out_ptr[offset] = IntegralConvert::convertValue<BB_T, double>(src_ptr[src_idx]) / division;
	}
	return false;
}

static bool convertDecimal(const LogicalType &decimal_type, idx_t target_offset, data_ptr_t target_data,
						    VectorData &idata, idx_t count) {
	auto dec_scale = decimal_type.getDecimalData().scale_;
	double division = pow(10, dec_scale);
	switch (decimal_type.getPhysicalType()) {
		case PhysicalType::SMALLINT:
			return convertDecimalInternal<int16_t>(target_offset, target_data, idata, count, division);
		case PhysicalType::INTEGER:
			return convertDecimalInternal<int32_t>(target_offset, target_data, idata, count, division);
		case PhysicalType::BIGINT:
			return convertDecimalInternal<int64_t>(target_offset, target_data, idata, count, division);
		default:
			throw NotImplementedException("Unimplemented internal type for DECIMAL");
	}
	return false;
}








RawVectorWrapper::RawVectorWrapper(const LogicalType &type) : data_(nullptr), type_(type), count_(0) {
	switch (type.type()) {
	case LogicalTypeId::BOOLEAN:
		width_ = sizeof(bool);
		break;
	case LogicalTypeId::UTINYINT:
		width_ = sizeof(uint8_t);
		break;
	case LogicalTypeId::USMALLINT:
		width_ = sizeof(uint16_t);
		break;
	case LogicalTypeId::UINTEGER:
		width_ = sizeof(uint32_t);
		break;
	case LogicalTypeId::UBIGINT:
		width_ = sizeof(uint64_t);
		break;
	case LogicalTypeId::TINYINT:
		width_ = sizeof(int8_t);
		break;
	case LogicalTypeId::SMALLINT:
		width_ = sizeof(int16_t);
		break;
	case LogicalTypeId::INTEGER:
		width_ = sizeof(int32_t);
		break;
	case LogicalTypeId::BIGINT:
		width_ = sizeof(int64_t);
		break;
	case LogicalTypeId::FLOAT:
		width_ = sizeof(float);
		break;
	case LogicalTypeId::DOUBLE:
	case LogicalTypeId::DECIMAL:
		width_ = sizeof(double);
		break;
	case LogicalTypeId::TIMESTAMP:
	case LogicalTypeId::DATE:
		width_ = sizeof(int64_t);
		break;
	case LogicalTypeId::STRING:
		width_ = sizeof(PyObject *);
		break;
	default:
		throw std::runtime_error("Unsupported type " + type.toString() + " for BumbleBee -> NumPy conversion");
	}
}

void RawVectorWrapper::initialize(idx_t capacity) {
	string dtype;
	switch (type_.type()) {
	case LogicalTypeId::BOOLEAN:
		dtype = "bool";
		break;
	case LogicalTypeId::TINYINT:
		dtype = "int8";
		break;
	case LogicalTypeId::SMALLINT:
		dtype = "int16";
		break;
	case LogicalTypeId::INTEGER:
		dtype = "int32";
		break;
	case LogicalTypeId::BIGINT:
		dtype = "int64";
		break;
	case LogicalTypeId::UTINYINT:
		dtype = "uint8";
		break;
	case LogicalTypeId::USMALLINT:
		dtype = "uint16";
		break;
	case LogicalTypeId::UINTEGER:
		dtype = "uint32";
		break;
	case LogicalTypeId::UBIGINT:
		dtype = "uint64";
		break;
	case LogicalTypeId::FLOAT:
		dtype = "float32";
		break;
	case LogicalTypeId::DOUBLE:
	case LogicalTypeId::DECIMAL:
		dtype = "float64";
		break;
	case LogicalTypeId::TIMESTAMP:
	case LogicalTypeId::DATE:
		dtype = "datetime64[ns]";
		break;
	case LogicalTypeId::STRING:
		dtype = "object";
		break;
	default:
		throw std::runtime_error("unsupported type " + type_.toString());
	}
	array_ = pybind11::array(pybind11::dtype(dtype), capacity);
	data_ = (data_ptr_t)array_.mutable_data();
}

void RawVectorWrapper::resize(idx_t new_capacity) {
	vector<ssize_t> new_shape {ssize_t(new_capacity)};
	array_.resize(new_shape, false);
	data_ = (data_ptr_t)array_.mutable_data();
}


VectorWrapper::VectorWrapper(const LogicalType &type) {
	data_ = std::make_unique<RawVectorWrapper>(type);
}

void VectorWrapper::initialize(idx_t capacity) {
	data_->initialize(capacity);
}

void VectorWrapper::resize(idx_t new_capacity) {
	data_->resize(new_capacity);
}

void VectorWrapper::append(idx_t current_offset, Vector &input, idx_t count) {
	auto dataptr = data_->data_;
	BB_ASSERT(dataptr);
	BB_ASSERT(input.getLogicalTypeId() == data_->type_.type());

	VectorData idata;
	input.orrify(count, idata);
	switch (input.getLogicalTypeId()) {
	case LogicalTypeId::BOOLEAN:
		convertColumnRegular<bool>(current_offset, dataptr, idata, count);
		break;
	case LogicalTypeId::TINYINT:
		convertColumnRegular<int8_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::SMALLINT:
		convertColumnRegular<int16_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::INTEGER:
		convertColumnRegular<int32_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::BIGINT:
		convertColumnRegular<int64_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::UTINYINT:
		convertColumnRegular<uint8_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::USMALLINT:
		convertColumnRegular<uint16_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::UINTEGER:
		convertColumnRegular<uint32_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::UBIGINT:
		convertColumnRegular<uint64_t>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::FLOAT:
		convertColumnRegular<float>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::DOUBLE:
		convertColumnRegular<double>(current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::DECIMAL:
		convertDecimal(input.getLogicalType(), current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::TIMESTAMP:
		convertColumn<timestamp_t, int64_t, TimestampConvert>(
		    current_offset, dataptr,  idata, count);
		break;
	case LogicalTypeId::DATE:
		convertColumn<date_t, int64_t, DateConvert>(current_offset, dataptr, idata, count);
		break;
	case LogicalTypeId::STRING:
		convertColumn<string_t, PyObject *, StringConvert>(current_offset, dataptr,
		                                                                                       idata, count);
		break;
	default:
		throw std::runtime_error("unsupported type " + input.getLogicalType().toString());
	}

	data_->count_ += count;
}

pybind11::object VectorWrapper::toArray() const {
	BB_ASSERT(data_->array_);
	data_->resize(data_->count_);
	// construct numpy arrays from the data and the mask
	auto values = std::move(data_->array_);
	return values;
}


NumpyResultConversion::NumpyResultConversion(const vector<LogicalType> &types, idx_t initial_capacity) : count_(0), capacity_(0) {
	data_.reserve(types.size());
	for (auto &type : types) {
		data_.emplace_back(type);
	}
	resize(initial_capacity);
}
void NumpyResultConversion::append(DataChunk &chunk) {
	if (count_ + chunk.getSize() > capacity_) {
		resize(capacity_ * 2);
	}
	auto chunk_types = chunk.getTypes();
	for (idx_t col_idx = 0; col_idx < data_.size(); col_idx++) {
		data_[col_idx].append(count_, chunk.data_[col_idx], chunk.getSize());
	}
	count_ += chunk.getSize();
}

void NumpyResultConversion::resize(idx_t new_capacity) {
	if (capacity_ == 0) {
		for (auto &data : data_) {
			data.initialize(new_capacity);
		}
	} else {
		for (auto &data : data_) {
			data.resize(new_capacity);
		}
	}
	capacity_ = new_capacity;
}
}
