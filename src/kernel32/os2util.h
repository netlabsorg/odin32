/* $Id: os2util.h,v 1.4 1999-06-19 13:57:51 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Misc util. procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Peter FitzSimmons
 * Copyright 1998 Patrick Haller
 *
 */
#ifndef __OS2UTIL_H__
#define __OS2UTIL_H__

#ifdef _OS2WIN_H
#include <winos2def.h>
#else
#include <win32type.h>
#endif

#ifdef __cplusplus
      extern "C" {
#endif

void OS2SetExitList(unsigned long handler);
void OS2ClearExitList();
void OS2RemoveExitList(unsigned long handler);

char *OS2GetDllName(ULONG hModule);

HMODULE OS2iGetModuleHandleA(PSZ pszModule);
HMODULE OS2QueryModuleHandle(char *modname);

ULONG OS2GetResourceSize(HMODULE hinstance, int id);
BOOL  OS2GetResource(HMODULE hinstance, int id, char *destbuf, int bufLength);

void  OS2Wait(ULONG msec);


BOOL  OS2AllocSel(ULONG size, USHORT *selector);
BOOL  OS2FreeSel(USHORT selector);
PVOID OS2SelToFlat(USHORT selector);

#define TIB_STACKTOP 	0
#define TIB_STACKLOW 	1

ULONG OS2GetTIB(int tiboff);

#define PIB_TASKHNDL	0
#define PIB_TASKTYPE    1

#define TASKTYPE_PM	0
#define TASKTYPE_VIO	1

ULONG OS2GetPIB(int piboff);

#ifdef __cplusplus
      }
#endif

#endif
