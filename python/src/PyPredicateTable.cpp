#include "include/PyPredicateTable.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/common/types/Value.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/execution/PartitionedPRLHashTable.hpp"

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

static py::list collectRows(PredicateTables& pt) {
    py::list rows;
    idx_t arity = pt.columnCount();
    pt.initializeChunks();

    if (!pt.isDistinct() || !pt.existPartitionedPRLHashTable()) {
        for (idx_t i = 0; i < pt.chunkCount(); ++i) {
            DataChunk& chunk = pt.getChunk(i);
            auto types = chunk.getTypes();
            for (idx_t row = 0; row < chunk.getSize(); ++row) {
                py::tuple t(arity);
                for (idx_t col = 0; col < arity; ++col) {
                    t[col] = valueToPython(chunk.getValue(col, row), types[col]);
                }
                rows.append(t);
            }
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
            for (idx_t row = 0; row < result.getSize(); ++row) {
                py::tuple t(arity);
                for (idx_t col = 0; col < arity; ++col) {
                    t[col] = valueToPython(result.getValue(col, row), types[col]);
                }
                rows.append(t);
            }
            offset += STANDARD_VECTOR_SIZE;
        }
    }
    return rows;
}

// ---------------------------------------------------------------------------
// PyPredicateTable
// ---------------------------------------------------------------------------

PyPredicateTable::PyPredicateTable(std::string name, int arity,
                                   bumblebee::PredicateTables* pt)
    : name_(std::move(name)), arity_(arity), pt_(pt) {}

pybind11::list PyPredicateTable::tuples() const {
    if (!cached_) {
        cached_ = collectRows(*pt_);
    }
    return *cached_;
}

const std::string& PyPredicateTable::getName() const {
    return name_;
}

int PyPredicateTable::getArity() const {
    return arity_;
}

} // namespace bumblebee::python