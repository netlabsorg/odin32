
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
#include "winuser.h"

#ifdef __BORLANDC__
#pragma warn -sig
#endif

DWORD andmask;		/* Used to parse 'NOT NUMBER' expressions */
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
dialogex_t *dialogex_exstyle(int st, dialogex_t *dlg);
dialogex_t *dialogex_style(int st, dialogex_t *dlg);
name_id_t *convert_ctlclass(name_id_t *cls);
control_t *ins_ctrl(int type, int style, control_t *ctrl, control_t *prev);
dialog_t *dialog_version(version_t *v, dialog_t *dlg);
dialog_t *dialog_characteristics(characts_t *c, dialog_t *dlg);
dialog_t *dialog_language(language_t *l, dialog_t *dlg);
dialog_t *dialog_menu(name_id_t *m, dialog_t *dlg);
dialog_t *dialog_class(name_id_t *n, dialog_t *dlg);
dialog_t *dialog_font(font_id_t *f, dialog_t *dlg);
dialog_t *dialog_caption(string_t *s, dialog_t *dlg);
dialog_t *dialog_exstyle(int st, dialog_t *dlg);
dialog_t *dialog_style(int st, dialog_t *dlg);
resource_t *build_stt_resources(stringtable_t *stthead);
stringtable_t *find_stringtable(lvc_t *lvc);
toolbar_item_t *ins_tlbr_button(toolbar_item_t *prev, toolbar_item_t *idrec);
toolbar_item_t *get_tlbr_buttons_head(toolbar_item_t *p, int *nitems);


#line 179 "parser.y"
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



#define	YYFINAL		597
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
   437,   455,   471,   476,   477,   480,   481,   484,   489,   491,
   493,   508,   509,   513,   517,   521,   524,   527,   531,   535,
   538,   541,   544,   545,   549,   553,   557,   561,   565,   569,
   573,   577,   581,   585,   589,   593,   597,   601,   605,   609,
   613,   624,   644,   661,   675,   687,   688,   690,   691,   694,
   704,   705,   708,   713,   717,   718,   725,   729,   735,   736,
   740,   744,   748,   752,   756,   760,   765,   769,   770,   775,
   779,   785,   786,   789,   795,   802,   803,   806,   811,   818,
   827,   832,   836,   837,   842,   843,   845,   851,   852,   862,
   872,   876,   880,   884,   888,   892,   893,   896,   902,   903,
   906,   908,   913,   918,   920,   924,   934,   935,   939,   942,
   943,   946,   949,   951,   953,   955,   957,   959,   961,   963,
   964,   967,   970,   973,   978,   981,   984,   988,   990,   992,
   994,   996,  1000,  1004,  1008,  1012,  1013,  1015,  1018,  1019,
  1023,  1027,  1031,  1035,  1039,  1043,  1046,  1049,  1053,  1055,
  1058,  1060
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
   119,   121,     0,   164,     0,   181,     0,   187,     0,    16,
     0,   119,     0,    15,     0,   131,     0,   122,     0,   123,
     0,   134,     0,   145,     0,   128,     0,   124,     0,   125,
     0,   155,     0,   159,     0,   126,     0,   127,     0,   175,
     0,   129,     0,   168,     0,    20,   177,    17,     0,    20,
   177,   184,     0,    21,   177,    17,     0,    21,   177,   184,
     0,    30,   177,    17,     0,    31,   177,    17,     0,    31,
   177,   184,     0,    26,    17,     0,    27,   177,   180,   184,
     0,    90,   177,   180,   184,     0,   130,   177,    17,     0,
   130,   177,   184,     0,    13,     0,    15,     0,    19,   177,
   180,    88,   132,    89,     0,     0,   132,    15,   111,   187,
   133,     0,   132,   187,   111,   187,   133,     0,     0,   133,
   111,    58,     0,   133,   111,    51,     0,   133,   111,    44,
     0,   133,   111,    52,     0,   133,   111,    53,     0,   133,
   111,    54,     0,    22,   177,   187,   111,   187,   111,   187,
   111,   187,   135,    88,   136,    89,     0,     0,   135,    70,
   187,     0,   135,    69,   187,     0,   135,    67,    15,     0,
   135,   141,     0,   135,    66,   120,     0,   135,    24,   119,
     0,   135,   181,     0,   135,   182,     0,   135,   183,     0,
     0,   136,    44,   140,     0,   136,    45,   138,     0,   136,
    42,   138,     0,   136,    41,   138,     0,   136,    43,   138,
     0,   136,    35,   137,     0,   136,    36,   137,     0,   136,
    40,   137,     0,   136,    37,   137,     0,   136,    38,   137,
     0,   136,    32,   137,     0,   136,    39,   137,     0,   136,
    33,   137,     0,   136,    34,   137,     0,   136,    48,   137,
     0,   136,    47,   137,     0,   136,    46,   137,     0,   136,
    31,   120,   111,   187,   111,   187,   111,   187,   139,     0,
    15,   111,   187,   111,   187,   111,   187,   111,   187,   111,
   187,   142,     0,   187,   111,   187,   111,   187,   111,   187,
   111,   187,   142,     0,     0,   111,   187,   111,   187,     0,
   111,   187,   111,   187,   111,   187,     0,   111,   187,   111,
   187,   111,   187,   111,   187,     0,   120,   111,   187,   111,
   144,   111,   187,   111,   187,   111,   187,   111,   187,   111,
   187,   111,   187,     0,   120,   111,   187,   111,   144,   111,
   187,   111,   187,   111,   187,   111,   187,   111,   187,     0,
    30,   187,   111,    15,     0,     0,   111,   187,     0,     0,
   111,   187,     0,   111,   187,   111,   187,     0,   187,     0,
    15,     0,    23,   177,   187,   111,   187,   111,   187,   111,
   187,   152,   146,    88,   147,    89,     0,     0,   146,    70,
   187,     0,   146,    69,   187,     0,   146,    67,    15,     0,
   146,   141,     0,   146,   153,     0,   146,    66,   120,     0,
   146,    24,   119,     0,   146,   181,     0,   146,   182,     0,
   146,   183,     0,     0,   147,    44,   148,     0,   147,    45,
   150,     0,   147,    42,   150,     0,   147,    41,   150,     0,
   147,    43,   150,     0,   147,    35,   149,     0,   147,    36,
   149,     0,   147,    40,   149,     0,   147,    37,   149,     0,
   147,    38,   149,     0,   147,    32,   149,     0,   147,    39,
   149,     0,   147,    33,   149,     0,   147,    34,   149,     0,
   147,    48,   149,     0,   147,    47,   149,     0,   147,    46,
   149,     0,   147,    31,   120,   111,   187,   111,   187,   111,
   187,   139,     0,   120,   111,   187,   111,   144,   111,   187,
   111,   187,   111,   187,   111,   187,   111,   187,   111,   186,
   152,   151,     0,   120,   111,   187,   111,   144,   111,   187,
   111,   187,   111,   187,   111,   187,   111,   187,   151,     0,
    15,   111,   187,   111,   187,   111,   187,   111,   187,   111,
   187,   143,   151,     0,   187,   111,   187,   111,   187,   111,
   187,   111,   187,   143,   151,     0,     0,   184,     0,     0,
   111,   187,     0,    30,   187,   111,    15,   111,   187,   111,
   187,   154,     0,     0,   111,   187,     0,    24,   177,   180,
   156,     0,    88,   157,    89,     0,     0,   157,    82,    15,
   111,   187,   158,     0,   157,    82,    84,     0,   157,    83,
    15,   158,   156,     0,     0,   111,    56,   158,     0,   111,
    55,   158,     0,   111,    85,   158,     0,   111,    57,   158,
     0,   111,    80,   158,     0,   111,    81,   158,     0,    25,
   177,   180,   160,     0,    88,   161,    89,     0,     0,   161,
    82,    15,   162,     0,   161,    82,    84,     0,   161,    83,
    15,   163,   160,     0,     0,   111,   187,     0,   111,   186,
   111,   186,   158,     0,   111,   186,   111,   186,   111,   187,
     0,     0,   111,   187,     0,   111,   186,   111,   187,     0,
   111,   186,   111,   186,   111,   187,     0,   111,   186,   111,
   186,   111,   186,   111,   187,     0,   165,    88,   166,    89,
     0,    29,   177,   180,     0,     0,   166,   187,   167,    15,
     0,     0,   111,     0,    28,   169,    88,   170,    89,     0,
     0,   169,    73,   187,   111,   187,   111,   187,   111,   187,
     0,   169,    74,   187,   111,   187,   111,   187,   111,   187,
     0,   169,    78,   187,     0,   169,    75,   187,     0,   169,
    76,   187,     0,   169,    77,   187,     0,   169,    79,   187,
     0,     0,   170,   171,     0,    49,    15,    88,   172,    89,
     0,     0,   172,   173,     0,   171,     0,    50,    15,   111,
    15,     0,    50,    15,   111,   174,     0,   187,     0,   174,
   111,   187,     0,    86,   177,   187,   111,   187,   180,    88,
   176,    89,     0,     0,   176,    87,   187,     0,   176,    84,
     0,     0,   177,   178,     0,   177,   179,     0,    63,     0,
    65,     0,    61,     0,    59,     0,    62,     0,    64,     0,
    60,     0,     0,   180,   181,     0,   180,   182,     0,   180,
   183,     0,    72,   187,   111,   187,     0,    68,   187,     0,
    71,   187,     0,    88,   185,    89,     0,    18,     0,    13,
     0,    14,     0,    15,     0,   185,   167,    18,     0,   185,
   167,    13,     0,   185,   167,    14,     0,   185,   167,    15,
     0,     0,   187,     0,   188,   189,     0,     0,   189,   102,
   189,     0,   189,   103,   189,     0,   189,    93,   189,     0,
   189,    95,   189,     0,   189,   104,   189,     0,   189,   105,
   189,     0,   106,   189,     0,   103,   189,     0,   109,   189,
   110,     0,   190,     0,   108,   190,     0,    13,     0,    14,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   303,   323,   324,   349,   350,   355,   356,   357,   358,   359,
   360,   363,   364,   365,   366,   367,   368,   369,   370,   371,
   372,   373,   374,   375,   376,   377,   378,   379,   383,   384,
   385,   386,   390,   391,   392,   393,   394,   399,   414,   422,
   435,   441,   452,   453,   463,   464,   465,   479,   480,   486,
   487,   488,   502,   503,   509,   510,   511,   512,   513,   517,
   518,   522,   523,   528,   532,   533,   541,   549,   562,   575,
   576,   595,   607,   617,   641,   642,   643,   646,   647,   648,
   649,   650,   651,   652,   657,   688,   689,   690,   691,   692,
   693,   694,   695,   696,   697,   700,   701,   702,   703,   704,
   705,   706,   707,   708,   709,   711,   712,   713,   714,   715,
   716,   717,   718,   720,   730,   750,   766,   769,   774,   781,
   792,   806,   821,   825,   826,   830,   831,   832,   836,   841,
   849,   888,   889,   890,   891,   892,   893,   894,   895,   896,
   897,   898,   901,   902,   903,   904,   905,   906,   907,   908,
   909,   910,   912,   913,   914,   915,   916,   917,   918,   919,
   921,   931,   957,   973,  1001,  1024,  1025,  1028,  1029,  1033,
  1040,  1041,  1045,  1068,  1072,  1073,  1082,  1088,  1107,  1108,
  1109,  1110,  1111,  1112,  1113,  1117,  1142,  1146,  1147,  1163,
  1169,  1189,  1190,  1194,  1202,  1213,  1214,  1218,  1224,  1232,
  1252,  1293,  1304,  1305,  1335,  1336,  1341,  1348,  1349,  1359,
  1369,  1376,  1383,  1390,  1397,  1407,  1408,  1417,  1425,  1426,
  1435,  1440,  1446,  1455,  1456,  1460,  1486,  1487,  1492,  1501,
  1502,  1512,  1527,  1528,  1529,  1530,  1533,  1534,  1535,  1539,
  1540,  1548,  1556,  1567,  1571,  1575,  1579,  1583,  1584,  1585,
  1586,  1587,  1588,  1589,  1590,  1597,  1598,  1600,  1603,  1606,
  1607,  1608,  1609,  1610,  1611,  1612,  1613,  1615,  1616,  1617,
  1620,  1621
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
"ctlclass","dialogex","dlgex_attribs","exctrls","gen_exctrl","lab_exctrl","exctrl_desc",
"opt_data","helpid","opt_exfont","opt_expr","menu","menu_body","item_definitions",
"item_options","menuex","menuex_body","itemex_definitions","itemex_options",
"itemex_p_options","stringtable","stt_head","strings","opt_comma","versioninfo",
"fix_version","ver_blocks","ver_block","ver_values","ver_value","ver_words",
"toolbar","toolbar_items","loadmemopts","lamo","lama","opt_lvc","opt_language",
"opt_characts","opt_version","raw_data","raw_elements","e_expr","expr","dummy",
"xpr","any_num", NULL
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
   145,   146,   146,   146,   146,   146,   146,   146,   146,   146,
   146,   146,   147,   147,   147,   147,   147,   147,   147,   147,
   147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
   147,   148,   148,   149,   150,   151,   151,   152,   152,   153,
   154,   154,   155,   156,   157,   157,   157,   157,   158,   158,
   158,   158,   158,   158,   158,   159,   160,   161,   161,   161,
   161,   162,   162,   162,   162,   163,   163,   163,   163,   163,
   164,   165,   166,   166,   167,   167,   168,   169,   169,   169,
   169,   169,   169,   169,   169,   170,   170,   171,   172,   172,
   173,   173,   173,   174,   174,   175,   176,   176,   176,   177,
   177,   177,   178,   178,   178,   178,   179,   179,   179,   180,
   180,   180,   180,   181,   182,   183,   184,   185,   185,   185,
   185,   185,   185,   185,   185,   186,   186,   187,   188,   189,
   189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
   190,   190
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
    17,    15,     4,     0,     2,     0,     2,     4,     1,     1,
    14,     0,     3,     3,     3,     2,     2,     3,     3,     2,
     2,     2,     0,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
    10,    19,    16,    13,    11,     0,     1,     0,     2,     9,
     0,     2,     4,     3,     0,     6,     3,     5,     0,     3,
     3,     3,     3,     3,     3,     4,     3,     0,     4,     3,
     5,     0,     2,     5,     6,     0,     2,     4,     6,     8,
     4,     3,     0,     4,     0,     1,     5,     0,     9,     9,
     3,     3,     3,     3,     3,     0,     2,     5,     0,     2,
     1,     4,     4,     1,     3,     9,     0,     3,     2,     0,
     2,     2,     1,     1,     1,     1,     1,     1,     1,     0,
     2,     2,     2,     4,     2,     2,     3,     1,     1,     1,
     1,     3,     3,     3,     3,     0,     1,     2,     0,     3,
     3,     3,     3,     3,     3,     2,     2,     3,     1,     2,
     1,     1
};

