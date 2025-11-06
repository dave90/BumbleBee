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
#line 254 "src/parser/aspcore2_parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 267 "src/parser/aspcore2_parser.hpp"

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
#define YYLAST   479

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  207
/* YYNRULES -- Number of states.  */
#define YYNSTATES  347

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   316

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
      55,    56,    57,    58,    59,    60,    61
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
     181,   191,   193,   195,   199,   203,   205,   209,   211,   215,
     219,   221,   223,   225,   227,   229,   231,   233,   235,   237,
     239,   241,   246,   250,   253,   255,   257,   259,   263,   265,
     267,   269,   273,   275,   279,   281,   283,   285,   287,   289,
     291,   294,   296,   298,   303,   306,   310,   312,   314,   318,
     320,   322,   324,   328,   330,   332,   334,   337,   340,   343,
     346,   349,   352,   355,   358,   360,   362,   365,   368,   371,
     374,   377,   380,   382,   384,   386,   389,   392,   394,   396,
     398,   400,   405,   409,   413,   415,   419,   421,   423,   425,
     427,   429,   431,   438,   444,   447,   451,   453,   455,   457,
     459,   461,   465,   468,   471,   473,   477,   480,   483,   488,
     496,   498,   500,   501,   504,   506,   508,   509,   512,   514,
     518,   522,   526,   528,   529,   533,   535,   539,   541,   545,
     549,   553,   557,   559,   561,   563,   565,   569,   574,   576,
     578,   582,   586,   590,   594,   596,   598,   600
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      64,     0,    -1,    45,    -1,    61,   133,    -1,    65,    -1,
       1,    -1,    66,    -1,    65,    66,    -1,    67,    -1,   113,
      -1,    68,    16,    -1,    68,    20,    16,    -1,    68,    20,
      69,    16,    -1,    20,    69,    16,    -1,    43,    69,    16,
      70,    -1,   114,    -1,    72,    -1,    74,    -1,    73,    -1,
      29,   100,    30,    -1,    29,   100,    71,    30,    -1,    42,
     100,    -1,    42,   100,    40,    91,    -1,    40,    91,    -1,
      85,    -1,    87,    -1,    72,    63,    85,    -1,    84,    -1,
      72,    63,    84,    -1,    83,    -1,    73,    40,    83,    -1,
      75,    31,    77,    32,    76,    -1,    75,    31,    77,    32,
      -1,    31,    77,    32,    -1,    31,    77,    32,    76,    -1,
     100,    95,    -1,    95,   100,    -1,    77,    18,    78,    -1,
      78,    -1,    79,    -1,    79,    19,    80,    -1,    86,    -1,
      82,    -1,    80,    40,    82,    -1,    82,    -1,    81,    40,
      82,    -1,    85,    -1,    41,    85,    -1,    93,    -1,    87,
      -1,    41,    87,    -1,    82,    -1,   126,    -1,    41,   126,
      -1,    46,   111,    86,    -1,    86,    -1,    39,    86,    -1,
     112,    -1,   112,    27,    91,    28,    -1,   112,    27,    28,
      -1,    44,   112,    27,    91,    88,    89,    88,    91,    28,
      -1,    18,    -1,    90,    -1,    89,    40,    90,    -1,   100,
      33,   100,    -1,   100,    -1,    91,    40,   100,    -1,   103,
      -1,    92,    40,   103,    -1,   100,    95,   100,    -1,    33,
      -1,    34,    -1,    94,    -1,   127,    -1,   128,    -1,    22,
      -1,    39,    -1,    23,    -1,    24,    -1,    25,    -1,    26,
      -1,   112,    27,    91,    28,    -1,     4,    17,     4,    -1,
      39,   100,    -1,   107,    -1,   112,    -1,     4,    -1,    27,
     100,    28,    -1,    98,    -1,   101,    -1,   102,    -1,   101,
      96,   102,    -1,    99,    -1,   102,    97,    99,    -1,   104,
      -1,   105,    -1,   106,    -1,     3,    -1,     6,    -1,     4,
      -1,    39,     4,    -1,     5,    -1,    26,    -1,   112,    27,
      91,    28,    -1,    29,    30,    -1,    29,   108,    30,    -1,
      91,    -1,   109,    -1,   100,    45,   110,    -1,     5,    -1,
     107,    -1,     5,    -1,   111,    40,     5,    -1,     3,    -1,
       6,    -1,     5,    -1,     7,     8,    -1,    86,    21,    -1,
     100,    94,    -1,    94,   100,    -1,   115,   129,    -1,   129,
     116,    -1,   100,   127,    -1,   118,   129,    -1,   119,    -1,
     122,    -1,   100,   128,    -1,   121,   129,    -1,   127,   100,
      -1,   128,   100,    -1,   129,   123,    -1,   129,   124,    -1,
     120,    -1,   125,    -1,   117,    -1,   119,   123,    -1,   122,
     124,    -1,    35,    -1,    37,    -1,    36,    -1,    38,    -1,
     132,    31,   130,    32,    -1,   132,    31,    32,    -1,   130,
      18,   131,    -1,   131,    -1,    92,    19,    81,    -1,     9,
      -1,    10,    -1,    11,    -1,    12,    -1,    13,    -1,   134,
      -1,    55,    27,   134,    28,   135,   136,    -1,   137,   138,
     142,   149,   153,    -1,    56,     6,    -1,    27,    89,    28,
      -1,    47,    -1,   139,    -1,    23,    -1,   140,    -1,   141,
      -1,   140,    40,   141,    -1,   155,   147,    -1,    48,   143,
      -1,   144,    -1,   143,    40,   144,    -1,   145,   147,    -1,
     146,   147,    -1,    27,   133,    28,   147,    -1,    44,   112,
      27,    91,    88,    89,    28,    -1,     3,    -1,     5,    -1,
      -1,    52,   148,    -1,     3,    -1,     5,    -1,    -1,    49,
     150,    -1,   151,    -1,   150,    53,   150,    -1,   150,    54,
     150,    -1,   152,    95,   155,    -1,   155,    -1,    -1,    50,
      51,   154,    -1,   155,    -1,   154,    40,   155,    -1,   156,
      -1,   155,    22,   156,    -1,   155,    39,   156,    -1,   155,
      23,   156,    -1,   155,    24,   156,    -1,   157,    -1,     4,
      -1,     6,    -1,   158,    -1,    27,   155,    28,    -1,   159,
      27,   155,    28,    -1,     3,    -1,     5,    -1,     3,    16,
       3,    -1,     5,    16,     3,    -1,     5,    16,     5,    -1,
       3,    16,     5,    -1,    57,    -1,    58,    -1,    59,    -1,
      60,    -1
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
     348,   356,   362,   363,   367,   374,   375,   379,   380,   384,
     391,   395,   402,   403,   404,   409,   410,   414,   415,   416,
     420,   424,   429,   435,   439,   444,   449,   454,   461,   462,
     468,   469,   477,   478,   484,   485,   486,   490,   495,   500,
     505,   513,   518,   525,   533,   537,   544,   548,   555,   564,
     569,   573,   578,   586,   587,   588,   592,   601,   608,   615,
     622,   623,   627,   634,   638,   639,   643,   650,   654,   661,
     668,   669,   673,   674,   675,   676,   677,   681,   685,   692,
     696,   703,   704,   708,   709,   714,   721,   726,   731,   736,
     741,   757,   758,   765,   769,   777,   781,   788,   797,   806,
     810,   811,   815,   824,   833,   834,   838,   842,   846,   853,
     860,   865,   873,   875,   879,   884,   892,   894,   901,   905,
     909,   917,   924,   931,   933,   937,   941,   949,   950,   954,
     958,   962,   969,   974,   979,   984,   985,   986,   990,   995,
    1000,  1006,  1012,  1018,  1029,  1034,  1039,  1044
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
  "SQL_AND", "SQL_OR", "SQL_COPY", "SQL_TO", "SQL_SUM", "SQL_MIN",
  "SQL_MAX", "SQL_AVG", "SQL_DIALECT", "$accept", "HEAD_SEPARATOR",
  "program", "rules", "rule", "simple_rule", "head", "body",
  "weight_at_levels", "levels_and_terms", "disjunction", "conjunction",
  "choice_atom", "lower_guard", "upper_guard", "choice_elements",
  "choice_element", "choice_element_atom", "choice_elements_literals",
  "naf_literals", "naf_literal", "naf_literal_aggregate",
  "existential_atom", "classic_literal", "atom", "extAtom", "extSemicol",
  "namedParameters", "namedParameter", "terms", "basic_terms",
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
  "aggregate_atom", "leftwardop", "rightwardop", "aggregate",
  "aggregate_elements", "aggregate_element", "aggregate_function", "sql",
  "sql_query", "sql_copy_to", "sql_copy_params", "sql_select", "select",
  "select_list", "select_items", "select_item", "from", "from_list",
  "from_item", "external_table", "table_ref", "opt_alias", "alias_name",
  "opt_where", "search_condition", "predicate", "predicate_value_expr",
  "opt_groupby", "group_list", "value_expr", "value_term", "value_primary",
  "qualified_name", "aggregate_func", 0
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
     315,   316
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    62,    63,    64,    64,    64,    65,    65,    66,    66,
      67,    67,    67,    67,    67,    67,    68,    68,    69,    70,
      70,    71,    71,    71,    72,    72,    72,    72,    72,    73,
      73,    74,    74,    74,    74,    75,    76,    77,    77,    78,
      78,    79,    80,    80,    81,    81,    82,    82,    82,    82,
      82,    83,    83,    83,    84,    85,    85,    86,    86,    86,
      87,    88,    89,    89,    90,    91,    91,    92,    92,    93,
      94,    94,    95,    95,    95,    96,    96,    97,    97,    97,
      98,    98,    98,    98,    98,    99,    99,    99,   100,   100,
     101,   101,   102,   102,   103,   103,   103,   104,   104,   104,
     104,   105,   105,   106,   107,   107,   108,   108,   109,   110,
     110,   111,   111,   112,   112,   112,   113,   114,   115,   116,
     117,   117,   118,   119,   120,   120,   121,   122,   123,   124,
     125,   125,   126,   126,   126,   126,   126,   127,   127,   128,
     128,   129,   129,   130,   130,   131,   132,   132,   132,   132,
     132,   133,   133,   134,   135,   136,   137,   138,   139,   139,
     140,   140,   141,   142,   143,   143,   144,   144,   144,   145,
     146,   146,   147,   147,   148,   148,   149,   149,   150,   150,
     150,   151,   152,   153,   153,   154,   154,   155,   155,   155,
     155,   155,   156,   157,   157,   157,   157,   157,   158,   158,
     158,   158,   158,   158,   159,   159,   159,   159
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
       9,     1,     1,     3,     3,     1,     3,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     3,     2,     1,     1,     1,     3,     1,     1,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     4,     2,     3,     1,     1,     3,     1,
       1,     1,     3,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     1,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     2,     2,     1,     1,     1,
       1,     4,     3,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     6,     5,     2,     3,     1,     1,     1,     1,
       1,     3,     2,     2,     1,     3,     2,     2,     4,     7,
       1,     1,     0,     2,     1,     1,     0,     2,     1,     3,
       3,     3,     1,     0,     3,     1,     3,     1,     3,     3,
       3,     3,     1,     1,     1,     1,     3,     4,     1,     1,
       3,     3,     3,     3,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,   113,    86,   115,   114,     0,     0,    80,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     6,
       8,     0,    16,    17,     0,    27,    24,    55,    25,    88,
      92,     0,    89,    90,    84,    85,     9,    15,     0,   116,
     146,   147,   148,   149,   150,     0,     0,    18,    51,    29,
      46,    55,    49,    48,     0,     0,   134,     0,   124,   132,
       0,   125,   133,    52,     0,     0,     0,     0,    85,   104,
     106,    65,     0,   107,     0,    38,    39,    41,    57,    56,
      83,     0,     0,   111,     0,   156,     0,     3,   151,     0,
       1,     7,    10,     0,     2,     0,     0,   117,    70,    71,
     137,   139,   138,   140,    72,    35,    73,    74,    75,    76,
       0,    77,    78,    79,     0,     0,    82,    47,    50,     0,
      53,    13,     0,    72,     0,    73,    74,   120,   123,   135,
       0,   127,   136,     0,     0,   121,   130,   131,     0,    87,
       0,     0,     0,   105,     0,    33,     0,     0,     0,     0,
       0,    54,     0,   198,   193,   199,   194,   158,     0,   204,
     205,   206,   207,     0,   157,   159,   160,   172,   187,   192,
     195,     0,    11,     0,     0,    28,    26,     0,    86,    91,
      85,    93,    59,     0,    65,   118,   122,   126,    30,    69,
     128,   129,   119,    97,    99,   101,    98,   102,   142,     0,
       0,    67,    94,    95,    96,     0,     0,   144,     0,    66,
     109,   110,   108,    37,    34,     0,     0,    40,    42,     0,
       0,     0,    14,     0,   112,     0,     0,     0,     0,     0,
     176,     0,     0,     0,     0,     0,     0,   162,     0,    12,
      32,    58,   100,     0,     0,     0,     0,   141,    81,    36,
       0,    58,     0,    61,     0,     0,   200,   203,   201,   202,
     196,   170,   171,     0,     0,   163,   164,   172,   172,     0,
     183,   161,   188,   190,   191,   189,   174,   175,   173,     0,
      31,   145,    44,    68,     0,   143,    43,    19,     0,     0,
       0,     0,    62,     0,     0,     0,     0,     0,     0,   166,
     167,   177,   178,     0,   182,     0,   153,   197,     0,   103,
      23,    21,    20,     0,     0,     0,   154,     0,   152,   172,
       0,   165,     0,     0,     0,     0,    45,     0,    63,     0,
      64,     0,   168,     0,   179,   180,   181,   184,   185,    22,
      60,   155,     0,     0,     0,   186,   169
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    95,    17,    18,    19,    20,    21,    46,   222,   290,
      22,    47,    23,    24,   214,    74,    75,    76,   217,   281,
      48,    49,    25,    50,    51,    52,   254,   291,   292,    70,
     200,    53,   104,   124,   110,   114,    29,    30,   184,    32,
      33,   201,   202,   203,   204,    34,    72,    73,   212,    84,
      68,    36,    37,    55,   135,    56,    57,    58,    59,    60,
      61,   129,   132,    62,    63,   106,   107,    64,   206,   207,
      65,    87,    88,   295,   318,    89,   163,   164,   165,   166,
     230,   265,   266,   267,   268,   237,   278,   270,   301,   302,
     303,   306,   337,   304,   168,   169,   170,   171
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -293
static const yytype_int16 yypact[] =
{
     151,  -293,  -293,    32,  -293,  -293,    57,   386,  -293,   201,
     306,    82,   201,   386,    82,    64,   162,    92,   300,  -293,
    -293,    47,    69,  -293,   105,  -293,  -293,   129,  -293,  -293,
    -293,   328,    37,   168,  -293,   251,  -293,  -293,   165,  -293,
    -293,  -293,  -293,  -293,  -293,   397,   171,   176,  -293,  -293,
    -293,  -293,  -293,  -293,   328,   275,  -293,   275,   197,  -293,
     275,   226,  -293,  -293,   328,   180,   201,   205,   214,  -293,
     184,   204,   222,  -293,    68,  -293,   234,  -293,   236,  -293,
    -293,   250,   243,  -293,    51,  -293,   247,  -293,  -293,   115,
    -293,  -293,  -293,   344,  -293,    67,    82,  -293,  -293,  -293,
    -293,  -293,  -293,  -293,  -293,  -293,  -293,  -293,  -293,  -293,
     196,  -293,  -293,  -293,   196,   440,  -293,  -293,  -293,   328,
    -293,  -293,   386,   422,   201,   438,   449,  -293,  -293,  -293,
     201,  -293,  -293,   201,   201,  -293,  -293,  -293,    77,  -293,
     201,   201,     9,  -293,    82,   328,   413,   440,   261,   201,
     289,  -293,   255,   281,  -293,   302,  -293,  -293,   241,  -293,
    -293,  -293,  -293,   271,  -293,   282,  -293,    55,  -293,  -293,
    -293,   294,  -293,   308,    82,  -293,  -293,    72,  -293,   168,
    -293,  -293,  -293,    95,  -293,  -293,  -293,  -293,  -293,  -293,
    -293,  -293,  -293,   298,  -293,   301,   303,  -293,  -293,   330,
      21,  -293,  -293,  -293,  -293,   310,    99,  -293,   148,  -293,
    -293,  -293,  -293,  -293,  -293,   201,   142,   312,  -293,   328,
     156,   201,  -293,    28,  -293,   313,   277,   290,   104,    31,
     291,   241,   241,   241,   241,   241,   311,  -293,   241,  -293,
     328,   341,  -293,   413,   159,   201,   159,  -293,  -293,  -293,
     413,  -293,   178,  -293,   201,   295,  -293,  -293,  -293,  -293,
    -293,  -293,  -293,   162,    82,   318,  -293,   286,   286,   241,
     309,  -293,  -293,  -293,  -293,  -293,  -293,  -293,  -293,   253,
    -293,   327,  -293,  -293,   185,  -293,  -293,  -293,   201,   201,
     338,    34,  -293,   336,   366,   353,   354,   357,    31,  -293,
    -293,   -34,  -293,   328,   144,   335,  -293,  -293,   413,  -293,
     184,   347,  -293,   201,   201,   201,  -293,   201,  -293,   286,
     201,  -293,   241,   241,   241,   241,  -293,   201,  -293,   186,
    -293,   208,  -293,    28,  -293,   340,   144,   364,   144,   184,
    -293,  -293,   201,   241,   211,   144,  -293
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -293,  -293,  -293,  -293,   363,  -293,  -293,    -9,  -293,  -293,
    -293,  -293,  -293,  -293,   154,   315,   270,  -293,  -293,  -293,
    -138,   283,   325,     3,     5,     6,  -280,  -292,   108,   -85,
    -293,  -293,   -17,   -30,  -293,  -293,  -293,   314,     0,  -293,
     319,   193,  -293,  -293,  -293,   280,  -293,  -293,  -293,  -293,
      15,  -293,  -293,  -293,  -293,  -293,  -293,  -293,  -293,  -293,
    -293,   374,   389,  -293,   410,   -23,   -22,   200,  -293,   210,
    -293,   202,   322,  -293,  -293,  -293,  -293,  -293,  -293,   232,
    -293,  -293,   166,  -293,  -293,  -224,  -293,  -293,  -183,  -293,
    -293,  -293,  -293,   -87,   238,  -293,  -293,  -293
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -127
static const yytype_int16 yytable[] =
{
      31,   105,   167,    26,    81,    27,    28,    54,   218,    67,
      71,   314,    80,    54,   210,    35,    77,    79,    31,   322,
     323,    26,    35,    27,    28,   331,    78,    35,    35,    82,
     183,   125,   126,    35,   261,   130,   262,   123,    10,   133,
     243,   130,   133,   299,   300,   119,   253,   134,   117,    38,
     344,   118,   253,   342,     2,   208,     4,     5,   263,   108,
      35,   244,   220,    92,   223,    39,    80,    93,   141,    83,
       2,   228,     4,     5,   313,   264,   109,   232,   233,   234,
     193,   194,   195,   196,   173,     2,   144,     4,     5,   151,
     144,   150,    90,    54,   235,   332,   186,   187,   176,    78,
     145,    77,   185,   197,   240,   282,   174,   236,    35,   198,
      78,    78,   286,    15,    94,   215,   199,   246,   153,   154,
     155,   156,    54,   241,   189,   180,   232,   233,   234,   180,
     190,   247,   260,   191,   192,   141,    96,    35,   157,   334,
     335,   209,   158,   235,   167,     2,   219,     4,     5,    77,
      97,   279,     1,   205,     2,     3,     4,     5,     6,    78,
     284,    35,   193,   194,   195,   196,   232,   233,   234,   116,
     326,     7,   159,   160,   161,   162,   248,     8,     9,    79,
      10,   174,    11,   235,   251,   197,    14,   121,   141,    78,
      12,   111,   112,   113,    13,    14,   141,    15,   199,     2,
     178,     4,     5,   310,     2,     3,     4,     5,   287,    85,
     215,   138,    16,   309,   340,   249,   122,    86,   288,   117,
     289,   252,   118,     9,   141,   141,   141,     8,     9,   329,
      10,    78,   100,   139,   102,   333,   341,   336,   338,   346,
      66,   140,   339,   219,   153,   154,   155,   156,   313,   142,
     219,   313,   143,   146,   293,   127,   345,   128,    35,   205,
     131,   205,   101,   147,   103,    35,   148,   -57,   158,   -57,
     149,   -57,   -57,   324,   152,   232,   233,   234,   115,   297,
     256,   307,   257,   -57,    40,    41,    42,    43,    44,   311,
     221,   -57,   235,   258,   224,   259,   -57,   226,   159,   160,
     161,   162,    85,     2,     3,     4,     5,     6,   219,     2,
       3,     4,     5,   293,   276,   330,   277,   293,   227,   229,
       7,   238,   231,    35,   239,  -113,     8,     9,  -115,    10,
    -114,    11,     8,     9,   242,    10,    69,   245,   236,    12,
     269,   255,   293,    13,    14,    66,    15,     2,     3,     4,
       5,   294,   250,    40,    41,    42,    43,    44,   298,   305,
     172,    98,    99,   100,   101,   102,   103,   308,   312,   315,
       8,     9,   316,    10,   -81,   -81,   -81,   -81,   -81,   -81,
     317,    91,   319,    12,   320,    45,   325,   327,    14,     2,
       3,     4,     5,   322,   280,    40,    41,    42,    43,    44,
       2,     3,     4,     5,   343,   188,    40,    41,    42,    43,
      44,   177,     8,     9,   213,    10,     2,     3,     4,     5,
     175,   328,   211,     8,     9,    12,    10,    45,   181,   179,
      14,  -118,  -118,  -118,  -118,  -118,    12,   283,   136,     8,
       9,    14,    10,     2,     3,     4,     5,  -122,  -122,  -122,
    -122,  -122,    12,   137,   216,   120,   285,    14,  -126,  -126,
    -126,  -126,  -126,   271,   321,   296,     8,     9,   182,    10,
     272,   273,   274,   275,   225,     0,     0,     0,     0,    66
};

static const yytype_int16 yycheck[] =
{
       0,    31,    89,     0,    13,     0,     0,     7,   146,     9,
      10,   291,    12,    13,     5,     0,    11,    12,    18,    53,
      54,    18,     7,    18,    18,   317,    11,    12,    13,    14,
     115,    54,    54,    18,     3,    58,     5,    54,    29,    61,
      19,    64,    64,   267,   268,    45,    18,    64,    45,    17,
     342,    45,    18,   333,     3,   140,     5,     6,    27,    22,
      45,    40,   147,    16,   149,     8,    66,    20,    40,     5,
       3,   158,     5,     6,    40,    44,    39,    22,    23,    24,
       3,     4,     5,     6,    93,     3,    18,     5,     6,    84,
      18,    40,     0,    93,    39,   319,   119,   119,    95,    84,
      32,    96,   119,    26,    32,   243,    39,    52,    93,    32,
      95,    96,   250,    46,    45,   145,    39,    18,     3,     4,
       5,     6,   122,    28,   124,   110,    22,    23,    24,   114,
     130,    32,    28,   133,   134,    40,    31,   122,    23,   322,
     323,   141,    27,    39,   231,     3,   146,     5,     6,   144,
      21,   238,     1,   138,     3,     4,     5,     6,     7,   144,
     245,   146,     3,     4,     5,     6,    22,    23,    24,     4,
     308,    20,    57,    58,    59,    60,    28,    26,    27,   174,
      29,    39,    31,    39,    28,    26,    44,    16,    40,   174,
      39,    23,    24,    25,    43,    44,    40,    46,    39,     3,
       4,     5,     6,   288,     3,     4,     5,     6,    30,    47,
     240,    31,    61,    28,    28,   215,    40,    55,    40,   216,
      42,   221,   216,    27,    40,    40,    40,    26,    27,   314,
      29,   216,    35,    28,    37,   320,    28,   324,   325,    28,
      39,    27,   327,   243,     3,     4,     5,     6,    40,    45,
     250,    40,    30,    19,   254,    55,   343,    57,   243,   244,
      60,   246,    36,    27,    38,   250,    16,    16,    27,    18,
      27,    20,    21,   303,    27,    22,    23,    24,    27,   264,
       3,    28,     5,    32,     9,    10,    11,    12,    13,   289,
      29,    40,    39,     3,     5,     5,    45,    16,    57,    58,
      59,    60,    47,     3,     4,     5,     6,     7,   308,     3,
       4,     5,     6,   313,     3,   315,     5,   317,    16,    48,
      20,    27,    40,   308,    16,    27,    26,    27,    27,    29,
      27,    31,    26,    27,     4,    29,    30,    27,    52,    39,
      49,    28,   342,    43,    44,    39,    46,     3,     4,     5,
       6,    56,    40,     9,    10,    11,    12,    13,    40,    50,
      16,    33,    34,    35,    36,    37,    38,    40,    30,    33,
      26,    27,     6,    29,    33,    34,    35,    36,    37,    38,
      27,    18,    28,    39,    27,    41,    51,    40,    44,     3,
       4,     5,     6,    53,   240,     9,    10,    11,    12,    13,
       3,     4,     5,     6,    40,   122,     9,    10,    11,    12,
      13,    96,    26,    27,   144,    29,     3,     4,     5,     6,
      95,   313,   142,    26,    27,    39,    29,    41,   114,   110,
      44,     9,    10,    11,    12,    13,    39,   244,    64,    26,
      27,    44,    29,     3,     4,     5,     6,     9,    10,    11,
      12,    13,    39,    64,    41,    45,   246,    44,     9,    10,
      11,    12,    13,   231,   298,   263,    26,    27,    28,    29,
     232,   233,   234,   235,   152,    -1,    -1,    -1,    -1,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    20,    26,    27,
      29,    31,    39,    43,    44,    46,    61,    64,    65,    66,
      67,    68,    72,    74,    75,    84,    85,    86,    87,    98,
      99,   100,   101,   102,   107,   112,   113,   114,    17,     8,
       9,    10,    11,    12,    13,    41,    69,    73,    82,    83,
      85,    86,    87,    93,   100,   115,   117,   118,   119,   120,
     121,   122,   125,   126,   129,   132,    39,   100,   112,    30,
      91,   100,   108,   109,    77,    78,    79,    86,   112,    86,
     100,    69,   112,     5,   111,    47,    55,   133,   134,   137,
       0,    66,    16,    20,    45,    63,    31,    21,    33,    34,
      35,    36,    37,    38,    94,    95,   127,   128,    22,    39,
      96,    23,    24,    25,    97,    27,     4,    85,    87,   100,
     126,    16,    40,    94,    95,   127,   128,   129,   129,   123,
     127,   129,   124,   128,    94,   116,   123,   124,    31,    28,
      27,    40,    45,    30,    18,    32,    19,    27,    16,    27,
      40,    86,    27,     3,     4,     5,     6,    23,    27,    57,
      58,    59,    60,   138,   139,   140,   141,   155,   156,   157,
     158,   159,    16,    69,    39,    84,    85,    77,     4,   102,
     112,    99,    28,    91,   100,    94,   127,   128,    83,   100,
     100,   100,   100,     3,     4,     5,     6,    26,    32,    39,
      92,   103,   104,   105,   106,   112,   130,   131,    91,   100,
       5,   107,   110,    78,    76,    95,    41,    80,    82,   100,
      91,    29,    70,    91,     5,   134,    16,    16,   155,    48,
     142,    40,    22,    23,    24,    39,    52,   147,    27,    16,
      32,    28,     4,    19,    40,    27,    18,    32,    28,   100,
      40,    28,   100,    18,    88,    28,     3,     5,     3,     5,
      28,     3,     5,    27,    44,   143,   144,   145,   146,    49,
     149,   141,   156,   156,   156,   156,     3,     5,   148,   155,
      76,    81,    82,   103,    91,   131,    82,    30,    40,    42,
      71,    89,    90,   100,    56,   135,   133,   112,    40,   147,
     147,   150,   151,   152,   155,    50,   153,    28,    40,    28,
      91,   100,    30,    40,    88,    33,     6,    27,   136,    28,
      27,   144,    53,    54,    95,    51,    82,    40,    90,    91,
     100,    89,   147,    91,   150,   150,   155,   154,   155,    91,
      28,    28,    88,    40,    89,   155,    28
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
#line 357 "src/parser/aspcore2.y"
    {
	     	director.getBuilder()->onSemicolon();
        }
    break;

  case 64:
#line 368 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onNamedParameter();
    }
    break;

  case 65:
#line 374 "src/parser/aspcore2.y"
    { (yyval.integer) = 1; }
    break;

  case 66:
#line 375 "src/parser/aspcore2.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) + 1; }
    break;

  case 69:
#line 385 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBuiltinAtom();
        }
    break;

  case 70:
#line 392 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onEqualOperator();
        }
    break;

  case 71:
#line 396 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnequalOperator();
        }
    break;

  case 75:
#line 409 "src/parser/aspcore2.y"
    { (yyval.single_char) = '+'; }
    break;

  case 76:
#line 410 "src/parser/aspcore2.y"
    { (yyval.single_char) = '-'; }
    break;

  case 77:
#line 414 "src/parser/aspcore2.y"
    { (yyval.single_char) = '*'; }
    break;

  case 78:
#line 415 "src/parser/aspcore2.y"
    { (yyval.single_char) = '/'; }
    break;

  case 79:
#line 416 "src/parser/aspcore2.y"
    { (yyval.single_char) = '\\'; }
    break;

  case 80:
#line 421 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnknownVariable();
        }
    break;

  case 81:
#line 425 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onFunction((yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
        }
    break;

  case 82:
#line 430 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermRange((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 83:
#line 436 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermDash();
        }
    break;

  case 85:
#line 445 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 86:
#line 450 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 87:
#line 455 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermParams();
        }
    break;

  case 88:
#line 461 "src/parser/aspcore2.y"
    {}
    break;

  case 91:
#line 470 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 92:
#line 477 "src/parser/aspcore2.y"
    {}
    break;

  case 93:
#line 479 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 97:
#line 491 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 98:
#line 496 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 99:
#line 501 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 100:
#line 506 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(2) - (2)].string),true);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 101:
#line 514 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateVariableTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 102:
#line 519 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    break;

  case 103:
#line 526 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunctionalTerm((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
       }
    break;

  case 104:
#line 534 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm(0);
       }
    break;

  case 105:
#line 538 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm((yyvsp[(2) - (3)].integer));
       }
    break;

  case 106:
#line 545 "src/parser/aspcore2.y"
    {
                (yyval.integer) = (yyvsp[(1) - (1)].integer);
        }
    break;

  case 107:
#line 549 "src/parser/aspcore2.y"
    {
                (yyval.integer) = 2;
        }
    break;

  case 108:
#line 556 "src/parser/aspcore2.y"
    {

                director.getBuilder()->onHeadTailList();

        }
    break;

  case 109:
#line 565 "src/parser/aspcore2.y"
    {
                director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
                delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 111:
#line 574 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 112:
#line 579 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 113:
#line 586 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 114:
#line 587 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 115:
#line 588 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 116:
#line 593 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onDirective((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string));
            delete[] (yyvsp[(1) - (2)].string);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 117:
#line 602 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAtom();
        }
    break;

  case 118:
#line 609 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 119:
#line 616 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 122:
#line 628 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 126:
#line 644 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 128:
#line 655 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 129:
#line 662 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 137:
#line 682 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOperator();
        }
    break;

  case 138:
#line 686 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOrEqualOperator();
        }
    break;

  case 139:
#line 693 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOperator();
        }
    break;

  case 140:
#line 697 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    break;

  case 145:
#line 715 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 146:
#line 722 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 147:
#line 727 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 148:
#line 732 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 149:
#line 737 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 150:
#line 742 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 152:
#line 759 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 154:
#line 770 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopyTo((yyvsp[(2) - (2)].string));
         delete [] (yyvsp[(2) - (2)].string);
    }
    break;

  case 156:
#line 782 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLStart();
    }
    break;

  case 157:
#line 789 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelect();
    }
    break;

  case 158:
#line 798 "src/parser/aspcore2.y"
    {
        char * c = new char[2];
        c[0] = '*';
        c[1] = '\0';
        director.getBuilder()->onSQLQualifiedName(c);
        delete[] c;
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 162:
#line 816 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 163:
#line 825 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFrom();
    }
    break;

  case 166:
#line 839 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTable();
    }
    break;

  case 167:
#line 843 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFromItem();
    }
    break;

  case 168:
#line 847 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSubQuery();
    }
    break;

  case 169:
#line 854 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableName((yyvsp[(2) - (7)].string));
        delete[] (yyvsp[(2) - (7)].string);
    }
    break;

  case 170:
#line 861 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 171:
#line 866 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 174:
#line 880 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 175:
#line 885 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
         delete[] (yyvsp[(1) - (1)].string);
     }
    break;

  case 177:
#line 895 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhere();
    }
    break;

  case 178:
#line 902 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
    }
    break;

  case 179:
#line 906 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLOperatorCondition("AND");
    }
    break;

  case 180:
#line 910 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLOperatorCondition("OR");
    }
    break;

  case 181:
#line 918 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExprOp();
    }
    break;

  case 182:
#line 925 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExpr();
    }
    break;

  case 185:
#line 938 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 186:
#line 942 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 188:
#line 951 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('+');
    }
    break;

  case 189:
#line 955 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('-');
    }
    break;

  case 190:
#line 959 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('*');
    }
    break;

  case 191:
#line 963 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('/');
    }
    break;

  case 193:
#line 975 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 194:
#line 980 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 198:
#line 991 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 199:
#line 996 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 200:
#line 1001 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 201:
#line 1007 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 202:
#line 1013 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 203:
#line 1019 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 204:
#line 1030 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 205:
#line 1035 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 206:
#line 1040 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 207:
#line 1045 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;


/* Line 1267 of yacc.c.  */
#line 2896 "src/parser/aspcore2_parser.hpp"
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



