
/*  A Bison parser, made from parser.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	tIF	258
#define	tIFDEF	259
#define	tIFNDEF	260
#define	tELSE	261
#define	tELIF	262
#define	tENDIF	263
#define	tDEFINED	264
#define	tNL	265
#define	tTYPEDEF	266
#define	tEXTERN	267
#define	NUMBER	268
#define	LNUMBER	269
#define	tSTRING	270
#define	IDENT	271
#define	FILENAME	272
#define	RAWDATA	273
#define	ACCELERATORS	274
#define	tBITMAP	275
#define	CURSOR	276
#define	DIALOG	277
#define	DIALOGEX	278
#define	MENU	279
#define	MENUEX	280
#define	MESSAGETABLE	281
#define	RCDATA	282
#define	VERSIONINFO	283
#define	STRINGTABLE	284
#define	FONT	285
#define	ICON	286
#define	AUTO3STATE	287
#define	AUTOCHECKBOX	288
#define	AUTORADIOBUTTON	289
#define	CHECKBOX	290
#define	DEFPUSHBUTTON	291
#define	PUSHBUTTON	292
#define	RADIOBUTTON	293
#define	STATE3	294
#define	GROUPBOX	295
#define	COMBOBOX	296
#define	LISTBOX	297
#define	SCROLLBAR	298
#define	CONTROL	299
#define	EDITTEXT	300
#define	RTEXT	301
#define	CTEXT	302
#define	LTEXT	303
#define	BLOCK	304
#define	VALUE	305
#define	SHIFT	306
#define	ALT	307
#define	ASCII	308
#define	VIRTKEY	309
#define	GRAYED	310
#define	CHECKED	311
#define	INACTIVE	312
#define	NOINVERT	313
#define	tPURE	314
#define	IMPURE	315
#define	DISCARDABLE	316
#define	LOADONCALL	317
#define	PRELOAD	318
#define	tFIXED	319
#define	MOVEABLE	320
#define	CLASS	321
#define	CAPTION	322
#define	CHARACTERISTICS	323
#define	EXSTYLE	324
#define	STYLE	325
#define	VERSION	326
#define	LANGUAGE	327
#define	FILEVERSION	328
#define	PRODUCTVERSION	329
#define	FILEFLAGSMASK	330
#define	FILEOS	331
#define	FILETYPE	332
#define	FILEFLAGS	333
#define	FILESUBTYPE	334
#define	MENUBARBREAK	335
#define	MENUBREAK	336
#define	MENUITEM	337
#define	POPUP	338
#define	SEPARATOR	339
#define	HELP	340
#define	TOOLBAR	341
#define	BUTTON	342
#define	tBEGIN	343
#define	tEND	344
#define	DLGINIT	345
#define	LOGOR	346
#define	LOGAND	347
#define	EQ	348
#define	NE	349
#define	LTE	350
#define	GTE	351
#define	NOT	352

#line 1 "parser.y"

/*
 * Copyright  Martin von Loewis, 1994
 * Copyright 1998 Bertho A. Stultiens (BS)
 *           1999 Juergen Schmied (JS)
 *
 * 6-Nov-1999 JS        - see CHANGES
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
#include "wingdi.h"
#include "winuser.h"

#ifdef __BORLANDC__
#pragma warn -sig
#endif

int indialog = 0;	/* Signal flex that we're parsing a dialog */
int want_rscname = 0;	/* Set when a resource's name is required */
stringtable_t *tagstt;	/* Stringtable tag.
			 * It is set while parsing a stringtable to one of
			 * the stringtables in the sttres list or a new one
			 * if the language was not parsed before.
			 */
stringtable_t *sttres;	/* Stringtable resources. This holds the list of
			 * stringtables with different lanuages
			 */
/* Set to the current options of the currently scanning stringtable */
static int *tagstt_memopt;
static characts_t *tagstt_characts;
static version_t *tagstt_version;

/* Prototypes of here defined functions */
void split_cursors(raw_data_t *rd, cursor_group_t *curg, int *ncur);
void split_icons(raw_data_t *rd, icon_group_t *icog, int *nico);
int alloc_cursor_id(language_t *);
int alloc_icon_id(language_t *);
void ins_stt_entry(stt_entry_t *ste);
int check_stt_entry(stringtable_t *tabs, stt_entry_t *ste);
event_t *get_event_head(event_t *p);
control_t *get_control_head(control_t *p);
ver_value_t *get_ver_value_head(ver_value_t *p);
ver_block_t *get_ver_block_head(ver_block_t *p);
resource_t *get_resource_head(resource_t *p);
menuex_item_t *get_itemex_head(menuex_item_t *p);
menu_item_t *get_item_head(menu_item_t *p);
raw_data_t *merge_raw_data_str(raw_data_t *r1, string_t *str);
raw_data_t *merge_raw_data_int(raw_data_t *r1, int i);
raw_data_t *merge_raw_data_long(raw_data_t *r1, int i);
raw_data_t *merge_raw_data(raw_data_t *r1, raw_data_t *r2);
raw_data_t *str2raw_data(string_t *str);
raw_data_t *int2raw_data(int i);
raw_data_t *long2raw_data(int i);
raw_data_t *load_file(string_t *name);
itemex_opt_t *new_itemex_opt(int id, int type, int state, int helpid);
event_t *add_string_event(string_t *key, int id, int flags, event_t *prev);
event_t *add_event(int key, int id, int flags, event_t *prev);
dialogex_t *dialogex_version(version_t *v, dialogex_t *dlg);
dialogex_t *dialogex_characteristics(characts_t *c, dialogex_t *dlg);
dialogex_t *dialogex_language(language_t *l, dialogex_t *dlg);
dialogex_t *dialogex_menu(name_id_t *m, dialogex_t *dlg);
dialogex_t *dialogex_class(name_id_t *n, dialogex_t *dlg);
dialogex_t *dialogex_font(font_id_t *f, dialogex_t *dlg);
dialogex_t *dialogex_caption(string_t *s, dialogex_t *dlg);
dialogex_t *dialogex_exstyle(style_t *st, dialogex_t *dlg);
dialogex_t *dialogex_style(style_t *st, dialogex_t *dlg);
name_id_t *convert_ctlclass(name_id_t *cls);
control_t *ins_ctrl(int type, int special_style, control_t *ctrl, control_t *prev);
dialog_t *dialog_version(version_t *v, dialog_t *dlg);
dialog_t *dialog_characteristics(characts_t *c, dialog_t *dlg);
dialog_t *dialog_language(language_t *l, dialog_t *dlg);
dialog_t *dialog_menu(name_id_t *m, dialog_t *dlg);
dialog_t *dialog_class(name_id_t *n, dialog_t *dlg);
dialog_t *dialog_font(font_id_t *f, dialog_t *dlg);
dialog_t *dialog_caption(string_t *s, dialog_t *dlg);
dialog_t *dialog_exstyle(style_t * st, dialog_t *dlg);
dialog_t *dialog_style(style_t * st, dialog_t *dlg);
resource_t *build_stt_resources(stringtable_t *stthead);
stringtable_t *find_stringtable(lvc_t *lvc);
toolbar_item_t *ins_tlbr_button(toolbar_item_t *prev, toolbar_item_t *idrec);
toolbar_item_t *get_tlbr_buttons_head(toolbar_item_t *p, int *nitems);


#line 183 "parser.y"
typedef union{
	string_t	*str;
	int		num;
	int		*iptr;
	resource_t	*res;
	accelerator_t	*acc;
	bitmap_t	*bmp;
	cursor_t	*cur;
	cursor_group_t	*curg;
	dialog_t	*dlg;
	dialogex_t	*dlgex;
	font_t		*fnt;
	icon_t		*ico;
	icon_group_t	*icog;
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



#define	YYFINAL		604
#define	YYFLAG		-32768
#define	YYNTBASE	112

#define YYTRANSLATE(x) ((unsigned)(x) <= 352 ? yytranslate[x] : 191)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   107,     2,     2,     2,     2,    95,     2,   109,
   110,   104,   102,   111,   103,     2,   105,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    98,
     2,   100,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    94,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    93,     2,   106,     2,     2,     2,     2,
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
    86,    87,    88,    89,    90,    91,    92,    96,    97,    99,
   101,   108
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     3,     6,     9,    12,    16,    20,    24,    28,
    31,    34,    36,    40,    44,    48,    52,    56,    60,    64,
    68,    72,    76,    80,    83,    86,    89,    92,    96,    98,
   100,   103,   108,   110,   112,   115,   118,   121,   124,   126,
   128,   130,   132,   134,   136,   138,   140,   142,   144,   146,
   148,   150,   152,   154,   156,   158,   160,   162,   164,   166,
   170,   174,   178,   182,   186,   190,   194,   197,   202,   207,
   211,   215,   217,   219,   226,   227,   233,   239,   240,   244,
   248,   252,   256,   260,   264,   278,   279,   283,   287,   291,
   294,   298,   302,   305,   308,   311,   312,   316,   320,   324,
   328,   332,   336,   340,   344,   348,   352,   356,   360,   364,
   368,   372,   376,   380,   391,   404,   415,   416,   421,   428,
   437,   455,   471,   476,   477,   480,   481,   484,   489,   493,
   497,   499,   502,   504,   506,   521,   522,   526,   530,   534,
   537,   540,   544,   548,   551,   554,   557,   558,   562,   566,
   570,   574,   578,   582,   586,   590,   594,   598,   602,   606,
   610,   614,   618,   622,   626,   637,   657,   674,   688,   700,
   701,   703,   704,   707,   717,   718,   721,   726,   730,   731,
   738,   742,   748,   749,   753,   757,   761,   765,   769,   773,
   778,   782,   783,   788,   792,   798,   799,   802,   808,   815,
   816,   819,   824,   831,   840,   845,   849,   850,   855,   856,
   858,   864,   865,   875,   885,   889,   893,   897,   901,   905,
   906,   909,   915,   916,   919,   921,   926,   931,   933,   937,
   947,   948,   952,   955,   956,   959,   962,   964,   966,   968,
   970,   972,   974,   976,   977,   980,   983,   986,   991,   994,
   997,  1001,  1003,  1005,  1007,  1009,  1013,  1017,  1021,  1025,
  1026,  1028,  1030,  1034,  1038,  1042,  1046,  1050,  1054,  1057,
  1060,  1064,  1066,  1069,  1071
};

