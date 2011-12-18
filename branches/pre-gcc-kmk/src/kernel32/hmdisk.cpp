/* $Id: hmdisk.cpp,v 1.62 2003-03-06 10:44:33 sandervl Exp $ */

/*
 * Win32 Disk API functions for OS/2
 *
 * Copyright 2000-2002 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <string.h>
#include <stdio.h>
#include <versionos2.h>

#include <misc.h>
#include "hmdisk.h"
#include "mmap.h"
#include <win\winioctl.h>
#include <win\ntddscsi.h>
#include <win\wnaspi32.h>
#include "oslibdos.h"
#include "osliblvm.h"
#include "oslibcdio.h"
#include "asmutil.h"
#include <custombuild.h>

#define DBG_LOCALLOG    DBG_hmdisk
#include "dbglocal.h"

#define BIT_0     (1)
#define BIT_1     (2)
#define BIT_2     (4)
#define BIT_11    (1<<11)

//Converts BCD to decimal; doesn't check for illegal BCD nrs
#define BCDToDec(a) ((a >> 4) * 10 + (a & 0xF))


#define UNMOUNTED_VOLUME(a) (a->fPhysicalDisk && (a->StartingOffset.HighPart != 0 || a->StartingOffset.LowPart != 0))

typedef struct
{
    BOOL      fCDIoSupported;    
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
    BOOL      fPhysicalDisk;
    DWORD     dwPhysicalDiskNr;
    BOOL      fCDPlaying;
    BOOL      fShareViolation;
    DWORD     fLocked;
    LARGE_INTEGER StartingOffset;
    LARGE_INTEGER PartitionSize;
    LARGE_INTEGER CurrentFilePointer;
    CHAR      szVolumeName[256];
} DRIVE_INFO;

static BOOL fPhysicalDiskAccess = FALSE;

//******************************************************************************
//******************************************************************************
void WIN32API EnablePhysicalDiskAccess(BOOL fEnable)
{
    fPhysicalDiskAccess = fEnable;
}
//******************************************************************************
//******************************************************************************
HMDeviceDiskClass::HMDeviceDiskClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName)
{
    HMDeviceRegisterEx("\\\\.\\PHYSICALDRIVE", this, NULL);
    HMDeviceRegisterEx(VOLUME_NAME_PREFIX, this, NULL);
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
    // check for "x:"
    if (namelength == 2)
    {
        if (lpDeviceName[1] != ':')
            return FALSE;

        if (!( ((lpDeviceName[0] >= 'A') &&
                (lpDeviceName[0] <= 'Z')) ||
               ((lpDeviceName[0] >= 'a') &&
                (lpDeviceName[0] <= 'z')) ))
            return FALSE;

        return TRUE;
    }

    //\\.\x:                -> length 6
    //\\.\PHYSICALDRIVEn    -> length 18
    if(namelength != 6 && namelength != 18) {
        if(VERSION_IS_WIN2000_OR_HIGHER()) {
            if(!strncmp(lpDeviceName, VOLUME_NAME_PREFIX, sizeof(VOLUME_NAME_PREFIX)-1)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    // \\.\x:             -> drive x (i.e. \\.\C:)
    // \\.\PHYSICALDRIVEn -> drive n (n>=0)
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
//******************************************************************************
DWORD HMDeviceDiskClass::CreateFile (LPCSTR        lpFileName,
                                     PHMHANDLEDATA pHMHandleData,
                                     PVOID         lpSecurityAttributes,
                                     PHMHANDLEDATA pHMHandleDataTemplate)
{
    HFILE               hFile;
    HFILE               hTemplate;
    DWORD               dwDriveType;
    CHAR                szDiskName[256];
    CHAR                szVolumeName[256] = "";
    VOLUME_DISK_EXTENTS volext = {0};
    BOOL                fPhysicalDisk = FALSE;
    DWORD               dwPhysicalDiskNr = 0;

    dprintf2(("KERNEL32: HMDeviceDiskClass::CreateFile %s(%s,%08x,%08x,%08x)\n",
             lpHMDeviceName, lpFileName, pHMHandleData, lpSecurityAttributes, pHMHandleDataTemplate));

    //TODO: check in NT if CREATE_ALWAYS is allowed!!
    if(pHMHandleData->dwCreation != OPEN_EXISTING) {
        dprintf(("Invalid creation flags %x!!", pHMHandleData->dwCreation));
        return ERROR_INVALID_PARAMETER;
    }

    char szDrive[4];
    szDrive[1] = ':';
    szDrive[2] = '\0';

    //if volume name, query 
    if(!strncmp(lpFileName, VOLUME_NAME_PREFIX, sizeof(VOLUME_NAME_PREFIX)-1)) 
    {
        int length;

        if(!VERSION_IS_WIN2000_OR_HIGHER() || !fPhysicalDiskAccess) {
            return ERROR_FILE_NOT_FOUND;    //not allowed
        }
        if(OSLibLVMStripVolumeName(lpFileName, szVolumeName, sizeof(szVolumeName))) 
        {
            BOOL fLVMVolume;

            dwDriveType = GetDriveTypeA(lpFileName);

            szDrive[0] = OSLibLVMQueryDriveFromVolumeName(szVolumeName);
            if(szDrive[0] == -1) {
                return ERROR_FILE_NOT_FOUND;    //not found
            }
            if((dwDriveType == DRIVE_FIXED) && OSLibLVMGetVolumeExtents(szDrive[0], szVolumeName, &volext, &fLVMVolume) == FALSE) {
                return ERROR_FILE_NOT_FOUND;    //not found
            }
            if(szDrive[0] == 0) 
            {
                //volume isn't mounted
                
                //Note: this only works on Warp 4.5 and up
                sprintf(szDiskName, "\\\\.\\Physical_Disk%d", volext.Extents[0].DiskNumber+1); 
                fPhysicalDisk    = TRUE;
                dwPhysicalDiskNr = volext.Extents[0].DiskNumber + 1;

                if(fLVMVolume && (pHMHandleData->dwAccess & GENERIC_WRITE)) {
                    //no write access allowed for LVM volumes
                    dprintf(("CreateFile: WARNING: Write access to LVM volume denied!!"));
                    pHMHandleData->dwAccess &= ~GENERIC_WRITE;
                }
            }
            else {
                //mounted drive, make sure access requested is readonly, else fail
                if(pHMHandleData->dwAccess & GENERIC_WRITE) {
                    //no write access allowed for mounted partitions
                    dprintf(("CreateFile: WARNING: Write access to mounted partition denied!!"));
                    pHMHandleData->dwAccess &= ~GENERIC_WRITE;
                }
                strcpy(szDiskName, szDrive);
            }
        }
        else return ERROR_FILE_NOT_FOUND;
    }
    else 
    if(strncmp(lpFileName, "\\\\.\\PHYSICALDRIVE", 17) == 0) 
    {
        if(!fPhysicalDiskAccess) {
            return ERROR_FILE_NOT_FOUND;    //not allowed
        }

        //Note: this only works on Warp 4.5 and up
        sprintf(szDiskName, "\\\\.\\Physical_Disk%c", lpFileName[17]+1);
        fPhysicalDisk    = TRUE;
        dwPhysicalDiskNr = (DWORD)(lpFileName[17] - '0')+1;

        //TODO: could be removable in theory
        dwDriveType = DRIVE_FIXED;

        if(pHMHandleData->dwAccess & GENERIC_WRITE) {
            //no write access allowed for whole disks
            dprintf(("CreateFile: WARNING: Write access to whole disk denied!!"));
            pHMHandleData->dwAccess &= ~GENERIC_WRITE;
        }
    }
    else {
        strcpy(szDiskName, lpFileName);
        szDrive[0] = *lpFileName;
        dwDriveType = GetDriveTypeA(szDrive);
        if(dwDriveType == DRIVE_DOESNOTEXIST) {
            //If the drive doesn't exist, then fail right here
            dprintf(("Drive %s does not exist; fail", szDrive));
            return ERROR_INVALID_DRIVE; //right error??
        }
    }

    //Disable error popus. NT allows an app to open a cdrom/dvd drive without a disk inside
    //OS/2 fails in that case with error ERROR_NOT_READY
    ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS);
    hFile = OSLibDosCreateFile(szDiskName,
                               pHMHandleData->dwAccess,
                               pHMHandleData->dwShare,
                               (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                               pHMHandleData->dwCreation,
                               pHMHandleData->dwFlags,
                               hTemplate);

    //It is not allowed to open a readonly device with GENERIC_WRITE in OS/2;
    //try with readonly again if that happened
    //NOTE: Some applications open it with GENERIC_WRITE as Windows 2000 requires
    //      this for some aspi functions
    if(hFile == INVALID_HANDLE_ERROR && dwDriveType == DRIVE_CDROM &&
       (pHMHandleData->dwAccess & GENERIC_WRITE))
    {
        pHMHandleData->dwAccess &= ~GENERIC_WRITE;
        hFile = OSLibDosCreateFile((LPSTR)szDiskName,
                                   pHMHandleData->dwAccess,
                                   pHMHandleData->dwShare,
                                   (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                                   pHMHandleData->dwCreation,
                                   pHMHandleData->dwFlags,
                                   hTemplate);
    }
    SetErrorMode(oldmode);

    DWORD lasterror = GetLastError();
    if(hFile != INVALID_HANDLE_ERROR || lasterror == ERROR_NOT_READY ||
       lasterror == ERROR_SHARING_VIOLATION)
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

        if(lasterror == ERROR_SHARING_VIOLATION) {
            drvInfo->fShareViolation = TRUE;
        }

        memset(drvInfo, 0, sizeof(DRIVE_INFO));
        drvInfo->dwAccess  = pHMHandleData->dwAccess;
        drvInfo->dwShare   = pHMHandleData->dwShare;
        drvInfo->lpSecurityAttributes  = (LPSECURITY_ATTRIBUTES)lpSecurityAttributes;
        drvInfo->dwCreation= pHMHandleData->dwCreation;
        drvInfo->dwFlags   = pHMHandleData->dwFlags;
        drvInfo->hTemplate = hTemplate;
        drvInfo->fLocked   = FALSE;

        //save volume start & length if volume must be accessed through the physical disk 
        //(no other choice for unmounted volumes)
        drvInfo->fPhysicalDisk    = fPhysicalDisk;
        drvInfo->dwPhysicalDiskNr = dwPhysicalDiskNr;
        drvInfo->StartingOffset   = volext.Extents[0].StartingOffset;
        drvInfo->CurrentFilePointer = drvInfo->StartingOffset;
        drvInfo->PartitionSize    = volext.Extents[0].ExtentLength;

        //save volume name for later (IOCtls)
        strncpy(drvInfo->szVolumeName, szVolumeName, sizeof(drvInfo->szVolumeName)-1);

        drvInfo->driveLetter = *lpFileName; //save drive letter
        if(drvInfo->driveLetter >= 'a') {
            drvInfo->driveLetter = drvInfo->driveLetter - ((int)'a' - (int)'A');
        }

        drvInfo->driveType = dwDriveType;
        if(drvInfo->driveType == DRIVE_CDROM)
        {
            drvInfo->fCDIoSupported = OSLibCdIoIsSupported(pHMHandleData->hHMHandle);

            //get cdrom signature
            DWORD parsize = 4;
            DWORD datasize = 4;
            strcpy(drvInfo->signature, "CD01");
            OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x80, 0x61, &drvInfo->signature[0], 4, &parsize,
                             &drvInfo->signature[0], 4, &datasize);
        }

        if(hFile && drvInfo->driveType != DRIVE_FIXED) {
            OSLibDosQueryVolumeSerialAndName(1 + drvInfo->driveLetter - 'A', &drvInfo->dwVolumelabel, NULL, 0);
        }

        //for an unmounted partition we open the physical disk that contains it, so we
        //must set the file pointer to the correct beginning
        if(drvInfo->fPhysicalDisk && (drvInfo->StartingOffset.HighPart != 0 ||
           drvInfo->StartingOffset.LowPart != 0))
        {
            SetFilePointer(pHMHandleData, 0, NULL, FILE_BEGIN);
        }
        //If the disk handle was valid, then we must lock it if the drive
        //was opened without share flags
        if(pHMHandleData->hHMHandle && drvInfo->dwShare == 0) {
            dprintf(("Locking drive"));
            if(OSLibDosDevIOCtl(pHMHandleData->hHMHandle,IOCTL_DISK,DSK_LOCKDRIVE,0,0,0,0,0,0)) 
            {
                dprintf(("Sharing violation while attempting to lock the drive"));
                OSLibDosClose(pHMHandleData->hHMHandle);
                free(drvInfo);
                return ERROR_SHARING_VIOLATION;
            }
            drvInfo->fLocked = TRUE;
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

    filename[0] = drvInfo->driveLetter;
    filename[1] = ':';
    filename[2] = 0;

    //Disable error popus. NT allows an app to open a cdrom/dvd drive without a disk inside
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
        if(drvInfo->driveType == DRIVE_CDROM)
        {
            drvInfo->fCDIoSupported = OSLibCdIoIsSupported(hFile);

            //get cdrom signature
            DWORD parsize = 4;
            DWORD datasize = 4;
            strcpy(drvInfo->signature, "CD01");
            OSLibDosDevIOCtl(hFile, 0x80, 0x61, &drvInfo->signature[0], 4, &parsize,
                             &drvInfo->signature[0], 4, &datasize);
        }
        OSLibDosQueryVolumeSerialAndName(1 + drvInfo->driveLetter - 'A', &drvInfo->dwVolumelabel, NULL, 0);

        //If the disk handle was valid, then we must lock it if the drive
        //was opened without share flags
        if(hFile && drvInfo->dwShare == 0) {
            dprintf(("Locking drive"));
            if(OSLibDosDevIOCtl(hFile,IOCTL_DISK,DSK_LOCKDRIVE,0,0,0,0,0,0)) 
            {
                dprintf(("Sharing violation while attempting to lock the drive"));
                OSLibDosClose(hFile);
                return 0;
            }
            drvInfo->fLocked = TRUE;
        }
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
        DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
        if(drvInfo && drvInfo->fLocked) {
            dprintf(("Unlocking drive"));
            OSLibDosDevIOCtl(pHMHandleData->hHMHandle,IOCTL_DISK,DSK_UNLOCKDRIVE,0,0,0,0,0,0);
        }
        ret = OSLibDosClose(pHMHandleData->hHMHandle);
    }
    if(pHMHandleData->dwUserData) {
       DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
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
  return (ret == ERROR_SUCCESS);
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
                         IOCTL_DISK,
                         DSK_UNLOCKEJECTMEDIA,
                         &ParameterBlock,
                         sizeof( ParameterBlock ),
                         &dwParameterSize,
                         NULL,
                         0,
                         &dwDataSize);
  return (ret == ERROR_SUCCESS);
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
    case IOCTL_DISK_IS_WRITABLE:
        msg = "IOCTL_DISK_IS_WRITABLE";
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
    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
        msg = "IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS";
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

    case IOCTL_DISK_IS_WRITABLE:
    {
        APIRET rc;
        DWORD  ret;
        ULONG  ulBytesRead    = 0;      /* Number of bytes read by DosRead */
        ULONG  ulWrote        = 0;      /* Number of bytes written by DosWrite */
        ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
        UCHAR  uchFileData[1] = {'0'};    /* Data to write to file */

        if(!pHMHandleData->hHMHandle)
        {
            pHMHandleData->hHMHandle = OpenDisk(drvInfo);
            if(!pHMHandleData->hHMHandle) {
                dprintf(("No disk inserted; aborting"));
                SetLastError((drvInfo->fShareViolation) ? ERROR_SHARING_VIOLATION : ERROR_NOT_READY);
                return FALSE;
            }
        }
        if(drvInfo->driveType == DRIVE_CDROM) {
            //TODO: check behaviour in NT
            SetLastError(ERROR_WRITE_PROTECT);
            return FALSE;
        }
        else
        if(drvInfo->driveType == DRIVE_FIXED) {
            SetLastError(ERROR_SUCCESS);
            return TRUE;
        }

        ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS);

        /* Read the first byte of the disk */
        rc = OSLibDosRead(pHMHandleData->hHMHandle,    /* File Handle */
                          uchFileData,                 /* String to be read */
                          1L,                          /* Length of string to be read */
                          &ulBytesRead);               /* Bytes actually read */

        if (rc == 0)
        {
            dprintf(("IOCTL_DISK_IS_WRITABLE:OSLibDosRead failed with rc %08xh %x", rc,GetLastError()));
            SetLastError(ERROR_ACCESS_DENIED);
            goto writecheckfail;
        }

        /* Move the file pointer back */
        rc = OSLibDosSetFilePtr (pHMHandleData->hHMHandle,           /* File Handle */
                                 -1,OSLIB_SETPTR_FILE_CURRENT);
        if (rc == -1)
        {
            dprintf(("IOCTL_DISK_IS_WRITABLE:OSLibDosSetFilePtr failed with rc %d", rc));
            SetLastError(ERROR_ACCESS_DENIED);
            goto writecheckfail;
        }

        rc =  OSLibDosWrite(pHMHandleData->hHMHandle,   /* File handle */
                            (PVOID) uchFileData,        /* String to be written */
                            1,                          /* Size of string to be written */
                            &ulWrote);                  /* Bytes actually written */

        dprintf2(("IOCTL_DISK_IS_WRITABLE:OSLibDosWrite returned with rc %x %x", rc,GetLastError()));
        if (rc == 0)
        {
           if (GetLastError() == ERROR_WRITE_PROTECT)
           {
               SetLastError(ERROR_WRITE_PROTECT);
               goto writecheckfail;
           }
        }
        SetErrorMode(oldmode);
        SetLastError(ERROR_SUCCESS);
        return TRUE;

