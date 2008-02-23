/* A Bison parser, made from ./ppy.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse ppparse
#define yylex   pplex
#define yyerror pperror
#define yylval  pplval
#define yychar  ppchar
#define yydebug ppdebug
#define yynerrs ppnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     tRCINCLUDE = 258,
     tIF = 259,
     tIFDEF = 260,
     tIFNDEF = 261,
     tELSE = 262,
     tELIF = 263,
     tENDIF = 264,
     tDEFINED = 265,
     tNL = 266,
     tINCLUDE = 267,
     tLINE = 268,
     tGCCLINE = 269,
     tERROR = 270,
     tWARNING = 271,
     tPRAGMA = 272,
     tPPIDENT = 273,
     tUNDEF = 274,
     tMACROEND = 275,
     tCONCAT = 276,
     tELIPSIS = 277,
     tSTRINGIZE = 278,
     tIDENT = 279,
     tLITERAL = 280,
     tMACRO = 281,
     tDEFINE = 282,
     tDQSTRING = 283,
     tSQSTRING = 284,
     tIQSTRING = 285,
     tUINT = 286,
     tSINT = 287,
     tULONG = 288,
     tSLONG = 289,
     tULONGLONG = 290,
     tSLONGLONG = 291,
     tRCINCLUDEPATH = 292,
     tLOGOR = 293,
     tLOGAND = 294,
     tNE = 295,
     tEQ = 296,
     tGTE = 297,
     tLTE = 298,
     tRSHIFT = 299,
     tLSHIFT = 300
   };
#endif
#define tRCINCLUDE 258
#define tIF 259
#define tIFDEF 260
#define tIFNDEF 261
#define tELSE 262
#define tELIF 263
#define tENDIF 264
#define tDEFINED 265
#define tNL 266
#define tINCLUDE 267
#define tLINE 268
#define tGCCLINE 269
#define tERROR 270
#define tWARNING 271
#define tPRAGMA 272
#define tPPIDENT 273
#define tUNDEF 274
#define tMACROEND 275
#define tCONCAT 276
#define tELIPSIS 277
#define tSTRINGIZE 278
#define tIDENT 279
#define tLITERAL 280
#define tMACRO 281
#define tDEFINE 282
#define tDQSTRING 283
#define tSQSTRING 284
#define tIQSTRING 285
#define tUINT 286
#define tSINT 287
#define tULONG 288
#define tSLONG 289
#define tULONGLONG 290
#define tSLONGLONG 291
#define tRCINCLUDEPATH 292
#define tLOGOR 293
#define tLOGAND 294
#define tNE 295
#define tEQ 296
#define tGTE 297
#define tLTE 298
#define tRSHIFT 299
#define tLSHIFT 300




/* Copy the first part of user declarations.  */
#line 3 "./ppy.y"

#include "config.h"
#include "wine/port.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "wpp_private.h"


#define UNARY_OP(r, v, OP)					\
	switch(v.type)						\
	{							\
	case cv_sint:	r.val.si  = OP v.val.si; break;		\
	case cv_uint:	r.val.ui  = OP v.val.ui; break;		\
	case cv_slong:	r.val.sl  = OP v.val.sl; break;		\
	case cv_ulong:	r.val.ul  = OP v.val.ul; break;		\
	case cv_sll:	r.val.sll = OP v.val.sll; break;	\
	case cv_ull:	r.val.ull = OP v.val.ull; break;	\
	}

#define cv_signed(v)	((v.type & FLAG_SIGNED) != 0)

#define BIN_OP_INT(r, v1, v2, OP)			\
	r.type = v1.type;				\
	if(cv_signed(v1) && cv_signed(v2))		\
		r.val.si = v1.val.si OP v2.val.si;	\
	else if(cv_signed(v1) && !cv_signed(v2))	\
		r.val.si = v1.val.si OP v2.val.ui;	\
	else if(!cv_signed(v1) && cv_signed(v2))	\
		r.val.ui = v1.val.ui OP v2.val.si;	\
	else						\
		r.val.ui = v1.val.ui OP v2.val.ui;

#define BIN_OP_LONG(r, v1, v2, OP)			\
	r.type = v1.type;				\
	if(cv_signed(v1) && cv_signed(v2))		\
		r.val.sl = v1.val.sl OP v2.val.sl;	\
	else if(cv_signed(v1) && !cv_signed(v2))	\
		r.val.sl = v1.val.sl OP v2.val.ul;	\
	else if(!cv_signed(v1) && cv_signed(v2))	\
		r.val.ul = v1.val.ul OP v2.val.sl;	\
	else						\
		r.val.ul = v1.val.ul OP v2.val.ul;

#define BIN_OP_LONGLONG(r, v1, v2, OP)			\
	r.type = v1.type;				\
	if(cv_signed(v1) && cv_signed(v2))		\
		r.val.sll = v1.val.sll OP v2.val.sll;	\
	else if(cv_signed(v1) && !cv_signed(v2))	\
		r.val.sll = v1.val.sll OP v2.val.ull;	\
	else if(!cv_signed(v1) && cv_signed(v2))	\
		r.val.ull = v1.val.ull OP v2.val.sll;	\
	else						\
		r.val.ull = v1.val.ull OP v2.val.ull;

