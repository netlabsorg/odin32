/* $Id: hmdisk.cpp,v 1.1 2000-09-13 21:10:58 sandervl Exp $ */

/*
 * Win32 Disk API functions for OS/2
 *
 * Copyright 2000 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSPROFILE
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_GPI
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <string.h>

#include <win32type.h>
#include <win32api.h>
#include <misc.h>
#include "hmdisk.h"
#include "oslibdos.h"
#include <win\winioctl.h>
#include <win\ntddscsi.h>

#define DBG_LOCALLOG	DBG_hmdisk
#include "dbglocal.h"

//******************************************************************************
//TODO: PHYSICALDRIVEn!!
//******************************************************************************
DWORD HMDeviceDiskClass::CreateFile (LPCSTR lpFileName,
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

  if(strncmp(lpFileName,       // "support" for local unc names
             "\\\\.\\",
             4) == 0)
  {
      	lpFileName+=4;
  }

  hFile = OSLibDosCreateFile((LPSTR)lpFileName,
                             pHMHandleData->dwAccess,
                             pHMHandleData->dwShare,
                             (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                             pHMHandleData->dwCreation,
                             pHMHandleData->dwFlags,
                             hTemplate);

  if (hFile != INVALID_HANDLE_ERROR)
  {
     	pHMHandleData->hHMHandle  = hFile;
     	return (NO_ERROR);
  }
  else {
	dprintf(("CreateFile failed; error %d", GetLastError()));
    	return(GetLastError());
  }
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceDiskClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  return OSLibDosClose(pHMHandleData->hHMHandle);
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
	        if(!lpOutBuffer || nOutBufferSize < 8) {
			SetLastError(ERROR_INSUFFICIENT_BUFFER_W);  //todo: right error?
	            	return(FALSE);
		}
		SCSI_ADDRESS *addr = (SCSI_ADDRESS *)lpOutBuffer;
		addr->Length = sizeof(SCSI_ADDRESS);
		addr->PortNumber = 0;
		addr->PathId     = 0;
		addr->TargetId   = 1;
		addr->Lun        = 3;
		return TRUE;
        }
	case IOCTL_SCSI_RESCAN_BUS:
	case IOCTL_SCSI_GET_DUMP_POINTERS:
	case IOCTL_SCSI_FREE_DUMP_POINTERS:
	case IOCTL_IDE_PASS_THROUGH:
		break;

   }
   dprintf(("HMDeviceDiskClass::DeviceIoControl: unimplemented dwIoControlCode=%08lx\n", dwIoControlCode));
   SetLastError( ERROR_CALL_NOT_IMPLEMENTED_W );
   return FALSE;
}
//******************************************************************************
//******************************************************************************
