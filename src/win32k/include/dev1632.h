/* $Id: dev1632.h,v 1.7 2001-02-10 11:11:43 bird Exp $
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


#ifdef _OS2KLDR_H_


#define MAXKRNLOBJECTS 24
typedef struct _KRNLINFO
{
    unsigned long   ulBuild;
    unsigned short  fKernel;
    unsigned char   cObjects;
    unsigned char   chPadding;
    OTE             aObjects[MAXKRNLOBJECTS];
} KRNLINFO, FAR * PKRNLINFO;

#else

#define PKRNLINFO void *

#endif

#pragma pack()


/*
 * Global data...
 */
extern CHAR DATA16_GLOBAL   szBuildDate[];
extern CHAR DATA16_GLOBAL   szBuildTime[];
#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map( szBuildDate , "_szBuildDate" )
    #pragma map( szBuildTime , "_szBuildTime" )
#endif

#endif
