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

#include "ParserInputBuilder.hpp"

#include <string>
#include <vector>

namespace bumblebee {

using builder_ptr_t = std::shared_ptr<ParserInputBuilder>;

class ParserInputDirector {

public:
    ParserInputDirector(OutputType type, ClientContext& context );
    ~ParserInputDirector() = default;

    void configureBuilder( builder_ptr_t );
    ParserInputBuilder* getBuilder();

    //////////////////////////////////////////////////////////////////////////////
    // Parsing functions
    int parse( vector<std::string> files);
    int parse( const char* filename, FILE *file );
    int parse( );

    int onError(const char* msg);
    void onNewLine() ;

    // Column tracking for error context
    int readInput(char* buf, int maxSize);
    void advanceColumn(int n) { parserColumn_ += n; }
    void setTokenStart() { parserTokenStart_ = parserColumn_; }
    int getColumn() const { return parserColumn_; }

private:
    int parserLine_;
    const char* parserFile_;
    unsigned parserErrors_;
    bool parserStateInternal_;
    builder_ptr_t builder_;
    bool findError_;
    OutputType outputType_;

    // Column and line tracking for error context
    int parserColumn_;
    int parserTokenStart_;
    std::vector<std::string> lines_;  // All lines read so far (0-indexed)
    std::string pendingLine_;         // Line currently being assembled by readInput

};
};

extern "C" int yylex(bumblebee::ParserInputDirector& director);
extern "C" int yyerror(bumblebee::ParserInputDirector&, const char*);
