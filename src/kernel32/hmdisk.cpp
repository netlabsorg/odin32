/* $Id: hmdisk.cpp,v 1.10 2001-06-19 10:50:24 sandervl Exp $ */

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

static HINSTANCE hInstAspi                           = 0;
static DWORD (WIN32API *GetASPI32SupportInfo)()      = NULL;
static DWORD (CDECL *SendASPI32Command)(LPSRB lpSRB) = NULL;

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
        if(hFile == INVALID_HANDLE_ERROR) {
             SetLastError(NO_ERROR);
             pHMHandleData->hHMHandle  = 0; //handle lookup fails if this is set to -1
        }
        else pHMHandleData->hHMHandle  = hFile;

        pHMHandleData->dwUserData = *lpFileName; //save drive letter
        if(pHMHandleData->dwUserData >= 'a') {
            pHMHandleData->dwUserData = pHMHandleData->dwUserData - ((int)'a' - (int)'A');
        }

        if(GetDriveTypeA(lpFileName) == DRIVE_CDROM && hInstAspi == NULL) {
            hInstAspi = LoadLibraryA("WNASPI32.DLL");
            if(hInstAspi == NULL) {
                return ERROR_INVALID_PARAMETER;
            }
            *(FARPROC *)&GetASPI32SupportInfo = GetProcAddress(hInstAspi, "GetASPI32SupportInfo");
            *(FARPROC *)&SendASPI32Command    = GetProcAddress(hInstAspi, "SendASPI32Command");
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
    }
    if(msg) {
        dprintf(("HMDeviceDiskClass::DeviceIoControl %s", msg));
    }
#endif

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

    case IOCTL_DISK_CHECK_VERIFY:
    case IOCTL_DISK_EJECT_MEDIA:
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
        if(OSLibDosGetDiskGeometry(pHMHandleData->dwUserData, pGeom) == FALSE) {
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

    case IOCTL_STORAGE_CHECK_VERIFY:
        if(lpBytesReturned) {
            lpBytesReturned = 0;
        }
        //TODO: check if disk has been inserted or removed
        if(pHMHandleData->hHMHandle == 0) {
            SetLastError(ERROR_NOT_READY);
            return FALSE;
        }
        SetLastError(NO_ERROR);
        return TRUE;

    case IOCTL_STORAGE_EJECT_MEDIA:
    case IOCTL_STORAGE_GET_MEDIA_TYPES:
    case IOCTL_STORAGE_LOAD_MEDIA:
    case IOCTL_STORAGE_MEDIA_REMOVAL:
        break;
    case IOCTL_SCSI_PASS_THROUGH:
    case IOCTL_SCSI_MINIPORT:
    case IOCTL_SCSI_GET_INQUIRY_DATA:
    case IOCTL_SCSI_GET_CAPABILITIES:
        break;

    case IOCTL_SCSI_PASS_THROUGH_DIRECT:
    {
        PSCSI_PASS_THROUGH_DIRECT pPacket = (PSCSI_PASS_THROUGH_DIRECT)lpOutBuffer;
        SRB_ExecSCSICmd *psrb;

        if(hInstAspi == NULL) {
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
        psrb->SRB_BufPointer = (BYTE *)pPacket->DataBuffer;
        memcpy(&psrb->CDBByte[0], &pPacket->Cdb[0], 16);
        if(psrb->SRB_SenseLen) {
            memcpy(&psrb->SenseArea[0], (char *)pPacket + pPacket->SenseInfoOffset, psrb->SRB_SenseLen);
        }
        //TODO: pPacket->TimeOutValue ignored
        int rc = SendASPI32Command((LPSRB)psrb);
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
        numAdapters = GetASPI32SupportInfo();
        if(LOBYTE(numAdapters) == 0) goto failure;

        memset(&srb, 0, sizeof(srb));
        srb.common.SRB_Cmd = SC_HA_INQUIRY;
        rc = SendASPI32Command(&srb);

        char drivename[3];
        drivename[0] = (char)pHMHandleData->dwUserData;
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
                    rc = SendASPI32Command(&srb);
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

  dprintf2(("KERNEL32: HMDeviceDiskClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
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

  ret = OSLibDosSetFilePointer(pHMHandleData->hHMHandle,
                               lDistanceToMove,
                               (DWORD *)lpDistanceToMoveHigh,
                               dwMoveMethod);

  if(ret == -1) {
    dprintf(("SetFilePointer failed (error = %d)", GetLastError()));
  }
  return ret;
}
//******************************************************************************
//******************************************************************************
