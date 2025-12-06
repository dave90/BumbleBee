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
     SQL_ORDER = 312,
     SQL_ASC = 313,
     SQL_DESC = 314,
     SQL_LIMIT = 315,
     SQL_SUM = 316,
     SQL_MIN = 317,
     SQL_MAX = 318,
     SQL_AVG = 319,
     SQL_COUNT = 320,
     SQL_DIALECT = 321
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
#define SQL_ORDER 312
#define SQL_ASC 313
#define SQL_DESC 314
#define SQL_LIMIT 315
#define SQL_SUM 316
#define SQL_MIN 317
#define SQL_MAX 318
#define SQL_AVG 319
#define SQL_COUNT 320
#define SQL_DIALECT 321




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
#line 264 "src/parser/aspcore2_parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 277 "src/parser/aspcore2_parser.hpp"

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
#define YYFINAL  90
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   523

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  105
/* YYNRULES -- Number of rules.  */
#define YYNRULES  234
/* YYNRULES -- Number of states.  */
#define YYNSTATES  381

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   321

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
      65,    66
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
     221,   225,   229,   231,   233,   235,   237,   239,   241,   243,
     245,   247,   249,   251,   256,   260,   263,   265,   267,   269,
     273,   275,   277,   279,   283,   285,   289,   291,   293,   295,
     297,   299,   301,   304,   306,   308,   313,   316,   320,   322,
     324,   328,   330,   332,   334,   338,   340,   342,   344,   347,
     350,   353,   356,   359,   362,   365,   368,   370,   372,   375,
     378,   381,   384,   387,   390,   392,   394,   396,   399,   402,
     404,   406,   408,   410,   415,   419,   423,   425,   429,   431,
     433,   435,   437,   439,   441,   448,   456,   459,   463,   465,
     467,   469,   471,   473,   477,   480,   483,   485,   489,   492,
     495,   500,   508,   510,   512,   514,   515,   518,   520,   522,
     524,   526,   528,   530,   532,   533,   536,   538,   542,   546,
     550,   552,   553,   557,   559,   563,   564,   568,   570,   574,
     577,   579,   581,   583,   585,   587,   589,   590,   592,   594,
     595,   598,   600,   604,   608,   612,   616,   618,   620,   622,
     624,   628,   633,   635,   637,   639,   641,   645,   649,   653,
     657,   659,   661,   663,   665
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      69,     0,    -1,    45,    -1,    66,   138,    -1,    70,    -1,
       1,    -1,    71,    -1,    70,    71,    -1,    72,    -1,   118,
      -1,    73,    16,    -1,    73,    20,    16,    -1,    73,    20,
      74,    16,    -1,    20,    74,    16,    -1,    43,    74,    16,
      75,    -1,   119,    -1,    77,    -1,    79,    -1,    78,    -1,
      29,   105,    30,    -1,    29,   105,    76,    30,    -1,    42,
     105,    -1,    42,   105,    40,    96,    -1,    40,    96,    -1,
      90,    -1,    92,    -1,    77,    68,    90,    -1,    89,    -1,
      77,    68,    89,    -1,    88,    -1,    78,    40,    88,    -1,
      80,    31,    82,    32,    81,    -1,    80,    31,    82,    32,
      -1,    31,    82,    32,    -1,    31,    82,    32,    81,    -1,
     105,   100,    -1,   100,   105,    -1,    82,    18,    83,    -1,
      83,    -1,    84,    -1,    84,    19,    85,    -1,    91,    -1,
      87,    -1,    85,    40,    87,    -1,    87,    -1,    86,    40,
      87,    -1,    90,    -1,    41,    90,    -1,    98,    -1,    92,
      -1,    41,    92,    -1,    87,    -1,   131,    -1,    41,   131,
      -1,    46,   116,    91,    -1,    91,    -1,    39,    91,    -1,
     117,    -1,   117,    27,    96,    28,    -1,   117,    27,    28,
      -1,    44,   117,    27,    96,    93,    94,    93,    96,    28,
      -1,    44,   117,    27,    93,    94,    93,    96,    28,    -1,
      18,    -1,    -1,    95,    -1,    94,    40,    95,    -1,   105,
      33,   105,    -1,   105,    -1,    96,    40,   105,    -1,   108,
      -1,    97,    40,   108,    -1,   105,   100,   105,    -1,    33,
      -1,    34,    -1,    99,    -1,   132,    -1,   133,    -1,    22,
      -1,    39,    -1,    23,    -1,    24,    -1,    25,    -1,    26,
      -1,   117,    27,    96,    28,    -1,     4,    17,     4,    -1,
      39,   105,    -1,   112,    -1,   117,    -1,     4,    -1,    27,
     105,    28,    -1,   103,    -1,   106,    -1,   107,    -1,   106,
     101,   107,    -1,   104,    -1,   107,   102,   104,    -1,   109,
      -1,   110,    -1,   111,    -1,     3,    -1,     6,    -1,     4,
      -1,    39,     4,    -1,     5,    -1,    26,    -1,   117,    27,
      96,    28,    -1,    29,    30,    -1,    29,   113,    30,    -1,
      96,    -1,   114,    -1,   105,    45,   115,    -1,     5,    -1,
     112,    -1,     5,    -1,   116,    40,     5,    -1,     3,    -1,
       6,    -1,     5,    -1,     7,     8,    -1,    91,    21,    -1,
     105,    99,    -1,    99,   105,    -1,   120,   134,    -1,   134,
     121,    -1,   105,   132,    -1,   123,   134,    -1,   124,    -1,
     127,    -1,   105,   133,    -1,   126,   134,    -1,   132,   105,
      -1,   133,   105,    -1,   134,   128,    -1,   134,   129,    -1,
     125,    -1,   130,    -1,   122,    -1,   124,   128,    -1,   127,
     129,    -1,    35,    -1,    37,    -1,    36,    -1,    38,    -1,
     137,    31,   135,    32,    -1,   137,    31,    32,    -1,   135,
      18,   136,    -1,   136,    -1,    97,    19,    86,    -1,     9,
      -1,    10,    -1,    11,    -1,    12,    -1,    13,    -1,   139,
      -1,    55,    27,   139,    28,   140,   141,    -1,   142,   143,
     147,   154,   158,   160,   165,    -1,    56,     6,    -1,    27,
      94,    28,    -1,    47,    -1,   144,    -1,    23,    -1,   145,
      -1,   146,    -1,   145,    40,   146,    -1,   166,   152,    -1,
      48,   148,    -1,   149,    -1,   148,    40,   149,    -1,   150,
     152,    -1,   151,   152,    -1,    27,   138,    28,   152,    -1,
      44,   117,    27,    96,    93,    94,    28,    -1,     6,    -1,
       3,    -1,     5,    -1,    -1,    52,   153,    -1,     3,    -1,
       5,    -1,    65,    -1,    64,    -1,    63,    -1,    62,    -1,
      61,    -1,    -1,    49,   155,    -1,   156,    -1,   155,    53,
     156,    -1,   155,    54,   156,    -1,   157,   100,   166,    -1,
     166,    -1,    -1,    50,    51,   159,    -1,   166,    -1,   159,
      40,   166,    -1,    -1,    57,    51,   161,    -1,   162,    -1,
     161,    40,   162,    -1,   163,   164,    -1,   170,    -1,    61,
      -1,    62,    -1,    63,    -1,    64,    -1,    65,    -1,    -1,
      58,    -1,    59,    -1,    -1,    60,     4,    -1,   167,    -1,
     166,    22,   167,    -1,   166,    39,   167,    -1,   166,    23,
     167,    -1,   166,    24,   167,    -1,   168,    -1,     4,    -1,
       6,    -1,   170,    -1,    27,   166,    28,    -1,   171,    27,
     169,    28,    -1,   166,    -1,    23,    -1,     3,    -1,     5,
      -1,     3,    16,     3,    -1,     5,    16,     3,    -1,     5,
      16,     5,    -1,     3,    16,     5,    -1,    61,    -1,    62,
      -1,    63,    -1,    64,    -1,    65,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    82,    82,    86,    87,    88,    93,    94,    98,    99,
     103,   107,   111,   115,   119,   123,   131,   135,   143,   150,
     155,   163,   168,   172,   182,   186,   191,   195,   199,   206,
     210,   217,   218,   219,   220,   224,   231,   238,   239,   243,
     247,   254,   261,   265,   272,   276,   283,   287,   291,   295,
     299,   306,   307,   311,   318,   324,   325,   329,   334,   339,
     348,   353,   361,   366,   368,   369,   373,   380,   381,   385,
     386,   390,   397,   401,   408,   409,   410,   415,   416,   420,
     421,   422,   426,   430,   435,   441,   445,   450,   455,   460,
     467,   468,   474,   475,   483,   484,   490,   491,   492,   496,
     501,   506,   511,   519,   524,   531,   539,   543,   550,   554,
     561,   570,   575,   579,   584,   592,   593,   594,   598,   607,
     614,   621,   628,   629,   633,   640,   644,   645,   649,   656,
     660,   667,   674,   675,   679,   680,   681,   682,   683,   687,
     691,   698,   702,   709,   710,   714,   715,   720,   727,   732,
     737,   742,   747,   766,   767,   774,   778,   786,   790,   797,
     806,   815,   819,   820,   824,   833,   842,   843,   847,   851,
     855,   862,   867,   874,   879,   887,   889,   893,   898,   903,
     908,   913,   918,   923,   931,   933,   940,   944,   949,   958,
     965,   972,   974,   978,   982,   989,   991,   995,   996,  1000,
    1007,  1008,  1013,  1018,  1023,  1028,  1036,  1039,  1044,  1051,
    1053,  1061,  1062,  1066,  1070,  1074,  1081,  1086,  1091,  1096,
    1097,  1098,  1102,  1103,  1111,  1116,  1121,  1127,  1133,  1139,
    1150,  1155,  1160,  1165,  1170
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SYMBOLIC_CONSTANT", "NUMBER",
  "VARIABLE", "STRING", "DIRECTIVE_NAME", "DIRECTIVE_VALUE", "AGGR_COUNT",
  "AGGR_MAX", "AGGR_MIN", "AGGR_SUM", "AGGR_AVG", "ERROR", "NEWLINE",
  "DOT", "DDOT", "SEMICOLON", "COLON", "CONS", "QUERY_MARK", "PLUS",
  "TIMES", "SLASH", "BACK_SLASH", "ANON_VAR", "PARAM_OPEN", "PARAM_CLOSE",
  "SQUARE_OPEN", "SQUARE_CLOSE", "CURLY_OPEN", "CURLY_CLOSE", "EQUAL",
  "UNEQUAL", "LESS", "GREATER", "LESS_OR_EQ", "GREATER_OR_EQ", "DASH",
  "COMMA", "NAF", "AT", "WCONS", "AMPERSAND", "VEL", "EXISTS",
  "SQL_SELECT", "SQL_FROM", "SQL_WHERE", "SQL_GROUP", "SQL_BY", "SQL_AS",
  "SQL_AND", "SQL_OR", "SQL_COPY", "SQL_TO", "SQL_ORDER", "SQL_ASC",
  "SQL_DESC", "SQL_LIMIT", "SQL_SUM", "SQL_MIN", "SQL_MAX", "SQL_AVG",
  "SQL_COUNT", "SQL_DIALECT", "$accept", "HEAD_SEPARATOR", "program",
  "rules", "rule", "simple_rule", "head", "body", "weight_at_levels",
  "levels_and_terms", "disjunction", "conjunction", "choice_atom",
  "lower_guard", "upper_guard", "choice_elements", "choice_element",
  "choice_element_atom", "choice_elements_literals", "naf_literals",
  "naf_literal", "naf_literal_aggregate", "existential_atom",
  "classic_literal", "atom", "ext_atom", "ext_semicol", "named_parameters",
  "namedParameter", "terms", "basic_terms", "builtin_atom", "compareop",
  "binop", "arithop1", "arithop2", "term_", "term__", "term", "expr",
  "factor", "basic_term", "ground_term", "variable_term",
  "functional_term", "list_term", "list_terms_term", "list_head_tail_term",
  "list_tail_term", "vars", "identifier", "directive", "query",
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
     315,   316,   317,   318,   319,   320,   321
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    67,    68,    69,    69,    69,    70,    70,    71,    71,
      72,    72,    72,    72,    72,    72,    73,    73,    74,    75,
      75,    76,    76,    76,    77,    77,    77,    77,    77,    78,
      78,    79,    79,    79,    79,    80,    81,    82,    82,    83,
      83,    84,    85,    85,    86,    86,    87,    87,    87,    87,
      87,    88,    88,    88,    89,    90,    90,    91,    91,    91,
      92,    92,    93,    94,    94,    94,    95,    96,    96,    97,
      97,    98,    99,    99,   100,   100,   100,   101,   101,   102,
     102,   102,   103,   103,   103,   103,   103,   104,   104,   104,
     105,   105,   106,   106,   107,   107,   108,   108,   108,   109,
     109,   109,   109,   110,   110,   111,   112,   112,   113,   113,
     114,   115,   115,   116,   116,   117,   117,   117,   118,   119,
     120,   121,   122,   122,   123,   124,   125,   125,   126,   127,
     128,   129,   130,   130,   131,   131,   131,   131,   131,   132,
     132,   133,   133,   134,   134,   135,   135,   136,   137,   137,
     137,   137,   137,   138,   138,   139,   140,   141,   142,   143,
     144,   144,   145,   145,   146,   147,   148,   148,   149,   149,
     149,   150,   150,   151,   151,   152,   152,   153,   153,   153,
     153,   153,   153,   153,   154,   154,   155,   155,   155,   156,
     157,   158,   158,   159,   159,   160,   160,   161,   161,   162,
     163,   163,   163,   163,   163,   163,   164,   164,   164,   165,
     165,   166,   166,   166,   166,   166,   167,   168,   168,   168,
     168,   168,   169,   169,   170,   170,   170,   170,   170,   170,
     171,   171,   171,   171,   171
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
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     3,     2,     1,     1,     1,     3,
       1,     1,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     4,     2,     3,     1,     1,
       3,     1,     1,     1,     3,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     2,     2,
       2,     2,     2,     2,     1,     1,     1,     2,     2,     1,
       1,     1,     1,     4,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     6,     7,     2,     3,     1,     1,
       1,     1,     1,     3,     2,     2,     1,     3,     2,     2,
       4,     7,     1,     1,     1,     0,     2,     1,     1,     1,
       1,     1,     1,     1,     0,     2,     1,     3,     3,     3,
       1,     0,     3,     1,     3,     0,     3,     1,     3,     2,
       1,     1,     1,     1,     1,     1,     0,     1,     1,     0,
       2,     1,     3,     3,     3,     3,     1,     1,     1,     1,
       3,     4,     1,     1,     1,     1,     3,     3,     3,     3,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,   115,    88,   117,   116,     0,     0,    82,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     6,
       8,     0,    16,    17,     0,    27,    24,    55,    25,    90,
      94,     0,    91,    92,    86,    87,     9,    15,     0,   118,
     148,   149,   150,   151,   152,     0,     0,    18,    51,    29,
      46,    55,    49,    48,     0,     0,   136,     0,   126,   134,
       0,   127,   135,    52,     0,     0,     0,     0,    87,   106,
     108,    67,     0,   109,     0,    38,    39,    41,    57,    56,
      85,     0,     0,   113,     0,   158,     0,     3,   153,     0,
       1,     7,    10,     0,     2,     0,     0,   119,    72,    73,
     139,   141,   140,   142,    74,    35,    75,    76,    77,    78,
       0,    79,    80,    81,     0,     0,    84,    47,    50,     0,
      53,    13,     0,    74,     0,    75,    76,   122,   125,   137,
       0,   129,   138,     0,     0,   123,   132,   133,     0,    89,
       0,     0,     0,   107,     0,    33,     0,     0,     0,     0,
       0,    54,     0,   224,   217,   225,   218,   160,     0,   230,
     231,   232,   233,   234,     0,   159,   161,   162,   175,   211,
     216,   219,     0,    11,     0,     0,    28,    26,     0,    88,
      93,    87,    95,    59,     0,    67,   120,   124,   128,    30,
      71,   130,   131,   121,    99,   101,   103,   100,   104,   144,
       0,     0,    69,    96,    97,    98,     0,     0,   146,     0,
      68,   111,   112,   110,    37,    34,     0,     0,    40,    42,
       0,     0,     0,    14,    62,    63,     0,   114,     0,     0,
       0,     0,     0,   184,     0,     0,     0,     0,     0,     0,
     164,     0,    12,    32,    58,   102,     0,     0,     0,     0,
     143,    83,    36,     0,    58,     0,     0,    64,     0,    63,
       0,   226,   229,   227,   228,   220,   173,   174,   172,     0,
       0,   165,   166,   175,   175,     0,   191,   163,   212,   214,
     215,   213,   177,   178,   183,   182,   181,   180,   179,   176,
     223,   222,     0,    31,   147,    44,    70,     0,   145,    43,
      19,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   168,   169,   185,   186,     0,   190,     0,
     195,   221,     0,   105,    23,    21,    20,    65,     0,    66,
       0,   156,    63,   154,   175,     0,   167,     0,     0,     0,
       0,     0,   209,    45,     0,    61,     0,     0,   170,     0,
     187,   188,   189,   192,   193,     0,     0,   155,    22,    60,
     157,    63,     0,   201,   202,   203,   204,   205,   196,   197,
     206,   200,   210,     0,   194,     0,   207,   208,   199,   171,
     198
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    95,    17,    18,    19,    20,    21,    46,   223,   303,
      22,    47,    23,    24,   215,    74,    75,    76,   218,   294,
      48,    49,    25,    50,    51,    52,   225,   256,   257,    70,
     201,    53,   104,   124,   110,   114,    29,    30,   185,    32,
      33,   202,   203,   204,   205,    34,    72,    73,   213,    84,
      68,    36,    37,    55,   135,    56,    57,    58,    59,    60,
      61,   129,   132,    62,    63,   106,   107,    64,   207,   208,
      65,    87,    88,   309,   333,    89,   164,   165,   166,   167,
     233,   271,   272,   273,   274,   240,   289,   276,   315,   316,
     317,   320,   353,   342,   368,   369,   370,   378,   357,   318,
     169,   170,   292,   171,   172
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -282
static const yytype_int16 yypact[] =
{
      52,  -282,  -282,    13,  -282,  -282,     6,   359,  -282,   428,
     309,   287,   428,   359,   287,    81,   -23,   102,   245,  -282,
    -282,   291,    82,  -282,   105,  -282,  -282,   123,  -282,  -282,
    -282,   462,    53,   326,  -282,   448,  -282,  -282,   153,  -282,
    -282,  -282,  -282,  -282,  -282,   370,   135,   128,  -282,  -282,
    -282,  -282,  -282,  -282,   462,   270,  -282,   270,   210,  -282,
     270,   220,  -282,  -282,   462,   155,   428,   150,   188,  -282,
     154,   156,   227,  -282,    44,  -282,   185,  -282,   191,  -282,
    -282,   247,   239,  -282,   174,  -282,   259,  -282,  -282,   166,
    -282,  -282,  -282,   314,  -282,   264,   287,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,
     170,  -282,  -282,  -282,   170,   413,  -282,  -282,  -282,   462,
    -282,  -282,   359,   395,   428,   498,   503,  -282,  -282,  -282,
     428,  -282,  -282,   428,   428,  -282,  -282,  -282,   444,  -282,
     428,   428,    34,  -282,   287,   462,   386,   413,   292,   417,
     323,  -282,   282,   318,  -282,   340,  -282,  -282,   233,  -282,
    -282,  -282,  -282,  -282,   294,  -282,   317,  -282,    28,  -282,
    -282,  -282,   332,  -282,   344,   287,  -282,  -282,   113,  -282,
     326,  -282,  -282,  -282,    -3,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,   339,  -282,   350,   351,  -282,  -282,
     363,    63,  -282,  -282,  -282,  -282,   357,   122,  -282,    92,
    -282,  -282,  -282,  -282,  -282,  -282,   428,    84,   347,  -282,
     462,   107,   428,  -282,  -282,   428,    24,  -282,   365,   328,
     349,   277,   111,   345,   233,   233,   233,   233,   233,   101,
    -282,   179,  -282,   462,   468,  -282,   386,   455,   428,   455,
    -282,  -282,  -282,   386,  -282,   118,   103,  -282,   362,   428,
     346,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,   -23,
     287,   361,  -282,   358,   358,   233,   374,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,    46,   383,  -282,   388,  -282,  -282,   159,  -282,  -282,
    -282,   428,   428,   396,   428,   428,   428,   103,   423,   409,
     410,   418,   111,  -282,  -282,   180,  -282,   462,    46,   400,
     380,  -282,   386,  -282,   154,   422,  -282,  -282,   160,  -282,
     428,  -282,   428,  -282,   358,   428,  -282,   233,   233,   233,
     233,   402,   403,  -282,   428,  -282,   163,   167,  -282,    24,
    -282,  -282,    46,   425,    46,   147,   467,  -282,   154,  -282,
    -282,   428,   233,  -282,  -282,  -282,  -282,  -282,   432,  -282,
     165,  -282,  -282,   177,    46,   147,  -282,  -282,  -282,  -282,
    -282
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -282,  -282,  -282,  -282,   456,  -282,  -282,    -5,  -282,  -282,
    -282,  -282,  -282,  -282,   230,   381,   334,  -282,  -282,  -282,
    -141,   360,   384,     2,    23,     3,  -207,  -248,   181,  -109,
    -282,  -282,   -10,   -30,  -282,  -282,  -282,   372,     0,  -282,
     377,   237,  -282,  -282,  -282,   348,  -282,  -282,  -282,  -282,
      15,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,   427,   453,  -282,   447,     7,   -18,   218,  -282,   240,
    -282,   249,   367,  -282,  -282,  -282,  -282,  -282,  -282,   286,
    -282,  -282,   209,  -282,  -282,  -257,  -282,  -282,  -282,   -29,
    -282,  -282,  -282,  -282,  -282,   148,  -282,  -282,  -282,   -85,
     109,  -282,  -282,  -281,  -282
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -129
static const yytype_int16 yytable[] =
{
      31,   105,    26,    28,   168,   219,   184,    54,    81,    67,
      71,   307,    80,    54,    39,    35,   313,   314,    31,   259,
      26,    28,    35,    27,    85,   244,    78,    35,    35,    82,
      38,   209,    86,    35,    77,    79,   126,   141,   221,   211,
     226,    27,   224,   133,   123,   119,   133,   117,   118,   305,
     235,   236,   237,     1,   134,     2,     3,     4,     5,     6,
      35,   125,   144,    10,   141,   130,    80,   238,   235,   236,
     237,   130,     7,   231,   371,   108,   145,   348,     8,     9,
     239,    10,   246,    11,   347,   238,    83,     2,   174,     4,
       5,    12,   109,    54,   371,    13,    14,   177,    15,    78,
     330,   188,    90,   247,   282,   295,   283,   151,    35,   186,
      78,    78,   299,   373,   266,   216,   267,   268,    16,    77,
     251,   224,    54,   175,   190,   181,   187,    94,    14,   181,
     191,   144,   141,   192,   193,   254,    96,    35,   269,   297,
     249,   210,   361,   304,    97,   243,   220,   141,   300,   168,
     153,   121,   155,   206,   250,   270,   291,   116,   301,    78,
     302,    35,   284,   285,   286,   287,   288,    77,   122,   153,
     154,   155,   156,     2,   179,     4,     5,     2,   139,     4,
       5,   343,   153,   154,   155,   156,   138,   323,   345,   157,
      78,   359,   324,   158,   141,   360,   328,     9,    79,   141,
     141,   142,   290,   141,   146,   379,   158,   304,   363,   364,
     365,   366,   367,   216,   150,   140,   252,   304,   147,   117,
     118,   346,   255,   376,   377,   258,   349,   159,   160,   161,
     162,   163,    78,   337,   338,   358,   153,   154,   155,   156,
     159,   160,   161,   162,   163,   100,   220,   102,     2,     3,
       4,     5,     6,   220,   352,   354,   101,   143,   103,   258,
     158,    35,   206,   148,   206,     7,   149,     2,    35,     4,
       5,     8,     9,   127,    10,   128,    11,   374,   131,    40,
      41,    42,    43,    44,    12,   311,   152,   339,    13,    14,
       2,    15,     4,     5,   159,   160,   161,   162,   163,   235,
     236,   237,   325,   175,   258,   265,   329,    92,   350,   351,
      15,    93,     2,     3,     4,     5,   238,     2,     3,     4,
       5,   222,   220,    40,    41,    42,    43,    44,   227,    85,
     173,   261,   258,   262,   229,     8,     9,    35,    10,    69,
       8,     9,   232,    10,   278,   279,   280,   281,    66,   111,
     112,   113,   263,    12,   264,    45,   230,   234,    14,   241,
     242,   258,     2,     3,     4,     5,  -115,   245,    40,    41,
      42,    43,    44,     2,     3,     4,     5,  -117,  -116,    40,
      41,    42,    43,    44,   248,     8,     9,   253,    10,     2,
       3,     4,     5,   260,   275,   306,     8,     9,    12,    10,
      45,   312,   308,    14,  -120,  -120,  -120,  -120,  -120,    12,
     239,   321,     8,     9,    14,    10,     2,     3,     4,     5,
       2,     3,     4,     5,   319,    12,   326,   217,   322,   331,
      14,     2,     3,     4,     5,   224,   332,   341,   334,     8,
       9,   183,    10,     8,     9,   335,    10,   194,   195,   196,
     197,   340,    66,   355,     8,     9,    66,    10,   194,   195,
     196,   197,   344,   356,   -57,   362,   -57,    66,   -57,   -57,
     198,   372,   375,   293,    91,   115,   199,   178,   214,   176,
     -57,   198,   189,   200,   296,   327,   182,   180,   -57,   298,
     212,   136,   120,   -57,   200,    98,    99,   100,   101,   102,
     103,   -83,   -83,   -83,   -83,   -83,   -83,  -124,  -124,  -124,
    -124,  -124,  -128,  -128,  -128,  -128,  -128,   137,   310,   228,
     277,   336,     0,   380
};

static const yytype_int16 yycheck[] =
{
       0,    31,     0,     0,    89,   146,   115,     7,    13,     9,
      10,   259,    12,    13,     8,     0,   273,   274,    18,   226,
      18,    18,     7,     0,    47,    28,    11,    12,    13,    14,
      17,   140,    55,    18,    11,    12,    54,    40,   147,     5,
     149,    18,    18,    61,    54,    45,    64,    45,    45,   256,
      22,    23,    24,     1,    64,     3,     4,     5,     6,     7,
      45,    54,    18,    29,    40,    58,    66,    39,    22,    23,
      24,    64,    20,   158,   355,    22,    32,   334,    26,    27,
      52,    29,    19,    31,   332,    39,     5,     3,    93,     5,
       6,    39,    39,    93,   375,    43,    44,    95,    46,    84,
     307,   119,     0,    40,     3,   246,     5,    84,    93,   119,
      95,    96,   253,   361,     3,   145,     5,     6,    66,    96,
      28,    18,   122,    39,   124,   110,   119,    45,    44,   114,
     130,    18,    40,   133,   134,    28,    31,   122,    27,   248,
      18,   141,   349,    40,    21,    32,   146,    40,    30,   234,
       3,    16,     5,   138,    32,    44,   241,     4,    40,   144,
      42,   146,    61,    62,    63,    64,    65,   144,    40,     3,
       4,     5,     6,     3,     4,     5,     6,     3,    28,     5,
       6,   322,     3,     4,     5,     6,    31,    28,    28,    23,
     175,    28,   301,    27,    40,    28,   305,    27,   175,    40,
      40,    45,    23,    40,    19,    28,    27,    40,    61,    62,
      63,    64,    65,   243,    40,    27,   216,    40,    27,   217,
     217,   330,   222,    58,    59,   225,   335,    61,    62,    63,
      64,    65,   217,    53,    54,   344,     3,     4,     5,     6,
      61,    62,    63,    64,    65,    35,   246,    37,     3,     4,
       5,     6,     7,   253,   339,   340,    36,    30,    38,   259,
      27,   246,   247,    16,   249,    20,    27,     3,   253,     5,
       6,    26,    27,    55,    29,    57,    31,   362,    60,     9,
      10,    11,    12,    13,    39,   270,    27,   317,    43,    44,
       3,    46,     5,     6,    61,    62,    63,    64,    65,    22,
      23,    24,   302,    39,   304,    28,   306,    16,   337,   338,
      46,    20,     3,     4,     5,     6,    39,     3,     4,     5,
       6,    29,   322,     9,    10,    11,    12,    13,     5,    47,
      16,     3,   332,     5,    16,    26,    27,   322,    29,    30,
      26,    27,    48,    29,   235,   236,   237,   238,    39,    23,
      24,    25,     3,    39,     5,    41,    16,    40,    44,    27,
      16,   361,     3,     4,     5,     6,    27,     4,     9,    10,
      11,    12,    13,     3,     4,     5,     6,    27,    27,     9,
      10,    11,    12,    13,    27,    26,    27,    40,    29,     3,
       4,     5,     6,    28,    49,    33,    26,    27,    39,    29,
      41,    40,    56,    44,     9,    10,    11,    12,    13,    39,
      52,    28,    26,    27,    44,    29,     3,     4,     5,     6,
       3,     4,     5,     6,    50,    39,    30,    41,    40,     6,
      44,     3,     4,     5,     6,    18,    27,    57,    28,    26,
      27,    28,    29,    26,    27,    27,    29,     3,     4,     5,
       6,    51,    39,    51,    26,    27,    39,    29,     3,     4,
       5,     6,    40,    60,    16,    40,    18,    39,    20,    21,
      26,     4,    40,   243,    18,    27,    32,    96,   144,    95,
      32,    26,   122,    39,   247,   304,   114,   110,    40,   249,
     142,    64,    45,    45,    39,    33,    34,    35,    36,    37,
      38,    33,    34,    35,    36,    37,    38,     9,    10,    11,
      12,    13,     9,    10,    11,    12,    13,    64,   269,   152,
     234,   312,    -1,   375
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    20,    26,    27,
      29,    31,    39,    43,    44,    46,    66,    69,    70,    71,
      72,    73,    77,    79,    80,    89,    90,    91,    92,   103,
     104,   105,   106,   107,   112,   117,   118,   119,    17,     8,
       9,    10,    11,    12,    13,    41,    74,    78,    87,    88,
      90,    91,    92,    98,   105,   120,   122,   123,   124,   125,
     126,   127,   130,   131,   134,   137,    39,   105,   117,    30,
      96,   105,   113,   114,    82,    83,    84,    91,   117,    91,
     105,    74,   117,     5,   116,    47,    55,   138,   139,   142,
       0,    71,    16,    20,    45,    68,    31,    21,    33,    34,
      35,    36,    37,    38,    99,   100,   132,   133,    22,    39,
     101,    23,    24,    25,   102,    27,     4,    90,    92,   105,
     131,    16,    40,    99,   100,   132,   133,   134,   134,   128,
     132,   134,   129,   133,    99,   121,   128,   129,    31,    28,
      27,    40,    45,    30,    18,    32,    19,    27,    16,    27,
      40,    91,    27,     3,     4,     5,     6,    23,    27,    61,
      62,    63,    64,    65,   143,   144,   145,   146,   166,   167,
     168,   170,   171,    16,    74,    39,    89,    90,    82,     4,
     107,   117,   104,    28,    96,   105,    99,   132,   133,    88,
     105,   105,   105,   105,     3,     4,     5,     6,    26,    32,
      39,    97,   108,   109,   110,   111,   117,   135,   136,    96,
     105,     5,   112,   115,    83,    81,   100,    41,    85,    87,
     105,    96,    29,    75,    18,    93,    96,     5,   139,    16,
      16,   166,    48,   147,    40,    22,    23,    24,    39,    52,
     152,    27,    16,    32,    28,     4,    19,    40,    27,    18,
      32,    28,   105,    40,    28,   105,    94,    95,   105,    93,
      28,     3,     5,     3,     5,    28,     3,     5,     6,    27,
      44,   148,   149,   150,   151,    49,   154,   146,   167,   167,
     167,   167,     3,     5,    61,    62,    63,    64,    65,   153,
      23,   166,   169,    81,    86,    87,   108,    96,   136,    87,
      30,    40,    42,    76,    40,    93,    33,    94,    56,   140,
     138,   117,    40,   152,   152,   155,   156,   157,   166,    50,
     158,    28,    40,    28,    96,   105,    30,    95,    96,   105,
      93,     6,    27,   141,    28,    27,   149,    53,    54,   100,
      51,    57,   160,    87,    40,    28,    96,    94,   152,    96,
     156,   156,   166,   159,   166,    51,    60,   165,    96,    28,
      28,    93,    40,    61,    62,    63,    64,    65,   161,   162,
     163,   170,     4,    94,   166,    40,    58,    59,   164,    28,
     162
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
#line 86 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 4:
#line 87 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 5:
#line 88 "src/parser/aspcore2.y"
    { yyerror(director,"Generic error"); }
    break;

  case 10:
#line 104 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 11:
#line 108 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 12:
#line 112 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 13:
#line 116 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onConstraint();
        }
    break;

  case 14:
#line 120 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeakConstraint();
        }
    break;

  case 15:
