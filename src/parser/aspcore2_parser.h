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
     ID = 269,
     ERROR = 270,
     NEWLINE = 271,
     DOT = 272,
     DDOT = 273,
     SEMICOLON = 274,
     COLON = 275,
     CONS = 276,
     QUERY_MARK = 277,
     PLUS = 278,
     TIMES = 279,
     SLASH = 280,
     BACK_SLASH = 281,
     ANON_VAR = 282,
     PARAM_OPEN = 283,
     PARAM_CLOSE = 284,
     SQUARE_OPEN = 285,
     SQUARE_CLOSE = 286,
     CURLY_OPEN = 287,
     CURLY_CLOSE = 288,
     EQUAL = 289,
     UNEQUAL = 290,
     LESS = 291,
     GREATER = 292,
     LESS_OR_EQ = 293,
     GREATER_OR_EQ = 294,
     OR = 295,
     AND = 296,
     DASH = 297,
     COMMA = 298,
     NAF = 299,
     AT = 300,
     WCONS = 301,
     AMPERSAND = 302,
     VEL = 303,
     EXISTS = 304,
     SQL_SELECT = 305,
     SQL_FROM = 306,
     SQL_WHERE = 307,
     SQL_GROUP = 308,
     SQL_BY = 309,
     SQL_AS = 310,
     SQL_COPY = 311,
     SQL_TO = 312,
     SQL_ORDER = 313,
     SQL_ASC = 314,
     SQL_DESC = 315,
     SQL_LIMIT = 316,
     SQL_SUM = 317,
     SQL_MIN = 318,
     SQL_MAX = 319,
     SQL_AVG = 320,
     SQL_COUNT = 321,
     SQL_DIALECT = 322
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
#define ID 269
#define ERROR 270
#define NEWLINE 271
#define DOT 272
#define DDOT 273
#define SEMICOLON 274
#define COLON 275
#define CONS 276
#define QUERY_MARK 277
#define PLUS 278
#define TIMES 279
#define SLASH 280
#define BACK_SLASH 281
#define ANON_VAR 282
#define PARAM_OPEN 283
#define PARAM_CLOSE 284
#define SQUARE_OPEN 285
#define SQUARE_CLOSE 286
#define CURLY_OPEN 287
#define CURLY_CLOSE 288
#define EQUAL 289
#define UNEQUAL 290
#define LESS 291
#define GREATER 292
#define LESS_OR_EQ 293
#define GREATER_OR_EQ 294
#define OR 295
#define AND 296
#define DASH 297
#define COMMA 298
#define NAF 299
#define AT 300
#define WCONS 301
#define AMPERSAND 302
#define VEL 303
#define EXISTS 304
#define SQL_SELECT 305
#define SQL_FROM 306
#define SQL_WHERE 307
#define SQL_GROUP 308
#define SQL_BY 309
#define SQL_AS 310
#define SQL_COPY 311
#define SQL_TO 312
#define SQL_ORDER 313
#define SQL_ASC 314
#define SQL_DESC 315
#define SQL_LIMIT 316
#define SQL_SUM 317
#define SQL_MIN 318
#define SQL_MAX 319
#define SQL_AVG 320
#define SQL_COUNT 321
#define SQL_DIALECT 322




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 33 "src/parser/aspcore2.y"
{
    char* string;
    char single_char;
    int integer;
}
/* Line 1529 of yacc.c.  */
#line 189 "src/parser/aspcore2_parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

