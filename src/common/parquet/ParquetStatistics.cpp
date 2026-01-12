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

#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/parquet/ColumnReader.hpp"
#include "bumblebee/storage/statistics/BaseStatistics.hpp"
#include "bumblebee/storage/statistics/NumericStatistics.hpp"
#include "bumblebee/storage/statistics/StringStatistics.hpp"
#include "parquet/parquet_types.h"

namespace bumblebee {


template <Value (*FUNC)(const_data_ptr_t input)>
static std::unique_ptr<BaseStatistics> templatedGetNumericStats(const PhysicalType &type,
                                                           const format::Statistics &parquet_stats) {
	auto stats = std::make_unique<NumericStatistics>(type);

	// for reasons unknown to science, Parquet defines *both* `min` and `min_value` as well as `max` and
	// `max_value`. All are optional. such elegance.
	if (parquet_stats.__isset.min) {
		stats->min_ = FUNC((const_data_ptr_t)parquet_stats.min.data());
	} else if (parquet_stats.__isset.min_value) {
		stats->min_ = FUNC((const_data_ptr_t)parquet_stats.min_value.data());
	} else {
		stats->min_ = Value::minimumValue(stats->type_);
	}
	if (parquet_stats.__isset.max) {
		stats->max_ = FUNC((const_data_ptr_t)parquet_stats.max.data());
	} else if (parquet_stats.__isset.max_value) {
		stats->max_ = FUNC((const_data_ptr_t)parquet_stats.max_value.data());
	} else {
		stats->max_ = Value::maximumValue(stats->type_);
	}
	// GCC 4.x insists on a move() here
	return std::move(stats);
}

template <class T>
static Value transformStatisticsPlain(const_data_ptr_t input) {
	return Value(load<T>(input));
}

static Value transformStatisticsFloat(const_data_ptr_t input) {
	auto val = load<float>(input);
	return Value(val);
}

static Value transformStatisticsDouble(const_data_ptr_t input) {
	auto val = load<double>(input);
	return Value(val);
}

std::unique_ptr<BaseStatistics> parquetTransformColumnStatistics(const bumblebee::SchemaElement &s_ele, const PhysicalType &type,
                                                            const ColumnChunk &column_chunk) {
	if (!column_chunk.__isset.meta_data || !column_chunk.meta_data.__isset.statistics) {
		// no stats present for row group
		return nullptr;
	}
	auto &parquet_stats = column_chunk.meta_data.statistics;
	std::unique_ptr<BaseStatistics> row_group_stats;

	switch (type) {

	case PhysicalType::UTINYINT:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<uint8_t>>(type, parquet_stats);
		break;

	case PhysicalType::USMALLINT:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<uint16_t>>(type, parquet_stats);
		break;

	case PhysicalType::UINTEGER:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<uint32_t>>(type, parquet_stats);
		break;

	case PhysicalType::UBIGINT:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<uint64_t>>(type, parquet_stats);
		break;
	case PhysicalType::TINYINT:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<int8_t>>(type, parquet_stats);
		break;
	case PhysicalType::SMALLINT:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<int16_t>>(type, parquet_stats);
		break;
	case PhysicalType::INTEGER:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<int32_t>>(type, parquet_stats);
		break;

	case PhysicalType::BIGINT:
		row_group_stats = templatedGetNumericStats<transformStatisticsPlain<int64_t>>(type, parquet_stats);
		break;

	case PhysicalType::FLOAT:
		row_group_stats = templatedGetNumericStats<transformStatisticsFloat>(type, parquet_stats);
		break;

	case PhysicalType::DOUBLE:
		row_group_stats = templatedGetNumericStats<transformStatisticsDouble>(type, parquet_stats);
		break;

	case PhysicalType::STRING: {
		auto string_stats = std::make_unique<StringStatistics>(type);
		if (parquet_stats.__isset.min) {
			string_stats->update(parquet_stats.min.c_str());
		} else if (parquet_stats.__isset.min_value) {
			string_stats->update(parquet_stats.min_value.c_str());
		} else {
			return nullptr;
		}
		if (parquet_stats.__isset.max) {
			string_stats->update(parquet_stats.max.c_str());
		} else if (parquet_stats.__isset.max_value) {
			string_stats->update(parquet_stats.max_value.c_str());
		} else {
			return nullptr;
		}

		string_stats->hasUnicode_ = true; // we dont know better
		row_group_stats = std::move(string_stats);
		break;
	}
	default:
		// no stats for you
		break;
	} // end of type switch

	// null count is generic
	if (row_group_stats) {
		if (column_chunk.meta_data.type == format::Type::FLOAT ||
		    column_chunk.meta_data.type == format::Type::DOUBLE) {
			// floats/doubles can have infinity, which becomes NULL
			row_group_stats->validityStats_ = std::make_unique<ValidityStatistics>(true);
		} else if (parquet_stats.__isset.null_count) {
			row_group_stats->validityStats_ = std::make_unique<ValidityStatistics>(parquet_stats.null_count != 0);
		} else {
			row_group_stats->validityStats_ = std::make_unique<ValidityStatistics>(true);
		}
	} else {
		return nullptr;
	}

	return row_group_stats;
}

}
