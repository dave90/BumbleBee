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
#include "bumblebee/function/predicate/StringLike.hpp"


namespace bumblebee{


template <class UNSIGNED, int NEEDLE_SIZE>
static idx_t containsUnaligned(const unsigned char *haystack, idx_t haystack_size, const unsigned char *needle,
                               idx_t base_offset) {
	if (NEEDLE_SIZE > haystack_size) {
		// needle is bigger than haystack: haystack cannot contain needle
		return -1;
	}
	// contains for a small unaligned needle (3/5/6/7 bytes)
	// we perform unsigned integer comparisons to check for equality of the entire needle in a single comparison

	UNSIGNED needle_entry = 0;
	UNSIGNED haystack_entry = 0;
	const UNSIGNED start = (sizeof(UNSIGNED) * 8) - 8;
	const UNSIGNED shift = (sizeof(UNSIGNED) - NEEDLE_SIZE) * 8;
	for (int i = 0; i < NEEDLE_SIZE; i++) {
		needle_entry |= UNSIGNED(needle[i]) << UNSIGNED(start - i * 8);
		haystack_entry |= UNSIGNED(haystack[i]) << UNSIGNED(start - i * 8);
	}
	// now we perform the actual search
	for (idx_t offset = NEEDLE_SIZE; offset < haystack_size; offset++) {
		// for this position we first compare the haystack with the needle
		if (haystack_entry == needle_entry) {
			return base_offset + offset - NEEDLE_SIZE;
		}
		// now we adjust the haystack entry by
		// (1) removing the left-most character (shift by 8)
		// (2) adding the next character (bitwise or, with potential shift)
		// this shift is only necessary if the needle size is not aligned with the unsigned integer size
		// (e.g. needle size 3, unsigned integer size 4, we need to shift by 1)
		haystack_entry = (haystack_entry << 8) | ((UNSIGNED(haystack[offset])) << shift);
	}
	if (haystack_entry == needle_entry) {
		return base_offset + haystack_size - NEEDLE_SIZE;
	}
	return -1;
}

template <class UNSIGNED>
static idx_t containsAligned(const unsigned char *haystack, idx_t haystack_size, const unsigned char *needle,
                             idx_t base_offset) {
	if (sizeof(UNSIGNED) > haystack_size) {
		// needle is bigger than haystack: haystack cannot contain needle
		return -1;
	}
	// contains for a small needle aligned with unsigned integer (2/4/8)
	// similar to ContainsUnaligned, but simpler because we only need to do a reinterpret cast
	auto needle_entry = load<UNSIGNED>(needle);
	for (idx_t offset = 0; offset <= haystack_size - sizeof(UNSIGNED); offset++) {
		// for this position we first compare the haystack with the needle
		auto haystack_entry = load<UNSIGNED>(haystack + offset);
		if (needle_entry == haystack_entry) {
			return base_offset + offset;
		}
	}
	return -1;
}

idx_t containsGeneric(const unsigned char *haystack, idx_t haystack_size, const unsigned char *needle,
                      idx_t needle_size, idx_t base_offset) {
	if (needle_size > haystack_size) {
		// needle is bigger than haystack: haystack cannot contain needle
		return -1;
	}
	// we keep track of a shifting window sum of all characters with window size equal to needle_size
	// this shifting sum is used to avoid calling into memcmp;
	// we only need to call into memcmp when the window sum is equal to the needle sum
	// when that happens, the characters are potentially the same and we call into memcmp to check if they are
	uint32_t sums_diff = 0;
	for (idx_t i = 0; i < needle_size; i++) {
		sums_diff += haystack[i];
		sums_diff -= needle[i];
	}
	idx_t offset = 0;
	while (true) {
		if (sums_diff == 0 && haystack[offset] == needle[0]) {
			if (memcmp(haystack + offset, needle, needle_size) == 0) {
				return base_offset + offset;
			}
		}
		if (offset >= haystack_size - needle_size) {
			return -1;
		}
		sums_diff -= haystack[offset];
		sums_diff += haystack[offset + needle_size];
		offset++;
	}
}

int find(const unsigned char *haystack, idx_t haystack_size, const unsigned char *needle, idx_t needle_size) {
	BB_ASSERT(needle_size > 0);
	// start off by performing a memchr to find the first character
	auto location = memchr(haystack, needle[0], haystack_size);
	if (location == nullptr) {
		return -1;
	}
	idx_t base_offset = (const unsigned char *)location - haystack;
	haystack_size -= base_offset;
	haystack = (const unsigned char *)location;
	// switch algorithm depending on needle size
	switch (needle_size) {
		case 1:
			return base_offset;
		case 2:
			return containsAligned<uint16_t>(haystack, haystack_size, needle, base_offset);
		case 3:
			return containsUnaligned<uint32_t, 3>(haystack, haystack_size, needle, base_offset);
		case 4:
			return containsAligned<uint32_t>(haystack, haystack_size, needle, base_offset);
		case 5:
			return containsUnaligned<uint64_t, 5>(haystack, haystack_size, needle, base_offset);
		case 6:
			return containsUnaligned<uint64_t, 6>(haystack, haystack_size, needle, base_offset);
		case 7:
			return containsUnaligned<uint64_t, 7>(haystack, haystack_size, needle, base_offset);
		case 8:
			return containsAligned<uint64_t>(haystack, haystack_size, needle, base_offset);
		default:
			return containsGeneric(haystack, haystack_size, needle, needle_size, base_offset);
	}
}


StringLikeData::StringLikeData(idx_t colIdx, string &likePattern, char escape): colIdx_(colIdx) {
	idx_t last_non_pattern = 0;
	hasStartPercentage_ = false;
	hasEndPercentage_ = false;
	for (idx_t i = 0; i < likePattern.size(); i++) {
		auto ch = likePattern[i];
		if (ch == escape || ch == '%' || ch == '_') {
			// special character, push a constant pattern
			if (i > last_non_pattern) {
				segments_.emplace_back(likePattern.substr(last_non_pattern, i - last_non_pattern));
			}
			last_non_pattern = i + 1;
			if (ch == escape || ch == '_') {
				// escape or underscore: could not create efficient like matcher
				// FIXME: we could handle escaped percentages here
				return;
			} else {
				// sample_size
				if (i == 0) {
					hasStartPercentage_ = true;
				}
				if (i + 1 == likePattern.size()) {
					hasEndPercentage_ = true;
				}
			}
		}
	}
	if (last_non_pattern < likePattern.size()) {
		segments_.emplace_back(likePattern.substr(last_non_pattern, likePattern.size() - last_non_pattern));
	}
}

bool StringLikeData::match(string_t &str) {
	auto str_data = (const unsigned char *)str.getDataUnsafe();
		auto str_len = str.size();
		idx_t segment_idx = 0;
		idx_t end_idx = segments_.size() - 1;
		if (!hasStartPercentage_) {
			auto &segment = segments_[0];
			if (str_len < segment.size()) {
				return false;
			}
			if (memcmp(str_data, segment.c_str(), segment.size()) != 0) {
				return false;
			}
			str_data += segment.size();
			str_len -= segment.size();
			segment_idx++;
			if (segments_.size() == 1) {
				// only one segment, and it matches
				return hasEndPercentage_ || str_len == 0;
			}
		}
		// main match loop: for every segment in the middle, use contains to find the needle in the haystack
		for (; segment_idx < end_idx; segment_idx++) {
			auto &segment = segments_[segment_idx];
			// find the pattern of the current segment
			int next_offset = find(str_data, str_len, (const unsigned char *)segment.c_str(), segment.size());
			if (next_offset == -1) {
				// could not find this pattern in the string: no match
				return false;
			}
			idx_t offset = next_offset + segment.size();
			str_data += offset;
			str_len -= offset;
		}
		if (!hasEndPercentage_) {
			end_idx--;
			// no end sample_size: match the final segment now
			auto &segment = segments_.back();
			if (str_len < segment.size()) {
				return false;
			}
			if (memcmp(str_data + str_len - segment.size(), segment.c_str(), segment.size()) != 0) {
				return false;
			}
			return true;
		}
		auto &segment = segments_.back();
		// find the pattern of the current segment
		int next_offset = find(str_data, str_len, (const unsigned char *)segment.c_str(), segment.size());
		return next_offset != -1;
}


static function_data_ptr_t stringLikeBind(ClientContext &context,
                                          vector<Value> &inputs,
                                          vector<LogicalType> & inputTypes,
                                          std::unordered_map<string, Value> &parameters,
                                          std::unordered_map<string, idx_t> &bindVarName,
                                          vector<LogicalType> &returnTypes, vector<string> &names,
                                          TableFilterSet& filters) {

	if (inputs.size() != 1 || inputs[0].getPhysicalType() != PhysicalType::STRING)
		ErrorHandler::errorParsing("Error, like function expect only one string parameter!");

	if (inputTypes.size() != 1 || inputTypes[0].getPhysicalType() != PhysicalType::STRING)
		ErrorHandler::errorParsing("Error, like function expect only one input variable that point to string data!");

	BB_ASSERT(inputs.size() == 1);
	BB_ASSERT(inputs[0].getPhysicalType() == PhysicalType::STRING);
	BB_ASSERT(inputTypes.size() == 1);
	BB_ASSERT(inputTypes[0].getPhysicalType() == PhysicalType::STRING);
	BB_ASSERT(inputTypes.size() == names.size());

	auto varName = names[0];
	if (!bindVarName.contains(varName))
		ErrorHandler::errorParsing("Error, variable "+ varName+" is not binded!");
	BB_ASSERT(bindVarName.contains(varName));
	idx_t colIdx = bindVarName[varName];

	auto likeString = inputs[0].toString();


	auto result = std::make_unique<StringLikeData>(colIdx, likeString);

	return result;
}

static function_op_data_ptr_t stringLikeInit(ClientContext &context, const FunctionData *bind_data_p) {
	auto result = std::make_unique<StringLikeOperatorData>();

	return result;
}



static void stringLikeFunction(ClientContext &context, const FunctionData *bind_data_p,
									 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {


}


static void stringLikeAddNamedParameters(PredFunction &table_function) {

}

string StringLikeFunc::getName() {
	return "&like";
}


function_ptr_t StringLikeFunc::createFunction(const vector<LogicalType> &type) {
	string name = getName();
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {PhysicalType::STRING}, stringLikeFunction, stringLikeBind, stringLikeInit, nullptr, nullptr, nullptr));
	stringLikeAddNamedParameters((PredFunction&)*fun);
	return fun;
}

void StringLikeFunc::registerFunction(FunctionRegister &funcRegister) {
	std::unique_ptr<FunctionGenerator> fg = std::make_unique<StringLikeFunc>();
	funcRegister.registerFunctionGen(fg);
}


}
