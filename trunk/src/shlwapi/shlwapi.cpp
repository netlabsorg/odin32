/* $Id: shlwapi.cpp,v 1.5 2000-03-30 15:40:43 cbratschi Exp $ */

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

/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_8,DWORD,x)
{
  dprintf(("not implemented"));

  return 0;
}

/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_9,DWORD,x)
{
  dprintf(("not implemented"));

  return 0;
}

/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_10,DWORD,x)
{
  dprintf(("not implemented"));

  return 0;
}

/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_16,DWORD,x)
{
  dprintf(("not implemented, explorer.exe will trap now"));

  return 0;
}

/*****************************************************************************
 * Name      : PathStripToRootA
 * Purpose   : return root (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(LPSTR,PathStripToRootA,LPSTR,pszPath)
{
  dprintf(("not implemented"));

  return NULL;
}

/*****************************************************************************
 * Name      : PathStripToRootW
 * Purpose   : return root (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(LPSTR,PathStripToRootW,LPSTR,pszPath)
{
  dprintf(("not implemented"));

  return NULL;
}

/*****************************************************************************
 * Name      : StrToIntA
 * Purpose   : convert string to integer (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(INT,StrToIntA,LPSTR,pszPath)
{
  dprintf(("not implemented"));

  return NULL;
}

/*************************************************************************
 * PathFindExtension [SHELL32.31]
 *
 * NOTES
 *     returns pointer to last . in last pathcomponent or at \0.
 */
LPCSTR WINAPI PathFindExtensionA(LPCSTR path)
{       LPCSTR   lastpoint = NULL;
        TRACE("%p %s\n",path,path);
        while (*path)
        { if (*path=='\\'||*path==' ')
            lastpoint=NULL;
          if (*path=='.')
            lastpoint=path;
          path++;
        }
        return lastpoint?lastpoint:path;
}
LPCWSTR WINAPI PathFindExtensionW(LPCWSTR path)
{       LPCWSTR   lastpoint = NULL;
        TRACE("(%p %s)\n",path,debugstr_w(path));
        while (*path)
        { if (*path==(WCHAR)'\\'||*path==(WCHAR)' ')
            lastpoint=NULL;
          if (*path==(WCHAR)'.')
            lastpoint=path;
          path++;
        }
        return lastpoint?lastpoint:path;
}

