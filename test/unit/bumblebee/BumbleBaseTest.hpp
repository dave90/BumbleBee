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

#include <gtest/gtest.h>
#include <algorithm>
#include <random>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/vector_operations/CreateSortKey.hpp"

using namespace bumblebee;

class BumbleBaseTest : public ::testing::Test {

protected:
    static const std::uint64_t SEED = 42;

    ClientContext clientContext;
    std::mt19937_64 rng;


protected:
    BumbleBaseTest():Test(), rng(SEED) {
    };

    struct RowComparator {
        const std::vector<OrderModifiers>& modifiers;
        const std::vector<std::vector<Value>>& data;

        bool operator()(std::size_t i, std::size_t j) const {
            const auto& a = data[i];
            const auto& b = data[j];

            idx_t col = 0;
            for (const auto& mod : modifiers) {

                if (a[col] == b[col]) {
                    col++;
                    continue;
                }

                switch (mod.order_type) {
                    case OrderType::ASCENDING:
                        return a[col] < b[col];
                    case OrderType::DESCENDING:
                        return a[col] > b[col];
                    default: {
                        col++;
                    }
                }
            }
            return false;
        }
    };

    template <class T>
    string formatDecimal(int scale, T values );

    template <class T>
    vector<string> formatDecimalVector(int scale, vector<T>& values );

    virtual Vector generateVector(LogicalType type, vector<Value>& values );

    template <class T>
    void setValuesVector(Vector &v, vector<T> values );

    template <class T>
    Vector generateVector(LogicalType type, vector<T> values );

    virtual Vector generateVector(idx_t size, LogicalType type);

    virtual DataChunk generateDataChunk(vector<LogicalType>& types, vector<vector<Value>>& data);

    template <class T>
    void addData(vector<vector<Value>>& table, vector<T> data);

    virtual vector<int> geenrateSequence(int start, int end, int step=1);

    vector<int> geenrateSequence(int start, int end,int elements,  int step);

    virtual void compareChunks(DataChunk &chunk1, DataChunk &chunk2, const std::vector<idx_t> &cols = {});

    template <class T>
    Value generateRandomNumericValue();

    Value randomValue(PhysicalType type, idx_t max_string_len = 128);

    DataChunk generateRandomDataChunk(vector<LogicalType> types);


};

template<class T>
void BumbleBaseTest::setValuesVector(Vector &v, vector<T> values) {
    for (idx_t i = 0; i < values.size(); i++) {
        Value val(values[i]);
        v.setValue(i, val.cast(v.getType()));
    }
}


template<class T>
string BumbleBaseTest::formatDecimal(int scale, T value ) {
    if (scale <= 0) {
        return std::to_string(value);
    }

    std::string s = std::to_string(value);

    // Handle negative numbers
    bool negative = (s[0] == '-');
    if (negative) {
        s.erase(s.begin());
    }

    // Ensure enough digits for the decimal point
    if (static_cast<int>(s.size()) <= scale) {
        s.insert(0, scale - s.size() + 1, '0');
    }

    // Insert decimal point
    s.insert(s.end() - scale, '.');

    if (negative) {
        s.insert(s.begin(), '-');
    }

    return s;
}

template<class T>
vector<string> BumbleBaseTest::formatDecimalVector(int scale, vector<T>& values ) {
    vector<string> result;
    for (auto& v:values)
        result.push_back(formatDecimal(scale,v));
    return result;
}


template<class T>
Vector BumbleBaseTest::generateVector(LogicalType type, vector<T> values) {
    Vector v1(type, values.size());
    setValuesVector<T>(v1,values);
    return v1;
}

template<class T>
void BumbleBaseTest::addData(vector<vector<Value>> &table, vector<T> data) {
    vector<Value> col_data;
    for (auto& d : data) {
        col_data.push_back(d);
    }
    table.push_back(std::move(col_data));
}

template<class T>
Value BumbleBaseTest::generateRandomNumericValue() {
    std::uniform_int_distribution<T> dist(
        std::numeric_limits<T>::min(),
        std::numeric_limits<T>::max()
    );
    return Value(dist(rng));
}

inline Vector BumbleBaseTest::generateVector(LogicalType type, vector<Value> &values) {
    Vector v1(type,values.size());
    for (idx_t i = 0; i < values.size(); i++) {
        v1.setValue(i, values[i].cast(v1.getType()));
    }
    return v1;
}

inline Vector BumbleBaseTest::generateVector(idx_t size, LogicalType type) {
    Vector result(type);
    for (idx_t i = 0; i < size; i++)
        result.setValue(i, Value((uint64_t)i).cast(type.getPhysicalType()) );

    return result;
}

