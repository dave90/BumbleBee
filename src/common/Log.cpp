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
#include "bumblebee/common/Log.hpp"

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


namespace bumblebee {

    static FILE* log_file = nullptr;
    static int print = 0;

    int init_logger(const char* filename, int p) {
        log_file = fopen(filename, "a");
        print = p;
        return (log_file != nullptr) ? 0 : -1;
    }

    void log_message(const char* level, const char* format, ...) {
        if (!log_file) return;

        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        char time_buf[64];
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);

        va_list args;
        va_start(args, format);
        fprintf(log_file, "[%s] ", time_buf);
        fprintf(log_file, " - %s - ", level);
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
        fflush(log_file);
        if(print){
            printf( "[%s] ", time_buf);
            printf(" - %s - ", level);
            vprintf(format, args);
            printf( "\n");
        }
        va_end(args);
    }

    void close_logger() {
        if (log_file) {
            fclose(log_file);
            log_file = nullptr;
        }
    }


}