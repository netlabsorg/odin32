/* $Id: disk.cpp,v 1.40 2004-03-24 09:23:47 sandervl Exp $ */

/*
 * Win32 Disk API functions for OS/2
 *
 * Copyright 1998-2002 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <versionos2.h>
#include "unicode.h"
#include "oslibdos.h"
#include "osliblvm.h"
#include "asmutil.h"
#include "profile.h"
#include "hmdisk.h"

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
//******************************************************************************
BOOL WIN32API GetDiskFreeSpaceA(LPCSTR lpszRootPathName, PDWORD lpSectorsPerCluster,
                                PDWORD lpBytesPerSector, PDWORD lpFreeClusters,
                                PDWORD lpClusters)
{
    BOOL rc;
    DWORD dwSectorsPerCluster;    // address of sectors per cluster ter
    DWORD dwBytesPerSector;   // address of bytes per sector
    DWORD dwNumberOfFreeClusters; // address of number of free clusters
    DWORD dwTotalNumberOfClusters;    // address of total number of clusters

    rc = OSLibGetDiskFreeSpace((LPSTR)lpszRootPathName, &dwSectorsPerCluster, &dwBytesPerSector,
                                &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
    if(rc)
    {
        if (lpSectorsPerCluster!=NULL)
            *lpSectorsPerCluster = dwSectorsPerCluster;
        if (lpBytesPerSector!=NULL)
            *lpBytesPerSector = dwBytesPerSector;
        if (lpFreeClusters!=NULL)
            *lpFreeClusters = dwNumberOfFreeClusters;
        if (lpClusters!=NULL)
            *lpClusters = dwTotalNumberOfClusters;

    /* CW: Windows Media Player setup complains about wrong clustersize when odin is installed on
       a TVFS drive. This fakes the clustersizes to 32. The following
       entry must be present in ODIN.INI:

       [DRIVESPACE]
       TVFSTOHPFS = 1
       */
        if(lpSectorsPerCluster!=NULL) 
        {
            if(*lpSectorsPerCluster==1024 && PROFILE_GetOdinIniBool("DRIVESPACE","CLUSTERTO32",0)) 
            {/* TVFS returns 1024 sectors per cluster */
                dprintf(("KERNEL32:  GetDiskFreeSpaceA, TVFS-Drive detected. Faking clustersize to 32.\n"));
                *lpSectorsPerCluster=32;
                if (lpFreeClusters!=NULL)
                    *lpFreeClusters = dwNumberOfFreeClusters<<0x5;
                if (lpClusters!=NULL)
                    *lpClusters = dwTotalNumberOfClusters<<0x5;
            }
        }
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetDiskFreeSpaceW(LPCWSTR lpszRootPathName,
                                PDWORD  lpSectorsPerCluster,
                                PDWORD  lpBytesPerSector,
                                PDWORD  lpFreeClusters,
                                PDWORD  lpClusters)
{
    BOOL  rc;
    char *astring;

    astring = UnicodeToAsciiString((LPWSTR)lpszRootPathName);
    rc = GetDiskFreeSpaceA(astring, lpSectorsPerCluster, lpBytesPerSector, lpFreeClusters, lpClusters);
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

BOOL WIN32API GetDiskFreeSpaceExA(LPCSTR lpDirectoryName,
                                  PULARGE_INTEGER lpFreeBytesAvailableToCaller,
                                  PULARGE_INTEGER lpTotalNumberOfBytes,
                                  PULARGE_INTEGER lpTotalNumberOfFreeBytes )
{
    BOOL rc;
    DWORD dwSectorsPerCluster;  // address of sectors per cluster ter
    DWORD dwBytesPerSector; // address of bytes per sector
    DWORD dwNumberOfFreeClusters;   // address of number of free clusters
    DWORD dwTotalNumberOfClusters;  // address of total number of clusters

    rc = GetDiskFreeSpaceA(lpDirectoryName, &dwSectorsPerCluster, &dwBytesPerSector,
                           &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
    if(rc)
    {
        if(lpFreeBytesAvailableToCaller!=NULL) {
            Mul32x32to64(lpFreeBytesAvailableToCaller, dwNumberOfFreeClusters, (dwSectorsPerCluster*dwBytesPerSector));
            dprintf(("lpFreeBytesAvailableToCaller %x%x", lpFreeBytesAvailableToCaller->HighPart, lpFreeBytesAvailableToCaller->LowPart));
        }
        if(lpTotalNumberOfBytes!=NULL) {
            Mul32x32to64(lpTotalNumberOfBytes, dwTotalNumberOfClusters, (dwSectorsPerCluster*dwBytesPerSector));
            dprintf(("lpTotalNumberOfBytes %x%x", lpTotalNumberOfBytes->HighPart, lpTotalNumberOfBytes->LowPart));
        }
        if(lpTotalNumberOfFreeBytes!=NULL) {
            memcpy(lpTotalNumberOfFreeBytes, lpFreeBytesAvailableToCaller, sizeof(*lpFreeBytesAvailableToCaller));
            dprintf(("lpTotalNumberOfFreeBytes %x%x", lpTotalNumberOfFreeBytes->HighPart, lpTotalNumberOfFreeBytes->LowPart));
        }
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetDiskFreeSpaceExW(LPCWSTR         lpDirectoryName,
                                  PULARGE_INTEGER lpFreeBytesAvailableToCaller,
                                  PULARGE_INTEGER lpTotalNumberOfBytes,
                                  PULARGE_INTEGER lpTotalNumberOfFreeBytes )
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
//Note: NT4, SP6 does not change the last error, regardless of the junk it receives!
//******************************************************************************
UINT WIN32API GetDriveTypeA(LPCSTR lpszDrive)
{
    UINT rc;
    ULONG driveIndex;

    if(lpszDrive == 0) {
        driveIndex = OSLibDosQueryCurrentDisk() - 1;
    }
    else
    if(*lpszDrive >= 'A' && *lpszDrive <= 'Z')
        driveIndex = (DWORD)(*lpszDrive - 'A');
    else
    if(*lpszDrive >= 'a' && *lpszDrive <= 'z') {
        driveIndex = (DWORD)(*lpszDrive - 'a');
    }
    else {
        //Volume functions only available in Windows 2000 and up
        if(VERSION_IS_WIN2000_OR_HIGHER() && !strncmp(lpszDrive, VOLUME_NAME_PREFIX, sizeof(VOLUME_NAME_PREFIX)-1)) 
        {
            char *pszVolume;
            int   length;

            //strip volume name prefix (\\\\?\\Volume\\)
            length = strlen(lpszDrive);
            pszVolume = (char *)alloca(length);

            if(OSLibLVMStripVolumeName(lpszDrive, pszVolume, length)) 
            {
                rc = OSLibLVMGetDriveType(pszVolume);
                dprintf(("KERNEL32:  GetDriveType %s = %d (LVM)", lpszDrive, rc));
                return rc;
            }
        }
        return DRIVE_NO_ROOT_DIR;   //return value checked in NT4, SP6 (GetDriveType(""), GetDriveType("4");
    }

    //NOTE: Although GetDriveTypeW handles -1, GetDriveTypeA crashes in NT 4, SP6
    rc = OSLibGetDriveType(driveIndex);
    dprintf(("KERNEL32:  GetDriveType %s = %d", lpszDrive, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDriveTypeW(LPCWSTR lpszDrive)
{
    UINT  rc;
    char *astring;

    if(lpszDrive == (LPCWSTR)-1) {
        return DRIVE_CANNOTDETERMINE;   //NT 4, SP6 returns this (VERIFIED)
    }
    astring = UnicodeToAsciiString((LPWSTR)lpszDrive);
    dprintf(("KERNEL32: GetDriveTypeW %s", astring));
    rc = GetDriveTypeA(astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
static int fForce2GBFileSize = FALSE;
//******************************************************************************
void WIN32API CustForce2GBFileSize()
{
    fForce2GBFileSize = TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVolumeInformationA(LPCSTR  lpRootPathName,
                                    LPSTR   lpVolumeNameBuffer,
                                    DWORD   nVolumeNameSize,
                                    PDWORD  lpVolumeSerialNumber,
                                    PDWORD  lpMaximumComponentLength,
                                    PDWORD  lpFileSystemFlags,
                                    LPSTR   lpFileSystemNameBuffer,
                                    DWORD   nFileSystemNameSize)
{
   CHAR   tmpstring[256];
   CHAR   szOrgFileSystemName[256] = "";
   ULONG  drive;
   BOOL   rc, fVolumeName = FALSE;
   char   *pszVolume;

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
        //Volume functions only available in Windows 2000 and up
        if(LOBYTE(GetVersion()) >= 5 && !strncmp(lpRootPathName, VOLUME_NAME_PREFIX, sizeof(VOLUME_NAME_PREFIX)-1)) 
        {
            int   length;

            //strip volume name prefix (\\\\?\\Volume\\)
            length = strlen(lpRootPathName);
            pszVolume = (char *)alloca(length);

            if(OSLibLVMStripVolumeName(lpRootPathName, pszVolume, length)) 
            {
                pszVolume[length-2] = 0;
                fVolumeName = TRUE;
                goto proceed;
            }
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
proceed:

    if(lpVolumeSerialNumber || lpVolumeNameBuffer) {
        if(fVolumeName) {
             rc = OSLibLVMQueryVolumeSerialAndName(pszVolume, lpVolumeSerialNumber, lpVolumeNameBuffer, nVolumeNameSize);
        }
        else rc = OSLibDosQueryVolumeSerialAndName(drive, lpVolumeSerialNumber, lpVolumeNameBuffer, nVolumeNameSize);
        if(lpVolumeSerialNumber) {
            dprintf2(("Volume serial number: %x", *lpVolumeSerialNumber));
        }
        if(lpVolumeNameBuffer) {
            dprintf2(("Volume name: %s", lpVolumeNameBuffer));
        }
    }
    if(lpFileSystemNameBuffer || lpMaximumComponentLength || lpFileSystemFlags) 
    {
        if(!lpFileSystemNameBuffer) {
            lpFileSystemNameBuffer = tmpstring;
            nFileSystemNameSize    = sizeof(tmpstring);
        }
        if(fVolumeName) {
             rc = OSLibLVMQueryVolumeFS(pszVolume, lpFileSystemNameBuffer, nFileSystemNameSize);
        }
        else rc = OSLibDosQueryVolumeFS(drive, lpFileSystemNameBuffer, nFileSystemNameSize);

        //save original file system name
        if(rc == ERROR_SUCCESS) strcpy(szOrgFileSystemName, lpFileSystemNameBuffer);

        if(lpFileSystemNameBuffer) 
        {
            dprintf2(("File system name: %s", lpFileSystemNameBuffer));
            if(!strcmp(lpFileSystemNameBuffer, "JFS")) 
            {
                strcpy(lpFileSystemNameBuffer, "NTFS");
            }
            else
            if(!strcmp(lpFileSystemNameBuffer, "CDFS") ||
               !strcmp(lpFileSystemNameBuffer, "UDF") ||
               !strcmp(lpFileSystemNameBuffer, "NTFS") ||
               !strcmp(lpFileSystemNameBuffer, "FAT32"))
            {
                //do nothing
            }
            else 
            {//pretend everything else is FAT16 (HPFS and FAT have the same file size limit)
                // @@VP:20040323 - nFileSystemNameSize may be 0!!!
                strncpy(lpFileSystemNameBuffer, "FAT16", nFileSystemNameSize);
            }
            if(fForce2GBFileSize) {
                if(strcmp(lpFileSystemNameBuffer, "CDFS") &&
                   strcmp(lpFileSystemNameBuffer, "UDF")) 
                {//everything is FAT -> 2 GB file size limit
                    strncpy(lpFileSystemNameBuffer, "FAT16", nFileSystemNameSize);
                }
            }
            dprintf2(("Final file system name: %s", lpFileSystemNameBuffer));
        }
    }
    if(lpMaximumComponentLength) {
        if(!strcmp(szOrgFileSystemName, "FAT16") || !strcmp(szOrgFileSystemName, "FAT")) {
             *lpMaximumComponentLength = 12;  //8.3
        }
        else *lpMaximumComponentLength = 255; //TODO: Always correct? (CDFS?)
    }
    if(lpFileSystemFlags)
    {
        if(strcmp(lpFileSystemNameBuffer, "FAT16")) {
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
        else *lpFileSystemFlags = 0;

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
BOOL WIN32API GetVolumeInformationW(LPCWSTR lpRootPathName,
                                    LPWSTR  lpVolumeNameBuffer,
                                    DWORD   nVolumeNameSize,
                                    PDWORD  lpVolumeSerialNumber,
                                    PDWORD  lpMaximumComponentLength,
                                    PDWORD  lpFileSystemFlags,
                                    LPWSTR  lpFileSystemNameBuffer,
                                    DWORD   nFileSystemNameSize)
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

    rc = GetVolumeInformationA(asciiroot, asciivol, nVolumeNameSize, lpVolumeSerialNumber,
                               lpMaximumComponentLength, lpFileSystemFlags, asciifs, nFileSystemNameSize);

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
    return OSLibGetLogicalDrives();
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetLogicalDriveStringsA(UINT cchBuffer, LPSTR lpszBuffer)
{
    dprintf(("KERNEL32: GetLogicalDriveStringsA", cchBuffer, lpszBuffer));
    return O32_GetLogicalDriveStrings(cchBuffer, lpszBuffer);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetLogicalDriveStringsW(UINT nBufferLength, LPWSTR lpBufferW)
{
   LPSTR  lpBufferA = (LPSTR)HeapAlloc( GetProcessHeap(), 0, sizeof(CHAR) * (nBufferLength + 1) );
   DWORD   dwResult = GetLogicalDriveStringsA( nBufferLength, lpBufferA );

   dprintf(("KERNEL32: GetLogicalDriveStringsW %d %x", nBufferLength, lpBufferA));

   if ( dwResult && dwResult <= nBufferLength )
	MultiByteToWideChar( CP_ACP, 0, lpBufferA, dwResult + 1, lpBufferW, nBufferLength );

   HeapFree( GetProcessHeap(), 0, lpBufferA );

   return dwResult;
}
//******************************************************************************
typedef struct {
    HANDLE hLVMVolumeControlData;
    DWORD  lastvol;
} VOLINFO;
//******************************************************************************
HANDLE WIN32API FindFirstVolumeA(LPTSTR lpszVolumeName, DWORD cchBufferLength)
{
    HANDLE   hVolume;
    VOLINFO *pVolInfo;
    char     szdrive[3];
    char     szVolume[256];

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    hVolume = GlobalAlloc(0, sizeof(VOLINFO));
    if(hVolume == 0) {
        dprintf(("ERROR: FindFirstVolumeA: out of memory!!"));
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    pVolInfo = (VOLINFO *)GlobalLock(hVolume);
    pVolInfo->hLVMVolumeControlData = OSLibLVMQueryVolumeControlData();
    pVolInfo->lastvol               = 0;

    if(OSLibLVMQueryVolumeName(pVolInfo->hLVMVolumeControlData, &pVolInfo->lastvol, szVolume, sizeof(szVolume)) == FALSE) {
        SetLastError(ERROR_NO_MORE_FILES);
        goto fail;
    }
    if(strlen(szVolume) + 14 + 1 > cchBufferLength) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        goto fail;
    }
    sprintf(lpszVolumeName, VOLUME_NAME_PREFIX"{%s}\\", szVolume);
    dprintf(("FindFirstVolumeA returned %s", lpszVolumeName));
    pVolInfo->lastvol++;
    GlobalUnlock(hVolume);
    SetLastError(ERROR_SUCCESS);
    return hVolume;

fail:
    GlobalUnlock(hVolume);
    GlobalFree(hVolume);
    return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API FindFirstVolumeW(LPWSTR lpszVolumeName, DWORD cchBufferLength)
{
    LPSTR  pszvolname = NULL;
    HANDLE hVolume;

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    if(cchBufferLength) {
        pszvolname = (char *)alloca(cchBufferLength);
    }
    hVolume = FindFirstVolumeA(pszvolname, cchBufferLength);
    if(hVolume) {
        int len = MultiByteToWideChar( CP_ACP, 0, pszvolname, -1, NULL, 0);
        MultiByteToWideChar(CP_ACP, 0, pszvolname, -1, lpszVolumeName, len);
    }
    return hVolume;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextVolumeA(HANDLE hFindVolume, LPTSTR lpszVolumeName, 
                              DWORD cchBufferLength)
{
    VOLINFO *pVolInfo;
    char     szdrive[3];
    DWORD    DeviceType;
    char     szVolume[256];

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    pVolInfo = (VOLINFO *)GlobalLock(hFindVolume);
    if(pVolInfo == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(OSLibLVMQueryVolumeName(pVolInfo->hLVMVolumeControlData, &pVolInfo->lastvol, 
                               szVolume, sizeof(szVolume)) == FALSE) {
        SetLastError(ERROR_NO_MORE_FILES);
        GlobalUnlock(hFindVolume);
        return FALSE;
    }
    if(strlen(szVolume) + 14 + 1 > cchBufferLength) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        GlobalUnlock(hFindVolume);
        return FALSE;
    }
    sprintf(lpszVolumeName, VOLUME_NAME_PREFIX"{%s}\\", szVolume);
    dprintf(("FindNextVolumeA returned %s", lpszVolumeName));
    pVolInfo->lastvol++;
    GlobalUnlock(hFindVolume);
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextVolumeW(HANDLE hFindVolume, LPWSTR lpszVolumeName, 
                              DWORD cchBufferLength)
{
    LPSTR  pszvolnameA = NULL;
    BOOL   ret;

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    if(cchBufferLength) {
        pszvolnameA = (char *)alloca(cchBufferLength);
    }
    ret = FindNextVolumeA(hFindVolume, pszvolnameA, cchBufferLength);
    if(ret) {
        int len = MultiByteToWideChar( CP_ACP, 0, pszvolnameA, -1, NULL, 0);
        MultiByteToWideChar(CP_ACP, 0, pszvolnameA, -1, lpszVolumeName, len);
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindVolumeClose(HANDLE hFindVolume)
{
    VOLINFO *pVolInfo;

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    if(hFindVolume) {
        pVolInfo = (VOLINFO *)GlobalLock(hFindVolume);
        OSLibLVMFreeVolumeControlData(pVolInfo->hLVMVolumeControlData);
        GlobalUnlock(hFindVolume);
        GlobalFree(hFindVolume);
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API FindFirstVolumeMountPointA(LPTSTR lpszRootPathName, 
                                           LPTSTR lpszVolumeMountPoint,
                                           DWORD cchBufferLength)
{
    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return INVALID_HANDLE_VALUE;
    }
    
    SetLastError(ERROR_NO_MORE_FILES);
    return INVALID_HANDLE_VALUE;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API FindFirstVolumeMountPointW(LPWSTR lpszRootPathName, 
                                           LPWSTR lpszVolumeMountPoint,
                                           DWORD cchBufferLength)
{
    LPSTR  pszmountpointnameA = NULL;
    LPSTR  pszrootA = NULL;
    HANDLE hVolume;

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return INVALID_HANDLE_VALUE;
    }

    if(cchBufferLength) {
        pszmountpointnameA = (char *)alloca(cchBufferLength);
    }
    if(lpszRootPathName) {
        pszrootA = (char *)alloca(lstrlenW(lpszRootPathName)+1);

        int len = WideCharToMultiByte( CP_ACP, 0, lpszRootPathName, -1, NULL, 0, 0, NULL);
        WideCharToMultiByte(CP_ACP, 0, lpszRootPathName, -1, pszrootA, len, 0, NULL);
    }

    hVolume = FindFirstVolumeMountPointA(pszrootA, pszmountpointnameA, cchBufferLength);
    if(hVolume) {
        int len = MultiByteToWideChar( CP_ACP, 0, pszmountpointnameA, -1, NULL, 0);
        MultiByteToWideChar(CP_ACP, 0, pszmountpointnameA, -1, lpszVolumeMountPoint, len);
    }
    return hVolume;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextVolumeMountPointA(HANDLE hFindVolumeMountPoint,
                                        LPTSTR lpszVolumeMountPoint,
                                        DWORD cchBufferLength)
{
    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
    SetLastError(ERROR_NO_MORE_FILES);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextVolumeMountPointW(HANDLE hFindVolumeMountPoint,
                                        LPWSTR lpszVolumeMountPoint,
                                        DWORD cchBufferLength)
{
    LPSTR  pszmoutpointnameA = NULL;
    BOOL   ret;

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    if(cchBufferLength) {
        pszmoutpointnameA = (char *)alloca(cchBufferLength);
    }
    ret = FindFirstVolumeA(pszmoutpointnameA, cchBufferLength);
    if(ret) {
        int len = MultiByteToWideChar( CP_ACP, 0, pszmoutpointnameA, -1, NULL, 0);
        MultiByteToWideChar(CP_ACP, 0, pszmoutpointnameA, -1, lpszVolumeMountPoint, len);
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindVolumeMountPointClose(HANDLE hFindVolumeMountPoint)
{
    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    if(hFindVolumeMountPoint) {
        GlobalFree(hFindVolumeMountPoint);
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVolumeNameForVolumeMountPointA(LPCSTR lpszVolumeMountPoint,
                                                LPSTR lpszVolumeName,
                                                DWORD cchBufferLength)
{
    LPSTR pszvol;

    pszvol = (char *)alloca(cchBufferLength);
    if(pszvol == NULL) {
        DebugInt3();
        return FALSE;
    }

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    if(OSLibLVMGetVolumeNameForVolumeMountPoint(lpszVolumeMountPoint, pszvol, 
                                                cchBufferLength) == TRUE) 
    {
        int length = strlen(pszvol);
        if(length + sizeof(VOLUME_NAME_PREFIX) - 1 + 3 > cchBufferLength) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        sprintf(lpszVolumeName, VOLUME_NAME_PREFIX"{%s}\\", pszvol);

        dprintf(("GetVolumeNameForVolumeMountPointA %s returned %s", lpszVolumeMountPoint, lpszVolumeName));
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    dprintf(("GetVolumeNameForVolumeMountPointA: %s not found!!", lpszVolumeMountPoint));
    SetLastError(ERROR_FILE_NOT_FOUND);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVolumeNameForVolumeMountPointW(LPCWSTR lpszVolumeMountPoint,
                                                LPWSTR lpszVolumeName,
                                                DWORD cchBufferLength)
{
    LPSTR  pszmoutpointnameA = NULL;
    LPSTR  pszvolumenameA = NULL;
    BOOL   ret;
    int    len;

    if(!VERSION_IS_WIN2000_OR_HIGHER()) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
    len = WideCharToMultiByte( CP_ACP, 0, lpszVolumeMountPoint, -1, NULL, 0, 0, NULL);
    pszmoutpointnameA = (char *)alloca(len+1);
    WideCharToMultiByte(CP_ACP, 0, lpszVolumeMountPoint, -1, pszmoutpointnameA, len, 0, NULL);

    if(cchBufferLength && lpszVolumeName) {
        pszvolumenameA = (char *)alloca(cchBufferLength);
    }
    ret = GetVolumeNameForVolumeMountPointA(pszmoutpointnameA, pszvolumenameA, cchBufferLength);
    if(ret) {
        int len = MultiByteToWideChar( CP_ACP, 0, pszvolumenameA, -1, NULL, 0);
        MultiByteToWideChar(CP_ACP, 0, pszvolumenameA, -1, lpszVolumeName, len);
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