static const short yydefact[] = {     2,
   259,     0,     0,     0,     0,     0,     0,    33,    34,    42,
   230,   259,     4,     5,     3,     0,    39,     0,    40,    41,
     0,     0,   271,   272,    30,     0,     0,     0,     0,     0,
     0,    12,    29,     0,     0,    10,     0,    11,    35,    37,
    36,   240,     0,    72,    73,   230,   230,   230,   230,   230,
   230,   230,     0,   230,   208,   230,   230,   230,   230,    38,
    46,    47,    51,    52,    55,    56,    50,    58,   230,    45,
    48,    49,    53,    54,    59,    57,   203,     0,     0,     0,
     0,   258,   269,    31,     0,    25,    26,    24,    27,     0,
     6,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     7,     8,     9,   236,   239,   235,   237,   233,
   238,   234,   231,   232,   202,   259,   240,     0,     0,   259,
   259,   240,   240,    67,   240,     0,     0,     0,   259,   240,
     0,   259,   267,   266,   270,     0,     0,     0,     0,     0,
     0,     0,     0,    28,    13,    14,    17,    18,    19,    20,
    22,    21,    23,    15,    16,   259,   259,   241,   242,   243,
   244,     0,    60,     0,    61,    62,    63,     0,     0,     0,
     0,     0,   259,   259,   259,   259,   259,   259,   259,   216,
    64,    65,    66,     0,     0,    70,    71,   201,   205,   268,
   262,   263,   260,   261,   264,   265,    32,   245,   246,    75,
   249,   250,   251,   248,   205,   259,   259,   175,   173,   188,
   186,    68,     0,     0,   212,   213,   214,   211,   215,     0,
   259,    69,   206,     0,   259,   247,     0,     0,     0,     0,
     0,   259,   259,     0,   207,   217,   240,   204,     0,    74,
     0,   253,   254,   255,   252,   259,   259,     0,     0,   174,
     0,     0,   187,     0,     0,     0,     0,   259,   259,     0,
     0,     0,   177,   179,   192,   190,   196,   259,   259,   219,
   227,    78,    78,   259,   259,   259,     0,     0,   259,   189,
   259,     0,     0,     0,     0,     0,    76,    77,    86,   168,
   179,   179,   179,   179,   179,   179,   179,   178,     0,   193,
     0,   197,   191,   259,   259,     0,   218,   221,   220,   229,
   259,   226,     0,     0,   259,   132,   176,   181,   180,   183,
   184,   185,   182,   259,   259,   209,   210,     0,   228,    81,
    80,    82,    83,    84,    79,   259,   259,   259,     0,   259,
   259,    96,    90,    93,    94,    95,   169,     0,   179,   257,
     0,   198,   259,    42,    92,     0,    44,    43,    91,    89,
    88,    87,     0,   259,   259,   259,     0,   259,   259,   143,
   136,   137,   140,   141,   142,   259,   194,   259,   222,   223,
   224,     0,   259,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   259,   259,   259,   259,   259,     0,     0,     0,
    85,   139,     0,   138,   135,   134,   133,     0,   195,     0,
   199,   259,   123,     0,     0,   107,   109,   110,   102,   103,
   105,   106,   108,   104,   100,     0,    99,   101,     0,    97,
    98,   113,   112,   111,     0,   259,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   259,   259,   259,   259,   259,
     0,     0,     0,   131,   259,   225,   259,   259,   259,   259,
   123,     0,     0,   154,   156,   157,   149,   150,   152,   153,
   155,   151,   147,     0,   146,   148,     0,   144,   145,   160,
   159,   158,   200,     0,     0,     0,     0,   259,   259,   259,
   259,   259,   259,   259,   259,   259,     0,     0,     0,     0,
     0,     0,     0,     0,   130,     0,   129,   259,   259,   259,
   259,   259,   259,   259,   259,   259,   171,     0,     0,     0,
     0,   117,     0,     0,     0,   259,   170,   259,   259,   259,
   259,   259,   114,   259,   259,   259,   172,   117,     0,     0,
     0,     0,     0,   124,     0,   161,   259,   259,   259,   259,
   259,   259,   116,   259,     0,   126,     0,   118,   124,   125,
     0,   259,   259,   166,   259,   259,   115,   259,   126,   127,
   165,   167,     0,   119,     0,   166,   259,   259,   259,   259,
   164,   128,     0,   120,   122,   259,   259,   166,   121,   256,
   163,   168,   166,   162,     0,     0,     0
};

