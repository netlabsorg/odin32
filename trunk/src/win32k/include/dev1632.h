/* $Id: dev1632.h,v 1.1 1999-09-06 02:19:57 bird Exp $
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

#ifdef _OS2Krnl_h_

#define MAXKRNLOBJECTS 24
typedef struct _KRNLOBJTABLE
{
    unsigned char   cObjects;
    OTE             aObjects[MAXKRNLOBJECTS];
} KRNLOBJTABLE, FAR * PKRNLOBJTABLE;
#endif

#pragma pack()
#endif
