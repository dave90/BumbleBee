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

#include "include/PyPredicateTable.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/common/types/Value.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/execution/PartitionedPRLHashTable.hpp"
#include "include/VectorWrapper.hpp"

namespace py = pybind11;

namespace bumblebee::python {

static py::object valueToPython(const Value& v, const LogicalType& lt) {
    switch (lt.type()) {
        case LogicalTypeId::TINYINT:
        case LogicalTypeId::SMALLINT:
        case LogicalTypeId::INTEGER:
            return py::int_(v.getNumericValue<int32_t>());
        case LogicalTypeId::BIGINT:
            return py::int_(v.getNumericValue<int64_t>());
        case LogicalTypeId::UTINYINT:
        case LogicalTypeId::USMALLINT:
        case LogicalTypeId::UINTEGER:
            return py::int_(v.getNumericValue<uint32_t>());
        case LogicalTypeId::UBIGINT:
            return py::int_(v.getNumericValue<uint64_t>());
        case LogicalTypeId::FLOAT:
            return py::float_(v.getNumericValue<float>());
        case LogicalTypeId::DOUBLE:
            return py::float_(v.getNumericValue<double>());
        case LogicalTypeId::DECIMAL: {
            int64_t raw = v.getNumericValue<int64_t>();
            int scale = lt.getDecimalData().scale_;
            double factor = 1.0;
            for (int i = 0; i < scale; ++i) factor *= 10.0;
            return py::float_(static_cast<double>(raw) / factor);
        }
        default:
            return py::str(v.toString());
    }
}

template <typename F>
static void collectRows(PredicateTables& pt, F&& callback) {
    pt.initializeChunks();

    if (!pt.isDistinct() || !pt.existPartitionedPRLHashTable()) {
        for (idx_t i = 0; i < pt.chunkCount(); ++i) {
            DataChunk& chunk = pt.getChunk(i);
            callback(chunk, chunk.getTypes());
        }
    } else {
        auto& ht = pt.getPartitionedPRLHashTable();
        auto types = pt.getTypes();
        idx_t offset = 0;
        DataChunk result;
        result.initialize(types);
        while (offset < pt.getCount()) {
            result.setCardinality(0);
            ht->scan(offset, result);
            callback(result, types);
            offset += STANDARD_VECTOR_SIZE;
        }
    }
}


// ---------------------------------------------------------------------------
// PyPredicateTable
// ---------------------------------------------------------------------------

PyPredicateTable::PyPredicateTable(std::string name, int arity,
                                   bumblebee::PredicateTables* pt)
    : name_(std::move(name)), arity_(arity), pt_(pt) {}

pybind11::list PyPredicateTable::tuples() const {
    py::list rows;
    idx_t arity = pt_->columnCount();
    collectRows(*pt_, [&](DataChunk& chunk, const std::vector<LogicalType>& types) {
        for (idx_t row = 0; row < chunk.getSize(); ++row) {
            py::tuple t(arity);
            for (idx_t col = 0; col < arity; ++col) {
                t[col] = valueToPython(chunk.getValue(col, row), types[col]);
            }
            rows.append(t);
        }
    });
    return rows;
}

pybind11::dict PyPredicateTable::fetchNumpyInternal(const vector<string>& names) const{
    if (names.size() != pt_->getTypes().size()) {
        throw std::invalid_argument("col_names size (" + std::to_string(names.size()) +
            ") does not match column count (" + std::to_string(pt_->getTypes().size()) + ")");
    }

    std::unique_ptr<NumpyResultConversion> conversion;
    collectRows(*pt_, [&](DataChunk& chunk, const std::vector<LogicalType>& types) {
        if (!conversion) {
            conversion = std::make_unique<NumpyResultConversion>(types, std::max(pt_->getCount(), (idx_t)STANDARD_VECTOR_SIZE));
        }
        conversion->append(chunk);
    });
    py::dict res;
    if (!conversion) {
        conversion = std::make_unique<NumpyResultConversion>(pt_->getTypes(), 0);
    }
    for (idx_t col_idx = 0; col_idx < names.size(); col_idx++) {
        res[names[col_idx].c_str()] = conversion->toArray(col_idx);
    }
    return res;
}


pybind11::object PyPredicateTable::toDf(const vector<string>& names) const {
    auto finalNames = names;
    if (finalNames.empty()) {
        for (idx_t i=0;i<pt_->getTypes().size();++i)
            finalNames.emplace_back("COL_"+std::to_string(i));
    }
    return py::module::import("pandas").attr("DataFrame").attr("from_dict")(fetchNumpyInternal(finalNames));
}

const std::string& PyPredicateTable::getName() const {
    return name_;
}

int PyPredicateTable::getArity() const {
    return arity_;
}

} // namespace bumblebee::python