writecheckfail:
        SetErrorMode(oldmode);
        return FALSE;
    }
    //Basically the same as IOCTL_DISK_GET_DRIVE_GEOMETRY, but these two ioctls
    //are supposed to work even without media in the drive
    case IOCTL_STORAGE_GET_MEDIA_TYPES:
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
        if(!pHMHandleData->hHMHandle) {
            pHMHandleData->hHMHandle = OpenDisk(drvInfo);
            //we don't care if there's a disk present or not
        }

        if(OSLibDosGetDiskGeometry(pHMHandleData->hHMHandle, drvInfo->driveLetter, pGeom) == FALSE) {
            dprintf(("!ERROR!: IOCTL_DISK_GET_MEDIA_TYPES: OSLibDosGetDiskGeometry failed!!"));
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = sizeof(DISK_GEOMETRY);
        }
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    //This ioctl is different from IOCTL_DISK_GET_MEDIA_TYPES; some applications
    //use it to determine if a disk is present or whether a media change has
    //occurred
    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
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
                SetLastError((drvInfo->fShareViolation) ? ERROR_SHARING_VIOLATION : ERROR_NOT_READY);
                return FALSE;
            }
        }

        //Applications can use this IOCTL to check if the floppy has been changed
        //OSLibDosGetDiskGeometry won't fail when that happens so we read the
        //volume label from the disk and return ERROR_MEDIA_CHANGED if the volume
        //label has changed
        //TODO: Find better way to determine if floppy was removed or switched
        if(drvInfo->driveType != DRIVE_FIXED) 
        {
            rc = OSLibDosQueryVolumeSerialAndName(1 + drvInfo->driveLetter - 'A', &volumelabel, NULL, 0);
            if(rc) {
                dprintf(("IOCTL_DISK_GET_DRIVE_GEOMETRY: OSLibDosQueryVolumeSerialAndName failed with rc %d", GetLastError()));
                if(pHMHandleData->hHMHandle) {
                    if(drvInfo->fLocked) {
                        dprintf(("Unlocking drive"));
                        OSLibDosDevIOCtl(pHMHandleData->hHMHandle,IOCTL_DISK,DSK_UNLOCKDRIVE,0,0,0,0,0,0);
                        drvInfo->fLocked = FALSE;
                    }
                    OSLibDosClose(pHMHandleData->hHMHandle);
                }
                pHMHandleData->hHMHandle = 0;
                SetLastError(ERROR_MEDIA_CHANGED);
                return FALSE;
            }
            if(volumelabel != drvInfo->dwVolumelabel) {
                dprintf(("IOCTL_DISK_GET_DRIVE_GEOMETRY: volume changed %x -> %x", drvInfo->dwVolumelabel, volumelabel));
                SetLastError(ERROR_MEDIA_CHANGED);
                return FALSE;
            }
        }

        if(drvInfo->fPhysicalDisk) {
            if(OSLibLVMGetDiskGeometry(drvInfo->dwPhysicalDiskNr, pGeom) == FALSE) {
                dprintf(("!ERROR!: IOCTL_DISK_GET_DRIVE_GEOMETRY: OSLibDosGetDiskGeometry failed!!"));
                return FALSE;
            }
        }
        else {
            if(OSLibDosGetDiskGeometry(pHMHandleData->hHMHandle, drvInfo->driveLetter, pGeom) == FALSE) {
                dprintf(("!ERROR!: IOCTL_DISK_GET_DRIVE_GEOMETRY: OSLibDosGetDiskGeometry failed!!"));
                return FALSE;
            }
        }
        dprintf(("Cylinders         %d", pGeom->Cylinders));
        dprintf(("TracksPerCylinder %d", pGeom->TracksPerCylinder));
        dprintf(("SectorsPerTrack   %d", pGeom->SectorsPerTrack));
        dprintf(("BytesPerSector    %d", pGeom->BytesPerSector));
        dprintf(("MediaType         %d", pGeom->MediaType));
        if(lpBytesReturned) {
            *lpBytesReturned = sizeof(DISK_GEOMETRY);
        }
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    case IOCTL_DISK_GET_PARTITION_INFO:
    {
        PPARTITION_INFORMATION pPartition = (PPARTITION_INFORMATION)lpOutBuffer;
        if(nOutBufferSize < sizeof(PARTITION_INFORMATION) || !pPartition) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = sizeof(PARTITION_INFORMATION);
        }
        if(OSLibLVMGetPartitionInfo(drvInfo->driveLetter, drvInfo->szVolumeName, pPartition) == FALSE) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY); //wrong error, but who cares
            return FALSE;
        }

        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    case IOCTL_DISK_LOAD_MEDIA:
    case IOCTL_DISK_MEDIA_REMOVAL:
    case IOCTL_DISK_PERFORMANCE:
    case IOCTL_DISK_REASSIGN_BLOCKS:
    case IOCTL_DISK_SET_DRIVE_LAYOUT:
    case IOCTL_DISK_SET_PARTITION_INFO:
    case IOCTL_DISK_VERIFY:
    case IOCTL_SERIAL_LSRMST_INSERT:
        break;


    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
    {
        PVOLUME_DISK_EXTENTS pVolExtent = (PVOLUME_DISK_EXTENTS)lpOutBuffer;
        if(nOutBufferSize < sizeof(VOLUME_DISK_EXTENTS) || !pVolExtent) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(OSLibLVMGetVolumeExtents(drvInfo->driveLetter, drvInfo->szVolumeName, pVolExtent) == FALSE) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY); //wrong error, but who cares
            return FALSE;
        }

        if(lpBytesReturned) {
            *lpBytesReturned = sizeof(VOLUME_DISK_EXTENTS);
        }
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

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
            parm.ucTrack = pTOC->FirstTrack+i;

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
            dprintf(("IOCTL_CDROM_READ_TOC track %d Control %d Adr %d address %x", pTOC->FirstTrack+i, pTOC->TrackData[i].Control, pTOC->TrackData[i].Adr, trackinfo.ulTrackAddr));
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

      if(nInBufferSize < sizeof(CDROM_SEEK_AUDIO_MSF)) {
          SetLastError(ERROR_INSUFFICIENT_BUFFER);
          return FALSE;
      }
      if(lpBytesReturned)
        *lpBytesReturned = 0;

      dprintf(("Play CDROM audio playback %d:%d (%d) - %d:%d (%d)", pPlay->StartingM, pPlay->StartingS, pPlay->StartingF, pPlay->EndingM, pPlay->EndingS, pPlay->EndingF));

      // setup the parameter block
      memcpy(&ParameterBlock.ucSignature, drvInfo->signature, 4);
      ParameterBlock.ucAddressingMode = 1;     // MSF format

      ParameterBlock.ulStartingMSF    = pPlay->StartingM << 16 |
                                        pPlay->StartingS << 8  |
                                        pPlay->StartingF;
      ParameterBlock.ulEndingMSF      = pPlay->EndingM << 16 |
                                        pPlay->EndingS << 8  |
                                        pPlay->EndingF;

      DWORD dwParameterSize = sizeof( ParameterBlock );
      DWORD dwDataSize      = 0;
      DWORD ret;

      ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                             0x81,  // IOCTL_CDROMAUDIO
                             0x50,  // CDROMAUDIO_PLAYAUDIO
                             &ParameterBlock,
                             sizeof( ParameterBlock ),
                             &dwParameterSize,
                             NULL,
                             0,
                             &dwDataSize);
      if(ret != ERROR_SUCCESS) {
          dprintf(("IOCTL_CDROMAUDIO, CDROMAUDIO_PLAYAUDIO failed!!"));
      }
      drvInfo->fCDPlaying = TRUE;
      return (ret == ERROR_SUCCESS);
    }

    case IOCTL_CDROM_SEEK_AUDIO_MSF:
    {
#pragma pack(1)
      struct
      {
        DWORD ucSignature;
        BYTE  ucAddressingMode;
        DWORD ulStartingMSF;
      } ParameterBlock;
#pragma pack()
        CDROM_SEEK_AUDIO_MSF *pSeek = (CDROM_SEEK_AUDIO_MSF *)lpInBuffer;

        if(nInBufferSize < sizeof(CDROM_SEEK_AUDIO_MSF)) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(lpBytesReturned)
          *lpBytesReturned = 0;

        dprintf(("IOCTL_CDROMDISK, CDROMDISK_SEEK %d:%d (%d)", pSeek->M, pSeek->S, pSeek->F));

        // setup the parameter block
        memcpy(&ParameterBlock.ucSignature, drvInfo->signature, 4);
        ParameterBlock.ucAddressingMode = 1;     // MSF format

        ParameterBlock.ulStartingMSF    = pSeek->M << 16 |
                                          pSeek->S << 8  |
                                          pSeek->F;

        DWORD dwParameterSize = sizeof( ParameterBlock );
        DWORD dwDataSize      = 0;
        DWORD ret;

        ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                               0x80,  // IOCTL_CDROMDISK
                               0x50,  // CDROMDISK_SEEK
                               &ParameterBlock,
                               sizeof( ParameterBlock ),
                               &dwParameterSize,
                               NULL,
                               0,
                               &dwDataSize);

        //@@PF Windows IOCTL pause playback before seeking and re-seek
        if ( ret == ERROR_DEVICE_IN_USE && drvInfo->fCDPlaying)
        {

        ioctlCDROMSimple(pHMHandleData,
                              0x81,   // IOCTL_CDROMAUDIO
                              0x51,   // CDROMAUDIO_STOPAUDIO
                              lpBytesReturned, drvInfo);

        drvInfo->fCDPlaying = FALSE;

        ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                               0x80,  // IOCTL_CDROMDISK
                               0x50,  // CDROMDISK_SEEK
                               &ParameterBlock,
                               sizeof( ParameterBlock ),
                               &dwParameterSize,
                               NULL,
                               0,
                               &dwDataSize);
        }

        if(ret != ERROR_SUCCESS) {
            dprintf(("IOCTL_CDROMDISK, CDROMDISK_SEEK %x failed with rc= %x !!", ParameterBlock.ulStartingMSF,ret));
        }
        return (ret == ERROR_SUCCESS);
    }

    case IOCTL_CDROM_PAUSE_AUDIO:
      // NO BREAK CASE
      // Note: for OS/2, pause and stop seems to be the same!

    case IOCTL_CDROM_STOP_AUDIO:
    {
      dprintf(("Stop / pause CDROM audio playback"));
      drvInfo->fCDPlaying = FALSE;
      return ioctlCDROMSimple(pHMHandleData,
                              0x81,   // IOCTL_CDROMAUDIO
                              0x51,   // CDROMAUDIO_STOPAUDIO
                              lpBytesReturned, drvInfo);
    }

    case IOCTL_CDROM_RESUME_AUDIO:
    {
      dprintf(("Resume CDROM audio playback"));
      drvInfo->fCDPlaying = TRUE;
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

        if(ret)
            return FALSE;

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

        if(ret)
            return FALSE;

        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    case IOCTL_CDROM_READ_Q_CHANNEL:
    {
#pragma pack(1)
      struct
      {
        BYTE  ucControlAddr;
        BYTE  ucTrackNr;
        BYTE  ucIndex;
        BYTE  ucRuntimeTrackMin;
        BYTE  ucRuntimeTrackSec;
        BYTE  ucRuntimeTrackFrame;
        BYTE  ucReserved;
        BYTE  ucRuntimeDiscMin;
        BYTE  ucRuntimeDiscSec;
        BYTE  ucRuntimeDiscFrame;
      } DataBlock;
      struct {
        WORD  usAudioStatus;
        DWORD ulStartLoc;
        DWORD ulEndLoc;
      } DataBlockStatus;
#pragma pack()
        CDROM_SUB_Q_DATA_FORMAT *pFormat = (CDROM_SUB_Q_DATA_FORMAT*)lpInBuffer;
        SUB_Q_CHANNEL_DATA      *pChannelData = (SUB_Q_CHANNEL_DATA *)lpOutBuffer;
        char                     signature[8];

        if(nInBufferSize < sizeof(CDROM_SUB_Q_DATA_FORMAT) || !pFormat) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(nOutBufferSize < sizeof(SUB_Q_CHANNEL_DATA) || !pChannelData) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }

        DWORD dwParameterSize = 4;
        DWORD dwDataSize      = sizeof(DataBlock);
        DWORD ret;

        memcpy(signature, drvInfo->signature, dwParameterSize);

        ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                               0x81,  // IOCTL_CDROMAUDIO
                               0x63,  // CDROMAUDIO_GETSUBCHANNELQ
                               signature,
                               dwParameterSize,
                               &dwParameterSize,
                               &DataBlock,
                               sizeof(DataBlock),
                               &dwDataSize);
        if(ret != ERROR_SUCCESS) {
            dprintf(("IOCTL_CDROMAUDIO, CDROMAUDIO_GETSUBCHANNELQ failed!!"));
            return FALSE;
        }

        dwDataSize = sizeof(DataBlockStatus);
        ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                               0x81,  // IOCTL_CDROMAUDIO
                               0x65,  // CDROMAUDIO_GETAUDIOSTATUS
                               signature,
                               dwParameterSize,
                               &dwParameterSize,
                               &DataBlockStatus,
                               sizeof(DataBlockStatus),
                               &dwDataSize);
        if(ret != ERROR_SUCCESS) {
            dprintf(("IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOSTATUS failed!!"));
            return FALSE;
        }
        dprintf(("CDROMAUDIO_GETAUDIOSTATUS returned %d %x %x", DataBlockStatus.usAudioStatus, DataBlockStatus.ulStartLoc, DataBlockStatus.ulEndLoc));

        pChannelData->CurrentPosition.Header.Reserved = 0;
        if(DataBlockStatus.usAudioStatus & 1) {
            pChannelData->CurrentPosition.Header.AudioStatus = AUDIO_STATUS_PAUSED;
        }
        else {
            if(DataBlockStatus.ulStartLoc == 0) {//no play command has been issued before
                 pChannelData->CurrentPosition.Header.AudioStatus = AUDIO_STATUS_NO_STATUS;
            }
            else {//assume in progress, but could alse be finished playing
                pChannelData->CurrentPosition.Header.AudioStatus = (drvInfo->fCDPlaying) ? AUDIO_STATUS_IN_PROGRESS : AUDIO_STATUS_PLAY_COMPLETE;
            }
        }

        switch(pFormat->Format) {
        case IOCTL_CDROM_SUB_Q_CHANNEL:
            dprintf(("IOCTL_CDROM_SUB_Q_CHANNEL not supported"));
            return FALSE;
        case IOCTL_CDROM_CURRENT_POSITION:
            pChannelData->CurrentPosition.Header.DataLength[0]    = sizeof(pChannelData->CurrentPosition);
            pChannelData->CurrentPosition.Header.DataLength[1]    = 0;
            pChannelData->CurrentPosition.Control                 = DataBlock.ucControlAddr >> 4;
            pChannelData->CurrentPosition.ADR                     = DataBlock.ucControlAddr & 0xF;
            pChannelData->CurrentPosition.IndexNumber             = BCDToDec(DataBlock.ucIndex);
            pChannelData->CurrentPosition.TrackNumber             = BCDToDec(DataBlock.ucTrackNr);
            pChannelData->CurrentPosition.TrackRelativeAddress[1] = DataBlock.ucRuntimeTrackMin;
            pChannelData->CurrentPosition.TrackRelativeAddress[2] = DataBlock.ucRuntimeTrackSec;
            pChannelData->CurrentPosition.TrackRelativeAddress[3] = DataBlock.ucRuntimeTrackFrame;
            pChannelData->CurrentPosition.AbsoluteAddress[1]      = DataBlock.ucRuntimeDiscMin;
            pChannelData->CurrentPosition.AbsoluteAddress[2]      = DataBlock.ucRuntimeDiscSec;
            pChannelData->CurrentPosition.AbsoluteAddress[3]      = DataBlock.ucRuntimeDiscFrame;
            pChannelData->CurrentPosition.FormatCode              = IOCTL_CDROM_CURRENT_POSITION;
            dprintf(("IOCTL_CDROM_CURRENT_POSITION: Control %x ADR %x Index %d Track %d Track Rel %d:%d (%d) Absolute %d:%d (%d)", pChannelData->CurrentPosition.Control, pChannelData->CurrentPosition.ADR, pChannelData->CurrentPosition.IndexNumber, pChannelData->CurrentPosition.TrackNumber, pChannelData->CurrentPosition.TrackRelativeAddress[1], pChannelData->CurrentPosition.TrackRelativeAddress[2], pChannelData->CurrentPosition.TrackRelativeAddress[3], pChannelData->CurrentPosition.AbsoluteAddress[1], pChannelData->CurrentPosition.AbsoluteAddress[2], pChannelData->CurrentPosition.AbsoluteAddress[3]));
            if(lpBytesReturned) {
                *lpBytesReturned = sizeof(*pChannelData);
            }
            break;
        case IOCTL_CDROM_MEDIA_CATALOG:
            dprintf(("IOCTL_CDROM_MEDIA_CATALOG not supported"));
            return FALSE;
        case IOCTL_CDROM_TRACK_ISRC:
            dprintf(("IOCTL_CDROM_TRACK_ISRC not supported"));
            return FALSE;
        }
        return (ret == ERROR_SUCCESS);
    }
   
    case IOCTL_CDROM_RAW_READ:
    {
#pragma pack(1)
       struct 
       {
        ULONG       ID_code;
        UCHAR       address_mode;
        USHORT      transfer_count;
        ULONG       start_sector;
        UCHAR       reserved;
        UCHAR       interleave_size;
        UCHAR       interleave_skip_factor;
       } ParameterBlock;

       struct OutputBlock
       {
         BYTE Sync[12];
         BYTE Header[4];
         BYTE DataArea[2048];
         BYTE EDCECC[288];
       } *PWinOutput;

#pragma pack()

        PRAW_READ_INFO rInfo = (PRAW_READ_INFO)lpInBuffer;
        PWinOutput = (struct OutputBlock*)lpOutBuffer;

        if( (nOutBufferSize < (sizeof(OutputBlock)*rInfo->SectorCount))
            || !lpOutBuffer) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        // setup the parameter block
        memcpy(&ParameterBlock.ID_code, drvInfo->signature, 4);
        ParameterBlock.address_mode = 0;
        ParameterBlock.transfer_count = rInfo->SectorCount;
        ParameterBlock.start_sector = rInfo->DiskOffset.LowPart / 2048;
        ParameterBlock.reserved = 0;
        ParameterBlock.interleave_size = 0;
        ParameterBlock.interleave_skip_factor = 0;

        DWORD dwParameterSize = sizeof( ParameterBlock );
        DWORD dwDataSize      = ParameterBlock.transfer_count * sizeof(struct OutputBlock);
        DWORD ret;

        ret = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                             0x80,  // IOCTL_CDROMAUDIO
                             0x72,  // CDROMDISK_READLONG
                             &ParameterBlock,
                             sizeof( ParameterBlock ),
                             &dwParameterSize,
                             PWinOutput,
                             ParameterBlock.transfer_count * sizeof(struct OutputBlock),
                             &dwDataSize);

        if(lpBytesReturned) {
            *lpBytesReturned = dwDataSize; 
        }

       if(ret != ERROR_SUCCESS) {
          dprintf(("CDROMDISK_READLONG, CDROMDISK_READLONG failed with %x!!", ret));
          SetLastError(ERROR_IO_DEVICE);
          return FALSE;
      }
      return TRUE;
    }
    case IOCTL_CDROM_GET_LAST_SESSION:
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
#pragma pack(1)
      typedef struct
      {
        BYTE  ucCommandInfo;
        WORD  usDriveUnit;
      } ParameterBlock;
