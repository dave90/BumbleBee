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
#include "bumblebee/parser/ParserInputDirector.hpp"

#include "bumblebee/common/ErrorHandler.hpp"
#include "aspcore2_lexer.hpp"

/* GLR mode makes bison generate: #define YYLEX yylex (, director)
 * (it prepends a YYLTYPE* location argument before the %lex-param).
 * The flex-generated yylex only accepts (director).
 * Bridge: capture the function pointer before the macro override, then
 * provide a 2-argument wrapper that discards the (empty) location arg. */
static int (*const yylex_original_ptr)(bumblebee::ParserInputDirector&) = &yylex;

static int yylex_glr_bridge(bumblebee::ParserInputDirector& director) {
    return yylex_original_ptr(director);
}

/* Override yylex so YYLEX expands to a valid call.
 * C99 variadic macros allow the first argument to be empty:
 *   yylex (, director)  →  yylex_glr_bridge(director)           */
#undef yylex
#define yylex(ignored_loc, ...) yylex_glr_bridge(__VA_ARGS__)

#include "aspcore2_parser.hpp"
#include "bumblebee/common/Log.hpp"

namespace bumblebee {

ParserInputDirector::ParserInputDirector(OutputType type, ClientContext& context) : parserLine_(0),
    parserFile_(nullptr),
    parserErrors_(0),
    parserStateInternal_(false),
    builder_(std::make_shared<ParserInputBuilder>(type,context)),
    outputType_(type),
    findError_(false),
    parserColumn_(0),
    parserTokenStart_(0){}


void ParserInputDirector::configureBuilder(builder_ptr_t newBuilder) {
    builder_ = newBuilder;
}

int ParserInputDirector::parse(vector<std::string> files) {
    if( files.empty() )
        ErrorHandler::errorGeneric( "No input files." );

    for(auto & s : files) {
        auto i = s.c_str();
        if(!strcmp(i,"--")){
            if(ParserInputDirector::parse()!=0)
                return -1;
            continue;
        }
        FILE *file = fopen(i, "r");
        if(file) {
            const int res = parse(i, file);
            fclose(file);
            if(res != 0)
                return res;
        }
        else {
            onError("Can't open input");
            return -1;
        }
    }
    return 0;
}

int ParserInputDirector::parse(const char *filename, FILE *file) {
    yyin = file;
    parserFile_ = filename;
    parserLine_ = 1;
    parserColumn_ = 0;
    parserTokenStart_ = 0;
    lines_.clear();
    pendingLine_.clear();
    yyparse(*this);

    if( parserErrors_ > 0 ) {
        return -1;
    }
    return 0;
}

int ParserInputDirector::parse(const std::string& program) {
    FILE* mem = fmemopen(const_cast<char*>(program.data()), program.size(), "r");
    if (!mem) {
        onError("Failed to open in-memory buffer for parsing.");
        return -1;
    }
    const int res = parse("<string>", mem);
    fclose(mem);
    return res;
}

int ParserInputDirector::parse() {
    yyin = stdin;
    parserFile_ = "STDIN";
    parserLine_ = 1;
    parserColumn_ = 0;
    parserTokenStart_ = 0;
    lines_.clear();
    pendingLine_.clear();
    yyparse(*this);

    if( parserErrors_ > 0 ) {
        ErrorHandler::errorParsing("Aborting due to parser errors.");
        return -1;
    }
    return 0;
}

int ParserInputDirector::onError(const char *msg) {
    if(findError_) return 0;

    findError_=true;

    parserErrors_++;

    string error;
    if( !parserStateInternal_ )
    {
        if(parserFile_ && strlen(parserFile_) > 0 ) {
            error = parserFile_;
            error += ": ";
        }
        error += "line " + std::to_string(parserLine_) + " : ";
    }
    else
        error = "Internal parser invocation: ";

    LOG_ERROR(error.c_str());
    LOG_ERROR(msg);

    // Show the offending source line with a caret pointing to the error position
    // parserLine_ is 1-based; lines_ is 0-indexed
    std::string line;
    int lineIdx = parserLine_ - 1;
    if (lineIdx >= 0 && lineIdx < (int)lines_.size()) {
        line = lines_[lineIdx];
    }
    if (line.empty() && !pendingLine_.empty()) {
        // Error on the last line that hasn't been terminated by newline yet
        line = pendingLine_;
    }
    if (line.empty() && lineIdx > 0 && lineIdx - 1 < (int)lines_.size()) {
        // Fall back to previous line (e.g., error at EOF after newline)
        line = lines_[lineIdx - 1];
    }
    if (!line.empty()) {
        LOG_ERROR("  %s", line.c_str());
        int caretPos = parserTokenStart_;
        if (caretPos > (int)line.size()) caretPos = (int)line.size();
        if (caretPos < 0) caretPos = 0;
        std::string caret(caretPos + 2, ' '); // +2 for the "  " prefix
        caret += '^';
        LOG_ERROR("%s", caret.c_str());
    }

    return 0;
}

void ParserInputDirector::onNewLine() {
    parserLine_++;
    parserColumn_ = 0;
}

int ParserInputDirector::readInput(char* buf, int maxSize) {
    int result = (int)fread(buf, 1, maxSize, yyin);
    for (int i = 0; i < result; i++) {
        if (buf[i] == '\n') {
            lines_.push_back(std::move(pendingLine_));
            pendingLine_.clear();
        } else if (buf[i] != '\r') {
            pendingLine_ += buf[i];
        }
    }
    return result;
}

ParserInputBuilder* ParserInputDirector::getBuilder() { return builder_.get(); }

} // bumblebee

// ----------------------------------------------------------
// ------------------- Parser Custom Functions -------------------------------

int yyerror(bumblebee::ParserInputDirector& director, const char* msg ){
    return director.onError(msg);
}


extern "C" int yywrap() { return 1; }    // End-of-file handler for LEX
extern FILE* yyin;     // Where LEX reads its input from
