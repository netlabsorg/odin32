/* $Id: dev16.h,v 1.11.2.1 2001-09-27 03:08:17 bird Exp $
 * dev16 - 16-bit specific. Should not be used in 32-bit C/C++.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (kosmunds@csc.com)
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


/*
 * init functions
 */
USHORT NEAR devInit(PRPINITIN pRpIn, PRPINITOUT pRpOut);


/*
 * Thunking "functions" prototypes
 */
USHORT NEAR CallR0Init32(LIN pRpInit);
USHORT NEAR CallWin32kIOCtl(LIN pRpIOCtl);
USHORT NEAR CallWin32kOpen(LIN pRpOpen);
USHORT NEAR CallWin32kClose(LIN pRpClose);
ULONG  NEAR GetR0InitPtr(void);


/*
 * These are only for use in the 'aDevHdrs'.
 */
extern void NEAR strategyAsm(void);

/*
 * Global data.
 */
extern DDHDR    aDevHdrs[1];
extern PFN      Device_Help;
extern BOOL     fInitTime;


/*
 * Stack to Flat DS - 16-bit version.
 */
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
