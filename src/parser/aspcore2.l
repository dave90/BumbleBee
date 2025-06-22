%{
/////////////////////////////////////////////////////////////////////////////
// scanner_asp_rfc.l

/*
This file is part of the ASPCOMP2013 ASP-Core-2 validator (validator in the following)

    The validator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The validator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the validator.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "aspcore2_parser.h"
#include "bumblebee/parser/ParserInputDirector.h"

#define YY_DECL int yylex(bumblebee::ParserInputDirector& director)

#define OPT_PARSER 1

#ifndef OPT_PARSER
#define YY_INPUT(buf,result,max_size)  {\
    result = GetNextChar(buf, max_size); \
    if (  result <= 0  ) \
      result = YY_NULL; \
    }
#endif
%}


%option nounput

BLANK               ([ \t]+)
NEWLINE             \15?\n
DOT 				\.

COMMENT_EMPTY       \%\n
COMMENT             \%[^@\n].*

AGGR_COUNT          #count
AGGR_MAX            #max
AGGR_MIN            #min
AGGR_SUM            #sum
DIRECTIVE_NAME      #([A-Za-z_]*)
DIRECTIVE_VALUE     .*

SYMBOLIC_CONSTANT   ([a-z][A-Za-z_0-9]*)
VARIABLE            ([A-Z][A-Za-z_0-9]*)
STRING              (\"[^\"]*\")
NUMBER              ([0-9]+)

%x ANNOTATION

%x DIRECTIVE_NAME_READ
                      // Only in state DIRECTIVE_NAME_READ a directive value
                      // token could be generated, while normally that token
                      // is not considered because it is too generic and,
                      // consequently, it might be matched anywhere.

%START WANT_NEWLINE
                      // Only in state WANT_NEWLINE a newline
                      // token for the parser is generated, while
                      // normally newlines are simply skipped.
%%
        // Newline is normally skipped, except when explicitly
        // requested with WANT_NEWLINE. (e.g. by a construct that is
        // terminated by a newline.)

<WANT_NEWLINE,DIRECTIVE_NAME_READ>{NEWLINE} {
                            director.onNewLine();
                            BEGIN(INITIAL);
                            return NEWLINE;
}

{NEWLINE}               { director.onNewLine(); }
"."                     { return DOT; }
".."                    { return DDOT; }
","                     { return COMMA; }
"|"                     { return VEL; }
";"                     { return SEMICOLON; }
":"                     { return COLON; }
"@"                     { return AT; }
"\\E"                   { return EXISTS; }

"not"                   { return NAF; }

":-"                    { return CONS; }
":~"                    { return WCONS; }

"+"                     { return PLUS; }
"-"                     { return DASH; }
"*"                     { return TIMES; }
"/"                     { return SLASH; }
"\\"                   	{ return BACK_SLASH; }

"("                     { return PARAM_OPEN; }
")"                     { return PARAM_CLOSE; }

"["                     { return SQUARE_OPEN; }
"]"                     { return SQUARE_CLOSE; }

"{"                     { return CURLY_OPEN; }
"}"                     { return CURLY_CLOSE; }

"?"                     { return QUERY_MARK; }

"_"                     { return ANON_VAR; }

"="     |
"=="          			{ return EQUAL; }
"<>"    |
"!="                    { return UNEQUAL; }
"<"                     { return LESS; }
">"                     { return GREATER; }
"<="                    { return LESS_OR_EQ; }
">="                    { return GREATER_OR_EQ; }
"&"                     { return AMPERSAND; }

{AGGR_COUNT}            {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return AGGR_COUNT;
                        }

{AGGR_MAX}              {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return AGGR_MAX;
                        }

{AGGR_MIN}              {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return AGGR_MIN;
                        }

{AGGR_SUM}              {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return AGGR_SUM;
                        }

{DIRECTIVE_NAME}        {
                            BEGIN(DIRECTIVE_NAME_READ);
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return DIRECTIVE_NAME;
                        }

<DIRECTIVE_NAME_READ>{DIRECTIVE_VALUE} {
                            BEGIN(INITIAL);
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return DIRECTIVE_VALUE;
                        }

{SYMBOLIC_CONSTANT}     {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return SYMBOLIC_CONSTANT;
                        }

{VARIABLE}				{
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return VARIABLE;
                        }

{STRING}                {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return STRING;
                        }

{NUMBER}                {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return NUMBER;
                        }

{COMMENT_EMPTY}			{ /* Skip comments */}
{COMMENT}				{ /* Skip comments */ }
{BLANK}					{ /* Skip blanks */ }

.                       {
                            // Anything not matching one of the patterns
                            // given above is considered an error.
                            char s[1024];
                            assert(yytext[1]==0);
                            sprintf(s,"Lexical syntax error (%s)",yytext);
                            yyerror(director,s);

                            return ERROR;
                        }




