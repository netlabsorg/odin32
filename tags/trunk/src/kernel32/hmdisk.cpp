/* $Id: hmdisk.cpp,v 1.30 2001-11-28 23:33:36 phaller Exp $ */

/*
 * Win32 Disk API functions for OS/2
 *
 * Copyright 2000 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <string.h>

#include <misc.h>
#include "hmdisk.h"
#include "mmap.h"
#include "oslibdos.h"
#include <win\winioctl.h>
#include <win\ntddscsi.h>
#include <win\wnaspi32.h>
#include <win\aspi.h>

#define DBG_LOCALLOG    DBG_hmdisk
#include "dbglocal.h"

#define BIT_0     (1)
#define BIT_1     (2)
#define BIT_2     (4)
#define BIT_11    (1<<11)

typedef struct 
{
    HINSTANCE hInstAspi;
    DWORD (WIN32API *GetASPI32SupportInfo)();
    DWORD (CDECL *SendASPI32Command)(LPSRB lpSRB);
    ULONG     driveLetter;
    ULONG     driveType;
    ULONG     dwVolumelabel;
    CHAR      signature[8];
    DWORD     dwAccess;
    DWORD     dwShare;
    DWORD     dwCreation;
    DWORD     dwFlags;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes;
    HFILE     hTemplate;
} DRIVE_INFO;

HMDeviceDiskClass::HMDeviceDiskClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName)
{
    HMDeviceRegisterEx("\\\\.\\PHYSICALDRIVE", this, NULL);
}

/*****************************************************************************
 * Name      : HMDeviceDiskClass::FindDevice
 * Purpose   : Checks if lpDeviceName belongs to this device class
 * Parameters: LPCSTR lpClassDevName
 *             LPCSTR lpDeviceName
 *             int namelength
 * Variables :
 * Result    : checks if name is for a drive of physical disk
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceDiskClass::FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength)
{
  // @@@PH
  // this is broken:
  // "\\.\C:" and "C:" is a LOGICAL drive
  // \\.\PHYSICALDRIVEx is a PHYSICAL drive!
  
    //\\.\x:                -> length 6
    //\\.\PHYSICALDRIVEn    -> length 18
    if(namelength != 6 && namelength != 18) {
        return FALSE;
    }

    //SvL: \\.\x:             -> drive x (i.e. \\.\C:)
    //     \\.\PHYSICALDRIVEn -> drive n (n>=0)
    if((strncmp(lpDeviceName, "\\\\.\\", 4) == 0) &&
        namelength == 6 && lpDeviceName[5] == ':')
    {
        return TRUE;
    }
    if((strncmp(lpDeviceName, "\\\\.\\PHYSICALDRIVE", 17) == 0) && namelength == 18) {
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//TODO: PHYSICALDRIVEn!!
//******************************************************************************
DWORD HMDeviceDiskClass::CreateFile (HANDLE        hHandle,
                                     LPCSTR        lpFileName,
                                     PHMHANDLEDATA pHMHandleData,
                                     PVOID         lpSecurityAttributes,
                                     PHMHANDLEDATA pHMHandleDataTemplate)
{
    HFILE hFile;
    HFILE hTemplate;

    dprintf2(("KERNEL32: HMDeviceDiskClass::CreateFile %s(%s,%08x,%08x,%08x)\n",
             lpHMDeviceName,
             lpFileName,
             pHMHandleData,
             lpSecurityAttributes,
             pHMHandleDataTemplate));

    //TODO: check in NT if CREATE_ALWAYS is allowed!!
    if(pHMHandleData->dwCreation != OPEN_EXISTING) {
        dprintf(("Invalid creation flags %x!!", pHMHandleData->dwCreation));
        return ERROR_INVALID_PARAMETER;
    }

    //Disable error popus. NT allows an app to open a cdrom/dvd drive without a disk inside
    //OS/2 fails in that case with error ERROR_NOT_READY
    ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS);
    hFile = OSLibDosCreateFile((LPSTR)lpFileName,
                               pHMHandleData->dwAccess,
                               pHMHandleData->dwShare,
                               (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                               pHMHandleData->dwCreation,
                               pHMHandleData->dwFlags,
                               hTemplate);
    SetErrorMode(oldmode);

    if (hFile != INVALID_HANDLE_ERROR || GetLastError() == ERROR_NOT_READY)
    {
        if(hFile == INVALID_HANDLE_ERROR) {
             dprintf(("Drive not ready"));
             SetLastError(NO_ERROR);
             pHMHandleData->hHMHandle  = 0; //handle lookup fails if this is set to -1
        }
        else pHMHandleData->hHMHandle  = hFile;

        DRIVE_INFO *drvInfo = (DRIVE_INFO *)malloc(sizeof(DRIVE_INFO));
        if(drvInfo == NULL) {
             DebugInt3();
             if(pHMHandleData->hHMHandle) OSLibDosClose(pHMHandleData->hHMHandle);
             return ERROR_OUTOFMEMORY;
        }
        pHMHandleData->dwUserData = (DWORD)drvInfo;

        memset(drvInfo, 0, sizeof(DRIVE_INFO));
        drvInfo->dwAccess  = pHMHandleData->dwAccess;
        drvInfo->dwAccess  = pHMHandleData->dwShare;
        drvInfo->lpSecurityAttributes  = (LPSECURITY_ATTRIBUTES)lpSecurityAttributes;
        drvInfo->dwAccess  = pHMHandleData->dwCreation;
        drvInfo->dwAccess  = pHMHandleData->dwFlags;
        drvInfo->hTemplate = hTemplate;

        drvInfo->driveLetter = *lpFileName; //save drive letter
        if(drvInfo->driveLetter >= 'a') {
            drvInfo->driveLetter = drvInfo->driveLetter - ((int)'a' - (int)'A');
        }

        drvInfo->driveType = GetDriveTypeA(lpFileName);
        if(drvInfo->driveType == DRIVE_CDROM) 
        {
            drvInfo->hInstAspi = LoadLibraryA("WNASPI32.DLL");
            if(drvInfo->hInstAspi == NULL) {
                if(pHMHandleData->hHMHandle) OSLibDosClose(pHMHandleData->hHMHandle);
                free(drvInfo);
                return ERROR_INVALID_PARAMETER;
            }
            *(FARPROC *)&drvInfo->GetASPI32SupportInfo = GetProcAddress(drvInfo->hInstAspi, "GetASPI32SupportInfo");
            *(FARPROC *)&drvInfo->SendASPI32Command    = GetProcAddress(drvInfo->hInstAspi, "SendASPI32Command");

            if(drvInfo->GetASPI32SupportInfo() == (SS_FAILED_INIT << 8)) {
                FreeLibrary(drvInfo->hInstAspi);
                drvInfo->hInstAspi = 0;
            }
   
            //get cdrom signature
            DWORD parsize = 4;
            DWORD datasize = 4;
            strcpy(drvInfo->signature, "CD01");
            OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x80, 0x61, &drvInfo->signature[0], 4, &parsize,
                             &drvInfo->signature[0], 4, &datasize);
        }

        if(hFile) {
            OSLibDosQueryVolumeSerialAndName(1 + drvInfo->driveLetter - 'A', &drvInfo->dwVolumelabel, NULL, 0);
        }

        return (NO_ERROR);
    }
    else {
        dprintf(("CreateFile failed; error %d", GetLastError()));
        return(GetLastError());
    }
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceDiskClass::OpenDisk(PVOID pDrvInfo)
{
    char filename[3];
    DRIVE_INFO *drvInfo = (DRIVE_INFO*)pDrvInfo;
    HFILE hFile;

    filename[0] = 'A' + drvInfo->driveLetter;
    filename[1] = ':';
    filename[2] = 0;

    //Disable error popups. NT allows an app to open a cdrom/dvd drive without a disk inside
    //OS/2 fails in that case with error ERROR_NOT_READY
    ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS);
    hFile = OSLibDosCreateFile(filename,
                               drvInfo->dwAccess,
                               drvInfo->dwShare,
                               drvInfo->lpSecurityAttributes,
                               drvInfo->dwCreation,
                               drvInfo->dwFlags,
                               drvInfo->hTemplate);
    SetErrorMode(oldmode);

    if (hFile != INVALID_HANDLE_ERROR || GetLastError() == ERROR_NOT_READY)
    {
        if(hFile == INVALID_HANDLE_ERROR) {
             dprintf(("Drive not ready"));
             return 0; 
        }
        OSLibDosQueryVolumeSerialAndName(1 + drvInfo->driveLetter - 'A', &drvInfo->dwVolumelabel, NULL, 0);
        return hFile;
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceDiskClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
    BOOL ret = TRUE;

    if(pHMHandleData->hHMHandle) {
        ret = OSLibDosClose(pHMHandleData->hHMHandle);
    }
    if(pHMHandleData->dwUserData) {
       DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
       if(drvInfo->hInstAspi)    FreeLibrary(drvInfo->hInstAspi);
       free(drvInfo);
    }
    return ret;
}
//******************************************************************************
//******************************************************************************


// this helper function just calls the specified
// ioctl function for the CDROM manager with no
// parameter packet other than the CD01 signature
// and no data packet.
static BOOL ioctlCDROMSimple(PHMHANDLEDATA pHMHandleData,
                             DWORD dwCategory,
                             DWORD dwFunction,
                             LPDWORD lpBytesReturned, DRIVE_INFO *pdrvInfo)
{
  DWORD dwParameterSize = 4;
  DWORD dwDataSize      = 0;
  DWORD ret;

  if(lpBytesReturned)
    *lpBytesReturned = 0;

  ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                         dwCategory,
                         dwFunction,
                         pdrvInfo->signature,
                         4,
                         &dwParameterSize,
                         NULL,
                         0,
                         &dwDataSize);
  if(ret)
  {
    SetLastError(error2WinError(ret));
    return FALSE;
  }
  SetLastError(ERROR_SUCCESS);
  return TRUE;
}


// this helper function just calls the specified
// ioctl function for the DISK manager with the
// specified function codes
static BOOL ioctlDISKUnlockEject(PHMHANDLEDATA pHMHandleData,
                                 DWORD dwCommand,
                                 DWORD dwDiskHandle,
                                 LPDWORD lpBytesReturned)
{
#pragma pack(1)
  struct
  {
    BYTE ucCommand;
    BYTE ucHandle;
  } ParameterBlock;
#pragma pack()
  
  DWORD dwParameterSize = sizeof( ParameterBlock );
  DWORD dwDataSize      = 0;
  DWORD ret;
  
  ParameterBlock.ucCommand = dwCommand;
  ParameterBlock.ucHandle  = dwDiskHandle;
  
  if(lpBytesReturned)
    *lpBytesReturned = 0;

  ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                         0x08,       // IOCTL_DISK
                         0x40,       // DSK_UNLOCKEJECTMEDIA
                         &ParameterBlock,
                         sizeof( ParameterBlock ),
                         &dwParameterSize,
                         NULL,
                         0,
                         &dwDataSize);
  if(ret)
  {
    SetLastError(error2WinError(ret));
    return FALSE;
  }
  SetLastError(ERROR_SUCCESS);
  return TRUE;
}



BOOL HMDeviceDiskClass::DeviceIoControl(PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                             LPVOID lpInBuffer, DWORD nInBufferSize,
                             LPVOID lpOutBuffer, DWORD nOutBufferSize,
                             LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
#ifdef DEBUG
    char *msg = NULL;

    switch(dwIoControlCode)
    {
    case FSCTL_DELETE_REPARSE_POINT:
        msg = "FSCTL_DELETE_REPARSE_POINT";
        break;
    case FSCTL_DISMOUNT_VOLUME:
        msg = "FSCTL_DISMOUNT_VOLUME";
        break;
    case FSCTL_GET_COMPRESSION:
        msg = "FSCTL_GET_COMPRESSION";
        break;
    case FSCTL_GET_REPARSE_POINT:
        msg = "FSCTL_GET_REPARSE_POINT";
        break;
    case FSCTL_LOCK_VOLUME:
        msg = "FSCTL_LOCK_VOLUME";
        break;
    case FSCTL_QUERY_ALLOCATED_RANGES:
        msg = "FSCTL_QUERY_ALLOCATED_RANGES";
        break;
    case FSCTL_SET_COMPRESSION:
        msg = "FSCTL_SET_COMPRESSION";
        break;
    case FSCTL_SET_REPARSE_POINT:
        msg = "FSCTL_SET_REPARSE_POINT";
        break;
    case FSCTL_SET_SPARSE:
        msg = "FSCTL_SET_SPARSE";
        break;
    case FSCTL_SET_ZERO_DATA:
        msg = "FSCTL_SET_ZERO_DATA";
        break;
    case FSCTL_UNLOCK_VOLUME:
        msg = "FSCTL_UNLOCK_VOLUME";
        break;
    case IOCTL_DISK_CHECK_VERIFY:
        msg = "IOCTL_DISK_CHECK_VERIFY";
        break;
    case IOCTL_DISK_EJECT_MEDIA:
        msg = "IOCTL_DISK_EJECT_MEDIA";
        break;
    case IOCTL_DISK_FORMAT_TRACKS:
        msg = "IOCTL_DISK_FORMAT_TRACKS";
        break;
    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
        msg = "IOCTL_DISK_GET_DRIVE_GEOMETRY";
        break;
    case IOCTL_DISK_GET_DRIVE_LAYOUT:
        msg = "IOCTL_DISK_GET_DRIVE_LAYOUT";
        break;
    case IOCTL_DISK_GET_MEDIA_TYPES:
        msg = "IOCTL_DISK_GET_MEDIA_TYPES";
        break;
    case IOCTL_DISK_GET_PARTITION_INFO:
        msg = "IOCTL_DISK_GET_PARTITION_INFO";
        break;
    case IOCTL_DISK_LOAD_MEDIA:
        msg = "IOCTL_DISK_LOAD_MEDIA";
        break;
    case IOCTL_DISK_MEDIA_REMOVAL:
        msg = "IOCTL_DISK_MEDIA_REMOVAL";
        break;
    case IOCTL_DISK_PERFORMANCE:
        msg = "IOCTL_DISK_PERFORMANCE";
        break;
    case IOCTL_DISK_REASSIGN_BLOCKS:
        msg = "IOCTL_DISK_REASSIGN_BLOCKS";
        break;
    case IOCTL_DISK_SET_DRIVE_LAYOUT:
        msg = "IOCTL_DISK_SET_DRIVE_LAYOUT";
        break;
    case IOCTL_DISK_SET_PARTITION_INFO:
        msg = "IOCTL_DISK_SET_PARTITION_INFO";
        break;
    case IOCTL_DISK_VERIFY:
        msg = "IOCTL_DISK_VERIFY";
        break;
    case IOCTL_SERIAL_LSRMST_INSERT:
        msg = "IOCTL_SERIAL_LSRMST_INSERT";
        break;
    case IOCTL_STORAGE_CHECK_VERIFY:
        msg = "IOCTL_STORAGE_CHECK_VERIFY";
        break;
    case IOCTL_STORAGE_EJECT_MEDIA:
        msg = "IOCTL_STORAGE_EJECT_MEDIA";
        break;
    case IOCTL_STORAGE_GET_MEDIA_TYPES:
        msg = "IOCTL_STORAGE_GET_MEDIA_TYPES";
        break;
    case IOCTL_STORAGE_LOAD_MEDIA:
        msg = "IOCTL_STORAGE_LOAD_MEDIA";
        break;
    case IOCTL_STORAGE_MEDIA_REMOVAL:
        msg = "IOCTL_STORAGE_MEDIA_REMOVAL";
        break;
    case IOCTL_SCSI_PASS_THROUGH:
        msg = "IOCTL_SCSI_PASS_THROUGH";
        break;
    case IOCTL_SCSI_MINIPORT:
        msg = "IOCTL_SCSI_MINIPORT";
        break;
    case IOCTL_SCSI_GET_INQUIRY_DATA:
        msg = "IOCTL_SCSI_GET_INQUIRY_DATA";
        break;
    case IOCTL_SCSI_GET_CAPABILITIES:
        msg = "IOCTL_SCSI_GET_CAPABILITIES";
        break;
    case IOCTL_SCSI_PASS_THROUGH_DIRECT:
        msg = "IOCTL_SCSI_PASS_THROUGH_DIRECT";
        break;
    case IOCTL_SCSI_GET_ADDRESS:
        msg = "IOCTL_SCSI_GET_ADDRESS";
        break;
    case IOCTL_SCSI_RESCAN_BUS:
        msg = "IOCTL_SCSI_RESCAN_BUS";
        break;
    case IOCTL_SCSI_GET_DUMP_POINTERS:
        msg = "IOCTL_SCSI_GET_DUMP_POINTERS";
        break;
    case IOCTL_SCSI_FREE_DUMP_POINTERS:
        msg = "IOCTL_SCSI_FREE_DUMP_POINTERS";
        break;
    case IOCTL_IDE_PASS_THROUGH:
        msg = "IOCTL_IDE_PASS_THROUGH";
        break;
    case IOCTL_CDROM_UNLOAD_DRIVER:
        msg = "IOCTL_CDROM_UNLOAD_DRIVER";
        break;
    case IOCTL_CDROM_READ_TOC:
        msg = "IOCTL_CDROM_READ_TOC";
        break;
    case IOCTL_CDROM_GET_CONTROL:
        msg = "IOCTL_CDROM_GET_CONTROL";
        break;
    case IOCTL_CDROM_PLAY_AUDIO_MSF:
        msg = "IOCTL_CDROM_PLAY_AUDIO_MSF";
        break;
    case IOCTL_CDROM_SEEK_AUDIO_MSF:
        msg = "IOCTL_CDROM_SEEK_AUDIO_MSF";
        break;
    case IOCTL_CDROM_STOP_AUDIO:
        msg = "IOCTL_CDROM_STOP_AUDIO";
        break;
    case IOCTL_CDROM_PAUSE_AUDIO:
        msg = "IOCTL_CDROM_PAUSE_AUDIO";
        break;
    case IOCTL_CDROM_RESUME_AUDIO:
        msg = "IOCTL_CDROM_RESUME_AUDIO";
        break;
    case IOCTL_CDROM_GET_VOLUME:
        msg = "IOCTL_CDROM_GET_VOLUME";
        break;
    case IOCTL_CDROM_SET_VOLUME:
        msg = "IOCTL_CDROM_SET_VOLUME";
        break;
    case IOCTL_CDROM_READ_Q_CHANNEL:
        msg = "IOCTL_CDROM_READ_Q_CHANNEL";
        break;
    case IOCTL_CDROM_GET_LAST_SESSION:
        msg = "IOCTL_CDROM_GET_LAST_SESSION";
        break;
    case IOCTL_CDROM_RAW_READ:
        msg = "IOCTL_CDROM_RAW_READ";
        break;
    case IOCTL_CDROM_DISK_TYPE:
        msg = "IOCTL_CDROM_DISK_TYPE";
        break;
    case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
        msg = "IOCTL_CDROM_GET_DRIVE_GEOMETRY";
        break;
    case IOCTL_CDROM_CHECK_VERIFY:
        msg = "IOCTL_CDROM_CHECK_VERIFY";
        break;
    case IOCTL_CDROM_MEDIA_REMOVAL:
        msg = "IOCTL_CDROM_MEDIA_REMOVAL";
        break;
    case IOCTL_CDROM_EJECT_MEDIA:
        msg = "IOCTL_CDROM_EJECT_MEDIA";
        break;
    case IOCTL_CDROM_LOAD_MEDIA:
        msg = "IOCTL_CDROM_LOAD_MEDIA";
        break;
    case IOCTL_CDROM_RESERVE:
        msg = "IOCTL_CDROM_RESERVE";
        break;
    case IOCTL_CDROM_RELEASE:
        msg = "IOCTL_CDROM_RELEASE";
        break;
    case IOCTL_CDROM_FIND_NEW_DEVICES:
        msg = "IOCTL_CDROM_FIND_NEW_DEVICES";
        break;
    }
    if(msg) {
        dprintf(("HMDeviceDiskClass::DeviceIoControl %s %x %d %x %d %x %x", msg, lpInBuffer, nInBufferSize,
                 lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped));
    }
#endif

    DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
    if(drvInfo == NULL) {
        dprintf(("ERROR: DeviceIoControl: drvInfo == NULL!!!"));
        DebugInt3();
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    switch(dwIoControlCode)
    {
    case FSCTL_DELETE_REPARSE_POINT:
    case FSCTL_DISMOUNT_VOLUME:
    case FSCTL_GET_COMPRESSION:
    case FSCTL_GET_REPARSE_POINT:
    case FSCTL_LOCK_VOLUME:
    case FSCTL_QUERY_ALLOCATED_RANGES:
    case FSCTL_SET_COMPRESSION:
    case FSCTL_SET_REPARSE_POINT:
    case FSCTL_SET_SPARSE:
    case FSCTL_SET_ZERO_DATA:
    case FSCTL_UNLOCK_VOLUME:
        break;

    case IOCTL_DISK_FORMAT_TRACKS:
    case IOCTL_DISK_GET_DRIVE_LAYOUT:
        break;

    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
    case IOCTL_DISK_GET_MEDIA_TYPES:
    {
        PDISK_GEOMETRY pGeom = (PDISK_GEOMETRY)lpOutBuffer;
        if(nOutBufferSize < sizeof(DISK_GEOMETRY) || !pGeom) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }

        ULONG  volumelabel;
        APIRET rc;

        if(!pHMHandleData->hHMHandle) {
            pHMHandleData->hHMHandle = OpenDisk(drvInfo);
            if(!pHMHandleData->hHMHandle) {
                dprintf(("No disk inserted; aborting"));
                SetLastError(ERROR_NOT_READY);
                return FALSE;
            }
        }

        //Applications can use this IOCTL to check if the floppy has been changed
        //OSLibDosGetDiskGeometry won't fail when that happens so we read the
        //volume label from the disk and return ERROR_MEDIA_CHANGED if the volume
        //label has changed
        //TODO: Find better way to determine if floppy was removed or switched
        rc = OSLibDosQueryVolumeSerialAndName(1 + drvInfo->driveLetter - 'A', &volumelabel, NULL, 0);
        if(rc) {
            dprintf(("IOCTL_DISK_GET_DRIVE_GEOMETRY: OSLibDosQueryVolumeSerialAndName failed with rc %d", GetLastError()));
            if(pHMHandleData->hHMHandle) OSLibDosClose(pHMHandleData->hHMHandle);
            pHMHandleData->hHMHandle = 0;
            SetLastError(ERROR_MEDIA_CHANGED);
            return FALSE;
        }
        if(volumelabel != drvInfo->dwVolumelabel) {
            dprintf(("IOCTL_DISK_GET_DRIVE_GEOMETRY: volume changed %x -> %x", drvInfo->dwVolumelabel, volumelabel));
            SetLastError(ERROR_MEDIA_CHANGED);
            return FALSE;
        }

        if(OSLibDosGetDiskGeometry(pHMHandleData->hHMHandle, drvInfo->driveLetter, pGeom) == FALSE) {
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = sizeof(DISK_GEOMETRY);
        }
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    case IOCTL_DISK_GET_PARTITION_INFO:
    case IOCTL_DISK_LOAD_MEDIA:
    case IOCTL_DISK_MEDIA_REMOVAL:
    case IOCTL_DISK_PERFORMANCE:
    case IOCTL_DISK_REASSIGN_BLOCKS:
    case IOCTL_DISK_SET_DRIVE_LAYOUT:
    case IOCTL_DISK_SET_PARTITION_INFO:
    case IOCTL_DISK_VERIFY:
    case IOCTL_SERIAL_LSRMST_INSERT:
        break;
      
      
    // -----------
    // CDROM class
    // -----------
    case IOCTL_CDROM_READ_TOC:
    {
#pragma pack(1)
        typedef struct
        {
            BYTE  ucFirstTrack;
            BYTE  ucLastTrack;
            DWORD ulLeadOutAddr;
        } AudioDiskInfo;
        typedef struct
        {
            DWORD ulTrackAddr;
            BYTE  ucTrackControl;
        } AudioTrackInfo;
        typedef struct 
        {
            BYTE  signature[4];
            BYTE  ucTrack;
        } ParameterBlock;
#pragma pack()

        PCDROM_TOC pTOC = (PCDROM_TOC)lpOutBuffer;
        DWORD rc, numtracks;
        DWORD parsize = 4;
        DWORD datasize;
        AudioDiskInfo diskinfo;
        AudioTrackInfo trackinfo;
        ParameterBlock parm;

        if(lpBytesReturned)
            *lpBytesReturned = 0;

        if(!pTOC) {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        if(nOutBufferSize < sizeof(CDROM_TOC)) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        memset(pTOC, 0, nOutBufferSize);
        //IOCTL_CDROMAUDIO (0x81), CDROMAUDIO_GETAUDIODISK (0x61)
        datasize = sizeof(diskinfo);
        rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x81, 0x61, &drvInfo->signature[0], 4, &parsize,
                              &diskinfo, sizeof(diskinfo), &datasize);
        if(rc != NO_ERROR) {
            dprintf(("OSLibDosDevIOCtl failed with rc %d", rc));
            return FALSE;
        }
        pTOC->FirstTrack = diskinfo.ucFirstTrack;
        pTOC->LastTrack  = diskinfo.ucLastTrack;
        numtracks = pTOC->LastTrack - pTOC->FirstTrack + 1;
        dprintf(("first %d, last %d, num %d", pTOC->FirstTrack, pTOC->LastTrack, numtracks));

        //numtracks+1, because we have to add a track at the end
        int length = 4 + (numtracks+1)*sizeof(TRACK_DATA); 
        //big endian format
        pTOC->Length[0] = HIBYTE((length-2));  //minus length itself;
        pTOC->Length[1] = LOBYTE((length-2));  //minus length itself;

        if(nOutBufferSize < length) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        
        for(int i=0;i<numtracks;i++) 
        {
            parsize = sizeof(parm);
            memcpy(parm.signature, drvInfo->signature, 4);
            parm.ucTrack = i;

            datasize = sizeof(trackinfo);

            //IOCTL_CDROMAUDIO (0x81), CDROMAUDIO_GETAUDIOTRACK (0x62)
            rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x81, 0x62, &parm, sizeof(parm), &parsize,
                                  &trackinfo, sizeof(trackinfo), &datasize);
            if(rc != NO_ERROR) {
                dprintf(("OSLibDosDevIOCtl failed with rc %d", rc));
                return FALSE;
            }
            pTOC->TrackData[i].TrackNumber = pTOC->FirstTrack + i;
            pTOC->TrackData[i].Reserved    = 0;
            pTOC->TrackData[i].Control     = trackinfo.ucTrackControl >> 4;
            pTOC->TrackData[i].Adr         = trackinfo.ucTrackControl & 0xF;
            pTOC->TrackData[i].Reserved1   = 0;
            //big endian format
            pTOC->TrackData[i].Address[0]  = HIBYTE(HIWORD(trackinfo.ulTrackAddr));
            pTOC->TrackData[i].Address[1]  = LOBYTE(HIWORD(trackinfo.ulTrackAddr));
            pTOC->TrackData[i].Address[2]  = HIBYTE(LOWORD(trackinfo.ulTrackAddr));
            pTOC->TrackData[i].Address[3]  = LOBYTE(LOWORD(trackinfo.ulTrackAddr));
        }

        //Add a track at the end (presumably so the app can determine the size of the 1st track)
        //That is what NT4, SP6 does anyway
        pTOC->TrackData[numtracks].TrackNumber = 0xAA;
        pTOC->TrackData[numtracks].Reserved    = 0;
        pTOC->TrackData[numtracks].Control     = pTOC->TrackData[numtracks-1].Control;
        pTOC->TrackData[numtracks].Adr         = pTOC->TrackData[numtracks-1].Adr;
        pTOC->TrackData[numtracks].Reserved1   = 0;
        //big endian format
        //Address of pseudo track is the address of the lead-out track
        pTOC->TrackData[numtracks].Address[0]  = HIBYTE(HIWORD(diskinfo.ulLeadOutAddr));
        pTOC->TrackData[numtracks].Address[1]  = LOBYTE(HIWORD(diskinfo.ulLeadOutAddr));
        pTOC->TrackData[numtracks].Address[2]  = HIBYTE(LOWORD(diskinfo.ulLeadOutAddr));
        pTOC->TrackData[numtracks].Address[3]  = LOBYTE(LOWORD(diskinfo.ulLeadOutAddr));

        if(lpBytesReturned)
            *lpBytesReturned = length;

        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    case IOCTL_CDROM_UNLOAD_DRIVER:
    case IOCTL_CDROM_GET_CONTROL:
        break;
    
    case IOCTL_CDROM_PLAY_AUDIO_MSF:
    {
      dprintf(("Play CDROM audio playback"));
      
#pragma pack(1)
      struct
      {
        DWORD ucSignature;
        BYTE  ucAddressingMode;
        DWORD ulStartingMSF;
        DWORD ulEndingMSF;
      } ParameterBlock;
#pragma pack()
      
      PCDROM_PLAY_AUDIO_MSF pPlay = (PCDROM_PLAY_AUDIO_MSF)lpInBuffer;
      
      // setup the parameter block
      
      memcpy(&ParameterBlock.ucSignature, drvInfo->signature, 4);
      ParameterBlock.ucAddressingMode = 1;     // MSF format
      
      // @@@PH unknown if this kind of MSF conversion is correct!
      ParameterBlock.ulStartingMSF    = pPlay->StartingM << 16 |
                                        pPlay->StartingS << 8  |
                                        pPlay->StartingF;
      ParameterBlock.ulEndingMSF      = pPlay->EndingM << 16 |
                                        pPlay->EndingS << 8  |
                                        pPlay->EndingF;
        
      DWORD dwParameterSize = sizeof( ParameterBlock );
      DWORD dwDataSize      = 0;
      DWORD ret;
    
      if(lpBytesReturned)
        *lpBytesReturned = 0;
    
      ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                             0x81,  // IOCTL_CDROMAUDIO
                             0x50,  // CDROMAUDIO_PLAYAUDIO
                             &ParameterBlock,
                             sizeof( ParameterBlock ),
                             &dwParameterSize,
                             NULL,
                             0,
                             &dwDataSize);
      if(ret)
      {
        SetLastError(error2WinError(ret));
        return FALSE;
      }
      SetLastError(ERROR_SUCCESS);
      return TRUE;
    }
      
    case IOCTL_CDROM_SEEK_AUDIO_MSF:
        break;

    case IOCTL_CDROM_PAUSE_AUDIO:
      // NO BREAK CASE
      // Note: for OS/2, pause and stop seems to be the same!
      
    case IOCTL_CDROM_STOP_AUDIO:
    {
      dprintf(("Stop / pause CDROM audio playback"));
      return ioctlCDROMSimple(pHMHandleData, 
                              0x81,   // IOCTL_CDROMAUDIO
                              0x51,   // CDROMAUDIO_STOPAUDIO
                              lpBytesReturned, drvInfo);
    }
      
    case IOCTL_CDROM_RESUME_AUDIO:
    {
      dprintf(("Resume CDROM audio playback"));
      return ioctlCDROMSimple(pHMHandleData, 
                              0x81,   // IOCTL_CDROMAUDIO
                              0x52,   // CDROMAUDIO_RESUMEAUDIO
                              lpBytesReturned, drvInfo);
    }

    case IOCTL_CDROM_GET_VOLUME:
    {
        PVOLUME_CONTROL pVol = (PVOLUME_CONTROL)lpOutBuffer;
        char volbuf[8];
        DWORD parsize, datasize, ret;

        if(nOutBufferSize < sizeof(VOLUME_CONTROL) || !pVol) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }
        parsize = 4;
        datasize = 8;
        ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x81, 0x60, drvInfo->signature, 4, &parsize,
                               volbuf, 8, &datasize);

        if(ret) {
            SetLastError(error2WinError(ret));
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = sizeof(VOLUME_CONTROL);
        }
        pVol->PortVolume[0] = volbuf[1];
        pVol->PortVolume[1] = volbuf[3];
        pVol->PortVolume[2] = volbuf[5];
        pVol->PortVolume[3] = volbuf[7];
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    case IOCTL_CDROM_SET_VOLUME:
    {
        PVOLUME_CONTROL pVol = (PVOLUME_CONTROL)lpInBuffer;
        char volbuf[8];
        DWORD parsize, datasize, ret;

        if(nInBufferSize < sizeof(VOLUME_CONTROL) || !pVol) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }
        parsize = 4;
        datasize = 8;
        volbuf[0] = 0;
        volbuf[1] = pVol->PortVolume[0];
        volbuf[2] = 1;
        volbuf[3] = pVol->PortVolume[1];
        volbuf[4] = 2;
        volbuf[5] = pVol->PortVolume[2];
        volbuf[6] = 3;
        volbuf[7] = pVol->PortVolume[3];
        dprintf(("Set CD volume (%d,%d)(%d,%d)", pVol->PortVolume[0], pVol->PortVolume[1], pVol->PortVolume[2], pVol->PortVolume[3]));
        ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x81, 0x40, drvInfo->signature, 4, &parsize,
                               volbuf, 8, &datasize);

        if(ret) {
            SetLastError(error2WinError(ret));
            return FALSE;
        }
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    case IOCTL_CDROM_READ_Q_CHANNEL:
    case IOCTL_CDROM_GET_LAST_SESSION:
    case IOCTL_CDROM_RAW_READ:
    case IOCTL_CDROM_DISK_TYPE:
    case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
    case IOCTL_CDROM_MEDIA_REMOVAL:
        break;
      
    case IOCTL_CDROM_EJECT_MEDIA:
    {
      dprintf(("Eject CDROM media"));
      return ioctlCDROMSimple(pHMHandleData, 
                              0x80,   // IOCTL_CDROM
                              0x44,   // CDROMDISK_EJECTDISK
                              lpBytesReturned, drvInfo);
    }
      
    case IOCTL_CDROM_LOAD_MEDIA:
    {
      dprintf(("Loading CDROM media"));
      return ioctlCDROMSimple(pHMHandleData, 
                              0x80,   // IOCTL_CDROM
                              0x45,   // CDROMDISK_CLOSETRAY
                              lpBytesReturned, drvInfo);
    }
      
    case IOCTL_CDROM_RESERVE:
    case IOCTL_CDROM_RELEASE:
    case IOCTL_CDROM_FIND_NEW_DEVICES:
        break;
      
      
    // -------------
    // STORAGE class
    // -------------
      
    case IOCTL_CDROM_CHECK_VERIFY:
        if(drvInfo->driveType != DRIVE_CDROM) {
            SetLastError(ERROR_GEN_FAILURE); //TODO: right error?
            return FALSE;
        }
        //no break;
    case IOCTL_DISK_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY:
    {
        dprintf(("IOCTL_CDROM(DISK/STORAGE)CHECK_VERIFY %s", drvInfo->signature));
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }

        if(!pHMHandleData->hHMHandle) {
            pHMHandleData->hHMHandle = OpenDisk(drvInfo);
            if(!pHMHandleData->hHMHandle) {
                dprintf(("No disk inserted; aborting"));
                SetLastError(ERROR_NOT_READY);
                return FALSE;
            }
        }

#pragma pack(1)
      typedef struct
      {
        BYTE  ucCommandInfo;
        WORD  usDriveUnit;
      } ParameterBlock;
#pragma pack()

        DWORD parsize = sizeof(ParameterBlock);
        DWORD datasize = 2;
        WORD status = 0;
        DWORD rc;
        ParameterBlock parm;

        parm.ucCommandInfo = 0;
        parm.usDriveUnit = drvInfo->driveLetter - 'A';
        //IOCTL_DISK (0x08), DSK_GETLOCKSTATUS (0x66)
//            rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x08, 0x66, &parm, sizeof(parm), &parsize,
        //TODO: this doesn't work for floppies for some reason...
        rc = OSLibDosDevIOCtl(-1, 0x08, 0x66, &parm, sizeof(parm), &parsize,
                              &status, sizeof(status), &datasize);
        if(rc != NO_ERROR) {
            dprintf(("OSLibDosDevIOCtl failed with rc %d datasize %d", rc, datasize));
            return FALSE;
        }
        dprintf(("Disk status 0x%x", status));
        //if no disk present, return FALSE
        if(!(status & (BIT_2))) {
            SetLastError(ERROR_NOT_READY);  //NT4, SP6 returns this
            return FALSE;
        }
        SetLastError(NO_ERROR);
        return TRUE;
    }

    case IOCTL_DISK_EJECT_MEDIA:
    case IOCTL_STORAGE_EJECT_MEDIA:
    {
      dprintf(("Ejecting storage media"));
      return ioctlDISKUnlockEject(pHMHandleData,
                                  0x02, // EJECT media
                                  -1,
                                  lpBytesReturned);
    }
      
    case IOCTL_STORAGE_GET_MEDIA_TYPES:
         break;
      
    case IOCTL_STORAGE_LOAD_MEDIA:
    // case IOCTL_STORAGE_LOAD_MEDIA2:
    {
      dprintf(("Loading storage media"));
      return ioctlDISKUnlockEject(pHMHandleData,
                                  0x03, // LOAD media
                                  -1,
                                  lpBytesReturned);
    }
      
    // case IOCTL_STORAGE_EJECTION_CONTROL:
    case IOCTL_STORAGE_MEDIA_REMOVAL:
         break;
      
      
    // -------------------
    // SCSI passthru class
    // -------------------
      
    case IOCTL_SCSI_PASS_THROUGH:
    case IOCTL_SCSI_MINIPORT:
    case IOCTL_SCSI_GET_INQUIRY_DATA:
    case IOCTL_SCSI_GET_CAPABILITIES:
        break;

    case IOCTL_SCSI_PASS_THROUGH_DIRECT:
    {
        PSCSI_PASS_THROUGH_DIRECT pPacket = (PSCSI_PASS_THROUGH_DIRECT)lpOutBuffer;
        SRB_ExecSCSICmd *psrb;

        if(drvInfo->hInstAspi == NULL) 
        {
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }

        if(nOutBufferSize < sizeof(SCSI_PASS_THROUGH_DIRECT) ||
           !pPacket || pPacket->Length < sizeof(SCSI_PASS_THROUGH_DIRECT))
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }

        psrb = (SRB_ExecSCSICmd *)alloca(sizeof(SRB_ExecSCSICmd)+pPacket->SenseInfoLength);
        if(psrb == NULL) {
            dprintf(("not enough memory!!"));
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return FALSE;
        }
        memset(psrb, 0, sizeof(*psrb));
        psrb->SRB_Cmd        = SC_EXEC_SCSI_CMD;
        psrb->SRB_Status     = pPacket->ScsiStatus;
        psrb->SRB_HaId       = pPacket->PathId;
        psrb->SRB_Target     = pPacket->TargetId;
        psrb->SRB_Lun        = pPacket->Lun;
        psrb->SRB_BufLen     = pPacket->DataTransferLength;
        psrb->SRB_SenseLen   = pPacket->SenseInfoLength;
        psrb->SRB_CDBLen     = pPacket->CdbLength;
        switch(pPacket->DataIn) {
        case SCSI_IOCTL_DATA_OUT:
             psrb->SRB_Flags = 0x2 << 3;
             break;
        case SCSI_IOCTL_DATA_IN:
             psrb->SRB_Flags = 0x1 << 3;
             break;
        case SCSI_IOCTL_DATA_UNSPECIFIED:
             psrb->SRB_Flags = 0x3 << 3;
             break;
        }
        if(pPacket->CdbLength > 16) {
             SetLastError(ERROR_INVALID_PARAMETER);
             return FALSE;
        }
        dprintf(("IOCTL_SCSI_PASS_THROUGH_DIRECT %x len %x, %x%02x%02x%02x %x%02x", pPacket->Cdb[0], pPacket->DataTransferLength, pPacket->Cdb[2], pPacket->Cdb[3], pPacket->Cdb[4], pPacket->Cdb[5], pPacket->Cdb[7], pPacket->Cdb[8]));
        psrb->SRB_BufPointer = (BYTE *)pPacket->DataBuffer;
        memcpy(&psrb->CDBByte[0], &pPacket->Cdb[0], 16);
        if(psrb->SRB_SenseLen) {
            memcpy(&psrb->SenseArea[0], (char *)pPacket + pPacket->SenseInfoOffset, psrb->SRB_SenseLen);
        }
        //TODO: pPacket->TimeOutValue ignored
        int rc = drvInfo->SendASPI32Command((LPSRB)psrb);
        if(rc != SS_COMP) {
            dprintf(("SendASPI32Command failed with error %d", rc));
            if(rc == SS_ERR) {
                 SetLastError(ERROR_ADAP_HDW_ERR); //returned by NT4, SP6
            }
            else SetLastError(ERROR_GEN_FAILURE);
            return FALSE;
        }
        pPacket->ScsiStatus = rc;
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }
        pPacket->DataTransferLength = psrb->SRB_BufLen;
        if(psrb->SRB_SenseLen) {
            memcpy((char *)pPacket + pPacket->SenseInfoOffset, &psrb->SenseArea[0], psrb->SRB_SenseLen);
        }
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    case IOCTL_SCSI_GET_ADDRESS:
    {
        DWORD numAdapters, rc;
        SRB   srb;
        int   i, j, k;

        if(!lpOutBuffer || nOutBufferSize < 8) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);  //todo: right error?
            return(FALSE);
        }
        SCSI_ADDRESS *addr = (SCSI_ADDRESS *)lpOutBuffer;
        addr->Length = sizeof(SCSI_ADDRESS);
        addr->PortNumber = 0;
        addr->PathId     = 0;

        if(drvInfo->hInstAspi == NULL) {
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }

        numAdapters = drvInfo->GetASPI32SupportInfo();

        memset(&srb, 0, sizeof(srb));
        srb.common.SRB_Cmd = SC_HA_INQUIRY;
        rc = drvInfo->SendASPI32Command(&srb);

        char drivename[3];
        drivename[0] = (char)drvInfo->driveLetter;
        drivename[1] = ':';
        drivename[2] = 0;

        for(i=0;i<LOBYTE(numAdapters);i++) {
            for(j=0;j<8;j++) {
                for(k=0;k<16;k++) {
                    memset(&srb, 0, sizeof(srb));
                    srb.common.SRB_Cmd     = SC_GET_DEV_TYPE;
                    srb.devtype.SRB_HaId   = i;
                    srb.devtype.SRB_Target = j;
                    srb.devtype.SRB_Lun    = k;
                    rc = drvInfo->SendASPI32Command(&srb);
                    if(rc == SS_COMP) {
                        if(srb.devtype.SRB_DeviceType == SS_DEVTYPE_CDROM &&
                           GetDriveTypeA(drivename) == DRIVE_CDROM)
                        {
                            goto done;
                        }
                    }
                }
            }
        }
done:
        if(rc == SS_COMP) {
            addr->TargetId   = j;
            addr->Lun        = k;
            SetLastError(ERROR_SUCCESS);
        }
        else    SetLastError(ERROR_FILE_NOT_FOUND); //todo
        return TRUE;
failure:
        SetLastError(ERROR_INVALID_PARAMETER); //todo
        return FALSE;
    }

    case IOCTL_SCSI_RESCAN_BUS:
    case IOCTL_SCSI_GET_DUMP_POINTERS:
    case IOCTL_SCSI_FREE_DUMP_POINTERS:
    case IOCTL_IDE_PASS_THROUGH:
        break;

    }
    dprintf(("HMDeviceDiskClass::DeviceIoControl: unimplemented dwIoControlCode=%08lx\n", dwIoControlCode));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
/*****************************************************************************
 * Name      : BOOL HMDeviceDiskClass::ReadFile
 * Purpose   : read data from handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToRead,
 *             LPDWORD       lpNumberOfBytesRead,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceDiskClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                 LPCVOID       lpBuffer,
                                 DWORD         nNumberOfBytesToRead,
                                 LPDWORD       lpNumberOfBytesRead,
                                 LPOVERLAPPED  lpOverlapped)
{
  LPVOID       lpRealBuf;
  Win32MemMap *map;
  DWORD        offset, bytesread;
  BOOL         bRC;

  dprintf2(("KERNEL32: HMDeviceDiskClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));

  //SvL: It's legal for this pointer to be NULL
  if(lpNumberOfBytesRead)
    *lpNumberOfBytesRead = 0;
  else
    lpNumberOfBytesRead = &bytesread;

  if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
    dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  //If we didn't get an OS/2 handle for the disk before, get one now
  if(!pHMHandleData->hHMHandle) {
      DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
      pHMHandleData->hHMHandle = OpenDisk(drvInfo);
      if(!pHMHandleData->hHMHandle) {
          dprintf(("No disk inserted; aborting"));
          SetLastError(ERROR_NOT_READY);
          return -1;
      }
  }

  if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
    dprintf(("Warning: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
  }

  //SvL: DosRead doesn't like writing to memory addresses returned by
  //     DosAliasMem -> search for original memory mapped pointer and use
  //     that one + commit pages if not already present
  map = Win32MemMapView::findMapByView((ULONG)lpBuffer, &offset, MEMMAP_ACCESS_WRITE);
  if(map) {
    lpRealBuf = (LPVOID)((ULONG)map->getMappingAddr() + offset);
    DWORD nrpages = nNumberOfBytesToRead/4096;
    if(offset & 0xfff)
        nrpages++;
    if(nNumberOfBytesToRead & 0xfff)
        nrpages++;

    map->commitPage(offset & ~0xfff, TRUE, nrpages);
  }
  else  lpRealBuf = (LPVOID)lpBuffer;

  if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) {
    dprintf(("ERROR: Overlapped IO not yet implememented!!"));
  }
//  else {
    bRC = OSLibDosRead(pHMHandleData->hHMHandle,
                           (PVOID)lpRealBuf,
                           nNumberOfBytesToRead,
                           lpNumberOfBytesRead);
//  }

  if(bRC == 0) {
      dprintf(("KERNEL32: HMDeviceDiskClass::ReadFile returned %08xh %x", bRC, GetLastError()));
      dprintf(("%x -> %d", lpBuffer, IsBadWritePtr((LPVOID)lpBuffer, nNumberOfBytesToRead)));
  }
  else dprintf2(("KERNEL32: HMDeviceDiskClass::ReadFile read %x bytes pos %x", *lpNumberOfBytesRead, SetFilePointer(pHMHandleData, 0, NULL, FILE_CURRENT)));

  return bRC;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceDiskClass::SetFilePointer
 * Purpose   : set file pointer
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LONG          lDistanceToMove
 *             PLONG         lpDistanceToMoveHigh
 *             DWORD         dwMoveMethod
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceDiskClass::SetFilePointer(PHMHANDLEDATA pHMHandleData,
                                        LONG          lDistanceToMove,
                                        PLONG         lpDistanceToMoveHigh,
                                        DWORD         dwMoveMethod)
{
  DWORD ret;

  dprintf2(("KERNEL32: HMDeviceDiskClass::SetFilePointer %s(%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           lDistanceToMove,
           lpDistanceToMoveHigh,
           dwMoveMethod));

  if(!pHMHandleData->hHMHandle) {
      DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
      pHMHandleData->hHMHandle = OpenDisk(drvInfo);
      if(!pHMHandleData->hHMHandle) {
          dprintf(("No disk inserted; aborting"));
          SetLastError(ERROR_NOT_READY);
          return -1;
      }
  }

  ret = OSLibDosSetFilePointer(pHMHandleData->hHMHandle,
                               lDistanceToMove,
                               (DWORD *)lpDistanceToMoveHigh,
                               dwMoveMethod);

  if(ret == -1) {
      dprintf(("SetFilePointer failed (error = %d)", GetLastError()));
  }
  return ret;
}
/*****************************************************************************
 * Name      : BOOL ReadFileEx
 * Purpose   : The ReadFileEx function reads data from a file asynchronously.
 *             It is designed solely for asynchronous operation, unlike the
 *             ReadFile function, which is designed for both synchronous and
 *             asynchronous operation. ReadFileEx lets an application perform
 *             other processing during a file read operation.
 *             The ReadFileEx function reports its completion status asynchronously,
 *             calling a specified completion routine when reading is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to read
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to read
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/
BOOL HMDeviceDiskClass::ReadFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToRead,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: ReadFileEx(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpOverlapped,
           lpCompletionRoutine));
  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL HMDeviceDiskClass::WriteFile
 * Purpose   : write data to handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToWrite,
 *             LPDWORD       lpNumberOfBytesWritten,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceDiskClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToWrite,
                                    LPDWORD       lpNumberOfBytesWritten,
                                    LPOVERLAPPED  lpOverlapped)
{
  LPVOID       lpRealBuf;
  Win32MemMap *map;
  DWORD        offset, byteswritten;
  BOOL         bRC;

  dprintf2(("KERNEL32: HMDeviceDiskClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

  DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
  if(drvInfo == NULL) {
      dprintf(("ERROR: DeviceIoControl: drvInfo == NULL!!!"));
      DebugInt3();
      SetLastError(ERROR_INVALID_HANDLE);
      return FALSE;
  }

  //SvL: It's legal for this pointer to be NULL
  if(lpNumberOfBytesWritten)
    *lpNumberOfBytesWritten = 0;
  else
    lpNumberOfBytesWritten = &byteswritten;

  if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
    dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
    dprintf(("Warning: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
  }

  //If we didn't get an OS/2 handle for the disk before, get one now
  if(!pHMHandleData->hHMHandle) {
      DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
      pHMHandleData->hHMHandle = OpenDisk(drvInfo);
      if(!pHMHandleData->hHMHandle) {
          dprintf(("No disk inserted; aborting"));
          SetLastError(ERROR_NOT_READY);
          return -1;
      }
  }
  //NOTE: For now only allow an application to write to drive A
  //      Might want to extend this to all removable media, but it's
  //      too dangerous to allow win32 apps to write to the harddisk directly
  if(drvInfo->driveLetter != 'A') {
      SetLastError(ERROR_ACCESS_DENIED);
      return -1;
  }


  //SvL: DosWrite doesn't like reading from memory addresses returned by
  //     DosAliasMem -> search for original memory mapped pointer and use
  //     that one + commit pages if not already present
  map = Win32MemMapView::findMapByView((ULONG)lpBuffer, &offset, MEMMAP_ACCESS_READ);
  if(map) {
    lpRealBuf = (LPVOID)((ULONG)map->getMappingAddr() + offset);
    DWORD nrpages = nNumberOfBytesToWrite/4096;
    if(offset & 0xfff)
        nrpages++;
    if(nNumberOfBytesToWrite & 0xfff)
        nrpages++;

    map->commitPage(offset & ~0xfff, TRUE, nrpages);
  }
  else  lpRealBuf = (LPVOID)lpBuffer;

  if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) {
    dprintf(("ERROR: Overlapped IO not yet implememented!!"));
  }
//  else {
    bRC = OSLibDosWrite(pHMHandleData->hHMHandle,
                            (PVOID)lpRealBuf,
                            nNumberOfBytesToWrite,
                            lpNumberOfBytesWritten);
//  }

  dprintf2(("KERNEL32: HMDeviceDiskClass::WriteFile returned %08xh\n",
           bRC));

  return bRC;
}

/*****************************************************************************
 * Name      : BOOL WriteFileEx
 * Purpose   : The WriteFileEx function writes data to a file. It is designed
 *             solely for asynchronous operation, unlike WriteFile, which is
 *             designed for both synchronous and asynchronous operation.
 *             WriteFileEx reports its completion status asynchronously,
 *             calling a specified completion routine when writing is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to write
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to write
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL HMDeviceDiskClass::WriteFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToWrite,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: WriteFileEx(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpOverlapped,
           lpCompletionRoutine));
  return FALSE;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceDiskClass::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceDiskClass::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintf2(("KERNEL32: HMDeviceDiskClass::GetFileType %s(%08x)\n",
             lpHMDeviceName,
             pHMHandleData));

  return FILE_TYPE_DISK;
}
//******************************************************************************
//******************************************************************************
