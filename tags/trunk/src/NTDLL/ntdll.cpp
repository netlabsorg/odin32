/* $Id: ntdll.cpp,v 1.3 1999-11-09 09:30:20 phaller Exp $ */

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

#include <os2win.h>
#include <winnt.h>
#include <ntdef.h>
#include <builtin.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "misc.h"
#include "unicode.h"

#include "ntdll.h"


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

#define NTSTATUS DWORD


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