static const short yyrhs[] = {   113,
     0,     0,   113,   118,     0,   113,   114,     0,   113,   117,
     0,     3,   115,    10,     0,     4,    16,    10,     0,     5,
    16,    10,     0,     7,   115,    10,     0,     6,    10,     0,
     8,    10,     0,   116,     0,   115,    91,   115,     0,   115,
    92,   115,     0,   115,   102,   115,     0,   115,   103,   115,
     0,   115,    94,   115,     0,   115,    96,   115,     0,   115,
    97,   115,     0,   115,    98,   115,     0,   115,   100,   115,
     0,   115,    99,   115,     0,   115,   101,   115,     0,   106,
   115,     0,   102,   115,     0,   103,   115,     0,   107,   115,
     0,   109,   115,   110,     0,   190,     0,    16,     0,     9,
    16,     0,     9,   109,    16,   110,     0,    11,     0,    12,
     0,    16,    16,     0,    16,   109,     0,    16,   104,     0,
   119,   121,     0,   165,     0,   182,     0,   188,     0,    16,
     0,   119,     0,    15,     0,   131,     0,   122,     0,   123,
     0,   134,     0,   146,     0,   128,     0,   124,     0,   125,
     0,   156,     0,   160,     0,   126,     0,   127,     0,   176,
     0,   129,     0,   169,     0,    20,   178,    17,     0,    20,
   178,   185,     0,    21,   178,    17,     0,    21,   178,   185,
     0,    30,   178,    17,     0,    31,   178,    17,     0,    31,
   178,   185,     0,    26,    17,     0,    27,   178,   181,   185,
     0,    90,   178,   181,   185,     0,   130,   178,    17,     0,
   130,   178,   185,     0,    13,     0,    15,     0,    19,   178,
   181,    88,   132,    89,     0,     0,   132,    15,   111,   188,
   133,     0,   132,   188,   111,   188,   133,     0,     0,   133,
   111,    58,     0,   133,   111,    51,     0,   133,   111,    44,
     0,   133,   111,    52,     0,   133,   111,    53,     0,   133,
   111,    54,     0,    22,   178,   188,   111,   188,   111,   188,
   111,   188,   135,    88,   136,    89,     0,     0,   135,    70,
   144,     0,   135,    69,   144,     0,   135,    67,    15,     0,
   135,   141,     0,   135,    66,   120,     0,   135,    24,   119,
     0,   135,   182,     0,   135,   183,     0,   135,   184,     0,
     0,   136,    44,   140,     0,   136,    45,   138,     0,   136,
    42,   138,     0,   136,    41,   138,     0,   136,    43,   138,
     0,   136,    35,   137,     0,   136,    36,   137,     0,   136,
    40,   137,     0,   136,    37,   137,     0,   136,    38,   137,
     0,   136,    32,   137,     0,   136,    39,   137,     0,   136,
    33,   137,     0,   136,    34,   137,     0,   136,    48,   137,
     0,   136,    47,   137,     0,   136,    46,   137,     0,   136,
    31,   120,   168,   188,   111,   188,   111,   188,   139,     0,
    15,   168,   188,   111,   188,   111,   188,   111,   188,   111,
   188,   142,     0,   188,   111,   188,   111,   188,   111,   188,
   111,   188,   142,     0,     0,   111,   188,   111,   188,     0,
   111,   188,   111,   188,   111,   144,     0,   111,   188,   111,
   188,   111,   144,   111,   144,     0,   120,   168,   188,   111,
   145,   111,   144,   111,   188,   111,   188,   111,   188,   111,
   188,   111,   144,     0,   120,   168,   188,   111,   145,   111,
   144,   111,   188,   111,   188,   111,   188,   111,   188,     0,
    30,   188,   111,    15,     0,     0,   111,   144,     0,     0,
   111,   144,     0,   111,   144,   111,   144,     0,   144,    93,
   144,     0,   109,   144,   110,     0,   190,     0,   108,   190,
     0,   188,     0,    15,     0,    23,   178,   188,   111,   188,
   111,   188,   111,   188,   153,   147,    88,   148,    89,     0,
     0,   147,    70,   144,     0,   147,    69,   144,     0,   147,
    67,    15,     0,   147,   141,     0,   147,   154,     0,   147,
    66,   120,     0,   147,    24,   119,     0,   147,   182,     0,
   147,   183,     0,   147,   184,     0,     0,   148,    44,   149,
     0,   148,    45,   151,     0,   148,    42,   151,     0,   148,
    41,   151,     0,   148,    43,   151,     0,   148,    35,   150,
     0,   148,    36,   150,     0,   148,    40,   150,     0,   148,
    37,   150,     0,   148,    38,   150,     0,   148,    32,   150,
     0,   148,    39,   150,     0,   148,    33,   150,     0,   148,
    34,   150,     0,   148,    48,   150,     0,   148,    47,   150,
     0,   148,    46,   150,     0,   148,    31,   120,   168,   188,
   111,   188,   111,   188,   139,     0,   120,   168,   188,   111,
   145,   111,   144,   111,   188,   111,   188,   111,   188,   111,
   188,   111,   144,   153,   152,     0,   120,   168,   188,   111,
   145,   111,   144,   111,   188,   111,   188,   111,   188,   111,
   188,   152,     0,    15,   168,   188,   111,   188,   111,   188,
   111,   188,   111,   188,   143,   152,     0,   188,   111,   188,
   111,   188,   111,   188,   111,   188,   143,   152,     0,     0,
   185,     0,     0,   111,   188,     0,    30,   188,   111,    15,
   111,   188,   111,   188,   155,     0,     0,   111,   188,     0,
    24,   178,   181,   157,     0,    88,   158,    89,     0,     0,
   158,    82,    15,   168,   188,   159,     0,   158,    82,    84,
     0,   158,    83,    15,   159,   157,     0,     0,   111,    56,
   159,     0,   111,    55,   159,     0,   111,    85,   159,     0,
   111,    57,   159,     0,   111,    80,   159,     0,   111,    81,
   159,     0,    25,   178,   181,   161,     0,    88,   162,    89,
     0,     0,   162,    82,    15,   163,     0,   162,    82,    84,
     0,   162,    83,    15,   164,   161,     0,     0,   111,   188,
     0,   111,   187,   111,   187,   159,     0,   111,   187,   111,
   187,   111,   188,     0,     0,   111,   188,     0,   111,   187,
   111,   188,     0,   111,   187,   111,   187,   111,   188,     0,
   111,   187,   111,   187,   111,   187,   111,   188,     0,   166,
    88,   167,    89,     0,    29,   178,   181,     0,     0,   167,
   188,   168,    15,     0,     0,   111,     0,    28,   170,    88,
   171,    89,     0,     0,   170,    73,   188,   111,   188,   111,
   188,   111,   188,     0,   170,    74,   188,   111,   188,   111,
   188,   111,   188,     0,   170,    78,   188,     0,   170,    75,
   188,     0,   170,    76,   188,     0,   170,    77,   188,     0,
   170,    79,   188,     0,     0,   171,   172,     0,    49,    15,
    88,   173,    89,     0,     0,   173,   174,     0,   172,     0,
    50,    15,   111,    15,     0,    50,    15,   111,   175,     0,
   188,     0,   175,   111,   188,     0,    86,   178,   188,   111,
   188,   181,    88,   177,    89,     0,     0,   177,    87,   188,
     0,   177,    84,     0,     0,   178,   179,     0,   178,   180,
     0,    63,     0,    65,     0,    61,     0,    59,     0,    62,
     0,    64,     0,    60,     0,     0,   181,   182,     0,   181,
   183,     0,   181,   184,     0,    72,   188,   111,   188,     0,
    68,   188,     0,    71,   188,     0,    88,   186,    89,     0,
    18,     0,    13,     0,    14,     0,    15,     0,   186,   168,
    18,     0,   186,   168,    13,     0,   186,   168,    14,     0,
   186,   168,    15,     0,     0,   188,     0,   189,     0,   189,
   102,   189,     0,   189,   103,   189,     0,   189,    93,   189,
     0,   189,    95,   189,     0,   189,   104,   189,     0,   189,
   105,   189,     0,   106,   189,     0,   103,   189,     0,   109,
   189,   110,     0,   190,     0,   108,   190,     0,    13,     0,
    14,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   310,   330,   331,   356,   357,   362,   363,   364,   365,   366,
   367,   370,   371,   372,   373,   374,   375,   376,   377,   378,
   379,   380,   381,   382,   383,   384,   385,   386,   390,   391,
   392,   393,   397,   398,   399,   400,   401,   406,   421,   429,
   442,   450,   461,   462,   472,   473,   474,   488,   489,   495,
   496,   497,   511,   512,   518,   519,   520,   521,   522,   526,
   527,   531,   532,   537,   541,   542,   550,   558,   571,   584,
   585,   604,   616,   626,   650,   651,   652,   655,   656,   657,
   658,   659,   660,   661,   666,   701,   702,   703,   704,   705,
   706,   707,   708,   709,   710,   713,   714,   715,   716,   717,
   718,   719,   720,   721,   722,   724,   725,   726,   727,   728,
   729,   730,   731,   733,   743,   762,   777,   780,   785,   792,
   803,   817,   832,   837,   838,   842,   843,   844,   848,   849,
   850,   851,   855,   860,   868,   913,   914,   915,   916,   917,
   918,   919,   920,   921,   922,   923,   926,   927,   928,   929,
   930,   931,   932,   933,   934,   935,   937,   938,   939,   940,
   941,   942,   943,   944,   946,   956,   981,   997,  1025,  1048,
  1049,  1052,  1053,  1057,  1064,  1065,  1069,  1092,  1096,  1097,
  1106,  1112,  1131,  1132,  1133,  1134,  1135,  1136,  1137,  1141,
  1166,  1170,  1171,  1187,  1193,  1213,  1214,  1218,  1226,  1237,
  1238,  1242,  1248,  1256,  1276,  1317,  1328,  1329,  1361,  1362,
  1367,  1374,  1375,  1385,  1395,  1402,  1409,  1416,  1423,  1433,
  1434,  1443,  1451,  1452,  1461,  1466,  1472,  1481,  1482,  1486,
  1512,  1513,  1518,  1527,  1528,  1538,  1553,  1554,  1555,  1556,
  1559,  1560,  1561,  1565,  1566,  1574,  1582,  1593,  1597,  1601,
  1605,  1609,  1610,  1611,  1612,  1613,  1614,  1615,  1616,  1623,
  1624,  1626,  1629,  1630,  1631,  1632,  1633,  1634,  1635,  1636,
  1638,  1639,  1640,  1643,  1644
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","tIF","tIFDEF",
"tIFNDEF","tELSE","tELIF","tENDIF","tDEFINED","tNL","tTYPEDEF","tEXTERN","NUMBER",
"LNUMBER","tSTRING","IDENT","FILENAME","RAWDATA","ACCELERATORS","tBITMAP","CURSOR",
"DIALOG","DIALOGEX","MENU","MENUEX","MESSAGETABLE","RCDATA","VERSIONINFO","STRINGTABLE",
"FONT","ICON","AUTO3STATE","AUTOCHECKBOX","AUTORADIOBUTTON","CHECKBOX","DEFPUSHBUTTON",
"PUSHBUTTON","RADIOBUTTON","STATE3","GROUPBOX","COMBOBOX","LISTBOX","SCROLLBAR",
"CONTROL","EDITTEXT","RTEXT","CTEXT","LTEXT","BLOCK","VALUE","SHIFT","ALT","ASCII",
"VIRTKEY","GRAYED","CHECKED","INACTIVE","NOINVERT","tPURE","IMPURE","DISCARDABLE",
"LOADONCALL","PRELOAD","tFIXED","MOVEABLE","CLASS","CAPTION","CHARACTERISTICS",
"EXSTYLE","STYLE","VERSION","LANGUAGE","FILEVERSION","PRODUCTVERSION","FILEFLAGSMASK",
"FILEOS","FILETYPE","FILEFLAGS","FILESUBTYPE","MENUBARBREAK","MENUBREAK","MENUITEM",
"POPUP","SEPARATOR","HELP","TOOLBAR","BUTTON","tBEGIN","tEND","DLGINIT","LOGOR",
"LOGAND","'|'","'^'","'&'","EQ","NE","'<'","LTE","'>'","GTE","'+'","'-'","'*'",
"'/'","'~'","'!'","NOT","'('","')'","','","resource_file","resources","preprocessor",
"pp_expr","pp_constant","cjunk","resource","nameid","nameid_s","resource_definition",
"bitmap","cursor","font","icon","messagetable","rcdata","dlginit","userres",
"usertype","accelerators","events","acc_opt","dialog","dlg_attributes","ctrls",
"lab_ctrl","ctrl_desc","iconinfo","gen_ctrl","opt_font","optional_style","optional_style_pair",
"style","ctlclass","dialogex","dlgex_attribs","exctrls","gen_exctrl","lab_exctrl",
"exctrl_desc","opt_data","helpid","opt_exfont","opt_expr","menu","menu_body",
"item_definitions","item_options","menuex","menuex_body","itemex_definitions",
"itemex_options","itemex_p_options","stringtable","stt_head","strings","opt_comma",
"versioninfo","fix_version","ver_blocks","ver_block","ver_values","ver_value",
"ver_words","toolbar","toolbar_items","loadmemopts","lamo","lama","opt_lvc",
"opt_language","opt_characts","opt_version","raw_data","raw_elements","e_expr",
"expr","xpr","any_num", NULL
};
#endif

