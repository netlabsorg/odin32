
/*  A Bison parser, made from parser.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	tTYPEDEF	258
#define	tEXTERN	259
#define	tSTRUCT	260
#define	tENUM	261
#define	tCPPCLASS	262
#define	tINLINE	263
#define	tSTATIC	264
#define	tNL	265
#define	tNUMBER	266
#define	tLNUMBER	267
#define	tSTRING	268
#define	tIDENT	269
#define	tFILENAME	270
#define	tRAWDATA	271
#define	tACCELERATORS	272
#define	tBITMAP	273
#define	tCURSOR	274
#define	tDIALOG	275
#define	tDIALOGEX	276
#define	tMENU	277
#define	tMENUEX	278
#define	tMESSAGETABLE	279
#define	tRCDATA	280
#define	tVERSIONINFO	281
#define	tSTRINGTABLE	282
#define	tFONT	283
#define	tFONTDIR	284
#define	tICON	285
#define	tAUTO3STATE	286
#define	tAUTOCHECKBOX	287
#define	tAUTORADIOBUTTON	288
#define	tCHECKBOX	289
#define	tDEFPUSHBUTTON	290
#define	tPUSHBUTTON	291
#define	tRADIOBUTTON	292
#define	tSTATE3	293
#define	tGROUPBOX	294
#define	tCOMBOBOX	295
#define	tLISTBOX	296
#define	tSCROLLBAR	297
#define	tCONTROL	298
#define	tEDITTEXT	299
#define	tRTEXT	300
#define	tCTEXT	301
#define	tLTEXT	302
#define	tBLOCK	303
#define	tVALUE	304
#define	tSHIFT	305
#define	tALT	306
#define	tASCII	307
#define	tVIRTKEY	308
#define	tGRAYED	309
#define	tCHECKED	310
#define	tINACTIVE	311
#define	tNOINVERT	312
#define	tPURE	313
#define	tIMPURE	314
#define	tDISCARDABLE	315
#define	tLOADONCALL	316
#define	tPRELOAD	317
#define	tFIXED	318
#define	tMOVEABLE	319
#define	tCLASS	320
#define	tCAPTION	321
#define	tCHARACTERISTICS	322
#define	tEXSTYLE	323
#define	tSTYLE	324
#define	tVERSION	325
#define	tLANGUAGE	326
#define	tFILEVERSION	327
#define	tPRODUCTVERSION	328
#define	tFILEFLAGSMASK	329
#define	tFILEOS	330
#define	tFILETYPE	331
#define	tFILEFLAGS	332
#define	tFILESUBTYPE	333
#define	tMENUBARBREAK	334
#define	tMENUBREAK	335
#define	tMENUITEM	336
#define	tPOPUP	337
#define	tSEPARATOR	338
#define	tHELP	339
#define	tTOOLBAR	340
#define	tBUTTON	341
#define	tBEGIN	342
#define	tEND	343
#define	tDLGINIT	344
#define	tNOT	345
#define	pUPM	346

#line 1 "parser.y"

/*
 * Copyright 1994	Martin von Loewis
 * Copyright 1998-2000	Bertho A. Stultiens (BS)
 *           1999	Juergen Schmied (JS)
 *
 * 24-Jul-2000 BS	- Made a fix for broken Berkeley yacc on
 *			  non-terminals (see cjunk rule).
 * 21-May-2000 BS	- Partial implementation of font resources.
 *			- Corrected language propagation for binary
 *			  resources such as bitmaps, isons, cursors,
 *			  userres and rcdata. The language is now
 *			  correct in .res files.
 *			- Fixed reading the resource name as ident,
 *			  so that it may overlap keywords.
 * 20-May-2000 BS	- Implemented animated cursors and icons
 *			  resource types.
 * 30-Apr-2000 BS	- Reintegration into the wine-tree
 * 14-Jan-2000 BS	- Redid the usertype resources so that they
 *			  are compatible.
 * 02-Jan-2000 BS	- Removed the preprocessor from the grammar
 *			  expect for the # command (line numbers).
 *
 * 06-Nov-1999 JS	- see CHANGES
 * 
 * 29-Dec-1998 AdH	- Grammar and function extensions.
 *			     grammar: TOOLBAR resources, Named ICONs in 
 *				DIALOGS
 *			     functions: semantic actions for the grammar 
 *				changes, resource files can now be anywhere
 *				on the include path instead of just in the
 *				current directory
 *
 * 20-Jun-1998 BS	- Fixed a bug in load_file() where the name was not
 *			  printed out correctly.
 *
 * 17-Jun-1998 BS	- Fixed a bug in CLASS statement parsing which should
 *			  also accept a tSTRING as argument.
 *
 * 25-May-1998 BS	- Found out that I need to support language, version
 *			  and characteristics in inline resources (bitmap,
 *			  cursor, etc) but they can also be specified with
 *			  a filename. This renders my filename-scanning scheme
 *			  worthless. Need to build newline parsing to solve
 *			  this one.
 *			  It will come with version 1.1.0 (sigh).
 *
 * 19-May-1998 BS	- Started to build a builtin preprocessor
 *
 * 30-Apr-1998 BS	- Redid the stringtable parsing/handling. My previous
 *			  ideas had some serious flaws.
 *
 * 27-Apr-1998 BS	- Removed a lot of dead comments and put it in a doc
 *			  file.
 *
 * 21-Apr-1998 BS	- Added correct behavior for cursors and icons.
 *			- This file is growing too big. It is time to strip
 *			  things and put it in a support file.
 *
 * 19-Apr-1998 BS	- Tagged the stringtable resource so that only one
 *			  resource will be created. This because the table
 *			  has a different layout than other resources. The
 *			  table has to be sorted, and divided into smaller
 *			  resource entries (see comment in source).
 *
 * 17-Apr-1998 BS	- Almost all strings, including identifiers, are parsed
 *			  as string_t which include unicode strings upon
 *			  input.
 *			- Parser now emits a warning when compiling win32
 *			  extensions in win16 mode.
 *
 * 16-Apr-1998 BS	- Raw data elements are now *optionally* seperated
 *			  by commas. Read the comments in file sq2dq.l.
 *			- FIXME: there are instances in the source that rely
 *			  on the fact that int==32bit and pointers are int size.
 *			- Fixed the conflict in menuex by changing a rule
 *			  back into right recursion. See note in source.
 *			- UserType resources cannot have an expression as its
 *			  typeclass. See note in source.
 *
 * 15-Apr-1998 BS	- Changed all right recursion into left recursion to
 *			  get reduction of the parsestack.
 *			  This also helps communication between bison and flex.
 *			  Main advantage is that the Empty rule gets reduced
 *			  first, which is used to allocate/link things.
 *			  It also added a shift/reduce conflict in the menuex
 *			  handling, due to expression/option possibility,
 *			  although not serious.
 *
 * 14-Apr-1998 BS	- Redone almost the entire parser. We're not talking
 *			  about making it more efficient, but readable (for me)
 *			  and slightly easier to expand/change.
 *			  This is done primarily by using more reduce states
 *			  with many (intuitive) types for the various resource
 *			  statements.
 *			- Added expression handling for all resources where a
 *			  number is accepted (not only for win32). Also added
 *			  multiply and division (not MS compatible, but handy).
 *			  Unary minus introduced a shift/reduce conflict, but
 *			  it is not serious.
 *
 * 13-Apr-1998 BS	- Reordered a lot of things
 *			- Made the source more readable
 *			- Added Win32 resource definitions
 *			- Corrected syntax problems with an old yacc (;)
 *			- Added extra comment about grammar
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

#include "wrc.h"
#include "utils.h"
#include "newstruc.h"
#include "dumpres.h"
#include "preproc.h"
#include "parser.h"
#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"

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

int want_nl = 0;	/* Signal flex that we need the next newline */
int want_id = 0;	/* Signal flex that we need the next identifier */
stringtable_t *tagstt;	/* Stringtable tag.
			 * It is set while parsing a stringtable to one of
			 * the stringtables in the sttres list or a new one
			 * if the language was not parsed before.
			 */
stringtable_t *sttres;	/* Stringtable resources. This holds the list of
			 * stringtables with different lanuages
			 */
static int dont_want_id = 0;	/* See language parsing for details */

/* Set to the current options of the currently scanning stringtable */
static int *tagstt_memopt;
static characts_t *tagstt_characts;
static version_t *tagstt_version;

static const char riff[4] = "RIFF";	/* RIFF file magic for animated cursor/icon */

/* Prototypes of here defined functions */
static event_t *get_event_head(event_t *p);
static control_t *get_control_head(control_t *p);
static ver_value_t *get_ver_value_head(ver_value_t *p);
static ver_block_t *get_ver_block_head(ver_block_t *p);
static resource_t *get_resource_head(resource_t *p);
static menuex_item_t *get_itemex_head(menuex_item_t *p);
static menu_item_t *get_item_head(menu_item_t *p);
static raw_data_t *merge_raw_data_str(raw_data_t *r1, string_t *str);
static raw_data_t *merge_raw_data_int(raw_data_t *r1, int i);
static raw_data_t *merge_raw_data_long(raw_data_t *r1, int i);
static raw_data_t *merge_raw_data(raw_data_t *r1, raw_data_t *r2);
static raw_data_t *str2raw_data(string_t *str);
static raw_data_t *int2raw_data(int i);
static raw_data_t *long2raw_data(int i);
static raw_data_t *load_file(string_t *name);
static itemex_opt_t *new_itemex_opt(int id, int type, int state, int helpid);
static event_t *add_string_event(string_t *key, int id, int flags, event_t *prev);
static event_t *add_event(int key, int id, int flags, event_t *prev);
static dialogex_t *dialogex_version(version_t *v, dialogex_t *dlg);
static dialogex_t *dialogex_characteristics(characts_t *c, dialogex_t *dlg);
static dialogex_t *dialogex_language(language_t *l, dialogex_t *dlg);
static dialogex_t *dialogex_menu(name_id_t *m, dialogex_t *dlg);
static dialogex_t *dialogex_class(name_id_t *n, dialogex_t *dlg);
static dialogex_t *dialogex_font(font_id_t *f, dialogex_t *dlg);
static dialogex_t *dialogex_caption(string_t *s, dialogex_t *dlg);
static dialogex_t *dialogex_exstyle(style_t *st, dialogex_t *dlg);
static dialogex_t *dialogex_style(style_t *st, dialogex_t *dlg);
static name_id_t *convert_ctlclass(name_id_t *cls);
static control_t *ins_ctrl(int type, int special_style, control_t *ctrl, control_t *prev);
static dialog_t *dialog_version(version_t *v, dialog_t *dlg);
static dialog_t *dialog_characteristics(characts_t *c, dialog_t *dlg);
static dialog_t *dialog_language(language_t *l, dialog_t *dlg);
static dialog_t *dialog_menu(name_id_t *m, dialog_t *dlg);
static dialog_t *dialog_class(name_id_t *n, dialog_t *dlg);
static dialog_t *dialog_font(font_id_t *f, dialog_t *dlg);
static dialog_t *dialog_caption(string_t *s, dialog_t *dlg);
static dialog_t *dialog_exstyle(style_t * st, dialog_t *dlg);
static dialog_t *dialog_style(style_t * st, dialog_t *dlg);
static resource_t *build_stt_resources(stringtable_t *stthead);
static stringtable_t *find_stringtable(lvc_t *lvc);
static toolbar_item_t *ins_tlbr_button(toolbar_item_t *prev, toolbar_item_t *idrec);
static toolbar_item_t *get_tlbr_buttons_head(toolbar_item_t *p, int *nitems);
static string_t *make_filename(string_t *s);
static resource_t *build_fontdirs(resource_t *tail);
static resource_t *build_fontdir(resource_t **fnt, int nfnt);
static int rsrcid_to_token(int lookahead);


#line 224 "parser.y"
typedef union{
	string_t	*str;
	int		num;
	int		*iptr;
	char		*cptr;
	resource_t	*res;
	accelerator_t	*acc;
	bitmap_t	*bmp;
	dialog_t	*dlg;
	dialogex_t	*dlgex;
	font_t		*fnt;
	fontdir_t	*fnd;
	menu_t		*men;
	menuex_t	*menex;
	rcdata_t	*rdt;
	stringtable_t	*stt;
	stt_entry_t	*stte;
	user_t		*usr;
	messagetable_t	*msg;
	versioninfo_t	*veri;
	control_t	*ctl;
	name_id_t	*nid;
	font_id_t	*fntid;
	language_t	*lan;
	version_t	*ver;
	characts_t	*chars;
	event_t		*event;
	menu_item_t	*menitm;
	menuex_item_t	*menexitm;
	itemex_opt_t	*exopt;
	raw_data_t	*raw;
	lvc_t		*lvc;
	ver_value_t	*val;
	ver_block_t	*blk;
	ver_words_t	*verw;
	toolbar_t	*tlbar;
	toolbar_item_t	*tlbarItems;
	dlginit_t       *dginit;
	style_pair_t	*styles;
	style_t		*style;
	ani_any_t	*ani;
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



#define	YYFINAL		577
#define	YYFLAG		-32768
#define	YYNTBASE	103

#define YYTRANSLATE(x) ((unsigned)(x) <= 346 ? yytranslate[x] : 186)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    92,     2,   100,
   102,    95,    93,   101,    94,     2,    96,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    91,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    90,     2,    97,     2,     2,     2,     2,
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
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    89,    98,    99
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     3,     6,     9,    11,    13,    15,    17,    19,
    21,    23,    26,    28,    32,    36,    41,    43,    44,    50,
    51,    53,    55,    57,    59,    61,    63,    65,    67,    69,
    71,    73,    75,    77,    79,    81,    83,    85,    87,    89,
    91,    93,    95,    97,   101,   105,   109,   113,   117,   121,
   125,   129,   133,   135,   137,   144,   145,   151,   157,   158,
   161,   163,   167,   169,   171,   173,   175,   177,   179,   193,
   194,   198,   202,   206,   209,   213,   217,   221,   224,   227,
   230,   231,   235,   239,   243,   247,   251,   255,   259,   263,
   267,   271,   275,   279,   283,   287,   291,   295,   299,   310,
   323,   334,   335,   340,   347,   356,   374,   390,   395,   396,
   399,   400,   403,   408,   412,   416,   418,   421,   423,   425,
   440,   441,   445,   449,   453,   456,   459,   463,   467,   471,
   474,   477,   480,   481,   485,   489,   493,   497,   501,   505,
   509,   513,   517,   521,   525,   529,   533,   537,   541,   545,
   549,   560,   580,   597,   611,   623,   624,   626,   627,   630,
   640,   641,   644,   649,   653,   654,   661,   665,   671,   672,
   676,   680,   684,   688,   692,   696,   701,   705,   706,   711,
   715,   721,   722,   725,   731,   738,   739,   742,   747,   754,
   763,   768,   772,   773,   778,   779,   781,   788,   789,   799,
   809,   813,   817,   821,   825,   829,   830,   833,   839,   840,
   843,   845,   850,   855,   857,   861,   871,   872,   876,   879,
   880,   883,   886,   888,   890,   892,   894,   896,   898,   900,
   901,   904,   907,   910,   915,   918,   921,   926,   928,   930,
   932,   934,   938,   942,   946,   950,   952,   954,   955,   957,
   959,   963,   967,   971,   975,   979,   983,   987,   990,   993,
   996,  1000,  1002,  1005,  1007
};

