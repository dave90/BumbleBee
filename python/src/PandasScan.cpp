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
#include "include/PandasScan.hpp"

namespace bumblebee::python {

idx_t PandasScanData::getMaxThread() {

}

PandasScanDataChunk PandasScanData::getNextFileToRead() {

}


static function_data_ptr_t pandasScanBind(ClientContext &context,
                                       vector<Value> &inputs,
                                       vector<LogicalType> & inputTypes,
                                       std::unordered_map<string, Value> &parameters,
                                       std::unordered_map<string, idx_t> &bindVarName,
                                       vector<LogicalType> &returnTypes,vector<string> &names,
                                       TableFilterSet& filters) {
	auto result = std::make_unique<PandasScanData>();

	BB_ASSERT(inputs.size() == 1);
	BB_ASSERT(returnTypes.empty());
	BB_ASSERT(!names.empty());

	return std::move(result);
}

static function_op_data_ptr_t pandasScanInit(ClientContext &context, const FunctionData *bind_data_p) {

	auto &bind_data = (PandasScanData &)*bind_data_p;

	auto result = std::make_unique<PandasScanOperatorData>();

	return std::move(result);
}

static idx_t readPandasScanThread(ClientContext &context, const FunctionData *bind_data_p) {
	auto &bind_data = (PandasScanData &)*bind_data_p;

	return bind_data.getMaxThread();
}

static void pandasScanFunction(ClientContext &context, const FunctionData *bind_data_p,
									 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (PandasScanData &)*bind_data_p;
	auto &data = (PandasScanData &)*operator_state;

}


static void readCSVAddNamedParameters(PredFunction &table_function) {

}

string PandasScanFunc::getName() {
	return "&pandas_scan";
}


function_ptr_t PandasScanFunc::createFunction(const vector<LogicalType> &type) {
	string name = getName();
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {PhysicalType::STRING}, pandasScanFunction, pandasScanBind, pandasScanInit, readPandasScanThread, nullptr, nullptr));
	readCSVAddNamedParameters((PredFunction&)*fun);
	return fun;
}

void PandasScanFunc::registerFunction(FunctionRegister &funcRegister) {
	std::unique_ptr<FunctionGenerator> fg = std::make_unique<PandasScanFunc>();
	funcRegister.registerFunctionGen(fg);
}

}