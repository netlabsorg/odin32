/* $Id: ntdll.c,v 1.5 2003-01-23 20:21:59 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * @(#) ntdll.cpp  1.0.1   1999/05/08 SvL: Changes for compilation with Wine headers
 *                 1.0.0   1998/05/20 PH Start from WINE/NTDLL.C
 *
 * NT basis DLL
 *
 * Copyright 1996 Marcus Meissner
 * Copyright 1998 Patrick Haller (adapted for win32os2)
 */

 /* Changes to the original NTDLL.C from the WINE project

  - includes replaced by the win32os2 standard includes
  - replaced WINAPI by WIN32API
  - moved in some string functions
  - replaced HANDLE32 by HANDLE
  - lstrlen32A -> OS2lstrlenA
  - lstrlen32W -> OS2lstrlenW
*/

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <windows.h>
#include <winnt.h>
#include <ntdef.h>
#ifndef __EMX__
#include <builtin.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <misc.h>
#include "unicode.h"

#include "ntdll.h"
#include <ntdllsec.h>
#include <versionos2.h>
/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

#define NTSTATUS DWORD

//SvL: per process heap for NTDLL
HANDLE NTDLL_hHeap = 0;

PROCESSTHREAD_SECURITYINFO ProcSecInfo = {0};

/*****************************************************************************
 * Name      : DbgPrint
 * Purpose   : print a debug line to somewhere?
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.21
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/
void __cdecl DbgPrint(LPCSTR lpcstrFormat,LPVOID args)
{
  UCHAR   szBuffer[600]; // as in original NTDLL.DLL
  int     rc;

  rc = wvsnprintfA((LPSTR)szBuffer,
                   sizeof(szBuffer),
                   lpcstrFormat,
                   (va_list)args);

  dprintf(("NTDLL: DbgPrint[%s]\n",
           szBuffer));

  //@@@PH raise debug exception if running in debugger
}

//******************************************************************************
BOOL WIN32API NTDLL_LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    dprintf(("NTDLL_LibMain: 0x%x 0x%lx %p\n", hinstDLL, fdwReason, lpvReserved));

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
    {
        SID_IDENTIFIER_AUTHORITY sidIdAuth = {0};

     	NTDLL_hHeap = HeapCreate(0, 0x10000, 0);

	ProcSecInfo.dwType = SECTYPE_PROCESS | SECTYPE_INITIALIZED;
        RtlAllocateAndInitializeSid(&sidIdAuth, 1, 0, 0, 0, 0, 0, 0, 0, 0, &ProcSecInfo.SidUser.User.Sid);
	ProcSecInfo.SidUser.User.Attributes = 0; //?????????

        ProcSecInfo.pTokenGroups = (TOKEN_GROUPS*)Heap_Alloc(sizeof(TOKEN_GROUPS));
	ProcSecInfo.pTokenGroups->GroupCount = 1;
        RtlAllocateAndInitializeSid(&sidIdAuth, 1, 0, 0, 0, 0, 0, 0, 0, 0, &ProcSecInfo.PrimaryGroup.PrimaryGroup);
	ProcSecInfo.pTokenGroups->Groups[0].Sid = ProcSecInfo.PrimaryGroup.PrimaryGroup;
	ProcSecInfo.pTokenGroups->Groups[0].Attributes = 0; //????

//        pPrivilegeSet   = NULL;
//        pTokenPrivileges= NULL;
//        TokenOwner      = {0};
//        DefaultDACL     = {0};
//        TokenSource     = {0};
        ProcSecInfo.TokenType = TokenPrimary;
	break;
    }
    case DLL_PROCESS_DETACH:
        HeapDestroy(NTDLL_hHeap);
        NTDLL_hHeap = 0;
	break;
    case DLL_THREAD_ATTACH:
	break;
    case DLL_THREAD_DETACH:
	break;
    default:
	break;
    }
    return TRUE;
}
