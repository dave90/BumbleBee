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
	return rowCount_ / MORSEL_SIZE +1 ;
}

PandasScanDataChunk PandasScanData::getNextFileToRead() {
	lock_guard lock(mutex_);
	if (linesCount_ >= rowCount_)
		return {.start_ = 0, .end_ = 0};
	auto start = linesCount_;
	auto end = minValue<idx_t>(linesCount_ + MORSEL_SIZE - 1, rowCount_ - 1);
	linesCount_ += MORSEL_SIZE;
	return {.start_ = start, .end_ = end};
}


static function_data_ptr_t pandasScanBind(ClientContext &context,
                                       vector<Value> &inputs,
                                       vector<LogicalType> & inputTypes,
                                       std::unordered_map<string, Value> &parameters,
                                       std::unordered_map<string, idx_t> &bindVarName,
                                       vector<LogicalType> &returnTypes,vector<string> &names,
                                       TableFilterSet& filters) {

	BB_ASSERT(inputs.size() == 1);
	BB_ASSERT(inputs[0].getPhysicalType() == PhysicalType::UBIGINT);
	BB_ASSERT(returnTypes.empty());
	BB_ASSERT(!names.empty());

	// input is the address of the python obj
	pybind11::gil_scoped_acquire acquire;
	auto address = inputs[0].getNumericValue<uint64_t>();
	pybind11::handle df((PyObject *)(address));

	vector<PandasColumnBindData> bind_data;
	vector<string> df_names;
	VectorConversion::bindPandas(df, bind_data, returnTypes, df_names);

	auto df_columns = pybind11::list(df.attr("columns"));
	auto get_fun = df.attr("__getitem__");

	idx_t row_count = pybind11::len(get_fun(df_columns[0]));
	auto result = std::make_unique<PandasScanData>(df, row_count, bind_data, returnTypes);

	return std::move(result);
}

static function_op_data_ptr_t pandasScanInit(ClientContext &context, const FunctionData *bind_data_p) {
	auto &bind_data = (PandasScanData &)*bind_data_p;

	auto result = std::make_unique<PandasScanOperatorData>();
	result->chunkToRead_ = bind_data.getNextFileToRead();
	result->chunk_.initialize(bind_data.types_);

	return std::move(result);
}

static idx_t pandasScanThread(ClientContext &context, const FunctionData *bind_data_p) {
	auto &bind_data = (PandasScanData &)*bind_data_p;

	return bind_data.getMaxThread();
}

static void pandasScanFunction(ClientContext &context, const FunctionData *bind_data_p,
									 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output) {
	auto &bind_data = (PandasScanData &)*bind_data_p;
	auto &data = (PandasScanOperatorData &)*operator_state;

	BB_ASSERT(output.columnCount() == data.chunk_.columnCount());
	BB_ASSERT(output.getTypes() == data.chunk_.getTypes());

	if (data.chunkToRead_.end_ == 0 || data.chunkToRead_.start_ > data.chunkToRead_.end_) {
		output.setCardinality(0);
		return;
	}

	auto count = minValue<idx_t>(data.chunkToRead_.end_ - data.chunkToRead_.start_ + 1, STANDARD_VECTOR_SIZE);
	data.chunk_.setCardinality(0);

	for (idx_t i = 0; i < data.chunk_.columnCount(); i++) {
		VectorConversion::numpyToBumbleBee(bind_data.bindData_[i], bind_data.bindData_[i].numpyCol_, count, data.chunkToRead_.start_, data.chunk_.data_[i]);
	}

	data.chunk_.setCardinality(count);
	data.chunkToRead_.start_ += count;

	output.reference(data.chunk_);
	BB_ASSERT(output.getSize() > 0);

	data.chunk_.reset();
}


static void pandasScanAddNamedParameters(PredFunction &table_function) {

}

string PandasScanFunc::getName() {
	return "&pandas_scan";
}


function_ptr_t PandasScanFunc::createFunction(const vector<LogicalType> &type) {
	string name = getName();
	function_ptr_t fun = function_ptr_t(new PredFunction( name, {LogicalTypeId::ADDRESS}, pandasScanFunction, pandasScanBind, pandasScanInit, pandasScanThread, nullptr, nullptr));
	pandasScanAddNamedParameters((PredFunction&)*fun);
	return fun;
}

void PandasScanFunc::registerFunction(FunctionRegister &funcRegister) {
	std::unique_ptr<FunctionGenerator> fg = std::make_unique<PandasScanFunc>();
	funcRegister.registerFunctionGen(fg);
}

}