#define BIN_OP(r, v1, v2, OP)						\
	switch(v1.type & SIZE_MASK)					\
	{								\
	case SIZE_INT:		BIN_OP_INT(r, v1, v2, OP); break;	\
	case SIZE_LONG:		BIN_OP_LONG(r, v1, v2, OP); break;	\
	case SIZE_LONGLONG:	BIN_OP_LONGLONG(r, v1, v2, OP); break;	\
	default: pp_internal_error(__FILE__, __LINE__, "Invalid type indicator (0x%04x)", v1.type);	\
	}


/*
 * Prototypes
 */
static int boolean(cval_t *v);
static void promote_equal_size(cval_t *v1, cval_t *v2);
static void cast_to_sint(cval_t *v);
static void cast_to_uint(cval_t *v);
static void cast_to_slong(cval_t *v);
static void cast_to_ulong(cval_t *v);
static void cast_to_sll(cval_t *v);
static void cast_to_ull(cval_t *v);
static marg_t *new_marg(char *str, def_arg_t type);
static marg_t *add_new_marg(char *str, def_arg_t type);
static int marg_index(char *id);
static mtext_t *new_mtext(char *str, int idx, def_exp_t type);
static mtext_t *combine_mtext(mtext_t *tail, mtext_t *mtp);
static char *merge_text(char *s1, char *s2);

/*
 * Local variables
 */
