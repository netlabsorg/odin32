/* $Id: dev16.h,v 1.4 2001-11-20 03:43:51 bird Exp $
 * dev16 - 16-bit specific. Should not be used in 32-bit C/C++.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _d16_h_
#define _d16_h_

#ifndef VMDHA_USEHIGHMEM /* macro from dhcalls.h where LIN and PLIN normally is declared. */
typedef ULONG       LIN;
typedef ULONG  FAR *PLIN;
#endif
#ifndef MAX_DISKDD_CMD   /* macro from reqpkt.h where these normally is declared. */
typedef void   FAR *PRPINITIN;
typedef void   FAR *PRPINITOUT;
typedef void   FAR *PRP_GENIOCTL;
typedef ULONG       DDHDR;
#endif


/**
 * Regards goes to Matthieu Willm for (parts of) this!
 */
#ifdef INCL_16
    typedef void FAR * FPVOID;
#else
    typedef ULONG FPVOID;
#endif
typedef struct _DosTable /* dt */
{
    UCHAR  cul;             /* count of entries (dwords) in this table.*/
    FPVOID fph_HardError;
    FPVOID fph_UCase;
    ULONG  UnknownOrReserved1;
    ULONG  UnknownOrReserved2;
    ULONG  UnknownOrReserved3;
    FPVOID fph_MemMapAlias;
    FPVOID fph_MemUnmapAlias;
    FPVOID fph_GoProtAll;
    FPVOID fph_GoRealAll;
    FPVOID fph_doshlp_RedirDev;
    ULONG  UnknownOrReserved4;
    ULONG  UnknownOrReserved5;
    FPVOID fph_SFFromSFN;
    FPVOID fph_SegGetInfo;
    FPVOID fph_AsgCheckDrive;
    ULONG  UnknownOrReserved6;
    ULONG  UnknownOrReserved7;
} DOSTABLE, FAR *PDOSTABLE , NEAR *NPDOSTABLE;

typedef struct _DosTable2 /* dt2 */
{
    UCHAR  cul;             /* count of entries (dwords) in this table.*/
    FPVOID fpErrMap24;
    FPVOID fpErrMap24End;
    FPVOID fpErr_Table_24;
    FPVOID fpCDSAddr;
    FPVOID fpGDT_RDR1;
    FPVOID fpInterrupLevel;
    FPVOID fp_cInDos;
    ULONG  UnknownOrReserved1;
    ULONG  UnknownOrReserved2;
    ULONG  R0FlatCS;
    ULONG  R0FlatDS;
    LIN    pTKSSBase;
    LIN    pintSwitchStack;
    LIN    pprivatStack;
    FPVOID fpPhysDiskTablePtr;
    LIN    pforceEMHandler;
    LIN    pReserveVM;
    LIN    p_pgpPageDir;
    ULONG  UnknownOrReserved3;
} DOSTABLE2, FAR *PDOSTABLE2 , NEAR *NPDOSTABLE2;


/*
 * init functions
 */
USHORT NEAR devInit(PRPINITIN pRpIn, PRPINITOUT pRpOut);
USHORT NEAR R0Init16(PRP_GENIOCTL pRp);
USHORT NEAR initGetDosTableData(void);
ULONG  NEAR GetR0InitPtr(void);
ULONG  NEAR GetVerifyImportTab32Ptr(void);
ULONG  NEAR GetGetKernelInfo32Ptr(void);
ULONG  NEAR GetLookupKrnlEntry32Ptr(void);

/*
 * Thunking "functions" prototypes
 */
ULONG  NEAR CallR0Addr32bit(LIN ulAddr, ULONG ulParam);

/*
 * These are only for use in the 'aDevHdrs'.
 */
extern void NEAR strategyAsm0(void);
extern void NEAR strategyAsm1(void);

/*
 * Global data.
 */
extern DDHDR    aDevHdrs[2];
extern PFN      Device_Help;
extern ULONG    TKSSBase16;
extern USHORT   R0FlatCS16;
extern USHORT   R0FlatDS16;
extern LIN      linDT;
extern LIN      linDT2;
extern BOOL     fInitTime;
extern unsigned long DATA16Base;


/*
 * Stack to Flat DS - 16-bit version.
 */
/*#define SSToDS_16(pStackVar) ((LIN)(getTKSSBaseValue() + (ULONG)(USHORT)(pStackVar)))*/
#define SSToDS_16(pStackVar) (SSToDS_16a((void NEAR *)pStackVar))

extern LIN NEAR SSToDS_16a(void NEAR *pStackVar);

/*
 * START and END labels. NOTE: these are not bytes only assembly labels.
 */
extern char PASCAL DATA16START      ;
extern char PASCAL DATA16_BSSSTART  ;
extern char PASCAL DATA16_CONSTSTART;
extern char PASCAL DATA16_INITSTART      ;
extern char PASCAL DATA16_INIT_BSSSTART  ;
extern char PASCAL DATA16_INIT_CONSTSTART;
extern char PASCAL CODE16START      ;
extern char PASCAL CODE16_INITSTART      ;
extern char PASCAL CODE32START      ;
extern char PASCAL DATA32START      ;
extern char PASCAL BSS32START       ;
extern char PASCAL CONST32_ROSTART  ;
extern char PASCAL _VFTSTART        ;
extern char PASCAL EH_DATASTART     ;

extern char PASCAL CODE16END      ;
extern char PASCAL DATA16END      ;
extern char PASCAL DATA16_BSSEND  ;
extern char PASCAL DATA16_CONSTEND;
extern char PASCAL DATA16_INITEND      ;
extern char PASCAL DATA16_INIT_BSSEND  ;
extern char PASCAL DATA16_INIT_CONSTEND;
extern char PASCAL CODE16END      ;
extern char PASCAL CODE16_INITEND      ;
extern char PASCAL CODE32END      ;
extern char PASCAL DATA32END      ;
extern char PASCAL BSS32END       ;
extern char PASCAL CONST32_ROEND  ;
extern char PASCAL _VFTEND        ;
extern char PASCAL EH_DATAEND     ;



#endif
