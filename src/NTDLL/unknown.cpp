/* $Id: unknown.cpp,v 1.1 1999-06-28 08:08:19 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 * NT basis DLL
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


/*****************************************************************************
 * Name      : NTSTATUS NtAllocateUuids
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.59
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

NTSTATUS WIN32API NtAllocateUuids(DWORD x1,
                                  DWORD x2,
                                  DWORD x3,
                                  DWORD x4)
{
  dprintf(("NTDLL: NtAllocateUuids(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


