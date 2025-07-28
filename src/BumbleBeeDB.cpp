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
#include "include/bumblebee/BumbleBeeDB.h"

#include <CLI11.hpp>

#include "bumblebee/common/Constants.h"
#include "bumblebee/common/Log.h"
#include "bumblebee/common/ErrorHandler.h"
#include "bumblebee/parallel/TaskExecutor.h"
#include "bumblebee/parser/ParserInputDirector.h"
#include "bumblebee/planner/Planner.h"
#include "bumblebee/planner/StatementDependency.h"

namespace bumblebee {
int BumbleBeeDB::parseArgs(int argc, char **argv) {
    CLI::App app{NAME};

    app.add_option("-l,--log-file", context_.logFilename_, "Log file")->default_val(DEFAULT_LOG_FILE);
    app.add_flag("-p,--print-log", context_.printLog_, "Print log")->default_val(0);
    // app.add_flag("-s,--single-shot", context_.singleShot_, "Single shot run")->default_val(1);
    app.add_option("-i,--input-files", context_.inputFiles_, "Single shot run")->expected(1, -1);
    app.add_option("-t,--threads", context_.threads_, "Numbers of threads")->expected(1, INT_MAX)->default_val(1);
    app.add_flag("-a,--print-all", context_.printAll_, "Print all predicates")->default_val(0);
    app.add_flag("-r,--print-profiling", context_.printProfiling_, "Print profilings")->default_val(0);

    CLI11_PARSE(app, argc, argv);
    init_logger(context_.logFilename_.c_str()  , context_.printLog_);
    printArgs();
    return 0;
}

void BumbleBeeDB::printArgs() {
    LOG_INFO("Arguments:\n\tLog Filename: %s\n\tPrint Log: %d\n\tSingle shot: %d \n\tThreads:%d \n\tPrint all predicates:%d",
        context_.logFilename_.c_str(),
        context_.printLog_,
        context_.singleShot_,
        context_.threads_,
        context_.printAll_);
}

void BumbleBeeDB::parseProgram(rules_vector_t &program) {
    // Parse the program
    ParserInputDirector inputDirector(TEXT, !context_.printAll_); // DEFAULT TEXT output
    inputDirector.parse(context_.inputFiles_);
    // Check errors during parsing
    if (inputDirector.getBuilder()->isFoundASafetyError()) {
        // found a rule unsafe
        std::string error = inputDirector.getBuilder()->getSafetyErrorMessage();
        LOG_ERROR("Error: %s ",error.c_str());
        ErrorHandler::errorParsing("Error, found unsafe rule.");
    }
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
    LOG_DEBUG("Starting scheduler and executors");
    Scheduler scheduler(context_);
    TaskExecutor executor(scheduler.queue_, context_.threads_);
    executor.startThreads();

    rules_vector_t program;
    parseProgram(program);

    processProgram(program, scheduler);
    executor.stopThreadsAndJoin();

    printIDB();
}

void BumbleBeeDB::processProgram(rules_vector_t& program, Scheduler& scheduler) {
    // Order the rules
    StatementDependency sd(std::move(program));
    auto orderedBucketRules = sd.orderRules();

    // Process each bucket of rules
    for (auto &bucket : orderedBucketRules) {
        processBucketRules(bucket, scheduler);
    }
}

void BumbleBeeDB::processBucketRules( RulesBucket &bucket, Scheduler& scheduler) {

    LOG_INFO("Processing bucket of rules...");
    LOG_INFO("Exit rules : %d",bucket.exit_.size());
    LOG_INFO("Recursive rules : %d",bucket.recursive_.size());
    LOG_INFO("Constraints rules : %d",bucket.constraints_.size());
    for (auto& rule : bucket.exit_) {
        LOG_DEBUG("Exit rule: %s", rule.toString().c_str());
    }

    LOG_INFO("Starting planner...");
    Planner planner(context_);
    auto pruleBucket = planner.plan(bucket);
    LOG_INFO("Planner completed");
    LOG_INFO("Starting execution...");
    scheduler.scheduleRules(pruleBucket);

    if (context_.printProfiling_) {
        // print the profiling result
        LOG_INFO("Profiling:\n%s",scheduler.profilingAsString().c_str());
    }
    scheduler.clearThreadContexts();

    if (!bucket.recursive_.empty()) {
        ErrorHandler::errorNotImplemented("Recursive rule execution not implemented :(");
    }
    if (!bucket.constraints_.empty()) {
        ErrorHandler::errorNotImplemented("Constraints rule execution not implemented :(");
    }
    // for (auto& rule : bucket.recursive_)LOG_DEBUG("Recursive rule: %s", rule.toString().c_str());
    // for (auto& rule : bucket.constraints_)LOG_DEBUG("Constraint rule: %s", rule.toString().c_str());
}

void BumbleBeeDB::printIDB() {
    auto predicates = context_.defaultSchema_.getPredicates();
    auto outputBuilder = OutputBuilder(TEXT); // //TODO extend with other format
    for (auto& predicate : predicates) {
        if (predicate->isInternal())continue;
        if (predicate->isEdb())continue; // facts printed during the parsing
        auto& pt = context_.defaultSchema_.getPredicateTable(predicate);
        if (!pt->chunkCount())continue;
        for (idx_t i = 0; i < pt->chunkCount(); ++i) {
            outputBuilder.outputAtoms(pt->getChunk(i), predicate);
        }
    }
}
} // bumblebee