static const short yydefgoto[] = {   595,
     1,    13,    31,    32,    14,    15,   358,   359,    60,    61,
    62,    63,    64,    65,    66,    67,    68,    69,    70,   225,
   287,    71,   314,   363,   416,   425,   533,   430,   343,   553,
   564,   506,    72,   348,   408,   478,   464,   473,   571,   316,
   372,   527,    73,   209,   230,   278,    74,   211,   231,   280,
   282,    17,    18,   132,   224,    75,   126,   220,   236,   285,
   309,   380,    76,   286,    42,   113,   114,   115,   158,   159,
   160,   572,   205,   299,    20,    21,    82,    33
};

static const short yypact[] = {-32768,
   199,     9,    20,    28,     6,     9,    51,-32768,-32768,    -7,
-32768,-32768,-32768,-32768,-32768,   253,-32768,    18,-32768,-32768,
   -10,    -8,-32768,-32768,-32768,     9,     9,     9,     9,     9,
    77,-32768,-32768,    61,    70,-32768,    90,-32768,-32768,-32768,
-32768,   484,     8,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   126,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -10,   -10,    13,
   -10,   570,-32768,-32768,   137,-32768,-32768,-32768,-32768,   525,
-32768,     9,     9,     9,     9,     9,     9,     9,     9,     9,
     9,     9,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   308,-32768,   484,   271,   309,   484,
   484,   484,   484,-32768,   484,   453,   179,   327,   484,   484,
   343,    81,   -70,-32768,-32768,   467,   -10,   -10,   -10,   -10,
   -10,   -10,    62,-32768,   584,   391,   371,   311,   311,   116,
   116,   116,   116,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    42,-32768,   237,-32768,-32768,-32768,    72,    74,   129,
   214,   219,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    87,   219,-32768,-32768,-32768,    97,-32768,
   206,   -14,   -70,   -70,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    -4,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   103,   105,-32768,-32768,-32768,-32768,-32768,   -30,
-32768,-32768,-32768,   222,    -5,-32768,   336,   134,   143,   144,
   215,-32768,-32768,   245,-32768,-32768,-32768,-32768,   154,-32768,
   159,-32768,-32768,-32768,-32768,-32768,-32768,    -1,   281,-32768,
     2,   285,-32768,   194,   195,   201,   274,-32768,-32768,   205,
   212,   213,-32768,   216,   227,-32768,   229,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   168,   259,   241,-32768,
   241,   267,   247,   250,   -17,   -44,   252,   252,-32768,   256,
   216,   216,   216,   216,   216,   216,   216,-32768,   264,   266,
   282,   266,-32768,-32768,-32768,   379,-32768,-32768,-32768,-32768,
-32768,-32768,   178,     7,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   -29,   241,-32768,-32768,   284,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   380,-32768,   279,   383,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   354,   288,-32768,
   290,   266,   401,-32768,-32768,   306,-32768,-32768,-32768,-32768,
-32768,-32768,   418,   380,-32768,   279,   403,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   168,-32768,   241,-32768,   316,
-32768,   404,   279,   413,   413,   413,   413,   413,   413,   413,
   413,   413,-32768,-32768,-32768,   279,-32768,   413,   413,   413,
-32768,-32768,   318,-32768,-32768,-32768,-32768,   548,-32768,   319,
   266,-32768,-32768,   321,   322,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   326,-32768,-32768,   328,-32768,
-32768,-32768,-32768,-32768,   425,   279,   426,   426,   426,   426,
   426,   426,   426,   426,   426,-32768,-32768,-32768,   279,-32768,
   426,   426,   426,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   333,   364,   375,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   384,-32768,-32768,   394,-32768,-32768,-32768,
-32768,-32768,-32768,   395,   397,   398,   399,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   496,   402,   410,   414,   422,
   423,   441,   444,   446,-32768,   447,-32768,-32768,-32768,-32768,
-32768,   496,-32768,-32768,-32768,-32768,   448,   450,   452,   462,
   465,   493,   495,   500,   501,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   493,   502,   503,
   504,   507,   518,   519,   520,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   521,   522,   523,   527,   519,-32768,
   536,-32768,-32768,   424,-32768,-32768,-32768,-32768,   522,   537,
-32768,-32768,   538,   539,   540,   424,-32768,-32768,-32768,-32768,
-32768,-32768,   553,-32768,   555,-32768,-32768,   -59,-32768,    43,
-32768,   256,   424,-32768,   515,   564,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    40,-32768,-32768,-32768,     1,  -355,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   363,-32768,-32768,-32768,   254,  -266,   133,-32768,   329,   108,
   110,   176,-32768,-32768,-32768,-32768,   217,  -300,  -546,    98,
-32768,-32768,-32768,   411,-32768,  -137,-32768,   409,-32768,-32768,
-32768,-32768,-32768,-32768,   487,-32768,-32768,-32768,   408,-32768,
-32768,-32768,-32768,-32768,   551,-32768,-32768,  -110,     0,  -293,
  -290,   -80,-32768,  -275,   -12,-32768,   180,   -16
};


#define	YYLAST		693


static const short yytable[] = {    43,
    19,    16,    23,    24,    83,   301,   162,    84,    39,   239,
   404,   170,   171,   262,   172,    36,   265,    22,   234,   185,
   345,    23,    24,   346,    25,    23,    24,   414,   164,   581,
   336,   234,   306,   141,   142,    34,   337,   165,   167,   310,
   429,   591,   311,    35,   312,    37,   594,   183,   349,   351,
   187,   590,  -256,  -256,   374,  -259,  -259,   375,   235,  -256,
    38,    83,    83,   135,    83,    86,    87,    88,    89,    90,
   103,   307,   338,   339,   156,   340,   341,   157,    12,   104,
   462,  -256,   263,   240,   226,   266,    91,   139,   140,   141,
   142,   212,    78,   477,   342,    79,    40,    80,    81,   105,
    85,    41,   410,   161,   222,    77,   223,   168,   169,   156,
    26,    27,   157,    12,    28,    29,   184,    30,   116,   189,
    83,    83,    83,    83,    83,    83,   257,   427,   428,   200,
   431,   145,   146,   147,   148,   149,   150,   151,   152,   153,
   154,   155,   124,   198,   199,  -259,   475,   476,  -259,   479,
  -259,  -259,   143,   317,   318,   319,   320,   321,   322,   323,
   213,   214,   215,   216,   217,   218,   219,    92,    93,   188,
    94,   197,    95,    96,    97,    98,    99,   100,   101,   102,
    92,    93,   206,    94,   207,    95,    96,    97,    98,    99,
   100,   101,   102,   228,   229,   181,   156,   221,    -1,   157,
    12,     2,     3,     4,     5,     6,     7,   223,   237,     8,
     9,   377,   241,   232,    10,   233,   208,   101,   102,   254,
   255,   330,   292,   293,   294,   248,   249,    11,   331,   332,
   333,   334,   250,   260,   261,   335,   238,   106,   107,   108,
   109,   110,   111,   112,   246,   272,   273,   295,   296,   201,
   202,   203,   297,   247,   204,   283,   284,   133,   134,   256,
   136,   289,   290,   291,   258,    44,   300,    45,   302,   259,
    12,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,   156,    56,    57,   157,    12,   156,   163,   270,   157,
    12,   326,   327,   357,   354,   264,   251,   252,   329,   267,
   138,   210,   347,   253,   268,   269,   164,   139,   140,   141,
   142,   350,   352,   344,   592,   274,   191,   192,   193,   194,
   195,   196,   275,   276,   356,   166,   277,   361,   362,   106,
   107,   108,   109,   110,   111,   112,   355,   279,    58,   281,
   381,   156,    59,   182,   157,    12,   208,   373,   242,   243,
   244,  -256,   403,   245,   210,   406,   407,   304,   164,   186,
   305,   271,   313,   409,   402,   411,   315,   106,   107,   108,
   109,   110,   111,   112,   324,   156,  -257,   364,   157,    12,
   426,   426,   426,   365,   426,   106,   107,   108,   109,   110,
   111,   112,   325,   328,   353,   354,   164,   360,   376,   456,
   378,   106,   107,   108,   109,   110,   111,   112,    97,    98,
    99,   100,   101,   102,   164,   379,   382,   405,   413,   366,
   367,   156,   368,   369,   157,    12,   412,   415,   435,   455,
   164,   457,   458,   474,   474,   474,   459,   474,   460,   461,
   463,   370,   483,   488,   484,   485,   486,   487,   383,   384,
   385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
   395,   396,   397,   398,   399,   400,    95,    96,    97,    98,
    99,   100,   101,   102,   489,   497,   498,   499,   500,   501,
   502,   503,   504,   507,    94,   490,    95,    96,    97,    98,
    99,   100,   101,   102,   491,   517,   518,   519,   520,   507,
   522,   523,   524,   525,   492,   493,   401,   494,   495,   496,
   505,   164,   508,   537,   596,   538,   539,   540,   541,   542,
   509,   543,   544,   545,   510,   173,   174,   175,   176,   177,
   178,   179,   511,   512,   555,   556,   557,   558,   559,   560,
   180,   561,   106,   107,   108,   109,   110,   111,   112,   569,
   570,   513,   573,   574,   514,   575,   515,   516,   526,   137,
   528,   138,   529,   597,   582,   583,   584,   585,   139,   140,
   141,   142,   530,   588,   589,   531,   190,   350,   436,   437,
   438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
   448,   449,   450,   451,   452,   453,   117,   118,   119,   120,
   121,   122,   123,   532,   125,   534,   127,   128,   129,   130,
   535,   536,   547,   548,   549,    92,    93,   550,    94,   131,
    95,    96,    97,    98,    99,   100,   101,   102,   551,   552,
   554,   562,   563,   565,   144,   288,   454,   566,   417,   418,
   419,   420,   421,   422,   423,   424,   568,   577,   578,   579,
   580,   432,   433,   434,   465,   466,   467,   468,   469,   470,
   471,   472,   137,   586,   138,   587,   567,   480,   481,   482,
   546,   139,   140,   141,   142,    93,   371,    94,   576,    95,
    96,    97,    98,    99,   100,   101,   102,   521,   298,   593,
   303,   227,   308
};

