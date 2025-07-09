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
#include <cstdint>

#include "Assert.h"
#include "Value.h"
#include "VectorDataMngr.h"
#include "bumblebee/common/TypeDefs.h"

namespace bumblebee{

enum class VectorType : uint8_t {
    FLAT_VECTOR,       // Flat vectors represent a standard uncompressed vector
    CONSTANT_VECTOR,   // Constant vector represents a single constant
    DICTIONARY_VECTOR, // Dictionary vector represents a selection vector on top of another vector
    SEQUENCE_VECTOR    // Sequence vector represents a sequence with a start point and end point
};

using vector_data_mngr_ptr_t = VectorDataMngr::vector_data_mngr_ptr_t;


// Lightweight struct to navigate vector data
struct VectorData {
	const SelectionVector *sel_;
	data_ptr_t data_;
	SelectionVector owned_sel_;
};

// Vector class for columnar execution
// Vector do not hold the data but is managed by the Vector Data Manager
class Vector {
	friend struct ConstantVector;
	friend struct FlatVector;
	friend struct DictionaryVector;
	friend struct StringVector;
	friend struct SequenceVector;

public:
	// Reference to the other vector
	explicit Vector(Vector &other);
	// Reference to the other vector using a selection vector
	explicit Vector(Vector &other, const SelectionVector &sel, idx_t count);
	// Reference to other vector starting from offset
	explicit Vector(Vector &other, idx_t offset);
	// Create a new constant vector with the value
	explicit Vector(const Value &value);
	// Create a new flat vector with 0
	explicit Vector(ConstantType type, idx_t capacity = STANDARD_VECTOR_SIZE);
	// Create a vector from a data
	Vector(ConstantType type, data_ptr_t dataptr);
	// Create a new vector
	Vector(ConstantType type, bool create_data, bool zero_data, idx_t capacity = STANDARD_VECTOR_SIZE);
	// Move a vector
	Vector(Vector &&other) noexcept;
	Vector(const Vector &) = delete;

	// Reference from another vector or value
	void reference(const Value &value);
	void reference(Vector &other);
	void reinterpret(Vector &other);
	void referenceAndSetType(Vector &other);

	// Slice from another vector starting from offset
	void slice(Vector &other, idx_t offset);
	// Slice from another vector following selection vector
	void slice(Vector &other, const SelectionVector &sel, idx_t count);
	void slice(const SelectionVector &sel, idx_t count);

	// Create empty vector
	void initialize(bool zero_data = false, idx_t capacity = STANDARD_VECTOR_SIZE);

	string toString(idx_t count) const;
	string toString() const;

	// Flatten the vector, removing any compression and turning it into a FLAT_VECTOR
	void normalify(idx_t count);
	void normalify(const SelectionVector &sel, idx_t count);
	// Obtains a selection vector and data pointer through which the data of this vector can be accessed
	void orrify(idx_t count, VectorData &data);

	void sequence(int64_t start, int64_t increment);

	// Verify functions enabled only with DEBUG compilation
	void verify(idx_t count);
	void verify(const SelectionVector &sel, idx_t count);
	void utfVerify(idx_t count);
	void utfVerify(const SelectionVector &sel, idx_t count);

	Value getValue(idx_t index) const;
	void setValue(idx_t index, const Value &val);

	void setAuxiliary(vector_data_mngr_ptr_t new_buffer);
	void resize(idx_t cur_size, idx_t new_size);

	inline VectorType getVectorType() const ;
	inline const ConstantType &getType() const ;
	inline data_ptr_t getData();
	vector_data_mngr_ptr_t getAuxiliary() ;
	vector_data_mngr_ptr_t getDataMngr();
	void setVectorType(VectorType vector_type);


protected:
    // Type of the vector
    VectorType vtype_{VectorType::FLAT_VECTOR};
    // Type of the data of the vector
    ConstantType ctype_;
    // Pointer of the data (does not own the data but is managed by the vector data mngr)
    data_ptr_t data_;
    // Vector Data Manager
    vector_data_mngr_ptr_t dataMngr_;
    // Aux data manager (used for string vectors where it contains the heap and for dictionary hold the real data)
    vector_data_mngr_ptr_t auxDataMngr_;
};


// The VectorChildDataMngr holds the real data of a dictionary vector
class VectorChildDataMngr : public VectorDataMngr {
public:
	VectorChildDataMngr(Vector vector) : VectorDataMngr(VectorDataMngrType::VECTOR_CHILD_BUFFER), data_(std::move(vector)) {
	}

public:
	Vector data_;
};
// ------------------  Struct Vector to handle the operations of Vector class --------------------

struct ConstantVector {
	static inline const_data_ptr_t getData(const Vector &vector) {
		BB_ASSERT(vector.getVectorType() == VectorType::CONSTANT_VECTOR ||
		         vector.getVectorType() == VectorType::FLAT_VECTOR);
		return vector.data_;
	}
	static inline data_ptr_t getData(Vector &vector) {
		BB_ASSERT(vector.getVectorType() == VectorType::CONSTANT_VECTOR ||
		         vector.getVectorType() == VectorType::FLAT_VECTOR);
		return vector.data_;
	}
	template <class T>
	static inline const T *getData(const Vector &vector) {
		return (const T *)getData(vector);
	}
	template <class T>
	static inline T *getData(Vector &vector) {
		return (T *)getData(vector);
	}

