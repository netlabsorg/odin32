/* $Id: dev16.h,v 1.1 1999-09-06 02:19:57 bird Exp $
 * dev16 - 16-bit specific. Should not be used in 32-bit C/C++.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _d16_h_
#define _d16_h_

#ifndef VMDHA_USEHIGHMEM /* macro from dhcalls.h where LIN and PLIN normally is declared. */
typedef ULONG       LIN;
typedef ULONG  FAR *PLIN;
#endif

/*
 * Ring 0 init IOCtl (dev1 -> dev0)
 */
#define D16_IOCTL_CAT               0xC0
#define D16_IOCTL_RING0INIT         0x40
typedef struct _D16R0INITDATA
{
    USHORT Status;
} D16R0INITDATA;
typedef D16R0INITDATA FAR *PD16R0INITDATA;

typedef struct _D16R0INITPARAM
{
    PRPINITIN pRpInitIn;
} D16R0INITPARAM;
typedef D16R0INITPARAM FAR *PD16R0INITPARAM;


/*
 * Get Kernel OTEs. No params. Data is KRNLOBJTABLE (dev1632.h).
 */
#define D16_IOCTL_GETKRNLOTES       0x41


/*
 * Get Kernel OTEs. No params. No data. aProcTab is used.
 */
#define D16_IOCTL_VERIFYPROCTAB     0x42


/**
 * Regards goes to Matthieu Willm for this!
 */
typedef struct _DosTable /* dt */
{
    UCHAR  cul;             /* count of entries (dwords) in this table.*/
    ULONG  notinterestingAtAll;
} DOSTABLE, FAR *PDOSTABLE , NEAR *NPDOSTABLE;

typedef struct _DosTable2 /* dt2 */
{
    UCHAR  cul;             /* count of entries (dwords) in this table.*/
    ULONG  notinteresting0;
    ULONG  notinteresting1;
    ULONG  notinteresting2;
    ULONG  notinteresting3;
    ULONG  notinteresting4;
    ULONG  notinteresting5;
    ULONG  notinteresting6;
    ULONG  notinteresting7;
    ULONG  notinteresting8;
    ULONG  R0FlatCS;
    ULONG  R0FlatDS;
    ULONG  TKSSBase;
    ULONG  notinterestingC;
    ULONG  notinterestingD;
    ULONG  notinterestingE;
    ULONG  notinterestingF;
    ULONG  ReserveVM;
    ULONG  notinterestingH;
    ULONG  notinterestingI;
} DOSTABLE2, FAR *PDOSTABLE2 , NEAR *NPDOSTABLE2;


/*
 * init functions
 */
USHORT NEAR dev0Init(PRPINITIN pRpIn, PRPINITOUT pRpOut);
USHORT NEAR dev1Init(PRPINITIN pRpIn, PRPINITOUT pRpOut);
USHORT NEAR R0Init16(PRP_GENIOCTL pRp);


/*
 * Thunking "functions" prototypes
 */
USHORT NEAR CallR0Init32(LIN pRpInit);


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


/*
 * Stack to Flat DS - 16-bit version.
 */
/*#define SSToDS_16(pStackVar) ((LIN)(getTKSSBaseValue() + (ULONG)(USHORT)(pStackVar)))*/
#define SSToDS_16(pStackVar) (SSToDS_16a((void NEAR *)pStackVar))

extern LIN   SSToDS_16a(void NEAR *pStackVar);

/*
 * START and END labels. NOTE: these are not bytes only assembly labels.
 */
extern char PASCAL CODE16START      ;
extern char PASCAL DATA16START      ;
extern char PASCAL DATA16START      ;
extern char PASCAL DATA16_BSSSTART  ;
extern char PASCAL DATA16_CONSTSTART;
extern char PASCAL CODE16START      ;
extern char PASCAL CODE32START      ;
extern char PASCAL DATA32START      ;
extern char PASCAL BSS32START       ;
extern char PASCAL CONST32_ROSTART  ;

extern char PASCAL CODE16END      ;
extern char PASCAL DATA16END      ;
extern char PASCAL DATA16END      ;
extern char PASCAL DATA16_BSSEND  ;
extern char PASCAL DATA16_CONSTEND;
extern char PASCAL CODE16END      ;
extern char PASCAL CODE32END      ;
extern char PASCAL DATA32END      ;
extern char PASCAL BSS32END       ;
extern char PASCAL CONST32_ROEND  ;




#endif
