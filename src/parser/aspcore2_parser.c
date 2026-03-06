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






#include "aspcore2_parser.h"

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
#line 89 "src/parser/aspcore2_parser.c"

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
#define YYFINAL  95
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   595

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  122
/* YYNRULES -- Number of rules.  */
#define YYNRULES  269
/* YYNRULES -- Number of states.  */
#define YYNSTATES  445
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 9
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   326

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
      65,    66,    67,    68,    69,    70,    71
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
     220,   224,   226,   230,   234,   236,   238,   240,   242,   244,
     246,   248,   250,   252,   254,   256,   260,   263,   265,   267,
     269,   273,   275,   277,   279,   283,   285,   289,   291,   293,
     295,   297,   299,   301,   303,   306,   308,   310,   315,   318,
     322,   324,   326,   330,   332,   334,   336,   340,   342,   344,
     346,   349,   352,   355,   358,   361,   364,   367,   370,   372,
     374,   377,   380,   383,   386,   389,   392,   394,   396,   398,
     401,   404,   410,   415,   419,   423,   425,   427,   429,   431,
     436,   440,   442,   446,   448,   452,   454,   458,   464,   466,
     468,   470,   472,   474,   476,   478,   481,   487,   494,   501,
     509,   517,   520,   524,   526,   528,   530,   532,   534,   538,
     541,   544,   546,   550,   553,   556,   561,   563,   567,   572,
     574,   578,   586,   588,   590,   592,   593,   596,   598,   600,
     602,   604,   606,   608,   610,   611,   614,   616,   620,   624,
     626,   630,   632,   638,   644,   651,   657,   664,   666,   670,
     672,   676,   680,   682,   683,   687,   689,   693,   694,   698,
     700,   704,   707,   709,   711,   713,   715,   717,   719,   720,
     722,   724,   725,   728,   730,   732,   736,   740,   742,   744,
     748,   752,   754,   756,   758,   759,   764,   769,   771,   773,
     775,   777,   781,   785,   789,   793,   795,   797,   799,   801
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      74,     0,    -1,    49,    -1,    71,   151,    -1,    75,    -1,
       1,    -1,    76,    -1,    75,    76,    -1,    77,    -1,   124,
      -1,    78,    18,    -1,    78,    22,    18,    -1,    78,    22,
      79,    18,    -1,    22,    79,    18,    -1,    47,    79,    18,
      80,    -1,   125,    -1,    82,    -1,    84,    -1,    83,    -1,
      31,   111,    32,    -1,    31,   111,    81,    32,    -1,    46,
     111,    -1,    46,   111,    44,   101,    -1,    44,   101,    -1,
      95,    -1,    97,    -1,    82,    73,    95,    -1,    94,    -1,
      82,    73,    94,    -1,    93,    -1,    83,    44,    93,    -1,
      85,    33,    87,    34,    86,    -1,    85,    33,    87,    34,
      -1,    33,    87,    34,    -1,    33,    87,    34,    86,    -1,
     111,   106,    -1,   106,   111,    -1,    87,    20,    88,    -1,
      88,    -1,    89,    -1,    89,    21,    90,    -1,    96,    -1,
      92,    -1,    90,    44,    92,    -1,    92,    -1,    91,    44,
      92,    -1,    95,    -1,    45,    95,    -1,   103,    -1,    97,
      -1,    45,    97,    -1,    92,    -1,   138,    -1,   137,    -1,
      50,   122,    96,    -1,    96,    -1,    43,    96,    -1,   123,
      -1,   123,    29,   101,    30,    -1,   123,    29,    30,    -1,
      48,   123,    29,   101,    98,    99,    98,   101,    30,    -1,
      48,   123,    29,    98,    99,    98,   101,    30,    -1,    20,
      -1,    -1,   100,    -1,    99,    44,   100,    -1,   111,    35,
     111,    -1,   111,    -1,   101,    44,   111,    -1,   114,    -1,
     102,    44,   114,    -1,   104,    -1,   103,    41,   104,    -1,
     111,   106,   111,    -1,    35,    -1,    36,    -1,   105,    -1,
     141,    -1,   142,    -1,    24,    -1,    43,    -1,    25,    -1,
      26,    -1,    27,    -1,    28,    -1,     4,    19,     4,    -1,
      43,   111,    -1,   118,    -1,   123,    -1,     4,    -1,    29,
     111,    30,    -1,   109,    -1,   112,    -1,   113,    -1,   112,
     107,   113,    -1,   110,    -1,   113,   108,   110,    -1,   115,
      -1,   116,    -1,   117,    -1,    15,    -1,     3,    -1,     6,
      -1,     4,    -1,    43,     4,    -1,     5,    -1,    28,    -1,
     123,    29,   101,    30,    -1,    31,    32,    -1,    31,   119,
      32,    -1,   101,    -1,   120,    -1,   111,    49,   121,    -1,
       5,    -1,   118,    -1,     5,    -1,   122,    44,     5,    -1,
       3,    -1,     6,    -1,     5,    -1,     7,     8,    -1,    96,
      23,    -1,   111,   105,    -1,   105,   111,    -1,   126,   143,
      -1,   143,   127,    -1,   111,   141,    -1,   129,   143,    -1,
     130,    -1,   133,    -1,   111,   142,    -1,   132,   143,    -1,
     141,   111,    -1,   142,   111,    -1,   143,   134,    -1,   143,
     135,    -1,   131,    -1,   136,    -1,   128,    -1,   130,   134,
      -1,   133,   135,    -1,   139,   144,    33,   147,    34,    -1,
      29,   140,    30,    35,    -1,     5,    44,     5,    -1,   140,
      44,     5,    -1,    37,    -1,    39,    -1,    38,    -1,    40,
      -1,   150,    33,   147,    34,    -1,    14,   145,    32,    -1,
     146,    -1,   145,    44,   146,    -1,   123,    -1,   147,    20,
     148,    -1,   148,    -1,   102,    21,    91,    -1,   102,   149,
     102,    21,    91,    -1,    20,    -1,     9,    -1,    10,    -1,
      11,    -1,    12,    -1,    13,    -1,   152,    -1,   152,    20,
      -1,    29,   152,    30,    56,   168,    -1,    29,   152,    30,
      56,   168,    20,    -1,    57,    29,   152,    30,   153,   154,
      -1,    57,    29,   152,    30,   153,   154,    20,    -1,   155,
     156,   160,   169,   177,   179,   184,    -1,    58,     6,    -1,
      29,    99,    30,    -1,    51,    -1,   157,    -1,    25,    -1,
     158,    -1,   159,    -1,   158,    44,   159,    -1,   185,   167,
      -1,    52,   161,    -1,   162,    -1,   161,    44,   162,    -1,
     165,   167,    -1,   163,   167,    -1,    29,   151,    30,   167,
      -1,   166,    -1,   166,    26,     4,    -1,   166,    29,   164,
      30,    -1,   123,    -1,   164,    44,   123,    -1,    48,   123,
      29,   101,    98,    99,    30,    -1,     6,    -1,     3,    -1,
       5,    -1,    -1,    56,   168,    -1,     3,    -1,     5,    -1,
      70,    -1,    69,    -1,    68,    -1,    67,    -1,    66,    -1,
      -1,    53,   170,    -1,   172,    -1,   170,    42,   172,    -1,
     170,    41,   172,    -1,   185,    -1,   171,    44,   185,    -1,
     175,    -1,   176,   106,    29,   152,    30,    -1,   176,    64,
      29,   171,    30,    -1,   176,    65,    64,    29,   171,    30,
      -1,   176,    64,    29,   152,    30,    -1,   176,    65,    64,
      29,   152,    30,    -1,   173,    -1,   174,   170,    30,    -1,
      29,    -1,   176,   106,   185,    -1,   176,    63,   185,    -1,
     185,    -1,    -1,    54,    55,   178,    -1,   185,    -1,   178,
      44,   185,    -1,    -1,    59,    55,   180,    -1,   181,    -1,
     180,    44,   181,    -1,   182,   183,    -1,   192,    -1,    66,
      -1,    67,    -1,    68,    -1,    69,    -1,    70,    -1,    -1,
      60,    -1,    61,    -1,    -1,    62,     4,    -1,   186,    -1,
     187,    -1,   186,    24,   187,    -1,   186,    43,   187,    -1,
     188,    -1,   189,    -1,   188,    25,   189,    -1,   188,    26,
     189,    -1,     4,    -1,     6,    -1,   192,    -1,    -1,    29,
     190,   185,    30,    -1,   193,    29,   191,    30,    -1,   185,
      -1,    25,    -1,     3,    -1,     5,    -1,     3,    18,     3,
      -1,     5,    18,     3,    -1,     5,    18,     5,    -1,     3,
      18,     5,    -1,    66,    -1,    67,    -1,    68,    -1,    69,
      -1,    70,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    88,    88,    92,    93,    94,    99,   100,   104,   105,
     109,   113,   117,   121,   125,   129,   137,   141,   149,   156,
     161,   169,   174,   178,   188,   192,   197,   201,   205,   212,
     216,   223,   224,   225,   226,   230,   237,   244,   245,   249,
     253,   260,   267,   271,   278,   282,   289,   293,   297,   302,
     306,   313,   314,   315,   322,   328,   329,   333,   338,   343,
     352,   357,   365,   370,   372,   373,   377,   384,   385,   389,
     390,   394,   395,   399,   406,   410,   417,   418,   419,   424,
     425,   429,   430,   431,   435,   439,   445,   449,   454,   459,
     464,   471,   472,   478,   479,   487,   488,   494,   495,   496,
     497,   505,   510,   515,   520,   528,   533,   540,   548,   552,
     559,   563,   570,   579,   584,   588,   593,   601,   602,   603,
     607,   616,   623,   630,   637,   638,   642,   649,   653,   654,
     658,   665,   669,   676,   683,   684,   688,   689,   690,   691,
     692,   696,   703,   707,   714,   722,   726,   733,   737,   744,
     748,   752,   753,   757,   765,   766,   771,   775,   782,   789,
     794,   799,   804,   809,   828,   829,   830,   834,   838,   842,
     849,   853,   861,   865,   872,   881,   890,   894,   895,   899,
     908,   917,   918,   922,   926,   930,   937,   938,   943,   947,
     952,   960,   965,   972,   977,   985,   987,   991,   996,  1001,
    1006,  1011,  1016,  1021,  1029,  1031,  1038,  1039,  1043,  1050,
    1052,  1057,  1061,  1065,  1069,  1073,  1077,  1081,  1085,  1092,
    1100,  1104,  1111,  1118,  1120,  1124,  1128,  1135,  1137,  1141,
    1142,  1146,  1153,  1154,  1159,  1164,  1169,  1174,  1182,  1185,
    1190,  1197,  1199,  1207,  1211,  1212,  1216,  1223,  1230,  1231,
    1235,  1243,  1248,  1253,  1254,  1254,  1255,  1259,  1260,  1268,
    1273,  1278,  1284,  1290,  1296,  1307,  1312,  1317,  1322,  1327
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
  "PARAM_OPEN", "PARAM_CLOSE", "SQUARE_OPEN", "SQUARE_CLOSE", "CURLY_OPEN",
  "CURLY_CLOSE", "EQUAL", "UNEQUAL", "LESS", "GREATER", "LESS_OR_EQ",
  "GREATER_OR_EQ", "OR", "AND", "DASH", "COMMA", "NAF", "AT", "WCONS",
  "AMPERSAND", "VEL", "EXISTS", "SQL_SELECT", "SQL_FROM", "SQL_WHERE",
  "SQL_GROUP", "SQL_BY", "SQL_AS", "SQL_COPY", "SQL_TO", "SQL_ORDER",
  "SQL_ASC", "SQL_DESC", "SQL_LIMIT", "SQL_LIKE", "SQL_IN", "SQL_NOT",
  "SQL_SUM", "SQL_MIN", "SQL_MAX", "SQL_AVG", "SQL_COUNT", "SQL_DIALECT",
  "$accept", "HEAD_SEPARATOR", "program", "rules", "rule", "simple_rule",
  "head", "body", "weight_at_levels", "levels_and_terms", "disjunction",
  "conjunction", "choice_atom", "lower_guard", "upper_guard",
  "choice_elements", "choice_element", "choice_element_atom",
  "choice_elements_literals", "naf_literals", "naf_literal",
  "naf_literal_aggregate", "existential_atom", "classic_literal", "atom",
  "ext_atom", "ext_semicol", "named_parameters", "namedParameter", "terms",
  "basic_terms", "builtin_or_list", "builtin_atom", "compareop", "binop",
  "arithop1", "arithop2", "term_", "term__", "term", "expr", "factor",
  "basic_term", "ground_term", "variable_term", "functional_term",
  "list_term", "list_terms_term", "list_head_tail_term", "list_tail_term",
  "vars", "identifier", "directive", "query",
  "lower_guard_compare_aggregate", "upper_guard_compare_aggregate",
  "compare_aggregate", "lower_guard_leftward_left_aggregate",
  "leftward_left_aggregate", "left_aggregate",
  "lower_guard_rightward_left_aggregate", "rightward_left_aggregate",
  "upper_guard_leftward_right_aggregate",
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
       0,    72,    73,    74,    74,    74,    75,    75,    76,    76,
      77,    77,    77,    77,    77,    77,    78,    78,    79,    80,
      80,    81,    81,    81,    82,    82,    82,    82,    82,    83,
      83,    84,    84,    84,    84,    85,    86,    87,    87,    88,
      88,    89,    90,    90,    91,    91,    92,    92,    92,    92,
      92,    93,    93,    93,    94,    95,    95,    96,    96,    96,
      97,    97,    98,    99,    99,    99,   100,   101,   101,   102,
     102,   103,   103,   104,   105,   105,   106,   106,   106,   107,
     107,   108,   108,   108,   109,   109,   109,   109,   110,   110,
     110,   111,   111,   112,   112,   113,   113,   114,   114,   114,
     114,   115,   115,   115,   115,   116,   116,   117,   118,   118,
     119,   119,   120,   121,   121,   122,   122,   123,   123,   123,
     124,   125,   126,   127,   128,   128,   129,   130,   131,   131,
     132,   133,   134,   135,   136,   136,   137,   137,   137,   137,
     137,   138,   139,   140,   140,   141,   141,   142,   142,   143,
     144,   145,   145,   146,   147,   147,   148,   148,   149,   150,
     150,   150,   150,   150,   151,   151,   151,   151,   151,   151,
     152,   153,   154,   155,   156,   157,   157,   158,   158,   159,
     160,   161,   161,   162,   162,   162,   163,   163,   163,   164,
     164,   165,   165,   166,   166,   167,   167,   168,   168,   168,
     168,   168,   168,   168,   169,   169,   170,   170,   170,   171,
     171,   172,   172,   172,   172,   172,   172,   172,   173,   174,
     175,   175,   176,   177,   177,   178,   178,   179,   179,   180,
     180,   181,   182,   182,   182,   182,   182,   182,   183,   183,
     183,   184,   184,   185,   186,   186,   186,   187,   188,   188,
     188,   189,   189,   189,   190,   189,   189,   191,   191,   192,
     192,   192,   192,   192,   192,   193,   193,   193,   193,   193
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
       3,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     4,     2,     3,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       2,     2,     2,     2,     2,     2,     1,     1,     1,     2,
       2,     5,     4,     3,     3,     1,     1,     1,     1,     4,
       3,     1,     3,     1,     3,     1,     3,     5,     1,     1,
       1,     1,     1,     1,     1,     2,     5,     6,     6,     7,
       7,     2,     3,     1,     1,     1,     1,     1,     3,     2,
       2,     1,     3,     2,     2,     4,     1,     3,     4,     1,
       3,     7,     1,     1,     1,     0,     2,     1,     1,     1,
       1,     1,     1,     1,     0,     2,     1,     3,     3,     1,
       3,     1,     5,     5,     6,     5,     6,     1,     3,     1,
       3,     3,     1,     0,     3,     1,     3,     0,     3,     1,
       3,     2,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     0,     2,     1,     1,     3,     3,     1,     1,     3,
       3,     1,     1,     1,     0,     4,     4,     1,     1,     1,
       1,     3,     3,     3,     3,     1,     1,     1,     1,     1
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     5,   117,    89,   119,   118,     0,     0,    84,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     6,
       8,     0,    16,    17,     0,    27,    24,    55,    25,    91,
      95,     0,    92,    93,    87,    88,     9,    15,     0,   120,
     159,   160,   161,   162,   163,     0,     0,     0,    18,    51,
      29,    46,    55,    49,    48,    71,     0,     0,   138,     0,
     128,   136,     0,   129,   137,    53,    52,     0,     0,     0,
       0,     0,    88,   108,   110,    67,     0,   111,     0,    38,
      39,    41,    57,    56,    86,     0,     0,   115,     0,     0,
     173,     0,     3,   164,     0,     1,     7,    10,     0,     2,
       0,     0,   121,    74,    75,   145,   147,   146,   148,    76,
      35,    77,    78,    79,    80,     0,    81,    82,    83,     0,
       0,    85,   119,     0,     0,    47,    50,    13,     0,     0,
      76,     0,    77,    78,   124,   127,   139,     0,   131,   140,
       0,     0,     0,     0,   125,   134,   135,     0,    90,     0,
       0,   109,     0,    33,     0,     0,     0,     0,    54,     0,
       0,   165,   259,   251,   260,   252,   175,   254,   265,   266,
     267,   268,   269,     0,   174,   176,   177,   195,   243,   244,
     247,   248,   253,     0,    11,     0,    28,    26,     0,    89,
      94,    96,    59,     0,    67,     0,     0,     0,    30,    72,
       0,    73,   132,   133,   153,     0,   151,     0,   123,   101,
     103,   105,   102,   100,   106,     0,     0,    69,    97,    98,
      99,     0,     0,   155,    68,   113,   114,   112,    37,    34,
       0,    40,    42,     0,    14,    62,    63,     0,   116,     0,
       0,     0,     0,     0,     0,   204,     0,     0,   179,     0,
       0,     0,     0,     0,    12,    32,    58,   143,   142,   144,
     150,     0,     0,   104,   158,     0,     0,     0,     0,     0,
     149,    36,     0,     0,     0,    64,     0,    63,     0,     0,
     261,   264,   262,   263,     0,   193,   194,   192,     0,     0,
     180,   181,   195,   195,   186,     0,   223,   178,   197,   198,
     203,   202,   201,   200,   199,   196,   245,   246,   249,   250,
     258,   257,     0,    31,   152,   141,   156,    44,    70,     0,
       0,   154,    43,    19,     0,     0,     0,     0,     0,     0,
       0,   166,     0,     0,   255,     0,     0,     0,   184,   183,
       0,     0,   219,   205,   206,   217,     0,   211,     0,   222,
       0,   227,   256,     0,     0,   107,    23,    21,    20,    65,
       0,    66,     0,   167,   171,    63,   168,   195,     0,   182,
     187,   189,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   241,    45,   157,     0,    61,     0,     0,   169,
     185,     0,   188,     0,   208,   207,   218,   221,     0,     0,
     254,   220,   224,   225,     0,     0,   170,    22,    60,   172,
      63,   190,     0,     0,   209,     0,     0,     0,   233,   234,
     235,   236,   237,   228,   229,   238,   232,   242,     0,   215,
     213,     0,     0,     0,   212,   226,     0,   239,   240,   231,
     191,   210,   216,   214,   230
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,   100,    17,    18,    19,    20,    21,    47,   234,   326,
      22,    48,    23,    24,   229,    78,    79,    80,   231,   316,
      49,    50,    25,    51,    52,    53,   236,   274,   275,    74,
     216,    54,    55,   109,   131,   115,   119,    29,    30,   194,
      32,    33,   217,   218,   219,   220,    34,    76,    77,   227,
      88,    72,    36,    37,    57,   144,    58,    59,    60,    61,
      62,    63,   136,   139,    64,    65,    66,    67,   123,   111,
     112,    68,   142,   205,   206,   222,   223,   267,    69,    92,
      93,   333,   366,    94,   173,   174,   175,   176,   245,   290,
     291,   292,   372,   293,   294,   248,   305,   296,   343,   413,
     344,   345,   346,   347,   348,   351,   402,   382,   423,   424,
     425,   439,   406,   349,   178,   179,   180,   181,   243,   312,
     182,   183
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -368
static const short int yypact[] =
{
      61,  -368,  -368,    53,  -368,  -368,    11,   448,  -368,   529,
     484,   295,   529,   448,   295,    94,     6,   169,   389,  -368,
    -368,    85,   123,  -368,   147,  -368,  -368,   192,  -368,  -368,
    -368,   388,    52,   183,  -368,   200,  -368,  -368,   231,  -368,
    -368,  -368,  -368,  -368,  -368,   536,    37,   241,   203,  -368,
    -368,  -368,  -368,  -368,   261,  -368,   388,   330,  -368,   330,
      42,  -368,   330,   229,  -368,  -368,  -368,   291,   388,   279,
     529,   276,  -368,  -368,   270,   267,   285,  -368,    10,  -368,
     302,  -368,   297,  -368,  -368,   306,   301,  -368,    49,   280,
    -368,   315,  -368,   326,   171,  -368,  -368,  -368,   437,  -368,
      28,   295,  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,
    -368,  -368,  -368,  -368,  -368,   198,  -368,  -368,  -368,   198,
     495,  -368,   303,    -8,   295,  -368,  -368,  -368,   448,   529,
     347,   529,   378,   539,  -368,  -368,  -368,   529,  -368,  -368,
     529,   295,   329,   529,  -368,  -368,  -368,   541,  -368,   529,
      15,  -368,   295,   388,   466,   333,   500,   358,  -368,   336,
     280,  -368,   351,  -368,   379,  -368,  -368,  -368,  -368,  -368,
    -368,  -368,  -368,   356,  -368,   359,  -368,   353,    58,  -368,
     121,  -368,  -368,   390,  -368,   403,  -368,  -368,    54,  -368,
     183,  -368,  -368,    86,  -368,   424,   395,   426,  -368,  -368,
     388,  -368,  -368,  -368,  -368,    74,  -368,   541,  -368,   404,
    -368,   405,   406,  -368,  -368,   434,    66,  -368,  -368,  -368,
    -368,   415,   114,  -368,  -368,  -368,  -368,  -368,  -368,  -368,
     529,   401,  -368,   529,  -368,  -368,   529,     4,  -368,   400,
     432,   268,   273,   304,   130,   410,   304,   194,  -368,   304,
     304,   304,   304,   188,  -368,   388,  -368,  -368,  -368,  -368,
    -368,   295,   137,  -368,  -368,   466,   541,   541,   529,   541,
    -368,  -368,   466,    81,    33,  -368,   429,   529,   194,   409,
    -368,  -368,  -368,  -368,   443,  -368,  -368,  -368,     6,   295,
     430,  -368,   353,   353,   190,   316,   421,  -368,  -368,  -368,
    -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,
    -368,  -368,   451,  -368,  -368,  -368,   439,  -368,  -368,    40,
     120,  -368,  -368,  -368,   529,   529,   446,   529,   529,   529,
      33,   464,   480,   463,  -368,   472,   478,   130,  -368,  -368,
     504,   295,   332,    17,  -368,  -368,   316,  -368,   341,  -368,
     455,   458,  -368,   466,   466,  -368,   270,   474,  -368,  -368,
     135,  -368,   529,  -368,  -368,   529,   499,   353,   529,  -368,
    -368,  -368,   143,   316,   316,   201,   304,   492,   473,   346,
     304,   467,   468,  -368,   439,   529,  -368,   151,   168,  -368,
    -368,     4,  -368,   295,  -368,  -368,  -368,  -368,   116,   507,
     280,  -368,   509,  -368,   221,   550,  -368,   270,  -368,  -368,
     529,  -368,   525,   184,  -368,   116,   531,   304,  -368,  -368,
    -368,  -368,  -368,   515,  -368,    92,  -368,  -368,   202,  -368,
    -368,   304,   532,   222,  -368,  -368,   221,  -368,  -368,  -368,
    -368,  -368,  -368,  -368,  -368
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -368,  -368,  -368,  -368,   545,  -368,  -368,     2,  -368,  -368,
    -368,  -368,  -368,  -368,   311,   469,   416,  -368,  -368,   217,
    -148,   445,   475,     5,    38,    29,  -233,  -269,   247,  -117,
     309,  -368,   449,    88,   -30,  -368,  -368,  -368,   461,     0,
    -368,   462,   317,  -368,  -368,  -368,   431,  -368,  -368,  -368,
    -368,    14,  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,
    -368,  -368,   514,   517,  -368,  -368,  -368,  -368,  -368,   214,
     236,   191,  -368,  -368,   325,   380,   319,  -368,  -368,   305,
     -87,  -368,  -368,  -368,  -368,  -368,  -368,   343,  -368,  -368,
     253,  -368,  -368,  -368,  -368,  -276,   313,  -368,   246,   179,
    -185,  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,   159,
    -368,  -368,  -368,   -83,  -368,    35,  -368,    43,  -368,  -368,
    -367,  -368
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -220
static const short int yytable[] =
{
      31,   110,   159,   193,   277,    26,   232,    56,   330,    71,
      75,   177,    84,    56,    35,    85,   338,   339,    31,    39,
     225,    35,   196,    26,   235,    82,    35,    35,    86,    28,
     152,     2,    35,     4,     5,    89,   197,   426,    27,   237,
       2,   328,     4,     5,   153,    71,    10,    28,   149,    81,
      83,   125,     2,   235,     4,     5,    27,    90,   373,   374,
      82,   354,     1,    91,     2,     3,     4,     5,     6,   426,
      84,   124,    38,   240,   152,   126,   113,   327,    15,   105,
     124,   107,   249,     7,   266,    14,   264,   265,   255,     8,
       9,   390,    10,   157,    11,   114,   388,   362,    56,    87,
     185,   250,    82,    97,    12,   187,   260,    98,    13,    14,
     266,    15,    35,   323,    82,    82,   256,   317,   261,   162,
     163,   164,   165,   230,   322,   324,   158,   325,    56,   200,
     149,   201,    16,   285,   269,   286,   287,   202,    82,    81,
     203,   428,    35,   208,   130,   167,   251,   252,   270,   224,
     355,   320,   437,   438,   200,   204,   143,   269,   410,   288,
     284,   221,    83,   177,   149,   386,    82,    90,    35,    95,
     311,   315,    99,   392,   162,   163,   164,   165,   289,   149,
     101,   408,   168,   169,   170,   171,   172,   393,   394,   395,
      81,   162,   163,   164,   165,   149,   166,   298,   409,   299,
     167,     2,   189,     4,     5,   383,   317,   356,   116,   117,
     118,   360,   327,   310,   430,   102,   340,   167,   -57,   341,
     -57,   221,   -57,   -57,   162,   230,   164,     9,   431,   120,
     271,   396,   440,   273,   -57,   121,   276,   168,   169,   170,
     171,   172,   373,   374,   -57,   387,   327,   128,   134,   -57,
     135,   391,   443,   138,   168,   169,   170,   171,   172,   127,
     300,   301,   302,   303,   304,   200,   431,   106,   407,   108,
     132,   280,   200,   281,   137,   204,   282,   276,   283,    35,
     221,   221,   137,   221,   306,   307,    35,   418,   419,   420,
     421,   422,   133,   397,   308,   309,   401,   403,     2,   140,
       4,     5,   129,   336,   140,   141,   148,   162,   163,   164,
     165,   412,   147,   416,   149,   414,   150,   151,   379,   162,
     163,   164,   165,   154,   155,   357,   120,   276,   432,   361,
     156,    90,   414,   167,   435,  -219,  -219,  -219,  -219,    40,
      41,    42,    43,    44,   160,   342,   161,   195,   441,   162,
     163,   164,   165,   200,   200,   371,  -122,  -122,  -122,  -122,
    -122,  -219,   207,   238,   233,   276,   239,    35,    35,   241,
     168,   169,   170,   171,   172,   400,   103,   104,   105,   106,
     107,   108,   168,   169,   170,   171,   172,  -126,  -126,  -126,
    -126,  -126,     2,     3,     4,     5,     6,   242,  -219,  -219,
    -219,  -219,  -219,   246,   376,   377,   378,   411,   244,   247,
     276,     7,   168,   169,   170,   171,   172,     8,     9,   253,
      10,   254,    11,   103,   104,   105,   106,   107,   108,   257,
     258,   259,    12,  -117,  -119,  -118,    13,    14,   263,    15,
       2,     3,     4,     5,   268,   272,    40,    41,    42,    43,
      44,     2,     3,     4,     5,   184,   278,    40,    41,    42,
      43,    44,   279,   295,   329,     8,    45,   332,    10,     2,
       3,     4,     5,   334,   337,   350,     8,    45,   358,    10,
      12,   352,    46,   353,   363,    14,   364,     2,     3,     4,
       5,    12,   365,    46,     8,     9,    14,    10,     2,     3,
       4,     5,   367,     2,     3,     4,     5,   368,   370,    12,
     380,    46,     8,     9,    14,    10,    73,   381,   385,   389,
     235,   398,   404,     8,     9,   192,    10,    70,     8,     9,
     405,    10,     2,     3,     4,     5,   415,   399,    70,     2,
       3,   122,     5,    70,   209,   210,   211,   212,  -130,  -130,
    -130,  -130,  -130,   417,   427,   429,   213,     8,     9,   436,
      10,   434,   442,    96,     8,     9,   313,    10,   228,   214,
     188,   384,    70,   198,   359,   186,   319,   190,   199,    70,
     191,   226,   145,   318,   215,   146,   314,   262,   321,   297,
     369,   331,   375,   335,   433,   444
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
       0,     0,     0,     0,     0,     1,     3,     5,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,    13,
      15,    17,    19,     0,     0,     0,     0,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   254,     0,   254,     0,   254,     0,   254,     0,   254,
       0,   254,     0,   254,     0,   254,     0,   254,     0,   254,
       0
};

static const unsigned short int yycheck[] =
{
       0,    31,    89,   120,   237,     0,   154,     7,   277,     9,
      10,    94,    12,    13,     0,    13,   292,   293,    18,     8,
       5,     7,    30,    18,    20,    11,    12,    13,    14,     0,
      20,     3,    18,     5,     6,    29,    44,   404,     0,   156,
       3,   274,     5,     6,    34,    45,    31,    18,    44,    11,
      12,    46,     3,    20,     5,     6,    18,    51,    41,    42,
      46,    21,     1,    57,     3,     4,     5,     6,     7,   436,
      70,    43,    19,   160,    20,    46,    24,    44,    50,    37,
      43,    39,    24,    22,    44,    48,    20,    21,    34,    28,
      29,   367,    31,    44,    33,    43,   365,   330,    98,     5,
      98,    43,    88,    18,    43,   100,    32,    22,    47,    48,
      44,    50,    98,    32,   100,   101,    30,   265,    44,     3,
       4,     5,     6,   153,   272,    44,    88,    46,   128,   129,
      44,   131,    71,     3,    20,     5,     6,   137,   124,   101,
     140,   410,   128,   143,    56,    29,    25,    26,    34,   149,
      30,   268,    60,    61,   154,   141,    68,    20,   391,    29,
     243,   147,   124,   246,    44,    30,   152,    51,   154,     0,
     253,    34,    49,    30,     3,     4,     5,     6,    48,    44,
      33,    30,    66,    67,    68,    69,    70,    44,   373,   374,
     152,     3,     4,     5,     6,    44,    25,     3,    30,     5,
      29,     3,     4,     5,     6,   353,   354,   324,    25,    26,
      27,   328,    44,    25,    30,    23,    26,    29,    18,    29,
      20,   207,    22,    23,     3,   255,     5,    29,    44,    29,
     230,    30,    30,   233,    34,     4,   236,    66,    67,    68,
      69,    70,    41,    42,    44,   362,    44,    44,    57,    49,
      59,   368,    30,    62,    66,    67,    68,    69,    70,    18,
      66,    67,    68,    69,    70,   265,    44,    38,   385,    40,
      56,     3,   272,     5,    60,   261,     3,   277,     5,   265,
     266,   267,    68,   269,   249,   250,   272,    66,    67,    68,
      69,    70,    56,   376,   251,   252,   379,   380,     3,    63,
       5,     6,    41,   289,    68,    14,    30,     3,     4,     5,
       6,   398,    33,   400,    44,   398,    49,    32,   348,     3,
       4,     5,     6,    21,    18,   325,    29,   327,   415,   329,
      29,    51,   415,    29,   417,     3,     4,     5,     6,     9,
      10,    11,    12,    13,    29,    29,    20,    44,   431,     3,
       4,     5,     6,   353,   354,   341,     9,    10,    11,    12,
      13,    29,    33,     5,    31,   365,    30,   353,   354,    18,
      66,    67,    68,    69,    70,    29,    35,    36,    37,    38,
      39,    40,    66,    67,    68,    69,    70,     9,    10,    11,
      12,    13,     3,     4,     5,     6,     7,    18,    66,    67,
      68,    69,    70,    44,    63,    64,    65,   393,    52,    56,
     410,    22,    66,    67,    68,    69,    70,    28,    29,    29,
      31,    18,    33,    35,    36,    37,    38,    39,    40,     5,
      35,     5,    43,    29,    29,    29,    47,    48,     4,    50,
       3,     4,     5,     6,    29,    44,     9,    10,    11,    12,
      13,     3,     4,     5,     6,    18,    56,     9,    10,    11,
      12,    13,    30,    53,    35,    28,    29,    58,    31,     3,
       4,     5,     6,    30,    44,    54,    28,    29,    32,    31,
      43,    30,    45,    44,    20,    48,     6,     3,     4,     5,
       6,    43,    29,    45,    28,    29,    48,    31,     3,     4,
       5,     6,    30,     3,     4,     5,     6,    29,     4,    43,
      55,    45,    28,    29,    48,    31,    32,    59,    44,    20,
      20,    29,    55,    28,    29,    30,    31,    43,    28,    29,
      62,    31,     3,     4,     5,     6,    29,    64,    43,     3,
       4,     5,     6,    43,     3,     4,     5,     6,     9,    10,
      11,    12,    13,    44,     4,    30,    15,    28,    29,    44,
      31,    30,    30,    18,    28,    29,   255,    31,   152,    28,
     101,   354,    43,   128,   327,   100,   267,   115,   129,    43,
     119,   150,    68,   266,    43,    68,   261,   207,   269,   246,
     337,   278,   346,   288,   415,   436
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    22,    28,    29,
      31,    33,    43,    47,    48,    50,    71,    74,    75,    76,
      77,    78,    82,    84,    85,    94,    95,    96,    97,   109,
     110,   111,   112,   113,   118,   123,   124,   125,    19,     8,
       9,    10,    11,    12,    13,    29,    45,    79,    83,    92,
      93,    95,    96,    97,   103,   104,   111,   126,   128,   129,
     130,   131,   132,   133,   136,   137,   138,   139,   143,   150,
      43,   111,   123,    32,   101,   111,   119,   120,    87,    88,
      89,    96,   123,    96,   111,    79,   123,     5,   122,    29,
      51,    57,   151,   152,   155,     0,    76,    18,    22,    49,
      73,    33,    23,    35,    36,    37,    38,    39,    40,   105,
     106,   141,   142,    24,    43,   107,    25,    26,    27,   108,
      29,     4,     5,   140,    43,    95,    97,    18,    44,    41,
     105,   106,   141,   142,   143,   143,   134,   141,   143,   135,
     142,    14,   144,   105,   127,   134,   135,    33,    30,    44,
      49,    32,    20,    34,    21,    18,    29,    44,    96,   152,
      29,    20,     3,     4,     5,     6,    25,    29,    66,    67,
      68,    69,    70,   156,   157,   158,   159,   185,   186,   187,
     188,   189,   192,   193,    18,    79,    94,    95,    87,     4,
     113,   110,    30,   101,   111,    44,    30,    44,    93,   104,
     111,   111,   111,   111,   123,   145,   146,    33,   111,     3,
       4,     5,     6,    15,    28,    43,   102,   114,   115,   116,
     117,   123,   147,   148,   111,     5,   118,   121,    88,    86,
     106,    90,    92,    31,    80,    20,    98,   101,     5,    30,
     152,    18,    18,   190,    52,   160,    44,    56,   167,    24,
      43,    25,    26,    29,    18,    34,    30,     5,    35,     5,
      32,    44,   147,     4,    20,    21,    44,   149,    29,    20,
      34,   111,    44,   111,    99,   100,   111,    98,    56,    30,
       3,     5,     3,     5,   185,     3,     5,     6,    29,    48,
     161,   162,   163,   165,   166,    53,   169,   159,     3,     5,
      66,    67,    68,    69,    70,   168,   187,   187,   189,   189,
      25,   185,   191,    86,   146,    34,    91,    92,   114,   102,
     101,   148,    92,    32,    44,    46,    81,    44,    98,    35,
      99,   168,    58,   153,    30,   151,   123,    44,   167,   167,
      26,    29,    29,   170,   172,   173,   174,   175,   176,   185,
      54,   177,    30,    44,    21,    30,   101,   111,    32,   100,
     101,   111,    98,    20,     6,    29,   154,    30,    29,   162,
       4,   123,   164,    41,    42,   170,    63,    64,    65,   106,
      55,    59,   179,    92,    91,    44,    30,   101,    99,    20,
     167,   101,    30,    44,   172,   172,    30,   185,    29,    64,
      29,   185,   178,   185,    55,    62,   184,   101,    30,    30,
      98,   123,   152,   171,   185,    29,   152,    44,    66,    67,
      68,    69,    70,   180,   181,   182,   192,     4,    99,    30,
      30,    44,   152,   171,    30,   185,    44,    60,    61,   183,
      30,   185,    30,    30,   181
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
#line 92 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 4:
#line 93 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 5:
#line 94 "src/parser/aspcore2.y"
    { yyerror(director,"Generic error"); }
    break;

  case 10:
#line 110 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 11:
#line 114 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 12:
#line 118 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 13:
#line 122 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onConstraint();
        }
    break;

  case 14:
#line 126 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeakConstraint();
        }
    break;

  case 15:
#line 130 "src/parser/aspcore2.y"
    {
			queryFound=true;
			director.getBuilder()->onQuery();
		}
    break;

  case 16:
#line 138 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHead();
        }
    break;

  case 17:
#line 142 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceAtom();
            director.getBuilder()->onHead();
        }
    break;

  case 18:
#line 150 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBody();
        }
    break;

  case 19:
#line 157 "src/parser/aspcore2.y"
    {
            // There is only the weight. No level and terms.
            director.getBuilder()->onWeightAtLevels(1,0,0);
        }
    break;

  case 20:
#line 162 "src/parser/aspcore2.y"
    {
            // There are also a level and/or some terms.
            // The finalization has been postponed to "level_and_terms".
        }
    break;

  case 21:
#line 170 "src/parser/aspcore2.y"
    {
            // There is no terms following the level.
            director.getBuilder()->onWeightAtLevels(1,1,0);
        }
    break;

  case 22:
#line 175 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeightAtLevels(1,1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.integer));
        }
    break;

  case 23:
#line 179 "src/parser/aspcore2.y"
    {
            // The level is omitted.
            // Thus, the first term, recognized as the
            // weight, should be a term of this list.
            director.getBuilder()->onWeightAtLevels(0,0,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.integer)+1);
        }
    break;

  case 24:
#line 189 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 25:
#line 193 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExtAtom();
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 26:
#line 198 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 27:
#line 202 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 28:
#line 206 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 29:
#line 213 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 30:
#line 217 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 35:
#line 231 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceLowerGuard();
        }
    break;

  case 36:
#line 238 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceUpperGuard();
        }
    break;

  case 39:
#line 250 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 40:
#line 254 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 41:
#line 261 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementAtom();
        }
    break;

  case 42:
#line 268 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 43:
#line 272 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 44:
#line 279 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 45:
#line 283 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 46:
#line 290 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 47:
#line 294 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral(true);
        }
    break;

  case 48:
#line 298 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBuiltinOrList();
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 49:
#line 303 "src/parser/aspcore2.y"
    {
    		director.getBuilder()->onExtAtom();
    	}
    break;

  case 50:
#line 307 "src/parser/aspcore2.y"
    {
			director.getBuilder()->onExtAtom(true);
		}
    break;

  case 53:
#line 316 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregate();
        }
    break;

  case 54:
#line 323 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialAtom();
        }
    break;

  case 55:
#line 328 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(); }
    break;

  case 56:
#line 329 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(true); }
    break;

  case 57:
#line 334 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 58:
#line 339 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string);
        }
    break;

  case 59:
#line 344 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 60:
#line 353 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExternalPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (9))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (9))].yystate.yysemantics.yysval.string);
        }
    break;

  case 61:
#line 358 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onExternalPredicateName((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.string);
      }
    break;

  case 62:
#line 366 "src/parser/aspcore2.y"
    {
	     	director.getBuilder()->onSemicolon();
        }
    break;

  case 66:
#line 378 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onNamedParameter();
    }
    break;

  case 67:
#line 384 "src/parser/aspcore2.y"
    { ((*yyvalp).integer) = 1; }
    break;

  case 68:
#line 385 "src/parser/aspcore2.y"
    { ((*yyvalp).integer) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.integer) + 1; }
    break;

  case 73:
#line 400 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onBuiltinAtom();
    }
    break;

  case 74:
#line 407 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onEqualOperator();
        }
    break;

  case 75:
#line 411 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnequalOperator();
        }
    break;

  case 79:
#line 424 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '+'; }
    break;

  case 80:
#line 425 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '-'; }
    break;

  case 81:
#line 429 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '*'; }
    break;

  case 82:
#line 430 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '/'; }
    break;

  case 83:
#line 431 "src/parser/aspcore2.y"
    { ((*yyvalp).single_char) = '\\'; }
    break;

  case 84:
#line 436 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnknownVariable();
        }
    break;

  case 85:
