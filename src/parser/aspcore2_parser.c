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
     EXISTS = 301
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

#include "bumblebee/parser/ParserInputDirector.h"
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
#line 224 "src/parser/aspcore2_parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 237 "src/parser/aspcore2_parser.c"

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
#define YYFINAL  80
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   372

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  67
/* YYNRULES -- Number of rules.  */
#define YYNRULES  142
/* YYNRULES -- Number of states.  */
#define YYNSTATES  225

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

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
      45,    46
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     8,    10,    12,    15,    17,
      19,    22,    26,    31,    35,    40,    42,    44,    46,    48,
      52,    57,    60,    65,    68,    70,    74,    76,    80,    82,
      86,    92,    97,   101,   106,   109,   112,   116,   118,   120,
     124,   126,   128,   132,   134,   138,   140,   143,   145,   147,
     149,   152,   156,   158,   161,   163,   168,   172,   174,   178,
     180,   184,   188,   190,   192,   194,   196,   198,   200,   202,
     204,   206,   208,   210,   215,   219,   222,   224,   226,   228,
     232,   234,   236,   238,   242,   244,   248,   250,   252,   254,
     256,   258,   260,   263,   265,   267,   272,   275,   279,   281,
     283,   287,   289,   291,   293,   297,   299,   301,   303,   306,
     309,   312,   315,   318,   321,   324,   327,   329,   331,   334,
     337,   340,   343,   346,   349,   351,   353,   355,   358,   361,
     363,   365,   367,   369,   374,   378,   382,   384,   388,   390,
     392,   394,   396
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      49,     0,    -1,    45,    -1,    -1,    50,    -1,     1,    -1,
      51,    -1,    50,    51,    -1,    52,    -1,    94,    -1,    53,
      16,    -1,    53,    20,    16,    -1,    53,    20,    54,    16,
      -1,    20,    54,    16,    -1,    43,    54,    16,    55,    -1,
      95,    -1,    57,    -1,    59,    -1,    58,    -1,    29,    81,
      30,    -1,    29,    81,    56,    30,    -1,    42,    81,    -1,
      42,    81,    40,    72,    -1,    40,    72,    -1,    70,    -1,
      57,    48,    70,    -1,    69,    -1,    57,    48,    69,    -1,
      68,    -1,    58,    40,    68,    -1,    60,    31,    62,    32,
      61,    -1,    60,    31,    62,    32,    -1,    31,    62,    32,
      -1,    31,    62,    32,    61,    -1,    81,    76,    -1,    76,
      81,    -1,    62,    18,    63,    -1,    63,    -1,    64,    -1,
      64,    19,    65,    -1,    71,    -1,    67,    -1,    65,    40,
      67,    -1,    67,    -1,    66,    40,    67,    -1,    70,    -1,
      41,    70,    -1,    74,    -1,    67,    -1,   107,    -1,    41,
     107,    -1,    46,    92,    71,    -1,    71,    -1,    39,    71,
      -1,    93,    -1,    93,    27,    72,    28,    -1,    93,    27,
      28,    -1,    81,    -1,    72,    40,    81,    -1,    84,    -1,
      73,    40,    84,    -1,    81,    76,    81,    -1,    33,    -1,
      34,    -1,    75,    -1,   108,    -1,   109,    -1,    22,    -1,
      39,    -1,    23,    -1,    24,    -1,    25,    -1,    26,    -1,
      93,    27,    72,    28,    -1,     4,    17,     4,    -1,    39,
      81,    -1,    88,    -1,    93,    -1,     4,    -1,    27,    81,
      28,    -1,    79,    -1,    82,    -1,    83,    -1,    82,    77,
      83,    -1,    80,    -1,    83,    78,    80,    -1,    85,    -1,
      86,    -1,    87,    -1,     3,    -1,     6,    -1,     4,    -1,
      39,     4,    -1,     5,    -1,    26,    -1,    93,    27,    72,
      28,    -1,    29,    30,    -1,    29,    89,    30,    -1,    72,
      -1,    90,    -1,    81,    45,    91,    -1,     5,    -1,    88,
      -1,     5,    -1,    92,    40,     5,    -1,     3,    -1,     6,
      -1,     5,    -1,     7,     8,    -1,    71,    21,    -1,    81,
      75,    -1,    75,    81,    -1,    96,   110,    -1,   110,    97,
      -1,    81,   108,    -1,    99,   110,    -1,   100,    -1,   103,
      -1,    81,   109,    -1,   102,   110,    -1,   108,    81,    -1,
     109,    81,    -1,   110,   104,    -1,   110,   105,    -1,   101,
      -1,   106,    -1,    98,    -1,   100,   104,    -1,   103,   105,
      -1,    35,    -1,    37,    -1,    36,    -1,    38,    -1,   113,
      31,   111,    32,    -1,   113,    31,    32,    -1,   111,    18,
     112,    -1,   112,    -1,    73,    19,    66,    -1,     9,    -1,
      10,    -1,    11,    -1,    12,    -1,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    73,    73,    75,    77,    78,    83,    84,    88,    89,
      93,    97,   101,   105,   109,   113,   121,   125,   133,   140,
     145,   153,   158,   162,   172,   176,   180,   184,   191,   195,
     202,   203,   204,   205,   209,   216,   223,   224,   228,   232,
     239,   246,   250,   257,   261,   268,   272,   276,   283,   284,
     288,   295,   301,   302,   306,   311,   316,   325,   326,   330,
     331,   335,   342,   346,   353,   354,   355,   360,   361,   365,
     366,   367,   371,   375,   380,   386,   390,   395,   400,   405,
     412,   413,   419,   420,   428,   429,   435,   436,   437,   441,
     446,   451,   456,   464,   469,   476,   484,   488,   495,   499,
     506,   515,   520,   524,   529,   537,   538,   539,   543,   552,
     559,   566,   573,   574,   578,   585,   589,   590,   594,   601,
     605,   612,   619,   620,   624,   625,   626,   627,   628,   632,
     636,   643,   647,   654,   655,   659,   660,   665,   672,   677,
     682,   687,   692
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
  "COMMA", "NAF", "AT", "WCONS", "AMPERSAND", "VEL", "EXISTS", "$accept",
  "HEAD_SEPARATOR", "program", "rules", "rule", "simple_rule", "head",
  "body", "weight_at_levels", "levels_and_terms", "disjunction",
  "conjunction", "choice_atom", "lower_guard", "upper_guard",
  "choice_elements", "choice_element", "choice_element_atom",
  "choice_elements_literals", "naf_literals", "naf_literal",
  "naf_literal_aggregate", "existential_atom", "classic_literal", "atom",
  "terms", "basic_terms", "builtin_atom", "compareop", "binop", "arithop1",
  "arithop2", "term_", "term__", "term", "expr", "factor", "basic_term",
  "ground_term", "variable_term", "functional_term", "list_term",
  "list_terms_term", "list_head_tail_term", "list_tail_term", "vars",
  "identifier", "directive", "query", "lower_guard_compare_aggregate",
  "upper_guard_compare_aggregate", "compare_aggregate",
  "lower_guard_leftward_left_aggregate", "leftward_left_aggregate",
  "left_aggregate", "lower_guard_rightward_left_aggregate",
  "rightward_left_aggregate", "upper_guard_leftward_right_aggregate",
  "upper_guard_rightward_right_aggregate", "right_aggregate",
  "aggregate_atom", "leftwardop", "rightwardop", "aggregate",
  "aggregate_elements", "aggregate_element", "aggregate_function", 0
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
     295,   296,   297,   298,   299,   300,   301
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    49,    49,    49,    50,    50,    51,    51,
      52,    52,    52,    52,    52,    52,    53,    53,    54,    55,
      55,    56,    56,    56,    57,    57,    57,    57,    58,    58,
      59,    59,    59,    59,    60,    61,    62,    62,    63,    63,
      64,    65,    65,    66,    66,    67,    67,    67,    68,    68,
      68,    69,    70,    70,    71,    71,    71,    72,    72,    73,
      73,    74,    75,    75,    76,    76,    76,    77,    77,    78,
      78,    78,    79,    79,    79,    79,    79,    80,    80,    80,
      81,    81,    82,    82,    83,    83,    84,    84,    84,    85,
      85,    85,    85,    86,    86,    87,    88,    88,    89,    89,
      90,    91,    91,    92,    92,    93,    93,    93,    94,    95,
      96,    97,    98,    98,    99,   100,   101,   101,   102,   103,
     104,   105,   106,   106,   107,   107,   107,   107,   107,   108,
     108,   109,   109,   110,   110,   111,   111,   112,   113,   113,
     113,   113,   113
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     1,     1,     1,     2,     1,     1,
       2,     3,     4,     3,     4,     1,     1,     1,     1,     3,
       4,     2,     4,     2,     1,     3,     1,     3,     1,     3,
       5,     4,     3,     4,     2,     2,     3,     1,     1,     3,
       1,     1,     3,     1,     3,     1,     2,     1,     1,     1,
       2,     3,     1,     2,     1,     4,     3,     1,     3,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     3,     2,     1,     1,     1,     3,
       1,     1,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     4,     2,     3,     1,     1,
       3,     1,     1,     1,     3,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     2,     2,
       2,     2,     2,     2,     1,     1,     1,     2,     2,     1,
       1,     1,     1,     4,     3,     3,     1,     3,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,   105,    78,   107,   106,     0,     0,    72,     0,
       0,     0,     0,     0,     0,     0,     4,     6,     8,     0,
      16,    17,     0,    26,    24,    52,    80,    84,     0,    81,
      82,    76,    77,     9,    15,     0,   108,   138,   139,   140,
     141,   142,     0,     0,    18,    48,    28,    45,    52,    47,
       0,     0,   126,     0,   116,   124,     0,   117,   125,    49,
       0,     0,     0,     0,    77,    96,    98,    57,     0,    99,
       0,    37,    38,    40,    54,    53,    75,     0,   103,     0,
       1,     7,    10,     0,     2,     0,     0,   109,    62,    63,
     129,   131,   130,   132,    64,    34,    65,    66,    67,    68,
       0,    69,    70,    71,     0,     0,    74,    46,     0,    50,
      13,     0,    64,     0,    65,    66,   112,   115,   127,     0,
     119,   128,     0,     0,   113,   122,   123,     0,    79,     0,
       0,     0,    97,     0,    32,     0,     0,     0,     0,    51,
      11,     0,     0,    27,    25,     0,    78,    83,    77,    85,
      56,     0,    57,   110,   114,   118,    29,    61,   120,   121,
     111,    89,    91,    93,    90,    94,   134,     0,     0,    59,
      86,    87,    88,     0,     0,   136,     0,    58,   101,   102,
     100,    36,    33,     0,     0,    39,    41,     0,     0,     0,
      14,   104,    12,    31,    55,    92,     0,     0,     0,     0,
     133,    73,    35,     0,    55,     0,    30,   137,    43,    60,
       0,   135,    42,    19,     0,     0,     0,     0,    95,    23,
      21,    20,    44,     0,    22
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    85,    15,    16,    17,    18,    19,    43,   190,   216,
      20,    44,    21,    22,   182,    70,    71,    72,   185,   207,
      45,    46,    23,    47,    48,    66,   168,    49,    94,   113,
     100,   104,    26,    27,   152,    29,    30,   169,   170,   171,
     172,    31,    68,    69,   180,    79,    64,    33,    34,    51,
     124,    52,    53,    54,    55,    56,    57,   118,   121,    58,
      59,    96,    97,    60,   174,   175,    61
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -128
static const yytype_int16 yypact[] =
{
     148,  -128,  -128,    12,  -128,  -128,    24,   285,  -128,   326,
     312,   158,   326,   285,    98,   108,   202,  -128,  -128,    81,
      85,  -128,    87,  -128,  -128,   100,  -128,  -128,   201,   -18,
     116,  -128,   327,  -128,  -128,   132,  -128,  -128,  -128,  -128,
    -128,  -128,   296,   127,   106,  -128,  -128,  -128,  -128,  -128,
     201,   160,  -128,   160,    25,  -128,   160,    76,  -128,  -128,
     201,   131,   326,   119,   140,  -128,   136,   133,   155,  -128,
      46,  -128,   169,  -128,   162,  -128,  -128,   174,  -128,    40,
    -128,  -128,  -128,   246,  -128,    35,   158,  -128,  -128,  -128,
    -128,  -128,  -128,  -128,  -128,  -128,  -128,  -128,  -128,  -128,
      44,  -128,  -128,  -128,    44,   154,  -128,  -128,   201,  -128,
    -128,   285,   203,   326,   267,   351,  -128,  -128,  -128,   326,
    -128,  -128,   326,   326,  -128,  -128,  -128,   221,  -128,   326,
     326,    60,  -128,   158,   201,    90,   154,   163,   190,  -128,
    -128,   184,   158,  -128,  -128,    73,  -128,   116,  -128,  -128,
    -128,   -23,  -128,  -128,  -128,  -128,  -128,  -128,  -128,  -128,
    -128,   175,  -128,   183,   192,  -128,  -128,   197,    32,  -128,
    -128,  -128,  -128,   193,    88,  -128,    42,  -128,  -128,  -128,
    -128,  -128,  -128,   326,    53,   181,  -128,   201,    82,   326,
    -128,  -128,  -128,   201,   230,  -128,    90,    49,   326,    49,
    -128,  -128,  -128,    90,  -128,    83,  -128,   200,  -128,  -128,
     104,  -128,  -128,  -128,   326,   326,   212,    90,  -128,   136,
     204,  -128,  -128,   326,   136
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -128,  -128,  -128,  -128,   207,  -128,  -128,   -10,  -128,  -128,
    -128,  -128,  -128,  -128,    50,   168,   113,  -128,  -128,  -128,
    -127,   150,   185,     2,    23,   -99,  -128,  -128,   -41,   -27,
    -128,  -128,  -128,   165,    15,  -128,   171,    77,  -128,  -128,
    -128,   151,  -128,  -128,  -128,  -128,     0,  -128,  -128,  -128,
    -128,  -128,  -128,  -128,  -128,  -128,  -128,   223,   224,  -128,
     239,   -40,   -24,    10,  -128,    93,  -128
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -119
static const yytype_int16 yytable[] =
{
      32,    95,    24,    77,    98,   194,   151,    32,   186,   112,
     114,    74,    32,    32,   119,    28,    32,   130,    24,   123,
     119,    99,    50,    25,    63,    67,   115,    76,    50,    35,
     176,    28,    36,   122,    73,    75,   122,   188,     2,    25,
       4,     5,    32,     2,   107,     4,     5,     2,   146,     4,
       5,   196,   161,   162,   163,   164,     2,   108,     4,     5,
      90,   116,    92,   117,   133,   178,   120,   153,   154,   208,
     201,     9,   197,   141,   142,   165,   212,    76,   134,    74,
     138,    14,   130,    32,   155,    74,    74,   144,   167,    10,
     222,   133,   142,     2,     3,     4,     5,    82,    50,   210,
     148,    83,   139,    78,   148,   193,   199,   183,    80,    73,
     204,    32,    91,   213,    93,   219,     8,     9,    86,    10,
     200,    87,   130,   214,   224,   215,    50,   173,   157,    12,
      84,   184,   218,    74,   158,    32,   106,   159,   160,   101,
     102,   103,    74,   110,   130,   177,   111,   128,    -3,     1,
     187,     2,     3,     4,     5,     6,    73,     2,     3,     4,
       5,     2,   127,     4,     5,    75,   183,   129,     7,    37,
      38,    39,    40,    41,     8,     9,   130,    10,   131,    11,
       8,     9,   150,    10,    74,   132,   107,    12,   135,   136,
     137,    13,   189,    62,    14,   191,    32,   173,   202,   173,
     192,   195,  -105,    32,   205,     2,     3,     4,     5,     6,
    -107,   187,  -110,  -110,  -110,  -110,  -110,    32,   187,  -106,
     198,   203,     7,    81,   161,   162,   163,   164,     8,     9,
     220,    10,   187,    11,    88,    89,    90,    91,    92,    93,
     217,    12,   221,   206,   223,    13,   181,   165,    14,     2,
       3,     4,     5,   166,   145,    37,    38,    39,    40,    41,
     167,   156,   140,   -73,   -73,   -73,   -73,   -73,   -73,   149,
     143,   147,     8,     9,   209,    10,  -114,  -114,  -114,  -114,
    -114,   109,   179,   125,   126,    12,     0,    42,     2,     3,
       4,     5,   211,     0,    37,    38,    39,    40,    41,     2,
       3,     4,     5,     0,     0,    37,    38,    39,    40,    41,
       0,     8,     9,     0,    10,     2,     3,     4,     5,     0,
       0,     0,     8,     9,    12,    10,    42,     0,     0,     2,
       3,     4,     5,     0,     0,    12,     0,     0,     8,     9,
       0,    10,    65,   -54,     0,   -54,     0,   -54,   -54,     0,
       0,    62,     8,     9,   105,    10,     0,     0,     0,   -54,
    -118,  -118,  -118,  -118,  -118,    62,     0,   -54,     0,     0,
       0,     0,   -54
};

static const yytype_int16 yycheck[] =
{
       0,    28,     0,    13,    22,    28,   105,     7,   135,    50,
      50,    11,    12,    13,    54,     0,    16,    40,    16,    60,
      60,    39,     7,     0,     9,    10,    50,    12,    13,    17,
     129,    16,     8,    57,    11,    12,    60,   136,     3,    16,
       5,     6,    42,     3,    42,     5,     6,     3,     4,     5,
       6,    19,     3,     4,     5,     6,     3,    42,     5,     6,
      35,    51,    37,    53,    18,     5,    56,   108,   108,   196,
      28,    27,    40,    83,    39,    26,   203,    62,    32,    79,
      40,    46,    40,    83,   108,    85,    86,    85,    39,    29,
     217,    18,    39,     3,     4,     5,     6,    16,    83,   198,
     100,    20,    79,     5,   104,    32,    18,   134,     0,    86,
      28,   111,    36,    30,    38,   214,    26,    27,    31,    29,
      32,    21,    40,    40,   223,    42,   111,   127,   113,    39,
      45,    41,    28,   133,   119,   135,     4,   122,   123,    23,
      24,    25,   142,    16,    40,   130,    40,    28,     0,     1,
     135,     3,     4,     5,     6,     7,   133,     3,     4,     5,
       6,     3,    31,     5,     6,   142,   193,    27,    20,     9,
      10,    11,    12,    13,    26,    27,    40,    29,    45,    31,
      26,    27,    28,    29,   184,    30,   184,    39,    19,    27,
      16,    43,    29,    39,    46,     5,   196,   197,   183,   199,
      16,     4,    27,   203,   189,     3,     4,     5,     6,     7,
      27,   196,     9,    10,    11,    12,    13,   217,   203,    27,
      27,    40,    20,    16,     3,     4,     5,     6,    26,    27,
     215,    29,   217,    31,    33,    34,    35,    36,    37,    38,
      40,    39,    30,   193,    40,    43,   133,    26,    46,     3,
       4,     5,     6,    32,    86,     9,    10,    11,    12,    13,
      39,   111,    16,    33,    34,    35,    36,    37,    38,   104,
      85,   100,    26,    27,   197,    29,     9,    10,    11,    12,
      13,    42,   131,    60,    60,    39,    -1,    41,     3,     4,
       5,     6,   199,    -1,     9,    10,    11,    12,    13,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    13,
      -1,    26,    27,    -1,    29,     3,     4,     5,     6,    -1,
      -1,    -1,    26,    27,    39,    29,    41,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,    39,    -1,    -1,    26,    27,
      -1,    29,    30,    16,    -1,    18,    -1,    20,    21,    -1,
      -1,    39,    26,    27,    27,    29,    -1,    -1,    -1,    32,
       9,    10,    11,    12,    13,    39,    -1,    40,    -1,    -1,
      -1,    -1,    45
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,    20,    26,    27,
      29,    31,    39,    43,    46,    49,    50,    51,    52,    53,
      57,    59,    60,    69,    70,    71,    79,    80,    81,    82,
      83,    88,    93,    94,    95,    17,     8,     9,    10,    11,
      12,    13,    41,    54,    58,    67,    68,    70,    71,    74,
      81,    96,    98,    99,   100,   101,   102,   103,   106,   107,
     110,   113,    39,    81,    93,    30,    72,    81,    89,    90,
      62,    63,    64,    71,    93,    71,    81,    54,     5,    92,
       0,    51,    16,    20,    45,    48,    31,    21,    33,    34,
      35,    36,    37,    38,    75,    76,   108,   109,    22,    39,
      77,    23,    24,    25,    78,    27,     4,    70,    81,   107,
      16,    40,    75,    76,   108,   109,   110,   110,   104,   108,
     110,   105,   109,    75,    97,   104,   105,    31,    28,    27,
      40,    45,    30,    18,    32,    19,    27,    16,    40,    71,
      16,    54,    39,    69,    70,    62,     4,    83,    93,    80,
      28,    72,    81,    75,   108,   109,    68,    81,    81,    81,
      81,     3,     4,     5,     6,    26,    32,    39,    73,    84,
      85,    86,    87,    93,   111,   112,    72,    81,     5,    88,
      91,    63,    61,    76,    41,    65,    67,    81,    72,    29,
      55,     5,    16,    32,    28,     4,    19,    40,    27,    18,
      32,    28,    81,    40,    28,    81,    61,    66,    67,    84,
      72,   112,    67,    30,    40,    42,    56,    40,    28,    72,
      81,    30,    67,    40,    72
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
        case 4:
#line 77 "src/parser/aspcore2.y"
    { director.getBuilder()->onEnd();}
    break;

  case 5:
#line 78 "src/parser/aspcore2.y"
    { yyerror(director,"Generic error"); }
    break;

  case 10:
#line 94 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 11:
#line 98 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 12:
#line 102 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onRule();
        }
    break;

  case 13:
#line 106 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onConstraint();
        }
    break;

  case 14:
#line 110 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeakConstraint();
        }
    break;

  case 15:
#line 114 "src/parser/aspcore2.y"
    {
			queryFound=true;
			director.getBuilder()->onQuery();
		}
    break;

  case 16:
#line 122 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHead();
        }
    break;

  case 17:
#line 126 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceAtom();
            director.getBuilder()->onHead();
        }
    break;

  case 18:
#line 134 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBody();
        }
    break;

  case 19:
#line 141 "src/parser/aspcore2.y"
    {
            // There is only the weight. No level and terms.
            director.getBuilder()->onWeightAtLevels(1,0,0);
        }
    break;

  case 20:
#line 146 "src/parser/aspcore2.y"
    {
            // There are also a level and/or some terms.
            // The finalization has been postponed to "level_and_terms".
        }
    break;

  case 21:
#line 154 "src/parser/aspcore2.y"
    {
            // There is no terms following the level.
            director.getBuilder()->onWeightAtLevels(1,1,0);
        }
    break;

  case 22:
#line 159 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onWeightAtLevels(1,1,(yyvsp[(4) - (4)].integer));
        }
    break;

  case 23:
#line 163 "src/parser/aspcore2.y"
    {
            // The level is omitted.
            // Thus, the first term, recognized as the
            // weight, should be a term of this list.
            director.getBuilder()->onWeightAtLevels(0,0,(yyvsp[(2) - (2)].integer)+1);
        }
    break;

  case 24:
#line 173 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 25:
#line 177 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 26:
#line 181 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 27:
#line 185 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onHeadAtom();
        }
    break;

  case 28:
#line 192 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 29:
#line 196 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBodyLiteral();
        }
    break;

  case 34:
#line 210 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceLowerGuard();
        }
    break;

  case 35:
#line 217 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceUpperGuard();
        }
    break;

  case 38:
#line 229 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 39:
#line 233 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElement();
        }
    break;

  case 40:
#line 240 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementAtom();
        }
    break;

  case 41:
#line 247 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 42:
#line 251 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onChoiceElementLiteral();
        }
    break;

  case 43:
#line 258 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 44:
#line 262 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateNafLiteral();
        }
    break;

  case 45:
#line 269 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 46:
#line 273 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral(true);
        }
    break;

  case 47:
#line 277 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onNafLiteral();
        }
    break;

  case 49:
#line 285 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregate();
        }
    break;

  case 50:
#line 289 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregate(true);
        }
    break;

  case 51:
#line 296 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialAtom();
        }
    break;

  case 52:
#line 301 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(); }
    break;

  case 53:
#line 302 "src/parser/aspcore2.y"
    { director.getBuilder()->onAtom(true); }
    break;

  case 54:
#line 307 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 55:
#line 312 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (4)].string));
            delete[] (yyvsp[(1) - (4)].string);
        }
    break;

  case 56:
#line 317 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onPredicateName((yyvsp[(1) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
        }
    break;

  case 57:
#line 325 "src/parser/aspcore2.y"
    { (yyval.integer) = 1; }
    break;

  case 58:
#line 326 "src/parser/aspcore2.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) + 1; }
    break;

  case 61:
#line 336 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onBuiltinAtom();
        }
    break;

  case 62:
#line 343 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onEqualOperator();
        }
    break;

  case 63:
#line 347 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnequalOperator();
        }
    break;

  case 67:
#line 360 "src/parser/aspcore2.y"
    { (yyval.single_char) = '+'; }
    break;

  case 68:
#line 361 "src/parser/aspcore2.y"
    { (yyval.single_char) = '-'; }
    break;

  case 69:
#line 365 "src/parser/aspcore2.y"
    { (yyval.single_char) = '*'; }
    break;

  case 70:
#line 366 "src/parser/aspcore2.y"
    { (yyval.single_char) = '/'; }
    break;

  case 71:
#line 367 "src/parser/aspcore2.y"
    { (yyval.single_char) = '\\'; }
    break;

  case 72:
#line 372 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onUnknownVariable();
        }
    break;

  case 73:
#line 376 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onFunction((yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
        }
    break;

  case 74:
#line 381 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermRange((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(1) - (3)].string);
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 75:
#line 387 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermDash();
        }
    break;

  case 77:
#line 396 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 78:
#line 401 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 79:
#line 406 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onTermParams();
        }
    break;

  case 80:
#line 412 "src/parser/aspcore2.y"
    {}
    break;

  case 83:
#line 421 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 84:
#line 428 "src/parser/aspcore2.y"
    {}
    break;

  case 85:
#line 430 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onArithmeticOperation((yyvsp[(2) - (3)].single_char));
        }
    break;

  case 89:
#line 442 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 90:
#line 447 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 91:
#line 452 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 92:
#line 457 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateGroundTerm((yyvsp[(2) - (2)].string),true);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 93:
#line 465 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateVariableTerm((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 94:
#line 470 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    break;

  case 95:
#line 477 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunctionalTerm((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].integer));
            delete[] (yyvsp[(1) - (4)].string);
       }
    break;

  case 96:
#line 485 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm(0);
       }
    break;

  case 97:
#line 489 "src/parser/aspcore2.y"
    {
               director.getBuilder()->onListTerm((yyvsp[(2) - (3)].integer));
       }
    break;

  case 98:
#line 496 "src/parser/aspcore2.y"
    {
                (yyval.integer) = (yyvsp[(1) - (1)].integer);
        }
    break;

  case 99:
#line 500 "src/parser/aspcore2.y"
    {
                (yyval.integer) = 2;
        }
    break;

  case 100:
#line 507 "src/parser/aspcore2.y"
    {

                director.getBuilder()->onHeadTailList();

        }
    break;

  case 101:
#line 516 "src/parser/aspcore2.y"
    {
                director.getBuilder()->onTerm((yyvsp[(1) - (1)].string));
                delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 103:
#line 525 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 104:
#line 530 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onExistentialVariable((yyvsp[(3) - (3)].string));
            delete[] (yyvsp[(3) - (3)].string);
        }
    break;

  case 105:
#line 537 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 106:
#line 538 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 107:
#line 539 "src/parser/aspcore2.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 108:
#line 544 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onDirective((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string));
            delete[] (yyvsp[(1) - (2)].string);
            delete[] (yyvsp[(2) - (2)].string);
        }
    break;

  case 109:
#line 553 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAtom();
        }
    break;

  case 110:
#line 560 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 111:
#line 567 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 114:
#line 579 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 118:
#line 595 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateLowerGuard();
        }
    break;

  case 120:
#line 606 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 121:
#line 613 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateUpperGuard();
        }
    break;

  case 129:
#line 633 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOperator();
        }
    break;

  case 130:
#line 637 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onLessOrEqualOperator();
        }
    break;

  case 131:
#line 644 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOperator();
        }
    break;

  case 132:
#line 648 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    break;

  case 137:
#line 666 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateElement();
        }
    break;

  case 138:
#line 673 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 139:
#line 678 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 140:
#line 683 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 141:
#line 688 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;

  case 142:
#line 693 "src/parser/aspcore2.y"
    {
            director.getBuilder()->onAggregateFunction((yyvsp[(1) - (1)].string));
            delete[] (yyvsp[(1) - (1)].string);
        }
    break;


/* Line 1267 of yacc.c.  */
#line 2397 "src/parser/aspcore2_parser.c"
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



