/* $Id: mailslot.cpp,v 1.2 2001-09-05 12:57:59 bird Exp $
 *
 * Win32 mailslot APIs
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <os2win.h>
#include <string.h>
#include "HandleManager.h"
#include <unicode.h>

#define DBG_LOCALLOG    DBG_mailslot
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-MAILSLOT)

/*****************************************************************************
 * Name      : HANDLE WIN32API CreateMailslotA
 * Purpose   : The CreateMailslot function creates a mailslot with the specified
 *             name and returns a handle that a mailslot server can use to
 *             perform operations on the mailslot. The mailslot is local to the
 *             computer that creates it. An error occurs if a mailslot with
 *             the specified name already exists.
 * Parameters: LPCSTR lpName              pointer to string for mailslot name
 *             DWORD nMaxMessageSize      maximum message size
 *             DWORD lReadTimeout         milliseconds before read time-out
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes pointer to security structure
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to
 *             the mailslot, for use in server mailslot operations.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE.
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

ODINFUNCTION4(HANDLE, CreateMailslotA, LPCSTR, lpName, DWORD, nMaxMessageSize,
                                       DWORD, lReadTimeout,
                                       LPSECURITY_ATTRIBUTES, lpSecurityAttributes)
{
  return HMCreateMailslotA(lpName, nMaxMessageSize, lReadTimeout, lpSecurityAttributes);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API CreateMailslotW
 * Purpose   : The CreateMailslot function creates a mailslot with the specified
 *             name and returns a handle that a mailslot server can use to
 *             perform operations on the mailslot. The mailslot is local to the
 *             computer that creates it. An error occurs if a mailslot with
 *             the specified name already exists.
 * Parameters: LPCWSTR lpName             pointer to string for mailslot name
 *             DWORD nMaxMessageSize      maximum message size
 *             DWORD lReadTimeout         milliseconds before read time-out
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes  pointer to security
 *                                                         structure
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to
 *             the mailslot, for use in server mailslot operations.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE.
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

ODINFUNCTION4(HANDLE, CreateMailslotW, LPCWSTR, lpName, DWORD, nMaxMessageSize,
                                       DWORD, lReadTimeout,
                                       LPSECURITY_ATTRIBUTES, lpSecurityAttributes)
{
  HANDLE rc;
  char  *astring;

  astring = UnicodeToAsciiString((LPWSTR)lpName);
  rc = HMCreateMailslotA(astring, nMaxMessageSize, lReadTimeout, lpSecurityAttributes);
  FreeAsciiString(astring);
  return(rc);
}

/*****************************************************************************
 * Name      : BOOL GetMailslotInfo
 * Purpose   : The GetMailslotInfo function retrieves information about the
 *             specified mailslot.
 * Parameters: HANDLE  hMailslot        mailslot handle
 *             LPDWORD lpMaxMessageSize address of maximum message size
 *             LPDWORD lpNextSize       address of size of next message
 *             LPDWORD lpMessageCount   address of number of messages
 *             LPDWORD lpReadTimeout    address of read time-out
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

ODINFUNCTION5(BOOL, GetMailslotInfo, HANDLE,  hMailslot,
                                    LPDWORD, lpMaxMessageSize,
                                    LPDWORD, lpNextSize,
                                    LPDWORD, lpMessageCount,
                                    LPDWORD, lpReadTimeout)
{
  return HMGetMailslotInfo(hMailslot, lpMaxMessageSize, lpNextSize,
                           lpMessageCount, lpReadTimeout);
}

/*****************************************************************************
 * Name      : BOOL SetMailslotInfo
 * Purpose   : The SetMailslotInfo function sets the time-out value used by the
 *             specified mailslot for a read operation.
 * Parameters: HANDLE hObject       handle to a mailslot object
 *             DWORD  dwReadTimeout read time-out
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

ODINFUNCTION2(BOOL, SetMailslotInfo,HANDLE, hMailslot, DWORD,  dwReadTimeout)
{
  return HMSetMailslotInfo(hMailslot, dwReadTimeout);
}
