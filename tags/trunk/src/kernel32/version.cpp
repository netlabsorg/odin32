/* $Id: version.cpp,v 1.5 2001-12-08 12:01:28 sandervl Exp $ */

/*
 * Win32 compatibility file functions for OS/2
 *
 * Copyright 1998-2000 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * Based on Wine code: (misc\version.c)
 * Copyright 1997 Alexandre Julliard
 * Copyright 1997 Marcus Meissner
 * Copyright 1998 Patrik Stridvall
 * Copyright 1998 Andreas Mohr
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winnt.h>
#include <winnls.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <heapstring.h>

#include <misc.h>
#include "heap.h"
#include <handlemanager.h>
#include "wprocess.h"
#include "oslibdos.h"
#include <versionos2.h>
#include "profile.h"

#define DBG_LOCALLOG	DBG_version
#include "dbglocal.h"

typedef struct
{
    LONG           getVersion16;
    LONG           getVersion;
    OSVERSIONINFOA getVersionEx;
} VERSION_DATA;

static VERSION_DATA VersionData[WINVERSION_MAX] =
{
    // Windows 98
    {
        0x070A5F03,
	0xC0000A04,
	{
	    sizeof(OSVERSIONINFOA), 4, 10, 0x40A07CE,
	    VER_PLATFORM_WIN32_WINDOWS, "Win98"
	}
    },
    // Windows ME
    {
        0x07005F03, /* Assuming DOS 7 like the other Win9x */
        0xC0005A04,
        {
            sizeof(OSVERSIONINFOA), 4, 90, 0x45A0BB8,
            VER_PLATFORM_WIN32_WINDOWS, " "
        }
    },
    // Windows NT 4.0 (SP6)
    {
        0x05000A03,
        ODINNT_VERSION,
        {
            sizeof(OSVERSIONINFOA), ODINNT_MAJOR_VERSION, ODINNT_MINOR_VERSION, 
            ODINNT_BUILD_NR, VER_PLATFORM_WIN32_NT, ODINNT_CSDVERSION
        }
    },
    // Windows 2000 (SP2)
    {
        0x05005F03,
        0x08930005,
        {
            sizeof(OSVERSIONINFOA), 5, 0, 0x893,
            VER_PLATFORM_WIN32_NT, "Service Pack 2"
        }
    },
    // Windows XP
    {
        0x05005F03, /* Assuming DOS 5 like the other NT */
        0x0A280105,
        {
            sizeof(OSVERSIONINFOA), 5, 1, 0xA28,
            VER_PLATFORM_WIN32_NT, ""
        }
    }
};

static BOOL fCheckVersion = FALSE;
static int  winversion    = WINVERSION_NT40;

//******************************************************************************
//******************************************************************************
void WIN32API OdinSetVersion(ULONG version) 
{
    switch(version) {
    case WINVERSION_WIN98:
    case WINVERSION_WINME:
    case WINVERSION_NT40:
    case WINVERSION_WIN2000:
    case WINVERSION_WINXP:
        break;
    default: 
        DebugInt3();
        return;
    }
    winversion = version;
}
//******************************************************************************
//******************************************************************************
void CheckVersion()
{
 char szVersion[16];

  if(PROFILE_GetOdinIniString(PROFILE_WINVERSION_SECTION, PROFILE_WINVERSION_KEY, 
                              "", szVersion, sizeof(szVersion)-1) > 1) 
  {
	if(!stricmp(szVersion, PROFILE_WINVERSION_WIN98)) {
		winversion = WINVERSION_WIN98;
	}
	else
	if(!stricmp(szVersion, PROFILE_WINVERSION_WINME)) {
		winversion = WINVERSION_WINME;
	}
	else
	if(!stricmp(szVersion, PROFILE_WINVERSION_WIN2000)) {
		winversion = WINVERSION_WIN2000;
	}
	else
	if(!stricmp(szVersion, PROFILE_WINVERSION_WINXP)) {
		winversion = WINVERSION_WINXP;
	}
  }
  fCheckVersion = TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVersionExA(OSVERSIONINFOA *lpVersionInformation)
{
   dprintf(("KERNEL32: GetVersionExA %x", lpVersionInformation));

   if(lpVersionInformation == NULL)
   {
	dprintf(("ERROR: invalid parameter"));
	SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
   }
   if(lpVersionInformation->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA))
   {
	dprintf(("ERROR: buffer too small (%d != %d)", lpVersionInformation->dwOSVersionInfoSize, sizeof(OSVERSIONINFOA)));
	SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return(FALSE);
   }

   if(fCheckVersion == FALSE) {
	CheckVersion();
   }
   lpVersionInformation->dwMajorVersion = VersionData[winversion].getVersionEx.dwMajorVersion;
   lpVersionInformation->dwMinorVersion = VersionData[winversion].getVersionEx.dwMinorVersion;
   lpVersionInformation->dwBuildNumber  = VersionData[winversion].getVersionEx.dwBuildNumber;
   lpVersionInformation->dwPlatformId   = VersionData[winversion].getVersionEx.dwPlatformId;
   strcpy(lpVersionInformation->szCSDVersion, VersionData[winversion].getVersionEx.szCSDVersion );

   SetLastError(ERROR_SUCCESS);
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVersionExW(OSVERSIONINFOW *lpVersionInformation)
{
   dprintf(("KERNEL32: GetVersionExW %x", lpVersionInformation));

   if(lpVersionInformation == NULL)
   {
	dprintf(("ERROR: invalid parameter"));
	SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
   }
   if(lpVersionInformation->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW))
   {
	dprintf(("ERROR: buffer too small"));
	SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return(FALSE);
   }

   if(fCheckVersion == FALSE) {
	CheckVersion();
   }
   lpVersionInformation->dwMajorVersion = VersionData[winversion].getVersionEx.dwMajorVersion;
   lpVersionInformation->dwMinorVersion = VersionData[winversion].getVersionEx.dwMinorVersion;
   lpVersionInformation->dwBuildNumber  = VersionData[winversion].getVersionEx.dwBuildNumber;
   lpVersionInformation->dwPlatformId   = VersionData[winversion].getVersionEx.dwPlatformId;
   lstrcpyAtoW(lpVersionInformation->szCSDVersion, VersionData[winversion].getVersionEx.szCSDVersion);
   SetLastError(ERROR_SUCCESS);
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetVersion()
{
   dprintf(("KERNEL32: GetVersion"));
  // highword 0 = NT, lowword high byte major ver, low byte minor ver
/* @@@PH 98/04/04 MFC30 makes assumptions about process control block        */
/*                structures that lead to crashes if we don't identify as NT */

   if(fCheckVersion == FALSE) {
	CheckVersion();
   }
   return VersionData[winversion].getVersion;
}
//******************************************************************************
//******************************************************************************
