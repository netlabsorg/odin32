/* $Id: shlwapi.cpp,v 1.12 2002-02-06 20:18:29 sandervl Exp $ */

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

#define CINTERFACE

#include "debugtools.h"
#include "winbase.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"
#include "crtdll.h"

#include <heapstring.h>
#include <misc.h>
#include <unicode.h>
#include <win/shell.h>
#include <win/winerror.h>

// import OLE support
#include <win/wtypes.h>
#define OLE_OK 0


ODINDEBUGCHANNEL(SHLWAPI)



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

