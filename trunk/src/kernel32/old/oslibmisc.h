/* $Id: oslibmisc.h,v 1.1 1999-09-15 23:32:59 sandervl Exp $ */
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

void OSLibSetExitList(unsigned long handler);
void OSLibClearExitList();
void OSLibRemoveExitList(unsigned long handler);

char *OSLibGetDllName(ULONG hModule);

char *OSLibStripPath(char *path);

ULONG OSLibiGetModuleHandleA(char *pszModule);
ULONG OSLibQueryModuleHandle(char *modname);

ULONG OSLibGetResourceSize(HMODULE hinstance, int id);
ULONG OSLibGetResource(HMODULE hinstance, int id, char *destbuf, int bufLength);

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

#endif
