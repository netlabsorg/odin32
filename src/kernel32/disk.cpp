/* $Id: disk.cpp,v 1.7 1999-11-11 19:10:09 sandervl Exp $ */

/*
 * Win32 Disk API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"


ODINDEBUGCHANNEL(KERNEL32-DISK)

//******************************************************************************
//******************************************************************************
BOOL WIN32API SetVolumeLabelA( LPCSTR arg1, LPCSTR  arg2)
{
    dprintf(("KERNEL32:  OS2SetVolumeLabelA\n"));
    return O32_SetVolumeLabel(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetVolumeLabelW(LPCWSTR lpRootPathName, LPCWSTR lpVolumeName)
{
  char *asciiroot, *asciivolname;
  BOOL  rc;

    dprintf(("KERNEL32:  OS2SetVolumeLabelW\n"));
    asciiroot    = UnicodeToAsciiString((LPWSTR)lpRootPathName);
    asciivolname = UnicodeToAsciiString((LPWSTR)lpVolumeName);
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

    astring = UnicodeToAsciiString((LPWSTR)arg1);
    dprintf(("KERNEL32:  OS2GetDriveTypeW %s", astring));
    rc = O32_GetDriveType(astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION8(BOOL,    GetVolumeInformationA,
              LPCSTR,  lpRootPathName,
              LPSTR,   lpVolumeNameBuffer,
              DWORD,   nVolumeNameSize,
              PDWORD,  lpVolumeSerialNumber,
              PDWORD,  lpMaximumComponentLength,
              PDWORD,  lpFileSystemFlags,
              LPSTR,   lpFileSystemNameBuffer,
              DWORD,   nFileSystemNameSize)
{
   dprintf(("GetVolumeInformationA %s", lpRootPathName));
   return O32_GetVolumeInformation(lpRootPathName,
                                   lpVolumeNameBuffer,
                                   nVolumeNameSize,
                                   lpVolumeSerialNumber,
                                   lpMaximumComponentLength,
                                   lpFileSystemFlags,
                                   lpFileSystemNameBuffer,
                                   nFileSystemNameSize);
}
//******************************************************************************
//******************************************************************************

ODINFUNCTION8(BOOL,    GetVolumeInformationW,
              LPCWSTR, lpRootPathName,
              LPWSTR,  lpVolumeNameBuffer,
              DWORD,   nVolumeNameSize,
              PDWORD,  lpVolumeSerialNumber,
              PDWORD,  lpMaximumComponentLength,
              PDWORD,  lpFileSystemFlags,
              LPWSTR,  lpFileSystemNameBuffer,
              DWORD,   nFileSystemNameSize)
{
  char *asciiroot,
       *asciivol,
       *asciifs;
  BOOL  rc;

  // transform into ascii
  asciivol  = (char *)malloc(nVolumeNameSize+1);
  asciifs   = (char *)malloc(nFileSystemNameSize+1);

  // clear ascii buffers
  memset (asciivol, 0, (nVolumeNameSize + 1));
  memset (asciifs,  0, (nFileSystemNameSize + 1));

  if (lpRootPathName != NULL) // NULL is valid!
    asciiroot = UnicodeToAsciiString((LPWSTR)lpRootPathName);
  else
    asciiroot = NULL;

  // @@@PH switch to ODIN_
  rc = GetVolumeInformationA(asciiroot,
                             asciivol,
                             nVolumeNameSize,
                             lpVolumeSerialNumber,
                             lpMaximumComponentLength,
                             lpFileSystemFlags,
                             asciifs,
                             nFileSystemNameSize);

    if (lpVolumeNameBuffer != NULL)  /* @@@PH 98/06/07 */
      AsciiToUnicodeN(asciivol, lpVolumeNameBuffer, nVolumeNameSize);

    if (lpFileSystemNameBuffer != NULL)  /* @@@PH 98/06/07 */
      AsciiToUnicodeN(asciifs, lpFileSystemNameBuffer, nFileSystemNameSize);


  if (asciiroot != NULL)
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
