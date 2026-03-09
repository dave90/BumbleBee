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

#include "bumblebee/common/NumericUtils.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/output/OutputBuilder.hpp"

namespace bumblebee {
//
// template <class SIGNED,class UNSIGNED>
// string formatDecimal(SIGNED value, int scale) {
//     DecimalToString converter;
//     int len = converter.decimalLength<SIGNED, UNSIGNED>(value, scale);
//     char str[len];
//     converter.formatDecimal<SIGNED,UNSIGNED>(value, scale, str, len);
//     return str;
// }
//
// string formatDecimalValue(const Value& val, const LogicalType& type) {
//     DecimalToString converter;
//     int precision, scale;
//     type.getDecimalWidhtAndScale(precision, scale);
//     switch (type.getPhysicalType()) {
//         case PhysicalType::SMALLINT:
//             return formatDecimal<int16_t, uint16_t>(val.getNumericValue<int16_t>(), scale);
//         case PhysicalType::INTEGER:
//             return formatDecimal<int32_t, uint32_t>(val.getNumericValue<int16_t>(), scale);
//         case PhysicalType::BIGINT:
//             return formatDecimal<int64_t, uint64_t>(val.getNumericValue<int16_t>(), scale);
//         default:
//             ErrorHandler::errorNotImplemented("Error print DECIMAL type, precision not supported");
//     }
//     return "";
// }

void printVal(const Value& val, string& line, const LogicalType& type) {
    // if (type.type() == LogicalTypeId::DECIMAL) {
    //     // Convert to decimal
    //     line.append(formatDecimalValue(val, type));
    // }
    if (type.getPhysicalType() != PhysicalType::STRING)
        line.append(val.toString());
    else {
        // quote the string
        line.append("\"");
        line.append(val.toString());
        line.append("\"");
    }
}

void TextOutputBuilder::outputAtoms(DataChunk &chunk, Predicate *predicate) {
    auto arity = predicate->getArity();
    std::string line;
    line.reserve(OUTPUT_BUFFER_SIZE);

    DataChunk stringChunk;
    vector<LogicalType> types (chunk.columnCount(), LogicalTypeId::STRING);
    stringChunk.initializeEmpty(types);
    for (auto i=0;i<chunk.columnCount();++i) {
        if (chunk.data_[i].getType() == PhysicalType::STRING)
            stringChunk.data_[i].reference(chunk.data_[i]);
        else {
            // cast it
            stringChunk.data_[i].initialize(false, chunk.getSize());
            VectorOperations::cast( chunk.data_[i], stringChunk.data_[i], chunk.getSize());
        }
    }
    stringChunk.setCardinality(chunk.getSize());

    for (idx_t row = 0; row < stringChunk.getSize();++row) {
        line.clear();
        line.append(predicate->getName());
        line.push_back('(');
        auto firstVal = stringChunk.getValue(0, row);
        printVal(firstVal, line,  chunk.data_[0].getLogicalType()); // pass the original value

        for (idx_t col = 1; col < arity; ++col) {
            line.push_back(',');
            auto val = stringChunk.getValue(col, row);
            printVal(val, line, chunk.data_[col].getLogicalType()); // pass the original value
        }
        line.append(").\n");
        std::cout.write(line.data(), static_cast<std::streamsize>(line.size()));
    }
    std::cout.flush();

}

}