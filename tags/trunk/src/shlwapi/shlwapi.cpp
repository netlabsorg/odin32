/* $Id: shlwapi.cpp,v 1.2 1999-12-28 23:17:04 sandervl Exp $ */

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