	static const SelectionVector *zeroSelectionVector(idx_t count, SelectionVector &owned_sel);
	//! Turns Vector into a constant vector by referencing a value within the source vector
	static void reference(Vector &vector, Vector &source, idx_t position, idx_t count);

	static const sel_t ZERO_VECTOR[STANDARD_VECTOR_SIZE];
	static const SelectionVector ZERO_SELECTION_VECTOR;
};

struct DictionaryVector {
	static inline const SelectionVector &selVector(const Vector &vector) {
		BB_ASSERT(vector.getVectorType() == VectorType::DICTIONARY_VECTOR);
		return ((const DictionaryDataMngr &)*vector.dataMngr_).getSelection();
	}
	static inline SelectionVector &selVector(Vector &vector) {
		BB_ASSERT(vector.getVectorType() == VectorType::DICTIONARY_VECTOR);
		return ((DictionaryDataMngr &)*vector.dataMngr_).getSelection();
	}
	static inline const Vector &child(const Vector &vector) {
		BB_ASSERT(vector.getVectorType() == VectorType::DICTIONARY_VECTOR);
		return ((const VectorChildDataMngr &)*vector.auxDataMngr_).data_;
	}
	static inline Vector &child(Vector &vector) {
		BB_ASSERT(vector.getVectorType() == VectorType::DICTIONARY_VECTOR);
		return ((VectorChildDataMngr &)*vector.auxDataMngr_).data_;
	}
};

struct FlatVector {
	static inline data_ptr_t getData(Vector &vector) {
		return ConstantVector::getData(vector);
	}
	template <class T>
	static inline const T *getData(const Vector &vector) {
		return ConstantVector::getData<T>(vector);
	}
	template <class T>
	static inline T *getData(Vector &vector) {
		return ConstantVector::getData<T>(vector);
	}
	static inline void setData(Vector &vector, data_ptr_t data) {
		BB_ASSERT(vector.getVectorType() == VectorType::FLAT_VECTOR);
		vector.data_ = data;
	}
	template <class T>
	static inline T getValue(Vector &vector, idx_t idx) {
		BB_ASSERT(vector.getVectorType() == VectorType::FLAT_VECTOR);
		return FlatVector::getData<T>(vector)[idx];
	}

	static const sel_t INCREMENTAL_VECTOR[STANDARD_VECTOR_SIZE];
	static const SelectionVector INCREMENTAL_SELECTION_VECTOR;
};


struct StringVector {
	// Add a string to the string heap of the vector (auxiliary data)
	static string_t addString(Vector &vector, const char *data, idx_t len);
	// Add a string to the string heap of the vector (auxiliary data)
	static string_t addString(Vector &vector, const char *data);
	// Add a string to the string heap of the vector (auxiliary data)
	static string_t addString(Vector &vector, string_t data);
	// Add a string to the string heap of the vector (auxiliary data)
	static string_t addString(Vector &vector, const string &data);
	// Allocates an empty string of the specified size, and returns a writable pointer that can be used to store the
	// result of an operation
	static string_t emptyString(Vector &vector, idx_t len);
	// Adds a reference to an unspecified vector buffer that stores strings of this vector
	static void addBuffer(Vector &vector, vector_data_mngr_ptr_t buffer);
	// Add a reference from this vector to the string heap of the provided vector
	static void addHeapReference(Vector &vector, Vector &other);
};

struct SequenceVector {
	static void getSequence(const Vector &vector, int64_t &start, int64_t &increment) {
		BB_ASSERT(vector.getVectorType() == VectorType::SEQUENCE_VECTOR);
		auto data = (int64_t *)vector.dataMngr_->getData();
		start = data[0];
		increment = data[1];
	}
};

}