static const short yyr1[] = {     0,
   112,   113,   113,   113,   113,   114,   114,   114,   114,   114,
   114,   115,   115,   115,   115,   115,   115,   115,   115,   115,
   115,   115,   115,   115,   115,   115,   115,   115,   116,   116,
   116,   116,   117,   117,   117,   117,   117,   118,   118,   118,
   119,   119,   120,   120,   121,   121,   121,   121,   121,   121,
   121,   121,   121,   121,   121,   121,   121,   121,   121,   122,
   122,   123,   123,   124,   125,   125,   126,   127,   128,   129,
   129,   130,   130,   131,   132,   132,   132,   133,   133,   133,
   133,   133,   133,   133,   134,   135,   135,   135,   135,   135,
   135,   135,   135,   135,   135,   136,   136,   136,   136,   136,
   136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
   136,   136,   136,   136,   137,   138,   139,   139,   139,   139,
   140,   140,   141,   142,   142,   143,   143,   143,   144,   144,
   144,   144,   145,   145,   146,   147,   147,   147,   147,   147,
   147,   147,   147,   147,   147,   147,   148,   148,   148,   148,
   148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
   148,   148,   148,   148,   148,   149,   149,   150,   151,   152,
   152,   153,   153,   154,   155,   155,   156,   157,   158,   158,
   158,   158,   159,   159,   159,   159,   159,   159,   159,   160,
   161,   162,   162,   162,   162,   163,   163,   163,   163,   164,
   164,   164,   164,   164,   165,   166,   167,   167,   168,   168,
   169,   170,   170,   170,   170,   170,   170,   170,   170,   171,
   171,   172,   173,   173,   174,   174,   174,   175,   175,   176,
   177,   177,   177,   178,   178,   178,   179,   179,   179,   179,
   180,   180,   180,   181,   181,   181,   181,   182,   183,   184,
   185,   186,   186,   186,   186,   186,   186,   186,   186,   187,
   187,   188,   189,   189,   189,   189,   189,   189,   189,   189,
   189,   189,   189,   190,   190
};

static const short yyr2[] = {     0,
     1,     0,     2,     2,     2,     3,     3,     3,     3,     2,
     2,     1,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     2,     2,     2,     2,     3,     1,     1,
     2,     4,     1,     1,     2,     2,     2,     2,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
     3,     3,     3,     3,     3,     3,     2,     4,     4,     3,
     3,     1,     1,     6,     0,     5,     5,     0,     3,     3,
     3,     3,     3,     3,    13,     0,     3,     3,     3,     2,
     3,     3,     2,     2,     2,     0,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,    10,    12,    10,     0,     4,     6,     8,
    17,    15,     4,     0,     2,     0,     2,     4,     3,     3,
     1,     2,     1,     1,    14,     0,     3,     3,     3,     2,
     2,     3,     3,     2,     2,     2,     0,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,    10,    19,    16,    13,    11,     0,
     1,     0,     2,     9,     0,     2,     4,     3,     0,     6,
     3,     5,     0,     3,     3,     3,     3,     3,     3,     4,
     3,     0,     4,     3,     5,     0,     2,     5,     6,     0,
     2,     4,     6,     8,     4,     3,     0,     4,     0,     1,
     5,     0,     9,     9,     3,     3,     3,     3,     3,     0,
     2,     5,     0,     2,     1,     4,     4,     1,     3,     9,
     0,     3,     2,     0,     2,     2,     1,     1,     1,     1,
     1,     1,     1,     0,     2,     2,     2,     4,     2,     2,
     3,     1,     1,     1,     1,     3,     3,     3,     3,     0,
     1,     1,     3,     3,     3,     3,     3,     3,     2,     2,
     3,     1,     2,     1,     1
};

static const short yydefact[] = {     2,
     1,     0,     0,     0,     0,     0,     0,    33,    34,   274,
   275,    42,   234,     0,     0,     0,     0,     0,     4,     5,
     3,     0,    39,     0,    40,    41,   262,   272,     0,    30,
     0,     0,     0,     0,     0,     0,    12,    29,     0,     0,
    10,     0,    11,    35,    37,    36,   244,     0,   270,   269,
   273,     0,    72,    73,   234,   234,   234,   234,   234,   234,
   234,     0,   234,   212,   234,   234,   234,   234,    38,    46,
    47,    51,    52,    55,    56,    50,    58,   234,    45,    48,
    49,    53,    54,    59,    57,   207,     0,     0,     0,     0,
     0,     0,    31,     0,    25,    26,    24,    27,     0,     6,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     7,     8,     9,   240,   243,   239,   241,   237,   242,
   238,   235,   236,   206,     0,   271,   244,     0,     0,     0,
     0,   244,   244,    67,   244,     0,     0,     0,     0,   244,
     0,     0,   265,   266,   263,   264,   267,   268,     0,    28,
    13,    14,    17,    18,    19,    20,    22,    21,    23,    15,
    16,     0,     0,   245,   246,   247,   248,     0,    60,     0,
    61,    62,    63,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   220,    64,    65,    66,     0,
     0,    70,    71,   205,   209,    32,   249,   250,    75,   253,
   254,   255,   252,   209,     0,     0,   179,   177,   192,   190,
    68,     0,     0,   216,   217,   218,   215,   219,     0,     0,
    69,   210,     0,     0,   251,     0,     0,     0,     0,     0,
     0,     0,     0,   211,   221,   244,   208,     0,    74,     0,
   257,   258,   259,   256,     0,     0,     0,     0,   178,     0,
     0,   191,     0,     0,     0,     0,     0,     0,     0,     0,
   209,   181,   183,   196,   194,   200,     0,     0,   223,   231,
    78,    78,     0,     0,     0,     0,     0,   260,   193,   260,
     0,     0,     0,     0,     0,    76,    77,    86,   172,   183,
   183,   183,   183,   183,   183,   183,   182,     0,   197,     0,
   201,   195,     0,     0,     0,   222,   225,   224,   233,     0,
   230,     0,     0,     0,   136,   180,   185,   184,   187,   188,
   189,   186,   260,   260,   213,   214,     0,   232,    81,    80,
    82,    83,    84,    79,     0,     0,     0,     0,     0,     0,
    96,    90,    93,    94,    95,   173,     0,   183,   261,     0,
   202,     0,    42,    92,     0,    44,    43,    91,    89,     0,
     0,    88,   131,    87,     0,     0,     0,     0,     0,     0,
     0,   147,   140,   141,   144,   145,   146,     0,   198,   260,
   226,   227,   228,     0,   132,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    85,   143,     0,   142,   139,
   138,   137,     0,   199,     0,   203,     0,   123,   130,   129,
   209,   209,   107,   109,   110,   102,   103,   105,   106,   108,
   104,   100,     0,    99,   101,   209,    97,    98,   113,   112,
   111,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   135,     0,   229,     0,     0,     0,     0,   123,   209,   209,
   158,   160,   161,   153,   154,   156,   157,   159,   155,   151,
     0,   150,   152,   209,   148,   149,   164,   163,   162,   204,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   134,     0,   133,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   175,     0,     0,     0,     0,   117,     0,
     0,     0,     0,   174,     0,     0,     0,     0,     0,   114,
     0,     0,     0,   176,   117,     0,     0,     0,     0,     0,
   124,     0,   165,     0,     0,     0,     0,     0,     0,   116,
     0,     0,   126,     0,   118,   124,   125,     0,     0,     0,
   170,     0,     0,   115,     0,   126,   127,   169,   171,     0,
   119,     0,   170,     0,     0,     0,     0,   168,   128,     0,
   120,   122,     0,     0,   170,   121,     0,   167,   172,   170,
   166,     0,     0,     0
};

static const short yydefgoto[] = {   602,
     1,    19,    36,    37,    20,    21,   357,   358,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,   224,
   286,    80,   313,   365,   423,   432,   540,   437,   342,   560,
   571,   362,   513,    81,   347,   413,   485,   471,   480,   578,
   315,   374,   534,    82,   208,   229,   277,    83,   210,   230,
   279,   281,    23,    24,   142,   223,    84,   136,   219,   235,
   284,   308,   382,    85,   285,    47,   122,   123,   124,   164,
   165,   166,   579,   204,   298,    26,    27,    28
};

static const short yypact[] = {-32768,
    39,    10,    -4,     0,    -1,    10,    15,-32768,-32768,-32768,
-32768,   -15,-32768,   197,   197,   197,    82,   197,-32768,-32768,
-32768,   442,-32768,   -68,-32768,-32768,   390,-32768,     6,-32768,
    10,    10,    10,    10,    10,   298,-32768,-32768,    31,    56,
-32768,   351,-32768,-32768,-32768,-32768,   611,   -41,    50,-32768,
-32768,   483,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    55,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   197,   197,   197,   197,
   197,   197,-32768,    58,-32768,-32768,-32768,-32768,   449,-32768,
    10,    10,    10,    10,    10,    10,    10,    10,    10,    10,
    10,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   349,   197,-32768,   611,   345,   417,   110,
   110,   611,   611,-32768,   611,   558,    19,   439,   110,   611,
   454,   114,   517,   125,    50,    50,-32768,-32768,   -25,-32768,
   690,   700,   556,   706,   706,    85,    85,    85,    85,-32768,
-32768,   197,   197,-32768,-32768,-32768,-32768,   153,-32768,   401,
-32768,-32768,-32768,   -14,     7,   195,   255,   262,   197,   197,
   197,   197,   197,   197,   197,-32768,-32768,-32768,-32768,    53,
   262,-32768,-32768,-32768,    78,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   -13,   197,   197,-32768,-32768,-32768,-32768,
-32768,    88,   123,-32768,-32768,-32768,-32768,-32768,   -22,   197,
-32768,-32768,   135,    73,-32768,   516,   132,   148,    74,    95,
   197,   197,   178,-32768,-32768,-32768,-32768,   154,-32768,   157,
-32768,-32768,-32768,-32768,   197,   197,    -9,   254,-32768,    -7,
   257,-32768,   173,   184,    22,   276,   197,   197,   214,   220,
    78,-32768,   224,   230,-32768,   234,   197,   197,-32768,-32768,
-32768,-32768,   197,   197,   197,   111,   209,   197,-32768,   197,
   258,   240,   245,    76,   268,   252,   252,-32768,   267,   224,
   224,   224,   224,   224,   224,   224,-32768,   269,   273,   280,
   273,-32768,   197,   197,   297,-32768,-32768,-32768,-32768,   197,
-32768,   723,   530,   197,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   197,   197,-32768,-32768,   282,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   193,   197,    77,   364,    35,    35,
-32768,-32768,-32768,-32768,-32768,-32768,   618,   291,-32768,   292,
   273,    43,-32768,-32768,   302,-32768,-32768,-32768,-32768,    82,
    35,   294,-32768,   294,   676,   193,   197,    77,   420,    35,
    35,-32768,-32768,-32768,-32768,-32768,-32768,   201,-32768,   197,
-32768,   328,-32768,   425,-32768,    48,    35,    77,   426,   426,
   426,   426,   426,   426,   426,   426,   426,   197,   197,   197,
    77,   197,   426,   426,   426,-32768,-32768,   335,-32768,-32768,
   294,   294,   694,-32768,   347,   273,   197,-32768,-32768,-32768,
    78,    78,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   348,-32768,-32768,    78,-32768,-32768,-32768,-32768,
-32768,   459,    77,   469,   469,   469,   469,   469,   469,   469,
   469,   469,   197,   197,   197,    77,   197,   469,   469,   469,
-32768,   197,-32768,   197,   197,   197,   197,   376,    78,    78,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   378,-32768,-32768,    78,-32768,-32768,-32768,-32768,-32768,-32768,
   379,   380,   385,   386,   197,   197,   197,   197,   197,   197,
   197,   197,   146,   398,   400,   409,   411,   412,   413,   415,
   422,-32768,   424,-32768,   197,   197,   197,   197,   146,   197,
   197,   197,    35,   427,   428,   433,   444,   445,   447,   453,
   455,   101,   197,-32768,   197,   197,   197,    35,   197,-32768,
   197,   197,   197,-32768,   447,   456,   458,   149,   466,   468,
   470,   472,-32768,   197,   197,   197,   197,   197,    35,-32768,
   197,   478,   479,   480,   502,   470,   294,   503,   197,    35,
   465,   197,    35,-32768,   197,   479,   151,-32768,-32768,   504,
   218,   505,   465,    35,   197,    35,   197,-32768,   294,   506,
   294,   527,   197,    35,   -71,   294,    35,-32768,   231,   465,
-32768,   512,   537,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    28,-32768,-32768,-32768,     1,  -248,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   308,-32768,-32768,-32768,    33,  -202,    49,-32768,   293,    94,
    63,  -333,   142,-32768,-32768,-32768,-32768,   312,   -84,  -223,
    68,-32768,-32768,-32768,   391,-32768,    26,-32768,   397,-32768,
-32768,-32768,-32768,-32768,-32768,  -190,-32768,-32768,-32768,   395,
-32768,-32768,-32768,-32768,-32768,   688,-32768,-32768,  -122,     2,
  -278,  -274,   107,-32768,  -259,   108,   186,    -2
};


