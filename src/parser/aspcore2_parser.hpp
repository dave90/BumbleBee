/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison GLR parsers in C

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

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

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0






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
     NULLKW = 284,
     IS_KW = 285,
     PARAM_OPEN = 286,
     PARAM_CLOSE = 287,
     SQUARE_OPEN = 288,
     SQUARE_CLOSE = 289,
     CURLY_OPEN = 290,
     CURLY_CLOSE = 291,
     EQUAL = 292,
     UNEQUAL = 293,
     LESS = 294,
     GREATER = 295,
     LESS_OR_EQ = 296,
     GREATER_OR_EQ = 297,
     OR = 298,
     AND = 299,
     DASH = 300,
     COMMA = 301,
     NAF = 302,
     AT = 303,
     WCONS = 304,
     AMPERSAND = 305,
     VEL = 306,
     EXISTS = 307,
     SQL_SELECT = 308,
     SQL_FROM = 309,
     SQL_WHERE = 310,
     SQL_GROUP = 311,
     SQL_BY = 312,
     SQL_AS = 313,
     SQL_COPY = 314,
     SQL_TO = 315,
     SQL_ORDER = 316,
     SQL_ASC = 317,
     SQL_DESC = 318,
     SQL_LIMIT = 319,
     SQL_LIKE = 320,
     SQL_IN = 321,
     SQL_NOT = 322,
     SQL_SUM = 323,
     SQL_MIN = 324,
     SQL_MAX = 325,
     SQL_AVG = 326,
     SQL_COUNT = 327,
     SQL_DIALECT = 328
   };
#endif


/* Copy the first part of user declarations.  */
#line 2 "src/parser/aspcore2.y"

//////////////////////////////////////////////////////////////////////////////
// aspcore2.y

//////////////////////////////////////////////////////////////////////////////
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

#include "bumblebee/parser/ParserInputDirector.hpp"
#include <iostream>

/* GLR mode causes bison to emit this preamble in both the .h and the .hpp files.
   Guard against the resulting redefinition when both headers are included together. */
#ifndef BUMBLEBEE_QUERY_FOUND_GUARD
#define BUMBLEBEE_QUERY_FOUND_GUARD
bool queryFound=false;
#endif




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 39 "src/parser/aspcore2.y"
{
    char* string;
    char single_char;
    int integer;
}
/* Line 204 of glr.c.  */
#line 185 "src/parser/aspcore2_parser.hpp"
	YYSTYPE;
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{

  char yydummy;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template,
   here we set the default value of $$ to a zeroed-out value.
   Since the default value is undefined, this behavior is
   technically correct.  */
static YYSTYPE yyval_default;

/* Copy the second part of user declarations.  */


/* Line 234 of glr.c.  */
#line 232 "src/parser/aspcore2_parser.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#define YYSIZEMAX ((size_t) -1)

#ifdef __cplusplus
   typedef bool yybool;
#else
   typedef unsigned char yybool;
#endif
#define yytrue 1
#define yyfalse 0

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(env) setjmp (env)
# define YYLONGJMP(env, val) longjmp (env, val)
#endif

/*-----------------.
| GCC extensions.  |
`-----------------*/

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__)
#  define __attribute__(Spec) /* empty */
# endif
#endif


#ifdef __cplusplus
# define YYOPTIONAL_LOC(Name) /* empty */
#else
# define YYOPTIONAL_LOC(Name) Name __attribute__ ((__unused__))
#endif

#ifndef YYASSERT
# define YYASSERT(condition) ((void) ((condition) || (abort (), 0)))
#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  96
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   643

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  122
/* YYNRULES -- Number of rules.  */
#define YYNRULES  274
/* YYNRULES -- Number of states.  */
#define YYNSTATES  454
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 9
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

