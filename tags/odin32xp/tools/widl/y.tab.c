/* A Bison parser, made from ./parser.y, by GNU bison 1.75.  */

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



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     aIDENTIFIER = 258,
     aKNOWNTYPE = 259,
     aNUM = 260,
     aHEXNUM = 261,
     aSTRING = 262,
     aUUID = 263,
     aEOF = 264,
     SHL = 265,
     SHR = 266,
     tAGGREGATABLE = 267,
     tALLOCATE = 268,
     tAPPOBJECT = 269,
     tARRAYS = 270,
     tASYNC = 271,
     tASYNCUUID = 272,
     tAUTOHANDLE = 273,
     tBINDABLE = 274,
     tBOOLEAN = 275,
     tBROADCAST = 276,
     tBYTE = 277,
     tBYTECOUNT = 278,
     tCALLAS = 279,
     tCALLBACK = 280,
     tCASE = 281,
     tCDECL = 282,
     tCHAR = 283,
     tCOCLASS = 284,
     tCODE = 285,
     tCOMMSTATUS = 286,
     tCONST = 287,
     tCONTEXTHANDLE = 288,
     tCONTEXTHANDLENOSERIALIZE = 289,
     tCONTEXTHANDLESERIALIZE = 290,
     tCONTROL = 291,
     tCPPQUOTE = 292,
     tDEFAULT = 293,
     tDEFAULTVALUE = 294,
     tDISPINTERFACE = 295,
     tDLLNAME = 296,
     tDOUBLE = 297,
     tDUAL = 298,
     tENDPOINT = 299,
     tENTRY = 300,
     tENUM = 301,
     tERRORSTATUST = 302,
     tEXPLICITHANDLE = 303,
     tEXTERN = 304,
     tFLOAT = 305,
     tHANDLE = 306,
     tHANDLET = 307,
     tHELPCONTEXT = 308,
     tHELPFILE = 309,
     tHELPSTRING = 310,
     tHELPSTRINGCONTEXT = 311,
     tHELPSTRINGDLL = 312,
     tHIDDEN = 313,
     tHYPER = 314,
     tID = 315,
     tIDEMPOTENT = 316,
     tIIDIS = 317,
     tIMPLICITHANDLE = 318,
     tIMPORT = 319,
     tIMPORTLIB = 320,
     tIN = 321,
     tINCLUDE = 322,
     tINLINE = 323,
     tINPUTSYNC = 324,
     tINT = 325,
     tINT64 = 326,
     tINTERFACE = 327,
     tLENGTHIS = 328,
     tLIBRARY = 329,
     tLOCAL = 330,
     tLONG = 331,
     tMETHODS = 332,
     tMODULE = 333,
     tNONCREATABLE = 334,
     tOBJECT = 335,
     tODL = 336,
     tOLEAUTOMATION = 337,
     tOPTIONAL = 338,
     tOUT = 339,
     tPOINTERDEFAULT = 340,
     tPROPERTIES = 341,
     tPROPGET = 342,
     tPROPPUT = 343,
     tPROPPUTREF = 344,
     tPUBLIC = 345,
     tREADONLY = 346,
     tREF = 347,
     tRESTRICTED = 348,
     tRETVAL = 349,
     tSHORT = 350,
     tSIGNED = 351,
     tSINGLE = 352,
     tSIZEIS = 353,
     tSIZEOF = 354,
     tSOURCE = 355,
     tSTDCALL = 356,
     tSTRING = 357,
     tSTRUCT = 358,
     tSWITCH = 359,
     tSWITCHIS = 360,
     tSWITCHTYPE = 361,
     tTRANSMITAS = 362,
     tTYPEDEF = 363,
     tUNION = 364,
     tUNIQUE = 365,
     tUNSIGNED = 366,
     tUUID = 367,
     tV1ENUM = 368,
     tVARARG = 369,
     tVERSION = 370,
     tVOID = 371,
     tWCHAR = 372,
     tWIREMARSHAL = 373,
     tPOINTERTYPE = 374,
     COND = 375,
     CAST = 376,
     PPTR = 377,
     NEG = 378
   };
#endif
#define aIDENTIFIER 258
#define aKNOWNTYPE 259
#define aNUM 260
#define aHEXNUM 261
#define aSTRING 262
#define aUUID 263
#define aEOF 264
#define SHL 265
#define SHR 266
#define tAGGREGATABLE 267
#define tALLOCATE 268
#define tAPPOBJECT 269
#define tARRAYS 270
#define tASYNC 271
#define tASYNCUUID 272
#define tAUTOHANDLE 273
#define tBINDABLE 274
#define tBOOLEAN 275
#define tBROADCAST 276
#define tBYTE 277
#define tBYTECOUNT 278
#define tCALLAS 279
#define tCALLBACK 280
#define tCASE 281
#define tCDECL 282
#define tCHAR 283
#define tCOCLASS 284
#define tCODE 285
#define tCOMMSTATUS 286
#define tCONST 287
#define tCONTEXTHANDLE 288
#define tCONTEXTHANDLENOSERIALIZE 289
#define tCONTEXTHANDLESERIALIZE 290
#define tCONTROL 291
#define tCPPQUOTE 292
#define tDEFAULT 293
#define tDEFAULTVALUE 294
#define tDISPINTERFACE 295
#define tDLLNAME 296
#define tDOUBLE 297
#define tDUAL 298
#define tENDPOINT 299
#define tENTRY 300
#define tENUM 301
#define tERRORSTATUST 302
#define tEXPLICITHANDLE 303
#define tEXTERN 304
#define tFLOAT 305
#define tHANDLE 306
#define tHANDLET 307
#define tHELPCONTEXT 308
#define tHELPFILE 309
#define tHELPSTRING 310
#define tHELPSTRINGCONTEXT 311
#define tHELPSTRINGDLL 312
#define tHIDDEN 313
#define tHYPER 314
#define tID 315
#define tIDEMPOTENT 316
#define tIIDIS 317
#define tIMPLICITHANDLE 318
#define tIMPORT 319
#define tIMPORTLIB 320
#define tIN 321
#define tINCLUDE 322
#define tINLINE 323
#define tINPUTSYNC 324
#define tINT 325
#define tINT64 326
#define tINTERFACE 327
#define tLENGTHIS 328
#define tLIBRARY 329
#define tLOCAL 330
#define tLONG 331
#define tMETHODS 332
#define tMODULE 333
#define tNONCREATABLE 334
#define tOBJECT 335
#define tODL 336
#define tOLEAUTOMATION 337
#define tOPTIONAL 338
#define tOUT 339
#define tPOINTERDEFAULT 340
#define tPROPERTIES 341
#define tPROPGET 342
#define tPROPPUT 343
#define tPROPPUTREF 344
#define tPUBLIC 345
#define tREADONLY 346
#define tREF 347
#define tRESTRICTED 348
#define tRETVAL 349
#define tSHORT 350
#define tSIGNED 351
#define tSINGLE 352
#define tSIZEIS 353
#define tSIZEOF 354
#define tSOURCE 355
#define tSTDCALL 356
#define tSTRING 357
#define tSTRUCT 358
#define tSWITCH 359
#define tSWITCHIS 360
#define tSWITCHTYPE 361
#define tTRANSMITAS 362
#define tTYPEDEF 363
#define tUNION 364
#define tUNIQUE 365
#define tUNSIGNED 366
#define tUUID 367
#define tV1ENUM 368
#define tVARARG 369
#define tVERSION 370
#define tVOID 371
#define tWCHAR 372
#define tWIREMARSHAL 373
#define tPOINTERTYPE 374
#define COND 375
#define CAST 376
#define PPTR 377
#define NEG 378




/* Copy the first part of user declarations.  */
#line 1 "./parser.y"

/*
 * IDL Compiler
 *
 * Copyright 2002 Ove Kaaven
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "windef.h"

#include "widl.h"
#include "utils.h"
#include "parser.h"
#include "header.h"
#include "typelib.h"

#if defined(YYBYACC)
	/* Berkeley yacc (byacc) doesn't seem to know about these */
	/* Some *BSD supplied versions do define these though */
# ifndef YYEMPTY
#  define YYEMPTY	(-1)	/* Empty lookahead value of yychar */
# endif
# ifndef YYLEX
#  define YYLEX		yylex()
# endif

#elif defined(YYBISON)
	/* Bison was used for original development */
	/* #define YYEMPTY -2 */
	/* #define YYLEX   yylex() */

#else
	/* No yacc we know yet */
# if !defined(YYEMPTY) || !defined(YYLEX)
#  error Yacc version/type unknown. This version needs to be verified for settings of YYEMPTY and YYLEX.
# elif defined(__GNUC__)	/* gcc defines the #warning directive */
#  warning Yacc version/type unknown. It defines YYEMPTY and YYLEX, but is not tested
  /* #else we just take a chance that it works... */
# endif
#endif

static attr_t *make_attr(enum attr_type type);
static attr_t *make_attrv(enum attr_type type, unsigned long val);
static attr_t *make_attrp(enum attr_type type, void *val);
static expr_t *make_expr(enum expr_type type);
static expr_t *make_exprl(enum expr_type type, long val);
static expr_t *make_exprs(enum expr_type type, char *val);
static expr_t *make_exprt(enum expr_type type, typeref_t *tref, expr_t *expr);
static expr_t *make_expr1(enum expr_type type, expr_t *expr);
static expr_t *make_expr2(enum expr_type type, expr_t *exp1, expr_t *exp2);
static expr_t *make_expr3(enum expr_type type, expr_t *expr1, expr_t *expr2, expr_t *expr3);
static type_t *make_type(unsigned char type, type_t *ref);
static typeref_t *make_tref(char *name, type_t *ref);
static typeref_t *uniq_tref(typeref_t *ref);
static type_t *type_ref(typeref_t *ref);
static void set_type(var_t *v, typeref_t *ref, expr_t *arr);
static ifref_t *make_ifref(type_t *iface);
static var_t *make_var(char *name);
static func_t *make_func(var_t *def, var_t *args);
static class_t *make_class(char *name);

static type_t *reg_type(type_t *type, char *name, int t);
static type_t *reg_types(type_t *type, var_t *names, int t);
static type_t *find_type(char *name, int t);
static type_t *find_type2(char *name, int t);
static type_t *get_type(unsigned char type, char *name, int t);
static type_t *get_typev(unsigned char type, var_t *name, int t);
static int get_struct_type(var_t *fields);

static var_t *reg_const(var_t *var);
static var_t *find_const(char *name, int f);

#define tsENUM   1
#define tsSTRUCT 2
#define tsUNION  3

