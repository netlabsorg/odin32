/* $Id: os2util.h,v 1.3 1999-06-10 19:11:31 phaller Exp $ */

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

#ifdef __cplusplus
      }
#endif

#endif
