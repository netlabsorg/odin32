/* $Id: dev32hlp.h,v 1.4 2001-07-10 16:44:08 bird Exp $
 *
 * Dev32Hlp - 32-bit Device helpers.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _dev32hlp_h_
#define _dev32hlp_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define D32HLPCALL _Optlink
#define D32RET     ULONG


/*
 * D32Hlp_VMAlloc
 */
#define VMDHA_16M               0x0001UL
#define VMDHA_FIXED             0x0002UL
#define VMDHA_SWAP              0x0004UL
#define VMDHA_CONTIG            0x0008UL
#define VMDHA_PHYS              0x0010UL
#define VMDHA_PROCESS           0x0020UL
#define VMDHA_SGSCONT           0x0040UL
#define VMDHA_RESERVE           0x0100UL
#define VMDHA_USEHIGHMEM        0x0800UL

/*
 * D32Hlp_VMLock
 */
#define VMDHL_NOBLOCK           0x0001UL
#define VMDHL_CONTIGUOUS        0x0002UL
#define VMDHL_16M               0x0004UL
#define VMDHL_WRITE             0x0008UL
#define VMDHL_LONG              0x0010UL
#define VMDHL_VERIFY            0x0020UL

/*
 * D32Hlp_VMSetMem
 */
#define VMDHS_DECOMMIT          0x0001UL
#define VMDHS_RESIDENT          0x0002UL
#define VMDHS_SWAP              0x0004UL

/*
 * D32Hlp_GetDOSVar
 */
/*                                       0  reserved */
#define DHGETDOSV_SYSINFOSEG             1
#define DHGETDOSV_LOCINFOSEG             2
/*                                       3  reserved */
#define DHGETDOSV_VECTORSDF              4
#define DHGETDOSV_VECTORREBOOT           5
#define DHGETDOSV_YIELDFLAG              7
#define DHGETDOSV_TCYIELDFLAG            8
#define DHGETDOSV_DOSTABLES              9  /* undocumented */
#define DHGETDOSV_STRP_COMMON           10  /* undocumented */
#define DHGETDOSV_DOSCODEPAGE           11
#define DHGETDOSV_INTERRUPTLEV          13
#define DHGETDOSV_DEVICECLASSTABLE      14
#define DHGETDOSV_DMQSSELECTOR          15
#define DHGETDOSV_APMINFO               16
#define DHGETDOSV_APM_LENGTH            17  /* undocumented? */



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#ifndef __infoseg_h
#define __infoseg_h
#include <infoseg.h>
#endif

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct vmlh_s /* #memb 5 size 12 (0x00c) */
{
    unsigned                lh_page  : 20,     /* off:   0(00)  size:  4(04) */
                            lh_flags : 12;
    short                   lh_hptda;          /* off:   4(04)  size:  2(02) */
    short                   lh_npages;         /* off:   6(06)  size:  2(02) */
    short                   lh_hob;            /* off:   8(08)  size:  2(02) */
    short                   lh_sig;            /* off:  10(0a)  size:  2(02) */
} LOCKHANDLE, *PLOCKHANDLE;


/*******************************************************************************
*   Function Prototypes                                                        *
*******************************************************************************/
PVOID  D32HLPCALL D32Hlp_VirtToLin(ULONG  ulPtr16);
PVOID  D32HLPCALL D32Hlp_VirtToLin2(USHORT usSelector,
                                    ULONG  ulOffset);
D32RET D32HLPCALL D32Hlp_VirtToLin3(USHORT usSelector,
                                    ULONG  ulOffset,
                                    PPVOID ppvLinAddress);
PVOID  D32HLPCALL D32Hlp_GetDOSVar(USHORT ulVarNumber, USHORT ulVarMember);
VOID   D32HLPCALL D32Hlp_Yield(VOID);
PVOID  D32HLPCALL D32Hlp_VMAlloc(ULONG  flFlags,
                                 ULONG  cbSize,
                                 ULONG  ulPhysAddr);
D32RET D32HLPCALL D32Hlp_VMFree(PVOID pvAddress);
D32RET D32HLPCALL D32Hlp_VMSetMem(PVOID pvAddress,
                                  ULONG cbSize,
                                  ULONG flFlags);
D32RET D32HLPCALL D32Hlp_VMLock(PVOID   pvAddress,
                                ULONG   ulLength,
                                ULONG   flFlags,
                                PVOID   pvPagelist,
                                PLOCKHANDLE  pLockHandle,
                                PULONG  pulPageListCount);
D32RET D32HLPCALL D32Hlp_VMLock2(PVOID   pvAddress,
                                 ULONG   ulLength,
                                 ULONG   flFlags,
                                 PLOCKHANDLE  pLockHandle);
D32RET D32HLPCALL D32Hlp_VMUnLock(PLOCKHANDLE pLockHandle);


#ifdef __cplusplus
}
#endif
#endif
