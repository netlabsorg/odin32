/* $Id: shlwapi.cpp,v 1.10 2001-04-04 09:05:50 sandervl Exp $ */

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
 *
 * Two functions (WINE_StringFromCLSID and StringFromGUID2) are directly
 * borrowed from ole32/clsid.cpp. This is to avoid the direct dependency
 * between SHLWAPI.DLL and the OLE32.DLL.
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
#include <unicode.h>
#include <win\shell.h>
#include <win\winerror.h>

// import OLE support
#include <win/wtypes.h>
#define OLE_OK 0


ODINDEBUGCHANNEL(SHLWAPI)

#include "shlwapi.h"


/*
 * This enables procedures to automatically take care or
 * required unicode conversion or not.
 */

BOOL VERSION_OsIsUnicode(void)
{
 static version = 0;

  if(version == 0) {
    	version = GetVersion();
  }
  /* if high-bit of version is 0, we are emulating NT */
  return !(version & 0x80000000);
}


// ----------------------------------------------------------------------
// WINE_StringFromCLSID
// ----------------------------------------------------------------------
HRESULT WINAPI WINE_StringFromCLSID(const CLSID *rclsid, LPSTR idstr)
{
//    dprintf(("OLE32: WINE_StringFromCLSID"));

    if (rclsid == NULL)
    {
	dprintf(("       clsid: (NULL)"));
	*idstr = 0;
	return E_FAIL;
    }

    // Setup new string...
    sprintf(idstr, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
	    rclsid->Data1,
	    rclsid->Data2,
	    rclsid->Data3,
	    rclsid->Data4[0],
	    rclsid->Data4[1],
	    rclsid->Data4[2],
	    rclsid->Data4[3],
	    rclsid->Data4[4],
	    rclsid->Data4[5],
	    rclsid->Data4[6],
	    rclsid->Data4[7]);

//    dprintf(("       clsid: %s", idstr));

    return OLE_OK;
}

// ----------------------------------------------------------------------
// StringFromGUID2
// ----------------------------------------------------------------------
int WINAPI StringFromGUID2(REFGUID rguid, LPOLESTR lpsz, int cbMax)
{
// NB cbMax is a CHARACTER count not a BYTE count... :-)
    char 	tmp[50];
    size_t	strLen;

    // Setup new string...
    WINE_StringFromCLSID(rguid, tmp);

    strLen = (strlen(tmp) + 1);
    if (strLen > cbMax)
	strLen = cbMax;

    AsciiToUnicodeN(tmp, lpsz, strLen);

    return strLen;  // Num CHARACTERS including 0 terminator
}


/*****************************************************************************
 * Name      : DllGetVersion
 * Purpose   : Return version information about the DLL used
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.446
 * Status    : UNTESTED 
 *
 * Author    : 
 *****************************************************************************/

typedef struct tagDLLVERSION
{
  DWORD dwLength; // == 0x14
  DWORD dwMajorVersion;
  DWORD dwMinorVersion;
  DWORD dwRevision;
  DWORD dwBuildNumber;
} DLLVERSION, *LPDLLVERSION;


ODINFUNCTION1(DWORD,        DllGetVersion,
              LPDLLVERSION, lpBuffer)
{
  if (lpBuffer == NULL)
    return E_INVALIDARG; // error code: invalid parameters

  if (IsBadWritePtr(lpBuffer,
                    20))
    return E_INVALIDARG;
  
  if (lpBuffer->dwLength != sizeof(DLLVERSION))
    return E_INVALIDARG;
      
  // our current version is 5.0.2314.1000
  lpBuffer->dwMajorVersion = 5;
//lpBuffer->dwMinorVersion = 0;  // @@@PH not touched in windows code ?
  lpBuffer->dwRevision     = 2314;
  lpBuffer->dwBuildNumber  = 1;
  
  return ERROR_SUCCESS;
}
