/* $Id: hmdevio.cpp,v 1.28 2002-07-05 17:59:30 sandervl Exp $ */

/*
 * Win32 Device IOCTL API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
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
#include <stdio.h>

#include <win32type.h>
#include <win32api.h>
#include <misc.h>
#include <win\winioctl.h>
#include "hmdevio.h"
#include "cio.h"
#include "map.h"
#include "exceptutil.h"
#include "oslibdos.h"

#define DBG_LOCALLOG	DBG_hmdevio
#include "dbglocal.h"

static BOOL fX86Init  = FALSE;
//SvL: Used in iccio.asm (how can you put these in the .asm data segment without messing things up?)
ULONG  ioentry   = 0;
USHORT gdt       = 0;
char   devname[] = "/dev/fastio$";

static BOOL GpdDevIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
static BOOL MAPMEMIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
static BOOL FXMEMMAPIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
static BOOL VPCIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

static WIN32DRV knownDriver[] =
    {{"\\\\.\\GpdDev", "",      TRUE,  666,   GpdDevIOCtl},
    { "\\\\.\\MAPMEM", "PMAP$", FALSE, 0,     MAPMEMIOCtl},
    { "FXMEMMAP.VXD",  "PMAP$", FALSE, 0,     FXMEMMAPIOCtl},
    };

static int nrKnownDrivers = sizeof(knownDriver)/sizeof(WIN32DRV);
BOOL fVirtualPC = FALSE;

//******************************************************************************
//******************************************************************************
void RegisterDevices()
{
 HMDeviceDriver *driver;
 DWORD rc;

    for(int i=0;i<nrKnownDrivers;i++) 
    {
	    driver = new HMDeviceDriver(knownDriver[i].szWin32Name,
                                    knownDriver[i].szOS2Name,
                                    knownDriver[i].fCreateFile,
                                    knownDriver[i].devIOCtl);

	    rc = HMDeviceRegister(knownDriver[i].szWin32Name, driver);
    	if (rc != NO_ERROR)                                  /* check for errors */
      		dprintf(("KERNEL32:RegisterDevices: registering %s failed with %u.\n",
              		  knownDriver[i].szWin32Name, rc));
    }

    //check registry for Odin driver plugin dlls
    HKEY hkDrivers, hkDrvDll;

    rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                       "System\\CurrentControlSet\\Services",
                       0, KEY_READ, &hkDrivers);

    if(rc == 0) {
        char szDllName[CCHMAXPATH];
        char szKeyName[CCHMAXPATH];
        char szDrvName[CCHMAXPATH];
        DWORD dwType, dwSize;
        int iSubKey = 0;

        while(rc == 0) {
            rc = RegEnumKeyA(hkDrivers, iSubKey++, szKeyName, sizeof(szKeyName));
            if(rc) break;

            rc = RegOpenKeyExA(hkDrivers, szKeyName,
                               0, KEY_READ, &hkDrvDll);
            if(rc == 0) {
                dwSize = sizeof(szDllName);
                rc = RegQueryValueExA(hkDrvDll,
                                      "DllName",
                                      NULL,
                                      &dwType,
                                      (LPBYTE)szDllName,
                                      &dwSize);

                RegCloseKey(hkDrvDll);
                if(rc == 0 && dwType == REG_SZ)
                {
                    HINSTANCE hDrvDll = LoadLibraryA(szDllName);
                    if(hDrvDll)
                    {
                        sprintf(szDrvName, "\\\\.\\%s", szKeyName);
                        driver = new HMCustomDriver(hDrvDll, szDrvName, NULL);

                        rc = HMDeviceRegister(szDrvName, driver);
                        if (rc != NO_ERROR)                                  /* check for errors */
                          dprintf(("KERNEL32:RegisterDevices: registering %s failed with %u.\n", szDrvName, rc));
                      
                        // @@@PH
                        // there should be an symbolic link:
                        // "\\.\drvname$" -> "drvname$"
                    }
                }
                rc = 0;
            }
        }   
        RegCloseKey(hkDrivers);
    }

    return;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RegisterCustomDriver(PFNDRVOPEN pfnDriverOpen, PFNDRVCLOSE pfnDriverClose, 
                                   PFNDRVIOCTL pfnDriverIOCtl, PFNDRVREAD pfnDriverRead,
                                   PFNDRVWRITE pfnDriverWrite, PFNDRVCANCELIO pfnDriverCancelIo,
                                   PFNDRVGETOVERLAPPEDRESULT pfnDriverGetOverlappedResult,
                                   LPCSTR lpDeviceName, LPVOID lpDriverData)
{
    HMCustomDriver *driver;
    DWORD rc;

    dprintf(("RegisterCustomDriver %s", lpDeviceName));
    driver = new HMCustomDriver(pfnDriverOpen, pfnDriverClose, pfnDriverIOCtl, pfnDriverRead, pfnDriverWrite, pfnDriverCancelIo, pfnDriverGetOverlappedResult, lpDeviceName, lpDriverData);
    if(driver == NULL) {
        DebugInt3();
        return FALSE;
    }
    rc = HMDeviceRegister((LPSTR)lpDeviceName, driver);
    if (rc != NO_ERROR) {                                /* check for errors */
        dprintf(("KERNEL32:RegisterDevices: registering %s failed with %u.\n", lpDeviceName, rc));
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
HMDeviceDriver::HMDeviceDriver(LPCSTR lpDeviceName, LPSTR lpOS2DevName, BOOL fCreate, 
                               WINIOCTL pDevIOCtl)
                : HMDeviceHandler(lpDeviceName)
{
    this->fCreateFile = fCreateFile;
    this->szOS2Name   = lpOS2DevName;
    this->devIOCtl    = pDevIOCtl;
}
//******************************************************************************
//******************************************************************************
HMDeviceDriver::HMDeviceDriver(LPCSTR lpDeviceName)
                : HMDeviceHandler(lpDeviceName)
{
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceDriver::CreateFile (LPCSTR lpFileName,
                                  PHMHANDLEDATA pHMHandleData,
                                  PVOID         lpSecurityAttributes,
                                  PHMHANDLEDATA pHMHandleDataTemplate)
{
 APIRET rc;
 HFILE  hfFileHandle   = 0L;     /* Handle for file being manipulated */
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  sharetype = 0;

  if(pHMHandleData->dwAccess & (GENERIC_READ | GENERIC_WRITE))
    sharetype |= OPEN_ACCESS_READWRITE;
  else
  if(pHMHandleData->dwAccess & GENERIC_WRITE)
    sharetype |= OPEN_ACCESS_WRITEONLY;

  if(pHMHandleData->dwShare == 0)
    sharetype |= OPEN_SHARE_DENYREADWRITE;
  else
  if(pHMHandleData->dwShare & (FILE_SHARE_READ | FILE_SHARE_WRITE))
    sharetype |= OPEN_SHARE_DENYNONE;
  else
  if(pHMHandleData->dwShare & FILE_SHARE_WRITE)
    sharetype |= OPEN_SHARE_DENYREAD;
  else
  if(pHMHandleData->dwShare & FILE_SHARE_READ)
    sharetype |= OPEN_SHARE_DENYWRITE;

  if(szOS2Name[0] == 0) {
     	pHMHandleData->hHMHandle = 0;
     	return (NO_ERROR);
  }

tryopen:
  rc = DosOpen(	szOS2Name,                        /* File path name */
               	&hfFileHandle,                  /* File handle */
               	&ulAction,                      /* Action taken */
	       	0,
         	FILE_NORMAL,
           	FILE_OPEN,
           	sharetype,
               	0L);                            /* No extended attribute */

  if(rc == ERROR_TOO_MANY_OPEN_FILES) {
   ULONG CurMaxFH;
   LONG  ReqCount = 32;

	rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
	if(rc) {
		dprintf(("DosSetRelMaxFH returned %d", rc));
		return error2WinError(rc);
	}
	dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
	goto tryopen;
  }

  dprintf(("DosOpen %s returned %d\n", szOS2Name, rc));

  if(rc == NO_ERROR) 
  {
    pHMHandleData->hHMHandle = hfFileHandle;
    return (NO_ERROR);
  }
  else  
    return(error2WinError(rc));
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceDriver::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
 DWORD rc = 0;

   if(pHMHandleData->hHMHandle) {
	rc = DosClose(pHMHandleData->hHMHandle);
   }
   pHMHandleData->hHMHandle = 0;
   return rc;
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceDriver::DeviceIoControl(PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                     LPVOID lpInBuffer, DWORD nInBufferSize,
                                     LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                     LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
   return devIOCtl(pHMHandleData->hHMHandle, dwIoControlCode, lpInBuffer, nInBufferSize,
                   lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped);
}
//******************************************************************************
//******************************************************************************
static BOOL GpdDevIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
 ULONG port, val = 0;

  if(fX86Init == FALSE) {
    	if(io_init() == 0)
        	fX86Init = TRUE;
    	else   	return(FALSE);
  }

  *lpBytesReturned = 0;

  port = ((GENPORT_WRITE_INPUT *)lpInBuffer)->PortNumber;

  switch((dwIoControlCode >> 2) & 0xFFF) {
    case IOCTL_GPD_READ_PORT_UCHAR:
        if(nOutBufferSize < sizeof(char))
            return(FALSE);

        val = c_inb(port);
        *(char *)lpOutBuffer = val;
        *lpBytesReturned = sizeof(char);
        break;
    case IOCTL_GPD_READ_PORT_USHORT:
        if(nOutBufferSize < sizeof(USHORT))
            return(FALSE);

        val = c_inw(port);
        *(USHORT *)lpOutBuffer = val;
        *lpBytesReturned = sizeof(USHORT);
        break;
    case IOCTL_GPD_READ_PORT_ULONG:
        if(nOutBufferSize < sizeof(ULONG))
            return(FALSE);

        val = c_inl(port);
        *(ULONG *)lpOutBuffer = val;
        *lpBytesReturned = sizeof(ULONG);
        break;
    case IOCTL_GPD_WRITE_PORT_UCHAR:
        val   = ((GENPORT_WRITE_INPUT *)lpInBuffer)->CharData;
        c_outb(port, val);
        break;
    case IOCTL_GPD_WRITE_PORT_USHORT:
        val   = ((GENPORT_WRITE_INPUT *)lpInBuffer)->ShortData;
        c_outw(port, val);
        break;
    case IOCTL_GPD_WRITE_PORT_ULONG:
        val   = ((GENPORT_WRITE_INPUT *)lpInBuffer)->LongData;
        c_outl(port, val);
        break;
    default:
        dprintf(("GpdDevIOCtl unknown func %X\n", (dwIoControlCode >> 2) & 0xFFF));
        return(FALSE);
  }

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
static BOOL MAPMEMIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
 PHYSICAL_MEMORY_INFO *meminfo = (PHYSICAL_MEMORY_INFO *)lpInBuffer;
 struct map_ioctl memmap;

  *lpBytesReturned = 0;

  switch((dwIoControlCode >> 2) & 0xFFF) {
    case IOCTL_MAPMEM_MAP_USER_PHYSICAL_MEMORY:
        if(nInBufferSize != sizeof(PHYSICAL_MEMORY_INFO))
            return(FALSE);

        memmap.a.phys = meminfo->BusAddress.u.LowPart;
        memmap.size = meminfo->Length;
        dprintf(("DeviceIoControl map phys address %X length %X\n", memmap.a.phys, memmap.size));
        if(mpioctl((HFILE)hDevice, IOCTL_MAP, &memmap) == -1) {
            dprintf(("mpioctl failed!\n"));
            return(FALSE);
        }

        dprintf(("DeviceIoControl map virt address = %X\n", memmap.a.user));
        *(ULONG *)lpOutBuffer = (ULONG)memmap.a.user;
        break;
    case IOCTL_MAPMEM_UNMAP_USER_PHYSICAL_MEMORY:
        dprintf(("Unmap mapping %X\n", *(ULONG *)lpInBuffer));
        memmap.a.phys = *(ULONG *)lpInBuffer;
        memmap.size = 0;
        if(mpioctl((HFILE)hDevice, IOCTL_MAP, &memmap) == -1)
            return(FALSE);
        break;
    default:
        dprintf(("MAPMEMIOCtl unknown func %X\n", (dwIoControlCode >> 2) & 0xFFF));
        return(FALSE);
  }

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
static BOOL FXMEMMAPIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
 struct map_ioctl memmap;
 MAPDEVREQUEST *vxdmem = (MAPDEVREQUEST *)lpInBuffer;

  switch(dwIoControlCode) {
    case 1:
        break;
    case 2:
        memmap.a.phys = (DWORD)vxdmem->mdr_PhysicalAddress;
        memmap.size = vxdmem->mdr_SizeInBytes;
        dprintf(("DeviceIoControl map phys address %X length %X\n", memmap.a.phys, memmap.size));
        if(mpioctl((HFILE)hDevice, IOCTL_MAP, &memmap) == -1) {
            dprintf(("mpioctl failed!\n"));
            return(FALSE);
        }

        dprintf(("DeviceIoControl map virt address = %X\n", memmap.a.user));
        vxdmem->mdr_LinearAddress = (PVOID)memmap.a.user;
        break;
    default:
        dprintf(("FXMEMMAPIOCtl unknown func %X\n", (dwIoControlCode >> 2) & 0xFFF));
        return(FALSE);
  }

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
HMCustomDriver::HMCustomDriver(HINSTANCE hInstance, LPCSTR lpDeviceName, LPVOID lpDriverData)
                : HMDeviceDriver(lpDeviceName), hDrvDll(0), lpDriverData(NULL)
{
    hDrvDll = hInstance ;
    pfnDriverOpen  = (PFNDRVOPEN) GetProcAddress(hDrvDll, "DrvOpen");
    pfnDriverClose = (PFNDRVCLOSE)GetProcAddress(hDrvDll, "DrvClose");
    pfnDriverRead  = (PFNDRVREAD) GetProcAddress(hDrvDll, "DrvRead");
    pfnDriverWrite = (PFNDRVWRITE)GetProcAddress(hDrvDll, "DrvWrite");
    pfnDriverIOCtl = (PFNDRVIOCTL)GetProcAddress(hDrvDll, "DrvIOCtl");
}
//******************************************************************************
//******************************************************************************
HMCustomDriver::HMCustomDriver(PFNDRVOPEN pfnDriverOpen, PFNDRVCLOSE pfnDriverClose, 
                               PFNDRVIOCTL pfnDriverIOCtl, PFNDRVREAD pfnDriverRead,
                               PFNDRVWRITE pfnDriverWrite, PFNDRVCANCELIO pfnDriverCancelIo,
                               PFNDRVGETOVERLAPPEDRESULT pfnDriverGetOverlappedResult,
                               LPCSTR lpDeviceName, LPVOID lpDriverData)
                : HMDeviceDriver(lpDeviceName), hDrvDll(0)
{
    if(!pfnDriverOpen || !pfnDriverClose) {
        DebugInt3();
    }
    this->pfnDriverOpen     = pfnDriverOpen;
    this->pfnDriverClose    = pfnDriverClose;
    this->pfnDriverIOCtl    = pfnDriverIOCtl;
    this->pfnDriverRead     = pfnDriverRead;
    this->pfnDriverWrite    = pfnDriverWrite;
    this->pfnDriverCancelIo = pfnDriverCancelIo;
    this->pfnDriverGetOverlappedResult = pfnDriverGetOverlappedResult;
    this->lpDriverData      = lpDriverData;
}
//******************************************************************************
//******************************************************************************
HMCustomDriver::~HMCustomDriver()
{
   if(hDrvDll) FreeLibrary(hDrvDll);
}
//******************************************************************************
//******************************************************************************
DWORD HMCustomDriver::CreateFile (LPCSTR lpFileName,
                                  PHMHANDLEDATA pHMHandleData,
                                  PVOID         lpSecurityAttributes,
                                  PHMHANDLEDATA pHMHandleDataTemplate)
{
   pHMHandleData->hHMHandle = pfnDriverOpen(lpDriverData, pHMHandleData->dwAccess, pHMHandleData->dwShare, pHMHandleData->dwFlags, (PVOID *)&pHMHandleData->dwUserData);
   if(pHMHandleData->hHMHandle == INVALID_HANDLE_VALUE_W) {
       return GetLastError();
   }
   return ERROR_SUCCESS_W;
}
//******************************************************************************
//******************************************************************************
BOOL HMCustomDriver::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
   if(pHMHandleData->hHMHandle) {
	pfnDriverClose(lpDriverData, pHMHandleData->hHMHandle, pHMHandleData->dwFlags, (LPVOID)pHMHandleData->dwUserData);
   }
   pHMHandleData->hHMHandle = 0;
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL HMCustomDriver::DeviceIoControl(PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                     LPVOID lpInBuffer, DWORD nInBufferSize,
                                     LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                     LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
   BOOL ret;

   if(pfnDriverIOCtl == NULL) {
       dprintf(("HMCustomDriver::DeviceIoControl: pfnDriverIOCtl == NULL"));
       ::SetLastError(ERROR_INVALID_FUNCTION_W);
       return FALSE;
   }

   ret = pfnDriverIOCtl(lpDriverData, pHMHandleData->hHMHandle, pHMHandleData->dwFlags, dwIoControlCode, lpInBuffer, nInBufferSize,
                        lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped, (LPVOID)pHMHandleData->dwUserData);
   dprintf(("DeviceIoControl %x returned %d", dwIoControlCode, ret));
   return ret;
}
/*****************************************************************************
 * Name      : BOOL HMCustomDriver::ReadFile
 * Purpose   : read data from handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToRead,
 *             LPDWORD       lpNumberOfBytesRead,
 *             LPOVERLAPPED  lpOverlapped
 *             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMCustomDriver::ReadFile(PHMHANDLEDATA pHMHandleData,
                              LPCVOID       lpBuffer,
                              DWORD         nNumberOfBytesToRead,
                              LPDWORD       lpNumberOfBytesRead,
                              LPOVERLAPPED  lpOverlapped,
                              LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
   BOOL ret;

   if(pfnDriverRead == NULL) {
       dprintf(("HMCustomDriver::ReadFile: pfnDriverRead == NULL"));
       ::SetLastError(ERROR_INVALID_FUNCTION_W);
       return FALSE;
   }
   ret = pfnDriverRead(lpDriverData, pHMHandleData->hHMHandle, pHMHandleData->dwFlags, lpBuffer, nNumberOfBytesToRead,
                      lpNumberOfBytesRead, lpOverlapped, lpCompletionRoutine,
                      (LPVOID)pHMHandleData->dwUserData);
   dprintf(("pfnDriverRead %x %x %x %x %x %x returned %x", pHMHandleData->hHMHandle, lpBuffer, nNumberOfBytesToRead,
             lpNumberOfBytesRead, lpOverlapped, lpCompletionRoutine, ret));
   return ret;
}
/*****************************************************************************
 * Name      : BOOL HMCustomDriver::WriteFile
 * Purpose   : write data to handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToWrite,
 *             LPDWORD       lpNumberOfBytesWritten,
 *             LPOVERLAPPED  lpOverlapped
 *             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMCustomDriver::WriteFile(PHMHANDLEDATA pHMHandleData,
                               LPCVOID       lpBuffer,
                               DWORD         nNumberOfBytesToWrite,
                               LPDWORD       lpNumberOfBytesWritten,
                               LPOVERLAPPED  lpOverlapped,
                               LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
   BOOL ret;

   if(pfnDriverWrite == NULL) {
       dprintf(("HMCustomDriver::WriteFile: pfnDriverWrite == NULL"));
       ::SetLastError(ERROR_INVALID_FUNCTION_W);
       return FALSE;
   }
   ret = pfnDriverWrite(lpDriverData, pHMHandleData->hHMHandle, pHMHandleData->dwFlags, lpBuffer, nNumberOfBytesToWrite,
                        lpNumberOfBytesWritten, lpOverlapped, lpCompletionRoutine,
                        (LPVOID)pHMHandleData->dwUserData);
   dprintf(("pfnDriverWrite %x %x %x %x %x %x returned %x", pHMHandleData->hHMHandle, lpBuffer, nNumberOfBytesToWrite,
            lpNumberOfBytesWritten, lpOverlapped, lpCompletionRoutine, ret));
   return ret;
}
/*****************************************************************************
 * Name      : DWORD HMCustomDriver::CancelIo
 * Purpose   : cancel pending IO operation
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMCustomDriver::CancelIo(PHMHANDLEDATA pHMHandleData)
{
   BOOL ret;

   if(pfnDriverCancelIo == NULL) {
       dprintf(("HMCustomDriver::CancelIo: pfnDriverCancelIo == NULL"));
       ::SetLastError(ERROR_INVALID_FUNCTION_W);
       return FALSE;
   }
   ret = pfnDriverCancelIo(lpDriverData, pHMHandleData->hHMHandle, pHMHandleData->dwFlags, (LPVOID)pHMHandleData->dwUserData);
   dprintf(("pfnDriverCancelIo %x returned %x", pHMHandleData->hHMHandle, ret));
   return ret;
}
/*****************************************************************************
 * Name      : DWORD HMCustomDriver::GetOverlappedResult
 * Purpose   : asynchronus I/O
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LPOVERLAPPED  arg2
 *             LPDWORD       arg3
 *             BOOL          arg4
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMCustomDriver::GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                         LPOVERLAPPED  lpOverlapped,
                                         LPDWORD       lpcbTransfer,
                                         BOOL          fWait)
{
    dprintf(("KERNEL32-HMCustomDriver: HMCustomDriver::GetOverlappedResult(%08xh,%08xh,%08xh,%08xh)",
             pHMHandleData->hHMHandle, lpOverlapped, lpcbTransfer, fWait));

    if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED_W)) {
        dprintf(("!WARNING!: GetOverlappedResult called for a handle that wasn't opened with FILE_FLAG_OVERLAPPED"));
        return TRUE; //NT4, SP6 doesn't fail
    }
    if(!lpOverlapped) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(pfnDriverGetOverlappedResult == NULL) {
        dprintf(("HMCustomDriver::GetOverlappedResult: pfnDriverGetOverlappedResult == NULL"));
        ::SetLastError(ERROR_INVALID_FUNCTION_W);
        return FALSE;
    }
    return pfnDriverGetOverlappedResult(lpDriverData, pHMHandleData->hHMHandle, pHMHandleData->dwFlags, 
                                        lpOverlapped, lpcbTransfer, fWait, (LPVOID)pHMHandleData->dwUserData);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
{
    QWORD  time;
    APIRET rc;

    rc = DosTmrQueryTime(&time);
    if(rc) {
    	dprintf(("DosTmrQueryTime returned %d\n", rc));
    	return(FALSE);
    }
    lpPerformanceCount->u.LowPart  = time.ulLo;
    lpPerformanceCount->u.HighPart = time.ulHi;
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency)
{
    APIRET  rc;
    ULONG   freq;

    rc = DosTmrQueryFreq(&freq);
    if(rc) {
    	dprintf(("DosTmrQueryFreq returned %d\n", rc));
    	return(FALSE);
    }
    lpFrequency->u.LowPart  = freq;
    lpFrequency->u.HighPart = 0;
    dprintf2(("QueryPerformanceFrequency returned 0x%X%X\n", lpFrequency->u.HighPart, lpFrequency->u.LowPart));
    return(TRUE);
}
//******************************************************************************
//******************************************************************************