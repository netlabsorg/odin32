/* $Id: hmdisk.cpp,v 1.6 2001-04-26 13:22:45 sandervl Exp $ */

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
#include "oslibdos.h"
#include <win\winioctl.h>
#include <win\ntddscsi.h>
#include <win\wnaspi32.h>
#include <win\aspi.h>

#define DBG_LOCALLOG    DBG_hmdisk
#include "dbglocal.h"

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
    //\\.\\x:               -> length 6
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
DWORD HMDeviceDiskClass::CreateFile (LPCSTR        lpFileName,
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
    if(strncmp(lpFileName,       // "support" for local unc names
             "\\\\.\\",
             4) == 0)
    {
        lpFileName+=4;
    }

    //Disable error popus. NT allows an app to open a cdrom/dvd drive without a disk inside
    //OS/2 fails in that case with error ERROR_NOT_READY
    OSLibDosDisableHardError(TRUE);
    hFile = OSLibDosCreateFile((LPSTR)lpFileName,
                               pHMHandleData->dwAccess,
                               pHMHandleData->dwShare,
                               (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                               pHMHandleData->dwCreation,
                               pHMHandleData->dwFlags,
                               hTemplate);
    OSLibDosDisableHardError(FALSE);

    if (hFile != INVALID_HANDLE_ERROR || GetLastError() == ERROR_NOT_READY)
    {
        if(hFile == INVALID_HANDLE_ERROR) SetLastError(NO_ERROR);

        pHMHandleData->hHMHandle  = hFile;
        pHMHandleData->dwUserData = GetDriveTypeA(lpFileName);
        return (NO_ERROR);
    }
    else {
        dprintf(("CreateFile failed; error %d", GetLastError()));
        return(GetLastError());
    }
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceDiskClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
    if(pHMHandleData->hHMHandle) {
        return OSLibDosClose(pHMHandleData->hHMHandle);
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceDiskClass::DeviceIoControl(PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                             LPVOID lpInBuffer, DWORD nInBufferSize,
                             LPVOID lpOutBuffer, DWORD nOutBufferSize,
                             LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
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
    case IOCTL_DISK_CHECK_VERIFY:
    case IOCTL_DISK_EJECT_MEDIA:
    case IOCTL_DISK_FORMAT_TRACKS:
    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
    case IOCTL_DISK_GET_DRIVE_LAYOUT:
    case IOCTL_DISK_GET_MEDIA_TYPES:
    case IOCTL_DISK_GET_PARTITION_INFO:
    case IOCTL_DISK_LOAD_MEDIA:
    case IOCTL_DISK_MEDIA_REMOVAL:
    case IOCTL_DISK_PERFORMANCE:
    case IOCTL_DISK_REASSIGN_BLOCKS:
    case IOCTL_DISK_SET_DRIVE_LAYOUT:
    case IOCTL_DISK_SET_PARTITION_INFO:
    case IOCTL_DISK_VERIFY:
    case IOCTL_SERIAL_LSRMST_INSERT:
    case IOCTL_STORAGE_CHECK_VERIFY:
    case IOCTL_STORAGE_EJECT_MEDIA:
    case IOCTL_STORAGE_GET_MEDIA_TYPES:
    case IOCTL_STORAGE_LOAD_MEDIA:
    case IOCTL_STORAGE_MEDIA_REMOVAL:
        break;
    case IOCTL_SCSI_PASS_THROUGH:
    case IOCTL_SCSI_MINIPORT:
    case IOCTL_SCSI_GET_INQUIRY_DATA:
    case IOCTL_SCSI_GET_CAPABILITIES:
    case IOCTL_SCSI_PASS_THROUGH_DIRECT:
        break;

    case IOCTL_SCSI_GET_ADDRESS:
    {
        HINSTANCE hInstAspi;
        DWORD (WIN32API *GetASPI32SupportInfo)();
        DWORD (CDECL *SendASPI32Command)(LPSRB lpSRB);
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
        hInstAspi = LoadLibraryA("WNASPI32.DLL");
        if(hInstAspi == NULL) {
            SetLastError(ERROR_INVALID_PARAMETER); //todo
            return FALSE;
        }
        *(FARPROC *)&GetASPI32SupportInfo = GetProcAddress(hInstAspi, "GetASPI32SupportInfo");
        *(FARPROC *)&SendASPI32Command    = GetProcAddress(hInstAspi, "SendASPI32Command");
        numAdapters = GetASPI32SupportInfo();
        if(LOBYTE(numAdapters) == 0) goto failure;

        memset(&srb, 0, sizeof(srb));
        srb.common.SRB_Cmd = SC_HA_INQUIRY;
        rc = SendASPI32Command(&srb);

        for(i=0;i<LOBYTE(numAdapters);i++) {
            for(j=0;j<8;j++) {
                for(k=0;k<16;k++) {
                    memset(&srb, 0, sizeof(srb));
                    srb.common.SRB_Cmd     = SC_GET_DEV_TYPE;
                    srb.devtype.SRB_HaId   = i;
                    srb.devtype.SRB_Target = j;
                    srb.devtype.SRB_Lun    = k;
                    rc = SendASPI32Command(&srb);
                    if(rc == SS_COMP) {
                        if(srb.devtype.SRB_DeviceType == SS_DEVTYPE_CDROM &&
                                                   pHMHandleData->dwUserData == DRIVE_CDROM)
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
        FreeLibrary(hInstAspi);
        return TRUE;
failure:
        FreeLibrary(hInstAspi);
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
//******************************************************************************
//******************************************************************************
