/* $Id: mailslot.cpp,v 1.3 2002-02-09 12:45:13 sandervl Exp $
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
#include <HandleManager.h>
#include <unicode.h>

#include "hmhandle.h"
#include "hmmailslot.h"

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

HANDLE WIN32API CreateMailslotA(LPCSTR lpName, DWORD nMaxMessageSize,
                                DWORD lReadTimeout,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  PHMHANDLE   pHandle;
  BOOL        rc;                                     /* API return code */
  HMMailslotClass *pDeviceHandler;

  SetLastError(ERROR_SUCCESS);

  pHandle = HMHandleGetFreePtr(HMTYPE_MAILSLOT);                         /* get free handle */
  if (pHandle == NULL)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return INVALID_HANDLE_VALUE;
  }

  /* call the device handler */
  pDeviceHandler = (HMMailslotClass *)pHandle->pDeviceHandler;
  rc = pDeviceHandler->CreateMailslotA(&pHandle->hmHandleData,
                                       lpName, nMaxMessageSize,
                                       lReadTimeout, lpSecurityAttributes);

  if (rc == FALSE)     /* oops, creation failed within the device handler */
  {
      HMHandleFree(pHandle->hmHandleData.hWin32Handle);
      return INVALID_HANDLE_VALUE;                                           /* signal error */
  }

  return pHandle->hmHandleData.hWin32Handle;
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

BOOL WIN32API GetMailslotInfo(HANDLE  hMailslot,
                              LPDWORD lpMaxMessageSize,
                              LPDWORD lpNextSize,
                              LPDWORD lpMessageCount,
                              LPDWORD lpReadTimeout)
{
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hMailslot);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->GetMailslotInfo(&pHMHandle->hmHandleData,
                                                        lpMaxMessageSize,
                                                        lpNextSize,
                                                        lpMessageCount,
                                                        lpReadTimeout);
  return (lpResult);                                  /* deliver return code */
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
BOOL SetMailslotInfo(HANDLE hMailslot,
                     DWORD  dwReadTimeout)
{
  BOOL      fResult;
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hMailslot);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  fResult = pHMHandle->pDeviceHandler->SetMailslotInfo(&pHMHandle->hmHandleData,
                                                       dwReadTimeout);

  return fResult;                                  /* deliver return code */
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

HANDLE WIN32API CreateMailslotW(LPCWSTR lpName, DWORD nMaxMessageSize,
                                DWORD lReadTimeout,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  HANDLE rc;
  char  *astring;

  astring = UnicodeToAsciiString((LPWSTR)lpName);
  rc = CreateMailslotA(astring, nMaxMessageSize, lReadTimeout, lpSecurityAttributes);
  FreeAsciiString(astring);
  return(rc);
}


