/* $Id: url_odin.cpp,v 1.4 2002-02-06 20:18:30 sandervl Exp $ */

/*
 * Win32 Lightweight SHELL32 API for OS/2
 *
 * 2000/04/20 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) url.cpp                 1.0.0   2000/04/20 PH Start from scratch
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

#include <winreg.h>

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>

#include "shlwapi_odin.h"

ODINDEBUGCHANNEL(SHLWAPI-URL)


/*****************************************************************************
 * Name      : UrlCreateFromPath
 * Purpose   : Takes a DOS path and converts it to a canonicalized URL.
 * Parameters: pszPath    Pointer to the string with the DOS path.
 *             pszUrl     Value used to return the URL.
 *             pcchPath   Length of pszUrl.
 *             dwReserved Reserved. Set this parameter to NULL.
 * Variables :
 * Result    : Returns S_FALSE if pszPath is already in URL format. In this
 *             case, pszPath will simply be copied to pszUrl. Otherwise, it
 *             returns S_OK if successful or a standard OLE error value if not.
 * Remark    : SHLWAPI.
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION4(HRESULT,UrlCreateFromPathA,
              LPCSTR, pszPath,
              LPSTR,  pszUrl,
              LPDWORD,pcchUrl,
              DWORD,  dwReserved)
{
  dprintf(("not implemented."));

  return S_FALSE;
}


/**
 * @status      stub
 */
ODINFUNCTION4(HRESULT,UrlCreateFromPathW,
              LPCWSTR,pszPath,
              LPWSTR, pszUrl,
              LPDWORD,pcchUrl,
              DWORD,  dwReserved)
{
  dprintf(("not implemented."));

  return S_FALSE;
}








/**
 * @status      stub
 */
ODINFUNCTION3(HRESULT,UrlHashW,
              LPCWSTR,pszURL,
              LPBYTE, pbHash,
              DWORD,  cbHash)
{
  dprintf(("not implemented."));

  return S_OK;
}