#line 124 "src/parser/aspcore2.y"
    {
			queryFound=true;
			director.getBuilder()->onQuery();
		}
    break;

  case 16:
#line 132 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHead();
        }
    break;

  case 17:
#line 136 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceAtom();
            director.getBuilder()->onHead();
        }
    break;

  case 18:
#line 144 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBody();
        }
    break;

  case 19:
#line 151 "src/parser/aspcore2.y"
    {
            // There is only the weight. No level and terms.
            director.getBuilder()->onWeightAtLevels(1,0,0);
        }
    break;

  case 20:
#line 156 "src/parser/aspcore2.y"
    {
            // There are also a level and/or some terms.
            // The finalization has been postponed to "level_and_terms".
        }
    break;

  case 21:
#line 164 "src/parser/aspcore2.y"
    {
            // There is no terms following the level.
            director.getBuilder()->onWeightAtLevels(1,1,0);
        }
    break;

  case 22:
#line 169 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeightAtLevels(1,1,(yyvsp[(4) - (4)].integer));
        }
    break;

  case 23:
#line 173 "src/parser/aspcore2.y"
    {
            // The level is omitted.
            // Thus, the first term, recognized as the
            // weight, should be a term of this list.
            director.getBuilder()->onWeightAtLevels(0,0,(yyvsp[(2) - (2)].integer)+1);
        }
    break;

  case 24:
#line 183 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 25:
#line 187 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExtAtom();
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 26:
#line 192 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 27:
#line 196 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 28:
#line 200 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 29:
#line 207 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 30:
#line 211 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 35:
#line 225 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceLowerGuard();
        }
    break;

  case 36:
#line 232 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceUpperGuard();
        }
    break;

  case 39:
#line 244 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 40:
#line 248 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 41:
#line 255 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementAtom();
        }
    break;

  case 42:
#line 262 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 43:
#line 266 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 44:
#line 273 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 45:
#line 277 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 46:
#line 284 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 47:
#line 288 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral(true);
        }
    break;

  case 48:
#line 292 "src/parser/aspcore2.y"
    {
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

  case 71:
#line 391 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBuiltinAtom();
        }
    break;

  case 72:
#line 398 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onEqualOperator();
        }
    break;

  case 73:
#line 402 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnequalOperator();
        }
    break;

  case 77:
#line 415 "src/parser/aspcore2.y"
    { (yyval.single_char) = '+'; }
    break;

  case 78:
#line 416 "src/parser/aspcore2.y"
    { (yyval.single_char) = '-'; }
    break;

  case 79:
#line 420 "src/parser/aspcore2.y"
    { (yyval.single_char) = '*'; }
    break;

  case 80:
#line 421 "src/parser/aspcore2.y"
    { (yyval.single_char) = '/'; }
    break;

  case 81:
#line 422 "src/parser/aspcore2.y"
    { (yyval.single_char) = '\\'; }
    break;

  case 82:
#line 427 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnknownVariable();
        }
    break;

  case 83:
#line 431 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onFunction((yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
        }
    break;

  case 84:
#line 436 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermRange((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 85:
#line 442 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermDash();
        }
    break;

  case 87:
#line 451 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 88:
#line 456 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 89:
#line 461 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermParams();
        }
    break;

  case 90:
#line 467 "src/parser/aspcore2.y"
    {}
    break;

  case 93:
#line 476 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 94:
#line 483 "src/parser/aspcore2.y"
    {}
    break;

  case 95:
#line 485 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 99:
#line 497 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 100:
#line 502 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 101:
#line 507 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 102:
#line 512 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(2) - (2)].string),true);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 103:
#line 520 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateVariableTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 104:
#line 525 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    break;

  case 105:
#line 532 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunctionalTerm((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
       }
    break;

  case 106:
#line 540 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm(0);
       }
    break;

  case 107:
#line 544 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm((yyvsp[(2) - (3)].integer));
       }
    break;

  case 108:
#line 551 "src/parser/aspcore2.y"
    {
                (yyval.integer) = (yyvsp[(1) - (1)].integer);
        }
    break;

  case 109:
#line 555 "src/parser/aspcore2.y"
    {
                (yyval.integer) = 2;
        }
    break;

  case 110:
#line 562 "src/parser/aspcore2.y"
    {

                director.getBuilder()->onHeadTailList();

        }
    break;

  case 111:
#line 571 "src/parser/aspcore2.y"
    {
                director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
                delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 113:
#line 580 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 114:
#line 585 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 115:
#line 592 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 116:
#line 593 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 117:
#line 594 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 118:
#line 599 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onDirective((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string));
            delete[] (yyvsp[(1) - (2)].string);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 119:
#line 608 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAtom();
        }
    break;

  case 120:
#line 615 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 121:
#line 622 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 124:
#line 634 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 128:
#line 650 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 130:
#line 661 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 131:
#line 668 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 139:
#line 688 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOperator();
        }
    break;

  case 140:
#line 692 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOrEqualOperator();
        }
    break;

  case 141:
#line 699 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOperator();
        }
    break;

  case 142:
#line 703 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    break;

  case 147:
#line 721 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 148:
#line 728 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 149:
#line 733 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 150:
#line 738 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 151:
#line 743 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 152:
#line 748 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 154:
#line 768 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 156:
#line 779 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopyTo((yyvsp[(2) - (2)].string));
         delete [] (yyvsp[(2) - (2)].string);
    }
    break;

  case 158:
