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
#define ERROR_DB "\nDatabase error"
#define ERROR_PARSING_CODE 100
#define ERROR_GENERIC_CODE 110

// Logs
#define DEFAULT_LOG_FILE "bbdb.log"
#define LOG_LVL_DEBUG 0
#define LOG_LVL_INFO 1
#define LOG_LVL_WARNING 2
#define LOG_LVL_ERROR 3

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LVL_DEBUG
#endif

}