static marg_t **macro_args;	/* Macro parameters array while parsing */
static int	nmacro_args;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YYSTYPE
#line 99 "./ppy.y"
typedef union {
	int		sint;
	unsigned int	uint;
	long		slong;
	unsigned long	ulong;
	wrc_sll_t	sll;
	wrc_ull_t	ull;
	int		*iptr;
	char		*cptr;
	cval_t		cval;
	marg_t		*marg;
	mtext_t		*mtext;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 282 "y.tab.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/share/bison/yacc.c.  */
#line 303 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
#define YYLAST   303

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  13
/* YYNRULES -- Number of rules. */
#define YYNRULES  84
/* YYNRULES -- Number of states. */
#define YYNSTATES  153

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   300

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,     2,     2,     2,     2,    44,     2,
      60,    61,    55,    53,    59,    54,     2,    56,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    39,     2,
      47,     2,    48,    38,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    43,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    42,     2,    57,     2,     2,     2,
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
      35,    36,    37,    40,    41,    45,    46,    49,    50,    51,
      52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    19,    23,    27,
      31,    34,    37,    41,    45,    52,    57,    62,    68,    75,
      83,    92,    95,    99,   103,   107,   111,   114,   117,   118,
     120,   122,   124,   126,   129,   132,   135,   136,   137,   139,
     141,   145,   149,   151,   152,   154,   156,   159,   161,   163,
     165,   167,   170,   172,   174,   176,   178,   180,   182,   184,
     187,   192,   194,   198,   202,   206,   210,   214,   218,   222,
     226,   230,   234,   238,   242,   246,   250,   254,   258,   262,
     265,   268,   271,   274,   278
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      63,     0,    -1,    -1,    63,    64,    -1,    12,    28,    11,
      -1,    12,    30,    11,    -1,     4,    74,    11,    -1,     5,
      24,    11,    -1,     6,    24,    11,    -1,     8,    74,    11,
      -1,     7,    11,    -1,     9,    11,    -1,    19,    24,    11,
      -1,    27,    65,    11,    -1,    26,    67,    68,    20,    71,
      11,    -1,    13,    32,    28,    11,    -1,    14,    32,    28,
      11,    -1,    14,    32,    28,    32,    11,    -1,    14,    32,
      28,    32,    32,    11,    -1,    14,    32,    28,    32,    32,
      32,    11,    -1,    14,    32,    28,    32,    32,    32,    32,
      11,    -1,    14,    11,    -1,    15,    65,    11,    -1,    16,
      65,    11,    -1,    17,    65,    11,    -1,    18,    65,    11,
      -1,     3,    37,    -1,     3,    28,    -1,    -1,    66,    -1,
      25,    -1,    28,    -1,    29,    -1,    66,    25,    -1,    66,
      28,    -1,    66,    29,    -1,    -1,    -1,    69,    -1,    70,
      -1,    70,    59,    22,    -1,    70,    59,    24,    -1,    24,
      -1,    -1,    72,    -1,    73,    -1,    72,    73,    -1,    25,
      -1,    28,    -1,    29,    -1,    21,    -1,    23,    24,    -1,
      24,    -1,    32,    -1,    31,    -1,    34,    -1,    33,    -1,
      36,    -1,    35,    -1,    10,    24,    -1,    10,    60,    24,
      61,    -1,    24,    -1,    74,    40,    74,    -1,    74,    41,
      74,    -1,    74,    46,    74,    -1,    74,    45,    74,    -1,
      74,    47,    74,    -1,    74,    48,    74,    -1,    74,    50,
      74,    -1,    74,    49,    74,    -1,    74,    53,    74,    -1,
      74,    54,    74,    -1,    74,    43,    74,    -1,    74,    44,
      74,    -1,    74,    42,    74,    -1,    74,    55,    74,    -1,
      74,    56,    74,    -1,    74,    52,    74,    -1,    74,    51,
      74,    -1,    53,    74,    -1,    54,    74,    -1,    57,    74,
      -1,    58,    74,    -1,    60,    74,    61,    -1,    74,    38,
      74,    39,    74,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   150,   150,   151,   155,   156,   157,   158,   159,   179,
     200,   223,   238,   239,   240,   243,   244,   245,   247,   249,
     251,   253,   254,   255,   256,   257,   258,   265,   271,   272,
     275,   276,   277,   278,   279,   280,   283,   286,   287,   290,
     291,   294,   295,   299,   300,   306,   307,   310,   311,   312,
     313,   314,   320,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "tRCINCLUDE", "tIF", "tIFDEF", "tIFNDEF", 
  "tELSE", "tELIF", "tENDIF", "tDEFINED", "tNL", "tINCLUDE", "tLINE", 
  "tGCCLINE", "tERROR", "tWARNING", "tPRAGMA", "tPPIDENT", "tUNDEF", 
  "tMACROEND", "tCONCAT", "tELIPSIS", "tSTRINGIZE", "tIDENT", "tLITERAL", 
  "tMACRO", "tDEFINE", "tDQSTRING", "tSQSTRING", "tIQSTRING", "tUINT", 
  "tSINT", "tULONG", "tSLONG", "tULONGLONG", "tSLONGLONG", 
  "tRCINCLUDEPATH", "'?'", "':'", "tLOGOR", "tLOGAND", "'|'", "'^'", 
  "'&'", "tNE", "tEQ", "'<'", "'>'", "tGTE", "tLTE", "tRSHIFT", "tLSHIFT", 
  "'+'", "'-'", "'*'", "'/'", "'~'", "'!'", "','", "'('", "')'", 
  "$accept", "pp_file", "preprocessor", "opt_text", "text", "res_arg", 
  "allmargs", "emargs", "margs", "opt_mtexts", "mtexts", "mtext", 
  "pp_expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,    63,    58,
     293,   294,   124,    94,    38,   295,   296,    60,    62,   297,
     298,   299,   300,    43,    45,    42,    47,   126,    33,    44,
      40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    62,    63,    63,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    65,    65,
      66,    66,    66,    66,    66,    66,    67,    68,    68,    69,
      69,    70,    70,    71,    71,    72,    72,    73,    73,    73,
      73,    73,    73,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     3,     3,     3,     3,
       2,     2,     3,     3,     6,     4,     4,     5,     6,     7,
       8,     2,     3,     3,     3,     3,     2,     2,     0,     1,
       1,     1,     1,     2,     2,     2,     0,     0,     1,     1,
       3,     3,     1,     0,     1,     1,     2,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       4,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     5
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    28,    28,    28,     0,    36,    28,
       3,    27,    26,     0,    61,    54,    53,    56,    55,    58,
      57,     0,     0,     0,     0,     0,     0,     0,     0,    10,
       0,    11,     0,     0,     0,    21,     0,    30,    31,    32,
       0,    29,     0,     0,     0,     0,    37,     0,    59,     0,
      79,    80,    81,    82,     0,     6,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     7,     8,     9,     4,     5,     0,
       0,    22,    33,    34,    35,    23,    24,    25,    12,    42,
       0,    38,    39,    13,     0,    83,     0,    62,    63,    74,
      72,    73,    65,    64,    66,    67,    69,    68,    78,    77,
      70,    71,    75,    76,    15,    16,     0,    43,     0,    60,
       0,    17,     0,    50,     0,    52,    47,    48,    49,     0,
      44,    45,    40,    41,    84,    18,     0,    51,    14,    46,
      19,     0,    20
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,    20,    50,    51,    56,   100,   101,   102,   139,
     140,   141,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -27
static const short yypact[] =
{
     -27,   142,   -27,   -26,    -3,   -12,    -2,    30,    -3,    41,
      91,    21,     2,   -19,   -19,   -19,   -19,    32,   -27,   -19,
     -27,   -27,   -27,   -23,   -27,   -27,   -27,   -27,   -27,   -27,
     -27,    -3,    -3,    -3,    -3,    -3,    38,    66,   109,   -27,
      85,   -27,   113,   133,   115,   -27,   124,   -27,   -27,   -27,
     179,    -9,   278,   280,   281,   282,   129,   283,   -27,   271,
     -10,   -10,   -27,   -27,    57,   -27,    -3,    -3,    -3,    -3,
      -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,
      -3,    -3,    -3,    -3,   -27,   -27,   -27,   -27,   -27,   285,
       3,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,
     277,   -27,   239,   -27,   238,   -27,   132,   167,   182,   196,
     209,   221,   231,   231,   111,   111,   111,   111,    61,    61,
     -10,   -10,   -27,   -27,   -27,   -27,     4,    19,   266,   -27,
      -3,   -27,     6,   -27,   276,   -27,   -27,   -27,   -27,   290,
      19,   -27,   -27,   -27,   151,   -27,     7,   -27,   -27,   -27,
     -27,   291,   -27
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
     -27,   -27,   -27,   -11,   -27,   -27,   -27,   -27,   -27,   -27,
     -27,   163,    -8
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      40,    58,    21,    52,    53,    54,    47,    23,    57,    48,
      49,    22,    37,    45,   125,   131,    92,   145,   150,    93,
      94,    24,    38,    60,    61,    62,    63,    64,    25,    26,
      27,    28,    29,    30,    46,   126,   132,    59,   146,   151,
     133,    39,   134,   135,   136,    82,    83,   137,   138,    65,
      31,    32,    41,    44,    33,    34,    55,    35,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,    66,    84,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    66,    86,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    80,    81,    82,    83,   105,    42,
      85,    43,   144,    66,    87,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,     2,    89,    88,     3,     4,     5,     6,     7,
       8,     9,    90,    99,    10,    11,    12,    13,    14,    15,
      16,    17,    78,    79,    80,    81,    82,    83,    18,    19,
      66,   130,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    66,
      91,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,   142,    95,
     143,    96,    97,    98,   103,   104,   124,   127,   128,   129,
     147,   148,   152,   149
};

static const unsigned char yycheck[] =
{
       8,    24,    28,    14,    15,    16,    25,    10,    19,    28,
      29,    37,    24,    11,    11,    11,    25,    11,    11,    28,
      29,    24,    24,    31,    32,    33,    34,    35,    31,    32,
      33,    34,    35,    36,    32,    32,    32,    60,    32,    32,
      21,    11,    23,    24,    25,    55,    56,    28,    29,    11,
      53,    54,    11,    32,    57,    58,    24,    60,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    38,    11,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    38,    11,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    53,    54,    55,    56,    61,    28,
      11,    30,   130,    38,    11,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    28,    11,     3,     4,     5,     6,     7,
       8,     9,    28,    24,    12,    13,    14,    15,    16,    17,
      18,    19,    51,    52,    53,    54,    55,    56,    26,    27,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    38,
      11,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    22,    11,
      24,    11,    11,    11,    11,    24,    11,    20,    59,    61,
      24,    11,    11,   140
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    63,     0,     3,     4,     5,     6,     7,     8,     9,
      12,    13,    14,    15,    16,    17,    18,    19,    26,    27,
      64,    28,    37,    10,    24,    31,    32,    33,    34,    35,
      36,    53,    54,    57,    58,    60,    74,    24,    24,    11,
      74,    11,    28,    30,    32,    11,    32,    25,    28,    29,
      65,    66,    65,    65,    65,    24,    67,    65,    24,    60,
      74,    74,    74,    74,    74,    11,    38,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    11,    11,    11,    11,    11,    28,
      28,    11,    25,    28,    29,    11,    11,    11,    11,    24,
      68,    69,    70,    11,    24,    61,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    11,    11,    32,    20,    59,    61,
      39,    11,    32,    21,    23,    24,    25,    28,    29,    71,
      72,    73,    22,    24,    74,    11,    32,    24,    11,    73,
      11,    32,    11
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

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
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 4:
#line 155 "./ppy.y"
    { pp_do_include(yyvsp[-1].cptr, 1); }
    break;

  case 5:
#line 156 "./ppy.y"
    { pp_do_include(yyvsp[-1].cptr, 0); }
    break;

  case 6:
#line 157 "./ppy.y"
    { pp_next_if_state(boolean(&yyvsp[-1].cval)); }
    break;

  case 7:
#line 158 "./ppy.y"
    { pp_next_if_state(pplookup(yyvsp[-1].cptr) != NULL); free(yyvsp[-1].cptr); }
    break;

  case 8:
#line 159 "./ppy.y"
    {
		int t = pplookup(yyvsp[-1].cptr) == NULL;
		if(pp_incl_state.state == 0 && t && !pp_incl_state.seen_junk)
		{
			pp_incl_state.state	= 1;
			pp_incl_state.ppp	= yyvsp[-1].cptr;
			pp_incl_state.ifdepth	= pp_get_if_depth();
		}
		else if(pp_incl_state.state != 1)
		{
			pp_incl_state.state = -1;
			free(yyvsp[-1].cptr);
		}
		else
			free(yyvsp[-1].cptr);
		pp_next_if_state(t);
		if(pp_status.debug)
			fprintf(stderr, "tIFNDEF: %s:%d: include_state=%d, include_ppp='%s', include_ifdepth=%d\n",
                                pp_status.input, pp_status.line_number, pp_incl_state.state, pp_incl_state.ppp, pp_incl_state.ifdepth);
		}
    break;

  case 9:
#line 179 "./ppy.y"
    {
		pp_if_state_t s = pp_pop_if();
		switch(s)
		{
		case if_true:
		case if_elif:
			pp_push_if(if_elif);
			break;
		case if_false:
			pp_push_if(boolean(&yyvsp[-1].cval) ? if_true : if_false);
			break;
		case if_ignore:
			pp_push_if(if_ignore);
			break;
		case if_elsetrue:
		case if_elsefalse:
			pperror("#elif cannot follow #else");
		default:
			pp_internal_error(__FILE__, __LINE__, "Invalid pp_if_state (%d) in #elif directive", s);
		}
		}
    break;

  case 10:
#line 200 "./ppy.y"
    {
		pp_if_state_t s = pp_pop_if();
		switch(s)
		{
		case if_true:
			pp_push_if(if_elsefalse);
			break;
		case if_elif:
			pp_push_if(if_elif);
			break;
		case if_false:
			pp_push_if(if_elsetrue);
			break;
		case if_ignore:
			pp_push_if(if_ignore);
			break;
		case if_elsetrue:
		case if_elsefalse:
			pperror("#else clause already defined");
		default:
			pp_internal_error(__FILE__, __LINE__, "Invalid pp_if_state (%d) in #else directive", s);
		}
		}
    break;

  case 11:
#line 223 "./ppy.y"
    {
		pp_pop_if();
		if(pp_incl_state.ifdepth == pp_get_if_depth() && pp_incl_state.state == 1)
		{
			pp_incl_state.state = 2;
			pp_incl_state.seen_junk = 0;
		}
		else if(pp_incl_state.state != 1)
		{
			pp_incl_state.state = -1;
		}
		if(pp_status.debug)
			fprintf(stderr, "tENDIF: %s:%d: include_state=%d, include_ppp='%s', include_ifdepth=%d\n",
                                pp_status.input, pp_status.line_number, pp_incl_state.state, pp_incl_state.ppp, pp_incl_state.ifdepth);
		}
    break;

  case 12:
#line 238 "./ppy.y"
    { pp_del_define(yyvsp[-1].cptr); free(yyvsp[-1].cptr); }
    break;

  case 13:
#line 239 "./ppy.y"
    { pp_add_define(yyvsp[-2].cptr, yyvsp[-1].cptr); }
    break;

  case 14:
#line 240 "./ppy.y"
    {
		pp_add_macro(yyvsp[-5].cptr, macro_args, nmacro_args, yyvsp[-1].mtext);
		}
    break;

  case 15:
#line 243 "./ppy.y"
    { fprintf(ppout, "# %d %s\n", yyvsp[-2].sint , yyvsp[-1].cptr); free(yyvsp[-1].cptr); }
    break;

  case 16:
#line 244 "./ppy.y"
    { fprintf(ppout, "# %d %s\n", yyvsp[-2].sint , yyvsp[-1].cptr); free(yyvsp[-1].cptr); }
    break;

  case 17:
#line 246 "./ppy.y"
    { fprintf(ppout, "# %d %s %d\n", yyvsp[-3].sint, yyvsp[-2].cptr, yyvsp[-1].sint); free(yyvsp[-2].cptr); }
    break;

  case 18:
#line 248 "./ppy.y"
    { fprintf(ppout, "# %d %s %d %d\n", yyvsp[-4].sint ,yyvsp[-3].cptr, yyvsp[-2].sint, yyvsp[-1].sint); free(yyvsp[-3].cptr); }
    break;

  case 19:
#line 250 "./ppy.y"
    { fprintf(ppout, "# %d %s %d %d %d\n", yyvsp[-5].sint ,yyvsp[-4].cptr ,yyvsp[-3].sint ,yyvsp[-2].sint, yyvsp[-1].sint); free(yyvsp[-4].cptr); }
    break;

  case 20:
#line 252 "./ppy.y"
    { fprintf(ppout, "# %d %s %d %d %d %d\n", yyvsp[-6].sint ,yyvsp[-5].cptr ,yyvsp[-4].sint ,yyvsp[-3].sint, yyvsp[-2].sint, yyvsp[-1].sint); free(yyvsp[-5].cptr); }
    break;

  case 22:
#line 254 "./ppy.y"
    { pperror("#error directive: '%s'", yyvsp[-1].cptr); if(yyvsp[-1].cptr) free(yyvsp[-1].cptr); }
    break;

  case 23:
#line 255 "./ppy.y"
    { ppwarning("#warning directive: '%s'", yyvsp[-1].cptr); if(yyvsp[-1].cptr) free(yyvsp[-1].cptr); }
    break;

  case 24:
#line 256 "./ppy.y"
    { fprintf(ppout, "#pragma %s\n", yyvsp[-1].cptr ? yyvsp[-1].cptr : ""); if (yyvsp[-1].cptr) free(yyvsp[-1].cptr); }
    break;

  case 25:
#line 257 "./ppy.y"
    { if(pp_status.pedantic) ppwarning("#ident ignored (arg: '%s')", yyvsp[-1].cptr); if(yyvsp[-1].cptr) free(yyvsp[-1].cptr); }
    break;

  case 26:
#line 258 "./ppy.y"
    {
                int nl=strlen(yyvsp[0].cptr) +3;
                char *fn=pp_xmalloc(nl);
                sprintf(fn,"\"%s\"",yyvsp[0].cptr);
		free(yyvsp[0].cptr);
		pp_do_include(fn,1);
	}
    break;

  case 27:
#line 265 "./ppy.y"
    {
		pp_do_include(yyvsp[0].cptr,1);
	}
    break;

  case 28:
#line 271 "./ppy.y"
    { yyval.cptr = NULL; }
    break;

  case 29:
#line 272 "./ppy.y"
    { yyval.cptr = yyvsp[0].cptr; }
    break;

  case 30:
#line 275 "./ppy.y"
    { yyval.cptr = yyvsp[0].cptr; }
    break;

  case 31:
#line 276 "./ppy.y"
    { yyval.cptr = yyvsp[0].cptr; }
    break;

  case 32:
#line 277 "./ppy.y"
    { yyval.cptr = yyvsp[0].cptr; }
    break;

  case 33:
#line 278 "./ppy.y"
    { yyval.cptr = merge_text(yyvsp[-1].cptr, yyvsp[0].cptr); }
    break;

  case 34:
#line 279 "./ppy.y"
    { yyval.cptr = merge_text(yyvsp[-1].cptr, yyvsp[0].cptr); }
    break;

  case 35:
#line 280 "./ppy.y"
    { yyval.cptr = merge_text(yyvsp[-1].cptr, yyvsp[0].cptr); }
    break;

  case 36:
#line 283 "./ppy.y"
    { macro_args = NULL; nmacro_args = 0; }
    break;

  case 37:
#line 286 "./ppy.y"
    { yyval.sint = 0; macro_args = NULL; nmacro_args = 0; }
    break;

  case 38:
#line 287 "./ppy.y"
    { yyval.sint = nmacro_args; }
    break;

  case 39:
#line 290 "./ppy.y"
    { yyval.marg = yyvsp[0].marg; }
    break;

  case 40:
#line 291 "./ppy.y"
    { yyval.marg = add_new_marg(NULL, arg_list); nmacro_args *= -1; }
    break;

  case 41:
#line 294 "./ppy.y"
    { yyval.marg = add_new_marg(yyvsp[0].cptr, arg_single); }
    break;

  case 42:
#line 295 "./ppy.y"
    { yyval.marg = add_new_marg(yyvsp[0].cptr, arg_single); }
    break;

  case 43:
#line 299 "./ppy.y"
    { yyval.mtext = NULL; }
    break;

  case 44:
#line 300 "./ppy.y"
    {
		for(yyval.mtext = yyvsp[0].mtext; yyval.mtext && yyval.mtext->prev; yyval.mtext = yyval.mtext->prev)
			;
		}
    break;

  case 45:
#line 306 "./ppy.y"
    { yyval.mtext = yyvsp[0].mtext; }
    break;

  case 46:
#line 307 "./ppy.y"
    { yyval.mtext = combine_mtext(yyvsp[-1].mtext, yyvsp[0].mtext); }
    break;

  case 47:
#line 310 "./ppy.y"
    { yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text); }
    break;

  case 48:
#line 311 "./ppy.y"
    { yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text); }
    break;

  case 49:
#line 312 "./ppy.y"
    { yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text); }
    break;

  case 50:
#line 313 "./ppy.y"
    { yyval.mtext = new_mtext(NULL, 0, exp_concat); }
    break;

  case 51:
#line 314 "./ppy.y"
    {
		int mat = marg_index(yyvsp[0].cptr);
		if(mat < 0)
			pperror("Stringification identifier must be an argument parameter");
		yyval.mtext = new_mtext(NULL, mat, exp_stringize);
		}
    break;

  case 52:
#line 320 "./ppy.y"
    {
		int mat = marg_index(yyvsp[0].cptr);
		if(mat >= 0)
			yyval.mtext = new_mtext(NULL, mat, exp_subst);
		else
			yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text);
		}
    break;

  case 53:
#line 329 "./ppy.y"
    { yyval.cval.type = cv_sint;  yyval.cval.val.si = yyvsp[0].sint; }
    break;

  case 54:
#line 330 "./ppy.y"
    { yyval.cval.type = cv_uint;  yyval.cval.val.ui = yyvsp[0].uint; }
    break;

  case 55:
#line 331 "./ppy.y"
    { yyval.cval.type = cv_slong; yyval.cval.val.sl = yyvsp[0].slong; }
    break;

  case 56:
#line 332 "./ppy.y"
    { yyval.cval.type = cv_ulong; yyval.cval.val.ul = yyvsp[0].ulong; }
    break;

  case 57:
#line 333 "./ppy.y"
    { yyval.cval.type = cv_sll;   yyval.cval.val.sl = yyvsp[0].sll; }
    break;

  case 58:
#line 334 "./ppy.y"
    { yyval.cval.type = cv_ull;   yyval.cval.val.ul = yyvsp[0].ull; }
    break;

  case 59:
#line 335 "./ppy.y"
    { yyval.cval.type = cv_sint;  yyval.cval.val.si = pplookup(yyvsp[0].cptr) != NULL; }
    break;

  case 60:
#line 336 "./ppy.y"
    { yyval.cval.type = cv_sint;  yyval.cval.val.si = pplookup(yyvsp[-1].cptr) != NULL; }
    break;

  case 61:
#line 337 "./ppy.y"
    { yyval.cval.type = cv_sint;  yyval.cval.val.si = 0; }
    break;

  case 62:
#line 338 "./ppy.y"
    { yyval.cval.type = cv_sint; yyval.cval.val.si = boolean(&yyvsp[-2].cval) || boolean(&yyvsp[0].cval); }
    break;

  case 63:
#line 339 "./ppy.y"
    { yyval.cval.type = cv_sint; yyval.cval.val.si = boolean(&yyvsp[-2].cval) && boolean(&yyvsp[0].cval); }
    break;

  case 64:
#line 340 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, ==) }
    break;

  case 65:
#line 341 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, !=) }
    break;

  case 66:
#line 342 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  <) }
    break;

  case 67:
#line 343 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  >) }
    break;

  case 68:
#line 344 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, <=) }
    break;

  case 69:
#line 345 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, >=) }
    break;

  case 70:
#line 346 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  +) }
    break;

  case 71:
#line 347 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  -) }
    break;

  case 72:
#line 348 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  ^) }
    break;

  case 73:
#line 349 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  &) }
    break;

  case 74:
#line 350 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  |) }
    break;

  case 75:
#line 351 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  *) }
    break;

  case 76:
#line 352 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  /) }
    break;

  case 77:
#line 353 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, <<) }
    break;

  case 78:
#line 354 "./ppy.y"
    { promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, >>) }
    break;

  case 79:
#line 355 "./ppy.y"
    { yyval.cval =  yyvsp[0].cval; }
    break;

  case 80:
#line 356 "./ppy.y"
    { UNARY_OP(yyval.cval, yyvsp[0].cval, -) }
    break;

  case 81:
#line 357 "./ppy.y"
    { UNARY_OP(yyval.cval, yyvsp[0].cval, ~) }
    break;

  case 82:
#line 358 "./ppy.y"
    { yyval.cval.type = cv_sint; yyval.cval.val.si = !boolean(&yyvsp[0].cval); }
    break;

  case 83:
#line 359 "./ppy.y"
    { yyval.cval =  yyvsp[-1].cval; }
    break;

  case 84:
#line 360 "./ppy.y"
    { yyval.cval = boolean(&yyvsp[-4].cval) ? yyvsp[-2].cval : yyvsp[0].cval; }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1790 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

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

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 363 "./ppy.y"


/*
 **************************************************************************
 * Support functions
 **************************************************************************
 */

static void cast_to_sint(cval_t *v)
{
	switch(v->type)
	{
	case cv_sint:	break;
	case cv_uint:	break;
	case cv_slong:	v->val.si = v->val.sl;	break;
	case cv_ulong:	v->val.si = v->val.ul;	break;
	case cv_sll:	v->val.si = v->val.sll;	break;
	case cv_ull:	v->val.si = v->val.ull;	break;
	}
	v->type = cv_sint;
}

static void cast_to_uint(cval_t *v)
{
	switch(v->type)
	{
	case cv_sint:	break;
	case cv_uint:	break;
	case cv_slong:	v->val.ui = v->val.sl;	break;
	case cv_ulong:	v->val.ui = v->val.ul;	break;
	case cv_sll:	v->val.ui = v->val.sll;	break;
	case cv_ull:	v->val.ui = v->val.ull;	break;
	}
	v->type = cv_uint;
}

static void cast_to_slong(cval_t *v)
{
	switch(v->type)
	{
	case cv_sint:	v->val.sl = v->val.si;	break;
	case cv_uint:	v->val.sl = v->val.ui;	break;
	case cv_slong:	break;
	case cv_ulong:	break;
	case cv_sll:	v->val.sl = v->val.sll;	break;
	case cv_ull:	v->val.sl = v->val.ull;	break;
	}
	v->type = cv_slong;
}

static void cast_to_ulong(cval_t *v)
{
	switch(v->type)
	{
	case cv_sint:	v->val.ul = v->val.si;	break;
	case cv_uint:	v->val.ul = v->val.ui;	break;
	case cv_slong:	break;
	case cv_ulong:	break;
	case cv_sll:	v->val.ul = v->val.sll;	break;
	case cv_ull:	v->val.ul = v->val.ull;	break;
	}
	v->type = cv_ulong;
}

static void cast_to_sll(cval_t *v)
{
	switch(v->type)
	{
	case cv_sint:	v->val.sll = v->val.si;	break;
	case cv_uint:	v->val.sll = v->val.ui;	break;
	case cv_slong:	v->val.sll = v->val.sl;	break;
	case cv_ulong:	v->val.sll = v->val.ul;	break;
	case cv_sll:	break;
	case cv_ull:	break;
	}
	v->type = cv_sll;
}