static type_t std_bool = { "boolean" };
static type_t std_int = { "int" };
static type_t std_int64 = { "__int64" };
static type_t std_uhyper = { "MIDL_uhyper" };



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
#line 108 "./parser.y"
typedef union {
	attr_t *attr;
	expr_t *expr;
	type_t *type;
	typeref_t *tref;
	var_t *var;
	func_t *func;
	ifref_t *ifref;
	class_t *clas;
	char *str;
	UUID *uuid;
	unsigned int num;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 441 "y.tab.c"
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
#line 462 "y.tab.c"

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
#define YYFINAL  3
#define YYLAST   930

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  143
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  73
/* YYNRULES -- Number of rules. */
#define YYNRULES  246
/* YYNRULES -- Number of states. */
#define YYNSTATES  458

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   378

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   123,     2,
     133,   134,   126,   125,   120,   124,   142,   127,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   139,   132,
       2,   140,     2,   141,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   137,     2,   138,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   135,   122,   136,   128,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   121,   129,   130,   131
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    12,    15,    18,    21,
      24,    25,    28,    31,    34,    37,    40,    41,    45,    48,
      50,    53,    55,    58,    61,    63,    66,    69,    72,    77,
      81,    85,    88,    92,    96,    97,    99,   101,   103,   107,
     109,   114,   118,   125,   131,   132,   136,   140,   142,   146,
     151,   152,   154,   158,   160,   164,   169,   171,   173,   178,
     183,   185,   187,   189,   191,   193,   198,   203,   208,   210,
     215,   220,   225,   227,   229,   234,   239,   244,   249,   254,
     256,   261,   263,   268,   274,   276,   278,   283,   285,   287,
     289,   291,   293,   295,   297,   302,   304,   306,   308,   310,
     312,   314,   316,   321,   323,   325,   330,   335,   340,   345,
     347,   349,   354,   359,   361,   362,   364,   365,   368,   373,
     377,   383,   384,   387,   389,   391,   395,   399,   401,   407,
     409,   413,   414,   416,   418,   420,   422,   428,   432,   436,
     440,   444,   448,   452,   456,   460,   463,   466,   469,   474,
     479,   483,   485,   489,   491,   496,   497,   500,   503,   507,
     510,   512,   517,   525,   526,   528,   529,   531,   533,   535,
     537,   539,   541,   543,   545,   547,   549,   552,   555,   557,
     559,   561,   563,   565,   567,   569,   570,   572,   574,   577,
     580,   583,   585,   587,   590,   593,   596,   601,   602,   605,
     608,   611,   614,   617,   620,   624,   627,   631,   637,   638,
     641,   644,   647,   650,   656,   664,   666,   669,   672,   675,
     678,   681,   686,   689,   692,   694,   696,   700,   702,   706,
     708,   710,   716,   718,   720,   722,   725,   727,   730,   732,
     735,   737,   740,   745,   751,   762,   764
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     144,     0,    -1,   145,    -1,    -1,   145,   203,    -1,   145,
     202,    -1,   145,   191,    -1,   145,   206,    -1,   145,   154,
      -1,   145,   148,    -1,    -1,   146,   203,    -1,   146,   202,
      -1,   146,   191,    -1,   146,   206,    -1,   146,   148,    -1,
      -1,   147,   182,   132,    -1,   147,   148,    -1,   132,    -1,
     168,   132,    -1,   149,    -1,   172,   132,    -1,   178,   132,
      -1,   151,    -1,   211,   132,    -1,   213,   132,    -1,   214,
     132,    -1,    37,   133,     7,   134,    -1,    64,     7,   132,
      -1,   150,   146,     9,    -1,    74,     3,    -1,   162,   152,
     135,    -1,   153,   146,   136,    -1,    -1,   157,    -1,   116,
      -1,   158,    -1,   157,   120,   158,    -1,   156,    -1,   162,
     212,   208,   159,    -1,   212,   208,   159,    -1,   162,   212,
     208,   133,   155,   134,    -1,   212,   208,   133,   155,   134,
      -1,    -1,   137,   160,   138,    -1,   137,   126,   138,    -1,
     174,    -1,   160,   120,   175,    -1,   160,   138,   137,   175,
      -1,    -1,   162,    -1,   137,   163,   138,    -1,   164,    -1,
     163,   120,   164,    -1,   163,   138,   137,   164,    -1,    16,
      -1,    18,    -1,    24,   133,   185,   134,    -1,    26,   133,
     176,   134,    -1,    33,    -1,    34,    -1,    35,    -1,    36,
      -1,    38,    -1,    39,   133,   177,   134,    -1,    39,   133,
       7,   134,    -1,    41,   133,     7,   134,    -1,    43,    -1,
      44,   133,     7,   134,    -1,    45,   133,     7,   134,    -1,
      45,   133,   177,   134,    -1,    48,    -1,    51,    -1,    53,
     133,   177,   134,    -1,    54,   133,     7,   134,    -1,    55,
     133,     7,   134,    -1,    56,   133,   177,   134,    -1,    57,
     133,     7,   134,    -1,    58,    -1,    60,   133,   177,   134,
      -1,    61,    -1,    62,   133,   185,   134,    -1,    63,   133,
      52,     3,   134,    -1,    66,    -1,    69,    -1,    73,   133,
     173,   134,    -1,    75,    -1,    79,    -1,    80,    -1,    81,
      -1,    82,    -1,    83,    -1,    84,    -1,    85,   133,   210,
     134,    -1,    87,    -1,    88,    -1,    89,    -1,    90,    -1,
      91,    -1,    93,    -1,    94,    -1,    98,   133,   173,   134,
      -1,   100,    -1,   102,    -1,   105,   133,   175,   134,    -1,
     106,   133,   212,   134,    -1,   107,   133,   212,   134,    -1,
     112,   133,     8,   134,    -1,   113,    -1,   114,    -1,   115,
     133,   215,   134,    -1,   118,   133,   212,   134,    -1,   210,
      -1,    -1,   101,    -1,    -1,   166,   167,    -1,    26,   175,
     139,   180,    -1,    38,   139,   180,    -1,    32,   212,   185,
     140,   177,    -1,    -1,   170,   120,    -1,   170,    -1,   171,
      -1,   170,   120,   171,    -1,   185,   140,   177,    -1,   185,
      -1,    46,   184,   135,   169,   136,    -1,   174,    -1,   173,
     120,   174,    -1,    -1,   175,    -1,     5,    -1,     6,    -1,
       3,    -1,   175,   141,   175,   139,   175,    -1,   175,   122,
     175,    -1,   175,   123,   175,    -1,   175,   125,   175,    -1,
     175,   124,   175,    -1,   175,   126,   175,    -1,   175,   127,
     175,    -1,   175,    10,   175,    -1,   175,    11,   175,    -1,
     128,   175,    -1,   124,   175,    -1,   126,   175,    -1,   133,
     212,   134,   175,    -1,    99,   133,   212,   134,    -1,   133,
     175,   134,    -1,   177,    -1,   176,   120,   177,    -1,   175,
      -1,    49,    32,   212,   185,    -1,    -1,   179,   180,    -1,
     181,   132,    -1,   161,   214,   132,    -1,   162,   132,    -1,
     132,    -1,   161,   212,   208,   159,    -1,   161,   212,   165,
     208,   133,   155,   134,    -1,    -1,   185,    -1,    -1,     3,
      -1,     4,    -1,     3,    -1,     4,    -1,    60,    -1,    94,
      -1,   115,    -1,    22,    -1,   117,    -1,   188,    -1,    96,
     188,    -1,   111,   188,    -1,   111,    -1,    50,    -1,    97,
      -1,    42,    -1,    20,    -1,    47,    -1,    52,    -1,    -1,
      70,    -1,    70,    -1,    95,   187,    -1,    76,   187,    -1,
      59,   187,    -1,    71,    -1,    28,    -1,    29,     3,    -1,
      29,     4,    -1,   162,   189,    -1,   190,   135,   192,   136,
      -1,    -1,   192,   193,    -1,   161,   203,    -1,    40,     3,
      -1,    40,     4,    -1,   162,   194,    -1,    86,   139,    -1,
     196,   181,   132,    -1,    77,   139,    -1,   197,   182,   132,
      -1,   195,   135,   196,   197,   136,    -1,    -1,   139,     4,
      -1,    72,     3,    -1,    72,     4,    -1,   162,   200,    -1,
     201,   199,   135,   147,   136,    -1,   201,   139,     3,   135,
     151,   147,   136,    -1,   198,    -1,   200,   132,    -1,   194,
     132,    -1,    78,     3,    -1,    78,     4,    -1,   162,   204,
      -1,   205,   135,   147,   136,    -1,   126,   208,    -1,    32,
     207,    -1,   185,    -1,   207,    -1,   133,   208,   134,    -1,
     208,    -1,   209,   120,   208,    -1,    92,    -1,   110,    -1,
     103,   184,   135,   179,   136,    -1,   116,    -1,     4,    -1,
     186,    -1,    32,   212,    -1,   172,    -1,    46,     3,    -1,
     211,    -1,   103,     3,    -1,   214,    -1,   109,     3,    -1,
     108,   161,   212,   209,    -1,   109,   184,   135,   179,   136,
      -1,   109,   184,   104,   133,   181,   134,   183,   135,   166,
     136,    -1,     5,    -1,     5,   142,     5,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   228,   228,   231,   232,   233,   234,   235,   236,   237,
     240,   241,   242,   243,   244,   245,   248,   249,   250,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   264,   266,
     269,   272,   274,   277,   280,   281,   284,   287,   288,   289,
     293,   297,   300,   306,   313,   314,   315,   318,   319,   320,
     323,   324,   327,   331,   332,   333,   336,   338,   339,   340,
     341,   342,   343,   344,   345,   346,   347,   348,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   358,   359,   360,
     361,   362,   363,   364,   365,   366,   367,   368,   369,   370,
     371,   372,   373,   374,   375,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,   389,   390,
     391,   392,   393,   394,   397,   398,   401,   402,   407,   411,
     417,   424,   425,   426,   429,   430,   436,   441,   447,   455,
     456,   461,   462,   465,   466,   467,   468,   469,   470,   471,
     472,   473,   474,   475,   476,   477,   478,   479,   480,   481,
     482,   485,   486,   489,   495,   500,   501,   506,   507,   508,
     509,   512,   515,   526,   527,   530,   531,   532,   535,   537,
     538,   539,   540,   543,   544,   545,   546,   547,   559,   560,
     561,   562,   563,   564,   565,   568,   569,   572,   573,   574,
     575,   576,   577,   580,   581,   584,   590,   595,   596,   599,
     603,   604,   607,   619,   620,   623,   624,   627,   638,   639,
     642,   643,   646,   654,   661,   668,   671,   673,   676,   677,
     680,   685,   691,   692,   695,   696,   697,   700,   702,   705,
     707,   710,   720,   721,   722,   723,   724,   725,   726,   727,
     728,   729,   732,   744,   748,   761,   763
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "aIDENTIFIER", "aKNOWNTYPE", "aNUM", 
  "aHEXNUM", "aSTRING", "aUUID", "aEOF", "SHL", "SHR", "tAGGREGATABLE", 
  "tALLOCATE", "tAPPOBJECT", "tARRAYS", "tASYNC", "tASYNCUUID", 
  "tAUTOHANDLE", "tBINDABLE", "tBOOLEAN", "tBROADCAST", "tBYTE", 
  "tBYTECOUNT", "tCALLAS", "tCALLBACK", "tCASE", "tCDECL", "tCHAR", 
  "tCOCLASS", "tCODE", "tCOMMSTATUS", "tCONST", "tCONTEXTHANDLE", 
  "tCONTEXTHANDLENOSERIALIZE", "tCONTEXTHANDLESERIALIZE", "tCONTROL", 
  "tCPPQUOTE", "tDEFAULT", "tDEFAULTVALUE", "tDISPINTERFACE", "tDLLNAME", 
  "tDOUBLE", "tDUAL", "tENDPOINT", "tENTRY", "tENUM", "tERRORSTATUST", 
  "tEXPLICITHANDLE", "tEXTERN", "tFLOAT", "tHANDLE", "tHANDLET", 
  "tHELPCONTEXT", "tHELPFILE", "tHELPSTRING", "tHELPSTRINGCONTEXT", 
  "tHELPSTRINGDLL", "tHIDDEN", "tHYPER", "tID", "tIDEMPOTENT", "tIIDIS", 
  "tIMPLICITHANDLE", "tIMPORT", "tIMPORTLIB", "tIN", "tINCLUDE", 
  "tINLINE", "tINPUTSYNC", "tINT", "tINT64", "tINTERFACE", "tLENGTHIS", 
  "tLIBRARY", "tLOCAL", "tLONG", "tMETHODS", "tMODULE", "tNONCREATABLE", 
  "tOBJECT", "tODL", "tOLEAUTOMATION", "tOPTIONAL", "tOUT", 
  "tPOINTERDEFAULT", "tPROPERTIES", "tPROPGET", "tPROPPUT", "tPROPPUTREF", 
  "tPUBLIC", "tREADONLY", "tREF", "tRESTRICTED", "tRETVAL", "tSHORT", 
  "tSIGNED", "tSINGLE", "tSIZEIS", "tSIZEOF", "tSOURCE", "tSTDCALL", 
  "tSTRING", "tSTRUCT", "tSWITCH", "tSWITCHIS", "tSWITCHTYPE", 
  "tTRANSMITAS", "tTYPEDEF", "tUNION", "tUNIQUE", "tUNSIGNED", "tUUID", 
  "tV1ENUM", "tVARARG", "tVERSION", "tVOID", "tWCHAR", "tWIREMARSHAL", 
  "tPOINTERTYPE", "','", "COND", "'|'", "'&'", "'-'", "'+'", "'*'", "'/'", 
  "'~'", "CAST", "PPTR", "NEG", "';'", "'('", "')'", "'{'", "'}'", "'['", 
  "']'", "':'", "'='", "'?'", "'.'", "$accept", "input", "gbl_statements", 
  "imp_statements", "int_statements", "statement", "cppquote", 
  "import_start", "import", "libraryhdr", "library_start", "librarydef", 
  "m_args", "no_args", "args", "arg", "array", "array_list", 
  "m_attributes", "attributes", "attrib_list", "attribute", "callconv", 
  "cases", "case", "constdef", "enums", "enum_list", "enum", "enumdef", 
  "m_exprs", "m_expr", "expr", "expr_list_const", "expr_const", 
  "externdef", "fields", "field", "s_field", "funcdef", "m_ident", 
  "t_ident", "ident", "base_type", "m_int", "int_std", "coclass", 
  "coclasshdr", "coclassdef", "coclass_ints", "coclass_int", 
  "dispinterface", "dispinterfacehdr", "dispint_props", "dispint_meths", 
  "dispinterfacedef", "inherit", "interface", "interfacehdr", 
  "interfacedef", "interfacedec", "module", "modulehdr", "moduledef", 
  "p_ident", "pident", "pident_list", "pointer_type", "structdef", "type", 
  "typedef", "uniondef", "version", 0
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
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
      44,   375,   124,    38,    45,    43,    42,    47,   126,   376,
     377,   378,    59,    40,    41,   123,   125,    91,    93,    58,
      61,    63,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   143,   144,   145,   145,   145,   145,   145,   145,   145,
     146,   146,   146,   146,   146,   146,   147,   147,   147,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   149,   150,
     151,   152,   153,   154,   155,   155,   156,   157,   157,   157,
     158,   158,   158,   158,   159,   159,   159,   160,   160,   160,
     161,   161,   162,   163,   163,   163,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   165,   165,   166,   166,   167,   167,
     168,   169,   169,   169,   170,   170,   171,   171,   172,   173,
     173,   174,   174,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   176,   176,   177,   178,   179,   179,   180,   180,   180,
     180,   181,   182,   183,   183,   184,   184,   184,   185,   185,
     185,   185,   185,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   187,   187,   188,   188,   188,
     188,   188,   188,   189,   189,   190,   191,   192,   192,   193,
     194,   194,   195,   196,   196,   197,   197,   198,   199,   199,
     200,   200,   201,   202,   202,   202,   203,   203,   204,   204,
     205,   206,   207,   207,   208,   208,   208,   209,   209,   210,
     210,   211,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   213,   214,   214,   215,   215
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     2,     2,     2,
       0,     2,     2,     2,     2,     2,     0,     3,     2,     1,
       2,     1,     2,     2,     1,     2,     2,     2,     4,     3,
       3,     2,     3,     3,     0,     1,     1,     1,     3,     1,
       4,     3,     6,     5,     0,     3,     3,     1,     3,     4,
       0,     1,     3,     1,     3,     4,     1,     1,     4,     4,
       1,     1,     1,     1,     1,     4,     4,     4,     1,     4,
       4,     4,     1,     1,     4,     4,     4,     4,     4,     1,
       4,     1,     4,     5,     1,     1,     4,     1,     1,     1,
       1,     1,     1,     1,     4,     1,     1,     1,     1,     1,
       1,     1,     4,     1,     1,     4,     4,     4,     4,     1,
       1,     4,     4,     1,     0,     1,     0,     2,     4,     3,
       5,     0,     2,     1,     1,     3,     3,     1,     5,     1,
       3,     0,     1,     1,     1,     1,     5,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     4,     4,
       3,     1,     3,     1,     4,     0,     2,     2,     3,     2,
       1,     4,     7,     0,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     1,     2,     2,
       2,     1,     1,     2,     2,     2,     4,     0,     2,     2,
       2,     2,     2,     2,     3,     2,     3,     5,     0,     2,
       2,     2,     2,     5,     7,     1,     2,     2,     2,     2,
       2,     4,     2,     2,     1,     1,     3,     1,     3,     1,
       1,     5,     1,     1,     1,     2,     1,     2,     1,     2,
       1,     2,     4,     5,    10,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,   165,     0,     0,
       0,   165,    50,   165,    19,     0,     9,    21,    10,    24,
      10,     8,     0,     0,     0,     0,     0,     6,     0,     0,
     215,     0,   208,     5,     4,     0,     7,     0,     0,     0,
     233,   182,   173,   192,     0,   181,   165,   183,   179,   184,
     185,   187,   191,   185,   185,     0,   180,   165,   165,   178,
     232,   174,   236,   234,   175,   238,     0,   240,     0,   200,
     201,   166,   167,     0,     0,     0,   210,   211,     0,     0,
      51,     0,    56,    57,     0,     0,    60,    61,    62,    63,
      64,     0,     0,    68,     0,     0,    72,    73,     0,     0,
       0,     0,     0,    79,     0,    81,     0,     0,    84,    85,
       0,    87,    88,    89,    90,    91,    92,    93,     0,    95,
      96,    97,    98,    99,   229,   100,   101,     0,   103,   104,
       0,     0,     0,   230,     0,   109,   110,     0,     0,     0,
      53,   113,     0,     0,     0,     0,     0,     0,   195,   202,
     212,   220,    20,    22,    23,   197,   217,     0,   216,     0,
       0,    16,    25,    26,    27,   235,   237,   186,   190,   189,
     188,   176,   239,   241,   177,   168,   169,   170,   171,   172,
       0,     0,   121,     0,    29,   155,     0,     0,   155,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   131,     0,   131,     0,     0,     0,     0,
       0,     0,     0,    52,    30,    15,     0,    13,    12,    11,
      14,    33,   193,   194,    31,   218,   219,    32,    50,     0,
      50,     0,   209,    16,    50,     0,    28,     0,   123,   124,
     127,   154,    50,     0,     0,     0,   224,   225,   227,   242,
      50,    50,     0,   135,   133,   134,     0,     0,     0,     0,
       0,   153,     0,   151,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
     132,     0,     0,     0,     0,     0,     0,   245,     0,     0,
      54,     0,   196,     0,   198,   203,     0,     0,     0,    50,
       0,    50,   221,    18,     0,     0,   120,   128,   122,     0,
     160,   231,     0,    51,   156,     0,   223,   222,     0,     0,
       0,   243,    58,     0,   146,   147,   145,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
      66,    65,    67,    69,    70,    71,    74,    75,    76,    77,
      78,    80,    82,     0,   131,    86,    94,   102,   105,   106,
     107,   108,     0,   111,   112,    55,   199,   205,     0,   204,
     207,     0,    16,   213,   114,    17,   125,   126,   240,   159,
     157,   226,   228,   163,     0,   150,     0,   143,   144,   137,
     138,   140,   139,   141,   142,     0,   152,    83,   130,   246,
      44,   206,    50,   115,     0,   158,     0,   164,   149,   148,
       0,   131,   161,   214,     0,   116,   136,     0,     0,    47,
      34,     0,    46,     0,    45,   232,     0,    39,    35,    37,
       0,     0,     0,     0,   244,   117,    48,     0,   162,     0,
       0,    44,     0,    50,    49,    38,    44,    34,    41,    50,
     119,    34,    40,     0,   118,     0,    43,    42
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,   142,   234,   303,    17,    18,    19,   147,
      20,    21,   426,   427,   428,   429,   412,   418,   304,    80,
     139,   140,   404,   421,   435,    23,   237,   238,   239,    62,
     278,   279,   261,   262,   263,    25,   242,   314,   315,   305,
     406,    73,   246,    63,   168,    64,   148,    26,   217,   228,
     294,    28,    29,   230,   299,    30,   160,    31,    32,   218,
     219,   151,    35,   220,   247,   248,   249,   141,    65,   431,
      38,    67,   288
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -405
static const short yypact[] =
{
    -405,    35,   626,  -405,   585,   -87,   117,   152,    36,    67,
     185,   152,   -57,   152,  -405,   812,  -405,  -405,  -405,  -405,
    -405,  -405,   -11,   -19,     0,    26,   -34,  -405,    44,     1,
    -405,    49,    -5,  -405,  -405,    15,  -405,    58,    61,    75,
    -405,  -405,  -405,  -405,   585,  -405,   206,  -405,  -405,  -405,
     142,  -405,  -405,   142,   142,    16,  -405,   211,   213,    16,
    -405,  -405,  -405,  -405,  -405,  -405,    18,  -405,   131,  -405,
    -405,  -405,  -405,    83,   585,    88,  -405,  -405,    87,   585,
    -405,   -71,  -405,  -405,    93,   104,  -405,  -405,  -405,  -405,
    -405,   112,   134,  -405,   135,   137,  -405,  -405,   138,   139,
     143,   145,   149,  -405,   150,  -405,   153,   154,  -405,  -405,
     155,  -405,  -405,  -405,  -405,  -405,  -405,  -405,   157,  -405,
    -405,  -405,  -405,  -405,  -405,  -405,  -405,   162,  -405,  -405,
     164,   165,   166,  -405,   171,  -405,  -405,   173,   174,   -88,
    -405,  -405,   493,   596,   221,   272,   231,   176,  -405,  -405,
    -405,  -405,  -405,  -405,  -405,  -405,  -405,   199,  -405,   249,
     178,  -405,  -405,  -405,  -405,  -405,   180,  -405,  -405,  -405,
    -405,  -405,   180,   -70,  -405,  -405,  -405,  -405,  -405,  -405,
     177,   182,    18,    18,  -405,  -405,   485,   188,  -405,    18,
     425,   274,   311,   316,   286,   425,   317,   318,   425,   321,
     425,    18,   277,   425,   -54,   425,   425,   585,   585,   323,
     328,   585,   812,   197,  -405,  -405,     7,  -405,  -405,  -405,
    -405,  -405,  -405,  -405,  -405,  -405,  -405,  -405,   125,   196,
     -49,   201,  -405,  -405,   646,   425,  -405,   202,   217,  -405,
     200,  -405,   -28,   -20,   485,   485,  -405,  -405,  -405,   222,
     -57,    -6,   207,  -405,  -405,  -405,   210,   425,   425,   425,
     464,   335,   -68,  -405,   214,   215,   218,   219,   220,   223,
     224,   225,   226,   227,   244,   246,   248,   341,   -62,  -405,
     335,   250,   -31,    72,   253,   254,   256,   205,   258,   259,
    -405,   812,  -405,   -10,  -405,  -405,   216,   585,   242,   127,
     330,   676,  -405,  -405,   585,   263,  -405,  -405,    18,   425,
    -405,  -405,   585,   267,  -405,   269,  -405,  -405,   271,   485,
     275,  -405,  -405,   585,   262,   262,   262,   105,   279,   425,
     425,   425,   425,   425,   425,   425,   425,   425,   425,  -405,
    -405,  -405,  -405,  -405,  -405,  -405,  -405,  -405,  -405,  -405,
    -405,  -405,  -405,   281,   425,  -405,  -405,  -405,  -405,  -405,
    -405,  -405,   411,  -405,  -405,  -405,  -405,  -405,   485,  -405,
    -405,   288,  -405,  -405,   320,  -405,  -405,  -405,   290,  -405,
    -405,  -405,  -405,    18,   289,  -405,   425,   262,   262,   245,
      59,   132,   132,    13,    13,    38,  -405,  -405,  -405,  -405,
     292,  -405,   707,  -405,   485,  -405,   298,  -405,  -405,   262,
     425,   488,  -405,  -405,   302,  -405,   335,    54,   -47,  -405,
     280,    -1,  -405,   425,   301,   -27,   291,  -405,   319,  -405,
     585,   485,   425,   303,  -405,  -405,   335,   425,  -405,   404,
     485,   -56,   240,    -8,   335,  -405,   -35,   280,  -405,    -8,
    -405,   280,  -405,   309,  -405,   310,  -405,  -405
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -405,  -405,  -405,   428,  -213,     6,  -405,  -405,   172,  -405,
    -405,  -405,  -337,  -405,  -405,    14,  -324,  -405,    -9,    -2,
    -405,  -186,  -405,  -405,  -405,  -405,  -405,  -405,   144,     4,
     260,  -327,  -163,  -405,  -181,  -405,   276,  -404,  -199,   156,
    -405,    42,   -64,  -405,   212,    68,  -405,  -405,   469,  -405,
    -405,   -15,  -405,  -405,  -405,  -405,  -405,   -13,  -405,   471,
       3,  -405,  -405,   475,   235,  -229,  -405,   278,     9,    -3,
    -405,     2,  -405
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -167
static const short yytable[] =
{
      22,    66,   180,    79,    39,    34,    24,   149,    16,   150,
     265,    37,   243,   269,   270,   317,   318,   273,   144,   275,
     301,   175,   176,   329,   330,   432,   290,   398,   296,     6,
       6,   298,   212,   187,  -166,     3,   144,   433,   124,   450,
     280,   165,   280,   283,    43,   454,    68,     6,   329,   330,
     213,   320,   338,    78,   306,    81,   133,   253,   354,   254,
     255,    10,    10,   145,   188,  -166,   339,   146,    74,   329,
     330,   183,   355,   423,    75,    50,   186,   447,   177,    10,
      15,   411,   329,   330,   419,   146,    51,    52,    15,   354,
     382,   424,    53,   -36,   324,   325,   326,   327,   451,    78,
      81,   155,   411,   357,   310,   365,   244,   -36,   311,    15,
     453,    54,   178,   152,   455,   329,   330,   448,   240,   241,
      69,    70,   452,   171,   310,   252,   310,   174,   377,    15,
     321,    15,   153,   179,   159,   434,   157,   276,   181,   400,
     216,   216,   329,   330,    39,    39,    24,    24,   215,   215,
     161,    37,    37,   256,   337,    71,    72,   396,   154,   402,
     331,   332,   333,   334,   335,   336,   387,   388,   389,   390,
     391,   392,   393,   394,   395,   414,   156,   410,   257,   337,
     258,   158,   259,   333,   334,   335,   336,   260,    76,    77,
     162,   280,   422,   163,   331,   332,   333,   334,   335,   336,
     337,   149,   441,   150,   284,   285,   358,   164,   289,   166,
      72,   446,   167,   337,   172,    72,   173,    72,   182,   293,
     184,   297,   185,   409,   222,   223,   189,   331,   332,   333,
     334,   335,   336,   312,   225,   226,    39,   190,    24,   385,
     313,   297,   312,    37,   240,   191,   337,   416,   280,   313,
     329,   330,   231,   232,   325,   329,   330,   328,   335,   336,
     436,   292,    15,   370,    15,   169,   170,   192,   193,   442,
     194,   195,   196,   337,   444,   224,   197,   253,   198,   254,
     255,   264,   199,   200,    40,   229,   201,   202,   203,   253,
     204,   254,   255,   268,   368,   205,   366,   206,   207,   208,
      41,   374,    42,    39,   209,    24,   210,   211,    43,   368,
      37,   227,    44,   233,   378,  -166,   236,   235,   266,   407,
     384,   250,    45,   267,   271,   272,    46,    47,   274,   277,
      48,   286,    49,   287,   291,   295,   300,   308,   307,    50,
     309,   322,   319,   323,   353,   329,   330,   362,   340,   341,
      51,    52,   342,   343,   344,   367,    53,   345,   346,   347,
     348,   349,   331,   332,   333,   334,   335,   336,   332,   333,
     334,   335,   336,   256,   369,    54,    55,    56,   350,   449,
     351,   337,   352,    57,   356,   256,   337,   359,   360,    58,
     361,    59,   363,   364,     9,   375,   425,    61,   257,   379,
     258,   380,   259,   337,    39,   381,    24,   260,    40,   383,
     257,    37,   258,   386,   259,   397,   399,    15,   430,   260,
     401,   403,   405,   408,    41,   438,    42,   440,   253,   411,
     254,   255,    43,   415,   312,   420,    44,   430,   437,   439,
     312,   313,   443,   456,   457,   430,    45,   313,   143,   430,
      46,    47,   376,   445,    48,   371,    49,   331,   332,   333,
     334,   335,   336,    50,   251,   282,     0,   253,    40,   254,
     255,    27,   372,    33,    51,    52,   337,    36,   316,     0,
      53,     0,   281,     0,    41,     0,    42,     0,   175,   176,
       0,   253,    43,   254,   255,     0,    44,     0,     0,    54,
      55,    56,   214,     0,     0,     0,    45,    57,     0,     0,
      46,    47,     0,    58,    48,    59,    49,   243,     0,     0,
      60,    61,     0,    50,   256,     4,     0,     0,     0,     0,
       5,     0,     0,     6,    51,    52,     0,     0,     0,     7,
      53,    15,     8,     0,     0,   177,     0,     0,     0,   257,
       0,   258,     0,   259,     0,     0,     0,     9,   260,    54,
      55,    56,     0,   256,     0,    10,     0,    57,     0,     0,
       0,     0,     0,    58,     0,    59,     0,     0,     0,   178,
      60,    61,     0,     0,     0,     0,     0,   256,   257,    40,
     258,     0,   259,     0,     0,     0,    11,   260,     0,     0,
     179,    12,    13,     0,     0,    41,     0,    42,     0,     0,
       0,   244,   257,    43,   417,     0,   259,    44,   245,     0,
       0,   260,     0,     0,     0,    14,     0,    45,     4,     0,
      15,    46,    47,     5,     0,    48,     6,    49,     0,     0,
       0,     0,     7,     0,    50,     8,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    51,    52,     0,     4,     0,
       9,    53,     0,     5,     0,     0,     6,     0,    10,     0,
       0,     0,     7,     0,     0,     8,     0,     0,     4,     0,
      54,    55,    56,     5,     0,     0,     0,     0,    57,     0,
       9,     0,     7,     0,    58,     8,    59,     0,    10,    11,
       0,    60,    61,     0,    12,    13,     0,     0,     4,     0,
       9,     0,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     7,     0,     0,     8,     0,     0,    14,    11,
       0,     0,   221,    15,    12,    13,     0,     0,     0,     4,
       9,     0,     0,     0,     5,     0,     0,     0,     0,    11,
       0,     0,     0,     7,    12,    13,     8,     0,    14,     0,
       0,     0,     0,    15,     0,     0,     0,     0,     0,     0,
       0,     9,     0,     0,     0,     0,     0,     0,    14,    11,
       0,     0,   302,    15,    12,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
      11,     0,   373,    15,     0,    12,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    82,     0,
      83,     0,     0,     0,     0,     0,    84,     0,    85,    14,
       0,     0,     0,   413,    15,    86,    87,    88,    89,     0,
      90,    91,     0,    92,     0,    93,    94,    95,     0,     0,
      96,     0,     0,    97,     0,    98,    99,   100,   101,   102,
     103,     0,   104,   105,   106,   107,     0,     0,   108,     0,
       0,   109,     0,     0,     0,   110,     0,   111,     0,     0,
       0,   112,   113,   114,   115,   116,   117,   118,     0,   119,
     120,   121,   122,   123,   124,   125,   126,     0,     0,     0,
     127,     0,   128,     0,   129,     0,     0,   130,   131,   132,
       0,     0,   133,     0,   134,   135,   136,   137,     0,     0,
     138
};

static const short yycheck[] =
{
       2,     4,    66,    12,     2,     2,     2,    22,     2,    22,
     191,     2,    32,   194,   195,   244,   245,   198,    29,   200,
     233,     3,     4,    10,    11,    26,   212,   354,    77,    40,
      40,   230,   120,   104,   104,     0,    29,    38,    92,   443,
     203,    44,   205,   206,    28,   449,   133,    40,    10,    11,
     138,   250,   120,    11,   235,    13,   110,     3,   120,     5,
       6,    72,    72,    74,   135,   135,   134,    78,    32,    10,
      11,    74,   134,   120,     7,    59,    79,   133,    60,    72,
     137,   137,    10,    11,   411,    78,    70,    71,   137,   120,
     319,   138,    76,   120,   257,   258,   259,   260,   133,    57,
      58,   135,   137,   134,   132,   291,   126,   134,   136,   137,
     447,    95,    94,   132,   451,    10,    11,   441,   182,   183,
       3,     4,   446,    55,   132,   189,   132,    59,   309,   137,
     136,   137,   132,   115,   139,   136,   135,   201,     7,   368,
     142,   143,    10,    11,   142,   143,   142,   143,   142,   143,
     135,   142,   143,    99,   141,     3,     4,   338,   132,   372,
     122,   123,   124,   125,   126,   127,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   404,   132,   139,   124,   141,
     126,   132,   128,   124,   125,   126,   127,   133,     3,     4,
     132,   354,   138,   132,   122,   123,   124,   125,   126,   127,
     141,   216,   431,   216,   207,   208,   134,   132,   211,     3,
       4,   440,    70,   141,     3,     4,     3,     4,   135,   228,
     132,   230,   135,   386,     3,     4,   133,   122,   123,   124,
     125,   126,   127,   242,     3,     4,   234,   133,   234,   134,
     242,   250,   251,   234,   308,   133,   141,   410,   411,   251,
      10,    11,     3,     4,   417,    10,    11,   260,   126,   127,
     423,   136,   137,   136,   137,    53,    54,   133,   133,   432,
     133,   133,   133,   141,   437,     3,   133,     3,   133,     5,
       6,     7,   133,   133,     4,    86,   133,   133,   133,     3,
     133,     5,     6,     7,   297,   133,   293,   133,   133,   133,
      20,   304,    22,   301,   133,   301,   133,   133,    28,   312,
     301,   135,    32,   135,   312,   135,   134,   140,     7,   383,
     323,   133,    42,     7,     7,     7,    46,    47,     7,    52,
      50,     8,    52,     5,   137,   139,   135,   120,   136,    59,
     140,   134,   120,   133,     3,    10,    11,   142,   134,   134,
      70,    71,   134,   134,   134,   139,    76,   134,   134,   134,
     134,   134,   122,   123,   124,   125,   126,   127,   123,   124,
     125,   126,   127,    99,   132,    95,    96,    97,   134,   139,
     134,   141,   134,   103,   134,    99,   141,   134,   134,   109,
     134,   111,   134,   134,    64,   132,   116,   117,   124,   132,
     126,   132,   128,   141,   402,   134,   402,   133,     4,   134,
     124,   402,   126,   134,   128,   134,     5,   137,   420,   133,
     132,   101,   132,   134,    20,   134,    22,   430,     3,   137,
       5,     6,    28,   135,   443,   133,    32,   439,   137,   120,
     449,   443,   139,   134,   134,   447,    42,   449,    20,   451,
      46,    47,   308,   439,    50,   299,    52,   122,   123,   124,
     125,   126,   127,    59,   188,   205,    -1,     3,     4,     5,
       6,     2,   300,     2,    70,    71,   141,     2,   243,    -1,
      76,    -1,   204,    -1,    20,    -1,    22,    -1,     3,     4,
      -1,     3,    28,     5,     6,    -1,    32,    -1,    -1,    95,
      96,    97,     9,    -1,    -1,    -1,    42,   103,    -1,    -1,
      46,    47,    -1,   109,    50,   111,    52,    32,    -1,    -1,
     116,   117,    -1,    59,    99,    32,    -1,    -1,    -1,    -1,
      37,    -1,    -1,    40,    70,    71,    -1,    -1,    -1,    46,
      76,   137,    49,    -1,    -1,    60,    -1,    -1,    -1,   124,
      -1,   126,    -1,   128,    -1,    -1,    -1,    64,   133,    95,
      96,    97,    -1,    99,    -1,    72,    -1,   103,    -1,    -1,
      -1,    -1,    -1,   109,    -1,   111,    -1,    -1,    -1,    94,
     116,   117,    -1,    -1,    -1,    -1,    -1,    99,   124,     4,
     126,    -1,   128,    -1,    -1,    -1,   103,   133,    -1,    -1,
     115,   108,   109,    -1,    -1,    20,    -1,    22,    -1,    -1,
      -1,   126,   124,    28,   126,    -1,   128,    32,   133,    -1,
      -1,   133,    -1,    -1,    -1,   132,    -1,    42,    32,    -1,
     137,    46,    47,    37,    -1,    50,    40,    52,    -1,    -1,
      -1,    -1,    46,    -1,    59,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    71,    -1,    32,    -1,
      64,    76,    -1,    37,    -1,    -1,    40,    -1,    72,    -1,
      -1,    -1,    46,    -1,    -1,    49,    -1,    -1,    32,    -1,
      95,    96,    97,    37,    -1,    -1,    -1,    -1,   103,    -1,
      64,    -1,    46,    -1,   109,    49,   111,    -1,    72,   103,
      -1,   116,   117,    -1,   108,   109,    -1,    -1,    32,    -1,
      64,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    -1,    -1,   132,   103,
      -1,    -1,   136,   137,   108,   109,    -1,    -1,    -1,    32,
      64,    -1,    -1,    -1,    37,    -1,    -1,    -1,    -1,   103,
      -1,    -1,    -1,    46,   108,   109,    49,    -1,   132,    -1,
      -1,    -1,    -1,   137,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    -1,    -1,    -1,    -1,   132,   103,
      -1,    -1,   136,   137,   108,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   132,    -1,
     103,    -1,   136,   137,    -1,   108,   109,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    26,   132,
      -1,    -1,    -1,   136,   137,    33,    34,    35,    36,    -1,
      38,    39,    -1,    41,    -1,    43,    44,    45,    -1,    -1,
      48,    -1,    -1,    51,    -1,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    62,    63,    -1,    -1,    66,    -1,
      -1,    69,    -1,    -1,    -1,    73,    -1,    75,    -1,    -1,
      -1,    79,    80,    81,    82,    83,    84,    85,    -1,    87,
      88,    89,    90,    91,    92,    93,    94,    -1,    -1,    -1,
      98,    -1,   100,    -1,   102,    -1,    -1,   105,   106,   107,
      -1,    -1,   110,    -1,   112,   113,   114,   115,    -1,    -1,
     118
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   144,   145,     0,    32,    37,    40,    46,    49,    64,
      72,   103,   108,   109,   132,   137,   148,   149,   150,   151,
     153,   154,   162,   168,   172,   178,   190,   191,   194,   195,
     198,   200,   201,   202,   203,   205,   206,   211,   213,   214,
       4,    20,    22,    28,    32,    42,    46,    47,    50,    52,
      59,    70,    71,    76,    95,    96,    97,   103,   109,   111,
     116,   117,   172,   186,   188,   211,   212,   214,   133,     3,
       4,     3,     4,   184,    32,     7,     3,     4,   184,   161,
     162,   184,    16,    18,    24,    26,    33,    34,    35,    36,
      38,    39,    41,    43,    44,    45,    48,    51,    53,    54,
      55,    56,    57,    58,    60,    61,    62,    63,    66,    69,
      73,    75,    79,    80,    81,    82,    83,    84,    85,    87,
      88,    89,    90,    91,    92,    93,    94,    98,   100,   102,
     105,   106,   107,   110,   112,   113,   114,   115,   118,   163,
     164,   210,   146,   146,    29,    74,    78,   152,   189,   194,
     200,   204,   132,   132,   132,   135,   132,   135,   132,   139,
     199,   135,   132,   132,   132,   212,     3,    70,   187,   187,
     187,   188,     3,     3,   188,     3,     4,    60,    94,   115,
     185,     7,   135,   212,   132,   135,   212,   104,   135,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   120,   138,     9,   148,   162,   191,   202,   203,
     206,   136,     3,     4,     3,     3,     4,   135,   192,    86,
     196,     3,     4,   135,   147,   140,   134,   169,   170,   171,
     185,   185,   179,    32,   126,   133,   185,   207,   208,   209,
     133,   179,   185,     3,     5,     6,    99,   124,   126,   128,
     133,   175,   176,   177,     7,   177,     7,     7,     7,   177,
     177,     7,     7,   177,     7,   177,   185,    52,   173,   174,
     175,   210,   173,   175,   212,   212,     8,     5,   215,   212,
     164,   137,   136,   161,   193,   139,    77,   161,   181,   197,
     135,   147,   136,   148,   161,   182,   177,   136,   120,   140,
     132,   136,   161,   162,   180,   181,   207,   208,   208,   120,
     181,   136,   134,   133,   175,   175,   175,   175,   212,    10,
      11,   122,   123,   124,   125,   126,   127,   141,   120,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,     3,   120,   134,   134,   134,   134,   134,
     134,   134,   142,   134,   134,   164,   203,   139,   212,   132,
     136,   182,   151,   136,   212,   132,   171,   177,   214,   132,
     132,   134,   208,   134,   212,   134,   134,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   177,   134,   174,     5,
     208,   132,   147,   101,   165,   132,   183,   185,   134,   175,
     139,   137,   159,   136,   208,   135,   175,   126,   160,   174,
     133,   166,   138,   120,   138,   116,   155,   156,   157,   158,
     162,   212,    26,    38,   136,   167,   175,   137,   134,   120,
     212,   208,   175,   139,   175,   158,   208,   133,   159,   139,
     180,   133,   159,   155,   180,   155,   134,   134
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
        case 2:
#line 228 "./parser.y"
    { write_proxies(yyvsp[0].ifref); write_client(yyvsp[0].ifref); write_server(yyvsp[0].ifref); }
    break;

  case 3:
#line 231 "./parser.y"
    { yyval.ifref = NULL; }
    break;

  case 4:
#line 232 "./parser.y"
    { yyval.ifref = yyvsp[-1].ifref; }
    break;

  case 5:
#line 233 "./parser.y"
    { yyval.ifref = make_ifref(yyvsp[0].type); LINK(yyval.ifref, yyvsp[-1].ifref); }
    break;

  case 6:
#line 234 "./parser.y"
    { yyval.ifref = yyvsp[-1].ifref; add_coclass(yyvsp[0].clas); }
    break;

  case 7:
#line 235 "./parser.y"
    { yyval.ifref = yyvsp[-1].ifref; add_module(yyvsp[0].type); }
    break;

  case 8:
#line 236 "./parser.y"
    { yyval.ifref = yyvsp[-1].ifref; }
    break;

  case 9:
#line 237 "./parser.y"
    { yyval.ifref = yyvsp[-1].ifref; }
    break;

  case 10:
#line 240 "./parser.y"
    {}
    break;

  case 11:
#line 241 "./parser.y"
    { if (!parse_only) add_interface(yyvsp[0].type); }
    break;

  case 12:
#line 242 "./parser.y"
    { if (!parse_only) add_interface(yyvsp[0].type); }
    break;

  case 13:
#line 243 "./parser.y"
    { if (!parse_only) add_coclass(yyvsp[0].clas); }
    break;

  case 14:
#line 244 "./parser.y"
    { if (!parse_only) add_module(yyvsp[0].type); }
    break;

  case 15:
#line 245 "./parser.y"
    {}
    break;

  case 16:
#line 248 "./parser.y"
    { yyval.func = NULL; }
    break;

  case 17:
#line 249 "./parser.y"
    { yyval.func = yyvsp[-1].func; LINK(yyval.func, yyvsp[-2].func); }
    break;

  case 18:
#line 250 "./parser.y"
    { yyval.func = yyvsp[-1].func; }
    break;

  case 19:
#line 253 "./parser.y"
    {}
    break;

  case 20:
#line 254 "./parser.y"
    { if (!parse_only && do_header) { write_constdef(yyvsp[-1].var); } }
    break;

  case 21:
#line 255 "./parser.y"
    {}
    break;

  case 22:
#line 256 "./parser.y"
    { if (!parse_only && do_header) { write_type(header, yyvsp[-1].type, NULL, NULL); fprintf(header, ";\n\n"); } }
    break;

  case 23:
#line 257 "./parser.y"
    { if (!parse_only && do_header) { write_externdef(yyvsp[-1].var); } }
    break;

  case 24:
#line 258 "./parser.y"
    {}
    break;

  case 25:
#line 259 "./parser.y"
    { if (!parse_only && do_header) { write_type(header, yyvsp[-1].type, NULL, NULL); fprintf(header, ";\n\n"); } }
    break;

  case 26:
#line 260 "./parser.y"
    {}
    break;

  case 27:
#line 261 "./parser.y"
    { if (!parse_only && do_header) { write_type(header, yyvsp[-1].type, NULL, NULL); fprintf(header, ";\n\n"); } }
    break;

  case 28:
#line 264 "./parser.y"
    { if (!parse_only && do_header) fprintf(header, "%s\n", yyvsp[-1].str); }
    break;

  case 29:
#line 266 "./parser.y"
    { assert(yychar == YYEMPTY);
						  if (!do_import(yyvsp[-1].str)) yychar = aEOF; }
    break;

  case 30:
#line 269 "./parser.y"
    {}
    break;

  case 31:
#line 272 "./parser.y"
    { yyval.str = yyvsp[0].str; }
    break;

  case 32:
#line 274 "./parser.y"
    { start_typelib(yyvsp[-1].str, yyvsp[-2].attr);
						  if (!parse_only && do_header) write_library(yyvsp[-1].str, yyvsp[-2].attr); }
    break;

  case 33:
#line 277 "./parser.y"
    { end_typelib(); }
    break;

  case 34:
#line 280 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 36:
#line 284 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 38:
#line 288 "./parser.y"
    { LINK(yyvsp[0].var, yyvsp[-2].var); yyval.var = yyvsp[0].var; }
    break;

  case 40:
#line 293 "./parser.y"
    { yyval.var = yyvsp[-1].var;
						  set_type(yyval.var, yyvsp[-2].tref, yyvsp[0].expr);
						  yyval.var->attrs = yyvsp[-3].attr;
						}
    break;

  case 41:
#line 297 "./parser.y"
    { yyval.var = yyvsp[-1].var;
						  set_type(yyval.var, yyvsp[-2].tref, yyvsp[0].expr);
						}
    break;

  case 42:
#line 300 "./parser.y"
    { yyval.var = yyvsp[-3].var;
						  yyval.var->ptr_level--;
						  set_type(yyval.var, yyvsp[-4].tref, NULL);
						  yyval.var->attrs = yyvsp[-5].attr;
						  yyval.var->args = yyvsp[-1].var;
						}
    break;

  case 43:
#line 306 "./parser.y"
    { yyval.var = yyvsp[-3].var;
						  yyval.var->ptr_level--;
						  set_type(yyval.var, yyvsp[-4].tref, NULL);
						  yyval.var->args = yyvsp[-1].var;
						}
    break;

  case 44:
#line 313 "./parser.y"
    { yyval.expr = NULL; }
    break;

  case 45:
#line 314 "./parser.y"
    { yyval.expr = yyvsp[-1].expr; }
    break;

  case 46:
#line 315 "./parser.y"
    { yyval.expr = make_expr(EXPR_VOID); }
    break;

  case 48:
#line 319 "./parser.y"
    { LINK(yyvsp[0].expr, yyvsp[-2].expr); yyval.expr = yyvsp[0].expr; }
    break;

  case 49:
#line 320 "./parser.y"
    { LINK(yyvsp[0].expr, yyvsp[-3].expr); yyval.expr = yyvsp[0].expr; }
    break;

  case 50:
#line 323 "./parser.y"
    { yyval.attr = NULL; }
    break;

  case 52:
#line 328 "./parser.y"
    { yyval.attr = yyvsp[-1].attr; }
    break;

  case 54:
#line 332 "./parser.y"
    { LINK(yyvsp[0].attr, yyvsp[-2].attr); yyval.attr = yyvsp[0].attr; }
    break;

  case 55:
#line 333 "./parser.y"
    { LINK(yyvsp[0].attr, yyvsp[-3].attr); yyval.attr = yyvsp[0].attr; }
    break;

  case 56:
#line 337 "./parser.y"
    { yyval.attr = make_attr(ATTR_ASYNC); }
    break;

  case 57:
#line 338 "./parser.y"
    { yyval.attr = make_attr(ATTR_AUTO_HANDLE); }
    break;

  case 58:
#line 339 "./parser.y"
    { yyval.attr = make_attrp(ATTR_CALLAS, yyvsp[-1].var); }
    break;

  case 59:
#line 340 "./parser.y"
    { yyval.attr = make_attrp(ATTR_CASE, yyvsp[-1].expr); }
    break;

  case 60:
#line 341 "./parser.y"
    { yyval.attr = make_attrv(ATTR_CONTEXTHANDLE, 0); }
    break;

  case 61:
#line 342 "./parser.y"
    { yyval.attr = make_attrv(ATTR_CONTEXTHANDLE, 0); /* RPC_CONTEXT_HANDLE_DONT_SERIALIZE */ }
    break;

  case 62:
#line 343 "./parser.y"
    { yyval.attr = make_attrv(ATTR_CONTEXTHANDLE, 0); /* RPC_CONTEXT_HANDLE_SERIALIZE */ }
    break;

  case 63:
#line 344 "./parser.y"
    { yyval.attr = make_attr(ATTR_CONTROL); }
    break;

  case 64:
#line 345 "./parser.y"
    { yyval.attr = make_attr(ATTR_DEFAULT); }
    break;

  case 65:
#line 346 "./parser.y"
    { yyval.attr = make_attrp(ATTR_DEFAULTVALUE_EXPR, yyvsp[-1].expr); }
    break;

  case 66:
#line 347 "./parser.y"
    { yyval.attr = make_attrp(ATTR_DEFAULTVALUE_STRING, yyvsp[-1].str); }
    break;

  case 67:
#line 348 "./parser.y"
    { yyval.attr = make_attrp(ATTR_DLLNAME, yyvsp[-1].str); }
    break;

  case 68:
#line 349 "./parser.y"
    { yyval.attr = make_attr(ATTR_DUAL); }
    break;

  case 69:
#line 350 "./parser.y"
    { yyval.attr = make_attrp(ATTR_ENDPOINT, yyvsp[-1].str); }
    break;

  case 70:
#line 351 "./parser.y"
    { yyval.attr = make_attrp(ATTR_ENTRY_STRING, yyvsp[-1].str); }
    break;

  case 71:
#line 352 "./parser.y"
    { yyval.attr = make_attrp(ATTR_ENTRY_ORDINAL, yyvsp[-1].expr); }
    break;

  case 72:
#line 353 "./parser.y"
    { yyval.attr = make_attr(ATTR_EXPLICIT_HANDLE); }
    break;

  case 73:
#line 354 "./parser.y"
    { yyval.attr = make_attr(ATTR_HANDLE); }
    break;

  case 74:
#line 355 "./parser.y"
    { yyval.attr = make_attrp(ATTR_HELPCONTEXT, yyvsp[-1].expr); }
    break;

  case 75:
#line 356 "./parser.y"
    { yyval.attr = make_attrp(ATTR_HELPFILE, yyvsp[-1].str); }
    break;

  case 76:
#line 357 "./parser.y"
    { yyval.attr = make_attrp(ATTR_HELPSTRING, yyvsp[-1].str); }
    break;

  case 77:
#line 358 "./parser.y"
    { yyval.attr = make_attrp(ATTR_HELPSTRINGCONTEXT, yyvsp[-1].expr); }
    break;

  case 78:
#line 359 "./parser.y"
    { yyval.attr = make_attrp(ATTR_HELPSTRINGDLL, yyvsp[-1].str); }
    break;

  case 79:
#line 360 "./parser.y"
    { yyval.attr = make_attr(ATTR_HIDDEN); }
    break;

  case 80:
#line 361 "./parser.y"
    { yyval.attr = make_attrp(ATTR_ID, yyvsp[-1].expr); }
    break;

  case 81:
#line 362 "./parser.y"
    { yyval.attr = make_attr(ATTR_IDEMPOTENT); }
    break;

  case 82:
#line 363 "./parser.y"
    { yyval.attr = make_attrp(ATTR_IIDIS, yyvsp[-1].var); }
    break;

  case 83:
#line 364 "./parser.y"
    { yyval.attr = make_attrp(ATTR_IMPLICIT_HANDLE, yyvsp[-1].str); }
    break;

  case 84:
#line 365 "./parser.y"
    { yyval.attr = make_attr(ATTR_IN); }
    break;

  case 85:
#line 366 "./parser.y"
    { yyval.attr = make_attr(ATTR_INPUTSYNC); }
    break;

  case 86:
#line 367 "./parser.y"
    { yyval.attr = make_attrp(ATTR_LENGTHIS, yyvsp[-1].expr); }
    break;

  case 87:
#line 368 "./parser.y"
    { yyval.attr = make_attr(ATTR_LOCAL); }
    break;

  case 88:
#line 369 "./parser.y"
    { yyval.attr = make_attr(ATTR_NONCREATABLE); }
    break;

  case 89:
#line 370 "./parser.y"
    { yyval.attr = make_attr(ATTR_OBJECT); }
    break;

  case 90:
#line 371 "./parser.y"
    { yyval.attr = make_attr(ATTR_ODL); }
    break;

  case 91:
#line 372 "./parser.y"
    { yyval.attr = make_attr(ATTR_OLEAUTOMATION); }
    break;

  case 92:
#line 373 "./parser.y"
    { yyval.attr = make_attr(ATTR_OPTIONAL); }
    break;

  case 93:
#line 374 "./parser.y"
    { yyval.attr = make_attr(ATTR_OUT); }
    break;

  case 94:
#line 375 "./parser.y"
    { yyval.attr = make_attrv(ATTR_POINTERDEFAULT, yyvsp[-1].num); }
    break;

  case 95:
#line 376 "./parser.y"
    { yyval.attr = make_attr(ATTR_PROPGET); }
    break;

  case 96:
#line 377 "./parser.y"
    { yyval.attr = make_attr(ATTR_PROPPUT); }
    break;

  case 97:
#line 378 "./parser.y"
    { yyval.attr = make_attr(ATTR_PROPPUTREF); }
    break;

  case 98:
#line 379 "./parser.y"
    { yyval.attr = make_attr(ATTR_PUBLIC); }
    break;

  case 99:
#line 380 "./parser.y"
    { yyval.attr = make_attr(ATTR_READONLY); }
    break;

  case 100:
#line 381 "./parser.y"
    { yyval.attr = make_attr(ATTR_RESTRICTED); }
    break;

  case 101:
#line 382 "./parser.y"
    { yyval.attr = make_attr(ATTR_RETVAL); }
    break;

  case 102:
#line 383 "./parser.y"
    { yyval.attr = make_attrp(ATTR_SIZEIS, yyvsp[-1].expr); }
    break;

  case 103:
#line 384 "./parser.y"
    { yyval.attr = make_attr(ATTR_SOURCE); }
    break;

  case 104:
#line 385 "./parser.y"
    { yyval.attr = make_attr(ATTR_STRING); }
    break;

  case 105:
#line 386 "./parser.y"
    { yyval.attr = make_attrp(ATTR_SWITCHIS, yyvsp[-1].expr); }
    break;

  case 106:
#line 387 "./parser.y"
    { yyval.attr = make_attrp(ATTR_SWITCHTYPE, type_ref(yyvsp[-1].tref)); }
    break;

  case 107:
#line 388 "./parser.y"
    { yyval.attr = make_attrp(ATTR_TRANSMITAS, type_ref(yyvsp[-1].tref)); }
    break;

  case 108:
#line 389 "./parser.y"
    { yyval.attr = make_attrp(ATTR_UUID, yyvsp[-1].uuid); }
    break;

  case 109:
#line 390 "./parser.y"
    { yyval.attr = make_attr(ATTR_V1ENUM); }
    break;

  case 110:
#line 391 "./parser.y"
    { yyval.attr = make_attr(ATTR_VARARG); }
    break;

  case 111:
#line 392 "./parser.y"
    { yyval.attr = make_attrv(ATTR_VERSION, yyvsp[-1].num); }
    break;

  case 112:
#line 393 "./parser.y"
    { yyval.attr = make_attrp(ATTR_WIREMARSHAL, type_ref(yyvsp[-1].tref)); }
    break;

  case 113:
#line 394 "./parser.y"
    { yyval.attr = make_attrv(ATTR_POINTERTYPE, yyvsp[0].num); }
    break;

  case 116:
#line 401 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 117:
#line 402 "./parser.y"
    { if (yyvsp[0].var) { LINK(yyvsp[0].var, yyvsp[-1].var); yyval.var = yyvsp[0].var; }
						  else { yyval.var = yyvsp[-1].var; }
						}
    break;

  case 118:
#line 407 "./parser.y"
    { attr_t *a = make_attrp(ATTR_CASE, yyvsp[-2].expr);
						  yyval.var = yyvsp[0].var; if (!yyval.var) yyval.var = make_var(NULL);
						  LINK(a, yyval.var->attrs); yyval.var->attrs = a;
						}
    break;

  case 119:
#line 411 "./parser.y"
    { attr_t *a = make_attr(ATTR_DEFAULT);
						  yyval.var = yyvsp[0].var; if (!yyval.var) yyval.var = make_var(NULL);
						  LINK(a, yyval.var->attrs); yyval.var->attrs = a;
						}
    break;

  case 120:
#line 417 "./parser.y"
    { yyval.var = reg_const(yyvsp[-2].var);
						  set_type(yyval.var, yyvsp[-3].tref, NULL);
						  yyval.var->eval = yyvsp[0].expr;
						  yyval.var->lval = yyvsp[0].expr->cval;
						}
    break;

  case 121:
#line 424 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 122:
#line 425 "./parser.y"
    { yyval.var = yyvsp[-1].var; }
    break;

  case 125:
#line 430 "./parser.y"
    { LINK(yyvsp[0].var, yyvsp[-2].var); yyval.var = yyvsp[0].var;
						  if (yyvsp[-2].var && !yyvsp[0].var->eval)
						    yyvsp[0].var->lval = yyvsp[-2].var->lval + 1;
						}
    break;

  case 126:
#line 436 "./parser.y"
    { yyval.var = reg_const(yyvsp[-2].var);
						  yyval.var->eval = yyvsp[0].expr;
						  yyval.var->lval = yyvsp[0].expr->cval;
                                                  yyval.var->type = make_type(RPC_FC_LONG, &std_int);
						}
    break;

  case 127:
#line 441 "./parser.y"
    { yyval.var = reg_const(yyvsp[0].var);
						  yyval.var->lval = 0; /* default for first enum entry */
                                                  yyval.var->type = make_type(RPC_FC_LONG, &std_int);
						}
    break;

  case 128:
#line 447 "./parser.y"
    { yyval.type = get_typev(RPC_FC_ENUM16, yyvsp[-3].var, tsENUM);
						  yyval.type->fields = yyvsp[-1].var;
						  yyval.type->defined = TRUE;
                                                  if(in_typelib)
                                                      add_enum(yyval.type);
						}
    break;

  case 130:
#line 456 "./parser.y"
    { LINK(yyvsp[0].expr, yyvsp[-2].expr); yyval.expr = yyvsp[0].expr; }
    break;

  case 131:
#line 461 "./parser.y"
    { yyval.expr = make_expr(EXPR_VOID); }
    break;

  case 133:
#line 465 "./parser.y"
    { yyval.expr = make_exprl(EXPR_NUM, yyvsp[0].num); }
    break;

  case 134:
#line 466 "./parser.y"
    { yyval.expr = make_exprl(EXPR_HEXNUM, yyvsp[0].num); }
    break;

  case 135:
#line 467 "./parser.y"
    { yyval.expr = make_exprs(EXPR_IDENTIFIER, yyvsp[0].str); }
    break;

  case 136:
#line 468 "./parser.y"
    { yyval.expr = make_expr3(EXPR_COND, yyvsp[-4].expr, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 137:
#line 469 "./parser.y"
    { yyval.expr = make_expr2(EXPR_OR , yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 138:
#line 470 "./parser.y"
    { yyval.expr = make_expr2(EXPR_AND, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 139:
#line 471 "./parser.y"
    { yyval.expr = make_expr2(EXPR_ADD, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 140:
#line 472 "./parser.y"
    { yyval.expr = make_expr2(EXPR_SUB, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 141:
#line 473 "./parser.y"
    { yyval.expr = make_expr2(EXPR_MUL, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 142:
#line 474 "./parser.y"
    { yyval.expr = make_expr2(EXPR_DIV, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 143:
#line 475 "./parser.y"
    { yyval.expr = make_expr2(EXPR_SHL, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 144:
#line 476 "./parser.y"
    { yyval.expr = make_expr2(EXPR_SHR, yyvsp[-2].expr, yyvsp[0].expr); }
    break;

  case 145:
#line 477 "./parser.y"
    { yyval.expr = make_expr1(EXPR_NOT, yyvsp[0].expr); }
    break;

  case 146:
#line 478 "./parser.y"
    { yyval.expr = make_expr1(EXPR_NEG, yyvsp[0].expr); }
    break;

  case 147:
#line 479 "./parser.y"
    { yyval.expr = make_expr1(EXPR_PPTR, yyvsp[0].expr); }
    break;

  case 148:
#line 480 "./parser.y"
    { yyval.expr = make_exprt(EXPR_CAST, yyvsp[-2].tref, yyvsp[0].expr); }
    break;

  case 149:
#line 481 "./parser.y"
    { yyval.expr = make_exprt(EXPR_SIZEOF, yyvsp[-1].tref, NULL); }
    break;

  case 150:
#line 482 "./parser.y"
    { yyval.expr = yyvsp[-1].expr; }
    break;

  case 152:
#line 486 "./parser.y"
    { LINK(yyvsp[0].expr, yyvsp[-2].expr); yyval.expr = yyvsp[0].expr; }
    break;

  case 153:
#line 489 "./parser.y"
    { yyval.expr = yyvsp[0].expr;
						  if (!yyval.expr->is_const)
						      yyerror("expression is not constant\n");
						}
    break;

  case 154:
#line 495 "./parser.y"
    { yyval.var = yyvsp[0].var;
						  set_type(yyval.var, yyvsp[-1].tref, NULL);
						}
    break;

  case 155:
#line 500 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 156:
#line 501 "./parser.y"
    { if (yyvsp[0].var) { LINK(yyvsp[0].var, yyvsp[-1].var); yyval.var = yyvsp[0].var; }
						  else { yyval.var = yyvsp[-1].var; }
						}
    break;

  case 157:
#line 506 "./parser.y"
    { yyval.var = yyvsp[-1].var; }
    break;

  case 158:
#line 507 "./parser.y"
    { yyval.var = make_var(NULL); yyval.var->type = yyvsp[-1].type; yyval.var->attrs = yyvsp[-2].attr; }
    break;

  case 159:
#line 508 "./parser.y"
    { yyval.var = make_var(NULL); yyval.var->attrs = yyvsp[-1].attr; }
    break;

  case 160:
#line 509 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 161:
#line 512 "./parser.y"
    { yyval.var = yyvsp[-1].var; set_type(yyval.var, yyvsp[-2].tref, yyvsp[0].expr); yyval.var->attrs = yyvsp[-3].attr; }
    break;

  case 162:
#line 517 "./parser.y"
    { set_type(yyvsp[-3].var, yyvsp[-5].tref, NULL);
						  yyvsp[-3].var->attrs = yyvsp[-6].attr;
						  yyval.func = make_func(yyvsp[-3].var, yyvsp[-1].var);
						  if (is_attr(yyvsp[-3].var->attrs, ATTR_IN)) {
						    yyerror("Inapplicable attribute");
						  }
						}
    break;

  case 163:
#line 526 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 165:
#line 530 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 166:
#line 531 "./parser.y"
    { yyval.var = make_var(yyvsp[0].str); }
    break;

  case 167:
#line 532 "./parser.y"
    { yyval.var = make_var(yyvsp[0].str); }
    break;

  case 168:
#line 535 "./parser.y"
    { yyval.var = make_var(yyvsp[0].str); }
    break;

  case 169:
#line 537 "./parser.y"
    { yyval.var = make_var(yyvsp[0].str); }
    break;

  case 170:
#line 538 "./parser.y"
    { yyval.var = make_var(yyvsp[0].str); }
    break;

  case 171:
#line 539 "./parser.y"
    { yyval.var = make_var(yyvsp[0].str); }
    break;

  case 172:
#line 540 "./parser.y"
    { yyval.var = make_var(yyvsp[0].str); }
    break;

  case 173:
#line 543 "./parser.y"
    { yyval.type = make_type(RPC_FC_BYTE, NULL); }
    break;

  case 174:
#line 544 "./parser.y"
    { yyval.type = make_type(RPC_FC_WCHAR, NULL); }
    break;

  case 176:
#line 546 "./parser.y"
    { yyval.type = yyvsp[0].type; yyval.type->sign = 1; }
    break;

  case 177:
#line 547 "./parser.y"
    { yyval.type = yyvsp[0].type; yyval.type->sign = -1;
						  switch (yyval.type->type) {
						  case RPC_FC_CHAR:  yyval.type->type = RPC_FC_BYTE; yyval.type->sign = 0; break;
						  case RPC_FC_SMALL: yyval.type->type = RPC_FC_USMALL; break;
						  case RPC_FC_SHORT: yyval.type->type = RPC_FC_USHORT; break;
						  case RPC_FC_LONG:  yyval.type->type = RPC_FC_ULONG;  break;
						  case RPC_FC_HYPER:
						    if (!yyval.type->ref) { yyval.type->ref = &std_uhyper; yyval.type->sign = 0; }
						    break;
						  default: break;
						  }
						}
    break;

  case 178:
#line 559 "./parser.y"
    { yyval.type = make_type(RPC_FC_ULONG, &std_int); yyval.type->sign = -1; }
    break;

  case 179:
#line 560 "./parser.y"
    { yyval.type = make_type(RPC_FC_FLOAT, NULL); }
    break;

  case 180:
#line 561 "./parser.y"
    { yyval.type = make_type(RPC_FC_FLOAT, NULL); }
    break;

  case 181:
#line 562 "./parser.y"
    { yyval.type = make_type(RPC_FC_DOUBLE, NULL); }
    break;

  case 182:
#line 563 "./parser.y"
    { yyval.type = make_type(RPC_FC_BYTE, &std_bool); /* ? */ }
    break;

  case 183:
#line 564 "./parser.y"
    { yyval.type = make_type(RPC_FC_ERROR_STATUS_T, NULL); }
    break;

  case 184:
#line 565 "./parser.y"
    { yyval.type = make_type(RPC_FC_BIND_PRIMITIVE, NULL); /* ? */ }
    break;

  case 187:
#line 572 "./parser.y"
    { yyval.type = make_type(RPC_FC_LONG, &std_int); }
    break;

  case 188:
#line 573 "./parser.y"
    { yyval.type = make_type(RPC_FC_SHORT, NULL); }
    break;

  case 189:
#line 574 "./parser.y"
    { yyval.type = make_type(RPC_FC_LONG, NULL); }
    break;

  case 190:
#line 575 "./parser.y"
    { yyval.type = make_type(RPC_FC_HYPER, NULL); }
    break;

  case 191:
#line 576 "./parser.y"
    { yyval.type = make_type(RPC_FC_HYPER, &std_int64); }
    break;

  case 192:
#line 577 "./parser.y"
    { yyval.type = make_type(RPC_FC_CHAR, NULL); }
    break;

  case 193:
#line 580 "./parser.y"
    { yyval.clas = make_class(yyvsp[0].str); }
    break;

  case 194:
#line 581 "./parser.y"
    { yyval.clas = make_class(yyvsp[0].str); }
    break;

  case 195:
#line 584 "./parser.y"
    { yyval.clas = yyvsp[0].clas;
						  yyval.clas->attrs = yyvsp[-1].attr;
                                                  if (!parse_only && do_header) write_coclass(yyval.clas);
						}
    break;

  case 196:
#line 590 "./parser.y"
    { yyval.clas = yyvsp[-3].clas;
						  yyval.clas->ifaces = yyvsp[-1].ifref;
						}
    break;

  case 197:
#line 595 "./parser.y"
    { yyval.ifref = NULL; }
    break;

  case 198:
#line 596 "./parser.y"
    { LINK(yyvsp[0].ifref, yyvsp[-1].ifref); yyval.ifref = yyvsp[0].ifref; }
    break;

  case 199:
#line 600 "./parser.y"
    { yyval.ifref = make_ifref(yyvsp[0].type); yyval.ifref->attrs = yyvsp[-1].attr; }
    break;

  case 200:
#line 603 "./parser.y"
    { yyval.type = get_type(0, yyvsp[0].str, 0); }
    break;

  case 201:
#line 604 "./parser.y"
    { yyval.type = get_type(0, yyvsp[0].str, 0); }
    break;

  case 202:
#line 607 "./parser.y"
    { yyval.type = yyvsp[0].type;
						  if (yyval.type->defined) yyerror("multiple definition error\n");
						  yyval.type->attrs = yyvsp[-1].attr;
						  yyval.type->attrs = make_attr(ATTR_DISPINTERFACE);
						  LINK(yyval.type->attrs, yyvsp[-1].attr);
						  yyval.type->ref = find_type("IDispatch", 0);
						  if (!yyval.type->ref) yyerror("IDispatch is undefined\n");
						  yyval.type->defined = TRUE;
						  if (!parse_only && do_header) write_forward(yyval.type);
						}
    break;

  case 203:
#line 619 "./parser.y"
    { yyval.var = NULL; }
    break;

  case 204:
#line 620 "./parser.y"
    { LINK(yyvsp[-1].var, yyvsp[-2].var); yyval.var = yyvsp[-1].var; }
    break;

  case 205:
#line 623 "./parser.y"
    { yyval.func = NULL; }
    break;

  case 206:
#line 624 "./parser.y"
    { LINK(yyvsp[-1].func, yyvsp[-2].func); yyval.func = yyvsp[-1].func; }
    break;

  case 207:
#line 630 "./parser.y"
    { yyval.type = yyvsp[-4].type;
						  yyval.type->fields = yyvsp[-2].var;
						  yyval.type->funcs = yyvsp[-1].func;
						  if (!parse_only && do_header) write_dispinterface(yyval.type);
						}
    break;

  case 208:
#line 638 "./parser.y"
    { yyval.type = NULL; }
    break;

  case 209:
#line 639 "./parser.y"
    { yyval.type = find_type2(yyvsp[0].str, 0); }
    break;

  case 210:
#line 642 "./parser.y"
    { yyval.type = get_type(RPC_FC_IP, yyvsp[0].str, 0); }
    break;

  case 211:
#line 643 "./parser.y"
    { yyval.type = get_type(RPC_FC_IP, yyvsp[0].str, 0); }
    break;

  case 212:
#line 646 "./parser.y"
    { yyval.type = yyvsp[0].type;
						  if (yyval.type->defined) yyerror("multiple definition error\n");
						  yyval.type->attrs = yyvsp[-1].attr;
						  yyval.type->defined = TRUE;
						  if (!parse_only && do_header) write_forward(yyval.type);
						}
    break;

  case 213:
#line 655 "./parser.y"
    { yyval.type = yyvsp[-4].type;
						  yyval.type->ref = yyvsp[-3].type;
						  yyval.type->funcs = yyvsp[-1].func;
						  if (!parse_only && do_header) write_interface(yyval.type);
						}
    break;

  case 214:
#line 662 "./parser.y"
    { yyval.type = yyvsp[-6].type;
						  yyval.type->ref = find_type2(yyvsp[-4].str, 0);
						  if (!yyval.type->ref) yyerror("base class %s not found in import\n", yyvsp[-4].str);
						  yyval.type->funcs = yyvsp[-1].func;
						  if (!parse_only && do_header) write_interface(yyval.type);
						}
    break;

  case 215:
#line 668 "./parser.y"
    { yyval.type = yyvsp[0].type; }
    break;

  case 216:
#line 672 "./parser.y"
    { yyval.type = yyvsp[-1].type; if (!parse_only && do_header) write_forward(yyval.type); }
    break;

  case 217:
#line 673 "./parser.y"
    { yyval.type = yyvsp[-1].type; if (!parse_only && do_header) write_forward(yyval.type); }
    break;

  case 218:
#line 676 "./parser.y"
    { yyval.type = make_type(0, NULL); yyval.type->name = yyvsp[0].str; }
    break;

  case 219:
#line 677 "./parser.y"
    { yyval.type = make_type(0, NULL); yyval.type->name = yyvsp[0].str; }
    break;

  case 220:
#line 680 "./parser.y"
    { yyval.type = yyvsp[0].type;
						  yyval.type->attrs = yyvsp[-1].attr;
						}
    break;

  case 221:
#line 685 "./parser.y"
    { yyval.type = yyvsp[-3].type;
						  yyval.type->funcs = yyvsp[-1].func;
						  /* FIXME: if (!parse_only && do_header) write_module($$); */
						}
    break;

  case 222:
#line 691 "./parser.y"
    { yyval.var = yyvsp[0].var; yyval.var->ptr_level++; }
    break;

  case 223:
#line 692 "./parser.y"
    { yyval.var = yyvsp[0].var; /* FIXME */ }
    break;

  case 226:
#line 697 "./parser.y"
    { yyval.var = yyvsp[-1].var; }
    break;

  case 228:
#line 702 "./parser.y"
    { LINK(yyvsp[0].var, yyvsp[-2].var); yyval.var = yyvsp[0].var; }
    break;

  case 229:
#line 706 "./parser.y"
    { yyval.num = RPC_FC_RP; }
    break;

  case 230:
#line 707 "./parser.y"
    { yyval.num = RPC_FC_UP; }
    break;

  case 231:
#line 710 "./parser.y"
    { yyval.type = get_typev(RPC_FC_STRUCT, yyvsp[-3].var, tsSTRUCT);
                                                  /* overwrite RPC_FC_STRUCT with a more exact type */
						  yyval.type->type = get_struct_type( yyvsp[-1].var );
						  yyval.type->fields = yyvsp[-1].var;
						  yyval.type->defined = TRUE;
                                                  if(in_typelib)
                                                      add_struct(yyval.type);
                                                }
    break;

  case 232:
#line 720 "./parser.y"
    { yyval.tref = make_tref(NULL, make_type(0, NULL)); }
    break;

  case 233:
#line 721 "./parser.y"
    { yyval.tref = make_tref(yyvsp[0].str, find_type(yyvsp[0].str, 0)); }
    break;

  case 234:
#line 722 "./parser.y"
    { yyval.tref = make_tref(NULL, yyvsp[0].type); }
    break;

  case 235:
#line 723 "./parser.y"
    { yyval.tref = uniq_tref(yyvsp[0].tref); yyval.tref->ref->is_const = TRUE; }
    break;

  case 236:
#line 724 "./parser.y"
    { yyval.tref = make_tref(NULL, yyvsp[0].type); }
    break;

  case 237:
#line 725 "./parser.y"
    { yyval.tref = make_tref(NULL, find_type2(yyvsp[0].str, tsENUM)); }
    break;

  case 238:
#line 726 "./parser.y"
    { yyval.tref = make_tref(NULL, yyvsp[0].type); }
    break;

  case 239:
#line 727 "./parser.y"
    { yyval.tref = make_tref(NULL, get_type(RPC_FC_STRUCT, yyvsp[0].str, tsSTRUCT)); }
    break;

  case 240:
#line 728 "./parser.y"
    { yyval.tref = make_tref(NULL, yyvsp[0].type); }
    break;

  case 241:
#line 729 "./parser.y"
    { yyval.tref = make_tref(NULL, find_type2(yyvsp[0].str, tsUNION)); }
    break;

  case 242:
#line 732 "./parser.y"
    { typeref_t *tref = uniq_tref(yyvsp[-1].tref); 
						  yyvsp[0].var->tname = tref->name;
						  tref->name = NULL;
						  yyval.type = type_ref(tref);
						  yyval.type->attrs = yyvsp[-2].attr;
						  if (!parse_only && do_header) write_typedef(yyval.type, yyvsp[0].var);
                                                  if (in_typelib && yyval.type->attrs)
                                                      add_typedef(yyval.type, yyvsp[0].var);
						  reg_types(yyval.type, yyvsp[0].var, 0);
						}
    break;

  case 243:
#line 744 "./parser.y"
    { yyval.type = get_typev(RPC_FC_NON_ENCAPSULATED_UNION, yyvsp[-3].var, tsUNION);
						  yyval.type->fields = yyvsp[-1].var;
						  yyval.type->defined = TRUE;
						}
    break;

  case 244:
#line 750 "./parser.y"
    { var_t *u = yyvsp[-3].var;
						  yyval.type = get_typev(RPC_FC_ENCAPSULATED_UNION, yyvsp[-8].var, tsUNION);
						  if (!u) u = make_var("tagged_union");
						  u->type = make_type(RPC_FC_NON_ENCAPSULATED_UNION, NULL);
						  u->type->fields = yyvsp[-1].var;
						  u->type->defined = TRUE;
						  LINK(u, yyvsp[-5].var); yyval.type->fields = u;
						  yyval.type->defined = TRUE;
						}
    break;

  case 245:
#line 762 "./parser.y"
    { yyval.num = MAKELONG(yyvsp[0].num, 0); }
    break;

  case 246:
#line 763 "./parser.y"
    { yyval.num = MAKELONG(yyvsp[-2].num, yyvsp[0].num); }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 3088 "y.tab.c"

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


#line 766 "./parser.y"


static attr_t *make_attr(enum attr_type type)
{
  attr_t *a = xmalloc(sizeof(attr_t));
  a->type = type;
  a->u.ival = 0;
  INIT_LINK(a);
  return a;
}

static attr_t *make_attrv(enum attr_type type, unsigned long val)
{
  attr_t *a = xmalloc(sizeof(attr_t));
  a->type = type;
  a->u.ival = val;
  INIT_LINK(a);
  return a;
}

static attr_t *make_attrp(enum attr_type type, void *val)
{
  attr_t *a = xmalloc(sizeof(attr_t));
  a->type = type;
  a->u.pval = val;
  INIT_LINK(a);
  return a;
}

static expr_t *make_expr(enum expr_type type)
{
  expr_t *e = xmalloc(sizeof(expr_t));
  e->type = type;
  e->ref = NULL;
  e->u.lval = 0;
  e->is_const = FALSE;
  INIT_LINK(e);
  return e;
}

static expr_t *make_exprl(enum expr_type type, long val)
{
  expr_t *e = xmalloc(sizeof(expr_t));
  e->type = type;
  e->ref = NULL;
  e->u.lval = val;
  e->is_const = FALSE;
  INIT_LINK(e);
  /* check for numeric constant */
  if (type == EXPR_NUM || type == EXPR_HEXNUM) {
    e->is_const = TRUE;
    e->cval = val;
  }
  return e;
}

static expr_t *make_exprs(enum expr_type type, char *val)
{
  expr_t *e;
  e = xmalloc(sizeof(expr_t));
  e->type = type;
  e->ref = NULL;
  e->u.sval = val;
  e->is_const = FALSE;
  INIT_LINK(e);
  /* check for predefined constants */
  if (type == EXPR_IDENTIFIER) {
    var_t *c = find_const(val, 0);
    if (c) {
      e->u.sval = c->name;
      free(val);
      e->is_const = TRUE;
      e->cval = c->lval;
    }
  }
  return e;
}

static expr_t *make_exprt(enum expr_type type, typeref_t *tref, expr_t *expr)
{
  expr_t *e;
  e = xmalloc(sizeof(expr_t));
  e->type = type;
  e->ref = expr;
  e->u.tref = tref;
  e->is_const = FALSE;
  INIT_LINK(e);
  /* check for cast of constant expression */
  if (type == EXPR_CAST && expr->is_const) {
    e->is_const = TRUE;
    e->cval = expr->cval;
  }
  return e;
}

static expr_t *make_expr1(enum expr_type type, expr_t *expr)
{
  expr_t *e;
  e = xmalloc(sizeof(expr_t));
  e->type = type;
  e->ref = expr;
  e->u.lval = 0;
  e->is_const = FALSE;
  INIT_LINK(e);
  /* check for compile-time optimization */
  if (expr->is_const) {
    e->is_const = TRUE;
    switch (type) {
    case EXPR_NEG:
      e->cval = -expr->cval;
      break;
    case EXPR_NOT:
      e->cval = ~expr->cval;
      break;
    default:
      e->is_const = FALSE;
      break;
    }
  }
  return e;
}

static expr_t *make_expr2(enum expr_type type, expr_t *expr1, expr_t *expr2)
{
  expr_t *e;
  e = xmalloc(sizeof(expr_t));
  e->type = type;
  e->ref = expr1;
  e->u.ext = expr2;
  e->is_const = FALSE;
  INIT_LINK(e);
  /* check for compile-time optimization */
  if (expr1->is_const && expr2->is_const) {
    e->is_const = TRUE;
    switch (type) {
    case EXPR_ADD:
      e->cval = expr1->cval + expr2->cval;
      break;
    case EXPR_SUB:
      e->cval = expr1->cval - expr2->cval;
      break;
    case EXPR_MUL:
      e->cval = expr1->cval * expr2->cval;
      break;
    case EXPR_DIV:
      e->cval = expr1->cval / expr2->cval;
      break;
    case EXPR_OR:
      e->cval = expr1->cval | expr2->cval;
      break;
    case EXPR_AND:
      e->cval = expr1->cval & expr2->cval;
      break;
    case EXPR_SHL:
      e->cval = expr1->cval << expr2->cval;
      break;
    case EXPR_SHR:
      e->cval = expr1->cval >> expr2->cval;
      break;
    default:
      e->is_const = FALSE;
      break;
    }
  }
  return e;
}

static expr_t *make_expr3(enum expr_type type, expr_t *expr1, expr_t *expr2, expr_t *expr3)
{
  expr_t *e;
  e = xmalloc(sizeof(expr_t));
  e->type = type;
  e->ref = expr1;
  e->u.ext = expr2;
  e->ext2 = expr3;
  e->is_const = FALSE;
  INIT_LINK(e);
  /* check for compile-time optimization */
  if (expr1->is_const && expr2->is_const && expr3->is_const) {
    e->is_const = TRUE;
    switch (type) {
    case EXPR_COND:
      e->cval = expr1->cval ? expr2->cval : expr3->cval;
      break;
    default:
      e->is_const = FALSE;
      break;
    }
  }
  return e;
}

static type_t *make_type(unsigned char type, type_t *ref)
{
  type_t *t = xmalloc(sizeof(type_t));
  t->name = NULL;
  t->type = type;
  t->ref = ref;
  t->rname = NULL;
  t->attrs = NULL;
  t->funcs = NULL;
  t->fields = NULL;
  t->ignore = parse_only;
  t->is_const = FALSE;
  t->sign = 0;
  t->defined = FALSE;
  t->written = FALSE;
  t->typelib_idx = -1;
  INIT_LINK(t);
  return t;
}

static typeref_t *make_tref(char *name, type_t *ref)
{
  typeref_t *t = xmalloc(sizeof(typeref_t));
  t->name = name;
  t->ref = ref;
  t->uniq = ref ? 0 : 1;
  return t;
}

static typeref_t *uniq_tref(typeref_t *ref)
{
  typeref_t *t = ref;
  type_t *tp;
  if (t->uniq) return t;
  tp = make_type(0, t->ref);
  tp->name = t->name;
  t->name = NULL;
  t->ref = tp;
  t->uniq = 1;
  return t;
}

static type_t *type_ref(typeref_t *ref)
{
  type_t *t = ref->ref;
  if (ref->name) free(ref->name);
  free(ref);
  return t;
}

static void set_type(var_t *v, typeref_t *ref, expr_t *arr)
{
  v->type = ref->ref;
  v->tname = ref->name;
  ref->name = NULL;
  free(ref);
  v->array = arr;
}

static ifref_t *make_ifref(type_t *iface)
{
  ifref_t *l = xmalloc(sizeof(ifref_t));
  l->iface = iface;
  l->attrs = NULL;
  INIT_LINK(l);
  return l;
}

static var_t *make_var(char *name)
{
  var_t *v = xmalloc(sizeof(var_t));
  v->name = name;
  v->ptr_level = 0;
  v->type = NULL;
  v->tname = NULL;
  v->attrs = NULL;
  v->array = NULL;
  v->eval = NULL;
  v->lval = 0;
  INIT_LINK(v);
  return v;
}

static func_t *make_func(var_t *def, var_t *args)
{
  func_t *f = xmalloc(sizeof(func_t));
  f->def = def;
  f->args = args;
  f->ignore = parse_only;
  f->idx = -1;
  INIT_LINK(f);
  return f;
}

static class_t *make_class(char *name)
{
  class_t *c = xmalloc(sizeof(class_t));
  c->name = name;
  c->attrs = NULL;
  c->ifaces = NULL;
  INIT_LINK(c);
  return c;
}

#define HASHMAX 64

static int hash_ident(const char *name)
{
  const char *p = name;
  int sum = 0;
  /* a simple sum hash is probably good enough */
  while (*p) {
    sum += *p;
    p++;
  }
  return sum & (HASHMAX-1);
}

/***** type repository *****/

struct rtype {
  char *name;
  type_t *type;
  int t;
  struct rtype *next;
};

struct rtype *type_hash[HASHMAX];

static type_t *reg_type(type_t *type, char *name, int t)
{
  struct rtype *nt;
  int hash;
  if (!name) {
    yyerror("registering named type without name\n");
    return type;
  }
  hash = hash_ident(name);
  nt = xmalloc(sizeof(struct rtype));
  nt->name = name;
  nt->type = type;
  nt->t = t;
  nt->next = type_hash[hash];
  type_hash[hash] = nt;
  return type;
}

/* determine pointer type from attrs */
static unsigned char get_pointer_type( type_t *type )
{
  int t;
  if (is_attr( type->attrs, ATTR_STRING ))
  {
    type_t *t = type;
    while( t->type == 0 && t->ref )
      t = t->ref;
    switch( t->type )
    {
    case RPC_FC_CHAR:
      return RPC_FC_C_CSTRING;
    case RPC_FC_WCHAR:
      return RPC_FC_C_WSTRING;
    }
  }
  t = get_attrv( type->attrs, ATTR_POINTERTYPE );
  if (t) return t;
  return RPC_FC_FP;
}

static type_t *reg_types(type_t *type, var_t *names, int t)
{
  type_t *ptr = type;
  int ptrc = 0;

  while (names) {
    var_t *next = NEXT_LINK(names);
    if (names->name) {
      type_t *cur = ptr;
      int cptr = names->ptr_level;
      if (cptr > ptrc) {
        while (cptr > ptrc) {
          int t = get_pointer_type( cur );
          cur = ptr = make_type(t, cur);
          ptrc++;
        }
      } else {
        while (cptr < ptrc) {
          cur = cur->ref;
          cptr++;
        }
      }
      reg_type(cur, names->name, t);
    }
    free(names);
    names = next;
  }
  return type;
}

static type_t *find_type(char *name, int t)
{
  struct rtype *cur = type_hash[hash_ident(name)];
  while (cur && (cur->t != t || strcmp(cur->name, name)))
    cur = cur->next;
  if (!cur) {
    yyerror("type %s not found\n", name);
    return NULL;
  }
  return cur->type;
}

static type_t *find_type2(char *name, int t)
{
  type_t *tp = find_type(name, t);
  free(name);
  return tp;
}

int is_type(const char *name)
{
  struct rtype *cur = type_hash[hash_ident(name)];
  while (cur && (cur->t || strcmp(cur->name, name)))
    cur = cur->next;
  if (cur) return TRUE;
  return FALSE;
}

static type_t *get_type(unsigned char type, char *name, int t)
{
  struct rtype *cur = NULL;
  type_t *tp;
  if (name) {
    cur = type_hash[hash_ident(name)];
    while (cur && (cur->t != t || strcmp(cur->name, name)))
      cur = cur->next;
  }
  if (cur) {
    free(name);
    return cur->type;
  }
  tp = make_type(type, NULL);
  tp->name = name;
  if (!name) return tp;
  return reg_type(tp, name, t);
}

static type_t *get_typev(unsigned char type, var_t *name, int t)
{
  char *sname = NULL;
  if (name) {
    sname = name->name;
    free(name);
  }
  return get_type(type, sname, t);
}

static int get_struct_type(var_t *field)
{
  int has_pointer = 0;
  int has_conformant_array = 0;
  int has_conformant_string = 0;

  while (field)
  {
    type_t *t = field->type;

    /* get the base type */
    while( (t->type == 0) && t->ref )
      t = t->ref;

    switch (t->type)
    {
    /*
     * RPC_FC_BYTE, RPC_FC_STRUCT, etc
     *  Simple types don't effect the type of struct.
     *  A struct containing a simple struct is still a simple struct.
     *  So long as we can block copy the data, we return RPC_FC_STRUCT.
     */
    case 0: /* void pointer */
    case RPC_FC_BYTE:
    case RPC_FC_CHAR:
    case RPC_FC_SMALL:
    case RPC_FC_USMALL:
    case RPC_FC_WCHAR:
    case RPC_FC_SHORT:
    case RPC_FC_USHORT:
    case RPC_FC_LONG:
    case RPC_FC_ULONG:
    case RPC_FC_INT3264:
    case RPC_FC_UINT3264:
    case RPC_FC_HYPER:
    case RPC_FC_FLOAT:
    case RPC_FC_DOUBLE:
    case RPC_FC_STRUCT:
    case RPC_FC_ENUM16:
    case RPC_FC_ENUM32:
      break;

    case RPC_FC_UP:
    case RPC_FC_FP:
      has_pointer = 1;
      break;
    case RPC_FC_CARRAY:
      has_conformant_array = 1;
      break;
    case RPC_FC_C_CSTRING:
    case RPC_FC_C_WSTRING:
      has_conformant_string = 1;
      break;

    /*
     * Propagate member attributes
     *  a struct should be at least as complex as its member
     */
    case RPC_FC_CVSTRUCT:
      has_conformant_string = 1;
      has_pointer = 1;
      break;

    case RPC_FC_CPSTRUCT:
      has_conformant_array = 1;
      has_pointer = 1;
      break;

    case RPC_FC_CSTRUCT:
      has_conformant_array = 1;
      break;

    case RPC_FC_PSTRUCT:
      has_pointer = 1;
      break;

    default:
      fprintf(stderr,"Unknown struct member %s with type (0x%02x)\n",
              field->name, t->type);
      /* fallthru - treat it as complex */

    /* as soon as we see one of these these members, it's bogus... */
    case RPC_FC_IP:
    case RPC_FC_ENCAPSULATED_UNION:
    case RPC_FC_NON_ENCAPSULATED_UNION:
    case RPC_FC_TRANSMIT_AS:
    case RPC_FC_REPRESENT_AS:
    case RPC_FC_PAD:
    case RPC_FC_EMBEDDED_COMPLEX:
    case RPC_FC_BOGUS_STRUCT:
      return RPC_FC_BOGUS_STRUCT;
    }
    field = NEXT_LINK(field);
  }

  if( has_conformant_string && has_pointer )
    return RPC_FC_CVSTRUCT;
  if( has_conformant_array && has_pointer )
    return RPC_FC_CPSTRUCT;
  if( has_conformant_array )
    return RPC_FC_CSTRUCT;
  if( has_pointer )
    return RPC_FC_PSTRUCT;
  return RPC_FC_STRUCT;
}

/***** constant repository *****/

struct rconst {
  char *name;
  var_t *var;
  struct rconst *next;
};

struct rconst *const_hash[HASHMAX];

static var_t *reg_const(var_t *var)
{
  struct rconst *nc;
  int hash;
  if (!var->name) {
    yyerror("registering constant without name\n");
    return var;
  }
  hash = hash_ident(var->name);
  nc = xmalloc(sizeof(struct rconst));
  nc->name = var->name;
  nc->var = var;
  nc->next = const_hash[hash];
  const_hash[hash] = nc;
  return var;
}

static var_t *find_const(char *name, int f)
{
  struct rconst *cur = const_hash[hash_ident(name)];
  while (cur && strcmp(cur->name, name))
    cur = cur->next;
  if (!cur) {
    if (f) yyerror("constant %s not found\n", name);
    return NULL;
  }
  return cur->var;
}

