/* $Id: oslibmisc.h,v 1.14 2004-05-24 08:56:07 sandervl Exp $ */
/*
 * Misc util. procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Peter FitzSimmons
 * Copyright 1998 Patrick Haller
 *
 */
#ifndef __OSLIBMISC_H__
#define __OSLIBMISC_H__

char *OSLibGetDllName(ULONG hModule);
BOOL  OSLibGetDllName(ULONG hModule, char *name, int length);

#ifdef __cplusplus
extern "C" {
#endif

char *OSLibStripPath(char *path);

ULONG OSLibiGetModuleHandleA(char *pszModule);
ULONG OSLibQueryModuleHandle(char *modname);

void  OSLibWait(ULONG msec);

ULONG OSLibAllocSel(ULONG size, USHORT *selector);
ULONG OSLibFreeSel(USHORT selector);
PVOID OSLibSelToFlat(USHORT selector);

//******************************************************************************
// Turn off hard errors & exception popups
//******************************************************************************
BOOL  OSLibDisablePopups();

#define TIB_STACKTOP    0
#define TIB_STACKLOW    1

ULONG OSLibGetTIB(int tiboff);

#define PIB_HMTE        0
#define PIB_TASKHNDL    PIB_HMTE
#define PIB_TASKTYPE    1
#define PIB_PCHCMD      2

#define TASKTYPE_PM     0
#define TASKTYPE_VIO    1

ULONG OSLibGetPIB(int iPIB);

ULONG OSLibAllocThreadLocalMemory(int nrdwords);

ULONG OSLibWinInitialize();
ULONG OSLibWinQueryMsgQueue(ULONG hab);
ULONG OSLibWinSetCp(ULONG hmq, ULONG codepage);

//#define CTRY_NONE                        0
#define CTRY_USA                         1
#define CTRY_CANADA                      2
#define CTRY_LATIN_AMERICA               3
#define CTRY_RUSSIA                      7
#define CTRY_GREECE                     30
#define CTRY_NETHERLANDS                31
#define CTRY_BELGIUM                    32
#define CTRY_FRANCE                     33
#define CTRY_SPAIN                      34
#define CTRY_ITALY                      39
#define CTRY_SWITZERLAND                41
#define CTRY_AUSTRIA                    43
#define CTRY_UNITED_KINGDOM             44
#define CTRY_DENMARK                    45
#define CTRY_SWEDEN                     46
#define CTRY_NORWAY                     47
#define CTRY_POLAND                     48
#define CTRY_GERMANY                    49
#define CTRY_MEXICO                     52
#define CTRY_BRAZIL                     55
#define CTRY_AUSTRALIA                  61
#define CTRY_NEW_ZEALAND                64
#define CTRY_SINGAPORE                  65
#define CTRY_JAPAN                      81
#define CTRY_KOREA                      82
#define CTRY_CHINA                      86
#define CTRY_TAIWAN                     88
#define CTRY_TURKEY                     90
#define CTRY_PORTUGAL                  351
#define CTRY_LUXEMBOURG                352
#define CTRY_IRELAND                   353
#define CTRY_ICELAND                   354
#define CTRY_FINLAND                   358
#define CTRY_BULGARIA                  359
#define CTRY_UKRAINE                   380
#define CTRY_CROATIA                   385
#define CTRY_SLOVENIA                  386
#define CTRY_CZECH_REPUBLIC            421
#define CTRY_SLOVAK_REPUBLIC           422
#define CTRY_HONG_KONG                 852
//#define CTRY_TAIWAN                  886

ULONG OSLibQueryCountry();

void OSLibSetBeginLibpath(char *lpszBeginlibpath);
void OSLibQueryBeginLibpath(char *lpszBeginlibpath, int size);

ULONG OSLibImSetMsgQueueProperty( ULONG hmq, ULONG ulFlag );

#ifdef __cplusplus
}
#endif

#endif