#define YYTRANSLATE(YYX)						\
  ((YYX <= 0) ? YYEOF :							\
   (unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    17,    19,
      21,    24,    28,    33,    37,    42,    44,    46,    48,    50,
      54,    59,    62,    67,    70,    72,    74,    78,    80,    84,
      86,    90,    96,   101,   105,   110,   113,   116,   120,   122,
     124,   128,   130,   132,   136,   138,   142,   144,   147,   149,
     151,   154,   156,   158,   160,   164,   166,   169,   171,   176,
     180,   190,   199,   201,   202,   204,   208,   212,   214,   218,
     220,   224,   226,   230,   234,   238,   243,   245,   247,   249,
     251,   253,   255,   257,   259,   261,   263,   265,   269,   272,
     274,   276,   278,   280,   284,   286,   288,   290,   294,   296,
     300,   302,   304,   306,   308,   310,   312,   314,   317,   319,
     321,   326,   329,   333,   335,   337,   341,   343,   345,   347,
     351,   353,   355,   357,   360,   363,   366,   369,   372,   375,
     378,   381,   383,   385,   388,   391,   394,   397,   400,   403,
     405,   407,   409,   412,   415,   421,   426,   430,   434,   436,
     438,   440,   442,   447,   451,   453,   457,   459,   463,   465,
     469,   475,   477,   479,   481,   483,   485,   487,   489,   492,
     498,   505,   512,   520,   528,   531,   535,   537,   539,   541,
     543,   545,   549,   552,   555,   557,   561,   564,   567,   572,
     574,   578,   583,   585,   589,   597,   599,   601,   603,   604,
     607,   609,   611,   613,   615,   617,   619,   621,   622,   625,
     627,   631,   635,   637,   641,   643,   649,   655,   662,   668,
     675,   679,   684,   686,   690,   692,   696,   700,   702,   703,
     707,   709,   713,   714,   718,   720,   724,   727,   729,   731,
     733,   735,   737,   739,   740,   742,   744,   745,   748,   750,
     752,   756,   760,   762,   764,   768,   772,   774,   776,   778,
     779,   784,   789,   791,   793,   795,   797,   801,   805,   809,
     813,   815,   817,   819,   821
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      76,     0,    -1,    51,    -1,    73,   153,    -1,    77,    -1,
       1,    -1,    78,    -1,    77,    78,    -1,    79,    -1,   126,
      -1,    80,    18,    -1,    80,    22,    18,    -1,    80,    22,
      81,    18,    -1,    22,    81,    18,    -1,    49,    81,    18,
      82,    -1,   127,    -1,    84,    -1,    86,    -1,    85,    -1,
      33,   113,    34,    -1,    33,   113,    83,    34,    -1,    48,
     113,    -1,    48,   113,    46,   103,    -1,    46,   103,    -1,
      97,    -1,    99,    -1,    84,    75,    97,    -1,    96,    -1,
      84,    75,    96,    -1,    95,    -1,    85,    46,    95,    -1,
      87,    35,    89,    36,    88,    -1,    87,    35,    89,    36,
      -1,    35,    89,    36,    -1,    35,    89,    36,    88,    -1,
     113,   108,    -1,   108,   113,    -1,    89,    20,    90,    -1,
      90,    -1,    91,    -1,    91,    21,    92,    -1,    98,    -1,
      94,    -1,    92,    46,    94,    -1,    94,    -1,    93,    46,
      94,    -1,    97,    -1,    47,    97,    -1,   105,    -1,    99,
      -1,    47,    99,    -1,    94,    -1,   140,    -1,   139,    -1,
      52,   124,    98,    -1,    98,    -1,    45,    98,    -1,   125,
      -1,   125,    31,   103,    32,    -1,   125,    31,    32,    -1,
      50,   125,    31,   103,   100,   101,   100,   103,    32,    -1,
      50,   125,    31,   100,   101,   100,   103,    32,    -1,    20,
      -1,    -1,   102,    -1,   101,    46,   102,    -1,   113,    37,
     113,    -1,   113,    -1,   103,    46,   113,    -1,   116,    -1,
     104,    46,   116,    -1,   106,    -1,   105,    43,   106,    -1,
     113,   108,   113,    -1,   113,    30,    29,    -1,   113,    30,
      47,    29,    -1,    37,    -1,    38,    -1,   107,    -1,   143,
      -1,   144,    -1,    24,    -1,    45,    -1,    25,    -1,    26,
      -1,    27,    -1,    28,    -1,     4,    19,     4,    -1,    45,
     113,    -1,   120,    -1,   125,    -1,     4,    -1,    29,    -1,
      31,   113,    32,    -1,   111,    -1,   114,    -1,   115,    -1,
     114,   109,   115,    -1,   112,    -1,   115,   110,   112,    -1,
     117,    -1,   118,    -1,   119,    -1,    15,    -1,     3,    -1,
       6,    -1,     4,    -1,    45,     4,    -1,     5,    -1,    28,
      -1,   125,    31,   103,    32,    -1,    33,    34,    -1,    33,
     121,    34,    -1,   103,    -1,   122,    -1,   113,    51,   123,
      -1,     5,    -1,   120,    -1,     5,    -1,   124,    46,     5,
      -1,     3,    -1,     6,    -1,     5,    -1,     7,     8,    -1,
      98,    23,    -1,   113,   107,    -1,   107,   113,    -1,   128,
     145,    -1,   145,   129,    -1,   113,   143,    -1,   131,   145,
      -1,   132,    -1,   135,    -1,   113,   144,    -1,   134,   145,
      -1,   143,   113,    -1,   144,   113,    -1,   145,   136,    -1,
     145,   137,    -1,   133,    -1,   138,    -1,   130,    -1,   132,
     136,    -1,   135,   137,    -1,   141,   146,    35,   149,    36,
      -1,    31,   142,    32,    37,    -1,     5,    46,     5,    -1,
     142,    46,     5,    -1,    39,    -1,    41,    -1,    40,    -1,
      42,    -1,   152,    35,   149,    36,    -1,    14,   147,    34,
      -1,   148,    -1,   147,    46,   148,    -1,   125,    -1,   149,
      20,   150,    -1,   150,    -1,   104,    21,    93,    -1,   104,
     151,   104,    21,    93,    -1,    20,    -1,     9,    -1,    10,
      -1,    11,    -1,    12,    -1,    13,    -1,   154,    -1,   154,
      20,    -1,    31,   154,    32,    58,   170,    -1,    31,   154,
      32,    58,   170,    20,    -1,    59,    31,   154,    32,   155,
     156,    -1,    59,    31,   154,    32,   155,   156,    20,    -1,
     157,   158,   162,   171,   179,   181,   186,    -1,    60,     6,
      -1,    31,   101,    32,    -1,    53,    -1,   159,    -1,    25,
      -1,   160,    -1,   161,    -1,   160,    46,   161,    -1,   187,
     169,    -1,    54,   163,    -1,   164,    -1,   163,    46,   164,
      -1,   167,   169,    -1,   165,   169,    -1,    31,   153,    32,
     169,    -1,   168,    -1,   168,    26,     4,    -1,   168,    31,
     166,    32,    -1,   125,    -1,   166,    46,   125,    -1,    50,
     125,    31,   103,   100,   101,    32,    -1,     6,    -1,     3,
      -1,     5,    -1,    -1,    58,   170,    -1,     3,    -1,     5,
      -1,    72,    -1,    71,    -1,    70,    -1,    69,    -1,    68,
      -1,    -1,    55,   172,    -1,   174,    -1,   172,    44,   174,
      -1,   172,    43,   174,    -1,   187,    -1,   173,    46,   187,
      -1,   177,    -1,   178,   108,    31,   154,    32,    -1,   178,
      66,    31,   173,    32,    -1,   178,    67,    66,    31,   173,
      32,    -1,   178,    66,    31,   154,    32,    -1,   178,    67,
      66,    31,   154,    32,    -1,   178,    30,    29,    -1,   178,
      30,    67,    29,    -1,   175,    -1,   176,   172,    32,    -1,
      31,    -1,   178,   108,   187,    -1,   178,    65,   187,    -1,
     187,    -1,    -1,    56,    57,   180,    -1,   187,    -1,   180,
      46,   187,    -1,    -1,    61,    57,   182,    -1,   183,    -1,
     182,    46,   183,    -1,   184,   185,    -1,   194,    -1,    68,
      -1,    69,    -1,    70,    -1,    71,    -1,    72,    -1,    -1,
      62,    -1,    63,    -1,    -1,    64,     4,    -1,   188,    -1,
     189,    -1,   188,    24,   189,    -1,   188,    45,   189,    -1,
     190,    -1,   191,    -1,   190,    25,   191,    -1,   190,    26,
     191,    -1,     4,    -1,     6,    -1,   194,    -1,    -1,    31,
     192,   187,    32,    -1,   195,    31,   193,    32,    -1,   187,
      -1,    25,    -1,     3,    -1,     5,    -1,     3,    18,     3,
      -1,     5,    18,     3,    -1,     5,    18,     5,    -1,     3,
      18,     5,    -1,    68,    -1,    69,    -1,    70,    -1,    71,
      -1,    72,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    90,    90,    94,    95,    96,   101,   102,   106,   107,
     111,   115,   119,   123,   127,   131,   139,   143,   151,   158,
     163,   171,   176,   180,   190,   194,   199,   203,   207,   214,
     218,   225,   226,   227,   228,   232,   239,   246,   247,   251,
     255,   262,   269,   273,   280,   284,   291,   295,   299,   304,
     308,   315,   316,   317,   324,   330,   331,   335,   340,   345,
     354,   359,   367,   372,   374,   375,   379,   386,   387,   391,
     392,   396,   397,   401,   405,   409,   416,   420,   427,   428,
     429,   434,   435,   439,   440,   441,   445,   449,   455,   459,
     464,   469,   474,   478,   485,   486,   492,   493,   501,   502,
     508,   509,   510,   511,   519,   524,   529,   534,   542,   547,
     554,   562,   566,   573,   577,   584,   593,   598,   602,   607,
     615,   616,   617,   621,   630,   637,   644,   651,   652,   656,
     663,   667,   668,   672,   679,   683,   690,   697,   698,   702,
     703,   704,   705,   706,   710,   717,   721,   728,   736,   740,
     747,   751,   758,   762,   766,   767,   771,   779,   780,   785,
     789,   796,   803,   808,   813,   818,   823,   842,   843,   844,
     848,   852,   856,   863,   867,   875,   879,   886,   895,   904,
     908,   909,   913,   922,   931,   932,   936,   940,   944,   951,
     952,   957,   961,   966,   974,   979,   986,   991,   999,  1001,
    1005,  1010,  1015,  1020,  1025,  1030,  1035,  1043,  1045,  1052,
    1053,  1057,  1064,  1066,  1071,  1075,  1079,  1083,  1087,  1091,
    1095,  1099,  1103,  1107,  1114,  1122,  1126,  1133,  1140,  1142,
    1146,  1150,  1157,  1159,  1163,  1164,  1168,  1175,  1176,  1181,
    1186,  1191,  1196,  1204,  1207,  1212,  1219,  1221,  1229,  1233,
    1234,  1238,  1245,  1252,  1253,  1257,  1265,  1270,  1275,  1276,
    1276,  1277,  1281,  1282,  1290,  1295,  1300,  1306,  1312,  1318,
    1329,  1334,  1339,  1344,  1349
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SYMBOLIC_CONSTANT", "NUMBER",
  "VARIABLE", "STRING", "DIRECTIVE_NAME", "DIRECTIVE_VALUE", "AGGR_COUNT",
  "AGGR_MAX", "AGGR_MIN", "AGGR_SUM", "AGGR_AVG", "AGGR_MULTI", "ID",
  "ERROR", "NEWLINE", "DOT", "DDOT", "SEMICOLON", "COLON", "CONS",
  "QUERY_MARK", "PLUS", "TIMES", "SLASH", "BACK_SLASH", "ANON_VAR",
  "NULLKW", "IS_KW", "PARAM_OPEN", "PARAM_CLOSE", "SQUARE_OPEN",
  "SQUARE_CLOSE", "CURLY_OPEN", "CURLY_CLOSE", "EQUAL", "UNEQUAL", "LESS",
  "GREATER", "LESS_OR_EQ", "GREATER_OR_EQ", "OR", "AND", "DASH", "COMMA",
  "NAF", "AT", "WCONS", "AMPERSAND", "VEL", "EXISTS", "SQL_SELECT",
  "SQL_FROM", "SQL_WHERE", "SQL_GROUP", "SQL_BY", "SQL_AS", "SQL_COPY",
  "SQL_TO", "SQL_ORDER", "SQL_ASC", "SQL_DESC", "SQL_LIMIT", "SQL_LIKE",
  "SQL_IN", "SQL_NOT", "SQL_SUM", "SQL_MIN", "SQL_MAX", "SQL_AVG",
  "SQL_COUNT", "SQL_DIALECT", "$accept", "HEAD_SEPARATOR", "program",
  "rules", "rule", "simple_rule", "head", "body", "weight_at_levels",
  "levels_and_terms", "disjunction", "conjunction", "choice_atom",
  "lower_guard", "upper_guard", "choice_elements", "choice_element",
  "choice_element_atom", "choice_elements_literals", "naf_literals",
  "naf_literal", "naf_literal_aggregate", "existential_atom",
  "classic_literal", "atom", "ext_atom", "ext_semicol", "named_parameters",
  "namedParameter", "terms", "basic_terms", "builtin_or_list",
  "builtin_atom", "compareop", "binop", "arithop1", "arithop2", "term_",
  "term__", "term", "expr", "factor", "basic_term", "ground_term",
  "variable_term", "functional_term", "list_term", "list_terms_term",
  "list_head_tail_term", "list_tail_term", "vars", "identifier",
  "directive", "query", "lower_guard_compare_aggregate",
  "upper_guard_compare_aggregate", "compare_aggregate",
  "lower_guard_leftward_left_aggregate", "leftward_left_aggregate",
  "left_aggregate", "lower_guard_rightward_left_aggregate",
  "rightward_left_aggregate", "upper_guard_leftward_right_aggregate",
  "upper_guard_rightward_right_aggregate", "right_aggregate",
  "single_aggregate_atom", "multi_assign_aggregate", "multi_assign_guard",
  "multi_assign_vars", "leftwardop", "rightwardop", "aggregate",
  "multi_aggregate_function", "aggregate_function_list",
  "aggregate_function_item_generic", "aggregate_elements",
  "aggregate_element", "agg_group_semicol", "aggregate_function", "sql",
  "sql_query", "sql_copy_to", "sql_copy_params", "sql_select", "select",
  "select_list", "select_items", "select_item", "from", "from_list",
  "from_item", "predicate_ref", "predicate_col_list", "external_table",
  "table_ref", "opt_alias", "alias_name", "opt_where", "search_condition",
  "in_value_list", "search_atom", "where_group", "begin_group",
  "predicate", "predicate_value_expr", "opt_groupby", "group_list",
  "opt_orderby", "order_list", "order_col", "order_col_name",
  "modifier_opt", "opt_limit", "value_expr", "value_add_expr",
  "finalized_mul_expr", "value_mul_expr", "value_primary", "@1",
  "aggregate_arg", "qualified_name", "aggregate_func", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    74,    75,    76,    76,    76,    77,    77,    78,    78,
      79,    79,    79,    79,    79,    79,    80,    80,    81,    82,
      82,    83,    83,    83,    84,    84,    84,    84,    84,    85,
      85,    86,    86,    86,    86,    87,    88,    89,    89,    90,
      90,    91,    92,    92,    93,    93,    94,    94,    94,    94,
      94,    95,    95,    95,    96,    97,    97,    98,    98,    98,
      99,    99,   100,   101,   101,   101,   102,   103,   103,   104,
     104,   105,   105,   106,   106,   106,   107,   107,   108,   108,
     108,   109,   109,   110,   110,   110,   111,   111,   111,   111,
     112,   112,   112,   112,   113,   113,   114,   114,   115,   115,
     116,   116,   116,   116,   117,   117,   117,   117,   118,   118,
     119,   120,   120,   121,   121,   122,   123,   123,   124,   124,
     125,   125,   125,   126,   127,   128,   129,   130,   130,   131,
     132,   133,   133,   134,   135,   136,   137,   138,   138,   139,
     139,   139,   139,   139,   140,   141,   142,   142,   143,   143,
     144,   144,   145,   146,   147,   147,   148,   149,   149,   150,
     150,   151,   152,   152,   152,   152,   152,   153,   153,   153,
     153,   153,   153,   154,   155,   156,   157,   158,   159,   159,
     160,   160,   161,   162,   163,   163,   164,   164,   164,   165,
     165,   165,   166,   166,   167,   167,   168,   168,   169,   169,
     170,   170,   170,   170,   170,   170,   170,   171,   171,   172,
     172,   172,   173,   173,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   175,   176,   177,   177,   178,   179,   179,
     180,   180,   181,   181,   182,   182,   183,   184,   184,   184,
     184,   184,   184,   185,   185,   185,   186,   186,   187,   188,
     188,   188,   189,   190,   190,   190,   191,   191,   191,   192,
     191,   191,   193,   193,   194,   194,   194,   194,   194,   194,
     195,   195,   195,   195,   195
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     2,     1,     1,
       2,     3,     4,     3,     4,     1,     1,     1,     1,     3,
       4,     2,     4,     2,     1,     1,     3,     1,     3,     1,
       3,     5,     4,     3,     4,     2,     2,     3,     1,     1,
       3,     1,     1,     3,     1,     3,     1,     2,     1,     1,
       2,     1,     1,     1,     3,     1,     2,     1,     4,     3,
       9,     8,     1,     0,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     3,     3,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     1,
       1,     1,     1,     3,     1,     1,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       4,     2,     3,     1,     1,     3,     1,     1,     1,     3,
       1,     1,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     1,     2,     2,     2,     2,     2,     2,     1,
       1,     1,     2,     2,     5,     4,     3,     3,     1,     1,
       1,     1,     4,     3,     1,     3,     1,     3,     1,     3,
       5,     1,     1,     1,     1,     1,     1,     1,     2,     5,
       6,     6,     7,     7,     2,     3,     1,     1,     1,     1,
       1,     3,     2,     2,     1,     3,     2,     2,     4,     1,
       3,     4,     1,     3,     7,     1,     1,     1,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     1,
       3,     3,     1,     3,     1,     5,     5,     6,     5,     6,
       3,     4,     1,     3,     1,     3,     3,     1,     0,     3,
       1,     3,     0,     3,     1,     3,     2,     1,     1,     1,
       1,     1,     1,     0,     1,     1,     0,     2,     1,     1,
       3,     3,     1,     1,     3,     3,     1,     1,     1,     0,
       4,     4,     1,     1,     1,     1,     3,     3,     3,     3,
       1,     1,     1,     1,     1
};

/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const unsigned char yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     5,   120,    91,   122,   121,     0,     0,    86,    92,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       6,     8,     0,    16,    17,     0,    27,    24,    55,    25,
      94,    98,     0,    95,    96,    89,    90,     9,    15,     0,
     123,   162,   163,   164,   165,   166,     0,     0,     0,    18,
      51,    29,    46,    55,    49,    48,    71,     0,     0,   141,
       0,   131,   139,     0,   132,   140,    53,    52,     0,     0,
       0,     0,     0,    90,   111,   113,    67,     0,   114,     0,
      38,    39,    41,    57,    56,    88,     0,     0,   118,     0,
       0,   176,     0,     3,   167,     0,     1,     7,    10,     0,
       2,     0,     0,   124,    76,    77,   148,   150,   149,   151,
      78,    35,    79,    80,    81,    82,     0,    83,    84,    85,
       0,     0,    87,   122,     0,     0,    47,    50,    13,     0,
       0,     0,    78,     0,    79,    80,   127,   130,   142,     0,
     134,   143,     0,     0,     0,     0,   128,   137,   138,     0,
      93,     0,     0,   112,     0,    33,     0,     0,     0,     0,
      54,     0,     0,   168,   264,   256,   265,   257,   178,   259,
     270,   271,   272,   273,   274,     0,   177,   179,   180,   198,
     248,   249,   252,   253,   258,     0,    11,     0,    28,    26,
       0,    91,    97,    99,    59,     0,    67,     0,     0,     0,
      30,    72,     0,    74,     0,    73,   135,   136,   156,     0,
     154,     0,   126,   104,   106,   108,   105,   103,   109,     0,
       0,    69,   100,   101,   102,     0,     0,   158,    68,   116,
     117,   115,    37,    34,     0,    40,    42,     0,    14,    62,
      63,     0,   119,     0,     0,     0,     0,     0,     0,   207,
       0,     0,   182,     0,     0,     0,     0,     0,    12,    32,
      58,   146,   145,   147,    75,   153,     0,     0,   107,   161,
       0,     0,     0,     0,     0,   152,    36,     0,     0,     0,
      64,     0,    63,     0,     0,   266,   269,   267,   268,     0,
     196,   197,   195,     0,     0,   183,   184,   198,   198,   189,
       0,   228,   181,   200,   201,   206,   205,   204,   203,   202,
     199,   250,   251,   254,   255,   263,   262,     0,    31,   155,
     144,   159,    44,    70,     0,     0,   157,    43,    19,     0,
       0,     0,     0,     0,     0,     0,   169,     0,     0,   260,
       0,     0,     0,   187,   186,     0,     0,   224,   208,   209,
     222,     0,   214,     0,   227,     0,   232,   261,     0,     0,
     110,    23,    21,    20,    65,     0,    66,     0,   170,   174,
      63,   171,   198,     0,   185,   190,   192,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   246,    45,
     160,     0,    61,     0,     0,   172,   188,     0,   191,     0,
     211,   210,   223,   220,     0,   226,     0,     0,   259,   225,
     229,   230,     0,     0,   173,    22,    60,   175,    63,   193,
     221,     0,     0,   212,     0,     0,     0,   238,   239,   240,
     241,   242,   233,   234,   243,   237,   247,     0,   218,   216,
       0,     0,     0,   215,   231,     0,   244,   245,   236,   194,
     213,   219,   217,   235
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,   101,    18,    19,    20,    21,    22,    48,   238,   331,
      23,    49,    24,    25,   233,    79,    80,    81,   235,   321,
      50,    51,    26,    52,    53,    54,   240,   279,   280,    75,
     220,    55,    56,   110,   133,   116,   120,    30,    31,   196,
      33,    34,   221,   222,   223,   224,    35,    77,    78,   231,
      89,    73,    37,    38,    58,   146,    59,    60,    61,    62,
      63,    64,   138,   141,    65,    66,    67,    68,   124,   112,
     113,    69,   144,   209,   210,   226,   227,   272,    70,    93,
      94,   338,   371,    95,   175,   176,   177,   178,   249,   295,
     296,   297,   377,   298,   299,   252,   310,   301,   348,   422,
     349,   350,   351,   352,   353,   356,   410,   388,   432,   433,
     434,   448,   414,   354,   180,   181,   182,   183,   247,   317,
     184,   185
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -393
static const short int yypact[] =
{
     183,  -393,  -393,    69,  -393,  -393,    41,   456,  -393,  -393,
     563,   510,   309,   563,   456,   309,    89,    11,   121,   448,
    -393,  -393,   233,   115,  -393,   186,  -393,  -393,   174,  -393,
    -393,  -393,   323,    92,   318,  -393,   592,  -393,  -393,   222,
    -393,  -393,  -393,  -393,  -393,  -393,   576,    45,   195,   190,
    -393,  -393,  -393,  -393,  -393,   241,  -393,   267,   401,  -393,
     401,    -7,  -393,   401,   160,  -393,  -393,  -393,   215,   323,
     252,   563,   284,  -393,  -393,   245,   278,   307,  -393,    21,
    -393,   325,  -393,   287,  -393,  -393,   333,   321,  -393,   109,
     301,  -393,   326,  -393,   336,   173,  -393,  -393,  -393,    56,
    -393,    30,   309,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,   259,  -393,  -393,  -393,
     259,   542,  -393,   320,    -6,   309,  -393,  -393,  -393,   456,
     563,   106,   413,   563,   462,   508,  -393,  -393,  -393,   563,
    -393,  -393,   563,   309,   334,   563,  -393,  -393,  -393,   165,
    -393,   563,    12,  -393,   309,   323,   504,   347,   557,   390,
    -393,   364,   301,  -393,   379,  -393,   380,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,   346,  -393,   355,  -393,   344,
      95,  -393,   166,  -393,  -393,   372,  -393,   387,  -393,  -393,
      66,  -393,   318,  -393,  -393,    -3,  -393,   402,   369,   410,
    -393,  -393,   267,  -393,   388,  -393,  -393,  -393,  -393,   127,
    -393,   165,  -393,   385,  -393,   389,   396,  -393,  -393,   415,
      71,  -393,  -393,  -393,  -393,   403,   108,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,   563,   417,  -393,   563,  -393,  -393,
     563,    34,  -393,   377,   432,   273,   297,   268,    73,   423,
     268,    78,  -393,   268,   268,   268,   268,   317,  -393,   323,
    -393,  -393,  -393,  -393,  -393,  -393,   309,   139,  -393,  -393,
     504,   165,   165,   563,   165,  -393,  -393,   504,   148,    76,
    -393,   443,   563,    78,   422,  -393,  -393,  -393,  -393,   454,
    -393,  -393,  -393,    11,   309,   442,  -393,   344,   344,   101,
     368,   434,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,   459,  -393,  -393,
    -393,   446,  -393,  -393,    26,   128,  -393,  -393,  -393,   563,
     563,   460,   563,   563,   563,    76,   475,   490,   468,  -393,
     470,   473,    73,  -393,  -393,   501,   309,   373,   209,  -393,
    -393,   368,  -393,   391,  -393,   455,   450,  -393,   504,   504,
    -393,   245,   476,  -393,  -393,   153,  -393,   563,  -393,  -393,
     563,   503,   344,   563,  -393,  -393,  -393,   163,   368,   368,
     225,    -4,   268,   493,   461,   378,   268,   469,   464,  -393,
     446,   563,  -393,   169,   171,  -393,  -393,    34,  -393,   309,
    -393,  -393,  -393,  -393,   496,  -393,   322,   498,   301,  -393,
     484,  -393,   178,   527,  -393,   245,  -393,  -393,   563,  -393,
    -393,   502,   176,  -393,   322,   518,   268,  -393,  -393,  -393,
    -393,  -393,   494,  -393,   232,  -393,  -393,   192,  -393,  -393,
     268,   520,   193,  -393,  -393,   178,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -393,  -393,  -393,  -393,   517,  -393,  -393,    -2,  -393,  -393,
    -393,  -393,  -393,  -393,   283,   451,   404,  -393,  -393,   198,
    -152,   427,   458,     8,    18,    58,  -235,  -277,   240,  -106,
     292,  -393,   435,   162,   -29,  -393,  -393,  -393,   463,     0,
    -393,   477,   305,  -393,  -393,  -393,   426,  -393,  -393,  -393,
    -393,     9,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,   515,   526,  -393,  -393,  -393,  -393,  -393,   197,
      67,   238,  -393,  -393,   331,   392,   315,  -393,  -393,   306,
     -89,  -393,  -393,  -393,  -393,  -393,  -393,   348,  -393,  -393,
     258,  -393,  -393,  -393,  -393,  -259,   328,  -393,   250,   182,
    -119,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,   168,
    -393,  -393,  -393,   -93,  -393,    57,  -393,    94,  -393,  -393,
    -392,  -393
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -225
static const short int yytable[] =
{
      32,   161,   179,   111,   236,   335,   282,    57,    27,    36,
      72,    76,    86,    85,    57,   195,    36,   229,    28,    32,
     435,    83,    36,    36,    87,   403,   198,    27,    36,   260,
      82,    84,   106,     2,   108,     4,     5,    28,   343,   344,
     199,   154,    90,   151,   333,    11,    72,   359,     2,    40,
       4,     5,   241,   435,   239,   126,    83,   155,    29,     2,
       3,     4,     5,   404,    91,    41,    42,    43,    44,    45,
      92,    85,   271,   244,   186,   125,   290,    29,   291,   292,
     151,   303,    16,   304,     8,     9,   154,    46,    39,    11,
     125,   269,   270,   394,    88,    15,   239,   187,    83,    57,
     367,    13,   259,    47,   293,   127,    15,   160,    36,   189,
      83,    83,     2,   396,     4,     5,   114,   271,   322,   253,
      82,    96,   332,   294,   135,   327,   234,   345,   274,    57,
     202,   142,   346,   205,    83,   203,   142,   115,    36,   206,
     254,   437,   207,    84,   275,   212,   305,   306,   307,   308,
     309,   228,   208,   204,   289,   159,   202,   179,   225,   274,
     360,   265,   418,    83,   316,    36,   100,   325,   213,   214,
     215,   216,    82,   266,   151,   320,   164,   165,   166,   167,
     217,   164,   328,   166,     1,   392,     2,     3,     4,     5,
       6,   255,   256,   218,   329,   398,   330,   103,   168,   151,
     107,   416,   109,   417,   169,     7,   389,   322,   439,   399,
     219,     8,     9,   128,    10,   151,    11,   332,    12,   132,
     225,   102,   440,   361,   449,   452,   122,   365,    13,   143,
     234,   145,    14,    15,   276,    16,   129,   278,   332,   440,
     281,   170,   171,   172,   173,   174,   427,   428,   429,   430,
     431,    98,   378,   379,   134,    99,    17,   402,   139,   400,
     401,   393,     2,   191,     4,     5,   139,   397,   378,   379,
     202,   164,   165,   166,   167,   208,   285,   202,   286,    36,
     225,   225,   281,   225,   130,   415,    36,   149,     9,   405,
      10,   151,   409,   411,   446,   447,   136,   131,   137,   169,
     287,   140,   288,   341,   104,   105,   106,   107,   108,   109,
     311,   312,     2,   423,     4,     5,   150,   421,   121,   425,
     164,   165,   166,   167,   385,   164,   165,   166,   167,   152,
     362,   423,   281,   444,   366,   441,   170,   171,   172,   173,
     174,   153,   315,   117,   118,   119,   156,   450,   169,   313,
     314,   157,   158,   169,    91,   376,   163,   162,   202,   202,
     104,   105,   106,   107,   108,   109,   197,    36,    36,   211,
     281,   164,   165,   166,   167,    91,  -224,  -224,  -224,  -224,
     237,   164,   165,   166,   167,   170,   171,   172,   173,   174,
     170,   171,   172,   173,   174,   242,   243,   245,   246,   347,
     248,   250,   251,   257,  -224,   258,   262,   261,   419,   408,
      41,    42,    43,    44,    45,   263,  -120,   264,   281,   268,
    -122,   381,  -125,  -125,  -125,  -125,  -125,  -121,   104,   105,
     106,   107,   108,   109,   273,   283,   170,   171,   172,   173,
     174,  -224,  -224,  -224,  -224,  -224,   170,   171,   172,   173,
     174,     2,     3,     4,     5,     6,   382,   383,   384,     2,
       3,     4,     5,   277,   284,    41,    42,    43,    44,    45,
       7,  -129,  -129,  -129,  -129,  -129,     8,     9,   300,    10,
     334,    11,   337,    12,     8,     9,   339,    46,   342,    11,
     355,   357,   358,    13,   363,   368,   369,    14,    15,   370,
      16,    13,   372,    47,   373,   375,    15,     2,     3,     4,
       5,   387,   386,     2,     3,     4,     5,  -133,  -133,  -133,
    -133,  -133,   391,   395,   406,   420,   412,   407,   413,   424,
     426,   436,     8,     9,   438,    10,    97,    11,     8,     9,
     445,    10,   318,    11,    74,     2,     3,     4,     5,    13,
     443,    47,   451,   190,    15,    71,   200,   390,   232,   188,
       2,     3,     4,     5,   324,   201,     2,     3,     4,     5,
       8,     9,   364,    10,   194,    11,   323,   239,   230,     2,
       3,   123,     5,   193,   147,     8,     9,    71,    10,   326,
      11,     8,     9,   192,    10,   148,    11,   319,   302,   340,
     374,   380,    71,   267,     8,     9,   442,    10,    71,    11,
     -57,   336,   -57,   453,   -57,   -57,     0,     0,     0,     0,
       0,    71,     0,   121,     0,     0,     0,     0,   -57,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   -57,     0,
       0,     0,     0,   -57
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned char yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     3,     5,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     9,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    11,    13,    15,    17,    19,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   259,     0,   259,     0,   259,     0,   259,     0,   259,
       0,   259,     0,   259,     0,   259,     0,   259,     0,   259,
       0
};

static const short int yycheck[] =
{
       0,    90,    95,    32,   156,   282,   241,     7,     0,     0,
      10,    11,    14,    13,    14,   121,     7,     5,     0,    19,
     412,    12,    13,    14,    15,    29,    32,    19,    19,    32,
      12,    13,    39,     3,    41,     5,     6,    19,   297,   298,
      46,    20,    31,    46,   279,    33,    46,    21,     3,     8,
       5,     6,   158,   445,    20,    47,    47,    36,     0,     3,
       4,     5,     6,    67,    53,     9,    10,    11,    12,    13,
      59,    71,    46,   162,    18,    45,     3,    19,     5,     6,
      46,     3,    52,     5,    28,    29,    20,    31,    19,    33,
      45,    20,    21,   370,     5,    50,    20,    99,    89,    99,
     335,    45,    36,    47,    31,    47,    50,    89,    99,   101,
     101,   102,     3,   372,     5,     6,    24,    46,   270,    24,
     102,     0,    46,    50,    57,   277,   155,    26,    20,   129,
     130,    64,    31,   133,   125,    29,    69,    45,   129,   139,
      45,   418,   142,   125,    36,   145,    68,    69,    70,    71,
      72,   151,   143,    47,   247,    46,   156,   250,   149,    20,
      32,    34,   397,   154,   257,   156,    51,   273,     3,     4,
       5,     6,   154,    46,    46,    36,     3,     4,     5,     6,
      15,     3,    34,     5,     1,    32,     3,     4,     5,     6,
       7,    25,    26,    28,    46,    32,    48,    23,    25,    46,
      40,    32,    42,    32,    31,    22,   358,   359,    32,    46,
      45,    28,    29,    18,    31,    46,    33,    46,    35,    57,
     211,    35,    46,   329,    32,    32,     4,   333,    45,    14,
     259,    69,    49,    50,   234,    52,    46,   237,    46,    46,
     240,    68,    69,    70,    71,    72,    68,    69,    70,    71,
      72,    18,    43,    44,    57,    22,    73,    32,    61,   378,
     379,   367,     3,     4,     5,     6,    69,   373,    43,    44,
     270,     3,     4,     5,     6,   266,     3,   277,     5,   270,
     271,   272,   282,   274,    43,   391,   277,    35,    29,   382,
      31,    46,   385,   386,    62,    63,    58,    30,    60,    31,
       3,    63,     5,   294,    37,    38,    39,    40,    41,    42,
     253,   254,     3,   406,     5,     6,    32,   406,    31,   408,
       3,     4,     5,     6,   353,     3,     4,     5,     6,    51,
     330,   424,   332,   426,   334,   424,    68,    69,    70,    71,
      72,    34,    25,    25,    26,    27,    21,   440,    31,   255,
     256,    18,    31,    31,    53,   346,    20,    31,   358,   359,
      37,    38,    39,    40,    41,    42,    46,   358,   359,    35,
     370,     3,     4,     5,     6,    53,     3,     4,     5,     6,
      33,     3,     4,     5,     6,    68,    69,    70,    71,    72,
      68,    69,    70,    71,    72,     5,    32,    18,    18,    31,
      54,    46,    58,    31,    31,    18,    37,     5,   399,    31,
       9,    10,    11,    12,    13,     5,    31,    29,   418,     4,
      31,    30,     9,    10,    11,    12,    13,    31,    37,    38,
      39,    40,    41,    42,    31,    58,    68,    69,    70,    71,
      72,    68,    69,    70,    71,    72,    68,    69,    70,    71,
      72,     3,     4,     5,     6,     7,    65,    66,    67,     3,
       4,     5,     6,    46,    32,     9,    10,    11,    12,    13,
      22,     9,    10,    11,    12,    13,    28,    29,    55,    31,
      37,    33,    60,    35,    28,    29,    32,    31,    46,    33,
      56,    32,    46,    45,    34,    20,     6,    49,    50,    31,
      52,    45,    32,    47,    31,     4,    50,     3,     4,     5,
       6,    61,    57,     3,     4,     5,     6,     9,    10,    11,
      12,    13,    46,    20,    31,    29,    57,    66,    64,    31,
      46,     4,    28,    29,    32,    31,    19,    33,    28,    29,
      46,    31,   259,    33,    34,     3,     4,     5,     6,    45,
      32,    47,    32,   102,    50,    45,   129,   359,   154,   101,
       3,     4,     5,     6,   272,   130,     3,     4,     5,     6,
      28,    29,   332,    31,    32,    33,   271,    20,   152,     3,
       4,     5,     6,   120,    69,    28,    29,    45,    31,   274,
      33,    28,    29,   116,    31,    69,    33,   266,   250,   293,
     342,   351,    45,   211,    28,    29,   424,    31,    45,    33,
      18,   283,    20,   445,    22,    23,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    22,    28,    29,
      31,    33,    35,    45,    49,    50,    52,    73,    76,    77,
      78,    79,    80,    84,    86,    87,    96,    97,    98,    99,
     111,   112,   113,   114,   115,   120,   125,   126,   127,    19,
       8,     9,    10,    11,    12,    13,    31,    47,    81,    85,
      94,    95,    97,    98,    99,   105,   106,   113,   128,   130,
     131,   132,   133,   134,   135,   138,   139,   140,   141,   145,
     152,    45,   113,   125,    34,   103,   113,   121,   122,    89,
      90,    91,    98,   125,    98,   113,    81,   125,     5,   124,
      31,    53,    59,   153,   154,   157,     0,    78,    18,    22,
      51,    75,    35,    23,    37,    38,    39,    40,    41,    42,
     107,   108,   143,   144,    24,    45,   109,    25,    26,    27,
     110,    31,     4,     5,   142,    45,    97,    99,    18,    46,
      43,    30,   107,   108,   143,   144,   145,   145,   136,   143,
     145,   137,   144,    14,   146,   107,   129,   136,   137,    35,
      32,    46,    51,    34,    20,    36,    21,    18,    31,    46,
      98,   154,    31,    20,     3,     4,     5,     6,    25,    31,
      68,    69,    70,    71,    72,   158,   159,   160,   161,   187,
     188,   189,   190,   191,   194,   195,    18,    81,    96,    97,
      89,     4,   115,   112,    32,   103,   113,    46,    32,    46,
      95,   106,   113,    29,    47,   113,   113,   113,   125,   147,
     148,    35,   113,     3,     4,     5,     6,    15,    28,    45,
     104,   116,   117,   118,   119,   125,   149,   150,   113,     5,
     120,   123,    90,    88,   108,    92,    94,    33,    82,    20,
     100,   103,     5,    32,   154,    18,    18,   192,    54,   162,
      46,    58,   169,    24,    45,    25,    26,    31,    18,    36,
      32,     5,    37,     5,    29,    34,    46,   149,     4,    20,
      21,    46,   151,    31,    20,    36,   113,    46,   113,   101,
     102,   113,   100,    58,    32,     3,     5,     3,     5,   187,
       3,     5,     6,    31,    50,   163,   164,   165,   167,   168,
      55,   171,   161,     3,     5,    68,    69,    70,    71,    72,
     170,   189,   189,   191,   191,    25,   187,   193,    88,   148,
      36,    93,    94,   116,   104,   103,   150,    94,    34,    46,
      48,    83,    46,   100,    37,   101,   170,    60,   155,    32,
     153,   125,    46,   169,   169,    26,    31,    31,   172,   174,
     175,   176,   177,   178,   187,    56,   179,    32,    46,    21,
      32,   103,   113,    34,   102,   103,   113,   100,    20,     6,
      31,   156,    32,    31,   164,     4,   125,   166,    43,    44,
     172,    30,    65,    66,    67,   108,    57,    61,   181,    94,
      93,    46,    32,   103,   101,    20,   169,   103,    32,    46,
     174,   174,    32,    29,    67,   187,    31,    66,    31,   187,
     180,   187,    57,    64,   186,   103,    32,    32,   100,   125,
      29,   154,   173,   187,    31,   154,    46,    68,    69,    70,
      71,    72,   182,   183,   184,   194,     4,   101,    32,    32,
      46,   154,   173,    32,   187,    46,    62,    63,   185,    32,
     187,    32,    32,   183
};


/* Prevent warning if -Wmissing-prototypes.  */
int yyparse (bumblebee::ParserInputDirector& director);

/* Error token number */
#define YYTERROR 1

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */


#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N) ((void) 0)
#endif


#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#define YYLEX yylex (, director)

YYSTYPE yylval;

YYLTYPE yylloc;

int yynerrs;
int yychar;

static const int YYEOF = 0;
static const int YYEMPTY = -2;

typedef enum { yyok, yyaccept, yyabort, yyerr } YYRESULTTAG;

#define YYCHK(YYE)							     \
   do { YYRESULTTAG yyflag = YYE; if (yyflag != yyok) return yyflag; }	     \
   while (YYID (0))

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, bumblebee::ParserInputDirector& director)
{
  if (!yyvaluep)
    return;
  YYUSE (director);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, bumblebee::ParserInputDirector& director)
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, director);
  YYFPRINTF (yyoutput, ")");
}

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			    \
do {									    \
  if (yydebug)								    \
    {									    \
      YYFPRINTF (stderr, "%s ", Title);					    \
      yy_symbol_print (stderr, Type,					    \
		       Value, director);  \
      YYFPRINTF (stderr, "\n");						    \
    }									    \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

#else /* !YYDEBUG */

# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)

#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
# if (! defined __cplusplus \
      || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL))
