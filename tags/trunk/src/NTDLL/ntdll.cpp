/* $Id: ntdll.cpp,v 1.2 1999-08-18 21:45:13 phaller Exp $ */

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


