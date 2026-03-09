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
#include "bumblebee/function/predicate/GenId.hpp"

#include "bumblebee/common/vector_operations/VectorOperations.hpp"


namespace bumblebee{

idx_t GenIdData::getNextId(idx_t step) {
    return counter_.fetch_add(step);
}


static function_data_ptr_t genIdBind(ClientContext &context,
                                       vector<Value> &inputs,
                                       vector<LogicalType> & inputTypes,
                                       std::unordered_map<string, Value> &parameters,
                                       std::unordered_map<string, idx_t> &bindVarName,
                                       vector<LogicalType> &returnTypes, vector<string> &names,
                                       TableFilterSet& filters) {
	auto result = std::make_unique<GenIdData>();

	returnTypes.clear();
	returnTypes.push_back(PhysicalType::UBIGINT);

	return std::move(result);
}

static function_op_data_ptr_t genIdInit(ClientContext &context, const FunctionData *bind_data_p) {
	function_op_data_ptr_t result = function_op_data_ptr_t(new FunctionOperatorData());

	return std::move(result);
}


static void genIdFunction(ClientContext &context, const FunctionData *bind_data_p,
									 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (GenIdData &)*bind_data_p;

	// generate the sequence in the first vector
	BB_ASSERT(output.columnCount() >= 1);
	BB_ASSERT(input);
	auto size = input->getSize();
	if (size == 0) {
		output.setCardinality(0);
		return;
	}
	BB_ASSERT(output.getCapacity() >= size);
	auto start = bind_data.getNextId(size);
	Vector v(PhysicalType::UBIGINT);
	VectorOperations::generateSequence(v, size, start, 1);
	output.data_[0].reference(v);
	output.setCardinality(size);
	output.setCapacity(size);
}

string GenIdFunc::getName() {
	return "&gen_id";
}


function_ptr_t GenIdFunc::createFunction(const vector<LogicalType> &type) {
	string name = getName();
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {}, genIdFunction, genIdBind, genIdInit, nullptr, nullptr, nullptr));
	return fun;
}

void GenIdFunc::registerFunction(FunctionRegister &funcRegister) {
	std::unique_ptr<FunctionGenerator> fg = std::make_unique<GenIdFunc>();
	funcRegister.registerFunctionGen(fg);
}
}