#  define YYSTACKEXPANDABLE 1
# else
#  define YYSTACKEXPANDABLE 0
# endif
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyexpandGLRStack (Yystack);			\
  } while (YYID (0))
#else
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyMemoryExhausted (Yystack);			\
  } while (YYID (0))
#endif


#if YYERROR_VERBOSE

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static size_t
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return strlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* !YYERROR_VERBOSE */

/** State numbers, as in LALR(1) machine */
typedef int yyStateNum;

/** Rule numbers, as in LALR(1) machine */
typedef int yyRuleNum;

/** Grammar symbol */
typedef short int yySymbol;

/** Item references, as in LALR(1) machine */
typedef short int yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState {
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yysval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yyStateNum yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the first token produced by my symbol */
  size_t yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  non-terminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yysval;
  } yysemantics;
  /** Source location for this state.  */
  YYLTYPE yyloc;
};

struct yyGLRStateSet {
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  size_t yysize, yycapacity;
};

struct yySemanticOption {
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  YYLTYPE yyloc;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;


  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  size_t yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

static void yyFail (yyGLRStack* yystackp, bumblebee::ParserInputDirector& director, const char* yymsg)
  __attribute__ ((__noreturn__));
static void
yyFail (yyGLRStack* yystackp, bumblebee::ParserInputDirector& director, const char* yymsg)
{
  if (yymsg != NULL)
    yyerror (director, yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

static void yyMemoryExhausted (yyGLRStack* yystackp)
  __attribute__ ((__noreturn__));
static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

#if YYDEBUG || YYERROR_VERBOSE
/** A printable representation of TOKEN.  */
static inline const char*
yytokenName (yySymbol yytoken)
{
  if (yytoken == YYEMPTY)
    return "";

  return yytname[yytoken];
}
#endif

/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) __attribute__ ((__unused__));
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  yyGLRState* s;
  int i;
  s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
      YYASSERT (s->yyresolved);
      yyvsp[i].yystate.yyresolved = yytrue;
      yyvsp[i].yystate.yysemantics.yysval = s->yysemantics.yysval;
      yyvsp[i].yystate.yyloc = s->yyloc;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     __attribute__ ((__unused__));
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT.  */
/*ARGSUSED*/ static YYRESULTTAG
yyuserAction (yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp,
	      YYSTYPE* yyvalp,
	      YYLTYPE* YYOPTIONAL_LOC (yylocp),
	      yyGLRStack* yystackp
	      , bumblebee::ParserInputDirector& director)
{
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  int yylow;
  YYUSE (director);
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, N, yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)						     \
  return yyerror (director, YY_("syntax error: cannot back up")),     \
	 yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yysval;
  YYLLOC_DEFAULT ((*yylocp), (yyvsp - yyrhslen), yyrhslen);

  switch (yyn)
    {
        case 3:
#line 94 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 4:
#line 95 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 5:
#line 96 "src/parser/aspcore2.y"
    { yyerror(director,"Generic error"); }
    break;

  case 10:
#line 112 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 11:
#line 116 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 12:
#line 120 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 13:
#line 124 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onConstraint();
        }
    break;

  case 14:
#line 128 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeakConstraint();
        }
    break;