static const short yyrhs[] = {   104,
     0,     0,   104,   106,     0,   104,   105,     0,     3,     0,
     5,     0,     4,     0,     6,     0,     7,     0,     9,     0,
     8,     0,    14,    95,     0,    10,     0,   183,   108,   111,
     0,    14,   108,   111,     0,    14,   108,    14,   100,     0,
   159,     0,     0,    71,   107,   183,   101,   183,     0,     0,
   183,     0,    14,     0,   109,     0,    13,     0,   123,     0,
   113,     0,   114,     0,   128,     0,   140,     0,   120,     0,
   116,     0,   117,     0,   115,     0,   150,     0,   154,     0,
   118,     0,   119,     0,   170,     0,   121,     0,   163,     0,
    15,     0,    14,     0,    13,     0,    18,   172,   181,     0,
    19,   172,   181,     0,    30,   172,   181,     0,    28,   172,
   181,     0,    29,   172,   181,     0,    24,   172,   181,     0,
    25,   172,   181,     0,    89,   172,   181,     0,   122,   172,
   181,     0,    11,     0,    14,     0,    17,   172,   175,    87,
   124,    88,     0,     0,   124,    13,   101,   183,   125,     0,
   124,   183,   101,   183,   125,     0,     0,   101,   126,     0,
   127,     0,   126,   101,   127,     0,    57,     0,    50,     0,
    43,     0,    51,     0,    52,     0,    53,     0,    20,   172,
   183,   101,   183,   101,   183,   101,   183,   129,    87,   130,
    88,     0,     0,   129,    69,   138,     0,   129,    68,   138,
     0,   129,    66,    13,     0,   129,   135,     0,   129,    65,
   110,     0,   129,     7,   110,     0,   129,    22,   109,     0,
   129,   176,     0,   129,   177,     0,   129,   178,     0,     0,
   130,    43,   134,     0,   130,    44,   132,     0,   130,    41,
   132,     0,   130,    40,   132,     0,   130,    42,   132,     0,
   130,    34,   131,     0,   130,    35,   131,     0,   130,    39,
   131,     0,   130,    36,   131,     0,   130,    37,   131,     0,
   130,    31,   131,     0,   130,    38,   131,     0,   130,    32,
   131,     0,   130,    33,   131,     0,   130,    47,   131,     0,
   130,    46,   131,     0,   130,    45,   131,     0,   130,    30,
   110,   162,   183,   101,   183,   101,   183,   133,     0,    13,
   162,   183,   101,   183,   101,   183,   101,   183,   101,   183,
   136,     0,   183,   101,   183,   101,   183,   101,   183,   101,
   183,   136,     0,     0,   101,   183,   101,   183,     0,   101,
   183,   101,   183,   101,   138,     0,   101,   183,   101,   183,
   101,   138,   101,   138,     0,   110,   162,   183,   101,   139,
   101,   138,   101,   183,   101,   183,   101,   183,   101,   183,
   101,   138,     0,   110,   162,   183,   101,   139,   101,   138,
   101,   183,   101,   183,   101,   183,   101,   183,     0,    28,
   183,   101,    13,     0,     0,   101,   138,     0,     0,   101,
   138,     0,   101,   138,   101,   138,     0,   138,    90,   138,
     0,   100,   138,   102,     0,   185,     0,    98,   185,     0,
   183,     0,    13,     0,    21,   172,   183,   101,   183,   101,
   183,   101,   183,   147,   141,    87,   142,    88,     0,     0,
   141,    69,   138,     0,   141,    68,   138,     0,   141,    66,
    13,     0,   141,   135,     0,   141,   148,     0,   141,    65,
   110,     0,   141,     7,   110,     0,   141,    22,   109,     0,
   141,   176,     0,   141,   177,     0,   141,   178,     0,     0,
   142,    43,   143,     0,   142,    44,   145,     0,   142,    41,
   145,     0,   142,    40,   145,     0,   142,    42,   145,     0,
   142,    34,   144,     0,   142,    35,   144,     0,   142,    39,
   144,     0,   142,    36,   144,     0,   142,    37,   144,     0,
   142,    31,   144,     0,   142,    38,   144,     0,   142,    32,
   144,     0,   142,    33,   144,     0,   142,    47,   144,     0,
   142,    46,   144,     0,   142,    45,   144,     0,   142,    30,
   110,   162,   183,   101,   183,   101,   183,   133,     0,   110,
   162,   183,   101,   139,   101,   138,   101,   183,   101,   183,
   101,   183,   101,   183,   101,   138,   147,   146,     0,   110,
   162,   183,   101,   139,   101,   138,   101,   183,   101,   183,
   101,   183,   101,   183,   146,     0,    13,   162,   183,   101,
   183,   101,   183,   101,   183,   101,   183,   137,   146,     0,
   183,   101,   183,   101,   183,   101,   183,   101,   183,   137,
   146,     0,     0,   179,     0,     0,   101,   183,     0,    28,
   183,   101,    13,   101,   183,   101,   183,   149,     0,     0,
   101,   183,     0,    22,   172,   175,   151,     0,    87,   152,
    88,     0,     0,   152,    81,    13,   162,   183,   153,     0,
   152,    81,    83,     0,   152,    82,    13,   153,   151,     0,
     0,   101,    55,   153,     0,   101,    54,   153,     0,   101,
    84,   153,     0,   101,    56,   153,     0,   101,    79,   153,
     0,   101,    80,   153,     0,    23,   172,   175,   155,     0,
    87,   156,    88,     0,     0,   156,    81,    13,   157,     0,
   156,    81,    83,     0,   156,    82,    13,   158,   155,     0,
     0,   101,   183,     0,   101,   182,   101,   182,   153,     0,
   101,   182,   101,   182,   101,   183,     0,     0,   101,   183,
     0,   101,   182,   101,   183,     0,   101,   182,   101,   182,
   101,   183,     0,   101,   182,   101,   182,   101,   182,   101,
   183,     0,   160,    87,   161,    88,     0,    27,   172,   175,
     0,     0,   161,   183,   162,    13,     0,     0,   101,     0,
    26,   172,   164,    87,   165,    88,     0,     0,   164,    72,
   183,   101,   183,   101,   183,   101,   183,     0,   164,    73,
   183,   101,   183,   101,   183,   101,   183,     0,   164,    77,
   183,     0,   164,    74,   183,     0,   164,    75,   183,     0,
   164,    76,   183,     0,   164,    78,   183,     0,     0,   165,
   166,     0,    48,    13,    87,   167,    88,     0,     0,   167,
   168,     0,   166,     0,    49,    13,   101,    13,     0,    49,
    13,   101,   169,     0,   183,     0,   169,   101,   183,     0,
    85,   172,   183,   101,   183,   175,    87,   171,    88,     0,
     0,   171,    86,   183,     0,   171,    83,     0,     0,   172,
   173,     0,   172,   174,     0,    62,     0,    64,     0,    60,
     0,    58,     0,    61,     0,    63,     0,    59,     0,     0,
   175,   176,     0,   175,   177,     0,   175,   178,     0,    71,
   183,   101,   183,     0,    67,   183,     0,    70,   183,     0,
   175,    87,   180,    88,     0,    16,     0,    11,     0,    12,
     0,    13,     0,   180,   162,    16,     0,   180,   162,    11,
     0,   180,   162,    12,     0,   180,   162,    13,     0,   112,
     0,   179,     0,     0,   183,     0,   184,     0,   184,    93,
   184,     0,   184,    94,   184,     0,   184,    90,   184,     0,
   184,    92,   184,     0,   184,    95,   184,     0,   184,    96,
   184,     0,   184,    91,   184,     0,    97,   184,     0,    94,
   184,     0,    93,   184,     0,   100,   184,   102,     0,   185,
     0,    98,   185,     0,    11,     0,    12,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   346,   380,   381,   426,   431,   432,   433,   434,   435,   436,
   437,   441,   442,   471,   483,   493,   514,   522,   522,   564,
   570,   577,   587,   588,   597,   598,   599,   623,   624,   630,
   631,   632,   633,   657,   658,   664,   665,   666,   667,   668,
   672,   673,   674,   678,   682,   698,   720,   730,   738,   746,
   750,   754,   765,   770,   779,   803,   804,   805,   814,   815,
   818,   819,   822,   823,   824,   825,   826,   827,   832,   866,
   867,   868,   869,   870,   871,   872,   873,   874,   875,   876,
   879,   880,   881,   882,   883,   884,   885,   886,   887,   888,
   890,   891,   892,   893,   894,   895,   896,   897,   899,   909,
   928,   943,   946,   951,   958,   969,   983,   998,  1003,  1004,
  1008,  1009,  1010,  1014,  1015,  1016,  1017,  1021,  1026,  1034,
  1078,  1079,  1080,  1081,  1082,  1083,  1084,  1085,  1086,  1087,
  1088,  1089,  1092,  1093,  1094,  1095,  1096,  1097,  1098,  1099,
  1100,  1101,  1103,  1104,  1105,  1106,  1107,  1108,  1109,  1110,
  1112,  1122,  1147,  1163,  1191,  1214,  1215,  1218,  1219,  1223,
  1230,  1231,  1235,  1258,  1262,  1263,  1272,  1278,  1297,  1298,
  1299,  1300,  1301,  1302,  1303,  1307,  1332,  1336,  1337,  1353,
  1359,  1379,  1380,  1384,  1392,  1403,  1404,  1408,  1414,  1422,
  1442,  1483,  1494,  1495,  1529,  1530,  1535,  1551,  1552,  1562,
  1572,  1579,  1586,  1593,  1600,  1610,  1611,  1620,  1628,  1629,
  1638,  1643,  1649,  1658,  1659,  1663,  1689,  1690,  1695,  1704,
  1705,  1715,  1730,  1731,  1732,  1733,  1736,  1737,  1738,  1742,
  1743,  1751,  1759,  1777,  1781,  1785,  1789,  1804,  1805,  1806,
  1807,  1808,  1809,  1810,  1811,  1815,  1819,  1826,  1827,  1831,
  1834,  1835,  1836,  1837,  1838,  1839,  1840,  1841,  1842,  1843,
  1844,  1845,  1846,  1849,  1850
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","tTYPEDEF",
"tEXTERN","tSTRUCT","tENUM","tCPPCLASS","tINLINE","tSTATIC","tNL","tNUMBER",
"tLNUMBER","tSTRING","tIDENT","tFILENAME","tRAWDATA","tACCELERATORS","tBITMAP",
"tCURSOR","tDIALOG","tDIALOGEX","tMENU","tMENUEX","tMESSAGETABLE","tRCDATA",
"tVERSIONINFO","tSTRINGTABLE","tFONT","tFONTDIR","tICON","tAUTO3STATE","tAUTOCHECKBOX",
"tAUTORADIOBUTTON","tCHECKBOX","tDEFPUSHBUTTON","tPUSHBUTTON","tRADIOBUTTON",
"tSTATE3","tGROUPBOX","tCOMBOBOX","tLISTBOX","tSCROLLBAR","tCONTROL","tEDITTEXT",
"tRTEXT","tCTEXT","tLTEXT","tBLOCK","tVALUE","tSHIFT","tALT","tASCII","tVIRTKEY",
"tGRAYED","tCHECKED","tINACTIVE","tNOINVERT","tPURE","tIMPURE","tDISCARDABLE",
"tLOADONCALL","tPRELOAD","tFIXED","tMOVEABLE","tCLASS","tCAPTION","tCHARACTERISTICS",
"tEXSTYLE","tSTYLE","tVERSION","tLANGUAGE","tFILEVERSION","tPRODUCTVERSION",
"tFILEFLAGSMASK","tFILEOS","tFILETYPE","tFILEFLAGS","tFILESUBTYPE","tMENUBARBREAK",
"tMENUBREAK","tMENUITEM","tPOPUP","tSEPARATOR","tHELP","tTOOLBAR","tBUTTON",
"tBEGIN","tEND","tDLGINIT","'|'","'^'","'&'","'+'","'-'","'*'","'/'","'~'","tNOT",
"pUPM","'('","','","')'","resource_file","resources","cjunk","resource","@1",
"usrcvt","nameid","nameid_s","resource_definition","filename","bitmap","cursor",
"icon","font","fontdir","messagetable","rcdata","dlginit","userres","usertype",
"accelerators","events","acc_opt","accs","acc","dialog","dlg_attributes","ctrls",
"lab_ctrl","ctrl_desc","iconinfo","gen_ctrl","opt_font","optional_style","optional_style_pair",
"style","ctlclass","dialogex","dlgex_attribs","exctrls","gen_exctrl","lab_exctrl",
"exctrl_desc","opt_data","helpid","opt_exfont","opt_expr","menu","menu_body",
"item_definitions","item_options","menuex","menuex_body","itemex_definitions",
"itemex_options","itemex_p_options","stringtable","stt_head","strings","opt_comma",
"versioninfo","fix_version","ver_blocks","ver_block","ver_values","ver_value",
"ver_words","toolbar","toolbar_items","loadmemopts","lamo","lama","opt_lvc",
"opt_language","opt_characts","opt_version","raw_data","raw_elements","file_raw",
"e_expr","expr","xpr","any_num", NULL
};
#endif

static const short yyr1[] = {     0,
   103,   104,   104,   104,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   106,   106,   106,   106,   107,   106,   108,
   109,   109,   110,   110,   111,   111,   111,   111,   111,   111,
   111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
   112,   112,   112,   113,   114,   115,   116,   117,   118,   119,
   120,   121,   122,   122,   123,   124,   124,   124,   125,   125,
   126,   126,   127,   127,   127,   127,   127,   127,   128,   129,
   129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
   130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
   130,   130,   130,   130,   130,   130,   130,   130,   130,   131,
   132,   133,   133,   133,   133,   134,   134,   135,   136,   136,
   137,   137,   137,   138,   138,   138,   138,   139,   139,   140,
   141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
   141,   141,   142,   142,   142,   142,   142,   142,   142,   142,
   142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
   142,   143,   143,   144,   145,   146,   146,   147,   147,   148,
   149,   149,   150,   151,   152,   152,   152,   152,   153,   153,
   153,   153,   153,   153,   153,   154,   155,   156,   156,   156,
   156,   157,   157,   157,   157,   158,   158,   158,   158,   158,
   159,   160,   161,   161,   162,   162,   163,   164,   164,   164,
   164,   164,   164,   164,   164,   165,   165,   166,   167,   167,
   168,   168,   168,   169,   169,   170,   171,   171,   171,   172,
   172,   172,   173,   173,   173,   173,   174,   174,   174,   175,
   175,   175,   175,   176,   177,   178,   179,   180,   180,   180,
   180,   180,   180,   180,   180,   181,   181,   182,   182,   183,
   184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
   184,   184,   184,   185,   185
};

static const short yyr2[] = {     0,
     1,     0,     2,     2,     1,     1,     1,     1,     1,     1,
     1,     2,     1,     3,     3,     4,     1,     0,     5,     0,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     1,     1,     6,     0,     5,     5,     0,     2,
     1,     3,     1,     1,     1,     1,     1,     1,    13,     0,
     3,     3,     3,     2,     3,     3,     3,     2,     2,     2,
     0,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,    10,    12,
    10,     0,     4,     6,     8,    17,    15,     4,     0,     2,
     0,     2,     4,     3,     3,     1,     2,     1,     1,    14,
     0,     3,     3,     3,     2,     2,     3,     3,     3,     2,
     2,     2,     0,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
    10,    19,    16,    13,    11,     0,     1,     0,     2,     9,
     0,     2,     4,     3,     0,     6,     3,     5,     0,     3,
     3,     3,     3,     3,     3,     4,     3,     0,     4,     3,
     5,     0,     2,     5,     6,     0,     2,     4,     6,     8,
     4,     3,     0,     4,     0,     1,     6,     0,     9,     9,
     3,     3,     3,     3,     3,     0,     2,     5,     0,     2,
     1,     4,     4,     1,     3,     9,     0,     3,     2,     0,
     2,     2,     1,     1,     1,     1,     1,     1,     1,     0,
     2,     2,     2,     4,     2,     2,     4,     1,     1,     1,
     1,     3,     3,     3,     3,     1,     1,     0,     1,     1,
     3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
     3,     1,     2,     1,     1
};

static const short yydefact[] = {     2,
     1,     5,     7,     6,     8,     9,    11,    10,    13,   264,
   265,    20,   220,    18,     0,     0,     0,     0,     0,     4,
     3,    17,     0,    20,   250,   262,    12,     0,   230,     0,
   260,   259,   258,   263,     0,   193,     0,     0,     0,     0,
     0,     0,     0,     0,    53,    54,   220,   220,   220,   220,
   220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
   220,    15,    26,    27,    33,    31,    32,    36,    37,    30,
    39,   220,    25,    28,    29,    34,    35,    40,    38,   226,
   229,   225,   227,   223,   228,   224,   221,   222,   192,     0,
   261,     0,    54,    14,   253,   257,   254,   251,   252,   255,
   256,    16,   230,   230,   230,     0,     0,   230,   230,   230,
   230,   198,   230,   230,   230,     0,   230,   230,     0,     0,
     0,   231,   232,   233,     0,   191,   195,     0,    43,    42,
    41,   246,     0,   247,    44,    45,     0,     0,     0,     0,
    49,    50,     0,    47,    48,    46,     0,    51,    52,   235,
   236,     0,    19,   196,     0,    56,     0,     0,     0,   165,
   163,   178,   176,     0,     0,     0,     0,     0,     0,     0,
   206,     0,     0,   194,     0,   239,   240,   241,   238,   195,
     0,     0,     0,     0,     0,     0,   202,   203,   204,   201,
   205,     0,   230,   234,     0,    55,     0,   237,     0,     0,
     0,     0,     0,   164,     0,     0,   177,     0,     0,     0,
   197,   207,     0,     0,     0,   243,   244,   245,   242,     0,
     0,   195,   167,   169,   182,   180,   186,     0,     0,     0,
   217,    59,    59,     0,     0,     0,     0,     0,   248,   179,
   248,     0,     0,     0,   209,     0,     0,    57,    58,    70,
   158,   169,   169,   169,   169,   169,   169,   169,   168,     0,
   183,     0,   187,   181,     0,     0,     0,   219,     0,   216,
    65,    64,    66,    67,    68,    63,    60,    61,     0,     0,
   121,   166,   171,   170,   173,   174,   175,   172,   248,   248,
     0,     0,     0,   208,   211,   210,   218,     0,     0,     0,
     0,     0,     0,     0,     0,    81,    74,    78,    79,    80,
   159,     0,   169,   249,     0,   188,   199,   200,     0,    62,
    24,    22,    23,    76,    21,    77,     0,    75,    73,     0,
     0,    72,   116,    71,     0,     0,     0,     0,     0,     0,
     0,     0,   133,   125,   126,   130,   131,   132,     0,   184,
   248,     0,     0,   117,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    69,   128,   129,     0,   127,   124,
   123,   122,     0,   185,     0,   189,   212,   213,   214,   108,
   115,   114,   195,   195,    92,    94,    95,    87,    88,    90,
    91,    93,    89,    85,     0,    84,    86,   195,    82,    83,
    98,    97,    96,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   120,     0,     0,     0,     0,     0,     0,   108,
   195,   195,   144,   146,   147,   139,   140,   142,   143,   145,
   141,   137,     0,   136,   138,   195,   134,   135,   150,   149,
   148,   190,   215,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   119,     0,   118,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   161,     0,     0,     0,
     0,   102,     0,     0,     0,     0,   160,     0,     0,     0,
     0,     0,    99,     0,     0,     0,   162,   102,     0,     0,
     0,     0,     0,   109,     0,   151,     0,     0,     0,     0,
     0,     0,   101,     0,     0,   111,     0,   103,   109,   110,
     0,     0,     0,   156,     0,     0,   100,     0,   111,   112,
   155,   157,     0,   104,     0,   156,     0,     0,     0,     0,
   154,   113,     0,   105,   107,     0,     0,   156,   106,     0,
   153,   158,   156,   152,     0,     0,     0
};

