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


#include <filesystem>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/output/OutputBuilder.hpp"
#include "bumblebee/parser/ParserInputDirector.hpp"
#include "bumblebee/parser/statement/Rule.hpp"
using namespace std;
using namespace filesystem;
using namespace bumblebee;


inline rules_vector_t getRulesFromFile(string testFileName) {
    path TEST_FILE_PATH = __FILE__;
    ClientContext context;
    ParserInputDirector pid(TEXT, context);
    path dataFilePath = TEST_FILE_PATH.parent_path() / "data" / testFileName;
    std::vector<string> fiels = {dataFilePath.c_str()};
    pid.parse(fiels);
    auto program = std::move(pid.getBuilder()->getProgram());
    return program;
}