  case 15:
#line 132 "src/parser/aspcore2.y"
    {
			queryFound=true;
			director.getBuilder()->onQuery();
		}
    break;

  case 16:
#line 140 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHead();
        }
    break;

  case 17:
#line 144 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceAtom();
            director.getBuilder()->onHead();
        }
    break;

  case 18:
#line 152 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBody();
        }
    break;

  case 19:
#line 159 "src/parser/aspcore2.y"
    {
            // There is only the weight. No level and terms.
            director.getBuilder()->onWeightAtLevels(1,0,0);
        }
    break;

  case 20:
#line 164 "src/parser/aspcore2.y"
    {
            // There are also a level and/or some terms.
            // The finalization has been postponed to "level_and_terms".
        }
    break;

  case 21:
#line 172 "src/parser/aspcore2.y"
    {
            // There is no terms following the level.
            director.getBuilder()->onWeightAtLevels(1,1,0);
        }
    break;

  case 22:
#line 177 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeightAtLevels(1,1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.integer));
        }
    break;

  case 23:
#line 181 "src/parser/aspcore2.y"
    {
            // The level is omitted.
            // Thus, the first term, recognized as the
            // weight, should be a term of this list.
            director.getBuilder()->onWeightAtLevels(0,0,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.integer)+1);
        }
    break;

  case 24:
#line 191 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 25:
#line 195 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExtAtom();
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 26:
#line 200 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 27:
#line 204 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 28:
#line 208 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 29:
#line 215 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 30:
#line 219 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 35:
#line 233 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceLowerGuard();
        }
    break;

  case 36:
#line 240 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceUpperGuard();
        }
    break;

  case 39:
#line 252 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 40:
#line 256 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 41:
#line 263 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementAtom();
        }
    break;

  case 42:
#line 270 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 43:
#line 274 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 44:
#line 281 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 45:
#line 285 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 46:
#line 292 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 47:
#line 296 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral(true);
        }
    break;

  case 48:
#line 300 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBuiltinOrList();
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 49:
#line 305 "src/parser/aspcore2.y"
    {
    		director.getBuilder()->onExtAtom();
    	}
    break;

  case 50:
#line 309 "src/parser/aspcore2.y"
    {
			director.getBuilder()->onExtAtom(true);
		}
    break;

  case 53:
#line 318 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregate();
        }
    break;

  case 54:
#line 325 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialAtom();
        }
    break;

  case 55:
#line 330 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(); }
    break;

  case 56:
#line 331 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(true); }
    break;

  case 57:
#line 336 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 58:
#line 341 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string);
        }
    break;

  case 59:
#line 346 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 60:
#line 355 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExternalPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (9))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (9))].yystate.yysemantics.yysval.string);
        }
    break;

  case 61:
#line 360 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onExternalPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.string);
      }
    break;

  case 62:
#line 368 "src/parser/aspcore2.y"
    {
	     	director.getBuilder()->onSemicolon();
        }
    break;

  case 66:
#line 380 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onNamedParameter();
    }
    break;

  case 67:
#line 386 "src/parser/aspcore2.y"
    { ((*yyvalp).integer) = 1; }
    break;

  case 68:
#line 387 "src/parser/aspcore2.y"
    { ((*yyvalp).integer) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.integer) + 1; }
    break;

  case 73:
#line 402 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onBuiltinAtom();
    }
    break;

  case 74:
#line 406 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onIsNullPredicate(false);
    }
    break;

  case 75:
#line 410 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onIsNullPredicate(true);
    }
    break;

  case 76:
#line 417 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onEqualOperator();
        }
    break;

  case 77:
#line 421 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnequalOperator();
        }
    break;

  case 81:
#line 434 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '+'; }
    break;

  case 82:
#line 435 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '-'; }
    break;

  case 83:
#line 439 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '*'; }
    break;

  case 84:
#line 440 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '/'; }
    break;

  case 85:
#line 441 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '\\'; }
    break;

  case 86:
#line 446 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnknownVariable();
        }
    break;

  case 87:
#line 450 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermRange((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 88:
#line 456 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermDash();
        }
    break;

  case 90:
#line 465 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 91:
#line 470 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 92:
#line 475 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNullTerm();
        }
    break;

  case 93:
#line 479 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermParams();
        }
    break;

  case 94:
#line 485 "src/parser/aspcore2.y"
    {}
    break;

  case 97:
#line 494 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.single_char));
        }
    break;

  case 98:
#line 501 "src/parser/aspcore2.y"
    {}
    break;

  case 99:
#line 503 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.single_char));
        }
    break;

  case 103:
#line 512 "src/parser/aspcore2.y"
    {
           director.getBuilder()->onAggregateVariableTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
           delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
       }
    break;

  case 104:
#line 520 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 105:
#line 525 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 106:
#line 530 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 107:
#line 535 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string),true);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
        }
    break;

  case 108:
#line 543 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateVariableTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 109:
#line 548 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    break;

  case 110:
#line 555 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunctionalTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string),(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.integer));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string);
       }
    break;

  case 111:
#line 563 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm(0);
       }
    break;

  case 112:
#line 567 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.integer));
       }
    break;

  case 113:
#line 574 "src/parser/aspcore2.y"
    {
                ((*yyvalp).integer) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.integer);
        }
    break;

  case 114:
#line 578 "src/parser/aspcore2.y"
    {
                ((*yyvalp).integer) = 2;
        }
    break;

  case 115:
#line 585 "src/parser/aspcore2.y"
    {

                director.getBuilder()->onHeadTailList();

        }
    break;

  case 116:
#line 594 "src/parser/aspcore2.y"
    {
                director.getBuilder()->onTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
                delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 118:
#line 603 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 119:
#line 608 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 120:
#line 615 "src/parser/aspcore2.y"
    { ((*yyvalp).string) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string); }
    break;

  case 121:
#line 616 "src/parser/aspcore2.y"
    { ((*yyvalp).string) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string); }
    break;

  case 122:
#line 617 "src/parser/aspcore2.y"
    { ((*yyvalp).string) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string); }
    break;

  case 123:
#line 622 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onDirective((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.string),(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.string);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
        }
    break;

  case 124:
#line 631 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAtom();
        }
    break;

  case 125:
#line 638 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 126:
#line 645 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 129:
#line 657 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 133:
#line 673 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 135:
#line 684 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 136:
#line 691 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 144:
#line 711 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAggregateAssignment();
        }
    break;

  case 146:
#line 722 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAssignVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
            director.getBuilder()->onMultiAssignVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 147:
#line 729 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAssignVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 148:
#line 737 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOperator();
        }
    break;

  case 149:
#line 741 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOrEqualOperator();
        }
    break;

  case 150:
#line 748 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOperator();
        }
    break;

  case 151:
#line 752 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    break;

  case 156:
#line 772 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 159:
#line 786 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 160:
#line 790 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 161:
#line 797 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroupSemicolon();
        }
    break;

  case 162:
#line 804 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 163:
#line 809 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 164:
#line 814 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 165:
#line 819 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 166:
#line 824 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 169:
#line 845 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTopLevelAlias();
    }
    break;

  case 170:
#line 849 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTopLevelAlias();
    }
    break;

  case 171:
#line 853 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 172:
#line 857 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 174:
#line 868 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopyTo((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string));
         delete [] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
    }
    break;

  case 176:
#line 880 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLStart();
    }
    break;

  case 177:
#line 887 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelect();
    }
    break;

  case 178:
#line 896 "src/parser/aspcore2.y"
    {
        char * c = new char[2];
        c[0] = '*';
        c[1] = '\0';
        director.getBuilder()->onSQLQualifiedName(c);
        delete[] c;
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 182:
#line 914 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 183:
#line 923 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFrom();
    }
    break;

  case 186:
#line 937 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTable();
    }
    break;

  case 187:
#line 941 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateFromItem();
    }
    break;

  case 188:
#line 945 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSubQuery();
    }
    break;

  case 190:
#line 953 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateArity((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 192:
#line 962 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateColumn((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 193:
#line 967 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateColumn((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 194:
#line 975 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableName((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.string);
    }
    break;

  case 195:
#line 980 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableNameString((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 196:
#line 987 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 197:
#line 992 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 200:
#line 1006 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 201:
#line 1011 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
     }
    break;

  case 202:
#line 1016 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 203:
#line 1021 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 204:
#line 1026 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 205:
#line 1031 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 206:
#line 1036 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 208:
#line 1046 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhere();
    }
    break;

  case 210:
#line 1054 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLOperatorCondition("AND");
    }
    break;

  case 211:
#line 1058 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLOperatorCondition("OR");
    }
    break;

  case 212:
#line 1065 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLInListValue(); }
    break;

  case 213:
#line 1067 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLInListValue(); }
    break;

  case 214:
#line 1072 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
    }
    break;

  case 215:
#line 1076 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhereSubqueryPredicate();
    }
    break;

  case 216:
#line 1080 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInListPredicate(false);
    }
    break;

  case 217:
#line 1084 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInListPredicate(true);
    }
    break;

  case 218:
#line 1088 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInSubqueryPredicate(false);
    }
    break;

  case 219:
#line 1092 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInSubqueryPredicate(true);
    }
    break;

  case 220:
#line 1096 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLIsNullPredicate(false);
    }
    break;

  case 221:
#line 1100 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLIsNullPredicate(true);
    }
    break;

  case 223:
#line 1108 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhereGroupEnd();
    }
    break;

  case 224:
#line 1115 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhereGroupBegin();
    }
    break;

  case 225:
#line 1123 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExprOp();
    }
    break;

  case 226:
#line 1127 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLikePredicate();
    }
    break;

  case 227:
#line 1134 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExpr();
    }
    break;

  case 230:
#line 1147 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 231:
#line 1151 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 236:
#line 1169 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderCol();
    }
    break;

  case 238:
#line 1177 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 239:
#line 1182 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 240:
#line 1187 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 241:
#line 1192 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 242:
#line 1197 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 243:
#line 1204 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderModifier(nullptr);
    }
    break;

  case 244:
#line 1208 "src/parser/aspcore2.y"
    {
        char c[] = "asc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 245:
#line 1213 "src/parser/aspcore2.y"
    {
        char c[] = "desc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 247:
#line 1222 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLimit((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
    }
    break;

  case 250:
#line 1235 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAddTerm('+');
    }
    break;

  case 251:
#line 1239 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAddTerm('-');
    }
    break;

  case 252:
#line 1246 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFinalizeMulExpr();
    }
    break;

  case 254:
#line 1254 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLMulTerm('*');
    }
    break;

  case 255:
#line 1258 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLMulTerm('/');
    }
    break;

  case 256:
#line 1266 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 257:
#line 1271 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 259:
#line 1276 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLParenOpen(); }
    break;

  case 260:
#line 1276 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLParenClose(); }
    break;

  case 263:
#line 1283 "src/parser/aspcore2.y"
    {
        char c[] = "*";
        director.getBuilder()->onSQLValue(c);
    }
    break;

  case 264:
#line 1291 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 265:
#line 1296 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 266:
#line 1301 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 267:
#line 1307 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 268:
#line 1313 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 269:
#line 1319 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 270:
#line 1330 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 271:
#line 1335 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 272:
#line 1340 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 273:
#line 1345 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 274:
#line 1350 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;


/* Line 930 of glr.c.  */
#line 2990 "src/parser/aspcore2_parser.hpp"
      default: break;
    }

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


/*ARGSUSED*/ static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YYUSE (yy0);
  YYUSE (yy1);

  switch (yyn)
    {
      
      default: break;
    }
}

			      /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, bumblebee::ParserInputDirector& director)
{
  YYUSE (yyvaluep);
  YYUSE (director);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys, bumblebee::ParserInputDirector& director)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yystos[yys->yylrState],
		&yys->yysemantics.yysval, director);
  else
    {
#if YYDEBUG
      if (yydebug)
	{
	  if (yys->yysemantics.yyfirstVal)
	    YYFPRINTF (stderr, "%s unresolved ", yymsg);
	  else
	    YYFPRINTF (stderr, "%s incomplete ", yymsg);
	  yy_symbol_print (stderr, yystos[yys->yylrState],
			   NULL, director);
	  YYFPRINTF (stderr, "\n");
	}
#endif

      if (yys->yysemantics.yyfirstVal)
	{
	  yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
	  yyGLRState *yyrh;
	  int yyn;
	  for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
	       yyn > 0;
	       yyrh = yyrh->yypred, yyn -= 1)
	    yydestroyGLRState (yymsg, yyrh, director);
	}
    }
}

/** Left-hand-side symbol for rule #RULE.  */
static inline yySymbol
yylhsNonterm (yyRuleNum yyrule)
{
  return yyr1[yyrule];
}

#define yyis_pact_ninf(yystate) \
  ((yystate) == YYPACT_NINF)

/** True iff LR state STATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yyStateNum yystate)
{
  return yyis_pact_ninf (yypact[yystate]);
}

/** The default reduction for STATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yyis_table_ninf(yytable_value) \
  YYID (0)

/** Set *YYACTION to the action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *CONFLICTS to a pointer into yyconfl to 0-terminated list of
 *  conflicting reductions.
 */
static inline void
yygetLRActions (yyStateNum yystate, int yytoken,
		int* yyaction, const short int** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyaction = -yydefact[yystate];
      *yyconflicts = yyconfl;
    }
  else if (! yyis_table_ninf (yytable[yyindex]))
    {
      *yyaction = yytable[yyindex];
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
  else
    {
      *yyaction = 0;
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
}

static inline yyStateNum
yyLRgotoState (yyStateNum yystate, yySymbol yylhs)
{
  int yyr;
  yyr = yypgoto[yylhs - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yylhs - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

				/* GLRStates */

/** Return a fresh GLRStackItem.  Callers should call
 * YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 * headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  RULENUM on the semantic values in RHS to the list of
 *  alternative actions for STATE.  Assumes that RHS comes from
 *  stack #K of *STACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, size_t yyk, yyGLRState* yystate,
		     yyGLRState* rhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  yynewOption->yystate = rhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
      yynewOption->yyloc = yylloc;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

				/* GLRStacks */

/** Initialize SET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates = (yyGLRState**) YYMALLOC (16 * sizeof yyset->yystates[0]);
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = NULL;
  yyset->yylookaheadNeeds =
    (yybool*) YYMALLOC (16 * sizeof yyset->yylookaheadNeeds[0]);
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize STACK to a single empty stack, with total maximum
 *  capacity for all stacks of SIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, size_t yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems =
    (yyGLRStackItem*) YYMALLOC (yysize * sizeof yystackp->yynextFree[0]);
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS,YYTOITEMS,YYX,YYTYPE) \
  &((YYTOITEMS) - ((YYFROMITEMS) - (yyGLRStackItem*) (YYX)))->YYTYPE

/** If STACK is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  size_t yysize, yynewSize;
  size_t yyn;
  yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems = (yyGLRStackItem*) YYMALLOC (yynewSize * sizeof yynewItems[0]);
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*(yybool *) yyp0)
	{
	  yyGLRState* yys0 = &yyp0->yystate;
	  yyGLRState* yys1 = &yyp1->yystate;
	  if (yys0->yypred != NULL)
	    yys1->yypred =
	      YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
	  if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != NULL)
	    yys1->yysemantics.yyfirstVal =
	      YYRELOC(yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
	}
      else
	{
	  yySemanticOption* yyv0 = &yyp0->yyoption;
	  yySemanticOption* yyv1 = &yyp1->yyoption;
	  if (yyv0->yystate != NULL)
	    yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
	  if (yyv0->yynext != NULL)
	    yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
	}
    }
  if (yystackp->yysplitPoint != NULL)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
				 yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != NULL)
      yystackp->yytops.yystates[yyn] =
	YYRELOC (yystackp->yyitems, yynewItems,
		 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that S is a GLRState somewhere on STACK, update the
 *  splitpoint of STACK, if needed, so that it is at least as deep as
 *  S.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != NULL && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #K in STACK.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yytops.yystates[yyk] != NULL)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = NULL;
}

/** Undelete the last stack that was marked as deleted.  Can only be
    done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == NULL || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YYDPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = NULL;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  size_t yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == NULL)
	{
	  if (yyi == yyj)
	    {
	      YYDPRINTF ((stderr, "Removing dead stacks.\n"));
	    }
	  yystackp->yytops.yysize -= 1;
	}
      else
	{
	  yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
	  /* In the current implementation, it's unnecessary to copy
	     yystackp->yytops.yylookaheadNeeds[yyi] since, after
	     yyremoveDeletes returns, the parser immediately either enters
	     deterministic operation or shifts a token.  However, it doesn't
	     hurt, and the code might evolve to need it.  */
	  yystackp->yytops.yylookaheadNeeds[yyj] =
	    yystackp->yytops.yylookaheadNeeds[yyi];
	  if (yyj != yyi)
	    {
	      YYDPRINTF ((stderr, "Rename stack %lu -> %lu.\n",
			  (unsigned long int) yyi, (unsigned long int) yyj));
	    }
	  yyj += 1;
	}
      yyi += 1;
    }
}

/** Shift to a new state on stack #K of STACK, corresponding to LR state
 * LRSTATE, at input position POSN, with (resolved) semantic value SVAL.  */
static inline void
yyglrShift (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
	    size_t yyposn,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yysval = *yyvalp;
  yynewState->yyloc = *yylocp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #K of YYSTACK, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
		 size_t yyposn, yyGLRState* rhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = NULL;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, rhs, yyrule);
}

/** Pop the symbols consumed by reduction #RULE from the top of stack
 *  #K of STACK, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *VALP to the resulting value,
 *  and *LOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp, bumblebee::ParserInputDirector& director)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == NULL)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* rhs = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
      YYASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, rhs,
			   yyvalp, yylocp, yystackp, director);
    }
  else
    {
      /* At present, doAction is never called in nondeterministic
       * mode, so this branch is never taken.  It is here in
       * anticipation of a future feature that will allow immediate
       * evaluation of selected actions in nondeterministic mode.  */
      int yyi;
      yyGLRState* yys;
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
	= yystackp->yytops.yystates[yyk];
      for (yyi = 0; yyi < yynrhs; yyi += 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp, director);
    }
}

#if !YYDEBUG
# define YY_REDUCE_PRINT(Args)
#else
# define YY_REDUCE_PRINT(Args)		\
do {					\
  if (yydebug)				\
    yy_reduce_print Args;		\
} while (YYID (0))

/*----------------------------------------------------------.
| Report that the RULE is going to be reduced on stack #K.  |
`----------------------------------------------------------*/

/*ARGSUSED*/ static inline void
yy_reduce_print (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp, bumblebee::ParserInputDirector& director)
{
  int yynrhs = yyrhsLength (yyrule);
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  yyGLRStackItem* yyvsp = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
  int yylow = 1;
  int yyi;
  YYUSE (yyvalp);
  YYUSE (yylocp);
  YYUSE (director);
  YYFPRINTF (stderr, "Reducing stack %lu by rule %d (line %lu):\n",
	     (unsigned long int) yyk, yyrule - 1,
	     (unsigned long int) yyrline[yyrule]);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yysemantics.yysval)
		       		       , director);
      fprintf (stderr, "\n");
    }
}
#endif