static const short yydefgoto[] = {   575,
     1,    20,    21,    30,    28,   323,   324,    62,   132,    63,
    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
   175,   248,   277,   278,    74,   279,   335,   395,   404,   513,
   409,   307,   533,   544,   332,   486,    75,   312,   383,   457,
   443,   452,   551,   281,   345,   507,    76,   161,   183,   238,
    77,   163,   184,   240,   242,    22,    23,    92,   155,    78,
   143,   192,   212,   267,   296,   388,    79,   246,    29,    87,
    88,   133,   122,   123,   124,   134,   180,   135,   260,   325,
    25,    26
};

static const short yypact[] = {-32768,
     4,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -61,-32768,-32768,   233,   233,   233,    49,   233,-32768,
-32768,-32768,     1,-32768,   248,-32768,-32768,   391,   651,   233,
-32768,-32768,-32768,-32768,   606,-32768,   559,   233,   233,   233,
   233,   233,   233,   233,-32768,   -19,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   215,     2,
-32768,    62,-32768,-32768,   625,   102,   269,    36,    36,-32768,
-32768,-32768,   651,   191,   191,    82,    82,   651,   651,   191,
   191,   651,   191,   191,   191,    82,   191,   191,   233,   233,
   233,-32768,-32768,-32768,   233,-32768,    16,    12,-32768,-32768,
-32768,-32768,   228,-32768,-32768,-32768,    22,    38,   237,   282,
-32768,-32768,   376,-32768,-32768,-32768,    48,-32768,-32768,-32768,
-32768,    52,-32768,-32768,    87,-32768,   114,   233,   233,-32768,
-32768,-32768,-32768,   233,   233,   233,   233,   233,   233,   233,
-32768,   233,   233,-32768,   124,-32768,-32768,-32768,-32768,   -29,
    60,    72,   -49,    40,    76,    77,-32768,-32768,-32768,-32768,
-32768,   -31,-32768,-32768,    80,-32768,   108,-32768,   264,   233,
   233,     7,   100,-32768,     9,   198,-32768,   233,   233,   206,
-32768,-32768,   360,   233,   233,-32768,-32768,-32768,-32768,   119,
   122,    16,-32768,   127,   128,-32768,   131,   140,   146,   171,
-32768,   161,   161,   233,   233,   233,   147,   180,   233,-32768,
   233,   187,   233,   233,-32768,   173,   342,-32768,-32768,-32768,
   182,   127,   127,   127,   127,   127,   127,   127,-32768,   186,
   196,   204,   196,-32768,   205,   211,   -21,-32768,   233,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   219,-32768,   289,   233,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   233,   233,
   233,   233,   308,-32768,-32768,-32768,-32768,   342,   172,   225,
   233,   172,   315,    14,    14,-32768,-32768,-32768,-32768,-32768,
-32768,   375,   231,-32768,   244,   196,-32768,-32768,   270,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   271,-32768,-32768,    49,
    14,   239,-32768,   239,   561,   172,   225,   233,   172,   321,
    14,    14,-32768,-32768,-32768,-32768,-32768,-32768,    54,-32768,
   233,   281,   364,-32768,   -60,    14,   172,   367,   367,   367,
   367,   367,   367,   367,   367,   367,   233,   233,   233,   172,
   233,   367,   367,   367,-32768,-32768,-32768,   272,-32768,-32768,
   239,   239,   579,-32768,   285,   196,-32768,   288,-32768,-32768,
-32768,-32768,    16,    16,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   295,-32768,-32768,    16,-32768,-32768,
-32768,-32768,-32768,   378,   172,   385,   385,   385,   385,   385,
   385,   385,   385,   385,   233,   233,   233,   172,   233,   385,
   385,   385,-32768,   233,   233,   233,   233,   233,   233,   299,
    16,    16,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   300,-32768,-32768,    16,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   303,   305,   306,   317,   233,   233,   233,
   233,   233,   233,   233,   233,   290,   322,   328,   331,   338,
   358,   363,   365,   377,-32768,   380,-32768,   233,   233,   233,
   233,   290,   233,   233,   233,    14,   381,   382,   383,   384,
   397,   399,   401,   402,   -39,   233,-32768,   233,   233,   233,
    14,   233,-32768,   233,   233,   233,-32768,   399,   403,   405,
   -14,   409,   411,   416,   424,-32768,   233,   233,   233,   233,
   233,    14,-32768,   233,   430,   431,   433,   434,   416,   239,
   435,   233,    14,   390,   233,    14,-32768,   233,   431,   -12,
-32768,-32768,   437,    -6,   438,   390,    14,   233,    14,   233,
-32768,   239,   439,   239,   441,   233,    14,   -24,   239,    14,
-32768,     6,   390,-32768,   422,   465,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,   455,  -296,  -299,   449,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   310,-32768,   247,-32768,-32768,-32768,   296,   -55,    28,
-32768,   238,    10,     3,  -286,    61,-32768,-32768,-32768,-32768,
   254,    29,  -504,   -18,-32768,-32768,-32768,   318,-32768,   -65,
-32768,   313,-32768,-32768,-32768,-32768,-32768,-32768,  -178,-32768,
-32768,-32768,   291,-32768,-32768,-32768,-32768,-32768,   580,-32768,
-32768,   -23,  -274,  -258,  -244,  -520,-32768,   577,  -240,    -1,
   480,     5
};


#define	YYLAST		721


static const short yytable[] = {    24,
   262,   199,   328,   326,   308,    89,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,   210,    12,   334,   222,
   309,   225,    34,   552,    10,    11,   210,   293,    90,   356,
    13,   202,   203,    27,   310,   552,   376,   346,   204,   379,
   377,   391,  -230,   236,   355,  -230,  -230,   552,   313,   315,
   356,   561,   552,   347,   381,   382,   211,   393,   198,    10,
    11,   516,  -230,   571,    10,    11,   294,   348,   574,   392,
   408,   154,    10,    11,    14,   356,   570,   356,   119,   128,
   102,   120,   121,   356,   139,   140,   529,    36,   557,   223,
   127,   226,    10,    11,   559,   356,    15,    16,   156,   174,
    17,    18,   125,    19,   137,   138,   280,   253,   254,   255,
   385,   330,   224,   331,   147,   441,   154,   150,   151,   152,
   205,   206,   158,   153,   176,   177,   178,   207,   456,   179,
    43,    44,   256,   257,    10,    11,   195,   258,   159,    80,
    81,    82,    83,    84,    85,    86,    15,    16,   172,   126,
    17,    18,   173,    19,    15,    16,   181,   182,    17,    18,
   200,    19,   185,   186,   187,   188,   189,   190,   191,   213,
   193,   194,   201,   197,    15,    16,   208,   209,    17,    18,
   214,    19,    10,    11,   321,   322,   282,   283,   284,   285,
   286,   287,   288,    40,    41,    42,    43,    44,   220,   221,
   253,   254,   255,   129,   130,   131,   228,   229,   215,   505,
   227,   196,   232,   233,   436,   437,    15,    16,   230,   234,
    17,    18,   235,    19,   521,   256,   257,   237,   239,   439,
   258,   241,   250,   251,   252,    10,    11,   261,   322,   263,
   243,   265,   266,    10,    11,   540,   244,   350,    80,    81,
    82,    83,    84,    85,    86,   268,   550,   245,   269,   554,
   270,   247,   469,   470,    15,    16,   160,   297,    17,    18,
   562,    19,   564,   162,   216,   217,   218,   472,   311,   219,
   569,   119,   280,   572,   120,   121,   289,   314,   316,   317,
   318,    10,    11,   387,   119,   299,  -249,   120,   121,   327,
    10,    11,   485,   119,   290,   291,   120,   121,   333,   333,
   300,   292,   406,   407,   157,   410,   301,    15,    16,   298,
   319,    17,    18,   160,    19,    15,    16,   329,   356,    17,
    18,   349,    19,   380,   354,   333,   378,    38,    39,    40,
    41,    42,    43,    44,   351,   333,   333,   384,   119,   386,
   389,   120,   121,   302,   303,   119,   304,   305,   120,   121,
   333,    41,    42,    43,    44,   405,   405,   405,   162,   405,
   352,   353,   414,    15,    16,   306,   390,    17,    18,   394,
    19,   336,    15,    16,   271,   434,    17,    18,   435,    19,
   440,   272,   273,   274,   275,   438,   337,   442,   276,   468,
   471,    45,   338,   473,    46,   474,   475,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    56,   476,    57,    58,
    59,   576,   488,   453,   453,   453,   119,   453,   489,   120,
   121,   490,   462,   463,   464,   465,   466,   467,   491,   339,
   340,   119,   341,   342,   120,   121,   231,   164,   165,   166,
   167,   168,   169,   170,   454,   455,  -230,   458,   492,  -230,
  -230,   343,   171,   493,   577,   494,   477,   478,   479,   480,
   481,   482,   483,   484,   487,    60,  -230,   495,    37,    61,
   496,   506,   508,   509,   510,    94,   497,   498,   499,   500,
   487,   502,   503,   504,    31,    32,    33,   511,    35,   512,
   333,   514,   515,   527,   517,   528,   518,   519,   520,   530,
   522,   531,   523,   524,   525,   333,   532,    95,    96,    97,
    98,    99,   100,   101,   534,   535,   536,   537,   538,   539,
   542,   543,   541,   545,   546,   548,   333,   558,   560,   566,
   549,   567,   249,   553,   320,   526,   555,   333,   547,   344,
   333,   556,   501,   573,   264,   259,   563,   295,   565,     0,
     0,   333,     0,   333,   568,     0,     0,     0,     0,    45,
     0,   333,    93,     0,   333,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,     0,    57,    58,    59,     0,
   357,   358,   359,   360,   361,   362,   363,   364,   365,   366,
   367,   368,   369,   370,   371,   372,   373,   374,   415,   416,
   417,   418,   419,   420,   421,   422,   423,   424,   425,   426,
   427,   428,   429,   430,   431,   432,   103,   104,   105,   106,
   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
   117,     0,     0,    60,     0,     0,     0,    61,   375,     0,
     0,   118,     0,     0,   396,   397,   398,   399,   400,   401,
   402,   403,     0,     0,     0,     0,   433,   411,   412,   413,
   444,   445,   446,   447,   448,   449,   450,   451,     0,     0,
     0,   136,     0,   459,   460,   461,   141,   142,     0,   144,
   145,   146,     0,   148,   149,    38,    39,    40,    41,    42,
    43,    44,     0,     0,     0,     0,     0,    91,    80,    81,
    82,    83,    84,    85,    86,    39,    40,    41,    42,    43,
    44
};

static const short yycheck[] = {     1,
   241,   180,   302,   300,   279,    29,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    48,    14,   305,    13,
   279,    13,    18,   544,    11,    12,    48,    49,    30,    90,
    27,    81,    82,    95,   279,   556,   336,   312,    88,   339,
   337,   102,    67,   222,   331,    70,    71,   568,   289,   290,
    90,   556,   573,   312,   341,   342,    88,   357,    88,    11,
    12,   101,    87,   568,    11,    12,    88,   312,   573,   356,
   370,   101,    11,    12,    71,    90,   101,    90,    67,   103,
   100,    70,    71,    90,   108,   109,   101,    87,   101,    83,
    92,    83,    11,    12,   101,    90,    93,    94,    87,    13,
    97,    98,   101,   100,   106,   107,   101,    54,    55,    56,
   351,    98,    13,   100,   116,   415,   101,   119,   120,   121,
    81,    82,   101,   125,    11,    12,    13,    88,   428,    16,
    95,    96,    79,    80,    11,    12,    13,    84,   101,    58,
    59,    60,    61,    62,    63,    64,    93,    94,   101,    88,
    97,    98,   101,   100,    93,    94,   158,   159,    97,    98,
   101,   100,   164,   165,   166,   167,   168,   169,   170,   193,
   172,   173,   101,   175,    93,    94,   101,   101,    97,    98,
   101,   100,    11,    12,    13,    14,   252,   253,   254,   255,
   256,   257,   258,    92,    93,    94,    95,    96,   200,   201,
    54,    55,    56,    13,    14,    15,   208,   209,   101,   496,
    13,    88,   214,   215,   393,   394,    93,    94,    13,   101,
    97,    98,   101,   100,   511,    79,    80,   101,   101,   408,
    84,   101,   234,   235,   236,    11,    12,   239,    14,   241,
   101,   243,   244,    11,    12,   532,   101,   313,    58,    59,
    60,    61,    62,    63,    64,    83,   543,    87,    86,   546,
    88,   101,   441,   442,    93,    94,    87,   269,    97,    98,
   557,   100,   559,    87,    11,    12,    13,   456,   280,    16,
   567,    67,   101,   570,    70,    71,   101,   289,   290,   291,
   292,    11,    12,    13,    67,     7,   101,    70,    71,   301,
    11,    12,    13,    67,   101,   101,    70,    71,   304,   305,
    22,   101,   368,   369,    87,   371,    28,    93,    94,   101,
    13,    97,    98,    87,   100,    93,    94,    13,    90,    97,
    98,   101,   100,    13,   330,   331,   338,    90,    91,    92,
    93,    94,    95,    96,   101,   341,   342,   349,    67,   351,
   352,    70,    71,    65,    66,    67,    68,    69,    70,    71,
   356,    93,    94,    95,    96,   367,   368,   369,    87,   371,
   101,   101,   101,    93,    94,    87,    13,    97,    98,    13,
   100,     7,    93,    94,    43,   101,    97,    98,   101,   100,
    13,    50,    51,    52,    53,   101,    22,    13,    57,   101,
   101,    11,    28,   101,    14,   101,   101,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,   101,    28,    29,
    30,     0,   101,   425,   426,   427,    67,   429,   101,    70,
    71,   101,   434,   435,   436,   437,   438,   439,   101,    65,
    66,    67,    68,    69,    70,    71,    87,    72,    73,    74,
    75,    76,    77,    78,   426,   427,    67,   429,   101,    70,
    71,    87,    87,   101,     0,   101,   468,   469,   470,   471,
   472,   473,   474,   475,   476,    85,    87,   101,    24,    89,
   101,   101,   101,   101,   101,    37,   488,   489,   490,   491,
   492,   493,   494,   495,    15,    16,    17,   101,    19,   101,
   496,   101,   101,   101,   506,   101,   508,   509,   510,   101,
   512,   101,   514,   515,   516,   511,   101,    38,    39,    40,
    41,    42,    43,    44,   101,   527,   528,   529,   530,   531,
   101,   101,   534,   101,   101,   101,   532,   101,   101,   101,
   542,   101,   233,   545,   298,   518,   548,   543,   539,   312,
   546,   549,   492,   572,   242,   238,   558,   267,   560,    -1,
    -1,   557,    -1,   559,   566,    -1,    -1,    -1,    -1,    11,
    -1,   567,    14,    -1,   570,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    -1,    28,    29,    30,    -1,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    44,    45,    46,    47,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    -1,    -1,    85,    -1,    -1,    -1,    89,    88,    -1,
    -1,    72,    -1,    -1,   359,   360,   361,   362,   363,   364,
   365,   366,    -1,    -1,    -1,    -1,    88,   372,   373,   374,
   417,   418,   419,   420,   421,   422,   423,   424,    -1,    -1,
    -1,   105,    -1,   430,   431,   432,   110,   111,    -1,   113,
   114,   115,    -1,   117,   118,    90,    91,    92,    93,    94,
    95,    96,    -1,    -1,    -1,    -1,    -1,   102,    58,    59,
    60,    61,    62,    63,    64,    91,    92,    93,    94,    95,
    96
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

case 1:
#line 346 "parser.y"
{
		resource_t *rsc;
		/* First add stringtables to the resource-list */
		rsc = build_stt_resources(sttres);
		/* 'build_stt_resources' returns a head and $1 is a tail */
		if(yyvsp[0].res)
		{
			yyvsp[0].res->next = rsc;
			if(rsc)
				rsc->prev = yyvsp[0].res;
		}
		else
			yyvsp[0].res = rsc;
		/* Find the tail again */
		while(yyvsp[0].res && yyvsp[0].res->next)
			yyvsp[0].res = yyvsp[0].res->next;
		/* Now add any fontdirecory */
		rsc = build_fontdirs(yyvsp[0].res);
		/* 'build_fontdir' returns a head and $1 is a tail */
		if(yyvsp[0].res)
		{
			yyvsp[0].res->next = rsc;
			if(rsc)
				rsc->prev = yyvsp[0].res;
		}
		else
			yyvsp[0].res = rsc;
		/* Final statement before were done */
		resource_top = get_resource_head(yyvsp[0].res);
		;
    break;}
case 2:
#line 380 "parser.y"
{ yyval.res = NULL; want_id = 1; ;
    break;}
case 3:
#line 381 "parser.y"
{
		if(yyvsp[0].res)
		{
			resource_t *tail = yyvsp[0].res;
			resource_t *head = yyvsp[0].res;
			while(tail->next)
				tail = tail->next;
			while(head->prev)
				head = head->prev;
			head->prev = yyvsp[-1].res;
			if(yyvsp[-1].res)
				yyvsp[-1].res->next = head;
			yyval.res = tail;
			/* Check for duplicate identifiers */
			while(yyvsp[-1].res && head)
			{
				resource_t *rsc = yyvsp[-1].res;
				while(rsc)
				{
					if(rsc->type == head->type
					&& rsc->lan->id == head->lan->id
					&& rsc->lan->sub == head->lan->sub
					&& !compare_name_id(rsc->name, head->name))
					{
						yyerror("Duplicate resource name '%s'", get_nameid_str(rsc->name));
					}
					rsc = rsc->prev;
				}
				head = head->next;
			}
		}
		else if(yyvsp[-1].res)
		{
			resource_t *tail = yyvsp[-1].res;
			while(tail->next)
				tail = tail->next;
			yyval.res = tail;
		}
		else
			yyval.res = NULL;

		if(!dont_want_id)	/* See comments in language parsing below */
			want_id = 1;
		dont_want_id = 0;
		;
    break;}
case 4:
#line 426 "parser.y"
{ yyval.res = yyvsp[-1].res; want_id = 1; ;
    break;}
case 5:
#line 431 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 6:
#line 432 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 7:
#line 433 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 8:
#line 434 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 9:
#line 435 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 10:
#line 436 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 11:
#line 437 "parser.y"
{ internal_error(__FILE__, __LINE__, "Don't yet know how to strip inline functions\n"); ;
    break;}
case 12:
#line 441 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 14:
#line 471 "parser.y"
{
		yyval.res = yyvsp[0].res;
		if(yyval.res)
		{
			if(yyvsp[-2].num > 65535 || yyvsp[-2].num < -32768)
				yyerror("Resource's ID out of range (%d)", yyvsp[-2].num);
			yyval.res->name = new_name_id();
			yyval.res->name->type = name_ord;
			yyval.res->name->name.i_name = yyvsp[-2].num;
			chat("Got %s (%d)", get_typename(yyvsp[0].res), yyval.res->name->name.i_name);
			}
			;
    break;}
case 15:
#line 483 "parser.y"
{
		yyval.res = yyvsp[0].res;
		if(yyval.res)
		{
			yyval.res->name = new_name_id();
			yyval.res->name->type = name_str;
			yyval.res->name->name.s_name = yyvsp[-2].str;
			chat("Got %s (%s)", get_typename(yyvsp[0].res), yyval.res->name->name.s_name->str.cstr);
		}
		;
    break;}
case 16:
#line 493 "parser.y"
{ /* cjunk */ strip_til_parenthesis(); yyval.res = NULL; ;
    break;}
case 17:
#line 514 "parser.y"
{
		/* Don't do anything, stringtables are converted to
		 * resource_t structures when we are finished parsing and
		 * the final rule of the parser is reduced (see above)
		 */
		yyval.res = NULL;
		chat("Got STRINGTABLE");
		;
    break;}
case 18:
#line 522 "parser.y"
{want_nl = 1; ;
    break;}
case 19:
#line 522 "parser.y"
{
		/* We *NEED* the newline to delimit the expression.
		 * Otherwise, we would not be able to set the next
		 * want_id anymore because of the token-lookahead.
		 *
		 * However, we can test the lookahead-token for
		 * being "non-expression" type, in which case we
		 * continue. Fortunately, tNL is the only token that
		 * will break expression parsing and is implicitely
		 * void, so we just remove it. This scheme makes it
		 * possible to do some (not all) fancy preprocessor
		 * stuff.
		 * BTW, we also need to make sure that the next
		 * reduction of 'resources' above will *not* set
		 * want_id because we already have a lookahead that
		 * cannot be undone.
		 */
		if(yychar != YYEMPTY && yychar != tNL)
			dont_want_id = 1;

		if(yychar == tNL)
			yychar = YYEMPTY;	/* Could use 'yyclearin', but we already need the*/
						/* direct access to yychar in rule 'usrcvt' below. */
		else if(yychar == tIDENT)
			yywarning("LANGUAGE statement not delimited with newline; next identifier might be wrong");

		want_nl = 0;	/* We don't want it anymore if we didn't get it */

		if(!win32)
			yywarning("LANGUAGE not supported in 16-bit mode");
		if(currentlanguage)
			free(currentlanguage);
		currentlanguage = new_language(yyvsp[-2].num, yyvsp[0].num);
		yyval.res = NULL;
		chat("Got LANGUAGE %d,%d (0x%04x)", yyvsp[-2].num, yyvsp[0].num, (yyvsp[0].num<<10) + yyvsp[-2].num);
		;
    break;}
case 20:
#line 564 "parser.y"
{ yychar = rsrcid_to_token(yychar); ;
    break;}
case 21:
#line 570 "parser.y"
{
		if(yyvsp[0].num > 65535 || yyvsp[0].num < -32768)
			yyerror("Resource's ID out of range (%d)", yyvsp[0].num);
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		;
    break;}
case 22:
#line 577 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		;
    break;}