static void cast_to_ull(cval_t *v)
{
	switch(v->type)
	{
	case cv_sint:	v->val.ull = v->val.si;	break;
	case cv_uint:	v->val.ull = v->val.ui;	break;
	case cv_slong:	v->val.ull = v->val.sl;	break;
	case cv_ulong:	v->val.ull = v->val.ul;	break;
	case cv_sll:	break;
	case cv_ull:	break;
	}
	v->type = cv_ull;
}


static void promote_equal_size(cval_t *v1, cval_t *v2)
{
#define cv_sizeof(v)	((int)(v->type & SIZE_MASK))
	int s1 = cv_sizeof(v1);
	int s2 = cv_sizeof(v2);
#undef cv_sizeof

	if(s1 == s2)
		return;
	else if(s1 > s2)
	{
		switch(v1->type)
		{
		case cv_sint:	cast_to_sint(v2); break;
		case cv_uint:	cast_to_uint(v2); break;
		case cv_slong:	cast_to_slong(v2); break;
		case cv_ulong:	cast_to_ulong(v2); break;
		case cv_sll:	cast_to_sll(v2); break;
		case cv_ull:	cast_to_ull(v2); break;
		}
	}
	else
	{
		switch(v2->type)
		{
		case cv_sint:	cast_to_sint(v1); break;
		case cv_uint:	cast_to_uint(v1); break;
		case cv_slong:	cast_to_slong(v1); break;
		case cv_ulong:	cast_to_ulong(v1); break;
		case cv_sll:	cast_to_sll(v1); break;
		case cv_ull:	cast_to_ull(v1); break;
		}
	}
}


static int boolean(cval_t *v)
{
	switch(v->type)
	{
	case cv_sint:	return v->val.si != (int)0;
	case cv_uint:	return v->val.ui != (unsigned int)0;
	case cv_slong:	return v->val.sl != (long)0;
	case cv_ulong:	return v->val.ul != (unsigned long)0;
	case cv_sll:	return v->val.sll != (wrc_sll_t)0;
	case cv_ull:	return v->val.ull != (wrc_ull_t)0;
	}
	return 0;
}

static marg_t *new_marg(char *str, def_arg_t type)
{
	marg_t *ma = pp_xmalloc(sizeof(marg_t));
	ma->arg = str;
	ma->type = type;
	ma->nnl = 0;
	return ma;
}

static marg_t *add_new_marg(char *str, def_arg_t type)
{
	marg_t *ma = new_marg(str, type);
	nmacro_args++;
	macro_args = pp_xrealloc(macro_args, nmacro_args * sizeof(macro_args[0]));
	macro_args[nmacro_args-1] = ma;
	return ma;
}

static int marg_index(char *id)
{
	int t;
	for(t = 0; t < nmacro_args; t++)
	{
		if(!strcmp(id, macro_args[t]->arg))
			break;
	}
	return t < nmacro_args ? t : -1;
}

static mtext_t *new_mtext(char *str, int idx, def_exp_t type)
{
	mtext_t *mt = pp_xmalloc(sizeof(mtext_t));
	if(str == NULL)
		mt->subst.argidx = idx;
	else
		mt->subst.text = str;
	mt->type = type;
	mt->next = mt->prev = NULL;
	return mt;
}

static mtext_t *combine_mtext(mtext_t *tail, mtext_t *mtp)
{
	if(!tail)
		return mtp;

	if(!mtp)
		return tail;

	if(tail->type == exp_text && mtp->type == exp_text)
	{
		tail->subst.text = pp_xrealloc(tail->subst.text, strlen(tail->subst.text)+strlen(mtp->subst.text)+1);
		strcat(tail->subst.text, mtp->subst.text);
		free(mtp->subst.text);
		free(mtp);
		return tail;
	}

	if(tail->type == exp_concat && mtp->type == exp_concat)
	{
		free(mtp);
		return tail;
	}

	if(tail->type == exp_concat && mtp->type == exp_text)
	{
		int len = strlen(mtp->subst.text);
		while(len)
		{
/* FIXME: should delete space from head of string */
			if(isspace(mtp->subst.text[len-1] & 0xff))
				mtp->subst.text[--len] = '\0';
			else
				break;
		}

		if(!len)
		{
			free(mtp->subst.text);
			free(mtp);
			return tail;
		}
	}

	if(tail->type == exp_text && mtp->type == exp_concat)
	{
		int len = strlen(tail->subst.text);
		while(len)
		{
			if(isspace(tail->subst.text[len-1] & 0xff))
				tail->subst.text[--len] = '\0';
			else
				break;
		}

		if(!len)
		{
			mtp->prev = tail->prev;
			mtp->next = tail->next;
			if(tail->prev)
				tail->prev->next = mtp;
			free(tail->subst.text);
			free(tail);
			return mtp;
		}
	}

	tail->next = mtp;
	mtp->prev = tail;

	return mtp;
}

static char *merge_text(char *s1, char *s2)
{
	int l1 = strlen(s1);
	int l2 = strlen(s2);
	s1 = pp_xrealloc(s1, l1+l2+1);
	memcpy(s1+l1, s2, l2+1);
	free(s2);
	return s1;
}

