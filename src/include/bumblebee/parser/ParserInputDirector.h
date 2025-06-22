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

#include "ParserInputBuilder.h"

#include <vector>
#include <cassert>

namespace bumblebee {

using BuilderPtr = std::shared_ptr<ParserInputBuilder>;

class ParserInputDirector {

public:
    ParserInputDirector();
    ~ParserInputDirector() = default;

    void configureBuilder( BuilderPtr );
    ParserInputBuilder* getBuilder();

    //////////////////////////////////////////////////////////////////////////////
    // Parsing functions
    int parse( std::vector<std::string> files);
    int parse( const char* filename, FILE *file );
    int parse( );

    int onError(const char* msg);
    void onNewLine() ;


private:
    int parserLine_;
    const char* parserFile_;
    unsigned parserErrors_;
    bool parserStateInternal_;
    BuilderPtr builder_;
    bool findError_;
};
};

struct ParsingVar{
    ParsingVar():eof(0),nRow(0),nBuffer(0),lBuffer(0),nTokenStart(0),nTokenNextStart(0),lMaxBuffer(10000),buffer(new char[lMaxBuffer]){}
    ~ParsingVar(){if(buffer!=nullptr)delete []buffer;}
    void clear(){
        eof=0;
        nRow=0;
        nBuffer=0;
        lBuffer=0;
        nTokenStart=0;
        nTokenNextStart=0;
    }
    int eof;
    int nRow ;
    int nBuffer ;
    int lBuffer ;
    int nTokenStart ;
    int nTokenNextStart ;
    int lMaxBuffer;
    char *buffer;
};
extern "C" int yylex(bumblebee::ParserInputDirector& director);
extern "C" int yyerror(bumblebee::ParserInputDirector&, const char*);
extern "C" int GetNextChar(char *b, int maxBuffer);