#pragma pack()

        dprintf(("IOCTL_CDROM(DISK/STORAGE)CHECK_VERIFY %s", drvInfo->signature));
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }

        if(!pHMHandleData->hHMHandle) {
            pHMHandleData->hHMHandle = OpenDisk(drvInfo);
            if(!pHMHandleData->hHMHandle) {
                dprintf(("No disk inserted; aborting"));
                SetLastError((drvInfo->fShareViolation) ? ERROR_SHARING_VIOLATION : ERROR_NOT_READY);
                return FALSE;
            }
        }

        DWORD parsize = sizeof(ParameterBlock);
        DWORD datasize = 2;
        WORD status = 0;
        DWORD rc;
        ParameterBlock parm;

        parm.ucCommandInfo = 0;
        parm.usDriveUnit = drvInfo->driveLetter - 'A';
//            rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle, 0x08, 0x66, &parm, sizeof(parm), &parsize,
        //TODO: this doesn't work for floppies for some reason...
        rc = OSLibDosDevIOCtl(-1, IOCTL_DISK, DSK_GETLOCKSTATUS, &parm, sizeof(parm), &parsize,
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

    case IOCTL_SCSI_MINIPORT:
    case IOCTL_SCSI_GET_INQUIRY_DATA:
        break;

    case IOCTL_SCSI_GET_CAPABILITIES:
    {
        PIO_SCSI_CAPABILITIES pPacket = (PIO_SCSI_CAPABILITIES)lpOutBuffer; 

        if(nOutBufferSize < sizeof(IO_SCSI_CAPABILITIES) ||
           !pPacket || pPacket->Length < sizeof(IO_SCSI_CAPABILITIES))
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        if(!drvInfo || drvInfo->fCDIoSupported == FALSE) {
            dprintf(("os2cdrom.dmd CD interface not supported!!"));
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }
        pPacket->MaximumTransferLength = 128*1024;
        pPacket->MaximumPhysicalPages  = 64*1024;
        pPacket->SupportedAsynchronousEvents = FALSE;
        pPacket->AlignmentMask         = -1;
        pPacket->TaggedQueuing         = FALSE;
        pPacket->AdapterScansDown      = FALSE;
        pPacket->AdapterUsesPio        = FALSE;
        if(lpBytesReturned)
            *lpBytesReturned = sizeof(*pPacket);

        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    case IOCTL_SCSI_PASS_THROUGH:
        //no break; same as IOCTL_SCSI_PASS_THROUGH_DIRECT
    case IOCTL_SCSI_PASS_THROUGH_DIRECT:
    {
        PSCSI_PASS_THROUGH_DIRECT pPacket = (PSCSI_PASS_THROUGH_DIRECT)lpOutBuffer;
        DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
        CDIO_CMD_BUFFER cdiocmd;

        if(nOutBufferSize < sizeof(SCSI_PASS_THROUGH_DIRECT) ||
           !pPacket || pPacket->Length < sizeof(SCSI_PASS_THROUGH_DIRECT))
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        
        if(!drvInfo || drvInfo->fCDIoSupported == FALSE) {
            dprintf(("os2cdrom.dmd CD interface not supported!!"));
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }
        if(lpBytesReturned) {
            *lpBytesReturned = 0;
        }

        if(pPacket->CdbLength > sizeof(cdiocmd.arCDB)) {
            dprintf(("CDB buffer too big (%d)!!", pPacket->CdbLength));
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        memset(&cdiocmd, 0, sizeof(cdiocmd));

        switch(pPacket->DataIn) {
        case SCSI_IOCTL_DATA_OUT:
            cdiocmd.flDirection = CMDDIR_OUTPUT;
            break;
        case SCSI_IOCTL_DATA_IN:
            cdiocmd.flDirection = CMDDIR_INPUT;
            break;
        case SCSI_IOCTL_DATA_UNSPECIFIED:
            cdiocmd.flDirection = CMDDIR_OUTPUT|CMDDIR_INPUT;;
            break;
        default:
            dprintf(("Invalid DataIn (%d)!!", pPacket->DataIn));
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        cdiocmd.cbCDB = pPacket->CdbLength;
        memcpy(cdiocmd.arCDB, pPacket->Cdb, pPacket->CdbLength);

        dprintf(("IOCTL_SCSI_PASS_THROUGH_DIRECT %x len %x, %x%02x%02x%02x %x%02x", pPacket->Cdb[0], pPacket->DataTransferLength, pPacket->Cdb[2], pPacket->Cdb[3], pPacket->Cdb[4], pPacket->Cdb[5], pPacket->Cdb[7], pPacket->Cdb[8]));

        if(OSLibCdIoSendCommand(pHMHandleData->hHMHandle, &cdiocmd, pPacket->DataBuffer, pPacket->DataTransferLength) == FALSE) {
            dprintf(("OSLibCdIoSendCommand failed!!"));
            pPacket->ScsiStatus = SS_ERR;
            SetLastError(ERROR_ADAP_HDW_ERR); //returned by NT4, SP6
            return FALSE;
        }

        if(pPacket->SenseInfoLength) {
            if(OSLibCdIoRequestSense(pHMHandleData->hHMHandle, (char *)pPacket + pPacket->SenseInfoOffset, pPacket->SenseInfoLength) == FALSE) {
                dprintf(("OSLibCdIoRequestSense failed!!"));
                pPacket->ScsiStatus = SS_ERR;
                SetLastError(ERROR_ADAP_HDW_ERR); //returned by NT4, SP6
                return FALSE;
            }
        }
        pPacket->ScsiStatus = SS_COMP;
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    case IOCTL_SCSI_GET_ADDRESS:
    {
        DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;

        if(!drvInfo || drvInfo->fCDIoSupported == FALSE) {
            dprintf(("os2cdrom.dmd CD interface not supported!!"));
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }

        if(!lpOutBuffer || nOutBufferSize < 8) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);  //todo: right error?
            return(FALSE);
        }
        SCSI_ADDRESS *addr = (SCSI_ADDRESS *)lpOutBuffer;
        addr->Length = sizeof(SCSI_ADDRESS);
        addr->PortNumber = 0;
        addr->PathId     = 0;
        //irrelevant, since os2cdrom.dmd doesn't need them
        addr->TargetId   = 1;
        addr->Lun        = 0;
        SetLastError(ERROR_SUCCESS);
        return TRUE;
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
                                 LPOVERLAPPED  lpOverlapped,
                                 LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    LPVOID       lpRealBuf;
    Win32MemMap *map;
    DWORD        offset, bytesread;
    BOOL         bRC;
    DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;

    dprintf2(("KERNEL32: HMDeviceDiskClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)",
               lpHMDeviceName, pHMHandleData, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped));

    //It's legal for this pointer to be NULL
    if(lpNumberOfBytesRead)
        *lpNumberOfBytesRead = 0;
    else
        lpNumberOfBytesRead = &bytesread;

    if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
        dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if(lpCompletionRoutine) {
        dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
    }

    //If we didn't get an OS/2 handle for the disk before, get one now
    if(!pHMHandleData->hHMHandle) {
        pHMHandleData->hHMHandle = OpenDisk(drvInfo);
        if(!pHMHandleData->hHMHandle) {
            dprintf(("No disk inserted; aborting"));
            SetLastError((drvInfo->fShareViolation) ? ERROR_SHARING_VIOLATION : ERROR_NOT_READY);
            return FALSE;
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
        DWORD nrpages = (nNumberOfBytesToRead+offset)/4096;
        if((nNumberOfBytesToRead+offset) & 0xfff)
            nrpages++;
    
        map->commitRange((ULONG)lpBuffer, offset & ~0xfff, TRUE, nrpages);
        map->Release();
    }
    else  lpRealBuf = (LPVOID)lpBuffer;

    //if unmounted volume, check upper boundary as we're accessing the entire physical drive
    //instead of just the volume
    if(UNMOUNTED_VOLUME(drvInfo))
    {
        LARGE_INTEGER distance, result, endpos;

        //calculate end position in partition
        Add64(&drvInfo->StartingOffset, &drvInfo->PartitionSize, &endpos);

        distance.HighPart = 0;
        distance.LowPart  = nNumberOfBytesToRead;
        Add64(&distance, &drvInfo->CurrentFilePointer, &result);

        //check upper boundary
        if(result.HighPart > endpos.HighPart ||
           (result.HighPart == endpos.HighPart && result.LowPart > endpos.LowPart) )
        {
            Sub64(&endpos, &drvInfo->CurrentFilePointer, &result);
            nNumberOfBytesToRead = result.LowPart;
            dprintf(("Read past end of volume; nNumberOfBytesToRead reduced to %d", nNumberOfBytesToRead));
            DebugInt3();
        }
    }

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

    //if unmounted volume, add starting offset to position as we're accessing the entire physical drive
    //instead of just the volume
    if(UNMOUNTED_VOLUME(drvInfo) && bRC == TRUE)
    {
        LARGE_INTEGER distance, result;

        distance.HighPart = 0;
        distance.LowPart  = *lpNumberOfBytesRead;
        Add64(&distance, &drvInfo->CurrentFilePointer, &result);
        drvInfo->CurrentFilePointer = result;

        dprintf(("New unmounted volume current file pointer %08x%08x", drvInfo->CurrentFilePointer.HighPart, drvInfo->CurrentFilePointer.LowPart));
    }

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

    if(lpDistanceToMoveHigh) {
        dprintf(("KERNEL32: HMDeviceDiskClass::SetFilePointer %s %08x%08x %d",
                 lpHMDeviceName, *lpDistanceToMoveHigh, lDistanceToMove, dwMoveMethod));
    }

    DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
    if(drvInfo == NULL) {
        dprintf(("ERROR: SetFilePointer: drvInfo == NULL!!!"));
        DebugInt3();
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if(!pHMHandleData->hHMHandle) {
        DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
        pHMHandleData->hHMHandle = OpenDisk(drvInfo);
        if(!pHMHandleData->hHMHandle) {
            dprintf(("No disk inserted; aborting"));
            SetLastError((drvInfo->fShareViolation) ? ERROR_SHARING_VIOLATION : ERROR_NOT_READY);
            return -1;
        }
    }

    //if unmounted volume, add starting offset to position as we're accessing the entire physical drive
    //instead of just the volume
    if(UNMOUNTED_VOLUME(drvInfo))
    {
        LARGE_INTEGER distance, result, endpos;
        LARGE_INTEGER position;

        if(lpDistanceToMoveHigh) {
            distance.HighPart =  *lpDistanceToMoveHigh;
        }
        else {
            if(lDistanceToMove < 0) {
                 distance.HighPart = -1;
            }
            else distance.HighPart = 0;
        }
        distance.LowPart = lDistanceToMove;

        //calculate end position in partition
        Add64(&drvInfo->StartingOffset, &drvInfo->PartitionSize, &endpos);

        switch(dwMoveMethod) {
        case FILE_BEGIN:
            Add64(&distance, &drvInfo->StartingOffset, &result);
            break;
        case FILE_CURRENT:
            Add64(&distance, &drvInfo->CurrentFilePointer, &result);
            break;
        case FILE_END:
            Add64(&distance, &endpos, &result);
            break;
        }
        //check upper boundary
        if(result.HighPart > endpos.HighPart ||
           (result.HighPart == endpos.HighPart && result.LowPart > endpos.LowPart) )
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return -1;
        }
        //check lower boundary
        if(result.HighPart < drvInfo->StartingOffset.HighPart ||
           (result.HighPart == drvInfo->StartingOffset.HighPart && result.LowPart < drvInfo->StartingOffset.LowPart))
        {
            SetLastError(ERROR_NEGATIVE_SEEK);
            return -1;
        }

        dprintf(("SetFilePointer (unmounted partition) %08x%08x -> %08x%08x", distance.HighPart, distance.LowPart, result.HighPart, result.LowPart));
        ret = OSLibDosSetFilePointer(pHMHandleData->hHMHandle,
                                     result.LowPart,
                                     (DWORD *)&result.HighPart,
                                     FILE_BEGIN);
        //save new file pointer
        drvInfo->CurrentFilePointer.HighPart = result.HighPart;
        drvInfo->CurrentFilePointer.LowPart  = ret;

        //subtract volume start to get relative offset
        Sub64(&drvInfo->CurrentFilePointer, &drvInfo->StartingOffset, &result);
        ret = result.LowPart;
        if(lpDistanceToMoveHigh) {
            *lpDistanceToMoveHigh = result.HighPart;
        }
    }
    else {
        ret = OSLibDosSetFilePointer(pHMHandleData->hHMHandle,
                                     lDistanceToMove,
                                     (DWORD *)lpDistanceToMoveHigh,
                                     dwMoveMethod);
    }

    if(ret == -1) {
        dprintf(("SetFilePointer failed (error = %d)", GetLastError()));
    }
    return ret;
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
                                  LPOVERLAPPED  lpOverlapped,
                                  LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    LPVOID       lpRealBuf;
    Win32MemMap *map;
    DWORD        offset, byteswritten;
    BOOL         bRC;

    dprintf2(("KERNEL32: HMDeviceDiskClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
              lpHMDeviceName, pHMHandleData, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten,
              lpOverlapped));

    DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
    if(drvInfo == NULL) {
        dprintf(("ERROR: WriteFile: drvInfo == NULL!!!"));
        DebugInt3();
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if(!(drvInfo->dwAccess & GENERIC_WRITE)) {
        dprintf(("ERROR: WriteFile: write access denied!"));
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    //It's legal for this pointer to be NULL
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
    if(lpCompletionRoutine) {
        dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
    }

    //If we didn't get an OS/2 handle for the disk before, get one now
    if(!pHMHandleData->hHMHandle) {
        DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
        pHMHandleData->hHMHandle = OpenDisk(drvInfo);
        if(!pHMHandleData->hHMHandle) {
            dprintf(("No disk inserted; aborting"));
            SetLastError((drvInfo->fShareViolation) ? ERROR_SHARING_VIOLATION : ERROR_NOT_READY);
            return FALSE;
        }
    }

    //SvL: DosWrite doesn't like reading from memory addresses returned by
    //     DosAliasMem -> search for original memory mapped pointer and use
    //     that one + commit pages if not already present
    map = Win32MemMapView::findMapByView((ULONG)lpBuffer, &offset, MEMMAP_ACCESS_READ);
    if(map) {
        lpRealBuf = (LPVOID)((ULONG)map->getMappingAddr() + offset);
        DWORD nrpages = (nNumberOfBytesToWrite+offset)/4096;
        if((nNumberOfBytesToWrite+offset) & 0xfff)
            nrpages++;
    
        map->commitRange((ULONG)lpBuffer, offset & ~0xfff, TRUE, nrpages);
        map->Release();
    }
    else  lpRealBuf = (LPVOID)lpBuffer;

    //if unmounted volume, check upper boundary as we're accessing the entire physical drive
    //instead of just the volume
    if(UNMOUNTED_VOLUME(drvInfo))
    {
        LARGE_INTEGER distance, result, endpos;

        //calculate end position in partition
        Add64(&drvInfo->StartingOffset, &drvInfo->PartitionSize, &endpos);

        distance.HighPart = 0;
        distance.LowPart  = nNumberOfBytesToWrite;
        Add64(&distance, &drvInfo->CurrentFilePointer, &result);

        //check upper boundary
        if(result.HighPart > endpos.HighPart ||
           (result.HighPart == endpos.HighPart && result.LowPart > endpos.LowPart) )
        {
            Sub64(&endpos, &drvInfo->CurrentFilePointer, &result);
            nNumberOfBytesToWrite = result.LowPart;
            dprintf(("Write past end of volume; nNumberOfBytesToWrite reduced to %d", nNumberOfBytesToWrite));
            DebugInt3();
        }
    }

    if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) {
        dprintf(("ERROR: Overlapped IO not yet implememented!!"));
    }
//  else {
#if 0
    bRC = TRUE;
    *lpNumberOfBytesWritten = nNumberOfBytesToWrite;
#else
    bRC = OSLibDosWrite(pHMHandleData->hHMHandle,
                            (PVOID)lpRealBuf,
                            nNumberOfBytesToWrite,
                            lpNumberOfBytesWritten);
#endif
//  }

    //if unmounted volume, add starting offset to position as we're accessing the entire physical drive
    //instead of just the volume
    if(UNMOUNTED_VOLUME(drvInfo) && bRC == TRUE)
    {
        LARGE_INTEGER distance, result;

        distance.HighPart = 0;
        distance.LowPart  = *lpNumberOfBytesWritten;
        Add64(&distance, &drvInfo->CurrentFilePointer, &result);
        drvInfo->CurrentFilePointer = result;

        dprintf(("New unmounted volume current file pointer %08x%08x", drvInfo->CurrentFilePointer.HighPart, drvInfo->CurrentFilePointer.LowPart));
    }

    dprintf2(("KERNEL32: HMDeviceDiskClass::WriteFile returned %08xh\n",
               bRC));

    return bRC;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceDiskClass::GetFileSize
 * Purpose   : set file time
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PDWORD        pSize
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceDiskClass::GetFileSize(PHMHANDLEDATA pHMHandleData,
                                     PDWORD        lpdwFileSizeHigh)
{
#if 1
    //Verified in NT4
    dprintf(("WARNING: GetFileSize doesn't work for drive objects; returning error (same as Windows)"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return -1; //INVALID_SET_FILE_POINTER
#else
    DRIVE_INFO *drvInfo = (DRIVE_INFO*)pHMHandleData->dwUserData;
    if(drvInfo == NULL) {
        dprintf(("ERROR: GetFileSize: drvInfo == NULL!!!"));
        DebugInt3();
        SetLastError(ERROR_INVALID_HANDLE);
        return -1; //INVALID_SET_FILE_POINTER
    }

    dprintf2(("KERNEL32: HMDeviceDiskClass::GetFileSize %s(%08xh,%08xh)\n",
              lpHMDeviceName, pHMHandleData, lpdwFileSizeHigh)); 

    //If we didn't get an OS/2 handle for the disk before, get one now
    if(!pHMHandleData->hHMHandle) {
        pHMHandleData->hHMHandle = OpenDisk(drvInfo);
        if(!pHMHandleData->hHMHandle) {
            dprintf(("No disk inserted; aborting"));
            SetLastError((drvInfo->fShareViolation) ? ERROR_SHARING_VIOLATION : ERROR_NOT_READY);
            return -1; //INVALID_SET_FILE_POINTER
        }
    }

    if(drvInfo->PartitionSize.HighPart || drvInfo->PartitionSize.LowPart) {
        if(lpdwFileSizeHigh)
            *lpdwFileSizeHigh = drvInfo->PartitionSize.HighPart;

        return drvInfo->PartitionSize.LowPart;
    }
    else {
        LARGE_INTEGER position, size;

        //get current position
        position.HighPart = 0;
        position.LowPart  = SetFilePointer(pHMHandleData, 0, (PLONG)&position.HighPart, FILE_CURRENT);
        SetFilePointer(pHMHandleData, 0, NULL, FILE_BEGIN);
        size.HighPart     = 0;
        size.LowPart      = SetFilePointer(pHMHandleData, 0, (PLONG)&size.HighPart, FILE_END);

        //restore old position
        SetFilePointer(pHMHandleData, position.LowPart, (PLONG)&position.HighPart, FILE_BEGIN);

        if(lpdwFileSizeHigh)
            *lpdwFileSizeHigh = size.HighPart;

        return size.LowPart;
    }
#endif
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
               lpHMDeviceName, pHMHandleData));

    return FILE_TYPE_DISK;
}
//******************************************************************************
//******************************************************************************
