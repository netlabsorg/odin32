/* $Id: reg_odin.cpp,v 1.1 2000-08-24 09:32:43 sandervl Exp $ */

/*
 * Win32 URL-handling APIs for OS/2
 *
 * Copyright 1999      Patrick Haller <phaller@gmx.net>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997 Uwe Bonnes
 * Copyright 1999 Jens Wiessner
 */


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#define HAVE_WCTYPE_H

#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"

#include <heapstring.h>
#include <misc.h>
#include <win\winerror.h>
#include "shlwapi_odin.h"


ODINDEBUGCHANNEL(SHLWAPI-REG)


/*****************************************************************************
 * Name      : DWORD SHRegCreateUSKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.594 SHLWAPI.SHRegCreateUSKeyA
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION5(DWORD,  SHRegCreateUSKeyA,
              LPSTR,  lpszKeyName,
              DWORD,  arg1,
              DWORD,  arg2,
              DWORD,  arg3,
              DWORD,  arg4)
{
  dprintf(("not implemented\n"));
  return 0;
}


/*****************************************************************************
 * Name      : DWORD SHRegCreateUSKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.595 SHLWAPI.SHRegCreateUSKeyW
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION5(DWORD,  SHRegCreateUSKeyW,
              LPWSTR, lpszKeyName,
              DWORD,  arg1,
              DWORD,  arg2,
              DWORD,  arg3,
              DWORD,  arg4)
{
  char szBuffer[256];

  // convert unicode to ascii
  if (0 == WideCharToMultiByte(0,
                               0,
                               lpszKeyName,
                               -1,
                               szBuffer,
                               sizeof(szBuffer),
                               0,
                               0))
    return GetLastError();

  return SHRegCreateUSKeyA(szBuffer,
                         arg1,
                         arg2,
                         arg3,
                         arg4);
}



/*
   shlwapi functions that have found their way in because most of
   shlwapi is unimplemented and doesn't have a home.

   FIXME: move to a more appropriate file( when one exists )
*/


/* gets a user-specific registry value. */


/*****************************************************************************
 * Name      : DWORD SHRegGetBoolUSValueA
 * Purpose   : unknown
 * Parameters: unknown
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.SHRegGetBoolUSValueA
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION4(BOOL,    SHRegGetBoolUSValueA,
              LPCSTR,  pszSubKey,
              LPCSTR,  pszValue,
              BOOL,    fIgnoreHKCU,
              BOOL,    fDefault)
{
  char  szBuffer[260];
  DWORD dwLength = sizeof(szBuffer);
  LONG  rc;

  dprintf(("subkey=%s, value=%s\n",
           pszSubKey,
           pszValue));

  rc = SHRegGetUSValueA(pszSubKey,
                        pszValue,
                        &fDefault,
                        szBuffer,
                        &dwLength,
                        fIgnoreHKCU,
                        fDefault ? "YES" : "NO",
                        fDefault ? 4 : 3);
  if (rc != ERROR_SUCCESS)
    return rc;

  if (lstrcmpiA("YES",
                szBuffer) == 0)
    return 1;
  else
  if (lstrcmpiA("TRUE",
                szBuffer) == 0)
    return 1;
  else
  if (lstrcmpiA("NO",
                szBuffer) == 0)
    return 0;
  else
  if (lstrcmpiA("FALSE",
                szBuffer) == 0)
    return 0;


  return ERROR_SUCCESS;  /* return success */
}


/*****************************************************************************
 * Name      : DWORD SHRegGetBoolUSValueW
 * Purpose   : unknown
 * Parameters: unknown
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.SHRegGetBoolUSValueW
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION4(BOOL,    SHRegGetBoolUSValueW,
              LPCWSTR, pszSubKey,
              LPCWSTR, pszValue,
              BOOL,    fIgnoreHKCU,
              BOOL,    fDefault)
{
  char  szBuffer[264];
  int   iLength;

  dprintf(("(%p),stub!\n", pszSubKey));

  return ERROR_SUCCESS;  /* return success */
}


/*****************************************************************************
 * Name      : DWORD SHRegSetUSValueA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.SHRegSetUSValueA SHLWAPI.615
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION6(LONG,    SHRegSetUSValueA,
              HKEY,    hKey,
              DWORD,   arg2,
              DWORD,   arg3,
              DWORD,   arg4,
              DWORD,   arg5,
              DWORD,   arg6)
{
  LONG rc;

#if 0
  rc = SHRegCreateUSKeyA(hKey,
                         3,
                         0,
                         &hKey);
  if (rc == ERROR_SUCCESS)
  {
    rc = SHRegWriteUSValueA(hKey,
                            arg2,
                            arg3,
                            arg4,
                            arg5,
                            arg6);
    SHRegCloseUSKey(hKey);
  }
#endif

  return rc;
}




/*****************************************************************************
 * Name      : DWORD SHRegSetUSValueW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.SHRegSetUSValueW SHLWAPI.616
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION6(LONG,    SHRegSetUSValueW,
              HKEY,    hKey,
              DWORD,   arg2,
              DWORD,   arg3,
              DWORD,   arg4,
              DWORD,   arg5,
              DWORD,   arg6)
{
  LONG rc;

#if 0
  rc = SHRegCreateUSKeyW(hKey,
                         3,
                         0,
                         &hKey);
  if (rc == ERROR_SUCCESS)
  {
    rc = SHRegWriteUSValueW(hKey,
                            arg2,
                            arg3,
                            arg4,
                            arg5,
                            arg6);
    SHRegCloseUSKey(hKey);
  }
#endif

  return rc;
}


