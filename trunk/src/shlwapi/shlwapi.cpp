/* $Id: shlwapi.cpp,v 1.4 2000-01-31 22:50:51 phaller Exp $ */

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
#include <odin.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"
#include "crtdll.h"

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>

ODINDEBUGCHANNEL(SHLWAPI)



/*
   shlwapi functions that have found their way in because most of
   shlwapi is unimplemented and doesn't have a home.

   FIXME: move to a more appropriate file( when one exists )
*/

 /* SHGetValue: Gets a value from the registry */

ODINFUNCTION6(DWORD,SHGetValueA,HKEY,     hkey,
                                LPCSTR,   pSubKey,
                                LPCSTR,   pValue,
                                LPDWORD,  pwType,
                                LPVOID,   pvData,
                                LPDWORD,  pbData)
{
    dprintf(("(%p),stub!\n", pSubKey));

	return ERROR_SUCCESS;  /* return success */
}

ODINFUNCTION6(DWORD,SHGetValueW,HKEY,     hkey,
                                LPCWSTR,  pSubKey,
                                LPCWSTR,  pValue,
                                LPDWORD,  pwType,
                                LPVOID,   pvData,
                                LPDWORD,  pbData)
{
    dprintf(("(%p),stub!\n", pSubKey));

	return ERROR_SUCCESS;  /* return success */
}

/* gets a user-specific registry value. */

ODINFUNCTION8(LONG,SHRegGetUSValueA,LPCSTR,   pSubKey,
                                    LPCSTR,   pValue,
                                    LPDWORD,  pwType,
                                    LPVOID,   pvData,
                                    LPDWORD,  pbData,
                                    BOOL,     fIgnoreHKCU,
                                    LPVOID,   pDefaultData,
                                    DWORD,    wDefaultDataSize)
{
    FIXME("(%p),stub!\n", pSubKey);

	return ERROR_SUCCESS;  /* return success */
}

ODINFUNCTION8(LONG,SHRegGetUSValueW,LPCWSTR, pSubKey,
                                    LPCWSTR, pValue,
                                    LPDWORD, pwType,
                                    LPVOID,  pvData,
                                    LPDWORD, pbData,
                                    BOOL,    flagIgnoreHKCU,
                                    LPVOID,  pDefaultData,
                                    DWORD,   wDefaultDataSize)
{
    dprintf(("(%p),stub!\n", pSubKey));

	return ERROR_SUCCESS;  /* return success */
}


/*****************************************************************************
 * Name      : DWORD SHRegGetBoolUSValueA
 * Purpose   : unknown
 * Parameters: unknown
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.SHRegGetBoolUSValueA
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION5(LONG,    SHRegGetBoolUSValueA,
              LPCSTR,  pSubKey,
              DWORD,   arg2,
              DWORD,   arg3,
              DWORD,   arg4,
              DWORD,   arg5)
{
  char  szBuffer[264];
  int   iLength;
  
  dprintf(("(%p),stub!\n", pSubKey));

  return ERROR_SUCCESS;  /* return success */
}


/*****************************************************************************
 * Name      : DWORD SHRegGetBoolUSValueW
 * Purpose   : unknown
 * Parameters: unknown
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.SHRegGetBoolUSValueW
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 23:02]
 *****************************************************************************/

ODINFUNCTION5(LONG,    SHRegGetBoolUSValueW,
              LPCWSTR, pSubKey,
              DWORD,   arg2,
              DWORD,   arg3,
              DWORD,   arg4,
              DWORD,   arg5)
{
  char  szBuffer[264];
  int   iLength;
  
  dprintf(("(%p),stub!\n", pSubKey));

  return ERROR_SUCCESS;  /* return success */
}


/*****************************************************************************
 * Name      : LPSTR PathSkipRootA
 * Purpose   : Parses a path, ignoring the drive letter or UNC server/share path parts.
 * Parameters: LPCSTR pszPath
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.PathSkipRootA
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 2000/01/31 23:02]
 *****************************************************************************/

ODINFUNCTION1(LPSTR, PathSkipRootA, LPCSTR, pszPath)
{
  // check if "driveletter:\"
  if (pszPath[1] == ':')
    return (LPSTR)(pszPath + 2);
  
  // check if UNC-style path
  if ( (pszPath[0] == '\\') &&
      (pszPath[1] == '\\') )
  {
    LPSTR pszTemp = strchr(pszPath + 2, '\\');
    if (NULL != pszTemp)
      // return root part, skip server/share
      return (LPSTR)pszTemp++;
    else
      // UNC syntax validation, return pszPath
      return (LPSTR)pszTemp;
  }
  
  // else ...
  return (LPSTR)pszPath;
}


/*****************************************************************************
 * Name      : LPWSTR PathSkipRootW
 * Purpose   : Parses a path, ignoring the drive letter or UNC server/share path parts.
 * Parameters: LPCWSTR pszPath
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.PathSkipRootW
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 2000/01/31 23:02]
 *****************************************************************************/

ODINFUNCTION1(LPWSTR, PathSkipRootW, LPCWSTR, pszPath)
{
  dprintf(("not implemented"));
  
  return (LPWSTR)pszPath;
}
