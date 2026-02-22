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
#include "bumblebee/parser/ParserInputDirector.hpp"

#define YY_DECL int yylex(bumblebee::ParserInputDirector& director)

static bumblebee::ParserInputDirector* g_director = nullptr;

#define YY_INPUT(buf,result,max_size)  { \
    result = g_director->readInput(buf, max_size); \
    if (  result <= 0  ) \
      result = YY_NULL; \
    }

#define YY_USER_ACTION \
    director.setTokenStart(); \
    director.advanceColumn(yyleng);
%}


%option nounput

BLANK               ([ \t]+)
NEWLINE             \15?\n
DOT 				\.

SQL_DIALECT         \%@sql


COMMENT_EMPTY       \%\n
COMMENT             \%[^@\n].*


OR                  OR
AND                 AND
AGGR_COUNT          #count
AGGR_MAX            #max
AGGR_MIN            #min
AGGR_SUM            #sum
AGGR_AVG            #avg
AGGR_MULTI          #\[
ID                  #ID
DIRECTIVE_NAME      #([A-Za-z_]*)
DIRECTIVE_VALUE     .*


SYMBOLIC_CONSTANT   ([a-z][A-Za-z_0-9]*)
VARIABLE            ([A-Z][A-Za-z_0-9]*)
STRING              (\"[^\"]*\"|\'[^\']*\')
NUMBER              ([0-9]+(\.[0-9]+)?)

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

%s SQL

%%
        g_director = &director;
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
"OR"                    { return OR; }
"AND"                   { return AND; }

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

{AGGR_AVG}              {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return AGGR_AVG;
                        }

{AGGR_MULTI}            {
                            return AGGR_MULTI;
                        }

{ID}              {
                            yylval.string=new char[strlen(yytext)+1];
                            strcpy(yylval.string,yytext);
                            return ID;
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

{SQL_DIALECT} {
                        BEGIN(SQL);
                        return SQL_DIALECT;
}


<SQL>[Ss][Ee][Ll][Ee][Cc][Tt]           { return SQL_SELECT; }
<SQL>[Ff][Rr][Oo][Mm]                   { return SQL_FROM; }
<SQL>[Ww][Hh][Ee][Rr][Ee]               { return SQL_WHERE; }
<SQL>[Gg][Rr][Oo][Uu][Pp]               { return SQL_GROUP; }
<SQL>[Oo][Rr][Dd][Ee][Rr]               { return SQL_ORDER; }
<SQL>[Bb][Yy]                           { return SQL_BY; }
<SQL>[Aa][Ss]                           { return SQL_AS; }
<SQL>[Cc][Oo][Po][Yy]                   { return SQL_COPY; }
<SQL>[Tt][Oo]                           { return SQL_TO; }
<SQL>[Ll][Ii][Mm][Ii][Tt]               { return SQL_LIMIT; }
<SQL>[Ll][Ii][Kk][Ee]                   { return SQL_LIKE; }

<SQL>[Ss][Uu][Mm]                       {
                                            yylval.string=new char[strlen(yytext)+1];
                                            strcpy(yylval.string,yytext);
                                            return SQL_SUM;
                                        }
<SQL>[Mm][Ii][Nn]                       {
                                            yylval.string=new char[strlen(yytext)+1];
                                            strcpy(yylval.string,yytext);
                                            return SQL_MIN;
                                        }
<SQL>[Mm][Aa][Xx]                       {
                                            yylval.string=new char[strlen(yytext)+1];
                                            strcpy(yylval.string,yytext);
                                            return SQL_MAX;
                                        }
<SQL>[Aa][Vv][Gg]                       {
                                            yylval.string=new char[strlen(yytext)+1];
                                            strcpy(yylval.string,yytext);
                                            return SQL_AVG;
                                        }
<SQL>[Cc][Oo][Uu][Nn][Tt]               {
                                            yylval.string=new char[strlen(yytext)+1];
                                            strcpy(yylval.string,yytext);
                                            return SQL_COUNT;
                                        }
<SQL>[Aa][Ss][Cc]                       {
                                            yylval.string=new char[strlen(yytext)+1];
                                            strcpy(yylval.string,yytext);
                                            return SQL_ASC;
                                        }
<SQL>[Dd][Ee][Ss][Cc]                   {
                                            yylval.string=new char[strlen(yytext)+1];
                                            strcpy(yylval.string,yytext);
                                            return SQL_DESC;
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