case 23:
#line 587 "parser.y"
{ yyval.nid = yyvsp[0].nid; ;
    break;}
case 24:
#line 588 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		;
    break;}
case 25:
#line 597 "parser.y"
{ yyval.res = new_resource(res_acc, yyvsp[0].acc, yyvsp[0].acc->memopt, yyvsp[0].acc->lvc.language); ;
    break;}
case 26:
#line 598 "parser.y"
{ yyval.res = new_resource(res_bmp, yyvsp[0].bmp, yyvsp[0].bmp->memopt, yyvsp[0].bmp->data->lvc.language); ;
    break;}
case 27:
#line 599 "parser.y"
{
		resource_t *rsc;
		if(yyvsp[0].ani->type == res_anicur)
		{
			yyval.res = rsc = new_resource(res_anicur, yyvsp[0].ani->u.ani, yyvsp[0].ani->u.ani->memopt, yyvsp[0].ani->u.ani->data->lvc.language);
		}
		else if(yyvsp[0].ani->type == res_curg)
		{
			cursor_t *cur;
			yyval.res = rsc = new_resource(res_curg, yyvsp[0].ani->u.curg, yyvsp[0].ani->u.curg->memopt, yyvsp[0].ani->u.curg->lvc.language);
			for(cur = yyvsp[0].ani->u.curg->cursorlist; cur; cur = cur->next)
			{
				rsc->prev = new_resource(res_cur, cur, yyvsp[0].ani->u.curg->memopt, yyvsp[0].ani->u.curg->lvc.language);
				rsc->prev->next = rsc;
				rsc = rsc->prev;
				rsc->name = new_name_id();
				rsc->name->type = name_ord;
				rsc->name->name.i_name = cur->id;
			}
		}
		else
			internal_error(__FILE__, __LINE__, "Invalid top-level type %d in cursor resource", yyvsp[0].ani->type);
		free(yyvsp[0].ani);
		;
    break;}
case 28:
#line 623 "parser.y"
{ yyval.res = new_resource(res_dlg, yyvsp[0].dlg, yyvsp[0].dlg->memopt, yyvsp[0].dlg->lvc.language); ;
    break;}
case 29:
#line 624 "parser.y"
{
		if(win32)
			yyval.res = new_resource(res_dlgex, yyvsp[0].dlgex, yyvsp[0].dlgex->memopt, yyvsp[0].dlgex->lvc.language);
		else
			yyval.res = NULL;
		;
    break;}
case 30:
#line 630 "parser.y"
{ yyval.res = new_resource(res_dlginit, yyvsp[0].dginit, yyvsp[0].dginit->memopt, yyvsp[0].dginit->data->lvc.language); ;
    break;}
case 31:
#line 631 "parser.y"
{ yyval.res = new_resource(res_fnt, yyvsp[0].fnt, yyvsp[0].fnt->memopt, yyvsp[0].fnt->data->lvc.language); ;
    break;}
case 32:
#line 632 "parser.y"
{ yyval.res = new_resource(res_fntdir, yyvsp[0].fnd, yyvsp[0].fnd->memopt, yyvsp[0].fnd->data->lvc.language); ;
    break;}
case 33:
#line 633 "parser.y"
{
		resource_t *rsc;
		if(yyvsp[0].ani->type == res_aniico)
		{
			yyval.res = rsc = new_resource(res_aniico, yyvsp[0].ani->u.ani, yyvsp[0].ani->u.ani->memopt, yyvsp[0].ani->u.ani->data->lvc.language);
		}
		else if(yyvsp[0].ani->type == res_icog)
		{
			icon_t *ico;
			yyval.res = rsc = new_resource(res_icog, yyvsp[0].ani->u.icog, yyvsp[0].ani->u.icog->memopt, yyvsp[0].ani->u.icog->lvc.language);
			for(ico = yyvsp[0].ani->u.icog->iconlist; ico; ico = ico->next)
			{
				rsc->prev = new_resource(res_ico, ico, yyvsp[0].ani->u.icog->memopt, yyvsp[0].ani->u.icog->lvc.language);
				rsc->prev->next = rsc;
				rsc = rsc->prev;
				rsc->name = new_name_id();
				rsc->name->type = name_ord;
				rsc->name->name.i_name = ico->id;
			}
		}
		else
			internal_error(__FILE__, __LINE__, "Invalid top-level type %d in icon resource", yyvsp[0].ani->type);
		free(yyvsp[0].ani);
		;
    break;}
case 34:
#line 657 "parser.y"
{ yyval.res = new_resource(res_men, yyvsp[0].men, yyvsp[0].men->memopt, yyvsp[0].men->lvc.language); ;
    break;}
case 35:
#line 658 "parser.y"
{
		if(win32)
			yyval.res = new_resource(res_menex, yyvsp[0].menex, yyvsp[0].menex->memopt, yyvsp[0].menex->lvc.language);
		else
			yyval.res = NULL;
		;
    break;}
case 36:
#line 664 "parser.y"
{ yyval.res = new_resource(res_msg, yyvsp[0].msg, WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE, dup_language(currentlanguage)); ;
    break;}
case 37:
#line 665 "parser.y"
{ yyval.res = new_resource(res_rdt, yyvsp[0].rdt, yyvsp[0].rdt->memopt, yyvsp[0].rdt->data->lvc.language); ;
    break;}
case 38:
#line 666 "parser.y"
{ yyval.res = new_resource(res_toolbar, yyvsp[0].tlbar, yyvsp[0].tlbar->memopt, yyvsp[0].tlbar->lvc.language); ;
    break;}
case 39:
#line 667 "parser.y"
{ yyval.res = new_resource(res_usr, yyvsp[0].usr, yyvsp[0].usr->memopt, yyvsp[0].usr->data->lvc.language); ;
    break;}
case 40:
#line 668 "parser.y"
{ yyval.res = new_resource(res_ver, yyvsp[0].veri, WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE, yyvsp[0].veri->lvc.language); ;
    break;}
case 41:
#line 672 "parser.y"
{ yyval.str = make_filename(yyvsp[0].str); ;
    break;}
case 42:
#line 673 "parser.y"
{ yyval.str = make_filename(yyvsp[0].str); ;
    break;}
case 43:
#line 674 "parser.y"
{ yyval.str = make_filename(yyvsp[0].str); ;
    break;}
