/* $Id: version.cpp,v 1.4 2001-10-15 17:10:55 sandervl Exp $ */

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
    LONG           getVersion;
    OSVERSIONINFOA getVersionEx;
} VERSION_DATA;

static VERSION_DATA VersionData[WINVERSION_MAX] =
{
    // Win98
    {
	0xC0000A04,
	{
	    sizeof(OSVERSIONINFOA), 4, 10, 0x40A07CE,
	    VER_PLATFORM_WIN32_WINDOWS, "Win98"
	}
    },
    // NT40
    {
        ODINNT_VERSION,
        {
            sizeof(OSVERSIONINFOA), ODINNT_MAJOR_VERSION, ODINNT_MINOR_VERSION, 
            ODINNT_BUILD_NR, VER_PLATFORM_WIN32_NT, ODINNT_CSDVERSION
        }
    },
    // Windows 2000 TODO!!!!!!!!!!!!!!!!!
    {
        ODINNT_VERSION,
        {
            sizeof(OSVERSIONINFOA), ODINNT_MAJOR_VERSION, ODINNT_MINOR_VERSION, 
            ODINNT_BUILD_NR, VER_PLATFORM_WIN32_NT, ODINNT_CSDVERSION
        }
    }
};

static BOOL fCheckVersion = FALSE;
static int  winversion    = WINVERSION_NT40;

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
	if(!stricmp(szVersion, PROFILE_WINVERSION_WIN2000)) {
		winversion = WINVERSION_WIN2000;
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
