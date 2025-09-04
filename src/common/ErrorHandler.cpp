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

namespace bumblebee {

void ErrorHandler::errorParsing( const std::string& message )
{
    errorParsing( message.c_str() );
}

void ErrorHandler::errorParsing( const char* message )
{
    std::cerr << ERROR_PARSING << ": " << message << std::endl;
    exit( ERROR_PARSING_CODE );
}

void ErrorHandler::errorGeneric( const std::string& message )
{
    errorGeneric( message.c_str() );
}

void ErrorHandler::errorGeneric( const char* message )
{
    std::cerr << ERROR_GENERIC << ": " << message << std::endl;
    exit( ERROR_GENERIC_CODE );
}

void ErrorHandler::errorNotImplemented(const char *message) {
    std::cerr << ERROR_NOT_IMPLEMENTED << ": " << message << std::endl;
    exit( ERROR_NOT_IMPLEMENTED );
}

void ErrorHandler::errorNotImplemented( const std::string& message )
{
    errorNotImplemented( message.c_str() );
}

void ErrorHandler::outOfMemory( const std::string& message )
{
    std::cerr << ERROR_OUT_OF_MEMORY << ": " << message << std::endl;
    exit( ERROR_OUT_OF_MEMORY );
}

void ErrorHandler::outOfMemory( const char* message )
{
    std::cerr << ERROR_OUT_OF_MEMORY << ": " << message << std::endl;
    exit( ERROR_OUT_OF_MEMORY );
}
} // bumblebee