/* $Id: unknown.cpp,v 1.3 1999-08-18 19:35:31 phaller Exp $ */

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


/*****************************************************************************
 * Name      : _alldiv
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD CDECL OS2_alldiv(DWORD x1,
                       DWORD x2)
{
  dprintf(("NTDLL: _alldiv(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (x1 / x2);
}


/*****************************************************************************
 * Name      : _allmul
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD CDECL OS2_allmul(DWORD x1,
                       DWORD x2)
{
  dprintf(("NTDLL: _allmul(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (x1 * x2);
}


/*****************************************************************************
 * Name      : _chkstk
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

void CDECL OS2_chkstk(void)
{
  dprintf(("NTDLL: _chkstk(%08xh,%08xh) not implemented.\n"));
}


/*****************************************************************************
 * Name      : RtlLargeIntegerToChar
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API RtlLargeIntegerToChar(LARGE_INTEGER li,
                                     LPWSTR        retstr,
                                     DWORD         retlen)
{
  dprintf(("NTDLL: RtlLargeIntegerToChar(%08xh,%08xh,%08xh) not implemented.\n",
           li,
           retstr,
           retlen));

  return 0;
}


/*****************************************************************************
 * Name      : RtlQueryTimeZoneInformation
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API RtlQueryTimeZoneInformation(DWORD x1,
                                           DWORD x2,
                                           DWORD x3)
{
  dprintf(("NTDLL: RtlQueryTimeZoneInformation(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 0;
}