static const short yycheck[] = {    12,
     1,     1,    13,    14,    21,   281,   117,    16,    16,    15,
   366,   122,   123,    15,   125,    10,    15,     9,    49,   130,
   314,    13,    14,   314,    16,    13,    14,   383,    88,   576,
    24,    49,    50,   104,   105,    16,    30,   118,   119,    84,
   396,   588,    87,    16,    89,     6,   593,   128,   324,   325,
   131,   111,    82,    83,   348,    13,    14,   348,    89,    89,
    10,    78,    79,    80,    81,    26,    27,    28,    29,    30,
    10,    89,    66,    67,    68,    69,    70,    71,    72,    10,
   436,   111,    84,    89,    89,    84,    10,   102,   103,   104,
   105,   172,   103,   449,    88,   106,   104,   108,   109,    10,
   109,   109,   378,   116,   185,    88,   111,   120,   121,    68,
   102,   103,    71,    72,   106,   107,   129,   109,   111,   132,
   137,   138,   139,   140,   141,   142,   237,   394,   395,    88,
   397,    92,    93,    94,    95,    96,    97,    98,    99,   100,
   101,   102,    17,   156,   157,   103,   447,   448,   106,   450,
   108,   109,    16,   291,   292,   293,   294,   295,   296,   297,
   173,   174,   175,   176,   177,   178,   179,    91,    92,    89,
    94,   110,    96,    97,    98,    99,   100,   101,   102,   103,
    91,    92,   111,    94,   111,    96,    97,    98,    99,   100,
   101,   102,   103,   206,   207,    17,    68,   111,     0,    71,
    72,     3,     4,     5,     6,     7,     8,   111,   221,    11,
    12,   349,   225,   111,    16,   111,    88,   102,   103,   232,
   233,    44,    55,    56,    57,    82,    83,    29,    51,    52,
    53,    54,    89,   246,   247,    58,    15,    59,    60,    61,
    62,    63,    64,    65,   111,   258,   259,    80,    81,    13,
    14,    15,    85,   111,    18,   268,   269,    78,    79,    15,
    81,   274,   275,   276,   111,    13,   279,    15,   281,   111,
    72,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    68,    30,    31,    71,    72,    68,    17,    88,    71,
    72,   304,   305,    15,    16,    15,    82,    83,   311,    15,
    95,    88,   315,    89,   111,   111,    88,   102,   103,   104,
   105,   324,   325,   314,   590,   111,   137,   138,   139,   140,
   141,   142,   111,   111,   337,    17,   111,   340,   341,    59,
    60,    61,    62,    63,    64,    65,   336,   111,    86,   111,
   353,    68,    90,    17,    71,    72,    88,   348,    13,    14,
    15,   111,   365,    18,    88,   368,   369,   111,    88,    17,
   111,    88,   111,   376,   364,   378,   111,    59,    60,    61,
    62,    63,    64,    65,   111,    68,   111,    24,    71,    72,
   393,   394,   395,    30,   397,    59,    60,    61,    62,    63,
    64,    65,   111,    15,   111,    16,    88,    15,   111,   412,
   111,    59,    60,    61,    62,    63,    64,    65,    98,    99,
   100,   101,   102,   103,    88,    15,   111,    15,    15,    66,
    67,    68,    69,    70,    71,    72,   111,    15,   111,   111,
    88,   111,   111,   446,   447,   448,   111,   450,   111,    15,
    15,    88,   455,   111,   457,   458,   459,   460,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    96,    97,    98,    99,
   100,   101,   102,   103,   111,   488,   489,   490,   491,   492,
   493,   494,   495,   496,    94,   111,    96,    97,    98,    99,
   100,   101,   102,   103,   111,   508,   509,   510,   511,   512,
   513,   514,   515,   516,   111,   111,    89,   111,   111,   111,
    15,    88,   111,   526,     0,   528,   529,   530,   531,   532,
   111,   534,   535,   536,   111,    73,    74,    75,    76,    77,
    78,    79,   111,   111,   547,   548,   549,   550,   551,   552,
    88,   554,    59,    60,    61,    62,    63,    64,    65,   562,
   563,   111,   565,   566,   111,   568,   111,   111,   111,    93,
   111,    95,   111,     0,   577,   578,   579,   580,   102,   103,
   104,   105,   111,   586,   587,   111,   110,   590,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    46,    47,    48,    49,
    50,    51,    52,   111,    54,   111,    56,    57,    58,    59,
   111,   111,   111,   111,   111,    91,    92,   111,    94,    69,
    96,    97,    98,    99,   100,   101,   102,   103,   111,   111,
   111,   111,   111,   111,   110,   273,    89,   111,   385,   386,
   387,   388,   389,   390,   391,   392,   111,   111,   111,   111,
   111,   398,   399,   400,   438,   439,   440,   441,   442,   443,
   444,   445,    93,   111,    95,   111,   559,   451,   452,   453,
   538,   102,   103,   104,   105,    92,   348,    94,   569,    96,
    97,    98,    99,   100,   101,   102,   103,   512,   278,   592,
   282,   205,   285
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
#line 303 "parser.y"
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
#line 323 "parser.y"
{ yyval.res = NULL; want_rscname = 1; ;
    break;}
case 3:
#line 324 "parser.y"
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
#line 349 "parser.y"
{ yyval.res = yyvsp[-1].res; want_rscname = 1; ;
    break;}
case 5:
#line 350 "parser.y"
{ yyval.res = yyvsp[-1].res; want_rscname = 1; ;
    break;}
case 6:
#line 355 "parser.y"
{ pop_start(); push_if(yyvsp[-1].iptr ? *(yyvsp[-1].iptr) : 0, 0, 0); if(yyvsp[-1].iptr) free(yyvsp[-1].iptr);;
    break;}
case 7:
#line 356 "parser.y"
{ pop_start(); push_if(pp_lookup(yyvsp[-1].str->str.cstr) != NULL, 0, 0); ;
    break;}
case 8:
#line 357 "parser.y"
{ pop_start(); push_if(pp_lookup(yyvsp[-1].str->str.cstr) == NULL, 0, 0); ;
    break;}
case 9:
#line 358 "parser.y"
{ pop_start(); push_if(yyvsp[-1].iptr ? *(yyvsp[-1].iptr) : 0, pop_if(), 0); if(yyvsp[-1].iptr) free(yyvsp[-1].iptr); ;
    break;}
case 10:
#line 359 "parser.y"
{ pop_start(); push_if(1, pop_if(), 0); ;
    break;}
case 11:
#line 360 "parser.y"
{ pop_if(); ;
    break;}
case 12:
#line 363 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; ;
    break;}
case 13:
#line 364 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr || *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 14:
#line 365 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr && *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 15:
#line 366 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  + *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 16:
#line 367 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  - *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 17:
#line 368 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  ^ *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 18:
#line 369 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr == *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 19:
#line 370 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr != *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 20:
#line 371 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  < *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 21:
#line 372 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr  > *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 22:
#line 373 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr <= *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 23:
#line 374 "parser.y"
{ yyval.iptr = new_int(yyvsp[-2].iptr && yyvsp[0].iptr ? (*yyvsp[-2].iptr >= *yyvsp[0].iptr) : 0); if(yyvsp[-2].iptr) free(yyvsp[-2].iptr); if(yyvsp[0].iptr) free(yyvsp[0].iptr); ;
    break;}
case 24:
#line 375 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; if(yyvsp[0].iptr) *yyvsp[0].iptr = ~(*yyvsp[0].iptr); ;
    break;}
case 25:
#line 376 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; ;
    break;}
case 26:
#line 377 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; if(yyvsp[0].iptr) *yyvsp[0].iptr = -(*yyvsp[0].iptr); ;
    break;}
case 27:
#line 378 "parser.y"
{ yyval.iptr = yyvsp[0].iptr; if(yyvsp[0].iptr) *yyvsp[0].iptr = !(*yyvsp[0].iptr); ;
    break;}
case 28:
#line 379 "parser.y"
{ yyval.iptr = yyvsp[-1].iptr; ;
    break;}
case 29:
#line 383 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 30:
#line 384 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 31:
#line 385 "parser.y"
{ yyval.iptr = new_int(pp_lookup(yyvsp[0].str->str.cstr) != NULL); ;
    break;}
