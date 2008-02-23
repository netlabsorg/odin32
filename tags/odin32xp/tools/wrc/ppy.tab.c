
/*  A Bison parser, made from ppy.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse ppparse
#define yylex pplex
#define yyerror pperror
#define yylval pplval
#define yychar ppchar
#define yydebug ppdebug
#define yynerrs ppnerrs
#define	tIF	258
#define	tIFDEF	259
#define	tIFNDEF	260
#define	tELSE	261
#define	tELIF	262
#define	tENDIF	263
#define	tDEFINED	264
#define	tNL	265
#define	tINCLUDE	266
#define	tLINE	267
#define	tGCCLINE	268
#define	tERROR	269
#define	tWARNING	270
#define	tPRAGMA	271
#define	tPPIDENT	272
#define	tUNDEF	273
#define	tMACROEND	274
#define	tCONCAT	275
#define	tELIPSIS	276
#define	tSTRINGIZE	277
#define	tIDENT	278
#define	tLITERAL	279
#define	tMACRO	280
#define	tDEFINE	281
#define	tDQSTRING	282
#define	tSQSTRING	283
#define	tIQSTRING	284
#define	tUINT	285
#define	tSINT	286
#define	tULONG	287
#define	tSLONG	288
#define	tULONGLONG	289
#define	tSLONGLONG	290
#define	tLOGOR	291
#define	tLOGAND	292
#define	tEQ	293
#define	tNE	294
#define	tLTE	295
#define	tGTE	296
#define	tLSHIFT	297
#define	tRSHIFT	298

#line 14 "ppy.y"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "utils.h"
#include "newstruc.h"
#include "wrc.h"
#include "preproc.h"


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
	default: internal_error(__FILE__, __LINE__, "Invalid type indicator (0x%04x)", v1.type);	\
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


#line 112 "ppy.y"
typedef union{
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
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		144
#define	YYFLAG		-32768
#define	YYNTBASE	60

#define YYTRANSLATE(x) ((unsigned)(x) <= 298 ? yytranslate[x] : 73)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    56,     2,     2,     2,     2,    42,     2,    58,
    59,    53,    51,    57,    52,     2,    54,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    37,     2,    45,
     2,    47,    36,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    41,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    40,     2,    55,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    38,    39,    43,    44,    46,    48,    49,    50
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     8,    12,    16,    20,    24,    28,    31,
    34,    38,    42,    49,    54,    59,    62,    66,    70,    74,
    78,    79,    81,    83,    85,    87,    90,    93,    96,    97,
    99,   102,   103,   105,   107,   111,   115,   117,   118,   120,
   122,   125,   127,   129,   131,   133,   136,   138,   140,   142,
   144,   146,   148,   150,   153,   158,   160,   164,   168,   172,
   176,   180,   184,   188,   192,   196,   200,   204,   208,   212,
   216,   220,   224,   228,   231,   234,   237,   240,   244
};

static const short yyrhs[] = {    -1,
    60,    61,     0,    11,    27,    10,     0,    11,    29,    10,
     0,     3,    72,    10,     0,     4,    23,    10,     0,     5,
    23,    10,     0,     7,    72,    10,     0,     6,    10,     0,
     8,    10,     0,    18,    23,    10,     0,    26,    62,    10,
     0,    25,    64,    66,    19,    69,    10,     0,    12,    31,
    27,    10,     0,    13,    27,    65,    10,     0,    13,    10,
     0,    14,    62,    10,     0,    15,    62,    10,     0,    16,
    62,    10,     0,    17,    62,    10,     0,     0,    63,     0,
    24,     0,    27,     0,    28,     0,    63,    24,     0,    63,
    27,     0,    63,    28,     0,     0,    31,     0,    65,    31,
     0,     0,    67,     0,    68,     0,    68,    57,    21,     0,
    68,    57,    23,     0,    23,     0,     0,    70,     0,    71,
     0,    70,    71,     0,    24,     0,    27,     0,    28,     0,
    20,     0,    22,    23,     0,    23,     0,    31,     0,    30,
     0,    33,     0,    32,     0,    35,     0,    34,     0,     9,
    23,     0,     9,    58,    23,    59,     0,    23,     0,    72,
    38,    72,     0,    72,    39,    72,     0,    72,    43,    72,
     0,    72,    44,    72,     0,    72,    45,    72,     0,    72,
    47,    72,     0,    72,    46,    72,     0,    72,    48,    72,
     0,    72,    51,    72,     0,    72,    52,    72,     0,    72,
    41,    72,     0,    72,    42,    72,     0,    72,    40,    72,
     0,    72,    53,    72,     0,    72,    54,    72,     0,    72,
    49,    72,     0,    72,    50,    72,     0,    51,    72,     0,
    52,    72,     0,    55,    72,     0,    56,    72,     0,    58,
    72,    59,     0,    72,    36,    72,    37,    72,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   164,   165,   169,   170,   171,   172,   173,   192,   213,   236,
   250,   251,   252,   255,   256,   257,   258,   259,   260,   261,
   265,   266,   269,   270,   271,   272,   273,   274,   277,   280,
   281,   284,   285,   288,   289,   292,   293,   297,   298,   304,
   305,   308,   309,   310,   311,   312,   318,   327,   328,   329,
   330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
   340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
   350,   351,   352,   353,   354,   355,   356,   357,   358
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","tIF","tIFDEF",
"tIFNDEF","tELSE","tELIF","tENDIF","tDEFINED","tNL","tINCLUDE","tLINE","tGCCLINE",
"tERROR","tWARNING","tPRAGMA","tPPIDENT","tUNDEF","tMACROEND","tCONCAT","tELIPSIS",
"tSTRINGIZE","tIDENT","tLITERAL","tMACRO","tDEFINE","tDQSTRING","tSQSTRING",
"tIQSTRING","tUINT","tSINT","tULONG","tSLONG","tULONGLONG","tSLONGLONG","'?'",
"':'","tLOGOR","tLOGAND","'|'","'^'","'&'","tEQ","tNE","'<'","tLTE","'>'","tGTE",
"tLSHIFT","tRSHIFT","'+'","'-'","'*'","'/'","'~'","'!'","','","'('","')'","pp_file",
"preprocessor","opt_text","text","res_arg","nums","allmargs","emargs","margs",
"opt_mtexts","mtexts","mtext","pp_expr", NULL
};
#endif

static const short yyr1[] = {     0,
    60,    60,    61,    61,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
    62,    62,    63,    63,    63,    63,    63,    63,    64,    65,
    65,    66,    66,    67,    67,    68,    68,    69,    69,    70,
    70,    71,    71,    71,    71,    71,    71,    72,    72,    72,
    72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
    72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
    72,    72,    72,    72,    72,    72,    72,    72,    72
};

static const short yyr2[] = {     0,
     0,     2,     3,     3,     3,     3,     3,     3,     2,     2,
     3,     3,     6,     4,     4,     2,     3,     3,     3,     3,
     0,     1,     1,     1,     1,     2,     2,     2,     0,     1,
     2,     0,     1,     1,     3,     3,     1,     0,     1,     1,
     2,     1,     1,     1,     1,     2,     1,     1,     1,     1,
     1,     1,     1,     2,     4,     1,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     2,     2,     2,     2,     3,     5
};

static const short yydefact[] = {     1,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    21,    21,    21,    21,     0,    29,    21,     2,     0,    56,
    49,    48,    51,    50,    53,    52,     0,     0,     0,     0,
     0,     0,     0,     0,     9,     0,    10,     0,     0,     0,
    16,     0,    23,    24,    25,     0,    22,     0,     0,     0,
     0,    32,     0,    54,     0,    74,    75,    76,    77,     0,
     5,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     6,
     7,     8,     3,     4,     0,    30,     0,    17,    26,    27,
    28,    18,    19,    20,    11,    37,     0,    33,    34,    12,
     0,    78,     0,    57,    58,    69,    67,    68,    59,    60,
    61,    63,    62,    64,    72,    73,    65,    66,    70,    71,
    14,    15,    31,    38,     0,    55,     0,    45,     0,    47,
    42,    43,    44,     0,    39,    40,    35,    36,    79,    46,
    13,    41,     0,     0
};

static const short yydefgoto[] = {     1,
    18,    46,    47,    52,    87,    97,    98,    99,   134,   135,
   136,    32
};

static const short yypact[] = {-32768,
   119,    -4,   -16,    11,    40,    -4,    43,   -25,    24,    -7,
   -12,   -12,   -12,   -12,    53,-32768,   -12,-32768,   -22,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    -4,    -4,    -4,    -4,
    -4,    39,    91,   118,-32768,    64,-32768,   131,   132,    93,
-32768,    98,-32768,-32768,-32768,   137,   254,   180,   269,   270,
   273,   261,   275,-32768,   263,   -36,   -36,-32768,-32768,   110,
-32768,    -4,    -4,    -4,    -4,    -4,    -4,    -4,    -4,    -4,
    -4,    -4,    -4,    -4,    -4,    -4,    -4,    -4,    -4,-32768,
-32768,-32768,-32768,-32768,   277,-32768,     1,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   271,-32768,   231,-32768,
   230,-32768,   134,   169,   184,   198,   211,   223,    -8,    -8,
    45,    45,    45,    45,   114,   114,   -36,   -36,-32768,-32768,
-32768,-32768,-32768,   -14,    12,-32768,    -4,-32768,   268,-32768,
-32768,-32768,-32768,   282,   -14,-32768,-32768,-32768,   153,-32768,
-32768,-32768,   293,-32768
};

static const short yypgoto[] = {-32768,
-32768,   126,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   159,    -6
};


#define	YYLAST		294


static const short yytable[] = {    36,
    54,    38,    41,    39,    19,   128,    33,   129,   130,   131,
   122,    43,   132,   133,    44,    45,    78,    79,    20,    42,
    56,    57,    58,    59,    60,    21,    22,    23,    24,    25,
    26,   123,   137,    34,   138,    55,    70,    71,    72,    73,
    74,    75,    76,    77,    78,    79,    27,    28,    61,    35,
    29,    30,    37,    31,    40,   103,   104,   105,   106,   107,
   108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
   118,   119,   120,    82,    62,    51,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    74,    75,    76,    77,    78,    79,    62,
    80,    63,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,   143,    85,
   139,     2,     3,     4,     5,     6,     7,    81,    86,     8,
     9,    10,    11,    12,    13,    14,    15,    48,    49,    50,
    83,    84,    53,    16,    17,    62,    88,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    76,    77,    78,    79,   102,    62,
   127,    63,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    62,    92,
    63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    79,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,    68,    69,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    79,    89,    93,    94,
    90,    91,    95,    96,   100,   101,   121,   125,   126,   124,
   140,   141,   144,   142
};

static const short yycheck[] = {     6,
    23,    27,    10,    29,     9,    20,    23,    22,    23,    24,
    10,    24,    27,    28,    27,    28,    53,    54,    23,    27,
    27,    28,    29,    30,    31,    30,    31,    32,    33,    34,
    35,    31,    21,    23,    23,    58,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    51,    52,    10,    10,
    55,    56,    10,    58,    31,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    10,    36,    23,    38,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    49,    50,    51,    52,    53,    54,    36,
    10,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,     0,    27,
   127,     3,     4,     5,     6,     7,     8,    10,    31,    11,
    12,    13,    14,    15,    16,    17,    18,    12,    13,    14,
    10,    10,    17,    25,    26,    36,    10,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    51,    52,    53,    54,    59,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    36,    10,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    51,    52,    53,    54,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    24,    10,    10,
    27,    28,    10,    23,    10,    23,    10,    57,    59,    19,
    23,    10,     0,   135
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 3:
#line 169 "ppy.y"
{ do_include(yyvsp[-1].cptr, 1); ;
    break;}
case 4:
#line 170 "ppy.y"
{ do_include(yyvsp[-1].cptr, 0); ;
    break;}
case 5:
#line 171 "ppy.y"
{ next_if_state(boolean(&yyvsp[-1].cval)); ;
    break;}
case 6:
#line 172 "ppy.y"
{ next_if_state(pplookup(yyvsp[-1].cptr) != NULL); free(yyvsp[-1].cptr); ;
    break;}
case 7:
#line 173 "ppy.y"
{
		int t = pplookup(yyvsp[-1].cptr) == NULL;
		if(include_state == 0 && t && !seen_junk)
		{
			include_state	= 1;
			include_ppp	= yyvsp[-1].cptr;
			include_ifdepth	= get_if_depth();
		}
		else if(include_state != 1)
		{
			include_state = -1;
			free(yyvsp[-1].cptr);
		}
		else
			free(yyvsp[-1].cptr);
		next_if_state(t);
		if(debuglevel & DEBUGLEVEL_PPMSG)
			fprintf(stderr, "tIFNDEF: %s:%d: include_state=%d, include_ppp='%s', include_ifdepth=%d\n", input_name, line_number, include_state, include_ppp, include_ifdepth);
		;
    break;}
case 8:
#line 192 "ppy.y"
{
		if_state_t s = pop_if();
		switch(s)
		{
		case if_true:
		case if_elif:
			push_if(if_elif);
			break;
		case if_false:
			push_if(boolean(&yyvsp[-1].cval) ? if_true : if_false);
			break;
		case if_ignore:
			push_if(if_ignore);
			break;
		case if_elsetrue:
		case if_elsefalse:
			pperror("#elif cannot follow #else");
		default:
			internal_error(__FILE__, __LINE__, "Invalid if_state (%d) in #elif directive", s);
		}
		;
    break;}
case 9:
#line 213 "ppy.y"
{
		if_state_t s = pop_if();
		switch(s)
		{
		case if_true:
			push_if(if_elsefalse);
			break;
		case if_elif:
			push_if(if_elif);
			break;
		case if_false:
			push_if(if_elsetrue);
			break;
		case if_ignore:
			push_if(if_ignore);
			break;
		case if_elsetrue:
		case if_elsefalse:
			pperror("#else clause already defined");
		default:
			internal_error(__FILE__, __LINE__, "Invalid if_state (%d) in #else directive", s);
		}
		;
    break;}
case 10:
#line 236 "ppy.y"
{
		pop_if();
		if(include_ifdepth == get_if_depth() && include_state == 1)
		{
			include_state = 2;
			seen_junk = 0;
		}
		else if(include_state != 1)
		{
			include_state = -1;
		}
		if(debuglevel & DEBUGLEVEL_PPMSG)
			fprintf(stderr, "tENDIF: %s:%d: include_state=%d, include_ppp='%s', include_ifdepth=%d\n", input_name, line_number, include_state, include_ppp, include_ifdepth);
		;
    break;}
case 11:
#line 250 "ppy.y"
{ del_define(yyvsp[-1].cptr); free(yyvsp[-1].cptr); ;
    break;}
case 12:
#line 251 "ppy.y"
{ add_define(yyvsp[-2].cptr, yyvsp[-1].cptr); ;
    break;}
case 13:
#line 252 "ppy.y"
{
		add_macro(yyvsp[-5].cptr, macro_args, nmacro_args, yyvsp[-1].mtext);
		;
    break;}
case 14:
#line 255 "ppy.y"
{ fprintf(ppout, "# %d %s\n", yyvsp[-2].sint - 1, yyvsp[-1].cptr); free(yyvsp[-1].cptr); ;
    break;}
case 15:
#line 256 "ppy.y"
{ fprintf(ppout, "# %d %s\n", yyvsp[-1].sint - 1, yyvsp[-2].cptr); free(yyvsp[-2].cptr); ;
    break;}
case 17:
#line 258 "ppy.y"
{ pperror("#error directive: '%s'", yyvsp[-1].cptr); if(yyvsp[-1].cptr) free(yyvsp[-1].cptr); ;
    break;}
case 18:
#line 259 "ppy.y"
{ ppwarning("#warning directive: '%s'", yyvsp[-1].cptr); if(yyvsp[-1].cptr) free(yyvsp[-1].cptr); ;
    break;}
case 19:
#line 260 "ppy.y"
{ if(pedantic) ppwarning("#pragma ignored (arg: '%s')", yyvsp[-1].cptr); if(yyvsp[-1].cptr) free(yyvsp[-1].cptr); ;
    break;}
case 20:
#line 261 "ppy.y"
{ if(pedantic) ppwarning("#ident ignored (arg: '%s')", yyvsp[-1].cptr); if(yyvsp[-1].cptr) free(yyvsp[-1].cptr); ;
    break;}
case 21:
#line 265 "ppy.y"
{ yyval.cptr = NULL; ;
    break;}
case 22:
#line 266 "ppy.y"
{ yyval.cptr = yyvsp[0].cptr; ;
    break;}
case 23:
#line 269 "ppy.y"
{ yyval.cptr = yyvsp[0].cptr; ;
    break;}
case 24:
#line 270 "ppy.y"
{ yyval.cptr = yyvsp[0].cptr; ;
    break;}
case 25:
#line 271 "ppy.y"
{ yyval.cptr = yyvsp[0].cptr; ;
    break;}
case 26:
#line 272 "ppy.y"
{ yyval.cptr = merge_text(yyvsp[-1].cptr, yyvsp[0].cptr); ;
    break;}
case 27:
#line 273 "ppy.y"
{ yyval.cptr = merge_text(yyvsp[-1].cptr, yyvsp[0].cptr); ;
    break;}
case 28:
#line 274 "ppy.y"
{ yyval.cptr = merge_text(yyvsp[-1].cptr, yyvsp[0].cptr); ;
    break;}
case 29:
#line 277 "ppy.y"
{ macro_args = NULL; nmacro_args = 0; ;
    break;}
case 30:
#line 280 "ppy.y"
{ yyval.sint = yyvsp[0].sint; ;
    break;}
case 32:
#line 284 "ppy.y"
{ yyval.sint = 0; macro_args = NULL; nmacro_args = 0; ;
    break;}
case 33:
#line 285 "ppy.y"
{ yyval.sint = nmacro_args; ;
    break;}
case 34:
#line 288 "ppy.y"
{ yyval.marg = yyvsp[0].marg; ;
    break;}
case 35:
#line 289 "ppy.y"
{ yyval.marg = add_new_marg(NULL, arg_list); nmacro_args *= -1; ;
    break;}
case 36:
#line 292 "ppy.y"
{ yyval.marg = add_new_marg(yyvsp[0].cptr, arg_single); ;
    break;}
case 37:
#line 293 "ppy.y"
{ yyval.marg = add_new_marg(yyvsp[0].cptr, arg_single); ;
    break;}
case 38:
#line 297 "ppy.y"
{ yyval.mtext = NULL; ;
    break;}
case 39:
#line 298 "ppy.y"
{
		for(yyval.mtext = yyvsp[0].mtext; yyval.mtext && yyval.mtext->prev; yyval.mtext = yyval.mtext->prev)
			;
		;
    break;}
case 40:
#line 304 "ppy.y"
{ yyval.mtext = yyvsp[0].mtext; ;
    break;}
case 41:
#line 305 "ppy.y"
{ yyval.mtext = combine_mtext(yyvsp[-1].mtext, yyvsp[0].mtext); ;
    break;}
case 42:
#line 308 "ppy.y"
{ yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text); ;
    break;}
case 43:
#line 309 "ppy.y"
{ yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text); ;
    break;}
case 44:
#line 310 "ppy.y"
{ yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text); ;
    break;}
case 45:
#line 311 "ppy.y"
{ yyval.mtext = new_mtext(NULL, 0, exp_concat); ;
    break;}
case 46:
#line 312 "ppy.y"
{
		int mat = marg_index(yyvsp[0].cptr);
		if(mat < 0)
			pperror("Stringification identifier must be an argument parameter");
		yyval.mtext = new_mtext(NULL, mat, exp_stringize);
		;
    break;}
case 47:
#line 318 "ppy.y"
{
		int mat = marg_index(yyvsp[0].cptr);
		if(mat >= 0)
			yyval.mtext = new_mtext(NULL, mat, exp_subst);
		else
			yyval.mtext = new_mtext(yyvsp[0].cptr, 0, exp_text);
		;
    break;}
case 48:
#line 327 "ppy.y"
{ yyval.cval.type = cv_sint;  yyval.cval.val.si = yyvsp[0].sint; ;
    break;}
case 49:
#line 328 "ppy.y"
{ yyval.cval.type = cv_uint;  yyval.cval.val.ui = yyvsp[0].uint; ;
    break;}
case 50:
#line 329 "ppy.y"
{ yyval.cval.type = cv_slong; yyval.cval.val.sl = yyvsp[0].slong; ;
    break;}
case 51:
#line 330 "ppy.y"
{ yyval.cval.type = cv_ulong; yyval.cval.val.ul = yyvsp[0].ulong; ;
    break;}
case 52:
#line 331 "ppy.y"
{ yyval.cval.type = cv_sll;   yyval.cval.val.sl = yyvsp[0].sll; ;
    break;}
case 53:
#line 332 "ppy.y"
{ yyval.cval.type = cv_ull;   yyval.cval.val.ul = yyvsp[0].ull; ;
    break;}
case 54:
#line 333 "ppy.y"
{ yyval.cval.type = cv_sint;  yyval.cval.val.si = pplookup(yyvsp[0].cptr) != NULL; ;
    break;}
case 55:
#line 334 "ppy.y"
{ yyval.cval.type = cv_sint;  yyval.cval.val.si = pplookup(yyvsp[-1].cptr) != NULL; ;
    break;}
case 56:
#line 335 "ppy.y"
{ yyval.cval.type = cv_sint;  yyval.cval.val.si = 0; ;
    break;}
case 57:
#line 336 "ppy.y"
{ yyval.cval.type = cv_sint; yyval.cval.val.si = boolean(&yyvsp[-2].cval) || boolean(&yyvsp[0].cval); ;
    break;}
case 58:
#line 337 "ppy.y"
{ yyval.cval.type = cv_sint; yyval.cval.val.si = boolean(&yyvsp[-2].cval) && boolean(&yyvsp[0].cval); ;
    break;}
case 59:
#line 338 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, ==) ;
    break;}
case 60:
#line 339 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, !=) ;
    break;}
case 61:
#line 340 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  <) ;
    break;}
case 62:
#line 341 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  >) ;
    break;}
case 63:
#line 342 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, <=) ;
    break;}
case 64:
#line 343 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, >=) ;
    break;}
case 65:
#line 344 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  +) ;
    break;}
case 66:
#line 345 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  -) ;
    break;}
case 67:
#line 346 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  ^) ;
    break;}
case 68:
#line 347 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  &) ;
    break;}
case 69:
#line 348 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  |) ;
    break;}
case 70:
#line 349 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  *) ;
    break;}
case 71:
#line 350 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval,  /) ;
    break;}
case 72:
#line 351 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, <<) ;
    break;}
case 73:
#line 352 "ppy.y"
{ promote_equal_size(&yyvsp[-2].cval, &yyvsp[0].cval); BIN_OP(yyval.cval, yyvsp[-2].cval, yyvsp[0].cval, >>) ;
    break;}
case 74:
#line 353 "ppy.y"
{ yyval.cval =  yyvsp[0].cval; ;
    break;}
case 75:
#line 354 "ppy.y"
{ UNARY_OP(yyval.cval, yyvsp[0].cval, -) ;
    break;}
case 76:
#line 355 "ppy.y"
{ UNARY_OP(yyval.cval, yyvsp[0].cval, ~) ;
    break;}
case 77:
#line 356 "ppy.y"
{ yyval.cval.type = cv_sint; yyval.cval.val.si = !boolean(&yyvsp[0].cval); ;
    break;}
case 78:
#line 357 "ppy.y"
{ yyval.cval =  yyvsp[-1].cval; ;
    break;}
case 79:
#line 358 "ppy.y"
{ yyval.cval = boolean(&yyvsp[-4].cval) ? yyvsp[-2].cval : yyvsp[0].cval; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 361 "ppy.y"


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
	marg_t *ma = (marg_t *)xmalloc(sizeof(marg_t));
	ma->arg = str;
	ma->type = type;
	return ma;
}

static marg_t *add_new_marg(char *str, def_arg_t type)
{
	marg_t *ma = new_marg(str, type);
	nmacro_args++;
	macro_args = (marg_t **)xrealloc(macro_args, nmacro_args * sizeof(macro_args[0]));
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
	mtext_t *mt = (mtext_t *)xmalloc(sizeof(mtext_t));
	if(str == NULL) 
		mt->subst.argidx = idx;
	else
		mt->subst.text = str;
	mt->type = type;
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
		tail->subst.text = xrealloc(tail->subst.text, strlen(tail->subst.text)+strlen(mtp->subst.text)+1);
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
	s1 = xrealloc(s1, l1+l2+1);
	memcpy(s1+l1, s2, l2+1);
	free(s2);
	return s1;
}