case 44:
#line 678 "parser.y"
{ yyval.bmp = new_bitmap(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 45:
#line 682 "parser.y"
{
		yyval.ani = new_ani_any();
		if(yyvsp[0].raw->size > 4 && !memcmp(yyvsp[0].raw->data, riff, sizeof(riff)))
		{
			yyval.ani->type = res_anicur;
			yyval.ani->u.ani = new_ani_curico(res_anicur, yyvsp[0].raw, yyvsp[-1].iptr);
		}
		else
		{
			yyval.ani->type = res_curg;
			yyval.ani->u.curg = new_cursor_group(yyvsp[0].raw, yyvsp[-1].iptr);
		}
	;
    break;}
case 46:
#line 698 "parser.y"
{
		yyval.ani = new_ani_any();
		if(yyvsp[0].raw->size > 4 && !memcmp(yyvsp[0].raw->data, riff, sizeof(riff)))
		{
			yyval.ani->type = res_aniico;
			yyval.ani->u.ani = new_ani_curico(res_aniico, yyvsp[0].raw, yyvsp[-1].iptr);
		}
		else
		{
			yyval.ani->type = res_icog;
			yyval.ani->u.icog = new_icon_group(yyvsp[0].raw, yyvsp[-1].iptr);
		}
	;
    break;}
case 47:
#line 720 "parser.y"
{ yyval.fnt = new_font(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 48:
#line 730 "parser.y"
{ yyval.fnd = new_fontdir(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 49:
#line 738 "parser.y"
{
		if(!win32)
			yywarning("MESSAGETABLE not supported in 16-bit mode");
		yyval.msg = new_messagetable(yyvsp[0].raw, yyvsp[-1].iptr);
		;
    break;}
case 50:
#line 746 "parser.y"
{ yyval.rdt = new_rcdata(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 51:
#line 750 "parser.y"
{ yyval.dginit = new_dlginit(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 52:
#line 754 "parser.y"
{
		#ifdef WORDS_BIGENDIAN
			if(pedantic && byteorder != WRC_BO_LITTLE)
		#else
			if(pedantic && byteorder == WRC_BO_BIG)
		#endif
				yywarning("Byteordering is not little-endian and type cannot be interpreted");
			yyval.usr = new_user(yyvsp[-2].nid, yyvsp[0].raw, yyvsp[-1].iptr);
		;
    break;}
case 53:
#line 765 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		;
    break;}
case 54:
#line 770 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		;
    break;}
case 55:
#line 779 "parser.y"
{
		yyval.acc = new_accelerator();
		if(yyvsp[-4].iptr)
		{
			yyval.acc->memopt = *(yyvsp[-4].iptr);
			free(yyvsp[-4].iptr);
		}
		else
		{
			yyval.acc->memopt = WRC_MO_MOVEABLE | WRC_MO_PURE;
		}
		if(!yyvsp[-1].event)
			yyerror("Accelerator table must have at least one entry");
		yyval.acc->events = get_event_head(yyvsp[-1].event);
		if(yyvsp[-3].lvc)
		{
			yyval.acc->lvc = *(yyvsp[-3].lvc);
			free(yyvsp[-3].lvc);
		}
		if(!yyval.acc->lvc.language)
			yyval.acc->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 56:
#line 803 "parser.y"
{ yyval.event=NULL; ;
    break;}
case 57:
#line 804 "parser.y"
{ yyval.event=add_string_event(yyvsp[-3].str, yyvsp[-1].num, yyvsp[0].num, yyvsp[-4].event); ;
    break;}
case 58:
#line 805 "parser.y"
{ yyval.event=add_event(yyvsp[-3].num, yyvsp[-1].num, yyvsp[0].num, yyvsp[-4].event); ;
    break;}
case 59:
#line 814 "parser.y"
{ yyval.num = 0; ;
    break;}
case 60:
#line 815 "parser.y"
{ yyval.num = yyvsp[0].num; ;
    break;}
case 61:
#line 818 "parser.y"
{ yyval.num = yyvsp[0].num; ;
    break;}
case 62:
#line 819 "parser.y"
{ yyval.num = yyvsp[-2].num | yyvsp[0].num; ;
    break;}
case 63:
#line 822 "parser.y"
{ yyval.num = WRC_AF_NOINVERT; ;
    break;}
case 64:
#line 823 "parser.y"
{ yyval.num = WRC_AF_SHIFT; ;
    break;}
case 65:
#line 824 "parser.y"
{ yyval.num = WRC_AF_CONTROL; ;
    break;}
case 66:
#line 825 "parser.y"
{ yyval.num = WRC_AF_ALT; ;
    break;}
case 67:
#line 826 "parser.y"
{ yyval.num = WRC_AF_ASCII; ;
    break;}
case 68:
#line 827 "parser.y"
{ yyval.num = WRC_AF_VIRTKEY; ;
    break;}
case 69:
#line 833 "parser.y"
{
		if(yyvsp[-11].iptr)
		{
			yyvsp[-3].dlg->memopt = *(yyvsp[-11].iptr);
			free(yyvsp[-11].iptr);
		}
		else
			yyvsp[-3].dlg->memopt = WRC_MO_MOVEABLE | WRC_MO_PURE | WRC_MO_DISCARDABLE;
		yyvsp[-3].dlg->x = yyvsp[-10].num;
		yyvsp[-3].dlg->y = yyvsp[-8].num;
		yyvsp[-3].dlg->width = yyvsp[-6].num;
		yyvsp[-3].dlg->height = yyvsp[-4].num;
		yyvsp[-3].dlg->controls = get_control_head(yyvsp[-1].ctl);
		yyval.dlg = yyvsp[-3].dlg;
		if(!yyval.dlg->gotstyle)
		{
			yyval.dlg->style->or_mask = WS_POPUP;
			yyval.dlg->gotstyle = TRUE;
		}
		if(yyval.dlg->title)
			yyval.dlg->style->or_mask |= WS_CAPTION;
		if(yyval.dlg->font)
			yyval.dlg->style->or_mask |= DS_SETFONT;

		yyval.dlg->style->or_mask &= ~(yyval.dlg->style->and_mask);
		yyval.dlg->style->and_mask = 0;

		if(!yyval.dlg->lvc.language)
			yyval.dlg->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 70:
#line 866 "parser.y"
{ yyval.dlg=new_dialog(); ;
    break;}
case 71:
#line 867 "parser.y"
{ yyval.dlg=dialog_style(yyvsp[0].style,yyvsp[-2].dlg); ;
    break;}
case 72:
#line 868 "parser.y"
{ yyval.dlg=dialog_exstyle(yyvsp[0].style,yyvsp[-2].dlg); ;
    break;}
case 73:
#line 869 "parser.y"
{ yyval.dlg=dialog_caption(yyvsp[0].str,yyvsp[-2].dlg); ;
    break;}
case 74:
#line 870 "parser.y"
{ yyval.dlg=dialog_font(yyvsp[0].fntid,yyvsp[-1].dlg); ;
    break;}
case 75:
#line 871 "parser.y"
{ yyval.dlg=dialog_class(yyvsp[0].nid,yyvsp[-2].dlg); ;
    break;}
case 76:
#line 872 "parser.y"
{ yyval.dlg=dialog_class(yyvsp[0].nid,yyvsp[-2].dlg); ;
    break;}
case 77:
#line 873 "parser.y"
{ yyval.dlg=dialog_menu(yyvsp[0].nid,yyvsp[-2].dlg); ;
    break;}
case 78:
#line 874 "parser.y"
{ yyval.dlg=dialog_language(yyvsp[0].lan,yyvsp[-1].dlg); ;
    break;}
case 79:
#line 875 "parser.y"
{ yyval.dlg=dialog_characteristics(yyvsp[0].chars,yyvsp[-1].dlg); ;
    break;}
case 80:
#line 876 "parser.y"
{ yyval.dlg=dialog_version(yyvsp[0].ver,yyvsp[-1].dlg); ;
    break;}
case 81:
#line 879 "parser.y"
{ yyval.ctl = NULL; ;
    break;}
case 82:
#line 880 "parser.y"
{ yyval.ctl=ins_ctrl(-1, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 83:
#line 881 "parser.y"
{ yyval.ctl=ins_ctrl(CT_EDIT, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 84:
#line 882 "parser.y"
{ yyval.ctl=ins_ctrl(CT_LISTBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 85:
#line 883 "parser.y"
{ yyval.ctl=ins_ctrl(CT_COMBOBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 86:
#line 884 "parser.y"
{ yyval.ctl=ins_ctrl(CT_SCROLLBAR, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 87:
#line 885 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_CHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 88:
#line 886 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_DEFPUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 89:
#line 887 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_GROUPBOX, yyvsp[0].ctl, yyvsp[-2].ctl);;
    break;}
case 90:
#line 888 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_PUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 91:
#line 890 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_RADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 92:
#line 891 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTO3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 93:
#line 892 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 94:
#line 893 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTOCHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 95:
#line 894 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTORADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 96:
#line 895 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_LEFT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 97:
#line 896 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_CENTER, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 98:
#line 897 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_RIGHT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 99:
#line 899 "parser.y"
{
		yyvsp[0].ctl->title = yyvsp[-7].nid;
		yyvsp[0].ctl->id = yyvsp[-5].num;
		yyvsp[0].ctl->x = yyvsp[-3].num;
		yyvsp[0].ctl->y = yyvsp[-1].num;
		yyval.ctl = ins_ctrl(CT_STATIC, SS_ICON, yyvsp[0].ctl, yyvsp[-9].ctl);
		;
    break;}
case 100:
#line 909 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = new_name_id();
		yyval.ctl->title->type = name_str;
		yyval.ctl->title->name.s_name = yyvsp[-11].str;
		yyval.ctl->id = yyvsp[-9].num;
		yyval.ctl->x = yyvsp[-7].num;
		yyval.ctl->y = yyvsp[-5].num;
		yyval.ctl->width = yyvsp[-3].num;
		yyval.ctl->height = yyvsp[-1].num;
		if(yyvsp[0].style)
		{
			yyval.ctl->style = yyvsp[0].style;
			yyval.ctl->gotstyle = TRUE;
		}
		;
    break;}
case 101:
#line 928 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->id = yyvsp[-9].num;
		yyval.ctl->x = yyvsp[-7].num;
		yyval.ctl->y = yyvsp[-5].num;
		yyval.ctl->width = yyvsp[-3].num;
		yyval.ctl->height = yyvsp[-1].num;
		if(yyvsp[0].style)
		{
			yyval.ctl->style = yyvsp[0].style;
			yyval.ctl->gotstyle = TRUE;
		}
		;
    break;}
case 102:
#line 944 "parser.y"
{ yyval.ctl = new_control(); ;
    break;}
case 103:
#line 946 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-2].num;
		yyval.ctl->height = yyvsp[0].num;
		;
    break;}
case 104:
#line 951 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-4].num;
		yyval.ctl->height = yyvsp[-2].num;
		yyval.ctl->style = yyvsp[0].style;
		yyval.ctl->gotstyle = TRUE;
		;
    break;}
case 105:
#line 958 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-6].num;
		yyval.ctl->height = yyvsp[-4].num;
		yyval.ctl->style = yyvsp[-2].style;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->exstyle = yyvsp[0].style;
		yyval.ctl->gotexstyle = TRUE;
		;
    break;}
case 106:
#line 969 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-16].nid;
		yyval.ctl->id = yyvsp[-14].num;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-12].nid);
		yyval.ctl->style = yyvsp[-10].style;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->x = yyvsp[-8].num;
		yyval.ctl->y = yyvsp[-6].num;
		yyval.ctl->width = yyvsp[-4].num;
		yyval.ctl->height = yyvsp[-2].num;
		yyval.ctl->exstyle = yyvsp[0].style;
		yyval.ctl->gotexstyle = TRUE;
		;
    break;}
case 107:
#line 983 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-14].nid;
		yyval.ctl->id = yyvsp[-12].num;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-10].nid);
		yyval.ctl->style = yyvsp[-8].style;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->x = yyvsp[-6].num;
		yyval.ctl->y = yyvsp[-4].num;
		yyval.ctl->width = yyvsp[-2].num;
		yyval.ctl->height = yyvsp[0].num;
		;
    break;}
case 108:
#line 998 "parser.y"
{ yyval.fntid = new_font_id(yyvsp[-2].num, yyvsp[0].str, 0, 0); ;
    break;}
case 109:
#line 1003 "parser.y"
{ yyval.style = NULL; ;
    break;}
case 110:
#line 1004 "parser.y"
{ yyval.style = yyvsp[0].style; ;
    break;}
case 111:
#line 1008 "parser.y"
{ yyval.styles = NULL; ;
    break;}
case 112:
#line 1009 "parser.y"
{ yyval.styles = new_style_pair(yyvsp[0].style, 0); ;
    break;}
case 113:
#line 1010 "parser.y"
{ yyval.styles = new_style_pair(yyvsp[-2].style, yyvsp[0].style); ;
    break;}
case 114:
#line 1014 "parser.y"
{ yyval.style = new_style(yyvsp[-2].style->or_mask | yyvsp[0].style->or_mask, yyvsp[-2].style->and_mask | yyvsp[0].style->and_mask); free(yyvsp[-2].style); free(yyvsp[0].style);;
    break;}
case 115:
#line 1015 "parser.y"
{ yyval.style = yyvsp[-1].style; ;
    break;}
case 116:
#line 1016 "parser.y"
{ yyval.style = new_style(yyvsp[0].num, 0); ;
    break;}
case 117:
#line 1017 "parser.y"
{ yyval.style = new_style(0, yyvsp[0].num); ;
    break;}
case 118:
#line 1021 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		;
    break;}
case 119:
#line 1026 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		;
    break;}
case 120:
#line 1035 "parser.y"
{
		if(!win32)
			yywarning("DIALOGEX not supported in 16-bit mode");
		if(yyvsp[-12].iptr)
		{
			yyvsp[-3].dlgex->memopt = *(yyvsp[-12].iptr);
			free(yyvsp[-12].iptr);
		}
		else
			yyvsp[-3].dlgex->memopt = WRC_MO_MOVEABLE | WRC_MO_PURE | WRC_MO_DISCARDABLE;
		yyvsp[-3].dlgex->x = yyvsp[-11].num;
		yyvsp[-3].dlgex->y = yyvsp[-9].num;
		yyvsp[-3].dlgex->width = yyvsp[-7].num;
		yyvsp[-3].dlgex->height = yyvsp[-5].num;
		if(yyvsp[-4].iptr)
		{
			yyvsp[-3].dlgex->helpid = *(yyvsp[-4].iptr);
			yyvsp[-3].dlgex->gothelpid = TRUE;
			free(yyvsp[-4].iptr);
		}
		yyvsp[-3].dlgex->controls = get_control_head(yyvsp[-1].ctl);
		yyval.dlgex = yyvsp[-3].dlgex;

		assert(yyval.dlgex->style != NULL);
		if(!yyval.dlgex->gotstyle)
		{
			yyval.dlgex->style->or_mask = WS_POPUP;
			yyval.dlgex->gotstyle = TRUE;
		}
		if(yyval.dlgex->title)
			yyval.dlgex->style->or_mask |= WS_CAPTION;
		if(yyval.dlgex->font)
			yyval.dlgex->style->or_mask |= DS_SETFONT;

		yyval.dlgex->style->or_mask &= ~(yyval.dlgex->style->and_mask);
		yyval.dlgex->style->and_mask = 0;

		if(!yyval.dlgex->lvc.language)
			yyval.dlgex->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 121:
#line 1078 "parser.y"
{ yyval.dlgex=new_dialogex(); ;
    break;}
case 122:
#line 1079 "parser.y"
{ yyval.dlgex=dialogex_style(yyvsp[0].style,yyvsp[-2].dlgex); ;
    break;}
case 123:
#line 1080 "parser.y"
{ yyval.dlgex=dialogex_exstyle(yyvsp[0].style,yyvsp[-2].dlgex); ;
    break;}
case 124:
#line 1081 "parser.y"
{ yyval.dlgex=dialogex_caption(yyvsp[0].str,yyvsp[-2].dlgex); ;
    break;}
case 125:
#line 1082 "parser.y"
{ yyval.dlgex=dialogex_font(yyvsp[0].fntid,yyvsp[-1].dlgex); ;
    break;}
case 126:
#line 1083 "parser.y"
{ yyval.dlgex=dialogex_font(yyvsp[0].fntid,yyvsp[-1].dlgex); ;
    break;}
case 127:
#line 1084 "parser.y"
{ yyval.dlgex=dialogex_class(yyvsp[0].nid,yyvsp[-2].dlgex); ;
    break;}
case 128:
#line 1085 "parser.y"
{ yyval.dlgex=dialogex_class(yyvsp[0].nid,yyvsp[-2].dlgex); ;
    break;}
case 129:
#line 1086 "parser.y"
{ yyval.dlgex=dialogex_menu(yyvsp[0].nid,yyvsp[-2].dlgex); ;
    break;}
case 130:
#line 1087 "parser.y"
{ yyval.dlgex=dialogex_language(yyvsp[0].lan,yyvsp[-1].dlgex); ;
    break;}
case 131:
#line 1088 "parser.y"
{ yyval.dlgex=dialogex_characteristics(yyvsp[0].chars,yyvsp[-1].dlgex); ;
    break;}
case 132:
#line 1089 "parser.y"
{ yyval.dlgex=dialogex_version(yyvsp[0].ver,yyvsp[-1].dlgex); ;
    break;}
case 133:
#line 1092 "parser.y"
{ yyval.ctl = NULL; ;
    break;}
case 134:
#line 1093 "parser.y"
{ yyval.ctl=ins_ctrl(-1, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 135:
#line 1094 "parser.y"
{ yyval.ctl=ins_ctrl(CT_EDIT, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 136:
#line 1095 "parser.y"
{ yyval.ctl=ins_ctrl(CT_LISTBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 137:
#line 1096 "parser.y"
{ yyval.ctl=ins_ctrl(CT_COMBOBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 138:
#line 1097 "parser.y"
{ yyval.ctl=ins_ctrl(CT_SCROLLBAR, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 139:
#line 1098 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_CHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 140:
#line 1099 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_DEFPUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 141:
#line 1100 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_GROUPBOX, yyvsp[0].ctl, yyvsp[-2].ctl);;
    break;}
case 142:
#line 1101 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_PUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 143:
#line 1103 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_RADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 144:
#line 1104 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTO3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 145:
#line 1105 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 146:
#line 1106 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTOCHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 147:
#line 1107 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTORADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 148:
#line 1108 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_LEFT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 149:
#line 1109 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_CENTER, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 150:
#line 1110 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_RIGHT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 151:
#line 1112 "parser.y"
{
		yyvsp[0].ctl->title = yyvsp[-7].nid;
		yyvsp[0].ctl->id = yyvsp[-5].num;
		yyvsp[0].ctl->x = yyvsp[-3].num;
		yyvsp[0].ctl->y = yyvsp[-1].num;
		yyval.ctl = ins_ctrl(CT_STATIC, SS_ICON, yyvsp[0].ctl, yyvsp[-9].ctl);
		;
    break;}
case 152:
#line 1123 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-18].nid;
		yyval.ctl->id = yyvsp[-16].num;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-14].nid);
		yyval.ctl->style = yyvsp[-12].style;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->x = yyvsp[-10].num;
		yyval.ctl->y = yyvsp[-8].num;
		yyval.ctl->width = yyvsp[-6].num;
		yyval.ctl->height = yyvsp[-4].num;
		if(yyvsp[-2].style)
		{
			yyval.ctl->exstyle = yyvsp[-2].style;
			yyval.ctl->gotexstyle = TRUE;
		}
		if(yyvsp[-1].iptr)
		{
			yyval.ctl->helpid = *(yyvsp[-1].iptr);
			yyval.ctl->gothelpid = TRUE;
			free(yyvsp[-1].iptr);
		}
		yyval.ctl->extra = yyvsp[0].raw;
		;
    break;}
case 153:
#line 1147 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-15].nid;
		yyval.ctl->id = yyvsp[-13].num;
		yyval.ctl->style = yyvsp[-9].style;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-11].nid);
		yyval.ctl->x = yyvsp[-7].num;
		yyval.ctl->y = yyvsp[-5].num;
		yyval.ctl->width = yyvsp[-3].num;
		yyval.ctl->height = yyvsp[-1].num;
		yyval.ctl->extra = yyvsp[0].raw;
		;
    break;}
case 154:
#line 1163 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = new_name_id();
		yyval.ctl->title->type = name_str;
		yyval.ctl->title->name.s_name = yyvsp[-12].str;
		yyval.ctl->id = yyvsp[-10].num;
		yyval.ctl->x = yyvsp[-8].num;
		yyval.ctl->y = yyvsp[-6].num;
		yyval.ctl->width = yyvsp[-4].num;
		yyval.ctl->height = yyvsp[-2].num;
		if(yyvsp[-1].styles)
		{
			yyval.ctl->style = yyvsp[-1].styles->style;
			yyval.ctl->gotstyle = TRUE;

			if (yyvsp[-1].styles->exstyle)
			{
			    yyval.ctl->exstyle = yyvsp[-1].styles->exstyle;
			    yyval.ctl->gotexstyle = TRUE;
			}
			free(yyvsp[-1].styles);
		}

		yyval.ctl->extra = yyvsp[0].raw;
		;
    break;}
case 155:
#line 1191 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->id = yyvsp[-10].num;
		yyval.ctl->x = yyvsp[-8].num;
		yyval.ctl->y = yyvsp[-6].num;
		yyval.ctl->width = yyvsp[-4].num;
		yyval.ctl->height = yyvsp[-2].num;
		if(yyvsp[-1].styles)
		{
			yyval.ctl->style = yyvsp[-1].styles->style;
			yyval.ctl->gotstyle = TRUE;

			if (yyvsp[-1].styles->exstyle)
			{
			    yyval.ctl->exstyle = yyvsp[-1].styles->exstyle;
			    yyval.ctl->gotexstyle = TRUE;
			}
			free(yyvsp[-1].styles);
		}
		yyval.ctl->extra = yyvsp[0].raw;
		;
    break;}
case 156:
#line 1214 "parser.y"
{ yyval.raw = NULL; ;
    break;}
case 157:
#line 1215 "parser.y"
{ yyval.raw = yyvsp[0].raw; ;
    break;}
case 158:
#line 1218 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 159:
#line 1219 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 160:
#line 1223 "parser.y"
{ yyval.fntid = new_font_id(yyvsp[-7].num, yyvsp[-5].str, yyvsp[-3].num, yyvsp[-1].num); ;
    break;}
case 161:
#line 1230 "parser.y"
{ yyval.fntid = NULL; ;
    break;}
case 162:
#line 1231 "parser.y"
{ yyval.fntid = NULL; ;
    break;}
case 163:
#line 1235 "parser.y"
{
		if(!yyvsp[0].menitm)
			yyerror("Menu must contain items");
		yyval.men = new_menu();
		if(yyvsp[-2].iptr)
		{
			yyval.men->memopt = *(yyvsp[-2].iptr);
			free(yyvsp[-2].iptr);
		}
		else
			yyval.men->memopt = WRC_MO_MOVEABLE | WRC_MO_PURE | WRC_MO_DISCARDABLE;
		yyval.men->items = get_item_head(yyvsp[0].menitm);
		if(yyvsp[-1].lvc)
		{
			yyval.men->lvc = *(yyvsp[-1].lvc);
			free(yyvsp[-1].lvc);
		}
		if(!yyval.men->lvc.language)
			yyval.men->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 164:
#line 1258 "parser.y"
{ yyval.menitm = yyvsp[-1].menitm; ;
    break;}
case 165:
#line 1262 "parser.y"
{yyval.menitm = NULL;;
    break;}
case 166:
#line 1263 "parser.y"
{
		yyval.menitm=new_menu_item();
		yyval.menitm->prev = yyvsp[-5].menitm;
		if(yyvsp[-5].menitm)
			yyvsp[-5].menitm->next = yyval.menitm;
		yyval.menitm->id =  yyvsp[-1].num;
		yyval.menitm->state = yyvsp[0].num;
		yyval.menitm->name = yyvsp[-3].str;
		;
    break;}
case 167:
#line 1272 "parser.y"
{
		yyval.menitm=new_menu_item();
		yyval.menitm->prev = yyvsp[-2].menitm;
		if(yyvsp[-2].menitm)
			yyvsp[-2].menitm->next = yyval.menitm;
		;
    break;}
case 168:
#line 1278 "parser.y"
{
		yyval.menitm = new_menu_item();
		yyval.menitm->prev = yyvsp[-4].menitm;
		if(yyvsp[-4].menitm)
			yyvsp[-4].menitm->next = yyval.menitm;
		yyval.menitm->popup = get_item_head(yyvsp[0].menitm);
		yyval.menitm->name = yyvsp[-2].str;
		;
    break;}
case 169:
#line 1297 "parser.y"
{ yyval.num = 0; ;
    break;}
case 170:
#line 1298 "parser.y"
{ yyval.num = yyvsp[0].num | MF_CHECKED; ;
    break;}
case 171:
#line 1299 "parser.y"
{ yyval.num = yyvsp[0].num | MF_GRAYED; ;
    break;}
case 172:
#line 1300 "parser.y"
{ yyval.num = yyvsp[0].num | MF_HELP; ;
    break;}
case 173:
#line 1301 "parser.y"
{ yyval.num = yyvsp[0].num | MF_DISABLED; ;
    break;}
case 174:
#line 1302 "parser.y"
{ yyval.num = yyvsp[0].num | MF_MENUBARBREAK; ;
    break;}
case 175:
#line 1303 "parser.y"
{ yyval.num = yyvsp[0].num | MF_MENUBREAK; ;
    break;}
case 176:
#line 1307 "parser.y"
{
		if(!win32)
			yywarning("MENUEX not supported in 16-bit mode");
		if(!yyvsp[0].menexitm)
			yyerror("MenuEx must contain items");
		yyval.menex = new_menuex();
		if(yyvsp[-2].iptr)
		{
			yyval.menex->memopt = *(yyvsp[-2].iptr);
			free(yyvsp[-2].iptr);
		}
		else
			yyval.menex->memopt = WRC_MO_MOVEABLE | WRC_MO_PURE | WRC_MO_DISCARDABLE;
		yyval.menex->items = get_itemex_head(yyvsp[0].menexitm);
		if(yyvsp[-1].lvc)
		{
			yyval.menex->lvc = *(yyvsp[-1].lvc);
			free(yyvsp[-1].lvc);
		}
		if(!yyval.menex->lvc.language)
			yyval.menex->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 177:
#line 1332 "parser.y"
{ yyval.menexitm = yyvsp[-1].menexitm; ;
    break;}
case 178:
#line 1336 "parser.y"
{yyval.menexitm = NULL; ;
    break;}
case 179:
#line 1337 "parser.y"
{
		yyval.menexitm = new_menuex_item();
		yyval.menexitm->prev = yyvsp[-3].menexitm;
		if(yyvsp[-3].menexitm)
			yyvsp[-3].menexitm->next = yyval.menexitm;
		yyval.menexitm->name = yyvsp[-1].str;
		yyval.menexitm->id = yyvsp[0].exopt->id;
		yyval.menexitm->type = yyvsp[0].exopt->type;
		yyval.menexitm->state = yyvsp[0].exopt->state;
		yyval.menexitm->helpid = yyvsp[0].exopt->helpid;
		yyval.menexitm->gotid = yyvsp[0].exopt->gotid;
		yyval.menexitm->gottype = yyvsp[0].exopt->gottype;
		yyval.menexitm->gotstate = yyvsp[0].exopt->gotstate;
		yyval.menexitm->gothelpid = yyvsp[0].exopt->gothelpid;
		free(yyvsp[0].exopt);
		;
    break;}
case 180:
#line 1353 "parser.y"
{
		yyval.menexitm = new_menuex_item();
		yyval.menexitm->prev = yyvsp[-2].menexitm;
		if(yyvsp[-2].menexitm)
			yyvsp[-2].menexitm->next = yyval.menexitm;
		;
    break;}
case 181:
#line 1359 "parser.y"
{
		yyval.menexitm = new_menuex_item();
		yyval.menexitm->prev = yyvsp[-4].menexitm;
		if(yyvsp[-4].menexitm)
			yyvsp[-4].menexitm->next = yyval.menexitm;
		yyval.menexitm->popup = get_itemex_head(yyvsp[0].menexitm);
		yyval.menexitm->name = yyvsp[-2].str;
		yyval.menexitm->id = yyvsp[-1].exopt->id;
		yyval.menexitm->type = yyvsp[-1].exopt->type;
		yyval.menexitm->state = yyvsp[-1].exopt->state;
		yyval.menexitm->helpid = yyvsp[-1].exopt->helpid;
		yyval.menexitm->gotid = yyvsp[-1].exopt->gotid;
		yyval.menexitm->gottype = yyvsp[-1].exopt->gottype;
		yyval.menexitm->gotstate = yyvsp[-1].exopt->gotstate;
		yyval.menexitm->gothelpid = yyvsp[-1].exopt->gothelpid;
		free(yyvsp[-1].exopt);
		;
    break;}
case 182:
#line 1379 "parser.y"
{ yyval.exopt = new_itemex_opt(0, 0, 0, 0); ;
    break;}
case 183:
#line 1380 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[0].num, 0, 0, 0);
		yyval.exopt->gotid = TRUE;
		;
    break;}
case 184:
#line 1384 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-3].iptr ? *(yyvsp[-3].iptr) : 0, yyvsp[-1].iptr ? *(yyvsp[-1].iptr) : 0, yyvsp[0].num, 0);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		if(yyvsp[-3].iptr) free(yyvsp[-3].iptr);
		if(yyvsp[-1].iptr) free(yyvsp[-1].iptr);
		;
    break;}
case 185:
#line 1392 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-4].iptr ? *(yyvsp[-4].iptr) : 0, yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		if(yyvsp[-4].iptr) free(yyvsp[-4].iptr);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		;
    break;}
case 186:
#line 1403 "parser.y"
{ yyval.exopt = new_itemex_opt(0, 0, 0, 0); ;
    break;}
case 187:
#line 1404 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[0].num, 0, 0, 0);
		yyval.exopt->gotid = TRUE;
		;
    break;}
case 188:
#line 1408 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0, 0);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		;
    break;}
case 189:
#line 1414 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-4].iptr ? *(yyvsp[-4].iptr) : 0, yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0);
		if(yyvsp[-4].iptr) free(yyvsp[-4].iptr);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		;
    break;}
case 190:
#line 1422 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-6].iptr ? *(yyvsp[-6].iptr) : 0, yyvsp[-4].iptr ? *(yyvsp[-4].iptr) : 0, yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num);
		if(yyvsp[-6].iptr) free(yyvsp[-6].iptr);
		if(yyvsp[-4].iptr) free(yyvsp[-4].iptr);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		yyval.exopt->gothelpid = TRUE;
		;
    break;}
case 191:
#line 1442 "parser.y"
{
		if(!yyvsp[-1].stt)
		{
			yyerror("Stringtable must have at least one entry");
		}
		else
		{
			stringtable_t *stt;
			/* Check if we added to a language table or created
			 * a new one.
			 */
			 for(stt = sttres; stt; stt = stt->next)
			 {
				if(stt == tagstt)
					break;
			 }
			 if(!stt)
			 {
				/* It is a new one */
				if(sttres)
				{
					sttres->prev = tagstt;
					tagstt->next = sttres;
					sttres = tagstt;
				}
				else
					sttres = tagstt;
			 }
			 /* Else were done */
		}
		if(tagstt_memopt)
		{
			free(tagstt_memopt);
			tagstt_memopt = NULL;
		}

		yyval.stt = tagstt;
		;
    break;}
case 192:
#line 1483 "parser.y"
{
		if((tagstt = find_stringtable(yyvsp[0].lvc)) == NULL)
			tagstt = new_stringtable(yyvsp[0].lvc);
		tagstt_memopt = yyvsp[-1].iptr;
		tagstt_version = yyvsp[0].lvc->version;
		tagstt_characts = yyvsp[0].lvc->characts;
		if(yyvsp[0].lvc)
			free(yyvsp[0].lvc);
		;
    break;}
case 193:
#line 1494 "parser.y"
{ yyval.stt = NULL; ;
    break;}
case 194:
#line 1495 "parser.y"
{
		int i;
		assert(tagstt != NULL);
		if(yyvsp[-2].num > 65535 || yyvsp[-2].num < -32768)
			yyerror("Stringtable entry's ID out of range (%d)", yyvsp[-2].num);
		/* Search for the ID */
		for(i = 0; i < tagstt->nentries; i++)
		{
			if(tagstt->entries[i].id == yyvsp[-2].num)
				yyerror("Stringtable ID %d already in use", yyvsp[-2].num);
		}
		/* If we get here, then we have a new unique entry */
		tagstt->nentries++;
		tagstt->entries = xrealloc(tagstt->entries, sizeof(tagstt->entries[0]) * tagstt->nentries);
		tagstt->entries[tagstt->nentries-1].id = yyvsp[-2].num;
		tagstt->entries[tagstt->nentries-1].str = yyvsp[0].str;
		if(tagstt_memopt)
			tagstt->entries[tagstt->nentries-1].memopt = *tagstt_memopt;
		else
			tagstt->entries[tagstt->nentries-1].memopt = WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE | WRC_MO_PURE;
		tagstt->entries[tagstt->nentries-1].version = tagstt_version;
		tagstt->entries[tagstt->nentries-1].characts = tagstt_characts;

		if(pedantic && !yyvsp[0].str->size)
			yywarning("Zero length strings make no sense");
		if(!win32 && yyvsp[0].str->size > 254)
			yyerror("Stringtable entry more than 254 characters");
		if(win32 && yyvsp[0].str->size > 65534) /* Hmm..., does this happen? */
			yyerror("Stringtable entry more than 65534 characters (probably something else that went wrong)");
		yyval.stt = tagstt;
		;
    break;}
case 197:
#line 1535 "parser.y"
{
		yyval.veri = yyvsp[-3].veri;
		if(yyvsp[-4].iptr)
		{
			yyval.veri->memopt = *(yyvsp[-4].iptr);
			free(yyvsp[-4].iptr);
		}
		else
			yyval.veri->memopt = WRC_MO_MOVEABLE | (win32 ? WRC_MO_PURE : 0);
		yyval.veri->blocks = get_ver_block_head(yyvsp[-1].blk);
		/* Set language; there is no version or characteristics */
		yyval.veri->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 198:
#line 1551 "parser.y"
{ yyval.veri = new_versioninfo(); ;
    break;}
case 199:
#line 1552 "parser.y"
{
		if(yyvsp[-8].veri->gotit.fv)
			yyerror("FILEVERSION already defined");
		yyval.veri = yyvsp[-8].veri;
		yyval.veri->filever_maj1 = yyvsp[-6].num;
		yyval.veri->filever_maj2 = yyvsp[-4].num;
		yyval.veri->filever_min1 = yyvsp[-2].num;
		yyval.veri->filever_min2 = yyvsp[0].num;
		yyval.veri->gotit.fv = 1;
		;
    break;}
case 200:
#line 1562 "parser.y"
{
		if(yyvsp[-8].veri->gotit.pv)
			yyerror("PRODUCTVERSION already defined");
		yyval.veri = yyvsp[-8].veri;
		yyval.veri->prodver_maj1 = yyvsp[-6].num;
		yyval.veri->prodver_maj2 = yyvsp[-4].num;
		yyval.veri->prodver_min1 = yyvsp[-2].num;
		yyval.veri->prodver_min2 = yyvsp[0].num;
		yyval.veri->gotit.pv = 1;
		;
    break;}
case 201:
#line 1572 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ff)
			yyerror("FILEFLAGS already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileflags = yyvsp[0].num;
		yyval.veri->gotit.ff = 1;
		;
    break;}
case 202:
#line 1579 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ffm)
			yyerror("FILEFLAGSMASK already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileflagsmask = yyvsp[0].num;
		yyval.veri->gotit.ffm = 1;
		;
    break;}
case 203:
#line 1586 "parser.y"
{
		if(yyvsp[-2].veri->gotit.fo)
			yyerror("FILEOS already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileos = yyvsp[0].num;
		yyval.veri->gotit.fo = 1;
		;
    break;}
case 204:
#line 1593 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ft)
			yyerror("FILETYPE already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->filetype = yyvsp[0].num;
		yyval.veri->gotit.ft = 1;
		;
    break;}
case 205:
#line 1600 "parser.y"
{
		if(yyvsp[-2].veri->gotit.fst)
			yyerror("FILESUBTYPE already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->filesubtype = yyvsp[0].num;
		yyval.veri->gotit.fst = 1;
		;
    break;}
case 206:
#line 1610 "parser.y"
{ yyval.blk = NULL; ;
    break;}
case 207:
#line 1611 "parser.y"
{
		yyval.blk = yyvsp[0].blk;
		yyval.blk->prev = yyvsp[-1].blk;
		if(yyvsp[-1].blk)
			yyvsp[-1].blk->next = yyval.blk;
		;
    break;}
case 208:
#line 1620 "parser.y"
{
		yyval.blk = new_ver_block();
		yyval.blk->name = yyvsp[-3].str;
		yyval.blk->values = get_ver_value_head(yyvsp[-1].val);
		;
    break;}
case 209:
#line 1628 "parser.y"
{ yyval.val = NULL; ;
    break;}
case 210:
#line 1629 "parser.y"
{
		yyval.val = yyvsp[0].val;
		yyval.val->prev = yyvsp[-1].val;
		if(yyvsp[-1].val)
			yyvsp[-1].val->next = yyval.val;
		;
    break;}
case 211:
#line 1638 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_block;
		yyval.val->value.block = yyvsp[0].blk;
		;
    break;}
case 212:
#line 1643 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_str;
		yyval.val->key = yyvsp[-2].str;
		yyval.val->value.str = yyvsp[0].str;
		;
    break;}
case 213:
#line 1649 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_words;
		yyval.val->key = yyvsp[-2].str;
		yyval.val->value.words = yyvsp[0].verw;
		;
    break;}
case 214:
#line 1658 "parser.y"
{ yyval.verw = new_ver_words(yyvsp[0].num); ;
    break;}
case 215:
#line 1659 "parser.y"
{ yyval.verw = add_ver_words(yyvsp[-2].verw, yyvsp[0].num); ;
    break;}
case 216:
#line 1663 "parser.y"
{
		int nitems;
		toolbar_item_t *items = get_tlbr_buttons_head(yyvsp[-1].tlbarItems, &nitems);
		yyval.tlbar = new_toolbar(yyvsp[-6].num, yyvsp[-4].num, items, nitems);
		if(yyvsp[-7].iptr)
		{
			yyval.tlbar->memopt = *(yyvsp[-7].iptr);
			free(yyvsp[-7].iptr); 
		}
		else
		{
			yyval.tlbar->memopt = WRC_MO_MOVEABLE | WRC_MO_PURE;
		}
		if(yyvsp[-3].lvc)
		{
			yyval.tlbar->lvc = *(yyvsp[-3].lvc);
			free(yyvsp[-3].lvc);
		}
		if(!yyval.tlbar->lvc.language)
		{
			yyval.tlbar->lvc.language = dup_language(currentlanguage);
		}
		;
    break;}
case 217:
#line 1689 "parser.y"
{ yyval.tlbarItems = NULL; ;
    break;}
case 218:
#line 1690 "parser.y"
{         
		toolbar_item_t *idrec = new_toolbar_item();
		idrec->id = yyvsp[0].num;
		yyval.tlbarItems = ins_tlbr_button(yyvsp[-2].tlbarItems, idrec); 
		;
    break;}
case 219:
#line 1695 "parser.y"
{         
		toolbar_item_t *idrec = new_toolbar_item();
		idrec->id = 0;
		yyval.tlbarItems = ins_tlbr_button(yyvsp[-1].tlbarItems, idrec); 
	;
    break;}
case 220:
#line 1704 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 221:
#line 1705 "parser.y"
{
		if(yyvsp[-1].iptr)
		{
			*(yyvsp[-1].iptr) |= *(yyvsp[0].iptr);
			yyval.iptr = yyvsp[-1].iptr;
			free(yyvsp[0].iptr);
		}
		else
			yyval.iptr = yyvsp[0].iptr;
		;
    break;}
case 222:
#line 1715 "parser.y"
{
		if(yyvsp[-1].iptr)
		{
			*(yyvsp[-1].iptr) &= *(yyvsp[0].iptr);
			yyval.iptr = yyvsp[-1].iptr;
			free(yyvsp[0].iptr);
		}
		else
		{
			*yyvsp[0].iptr &= WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE | WRC_MO_PURE;
			yyval.iptr = yyvsp[0].iptr;
		}
		;
    break;}
case 223:
#line 1730 "parser.y"
{ yyval.iptr = new_int(WRC_MO_PRELOAD); ;
    break;}
case 224:
#line 1731 "parser.y"
{ yyval.iptr = new_int(WRC_MO_MOVEABLE); ;
    break;}
case 225:
#line 1732 "parser.y"
{ yyval.iptr = new_int(WRC_MO_DISCARDABLE); ;
    break;}
case 226:
#line 1733 "parser.y"
{ yyval.iptr = new_int(WRC_MO_PURE); ;
    break;}
case 227:
#line 1736 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_PRELOAD); ;
    break;}
case 228:
#line 1737 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_MOVEABLE); ;
    break;}
case 229:
#line 1738 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_PURE); ;
    break;}
