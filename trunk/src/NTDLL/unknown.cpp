/* $Id: unknown.cpp,v 1.6 1999-08-19 20:43:19 phaller Exp $ */

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


/*****************************************************************************
 * Name      : NtAlertThread
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtAlertThread(DWORD x1,
                             DWORD x2)
{
  dprintf(("NTDLL: NtAlertThread(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return 0;
}


/*****************************************************************************
 * Name      : NtTestAlert
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtTestAlert(DWORD x1,
                             DWORD x2)
{
  dprintf(("NTDLL: NtTestAlert(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return 0;
}



/*****************************************************************************
 * Name      : NtWriteRequestData
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtWriteRequestData(DWORD x1,
                                  DWORD x2,
                                  DWORD x3,
                                  DWORD x4,
                                  DWORD x5)
{
  dprintf(("NTDLL: NtWriteRequestData(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return 0;
}


/*****************************************************************************
 * Name      : NtImpersonateClientOfPort
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtImpersonateClientOfPort(DWORD x1,
                                         DWORD x2,
                                         DWORD x3,
                                         DWORD x4)
{
  dprintf(("NTDLL: NtImpersonateClientOfPort(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/*****************************************************************************
 * Name      : NtReplyWaitReplyPort
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtReplyWaitReplyPort(DWORD x1,
                                    DWORD x2,
                                    DWORD x3,
                                    DWORD x4)
{
  dprintf(("NTDLL: NtReplyWaitReplyPort(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/*****************************************************************************
 * Name      : NtReadRequestData
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtReadRequestData(DWORD x1,
                                 DWORD x2,
                                 DWORD x3,
                                 DWORD x4,
                                 DWORD x5)
{
  dprintf(("NTDLL: NtReadRequestData(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return 0;
}


/*****************************************************************************
 * Name      : NtRequestPort
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtRequestPort(DWORD x1,
                             DWORD x2,
                             DWORD x3,
                             DWORD x4)
{
  dprintf(("NTDLL: NtRequestPort(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/*****************************************************************************
 * Name      : NtReplyPort
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API NtReplyPort(DWORD x1,
                           DWORD x2,
                           DWORD x3,
                           DWORD x4)
{
  dprintf(("NTDLL: NtReplyPort(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}



/**************************************************************************
 *                 NTDLL_chkstk                   [NTDLL.862]
 *                 NTDLL_alloca_probe             [NTDLL.861]
 * Glorified "enter xxxx".
 */
void CDECL OS2_chkstk(DWORD x1)
{
  dprintf(("NTDLL: _chkstk(%08xh) not implemented.\n",
           x1));

    //ESP_reg(context) -= EAX_reg(context);
}

void CDECL OS2_alloca_probe(DWORD x1)
{
  dprintf(("NTDLL: _alloca_probe(%08xh) not implemented.\n",
           x1));

    //ESP_reg(context) -= EAX_reg(context);
}


/*****************************************************************************
 * Name      : RtlConvertSidToUnicodeString
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API RtlConvertSidToUnicodeString(DWORD x1,
                                            DWORD x2,
                                            DWORD x3)
{
  dprintf(("NTDLL: RtlConvertSidToUnicodeString(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 0;
}


/*****************************************************************************
 * Name      : RtlRandom
 * Purpose   : unknown
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : NTDLL.466
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API RtlRandom(DWORD x1)
{
  dprintf(("NTDLL: RtlRandom(%08xh) not implemented.\n",
           x1));

  return rand();
}

