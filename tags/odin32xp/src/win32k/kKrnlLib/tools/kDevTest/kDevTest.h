/* $Id: kDevTest.h,v 1.1 2002-09-30 23:53:53 bird Exp $
 *
 * Ring-3 Device Testing: Prototypes, structures, defines.
 *
 * Copyright (c) 2002 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __kDevTest_h__
#define __kDevTest_h__


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define SEL_FLAT_MASK       0x1fff0000
#define SEL_FLAT_SHIFT      0x0d
#define SEL_LDT_RPL3        0x07

#define FPToFlat(fp)    SelToFlat( (((ULONG)(fp)) >> 16), (USHORT)(fp))

#define SelToFlat(sel, off) \
    (PVOID)( (((unsigned)(sel) << SEL_FLAT_SHIFT) & SEL_FLAT_MASK) + (unsigned)(off))

#define FlatToSel(flataddr) \
    (PVOID)( ( (((unsigned)(flataddr) << 3) & 0xfff80000) | (SEL_LDT_RPL3 << 16) ) | ((unsigned)(flataddr) & 0xffff) )

/*
 * Strategy Commands.
 */
#define CMDInit                  0      /* INIT command */
#define CMDMedChk                1      /* Media Check */
#define CMDBldBPB                2      /* build BPB */
#define CMDIOCTLR                3      /* reserved for 3.x compatability */
#define CMDINPUT                 4      /* read data from device */
#define CMDNDR                   5      /* non-destructive read */
#define CMDInputS                6      /* input status */
#define CMDInputF                7      /* input flush */
#define CMDOUTPUT                8      /* write data to device */
#define CMDOUTPUTV               9      /* write data and verify */
#define CMDOutputS              10      /* output status */
#define CMDOutputF              11      /* output flush */
#define CMDIOCTLW               12      /* reserved for 3.x compatability */
#define CMDOpen                 13      /* device open */
#define CMDClose                14      /* device close */
#define CMDRemMed               15      /* is media removable */
#define CMDGenIOCTL             16      /* Generic IOCTL */
#define CMDResMed               17      /* reset media uncertain */
#define CMDGetLogMap            18
#define CMDSetLogMap            19
#define CMDDeInstall            20      /* De-Install driver */
#define CMDPartfixeddisks       22      /* Partitionable Fixed Disks */
#define CMDGetfd_logunitsmap    23      /* Get Fixed Disk/Logical Unit Map */
#define CMDInputBypass          24      /* cache bypass read data */
#define CMDOutputBypass         25      /* cache bypass write data */
#define CMDOutputBypassV        26      /* cache bypass write data and verify */
#define CMDInitBase             27      /* INIT command for base DDs */
#define CMDShutdown             28
#define CMDGetDevSupport        29      /* query for extended capability */
#define CMDInitComplete         31      /* Init complete for all DD's    */
#define CMDSaveRestore          32
#define CMDAddOnPrep            97      /* Prepare for add on */
#define CMDStar                 98      /* start console output */
#define CMDStop                 99      /* stop console output */


/*
 * Status codes.
 */
#define STERR               0x8000      /* Bit 15 - Error  */
#define STINTER             0x0400      /* Bit 10 - Interim character */
#define STBUI               0x0200      /* Bit  9 - Busy */
#define STDON               0x0100      /* Bit  8 - Done */
#define STECODE             0x00FF      /* Error code */
#define WRECODE             0x0000



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)

/*
 * Device header.
 */
typedef struct _DevHdr
{
    unsigned long   fpNext;
    unsigned short  usAttr;
    unsigned short  offStrat;
    unsigned short  offInt;
    unsigned char   achName[8];
    unsigned short  selProtCS;
    unsigned short  selProtDS;
    unsigned short  selRealCS;
    unsigned short  selRealDS;
    unsigned long   ulCaps;
} DEVHDR, *PDEVHDR;



/*
 * Request packets.
 */

typedef struct _RPH32
{
  UCHAR         Len;
  UCHAR         Unit;
  UCHAR         Cmd;
  USHORT        Status;
  UCHAR         Flags;
  UCHAR         Reserved_1[3];
  ULONG         Link;
} RPH32;

typedef struct _RP32INITIN
{
    RPH32       rph;
    UCHAR       Unit;
    ULONG       DevHlpEP;               /* 16 bit far pointer */
    ULONG       InitArgs;               /* 16 bit far pointer */
    UCHAR       DriveNum;
} RP32INITIN, *PRP32INITIN;

typedef struct _RP32INITOUT
{
    RPH32       rph;
    UCHAR       Unit;
    USHORT      CodeEnd;
    USHORT      DataEnd;
    ULONG       BPBArray;               /* (BPBS far ptr) */
    USHORT      Status;
} RP32INITOUT, *PRP32INITOUT;

typedef union _RP32INIT
{
    RP32INITIN  in;
    RP32INITOUT out;
} RP32INIT, *PRP32INIT;



/*
 * Module structures.
 */

typedef struct _ModObjTable
{
    ULONG       ote_size;               /* Object virtual size */
    ULONG       ote_base;               /* Object base virtual address */
    ULONG       ote_flags;              /* Attribute flags */
    ULONG       ote_pagemap;            /* Object page map index */
    ULONG       ote_mapsize;            /* Num of entries in obj page map */
    USHORT      ote_sel;                /* Object Selector */
    USHORT      ote_hob;                /* Object Handle */
} MODOBJTABLE, *PMODOBJTABLE;

#define MOD_MAX_OBJS 32
typedef struct _ModInfo
{
    CHAR        szName[CCHMAXPATH];     /* Module name (no path) */
    HMODULE     hmod;                   /* module handle */
    ULONG       cObjs;                  /* Number of object. */
    MODOBJTABLE aObjs[MOD_MAX_OBJS];    /* Object details. */
} MODINFO, *PMODINFO;

#pragma pack()


/*
 * Device Help + more.
 */
void            kdtDHInit(void);            /* _System/_Optlink !! */
ULONG           kdtDHGetRouterFP(void);     /* _System/_Optlink !! */
void            kdtStackThunk32To16(void);  /* _System/_Optlink !! */
void            kdtStackThunk16To32(void);  /* _System/_Optlink !! */


/*
 * Loader
 */
PMODINFO        kdtLoadDriver(const char *pszModule);
BOOL            kdtLoadValidateDriver(PMODINFO pModInfo);


/*
 * Init.
 */
BOOL            kdtInitDriver(PMODINFO pModInfo, const char *pszArgs, BOOL fBaseDev);


/*
 * Strategy.
 */
BOOL            kdtStrategySend(ULONG fpfnStrat, ULONG fpRP);


#endif