#define	YYLAST		809


static const short yytable[] = {    38,
    44,    22,    25,    38,   168,   261,   364,   264,    41,   176,
   177,    39,   178,   226,    51,    40,   170,   191,    29,    86,
   300,    93,    10,    11,    43,    30,   233,   386,    38,    38,
    38,    38,    38,    42,   344,   187,   411,   412,   345,   597,
   112,     2,     3,     4,     5,     6,     7,    10,    11,     8,
     9,    10,    11,   420,    12,    10,    11,   381,    95,    96,
    97,    98,    99,   348,   350,   113,   234,    13,   376,   125,
   275,   134,   377,   149,   262,   225,   265,   115,   116,   117,
   118,   119,   120,   121,   196,    10,    11,   238,    45,    10,
    11,   356,   353,    46,    10,    11,   205,   222,    38,    38,
    38,    38,    38,    38,    38,    38,    38,    38,    38,   269,
    14,    31,    32,   256,    94,    33,    34,   206,    35,   409,
   415,    48,    10,    11,   233,   305,    10,    11,   151,   152,
   153,   154,   155,   156,   157,   158,   159,   160,   161,   421,
   387,    15,   360,   361,    16,    15,    17,    18,    16,   237,
    17,    18,   436,    91,    92,   247,   248,   419,    10,    11,
   512,   239,   249,   220,   306,   291,   292,   293,   115,   116,
   117,   118,   119,   120,   121,    15,   250,   251,    16,    15,
    17,    18,    16,   252,    17,    18,   110,   111,   222,   532,
   294,   295,   255,   387,   469,   296,   434,   435,   231,   438,
    49,    50,   194,    52,   548,    10,    11,   484,   353,    10,
    11,   543,    15,    10,    11,    16,    15,    17,    18,    16,
   162,    17,    18,   163,    14,   567,    89,    90,    91,    92,
   464,   465,   167,   232,   171,   173,   577,   174,   175,   581,
   199,   387,   245,   387,   189,   467,   190,   193,    15,   195,
   589,    16,   591,    17,    18,   291,   292,   293,   246,   556,
   596,   584,   162,   599,   257,   163,    14,   258,   263,   197,
   198,   266,   143,   144,   145,   146,   147,   148,   496,   497,
   294,   295,   207,   267,   211,   296,   212,   213,   214,   215,
   216,   217,   218,   499,   268,    15,   207,   221,    16,    15,
    17,    18,    16,    15,    17,    18,    16,   100,    17,    18,
   387,   327,   227,   228,   343,   316,   317,   318,   319,   320,
   321,   322,   162,   387,   273,   163,    14,   236,   586,   162,
   274,   240,   163,    14,   276,   354,   363,   363,   253,   254,
   278,   314,   209,   162,   280,   209,   163,    14,   375,   170,
   303,   309,   259,   260,   310,   304,   311,   385,   363,   588,
   114,   169,   312,   270,   271,   272,   407,   363,   363,   482,
   483,   598,   486,   379,   282,   283,   601,   314,   359,   323,
   288,   289,   290,  -261,   363,   299,   387,   301,   101,   102,
   324,   103,   352,   104,   105,   106,   107,   108,   109,   110,
   111,   378,   380,   115,   116,   117,   118,   119,   120,   121,
   325,   326,   384,   200,   201,   202,   162,   328,   203,   163,
    14,   346,   424,   425,   426,   427,   428,   429,   430,   431,
   349,   351,   170,   172,   410,   439,   440,   441,   417,   418,
   422,   101,   102,   355,   103,   442,   104,   105,   106,   107,
   108,   109,   110,   111,    53,   188,    54,   462,   466,   383,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
   192,    65,    66,   468,   408,   115,   116,   117,   118,   119,
   120,   121,    87,   470,    88,   414,   495,   416,   498,   500,
   501,    89,    90,    91,    92,   502,   503,   115,   116,   117,
   118,   119,   120,   121,   170,   433,   433,   433,   515,   433,
   516,   603,   115,   116,   117,   118,   119,   120,   121,   517,
   363,   518,   519,   520,   463,   521,   170,    67,   241,   242,
   243,    68,   522,   244,   523,   363,   604,   533,   535,   101,
   102,   170,   103,   536,   104,   105,   106,   107,   108,   109,
   110,   111,   170,   335,   537,   538,   363,   539,   150,   336,
   481,   481,   481,   541,   481,   542,   554,   363,   555,   490,
   363,   491,   492,   493,   494,    87,   557,    88,   558,   287,
   559,   363,   561,   363,    89,    90,    91,    92,   569,   570,
   572,   363,   126,   553,   363,   337,   338,   162,   339,   340,
   163,    14,   504,   505,   506,   507,   508,   509,   510,   511,
   514,    88,   573,   575,   585,   587,   593,   341,    89,    90,
    91,    92,   524,   525,   526,   527,   514,   529,   530,   531,
   179,   180,   181,   182,   183,   184,   185,   594,   583,   373,
   544,   366,   545,   546,   547,   186,   549,   367,   550,   551,
   552,   104,   105,   106,   107,   108,   109,   110,   111,   574,
   528,   562,   563,   564,   565,   566,   600,   297,   568,   115,
   116,   117,   118,   119,   120,   121,   576,   302,   307,   580,
     0,     0,   582,   368,   369,   162,   370,   371,   163,    14,
     0,     0,   590,     0,   592,     0,     0,     0,     0,     0,
   595,     0,     0,     0,     0,   372,   388,   389,   390,   391,
   392,   393,   394,   395,   396,   397,   398,   399,   400,   401,
   402,   403,   404,   405,   443,   444,   445,   446,   447,   448,
   449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
   459,   460,   127,   128,   129,   130,   131,   132,   133,     0,
   135,     0,   137,   138,   139,   140,   472,   473,   474,   475,
   476,   477,   478,   479,   406,   141,   329,     0,     0,   487,
   488,   489,     0,   330,   331,   332,   333,     0,     0,     0,
   334,   102,   461,   103,     0,   104,   105,   106,   107,   108,
   109,   110,   111,   103,     0,   104,   105,   106,   107,   108,
   109,   110,   111,   106,   107,   108,   109,   110,   111
};

static const short yycheck[] = {     2,
    16,     1,     1,     6,   127,    15,   340,    15,    10,   132,
   133,    16,   135,   204,    17,    16,    88,   140,     9,    88,
   280,    16,    13,    14,    10,    16,    49,   361,    31,    32,
    33,    34,    35,     6,   313,    17,   370,   371,   313,   111,
    10,     3,     4,     5,     6,     7,     8,    13,    14,    11,
    12,    13,    14,   387,    16,    13,    14,    15,    31,    32,
    33,    34,    35,   323,   324,    10,    89,    29,   347,   111,
   261,    17,   347,    16,    84,    89,    84,    59,    60,    61,
    62,    63,    64,    65,   110,    13,    14,    15,   104,    13,
    14,    15,    16,   109,    13,    14,   111,   111,   101,   102,
   103,   104,   105,   106,   107,   108,   109,   110,   111,    88,
    72,   102,   103,   236,   109,   106,   107,   111,   109,   368,
   380,    14,    13,    14,    49,    50,    13,    14,   101,   102,
   103,   104,   105,   106,   107,   108,   109,   110,   111,   388,
    93,   103,   108,   109,   106,   103,   108,   109,   106,    15,
   108,   109,   401,   104,   105,    82,    83,   110,    13,    14,
    15,    89,    89,   111,    89,    55,    56,    57,    59,    60,
    61,    62,    63,    64,    65,   103,    82,    83,   106,   103,
   108,   109,   106,    89,   108,   109,   102,   103,   111,   523,
    80,    81,    15,    93,   443,    85,   399,   400,   111,   402,
    15,    16,    89,    18,   538,    13,    14,   456,    16,    13,
    14,   111,   103,    13,    14,   106,   103,   108,   109,   106,
    68,   108,   109,    71,    72,   559,   102,   103,   104,   105,
   421,   422,   125,   111,   128,   129,   570,   130,   131,   573,
    88,    93,   111,    93,   138,   436,   139,   141,   103,   142,
   584,   106,   586,   108,   109,    55,    56,    57,   111,   111,
   594,   111,    68,   597,   111,    71,    72,   111,    15,   162,
   163,    15,    87,    88,    89,    90,    91,    92,   469,   470,
    80,    81,    88,   111,   178,    85,   179,   180,   181,   182,
   183,   184,   185,   484,   111,   103,    88,   191,   106,   103,
   108,   109,   106,   103,   108,   109,   106,    10,   108,   109,
    93,    15,   205,   206,   313,   290,   291,   292,   293,   294,
   295,   296,    68,    93,   111,    71,    72,   220,   111,    68,
   111,   224,    71,    72,   111,   335,   339,   340,   231,   232,
   111,   111,    88,    68,   111,    88,    71,    72,   347,    88,
   111,    84,   245,   246,    87,   111,    89,   360,   361,   583,
    10,    17,   111,    88,   257,   258,   366,   370,   371,   454,
   455,   595,   457,   348,   267,   268,   600,   111,    15,   111,
   273,   274,   275,   111,   387,   278,    93,   280,    91,    92,
   111,    94,   111,    96,    97,    98,    99,   100,   101,   102,
   103,   111,   111,    59,    60,    61,    62,    63,    64,    65,
   303,   304,   111,    13,    14,    15,    68,   310,    18,    71,
    72,   314,   390,   391,   392,   393,   394,   395,   396,   397,
   323,   324,    88,    17,    15,   403,   404,   405,   111,    15,
    15,    91,    92,   336,    94,   111,    96,    97,    98,    99,
   100,   101,   102,   103,    13,    17,    15,   111,   111,   352,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    17,    30,    31,    15,   367,    59,    60,    61,    62,    63,
    64,    65,    93,    15,    95,   378,   111,   380,   111,   111,
   111,   102,   103,   104,   105,   111,   111,    59,    60,    61,
    62,    63,    64,    65,    88,   398,   399,   400,   111,   402,
   111,     0,    59,    60,    61,    62,    63,    64,    65,   111,
   523,   111,   111,   111,   417,   111,    88,    86,    13,    14,
    15,    90,   111,    18,   111,   538,     0,   111,   111,    91,
    92,    88,    94,   111,    96,    97,    98,    99,   100,   101,
   102,   103,    88,    24,   111,   111,   559,   111,   110,    30,
   453,   454,   455,   111,   457,   111,   111,   570,   111,   462,
   573,   464,   465,   466,   467,    93,   111,    95,   111,   272,
   111,   584,   111,   586,   102,   103,   104,   105,   111,   111,
   111,   594,   110,   545,   597,    66,    67,    68,    69,    70,
    71,    72,   495,   496,   497,   498,   499,   500,   501,   502,
   503,    95,   111,   111,   111,   111,   111,    88,   102,   103,
   104,   105,   515,   516,   517,   518,   519,   520,   521,   522,
    73,    74,    75,    76,    77,    78,    79,   111,   576,   347,
   533,    24,   535,   536,   537,    88,   539,    30,   541,   542,
   543,    96,    97,    98,    99,   100,   101,   102,   103,   566,
   519,   554,   555,   556,   557,   558,   599,   277,   561,    59,
    60,    61,    62,    63,    64,    65,   569,   281,   284,   572,
    -1,    -1,   575,    66,    67,    68,    69,    70,    71,    72,
    -1,    -1,   585,    -1,   587,    -1,    -1,    -1,    -1,    -1,
   593,    -1,    -1,    -1,    -1,    88,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    55,    56,    57,    58,    59,    60,    61,    -1,
    63,    -1,    65,    66,    67,    68,   445,   446,   447,   448,
   449,   450,   451,   452,    89,    78,    44,    -1,    -1,   458,
   459,   460,    -1,    51,    52,    53,    54,    -1,    -1,    -1,
    58,    92,    89,    94,    -1,    96,    97,    98,    99,   100,
   101,   102,   103,    94,    -1,    96,    97,    98,    99,   100,
   101,   102,   103,    98,    99,   100,   101,   102,   103
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
#line 310 "parser.y"
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
		/* Final statement before were done */
		resource_top = get_resource_head(yyvsp[0].res);
		;
    break;}
