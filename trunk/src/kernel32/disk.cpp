/* $Id: disk.cpp,v 1.17 2000-10-02 18:39:33 sandervl Exp $ */

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
#include "oslibdos.h"
#include "exceptutil.h"

#define DBG_LOCALLOG  DBG_disk
#include "dbglocal.h"


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
    BOOL rc;
    DWORD dwSectorsPerCluster;	// address of sectors per cluster ter
    DWORD dwBytesPerSector;	// address of bytes per sector
    DWORD dwNumberOfFreeClusters;	// address of number of free clusters
    DWORD dwTotalNumberOfClusters; 	// address of total number of clusters
    dprintf(("KERNEL32:  GetDiskFreeSpaceA %s, 0x%08X, 0x%08X, 0x%08X, 0x%08X,\n",
             arg1!=NULL?arg1:"NULL", arg2,arg3,arg4,arg5));

    rc = OSLibGetDiskFreeSpace((LPSTR)arg1, &dwSectorsPerCluster, &dwBytesPerSector,
                               &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
    if(rc)
    {
      if (arg2!=NULL)
        *arg2 = dwSectorsPerCluster;
      if (arg3!=NULL)
        *arg3 = dwBytesPerSector;
      if (arg4!=NULL)
        *arg4 = dwNumberOfFreeClusters;
      if (arg5!=NULL)
        *arg5 = dwTotalNumberOfClusters;
    }
    dprintf((" returned %d\n",rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetDiskFreeSpaceW(LPCWSTR arg1, PDWORD arg2, PDWORD arg3, PDWORD arg4, PDWORD  arg5)
{
 BOOL  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2GetDiskFreeSpaceW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = GetDiskFreeSpaceA(astring, arg2, arg3, arg4, arg5);
    FreeAsciiString(astring);
    return(rc);
}


/*****************************************************************************
 * Name      : GetDiskFreeSpaceEx
 * Purpose   :
 * Parameters: lpDirectoryName [in] Pointer to a null-terminated string that
 *                             specifies a directory on the disk of interest.
 *                             This string can be a UNC name. If this
 *                             parameter is a UNC name, you must follow it
 *                             with an additional backslash. For example, you
 *                             would specify \\MyServer\MyShare as
 *                             \\MyServer\MyShare\.
 *                             If lpDirectoryName is NULL, the
 *                             GetDiskFreeSpaceEx function obtains
 *                             information about the object store.
 *                             Note that lpDirectoryName does not have to
 *                             specify the root directory on a disk. The
 *                             function accepts any directory on the disk.
 *
 *             lpFreeBytesAvailableToCaller
 *                             [out] Pointer to a variable to receive the
 *                             total number of free bytes on the disk that
 *                             are available to the user associated with the
 *                             calling thread.
 *             lpTotalNumberOfBytes
 *                             [out] Pointer to a variable to receive the
 *                             total number of bytes on the disk that are
 *                             available to the user associated with the
 *                             calling thread.
 *             lpTotalNumberOfFreeBytes
 *                             [out] Pointer to a variable to receive the
 *                             total number of free bytes on the disk.
 *                             This parameter can be NULL.
 * Variables :
 * Result    : Nonzero indicates success. Zero indicates failure. To get
 *             extended error information, call GetLastError.
 * Remark    : Note that the values obtained by this function are of type
 *             ULARGE_INTEGER. Be careful not to truncate these values to
 *             32 bits.
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 2000/01/08 23:44]
 *****************************************************************************/

ODINFUNCTION4(BOOL,GetDiskFreeSpaceExA,
              LPCSTR,          lpDirectoryName,
              PULARGE_INTEGER, lpFreeBytesAvailableToCaller,
              PULARGE_INTEGER, lpTotalNumberOfBytes,
              PULARGE_INTEGER, lpTotalNumberOfFreeBytes )
{
    BOOL rc;
    DWORD dwSectorsPerCluster;	// address of sectors per cluster ter
    DWORD dwBytesPerSector;	// address of bytes per sector
    DWORD dwNumberOfFreeClusters;	// address of number of free clusters
    DWORD dwTotalNumberOfClusters; 	// address of total number of clusters

    rc = GetDiskFreeSpaceA(lpDirectoryName, &dwSectorsPerCluster, &dwBytesPerSector,
                           &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
    if(rc)
    {
      	if(lpFreeBytesAvailableToCaller!=NULL) {
		Mul32x32to64(lpFreeBytesAvailableToCaller, dwNumberOfFreeClusters, (dwSectorsPerCluster*dwBytesPerSector));
		dprintf(("lpFreeBytesAvailableToCaller %x%x", lpFreeBytesAvailableToCaller->LowPart, lpFreeBytesAvailableToCaller->HighPart));
	}
      	if(lpTotalNumberOfBytes!=NULL) {
		Mul32x32to64(lpTotalNumberOfBytes, dwTotalNumberOfClusters, (dwSectorsPerCluster*dwBytesPerSector));
		dprintf(("lpTotalNumberOfBytes %x%x", lpTotalNumberOfBytes->LowPart, lpTotalNumberOfBytes->HighPart));
	}
      	if(lpTotalNumberOfFreeBytes!=NULL) {
		memcpy(lpTotalNumberOfFreeBytes, lpFreeBytesAvailableToCaller, sizeof(*lpFreeBytesAvailableToCaller));
		dprintf(("lpTotalNumberOfFreeBytes %x%x", lpTotalNumberOfFreeBytes->LowPart, lpTotalNumberOfFreeBytes->HighPart));
	}
    }
    return rc;
}


ODINFUNCTION4(BOOL,GetDiskFreeSpaceExW,
              LPCWSTR,         lpDirectoryName,
              PULARGE_INTEGER, lpFreeBytesAvailableToCaller,
              PULARGE_INTEGER, lpTotalNumberOfBytes,
              PULARGE_INTEGER, lpTotalNumberOfFreeBytes )
{
 BOOL  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2GetDiskFreeSpaceExW\n"));
    astring = UnicodeToAsciiString((LPWSTR)lpDirectoryName);
    rc = GetDiskFreeSpaceExA(astring, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
    FreeAsciiString(astring);
    return(rc);
}


//******************************************************************************
//******************************************************************************
UINT WIN32API GetDriveTypeA( LPCSTR arg1)
{
    UINT rc;
    rc = O32_GetDriveType(arg1);
    dprintf(("KERNEL32:  GetDriveType %s = %d\n", arg1,rc));
    return rc;
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
   CHAR   tmpstring[256];
   ULONG  drive;
   BOOL   rc;

   dprintf(("GetVolumeInformationA %s", lpRootPathName));

   if(lpRootPathName == NULL) {
	GetCurrentDirectoryA(sizeof(tmpstring), tmpstring);
	lpRootPathName = tmpstring;
   }

   if('A' <= *lpRootPathName && *lpRootPathName <= 'Z') {
      	drive = *lpRootPathName - 'A' + 1;
   }
   else 
   if('a' <= *lpRootPathName && *lpRootPathName <= 'z') {
      	drive = *lpRootPathName - 'a' + 1;
   }
   else {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
   }

   if(lpVolumeSerialNumber || lpVolumeNameBuffer) {
   	rc = OSLibDosQueryVolumeSerialAndName(drive, lpVolumeSerialNumber, lpVolumeNameBuffer, nVolumeNameSize);
        if(lpVolumeSerialNumber) {
		dprintf2(("Volume serial number: %x", *lpVolumeSerialNumber));
	}
        if(lpVolumeNameBuffer) {
		dprintf2(("Volume name: %s", lpVolumeNameBuffer));
	}
   }
   if(lpFileSystemNameBuffer || lpMaximumComponentLength) {
	if(!lpFileSystemNameBuffer) {
		lpFileSystemNameBuffer = tmpstring;
		nFileSystemNameSize    = sizeof(tmpstring);
	}
	rc = OSLibDosQueryVolumeFS(drive, lpFileSystemNameBuffer, nFileSystemNameSize);
        if(lpFileSystemNameBuffer) {
		dprintf2(("File system name: %s", lpFileSystemNameBuffer));
	}
   }
   if(lpMaximumComponentLength) {
	if(!strcmp(lpFileSystemNameBuffer, "FAT")) {
		*lpMaximumComponentLength = 12;
	}
	else	*lpMaximumComponentLength = 255; //TODO: Always correct? (CDFS?)
   }
   if(lpFileSystemFlags) {
	if(strcmp(lpFileSystemNameBuffer, "FAT")) {
		*lpFileSystemFlags = FS_CASE_IS_PRESERVED;
	}
	else
	if(!strcmp(lpFileSystemNameBuffer, "CDFS")) {
		*lpFileSystemFlags = FS_CASE_SENSITIVE; //NT4 returns this
	}
	else
	if(!strcmp(lpFileSystemNameBuffer, "UDF")) {//TODO: correct?
		*lpFileSystemFlags = FS_CASE_SENSITIVE | FS_UNICODE_STORED_ON_DISK;
	}
	else	*lpFileSystemFlags = 0;

	dprintf2(("File system flags: %x", lpFileSystemFlags));
   }

   if(rc) {
	SetLastError(rc);
	return FALSE;
   }
   SetLastError(ERROR_SUCCESS);
   return TRUE;
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

  rc = CALL_ODINFUNC(GetVolumeInformationA)(asciiroot,
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
