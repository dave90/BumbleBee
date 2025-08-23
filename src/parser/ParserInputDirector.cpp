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
#include "bumblebee/parser/ParserInputDirector.h"

#include "bumblebee/common/ErrorHandler.h"
#include "aspcore2_lexer.hpp"
#include "aspcore2_parser.hpp"

/*
 * local variable for parsing file
 */
static ParsingVar pv;

namespace bumblebee {

ParserInputDirector::ParserInputDirector(OutputType type, bool hiddenNewPredicates) : parserLine_(0),
    parserFile_(nullptr),
    parserErrors_(0),
    parserStateInternal_(false),
    builder_(std::make_shared<ParserInputBuilder>(type,hiddenNewPredicates)),
    outputType_(type),
    findError_(false){}


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
    yyparse(*this);

    if( parserErrors_ > 0 ) {
        std::cerr << "Aborting due to parser errors." << std::endl;
        return -1;
    }
    return 0;
}

int ParserInputDirector::parse() {
    pv.clear();
    yyin = stdin;
    parserFile_ = "STDIN";
    parserLine_ = 1;
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

    if( !parserStateInternal_ )
    {
        if(parserFile_ && strlen(parserFile_) > 0 )
            std::cerr << parserFile_ << ": ";
        std::cerr << "line " << parserLine_;

        std::cerr << ": ";
    }
    else
        std::cerr << "Internal parser invocation: ";

    std::cerr << msg << "." << std::endl;

    return 0;
}

void ParserInputDirector::onNewLine() { parserLine_++; }

ParserInputBuilder* ParserInputDirector::getBuilder() { return builder_.get(); }

} // bumblebee

// ----------------------------------------------------------
// ------------------- Parser Custom Functions -------------------------------

int yyerror(bumblebee::ParserInputDirector& director, const char* msg ){
    return director.onError(msg);
}


extern "C" int yywrap() { return 1; }    // End-of-file handler for LEX
extern FILE* yyin;     // Where LEX reads its input from


/*--------------------------------------------------------------------
 * getNextLine
 *
 * reads a line into the buffer
 *------------------------------------------------------------------*/
static
int getNextLine(void) {
  char *p;

  /*================================================================*/
  /*----------------------------------------------------------------*/
  pv.nBuffer = 0;
  pv.nTokenStart = -1;
  pv.nTokenNextStart = 1;
  pv.eof = false;

  /*================================================================*/
  /* read a line ---------------------------------------------------*/
  p = fgets(pv.buffer, pv.lMaxBuffer, yyin);
  if (  p == NULL  ) {
    if (  ferror(yyin)  )
      return -1;
    pv.eof = true;
    return 1;
  }

  pv.nRow += 1;
  pv.lBuffer = strlen(pv.buffer);

  /*================================================================*/
  /* that's it -----------------------------------------------------*/
  return 0;
}
/*--------------------------------------------------------------------
 * GetNextChar
 *
 * reads a character from input for flex
 *------------------------------------------------------------------*/
extern
int GetNextChar(char *b, int maxBuffer) {
  int frc;

  /*================================================================*/
  /*----------------------------------------------------------------*/
  if (  pv.eof  )
    return 0;

  /*================================================================*/
  /* read next line if at the end of the current -------------------*/
  while (  pv.nBuffer >= pv.lBuffer  ) {
    frc = getNextLine();
    if (  frc != 0  )
      return 0;
    }

  /*================================================================*/
  /* ok, return character ------------------------------------------*/
  b[0] = pv.buffer[pv.nBuffer];
  pv.nBuffer += 1;

  return b[0]==0?0:1;
}