case 2:
#line 330 "parser.y"
{ yyval.res = NULL; want_rscname = 1; ;
    break;}
case 3:
#line 331 "parser.y"
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
		want_rscname = 1;
		;
    break;}
case 4:
#line 356 "parser.y"
{ yyval.res = yyvsp[-1].res; want_rscname = 1; ;
    break;}
case 5:
#line 357 "parser.y"
{ yyval.res = yyvsp[-1].res; want_rscname = 1; ;
    break;}
case 6:
#line 362 "parser.y"
{ pop_start(); push_if(yyvsp[-1].iptr ? *(yyvsp[-1].iptr) : 0, 0, 0); if(yyvsp[-1].iptr) free(yyvsp[-1].iptr);;
    break;}
case 7:
#line 363 "parser.y"
{ pop_start(); push_if(pp_lookup(yyvsp[-1].str->str.cstr) != NULL, 0, 0); ;
    break;}
case 8:
#line 364 "parser.y"
{ pop_start(); push_if(pp_lookup(yyvsp[-1].str->str.cstr) == NULL, 0, 0); ;
    break;}
case 9:
#line 365 "parser.y"
{ pop_start(); push_if(yyvsp[-1].iptr ? *(yyvsp[-1].iptr) : 0, pop_if(), 0); if(yyvsp[-1].iptr) free(yyvsp[-1].iptr); ;
    break;}
case 10:
#line 366 "parser.y"
{ pop_start(); push_if(1, pop_if(), 0); ;
    break;}
case 11:
#line 367 "parser.y"
{ pop_if(); ;
    break;}
case 12:
#line 370 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; ;
    break;}
case 13:
#line 371 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr || *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 14:
#line 372 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr && *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 15:
#line 373 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  + *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 16:
#line 374 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  - *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 17:
#line 375 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  ^ *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 18:
#line 376 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr == *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 19:
#line 377 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr != *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 20:
#line 378 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  < *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 21:
#line 379 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  > *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 22:
#line 380 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr <= *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 23:
#line 381 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr >= *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 24:
#line 382 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; if(yyvsp[0].iptr) *yyvsp[0].iptr = ~(*yyvsp[0].iptr); ;
    break;}
case 25:
#line 383 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; ;
    break;}
case 26:
#line 384 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; if(yyvsp[0].iptr) *yyvsp[0].iptr = -(*yyvsp[0].iptr); ;
    break;}
case 27:
#line 385 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; if(yyvsp[0].iptr) *yyvsp[0].iptr = !(*yyvsp[0].iptr); ;
    break;}
case 28:
#line 386 "parser.y"
{ yyval.iptr = yyvsp[-1].iptr; ;
    break;}
case 29:
#line 390 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 30:
#line 391 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 31:
#line 392 "parser.y"
{ yyval.iptr = new_int(pp_lookup(yyvsp[0].str->str.cstr) != NULL); ;
    break;}
case 32:
#line 393 "parser.y"
{ yyval.iptr = new_int(pp_lookup(yyvsp[-1].str->str.cstr) != NULL); ;
    break;}
case 33:
#line 397 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 34:
#line 398 "parser.y"
{ strip_extern(); ;
    break;}
case 35:
#line 399 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 36:
#line 400 "parser.y"
{ strip_til_parenthesis(); ;
    break;}
case 37:
#line 401 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 38:
#line 406 "parser.y"
{
		yyval.res = yyvsp[0].res;
		if(yyval.res)
		{
			yyval.res->name = yyvsp[-1].nid;
			if(yyvsp[-1].nid->type == name_ord)
			{
				chat("Got %s (%d)",get_typename(yyvsp[0].res),yyvsp[-1].nid->name.i_name);
			}
			else if(yyvsp[-1].nid->type == name_str)
			{
				chat("Got %s (%s)",get_typename(yyvsp[0].res),yyvsp[-1].nid->name.s_name->str.cstr);
			}
		}
		;
    break;}
case 39:
#line 421 "parser.y"
{
		/* Don't do anything, stringtables are converted to
		 * resource_t structures when we are finished parsing and
		 * the final rule of the parser is reduced (see above)
		 */
		yyval.res = NULL;
		chat("Got STRINGTABLE");
		;
    break;}
case 40:
#line 429 "parser.y"
{
		if(!win32)
			yywarning("LANGUAGE not supported in 16-bit mode");
		if(currentlanguage)
			free(currentlanguage);
		currentlanguage = yyvsp[0].lan;
		yyval.res = NULL;
		;
    break;}
case 41:
#line 442 "parser.y"
{
		if(yyvsp[0].num > 65535 || yyvsp[0].num < -32768)
			yyerror("Resource's ID out of range (%d)", yyvsp[0].num);
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		want_rscname = 0;
		;
    break;}
case 42:
#line 450 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		want_rscname = 0;
		;
    break;}
case 43:
#line 461 "parser.y"
{ yyval.nid = yyvsp[0].nid; ;
    break;}
case 44:
#line 462 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		want_rscname = 0;
		;
    break;}
case 45:
#line 472 "parser.y"
{ yyval.res = new_resource(res_acc, yyvsp[0].acc, yyvsp[0].acc->memopt, yyvsp[0].acc->lvc.language); ;
    break;}
case 46:
#line 473 "parser.y"
{ yyval.res = new_resource(res_bmp, yyvsp[0].bmp, yyvsp[0].bmp->memopt, dup_language(currentlanguage)); ;
    break;}
case 47:
#line 474 "parser.y"
{
		resource_t *rsc;
		cursor_t *cur;
		yyval.res = rsc = new_resource(res_curg, yyvsp[0].curg, yyvsp[0].curg->memopt, dup_language(currentlanguage));
		for(cur = yyvsp[0].curg->cursorlist; cur; cur = cur->next)
		{
			rsc->prev = new_resource(res_cur, cur, yyvsp[0].curg->memopt, dup_language(currentlanguage));
			rsc->prev->next = rsc;
			rsc = rsc->prev;
			rsc->name = new_name_id();
			rsc->name->type = name_ord;
			rsc->name->name.i_name = cur->id;
		}
		;
    break;}
case 48:
#line 488 "parser.y"
{ yyval.res = new_resource(res_dlg, yyvsp[0].dlg, yyvsp[0].dlg->memopt, yyvsp[0].dlg->lvc.language); ;
    break;}
case 49:
#line 489 "parser.y"
{
		if(win32)
			yyval.res = new_resource(res_dlgex, yyvsp[0].dlgex, yyvsp[0].dlgex->memopt, yyvsp[0].dlgex->lvc.language);
		else
			yyval.res = NULL;
		;
    break;}
case 50:
#line 495 "parser.y"
{ yyval.res = new_resource(res_dlginit, yyvsp[0].dginit, yyvsp[0].dginit->memopt, yyvsp[0].dginit->lvc.language); ;
    break;}
case 51:
#line 496 "parser.y"
{ yyval.res = new_resource(res_fnt, yyvsp[0].fnt, yyvsp[0].fnt->memopt, dup_language(currentlanguage)); ;
    break;}
case 52:
#line 497 "parser.y"
{
		resource_t *rsc;
		icon_t *ico;
		yyval.res = rsc = new_resource(res_icog, yyvsp[0].icog, yyvsp[0].icog->memopt, dup_language(currentlanguage));
		for(ico = yyvsp[0].icog->iconlist; ico; ico = ico->next)
		{
			rsc->prev = new_resource(res_ico, ico, yyvsp[0].icog->memopt, dup_language(currentlanguage));
			rsc->prev->next = rsc;
			rsc = rsc->prev;
			rsc->name = new_name_id();
			rsc->name->type = name_ord;
			rsc->name->name.i_name = ico->id;
		}
		;
    break;}
case 53:
#line 511 "parser.y"
{ yyval.res = new_resource(res_men, yyvsp[0].men, yyvsp[0].men->memopt, yyvsp[0].men->lvc.language); ;
    break;}
case 54:
#line 512 "parser.y"
{
		if(win32)
			yyval.res = new_resource(res_menex, yyvsp[0].menex, yyvsp[0].menex->memopt, yyvsp[0].menex->lvc.language);
		else
			yyval.res = NULL;
		;
    break;}
case 55:
#line 518 "parser.y"
{ yyval.res = new_resource(res_msg, yyvsp[0].msg, WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE, dup_language(currentlanguage)); ;
    break;}
case 56:
#line 519 "parser.y"
{ yyval.res = new_resource(res_rdt, yyvsp[0].rdt, yyvsp[0].rdt->memopt, yyvsp[0].rdt->lvc.language); ;
    break;}
case 57:
#line 520 "parser.y"
{ yyval.res = new_resource(res_toolbar, yyvsp[0].tlbar, yyvsp[0].tlbar->memopt, yyvsp[0].tlbar->lvc.language); ;
    break;}
case 58:
#line 521 "parser.y"
{ yyval.res = new_resource(res_usr, yyvsp[0].usr, yyvsp[0].usr->memopt, dup_language(currentlanguage)); ;
    break;}
case 59:
#line 522 "parser.y"
{ yyval.res = new_resource(res_ver, yyvsp[0].veri, WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE, dup_language(currentlanguage)); ;
    break;}
