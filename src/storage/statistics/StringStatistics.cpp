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
#include "bumblebee/storage/statistics/StringStatistics.hpp"
#include "bumblebee/storage/statistics/ValidityStatistics.hpp"
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "utf8proc/utf8proc_wrapper.hpp"

namespace bumblebee{

StringStatistics::StringStatistics(PhysicalType type_p) : BaseStatistics(type_p) {
	for (idx_t i = 0; i < MAX_STRING_MINMAX_SIZE; i++) {
		min_[i] = 0xFF;
		max_[i] = 0;
	}
	maxStringLength_ = 0;
	hasUnicode_ = false;
	hasOverflowStrings_ = false;
	validityStats_ = std::make_unique<ValidityStatistics>(false);
}

std::unique_ptr<BaseStatistics> StringStatistics::copy() {
	auto stats = std::make_unique<StringStatistics>(type_);
	memcpy(stats->min_, min_, MAX_STRING_MINMAX_SIZE);
	memcpy(stats->max_, max_, MAX_STRING_MINMAX_SIZE);
	stats->hasUnicode_ = hasUnicode_;
	stats->maxStringLength_ = maxStringLength_;
	if (validityStats_) {
		stats->validityStats_ = validityStats_->copy();
	}
	return std::move(stats);
}


static int stringValueComparison(const_data_ptr_t data, idx_t len, const_data_ptr_t comparison) {
	BB_ASSERT(len <= StringStatistics::MAX_STRING_MINMAX_SIZE);
	for (idx_t i = 0; i < len; i++) {
		if (data[i] < comparison[i]) {
			return -1;
		} else if (data[i] > comparison[i]) {
			return 1;
		}
	}
	return 0;
}

static void constructValue(const_data_ptr_t data, idx_t size, data_t target[]) {
	idx_t value_size =
	    size > StringStatistics::MAX_STRING_MINMAX_SIZE ? StringStatistics::MAX_STRING_MINMAX_SIZE : size;
	memcpy(target, data, value_size);
	for (idx_t i = value_size; i < StringStatistics::MAX_STRING_MINMAX_SIZE; i++) {
		target[i] = '\0';
	}
}

void StringStatistics::update(const string_t &value) {
	auto data = (const_data_ptr_t)value.getDataUnsafe();
	auto size = value.size();

	// we can only fit 8 bytes, so we might need to trim our string
	// construct the value
	data_t target[MAX_STRING_MINMAX_SIZE];
	constructValue(data, size, target);

	// update the min and max
	if (stringValueComparison(target, MAX_STRING_MINMAX_SIZE, min_) < 0) {
		memcpy(min_, target, MAX_STRING_MINMAX_SIZE);
	}
	if (stringValueComparison(target, MAX_STRING_MINMAX_SIZE, max_) > 0) {
		memcpy(max_, target, MAX_STRING_MINMAX_SIZE);
	}
	if (size > maxStringLength_) {
		maxStringLength_ = size;
	}
	if (!hasUnicode_) {
		auto unicode = Utf8Proc::Analyze((const char *)data, size);
		if (unicode == UnicodeType::UNICODE) {
			hasUnicode_ = true;
		} else if (unicode == UnicodeType::INVALID) {
			ErrorHandler::errorGeneric("Invalid unicode detected in segment statistics update!");
		}
	}
}

void StringStatistics::merge(const BaseStatistics &other_p) {
	BaseStatistics::merge(other_p);
	auto &other = (const StringStatistics &)other_p;
	if (stringValueComparison(other.min_, MAX_STRING_MINMAX_SIZE, min_) < 0) {
		memcpy(min_, other.min_, MAX_STRING_MINMAX_SIZE);
	}
	if (stringValueComparison(other.max_, MAX_STRING_MINMAX_SIZE, max_) > 0) {
		memcpy(max_, other.max_, MAX_STRING_MINMAX_SIZE);
	}
	hasUnicode_ = hasUnicode_ || other.hasUnicode_;
	maxStringLength_ = maxValue<uint32_t>(maxStringLength_, other.maxStringLength_);
	hasOverflowStrings_ = hasOverflowStrings_ || other.hasOverflowStrings_;
}

FilterPropagateResult StringStatistics::checkZonemap(Binop comparison_type, const string &constant) {
	auto data = (const_data_ptr_t)constant.c_str();
	auto size = constant.size();

	idx_t value_size = size > MAX_STRING_MINMAX_SIZE ? MAX_STRING_MINMAX_SIZE : size;
	int min_comp = stringValueComparison(data, value_size, min_);
	int max_comp = stringValueComparison(data, value_size, max_);
	switch (comparison_type) {
	case EQUAL:
		if (min_comp >= 0 && max_comp <= 0) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
	case GREATER:
	case GREATER_OR_EQ:
		if (max_comp <= 0) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
	case LESS:
	case LESS_OR_EQ:
		if (min_comp >= 0) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
		default:
			ErrorHandler::errorNotImplemented("Expression type not implemented for string statistics zone map");
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
}

static idx_t getValidMinMaxSubstring(data_ptr_t data) {
	for (idx_t i = 0; i < StringStatistics::MAX_STRING_MINMAX_SIZE; i++) {
		if (data[i] == '\0') {
			return i;
		}
		if ((data[i] & 0x80) != 0) {
			return i;
		}
	}
	return StringStatistics::MAX_STRING_MINMAX_SIZE;
}

string StringStatistics::toString() {
	idx_t min_len = getValidMinMaxSubstring(min_);
	idx_t max_len = getValidMinMaxSubstring(max_);
	return StringUtils::format("[Min: %s, Max: %s, Has Unicode: %s, Max String Length: %lld]%s",
	                          string((const char *)min_, min_len), string((const char *)max_, max_len),
	                          hasUnicode_ ? "true" : "false", maxStringLength_,
	                          validityStats_ ? validityStats_->toString() : "");
}


}