case 32:
#line 386 "parser.y"
{ yyval.iptr = new_int(pp_lookup(yyvsp[-1].str->str.cstr) != NULL); ;
    break;}
case 33:
#line 390 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 34:
#line 391 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 35:
#line 392 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 36:
#line 393 "parser.y"
{ strip_til_parenthesis(); ;
    break;}
case 37:
#line 394 "parser.y"
{ strip_til_semicolon(); ;
    break;}
case 38:
#line 399 "parser.y"
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
#line 414 "parser.y"
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
#line 422 "parser.y"
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
#line 435 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		want_rscname = 0;
		;
    break;}
case 42:
#line 441 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		want_rscname = 0;
		;
    break;}
case 43:
#line 452 "parser.y"
{ yyval.nid = yyvsp[0].nid; ;
    break;}
case 44:
#line 453 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		want_rscname = 0;
		;
    break;}
case 45:
#line 463 "parser.y"
{ yyval.res = new_resource(res_acc, yyvsp[0].acc, yyvsp[0].acc->memopt, yyvsp[0].acc->lvc.language); ;
    break;}
case 46:
#line 464 "parser.y"
{ yyval.res = new_resource(res_bmp, yyvsp[0].bmp, yyvsp[0].bmp->memopt, dup_language(currentlanguage)); ;
    break;}
case 47:
#line 465 "parser.y"
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
#line 479 "parser.y"
{ yyval.res = new_resource(res_dlg, yyvsp[0].dlg, yyvsp[0].dlg->memopt, yyvsp[0].dlg->lvc.language); ;
    break;}
case 49:
#line 480 "parser.y"
{
		if(win32)
			yyval.res = new_resource(res_dlgex, yyvsp[0].dlgex, yyvsp[0].dlgex->memopt, yyvsp[0].dlgex->lvc.language);
		else
			yyval.res = NULL;
		;
    break;}
case 50:
#line 486 "parser.y"
{ yyval.res = new_resource(res_dlginit, yyvsp[0].dginit, yyvsp[0].dginit->memopt, yyvsp[0].dginit->lvc.language); ;
    break;}
case 51:
#line 487 "parser.y"
{ yyval.res = new_resource(res_fnt, yyvsp[0].fnt, yyvsp[0].fnt->memopt, dup_language(currentlanguage)); ;
    break;}
case 52:
#line 488 "parser.y"
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
#line 502 "parser.y"
{ yyval.res = new_resource(res_men, yyvsp[0].men, yyvsp[0].men->memopt, yyvsp[0].men->lvc.language); ;
    break;}
case 54:
#line 503 "parser.y"
{
		if(win32)
			yyval.res = new_resource(res_menex, yyvsp[0].menex, yyvsp[0].menex->memopt, yyvsp[0].menex->lvc.language);
		else
			yyval.res = NULL;
		;
    break;}
case 55:
#line 509 "parser.y"
{ yyval.res = new_resource(res_msg, yyvsp[0].msg, WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE, dup_language(currentlanguage)); ;
    break;}
case 56:
#line 510 "parser.y"
{ yyval.res = new_resource(res_rdt, yyvsp[0].rdt, yyvsp[0].rdt->memopt, yyvsp[0].rdt->lvc.language); ;
    break;}
case 57:
#line 511 "parser.y"
{ yyval.res = new_resource(res_toolbar, yyvsp[0].tlbar, yyvsp[0].tlbar->memopt, yyvsp[0].tlbar->lvc.language); ;
    break;}
case 58:
#line 512 "parser.y"
{ yyval.res = new_resource(res_usr, yyvsp[0].usr, yyvsp[0].usr->memopt, dup_language(currentlanguage)); ;
    break;}
case 59:
#line 513 "parser.y"
{ yyval.res = new_resource(res_ver, yyvsp[0].veri, WRC_MO_MOVEABLE | WRC_MO_DISCARDABLE, dup_language(currentlanguage)); ;
    break;}
