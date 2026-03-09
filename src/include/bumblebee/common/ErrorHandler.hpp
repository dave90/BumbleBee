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

#include <string>
#include <exception>

#include "Constants.hpp"

namespace bumblebee {

class BumbleBeeException : public std::exception {
public:
    explicit BumbleBeeException(std::string message, int code)
        : message_(std::move(message)), code_(code) {}
    const char* what() const noexcept override { return message_.c_str(); }
    int code() const noexcept { return code_; }
private:
    std::string message_;
    int code_;
};

class ParsingException : public BumbleBeeException {
public:
    explicit ParsingException(const std::string& message)
        : BumbleBeeException(message, ERROR_PARSING_CODE) {}
};

class GenericException : public BumbleBeeException {
public:
    explicit GenericException(const std::string& message)
        : BumbleBeeException(message, ERROR_GENERIC_CODE) {}
};

class NotImplementedException : public BumbleBeeException {
public:
    explicit NotImplementedException(const std::string& message)
        : BumbleBeeException(message, ERROR_NOT_IMPLEMENTED_CODE) {}
};

class OutOfMemoryException : public BumbleBeeException {
public:
    explicit OutOfMemoryException(const std::string& message)
        : BumbleBeeException(message, ERROR_OUT_OF_MEMORY_CODE) {}
};

class ErrorHandler
{

public:
    static void errorParsing( const std::string& );
    static void errorParsing( const char*  );

    static void errorGeneric( const std::string&  );
    static void errorGeneric( const char*  );

    static void errorNotImplemented( const std::string&  );
    static void errorNotImplemented( const char*  );

    static void outOfMemory( const std::string&  );
    static void outOfMemory( const char*  );


};



} // bumblebee
