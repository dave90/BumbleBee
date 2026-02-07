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
     SQL_SUM = 318,
     SQL_MIN = 319,
     SQL_MAX = 320,
     SQL_AVG = 321,
     SQL_COUNT = 322,
     SQL_DIALECT = 323
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
#define SQL_SUM 318
#define SQL_MIN 319
#define SQL_MAX 320
#define SQL_AVG 321
#define SQL_COUNT 322
#define SQL_DIALECT 323




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
#line 268 "src/parser/aspcore2_parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 281 "src/parser/aspcore2_parser.c"

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
#define YYFINAL  94
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   519

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  113
/* YYNRULES -- Number of rules.  */
#define YYNRULES  245
/* YYNRULES -- Number of states.  */
#define YYNSTATES  398

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   323

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
      65,    66,    67,    68
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
     468,   470,   472,   474,   476,   478,   485,   493,   496,   500,
     502,   504,   506,   508,   510,   514,   517,   520,   522,   526,
     529,   532,   537,   545,   547,   549,   551,   552,   555,   557,
     559,   561,   563,   565,   567,   569,   570,   573,   575,   579,
     583,   587,   589,   590,   594,   596,   600,   601,   605,   607,
     611,   614,   616,   618,   620,   622,   624,   626,   627,   629,
     631,   632,   635,   637,   641,   645,   649,   653,   655,   657,
     659,   661,   665,   670,   672,   674,   676,   678,   682,   686,
     690,   694,   696,   698,   700,   702
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      71,     0,    -1,    49,    -1,    68,   148,    -1,    72,    -1,
       1,    -1,    73,    -1,    72,    73,    -1,    74,    -1,   121,
      -1,    75,    18,    -1,    75,    22,    18,    -1,    75,    22,
      76,    18,    -1,    22,    76,    18,    -1,    47,    76,    18,
      77,    -1,   122,    -1,    79,    -1,    81,    -1,    80,    -1,
      31,   108,    32,    -1,    31,   108,    78,    32,    -1,    46,
     108,    -1,    46,   108,    44,    98,    -1,    44,    98,    -1,
      92,    -1,    94,    -1,    79,    70,    92,    -1,    91,    -1,
      79,    70,    91,    -1,    90,    -1,    80,    44,    90,    -1,
      82,    33,    84,    34,    83,    -1,    82,    33,    84,    34,
      -1,    33,    84,    34,    -1,    33,    84,    34,    83,    -1,
     108,   103,    -1,   103,   108,    -1,    84,    20,    85,    -1,
      85,    -1,    86,    -1,    86,    21,    87,    -1,    93,    -1,
      89,    -1,    87,    44,    89,    -1,    89,    -1,    88,    44,
      89,    -1,    92,    -1,    45,    92,    -1,   100,    -1,    94,
      -1,    45,    94,    -1,    89,    -1,   135,    -1,   134,    -1,
      50,   119,    93,    -1,    93,    -1,    43,    93,    -1,   120,
      -1,   120,    29,    98,    30,    -1,   120,    29,    30,    -1,
      48,   120,    29,    98,    95,    96,    95,    98,    30,    -1,
      48,   120,    29,    95,    96,    95,    98,    30,    -1,    20,
      -1,    -1,    97,    -1,    96,    44,    97,    -1,   108,    35,
     108,    -1,   108,    -1,    98,    44,   108,    -1,   111,    -1,
      99,    44,   111,    -1,   101,    -1,   100,    41,   101,    -1,
     108,   103,   108,    -1,    35,    -1,    36,    -1,   102,    -1,
     138,    -1,   139,    -1,    24,    -1,    43,    -1,    25,    -1,
      26,    -1,    27,    -1,    28,    -1,     4,    19,     4,    -1,
      43,   108,    -1,   115,    -1,   120,    -1,     4,    -1,    29,
     108,    30,    -1,   106,    -1,   109,    -1,   110,    -1,   109,
     104,   110,    -1,   107,    -1,   110,   105,   107,    -1,   112,
      -1,   113,    -1,   114,    -1,    15,    -1,     3,    -1,     6,
      -1,     4,    -1,    43,     4,    -1,     5,    -1,    28,    -1,
     120,    29,    98,    30,    -1,    31,    32,    -1,    31,   116,
      32,    -1,    98,    -1,   117,    -1,   108,    49,   118,    -1,
       5,    -1,   115,    -1,     5,    -1,   119,    44,     5,    -1,
       3,    -1,     6,    -1,     5,    -1,     7,     8,    -1,    93,
      23,    -1,   108,   102,    -1,   102,   108,    -1,   123,   140,
      -1,   140,   124,    -1,   108,   138,    -1,   126,   140,    -1,
     127,    -1,   130,    -1,   108,   139,    -1,   129,   140,    -1,
     138,   108,    -1,   139,   108,    -1,   140,   131,    -1,   140,
     132,    -1,   128,    -1,   133,    -1,   125,    -1,   127,   131,
      -1,   130,   132,    -1,   136,   141,    33,   144,    34,    -1,
      29,   137,    30,    35,    -1,     5,    44,     5,    -1,   137,
      44,     5,    -1,    37,    -1,    39,    -1,    38,    -1,    40,
      -1,   147,    33,   144,    34,    -1,    14,   142,    32,    -1,
     143,    -1,   142,    44,   143,    -1,   120,    -1,   144,    20,
     145,    -1,   145,    -1,    99,    21,    88,    -1,    99,   146,
      99,    21,    88,    -1,    20,    -1,     9,    -1,    10,    -1,
      11,    -1,    12,    -1,    13,    -1,   149,    -1,    57,    29,
     149,    30,   150,   151,    -1,   152,   153,   157,   164,   168,
     170,   175,    -1,    58,     6,    -1,    29,    96,    30,    -1,
      51,    -1,   154,    -1,    25,    -1,   155,    -1,   156,    -1,
     155,    44,   156,    -1,   176,   162,    -1,    52,   158,    -1,
     159,    -1,   158,    44,   159,    -1,   160,   162,    -1,   161,
     162,    -1,    29,   148,    30,   162,    -1,    48,   120,    29,
      98,    95,    96,    30,    -1,     6,    -1,     3,    -1,     5,
      -1,    -1,    56,   163,    -1,     3,    -1,     5,    -1,    67,
      -1,    66,    -1,    65,    -1,    64,    -1,    63,    -1,    -1,
      53,   165,    -1,   166,    -1,   165,    42,   166,    -1,   165,
      41,   166,    -1,   167,   103,   176,    -1,   176,    -1,    -1,
      54,    55,   169,    -1,   176,    -1,   169,    44,   176,    -1,
      -1,    59,    55,   171,    -1,   172,    -1,   171,    44,   172,
      -1,   173,   174,    -1,   180,    -1,    63,    -1,    64,    -1,
      65,    -1,    66,    -1,    67,    -1,    -1,    60,    -1,    61,
      -1,    -1,    62,     4,    -1,   177,    -1,   176,    24,   177,
      -1,   176,    43,   177,    -1,   176,    25,   177,    -1,   176,
      26,   177,    -1,   178,    -1,     4,    -1,     6,    -1,   180,
      -1,    29,   176,    30,    -1,   181,    29,   179,    30,    -1,
     176,    -1,    25,    -1,     3,    -1,     5,    -1,     3,    18,
       3,    -1,     5,    18,     3,    -1,     5,    18,     5,    -1,
       3,    18,     5,    -1,    63,    -1,    64,    -1,    65,    -1,
      66,    -1,    67,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    82,    82,    86,    87,    88,    93,    94,    98,    99,
     103,   107,   111,   115,   119,   123,   131,   135,   143,   150,
     155,   163,   168,   172,   182,   186,   191,   195,   199,   206,
     210,   217,   218,   219,   220,   224,   231,   238,   239,   243,
     247,   254,   261,   265,   272,   276,   283,   287,   291,   296,
     300,   307,   308,   309,   316,   322,   323,   327,   332,   337,
     346,   351,   359,   364,   366,   367,   371,   378,   379,   383,
     384,   388,   389,   393,   400,   404,   411,   412,   413,   418,
     419,   423,   424,   425,   429,   433,   439,   443,   448,   453,
     458,   465,   466,   472,   473,   481,   482,   488,   489,   490,
     491,   499,   504,   509,   514,   522,   527,   534,   542,   546,
     553,   557,   564,   573,   578,   582,   587,   595,   596,   597,
     601,   610,   617,   624,   631,   632,   636,   643,   647,   648,
     652,   659,   663,   670,   677,   678,   682,   683,   684,   685,
     686,   690,   697,   701,   708,   716,   720,   727,   731,   738,
     742,   746,   747,   751,   759,   760,   765,   769,   776,   783,
     788,   793,   798,   803,   822,   823,   830,   834,   842,   846,
     853,   862,   871,   875,   876,   880,   889,   898,   899,   903,
     907,   911,   918,   923,   930,   935,   943,   945,   949,   954,
     959,   964,   969,   974,   979,   987,   989,   996,  1000,  1005,
    1014,  1021,  1028,  1030,  1034,  1038,  1045,  1047,  1051,  1052,
    1056,  1063,  1064,  1069,  1074,  1079,  1084,  1092,  1095,  1100,
    1107,  1109,  1117,  1118,  1122,  1126,  1130,  1137,  1142,  1147,
    1152,  1153,  1154,  1158,  1159,  1167,  1172,  1177,  1183,  1189,
    1195,  1206,  1211,  1216,  1221,  1226
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
  "SQL_ASC", "SQL_DESC", "SQL_LIMIT", "SQL_SUM", "SQL_MIN", "SQL_MAX",
  "SQL_AVG", "SQL_COUNT", "SQL_DIALECT", "$accept", "HEAD_SEPARATOR",
  "program", "rules", "rule", "simple_rule", "head", "body",
  "weight_at_levels", "levels_and_terms", "disjunction", "conjunction",
  "choice_atom", "lower_guard", "upper_guard", "choice_elements",
  "choice_element", "choice_element_atom", "choice_elements_literals",
  "naf_literals", "naf_literal", "naf_literal_aggregate",
  "existential_atom", "classic_literal", "atom", "ext_atom", "ext_semicol",
  "named_parameters", "namedParameter", "terms", "basic_terms",
  "builtin_or_list", "builtin_atom", "compareop", "binop", "arithop1",
  "arithop2", "term_", "term__", "term", "expr", "factor", "basic_term",
  "ground_term", "variable_term", "functional_term", "list_term",
  "list_terms_term", "list_head_tail_term", "list_tail_term", "vars",
  "identifier", "directive", "query", "lower_guard_compare_aggregate",
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
     315,   316,   317,   318,   319,   320,   321,   322,   323
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    69,    70,    71,    71,    71,    72,    72,    73,    73,
      74,    74,    74,    74,    74,    74,    75,    75,    76,    77,
      77,    78,    78,    78,    79,    79,    79,    79,    79,    80,
      80,    81,    81,    81,    81,    82,    83,    84,    84,    85,
      85,    86,    87,    87,    88,    88,    89,    89,    89,    89,
      89,    90,    90,    90,    91,    92,    92,    93,    93,    93,
      94,    94,    95,    96,    96,    96,    97,    98,    98,    99,
      99,   100,   100,   101,   102,   102,   103,   103,   103,   104,
     104,   105,   105,   105,   106,   106,   106,   106,   107,   107,
     107,   108,   108,   109,   109,   110,   110,   111,   111,   111,
     111,   112,   112,   112,   112,   113,   113,   114,   115,   115,
     116,   116,   117,   118,   118,   119,   119,   120,   120,   120,
     121,   122,   123,   124,   125,   125,   126,   127,   128,   128,
     129,   130,   131,   132,   133,   133,   134,   134,   134,   134,
     134,   135,   136,   137,   137,   138,   138,   139,   139,   140,
     141,   142,   142,   143,   144,   144,   145,   145,   146,   147,
     147,   147,   147,   147,   148,   148,   149,   150,   151,   152,
     153,   154,   154,   155,   155,   156,   157,   158,   158,   159,
     159,   159,   160,   160,   161,   161,   162,   162,   163,   163,
     163,   163,   163,   163,   163,   164,   164,   165,   165,   165,
     166,   167,   168,   168,   169,   169,   170,   170,   171,   171,
     172,   173,   173,   173,   173,   173,   173,   174,   174,   174,
     175,   175,   176,   176,   176,   176,   176,   177,   178,   178,
     178,   178,   178,   179,   179,   180,   180,   180,   180,   180,
     180,   181,   181,   181,   181,   181
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
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
       1,     1,     1,     1,     1,     6,     7,     2,     3,     1,
       1,     1,     1,     1,     3,     2,     2,     1,     3,     2,
       2,     4,     7,     1,     1,     1,     0,     2,     1,     1,
       1,     1,     1,     1,     1,     0,     2,     1,     3,     3,
       3,     1,     0,     3,     1,     3,     0,     3,     1,     3,
       2,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       0,     2,     1,     3,     3,     3,     3,     1,     1,     1,
       1,     3,     4,     1,     1,     1,     1,     3,     3,     3,
       3,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,   117,    89,   119,   118,     0,     0,    84,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     6,
       8,     0,    16,    17,     0,    27,    24,    55,    25,    91,
      95,     0,    92,    93,    87,    88,     9,    15,     0,   120,
     159,   160,   161,   162,   163,     0,     0,     0,    18,    51,
      29,    46,    55,    49,    48,    71,     0,     0,   138,     0,
     128,   136,     0,   129,   137,    53,    52,     0,     0,     0,
       0,     0,    88,   108,   110,    67,     0,   111,     0,    38,
      39,    41,    57,    56,    86,     0,     0,   115,     0,   169,
       0,     3,   164,     0,     1,     7,    10,     0,     2,     0,
       0,   121,    74,    75,   145,   147,   146,   148,    76,    35,
      77,    78,    79,    80,     0,    81,    82,    83,     0,     0,
      85,   119,     0,     0,    47,    50,    13,     0,     0,    76,
       0,    77,    78,   124,   127,   139,     0,   131,   140,     0,
       0,     0,     0,   125,   134,   135,     0,    90,     0,     0,
     109,     0,    33,     0,     0,     0,     0,    54,     0,   235,
     228,   236,   229,   171,     0,   241,   242,   243,   244,   245,
       0,   170,   172,   173,   186,   222,   227,   230,     0,    11,
       0,    28,    26,     0,    89,    94,    96,    59,     0,    67,
       0,     0,     0,    30,    72,     0,    73,   132,   133,   153,
       0,   151,     0,   123,   101,   103,   105,   102,   100,   106,
       0,     0,    69,    97,    98,    99,     0,     0,   155,    68,
     113,   114,   112,    37,    34,     0,    40,    42,     0,    14,
      62,    63,     0,   116,     0,     0,     0,     0,     0,   195,
       0,     0,     0,     0,     0,     0,   175,     0,    12,    32,
      58,   143,   142,   144,   150,     0,     0,   104,   158,     0,
       0,     0,     0,     0,   149,    36,     0,     0,     0,    64,
       0,    63,     0,   237,   240,   238,   239,   231,   184,   185,
     183,     0,     0,   176,   177,   186,   186,     0,   202,   174,
     223,   225,   226,   224,   188,   189,   194,   193,   192,   191,
     190,   187,   234,   233,     0,    31,   152,   141,   156,    44,
      70,     0,     0,   154,    43,    19,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   179,   180,
     196,   197,     0,   201,     0,   206,   232,     0,     0,   107,
      23,    21,    20,    65,     0,    66,     0,   167,    63,   165,
     186,     0,   178,     0,     0,     0,     0,     0,   220,    45,
     157,     0,    61,     0,     0,   181,     0,   199,   198,   200,
     203,   204,     0,     0,   166,    22,    60,   168,    63,     0,
     212,   213,   214,   215,   216,   207,   208,   217,   211,   221,
       0,   205,     0,   218,   219,   210,   182,   209
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    99,    17,    18,    19,    20,    21,    47,   229,   318,
      22,    48,    23,    24,   224,    78,    79,    80,   226,   308,
      49,    50,    25,    51,    52,    53,   231,   268,   269,    74,
     211,    54,    55,   108,   130,   114,   118,    29,    30,   189,
      32,    33,   212,   213,   214,   215,    34,    76,    77,   222,
      88,    72,    36,    37,    57,   143,    58,    59,    60,    61,
      62,    63,   135,   138,    64,    65,    66,    67,   122,   110,
     111,    68,   141,   200,   201,   217,   218,   261,    69,    91,
      92,   324,   349,    93,   170,   171,   172,   173,   239,   283,
     284,   285,   286,   246,   301,   288,   330,   331,   332,   335,
     370,   358,   385,   386,   387,   395,   374,   333,   175,   176,
     304,   177,   178
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -309
static const yytype_int16 yypact[] =
{
     165,  -309,  -309,    -2,  -309,  -309,    95,   387,  -309,   468,
     423,   252,   468,   387,   252,    17,    21,   116,   325,  -309,
    -309,    49,   -11,  -309,   119,  -309,  -309,   112,  -309,  -309,
    -309,   254,    53,   289,  -309,   466,  -309,  -309,   207,  -309,
    -309,  -309,  -309,  -309,  -309,   475,   158,   137,   182,  -309,
    -309,  -309,  -309,  -309,   208,  -309,   254,   294,  -309,   294,
     205,  -309,   294,   189,  -309,  -309,  -309,   227,   254,   213,
     468,   231,  -309,  -309,   223,   229,   230,  -309,    41,  -309,
     255,  -309,   250,  -309,  -309,   268,   258,  -309,    43,  -309,
     266,  -309,  -309,   115,  -309,  -309,  -309,   376,  -309,    38,
     252,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,  -309,  -309,   102,  -309,  -309,  -309,   102,   434,
    -309,   264,   -10,   252,  -309,  -309,  -309,   387,   468,   314,
     468,   330,   350,  -309,  -309,  -309,   468,  -309,  -309,   468,
     252,   276,   468,  -309,  -309,  -309,   307,  -309,   468,    35,
    -309,   252,   254,   405,   287,   439,   315,  -309,   282,   316,
    -309,   318,  -309,  -309,   234,  -309,  -309,  -309,  -309,  -309,
     292,  -309,   301,  -309,    33,  -309,  -309,  -309,   317,  -309,
     331,  -309,  -309,    48,  -309,   289,  -309,  -309,    39,  -309,
     352,   320,   364,  -309,  -309,   254,  -309,  -309,  -309,  -309,
     118,  -309,   307,  -309,   341,  -309,   342,   345,  -309,  -309,
     372,    42,  -309,  -309,  -309,  -309,   348,    89,  -309,  -309,
    -309,  -309,  -309,  -309,  -309,   468,   339,  -309,   468,  -309,
    -309,   468,    10,  -309,   354,   181,   267,   108,    50,   349,
     234,   234,   234,   234,   234,    28,  -309,   218,  -309,   254,
    -309,  -309,  -309,  -309,  -309,   252,   109,  -309,  -309,   405,
     307,   307,   468,   307,  -309,  -309,   405,   151,    30,  -309,
     360,   468,   343,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,    21,   252,   359,  -309,   356,   356,   234,   363,  -309,
    -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,  -309,   133,   383,  -309,  -309,  -309,   362,  -309,
    -309,     8,   147,  -309,  -309,  -309,   468,   468,   382,   468,
     468,   468,    30,   414,   393,   395,   394,    50,  -309,  -309,
      58,  -309,   254,   133,   386,   388,  -309,   405,   405,  -309,
     223,   402,  -309,  -309,   155,  -309,   468,  -309,   468,  -309,
     356,   468,  -309,   234,   234,   234,   234,   401,   369,  -309,
     362,   468,  -309,   173,   188,  -309,    10,  -309,  -309,   133,
     413,   133,   187,   445,  -309,   223,  -309,  -309,   468,   234,
    -309,  -309,  -309,  -309,  -309,   416,  -309,    65,  -309,  -309,
     190,   133,   187,  -309,  -309,  -309,  -309,  -309
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -309,  -309,  -309,  -309,   440,  -309,  -309,    -7,  -309,  -309,
    -309,  -309,  -309,  -309,   212,   375,   332,  -309,  -309,   131,
    -149,   358,   377,     5,    24,    19,  -221,  -263,   168,  -116,
     232,  -309,   366,   180,   -30,  -309,  -309,  -309,   373,     0,
    -309,   378,   238,  -309,  -309,  -309,   353,  -309,  -309,  -309,
    -309,    14,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,   422,   433,  -309,  -309,  -309,  -309,  -309,   200,
     146,   148,  -309,  -309,   253,   303,   244,  -309,  -309,   228,
     355,  -309,  -309,  -309,  -309,  -309,  -309,   272,  -309,  -309,
     192,  -309,  -309,  -270,  -309,  -309,  -309,  -180,  -309,  -309,
    -309,  -309,  -309,   122,  -309,  -309,  -309,   -91,   123,  -309,
    -309,  -308,  -309
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -131
static const yytype_int16 yytable[] =
{
      31,   109,   174,   188,   227,    26,    85,    56,   322,    71,
      75,   271,    84,    56,    35,   328,   329,    38,    31,    28,
     191,    35,    87,    26,    27,    82,    35,    35,    86,   338,
     230,   294,    35,   295,   192,    81,    83,    28,    98,   232,
     220,     2,    27,     4,     5,    71,     2,   320,     4,     5,
     230,   124,   260,   278,   148,   279,   280,   241,   242,   243,
      82,   151,   258,   259,   388,   125,    10,    96,   151,   250,
      84,    97,    89,   237,   319,   152,   244,   112,    90,   281,
     365,   123,   249,   148,   388,   364,   260,   156,    15,   245,
     180,   296,   297,   298,   299,   300,   113,    56,   282,   353,
     354,   346,    82,    39,   182,     2,   184,     4,     5,   263,
     309,    35,   157,    82,    82,   390,    94,   314,   159,   160,
     161,   162,   225,   264,    81,   393,   394,    56,   195,   263,
     196,     9,   241,   242,   243,   101,   197,    82,   277,   198,
     163,    35,   203,   307,   164,   378,   312,    83,   219,   174,
     254,   244,   100,   195,   199,   126,   303,   241,   242,   243,
     216,     2,   255,     4,     5,    82,     1,    35,     2,     3,
       4,     5,     6,   367,   368,    81,   244,   339,   165,   166,
     167,   168,   169,   315,   273,   362,   274,     7,   359,   309,
     159,   148,   161,     8,     9,   316,    10,   317,    11,   148,
     340,   123,   132,   376,   344,   133,    14,   134,    12,   139,
     137,   120,    13,    14,   139,    15,   216,   148,   377,   225,
     396,   159,   160,   161,   162,   265,   127,   105,   267,   107,
     363,   270,   319,    16,   319,   366,   129,   159,   160,   161,
     162,   140,   104,   302,   106,   375,   146,   164,   142,   128,
     380,   381,   382,   383,   384,     2,   131,     4,     5,   195,
     136,   147,   150,   164,   369,   371,   195,   148,   136,   199,
     275,   270,   276,    35,   216,   216,   153,   216,   149,   119,
      35,   165,   166,   167,   168,   169,   154,   155,   391,   102,
     103,   104,   105,   106,   107,   158,   326,   165,   166,   167,
     168,   169,   355,    40,    41,    42,    43,    44,   190,   202,
     204,   205,   206,   207,   115,   116,   117,   341,   228,   270,
     233,   345,   208,  -122,  -122,  -122,  -122,  -122,     2,     3,
       4,     5,     6,    89,   235,   209,   236,   195,   195,  -126,
    -126,  -126,  -126,  -126,   238,   240,   247,     7,   270,   248,
     210,    35,    35,     8,     9,   252,    10,   251,    11,  -130,
    -130,  -130,  -130,  -130,   290,   291,   292,   293,    12,   253,
    -117,  -119,    13,    14,  -118,    15,   257,   262,   270,     2,
       3,     4,     5,   266,   272,    40,    41,    42,    43,    44,
       2,     3,     4,     5,   179,   321,    40,    41,    42,    43,
      44,   323,   287,   327,     8,    45,   337,    10,     2,     3,
       4,     5,   245,   336,   342,     8,    45,   334,    10,    12,
     347,    46,   348,   351,    14,   350,     2,     3,     4,     5,
      12,   373,    46,     8,     9,    14,    10,     2,     3,     4,
       5,   356,     2,     3,     4,     5,   361,   357,    12,   389,
      46,     8,     9,    14,    10,    73,   372,   379,    95,   230,
     392,   305,     8,     9,   187,    10,    70,     8,     9,   360,
      10,     2,     3,     4,     5,   183,   181,    70,     2,     3,
     121,     5,    70,   223,   -57,   193,   -57,   343,   -57,   -57,
     144,   186,   185,   311,   194,   119,     8,     9,   310,    10,
     -57,   145,   221,     8,     9,   256,    10,   313,   306,   325,
     -57,    70,   289,   234,   397,   -57,     0,     0,    70,   352
};

static const yytype_int16 yycheck[] =
{
       0,    31,    93,   119,   153,     0,    13,     7,   271,     9,
      10,   232,    12,    13,     0,   285,   286,    19,    18,     0,
      30,     7,     5,    18,     0,    11,    12,    13,    14,    21,
      20,     3,    18,     5,    44,    11,    12,    18,    49,   155,
       5,     3,    18,     5,     6,    45,     3,   268,     5,     6,
      20,    46,    44,     3,    44,     5,     6,    24,    25,    26,
      46,    20,    20,    21,   372,    46,    31,    18,    20,    30,
      70,    22,    51,   164,    44,    34,    43,    24,    57,    29,
     350,    43,    34,    44,   392,   348,    44,    44,    50,    56,
      97,    63,    64,    65,    66,    67,    43,    97,    48,    41,
      42,   322,    88,     8,    99,     3,     4,     5,     6,    20,
     259,    97,    88,    99,   100,   378,     0,   266,     3,     4,
       5,     6,   152,    34,   100,    60,    61,   127,   128,    20,
     130,    29,    24,    25,    26,    23,   136,   123,    30,   139,
      25,   127,   142,    34,    29,   366,   262,   123,   148,   240,
      32,    43,    33,   153,   140,    18,   247,    24,    25,    26,
     146,     3,    44,     5,     6,   151,     1,   153,     3,     4,
       5,     6,     7,   353,   354,   151,    43,    30,    63,    64,
      65,    66,    67,    32,     3,    30,     5,    22,   337,   338,
       3,    44,     5,    28,    29,    44,    31,    46,    33,    44,
     316,    43,    56,    30,   320,    57,    48,    59,    43,    63,
      62,     4,    47,    48,    68,    50,   202,    44,    30,   249,
      30,     3,     4,     5,     6,   225,    44,    38,   228,    40,
     346,   231,    44,    68,    44,   351,    56,     3,     4,     5,
       6,    14,    37,    25,    39,   361,    33,    29,    68,    41,
      63,    64,    65,    66,    67,     3,    56,     5,     6,   259,
      60,    30,    32,    29,   355,   356,   266,    44,    68,   255,
       3,   271,     5,   259,   260,   261,    21,   263,    49,    29,
     266,    63,    64,    65,    66,    67,    18,    29,   379,    35,
      36,    37,    38,    39,    40,    29,   282,    63,    64,    65,
      66,    67,   332,     9,    10,    11,    12,    13,    44,    33,
       3,     4,     5,     6,    25,    26,    27,   317,    31,   319,
       5,   321,    15,     9,    10,    11,    12,    13,     3,     4,
       5,     6,     7,    51,    18,    28,    18,   337,   338,     9,
      10,    11,    12,    13,    52,    44,    29,    22,   348,    18,
      43,   337,   338,    28,    29,    35,    31,     5,    33,     9,
      10,    11,    12,    13,   241,   242,   243,   244,    43,     5,
      29,    29,    47,    48,    29,    50,     4,    29,   378,     3,
       4,     5,     6,    44,    30,     9,    10,    11,    12,    13,
       3,     4,     5,     6,    18,    35,     9,    10,    11,    12,
      13,    58,    53,    44,    28,    29,    44,    31,     3,     4,
       5,     6,    56,    30,    32,    28,    29,    54,    31,    43,
       6,    45,    29,    29,    48,    30,     3,     4,     5,     6,
      43,    62,    45,    28,    29,    48,    31,     3,     4,     5,
       6,    55,     3,     4,     5,     6,    44,    59,    43,     4,
      45,    28,    29,    48,    31,    32,    55,    44,    18,    20,
      44,   249,    28,    29,    30,    31,    43,    28,    29,   338,
      31,     3,     4,     5,     6,   100,    99,    43,     3,     4,
       5,     6,    43,   151,    18,   127,    20,   319,    22,    23,
      68,   118,   114,   261,   128,    29,    28,    29,   260,    31,
      34,    68,   149,    28,    29,   202,    31,   263,   255,   281,
      44,    43,   240,   158,   392,    49,    -1,    -1,    43,   327
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    22,    28,    29,
      31,    33,    43,    47,    48,    50,    68,    71,    72,    73,
      74,    75,    79,    81,    82,    91,    92,    93,    94,   106,
     107,   108,   109,   110,   115,   120,   121,   122,    19,     8,
       9,    10,    11,    12,    13,    29,    45,    76,    80,    89,
      90,    92,    93,    94,   100,   101,   108,   123,   125,   126,
     127,   128,   129,   130,   133,   134,   135,   136,   140,   147,
      43,   108,   120,    32,    98,   108,   116,   117,    84,    85,
      86,    93,   120,    93,   108,    76,   120,     5,   119,    51,
      57,   148,   149,   152,     0,    73,    18,    22,    49,    70,
      33,    23,    35,    36,    37,    38,    39,    40,   102,   103,
     138,   139,    24,    43,   104,    25,    26,    27,   105,    29,
       4,     5,   137,    43,    92,    94,    18,    44,    41,   102,
     103,   138,   139,   140,   140,   131,   138,   140,   132,   139,
      14,   141,   102,   124,   131,   132,    33,    30,    44,    49,
      32,    20,    34,    21,    18,    29,    44,    93,    29,     3,
       4,     5,     6,    25,    29,    63,    64,    65,    66,    67,
     153,   154,   155,   156,   176,   177,   178,   180,   181,    18,
      76,    91,    92,    84,     4,   110,   107,    30,    98,   108,
      44,    30,    44,    90,   101,   108,   108,   108,   108,   120,
     142,   143,    33,   108,     3,     4,     5,     6,    15,    28,
      43,    99,   111,   112,   113,   114,   120,   144,   145,   108,
       5,   115,   118,    85,    83,   103,    87,    89,    31,    77,
      20,    95,    98,     5,   149,    18,    18,   176,    52,   157,
      44,    24,    25,    26,    43,    56,   162,    29,    18,    34,
      30,     5,    35,     5,    32,    44,   144,     4,    20,    21,
      44,   146,    29,    20,    34,   108,    44,   108,    96,    97,
     108,    95,    30,     3,     5,     3,     5,    30,     3,     5,
       6,    29,    48,   158,   159,   160,   161,    53,   164,   156,
     177,   177,   177,   177,     3,     5,    63,    64,    65,    66,
      67,   163,    25,   176,   179,    83,   143,    34,    88,    89,
     111,    99,    98,   145,    89,    32,    44,    46,    78,    44,
      95,    35,    96,    58,   150,   148,   120,    44,   162,   162,
     165,   166,   167,   176,    54,   168,    30,    44,    21,    30,
      98,   108,    32,    97,    98,   108,    95,     6,    29,   151,
      30,    29,   159,    41,    42,   103,    55,    59,   170,    89,
      88,    44,    30,    98,    96,   162,    98,   166,   166,   176,
     169,   176,    55,    62,   175,    98,    30,    30,    95,    44,
      63,    64,    65,    66,    67,   171,   172,   173,   180,     4,
      96,   176,    44,    60,    61,   174,    30,   172
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
            director.getBuilder()->onBuiltinOrList();
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 49:
#line 297 "src/parser/aspcore2.y"
    {
    		director.getBuilder()->onExtAtom();
    	}
    break;

  case 50:
#line 301 "src/parser/aspcore2.y"
    {
			director.getBuilder()->onExtAtom(true);
		}
    break;

  case 53:
#line 310 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregate();
        }
    break;

  case 54:
#line 317 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialAtom();
        }
    break;

  case 55:
#line 322 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(); }
    break;

  case 56:
#line 323 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(true); }
    break;

  case 57:
#line 328 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 58:
#line 333 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (4)].string));
            delete[] (yyvsp[(1) - (4)].string);
        }
    break;

  case 59:
#line 338 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
        }
    break;

  case 60:
#line 347 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExternalPredicateName((yyvsp[(2) - (9)].string));
            delete[] (yyvsp[(2) - (9)].string);
        }
    break;

  case 61:
#line 352 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onExternalPredicateName((yyvsp[(2) - (8)].string));
          delete[] (yyvsp[(2) - (8)].string);
      }
    break;

  case 62:
#line 360 "src/parser/aspcore2.y"
    {
	     	director.getBuilder()->onSemicolon();
        }
    break;

  case 66:
#line 372 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onNamedParameter();
    }
    break;

  case 67:
#line 378 "src/parser/aspcore2.y"
    { (yyval.integer) = 1; }
    break;

  case 68:
#line 379 "src/parser/aspcore2.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) + 1; }
    break;

  case 73:
#line 394 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onBuiltinAtom();
    }
    break;

  case 74:
#line 401 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onEqualOperator();
        }
    break;

  case 75:
#line 405 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnequalOperator();
        }
    break;

  case 79:
#line 418 "src/parser/aspcore2.y"
    { (yyval.single_char) = '+'; }
    break;

  case 80:
#line 419 "src/parser/aspcore2.y"
    { (yyval.single_char) = '-'; }
    break;

  case 81:
#line 423 "src/parser/aspcore2.y"
    { (yyval.single_char) = '*'; }
    break;

  case 82:
#line 424 "src/parser/aspcore2.y"
    { (yyval.single_char) = '/'; }
    break;

  case 83:
#line 425 "src/parser/aspcore2.y"
    { (yyval.single_char) = '\\'; }
    break;

  case 84:
#line 430 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnknownVariable();
        }
    break;

  case 85:
#line 434 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermRange((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 86:
#line 440 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermDash();
        }
    break;

  case 88:
#line 449 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 89:
#line 454 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 90:
#line 459 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermParams();
        }
    break;

  case 91:
#line 465 "src/parser/aspcore2.y"
    {}
    break;

  case 94:
#line 474 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 95:
#line 481 "src/parser/aspcore2.y"
    {}
    break;

  case 96:
#line 483 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 100:
#line 492 "src/parser/aspcore2.y"
    {
           director.getBuilder()->onAggregateVariableTerm((yyvsp[(1) - (1)].string));
           delete[] (yyvsp[(1) - (1)].string);
       }
    break;

  case 101:
#line 500 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 102:
#line 505 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 103:
#line 510 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 104:
#line 515 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(2) - (2)].string),true);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 105:
#line 523 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateVariableTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 106:
#line 528 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    break;

  case 107:
#line 535 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunctionalTerm((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
       }
    break;

  case 108:
#line 543 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm(0);
       }
    break;

  case 109:
#line 547 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm((yyvsp[(2) - (3)].integer));
       }
    break;

  case 110:
#line 554 "src/parser/aspcore2.y"
    {
                (yyval.integer) = (yyvsp[(1) - (1)].integer);
        }
    break;

  case 111:
#line 558 "src/parser/aspcore2.y"
    {
                (yyval.integer) = 2;
        }
    break;

  case 112:
#line 565 "src/parser/aspcore2.y"
    {

                director.getBuilder()->onHeadTailList();

        }
    break;

  case 113:
#line 574 "src/parser/aspcore2.y"
    {
                director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
                delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 115:
#line 583 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 116:
#line 588 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 117:
#line 595 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 118:
#line 596 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 119:
#line 597 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 120:
#line 602 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onDirective((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string));
            delete[] (yyvsp[(1) - (2)].string);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 121:
#line 611 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAtom();
        }
    break;

  case 122:
#line 618 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 123:
#line 625 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 126:
#line 637 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 130:
#line 653 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 132:
#line 664 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 133:
#line 671 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 141:
#line 691 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAggregateAssignment();
        }
    break;

  case 143:
#line 702 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAssignVariable((yyvsp[(1) - (3)].string));
            director.getBuilder()->onMultiAssignVariable((yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 144:
#line 709 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onMultiAssignVariable((yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 145:
#line 717 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOperator();
        }
    break;

  case 146:
#line 721 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOrEqualOperator();
        }
    break;

  case 147:
#line 728 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOperator();
        }
    break;

  case 148:
#line 732 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    break;

  case 153:
#line 752 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 156:
#line 766 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 157:
#line 770 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 158:
#line 777 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroupSemicolon();
        }
    break;

  case 159:
#line 784 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 160:
#line 789 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 161:
#line 794 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 162:
#line 799 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 163:
#line 804 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 165:
#line 824 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopy();
    }
    break;

  case 167:
#line 835 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLCopyTo((yyvsp[(2) - (2)].string));
         delete [] (yyvsp[(2) - (2)].string);
    }
    break;

  case 169:
#line 847 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLStart();
    }
    break;

  case 170:
#line 854 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelect();
    }
    break;

  case 171:
#line 863 "src/parser/aspcore2.y"
    {
        char * c = new char[2];
        c[0] = '*';
        c[1] = '\0';
        director.getBuilder()->onSQLQualifiedName(c);
        delete[] c;
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 175:
#line 881 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSelectItem();
    }
    break;

  case 176:
#line 890 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFrom();
    }
    break;

  case 179:
#line 904 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTable();
    }
    break;

  case 180:
#line 908 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFromItem();
    }
    break;

  case 181:
#line 912 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLSubQuery();
    }
    break;

  case 182:
#line 919 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableName((yyvsp[(2) - (7)].string));
        delete[] (yyvsp[(2) - (7)].string);
    }
    break;

  case 183:
#line 924 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLExtTableNameString((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 184:
#line 931 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 185:
#line 936 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLTableRef((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 188:
#line 950 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 189:
#line 955 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
         delete[] (yyvsp[(1) - (1)].string);
     }
    break;

  case 190:
#line 960 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 191:
#line 965 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 192:
#line 970 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 193:
#line 975 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 194:
#line 980 "src/parser/aspcore2.y"
    {
          director.getBuilder()->onSQLAlias((yyvsp[(1) - (1)].string));
          delete[] (yyvsp[(1) - (1)].string);
      }
    break;

  case 196:
#line 990 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLWhere();
    }
    break;

  case 197:
#line 997 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
    }
    break;

  case 198:
#line 1001 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
        director.getBuilder()->onSQLOperatorCondition("AND");
    }
    break;

  case 199:
#line 1006 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicate();
        director.getBuilder()->onSQLOperatorCondition("OR");
    }
    break;

  case 200:
#line 1015 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExprOp();
    }
    break;

  case 201:
#line 1022 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExpr();
    }
    break;

  case 204:
#line 1035 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 205:
#line 1039 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 210:
#line 1057 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderCol();
    }
    break;

  case 212:
#line 1065 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 213:
#line 1070 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 214:
#line 1075 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 215:
#line 1080 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 216:
#line 1085 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 217:
#line 1092 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderModifier(nullptr);
    }
    break;

  case 218:
#line 1096 "src/parser/aspcore2.y"
    {
        char c[] = "asc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 219:
#line 1101 "src/parser/aspcore2.y"
    {
        char c[] = "desc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 221:
#line 1110 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLimit((yyvsp[(2) - (2)].string));
        delete[] (yyvsp[(2) - (2)].string);
    }
    break;

  case 223:
#line 1119 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('+');
    }
    break;

  case 224:
#line 1123 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('-');
    }
    break;

  case 225:
#line 1127 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('*');
    }
    break;

  case 226:
#line 1131 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValueTerm('/');
    }
    break;

  case 228:
#line 1143 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 229:
#line 1148 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 234:
#line 1160 "src/parser/aspcore2.y"
    {
        char c[] = "*";
        director.getBuilder()->onSQLValue(c);
    }
    break;

  case 235:
#line 1168 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 236:
#line 1173 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 237:
#line 1178 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 238:
#line 1184 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 239:
#line 1190 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 240:
#line 1196 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 241:
#line 1207 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 242:
#line 1212 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 243:
#line 1217 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 244:
#line 1222 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 245:
#line 1227 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;


/* Line 1267 of yacc.c.  */
#line 3163 "src/parser/aspcore2_parser.c"
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



