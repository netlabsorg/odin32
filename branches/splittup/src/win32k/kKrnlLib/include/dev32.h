/* $Id: dev32.h,v 1.5 2001-09-26 04:00:29 bird Exp $
 *
 * dev32 - header file for 32-bit part of the driver.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _dev32_h_
#define _dev32_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Wrapper for declaration from dev1632.h used in this file.
 */
#ifndef _dev1632_h_
    #define RP32INIT        void
    #define PRP32INIT       void *
    #define RP32GENIOCTL    void
    #define PRP32GENIOCTL   void *
    #define RP32GENIOCTL    void
    #define PRP32GENIOCTL   void *
    #define RP32OPENCLOSE   void
    #define PRP32OPENCLOSE  void *
    #define PKRNLINFO       void *
#endif


/*
 * Defines from reqpkt.h.
 */
#define MAX_DISKDD_CMD      29

#define STERR               0x8000      /* Bit 15 - Error                   */
#define STINTER             0x0400      /* Bit 10 - Interim character       */
#define STBUI               0x0200      /* Bit  9 - Busy                    */
#define STDON               0x0100      /* Bit  8 - Done                    */
#define STECODE             0x00FF      /* Error code                       */
#define WRECODE             0x0000

#define STATUS_DONE         0x0100
#define STATUS_ERR_UNKCMD   0x8003


#define RPF_Int13RP         0x01        /* Int 13 Request Packet            */
#define RPF_CallOutDone     0x02        /* Int 13 Callout completed         */
#define RPF_PktDiskIOTchd   0x04        /* Disk_IO has touched this packet  */
#define RPF_CHS_ADDRESSING  0x08        /* CHS Addressing used in RBA field */
#define RPF_Internal        0x10        /* Internal request packet command  */
#define RPF_TraceComplete   0x20        /* Trace completion flag            */


/*
 * Function prototypes.
 */
#if !defined(__cplusplus) && defined(RING0)
USHORT _loadds _Far32 _Pascal GetKernelInfo32(PKRNLINFO pKrnlInfo);
USHORT _loadds _Far32 _Pascal VerifyImportTab32(void);
#endif
#ifdef _OS2KLDR_H_
PMTE _System GetOS2KrnlMTE(void); /* (devfirst.asm) */
#endif
ULONG  _Optlink InitCallGate(void);
ULONG  _Optlink x86DisableWriteProtect(void);
ULONG  _Optlink x86RestoreWriteProtect(ULONG flWP);
int     interpretFunctionProlog32(char *pach);
int     interpretFunctionProlog16(char *pach);
int     krnlInit(void);
int     krnlInit2(void);



/*
 * Global variables
 */
#ifdef _OS2KLDR_H_
extern POTE    pKrnlOTE;                /* d32init.c */
extern PSMTE   pKrnlSMTE;               /* d32init.c */
#endif
extern PULONG TKSSBase16;
#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map( TKSSBase16 , "_TKSSBase16"  )
#endif
#ifndef SSToDS
extern PULONG pulTKSSBase32;
extern USHORT CallGateGDT;

/*
 * SSToDS( stack pointer )
 *
 * In RING-0 this translates pointers to stack memory in the
 * Stack Segment (SS) to pointers to stack memory in the
 * Data Segment (DS). SS is 16-bit compatible, DS is 32-bit flat.
 * It is vitally important to use this macro whenever creating
 * a pointer to stack memory which don't implies that is a pointer
 * relative to SS. For example when passing the pointer to an stack
 * variable into an function call; like the addToModule calls.
 *
 * Warning - At init-time you have ot make sure that the pulTKSSBase32 pointer is set!
 *
 * @returns    32-bit FLAT pointer.
 */
#ifdef RING0
    #define SSToDS(a)   ((PVOID)((ULONG)(a) + *pulTKSSBase32))
#else
    #define SSToDS(a)   ((PVOID)(a))
#endif

#endif /* SSToDS */

#ifdef __cplusplus
}
#endif


/*
 * START and END labels. NOTE: these are not bytes only assembly labels.
 */
extern char DATA16START      ;
extern char DATA16_BSSSTART  ;
extern char DATA16_CONSTSTART;
extern char DATA16_INITSTART      ;
extern char DATA16_INIT_BSSSTART  ;
extern char DATA16_INIT_CONSTSTART;
extern char CODE16START      ;
extern char CODE16_INITSTART      ;
extern char CODE32START      ;
extern char DATA32START      ;
extern char BSS32START       ;
extern char CONST32_ROSTART  ;
extern char _VFTSTART        ;
extern char EH_DATASTART     ;

extern char CODE16END      ;
extern char DATA16END      ;
extern char DATA16_BSSEND  ;
extern char DATA16_CONSTEND;
extern char DATA16_INITEND      ;
extern char DATA16_INIT_BSSEND  ;
extern char DATA16_INIT_CONSTEND;
extern char CODE16END      ;
extern char CODE16_INITEND      ;
extern char CODE32END      ;
extern char DATA32END      ;
extern char BSS32END       ;
extern char CONST32_ROEND  ;
extern char _VFTEND        ;
extern char EH_DATAEND     ;

#endif
