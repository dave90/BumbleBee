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
     AGGR_MULTI = 269,
     ID = 270,
     ERROR = 271,
     NEWLINE = 272,
     DOT = 273,
     DDOT = 274,
     SEMICOLON = 275,
     COLON = 276,
     CONS = 277,
     QUERY_MARK = 278,
     PLUS = 279,
     TIMES = 280,
     SLASH = 281,
     BACK_SLASH = 282,
     ANON_VAR = 283,
     PARAM_OPEN = 284,
     PARAM_CLOSE = 285,
     SQUARE_OPEN = 286,
     SQUARE_CLOSE = 287,
     CURLY_OPEN = 288,
     CURLY_CLOSE = 289,
     EQUAL = 290,
     UNEQUAL = 291,
     LESS = 292,
     GREATER = 293,
     LESS_OR_EQ = 294,
     GREATER_OR_EQ = 295,
     OR = 296,
     AND = 297,
     DASH = 298,
     COMMA = 299,
     NAF = 300,
     AT = 301,
     WCONS = 302,
     AMPERSAND = 303,
     VEL = 304,
     EXISTS = 305,
     SQL_SELECT = 306,
     SQL_FROM = 307,
     SQL_WHERE = 308,
     SQL_GROUP = 309,
     SQL_BY = 310,
     SQL_AS = 311,
     SQL_COPY = 312,
     SQL_TO = 313,
     SQL_ORDER = 314,
     SQL_ASC = 315,
     SQL_DESC = 316,
     SQL_LIMIT = 317,
     SQL_LIKE = 318,
     SQL_SUM = 319,
     SQL_MIN = 320,
     SQL_MAX = 321,
     SQL_AVG = 322,
     SQL_COUNT = 323,
     SQL_DIALECT = 324
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
#define AGGR_MULTI 269
#define ID 270
#define ERROR 271
#define NEWLINE 272
#define DOT 273
#define DDOT 274
#define SEMICOLON 275
#define COLON 276
#define CONS 277
#define QUERY_MARK 278
#define PLUS 279
#define TIMES 280
#define SLASH 281
#define BACK_SLASH 282
#define ANON_VAR 283
#define PARAM_OPEN 284
#define PARAM_CLOSE 285
#define SQUARE_OPEN 286
#define SQUARE_CLOSE 287
#define CURLY_OPEN 288
#define CURLY_CLOSE 289
#define EQUAL 290
#define UNEQUAL 291
#define LESS 292
#define GREATER 293
#define LESS_OR_EQ 294
#define GREATER_OR_EQ 295
#define OR 296
#define AND 297
#define DASH 298
#define COMMA 299
#define NAF 300
#define AT 301
#define WCONS 302
#define AMPERSAND 303
#define VEL 304
#define EXISTS 305
#define SQL_SELECT 306
#define SQL_FROM 307
#define SQL_WHERE 308
#define SQL_GROUP 309
#define SQL_BY 310
#define SQL_AS 311
#define SQL_COPY 312
#define SQL_TO 313
#define SQL_ORDER 314
#define SQL_ASC 315
#define SQL_DESC 316
#define SQL_LIMIT 317
#define SQL_LIKE 318
#define SQL_SUM 319
#define SQL_MIN 320
#define SQL_MAX 321
#define SQL_AVG 322
#define SQL_COUNT 323
#define SQL_DIALECT 324




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 33 "src/parser/aspcore2.y"
{
    char* string;
    char single_char;
    int integer;
}
/* Line 1529 of yacc.c.  */
#line 193 "src/parser/aspcore2_parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

