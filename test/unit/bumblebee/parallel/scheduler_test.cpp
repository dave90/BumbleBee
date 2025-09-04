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

#include <numeric>
#include <thread>

#include "bumblebee/catalog/PredicateTables.hpp"
#include <gtest/gtest.h>

#include "bumblebee/execution/Expression.hpp"
#include "bumblebee/execution/PhysicalRule.hpp"
#include "bumblebee/execution/atom/expression/PhysicalExpression.hpp"
#include "bumblebee/execution/atom/output/PhysicalChunkOutput.hpp"
#include "bumblebee/execution/atom/scan/PhysicalChunkScan.hpp"
#include "bumblebee/parallel/Scheduler.hpp"
#include "bumblebee/parallel/TaskExecutor.hpp"

using namespace bumblebee;

class SchedulerTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    std::shared_ptr<PredicateTables> sourcePtable;
    std::shared_ptr<PredicateTables> sinkPtable;
    ClientContext client_context;
    ThreadContext context{client_context};

    void SetUp() override{
        sourcePtable = std::make_shared<PredicateTables>("a",3);
        sinkPtable = std::make_shared<PredicateTables>("b",2);
    }



    vector<ConstantType> sourceTypes{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};
    vector<idx_t> cols_to_project = {0,2};
    ClientContext cc;

    DataChunk createChunkWithValue( idx_t count = 1, idx_t offset=0) {
        DataChunk chunk;
        chunk.initialize(sourceTypes);
        chunk.setCardinality(count);
        for (idx_t i = 0; i < count; ++i) {
            chunk.setValue(0, i, Value((int32_t) (i + offset) ));
            chunk.setValue(1, i, Value((uint32_t) ((i + offset) * 10) ));
            chunk.setValue(2, i, Value((int64_t) ((i + offset) * 100) ));
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void populatePTable(idx_t chunks = 10, idx_t elements=STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(elements, i*STANDARD_VECTOR_SIZE);
            sourcePtable->append(chunk);
        }
    }

    Expression generateExpression(Binop op, vector<idx_t> leftCols, vector<Operator> leftOps,vector<idx_t> rightCols, vector<Operator> rightOps) {
        return Expression(op, leftCols, leftOps, rightCols, rightOps);
    }

};


TEST_F(SchedulerTest, ScheduleSingleRuleTest) {
    int THREADS = 10;
    // populate with 100 chunks
    populatePTable(100);
    // create PRule
    vector<idx_t> cols = {0,1,2};
    auto source = patom_ptr_t(new PhysicalChunkScan(sourceTypes, cols,cols, sourcePtable.get() ));
    // we do not care of estimated cardinality of sink and other patom
    auto sink = patom_ptr_t(new PhysicalChunkOutput(sourceTypes,cols_to_project, sinkPtable.get()));
    patom_ptr_vector_t patoms;
    prule_ptr_t rule = prule_ptr_t(new PhysicalRule(source, sink, patoms, 0 ));

    Scheduler scheduler(cc);
    TaskExecutor executor(scheduler.queue_, THREADS);
    executor.startThreads();

    auto tasksGenerated = scheduler.scheduleRule(rule);
    EXPECT_EQ(tasksGenerated, sourcePtable->getCount() / MORSEL_SIZE +1);

    executor.stopThreadsAndJoin();
}


TEST_F(SchedulerTest, ScheduleSingleRulePartialChunkTest) {
    int THREADS = 10;
    // populate with 100 chunks
    populatePTable(1, 100);
    // create PRule
    vector<idx_t> cols = {0,1,2};
    auto source = patom_ptr_t(new PhysicalChunkScan(sourceTypes, cols,cols, sourcePtable.get() ));
    // we do not care of estimated cardinality of sink and other patom
    auto sink = patom_ptr_t(new PhysicalChunkOutput(sourceTypes, cols_to_project, sinkPtable.get()));
    patom_ptr_vector_t patoms;
    prule_ptr_t rule = prule_ptr_t(new PhysicalRule(source, sink, patoms, 0 ));

    Scheduler scheduler(cc);
    TaskExecutor executor(scheduler.queue_, THREADS);
    executor.startThreads();

    prule_ptr_vector_t rules = {rule};
    scheduler.schedulePriorityRules(rules);

    executor.stopThreadsAndJoin();
    EXPECT_EQ(sinkPtable->getCount(), sourcePtable->getCount());
}


TEST_F(SchedulerTest, ScheduleRuleSourceAndSinkTest) {
    int THREADS = 10;
    // populate with 100 chunks
    populatePTable(100);
    // create PRule
    vector<idx_t> cols = {0,1,2};
    auto source = patom_ptr_t(new PhysicalChunkScan(sourceTypes, cols, cols, sourcePtable.get() ));
    // we do not care of estimated cardinality of sink and other patom
    auto sink = patom_ptr_t(new PhysicalChunkOutput(sourceTypes,cols_to_project, sinkPtable.get()));
    patom_ptr_vector_t patoms;
    prule_ptr_t rule = prule_ptr_t(new PhysicalRule(source, sink, patoms, 0 ));

    Scheduler scheduler(cc);
    TaskExecutor executor(scheduler.queue_, THREADS);
    executor.startThreads();
    EXPECT_EQ(sinkPtable->getCount(), 0);

    prule_ptr_vector_t rules = {rule};
    scheduler.schedulePriorityRules(rules);

    executor.stopThreadsAndJoin();

    EXPECT_EQ(sinkPtable->getCount(), sourcePtable->getCount());

}


TEST_F(SchedulerTest, ScheduleRuleWithFilterTest) {
    int THREADS = 10;
    // populate with 100 chunks
    populatePTable(100);
    // create PRule
    vector<idx_t> cols = {0,1,2};
    auto source = patom_ptr_t(new PhysicalChunkScan(sourceTypes, cols,cols, sourcePtable.get() ));
    // we do not care of estimated cardinality of sink and other patom
    auto sink = patom_ptr_t(new PhysicalChunkOutput(sourceTypes,cols_to_project, sinkPtable.get()));
    patom_ptr_vector_t patoms;
    auto expr = generateExpression(EQUAL, {0}, {}, {2}, {});
    patoms.emplace_back(  new PhysicalExpression(expr, sourceTypes));

    prule_ptr_t rule = prule_ptr_t(new PhysicalRule(source, sink, patoms, 0 ));

    Scheduler scheduler(cc);
    TaskExecutor executor(scheduler.queue_, THREADS);
    executor.startThreads();
    EXPECT_EQ(sinkPtable->getCount(), 0);

    prule_ptr_vector_t rules = {rule};
    scheduler.schedulePriorityRules(rules);

    executor.stopThreadsAndJoin();

    EXPECT_EQ(sinkPtable->getCount(), 1); // only first row

}