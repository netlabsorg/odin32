/* $Id: reg_odin.cpp,v 1.2 2001-08-30 19:19:57 phaller Exp $ */

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
              REGSAM,  AccessType,
              HUSKEY,  hRelativeUSKey,
              PHUSKEY, phNewUSKey,
              DWORD,   dwFlags)
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

ODINFUNCTION5(DWORD,   SHRegCreateUSKeyW,
              LPWSTR,  lpszKeyName,
              REGSAM,  AccessType,
              HUSKEY,  hRelativeUSKey,
              PHUSKEY, phNewUSKey,
              DWORD,   dwFlags)
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
                         AccessType,
                         hRelativeUSKey,
                         phNewUSKey,
                         dwFlags);
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
              LPCSTR,  lpszSubKeyName,
              LPCSTR,  lpszValueName,
              DWORD,   dwValueType,
              LPVOID,  lpValue,
              DWORD,   dwValueSize,
              DWORD,   dwFlags)
{
  dprintf(("not yet implemented"));
  
  LONG rc = 0;
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
              LPCWSTR, lpszSubKeyName,
              LPCWSTR, lpszValueName,
              DWORD,   dwValueType,
              LPVOID,  lpValue,
              DWORD,   dwValueSize,
              DWORD,   dwFlags)
{
  dprintf(("not yet implemented"));
  
  LONG rc = 0;
  return rc;
}


/*****************************************************************************
 * Name      : LONG SHRegCloseUSKey
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION1(LONG,    SHRegCloseUSKey,
              HUSKEY,  hUSKey)
{
  dprintf(("not implemented\n"));
  
  LONG rc = RegCloseKey(hUSKey);
  return rc;
}


/*****************************************************************************
 * Name      : LONG SHRegDeleteUSValueA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION3(LONG,    SHRegDeleteUSValueA,
              HUSKEY,  hUSKey,
              LPSTR,   lpValue,
              DWORD,   dwFlags)
{
  dprintf(("not implemented\n"));
  
  LONG rc = RegDeleteValueA(hUSKey,
                            lpValue);
  return rc;
}


/*****************************************************************************
 * Name      : LONG SHRegDeleteUSValueW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION3(LONG,    SHRegDeleteUSValueW,
              HKEY,    hKey,
              LPWSTR,  lpValue,
              DWORD,   dwFlags)
{
  dprintf(("not implemented\n"));
  
  LONG rc = RegDeleteValueW(hKey,
                            lpValue);
  return rc;
}


/*****************************************************************************
 * Name      : LONG SHDeleteOrphanKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION2(LONG,    SHDeleteOrphanKeyA,
              HKEY,    hKey,
              LPCSTR,  lpszSubkey)
{
  dprintf(("not implemented\n"));
  
  LONG rc = RegDeleteKeyA(hKey,
                          lpszSubkey);
  return rc;
}


/*****************************************************************************
 * Name      : LONG SHDeleteOrphanKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION2(LONG,    SHDeleteOrphanKeyW,
              HKEY,    hKey,
              LPWSTR,  lpszSubkey)
{
  dprintf(("not implemented\n"));
  
  LONG rc = RegDeleteKeyW(hKey,
                          lpszSubkey);
  return rc;
}


/*****************************************************************************
 * Name      : LONG SHRegDeleteEmptyUSKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION3(LONG,    SHRegDeleteEmptyUSKeyA,
              HUSKEY,  hUSKey,
              LPSTR,   lpszSubkey,
              DWORD,   dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegDeleteEmptyUSKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION3(LONG,    SHRegDeleteEmptyUSKeyW,
              HUSKEY,  hUSKey,
              LPWSTR,  lpszSubkey,
              DWORD,   dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegDeleteUSKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION3(LONG,    SHRegDeleteUSKeyA,
              HUSKEY,  hUSKey,
              LPSTR,   lpszSubkey,
              DWORD,   dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegDeleteUSKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION3(LONG,    SHRegDeleteUSKeyW,
              HUSKEY,  hUSKey,
              LPWSTR,  lpszSubkey,
              DWORD,   dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegEnumUSKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION5(LONG,    SHRegEnumUSKeyA,
              HUSKEY,  hUSKey,
              DWORD,   dwIndex,
              LPSTR,   lpszKeyName,
              LPDWORD, lpdwKeyNameSize,
              SHREGENUM_FLAGS, dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegEnumUSKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION5(LONG,    SHRegEnumUSKeyW,
              HUSKEY,  hUSKey,
              DWORD,   dwIndex,
              LPWSTR,  lpszKeyName,
              LPDWORD, lpdwKeyNameSize,
              SHREGENUM_FLAGS, dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegEnumUSValueA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION8(LONG,    SHRegEnumUSValueA,
              HUSKEY,  hUSKey,
              DWORD,   dwIndex,
              LPSTR,   lpszValueName,
              LPDWORD, lpdwValueNameSize,
              LPDWORD, lpdwValueType,
              LPVOID,  lpValue,
              LPDWORD, lpdwValueSize,
              SHREGENUM_FLAGS, dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegEnumUSValueW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION8(LONG,    SHRegEnumUSValueW,
              HUSKEY,  hUSKey,
              DWORD,   dwIndex,
              LPWSTR,  lpszValueName,
              LPDWORD, lpdwValueNameSize,
              LPDWORD, lpdwValueType,
              LPVOID,  lpValue,
              LPDWORD, lpdwValueSize,
              SHREGENUM_FLAGS, dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegQueryInfoUSKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION5(LONG,    SHRegQueryInfoUSKeyA,
              LPDWORD,  lpdwSubKeyNum,
              LPDWORD,  lpdwMaxSubKeyNameSize,
              LPDWORD,  lpdwValueNum,
              LPDWORD,  lpdwMaxValueNameSize,
              SHREGENUM_FLAGS, dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegQueryInfoUSKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION5(LONG,    SHRegQueryInfoUSKeyW,
              LPDWORD,  lpdwSubKeyNum,
              LPDWORD,  lpdwMaxSubKeyNameSize,
              LPDWORD,  lpdwValueNum,
              LPDWORD,  lpdwMaxValueNameSize,
              SHREGENUM_FLAGS, dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegWriteUSValueA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION6(LONG,     SHRegWriteUSValueA,
              HUSKEY,   hUSKey,
              LPCSTR,   lpszValueName,
              DWORD,    dwValueType,
              LPVOID,   lpValue,
              DWORD,    dwValueSize,
              DWORD,    dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : LONG SHRegWriteUSValueW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

ODINFUNCTION6(LONG,     SHRegWriteUSValueW,
              HUSKEY,   hUSKey,
              LPCWSTR,  lpszValueName,
              DWORD,    dwValueType,
              LPVOID,   lpValue,
              DWORD,    dwValueSize,
              DWORD,    dwFlags)
{
  dprintf(("not yet implemented"));
  return 0;
}
