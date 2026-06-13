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

#include "bumblebee/common/vector_operations/UnaryExecution.hpp"
#include <cstring>


namespace bumblebee{


// Find the first occurrence of `needle` within `haystack`, or -1 if absent.
// Delegates to memmem, whose Two-Way/SIMD implementation is faster than a
// per-position windowed scan for the long haystacks (URLs) typical of LIKE
// '%substr%' predicates, which dominate such queries.
int find(const unsigned char *haystack, idx_t haystack_size, const unsigned char *needle, idx_t needle_size) {
	BB_ASSERT(needle_size > 0);
	if (needle_size > haystack_size) {
		return -1;
	}
	auto location = memmem(haystack, haystack_size, needle, needle_size);
	if (location == nullptr) {
		return -1;
	}
	return (int)((const unsigned char *)location - haystack);
}



StringLikeData::StringLikeData(string &likePattern, char escape)
    :   escape_(escape) {
	idx_t last_non_pattern = 0;
	hasStartPercentage_ = false;
	hasEndPercentage_ = false;
	for (idx_t i = 0; i < likePattern.size(); i++) {
		auto ch = likePattern[i];
		if ( ch == '%' ) {
			// special character, push a constant pattern
			if (i > last_non_pattern) {
				segments_.emplace_back(likePattern.substr(last_non_pattern, i - last_non_pattern));
			}
			last_non_pattern = i + 1;
			// '%' wildcard
			if (i == 0) {
				hasStartPercentage_ = true;
			}
			if (i + 1 == likePattern.size()) {
				hasEndPercentage_ = true;
			}
		}
	}
	if (last_non_pattern < likePattern.size()) {
		segments_.emplace_back(likePattern.substr(last_non_pattern, likePattern.size() - last_non_pattern));
	}
}

bool StringLikeData::match(string_t &str) {
	// TODO handle the "_" special character

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

	auto likeString = inputs[0].toString();

	if (names.empty() || !bindVarName.count(names[0]))
		ErrorHandler::errorParsing("Error, like function requires the input variable to be bound in the context!");
	BB_ASSERT(!names.empty() && bindVarName.count(names[0]));

	auto result = std::make_unique<StringLikeData>(likeString);
	result->colIndex_ = bindVarName.at(names[0]);

	return result;
}

static function_op_data_ptr_t stringLikeInit(ClientContext &context, const FunctionData *bind_data_p) {
	auto result = std::make_unique<StringLikeOperatorData>();

	return result;
}



static void stringLikeFunction(ClientContext &context, const FunctionData *bind_data_p,
									 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (StringLikeData &)*bind_data_p;
	auto &data = (StringLikeOperatorData &)*operator_state;

	BB_ASSERT(input);
	BB_ASSERT(bind_data.colIndex_ < input->columnCount());

	auto& inputVector = input->data_[bind_data.colIndex_];
	BB_ASSERT(inputVector.getType() == PhysicalType::STRING);
	auto func = [&](string_t& inputString) {
		return bind_data.match(inputString);
	};
	SelectionVector trueSel(input->getSize());
	idx_t falseCount = 0;
	auto trueCount = UnaryExecution::select<string_t>(inputVector, nullptr, input->getSize(), &trueSel, nullptr, falseCount, func);

	output.reference(*input);
	output.slice(trueSel, trueCount);
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
