/* $Id: shlstub.cpp,v 1.4 2002-02-13 16:15:07 sandervl Exp $ */

/*
 * Win32 stub shlwapi APIs for OS/2
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#define HAVE_WCTYPE_H
#include <odin.h>

#define CINTERFACE

#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"
#include "crtdll.h"

#include <heapstring.h>
#include <misc.h>
#include <win/shell.h>
#include <win/winerror.h>


ODINDEBUGCHANNEL(SHLWAPI-SHLSTUB)


/*****************************************************************************
 * Name      : StrFromTimeIntervalA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

LPSTR WIN32API StrFromTimeIntervalA(LPVOID lp1, LPVOID lp2, LPVOID lp3, LPVOID lp4)
{
  dprintf(("not yet implemented"));

  return NULL;
}


/*****************************************************************************
 * Name      : StrFromTimeIntervalW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

LPWSTR WIN32API StrFromTimeIntervalW(LPVOID lp1, LPVOID lp2, LPVOID lp3, LPVOID lp4)
{
  dprintf(("not yet implemented"));

  return NULL;
}

/*****************************************************************************
 * Name      : AssocQueryStringA
 * Purpose   : Searches for and retreives a file or protocol association-related
 *             string from the registry.
 */
HRESULT WIN32API AssocQueryStringA( DWORD /*ASSOCF*/ flags, DWORD /*ASSOCSTR*/ str,
                                    LPCSTR pszAssoc, LPCSTR pszExtra,
                                    LPSTR pszOut, DWORD *pcchOut )
{
  dprintf(("SHLWAPI: AssocQueryStringA() not implemented\n"));
  return (E_NOTIMPL);
}

/*****************************************************************************
 * Name      : AssocQueryStringW
 * Purpose   : Searches for and retreives a file or protocol association-related
 *             string from the registry.
 */
HRESULT WIN32API AssocQueryStringW( DWORD /*ASSOCF*/ flags, DWORD /*ASSOCSTR*/ str,
                                    LPCWSTR pszAssoc, LPCWSTR pszExtra,
                                    LPWSTR pszOut, DWORD *pcchOut )
{
  dprintf(("SHLWAPI: AssocQueryStringW() not implemented\n"));
  return (E_NOTIMPL);
}
