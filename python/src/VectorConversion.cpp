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
#include "include/VectorConversion.hpp"

#include "bumblebee/common/Limits.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/common/types/Timestamp.hpp"
#include "utf8proc/utf8proc_wrapper.hpp"

namespace bumblebee::python {


static void convertPandasType(const string &col_type, LogicalType &bb_col_type, PandasType &pandas_type) {
	if (col_type == "bool") {
		bb_col_type = LogicalTypeId::BOOLEAN;
		pandas_type = PandasType::BOOLEAN;
	} else if (col_type == "uint8" || col_type == "Uint8") {
		bb_col_type = LogicalTypeId::UTINYINT;
		pandas_type = PandasType::UTINYINT;
	} else if (col_type == "uint16" || col_type == "Uint16") {
		bb_col_type = LogicalTypeId::USMALLINT;
		pandas_type = PandasType::USMALLINT;
	} else if (col_type == "uint32" || col_type == "Uint32") {
		bb_col_type = LogicalTypeId::UINTEGER;
		pandas_type = PandasType::UINTEGER;
	} else if (col_type == "uint64" || col_type == "Uint64") {
		bb_col_type = LogicalTypeId::UBIGINT;
		pandas_type = PandasType::UBIGINT;
	} else if (col_type == "int8" || col_type == "Int8") {
		bb_col_type = LogicalTypeId::TINYINT;
		pandas_type = PandasType::TINYINT;
	} else if (col_type == "int16" || col_type == "Int16") {
		bb_col_type = LogicalTypeId::SMALLINT;
		pandas_type = PandasType::SMALLINT;
	} else if (col_type == "int32" || col_type == "Int32") {
		bb_col_type = LogicalTypeId::INTEGER;
		pandas_type = PandasType::INTEGER;
	} else if (col_type == "int64" || col_type == "Int64") {
		bb_col_type = LogicalTypeId::BIGINT;
		pandas_type = PandasType::BIGINT;
	} else if (col_type == "float32") {
		bb_col_type = LogicalTypeId::FLOAT;
		pandas_type = PandasType::FLOAT;
	} else if (col_type == "float64") {
		bb_col_type = LogicalTypeId::DOUBLE;
		pandas_type = PandasType::DOUBLE;
	} else if (col_type == "object") {
		//! this better be castable to strings
		bb_col_type = LogicalTypeId::STRING;
		pandas_type = PandasType::OBJECT;
	} else if (col_type == "string") {
		bb_col_type = LogicalTypeId::STRING;
		pandas_type = PandasType::VARCHAR;
	} else if (col_type == "timedelta64[ns]") {
		// time interval in Nanoseconds
		bb_col_type = LogicalTypeId::BIGINT;
		pandas_type = PandasType::INTERVAL;
	} else {
		throw std::runtime_error("unsupported python type " + col_type);
	}
}

template <class T>
void scanPandasColumn(pybind11::array &numpy_col, idx_t stride, idx_t offset, Vector &out, idx_t count) {
	auto src_ptr = (T *)numpy_col.data();
	if (stride == sizeof(T)) {
		FlatVector::setData(out, (data_ptr_t)(src_ptr + offset));
	} else {
		auto tgt_ptr = (T *)FlatVector::getData(out);
		for (idx_t i = 0; i < count; i++) {
			tgt_ptr[i] = src_ptr[stride / sizeof(T) * (i + offset)];
		}
	}
}


template <class T>
void scanPandasNumeric(PandasColumnBindData &bind_data, idx_t count, idx_t offset, Vector &out) {
	scanPandasColumn<T>(bind_data.numpyCol_, bind_data.numpyStride_, offset, out, count);
	if (bind_data.mask_) {
		auto mask = (bool *)bind_data.mask_->numpyArray_.data();
		auto ptr = (T *)FlatVector::getData(out);
		for (idx_t i = 0; i < count; i++) {
			auto is_null = mask[offset + i];
			if (is_null) {
				// Set null as max value
				ptr[i] = NumericLimits<T>::maximum();
			}
		}
	}
}


template <class T>
void scanPandasFpColumn(T *src_ptr, idx_t count, idx_t offset, Vector &out) {
	FlatVector::setData(out, (data_ptr_t)(src_ptr + offset));
}


template <class T>
static string_t decodePythonUnicode(T *codepoints, idx_t codepoint_count, Vector &out) {
	// first figure out how many bytes to allocate
	idx_t utf8_length = 0;
	for (idx_t i = 0; i < codepoint_count; i++) {
		int len = Utf8Proc::CodepointLength(int(codepoints[i]));
		BB_ASSERT(len >= 1);
		utf8_length += len;
	}
	int sz;
	auto result = StringVector::emptyString(out, utf8_length);
	auto target = result.getDataWriteable();
	for (idx_t i = 0; i < codepoint_count; i++) {
		Utf8Proc::CodepointToUtf8(int(codepoints[i]), sz, target);
		BB_ASSERT(sz >= 1);
		target += sz;
	}
	return result;
}

template <class T>
void scanPandasCategoryTemplated(pybind11::array &column, idx_t offset, Vector &out, idx_t count,  const vector<string> &enumEntries) {
	auto tgt_ptr = FlatVector::getData<string_t>(out);
	auto src_ptr = (T *)column.data();
	for (idx_t i = 0; i < count; i++) {
		auto idx = src_ptr[i + offset];
		if (idx == -1 || idx >= enumEntries.size()) {
			// Null value
			tgt_ptr[i] = string_t("");
		} else {
			tgt_ptr[i] = StringVector::addString(out, enumEntries[idx]);
		}
	}
}

void scanPandasCategory(pybind11::array &column, idx_t count, idx_t offset, Vector &out, string &src_type, const vector<string> &enumEntries) {
	if (src_type == "int8") {
		scanPandasCategoryTemplated<int8_t>(column, offset, out, count, enumEntries);
	} else if (src_type == "int16") {
		scanPandasCategoryTemplated<int16_t>(column, offset, out, count, enumEntries);
	} else if (src_type == "int32") {
		scanPandasCategoryTemplated<int32_t>(column, offset, out, count, enumEntries);
	} else {
		throw std::runtime_error("The Pandas type " + src_type + " for categorical types is not implemented yet");
	}
}



void VectorConversion::numpyToBumbleBee(PandasColumnBindData &bind_data, pybind11::array &numpy_col,
    idx_t count, idx_t offset, Vector &out) {
switch (bind_data.pandasType_) {
	case PandasType::BOOLEAN:
		scanPandasColumn<bool>(numpy_col, bind_data.numpyStride_, offset, out, count);
		break;
	case PandasType::UTINYINT:
		scanPandasNumeric<uint8_t>(bind_data, count, offset, out);
		break;
	case PandasType::USMALLINT:
		scanPandasNumeric<uint16_t>(bind_data, count, offset, out);
		break;
	case PandasType::UINTEGER:
		scanPandasNumeric<uint32_t>(bind_data, count, offset, out);
		break;
	case PandasType::UBIGINT:
		scanPandasNumeric<uint64_t>(bind_data, count, offset, out);
		break;
	case PandasType::TINYINT:
		scanPandasNumeric<int8_t>(bind_data, count, offset, out);
		break;
	case PandasType::SMALLINT:
		scanPandasNumeric<int16_t>(bind_data, count, offset, out);
		break;
	case PandasType::INTEGER:
		scanPandasNumeric<int32_t>(bind_data, count, offset, out);
		break;
	case PandasType::INTERVAL:
	case PandasType::BIGINT:
		scanPandasNumeric<int64_t>(bind_data, count, offset, out);
		break;
	case PandasType::FLOAT:
		scanPandasFpColumn<float>((float *)numpy_col.data(), count, offset, out);
		break;
	case PandasType::DOUBLE:
		scanPandasFpColumn<double>((double *)numpy_col.data(), count, offset, out);
		break;
	case PandasType::TIMESTAMP: {
		auto src_ptr = (int64_t *)numpy_col.data();
		auto tgt_ptr = FlatVector::getData<timestamp_t>(out);

		for (idx_t row = 0; row < count; row++) {
			auto source_idx = offset + row;
			if (src_ptr[source_idx] <= NumericLimits<int64_t>::minimum()) {
				// pandas Not a Time (NaT)
				// flag it
				tgt_ptr[row] = NumericLimits<int64_t>::maximum();
				continue;
			}
			tgt_ptr[row] = Timestamp::fromEpochNano(src_ptr[source_idx]);
		}
		break;
	}
	case PandasType::VARCHAR:
	case PandasType::OBJECT: {
		auto src_ptr = (PyObject **)numpy_col.data();
		auto tgt_ptr = FlatVector::getData<string_t>(out);
		// TODO null object as empty string, find a better way :(
		std::unique_ptr<PythonGILWrapper> gil;
		for (idx_t row = 0; row < count; row++) {
			auto source_idx = offset + row;
			PyObject *val = src_ptr[source_idx];
			if (bind_data.pandasType_ == PandasType::OBJECT && !PyUnicode_CheckExact(val)) {
				if (val == Py_None) {
					tgt_ptr[row] = string_t("");
					continue;
				}
				if (pybind11::isinstance<pybind11::float_>(val) && std::isnan(PyFloat_AsDouble(val))) {
					tgt_ptr[row] = string_t("");
					continue;
				}
				if (!pybind11::isinstance<pybind11::str>(val)) {
					if (!gil) {
						gil = bind_data.objectStrVal_.getLock();
					}
					bind_data.objectStrVal_.assignInternal<PyObject>(
					    [](pybind11::str &obj, PyObject &new_val) {
						    pybind11::handle object_handle = &new_val;
						    obj = pybind11::str(object_handle);
					    },
					    *val, *gil);
					val = (PyObject *)bind_data.objectStrVal_.getPointerTop()->ptr();
				}
			}
			// Python 3 string representation:
			// https://github.com/python/cpython/blob/3a8fdb28794b2f19f6c8464378fb8b46bce1f5f4/Include/cpython/unicodeobject.h#L79
			if (!PyUnicode_CheckExact(val)) {
				tgt_ptr[row] = string_t("");
				continue;
			}
			if (PyUnicode_IS_COMPACT_ASCII(val)) {
				// ascii string: we can zero copy
				tgt_ptr[row] = string_t((const char *)PyUnicode_DATA(val), PyUnicode_GET_LENGTH(val));
			} else {
				// unicode gunk
				auto ascii_obj = (PyASCIIObject *)val;
				auto unicode_obj = (PyCompactUnicodeObject *)val;
				// compact unicode string: is there utf8 data available?
				if (unicode_obj->utf8) {
					tgt_ptr[row] = string_t((const char *)unicode_obj->utf8, unicode_obj->utf8_length);
				} else if (PyUnicode_IS_COMPACT(unicode_obj) && !PyUnicode_IS_ASCII(unicode_obj)) {
					auto kind = PyUnicode_KIND(val);
					switch (kind) {
					case PyUnicode_1BYTE_KIND:
						tgt_ptr[row] =
						    decodePythonUnicode<Py_UCS1>(PyUnicode_1BYTE_DATA(val), PyUnicode_GET_LENGTH(val), out);
						break;
					case PyUnicode_2BYTE_KIND:
						tgt_ptr[row] =
						    decodePythonUnicode<Py_UCS2>(PyUnicode_2BYTE_DATA(val), PyUnicode_GET_LENGTH(val), out);
						break;
					case PyUnicode_4BYTE_KIND:
						tgt_ptr[row] =
						    decodePythonUnicode<Py_UCS4>(PyUnicode_4BYTE_DATA(val), PyUnicode_GET_LENGTH(val), out);
						break;
					default:
						throw std::runtime_error("Unsupported type kind for Python Unicode Compact decode");
					}
				}  else {
					throw std::runtime_error("Unsupported string type in pandas dataframe :(");
				}
			}
		}
		break;
	}
	case PandasType::CATEGORY: {
		BB_ASSERT(out.getType() == PhysicalType::STRING);
		scanPandasCategory(numpy_col, count, offset, out, bind_data.internalCategoricalType_, bind_data.enumEntries_);
		break;
	}

	default:
		throw std::runtime_error("Unsupported type " + out.getLogicalType().toString());
	}
}

void python::VectorConversion::bindPandas(pybind11::handle original_df, vector<PandasColumnBindData> &out,
    vector<LogicalType> &return_types, vector<string> &names) {

    // This performs a shallow copy that allows us to rename the dataframe
	auto df = original_df.attr("copy")(false);
	auto df_columns = pybind11::list(df.attr("columns"));
	auto df_types = pybind11::list(df.attr("dtypes"));
	auto get_fun = df.attr("__getitem__");
	// TODO support masked arrays as well
	// TODO support dicts of numpy arrays as well
	if (pybind11::len(df_columns) == 0 || pybind11::len(df_types) == 0 || pybind11::len(df_columns) != pybind11::len(df_types)) {
		throw std::runtime_error("Need a DataFrame with at least one column");
	}

	// check if names in pandas dataframe are unique
	std::unordered_map<string, idx_t> pandas_column_names_map;
	pybind11::array column_attributes = df.attr("columns").attr("values");
	for (idx_t col_idx = 0; col_idx < pybind11::len(df_columns); col_idx++) {
		auto column_name_py = pybind11::str(df_columns[col_idx]);
		pandas_column_names_map[column_name_py]++;
		if (pandas_column_names_map[column_name_py] > 1) {
			// If the column name is repeated we start adding _x where x is the repetition number
			string column_name = column_name_py;
			column_name += "_" + std::to_string(pandas_column_names_map[column_name_py] - 1);
			auto new_column_name_py = pybind11::str(column_name);
			names.emplace_back(new_column_name_py);
			column_attributes[pybind11::cast(col_idx)] = new_column_name_py;
			pandas_column_names_map[new_column_name_py]++;
		} else {
			names.emplace_back(column_name_py);
		}
	}

	for (idx_t col_idx = 0; col_idx < pybind11::len(df_columns); col_idx++) {
		LogicalType bb_col_type;
		PandasColumnBindData bind_data;
		auto col_type = string(pybind11::str(df_types[col_idx]));
		if (col_type == "Int8" || col_type == "Int16" || col_type == "Int32" || col_type == "Int64") {
			// numeric object
			// fetch the internal data and mask array
			bind_data.numpyCol_ = get_fun(df_columns[col_idx]).attr("array").attr("_data");
			bind_data.mask_ = std::make_unique<NumPyArrayWrapper>(get_fun(df_columns[col_idx]).attr("array").attr("_mask"));
			convertPandasType(col_type, bb_col_type, bind_data.pandasType_);
		} else if (StringUtils::startsWith(col_type, "datetime64[ns") || col_type == "<M8[ns]") {
			// timestamp type
			bind_data.numpyCol_ = get_fun(df_columns[col_idx]).attr("array").attr("_data");
			bind_data.mask_ = nullptr;
			bb_col_type = LogicalTypeId::TIMESTAMP;
			bind_data.pandasType_ = PandasType::TIMESTAMP;
		} else {
			// regular type
			auto column = get_fun(df_columns[col_idx]);
			if (col_type == "category") {
				// for category types, we create an ENUM type for string or use the converted numpy type for the rest
				BB_ASSERT(pybind11::hasattr(column, "cat"));
				BB_ASSERT(pybind11::hasattr(column.attr("cat"), "categories"));
				auto categories = pybind11::array(column.attr("cat").attr("categories"));
				auto category_type = string(pybind11::str(categories.attr("dtype")));
				if (category_type == "object") {
					// Let's hope the object type is a string.
					bind_data.pandasType_ = PandasType::CATEGORY;
					auto enum_name = string(pybind11::str(df_columns[col_idx]));
					vector<string> enum_entries = pybind11::cast<vector<string>>(categories);
					BB_ASSERT(pybind11::hasattr(column.attr("cat"), "codes"));
					bb_col_type = LogicalTypeId::STRING;
					bind_data.numpyCol_ = pybind11::array(column.attr("cat").attr("codes"));
					bind_data.mask_ = nullptr;
					bind_data.enumEntries_ = enum_entries;
					BB_ASSERT(pybind11::hasattr(bind_data.numpyCol_, "dtype"));
					bind_data.internalCategoricalType_ = string(pybind11::str(bind_data.numpyCol_.attr("dtype")));
				} else {
					bind_data.numpyCol_ = pybind11::array(column.attr("to_numpy")());
					bind_data.mask_ = nullptr;
					auto numpy_type = bind_data.numpyCol_.attr("dtype");
					// for category types (non-strings), we use the converted numpy type
					category_type = string(pybind11::str(numpy_type));
					convertPandasType(category_type, bb_col_type, bind_data.pandasType_);
				}
			} else {
				bind_data.numpyCol_ = pybind11::array(column.attr("to_numpy")());
				bind_data.mask_ = nullptr;
				convertPandasType(col_type, bb_col_type, bind_data.pandasType_);
			}
		}
		BB_ASSERT(pybind11::hasattr(bind_data.numpyCol_, "strides"));
		bind_data.numpyStride_ = bind_data.numpyCol_.attr("strides").attr("__getitem__")(0).cast<idx_t>();
		return_types.push_back(bb_col_type);
		out.push_back(std::move(bind_data));
	}
}
}
