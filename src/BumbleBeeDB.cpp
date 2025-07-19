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
#include "bumblebee/parser/ParserInputDirector.h"
#include "bumblebee/planner/StatementDependency.h"

namespace bumblebee {
int BumbleBeeDB::parseArgs(int argc, char **argv) {
    CLI::App app{NAME};

    app.add_option("-l,--log-file", logFilename_, "Log file")->default_val(DEFAULT_LOG_FILE);
    app.add_flag("-p,--print-log", printLog_, "Print log")->default_val(0);
    app.add_flag("-s,--single-shot", singleShot_, "Single shot run")->default_val(1);
    app.add_option("-i,--input-files", inputFiles_, "Single shot run")->expected(1, -1);

    CLI11_PARSE(app, argc, argv);
    bumblebee::init_logger(logFilename_.c_str()  , printLog_);

    return 0;
}

void BumbleBeeDB::printArgs() {
    LOG_INFO("Arguments:\n\tLog Filename: %s\n\tPrint Log: %d\n\tSingle shot: %d",
        logFilename_.c_str(),
        printLog_,
        singleShot_);
}

void BumbleBeeDB::run() {
    if (!singleShot_) {
        LOG_ERROR("Error, only single shot mode is avaliable.");
        ErrorHandler::errorGeneric("Error, only single shot mode is avaliable.");
    }
    // Parse the program
    ParserInputDirector inputDirector(TEXT); // DEFAULT TEXT output
    inputDirector.parse(inputFiles_);
    // Check errors during parsing
    if (inputDirector.getBuilder()->isFoundASafetyError()) {
        // found a rule unsafe
        std::string error = inputDirector.getBuilder()->getSafetyErrorMessage();
        LOG_ERROR("Error: %s ",error.c_str());
        ErrorHandler::errorParsing("Error, found unsafe rule.");
    }
    rules_vector_t program = std::move(inputDirector.getBuilder()->getProgram());

    LOG_INFO("Program size: %u", program.size());
    for (auto& rule : program) {
        LOG_DEBUG("Rule: %s", rule.toString().c_str());
    }

    // Order the rules
    StatementDependency sd(std::move(program));
    auto orderedBucketRules = sd.orderRules();

    // Process each bucket of rules
    for (auto &bucket : orderedBucketRules) {
        processBucketRules(bucket);
    }

}

void BumbleBeeDB::processBucketRules( RulesBucket &bucket) {
    LOG_INFO("Processing bucket of rules...");
    for (auto& rule : bucket.exit)LOG_DEBUG("Exit rule: %s", rule.toString().c_str());
    for (auto& rule : bucket.recursive)LOG_DEBUG("Recursive rule: %s", rule.toString().c_str());
    for (auto& rule : bucket.constraints)LOG_DEBUG("Constraint rule: %s", rule.toString().c_str());
}
} // bumblebee