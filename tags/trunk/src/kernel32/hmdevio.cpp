/* $Id: hmdevio.cpp,v 1.17 2001-11-26 17:16:24 sandervl Exp $ */

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
#if 1
    { "\\\\.\\VPCAppSv", "", TRUE,  667,   VPCIOCtl}};
#else
    };
#endif

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
                    if(hDrvDll) {
                        sprintf(szDrvName, "\\\\.\\%s", szKeyName);
                        driver = new HMCustomDriver(hDrvDll, szDrvName);

                        rc = HMDeviceRegister(szDrvName, driver);
                        if (rc != NO_ERROR)                                  /* check for errors */
                              dprintf(("KERNEL32:RegisterDevices: registering %s failed with %u.\n", szDrvName, rc));
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
                                   PFNDRVIOCTL pfnDriverIOCtl, LPCSTR lpDeviceName)
{
 HMDeviceDriver *driver;
 DWORD rc;
 
    driver = new HMCustomDriver(pfnDriverOpen, pfnDriverClose, pfnDriverIOCtl, lpDeviceName);
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
                : HMDeviceKernelObjectClass(lpDeviceName)
{
    this->fCreateFile = fCreateFile;
    this->szOS2Name   = lpOS2DevName;
    this->devIOCtl    = pDevIOCtl;
}
//******************************************************************************
//******************************************************************************
HMDeviceDriver::HMDeviceDriver(LPCSTR lpDeviceName)
                : HMDeviceKernelObjectClass(lpDeviceName)
{
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceDriver::CreateFile (HANDLE        hHandle,
                                  LPCSTR lpFileName,
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
		return rc;
	}
	dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
	goto tryopen;
  }

  dprintf(("DosOpen %s returned %d\n", szOS2Name, rc));

  if(rc == NO_ERROR) {
     	pHMHandleData->hHMHandle = hfFileHandle;
     	return (NO_ERROR);
  }
  else  return(rc);
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
static BOOL VPCIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
  APIRET rc;

  dprintf(("VPCIOCtl func %x: %x %d %x %d %x %x", dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped));
  switch(dwIoControlCode) {
  case 0x9C402880: //0x00
        if(nOutBufferSize < 4) {
            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }
        *(DWORD *)lpOutBuffer = 0x60001;
        *lpBytesReturned = 4;
        return TRUE;

  case 0x9C402894: //0x14 (get IDT table)
  {
        DWORD *lpBuffer = (DWORD *)lpOutBuffer;
        if(nOutBufferSize < 0x800) {
            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }
        memset(lpOutBuffer, 0, nOutBufferSize);
        for(int i=0;i<32;i++) {
            lpBuffer[i*2]   = 0x0178c4c8;
            lpBuffer[i*2+1] = 0xfff18F00;
        }
        for(i=0x50;i<0x57;i++) {
            lpBuffer[i*2]   = 0x0178c4c8;
            lpBuffer[i*2+1] = 0xfff18E00;
        }
        for(i=0x70;i<0x77;i++) {
            lpBuffer[i*2]   = 0x0178c4c8;
            lpBuffer[i*2+1] = 0xfff18E00;
        }
        lpBuffer[0x4F*2]   = 0x0178c4c8;
        lpBuffer[0x4F*2+1] = 0xfff18E00;
        lpBuffer[0xEF*2]   = 0x0178c4c8;
        lpBuffer[0xEF*2+1] = 0xfff18E00;
        *lpBytesReturned = 0xFF*8;
        return TRUE;
  }
  case 0x9C40288C: //0x0C change IDT
        if(nInBufferSize < 0x22) {
            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }
        fVirtualPC = TRUE;
        return TRUE;

  case 0x9C402884: //0x04 ExAllocatePoolWithTag
  {
        DWORD *lpBuffer = (DWORD *)lpInBuffer;
        if(nInBufferSize < 0x08) {
            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }
        dprintf(("In: %x %x", lpBuffer[0], lpBuffer[1]));
        return TRUE;
  }

  case 0x9C402898: //0x18 Remove IDT patch
        if(nInBufferSize < 0x01) {
            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }
        fVirtualPC = FALSE;
        return TRUE;
  default:
        dprintf(("VPCIOCtl unknown func %X\n", dwIoControlCode));
        return FALSE;
  }
}
//******************************************************************************
//******************************************************************************
HMCustomDriver::HMCustomDriver(HINSTANCE hInstance, LPCSTR lpDeviceName)
                : HMDeviceDriver(lpDeviceName), hDrvDll(0)
{
    hDrvDll = hInstance ;
    *(ULONG *)&driverOpen  = (ULONG)GetProcAddress(hDrvDll, "DrvOpen");
    *(ULONG *)&driverClose = (ULONG)GetProcAddress(hDrvDll, "DrvClose");
    *(ULONG *)&driverIOCtl = (ULONG)GetProcAddress(hDrvDll, "DrvIOCtl");
}
//******************************************************************************
//******************************************************************************
HMCustomDriver::HMCustomDriver(PFNDRVOPEN pfnDriverOpen, PFNDRVCLOSE pfnDriverClose, 
                               PFNDRVIOCTL pfnDriverIOCtl, LPCSTR lpDeviceName)
                : HMDeviceDriver(lpDeviceName), hDrvDll(0)
{
    driverOpen  = pfnDriverOpen;
    driverClose = pfnDriverClose;
    driverIOCtl = pfnDriverIOCtl;
}
//******************************************************************************
//******************************************************************************
HMCustomDriver::~HMCustomDriver()
{
   if(hDrvDll) FreeLibrary(hDrvDll);
}
//******************************************************************************
//******************************************************************************
DWORD HMCustomDriver::CreateFile (HANDLE        hHandle, 
                                  LPCSTR lpFileName,
                                  PHMHANDLEDATA pHMHandleData,
                                  PVOID         lpSecurityAttributes,
                                  PHMHANDLEDATA pHMHandleDataTemplate)
{
   pHMHandleData->hHMHandle = driverOpen(pHMHandleData->dwAccess, pHMHandleData->dwShare);
   if(pHMHandleData->hHMHandle == 0) {
       return 2;
   }
   return 0;
}
//******************************************************************************
//******************************************************************************
BOOL HMCustomDriver::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
   if(pHMHandleData->hHMHandle) {
	driverClose(pHMHandleData->hHMHandle);
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

   ret = driverIOCtl(pHMHandleData->hHMHandle, dwIoControlCode, lpInBuffer, nInBufferSize,
                     lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped);
   dprintf(("DeviceIoControl %x returned %d", dwIoControlCode, ret));
   return ret;
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
