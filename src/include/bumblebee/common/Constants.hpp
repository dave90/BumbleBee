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
#pragma once

namespace bumblebee {

#define NAME "BumbleBee"

// Error messages
#define ERROR_PARSING "\nError during parsing"
#define ERROR_GENERIC "\nGeneric error"
#define ERROR_NOT_IMPLEMENTED "\nError not implemented"
#define ERROR_OUT_OF_MEMORY "\nError out of memory"
#define ERROR_DB "\nDatabase error"
#define ERROR_PARSING_CODE 100
#define ERROR_GENERIC_CODE 110
#define ERROR_NOT_IMPLEMENTED_CODE 120
#define ERROR_OUT_OF_MEMORY_CODE 130

// Logs
#define DEFAULT_LOG_FILE "bbdb.log"
#define LOG_LVL_DEBUG 0
#define LOG_LVL_INFO 1
#define LOG_LVL_WARNING 2
#define LOG_LVL_ERROR 3

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LVL_DEBUG
#endif

// Vector size
#define STANDARD_VECTOR_SIZE 1024
#define HT_INIT_CAPACITY 32768

// String Heap. NOTE: Maximum string supported is 4096 bytes
#define MINIMUM_HEAP_SIZE 4096

// Default size of morsel (chunk to process in one thread) Must be multiple of STANDARD_VECTOR_SIZE
#define MORSEL_SIZE (STANDARD_VECTOR_SIZE*4)

#define OUTPUT_BUFFER_SIZE 1024

// 1 enable the profiling 0 disable it
#define PROFILING 1

#define DEFAULT_TMP_DIR "./bumblebee/"

}