#line 440 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermRange((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 86:
#line 446 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermDash();
        }
    break;

  case 88:
#line 455 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 89:
#line 460 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 90:
#line 465 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermParams();
        }
    break;

  case 91:
#line 471 "src/parser/aspcore2.y"
    {}
    break;

  case 94:
#line 480 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.single_char));
        }
    break;

  case 95:
#line 487 "src/parser/aspcore2.y"
    {}
    break;

  case 96:
#line 489 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.single_char));
        }
    break;

  case 100:
#line 498 "src/parser/aspcore2.y"
    {
           director.getBuilder()->onAggregateVariableTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
           delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
       }
    break;

  case 101:
#line 506 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 102:
#line 511 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 103:
#line 516 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 104:
#line 521 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string),true);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
        }
    break;

  case 105:
#line 529 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateVariableTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 106:
#line 534 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    break;

  case 107:
#line 541 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunctionalTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string),(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.integer));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.string);
       }
    break;

  case 108:
#line 549 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm(0);
       }
    break;

  case 109:
#line 553 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.integer));
       }
    break;

  case 110:
#line 560 "src/parser/aspcore2.y"
    {
                ((*yyvalp).integer) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.integer);
        }
    break;

  case 111:
#line 564 "src/parser/aspcore2.y"
    {
                ((*yyvalp).integer) = 2;
        }
    break;

  case 112:
#line 571 "src/parser/aspcore2.y"
    {

                director.getBuilder()->onHeadTailList();

        }
    break;

  case 113:
#line 580 "src/parser/aspcore2.y"
    {
                director.getBuilder()->onTerm((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
                delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 115:
#line 589 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 116:
#line 594 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 117:
#line 601 "src/parser/aspcore2.y"
    { ((*yyvalp).string) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string); }
    break;

  case 118:
#line 602 "src/parser/aspcore2.y"
    { ((*yyvalp).string) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string); }
    break;

  case 119:
#line 603 "src/parser/aspcore2.y"
    { ((*yyvalp).string) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string); }
    break;

  case 120:
#line 608 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onDirective((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.string),(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.string);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
        }
    break;

  case 121:
#line 617 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAtom();
        }
    break;

  case 122:
#line 624 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 123:
#line 631 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 126:
#line 643 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 130:
#line 659 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 132:
#line 670 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 133:
#line 677 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 141:
#line 697 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAggregateAssignment();
        }
    break;

  case 143:
#line 708 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAssignVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
            director.getBuilder()->onMultiAssignVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 144:
#line 715 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAssignVariable((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
        }
    break;

  case 145:
#line 723 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOperator();
        }
    break;

  case 146:
#line 727 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOrEqualOperator();
        }
    break;

  case 147:
#line 734 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOperator();
        }
    break;

  case 148:
#line 738 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    break;

  case 153:
#line 758 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 156:
#line 772 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 157:
#line 776 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 158:
#line 783 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroupSemicolon();
        }
    break;

  case 159:
#line 790 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 160:
#line 795 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 161:
#line 800 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 162:
#line 805 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 163:
#line 810 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
            delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
        }
    break;

  case 166:
#line 831 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTopLevelAlias();
    }
    break;

  case 167:
#line 835 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTopLevelAlias();
    }
    break;

  case 168:
#line 839 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 169:
#line 843 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 171:
#line 854 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopyTo((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string));
         delete [] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
    }
    break;

  case 173:
#line 866 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLStart();
    }
    break;

  case 174:
#line 873 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelect();
    }
    break;

  case 175:
#line 882 "src/parser/aspcore2.y"
    {
        char * c = new char[2];
        c[0] = '*';
        c[1] = '\0';
        director.getBuilder()->onSQLQualifiedName(c);
        delete[] c;
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 179:
#line 900 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 180:
#line 909 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFrom();
    }
    break;

  case 183:
#line 923 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTable();
    }
    break;

  case 184:
#line 927 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateFromItem();
    }
    break;

  case 185:
#line 931 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSubQuery();
    }
    break;

  case 187:
#line 939 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateArity((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 189:
#line 948 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateColumn((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 190:
#line 953 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateColumn((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 191:
#line 961 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableName((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.string);
    }
    break;

  case 192:
#line 966 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableNameString((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 193:
#line 973 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 194:
#line 978 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 197:
#line 992 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 198:
#line 997 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
     }
    break;

  case 199:
#line 1002 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 200:
#line 1007 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 201:
#line 1012 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 202:
#line 1017 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 203:
#line 1022 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
          delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
      }
    break;

  case 205:
#line 1032 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhere();
    }
    break;

  case 207:
#line 1040 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLOperatorCondition("AND");
    }
    break;

  case 208:
#line 1044 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLOperatorCondition("OR");
    }
    break;

  case 209:
#line 1051 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLInListValue(); }
    break;

  case 210:
#line 1053 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLInListValue(); }
    break;

  case 211:
#line 1058 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
    }
    break;

  case 212:
#line 1062 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhereSubqueryPredicate();
    }
    break;

  case 213:
#line 1066 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInListPredicate(false);
    }
    break;

  case 214:
#line 1070 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInListPredicate(true);
    }
    break;

  case 215:
#line 1074 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInSubqueryPredicate(false);
    }
    break;

  case 216:
#line 1078 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLInSubqueryPredicate(true);
    }
    break;

  case 218:
#line 1086 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhereGroupEnd();
    }
    break;

  case 219:
#line 1093 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhereGroupBegin();
    }
    break;

  case 220:
#line 1101 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExprOp();
    }
    break;

  case 221:
#line 1105 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLikePredicate();
    }
    break;

  case 222:
#line 1112 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExpr();
    }
    break;

  case 225:
#line 1125 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 226:
#line 1129 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 231:
#line 1147 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderCol();
    }
    break;

  case 233:
#line 1155 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 234:
#line 1160 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 235:
#line 1165 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 236:
#line 1170 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 237:
#line 1175 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 238:
#line 1182 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderModifier(nullptr);
    }
    break;

  case 239:
#line 1186 "src/parser/aspcore2.y"
    {
        char c[] = "asc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 240:
#line 1191 "src/parser/aspcore2.y"
    {
        char c[] = "desc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 242:
#line 1200 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLimit((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.string);
    }
    break;

  case 245:
#line 1213 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAddTerm('+');
    }
    break;

  case 246:
#line 1217 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAddTerm('-');
    }
    break;

  case 247:
#line 1224 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFinalizeMulExpr();
    }
    break;

  case 249:
#line 1232 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLMulTerm('*');
    }
    break;

  case 250:
#line 1236 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLMulTerm('/');
    }
    break;

  case 251:
#line 1244 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 252:
#line 1249 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 254:
#line 1254 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLParenOpen(); }
    break;

  case 255:
#line 1254 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLParenClose(); }
    break;

  case 258:
#line 1261 "src/parser/aspcore2.y"
    {
        char c[] = "*";
        director.getBuilder()->onSQLValue(c);
    }
    break;

  case 259:
#line 1269 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 260:
#line 1274 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 261:
#line 1279 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 262:
#line 1285 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 263:
#line 1291 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 264:
#line 1297 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string));
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.string);
         delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.string);
    }
    break;

  case 265:
#line 1308 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 266:
#line 1313 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 267:
#line 1318 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 268:
#line 1323 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;

  case 269:
#line 1328 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string));
        delete[] (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.string);
    }
    break;


/* Line 930 of glr.c.  */
#line 2786 "src/parser/aspcore2_parser.c"
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



