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
#include "bumblebee/common/ErrorHandler.hpp"

#include <iostream>
#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee {

static void log_error(const char* error, const std::string& message) {
    std::string e = error +std::string(": ")+message;
    std::cerr << e << std::endl;
    LOG_ERROR( e.c_str() );
}

void ErrorHandler::errorParsing( const std::string& message )
{
    log_error(ERROR_PARSING, message);
    throw ParsingException(message);
}

void ErrorHandler::errorParsing( const char* message )
{
    errorParsing( std::string(message) );
}

void ErrorHandler::errorGeneric( const std::string& message )
{
    log_error(ERROR_GENERIC, message);
    throw GenericException(message);
}

void ErrorHandler::errorGeneric( const char* message )
{
    errorGeneric(std::string(message));
}

void ErrorHandler::errorNotImplemented(const char *message) {
    errorNotImplemented(std::string(message));
}

void ErrorHandler::errorNotImplemented( const std::string& message )
{
    log_error(ERROR_NOT_IMPLEMENTED, message);
    throw NotImplementedException(message);
}

void ErrorHandler::outOfMemory( const std::string& message )
{
    log_error(ERROR_OUT_OF_MEMORY, message);
    throw OutOfMemoryException(message);
}

void ErrorHandler::outOfMemory( const char* message )
{
    outOfMemory(std::string(message));
}
} // bumblebee