/* $Id: version.cpp,v 1.9 2002-07-05 14:48:06 sandervl Exp $ */

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
#include "debugtools.h"

typedef struct
{
    LONG             getVersion16;
    LONG             getVersion;
    OSVERSIONINFOEXA getVersionEx;
} VERSION_DATA;

static VERSION_DATA VersionData[WINVERSION_MAX] =
{
    // Windows 98
    {
        0x070A5F03,
	0xC0000A04,
	{
	    sizeof(OSVERSIONINFOA), 4, 10, 0x40A07CE,
	    VER_PLATFORM_WIN32_WINDOWS, "Win98",
            0, 0, 0, 0
	}
    },
    // Windows ME
    {
        0x07005F03, /* Assuming DOS 7 like the other Win9x */
        0xC0005A04,
        {
            sizeof(OSVERSIONINFOA), 4, 90, 0x45A0BB8,
            VER_PLATFORM_WIN32_WINDOWS, " ",
            0, 0, 0, 0
        }
    },
    // Windows NT 4.0 (SP6)
    {
        0x05000A03,
        ODINNT_VERSION,
        {
            sizeof(OSVERSIONINFOA), ODINNT_MAJOR_VERSION, ODINNT_MINOR_VERSION, 
            ODINNT_BUILD_NR, VER_PLATFORM_WIN32_NT, ODINNT_CSDVERSION,
            6, 0, 0, 0
        }
    },
    // Windows 2000 (SP2)
    {
        0x05005F03,
        0x08930005,
        {
            sizeof(OSVERSIONINFOA), 5, 0, 0x893,
            VER_PLATFORM_WIN32_NT, "Service Pack 2",
            2, 0, 0, 0
        }
    },
    // Windows XP
    {
        0x05005F03, /* Assuming DOS 5 like the other NT */
        0x0A280105,
        {
            sizeof(OSVERSIONINFOA), 5, 1, 0xA28,
            VER_PLATFORM_WIN32_NT, "",
            0, 0, 0, 0
        }
    },
    // Windows XP SP3
    {
        0x05005F03, /* Assuming DOS 5 like the other NT */
        0x0A280105,
        {
            sizeof(OSVERSIONINFOA), 5, 1, 0xA28,
            VER_PLATFORM_WIN32_NT, "Service Pack 3",
            +3, 0, 0, 0
        }
    },
    // Windows 7 SP1
    {
        0x05005F03, /* Assuming DOS 5 like the other NT */
        0x1DB10106,
        {
            sizeof(OSVERSIONINFOA), 6, 1, 7601,
            VER_PLATFORM_WIN32_NT, "Service Pack 1",
            1, 0, 0, 0
        }
    }
};

static BOOL fCheckVersion = FALSE;
static int  winversion    = WINVERSION_WINXPSP3;

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
    case WINVERSION_WINXPSP3:
    case WINVERSION_WIN7:
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
    	if(!stricmp(szVersion, PROFILE_WINVERSION_NT40)) {
		    winversion = WINVERSION_NT40;
	    }
	    else
	    if(!stricmp(szVersion, PROFILE_WINVERSION_WIN2000)) {
    		winversion = WINVERSION_WIN2000;
    	}
    	else
    	if(!stricmp(szVersion, PROFILE_WINVERSION_WINXP)) {
		    winversion = WINVERSION_WINXP;
	    }
    	else
    	if(!stricmp(szVersion, PROFILE_WINVERSION_WINXPSP3)) {
		    winversion = WINVERSION_WINXPSP3;
	    }
	    else
	    if(!stricmp(szVersion, PROFILE_WINVERSION_WIN7)) {
		    winversion = WINVERSION_WIN7;
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
    if(lpVersionInformation->dwOSVersionInfoSize < sizeof(OSVERSIONINFOA))
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

    dprintf(("version      %x.%x", lpVersionInformation->dwMajorVersion, lpVersionInformation->dwMinorVersion));
    dprintf(("build nr     %x", lpVersionInformation->dwBuildNumber));
    dprintf(("Platform Id  %x", lpVersionInformation->dwPlatformId));
    dprintf(("szCSDVersion %s", lpVersionInformation->szCSDVersion));

    if(lpVersionInformation->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXA)) 
    {//Windows 2000 (and up) extension
        LPOSVERSIONINFOEXA lpVersionExInformation = (LPOSVERSIONINFOEXA)lpVersionInformation;

        lpVersionExInformation->wServicePackMajor = VersionData[winversion].getVersionEx.wServicePackMajor;
        lpVersionExInformation->wServicePackMinor = VersionData[winversion].getVersionEx.wServicePackMinor;
        lpVersionExInformation->wReserved[0]      = VersionData[winversion].getVersionEx.wReserved[0];
        lpVersionExInformation->wReserved[1]      = VersionData[winversion].getVersionEx.wReserved[1];
    
        dprintf(("service pack version %x.%x", lpVersionExInformation->wServicePackMajor, lpVersionExInformation->wServicePackMinor));
    }
    
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
    if(lpVersionInformation->dwOSVersionInfoSize < sizeof(OSVERSIONINFOW))
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

    if(lpVersionInformation->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXW)) 
    {//Windows 2000 (and up) extension
        LPOSVERSIONINFOEXW lpVersionExInformation = (LPOSVERSIONINFOEXW)lpVersionInformation;
    
        lpVersionExInformation->wServicePackMajor = VersionData[winversion].getVersionEx.wServicePackMajor;
        lpVersionExInformation->wServicePackMinor = VersionData[winversion].getVersionEx.wServicePackMinor;
        lpVersionExInformation->wReserved[0]      = VersionData[winversion].getVersionEx.wReserved[0];
        lpVersionExInformation->wReserved[1]      = VersionData[winversion].getVersionEx.wReserved[1];
    }
    SetLastError(ERROR_SUCCESS);
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetVersion()
{
    dprintf(("KERNEL32: GetVersion"));
    if(fCheckVersion == FALSE) {
	    CheckVersion();
    }
    return VersionData[winversion].getVersion;
}
//******************************************************************************
//******************************************************************************
 /******************************************************************************
  *        VerifyVersionInfoW   (KERNEL32.@)
  */