/** Pop items off stack #K of STACK according to grammar rule RULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with RULE and store its value with the
 *  newly pushed state, if FORCEEVAL or if STACK is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #K from
 *  the STACK.  In this case, the (necessarily deferred) semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	     yybool yyforceEval, bumblebee::ParserInputDirector& director)
{
  size_t yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == NULL)
    {
      YYSTYPE yysval;
      YYLTYPE yyloc;

      YY_REDUCE_PRINT ((yystackp, yyk, yyrule, &yysval, &yyloc, director));
      YYCHK (yydoAction (yystackp, yyk, yyrule, &yysval,
			 &yyloc, director));
      YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyrule], &yysval, &yyloc);
      yyglrShift (yystackp, yyk,
		  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
				 yylhsNonterm (yyrule)),
		  yyposn, &yysval, &yyloc);
    }
  else
    {
      size_t yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yyStateNum yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
	   0 < yyn; yyn -= 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YYDPRINTF ((stderr,
		  "Reduced stack %lu by rule #%d; action deferred.  Now in state %d.\n",
		  (unsigned long int) yyk, yyrule - 1, yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
	if (yyi != yyk && yystackp->yytops.yystates[yyi] != NULL)
	  {
	    yyGLRState* yyp, *yysplit = yystackp->yysplitPoint;
	    yyp = yystackp->yytops.yystates[yyi];
	    while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
	      {
		if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
		  {
		    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
		    yymarkStackDeleted (yystackp, yyk);
		    YYDPRINTF ((stderr, "Merging stack %lu into stack %lu.\n",
				(unsigned long int) yyk,
				(unsigned long int) yyi));
		    return yyok;
		  }
		yyp = yyp->yypred;
	      }
	  }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static size_t
yysplitStack (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yysplitPoint == NULL)
    {
      YYASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yysize >= yystackp->yytops.yycapacity)
    {
      yyGLRState** yynewStates;
      yybool* yynewLookaheadNeeds;

      yynewStates = NULL;

      if (yystackp->yytops.yycapacity
	  > (YYSIZEMAX / (2 * sizeof yynewStates[0])))
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      yynewStates =
	(yyGLRState**) YYREALLOC (yystackp->yytops.yystates,
				  (yystackp->yytops.yycapacity
				   * sizeof yynewStates[0]));
      if (yynewStates == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yystates = yynewStates;

      yynewLookaheadNeeds =
	(yybool*) YYREALLOC (yystackp->yytops.yylookaheadNeeds,
			     (yystackp->yytops.yycapacity
			      * sizeof yynewLookaheadNeeds[0]));
      if (yynewLookaheadNeeds == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize-1;
}

/** True iff Y0 and Y1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
	   yyn = yyrhsLength (yyy0->yyrule);
	   yyn > 0;
	   yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
	if (yys0->yyposn != yys1->yyposn)
	  return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (Y0,Y1), destructively merge the
 *  alternative semantic values for the RHS-symbols of Y1 and Y0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       yyn > 0;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
	break;
      else if (yys0->yyresolved)
	{
	  yys1->yyresolved = yytrue;
	  yys1->yysemantics.yysval = yys0->yysemantics.yysval;
	}
      else if (yys1->yyresolved)
	{
	  yys0->yyresolved = yytrue;
	  yys0->yysemantics.yysval = yys1->yysemantics.yysval;
	}
      else
	{
	  yySemanticOption** yyz0p;
	  yySemanticOption* yyz1;
	  yyz0p = &yys0->yysemantics.yyfirstVal;
	  yyz1 = yys1->yysemantics.yyfirstVal;
	  while (YYID (yytrue))
	    {
	      if (yyz1 == *yyz0p || yyz1 == NULL)
		break;
	      else if (*yyz0p == NULL)
		{
		  *yyz0p = yyz1;
		  break;
		}
	      else if (*yyz0p < yyz1)
		{
		  yySemanticOption* yyz = *yyz0p;
		  *yyz0p = yyz1;
		  yyz1 = yyz1->yynext;
		  (*yyz0p)->yynext = yyz;
		}
	      yyz0p = &(*yyz0p)->yynext;
	    }
	  yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
	}
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
	return 0;
      else
	return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG yyresolveValue (yyGLRState* yys,
				   yyGLRStack* yystackp, bumblebee::ParserInputDirector& director);


/** Resolve the previous N states starting at and including state S.  If result
 *  != yyok, some states may have been left unresolved possibly with empty
 *  semantic option chains.  Regardless of whether result = yyok, each state
 *  has been left with consistent data so that yydestroyGLRState can be invoked
 *  if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
		 yyGLRStack* yystackp, bumblebee::ParserInputDirector& director)
{
  if (0 < yyn)
    {
      YYASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp, director));
      if (! yys->yyresolved)
	YYCHK (yyresolveValue (yys, yystackp, director));
    }
  return yyok;
}

/** Resolve the states for the RHS of OPT, perform its user action, and return
 *  the semantic value and location.  Regardless of whether result = yyok, all
 *  RHS states have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp, bumblebee::ParserInputDirector& director)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs;
  int yychar_current;
  YYSTYPE yylval_current;
  YYLTYPE yylloc_current;
  YYRESULTTAG yyflag;

  yynrhs = yyrhsLength (yyopt->yyrule);
  yyflag = yyresolveStates (yyopt->yystate, yynrhs, yystackp, director);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
	yydestroyGLRState ("Cleanup: popping", yys, director);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  yychar_current = yychar;
  yylval_current = yylval;
  yylloc_current = yylloc;
  yychar = yyopt->yyrawchar;
  yylval = yyopt->yyval;
  yylloc = yyopt->yyloc;
  yyflag = yyuserAction (yyopt->yyrule, yynrhs,
			   yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp, director);
  yychar = yychar_current;
  yylval = yylval_current;
  yylloc = yylloc_current;
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == NULL)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, empty>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1);
  else
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, tokens %lu .. %lu>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1, (unsigned long int) (yys->yyposn + 1),
	       (unsigned long int) yyx->yystate->yyposn);
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
	{
	  if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
	    YYFPRINTF (stderr, "%*s%s <empty>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]));
	  else
	    YYFPRINTF (stderr, "%*s%s <tokens %lu .. %lu>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]),
		       (unsigned long int) (yystates[yyi - 1]->yyposn + 1),
		       (unsigned long int) yystates[yyi]->yyposn);
	}
      else
	yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

/*ARGSUSED*/ static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
		   yySemanticOption* yyx1, bumblebee::ParserInputDirector& director)
{
  YYUSE (yyx0);
  YYUSE (yyx1);

#if YYDEBUG
  YYFPRINTF (stderr, "Ambiguity detected.\n");
  YYFPRINTF (stderr, "Option 1,\n");
  yyreportTree (yyx0, 2);
  YYFPRINTF (stderr, "\nOption 2,\n");
  yyreportTree (yyx1, 2);
  YYFPRINTF (stderr, "\n");
#endif

  yyerror (director, YY_("syntax is ambiguous"));
  return yyabort;
}

/** Starting at and including state S1, resolve the location for each of the
 *  previous N1 states that is unresolved.  The first semantic option of a state
 *  is always chosen.  */
static void
yyresolveLocations (yyGLRState* yys1, int yyn1,
		    yyGLRStack *yystackp, bumblebee::ParserInputDirector& director)
{
  if (0 < yyn1)
    {
      yyresolveLocations (yys1->yypred, yyn1 - 1, yystackp, director);
      if (!yys1->yyresolved)
	{
	  yySemanticOption *yyoption;
	  yyGLRStackItem yyrhsloc[1 + YYMAXRHS];
	  int yynrhs;
	  int yychar_current;
	  YYSTYPE yylval_current;
	  YYLTYPE yylloc_current;
	  yyoption = yys1->yysemantics.yyfirstVal;
	  YYASSERT (yyoption != NULL);
	  yynrhs = yyrhsLength (yyoption->yyrule);
	  if (yynrhs > 0)
	    {
	      yyGLRState *yys;
	      int yyn;
	      yyresolveLocations (yyoption->yystate, yynrhs,
				  yystackp, director);
	      for (yys = yyoption->yystate, yyn = yynrhs;
		   yyn > 0;
		   yys = yys->yypred, yyn -= 1)
		yyrhsloc[yyn].yystate.yyloc = yys->yyloc;
	    }
	  else
	    {
	      /* Both yyresolveAction and yyresolveLocations traverse the GSS
		 in reverse rightmost order.  It is only necessary to invoke
		 yyresolveLocations on a subforest for which yyresolveAction
		 would have been invoked next had an ambiguity not been
		 detected.  Thus the location of the previous state (but not
		 necessarily the previous state itself) is guaranteed to be
		 resolved already.  */
	      yyGLRState *yyprevious = yyoption->yystate;
	      yyrhsloc[0].yystate.yyloc = yyprevious->yyloc;
	    }
	  yychar_current = yychar;
	  yylval_current = yylval;
	  yylloc_current = yylloc;
	  yychar = yyoption->yyrawchar;
	  yylval = yyoption->yyval;
	  yylloc = yyoption->yyloc;
	  YYLLOC_DEFAULT ((yys1->yyloc), yyrhsloc, yynrhs);
	  yychar = yychar_current;
	  yylval = yylval_current;
	  yylloc = yylloc_current;
	}
    }
}

/** Resolve the ambiguity represented in state S, perform the indicated
 *  actions, and set the semantic value of S.  If result != yyok, the chain of
 *  semantic options in S has been cleared instead or it has been left
 *  unmodified except that redundant options may have been removed.  Regardless
 *  of whether result = yyok, S has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp, bumblebee::ParserInputDirector& director)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest;
  yySemanticOption** yypp;
  yybool yymerge;
  YYSTYPE yysval;
  YYRESULTTAG yyflag;
  YYLTYPE *yylocp = &yys->yyloc;

  yybest = yyoptionList;
  yymerge = yyfalse;
  for (yypp = &yyoptionList->yynext; *yypp != NULL; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
	{
	  yymergeOptionSets (yybest, yyp);
	  *yypp = yyp->yynext;
	}
      else
	{
	  switch (yypreference (yybest, yyp))
	    {
	    case 0:
	      yyresolveLocations (yys, 1, yystackp, director);
	      return yyreportAmbiguity (yybest, yyp, director);
	      break;
	    case 1:
	      yymerge = yytrue;
	      break;
	    case 2:
	      break;
	    case 3:
	      yybest = yyp;
	      yymerge = yyfalse;
	      break;
	    default:
	      /* This cannot happen so it is not worth a YYASSERT (yyfalse),
		 but some compilers complain if the default case is
		 omitted.  */
	      break;
	    }
	  yypp = &yyp->yynext;
	}
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yysval,
				yylocp, director);
      if (yyflag == yyok)
	for (yyp = yybest->yynext; yyp != NULL; yyp = yyp->yynext)
	  {
	    if (yyprec == yydprec[yyp->yyrule])
	      {
		YYSTYPE yysval_other;
		YYLTYPE yydummy;
		yyflag = yyresolveAction (yyp, yystackp, &yysval_other,
					  &yydummy, director);
		if (yyflag != yyok)
		  {
		    yydestruct ("Cleanup: discarding incompletely merged value for",
				yystos[yys->yylrState],
				&yysval, director);
		    break;
		  }
		yyuserMerge (yymerger[yyp->yyrule], &yysval, &yysval_other);
	      }
	  }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yysval, yylocp, director);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yysval = yysval;
    }
  else
    yys->yysemantics.yyfirstVal = NULL;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp, bumblebee::ParserInputDirector& director)
{
  if (yystackp->yysplitPoint != NULL)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
	   yys != yystackp->yysplitPoint;
	   yys = yys->yypred, yyn += 1)
	continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
			     , director));
    }
  return yyok;
}

static void
yycompressStack (yyGLRStack* yystackp)
{
  yyGLRState* yyp, *yyq, *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == NULL)
    return;

  for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = NULL;
       yyp != yystackp->yysplitPoint;
       yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = ((yyGLRStackItem*) yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;

  while (yyr != NULL)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, size_t yyk,
		   size_t yyposn, bumblebee::ParserInputDirector& director)
{
  int yyaction;
  const short int* yyconflicts;
  yyRuleNum yyrule;

  while (yystackp->yytops.yystates[yyk] != NULL)
    {
      yyStateNum yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YYDPRINTF ((stderr, "Stack %lu Entering state %d\n",
		  (unsigned long int) yyk, yystate));

      YYASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
	{
	  yyrule = yydefaultAction (yystate);
	  if (yyrule == 0)
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      return yyok;
	    }
	  YYCHK (yyglrReduce (yystackp, yyk, yyrule, yyfalse, director));
	}
      else
	{
	  yySymbol yytoken;
	  yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;
	  if (yychar == YYEMPTY)
	    {
	      YYDPRINTF ((stderr, "Reading a token: "));
	      yychar = YYLEX;
	      yytoken = YYTRANSLATE (yychar);
	      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	    }
	  else
	    yytoken = YYTRANSLATE (yychar);
	  yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);

	  while (*yyconflicts != 0)
	    {
	      size_t yynewStack = yysplitStack (yystackp, yyk);
	      YYDPRINTF ((stderr, "Splitting off stack %lu from %lu.\n",
			  (unsigned long int) yynewStack,
			  (unsigned long int) yyk));
	      YYCHK (yyglrReduce (yystackp, yynewStack,
				  *yyconflicts, yyfalse, director));
	      YYCHK (yyprocessOneStack (yystackp, yynewStack,
					yyposn, director));
	      yyconflicts += 1;
	    }

	  if (yyisShiftAction (yyaction))
	    break;
	  else if (yyisErrorAction (yyaction))
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      break;
	    }
	  else
	    YYCHK (yyglrReduce (yystackp, yyk, -yyaction,
				yyfalse, director));
	}
    }
  return yyok;
}