case 230:
#line 1742 "parser.y"
{ yyval.lvc = new_lvc(); ;
    break;}
case 231:
#line 1743 "parser.y"
{
		if(!win32)
			yywarning("LANGUAGE not supported in 16-bit mode");
		if(yyvsp[-1].lvc->language)
			yyerror("Language already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->language = yyvsp[0].lan;
		;
    break;}
case 232:
#line 1751 "parser.y"
{
		if(!win32)
			yywarning("CHARACTERISTICS not supported in 16-bit mode");
		if(yyvsp[-1].lvc->characts)
			yyerror("Characteristics already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->characts = yyvsp[0].chars;
		;
    break;}
case 233:
#line 1759 "parser.y"
{
		if(!win32)
			yywarning("VERSION not supported in 16-bit mode");
		if(yyvsp[-1].lvc->version)
			yyerror("Version already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->version = yyvsp[0].ver;
		;
    break;}
case 234:
#line 1777 "parser.y"
{ yyval.lan = new_language(yyvsp[-2].num, yyvsp[0].num); ;
    break;}
case 235:
#line 1781 "parser.y"
{ yyval.chars = new_characts(yyvsp[0].num); ;
    break;}
case 236:
#line 1785 "parser.y"
{ yyval.ver = new_version(yyvsp[0].num); ;
    break;}
case 237:
#line 1789 "parser.y"
{
		if(yyvsp[-3].lvc)
		{
			yyvsp[-1].raw->lvc = *(yyvsp[-3].lvc);
			free(yyvsp[-3].lvc);
		}

		if(!yyvsp[-1].raw->lvc.language)
			yyvsp[-1].raw->lvc.language = dup_language(currentlanguage);

		yyval.raw = yyvsp[-1].raw;
		;
    break;}
case 238:
#line 1804 "parser.y"
{ yyval.raw = yyvsp[0].raw; ;
    break;}
case 239:
#line 1805 "parser.y"
{ yyval.raw = int2raw_data(yyvsp[0].num); ;
    break;}
case 240:
#line 1806 "parser.y"
{ yyval.raw = long2raw_data(yyvsp[0].num); ;
    break;}
case 241:
#line 1807 "parser.y"
{ yyval.raw = str2raw_data(yyvsp[0].str); ;
    break;}
case 242:
#line 1808 "parser.y"
{ yyval.raw = merge_raw_data(yyvsp[-2].raw, yyvsp[0].raw); free(yyvsp[0].raw->data); free(yyvsp[0].raw); ;
    break;}
case 243:
#line 1809 "parser.y"
{ yyval.raw = merge_raw_data_int(yyvsp[-2].raw, yyvsp[0].num); ;
    break;}
case 244:
#line 1810 "parser.y"
{ yyval.raw = merge_raw_data_long(yyvsp[-2].raw, yyvsp[0].num); ;
    break;}
case 245:
#line 1811 "parser.y"
{ yyval.raw = merge_raw_data_str(yyvsp[-2].raw, yyvsp[0].str); ;
    break;}
case 246:
#line 1815 "parser.y"
{
		yyval.raw = load_file(yyvsp[0].str);
		yyval.raw->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 247:
#line 1819 "parser.y"
{ yyval.raw = yyvsp[0].raw; ;
    break;}
case 248:
#line 1826 "parser.y"
{ yyval.iptr = 0; ;
    break;}
case 249:
#line 1827 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 250:
#line 1831 "parser.y"
{ yyval.num = (yyvsp[0].num); ;
    break;}
case 251:
#line 1834 "parser.y"
{ yyval.num = (yyvsp[-2].num) + (yyvsp[0].num); ;
    break;}
case 252:
#line 1835 "parser.y"
{ yyval.num = (yyvsp[-2].num) - (yyvsp[0].num); ;
    break;}
case 253:
#line 1836 "parser.y"
{ yyval.num = (yyvsp[-2].num) | (yyvsp[0].num); ;
    break;}
case 254:
#line 1837 "parser.y"
{ yyval.num = (yyvsp[-2].num) & (yyvsp[0].num); ;
    break;}
case 255:
#line 1838 "parser.y"
{ yyval.num = (yyvsp[-2].num) * (yyvsp[0].num); ;
    break;}
case 256:
#line 1839 "parser.y"
{ yyval.num = (yyvsp[-2].num) / (yyvsp[0].num); ;
    break;}
case 257:
#line 1840 "parser.y"
{ yyval.num = (yyvsp[-2].num) ^ (yyvsp[0].num); ;
    break;}
case 258:
#line 1841 "parser.y"
{ yyval.num = ~(yyvsp[0].num); ;
    break;}
case 259:
#line 1842 "parser.y"
{ yyval.num = -(yyvsp[0].num); ;
    break;}
case 260:
#line 1843 "parser.y"
{ yyval.num = yyvsp[0].num; ;
    break;}
case 261:
#line 1844 "parser.y"
{ yyval.num = yyvsp[-1].num; ;
    break;}
case 262:
#line 1845 "parser.y"
{ yyval.num = yyvsp[0].num; ;
    break;}
case 263:
#line 1846 "parser.y"
{ yyval.num = ~(yyvsp[0].num); ;
    break;}
case 264:
#line 1849 "parser.y"
{ yyval.num = yyvsp[0].num; ;
    break;}
case 265:
#line 1850 "parser.y"
{ yyval.num = yyvsp[0].num; ;
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
#line 1853 "parser.y"

/* Dialog specific functions */
static dialog_t *dialog_style(style_t * st, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->style == NULL)
	{
		dlg->style = new_style(0,0);
	}

	if(dlg->gotstyle)
	{
		yywarning("Style already defined, or-ing together");
	}
	else
	{
		dlg->style->or_mask = 0;
		dlg->style->and_mask = 0;
	}
	dlg->style->or_mask |= st->or_mask;
	dlg->style->and_mask |= st->and_mask;
	dlg->gotstyle = TRUE;
	free(st);
	return dlg;
}

static dialog_t *dialog_exstyle(style_t *st, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->exstyle == NULL)
	{
		dlg->exstyle = new_style(0,0);
	}

	if(dlg->gotexstyle)
	{
		yywarning("ExStyle already defined, or-ing together");
	}
	else
	{
		dlg->exstyle->or_mask = 0;
		dlg->exstyle->and_mask = 0;
	}
	dlg->exstyle->or_mask |= st->or_mask;
	dlg->exstyle->and_mask |= st->and_mask;
	dlg->gotexstyle = TRUE;
	free(st);
	return dlg;
}

static dialog_t *dialog_caption(string_t *s, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->title)
		yyerror("Caption already defined");
	dlg->title = s;
	return dlg;
}

static dialog_t *dialog_font(font_id_t *f, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->font)
		yyerror("Font already defined");
	dlg->font = f;
	return dlg;
}

static dialog_t *dialog_class(name_id_t *n, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->dlgclass)
		yyerror("Class already defined");
	dlg->dlgclass = n;
	return dlg;
}