/* Compares the version Odin reports with the requested one, following the rules of
 * VerifyVersionInfo: the condition of every field is in 3 bits of the mask (VER_EQUAL 1,
 * VER_GREATER 2, VER_GREATER_EQUAL 3, VER_LESS 4, VER_LESS_EQUAL 5); major, minor,
 * build number, service pack major and minor are compared as a hierarchy, the other
 * fields independently. */
static BOOL verifyVersion(DWORD typeMask, DWORDLONG condMask,
                          DWORD reqMajor, DWORD reqMinor, DWORD reqBuild, DWORD reqPlatform,
                          DWORD reqSpMajor, DWORD reqSpMinor, DWORD reqSuite, DWORD reqProduct)
{
    OSVERSIONINFOEXA *cur;
    DWORD act[8], req[8];
    static const int hier[5] = { 1, 0, 2, 5, 4 };   /* bit index: major, minor, build, sp major, sp minor */
    int   i, passed = 0;

    if(fCheckVersion == FALSE) {
        CheckVersion();
    }
    cur = &VersionData[winversion].getVersionEx;
    act[0] = cur->dwMinorVersion;           req[0] = reqMinor;
    act[1] = cur->dwMajorVersion;           req[1] = reqMajor;
    act[2] = cur->dwBuildNumber & 0xFFFF;   req[2] = reqBuild;
    act[3] = cur->dwPlatformId;             req[3] = reqPlatform;
    act[4] = cur->wServicePackMinor;        req[4] = reqSpMinor;
    act[5] = cur->wServicePackMajor;        req[5] = reqSpMajor;
    act[6] = cur->wReserved[0];               req[6] = reqSuite;
    act[7] = cur->wReserved[1] ? cur->wReserved[1] : 1 /* VER_NT_WORKSTATION */;
    req[7] = reqProduct;

    for(i = 0; i < 5 && !passed; i++) {
        int  bit = hier[i], cmp;
        int  cond;
        if(!(typeMask & (1u << bit)))
            continue;
        cond = (int)((condMask >> (3 * bit)) & 7);
        cmp  = act[bit] > req[bit] ? 1 : (act[bit] < req[bit] ? -1 : 0);
        switch(cond) {
        case 1:                                            /* VER_EQUAL */
            if(cmp != 0) goto fail;
            break;
        case 2:                                            /* VER_GREATER */
        case 3:                                            /* VER_GREATER_EQUAL */
            if(cmp > 0) passed = 1;
            else if(cmp < 0) goto fail;
            else if(i == 4 && cond == 2) goto fail;
            break;
        case 4:                                            /* VER_LESS */
        case 5:                                            /* VER_LESS_EQUAL */
            if(cmp < 0) passed = 1;
            else if(cmp > 0) goto fail;
            else if(i == 4 && cond == 4) goto fail;
            break;
        default:
            goto fail;
        }
    }
    /* last hierarchy field tested with an exclusive condition but all equal: handled above only for
       the last of the five; the common case (>= or equal everywhere) passes here */
    for(i = 3; i < 8; i += (i == 3 ? 3 : 1)) {         /* platform, suite, product type */
        int bit = i, cmp, cond;
        if(!(typeMask & (1u << bit)))
            continue;
        cond = (int)((condMask >> (3 * bit)) & 7);
        cmp  = act[bit] > req[bit] ? 1 : (act[bit] < req[bit] ? -1 : 0);
        if(bit == 6) {                                     /* suite mask: VER_AND / VER_OR */
            if(cond == 7 /* VER_AND */ ? (act[bit] & req[bit]) != req[bit] : (act[bit] & req[bit]) == 0)
                goto fail;
            continue;
        }
        if(cond == 1 && cmp != 0) goto fail;
        else if(cond == 2 && cmp <= 0) goto fail;
        else if(cond == 3 && cmp < 0) goto fail;
        else if(cond == 4 && cmp >= 0) goto fail;
        else if(cond == 5 && cmp > 0) goto fail;
    }
    return TRUE;
fail:
    SetLastError(ERROR_OLD_WIN_VERSION);
    return FALSE;
}

BOOL WINAPI VerifyVersionInfoW( /* LPOSVERSIONINFOEXW */ LPVOID lpVersionInfo, DWORD dwTypeMask,
                                 DWORDLONG dwlConditionMask)
{
    const BYTE *p = (const BYTE *)lpVersionInfo;
    if(p == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return verifyVersion(dwTypeMask, dwlConditionMask,
                         *(const DWORD *)(p + 4), *(const DWORD *)(p + 8), *(const DWORD *)(p + 12),
                         *(const DWORD *)(p + 16),
                         *(const WORD *)(p + 276), *(const WORD *)(p + 278),
                         *(const WORD *)(p + 280), p[282]);
}