#line 791 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLStart();
    }
    break;

  case 159:
#line 798 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelect();
    }
    break;

  case 160:
#line 807 "src/parser/aspcore2.y"
    {
        char * c = new char[2];
        c[0] = '*';
        c[1] = '\0';
        director.getBuilder()->onSQLQualifiedName(c);
        delete[] c;
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 164:
#line 825 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 165:
#line 834 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFrom();
    }
    break;

  case 168:
#line 848 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTable();
    }
    break;

  case 169:
#line 852 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFromItem();
    }
    break;

  case 170:
#line 856 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSubQuery();
    }
    break;

  case 171:
#line 863 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableName((yyvsp[(2) - (7)].string));
        delete[] (yyvsp[(2) - (7)].string);
    }
    break;

  case 172:
#line 868 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableNameString((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 173:
#line 875 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 174:
#line 880 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 177:
#line 894 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 178:
#line 899 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
         delete[] (yyvsp[(1) - (1)].string);
     }
    break;

  case 179:
#line 904 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 180:
#line 909 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 181:
#line 914 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 182:
#line 919 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 183:
#line 924 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 185:
#line 934 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhere();
    }
    break;

  case 186:
#line 941 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
    }
    break;

  case 187:
#line 945 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
        director.getBuilder()->onSQLOperatorCondition("AND");
    }
    break;

  case 188:
#line 950 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
        director.getBuilder()->onSQLOperatorCondition("OR");
    }
    break;

  case 189:
#line 959 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExprOp();
    }
    break;

  case 190:
#line 966 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExpr();
    }
    break;

  case 193:
#line 979 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 194:
#line 983 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 199:
#line 1001 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderCol();
    }
    break;

  case 201:
#line 1009 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 202:
#line 1014 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 203:
#line 1019 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 204:
#line 1024 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 205:
#line 1029 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 206:
#line 1036 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderModifier(nullptr);
    }
    break;

  case 207:
#line 1040 "src/parser/aspcore2.y"
    {
        char c[] = "asc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 208:
#line 1045 "src/parser/aspcore2.y"
    {
        char c[] = "desc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 210:
#line 1054 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLimit((yyvsp[(2) - (2)].string));
        delete[] (yyvsp[(2) - (2)].string);
    }
    break;

  case 212:
#line 1063 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('+');
    }
    break;

  case 213:
#line 1067 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('-');
    }
    break;

  case 214:
#line 1071 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('*');
    }
    break;

  case 215:
#line 1075 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('/');
    }
    break;

  case 217:
#line 1087 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 218:
#line 1092 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 223:
#line 1104 "src/parser/aspcore2.y"
    {
        char c[] = "*";
        director.getBuilder()->onSQLValue(c);
    }
    break;

  case 224:
#line 1112 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 225:
#line 1117 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 226:
#line 1122 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 227:
#line 1128 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 228:
#line 1134 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 229:
#line 1140 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 230:
#line 1151 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 231:
#line 1156 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 232:
#line 1161 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 233:
#line 1166 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 234:
#line 1171 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;


/* Line 1267 of yacc.c.  */
#line 3104 "src/parser/aspcore2_parser.hpp"
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



