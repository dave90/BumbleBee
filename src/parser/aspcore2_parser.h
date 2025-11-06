/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SYMBOLIC_CONSTANT = 258,
     NUMBER = 259,
     VARIABLE = 260,
     STRING = 261,
     DIRECTIVE_NAME = 262,
     DIRECTIVE_VALUE = 263,
     AGGR_COUNT = 264,
     AGGR_MAX = 265,
     AGGR_MIN = 266,
     AGGR_SUM = 267,
     AGGR_AVG = 268,
     ERROR = 269,
     NEWLINE = 270,
     DOT = 271,
     DDOT = 272,
     SEMICOLON = 273,
     COLON = 274,
     CONS = 275,
     QUERY_MARK = 276,
     PLUS = 277,
     TIMES = 278,
     SLASH = 279,
     BACK_SLASH = 280,
     ANON_VAR = 281,
     PARAM_OPEN = 282,
     PARAM_CLOSE = 283,
     SQUARE_OPEN = 284,
     SQUARE_CLOSE = 285,
     CURLY_OPEN = 286,
     CURLY_CLOSE = 287,
     EQUAL = 288,
     UNEQUAL = 289,
     LESS = 290,
     GREATER = 291,
     LESS_OR_EQ = 292,
     GREATER_OR_EQ = 293,
     DASH = 294,
     COMMA = 295,
     NAF = 296,
     AT = 297,
     WCONS = 298,
     AMPERSAND = 299,
     VEL = 300,
     EXISTS = 301,
     SQL_SELECT = 302,
     SQL_FROM = 303,
     SQL_WHERE = 304,
     SQL_GROUP = 305,
     SQL_BY = 306,
     SQL_AS = 307,
     SQL_AND = 308,
     SQL_OR = 309,
     SQL_COPY = 310,
     SQL_TO = 311,
     SQL_SUM = 312,
     SQL_MIN = 313,
     SQL_MAX = 314,
     SQL_AVG = 315,
     SQL_DIALECT = 316
   };
#endif
/* Tokens.  */
#define SYMBOLIC_CONSTANT 258
#define NUMBER 259
#define VARIABLE 260
#define STRING 261
#define DIRECTIVE_NAME 262
#define DIRECTIVE_VALUE 263
#define AGGR_COUNT 264
#define AGGR_MAX 265
#define AGGR_MIN 266
#define AGGR_SUM 267
#define AGGR_AVG 268
#define ERROR 269
#define NEWLINE 270
#define DOT 271
#define DDOT 272
#define SEMICOLON 273
#define COLON 274
#define CONS 275
#define QUERY_MARK 276
#define PLUS 277
#define TIMES 278
#define SLASH 279
#define BACK_SLASH 280
#define ANON_VAR 281
#define PARAM_OPEN 282
#define PARAM_CLOSE 283
#define SQUARE_OPEN 284
#define SQUARE_CLOSE 285
#define CURLY_OPEN 286
#define CURLY_CLOSE 287
#define EQUAL 288
#define UNEQUAL 289
#define LESS 290
#define GREATER 291
#define LESS_OR_EQ 292
#define GREATER_OR_EQ 293
#define DASH 294
#define COMMA 295
#define NAF 296
#define AT 297
#define WCONS 298
#define AMPERSAND 299
#define VEL 300
#define EXISTS 301
#define SQL_SELECT 302
#define SQL_FROM 303
#define SQL_WHERE 304
#define SQL_GROUP 305
#define SQL_BY 306
#define SQL_AS 307
#define SQL_AND 308
#define SQL_OR 309
#define SQL_COPY 310
#define SQL_TO 311
#define SQL_SUM 312
#define SQL_MIN 313
#define SQL_MAX 314
#define SQL_AVG 315
#define SQL_DIALECT 316




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 33 "src/parser/aspcore2.y"
{
    char* string;
    char single_char;
    int integer;
}
/* Line 1529 of yacc.c.  */
#line 177 "src/parser/aspcore2_parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

