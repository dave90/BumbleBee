/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

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




/* Copy the first part of user declarations.  */
#line 1 "src/parser/aspcore2.y"

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

bool queryFound=false;




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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 33 "src/parser/aspcore2.y"
{
    char* string;
    char single_char;
    int integer;
}
/* Line 193 of yacc.c.  */
#line 266 "src/parser/aspcore2_parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 279 "src/parser/aspcore2_parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  91
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   539

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  106
/* YYNRULES -- Number of rules.  */
#define YYNRULES  237
/* YYNRULES -- Number of states.  */
#define YYNSTATES  385

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   322

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
      65,    66,    67
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    17,    19,
      21,    24,    28,    33,    37,    42,    44,    46,    48,    50,
      54,    59,    62,    67,    70,    72,    74,    78,    80,    84,
      86,    90,    96,   101,   105,   110,   113,   116,   120,   122,
     124,   128,   130,   132,   136,   138,   142,   144,   147,   149,
     151,   154,   156,   158,   161,   165,   167,   170,   172,   177,
     181,   191,   200,   202,   203,   205,   209,   213,   215,   219,
     221,   225,   227,   231,   235,   237,   239,   241,   243,   245,
     247,   249,   251,   253,   255,   257,   262,   266,   269,   271,
     273,   275,   279,   281,   283,   285,   289,   291,   295,   297,
     299,   301,   303,   305,   307,   309,   312,   314,   316,   321,
     324,   328,   330,   332,   336,   338,   340,   342,   346,   348,
     350,   352,   355,   358,   361,   364,   367,   370,   373,   376,
     378,   380,   383,   386,   389,   392,   395,   398,   400,   402,
     404,   407,   410,   412,   414,   416,   418,   423,   427,   431,
     433,   437,   439,   441,   443,   445,   447,   449,   456,   464,
     467,   471,   473,   475,   477,   479,   481,   485,   488,   491,
     493,   497,   500,   503,   508,   516,   518,   520,   522,   523,
     526,   528,   530,   532,   534,   536,   538,   540,   541,   544,
     546,   550,   554,   558,   560,   561,   565,   567,   571,   572,
     576,   578,   582,   585,   587,   589,   591,   593,   595,   597,
     598,   600,   602,   603,   606,   608,   612,   616,   620,   624,
     626,   628,   630,   632,   636,   641,   643,   645,   647,   649,
     653,   657,   661,   665,   667,   669,   671,   673
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      70,     0,    -1,    48,    -1,    67,   140,    -1,    71,    -1,
       1,    -1,    72,    -1,    71,    72,    -1,    73,    -1,   120,
      -1,    74,    17,    -1,    74,    21,    17,    -1,    74,    21,
      75,    17,    -1,    21,    75,    17,    -1,    46,    75,    17,
      76,    -1,   121,    -1,    78,    -1,    80,    -1,    79,    -1,
      30,   107,    31,    -1,    30,   107,    77,    31,    -1,    45,
     107,    -1,    45,   107,    43,    97,    -1,    43,    97,    -1,
      91,    -1,    93,    -1,    78,    69,    91,    -1,    90,    -1,
      78,    69,    90,    -1,    89,    -1,    79,    43,    89,    -1,
      81,    32,    83,    33,    82,    -1,    81,    32,    83,    33,
      -1,    32,    83,    33,    -1,    32,    83,    33,    82,    -1,
     107,   102,    -1,   102,   107,    -1,    83,    19,    84,    -1,
      84,    -1,    85,    -1,    85,    20,    86,    -1,    92,    -1,
      88,    -1,    86,    43,    88,    -1,    88,    -1,    87,    43,
      88,    -1,    91,    -1,    44,    91,    -1,    99,    -1,    93,
      -1,    44,    93,    -1,    88,    -1,   133,    -1,    44,   133,
      -1,    49,   118,    92,    -1,    92,    -1,    42,    92,    -1,
     119,    -1,   119,    28,    97,    29,    -1,   119,    28,    29,
      -1,    47,   119,    28,    97,    94,    95,    94,    97,    29,
      -1,    47,   119,    28,    94,    95,    94,    97,    29,    -1,
      19,    -1,    -1,    96,    -1,    95,    43,    96,    -1,   107,
      34,   107,    -1,   107,    -1,    97,    43,   107,    -1,   110,
      -1,    98,    43,   110,    -1,   100,    -1,    99,    40,   100,
      -1,   107,   102,   107,    -1,    34,    -1,    35,    -1,   101,
      -1,   134,    -1,   135,    -1,    23,    -1,    42,    -1,    24,
      -1,    25,    -1,    26,    -1,    27,    -1,   119,    28,    97,
      29,    -1,     4,    18,     4,    -1,    42,   107,    -1,   114,
      -1,   119,    -1,     4,    -1,    28,   107,    29,    -1,   105,
      -1,   108,    -1,   109,    -1,   108,   103,   109,    -1,   106,
      -1,   109,   104,   106,    -1,   111,    -1,   112,    -1,   113,
      -1,    14,    -1,     3,    -1,     6,    -1,     4,    -1,    42,
       4,    -1,     5,    -1,    27,    -1,   119,    28,    97,    29,
      -1,    30,    31,    -1,    30,   115,    31,    -1,    97,    -1,
     116,    -1,   107,    48,   117,    -1,     5,    -1,   114,    -1,
       5,    -1,   118,    43,     5,    -1,     3,    -1,     6,    -1,
       5,    -1,     7,     8,    -1,    92,    22,    -1,   107,   101,
      -1,   101,   107,    -1,   122,   136,    -1,   136,   123,    -1,
     107,   134,    -1,   125,   136,    -1,   126,    -1,   129,    -1,
     107,   135,    -1,   128,   136,    -1,   134,   107,    -1,   135,
     107,    -1,   136,   130,    -1,   136,   131,    -1,   127,    -1,
     132,    -1,   124,    -1,   126,   130,    -1,   129,   131,    -1,
      36,    -1,    38,    -1,    37,    -1,    39,    -1,   139,    32,
     137,    33,    -1,   139,    32,    33,    -1,   137,    19,   138,
      -1,   138,    -1,    98,    20,    87,    -1,     9,    -1,    10,
      -1,    11,    -1,    12,    -1,    13,    -1,   141,    -1,    56,
      28,   141,    29,   142,   143,    -1,   144,   145,   149,   156,
     160,   162,   167,    -1,    57,     6,    -1,    28,    95,    29,
      -1,    50,    -1,   146,    -1,    24,    -1,   147,    -1,   148,
      -1,   147,    43,   148,    -1,   168,   154,    -1,    51,   150,
      -1,   151,    -1,   150,    43,   151,    -1,   152,   154,    -1,
     153,   154,    -1,    28,   140,    29,   154,    -1,    47,   119,
      28,    97,    94,    95,    29,    -1,     6,    -1,     3,    -1,
       5,    -1,    -1,    55,   155,    -1,     3,    -1,     5,    -1,
      66,    -1,    65,    -1,    64,    -1,    63,    -1,    62,    -1,
      -1,    52,   157,    -1,   158,    -1,   157,    41,   158,    -1,
     157,    40,   158,    -1,   159,   102,   168,    -1,   168,    -1,
      -1,    53,    54,   161,    -1,   168,    -1,   161,    43,   168,
      -1,    -1,    58,    54,   163,    -1,   164,    -1,   163,    43,
     164,    -1,   165,   166,    -1,   172,    -1,    62,    -1,    63,
      -1,    64,    -1,    65,    -1,    66,    -1,    -1,    59,    -1,
      60,    -1,    -1,    61,     4,    -1,   169,    -1,   168,    23,
     169,    -1,   168,    42,   169,    -1,   168,    24,   169,    -1,
     168,    25,   169,    -1,   170,    -1,     4,    -1,     6,    -1,
     172,    -1,    28,   168,    29,    -1,   173,    28,   171,    29,
      -1,   168,    -1,    24,    -1,     3,    -1,     5,    -1,     3,
      17,     3,    -1,     5,    17,     3,    -1,     5,    17,     5,
      -1,     3,    17,     5,    -1,    62,    -1,    63,    -1,    64,
      -1,    65,    -1,    66,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    81,    81,    85,    86,    87,    92,    93,    97,    98,
     102,   106,   110,   114,   118,   122,   130,   134,   142,   149,
     154,   162,   167,   171,   181,   185,   190,   194,   198,   205,
     209,   216,   217,   218,   219,   223,   230,   237,   238,   242,
     246,   253,   260,   264,   271,   275,   282,   286,   290,   295,
     299,   306,   307,   311,   318,   324,   325,   329,   334,   339,
     348,   353,   361,   366,   368,   369,   373,   380,   381,   385,
     386,   390,   391,   395,   402,   406,   413,   414,   415,   420,
     421,   425,   426,   427,   431,   435,   440,   446,   450,   455,
     460,   465,   472,   473,   479,   480,   488,   489,   495,   496,
     497,   498,   506,   511,   516,   521,   529,   534,   541,   549,
     553,   560,   564,   571,   580,   585,   589,   594,   602,   603,
     604,   608,   617,   624,   631,   638,   639,   643,   650,   654,
     655,   659,   666,   670,   677,   684,   685,   689,   690,   691,
     692,   693,   697,   701,   708,   712,   719,   720,   724,   725,
     730,   737,   742,   747,   752,   757,   776,   777,   784,   788,
     796,   800,   807,   816,   825,   829,   830,   834,   843,   852,
     853,   857,   861,   865,   872,   877,   884,   889,   897,   899,
     903,   908,   913,   918,   923,   928,   933,   941,   943,   950,
     954,   959,   968,   975,   982,   984,   988,   992,   999,  1001,
    1005,  1006,  1010,  1017,  1018,  1023,  1028,  1033,  1038,  1046,
    1049,  1054,  1061,  1063,  1071,  1072,  1076,  1080,  1084,  1091,
    1096,  1101,  1106,  1107,  1108,  1112,  1113,  1121,  1126,  1131,
    1137,  1143,  1149,  1160,  1165,  1170,  1175,  1180
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SYMBOLIC_CONSTANT", "NUMBER",
  "VARIABLE", "STRING", "DIRECTIVE_NAME", "DIRECTIVE_VALUE", "AGGR_COUNT",
  "AGGR_MAX", "AGGR_MIN", "AGGR_SUM", "AGGR_AVG", "ID", "ERROR", "NEWLINE",
  "DOT", "DDOT", "SEMICOLON", "COLON", "CONS", "QUERY_MARK", "PLUS",
  "TIMES", "SLASH", "BACK_SLASH", "ANON_VAR", "PARAM_OPEN", "PARAM_CLOSE",
  "SQUARE_OPEN", "SQUARE_CLOSE", "CURLY_OPEN", "CURLY_CLOSE", "EQUAL",
  "UNEQUAL", "LESS", "GREATER", "LESS_OR_EQ", "GREATER_OR_EQ", "OR", "AND",
  "DASH", "COMMA", "NAF", "AT", "WCONS", "AMPERSAND", "VEL", "EXISTS",
  "SQL_SELECT", "SQL_FROM", "SQL_WHERE", "SQL_GROUP", "SQL_BY", "SQL_AS",
  "SQL_COPY", "SQL_TO", "SQL_ORDER", "SQL_ASC", "SQL_DESC", "SQL_LIMIT",
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
  "aggregate_atom", "leftwardop", "rightwardop", "aggregate",
  "aggregate_elements", "aggregate_element", "aggregate_function", "sql",
  "sql_query", "sql_copy_to", "sql_copy_params", "sql_select", "select",
  "select_list", "select_items", "select_item", "from", "from_list",
  "from_item", "external_table", "table_ref", "opt_alias", "alias_name",
  "opt_where", "search_condition", "predicate", "predicate_value_expr",
  "opt_groupby", "group_list", "opt_orderby", "order_list", "order_col",
  "order_col_name", "modifier_opt", "opt_limit", "value_expr",
  "value_term", "value_primary", "aggregate_arg", "qualified_name",
  "aggregate_func", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    68,    69,    70,    70,    70,    71,    71,    72,    72,
      73,    73,    73,    73,    73,    73,    74,    74,    75,    76,
      76,    77,    77,    77,    78,    78,    78,    78,    78,    79,
      79,    80,    80,    80,    80,    81,    82,    83,    83,    84,
      84,    85,    86,    86,    87,    87,    88,    88,    88,    88,
      88,    89,    89,    89,    90,    91,    91,    92,    92,    92,
      93,    93,    94,    95,    95,    95,    96,    97,    97,    98,
      98,    99,    99,   100,   101,   101,   102,   102,   102,   103,
     103,   104,   104,   104,   105,   105,   105,   105,   105,   106,
     106,   106,   107,   107,   108,   108,   109,   109,   110,   110,
     110,   110,   111,   111,   111,   111,   112,   112,   113,   114,
     114,   115,   115,   116,   117,   117,   118,   118,   119,   119,
     119,   120,   121,   122,   123,   124,   124,   125,   126,   127,
     127,   128,   129,   130,   131,   132,   132,   133,   133,   133,
     133,   133,   134,   134,   135,   135,   136,   136,   137,   137,
     138,   139,   139,   139,   139,   139,   140,   140,   141,   142,
     143,   144,   145,   146,   146,   147,   147,   148,   149,   150,
     150,   151,   151,   151,   152,   152,   153,   153,   154,   154,
     155,   155,   155,   155,   155,   155,   155,   156,   156,   157,
     157,   157,   158,   159,   160,   160,   161,   161,   162,   162,
     163,   163,   164,   165,   165,   165,   165,   165,   165,   166,
     166,   166,   167,   167,   168,   168,   168,   168,   168,   169,
     170,   170,   170,   170,   170,   171,   171,   172,   172,   172,
     172,   172,   172,   173,   173,   173,   173,   173
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     2,     1,     1,
       2,     3,     4,     3,     4,     1,     1,     1,     1,     3,
       4,     2,     4,     2,     1,     1,     3,     1,     3,     1,
       3,     5,     4,     3,     4,     2,     2,     3,     1,     1,
       3,     1,     1,     3,     1,     3,     1,     2,     1,     1,
       2,     1,     1,     2,     3,     1,     2,     1,     4,     3,
       9,     8,     1,     0,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     3,     2,     1,     1,
       1,     3,     1,     1,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     4,     2,
       3,     1,     1,     3,     1,     1,     1,     3,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       1,     2,     2,     2,     2,     2,     2,     1,     1,     1,
       2,     2,     1,     1,     1,     1,     4,     3,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     6,     7,     2,
       3,     1,     1,     1,     1,     1,     3,     2,     2,     1,
       3,     2,     2,     4,     7,     1,     1,     1,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     1,
       3,     3,     3,     1,     0,     3,     1,     3,     0,     3,
       1,     3,     2,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     0,     2,     1,     3,     3,     3,     3,     1,
       1,     1,     1,     3,     4,     1,     1,     1,     1,     3,
       3,     3,     3,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,   118,    90,   120,   119,     0,     0,    84,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     6,
       8,     0,    16,    17,     0,    27,    24,    55,    25,    92,
      96,     0,    93,    94,    88,    89,     9,    15,     0,   121,
     151,   152,   153,   154,   155,     0,     0,    18,    51,    29,
      46,    55,    49,    48,    71,     0,     0,   139,     0,   129,
     137,     0,   130,   138,    52,     0,     0,     0,     0,    89,
     109,   111,    67,     0,   112,     0,    38,    39,    41,    57,
      56,    87,     0,     0,   116,     0,   161,     0,     3,   156,
       0,     1,     7,    10,     0,     2,     0,     0,   122,    74,
      75,   142,   144,   143,   145,    76,    35,    77,    78,    79,
      80,     0,    81,    82,    83,     0,     0,    86,    47,    50,
       0,    53,    13,     0,     0,    76,     0,    77,    78,   125,
     128,   140,     0,   132,   141,     0,     0,   126,   135,   136,
       0,    91,     0,     0,     0,   110,     0,    33,     0,     0,
       0,     0,     0,    54,     0,   227,   220,   228,   221,   163,
       0,   233,   234,   235,   236,   237,     0,   162,   164,   165,
     178,   214,   219,   222,     0,    11,     0,     0,    28,    26,
       0,    90,    95,    89,    97,    59,     0,    67,   123,   127,
     131,    30,    72,     0,    73,   133,   134,   124,   102,   104,
     106,   103,   101,   107,   147,     0,     0,    69,    98,    99,
     100,     0,     0,   149,     0,    68,   114,   115,   113,    37,
      34,     0,     0,    40,    42,     0,     0,    14,    62,    63,
       0,   117,     0,     0,     0,     0,     0,   187,     0,     0,
       0,     0,     0,     0,   167,     0,    12,    32,    58,   105,
       0,     0,     0,     0,   146,    85,    36,     0,    58,     0,
       0,    64,     0,    63,     0,   229,   232,   230,   231,   223,
     176,   177,   175,     0,     0,   168,   169,   178,   178,     0,
     194,   166,   215,   217,   218,   216,   180,   181,   186,   185,
     184,   183,   182,   179,   226,   225,     0,    31,   150,    44,
      70,     0,   148,    43,    19,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   171,   172,   188,
     189,     0,   193,     0,   198,   224,     0,   108,    23,    21,
      20,    65,     0,    66,     0,   159,    63,   157,   178,     0,
     170,     0,     0,     0,     0,     0,   212,    45,     0,    61,
       0,     0,   173,     0,   191,   190,   192,   195,   196,     0,
       0,   158,    22,    60,   160,    63,     0,   204,   205,   206,
     207,   208,   199,   200,   209,   203,   213,     0,   197,     0,
     210,   211,   202,   174,   201
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    96,    17,    18,    19,    20,    21,    46,   227,   307,
      22,    47,    23,    24,   220,    75,    76,    77,   223,   298,
      48,    49,    25,    50,    51,    52,   229,   260,   261,    71,
     206,    53,    54,   105,   126,   111,   115,    29,    30,   187,
      32,    33,   207,   208,   209,   210,    34,    73,    74,   218,
      85,    69,    36,    37,    56,   137,    57,    58,    59,    60,
      61,    62,   131,   134,    63,    64,   107,   108,    65,   212,
     213,    66,    88,    89,   313,   337,    90,   166,   167,   168,
     169,   237,   275,   276,   277,   278,   244,   293,   280,   319,
     320,   321,   324,   357,   346,   372,   373,   374,   382,   361,
     322,   171,   172,   296,   173,   174
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -284
static const yytype_int16 yypact[] =
{
     292,  -284,  -284,   -10,  -284,  -284,    29,   399,  -284,   479,
     445,   161,   479,   399,   161,    43,   140,    85,   324,  -284,
    -284,    -2,    42,  -284,    90,  -284,  -284,   105,  -284,  -284,
    -284,   246,    61,   227,  -284,   491,  -284,  -284,   135,  -284,
    -284,  -284,  -284,  -284,  -284,   427,   125,   108,  -284,  -284,
    -284,  -284,  -284,   131,  -284,   246,   305,  -284,   305,     8,
    -284,   305,    63,  -284,  -284,   246,   127,   479,   164,   199,
    -284,   176,   192,   193,  -284,   101,  -284,   211,  -284,   244,
    -284,  -284,   259,   258,  -284,    52,  -284,   264,  -284,  -284,
     141,  -284,  -284,  -284,   371,  -284,   150,   161,  -284,  -284,
    -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,
    -284,    65,  -284,  -284,  -284,    65,   462,  -284,  -284,  -284,
     246,  -284,  -284,   399,   479,   337,   479,   383,   516,  -284,
    -284,  -284,   479,  -284,  -284,   479,   479,  -284,  -284,  -284,
     358,  -284,   479,   479,    36,  -284,   161,   246,   417,   462,
     275,   475,   289,  -284,   261,   304,  -284,   306,  -284,  -284,
     183,  -284,  -284,  -284,  -284,  -284,   281,  -284,   282,  -284,
     160,  -284,  -284,  -284,   307,  -284,   316,   161,  -284,  -284,
     119,  -284,   227,  -284,  -284,  -284,   129,  -284,  -284,  -284,
    -284,  -284,  -284,   246,  -284,  -284,  -284,  -284,   309,  -284,
     314,   315,  -284,  -284,  -284,   340,    54,  -284,  -284,  -284,
    -284,   325,   149,  -284,   166,  -284,  -284,  -284,  -284,  -284,
    -284,   479,    86,   312,  -284,   171,   479,  -284,  -284,   479,
      32,  -284,   328,   270,   274,   245,   207,   308,   183,   183,
     183,   183,   183,    17,  -284,   238,  -284,   246,   461,  -284,
     417,    59,   479,    59,  -284,  -284,  -284,   417,  -284,   118,
      35,  -284,   333,   479,   301,  -284,  -284,  -284,  -284,  -284,
    -284,  -284,  -284,   140,   161,   326,  -284,   313,   313,   183,
     334,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,
    -284,  -284,  -284,  -284,  -284,   155,   349,  -284,   336,  -284,
    -284,   179,  -284,  -284,  -284,   479,   479,   355,   479,   479,
     479,    35,   384,   361,   368,   378,   207,  -284,  -284,    78,
    -284,   246,   155,   353,   356,  -284,   417,  -284,   176,   373,
    -284,  -284,   187,  -284,   479,  -284,   479,  -284,   313,   479,
    -284,   183,   183,   183,   183,   363,   364,  -284,   479,  -284,
     189,   191,  -284,    32,  -284,  -284,   155,   376,   155,   111,
     420,  -284,   176,  -284,  -284,   479,   183,  -284,  -284,  -284,
    -284,  -284,   385,  -284,    71,  -284,  -284,   196,   155,   111,
    -284,  -284,  -284,  -284,  -284
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -284,  -284,  -284,  -284,   416,  -284,  -284,    -7,  -284,  -284,
    -284,  -284,  -284,  -284,   188,   345,   310,  -284,  -284,  -284,
    -145,   329,   357,    11,    24,    16,  -213,  -259,   152,  -111,
    -284,  -284,   338,    -5,   -30,  -284,  -284,  -284,   343,     0,
    -284,   352,   219,  -284,  -284,  -284,   327,  -284,  -284,  -284,
    -284,    14,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,
    -284,  -284,   412,   421,  -284,   443,   -16,   -32,   251,  -284,
     240,  -284,   228,   360,  -284,  -284,  -284,  -284,  -284,  -284,
     273,  -284,  -284,   200,  -284,  -284,  -225,  -284,  -284,  -284,
     -52,  -284,  -284,  -284,  -284,  -284,   136,  -284,  -284,  -284,
     -88,    19,  -284,  -284,  -283,  -284
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -132
static const yytype_int16 yytable[] =
{
      31,   106,   170,   224,   311,   186,    82,    55,    38,    68,
      72,    26,    81,    55,    35,    93,    28,   263,    31,    94,
     286,    35,   287,   128,    27,    79,    35,    35,    83,    26,
     135,   214,    35,   135,    28,    78,    80,    39,   225,   127,
     230,   216,    27,   132,   101,   120,   103,   309,    84,   132,
     125,   228,   317,   318,   228,     2,   118,     4,     5,    35,
     136,   119,   198,   199,   200,   201,    10,    81,     2,   181,
       4,     5,   235,   202,   250,   143,   375,   351,   308,   288,
     289,   290,   291,   292,   109,    91,   203,   176,   190,     2,
      95,     4,     5,     9,    55,   152,   375,   251,   334,    79,
     102,   205,   104,   110,   189,   299,   377,   179,    35,   153,
      79,    79,   303,   352,   155,   188,   157,   221,   341,   342,
     146,    78,    97,    55,   193,   183,   194,    98,   177,   183,
     380,   381,   195,    14,   147,   196,   197,    35,   146,   117,
     365,   301,   122,   215,   155,   156,   157,   158,   193,   304,
     170,   123,   247,     2,   211,     4,     5,   295,   248,   140,
      79,   305,    35,   306,     2,   159,     4,     5,   253,   160,
      78,   124,   143,   367,   368,   369,   370,   371,   239,   240,
     241,   347,   254,   239,   240,   241,   155,   156,   157,   158,
      86,    79,   177,   141,   328,   255,    87,   242,   332,    15,
     258,    80,   242,   161,   162,   163,   164,   165,   327,   143,
     270,   160,   271,   272,   143,   243,   349,   221,   363,   143,
     364,   256,   143,   350,   145,   383,   259,   142,   353,   262,
     143,   148,   143,   118,   308,   273,    79,   362,   119,   308,
     144,   155,   156,   157,   158,   161,   162,   163,   164,   165,
     193,   112,   113,   114,   274,   356,   358,   193,   282,   283,
     284,   285,   294,   262,    35,   211,   160,   211,   239,   240,
     241,    35,   149,   265,   269,   266,   150,   267,   378,   268,
      99,   100,   101,   102,   103,   104,   151,   242,   315,   354,
     355,   343,   154,     1,   231,     2,     3,     4,     5,     6,
     161,   162,   163,   164,   165,   226,   329,   129,   262,   130,
     333,    86,   133,     7,    40,    41,    42,    43,    44,     8,
       9,   233,    10,   234,    11,   238,   193,     2,     3,     4,
       5,     6,   236,   246,    12,   245,   262,  -118,    13,    14,
      35,    15,  -120,  -119,   249,     7,  -123,  -123,  -123,  -123,
    -123,     8,     9,   252,    10,   257,    11,   264,   312,    16,
     279,   198,   199,   200,   201,   262,    12,   310,   243,   316,
      13,    14,   202,    15,     2,     3,     4,     5,   325,   326,
      40,    41,    42,    43,    44,   203,   330,   323,   175,   336,
     335,   204,  -127,  -127,  -127,  -127,  -127,   338,     8,     9,
     205,    10,     2,     3,     4,     5,   339,   344,    40,    41,
      42,    43,    44,    12,   345,    45,   348,   359,    14,   366,
       2,     3,     4,     5,   376,   360,     8,     9,   379,    10,
       2,     3,     4,     5,    92,   297,    40,    41,    42,    43,
      44,    12,   180,    45,     8,     9,    14,    10,     2,     3,
       4,     5,   191,   178,     8,     9,   219,    10,   184,    12,
     331,   222,   192,   182,    14,     2,     3,     4,     5,    12,
     300,   217,     8,     9,    14,    10,    70,   138,     2,     3,
       4,     5,     2,     3,     4,     5,   139,    67,   121,     8,
       9,   185,    10,   302,   228,   -85,   -85,   -85,   -85,   -85,
     -85,   314,     8,     9,    67,    10,     8,     9,   -57,    10,
     -57,   281,   -57,   -57,   232,   384,   340,    67,     0,   116,
       0,    67,     0,     0,   -57,  -131,  -131,  -131,  -131,  -131,
       0,     0,     0,     0,   -57,     0,     0,     0,     0,   -57
};

static const yytype_int16 yycheck[] =
{
       0,    31,    90,   148,   263,   116,    13,     7,    18,     9,
      10,     0,    12,    13,     0,    17,     0,   230,    18,    21,
       3,     7,     5,    55,     0,    11,    12,    13,    14,    18,
      62,   142,    18,    65,    18,    11,    12,     8,   149,    55,
     151,     5,    18,    59,    36,    45,    38,   260,     5,    65,
      55,    19,   277,   278,    19,     3,    45,     5,     6,    45,
      65,    45,     3,     4,     5,     6,    30,    67,     3,     4,
       5,     6,   160,    14,    20,    43,   359,   336,    43,    62,
      63,    64,    65,    66,    23,     0,    27,    94,   120,     3,
      48,     5,     6,    28,    94,    43,   379,    43,   311,    85,
      37,    42,    39,    42,   120,   250,   365,    96,    94,    85,
      96,    97,   257,   338,     3,   120,     5,   147,    40,    41,
      19,    97,    32,   123,   124,   111,   126,    22,    42,   115,
      59,    60,   132,    47,    33,   135,   136,   123,    19,     4,
     353,   252,    17,   143,     3,     4,     5,     6,   148,    31,
     238,    43,    33,     3,   140,     5,     6,   245,    29,    32,
     146,    43,   148,    45,     3,    24,     5,     6,    19,    28,
     146,    40,    43,    62,    63,    64,    65,    66,    23,    24,
      25,   326,    33,    23,    24,    25,     3,     4,     5,     6,
      50,   177,    42,    29,   305,    29,    56,    42,   309,    49,
      29,   177,    42,    62,    63,    64,    65,    66,    29,    43,
       3,    28,     5,     6,    43,    55,    29,   247,    29,    43,
      29,   221,    43,   334,    31,    29,   226,    28,   339,   229,
      43,    20,    43,   222,    43,    28,   222,   348,   222,    43,
      48,     3,     4,     5,     6,    62,    63,    64,    65,    66,
     250,    24,    25,    26,    47,   343,   344,   257,   239,   240,
     241,   242,    24,   263,   250,   251,    28,   253,    23,    24,
      25,   257,    28,     3,    29,     5,    17,     3,   366,     5,
      34,    35,    36,    37,    38,    39,    28,    42,   274,   341,
     342,   321,    28,     1,     5,     3,     4,     5,     6,     7,
      62,    63,    64,    65,    66,    30,   306,    56,   308,    58,
     310,    50,    61,    21,     9,    10,    11,    12,    13,    27,
      28,    17,    30,    17,    32,    43,   326,     3,     4,     5,
       6,     7,    51,    17,    42,    28,   336,    28,    46,    47,
     326,    49,    28,    28,     4,    21,     9,    10,    11,    12,
      13,    27,    28,    28,    30,    43,    32,    29,    57,    67,
      52,     3,     4,     5,     6,   365,    42,    34,    55,    43,
      46,    47,    14,    49,     3,     4,     5,     6,    29,    43,
       9,    10,    11,    12,    13,    27,    31,    53,    17,    28,
       6,    33,     9,    10,    11,    12,    13,    29,    27,    28,
      42,    30,     3,     4,     5,     6,    28,    54,     9,    10,
      11,    12,    13,    42,    58,    44,    43,    54,    47,    43,
       3,     4,     5,     6,     4,    61,    27,    28,    43,    30,
       3,     4,     5,     6,    18,   247,     9,    10,    11,    12,
      13,    42,    97,    44,    27,    28,    47,    30,     3,     4,
       5,     6,   123,    96,    27,    28,   146,    30,   115,    42,
     308,    44,   124,   111,    47,     3,     4,     5,     6,    42,
     251,   144,    27,    28,    47,    30,    31,    65,     3,     4,
       5,     6,     3,     4,     5,     6,    65,    42,    45,    27,
      28,    29,    30,   253,    19,    34,    35,    36,    37,    38,
      39,   273,    27,    28,    42,    30,    27,    28,    17,    30,
      19,   238,    21,    22,   154,   379,   316,    42,    -1,    28,
      -1,    42,    -1,    -1,    33,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,    48
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    21,    27,    28,
      30,    32,    42,    46,    47,    49,    67,    70,    71,    72,
      73,    74,    78,    80,    81,    90,    91,    92,    93,   105,
     106,   107,   108,   109,   114,   119,   120,   121,    18,     8,
       9,    10,    11,    12,    13,    44,    75,    79,    88,    89,
      91,    92,    93,    99,   100,   107,   122,   124,   125,   126,
     127,   128,   129,   132,   133,   136,   139,    42,   107,   119,
      31,    97,   107,   115,   116,    83,    84,    85,    92,   119,
      92,   107,    75,   119,     5,   118,    50,    56,   140,   141,
     144,     0,    72,    17,    21,    48,    69,    32,    22,    34,
      35,    36,    37,    38,    39,   101,   102,   134,   135,    23,
      42,   103,    24,    25,    26,   104,    28,     4,    91,    93,
     107,   133,    17,    43,    40,   101,   102,   134,   135,   136,
     136,   130,   134,   136,   131,   135,   101,   123,   130,   131,
      32,    29,    28,    43,    48,    31,    19,    33,    20,    28,
      17,    28,    43,    92,    28,     3,     4,     5,     6,    24,
      28,    62,    63,    64,    65,    66,   145,   146,   147,   148,
     168,   169,   170,   172,   173,    17,    75,    42,    90,    91,
      83,     4,   109,   119,   106,    29,    97,   107,   101,   134,
     135,    89,   100,   107,   107,   107,   107,   107,     3,     4,
       5,     6,    14,    27,    33,    42,    98,   110,   111,   112,
     113,   119,   137,   138,    97,   107,     5,   114,   117,    84,
      82,   102,    44,    86,    88,    97,    30,    76,    19,    94,
      97,     5,   141,    17,    17,   168,    51,   149,    43,    23,
      24,    25,    42,    55,   154,    28,    17,    33,    29,     4,
      20,    43,    28,    19,    33,    29,   107,    43,    29,   107,
      95,    96,   107,    94,    29,     3,     5,     3,     5,    29,
       3,     5,     6,    28,    47,   150,   151,   152,   153,    52,
     156,   148,   169,   169,   169,   169,     3,     5,    62,    63,
      64,    65,    66,   155,    24,   168,   171,    82,    87,    88,
     110,    97,   138,    88,    31,    43,    45,    77,    43,    94,
      34,    95,    57,   142,   140,   119,    43,   154,   154,   157,
     158,   159,   168,    53,   160,    29,    43,    29,    97,   107,
      31,    96,    97,   107,    94,     6,    28,   143,    29,    28,
     151,    40,    41,   102,    54,    58,   162,    88,    43,    29,
      97,    95,   154,    97,   158,   158,   168,   161,   168,    54,
      61,   167,    97,    29,    29,    94,    43,    62,    63,    64,
      65,    66,   163,   164,   165,   172,     4,    95,   168,    43,
      59,    60,   166,    29,   164
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (director, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex (director)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, director); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, bumblebee::ParserInputDirector& director)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, director)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    bumblebee::ParserInputDirector& director;
#endif
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

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, bumblebee::ParserInputDirector& director)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, director)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    bumblebee::ParserInputDirector& director;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, director);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, bumblebee::ParserInputDirector& director)
#else
static void
yy_reduce_print (yyvsp, yyrule, director)
    YYSTYPE *yyvsp;
    int yyrule;
    bumblebee::ParserInputDirector& director;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , director);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, director); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
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

      yyarg[0] = yytname[yytype];
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
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
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
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, bumblebee::ParserInputDirector& director)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, director)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    bumblebee::ParserInputDirector& director;
#endif
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


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (bumblebee::ParserInputDirector& director);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (bumblebee::ParserInputDirector& director)
#else
int
yyparse (director)
    bumblebee::ParserInputDirector& director;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 85 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 4:
#line 86 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 5:
#line 87 "src/parser/aspcore2.y"
    { yyerror(director,"Generic error"); }
    break;

  case 10:
#line 103 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 11:
#line 107 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 12:
#line 111 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 13:
#line 115 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onConstraint();
        }
    break;

  case 14:
#line 119 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeakConstraint();
        }
    break;

  case 15:
#line 123 "src/parser/aspcore2.y"
    {
			queryFound=true;
			director.getBuilder()->onQuery();
		}
    break;

  case 16:
#line 131 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHead();
        }
    break;

  case 17:
#line 135 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceAtom();
            director.getBuilder()->onHead();
        }
    break;

  case 18:
#line 143 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBody();
        }
    break;

  case 19:
#line 150 "src/parser/aspcore2.y"
    {
            // There is only the weight. No level and terms.
            director.getBuilder()->onWeightAtLevels(1,0,0);
        }
    break;

  case 20:
#line 155 "src/parser/aspcore2.y"
    {
            // There are also a level and/or some terms.
            // The finalization has been postponed to "level_and_terms".
        }
    break;

  case 21:
#line 163 "src/parser/aspcore2.y"
    {
            // There is no terms following the level.
            director.getBuilder()->onWeightAtLevels(1,1,0);
        }
    break;

  case 22:
#line 168 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeightAtLevels(1,1,(yyvsp[(4) - (4)].integer));
        }
    break;

  case 23:
#line 172 "src/parser/aspcore2.y"
    {
            // The level is omitted.
            // Thus, the first term, recognized as the
            // weight, should be a term of this list.
            director.getBuilder()->onWeightAtLevels(0,0,(yyvsp[(2) - (2)].integer)+1);
        }
    break;

  case 24:
#line 182 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 25:
#line 186 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExtAtom();
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 26:
#line 191 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 27:
#line 195 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 28:
#line 199 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 29:
#line 206 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 30:
#line 210 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 35:
#line 224 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceLowerGuard();
        }
    break;

  case 36:
#line 231 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceUpperGuard();
        }
    break;

  case 39:
#line 243 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 40:
#line 247 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 41:
#line 254 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementAtom();
        }
    break;

  case 42:
#line 261 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 43:
#line 265 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 44:
#line 272 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 45:
#line 276 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 46:
#line 283 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 47:
#line 287 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral(true);
        }
    break;

  case 48:
#line 291 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBuiltinOrList();
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 49:
#line 296 "src/parser/aspcore2.y"
    {
    		director.getBuilder()->onExtAtom();
    	}
    break;

  case 50:
#line 300 "src/parser/aspcore2.y"
    {
			director.getBuilder()->onExtAtom(true);
		}
    break;

  case 52:
#line 308 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregate();
        }
    break;

  case 53:
#line 312 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregate(true);
        }
    break;

  case 54:
#line 319 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialAtom();
        }
    break;

  case 55:
#line 324 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(); }
    break;

  case 56:
#line 325 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(true); }
    break;

  case 57:
#line 330 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 58:
#line 335 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (4)].string));
            delete[] (yyvsp[(1) - (4)].string);
        }
    break;

  case 59:
#line 340 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
        }
    break;

  case 60:
#line 349 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExternalPredicateName((yyvsp[(2) - (9)].string));
            delete[] (yyvsp[(2) - (9)].string);
        }
    break;

  case 61:
#line 354 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onExternalPredicateName((yyvsp[(2) - (8)].string));
          delete[] (yyvsp[(2) - (8)].string);
      }
    break;

  case 62:
#line 362 "src/parser/aspcore2.y"
    {
	     	director.getBuilder()->onSemicolon();
        }
    break;

  case 66:
#line 374 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onNamedParameter();
    }
    break;

  case 67:
#line 380 "src/parser/aspcore2.y"
    { (yyval.integer) = 1; }
    break;

  case 68:
#line 381 "src/parser/aspcore2.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) + 1; }
    break;

  case 73:
#line 396 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onBuiltinAtom();
    }
    break;

  case 74:
#line 403 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onEqualOperator();
        }
    break;

  case 75:
#line 407 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnequalOperator();
        }
    break;

  case 79:
#line 420 "src/parser/aspcore2.y"
    { (yyval.single_char) = '+'; }
    break;

  case 80:
#line 421 "src/parser/aspcore2.y"
    { (yyval.single_char) = '-'; }
    break;

  case 81:
#line 425 "src/parser/aspcore2.y"
    { (yyval.single_char) = '*'; }
    break;

  case 82:
#line 426 "src/parser/aspcore2.y"
    { (yyval.single_char) = '/'; }
    break;

  case 83:
#line 427 "src/parser/aspcore2.y"
    { (yyval.single_char) = '\\'; }
    break;

  case 84:
#line 432 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnknownVariable();
        }
    break;

  case 85:
#line 436 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onFunction((yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
        }
    break;

  case 86:
#line 441 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermRange((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 87:
#line 447 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermDash();
        }
    break;

  case 89:
#line 456 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 90:
#line 461 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 91:
#line 466 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermParams();
        }
    break;

  case 92:
#line 472 "src/parser/aspcore2.y"
    {}
    break;

  case 95:
#line 481 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 96:
#line 488 "src/parser/aspcore2.y"
    {}
    break;

  case 97:
#line 490 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 101:
#line 499 "src/parser/aspcore2.y"
    {
           director.getBuilder()->onAggregateVariableTerm((yyvsp[(1) - (1)].string));
           delete[] (yyvsp[(1) - (1)].string);
       }
    break;

  case 102:
#line 507 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 103:
#line 512 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 104:
#line 517 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 105:
#line 522 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(2) - (2)].string),true);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 106:
#line 530 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateVariableTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 107:
#line 535 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    break;

  case 108:
#line 542 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunctionalTerm((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
       }
    break;

  case 109:
#line 550 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm(0);
       }
    break;

  case 110:
#line 554 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm((yyvsp[(2) - (3)].integer));
       }
    break;

  case 111:
#line 561 "src/parser/aspcore2.y"
    {
                (yyval.integer) = (yyvsp[(1) - (1)].integer);
        }
    break;

  case 112:
#line 565 "src/parser/aspcore2.y"
    {
                (yyval.integer) = 2;
        }
    break;

  case 113:
#line 572 "src/parser/aspcore2.y"
    {

                director.getBuilder()->onHeadTailList();

        }
    break;

  case 114:
#line 581 "src/parser/aspcore2.y"
    {
                director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
                delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 116:
#line 590 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 117:
#line 595 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 118:
#line 602 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 119:
#line 603 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 120:
#line 604 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 121:
#line 609 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onDirective((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string));
            delete[] (yyvsp[(1) - (2)].string);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 122:
#line 618 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAtom();
        }
    break;

  case 123:
#line 625 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 124:
#line 632 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 127:
#line 644 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 131:
#line 660 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 133:
#line 671 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 134:
#line 678 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 142:
#line 698 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOperator();
        }
    break;

  case 143:
#line 702 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOrEqualOperator();
        }
    break;

  case 144:
#line 709 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOperator();
        }
    break;

  case 145:
#line 713 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    break;

  case 150:
#line 731 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 151:
#line 738 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 152:
#line 743 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 153:
#line 748 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 154:
#line 753 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 155:
#line 758 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 157:
#line 778 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 159:
#line 789 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopyTo((yyvsp[(2) - (2)].string));
         delete [] (yyvsp[(2) - (2)].string);
    }
    break;

  case 161:
#line 801 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLStart();
    }
    break;

  case 162:
#line 808 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelect();
    }
    break;

  case 163:
#line 817 "src/parser/aspcore2.y"
    {
        char * c = new char[2];
        c[0] = '*';
        c[1] = '\0';
        director.getBuilder()->onSQLQualifiedName(c);
        delete[] c;
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 167:
#line 835 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 168:
#line 844 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFrom();
    }
    break;

  case 171:
#line 858 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTable();
    }
    break;

  case 172:
#line 862 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFromItem();
    }
    break;

  case 173:
#line 866 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSubQuery();
    }
    break;

  case 174:
#line 873 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableName((yyvsp[(2) - (7)].string));
        delete[] (yyvsp[(2) - (7)].string);
    }
    break;

  case 175:
#line 878 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableNameString((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 176:
#line 885 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 177:
#line 890 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 180:
#line 904 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 181:
#line 909 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
         delete[] (yyvsp[(1) - (1)].string);
     }
    break;

  case 182:
#line 914 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 183:
#line 919 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 184:
#line 924 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 185:
#line 929 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 186:
#line 934 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 188:
#line 944 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhere();
    }
    break;

  case 189:
#line 951 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
    }
    break;

  case 190:
#line 955 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
        director.getBuilder()->onSQLOperatorCondition("AND");
    }
    break;

  case 191:
#line 960 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
        director.getBuilder()->onSQLOperatorCondition("OR");
    }
    break;

  case 192:
#line 969 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExprOp();
    }
    break;

  case 193:
#line 976 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExpr();
    }
    break;

  case 196:
#line 989 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 197:
#line 993 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 202:
#line 1011 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderCol();
    }
    break;

  case 204:
#line 1019 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 205:
#line 1024 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 206:
#line 1029 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 207:
#line 1034 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 208:
#line 1039 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 209:
#line 1046 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderModifier(nullptr);
    }
    break;

  case 210:
#line 1050 "src/parser/aspcore2.y"
    {
        char c[] = "asc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 211:
#line 1055 "src/parser/aspcore2.y"
    {
        char c[] = "desc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 213:
#line 1064 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLimit((yyvsp[(2) - (2)].string));
        delete[] (yyvsp[(2) - (2)].string);
    }
    break;

  case 215:
#line 1073 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('+');
    }
    break;

  case 216:
#line 1077 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('-');
    }
    break;

  case 217:
#line 1081 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('*');
    }
    break;

  case 218:
#line 1085 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('/');
    }
    break;

  case 220:
#line 1097 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 221:
#line 1102 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 226:
#line 1114 "src/parser/aspcore2.y"
    {
        char c[] = "*";
        director.getBuilder()->onSQLValue(c);
    }
    break;

  case 227:
#line 1122 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 228:
#line 1127 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 229:
#line 1132 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 230:
#line 1138 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 231:
#line 1144 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 232:
#line 1150 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 233:
#line 1161 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 234:
#line 1166 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 235:
#line 1171 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 236:
#line 1176 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 237:
#line 1181 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;


/* Line 1267 of yacc.c.  */
#line 3118 "src/parser/aspcore2_parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (director, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (director, yymsg);
	  }
	else
	  {
	    yyerror (director, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, director);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, director);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (director, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, director);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, director);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



