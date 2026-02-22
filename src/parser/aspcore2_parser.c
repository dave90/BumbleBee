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
#line 270 "src/parser/aspcore2_parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 283 "src/parser/aspcore2_parser.c"

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
#define YYLAST   516

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  249
/* YYNRULES -- Number of states.  */
#define YYNSTATES  403

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   324

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
      65,    66,    67,    68,    69
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
     583,   587,   591,   593,   594,   598,   600,   604,   605,   609,
     611,   615,   618,   620,   622,   624,   626,   628,   630,   631,
     633,   635,   636,   639,   641,   643,   647,   651,   653,   655,
     659,   663,   665,   667,   669,   670,   675,   680,   682,   684,
     686,   688,   692,   696,   700,   704,   706,   708,   710,   712
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      72,     0,    -1,    49,    -1,    69,   149,    -1,    73,    -1,
       1,    -1,    74,    -1,    73,    74,    -1,    75,    -1,   122,
      -1,    76,    18,    -1,    76,    22,    18,    -1,    76,    22,
      77,    18,    -1,    22,    77,    18,    -1,    47,    77,    18,
      78,    -1,   123,    -1,    80,    -1,    82,    -1,    81,    -1,
      31,   109,    32,    -1,    31,   109,    79,    32,    -1,    46,
     109,    -1,    46,   109,    44,    99,    -1,    44,    99,    -1,
      93,    -1,    95,    -1,    80,    71,    93,    -1,    92,    -1,
      80,    71,    92,    -1,    91,    -1,    81,    44,    91,    -1,
      83,    33,    85,    34,    84,    -1,    83,    33,    85,    34,
      -1,    33,    85,    34,    -1,    33,    85,    34,    84,    -1,
     109,   104,    -1,   104,   109,    -1,    85,    20,    86,    -1,
      86,    -1,    87,    -1,    87,    21,    88,    -1,    94,    -1,
      90,    -1,    88,    44,    90,    -1,    90,    -1,    89,    44,
      90,    -1,    93,    -1,    45,    93,    -1,   101,    -1,    95,
      -1,    45,    95,    -1,    90,    -1,   136,    -1,   135,    -1,
      50,   120,    94,    -1,    94,    -1,    43,    94,    -1,   121,
      -1,   121,    29,    99,    30,    -1,   121,    29,    30,    -1,
      48,   121,    29,    99,    96,    97,    96,    99,    30,    -1,
      48,   121,    29,    96,    97,    96,    99,    30,    -1,    20,
      -1,    -1,    98,    -1,    97,    44,    98,    -1,   109,    35,
     109,    -1,   109,    -1,    99,    44,   109,    -1,   112,    -1,
     100,    44,   112,    -1,   102,    -1,   101,    41,   102,    -1,
     109,   104,   109,    -1,    35,    -1,    36,    -1,   103,    -1,
     139,    -1,   140,    -1,    24,    -1,    43,    -1,    25,    -1,
      26,    -1,    27,    -1,    28,    -1,     4,    19,     4,    -1,
      43,   109,    -1,   116,    -1,   121,    -1,     4,    -1,    29,
     109,    30,    -1,   107,    -1,   110,    -1,   111,    -1,   110,
     105,   111,    -1,   108,    -1,   111,   106,   108,    -1,   113,
      -1,   114,    -1,   115,    -1,    15,    -1,     3,    -1,     6,
      -1,     4,    -1,    43,     4,    -1,     5,    -1,    28,    -1,
     121,    29,    99,    30,    -1,    31,    32,    -1,    31,   117,
      32,    -1,    99,    -1,   118,    -1,   109,    49,   119,    -1,
       5,    -1,   116,    -1,     5,    -1,   120,    44,     5,    -1,
       3,    -1,     6,    -1,     5,    -1,     7,     8,    -1,    94,
      23,    -1,   109,   103,    -1,   103,   109,    -1,   124,   141,
      -1,   141,   125,    -1,   109,   139,    -1,   127,   141,    -1,
     128,    -1,   131,    -1,   109,   140,    -1,   130,   141,    -1,
     139,   109,    -1,   140,   109,    -1,   141,   132,    -1,   141,
     133,    -1,   129,    -1,   134,    -1,   126,    -1,   128,   132,
      -1,   131,   133,    -1,   137,   142,    33,   145,    34,    -1,
      29,   138,    30,    35,    -1,     5,    44,     5,    -1,   138,
      44,     5,    -1,    37,    -1,    39,    -1,    38,    -1,    40,
      -1,   148,    33,   145,    34,    -1,    14,   143,    32,    -1,
     144,    -1,   143,    44,   144,    -1,   121,    -1,   145,    20,
     146,    -1,   146,    -1,   100,    21,    89,    -1,   100,   147,
     100,    21,    89,    -1,    20,    -1,     9,    -1,    10,    -1,
      11,    -1,    12,    -1,    13,    -1,   150,    -1,    57,    29,
     150,    30,   151,   152,    -1,   153,   154,   158,   165,   169,
     171,   176,    -1,    58,     6,    -1,    29,    97,    30,    -1,
      51,    -1,   155,    -1,    25,    -1,   156,    -1,   157,    -1,
     156,    44,   157,    -1,   177,   163,    -1,    52,   159,    -1,
     160,    -1,   159,    44,   160,    -1,   161,   163,    -1,   162,
     163,    -1,    29,   149,    30,   163,    -1,    48,   121,    29,
      99,    96,    97,    30,    -1,     6,    -1,     3,    -1,     5,
      -1,    -1,    56,   164,    -1,     3,    -1,     5,    -1,    68,
      -1,    67,    -1,    66,    -1,    65,    -1,    64,    -1,    -1,
      53,   166,    -1,   167,    -1,   166,    42,   167,    -1,   166,
      41,   167,    -1,   168,   104,   177,    -1,   168,    63,   177,
      -1,   177,    -1,    -1,    54,    55,   170,    -1,   177,    -1,
     170,    44,   177,    -1,    -1,    59,    55,   172,    -1,   173,
      -1,   172,    44,   173,    -1,   174,   175,    -1,   184,    -1,
      64,    -1,    65,    -1,    66,    -1,    67,    -1,    68,    -1,
      -1,    60,    -1,    61,    -1,    -1,    62,     4,    -1,   178,
      -1,   179,    -1,   178,    24,   179,    -1,   178,    43,   179,
      -1,   180,    -1,   181,    -1,   180,    25,   181,    -1,   180,
      26,   181,    -1,     4,    -1,     6,    -1,   184,    -1,    -1,
      29,   182,   177,    30,    -1,   185,    29,   183,    30,    -1,
     177,    -1,    25,    -1,     3,    -1,     5,    -1,     3,    18,
       3,    -1,     5,    18,     3,    -1,     5,    18,     5,    -1,
       3,    18,     5,    -1,    64,    -1,    65,    -1,    66,    -1,
      67,    -1,    68,    -1
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
    1014,  1018,  1025,  1032,  1034,  1038,  1042,  1049,  1051,  1055,
    1056,  1060,  1067,  1068,  1073,  1078,  1083,  1088,  1096,  1099,
    1104,  1111,  1113,  1121,  1125,  1126,  1130,  1137,  1144,  1145,
    1149,  1157,  1162,  1167,  1168,  1168,  1169,  1173,  1174,  1182,
    1187,  1192,  1198,  1204,  1210,  1221,  1226,  1231,  1236,  1241
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
  "SQL_ASC", "SQL_DESC", "SQL_LIMIT", "SQL_LIKE", "SQL_SUM", "SQL_MIN",
  "SQL_MAX", "SQL_AVG", "SQL_COUNT", "SQL_DIALECT", "$accept",
  "HEAD_SEPARATOR", "program", "rules", "rule", "simple_rule", "head",
  "body", "weight_at_levels", "levels_and_terms", "disjunction",
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
  "from_item", "external_table", "table_ref", "opt_alias", "alias_name",
  "opt_where", "search_condition", "predicate", "predicate_value_expr",
  "opt_groupby", "group_list", "opt_orderby", "order_list", "order_col",
  "order_col_name", "modifier_opt", "opt_limit", "value_expr",
  "value_add_expr", "finalized_mul_expr", "value_mul_expr",
  "value_primary", "@1", "aggregate_arg", "qualified_name",
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
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    71,    72,    72,    72,    73,    73,    74,    74,
      75,    75,    75,    75,    75,    75,    76,    76,    77,    78,
      78,    79,    79,    79,    80,    80,    80,    80,    80,    81,
      81,    82,    82,    82,    82,    83,    84,    85,    85,    86,
      86,    87,    88,    88,    89,    89,    90,    90,    90,    90,
      90,    91,    91,    91,    92,    93,    93,    94,    94,    94,
      95,    95,    96,    97,    97,    97,    98,    99,    99,   100,
     100,   101,   101,   102,   103,   103,   104,   104,   104,   105,
     105,   106,   106,   106,   107,   107,   107,   107,   108,   108,
     108,   109,   109,   110,   110,   111,   111,   112,   112,   112,
     112,   113,   113,   113,   113,   114,   114,   115,   116,   116,
     117,   117,   118,   119,   119,   120,   120,   121,   121,   121,
     122,   123,   124,   125,   126,   126,   127,   128,   129,   129,
     130,   131,   132,   133,   134,   134,   135,   135,   135,   135,
     135,   136,   137,   138,   138,   139,   139,   140,   140,   141,
     142,   143,   143,   144,   145,   145,   146,   146,   147,   148,
     148,   148,   148,   148,   149,   149,   150,   151,   152,   153,
     154,   155,   155,   156,   156,   157,   158,   159,   159,   160,
     160,   160,   161,   161,   162,   162,   163,   163,   164,   164,
     164,   164,   164,   164,   164,   165,   165,   166,   166,   166,
     167,   167,   168,   169,   169,   170,   170,   171,   171,   172,
     172,   173,   174,   174,   174,   174,   174,   174,   175,   175,
     175,   176,   176,   177,   178,   178,   178,   179,   180,   180,
     180,   181,   181,   181,   182,   181,   181,   183,   183,   184,
     184,   184,   184,   184,   184,   185,   185,   185,   185,   185
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
       3,     3,     1,     0,     3,     1,     3,     0,     3,     1,
       3,     2,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     0,     2,     1,     1,     3,     3,     1,     1,     3,
       3,     1,     1,     1,     0,     4,     4,     1,     1,     1,
       1,     3,     3,     3,     3,     1,     1,     1,     1,     1
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
     109,     0,    33,     0,     0,     0,     0,    54,     0,   239,
     231,   240,   232,   171,   234,   245,   246,   247,   248,   249,
       0,   170,   172,   173,   186,   223,   224,   227,   228,   233,
       0,    11,     0,    28,    26,     0,    89,    94,    96,    59,
       0,    67,     0,     0,     0,    30,    72,     0,    73,   132,
     133,   153,     0,   151,     0,   123,   101,   103,   105,   102,
     100,   106,     0,     0,    69,    97,    98,    99,     0,     0,
     155,    68,   113,   114,   112,    37,    34,     0,    40,    42,
       0,    14,    62,    63,     0,   116,     0,     0,     0,     0,
       0,   195,     0,     0,   175,     0,     0,     0,     0,     0,
      12,    32,    58,   143,   142,   144,   150,     0,     0,   104,
     158,     0,     0,     0,     0,     0,   149,    36,     0,     0,
       0,    64,     0,    63,     0,   241,   244,   242,   243,     0,
     184,   185,   183,     0,     0,   176,   177,   186,   186,     0,
     203,   174,   188,   189,   194,   193,   192,   191,   190,   187,
     225,   226,   229,   230,   238,   237,     0,    31,   152,   141,
     156,    44,    70,     0,     0,   154,    43,    19,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   235,     0,     0,
       0,   179,   180,   196,   197,     0,   202,     0,   207,   236,
       0,     0,   107,    23,    21,    20,    65,     0,    66,     0,
     167,    63,   165,   186,     0,   178,     0,     0,     0,     0,
       0,     0,   221,    45,   157,     0,    61,     0,     0,   181,
       0,   199,   198,   201,   200,   204,   205,     0,     0,   166,
      22,    60,   168,    63,     0,   213,   214,   215,   216,   217,
     208,   209,   218,   212,   222,     0,   206,     0,   219,   220,
     211,   182,   210
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    99,    17,    18,    19,    20,    21,    47,   231,   320,
      22,    48,    23,    24,   226,    78,    79,    80,   228,   310,
      49,    50,    25,    51,    52,    53,   233,   270,   271,    74,
     213,    54,    55,   108,   130,   114,   118,    29,    30,   191,
      32,    33,   214,   215,   216,   217,    34,    76,    77,   224,
      88,    72,    36,    37,    57,   143,    58,    59,    60,    61,
      62,    63,   135,   138,    64,    65,    66,    67,   122,   110,
     111,    68,   141,   202,   203,   219,   220,   263,    69,    91,
      92,   326,   352,    93,   170,   171,   172,   173,   241,   285,
     286,   287,   288,   244,   299,   290,   333,   334,   335,   338,
     375,   362,   390,   391,   392,   400,   379,   336,   175,   176,
     177,   178,   239,   306,   179,   180
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -356
static const yytype_int16 yypact[] =
{
     167,  -356,  -356,    32,  -356,  -356,    77,   387,  -356,   241,
     341,   253,   241,   387,   253,   145,    16,   162,   328,  -356,
    -356,   133,   107,  -356,   161,  -356,  -356,   179,  -356,  -356,
    -356,   420,    47,    83,  -356,   453,  -356,  -356,   209,  -356,
    -356,  -356,  -356,  -356,  -356,   441,    31,   198,   187,  -356,
    -356,  -356,  -356,  -356,   208,  -356,   420,   297,  -356,   297,
      94,  -356,   297,   100,  -356,  -356,  -356,   237,   420,   230,
     241,   236,  -356,  -356,   221,   218,   242,  -356,    30,  -356,
     259,  -356,   252,  -356,  -356,   265,   260,  -356,    63,  -356,
     267,  -356,  -356,   120,  -356,  -356,  -356,   376,  -356,    56,
     253,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,    51,  -356,  -356,  -356,    51,   434,
    -356,   244,    45,   253,  -356,  -356,  -356,   387,   241,   479,
     241,   494,   499,  -356,  -356,  -356,   241,  -356,  -356,   241,
     253,   264,   241,  -356,  -356,  -356,   321,  -356,   241,    34,
    -356,   253,   420,   405,   273,   423,   307,  -356,   269,   304,
    -356,   310,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
     277,  -356,   286,  -356,   281,    60,  -356,   156,  -356,  -356,
     309,  -356,   324,  -356,  -356,    66,  -356,    83,  -356,  -356,
      90,  -356,   334,   308,   343,  -356,  -356,   420,  -356,  -356,
    -356,  -356,   -21,  -356,   321,  -356,   323,  -356,   329,   331,
    -356,  -356,   349,    57,  -356,  -356,  -356,  -356,   333,   101,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,   241,   319,  -356,
     241,  -356,  -356,   241,    -1,  -356,   335,   172,   203,   249,
      43,   313,   249,    28,  -356,   249,   249,   249,   249,   235,
    -356,   420,  -356,  -356,  -356,  -356,  -356,   253,   112,  -356,
    -356,   405,   321,   321,   241,   321,  -356,  -356,   405,    72,
      38,  -356,   332,   241,   316,  -356,  -356,  -356,  -356,   338,
    -356,  -356,  -356,    16,   253,   351,  -356,   281,   281,   249,
     347,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,   372,  -356,  -356,  -356,
     359,  -356,  -356,     9,   113,  -356,  -356,  -356,   241,   241,
     345,   241,   241,   241,    38,   400,   383,  -356,   384,   388,
      43,  -356,  -356,   117,  -356,   255,  -356,   358,   361,  -356,
     405,   405,  -356,   221,   378,  -356,  -356,   134,  -356,   241,
    -356,   241,  -356,   281,   241,  -356,   249,   249,   249,   249,
     249,   368,   363,  -356,   359,   241,  -356,   149,   153,  -356,
      -1,  -356,  -356,  -356,  -356,   397,  -356,   158,   427,  -356,
     221,  -356,  -356,   241,   249,  -356,  -356,  -356,  -356,  -356,
     398,  -356,   159,  -356,  -356,   160,  -356,   158,  -356,  -356,
    -356,  -356,  -356
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -356,  -356,  -356,  -356,   431,  -356,  -356,   -10,  -356,  -356,
    -356,  -356,  -356,  -356,   210,   367,   317,  -356,  -356,   137,
    -149,   352,   375,     6,    29,    17,  -226,  -268,   164,  -117,
     217,  -356,   353,    20,   -30,  -356,  -356,  -356,   365,     0,
    -356,   379,   224,  -356,  -356,  -356,   346,  -356,  -356,  -356,
    -356,    14,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,   426,   428,  -356,  -356,  -356,  -356,  -356,   143,
     194,   150,  -356,  -356,   243,   294,   234,  -356,  -356,   231,
     355,  -356,  -356,  -356,  -356,  -356,  -356,   274,  -356,  -356,
     171,  -356,  -356,  -272,  -356,  -356,  -356,  -128,  -356,  -356,
    -356,  -356,  -356,   118,  -356,  -356,  -356,   -73,  -356,   -11,
    -356,    -5,  -356,  -356,  -355,  -356
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -131
static const yytype_int16 yytable[] =
{
      31,   109,   190,    85,   229,   324,    26,    56,   273,    71,
      75,   256,    84,    56,    35,   331,   332,    28,    31,   232,
     174,    35,   393,   257,    26,    82,    35,    35,    86,    27,
     341,   292,    35,   293,     2,    28,     4,     5,   234,   222,
      81,    83,   393,   148,   322,    71,   280,    27,   281,   282,
     151,    38,   124,   262,     2,   186,     4,     5,   232,     2,
      82,     4,     5,   125,   152,    10,     2,    89,     4,     5,
      84,   112,   283,    90,   123,   193,   129,   260,   261,    14,
       9,   369,   321,   368,   245,    39,   151,   182,   142,   194,
     113,   284,   294,   295,   296,   297,   298,    56,   349,   123,
     251,   262,    82,   246,   317,   184,    15,   156,   115,   116,
     117,    35,   311,    82,    82,   395,   318,   157,   319,   316,
     252,   265,   227,   159,   160,   161,   162,    56,   197,    81,
     198,   104,   265,   106,   148,   266,   199,    82,   105,   200,
     107,    35,   205,   342,   383,   163,   309,   314,   221,   164,
      87,    96,    83,   197,   201,    97,    98,   148,   356,   357,
     218,   159,    94,   161,   366,    82,   279,    35,     1,   174,
       2,     3,     4,     5,     6,   275,   305,   276,   148,   381,
      81,   247,   248,   382,   165,   166,   167,   168,   169,     7,
     401,   363,   311,   148,   100,     8,     9,   321,    10,   131,
      11,   343,   101,   136,   321,   347,   277,   133,   278,   134,
      12,   136,   137,   120,    13,    14,   126,    15,   218,   398,
     399,   227,   385,   386,   387,   388,   389,   267,   371,   372,
     269,   127,   367,   272,   300,   301,    16,   370,   159,   160,
     161,   162,   302,   303,     2,     3,     4,     5,   380,   128,
     132,   140,   159,   160,   161,   162,     2,   139,     4,     5,
     304,   197,   139,   146,   164,   148,   147,   149,   197,     8,
       9,   201,    10,   272,   150,    35,   218,   218,   164,   218,
     153,   119,    35,   154,    70,   373,   374,   376,   192,   155,
     102,   103,   104,   105,   106,   107,   158,   204,   329,   165,
     166,   167,   168,   169,   230,   359,    40,    41,    42,    43,
      44,   396,   235,   165,   166,   167,   168,   169,   358,   344,
      89,   272,   237,   348,   206,   207,   208,   209,   238,   240,
     242,     2,     3,     4,     5,     6,   210,   243,   249,   253,
     197,   197,   250,   254,     2,     3,     4,     5,   255,   211,
       7,   272,  -117,   259,    35,    35,     8,     9,  -119,    10,
    -118,    11,   264,   268,   212,   274,   289,   323,   327,     8,
       9,    12,    10,    73,   325,    13,    14,   345,    15,     2,
       3,     4,     5,   272,    70,    40,    41,    42,    43,    44,
       2,     3,     4,     5,   181,   330,    40,    41,    42,    43,
      44,   337,   339,   340,     8,    45,   350,    10,     2,     3,
       4,     5,   351,   360,   353,     8,    45,   354,    10,    12,
     361,    46,   365,   377,    14,   378,     2,     3,     4,     5,
      12,   394,    46,     8,     9,    14,    10,     2,     3,     4,
       5,   384,   397,   232,     2,     3,   121,     5,    12,    95,
      46,     8,     9,    14,    10,   102,   103,   104,   105,   106,
     107,   307,     8,     9,   189,    10,    70,   185,   225,     8,
       9,   -57,    10,   -57,   183,   -57,   -57,    70,   364,   195,
     313,   196,   119,   188,    70,   346,   312,   -57,  -122,  -122,
    -122,  -122,  -122,   187,   144,   223,   145,   -57,   258,   315,
     308,   355,   -57,  -126,  -126,  -126,  -126,  -126,  -130,  -130,
    -130,  -130,  -130,   236,   328,   402,   291
};

static const yytype_uint16 yycheck[] =
{
       0,    31,   119,    13,   153,   273,     0,     7,   234,     9,
      10,    32,    12,    13,     0,   287,   288,     0,    18,    20,
      93,     7,   377,    44,    18,    11,    12,    13,    14,     0,
      21,     3,    18,     5,     3,    18,     5,     6,   155,     5,
      11,    12,   397,    44,   270,    45,     3,    18,     5,     6,
      20,    19,    46,    44,     3,     4,     5,     6,    20,     3,
      46,     5,     6,    46,    34,    31,     3,    51,     5,     6,
      70,    24,    29,    57,    43,    30,    56,    20,    21,    48,
      29,   353,    44,   351,    24,     8,    20,    97,    68,    44,
      43,    48,    64,    65,    66,    67,    68,    97,   324,    43,
      34,    44,    88,    43,    32,    99,    50,    44,    25,    26,
      27,    97,   261,    99,   100,   383,    44,    88,    46,   268,
      30,    20,   152,     3,     4,     5,     6,   127,   128,   100,
     130,    37,    20,    39,    44,    34,   136,   123,    38,   139,
      40,   127,   142,    30,   370,    25,    34,   264,   148,    29,
       5,    18,   123,   153,   140,    22,    49,    44,    41,    42,
     146,     3,     0,     5,    30,   151,   239,   153,     1,   242,
       3,     4,     5,     6,     7,     3,   249,     5,    44,    30,
     151,    25,    26,    30,    64,    65,    66,    67,    68,    22,
      30,   340,   341,    44,    33,    28,    29,    44,    31,    56,
      33,   318,    23,    60,    44,   322,     3,    57,     5,    59,
      43,    68,    62,     4,    47,    48,    18,    50,   204,    60,
      61,   251,    64,    65,    66,    67,    68,   227,   356,   357,
     230,    44,   349,   233,   245,   246,    69,   354,     3,     4,
       5,     6,   247,   248,     3,     4,     5,     6,   365,    41,
      56,    14,     3,     4,     5,     6,     3,    63,     5,     6,
      25,   261,    68,    33,    29,    44,    30,    49,   268,    28,
      29,   257,    31,   273,    32,   261,   262,   263,    29,   265,
      21,    29,   268,    18,    43,   358,   359,   360,    44,    29,
      35,    36,    37,    38,    39,    40,    29,    33,   284,    64,
      65,    66,    67,    68,    31,   335,     9,    10,    11,    12,
      13,   384,     5,    64,    65,    66,    67,    68,    63,   319,
      51,   321,    18,   323,     3,     4,     5,     6,    18,    52,
      44,     3,     4,     5,     6,     7,    15,    56,    29,     5,
     340,   341,    18,    35,     3,     4,     5,     6,     5,    28,
      22,   351,    29,     4,   340,   341,    28,    29,    29,    31,
      29,    33,    29,    44,    43,    30,    53,    35,    30,    28,
      29,    43,    31,    32,    58,    47,    48,    32,    50,     3,
       4,     5,     6,   383,    43,     9,    10,    11,    12,    13,
       3,     4,     5,     6,    18,    44,     9,    10,    11,    12,
      13,    54,    30,    44,    28,    29,     6,    31,     3,     4,
       5,     6,    29,    55,    30,    28,    29,    29,    31,    43,
      59,    45,    44,    55,    48,    62,     3,     4,     5,     6,
      43,     4,    45,    28,    29,    48,    31,     3,     4,     5,
       6,    44,    44,    20,     3,     4,     5,     6,    43,    18,
      45,    28,    29,    48,    31,    35,    36,    37,    38,    39,
      40,   251,    28,    29,    30,    31,    43,   100,   151,    28,
      29,    18,    31,    20,    99,    22,    23,    43,   341,   127,
     263,   128,    29,   118,    43,   321,   262,    34,     9,    10,
      11,    12,    13,   114,    68,   149,    68,    44,   204,   265,
     257,   330,    49,     9,    10,    11,    12,    13,     9,    10,
      11,    12,    13,   158,   283,   397,   242
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    22,    28,    29,
      31,    33,    43,    47,    48,    50,    69,    72,    73,    74,
      75,    76,    80,    82,    83,    92,    93,    94,    95,   107,
     108,   109,   110,   111,   116,   121,   122,   123,    19,     8,
       9,    10,    11,    12,    13,    29,    45,    77,    81,    90,
      91,    93,    94,    95,   101,   102,   109,   124,   126,   127,
     128,   129,   130,   131,   134,   135,   136,   137,   141,   148,
      43,   109,   121,    32,    99,   109,   117,   118,    85,    86,
      87,    94,   121,    94,   109,    77,   121,     5,   120,    51,
      57,   149,   150,   153,     0,    74,    18,    22,    49,    71,
      33,    23,    35,    36,    37,    38,    39,    40,   103,   104,
     139,   140,    24,    43,   105,    25,    26,    27,   106,    29,
       4,     5,   138,    43,    93,    95,    18,    44,    41,   103,
     104,   139,   140,   141,   141,   132,   139,   141,   133,   140,
      14,   142,   103,   125,   132,   133,    33,    30,    44,    49,
      32,    20,    34,    21,    18,    29,    44,    94,    29,     3,
       4,     5,     6,    25,    29,    64,    65,    66,    67,    68,
     154,   155,   156,   157,   177,   178,   179,   180,   181,   184,
     185,    18,    77,    92,    93,    85,     4,   111,   108,    30,
      99,   109,    44,    30,    44,    91,   102,   109,   109,   109,
     109,   121,   143,   144,    33,   109,     3,     4,     5,     6,
      15,    28,    43,   100,   112,   113,   114,   115,   121,   145,
     146,   109,     5,   116,   119,    86,    84,   104,    88,    90,
      31,    78,    20,    96,    99,     5,   150,    18,    18,   182,
      52,   158,    44,    56,   163,    24,    43,    25,    26,    29,
      18,    34,    30,     5,    35,     5,    32,    44,   145,     4,
      20,    21,    44,   147,    29,    20,    34,   109,    44,   109,
      97,    98,   109,    96,    30,     3,     5,     3,     5,   177,
       3,     5,     6,    29,    48,   159,   160,   161,   162,    53,
     165,   157,     3,     5,    64,    65,    66,    67,    68,   164,
     179,   179,   181,   181,    25,   177,   183,    84,   144,    34,
      89,    90,   112,   100,    99,   146,    90,    32,    44,    46,
      79,    44,    96,    35,    97,    58,   151,    30,   149,   121,
      44,   163,   163,   166,   167,   168,   177,    54,   169,    30,
      44,    21,    30,    99,   109,    32,    98,    99,   109,    96,
       6,    29,   152,    30,    29,   160,    41,    42,    63,   104,
      55,    59,   171,    90,    89,    44,    30,    99,    97,   163,
      99,   167,   167,   177,   177,   170,   177,    55,    62,   176,
      99,    30,    30,    96,    44,    64,    65,    66,    67,    68,
     172,   173,   174,   184,     4,    97,   177,    44,    60,    61,
     175,    30,   173
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
#line 1019 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLikePredicate();
    }
    break;

  case 202:
#line 1026 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLPredicateValueExpr();
    }
    break;

  case 205:
#line 1039 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 206:
#line 1043 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLGroupByItem();
    }
    break;

  case 211:
#line 1061 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderCol();
    }
    break;

  case 213:
#line 1069 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 214:
#line 1074 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 215:
#line 1079 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 216:
#line 1084 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 217:
#line 1089 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 218:
#line 1096 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSqlOrderModifier(nullptr);
    }
    break;

  case 219:
#line 1100 "src/parser/aspcore2.y"
    {
        char c[] = "asc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 220:
#line 1105 "src/parser/aspcore2.y"
    {
        char c[] = "desc";
        director.getBuilder()->onSqlOrderModifier(c);
    }
    break;

  case 222:
#line 1114 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLLimit((yyvsp[(2) - (2)].string));
        delete[] (yyvsp[(2) - (2)].string);
    }
    break;

  case 225:
#line 1127 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAddTerm('+');
    }
    break;

  case 226:
#line 1131 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAddTerm('-');
    }
    break;

  case 227:
#line 1138 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLFinalizeMulExpr();
    }
    break;

  case 229:
#line 1146 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLMulTerm('*');
    }
    break;

  case 230:
#line 1150 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLMulTerm('/');
    }
    break;

  case 231:
#line 1158 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 232:
#line 1163 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLValue((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 234:
#line 1168 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLParenOpen(); }
    break;

  case 235:
#line 1168 "src/parser/aspcore2.y"
    { director.getBuilder()->onSQLParenClose(); }
    break;

  case 238:
#line 1175 "src/parser/aspcore2.y"
    {
        char c[] = "*";
        director.getBuilder()->onSQLValue(c);
    }
    break;

  case 239:
#line 1183 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 240:
#line 1188 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLQualifiedName((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 241:
#line 1193 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 242:
#line 1199 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 243:
#line 1205 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 244:
#line 1211 "src/parser/aspcore2.y"
    {
         director.getBuilder()->onSQLQualifiedName((yyvsp[(3) - (3)].string), (yyvsp[(1) - (3)].string));
         delete[] (yyvsp[(1) - (3)].string);
         delete[] (yyvsp[(3) - (3)].string);
    }
    break;

  case 245:
#line 1222 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 246:
#line 1227 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 247:
#line 1232 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 248:
#line 1237 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;

  case 249:
#line 1242 "src/parser/aspcore2.y"
    {
        director.getBuilder()->onSQLAggregateFunction((yyvsp[(1) - (1)].string));
        delete[] (yyvsp[(1) - (1)].string);
    }
    break;


/* Line 1267 of yacc.c.  */
#line 3195 "src/parser/aspcore2_parser.c"
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



