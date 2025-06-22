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
     ERROR = 268,
     NEWLINE = 269,
     DOT = 270,
     DDOT = 271,
     SEMICOLON = 272,
     COLON = 273,
     CONS = 274,
     QUERY_MARK = 275,
     PLUS = 276,
     TIMES = 277,
     SLASH = 278,
     BACK_SLASH = 279,
     ANON_VAR = 280,
     PARAM_OPEN = 281,
     PARAM_CLOSE = 282,
     SQUARE_OPEN = 283,
     SQUARE_CLOSE = 284,
     CURLY_OPEN = 285,
     CURLY_CLOSE = 286,
     EQUAL = 287,
     UNEQUAL = 288,
     LESS = 289,
     GREATER = 290,
     LESS_OR_EQ = 291,
     GREATER_OR_EQ = 292,
     DASH = 293,
     COMMA = 294,
     NAF = 295,
     AT = 296,
     WCONS = 297,
     AMPERSAND = 298,
     VEL = 299,
     EXISTS = 300
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
#define ERROR 268
#define NEWLINE 269
#define DOT 270
#define DDOT 271
#define SEMICOLON 272
#define COLON 273
#define CONS 274
#define QUERY_MARK 275
#define PLUS 276
#define TIMES 277
#define SLASH 278
#define BACK_SLASH 279
#define ANON_VAR 280
#define PARAM_OPEN 281
#define PARAM_CLOSE 282
#define SQUARE_OPEN 283
#define SQUARE_CLOSE 284
#define CURLY_OPEN 285
#define CURLY_CLOSE 286
#define EQUAL 287
#define UNEQUAL 288
#define LESS 289
#define GREATER 290
#define LESS_OR_EQ 291
#define GREATER_OR_EQ 292
#define DASH 293
#define COMMA 294
#define NAF 295
#define AT 296
#define WCONS 297
#define AMPERSAND 298
#define VEL 299
#define EXISTS 300




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 33 "src/parser/aspcore2.y"
{
    char* string;
    char single_char;
    int integer;
}
/* Line 1529 of yacc.c.  */
#line 145 "src/parser/aspcore2_parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