static dialog_t *dialog_menu(name_id_t *m, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->menu)
		yyerror("Menu already defined");
	dlg->menu = m;
	return dlg;
}

static dialog_t *dialog_language(language_t *l, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.language)
		yyerror("Language already defined");
	dlg->lvc.language = l;
	return dlg;
}

static dialog_t *dialog_characteristics(characts_t *c, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.characts)
		yyerror("Characteristics already defined");
	dlg->lvc.characts = c;
	return dlg;
}

static dialog_t *dialog_version(version_t *v, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.version)
		yyerror("Version already defined");
	dlg->lvc.version = v;
	return dlg;
}

/* Controls specific functions */
static control_t *ins_ctrl(int type, int special_style, control_t *ctrl, control_t *prev)
{
	/* Hm... this seems to be jammed in at all time... */
	int defaultstyle = WS_CHILD | WS_VISIBLE;

	assert(ctrl != NULL);
	ctrl->prev = prev;

	if(prev)
		prev->next = ctrl;

	if(type != -1)
	{
		ctrl->ctlclass = new_name_id();
		ctrl->ctlclass->type = name_ord;
		ctrl->ctlclass->name.i_name = type;
	}

	switch(type)
	{
	case CT_BUTTON:
		if(special_style != BS_GROUPBOX && special_style != BS_RADIOBUTTON)
			defaultstyle |= WS_TABSTOP;
		break;
	case CT_EDIT:
		defaultstyle |= WS_TABSTOP | WS_BORDER;
		break;
	case CT_LISTBOX:
		defaultstyle |= LBS_NOTIFY | WS_BORDER;
		break;
	case CT_COMBOBOX:
		defaultstyle |= CBS_SIMPLE;
		break;
	case CT_STATIC:
		if(special_style == SS_CENTER || special_style == SS_LEFT || special_style == SS_RIGHT)
			defaultstyle |= WS_GROUP;
		break;
	}

	if(!ctrl->gotstyle)	/* Handle default style setting */
	{
		switch(type)
		{
		case CT_EDIT:
			defaultstyle |= ES_LEFT;
			break;
		case CT_LISTBOX:
			defaultstyle |= LBS_NOTIFY;
			break;
		case CT_COMBOBOX:
			defaultstyle |= CBS_SIMPLE | WS_TABSTOP;
			break;
		case CT_SCROLLBAR:
			defaultstyle |= SBS_HORZ;
			break;
		case CT_BUTTON:
			switch(special_style)
			{
			case BS_CHECKBOX:
			case BS_DEFPUSHBUTTON:
			case BS_PUSHBUTTON:
			case BS_GROUPBOX:
/*			case BS_PUSHBOX:	*/
			case BS_AUTORADIOBUTTON:
			case BS_AUTO3STATE:
			case BS_3STATE:
			case BS_AUTOCHECKBOX:
				defaultstyle |= WS_TABSTOP;
				break;
			default:
				yywarning("Unknown default button control-style 0x%08x", special_style);
			case BS_RADIOBUTTON:
				break;
			}
			break;

		case CT_STATIC:
			switch(special_style)
			{
			case SS_LEFT:
			case SS_RIGHT:
			case SS_CENTER:
				defaultstyle |= WS_GROUP;
				break;
			case SS_ICON:	/* Special case */
				break;
			default:
				yywarning("Unknown default static control-style 0x%08x", special_style);
				break;
			}
			break;
		case -1:	/* Generic control */
			goto byebye;

		default:
			yyerror("Internal error (report this): Got weird control type 0x%08x", type);
		}
	}

	/* The SS_ICON flag is always forced in for icon controls */
	if(type == CT_STATIC && special_style == SS_ICON)
		defaultstyle |= SS_ICON;

	if (!ctrl->gotstyle)
		ctrl->style = new_style(0,0);

	/* combine all styles */
	ctrl->style->or_mask = ctrl->style->or_mask | defaultstyle | special_style;
	ctrl->gotstyle = TRUE;
byebye:
	/* combine with NOT mask */
	if (ctrl->gotstyle)
	{
		ctrl->style->or_mask &= ~(ctrl->style->and_mask);
		ctrl->style->and_mask = 0;
	}
	if (ctrl->gotexstyle)
	{
		ctrl->exstyle->or_mask &= ~(ctrl->exstyle->and_mask);
		ctrl->exstyle->and_mask = 0;
	}
	return ctrl;
}

static name_id_t *convert_ctlclass(name_id_t *cls)
{
	char *cc = NULL;
	int iclass;

	if(cls->type == name_ord)
		return cls;
	assert(cls->type == name_str);
	if(cls->type == str_unicode)
	{
		yyerror("Don't yet support unicode class comparison");
	}
	else
		cc = cls->name.s_name->str.cstr;

	if(!strcasecmp("BUTTON", cc))
		iclass = CT_BUTTON;
	else if(!strcasecmp("COMBOBOX", cc))
		iclass = CT_COMBOBOX;
	else if(!strcasecmp("LISTBOX", cc))
		iclass = CT_LISTBOX;
	else if(!strcasecmp("EDIT", cc))
		iclass = CT_EDIT;
	else if(!strcasecmp("STATIC", cc))
		iclass = CT_STATIC;
	else if(!strcasecmp("SCROLLBAR", cc))
		iclass = CT_SCROLLBAR;
	else
		return cls;	/* No default, return user controlclass */

	free(cls->name.s_name->str.cstr);
	free(cls->name.s_name);
	cls->type = name_ord;
	cls->name.i_name = iclass;
	return cls;
}

/* DialogEx specific functions */
static dialogex_t *dialogex_style(style_t * st, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->style == NULL)
	{
		dlg->style = new_style(0,0);
	}

	if(dlg->gotstyle)
	{
		yywarning("Style already defined, or-ing together");
	}
	else
	{
		dlg->style->or_mask = 0;
		dlg->style->and_mask = 0;
	}
	dlg->style->or_mask |= st->or_mask;
	dlg->style->and_mask |= st->and_mask;
	dlg->gotstyle = TRUE;
	free(st);
	return dlg;
}

static dialogex_t *dialogex_exstyle(style_t * st, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->exstyle == NULL)
	{
		dlg->exstyle = new_style(0,0);
	}

	if(dlg->gotexstyle)
	{
		yywarning("ExStyle already defined, or-ing together");
	}
	else
	{
		dlg->exstyle->or_mask = 0;
		dlg->exstyle->and_mask = 0;
	}
	dlg->exstyle->or_mask |= st->or_mask;
	dlg->exstyle->and_mask |= st->and_mask;
	dlg->gotexstyle = TRUE;
	free(st);
	return dlg;
}

static dialogex_t *dialogex_caption(string_t *s, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->title)
		yyerror("Caption already defined");
	dlg->title = s;
	return dlg;
}

static dialogex_t *dialogex_font(font_id_t *f, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->font)
		yyerror("Font already defined");
	dlg->font = f;
	return dlg;
}

static dialogex_t *dialogex_class(name_id_t *n, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->dlgclass)
		yyerror("Class already defined");
	dlg->dlgclass = n;
	return dlg;
}

static dialogex_t *dialogex_menu(name_id_t *m, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->menu)
		yyerror("Menu already defined");
	dlg->menu = m;
	return dlg;
}

static dialogex_t *dialogex_language(language_t *l, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.language)
		yyerror("Language already defined");
	dlg->lvc.language = l;
	return dlg;
}

static dialogex_t *dialogex_characteristics(characts_t *c, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.characts)
		yyerror("Characteristics already defined");
	dlg->lvc.characts = c;
	return dlg;
}

static dialogex_t *dialogex_version(version_t *v, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.version)
		yyerror("Version already defined");
	dlg->lvc.version = v;
	return dlg;
}

/* Accelerator specific functions */
static event_t *add_event(int key, int id, int flags, event_t *prev)
{
	event_t *ev = new_event();

	if((flags & (WRC_AF_VIRTKEY | WRC_AF_ASCII)) == (WRC_AF_VIRTKEY | WRC_AF_ASCII))
		yyerror("Cannot use both ASCII and VIRTKEY");

	ev->key = key;
	ev->id = id;
	ev->flags = flags & ~WRC_AF_ASCII;
	ev->prev = prev;
	if(prev)
		prev->next = ev;
	return ev;
}

static event_t *add_string_event(string_t *key, int id, int flags, event_t *prev)
{
	int keycode = 0;
	event_t *ev = new_event();

	if(key->type != str_char)
		yyerror("Key code must be an ascii string");

	if((flags & WRC_AF_VIRTKEY) && (!isupper(key->str.cstr[0] & 0xff) && !isdigit(key->str.cstr[0] & 0xff)))
		yyerror("VIRTKEY code is not equal to ascii value");

	if(key->str.cstr[0] == '^' && (flags & WRC_AF_CONTROL) != 0)
	{
		yyerror("Cannot use both '^' and CONTROL modifier");
	}
	else if(key->str.cstr[0] == '^')
	{
		keycode = toupper(key->str.cstr[1]) - '@';
		if(keycode >= ' ')
			yyerror("Control-code out of range");
	}
	else
		keycode = key->str.cstr[0];
	ev->key = keycode;
	ev->id = id;
	ev->flags = flags & ~WRC_AF_ASCII;
	ev->prev = prev;
	if(prev)
		prev->next = ev;
	return ev;
}

/* MenuEx specific functions */
static itemex_opt_t *new_itemex_opt(int id, int type, int state, int helpid)
{
	itemex_opt_t *opt = (itemex_opt_t *)xmalloc(sizeof(itemex_opt_t));
	opt->id = id;
	opt->type = type;
	opt->state = state;
	opt->helpid = helpid;
	return opt;
}

/* Raw data functions */
static raw_data_t *load_file(string_t *name)
{
	FILE *fp;
	raw_data_t *rd;
	if(name->type != str_char)
		yyerror("Filename must be ASCII string");
		
	fp = open_include(name->str.cstr, 1, NULL);
	if(!fp)
		yyerror("Cannot open file %s", name->str.cstr);
	rd = new_raw_data();
	fseek(fp, 0, SEEK_END);
	rd->size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	rd->data = (char *)xmalloc(rd->size);
	fread(rd->data, rd->size, 1, fp);
	fclose(fp);
	return rd;
}

static raw_data_t *int2raw_data(int i)
{
	raw_data_t *rd;

	if((int)((short)i) != i)
		yywarning("Integer constant out of 16bit range (%d), truncated to %d\n", i, (short)i);

	rd = new_raw_data();
	rd->size = sizeof(short);
	rd->data = (char *)xmalloc(rd->size);
	switch(byteorder)
	{
#ifdef WORDS_BIGENDIAN
	default:
#endif
	case WRC_BO_BIG:
		rd->data[0] = HIBYTE(i);
		rd->data[1] = LOBYTE(i);
		break;

#ifndef WORDS_BIGENDIAN
	default:
#endif
	case WRC_BO_LITTLE:
		rd->data[1] = HIBYTE(i);
		rd->data[0] = LOBYTE(i);
		break;
	}
	return rd;
}