inline DataChunk BumbleBaseTest::generateDataChunk(vector<LogicalType> &types, vector<vector<Value>> &data) {
    BB_ASSERT(types.size() == data.size());
    DataChunk chunk;
    chunk.initializeEmpty(types);
    idx_t idx = 0;
    for (auto& data_col : data) {
        Vector vec = generateVector(types[idx], data_col);
        chunk.data_[idx++].reference(vec);
    }
    chunk.setCapacity(data[0].size());
    chunk.setCardinality(data[0].size());
    return chunk;
}

inline vector<int> BumbleBaseTest::geenrateSequence(int start, int end, int step) {
    vector<int> result;
    for (int i = start; i <= end; i += step) {
        result.push_back(i);
    }
    return result;
}

inline vector<int> BumbleBaseTest::geenrateSequence(int start, int end, int elements, int step) {
    vector<int> result;
    auto t = start;
    for (int i = 0; i < elements; i++) {
        result.push_back(t);
        t += step;
        if (t > end) t = start;
    }
    return result;
}

inline void BumbleBaseTest::compareChunks(DataChunk &chunk1, DataChunk &chunk2, const std::vector<idx_t> &cols) {
    auto internalCols = cols;
    if (cols.empty()) {
        std::iota(internalCols.begin(), internalCols.end(), 0);
        std::unordered_set colsSet(internalCols.begin(), internalCols.end());
    }

    std::unordered_set colsSet(internalCols.begin(), internalCols.end());

    EXPECT_EQ(chunk1.columnCount(), chunk2.columnCount());
    EXPECT_EQ(chunk1.getSize(), chunk2.getSize());
    std::unordered_set<string> chunk1Str, chunk2Str;
    // compare the chunks
    for (idx_t i = 0; i < chunk1.getSize(); i++) {
        string row1, row2;
        for (idx_t j = 0; j < chunk1.columnCount(); j++) {
            if (!colsSet.contains(j))continue;
            row1 += chunk1.getValue(j, i).toString() + " ; ";
            row2 += chunk2.getValue(j, i).toString()+ " ; ";;
        }
        chunk1Str.insert(row1);
        chunk2Str.insert(row2);
    }
    EXPECT_EQ(chunk1Str.size(), chunk2Str.size());
    EXPECT_EQ(chunk1Str, chunk2Str);
}

inline Value BumbleBaseTest::randomValue(PhysicalType type, idx_t max_string_len) {

    switch (type) {
        case PhysicalType::TINYINT:
            return generateRandomNumericValue<int8_t>();

        case PhysicalType::SMALLINT:
            return generateRandomNumericValue<int16_t>();

        case PhysicalType::INTEGER:
            return generateRandomNumericValue<int32_t>();

        case PhysicalType::BIGINT:
            return generateRandomNumericValue<int64_t>();

        case PhysicalType::UTINYINT:
            return generateRandomNumericValue<uint8_t>();

        case PhysicalType::USMALLINT:
            return generateRandomNumericValue<uint16_t>();

        case PhysicalType::UINTEGER:
            return generateRandomNumericValue<uint32_t>();

        case PhysicalType::UBIGINT:
            return generateRandomNumericValue<uint64_t>();

        case PhysicalType::FLOAT: {
            // Keep float range reasonable to avoid infinities
            std::uniform_real_distribution<float> dist(-1.0e6f, 1.0e6f);
            return Value(dist(rng));
        }
        case PhysicalType::DOUBLE: {
            std::uniform_real_distribution<double> dist(-1.0e12, 1.0e12);
            return Value(dist(rng));
        }
        case PhysicalType::STRING: {
            // Random length in [0, max_string_len]
            if (max_string_len == 0) {
                // Empty string is valid
                return Value(string{});
            }

            std::uniform_int_distribution<idx_t> len_dist(0, max_string_len);
            idx_t len = len_dist(rng);

            static const char charset[] =
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
            static constexpr std::size_t charset_size = sizeof(charset) - 1; // exclude '\0'

            std::uniform_int_distribution<std::size_t> char_dist(0, charset_size - 1);

            string s;
            s.reserve(len);
            for (idx_t i = 0; i < len; ++i) {
                s.push_back(charset[char_dist(rng)]);
            }

            return Value(std::move(s));
        }

        default:
            ErrorHandler::errorNotImplemented("randomValue: unsupported ConstantType");
            // Return something to keep compiler happy; won't be reached if errorNotImplemented aborts.
            return Value(int32_t{0});
    }
}

inline DataChunk BumbleBaseTest::generateRandomDataChunk(vector<LogicalType> types) {
    DataChunk chunk;
    chunk.initialize(types);

    for (idx_t i = 0; i < STANDARD_VECTOR_SIZE; i++) {
        for (auto j = 0; j < types.size(); j++) {
            Value v = randomValue(types[j].getPhysicalType());
            chunk.setValue(j, i, v);
        }
    }
    chunk.setCardinality(STANDARD_VECTOR_SIZE);
    return chunk;
}
