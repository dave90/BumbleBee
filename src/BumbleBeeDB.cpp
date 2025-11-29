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
#include "include/bumblebee/BumbleBeeDB.hpp"

#include <CLI11.hpp>
#include <thread>

#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/parallel/TaskExecutor.hpp"
#include "bumblebee/parser/ParserInputDirector.hpp"
#include "bumblebee/parser/statement/sql/SqlToDatalog.hpp"
#include "bumblebee/planner/Planner.hpp"
#include "bumblebee/planner/StatementDependency.hpp"
#include "bumblebee/planner/rewriter/AggregatesRewriter.hpp"

namespace bumblebee {
int BumbleBeeDB::parseArgs(int argc, char **argv) {
    CLI::App app{NAME};

    const unsigned int threads = std::thread::hardware_concurrency();

    app.add_option("-l,--log-file", context_.logFilename_, "Log file")->default_val(DEFAULT_LOG_FILE);
    app.add_flag("-p,--print-log", context_.printLog_, "Print log")->default_val(0);
    app.add_flag("--print-program", context_.printProgram_, "Print only the datalog program")->default_val(0);
    // app.add_flag("-s,--single-shot", context_.singleShot_, "Single shot run")->default_val(1);
    app.add_option("-i,--input-files", context_.inputFiles_, "Single shot run")->expected(1, -1);
    app.add_option("-t,--threads", context_.threads_, "Numbers of threads")->expected(1, INT_MAX)->default_val(threads);
    app.add_flag("-a,--print-all", context_.printAll_, "Print all predicates")->default_val(0);
    app.add_flag("-r,--print-profiling", context_.printProfiling_, "Print profilings")->default_val(0);
    app.add_flag("-d,--distinct", context_.distinct_, "Set distinct all the predicates")->default_val(0);

    CLI11_PARSE(app, argc, argv);
    init_logger(context_.logFilename_.c_str()  , context_.printLog_);
    printArgs();
    return 0;
}

void BumbleBeeDB::printArgs() {
    LOG_INFO("Arguments:\n\tLog Filename: %s\n\tPrint Log: %d\n\tSingle shot: %d \n\tThreads:%d \n\tPrint all predicates:%d \n\tDistinct:%d",
        context_.logFilename_.c_str(),
        context_.printLog_,
        context_.singleShot_,
        context_.threads_,
        context_.printAll_,
        context_.distinct_);
}

void BumbleBeeDB::parseProgram(rules_vector_t &program) {
    // Parse the program
    ParserInputDirector inputDirector(TEXT, context_); // DEFAULT TEXT output
    auto res = inputDirector.parse(context_.inputFiles_);
    // Check errors during parsing
    if (inputDirector.getBuilder()->isFoundASafetyError() || res != 0) {
        if (inputDirector.getBuilder()->isFoundASafetyError()) {
            // found a rule unsafe
            std::string error = inputDirector.getBuilder()->getSafetyErrorMessage();
            LOG_ERROR("Error: %s ",error.c_str());
            ErrorHandler::errorParsing("aborting due to unsafe rule.");
        }else {
            ErrorHandler::errorParsing("aborting due to parser errors.");
        }
    }
    if (inputDirector.getBuilder()->isSQL()) {
        // we need to rewrite sql into datalog
        sql::SQLStatement sqlStatement = std::move(inputDirector.getBuilder()->getSqlStatement());
        LOG_INFO("SQL: %s", sqlStatement.toString().c_str());
        SqlToDatalog rewriter(context_);
        bool foundError=false;
        string error;
        program = rewriter.sqlToDatalog(sqlStatement, foundError, error);
        if (foundError) {
            LOG_ERROR("Error: %s ",error.c_str());
            ErrorHandler::errorParsing("aborting due to incorrect sql.");
        }
    }else
        program = std::move(inputDirector.getBuilder()->getProgram());
    LOG_INFO("Program size: %u", program.size());
    for (auto& rule : program) {
        LOG_DEBUG("Rule: %s", rule.toString().c_str());
    }
}



void BumbleBeeDB::run() {
    if (!context_.singleShot_) {
        LOG_ERROR("Error, only single shot mode is avaliable.");
        ErrorHandler::errorGeneric("Error, only single shot mode is avaliable.");
    }

    rules_vector_t program;
    parseProgram(program);
    if (context_.printProgram_) {
        // print the program and exist
        printProgram(program);
        return;
    }

    LOG_DEBUG("Starting scheduler and executors");
    Scheduler scheduler(context_);
    TaskExecutor executor(scheduler.queue_, context_.threads_);
    executor.startThreads();

    processProgram(program, scheduler);
    executor.stopThreadsAndJoin();

    print();
}

void BumbleBeeDB::processProgram(rules_vector_t& program, Scheduler& scheduler) {
    // rewrite the aggregates
    AggregatesRewriter rewriter(context_);
    rewriter.rewrite(program);

    // Order the rules
    StatementDependency sd(std::move(program));
    auto orderedBucketRules = sd.orderRules();

    for (idx_t i = 0; i < orderedBucketRules.size(); i++) {
        LOG_DEBUG("Bucket %d:", i);
        for (auto& rule : orderedBucketRules[i].exit_) {
            LOG_DEBUG("Rule: %s", rule.toString().c_str());
        }
        for (auto& rule : orderedBucketRules[i].constraints_) {
            LOG_DEBUG("Rule: %s", rule.toString().c_str());
        }
        for (auto& rule : orderedBucketRules[i].recursive_) {
            LOG_DEBUG("Rule: %s", rule.toString().c_str());
        }
        LOG_DEBUG("---------------------");
    }

    // Process each bucket of rules
    for (auto &bucket : orderedBucketRules) {
        processBucketRules(bucket, scheduler);
    }
}

void BumbleBeeDB::processExit(RulesBucket &bucket, Scheduler &scheduler) {
    LOG_INFO("Starting planner exit rules...");
    Planner planner(context_);
    prule_ptr_vector_t pruleBucket = planner.plan(bucket.exit_);
    LOG_INFO("Planner completed");


    for (auto& rule : bucket.exit_) {
        LOG_DEBUG("Exit rule: %s", rule.toString().c_str());
    }

    LOG_INFO("Starting execution...");
    scheduler.scheduleRules(pruleBucket);


    if (context_.printProfiling_) {
        // print the profiling result
        auto atomProfiler = scheduler.getAtomProfiler();
        for (auto& prule : pruleBucket) {
            auto patoms = prule->getPhysicalAtoms();
            LOG_INFO("Profile rule: \n%s\n%s", prule->toString().c_str(),atomProfiler.toString(patoms).c_str());
        }
    }
}



void BumbleBeeDB::processRecursive(RulesBucket &bucket, Scheduler &scheduler) {
    if (bucket.recursive_.empty()) return;

    for (auto& rule : bucket.recursive_) {
        LOG_DEBUG("Recursive rule: %s", rule.toString().c_str());
    }


    // check that the head predicate are recursive
    for (auto& rule : bucket.recursive_) {
        BB_ASSERT(rule.getHead().size() == 1);
        auto& pt = context_.defaultSchema_.getPredicateTable(rule.getHead()[0].getPredicate());
        BB_ASSERT(pt->isRecursive());
        BB_ASSERT(pt->isDistinct());
    }

    LOG_INFO("Starting recursive execution...");
    idx_t iteration = 0;
    while (true) {
        LOG_DEBUG("Iteration %d", iteration);

        LOG_INFO("Starting planner recursive rules...");
        Planner planner(context_, true);
        prule_ptr_vector_t pruleBucket = planner.plan(bucket.recursive_);
        LOG_INFO("Planner completed");

        // execute the rules
        scheduler.scheduleRules(pruleBucket);

        if (context_.printProfiling_) {
            // print the profiling result
            auto atomProfiler = scheduler.getAtomProfiler();
            for (auto& prule : pruleBucket) {
                auto patoms = prule->getPhysicalAtoms();
                LOG_INFO("Profile rule: %s\n%s", prule->toString().c_str(),atomProfiler.toString(patoms).c_str());
            }
        }

        // merge delta and fetch delta count
        idx_t deltaCount = 0;
        for (auto& rule : bucket.recursive_) {
            auto& pt = context_.defaultSchema_.getPredicateTable(rule.getHead()[0].getPredicate());
            pt->mergeDelta();
            deltaCount += pt->getDeltaCount();
        }
        LOG_DEBUG("Iteration %d delta count: %d", iteration, deltaCount);

        // stop recursion if deltaCount is zero as no new data was found
        if (deltaCount == 0) break;
        ++iteration;

    }

    // set recursive to false
    for (auto& rule : bucket.recursive_) {
        BB_ASSERT(rule.getHead().size() == 1);
        auto& pt = context_.defaultSchema_.getPredicateTable(rule.getHead()[0].getPredicate());
        pt->setRecursive(false);
    }

}


void BumbleBeeDB::processBucketRules( RulesBucket &bucket, Scheduler& scheduler) {

    LOG_INFO("Processing bucket of rules...");
    LOG_INFO("Exit rules : %d",bucket.exit_.size());
    LOG_INFO("Recursive rules : %d",bucket.recursive_.size());
    LOG_INFO("Constraints rules : %d",bucket.constraints_.size());


    // set the recursive predicates as recursive and distinct
    for (auto& rule: bucket.recursive_) {
        auto& pt = context_.defaultSchema_.getPredicateTable(rule.getHead()[0].getPredicate());
        pt->setRecursive(true);
        pt->predicate_->setDistinct();
        if (pt->getCount() > 0) {
            // init recursive table with non empty data
            pt->createJoinPRLHashTable(pt->getTypes(), pt->getKeys(), {});
        }
    }

    processExit(bucket, scheduler);
    processRecursive(bucket, scheduler);

    scheduler.clearThreadContexts();

    if (!bucket.constraints_.empty()) {
        ErrorHandler::errorNotImplemented("Constraints rule execution not implemented :(");
    }
    // for (auto& rule : bucket.recursive_)LOG_DEBUG("Recursive rule: %s", rule.toString().c_str());
    // for (auto& rule : bucket.constraints_)LOG_DEBUG("Constraint rule: %s", rule.toString().c_str());
}

void BumbleBeeDB::print() {
    auto predicates = context_.defaultSchema_.getPredicates();
    auto outputBuilder = OutputBuilder(TEXT); // //TODO extend with other format
    for (auto& predicate : predicates) {
        if (predicate->isInternal())continue;
        auto& pt = context_.defaultSchema_.getPredicateTable(predicate);
        pt->initializeChunks(); // init chunks if no rules initialized
        if (!pt->isDistinct() || !pt->existJoinPRLHashTable(pt->getKeys(), {})) {
            if (!pt->chunkCount())continue;
            for (idx_t i = 0; i < pt->chunkCount(); ++i) {
                outputBuilder.outputAtoms(pt->getChunk(i), predicate);
            }
        }else {
            auto& ht = pt->getJoinPRLHashTable(pt->getKeys(), {});
            idx_t offset = 0;
            DataChunk result;
            result.initialize(pt->getTypes());
            while (offset < pt->getCount()) {
                result.setCardinality(0);
                ht->scan(offset, result);
                outputBuilder.outputAtoms(result, predicate);
                offset += STANDARD_VECTOR_SIZE;
            }
        }
    }
}

void BumbleBeeDB::printProgram(rules_vector_t &program) {
    for (auto& rule : program) {
        std::cout << rule.toString() << std::endl;
    }
}

BumbleBeeDB::~BumbleBeeDB() {
    Catalog::instance().dropCatalog();
}

} // bumblebee