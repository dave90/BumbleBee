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

#if LOG_LEVEL <= LOG_LVL_DEBUG
#define LOG_DEBUG(...) log_message("DEBUG",__VA_ARGS__)
#else
#define LOG_DEBUG(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LVL_INFO
#define LOG_INFO(...) log_message("INFO",__VA_ARGS__)
#else
#define LOG_INFO(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LVL_WARNING
#define LOG_WARNING(...) log_message("WARNING",__VA_ARGS__)
#else
#define LOG_WARNING(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LVL_ERROR
#define LOG_ERROR(...) log_message("ERROR",__VA_ARGS__)
#else
#define LOG_ERROR(...) ((void)0)
#endif


// Initializes the logger with a file path
int init_logger(const char* filename, int print);

// Writes a message to the log file
void log_message(const char* level, const char* format, ...);

// Closes the log file
void close_logger();

} // bumblebee