case 60:
#line 517 "parser.y"
{ yyval.bmp = new_bitmap(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 61:
#line 518 "parser.y"
{ yyval.bmp = new_bitmap(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 62:
#line 522 "parser.y"
{ yyval.curg = new_cursor_group(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 63:
#line 523 "parser.y"
{ yyval.curg = new_cursor_group(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 64:
#line 528 "parser.y"
{ yyval.fnt = new_font(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 65:
#line 532 "parser.y"
{ yyval.icog = new_icon_group(load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 66:
#line 533 "parser.y"
{ yyval.icog = new_icon_group(yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 67:
#line 541 "parser.y"
{
		if(!win32)
			yywarning("MESSAGETABLE not supported in 16-bit mode");
		yyval.msg = new_messagetable(load_file(yyvsp[0].str));
		;
    break;}
case 68:
#line 549 "parser.y"
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
#line 562 "parser.y"
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
#line 575 "parser.y"
{ yyval.usr = new_user(yyvsp[-2].nid, load_file(yyvsp[0].str), yyvsp[-1].iptr); ;
    break;}
case 71:
#line 576 "parser.y"
{ yyval.usr = new_user(yyvsp[-2].nid, yyvsp[0].raw, yyvsp[-1].iptr); ;
    break;}
case 72:
#line 595 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		set_yywf();
		;
    break;}
case 73:
#line 607 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		set_yywf();
		;
    break;}
case 74:
#line 617 "parser.y"
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
#line 641 "parser.y"
{ yyval.event=NULL; ;
    break;}
case 76:
#line 642 "parser.y"
{ yyval.event=add_string_event(yyvsp[-3].str, yyvsp[-1].num, yyvsp[0].num, yyvsp[-4].event); ;
    break;}
case 77:
#line 643 "parser.y"
{ yyval.event=add_event(yyvsp[-3].num, yyvsp[-1].num, yyvsp[0].num, yyvsp[-4].event); ;
    break;}
case 78:
#line 646 "parser.y"
{ yyval.num=0; ;
    break;}
case 79:
#line 647 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_NOINVERT; ;
    break;}
case 80:
#line 648 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_SHIFT; ;
    break;}
case 81:
#line 649 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_CONTROL; ;
    break;}
case 82:
#line 650 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_ALT; ;
    break;}
case 83:
#line 651 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_ASCII; ;
    break;}
case 84:
#line 652 "parser.y"
{ yyval.num=yyvsp[-2].num | WRC_AF_VIRTKEY; ;
    break;}
case 85:
#line 658 "parser.y"
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
			yyval.dlg->style = WS_POPUP;
			yyval.dlg->gotstyle = TRUE;
		}
		if(yyval.dlg->title)
			yyval.dlg->style |= WS_CAPTION;
		if(yyval.dlg->font)
			yyval.dlg->style |= DS_SETFONT;
		indialog = FALSE;
		if(!yyval.dlg->lvc.language)
			yyval.dlg->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 86:
#line 688 "parser.y"
{ yyval.dlg=new_dialog(); ;
    break;}
case 87:
#line 689 "parser.y"
{ yyval.dlg=dialog_style(yyvsp[0].num,yyvsp[-2].dlg); ;
    break;}
case 88:
#line 690 "parser.y"
{ yyval.dlg=dialog_exstyle(yyvsp[0].num,yyvsp[-2].dlg); ;
    break;}
case 89:
#line 691 "parser.y"
{ yyval.dlg=dialog_caption(yyvsp[0].str,yyvsp[-2].dlg); ;
    break;}
case 90:
#line 692 "parser.y"
{ yyval.dlg=dialog_font(yyvsp[0].fntid,yyvsp[-1].dlg); ;
    break;}
case 91:
#line 693 "parser.y"
{ yyval.dlg=dialog_class(yyvsp[0].nid,yyvsp[-2].dlg); ;
    break;}
case 92:
#line 694 "parser.y"
{ yyval.dlg=dialog_menu(yyvsp[0].nid,yyvsp[-2].dlg); ;
    break;}
case 93:
#line 695 "parser.y"
{ yyval.dlg=dialog_language(yyvsp[0].lan,yyvsp[-1].dlg); ;
    break;}
case 94:
#line 696 "parser.y"
{ yyval.dlg=dialog_characteristics(yyvsp[0].chars,yyvsp[-1].dlg); ;
    break;}
case 95:
#line 697 "parser.y"
{ yyval.dlg=dialog_version(yyvsp[0].ver,yyvsp[-1].dlg); ;
    break;}
case 96:
#line 700 "parser.y"
{ yyval.ctl = NULL; ;
    break;}
case 97:
#line 701 "parser.y"
{ yyval.ctl=ins_ctrl(-1, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 98:
#line 702 "parser.y"
{ yyval.ctl=ins_ctrl(CT_EDIT, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 99:
#line 703 "parser.y"
{ yyval.ctl=ins_ctrl(CT_LISTBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 100:
#line 704 "parser.y"
{ yyval.ctl=ins_ctrl(CT_COMBOBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 101:
#line 705 "parser.y"
{ yyval.ctl=ins_ctrl(CT_SCROLLBAR, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 102:
#line 706 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_CHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 103:
#line 707 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_DEFPUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 104:
#line 708 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_GROUPBOX, yyvsp[0].ctl, yyvsp[-2].ctl);;
    break;}
case 105:
#line 709 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_PUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 106:
#line 711 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_RADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 107:
#line 712 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTO3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 108:
#line 713 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 109:
#line 714 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTOCHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 110:
#line 715 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTORADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 111:
#line 716 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_LEFT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 112:
#line 717 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_CENTER, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 113:
#line 718 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_RIGHT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 114:
#line 720 "parser.y"
{
		yyvsp[0].ctl->title = yyvsp[-7].nid;
		yyvsp[0].ctl->id = yyvsp[-5].num;
		yyvsp[0].ctl->x = yyvsp[-3].num;
		yyvsp[0].ctl->y = yyvsp[-1].num;
		yyval.ctl = ins_ctrl(CT_STATIC, SS_ICON, yyvsp[0].ctl, yyvsp[-9].ctl);
		;
    break;}
case 115:
#line 730 "parser.y"
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
		if(yyvsp[0].iptr)
		{
			yyval.ctl->style = *(yyvsp[0].iptr);
			yyval.ctl->gotstyle = TRUE;
			free(yyvsp[0].iptr);
		}
		;
    break;}
case 116:
#line 750 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->id = yyvsp[-9].num;
		yyval.ctl->x = yyvsp[-7].num;
		yyval.ctl->y = yyvsp[-5].num;
		yyval.ctl->width = yyvsp[-3].num;
		yyval.ctl->height = yyvsp[-1].num;
		if(yyvsp[0].iptr)
		{
			yyval.ctl->style = *(yyvsp[0].iptr);
			yyval.ctl->gotstyle = TRUE;
			free(yyvsp[0].iptr);
		}
		;
    break;}
case 117:
#line 767 "parser.y"
{ yyval.ctl = new_control(); ;
    break;}
case 118:
#line 769 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-2].num;
		yyval.ctl->height = yyvsp[0].num;
		;
    break;}
case 119:
#line 774 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-4].num;
		yyval.ctl->height = yyvsp[-2].num;
		yyval.ctl->style = yyvsp[0].num;
		yyval.ctl->gotstyle = TRUE;
		;
    break;}
case 120:
#line 781 "parser.y"
{
		yyval.ctl = new_control();
		yyval.ctl->width = yyvsp[-6].num;
		yyval.ctl->height = yyvsp[-4].num;
		yyval.ctl->style = yyvsp[-2].num;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->exstyle = yyvsp[0].num;
		yyval.ctl->gotexstyle = TRUE;
		;
    break;}
case 121:
#line 792 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-16].nid;
		yyval.ctl->id = yyvsp[-14].num;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-12].nid);
		yyval.ctl->style = yyvsp[-10].num;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->x = yyvsp[-8].num;
		yyval.ctl->y = yyvsp[-6].num;
		yyval.ctl->width = yyvsp[-4].num;
		yyval.ctl->height = yyvsp[-2].num;
		yyval.ctl->exstyle = yyvsp[0].num;
		yyval.ctl->gotexstyle = TRUE;
		;
    break;}
case 122:
#line 806 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-14].nid;
		yyval.ctl->id = yyvsp[-12].num;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-10].nid);
		yyval.ctl->style = yyvsp[-8].num;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->x = yyvsp[-6].num;
		yyval.ctl->y = yyvsp[-4].num;
		yyval.ctl->width = yyvsp[-2].num;
		yyval.ctl->height = yyvsp[0].num;
		;
    break;}
case 123:
#line 821 "parser.y"
{ yyval.fntid = new_font_id(yyvsp[-2].num, yyvsp[0].str, 0, 0); ;
    break;}
case 124:
#line 825 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 125:
#line 826 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 126:
#line 830 "parser.y"
{ yyval.styles = NULL; ;
    break;}
case 127:
#line 831 "parser.y"
{ yyval.styles = new_style_pair(yyvsp[0].num, 0); ;
    break;}
case 128:
#line 832 "parser.y"
{ yyval.styles = new_style_pair(yyvsp[-2].num, yyvsp[0].num); ;
    break;}
case 129:
#line 836 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_ord;
		yyval.nid->name.i_name = yyvsp[0].num;
		;
    break;}
case 130:
#line 841 "parser.y"
{
		yyval.nid = new_name_id();
		yyval.nid->type = name_str;
		yyval.nid->name.s_name = yyvsp[0].str;
		;
    break;}
case 131:
#line 850 "parser.y"
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
		if(!yyval.dlgex->gotstyle)
		{
			yyval.dlgex->style = WS_POPUP;
			yyval.dlgex->gotstyle = TRUE;
		}
		if(yyval.dlgex->title)
			yyval.dlgex->style |= WS_CAPTION;
		if(yyval.dlgex->font)
			yyval.dlgex->style |= DS_SETFONT;
		indialog = FALSE;
		if(!yyval.dlgex->lvc.language)
			yyval.dlgex->lvc.language = dup_language(currentlanguage);
		;
    break;}
case 132:
#line 888 "parser.y"
{ yyval.dlgex=new_dialogex(); ;
    break;}
case 133:
#line 889 "parser.y"
{ yyval.dlgex=dialogex_style(yyvsp[0].num,yyvsp[-2].dlgex); ;
    break;}
case 134:
#line 890 "parser.y"
{ yyval.dlgex=dialogex_exstyle(yyvsp[0].num,yyvsp[-2].dlgex); ;
    break;}
case 135:
#line 891 "parser.y"
{ yyval.dlgex=dialogex_caption(yyvsp[0].str,yyvsp[-2].dlgex); ;
    break;}
case 136:
#line 892 "parser.y"
{ yyval.dlgex=dialogex_font(yyvsp[0].fntid,yyvsp[-1].dlgex); ;
    break;}
case 137:
#line 893 "parser.y"
{ yyval.dlgex=dialogex_font(yyvsp[0].fntid,yyvsp[-1].dlgex); ;
    break;}
case 138:
#line 894 "parser.y"
{ yyval.dlgex=dialogex_class(yyvsp[0].nid,yyvsp[-2].dlgex); ;
    break;}
case 139:
#line 895 "parser.y"
{ yyval.dlgex=dialogex_menu(yyvsp[0].nid,yyvsp[-2].dlgex); ;
    break;}
case 140:
#line 896 "parser.y"
{ yyval.dlgex=dialogex_language(yyvsp[0].lan,yyvsp[-1].dlgex); ;
    break;}
case 141:
#line 897 "parser.y"
{ yyval.dlgex=dialogex_characteristics(yyvsp[0].chars,yyvsp[-1].dlgex); ;
    break;}
case 142:
#line 898 "parser.y"
{ yyval.dlgex=dialogex_version(yyvsp[0].ver,yyvsp[-1].dlgex); ;
    break;}
case 143:
#line 901 "parser.y"
{ yyval.ctl = NULL; ;
    break;}
case 144:
#line 902 "parser.y"
{ yyval.ctl=ins_ctrl(-1, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 145:
#line 903 "parser.y"
{ yyval.ctl=ins_ctrl(CT_EDIT, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 146:
#line 904 "parser.y"
{ yyval.ctl=ins_ctrl(CT_LISTBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 147:
#line 905 "parser.y"
{ yyval.ctl=ins_ctrl(CT_COMBOBOX, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 148:
#line 906 "parser.y"
{ yyval.ctl=ins_ctrl(CT_SCROLLBAR, 0, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 149:
#line 907 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_CHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 150:
#line 908 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_DEFPUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 151:
#line 909 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_GROUPBOX, yyvsp[0].ctl, yyvsp[-2].ctl);;
    break;}
case 152:
#line 910 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_PUSHBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 153:
#line 912 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_RADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 154:
#line 913 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTO3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 155:
#line 914 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_3STATE, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 156:
#line 915 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTOCHECKBOX, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 157:
#line 916 "parser.y"
{ yyval.ctl=ins_ctrl(CT_BUTTON, BS_AUTORADIOBUTTON, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 158:
#line 917 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_LEFT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 159:
#line 918 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_CENTER, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 160:
#line 919 "parser.y"
{ yyval.ctl=ins_ctrl(CT_STATIC, SS_RIGHT, yyvsp[0].ctl, yyvsp[-2].ctl); ;
    break;}
case 161:
#line 921 "parser.y"
{
		yyvsp[0].ctl->title = yyvsp[-7].nid;
		yyvsp[0].ctl->id = yyvsp[-5].num;
		yyvsp[0].ctl->x = yyvsp[-3].num;
		yyvsp[0].ctl->y = yyvsp[-1].num;
		yyval.ctl = ins_ctrl(CT_STATIC, SS_ICON, yyvsp[0].ctl, yyvsp[-9].ctl);
		;
    break;}
case 162:
#line 932 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-18].nid;
		yyval.ctl->id = yyvsp[-16].num;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-14].nid);
		yyval.ctl->style = yyvsp[-12].num;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->x = yyvsp[-10].num;
		yyval.ctl->y = yyvsp[-8].num;
		yyval.ctl->width = yyvsp[-6].num;
		yyval.ctl->height = yyvsp[-4].num;
		if(yyvsp[-2].iptr)
		{
			yyval.ctl->exstyle = *(yyvsp[-2].iptr);
			yyval.ctl->gotexstyle = TRUE;
			free(yyvsp[-2].iptr);
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
case 163:
#line 957 "parser.y"
{
		yyval.ctl=new_control();
		yyval.ctl->title = yyvsp[-15].nid;
		yyval.ctl->id = yyvsp[-13].num;
		yyval.ctl->style = yyvsp[-9].num;
		yyval.ctl->gotstyle = TRUE;
		yyval.ctl->ctlclass = convert_ctlclass(yyvsp[-11].nid);
		yyval.ctl->x = yyvsp[-7].num;
		yyval.ctl->y = yyvsp[-5].num;
		yyval.ctl->width = yyvsp[-3].num;
		yyval.ctl->height = yyvsp[-1].num;
		yyval.ctl->extra = yyvsp[0].raw;
		;
    break;}
case 164:
#line 973 "parser.y"
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
case 165:
#line 1001 "parser.y"
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
case 166:
#line 1024 "parser.y"
{ yyval.raw = NULL; ;
    break;}
case 167:
#line 1025 "parser.y"
{ yyval.raw = yyvsp[0].raw; ;
    break;}
case 168:
#line 1028 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 169:
#line 1029 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 170:
#line 1033 "parser.y"
{ yyval.fntid = new_font_id(yyvsp[-7].num, yyvsp[-5].str, yyvsp[-3].num, yyvsp[-1].num); ;
    break;}
case 171:
#line 1040 "parser.y"
{ yyval.fntid = NULL; ;
    break;}
case 172:
#line 1041 "parser.y"
{ yyval.fntid = NULL; ;
    break;}
case 173:
#line 1045 "parser.y"
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
case 174:
#line 1068 "parser.y"
{ yyval.menitm = yyvsp[-1].menitm; ;
    break;}
case 175:
#line 1072 "parser.y"
{yyval.menitm = NULL;;
    break;}
case 176:
#line 1073 "parser.y"
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
case 177:
#line 1082 "parser.y"
{
		yyval.menitm=new_menu_item();
		yyval.menitm->prev = yyvsp[-2].menitm;
		if(yyvsp[-2].menitm)
			yyvsp[-2].menitm->next = yyval.menitm;
		;
    break;}
case 178:
#line 1088 "parser.y"
{
		yyval.menitm = new_menu_item();
		yyval.menitm->prev = yyvsp[-4].menitm;
		if(yyvsp[-4].menitm)
			yyvsp[-4].menitm->next = yyval.menitm;
		yyval.menitm->popup = get_item_head(yyvsp[0].menitm);
		yyval.menitm->name = yyvsp[-2].str;
		;
    break;}
case 179:
#line 1107 "parser.y"
{ yyval.num = 0; ;
    break;}
case 180:
#line 1108 "parser.y"
{ yyval.num = yyvsp[0].num | MF_CHECKED; ;
    break;}
case 181:
#line 1109 "parser.y"
{ yyval.num = yyvsp[0].num | MF_GRAYED; ;
    break;}
case 182:
#line 1110 "parser.y"
{ yyval.num = yyvsp[0].num | MF_HELP; ;
    break;}
case 183:
#line 1111 "parser.y"
{ yyval.num = yyvsp[0].num | MF_DISABLED; ;
    break;}
case 184:
#line 1112 "parser.y"
{ yyval.num = yyvsp[0].num | MF_MENUBARBREAK; ;
    break;}
case 185:
#line 1113 "parser.y"
{ yyval.num = yyvsp[0].num | MF_MENUBREAK; ;
    break;}
case 186:
#line 1117 "parser.y"
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
case 187:
#line 1142 "parser.y"
{ yyval.menexitm = yyvsp[-1].menexitm; ;
    break;}
case 188:
#line 1146 "parser.y"
{yyval.menexitm = NULL; ;
    break;}
case 189:
#line 1147 "parser.y"
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
case 190:
#line 1163 "parser.y"
{
		yyval.menexitm = new_menuex_item();
		yyval.menexitm->prev = yyvsp[-2].menexitm;
		if(yyvsp[-2].menexitm)
			yyvsp[-2].menexitm->next = yyval.menexitm;
		;
    break;}
case 191:
#line 1169 "parser.y"
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
case 192:
#line 1189 "parser.y"
{ yyval.exopt = new_itemex_opt(0, 0, 0, 0); ;
    break;}
case 193:
#line 1190 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[0].num, 0, 0, 0);
		yyval.exopt->gotid = TRUE;
		;
    break;}
case 194:
#line 1194 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-3].iptr ? *(yyvsp[-3].iptr) : 0, yyvsp[-1].iptr ? *(yyvsp[-1].iptr) : 0, yyvsp[0].num, 0);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		if(yyvsp[-3].iptr) free(yyvsp[-3].iptr);
		if(yyvsp[-1].iptr) free(yyvsp[-1].iptr);
		;
    break;}
case 195:
#line 1202 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-4].iptr ? *(yyvsp[-4].iptr) : 0, yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		if(yyvsp[-4].iptr) free(yyvsp[-4].iptr);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
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
		yyval.exopt = new_itemex_opt(yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0, 0);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		;
    break;}
case 199:
#line 1224 "parser.y"
{
		yyval.exopt = new_itemex_opt(yyvsp[-4].iptr ? *(yyvsp[-4].iptr) : 0, yyvsp[-2].iptr ? *(yyvsp[-2].iptr) : 0, yyvsp[0].num, 0);
		if(yyvsp[-4].iptr) free(yyvsp[-4].iptr);
		if(yyvsp[-2].iptr) free(yyvsp[-2].iptr);
		yyval.exopt->gotid = TRUE;
		yyval.exopt->gottype = TRUE;
		yyval.exopt->gotstate = TRUE;
		;
    break;}
case 200:
#line 1232 "parser.y"
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
case 201:
#line 1252 "parser.y"
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
case 202:
#line 1293 "parser.y"
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
case 203:
#line 1304 "parser.y"
{ yyval.stt = NULL; ;
    break;}
case 204:
#line 1305 "parser.y"
{
		int i;
		assert(tagstt != NULL);
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
case 207:
#line 1341 "parser.y"
{
		yyval.veri = yyvsp[-3].veri;
		yyvsp[-3].veri->blocks = get_ver_block_head(yyvsp[-1].blk);
		;
    break;}
case 208:
#line 1348 "parser.y"
{ yyval.veri = new_versioninfo(); ;
    break;}
case 209:
#line 1349 "parser.y"
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
case 210:
#line 1359 "parser.y"
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
case 211:
#line 1369 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ff)
			yyerror("FILEFLAGS already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileflags = yyvsp[0].num;
		yyval.veri->gotit.ff = 1;
		;
    break;}
case 212:
#line 1376 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ffm)
			yyerror("FILEFLAGSMASK already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileflagsmask = yyvsp[0].num;
		yyval.veri->gotit.ffm = 1;
		;
    break;}
case 213:
#line 1383 "parser.y"
{
		if(yyvsp[-2].veri->gotit.fo)
			yyerror("FILEOS already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->fileos = yyvsp[0].num;
		yyval.veri->gotit.fo = 1;
		;
    break;}
case 214:
#line 1390 "parser.y"
{
		if(yyvsp[-2].veri->gotit.ft)
			yyerror("FILETYPE already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->filetype = yyvsp[0].num;
		yyval.veri->gotit.ft = 1;
		;
    break;}
case 215:
#line 1397 "parser.y"
{
		if(yyvsp[-2].veri->gotit.fst)
			yyerror("FILESUBTYPE already defined");
		yyval.veri = yyvsp[-2].veri;
		yyval.veri->filesubtype = yyvsp[0].num;
		yyval.veri->gotit.fst = 1;
		;
    break;}
case 216:
#line 1407 "parser.y"
{ yyval.blk = NULL; ;
    break;}
case 217:
#line 1408 "parser.y"
{
		yyval.blk = yyvsp[0].blk;
		yyval.blk->prev = yyvsp[-1].blk;
		if(yyvsp[-1].blk)
			yyvsp[-1].blk->next = yyval.blk;
		;
    break;}
case 218:
#line 1417 "parser.y"
{
		yyval.blk = new_ver_block();
		yyval.blk->name = yyvsp[-3].str;
		yyval.blk->values = get_ver_value_head(yyvsp[-1].val);
		;
    break;}
case 219:
#line 1425 "parser.y"
{ yyval.val = NULL; ;
    break;}
case 220:
#line 1426 "parser.y"
{
		yyval.val = yyvsp[0].val;
		yyval.val->prev = yyvsp[-1].val;
		if(yyvsp[-1].val)
			yyvsp[-1].val->next = yyval.val;
		;
    break;}
case 221:
#line 1435 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_block;
		yyval.val->value.block = yyvsp[0].blk;
		;
    break;}
case 222:
#line 1440 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_str;
		yyval.val->key = yyvsp[-2].str;
		yyval.val->value.str = yyvsp[0].str;
		;
    break;}
case 223:
#line 1446 "parser.y"
{
		yyval.val = new_ver_value();
		yyval.val->type = val_words;
		yyval.val->key = yyvsp[-2].str;
		yyval.val->value.words = yyvsp[0].verw;
		;
    break;}
case 224:
#line 1455 "parser.y"
{ yyval.verw = new_ver_words(yyvsp[0].num); ;
    break;}
case 225:
#line 1456 "parser.y"
{ yyval.verw = add_ver_words(yyvsp[-2].verw, yyvsp[0].num); ;
    break;}
case 226:
#line 1460 "parser.y"
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
case 227:
#line 1486 "parser.y"
{ yyval.tlbarItems = NULL; ;
    break;}
case 228:
#line 1487 "parser.y"
{         
		toolbar_item_t *idrec = new_toolbar_item();
		idrec->id = yyvsp[0].num;
		yyval.tlbarItems = ins_tlbr_button(yyvsp[-2].tlbarItems, idrec); 
		;
    break;}
case 229:
#line 1492 "parser.y"
{         
		toolbar_item_t *idrec = new_toolbar_item();
		idrec->id = 0;
		yyval.tlbarItems = ins_tlbr_button(yyvsp[-1].tlbarItems, idrec); 
	;
    break;}
case 230:
#line 1501 "parser.y"
{ yyval.iptr = NULL; ;
    break;}
case 231:
#line 1502 "parser.y"
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
case 232:
#line 1512 "parser.y"
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
case 233:
#line 1527 "parser.y"
{ yyval.iptr = new_int(WRC_MO_PRELOAD); ;
    break;}
case 234:
#line 1528 "parser.y"
{ yyval.iptr = new_int(WRC_MO_MOVEABLE); ;
    break;}
case 235:
#line 1529 "parser.y"
{ yyval.iptr = new_int(WRC_MO_DISCARDABLE); ;
    break;}
case 236:
#line 1530 "parser.y"
{ yyval.iptr = new_int(WRC_MO_PURE); ;
    break;}
case 237:
#line 1533 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_PRELOAD); ;
    break;}
case 238:
#line 1534 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_MOVEABLE); ;
    break;}
case 239:
#line 1535 "parser.y"
{ yyval.iptr = new_int(~WRC_MO_PURE); ;
    break;}
case 240:
#line 1539 "parser.y"
{ yyval.lvc = new_lvc(); ;
    break;}
case 241:
#line 1540 "parser.y"
{
		if(!win32)
			yywarning("LANGUAGE not supported in 16-bit mode");
		if(yyvsp[-1].lvc->language)
			yyerror("Language already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->language = yyvsp[0].lan;
		;
    break;}
case 242:
#line 1548 "parser.y"
{
		if(!win32)
			yywarning("CHARACTERISTICS not supported in 16-bit mode");
		if(yyvsp[-1].lvc->characts)
			yyerror("Characteristics already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->characts = yyvsp[0].chars;
		;
    break;}
case 243:
#line 1556 "parser.y"
{
		if(!win32)
			yywarning("VERSION not supported in 16-bit mode");
		if(yyvsp[-1].lvc->version)
			yyerror("Version already defined");
		yyval.lvc = yyvsp[-1].lvc;
		yyvsp[-1].lvc->version = yyvsp[0].ver;
		;
    break;}
case 244:
#line 1567 "parser.y"
{ yyval.lan = new_language(yyvsp[-2].num, yyvsp[0].num); ;
    break;}
case 245:
#line 1571 "parser.y"
{ yyval.chars = new_characts(yyvsp[0].num); ;
    break;}
case 246:
#line 1575 "parser.y"
{ yyval.ver = new_version(yyvsp[0].num); ;
    break;}
case 247:
#line 1579 "parser.y"
{ yyval.raw = yyvsp[-1].raw; ;
    break;}
case 248:
#line 1583 "parser.y"
{ yyval.raw = yyvsp[0].raw; ;
    break;}
case 249:
#line 1584 "parser.y"
{ yyval.raw = int2raw_data(yyvsp[0].num); ;
    break;}
case 250:
#line 1585 "parser.y"
{ yyval.raw = long2raw_data(yyvsp[0].num); ;
    break;}
case 251:
#line 1586 "parser.y"
{ yyval.raw = str2raw_data(yyvsp[0].str); ;
    break;}
case 252:
#line 1587 "parser.y"
{ yyval.raw = merge_raw_data(yyvsp[-2].raw, yyvsp[0].raw); free(yyvsp[0].raw->data); free(yyvsp[0].raw); ;
    break;}
case 253:
#line 1588 "parser.y"
{ yyval.raw = merge_raw_data_int(yyvsp[-2].raw, yyvsp[0].num); ;
    break;}
case 254:
#line 1589 "parser.y"
{ yyval.raw = merge_raw_data_long(yyvsp[-2].raw, yyvsp[0].num); ;
    break;}
case 255:
#line 1590 "parser.y"
{ yyval.raw = merge_raw_data_str(yyvsp[-2].raw, yyvsp[0].str); ;
    break;}
case 256:
#line 1597 "parser.y"
{ yyval.iptr = 0; ;
    break;}
case 257:
#line 1598 "parser.y"
{ yyval.iptr = new_int(yyvsp[0].num); ;
    break;}
case 258:
#line 1600 "parser.y"
{ yyval.num = (yyvsp[0].num) & andmask; ;
    break;}
case 259:
#line 1603 "parser.y"
{ yyval.num = 0; andmask = -1; ;
    break;}
case 260:
#line 1606 "parser.y"
{ yyval.num = (yyvsp[-2].num) + (yyvsp[0].num); ;
    break;}
case 261:
#line 1607 "parser.y"
{ yyval.num = (yyvsp[-2].num) - (yyvsp[0].num); ;
    break;}
case 262:
#line 1608 "parser.y"
{ yyval.num = (yyvsp[-2].num) | (yyvsp[0].num); ;
    break;}
case 263:
#line 1609 "parser.y"
{ yyval.num = (yyvsp[-2].num) & (yyvsp[0].num); ;
    break;}
case 264:
#line 1610 "parser.y"
{ yyval.num = (yyvsp[-2].num) * (yyvsp[0].num); ;
    break;}
case 265:
#line 1611 "parser.y"
{ yyval.num = (yyvsp[-2].num) / (yyvsp[0].num); ;
    break;}
case 266:
#line 1612 "parser.y"
{ yyval.num = ~(yyvsp[0].num); ;
    break;}
case 267:
#line 1613 "parser.y"
{ yyval.num = -(yyvsp[0].num); ;
    break;}
case 268:
#line 1615 "parser.y"
{ yyval.num = yyvsp[-1].num; ;
    break;}
case 269:
#line 1616 "parser.y"
{ yyval.num = yyvsp[0].num; want_rscname = 0; ;
    break;}
case 270:
#line 1617 "parser.y"
{ yyval.num = 0; andmask &= ~(yyvsp[0].num); ;
    break;}
case 271:
#line 1620 "parser.y"
{ yyval.num = yyvsp[0].num; ;
    break;}
case 272:
#line 1621 "parser.y"
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
#line 1624 "parser.y"

/* Dialog specific functions */
dialog_t *dialog_style(int st, dialog_t *dlg)
{
	DWORD s = 0;
	assert(dlg != NULL);
	if(dlg->gotstyle)
	{
		yywarning("Style already defined, or-ing together");
		s = dlg->style;
	}
	dlg->style = st | s;
	dlg->gotstyle = TRUE;
	return dlg;
}

dialog_t *dialog_exstyle(int st, dialog_t *dlg)
{
	DWORD s = 0;
	assert(dlg != NULL);
	if(dlg->gotexstyle)
	{
		yywarning("ExStyle already defined, or-ing together");
		s = dlg->style;
	}
	dlg->exstyle = st | s;
	dlg->gotexstyle = TRUE;
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
control_t *ins_ctrl(int type, int style, control_t *ctrl, control_t *prev)
{
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

	/* Hm... this seems to be jammed in at all time... */
	ctrl->style |= WS_CHILD | WS_VISIBLE;
	switch(type)
	{
	case CT_BUTTON:
		ctrl->style |= style;
		if(style != BS_GROUPBOX && style != BS_RADIOBUTTON)
			ctrl->style |= WS_TABSTOP;
		break;
	case CT_EDIT:
		ctrl->style |= WS_TABSTOP | WS_BORDER;
		break;
	case CT_LISTBOX:
		ctrl->style |= LBS_NOTIFY | WS_BORDER;
		break;
	case CT_COMBOBOX:
		ctrl->style |= CBS_SIMPLE;
		break;
	case CT_STATIC:
		ctrl->style |= style;
		if(style == SS_CENTER || style == SS_LEFT || style == SS_RIGHT)
			ctrl->style |= WS_GROUP;
		break;
	}

	if(!ctrl->gotstyle)	/* Handle default style setting */
	{
		switch(type)
		{
		case CT_EDIT:
			ctrl->style |= ES_LEFT;
			break;
		case CT_LISTBOX:
			ctrl->style |= LBS_NOTIFY;
			break;
		case CT_COMBOBOX:
			ctrl->style |= CBS_SIMPLE | WS_TABSTOP;
			break;
		case CT_SCROLLBAR:
			ctrl->style |= SBS_HORZ;
			break;
		case CT_BUTTON:
			switch(style)
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
				ctrl->style |= WS_TABSTOP;
				break;
			default:
				yywarning("Unknown default button control-style 0x%08x", style);
			case BS_RADIOBUTTON:
				break;
			}
			break;

		case CT_STATIC:
			switch(style)
			{
			case SS_LEFT:
			case SS_RIGHT:
			case SS_CENTER:
				ctrl->style |= WS_GROUP;
				break;
			case SS_ICON:	/* Special case */
				break;
			default:
				yywarning("Unknown default static control-style 0x%08x", style);
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
	if(type == CT_STATIC && style == SS_ICON)
		ctrl->style |= SS_ICON;

	ctrl->gotstyle = TRUE;
byebye:
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
dialogex_t *dialogex_style(int st, dialogex_t *dlg)
{
	DWORD s = 0;
	assert(dlg != NULL);
	if(dlg->gotstyle)
	{
		yywarning("Style already defined, or-ing together");
		s = dlg->style;
	}
	dlg->style = st | s;
	dlg->gotstyle = TRUE;
	return dlg;
}

dialogex_t *dialogex_exstyle(int st, dialogex_t *dlg)
{
	DWORD s = 0;
	assert(dlg != NULL);
	if(dlg->gotexstyle)
	{
		yywarning("ExStyle already defined, or-ing together");
		s = dlg->exstyle;
	}
	dlg->exstyle = st | s;
	dlg->gotexstyle = TRUE;
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

