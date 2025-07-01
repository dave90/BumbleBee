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
#include  "bumblebee/common/Log.h"
#include  "bumblebee/common/ErrorHandler.h"
#include  "bumblebee/parser/ParserInputDirector.h"

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
    ParserInputDirector inputDirector;
    inputDirector.parse(inputFiles_);

    if (inputDirector.getBuilder()->isFoundASafetyError()) {
        // found a rule unsafe
        std::string error = inputDirector.getBuilder()->getSafetyErrorMessage();
        LOG_ERROR("Error: %s ",error.c_str());
        ErrorHandler::errorParsing("Error, found unsafe rule.");
    }

    // TODO continue processing
    // create dags
    // rewrite rule
    rules_vector_t program = std::move(inputDirector.getBuilder()->getProgram());
    for (auto& rule : program) {
        std::cout << rule.toString() << std::endl;
    }

}

} // bumblebee