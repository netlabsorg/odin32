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


extern YYSTYPE pplval;