case 60:
#line 526 "parser.y"
{ yyval.bmp = new_bitmap(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 61:
#line 527 "parser.y"
{ yyval.bmp = new_bitmap(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 62:
#line 531 "parser.y"
{ yyval.curg = new_cursor_group(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 63:
#line 532 "parser.y"
{ yyval.curg = new_cursor_group(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 64:
#line 537 "parser.y"
{ yyval.fnt = new_font(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 65:
#line 541 "parser.y"
{ yyval.icog = new_icon_group(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 66:
#line 542 "parser.y"
{ yyval.icog = new_icon_group(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 67:
#line 550 "parser.y"
{
		if(!win32)
			yywarning("MESSAGETABLE not supported in 16-bit mode");
		yyval.msg = new_messagetable(load_file(yyvsp[0].str));
		;
    break;}
case 68:
#line 558 "parser.y"
{
		yyval.rdt = new_rcdata(yyvsp[0].raw, yyvsp[-2].iptr);
		if(yyvsp[-1].lvc)
		{
			yyval.rdt->lvc = *(yyvsp[-1].lvc);
			free(yyvsp[-1].lvc);
		}
		if(!yyval.rdt->lvc.language)
			yyval.rdt->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 69:
#line 571 "parser.y"
{
		yyval.dginit = new_dlginit(yyvsp[0].raw, yyvsp[-2].iptr);
		if(yyvsp[-1].lvc)
		{
			yyval.dginit->lvc = *(yyvsp[-1].lvc);
			free(yyvsp[-1].lvc);
		}
		if(!yyval.dginit->lvc.language)
			yyval.dginit->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 70:
#line 584 "parser.y"
{ yyval.usr = new_user(yyvsp[-2].nid, load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 71:
#line 585 "parser.y"
{ yyval.usr = new_user(yyvsp[-2].nid, yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 72:
#line 604 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		set_yywf();
		;
    break;}
case 73:
#line 616 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		set_yywf();
		;
    break;}
case 74:
#line 626 "parser.y"
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
case 75:
#line 650 "parser.y"
{ yyval.event=NULL; ;
    break;}
case 76:
#line 651 "parser.y"
{ yyval.event=add_string_event(yyvsp[-3].str, yyvsp[-1].num, yyvsp[0].num, yyvsp[-4].event); ;
    break;}
case 77:
#line 652 "parser.y"
{ yyval.event=add_event(yyvsp[-3].num, yyvsp[-1].num, yyvsp[0].num, yyvsp[-4].event); ;
    break;}
case 78:
#line 655 "parser.y"
{ yyval.num=0; ;
    break;}
case 79:
#line 656 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_NOINVERT; ;
    break;}
case 80:
#line 657 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_SHIFT; ;
    break;}
case 81:
#line 658 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_CONTROL; ;
    break;}
case 82:
#line 659 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_ALT; ;
    break;}
case 83:
#line 660 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_ASCII; ;
    break;}
case 84:
#line 661 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_VIRTKEY; ;
    break;}
case 85:
#line 667 "parser.y"
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

		indialog = FALSE;
		if(!yyval.dlg->lvc.language)
			yyval.dlg->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 86:
#line 701 "parser.y"
{ yyval.dlg=new_dialog(); ;
    break;}
case 87:
#line 702 "parser.y"
{ yyval.dlg=dialog_style(yyvsp[0].style,yyvsp[-2].dlg); ;
    break;}
case 88:
#line 703 "parser.y"
{ yyval.dlg=dialog_exstyle(yyvsp[0].style,yyvsp[-2].dlg); ;
    break;}
case 89:
#line 704 "parser.y"
{ yyval.dlg=dialog_caption(yyvsp[0].str,yyvsp[-2].dlg); ;
    break;}
case 90:
#line 705 "parser.y"
{ yyval.dlg=dialog_font(yyvsp[0].fntid,yyvsp[-1].dlg); ;
    break;}
case 91:
#line 706 "parser.y"
{ yyval.dlg=dialog_class(yyvsp[0].nid,yyvsp[-2].dlg); ;
    break;}
case 92:
#line 707 "parser.y"
{ yyval.dlg=dialog_menu(yyvsp[0].nid,yyvsp[-2].dlg); ;
    break;}
case 93:
#line 708 "parser.y"
{ yyval.dlg=dialog_language(yyvsp[0].lan,yyvsp[-1].dlg); ;
    break;}
case 94:
#line 709 "parser.y"
{ yyval.dlg=dialog_characteristics(yyvsp[0].chars,yyvsp[-1].dlg); ;
    break;}
case 95:
#line 710 "parser.y"
{ yyval.dlg=dialog_version(yyvsp[0].ver,yyvsp[-1].dlg); ;
    break;}
case 96:
#line 713 "parser.y"
{ yyval.ctl = NULL; ;
    break;}
case 97:
#line 714 "parser.y"
{ yyval.ctl=ins_ctrl(-1, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 98:
#line 715 "parser.y"
{ yyval.ctl=ins_ctrl(CT_EDIT, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 99:
#line 716 "parser.y"
{ yyval.ctl=ins_ctrl(CT_LISTBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 100:
#line 717 "parser.y"
{ yyval.ctl=ins_ctrl(CT_COMBOBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 101:
#line 718 "parser.y"
{ yyval.ctl=ins_ctrl(CT_SCROLLBAR, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 102:
#line 719 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_CHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 103:
#line 720 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_DEFPUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 104:
#line 721 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_GROUPBOX, yyvsp[0].ctl, yyvsp[-2].ctl);;
    break;}
case 105:
#line 722 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_PUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 106:
#line 724 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_RADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 107:
#line 725 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTO3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 108:
#line 726 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 109:
#line 727 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTOCHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 110:
#line 728 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTORADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 111:
#line 729 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_LEFT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 112:
#line 730 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_CENTER, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 113:
#line 731 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_RIGHT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 114:
#line 733 "parser.y"
{
		yyvsp[0].ctl->title = yyvsp[-7].nid;
		yyvsp[0].ctl->id = yyvsp[-5].num;
		yyvsp[0].ctl->x = yyvsp[-3].num;
		yyvsp[0].ctl->y = yyvsp[-1].num;
		yyval.ctl = ins_ctrl(CT_STATIC, SS_ICON, yyvsp[0].ctl, yyvsp[-9].ctl);
		;
    break;}
case 115:
#line 743 "parser.y"
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
case 116:
#line 762 "parser.y"
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
case 117:
#line 778 "parser.y"
{ yyval.ctl = new_control(); ;
    break;}
case 118:
#line 780 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-2].num;
		yyval.ctl->height = yyvsp[0].num;
		;
    break;}
case 119:
#line 785 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-4].num;
		yyval.ctl->height = yyvsp[-2].num;
		yyval.ctl->style = yyvsp[0].style;
		yyval.ctl->gotstyle = TRUE;
		;
    break;}
case 120:
#line 792 "parser.y"
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
case 121:
#line 803 "parser.y"
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
case 122:
#line 817 "parser.y"
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
case 123:
#line 832 "parser.y"
{ yyval.fntid = new_font_id(yyvsp[-2].num, yyvsp[0].str, 0, 0); ;
    break;}
case 124:
#line 837 "parser.y"
{ yyval.style = NULL; ;
    break;}
case 125:
#line 838 "parser.y"
{ yyval.style = yyvsp[0].style; ;
    break;}
case 126:
#line 842 "parser.y"
{ yyval.styles = NULL; ;
    break;}
case 127:
#line 843 "parser.y"
{ yyval.styles = new_style_pair(yyvsp[0].style, 0); ;
    break;}
case 128:
#line 844 "parser.y"
{ yyval.styles = new_style_pair(yyvsp[-2].style, yyvsp[0].style); ;
    break;}
case 129:
#line 848 "parser.y"
{ yyval.style = new_style(yyvsp[-2].style->or_mask | yyvsp[0].style->or_mask, yyvsp[-2].style->and_mask | yyvsp[0].style->and_mask); free(yyvsp[-2].style); free(yyvsp[0].style);;
    break;}
case 130:
#line 849 "parser.y"
{ yyval.style = yyvsp[-1].style; ;
    break;}
case 131:
#line 850 "parser.y"
{ yyval.style = new_style(yyvsp[0].num, 0); ;
    break;}
case 132:
#line 851 "parser.y"
{ yyval.style = new_style(0, yyvsp[0].num); ;
    break;}
case 133:
#line 855 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		;
    break;}
case 134:
#line 860 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		;
    break;}
case 135:
#line 869 "parser.y"
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

		indialog = FALSE;
		if(!yyval.dlgex->lvc.language)
			yyval.dlgex->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 136:
#line 913 "parser.y"
{ yyval.dlgex=new_dialogex(); ;
    break;}
case 137:
#line 914 "parser.y"
{ yyval.dlgex=dialogex_style(yyvsp[0].style,yyvsp[-2].dlgex); ;
    break;}
case 138:
#line 915 "parser.y"
{ yyval.dlgex=dialogex_exstyle(yyvsp[0].style,yyvsp[-2].dlgex); ;
    break;}
case 139:
#line 916 "parser.y"
{ yyval.dlgex=dialogex_caption(yyvsp[0].str,yyvsp[-2].dlgex); ;
    break;}
case 140:
#line 917 "parser.y"
{ yyval.dlgex=dialogex_font(yyvsp[0].fntid,yyvsp[-1].dlgex); ;
    break;}
case 141:
#line 918 "parser.y"
{ yyval.dlgex=dialogex_font(yyvsp[0].fntid,yyvsp[-1].dlgex); ;
    break;}
case 142:
#line 919 "parser.y"
{ yyval.dlgex=dialogex_class(yyvsp[0].nid,yyvsp[-2].dlgex); ;
    break;}
case 143:
#line 920 "parser.y"
{ yyval.dlgex=dialogex_menu(yyvsp[0].nid,yyvsp[-2].dlgex); ;
    break;}
case 144:
#line 921 "parser.y"
{ yyval.dlgex=dialogex_language(yyvsp[0].lan,yyvsp[-1].dlgex); ;
    break;}
case 145:
#line 922 "parser.y"
{ yyval.dlgex=dialogex_characteristics(yyvsp[0].chars,yyvsp[-1].dlgex); ;
    break;}
case 146:
#line 923 "parser.y"
{ yyval.dlgex=dialogex_version(yyvsp[0].ver,yyvsp[-1].dlgex); ;
    break;}
case 147:
#line 926 "parser.y"
{ yyval.ctl = NULL; ;
    break;}
case 148:
#line 927 "parser.y"
{ yyval.ctl=ins_ctrl(-1, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 149:
#line 928 "parser.y"
{ yyval.ctl=ins_ctrl(CT_EDIT, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 150:
#line 929 "parser.y"
{ yyval.ctl=ins_ctrl(CT_LISTBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 151:
#line 930 "parser.y"
{ yyval.ctl=ins_ctrl(CT_COMBOBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 152:
#line 931 "parser.y"
{ yyval.ctl=ins_ctrl(CT_SCROLLBAR, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 153:
#line 932 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_CHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 154:
#line 933 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_DEFPUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 155:
#line 934 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_GROUPBOX, yyvsp[0].ctl, yyvsp[-2].ctl);;
    break;}
case 156:
#line 935 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_PUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 157:
#line 937 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_RADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 158:
#line 938 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTO3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 159:
#line 939 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 160:
#line 940 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTOCHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 161:
#line 941 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTORADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 162:
#line 942 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_LEFT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 163:
#line 943 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_CENTER, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 164:
#line 944 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_RIGHT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 165:
#line 946 "parser.y"
{
		yyvsp[0].ctl->title = yyvsp[-7].nid;
		yyvsp[0].ctl->id = yyvsp[-5].num;
		yyvsp[0].ctl->x = yyvsp[-3].num;
		yyvsp[0].ctl->y = yyvsp[-1].num;
		yyval.ctl = ins_ctrl(CT_STATIC, SS_ICON, yyvsp[0].ctl, yyvsp[-9].ctl);
		;
    break;}
case 166:
#line 957 "parser.y"
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
case 167:
#line 981 "parser.y"
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
case 168:
#line 997 "parser.y"
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
case 169:
#line 1025 "parser.y"
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
case 170:
#line 1048 "parser.y"
{ yyval.raw = NULL; ;
    break;}
case 171:
#line 1049 "parser.y"
{ yyval.raw = yyvsp[0].raw; ;
    break;}
case 172:
#line 1052 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 173:
#line 1053 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 174:
#line 1057 "parser.y"
{ yyval.fntid = new_font_id(yyvsp[-7].num, yyvsp[-5].str, yyvsp[-3].num, yyvsp[-1].num); ;
    break;}
case 175:
#line 1064 "parser.y"
{ yyval.fntid = NULL; ;
    break;}
case 176:
#line 1065 "parser.y"
{ yyval.fntid = NULL; ;
    break;}
case 177:
#line 1069 "parser.y"
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
case 178:
#line 1092 "parser.y"
{ yyval.menitm = yyvsp[-1].menitm; ;
    break;}
case 179:
#line 1096 "parser.y"
{yyval.menitm = NULL;;
    break;}
case 180:
#line 1097 "parser.y"
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
case 181:
#line 1106 "parser.y"
{
		yyval.menitm=new_menu_item();
		yyval.menitm->prev = yyvsp[-2].menitm;
		if(yyvsp[-2].menitm)
			yyvsp[-2].menitm->next = yyval.menitm;
		;
    break;}
case 182:
#line 1112 "parser.y"
{
		yyval.menitm = new_menu_item();
		yyval.menitm->prev = yyvsp[-4].menitm;
		if(yyvsp[-4].menitm)
			yyvsp[-4].menitm->next = yyval.menitm;
		yyval.menitm->popup = get_item_head(yyvsp[0].menitm);
		yyval.menitm->name = yyvsp[-2].str;
		;
    break;}
case 183:
#line 1131 "parser.y"
{ yyval.num = 0; ;
    break;}
case 184:
#line 1132 "parser.y"
{ yyval.num = yyvsp[0].num | MF_CHECKED; ;
    break;}
case 185:
#line 1133 "parser.y"
{ yyval.num = yyvsp[0].num | MF_GRAYED; ;
    break;}
case 186:
#line 1134 "parser.y"
{ yyval.num = yyvsp[0].num | MF_HELP; ;
    break;}
case 187:
#line 1135 "parser.y"
{ yyval.num = yyvsp[0].num | MF_DISABLED; ;
    break;}
case 188:
#line 1136 "parser.y"
{ yyval.num = yyvsp[0].num | MF_MENUBARBREAK; ;
    break;}
case 189:
#line 1137 "parser.y"
{ yyval.num = yyvsp[0].num | MF_MENUBREAK; ;
    break;}
case 190:
#line 1141 "parser.y"
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
case 191:
#line 1166 "parser.y"
{ yyval.menexitm = yyvsp[-1].menexitm; ;
    break;}
case 192:
#line 1170 "parser.y"
{yyval.menexitm = NULL; ;
    break;}
case 193:
#line 1171 "parser.y"
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
case 194:
#line 1187 "parser.y"
{
		yyval.menexitm = new_menuex_item();
		yyval.menexitm->prev = yyvsp[-2].menexitm;
		if(yyvsp[-2].menexitm)
			yyvsp[-2].menexitm->next = yyval.menexitm;
		;
    break;}
case 195:
#line 1193 "parser.y"
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
case 196:
#line 1213 "parser.y"
{ yyval.exopt = new_itemex_opt(0, 0, 0, 0); ;
    break;}
case 197:
#line 1214 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[0].num, 0, 0, 0);
		yyval.exopt->gotid = TRUE;
		;
    break;}
case 198:
#line 1218 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-3].iptr ? *(yyvsp[-3].iptr) : 0, yyvsp[-1].iptr ? *(yyvsp[-1].iptr) : 0, yyvsp[0].num, 0);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		if(yyvsp[-3].iptr) free(yyvsp[-3].iptr);
		if(yyvsp[-1].iptr) free(yyvsp[-1].iptr);
		;
    break;}
case 199:
#line 1226 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-4].iptr ? *(yyvsp[-4].iptr) : 0, yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		if(yyvsp[-4].iptr) free(yyvsp[-4].iptr);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		;
    break;}
case 200:
#line 1237 "parser.y"
{ yyval.exopt = new_itemex_opt(0, 0, 0, 0); ;
    break;}
case 201:
#line 1238 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[0].num, 0, 0, 0);
		yyval.exopt->gotid = TRUE;
		;
    break;}
case 202:
#line 1242 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0, 0);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		;
    break;}
case 203:
#line 1248 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-4].iptr ? *(yyvsp[-4].iptr) : 0, yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0);
		if(yyvsp[-4].iptr) free(yyvsp[-4].iptr);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		;
    break;}
case 204:
#line 1256 "parser.y"
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
case 205:
#line 1276 "parser.y"
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
case 206:
#line 1317 "parser.y"
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
case 207:
#line 1328 "parser.y"
{ yyval.stt = NULL; ;
    break;}
case 208:
#line 1329 "parser.y"
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

		if(!win32 && yyvsp[0].str->size > 254)
			yyerror("Stringtable entry more than 254 characters");
		if(win32 && yyvsp[0].str->size > 65534) /* Hmm..., does this happen? */
			yyerror("Stringtable entry more than 65534 characters (probably something else that went wrong)");
		yyval.stt = tagstt;
		;
    break;}
case 211:
#line 1367 "parser.y"
{
		yyval.veri = yyvsp[-3].veri;
		yyvsp[-3].veri->blocks = get_ver_block_head(yyvsp[-1].blk);
		;
    break;}
case 212:
#line 1374 "parser.y"
{ yyval.veri = new_versioninfo(); ;
    break;}
case 213:
#line 1375 "parser.y"
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
case 214:
#line 1385 "parser.y"
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
case 215:
#line 1395 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ff)
			yyerror("FILEFLAGS already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileflags = yyvsp[0].num;
		yyval.veri->gotit.ff = 1;
		;
    break;}
case 216:
#line 1402 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ffm)
			yyerror("FILEFLAGSMASK already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileflagsmask = yyvsp[0].num;
		yyval.veri->gotit.ffm = 1;
		;
    break;}
case 217:
#line 1409 "parser.y"
{
		if(yyvsp[-2].veri->gotit.fo)
			yyerror("FILEOS already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileos = yyvsp[0].num;
		yyval.veri->gotit.fo = 1;
		;
    break;}
case 218:
#line 1416 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ft)
			yyerror("FILETYPE already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->filetype = yyvsp[0].num;
		yyval.veri->gotit.ft = 1;
		;
    break;}
case 219:
#line 1423 "parser.y"
{
		if(yyvsp[-2].veri->gotit.fst)
			yyerror("FILESUBTYPE already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->filesubtype = yyvsp[0].num;
		yyval.veri->gotit.fst = 1;
		;
    break;}
case 220:
#line 1433 "parser.y"
{ yyval.blk = NULL; ;
    break;}
case 221:
#line 1434 "parser.y"
{
		yyval.blk = yyvsp[0].blk;
		yyval.blk->prev = yyvsp[-1].blk;
		if(yyvsp[-1].blk)
			yyvsp[-1].blk->next = yyval.blk;
		;
    break;}
case 222:
#line 1443 "parser.y"
{
		yyval.blk = new_ver_block();
		yyval.blk->name = yyvsp[-3].str;
		yyval.blk->values = get_ver_value_head(yyvsp[-1].val);
		;
    break;}
case 223:
#line 1451 "parser.y"
{ yyval.val = NULL; ;
    break;}
case 224:
#line 1452 "parser.y"
{
		yyval.val = yyvsp[0].val;
		yyval.val->prev = yyvsp[-1].val;
		if(yyvsp[-1].val)
			yyvsp[-1].val->next = yyval.val;
		;
    break;}
case 225:
#line 1461 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_block;
		yyval.val->value.block = yyvsp[0].blk;
		;
    break;}
case 226:
#line 1466 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_str;
		yyval.val->key = yyvsp[-2].str;
		yyval.val->value.str = yyvsp[0].str;
		;
    break;}
case 227:
#line 1472 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_words;
		yyval.val->key = yyvsp[-2].str;
		yyval.val->value.words = yyvsp[0].verw;
		;
    break;}
case 228:
#line 1481 "parser.y"
{ yyval.verw = new_ver_words(yyvsp[0].num); ;
    break;}
case 229:
#line 1482 "parser.y"
{ yyval.verw = add_ver_words(yyvsp[-2].verw, yyvsp[0].num); ;
    break;}
case 230:
#line 1486 "parser.y"
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
case 231:
#line 1512 "parser.y"
{ yyval.tlbarItems = NULL; ;
    break;}
case 232:
#line 1513 "parser.y"
{         
		toolbar_item_t *idrec = new_toolbar_item();
		idrec->id = yyvsp[0].num;
		yyval.tlbarItems = ins_tlbr_button(yyvsp[-2].tlbarItems, idrec); 
		;
    break;}
case 233:
#line 1518 "parser.y"
{         
		toolbar_item_t *idrec = new_toolbar_item();
		idrec->id = 0;
		yyval.tlbarItems = ins_tlbr_button(yyvsp[-1].tlbarItems, idrec); 
	;
    break;}
case 234:
#line 1527 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 235:
#line 1528 "parser.y"
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
case 236:
#line 1538 "parser.y"
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
case 237:
#line 1553 "parser.y"
{ yyval.iptr = new_int(WRC_MO_PRELOAD); ;
    break;}
case 238:
#line 1554 "parser.y"
{ yyval.iptr = new_int(WRC_MO_MOVEABLE); ;
    break;}
case 239:
#line 1555 "parser.y"
{ yyval.iptr = new_int(WRC_MO_DISCARDABLE); ;
    break;}
case 240:
#line 1556 "parser.y"
{ yyval.iptr = new_int(WRC_MO_PURE); ;
    break;}
case 241:
#line 1559 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_PRELOAD); ;
    break;}
case 242:
#line 1560 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_MOVEABLE); ;
    break;}
case 243:
#line 1561 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_PURE); ;
    break;}
case 244:
#line 1565 "parser.y"
{ yyval.lvc = new_lvc(); ;
    break;}
case 245:
#line 1566 "parser.y"
{
		if(!win32)
			yywarning("LANGUAGE not supported in 16-bit mode");
		if(yyvsp[-1].lvc->language)
			yyerror("Language already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->language = yyvsp[0].lan;
		;
    break;}
case 246:
#line 1574 "parser.y"
{
		if(!win32)
			yywarning("CHARACTERISTICS not supported in 16-bit mode");
		if(yyvsp[-1].lvc->characts)
			yyerror("Characteristics already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->characts = yyvsp[0].chars;
		;
    break;}
case 247:
#line 1582 "parser.y"
{
		if(!win32)
			yywarning("VERSION not supported in 16-bit mode");
		if(yyvsp[-1].lvc->version)
			yyerror("Version already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->version = yyvsp[0].ver;
		;
    break;}
case 248:
#line 1593 "parser.y"
{ yyval.lan = new_language(yyvsp[-2].num, yyvsp[0].num); ;
    break;}
case 249:
#line 1597 "parser.y"
{ yyval.chars = new_characts(yyvsp[0].num); ;
    break;}
case 250:
#line 1601 "parser.y"
{ yyval.ver = new_version(yyvsp[0].num); ;
    break;}
case 251:
#line 1605 "parser.y"
{ yyval.raw = yyvsp[-1].raw; ;
    break;}
case 252:
#line 1609 "parser.y"
{ yyval.raw = yyvsp[0].raw; ;
    break;}
case 253:
#line 1610 "parser.y"
{ yyval.raw = int2raw_data(yyvsp[0].num); ;
    break;}
case 254:
#line 1611 "parser.y"
{ yyval.raw = long2raw_data(yyvsp[0].num); ;
    break;}
case 255:
#line 1612 "parser.y"
{ yyval.raw = str2raw_data(yyvsp[0].str); ;
    break;}
case 256:
#line 1613 "parser.y"
{ yyval.raw = merge_raw_data(yyvsp[-2].raw, yyvsp[0].raw); free(yyvsp[0].raw->data); free(yyvsp[0].raw); ;
    break;}
case 257:
#line 1614 "parser.y"
{ yyval.raw = merge_raw_data_int(yyvsp[-2].raw, yyvsp[0].num); ;
    break;}
case 258:
#line 1615 "parser.y"
{ yyval.raw = merge_raw_data_long(yyvsp[-2].raw, yyvsp[0].num); ;
    break;}
case 259:
#line 1616 "parser.y"
{ yyval.raw = merge_raw_data_str(yyvsp[-2].raw, yyvsp[0].str); ;
    break;}
case 260:
#line 1623 "parser.y"
{ yyval.iptr = 0; ;
    break;}
case 261:
#line 1624 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 262:
#line 1626 "parser.y"
{ yyval.num = (yyvsp[0].num); ;
    break;}
case 263:
#line 1629 "parser.y"
{ yyval.num = (yyvsp[-2].num) + (yyvsp[0].num); ;
    break;}
case 264:
#line 1630 "parser.y"
{ yyval.num = (yyvsp[-2].num) - (yyvsp[0].num); ;
    break;}
case 265:
#line 1631 "parser.y"
{ yyval.num = (yyvsp[-2].num) | (yyvsp[0].num); ;
    break;}
case 266:
#line 1632 "parser.y"
{ yyval.num = (yyvsp[-2].num) & (yyvsp[0].num); ;
    break;}
case 267:
#line 1633 "parser.y"
{ yyval.num = (yyvsp[-2].num) * (yyvsp[0].num); ;
    break;}
case 268:
#line 1634 "parser.y"
{ yyval.num = (yyvsp[-2].num) / (yyvsp[0].num); ;
    break;}
case 269:
#line 1635 "parser.y"
{ yyval.num = ~(yyvsp[0].num); ;
    break;}
case 270:
#line 1636 "parser.y"
{ yyval.num = -(yyvsp[0].num); ;
    break;}
case 271:
#line 1638 "parser.y"
{ yyval.num = yyvsp[-1].num; ;
    break;}
case 272:
#line 1639 "parser.y"
{ yyval.num = yyvsp[0].num; want_rscname = 0; ;
    break;}
case 273:
#line 1640 "parser.y"
{ yyval.num = ~(yyvsp[0].num); ;
    break;}
case 274:
#line 1643 "parser.y"
{ yyval.num = yyvsp[0].num; ;
    break;}
case 275:
#line 1644 "parser.y"
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
#line 1647 "parser.y"

/* Dialog specific functions */
dialog_t *dialog_style(style_t * st, dialog_t *dlg)
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

dialog_t *dialog_exstyle(style_t *st, dialog_t *dlg)
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

dialog_t *dialog_caption(string_t *s, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->title)
		yyerror("Caption already defined");
	dlg->title = s;
	return dlg;
}

dialog_t *dialog_font(font_id_t *f, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->font)
		yyerror("Font already defined");
	dlg->font = f;
	return dlg;
}

dialog_t *dialog_class(name_id_t *n, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->dlgclass)
		yyerror("Class already defined");
	dlg->dlgclass = n;
	return dlg;
}

dialog_t *dialog_menu(name_id_t *m, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->menu)
		yyerror("Menu already defined");
	dlg->menu = m;
	return dlg;
}

dialog_t *dialog_language(language_t *l, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.language)
		yyerror("Language already defined");
	dlg->lvc.language = l;
	return dlg;
}

dialog_t *dialog_characteristics(characts_t *c, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.characts)
		yyerror("Characteristics already defined");
	dlg->lvc.characts = c;
	return dlg;
}

dialog_t *dialog_version(version_t *v, dialog_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.version)
		yyerror("Version already defined");
	dlg->lvc.version = v;
	return dlg;
}

/* Controls specific functions */
control_t *ins_ctrl(int type, int special_style, control_t *ctrl, control_t *prev)
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

name_id_t *convert_ctlclass(name_id_t *cls)
{
	char *cc;
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
dialogex_t *dialogex_style(style_t * st, dialogex_t *dlg)
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

dialogex_t *dialogex_exstyle(style_t * st, dialogex_t *dlg)
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

dialogex_t *dialogex_caption(string_t *s, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->title)
		yyerror("Caption already defined");
	dlg->title = s;
	return dlg;
}

dialogex_t *dialogex_font(font_id_t *f, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->font)
		yyerror("Font already defined");
	dlg->font = f;
	return dlg;
}

dialogex_t *dialogex_class(name_id_t *n, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->dlgclass)
		yyerror("Class already defined");
	dlg->dlgclass = n;
	return dlg;
}

dialogex_t *dialogex_menu(name_id_t *m, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->menu)
		yyerror("Menu already defined");
	dlg->menu = m;
	return dlg;
}

dialogex_t *dialogex_language(language_t *l, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.language)
		yyerror("Language already defined");
	dlg->lvc.language = l;
	return dlg;
}

dialogex_t *dialogex_characteristics(characts_t *c, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.characts)
		yyerror("Characteristics already defined");
	dlg->lvc.characts = c;
	return dlg;
}

dialogex_t *dialogex_version(version_t *v, dialogex_t *dlg)
{
	assert(dlg != NULL);
	if(dlg->lvc.version)
		yyerror("Version already defined");
	dlg->lvc.version = v;
	return dlg;
}

/* Accelerator specific functions */
event_t *add_event(int key, int id, int flags, event_t *prev)
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

event_t *add_string_event(string_t *key, int id, int flags, event_t *prev)
{
	int keycode = 0;
	event_t *ev = new_event();

	if(key->type != str_char)
		yyerror("Key code must be an ascii string");

	if((flags & WRC_AF_VIRTKEY) && (!isupper(key->str.cstr[0]) && !isdigit(key->str.cstr[0])))
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
itemex_opt_t *new_itemex_opt(int id, int type, int state, int helpid)
{
	itemex_opt_t *opt = (itemex_opt_t *)xmalloc(sizeof(itemex_opt_t));
	opt->id = id;
	opt->type = type;
	opt->state = state;
	opt->helpid = helpid;
	return opt;
}

/* Raw data functions */
raw_data_t *load_file(string_t *name)
{
	FILE *fp;
	raw_data_t *rd;
	if(name->type != str_char)
		yyerror("Filename must be ASCII string");
		
	fp = open_include(name->str.cstr, 1);
	if(!fp)
		yyerror("Cannot open file %s", name->str.cstr);
	rd = new_raw_data();
	fseek(fp, 0, SEEK_END);
	rd->size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	rd->data = (char *)xmalloc(rd->size);
	fread(rd->data, rd->size, 1, fp);
	fclose(fp);
	HEAPCHECK();
	return rd;
}

raw_data_t *int2raw_data(int i)
{
	raw_data_t *rd;

	if((int)((short)i) != i)
		yywarning("Integer constant out of 16bit range (%d), truncated to %d\n", i, (short)i);

	rd = new_raw_data();
	rd->size = sizeof(short);
	rd->data = (char *)xmalloc(rd->size);
	*(short *)(rd->data) = (short)i;
	return rd;
}

raw_data_t *long2raw_data(int i)
{
	raw_data_t *rd;
	rd = new_raw_data();
	rd->size = sizeof(int);
	rd->data = (char *)xmalloc(rd->size);
	*(int *)(rd->data) = i;
	return rd;
}

raw_data_t *str2raw_data(string_t *str)
{
	raw_data_t *rd;
	rd = new_raw_data();
	rd->size = str->size * (str->type == str_char ? 1 : 2);
	rd->data = (char *)xmalloc(rd->size);
	memcpy(rd->data, str->str.cstr, rd->size);
	return rd;
}

raw_data_t *merge_raw_data(raw_data_t *r1, raw_data_t *r2)
{
	r1->data = xrealloc(r1->data, r1->size + r2->size);
	memcpy(r1->data + r1->size, r2->data, r2->size);
	r1->size += r2->size;
	return r1;
}

raw_data_t *merge_raw_data_int(raw_data_t *r1, int i)
{
	raw_data_t *t = int2raw_data(i);
	merge_raw_data(r1, t);
	free(t->data);
	free(t);
	return r1;
}

raw_data_t *merge_raw_data_long(raw_data_t *r1, int i)
{
	raw_data_t *t = long2raw_data(i);
	merge_raw_data(r1, t);
	free(t->data);
	free(t);
	return r1;
}

raw_data_t *merge_raw_data_str(raw_data_t *r1, string_t *str)
{
	raw_data_t *t = str2raw_data(str);
	merge_raw_data(r1, t);
	free(t->data);
	free(t);
	return r1;
}

/* Function the go back in a list to get the head */
menu_item_t *get_item_head(menu_item_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

menuex_item_t *get_itemex_head(menuex_item_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

resource_t *get_resource_head(resource_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

ver_block_t *get_ver_block_head(ver_block_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

ver_value_t *get_ver_value_head(ver_value_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

control_t *get_control_head(control_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

event_t *get_event_head(event_t *p)
{
	if(!p)
		return NULL;
	while(p->prev)
		p = p->prev;
	return p;
}

/* Find a stringtable with given language */
stringtable_t *find_stringtable(lvc_t *lvc)
{
	stringtable_t *stt;

	assert(lvc != NULL);

	if(!lvc->language)
		lvc->language = dup_language(currentlanguage);

	for(stt = sttres; stt; stt = stt->next)
	{
		if(stt->lvc.language->id == lvc->language->id
		&& stt->lvc.language->id == lvc->language->id)
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
int sort_stt_entry(const void *e1, const void *e2)
{
	return STE(e1)->id - STE(e2)->id;
}
#undef STE

resource_t *build_stt_resources(stringtable_t *stthead)
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

/* Cursor and icon splitter functions */
typedef struct {
	language_t	lan;
	int		id;
} id_alloc_t;

static int get_new_id(id_alloc_t **list, int *n, language_t *lan)
{
	int i;
	assert(lan != NULL);
	assert(list != NULL);
	assert(n != NULL);

	if(!*list)
	{
		*list = (id_alloc_t *)xmalloc(sizeof(id_alloc_t));
		*n = 1;
		(*list)[0].lan = *lan;
		(*list)[0].id = 1;
		return 1;
	}

	for(i = 0; i < *n; i++)
	{
		if((*list)[i].lan.id == lan->id && (*list)[i].lan.sub == lan->sub)
			return ++((*list)[i].id);
	}

	*list = (id_alloc_t *)xrealloc(*list, sizeof(id_alloc_t) * (*n+1));
	(*list)[*n].lan = *lan;
	(*list)[*n].id = 1;
	*n += 1;
	return 1;
}

int alloc_icon_id(language_t *lan)
{
	static id_alloc_t *idlist = NULL;
	static int nid = 0;

	return get_new_id(&idlist, &nid, lan);
}

int alloc_cursor_id(language_t *lan)
{
	static id_alloc_t *idlist = NULL;
	static int nid = 0;

	return get_new_id(&idlist, &nid, lan);
}

#define BPTR(base)	((char *)(rd->data + (base)))
#define WPTR(base)	((WORD *)(rd->data + (base)))
#define DPTR(base)	((DWORD *)(rd->data + (base)))
void split_icons(raw_data_t *rd, icon_group_t *icog, int *nico)
{
	int cnt;
	int i;
	icon_dir_entry_t *ide;
	icon_t *ico;
	icon_t *list = NULL;

	/* FIXME: Distinguish between normal and animated icons (RIFF format) */
	if(WPTR(0)[1] != 1)
		yyerror("Icon resource data has invalid type id %d", WPTR(0)[1]);
	cnt = WPTR(0)[2];
	ide = (icon_dir_entry_t *)&(WPTR(0)[3]);
	for(i = 0; i < cnt; i++)
	{
		ico = new_icon();
		ico->id = alloc_icon_id(icog->lvc.language);
		ico->lvc.language = dup_language(icog->lvc.language);
		if(ide[i].offset > rd->size
		|| ide[i].offset + ide[i].ressize > rd->size)
			yyerror("Icon resource data corrupt");
		ico->width = ide[i].width;
		ico->height = ide[i].height;
		ico->nclr = ide[i].nclr;
		ico->planes = ide[i].planes;
		ico->bits = ide[i].bits;
		if(!ico->planes)
		{
			/* Argh! They did not fill out the resdir structure */
			ico->planes = ((BITMAPINFOHEADER *)BPTR(ide[i].offset))->biPlanes;
		}
		if(!ico->bits)
		{
			/* Argh! They did not fill out the resdir structure */
			ico->bits = ((BITMAPINFOHEADER *)BPTR(ide[i].offset))->biBitCount;
		}
		ico->data = new_raw_data();
		copy_raw_data(ico->data, rd, ide[i].offset, ide[i].ressize);
		if(!list)
		{
			list = ico;
		}
		else
		{
			ico->next = list;
			list->prev = ico;
			list = ico;
		}
	}
	icog->iconlist = list;
	*nico = cnt;
}

void split_cursors(raw_data_t *rd, cursor_group_t *curg, int *ncur)
{
	int cnt;
	int i;
	cursor_dir_entry_t *cde;
	cursor_t *cur;
	cursor_t *list = NULL;

	/* FIXME: Distinguish between normal and animated cursors (RIFF format)*/
	if(WPTR(0)[1] != 2)
		yyerror("Cursor resource data has invalid type id %d", WPTR(0)[1]);
	cnt = WPTR(0)[2];
	cde = (cursor_dir_entry_t *)&(WPTR(0)[3]);
	for(i = 0; i < cnt; i++)
	{
		cur = new_cursor();
		cur->id = alloc_cursor_id(curg->lvc.language);
		cur->lvc.language = dup_language(curg->lvc.language);
		if(cde[i].offset > rd->size
		|| cde[i].offset + cde[i].ressize > rd->size)
			yyerror("Cursor resource data corrupt");
		cur->width = cde[i].width;
		cur->height = cde[i].height;
		cur->nclr = cde[i].nclr;
		/* The next two are to support color cursors */
		cur->planes = ((BITMAPINFOHEADER *)BPTR(cde[i].offset))->biPlanes;
		cur->bits = ((BITMAPINFOHEADER *)BPTR(cde[i].offset))->biBitCount;
		if(!win32 && (cur->planes != 1 || cur->bits != 1))
			yywarning("Win16 cursor contains colors");
		cur->xhot = cde[i].xhot;
		cur->yhot = cde[i].yhot;
		cur->data = new_raw_data();
		copy_raw_data(cur->data, rd, cde[i].offset, cde[i].ressize);
		if(!list)
		{
			list = cur;
		}
		else
		{
			cur->next = list;
			list->prev = cur;
			list = cur;
		}
	}
	curg->cursorlist = list;
	*ncur = cnt;
}

#undef	BPTR
#undef	WPTR
#undef	DPTR


toolbar_item_t *ins_tlbr_button(toolbar_item_t *prev, toolbar_item_t *idrec)
{
	idrec->prev = prev;
	if(prev)
		prev->next = idrec;

	return idrec;
}

toolbar_item_t *get_tlbr_buttons_head(toolbar_item_t *p, int *nitems)
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

