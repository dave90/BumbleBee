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
#include <iostream>

#include "Constants.hpp"

namespace bumblebee {

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
