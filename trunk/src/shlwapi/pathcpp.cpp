/* $Id: pathcpp.cpp,v 1.2 2000-05-19 12:10:04 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Path Functions
 *
 * Many of this functions are in SHLWAPI.DLL also
 *
 * Corel WINE 20000324 level (without CRTDLL_* calls)
 */

/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <wcstr.h>
#define HAVE_WCTYPE_H

#include "debugtools.h"

#include <winreg.h>

#include <heapstring.h>

ODINDEBUGCHANNEL(SHLWAPI-SHELLPATH)

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

//SvL: NT seems to concatenate both strings (not tested very well)
ODINFUNCTION2(BOOL, PathAddExtensionA, LPSTR, pszPath, LPSTR, pszExtension)
{
  lstrcatA(pszPath, pszExtension);
  return 1;
}

ODINFUNCTION2(BOOL, PathAddExtensionW, LPWSTR, pszPath, LPWSTR, pszExtension)
{
  lstrcatW(pszPath, pszExtension);
  return 1;
}

