/* $Id: oslibmisc.h,v 1.5 2000-06-23 19:02:38 sandervl Exp $ */
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

char *OSLibStripPath(char *path);

ULONG OSLibiGetModuleHandleA(char *pszModule);
ULONG OSLibQueryModuleHandle(char *modname);

void  OSLibWait(ULONG msec);

ULONG OSLibAllocSel(ULONG size, USHORT *selector);
ULONG OSLibFreeSel(USHORT selector);
PVOID OSLibSelToFlat(USHORT selector);

#define TIB_STACKTOP 	0
#define TIB_STACKLOW 	1

ULONG OSLibGetTIB(int tiboff);

#define PIB_TASKHNDL	0
#define PIB_TASKTYPE    1

#define TASKTYPE_PM	0
#define TASKTYPE_VIO	1

ULONG OSLibGetPIB(int piboff);

ULONG OSLibAllocThreadLocalMemory(int nrdwords);

ULONG OSLibWinInitialize();
ULONG OSLibWinQueryMsgQueue(ULONG hab);

//#define CC_NONE                        0
#define CC_USA                         1
#define CC_CANADA                      2
#define CC_LATIN_AMERICA               3
#define CC_GREECE                     30
#define CC_NETHERLANDS                31
#define CC_BELGIUM                    32
#define CC_FRANCE                     33
#define CC_SPAIN                      34
#define CC_ITALY                      39
#define CC_SWITZERLAND                41
#define CC_AUSTRIA                    43
#define CC_UNITED_KINGDOM             44
#define CC_DENMARK                    45
#define CC_SWEDEN                     46
#define CC_NORWAY                     47
#define CC_POLAND                     48
#define CC_WEST_GERMANY               49
#define CC_MEXICO                     52
#define CC_BRAZIL                     55
#define CC_AUSTRALIA                  61
#define CC_NEW_ZEALAND                64
#define CC_JAPAN                      81
#define CC_KOREA                      82
#define CC_CHINA                      86
#define CC_TAIWAN                     88
#define CC_TURKEY                     90
#define CC_PORTUGAL                  351
#define CC_LUXEMBOURG                352
#define CC_ICELAND                   354
#define CC_FINLAND                   358
#define CC_CZECH_REPUBLIC            421
#define CC_SLOVAK_REPUBLIC           422

ULONG OSLibQueryCountry();

#endif
