/*
 * Win32 Disk API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include "unicode.h"

//******************************************************************************
//******************************************************************************
BOOL WIN32API SetVolumeLabelA( LPCSTR arg1, LPCSTR  arg2)
{
    dprintf(("KERNEL32:  OS2SetVolumeLabelA\n"));
    return O32_SetVolumeLabel(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetVolumeLabelW(LPWSTR lpRootPathName, LPWSTR lpVolumeName)
{
  char *asciiroot, *asciivolname;
  BOOL  rc;

    dprintf(("KERNEL32:  OS2SetVolumeLabelW\n"));
    asciiroot    = UnicodeToAsciiString(lpRootPathName);
    asciivolname = UnicodeToAsciiString(lpVolumeName);
    rc = O32_SetVolumeLabel(asciiroot, asciivolname);
    FreeAsciiString(asciivolname);
    FreeAsciiString(asciiroot);
    return(rc);
}

//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
BOOL WIN32API GetDiskFreeSpaceA( LPCSTR arg1, PDWORD arg2, PDWORD arg3, PDWORD arg4, PDWORD  arg5)
{
    dprintf(("KERNEL32:  OS2GetDiskFreeSpaceA\n"));
    return O32_GetDiskFreeSpace(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetDiskFreeSpaceW(LPCWSTR arg1, PDWORD arg2, PDWORD arg3, PDWORD arg4, PDWORD  arg5)
{
 BOOL  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2GetDiskFreeSpaceW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = O32_GetDiskFreeSpace(astring, arg2, arg3, arg4, arg5);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDriveTypeA( LPCSTR arg1)
{
    dprintf(("KERNEL32:  GetDriveType %s\n", arg1));
    return O32_GetDriveType(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDriveTypeW(LPCWSTR arg1)
{
 UINT  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2GetDriveTypeW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = O32_GetDriveType(astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVolumeInformationA(LPCSTR arg1, LPSTR arg2, DWORD arg3, PDWORD arg4,
                                    PDWORD arg5, PDWORD arg6, LPSTR arg7, DWORD  arg8)
{
    dprintf(("KERNEL32:  GetVolumeInformation %s %s\n", arg1, arg2));
    return O32_GetVolumeInformation(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVolumeInformationW(LPCWSTR arg1, LPWSTR arg2, DWORD arg3,
                                    PDWORD arg4, PDWORD arg5, PDWORD arg6,
                                    LPWSTR arg7, DWORD  arg8)
{
 char *asciiroot, *asciivol, *asciifs;
 BOOL  rc;

    dprintf(("KERNEL32:  OS2GetVolumeInformationW\n"));
    asciivol  = (char *)malloc(arg3+1);
    asciifs   = (char *)malloc(arg8+1);
    asciiroot = UnicodeToAsciiString((LPWSTR)arg1);
    rc = O32_GetVolumeInformation(asciiroot, asciivol, arg3, arg4, arg5, arg6, asciifs, arg8);

    if (arg2 != NULL)  /* @@@PH 98/06/07 */
      AsciiToUnicode(asciivol, arg2);

    if (arg7 != NULL)  /* @@@PH 98/06/07 */
      AsciiToUnicode(asciifs, arg7);

    FreeAsciiString(asciiroot);
    free(asciifs);
    free(asciivol);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetLogicalDrives(void)
{
    dprintf(("KERNEL32:  GetLogicalDrives\n"));
    return O32_GetLogicalDrives();
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetLogicalDriveStringsA(UINT arg1, LPSTR  arg2)
{
    dprintf(("KERNEL32:  OS2GetLogicalDriveStringsA\n"));
    return O32_GetLogicalDriveStrings(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetLogicalDriveStringsW(UINT nBufferLength, LPWSTR lpBuffer)
{
  char *asciibuffer = (char *)malloc(nBufferLength+1);
  DWORD rc;

    dprintf(("KERNEL32:  OS2GetLogicalDriveStringsW\n"));

    rc = O32_GetLogicalDriveStrings(nBufferLength, asciibuffer);
    if(rc)      AsciiToUnicode(asciibuffer, lpBuffer);
    free(asciibuffer);
    return(rc);
}
