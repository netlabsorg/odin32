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


extern YYSTYPE yylval;