static raw_data_t *long2raw_data(int i)
{
	raw_data_t *rd;
	rd = new_raw_data();
	rd->size = sizeof(int);
	rd->data = (char *)xmalloc(rd->size);
	switch(byteorder)
	{
#ifdef WORDS_BIGENDIAN
	default:
#endif
	case WRC_BO_BIG:
		rd->data[0] = HIBYTE(HIWORD(i));
		rd->data[1] = LOBYTE(HIWORD(i));
		rd->data[2] = HIBYTE(LOWORD(i));
		rd->data[3] = LOBYTE(LOWORD(i));
		break;

#ifndef WORDS_BIGENDIAN
	default:
#endif
	case WRC_BO_LITTLE:
		rd->data[3] = HIBYTE(HIWORD(i));
		rd->data[2] = LOBYTE(HIWORD(i));
		rd->data[1] = HIBYTE(LOWORD(i));
		rd->data[0] = LOBYTE(LOWORD(i));
		break;
	}
	return rd;
}

static raw_data_t *str2raw_data(string_t *str)
{
	raw_data_t *rd;
	rd = new_raw_data();
	rd->size = str->size * (str->type == str_char ? 1 : 2);
	rd->data = (char *)xmalloc(rd->size);
	if(str->type == str_char)
		memcpy(rd->data, str->str.cstr, rd->size);
	else if(str->type == str_unicode)
	{
		int i;
		switch(byteorder)
		{
#ifdef WORDS_BIGENDIAN
		default:
#endif
		case WRC_BO_BIG:
			for(i = 0; i < str->size; i++)
			{
				rd->data[2*i + 0] = HIBYTE((WORD)str->str.wstr[i]);
				rd->data[2*i + 1] = LOBYTE((WORD)str->str.wstr[i]);
			}
			break;
#ifndef WORDS_BIGENDIAN
		default:
#endif
		case WRC_BO_LITTLE:
			for(i = 0; i < str->size; i++)
			{
				rd->data[2*i + 1] = HIBYTE((WORD)str->str.wstr[i]);
				rd->data[2*i + 0] = LOBYTE((WORD)str->str.wstr[i]);
			}
			break;
		}
	}
	else
		internal_error(__FILE__, __LINE__, "Invalid stringtype");
	return rd;
}

static raw_data_t *merge_raw_data(raw_data_t *r1, raw_data_t *r2)
{
	r1->data = xrealloc(r1->data, r1->size + r2->size);
	memcpy(r1->data + r1->size, r2->data, r2->size);
	r1->size += r2->size;
	return r1;
}

static raw_data_t *merge_raw_data_int(raw_data_t *r1, int i)
{
	raw_data_t *t = int2raw_data(i);
	merge_raw_data(r1, t);
	free(t->data);
	free(t);
	return r1;
}

static raw_data_t *merge_raw_data_long(raw_data_t *r1, int i)
{
	raw_data_t *t = long2raw_data(i);
	merge_raw_data(r1, t);
	free(t->data);
	free(t);
	return r1;
}

static raw_data_t *merge_raw_data_str(raw_data_t *r1, string_t *str)
{
	raw_data_t *t = str2raw_data(str);
	merge_raw_data(r1, t);
	free(t->data);
	free(t);
	return r1;
}

/* Function the go back in a list to get the head */
static menu_item_t *get_item_head(menu_item_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

static menuex_item_t *get_itemex_head(menuex_item_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

static resource_t *get_resource_head(resource_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

static ver_block_t *get_ver_block_head(ver_block_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

static ver_value_t *get_ver_value_head(ver_value_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

static control_t *get_control_head(control_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

static event_t *get_event_head(event_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

/* Find a stringtable with given language */
static stringtable_t *find_stringtable(lvc_t *lvc)
{
	stringtable_t *stt;

	assert(lvc != NULL);

	if(!lvc->language)
		lvc->language = dup_language(currentlanguage);

	for(stt = sttres; stt; stt = stt->next)
	{
		if(stt->lvc.language->id == lvc->language->id
		&& stt->lvc.language->sub == lvc->language->sub)
		{
			/* Found a table with the same language */
			/* The version and characteristics are now handled
			 * in the generation of the individual stringtables.
			 * This enables localized analysis.
			if((stt->lvc.version && lvc->version && *(stt->lvc.version) != *(lvc->version))
			|| (!stt->lvc.version && lvc->version)
			|| (stt->lvc.version && !lvc->version))
				yywarning("Stringtable's versions are not the same, using first definition");

			if((stt->lvc.characts && lvc->characts && *(stt->lvc.characts) != *(lvc->characts))
			|| (!stt->lvc.characts && lvc->characts)
			|| (stt->lvc.characts && !lvc->characts))
				yywarning("Stringtable's characteristics are not the same, using first definition");
			*/
			return stt;
		}
	}
	return NULL;
}

/* qsort sorting function for string table entries */
#define STE(p)	((stt_entry_t *)(p))
static int sort_stt_entry(const void *e1, const void *e2)
{
	return STE(e1)->id - STE(e2)->id;
}
#undef STE

static resource_t *build_stt_resources(stringtable_t *stthead)
{
	stringtable_t *stt;
	stringtable_t *newstt;
	resource_t *rsc;
	resource_t *rsclist = NULL;
	resource_t *rsctail = NULL;
	int i;
	int j;
	DWORD andsum;
	DWORD orsum;
	characts_t *characts;
	version_t *version;

	if(!stthead)
		return NULL;

	/* For all languages defined */
	for(stt = stthead; stt; stt = stt->next)
	{
		assert(stt->nentries > 0);

		/* Sort the entries */
		if(stt->nentries > 1)
			qsort(stt->entries, stt->nentries, sizeof(stt->entries[0]), sort_stt_entry);

		for(i = 0; i < stt->nentries; )
		{
			newstt = new_stringtable(&stt->lvc);
			newstt->entries = (stt_entry_t *)xmalloc(16 * sizeof(stt_entry_t));
			newstt->nentries = 16;
			newstt->idbase = stt->entries[i].id & ~0xf;
			for(j = 0; j < 16 && i < stt->nentries; j++)
			{
				if(stt->entries[i].id - newstt->idbase == j)
				{
					newstt->entries[j] = stt->entries[i];
					i++;
				}
			}
			andsum = ~0;
			orsum = 0;
			characts = NULL;
			version = NULL;
			/* Check individual memory options and get
			 * the first characteristics/version
			 */
			for(j = 0; j < 16; j++)
			{
				if(!newstt->entries[j].str)
					continue;
				andsum &= newstt->entries[j].memopt;
				orsum |= newstt->entries[j].memopt;
				if(!characts)
					characts = newstt->entries[j].characts;
				if(!version)
					version = newstt->entries[j].version;
			}
			if(andsum != orsum)
			{
				warning("Stringtable's memory options are not equal (idbase: %d)", newstt->idbase);
			}
			/* Check version and characteristics */
			for(j = 0; j < 16; j++)
			{
				if(characts
				&& newstt->entries[j].characts
				&& *newstt->entries[j].characts != *characts)
					warning("Stringtable's characteristics are not the same (idbase: %d)", newstt->idbase);
				if(version
				&& newstt->entries[j].version
				&& *newstt->entries[j].version != *version)
					warning("Stringtable's versions are not the same (idbase: %d)", newstt->idbase);
			}
			rsc = new_resource(res_stt, newstt, newstt->memopt, newstt->lvc.language);
			rsc->name = new_name_id();
			rsc->name->type = name_ord;
			rsc->name->name.i_name = (newstt->idbase >> 4) + 1;
			rsc->memopt = andsum; /* Set to least common denominator */
			newstt->memopt = andsum;
			newstt->lvc.characts = characts;
			newstt->lvc.version = version;
			if(!rsclist)
			{
				rsclist = rsc;
				rsctail = rsc;
			}
			else
			{
				rsctail->next = rsc;
				rsc->prev = rsctail;
				rsctail = rsc;
			}
		}
	}
	return rsclist;
}


static toolbar_item_t *ins_tlbr_button(toolbar_item_t *prev, toolbar_item_t *idrec)
{
	idrec->prev = prev;
	if(prev)
		prev->next = idrec;

	return idrec;
}

static toolbar_item_t *get_tlbr_buttons_head(toolbar_item_t *p, int *nitems)
{
	if(!p)
	{
		*nitems = 0;
		return NULL;
	} 

	*nitems = 1;

	while(p->prev)
	{
		(*nitems)++;
		p = p->prev;
	}

	return p;
}

static string_t *make_filename(string_t *str)
{
	char *cptr;

	if(str->type != str_char)
		yyerror("Cannot handle UNICODE filenames");

	/* Remove escaped backslash and convert to forward */
	cptr = str->str.cstr;
	for(cptr = str->str.cstr; (cptr = strchr(cptr, '\\')) != NULL; cptr++)
	{
		if(cptr[1] == '\\')
		{
			memmove(cptr, cptr+1, strlen(cptr));
			str->size--;
		}
		*cptr = '/';
	}

	/* Convert to lower case. Seems to be reasonable to do */
	for(cptr = str->str.cstr; !leave_case && *cptr; cptr++)
	{
		*cptr = tolower(*cptr);
	}
	return str;
}

/*
 * Process all resources to extract fonts and build
 * a fontdir resource.
 *
 * Note: MS' resource compiler (build 1472) does not
 * handle font resources with different languages.
 * The fontdir is generated in the last active language
 * and font identifiers must be unique across the entire
 * source.
 * This is not logical considering the localization
 * constraints of all other resource types. MS has,
 * most probably, never testet localized fonts. However,
 * using fontresources is rare, so it might not occur
 * in normal applications.
 * Wine does require better localization because a lot
 * of languages are coded into the same executable.
 * Therefore, I will generate fontdirs for *each*
 * localized set of fonts.
 */
static resource_t *build_fontdir(resource_t **fnt, int nfnt)
{
	static int once = 0;
	if(!once)
	{
		warning("Need to parse fonts, not yet implemented (fnt: %p, nfnt: %d)", fnt, nfnt);
		once++;
	}
	return NULL;
}

static resource_t *build_fontdirs(resource_t *tail)
{
	resource_t *rsc;
	resource_t *lst = NULL;
	resource_t **fnt = NULL;	/* List of all fonts */
	int nfnt = 0;
	resource_t **fnd = NULL;	/* List of all fontdirs */
	int nfnd = 0;
	resource_t **lanfnt = NULL;
	int nlanfnt = 0;
	int i;
	name_id_t nid;
	string_t str;
	int fntleft;

	nid.type = name_str;
	nid.name.s_name = &str;
	str.type = str_char;
	str.str.cstr = "FONTDIR";
	str.size = 7;

	/* Extract all fonts and fontdirs */
	for(rsc = tail; rsc; rsc = rsc->prev)
	{
		if(rsc->type == res_fnt)
		{
			nfnt++;
			fnt = xrealloc(fnt, nfnt * sizeof(*fnt));
			fnt[nfnt-1] = rsc;
		}
		else if(rsc->type == res_fntdir)
		{
			nfnd++;
			fnd = xrealloc(fnd, nfnd * sizeof(*fnd));
			fnd[nfnd-1] = rsc;
		}
	}

	/* Verify the name of the present fontdirs */
	for(i = 0; i < nfnd; i++)
	{
		if(compare_name_id(&nid, fnd[i]->name))
		{
			warning("User supplied FONTDIR entry has an invalid name '%s', ignored",
				get_nameid_str(fnd[i]->name));
			fnd[i] = NULL;
		}
	}

	/* Sanity check */
	if(nfnt == 0)
	{
		if(nfnd != 0)
			warning("Found %d FONTDIR entries without any fonts present", nfnd);
		goto clean;
	}

	/* Copy space */
	lanfnt = xmalloc(nfnt * sizeof(*lanfnt));

	/* Get all fonts covered by fontdirs */
	for(i = 0; i < nfnd; i++)
	{
		int j;
		WORD cnt;
		int isswapped = 0;

		if(!fnd[i])
			continue;
		for(j = 0; j < nfnt; j++)
		{
			if(!fnt[j])
				continue;
			if(fnt[j]->lan->id == fnd[i]->lan->id && fnt[j]->lan->sub == fnd[i]->lan->sub)
			{
				lanfnt[nlanfnt] = fnt[j];
				nlanfnt++;
				fnt[j] = NULL;
			}
		}

		cnt = *(WORD *)fnd[i]->res.fnd->data->data;
		if(nlanfnt == cnt)
			isswapped = 0;
		else if(nlanfnt == BYTESWAP_WORD(cnt))
			isswapped = 1;
		else
			error("FONTDIR for language %d,%d has wrong count (%d, expected %d)",
				fnd[i]->lan->id, fnd[i]->lan->sub, cnt, nlanfnt);
#ifdef WORDS_BIGENDIAN
		if((byteorder == WRC_BO_LITTLE && !isswapped) || (byteorder != WRC_BO_LITTLE && isswapped))
#else
		if((byteorder == WRC_BO_BIG && !isswapped) || (byteorder != WRC_BO_BIG && isswapped))
#endif
		{
			internal_error(__FILE__, __LINE__, "User supplied FONTDIR needs byteswapping");
		}
	}

	/* We now have fonts left where we need to make a fontdir resource */
	for(i = fntleft = 0; i < nfnt; i++)
	{
		if(fnt[i])
			fntleft++;
	}
	while(fntleft)
	{
		/* Get fonts of same language in lanfnt[] */
		for(i = nlanfnt = 0; i < nfnt; i++)
		{
			if(fnt[i])
			{
				if(!nlanfnt)
				{
			addlanfnt:
					lanfnt[nlanfnt] = fnt[i];
					nlanfnt++;
					fnt[i] = NULL;
					fntleft--;
				}
				else if(fnt[i]->lan->id == lanfnt[0]->lan->id && fnt[i]->lan->sub == lanfnt[0]->lan->sub)
					goto addlanfnt;
			}
		}
		/* and build a fontdir */
		rsc = build_fontdir(lanfnt, nlanfnt);
		if(rsc)
		{
			if(lst)
			{
				lst->next = rsc;
				rsc->prev = lst;
			}
			lst = rsc;
		}
	}

	free(lanfnt);
clean:
	if(fnt)
		free(fnt);
	if(fnd)
		free(fnd);
	return lst;
}

/*
 * This gets invoked to determine whether the next resource
 * is to be of a standard-type (e.g. bitmaps etc.), or should
 * be a user-type resource. This function is required because
 * there is the _possibility_ of a lookahead token in the
 * parser, which is generated from the "expr" state in the
 * "nameid" parsing.
 *
 * The general resource format is:
 * <identifier> <type> <flags> <resourcebody>
 *
 * The <identifier> can either be tIDENT or "expr". The latter
 * will always generate a lookahead, which is the <type> of the
 * resource to parse. Otherwise, we need to get a new token from
 * the scanner to determine the next step.
 *
 * The problem arrises when <type> is numerical. This case should
 * map onto default resource-types and be parsed as such instead
 * of being mapped onto user-type resources.
 *
 * The trick lies in the fact that yacc (bison) doesn't care about
 * intermediate changes of the lookahead while reducing a rule. We
 * simply replace the lookahead with a token that will result in
 * a shift to the appropriate rule for the specific resource-type.
 */
static int rsrcid_to_token(int lookahead)
{
	int token;
	char *type = "?";

	/* Get a token if we don't have one yet */
	if(lookahead == YYEMPTY)
		lookahead = YYLEX;

	/* Only numbers are possibly interesting */
	switch(lookahead)
	{
	case tNUMBER:
	case tLNUMBER:
		break;
	default:
		return lookahead;
	}

	token = lookahead;

	switch(yylval.num)
	{
	case WRC_RT_CURSOR:
		type = "CURSOR";
		token = tCURSOR;
		break;
	case WRC_RT_ICON:
		type = "ICON";
		token = tICON;
		break;
	case WRC_RT_BITMAP:
		type = "BITMAP";
		token = tBITMAP;
		break;
	case WRC_RT_FONT:
		type = "FONT";
		token = tFONT;
		break;
	case WRC_RT_FONTDIR:
		type = "FONTDIR";
		token = tFONTDIR;
		break;
	case WRC_RT_RCDATA:
		type = "RCDATA";
		token = tRCDATA;
		break;
	case WRC_RT_MESSAGETABLE:
		type = "MESSAGETABLE";
		token = tMESSAGETABLE;
		break;
	case WRC_RT_DLGINIT:
		type = "DLGINIT";
		token = tDLGINIT;
		break;
	case WRC_RT_ACCELERATOR:
		type = "ACCELERATOR";
		token = tACCELERATORS;
		break;
	case WRC_RT_MENU:
		type = "MENU";
		token = tMENU;
		break;
	case WRC_RT_DIALOG:
		type = "DIALOG";
		token = tDIALOG;
		break;
	case WRC_RT_VERSION:
		type = "VERSION";
		token = tVERSIONINFO;
		break;
	case WRC_RT_TOOLBAR:
		type = "TOOLBAR";
		token = tTOOLBAR;
		break;

	case WRC_RT_STRING:
		type = "STRINGTABLE";
		break;

	case WRC_RT_ANICURSOR:
	case WRC_RT_ANIICON:
	case WRC_RT_GROUP_CURSOR:
	case WRC_RT_GROUP_ICON:
		yywarning("Usertype uses reserved type-ID %d, which is auto-generated", yylval.num);
		return lookahead;

	case WRC_RT_DLGINCLUDE:
	case WRC_RT_PLUGPLAY:
	case WRC_RT_VXD:
	case WRC_RT_HTML:
		yywarning("Usertype uses reserved type-ID %d, which is not supported by wrc", yylval.num);
	default:
		return lookahead;
	}

	if(remap)
		return token;
	else
		yywarning("Usertype uses reserved type-ID %d, which is used by %s", yylval.num, type);
	return lookahead;
}

