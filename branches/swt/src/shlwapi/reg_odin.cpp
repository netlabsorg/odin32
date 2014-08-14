/* $Id: reg_odin.cpp,v 1.3 2002-02-06 20:18:28 sandervl Exp $ */

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
#include "winuser.h"
#include "winreg.h"
#include "wingdi.h"

#include <heapstring.h>
#include <misc.h>
#include <win/winerror.h>
#include "shlwapi_odin.h"
#define NO_SHLWAPI_STREAM
#include <shlwapi.h>


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

ODINFUNCTION5(LONG,  SHRegCreateUSKeyA,
              LPCSTR,  lpszKeyName,
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

ODINFUNCTION5(LONG,   SHRegCreateUSKeyW,
              LPCWSTR,  lpszKeyName,
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
              LPCSTR,   lpValue,
              SHREGDEL_FLAGS,   dwFlags)
{
  dprintf(("not implemented\n"));

  LONG rc = RegDeleteValueA(hUSKey,
                            (LPSTR)lpValue);
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
              HUSKEY,   hKey,
              LPCWSTR,  lpValue,
              SHREGDEL_FLAGS,   dwFlags)
{
  dprintf(("not implemented\n"));

  LONG rc = RegDeleteValueW(hKey,
                            (LPWSTR)lpValue);
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
              LPCSTR,   lpszSubkey,
              SHREGDEL_FLAGS,   dwFlags)
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
              LPCWSTR,  lpszSubkey,
              SHREGDEL_FLAGS,   dwFlags)
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



