/* $Id: dev1632.h,v 1.5 2000-04-05 18:40:41 bird Exp $
 * dev1632.h - Common header file for 16-bit and 32-bit C
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

#ifndef _dev1632_h_
#define _dev1632_h_

#pragma pack(1)

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

typedef struct _RP32INIT
{
    RPH32       rph;
    UCHAR       Unit;
    ULONG       DevHlpEP; /* 16 bit far pointer */
    PSZ         InitArgs;
    UCHAR       DriveNum;
} RP32INIT;

typedef struct _RP32GENIOCTL
{
    RPH32       rph;
    UCHAR       Category;
    UCHAR       Function;
    PVOID       ParmPacket;
    PVOID       DataPacket;
    USHORT      sfn;
    USHORT      ParmLen;
    USHORT      DataLen;
} RP32GENIOCTL, *PRP32GENIOCTL;


#ifdef _OS2Krnl_h_


#define MAXKRNLOBJECTS 24
typedef struct _KRNLINFO
{
    unsigned char   cObjects;
    unsigned char   fDebug;
    unsigned char   fchType;             /* TYPE_SMP, TYPE_UNI, TYPE_W4 */
    unsigned short  usBuild;
    OTE             aObjects[MAXKRNLOBJECTS];
} KRNLINFO, FAR * PKRNLINFO;

#else

#define PKRNLINFO void *

#endif

#pragma pack()
#endif