/*ARGSUSED*/ static void
yyreportSyntaxError (yyGLRStack* yystackp, bumblebee::ParserInputDirector& director)
{
  if (yystackp->yyerrState == 0)
    {
#if YYERROR_VERBOSE
      int yyn;
      yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
      if (YYPACT_NINF < yyn && yyn <= YYLAST)
	{
	  yySymbol yytoken = YYTRANSLATE (yychar);
	  size_t yysize0 = yytnamerr (NULL, yytokenName (yytoken));
	  size_t yysize = yysize0;
	  size_t yysize1;
	  yybool yysize_overflow = yyfalse;
	  char* yymsg = NULL;
	  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn + 1;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytokenName (yytoken);
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytokenName (yyx);
		yysize1 = yysize + yytnamerr (NULL, yytokenName (yyx));
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + strlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow)
	    yymsg = (char *) YYMALLOC (yysize);

	  if (yymsg)
	    {
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (director, yymsg);
	      YYFREE (yymsg);
	    }
	  else
	    {
	      yyerror (director, YY_("syntax error"));
	      yyMemoryExhausted (yystackp);
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (director, YY_("syntax error"));
      yynerrs += 1;
    }
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the look-ahead.  */
/*ARGSUSED*/ static void
yyrecoverSyntaxError (yyGLRStack* yystackp, bumblebee::ParserInputDirector& director)
{
  size_t yyk;
  int yyj;

  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (YYID (yytrue))
      {
	yySymbol yytoken;
	if (yychar == YYEOF)
	  yyFail (yystackp, director, NULL);
	if (yychar != YYEMPTY)
	  {
	    yytoken = YYTRANSLATE (yychar);
	    yydestruct ("Error: discarding",
			yytoken, &yylval, director);
	  }
	YYDPRINTF ((stderr, "Reading a token: "));
	yychar = YYLEX;
	yytoken = YYTRANSLATE (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
	if (yyis_pact_ninf (yyj))
	  return;
	yyj += yytoken;
	if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
	  {
	    if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
	      return;
	  }
	else if (yytable[yyj] != 0 && ! yyis_table_ninf (yytable[yyj]))
	  return;
      }

  /* Reduce to one stack.  */
  for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
    if (yystackp->yytops.yystates[yyk] != NULL)
      break;
  if (yyk >= yystackp->yytops.yysize)
    yyFail (yystackp, director, NULL);
  for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
    yymarkStackDeleted (yystackp, yyk);
  yyremoveDeletes (yystackp);
  yycompressStack (yystackp);

  /* Now pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != NULL)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
      yyj = yypact[yys->yylrState];
      if (! yyis_pact_ninf (yyj))
	{
	  yyj += YYTERROR;
	  if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYTERROR
	      && yyisShiftAction (yytable[yyj]))
	    {
	      /* Shift the error token having adjusted its location.  */
	      YYLTYPE yyerrloc;
	      YY_SYMBOL_PRINT ("Shifting", yystos[yytable[yyj]],
			       &yylval, &yyerrloc);
	      yyglrShift (yystackp, 0, yytable[yyj],
			  yys->yyposn, &yylval, &yyerrloc);
	      yys = yystackp->yytops.yystates[0];
	      break;
	    }
	}

      yydestroyGLRState ("Error: popping", yys, director);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == NULL)
    yyFail (yystackp, director, NULL);
}

#define YYCHK1(YYE)							     \
  do {									     \
    switch (YYE) {							     \
    case yyok:								     \
      break;								     \
    case yyabort:							     \
      goto yyabortlab;							     \
    case yyaccept:							     \
      goto yyacceptlab;							     \
    case yyerr:								     \
      goto yyuser_error;						     \
    default:								     \
      goto yybuglab;							     \
    }									     \
  } while (YYID (0))


/*----------.
| yyparse.  |
`----------*/

int
yyparse (bumblebee::ParserInputDirector& director)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  size_t yyposn;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;


  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval, &yylloc);
  yyposn = 0;

  while (YYID (yytrue))
    {
      /* For efficiency, we have two loops, the first of which is
	 specialized to deterministic operation (single stack, no
	 potential ambiguity).  */
      /* Standard mode */
      while (YYID (yytrue))
	{
	  yyRuleNum yyrule;
	  int yyaction;
	  const short int* yyconflicts;

	  yyStateNum yystate = yystack.yytops.yystates[0]->yylrState;
	  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
	  if (yystate == YYFINAL)
	    goto yyacceptlab;
	  if (yyisDefaultedState (yystate))
	    {
	      yyrule = yydefaultAction (yystate);
	      if (yyrule == 0)
		{

		  yyreportSyntaxError (&yystack, director);
		  goto yyuser_error;
		}
	      YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue, director));
	    }
	  else
	    {
	      yySymbol yytoken;
	      if (yychar == YYEMPTY)
		{
		  YYDPRINTF ((stderr, "Reading a token: "));
		  yychar = YYLEX;
		  yytoken = YYTRANSLATE (yychar);
		  YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
		}
	      else
		yytoken = YYTRANSLATE (yychar);
	      yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);
	      if (*yyconflicts != 0)
		break;
	      if (yyisShiftAction (yyaction))
		{
		  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
		  if (yychar != YYEOF)
		    yychar = YYEMPTY;
		  yyposn += 1;
		  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval, &yylloc);
		  if (0 < yystack.yyerrState)
		    yystack.yyerrState -= 1;
		}
	      else if (yyisErrorAction (yyaction))
		{

		  yyreportSyntaxError (&yystack, director);
		  goto yyuser_error;
		}
	      else
		YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue, director));
	    }
	}

      while (YYID (yytrue))
	{
	  yySymbol yytoken_to_shift;
	  size_t yys;

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

	  /* yyprocessOneStack returns one of three things:

	      - An error flag.  If the caller is yyprocessOneStack, it
		immediately returns as well.  When the caller is finally
		yyparse, it jumps to an error label via YYCHK1.

	      - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
		(&yystack, yys), which sets the top state of yys to NULL.  Thus,
		yyparse's following invocation of yyremoveDeletes will remove
		the stack.

	      - yyok, when ready to shift a token.

	     Except in the first case, yyparse will invoke yyremoveDeletes and
	     then shift the next token onto all remaining stacks.  This
	     synchronization of the shift (that is, after all preceding
	     reductions on all stacks) helps prevent double destructor calls
	     on yylval in the event of memory exhaustion.  */

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn, director));
	  yyremoveDeletes (&yystack);
	  if (yystack.yytops.yysize == 0)
	    {
	      yyundeleteLastStack (&yystack);
	      if (yystack.yytops.yysize == 0)
		yyFail (&yystack, director, YY_("syntax error"));
	      YYCHK1 (yyresolveStack (&yystack, director));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));

	      yyreportSyntaxError (&yystack, director);
	      goto yyuser_error;
	    }

	  /* If any yyglrShift call fails, it will fail after shifting.  Thus,
	     a copy of yylval will already be on stack 0 in the event of a
	     failure in the following loop.  Thus, yychar is set to YYEMPTY
	     before the loop to make sure the user destructor for yylval isn't
	     called twice.  */
	  yytoken_to_shift = YYTRANSLATE (yychar);
	  yychar = YYEMPTY;
	  yyposn += 1;
	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    {
	      int yyaction;
	      const short int* yyconflicts;
	      yyStateNum yystate = yystack.yytops.yystates[yys]->yylrState;
	      yygetLRActions (yystate, yytoken_to_shift, &yyaction,
			      &yyconflicts);
	      /* Note that yyconflicts were handled by yyprocessOneStack.  */
	      YYDPRINTF ((stderr, "On stack %lu, ", (unsigned long int) yys));
	      YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
	      yyglrShift (&yystack, yys, yyaction, yyposn,
			  &yylval, &yylloc);
	      YYDPRINTF ((stderr, "Stack %lu now in state #%d\n",
			  (unsigned long int) yys,
			  yystack.yytops.yystates[yys]->yylrState));
	    }

	  if (yystack.yytops.yysize == 1)
	    {
	      YYCHK1 (yyresolveStack (&yystack, director));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yycompressStack (&yystack);
	      break;
	    }
	}
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack, director);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturn;

 yybuglab:
  YYASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturn;

 yyexhaustedlab:
  yyerror (director, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;

 yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
		YYTRANSLATE (yychar),
		&yylval, director);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
	{
	  size_t yysize = yystack.yytops.yysize;
	  size_t yyk;
	  for (yyk = 0; yyk < yysize; yyk += 1)
	    if (yystates[yyk])
	      {
		while (yystates[yyk])
		  {
		    yyGLRState *yys = yystates[yyk];
		    yydestroyGLRState ("Cleanup: popping", yys, director);
		    yystates[yyk] = yys->yypred;
		    yystack.yynextFree -= 1;
		    yystack.yyspaceLeft += 1;
		  }
		break;
	      }
	}
      yyfreeGLRStack (&yystack);
    }

  /* Make sure YYID is used.  */
  return YYID (yyresult);
}

/* DEBUGGING ONLY */
#ifdef YYDEBUG
static void yypstack (yyGLRStack* yystackp, size_t yyk)
  __attribute__ ((__unused__));
static void yypdumpstack (yyGLRStack* yystackp) __attribute__ ((__unused__));

static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      fprintf (stderr, " -> ");
    }
  fprintf (stderr, "%d@%lu", yys->yylrState, (unsigned long int) yys->yyposn);
}

static void
yypstates (yyGLRState* yyst)
{
  if (yyst == NULL)
    fprintf (stderr, "<null>");
  else
    yy_yypstack (yyst);
  fprintf (stderr, "\n");
}

static void
yypstack (yyGLRStack* yystackp, size_t yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

#define YYINDEX(YYX)							     \
    ((YYX) == NULL ? -1 : (yyGLRStackItem*) (YYX) - yystackp->yyitems)


static void
yypdumpstack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yyp;
  size_t yyi;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      fprintf (stderr, "%3lu. ", (unsigned long int) (yyp - yystackp->yyitems));
      if (*(yybool *) yyp)
	{
	  fprintf (stderr, "Res: %d, LR State: %d, posn: %lu, pred: %ld",
		   yyp->yystate.yyresolved, yyp->yystate.yylrState,
		   (unsigned long int) yyp->yystate.yyposn,
		   (long int) YYINDEX (yyp->yystate.yypred));
	  if (! yyp->yystate.yyresolved)
	    fprintf (stderr, ", firstVal: %ld",
		     (long int) YYINDEX (yyp->yystate.yysemantics.yyfirstVal));
	}
      else
	{
	  fprintf (stderr, "Option. rule: %d, state: %ld, next: %ld",
		   yyp->yyoption.yyrule - 1,
		   (long int) YYINDEX (yyp->yyoption.yystate),
		   (long int) YYINDEX (yyp->yyoption.yynext));
	}
      fprintf (stderr, "\n");
    }
  fprintf (stderr, "Tops:");
  for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
    fprintf (stderr, "%lu: %ld; ", (unsigned long int) yyi,
	     (long int) YYINDEX (yystackp->yytops.yystates[yyi]));
  fprintf (stderr, "\n");
}
#endif




