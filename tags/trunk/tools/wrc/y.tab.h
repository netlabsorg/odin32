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


extern YYSTYPE yylval;
