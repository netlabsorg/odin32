/* $Id: devio.cpp,v 1.3 1999-06-19 10:54:41 sandervl Exp $ */

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

#include <win32type.h>
#include <misc.h>
#include "devio.h"
#include "cio.h"
#include "map.h"
#include "except.h"

static fX86Init  = FALSE;
//SvL: Used in iccio.asm (how can you put these in the .asm data segment without messing things up?)
ULONG  ioentry   = 0;
USHORT gdt       = 0;
char   devname[] = "/dev/fastio$";

static BOOL GpdDevIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
static BOOL MAPMEMIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
static BOOL FXMEMMAPIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

static WIN32DRV knownDriver[] =
    {{"GpdDev", "", TRUE, 666,          GpdDevIOCtl},
    {"MAPMEM", "PMAP$", FALSE, 0,       MAPMEMIOCtl},
    {"FXMEMMAP.VXD", "PMAP$", FALSE, 0,     FXMEMMAPIOCtl}};

static int nrKnownDrivers = sizeof(knownDriver)/sizeof(WIN32DRV);

//******************************************************************************
//******************************************************************************
BOOL WIN32API DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode,
                 LPVOID lpInBuffer, DWORD nInBufferSize,
                 LPVOID lpOutBuffer, DWORD nOutBufferSize,
                 LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
 int i;

  for(i=0;i<nrKnownDrivers;i++) {
    if(hDevice == knownDriver[i].hDevice) {
        return knownDriver[i].devIOCtl(hDevice, dwIoControlCode,
                           lpInBuffer, nInBufferSize,
                           lpOutBuffer, nOutBufferSize,
                           lpBytesReturned, lpOverlapped);
    }
  }
  dprintf(("DeviceIoControl: Device not found!\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
static BOOL GpdDevIOCtl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
 ULONG port, val = 0;

  if(fX86Init == FALSE) {
    if(io_init() == 0)
        fX86Init = TRUE;
    else    return(FALSE);
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
//      _interrupt(3);
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
void OS2CloseFile(HANDLE hDevice)
{
  DosClose((HFILE)hDevice);
}
//******************************************************************************
//******************************************************************************
HANDLE OS2CreateFile(char *szName, DWORD fdwAccess, DWORD fdwShareMode)
{
 APIRET rc;
 int    i;
 char  *drvname = NULL;
 HFILE  hfFileHandle   = 0L;     /* Handle for file being manipulated */
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  sharetype = 0;

  for(i=0;i<nrKnownDrivers;i++) {
    if(strcmp(szName, knownDriver[i].szWin32Name) == 0) {
        drvname = knownDriver[i].szOS2Name;
        break;
    }
  }
  if(drvname == 0) {
    return(0);
  }

  //TODO
  if(knownDriver[i].fCreateFile == TRUE)
    return(knownDriver[i].hDevice);

  if(fdwAccess & (GENERIC_READ | GENERIC_WRITE))
    sharetype |= OPEN_ACCESS_READWRITE;
  else
  if(fdwAccess & GENERIC_WRITE)
    sharetype |= OPEN_ACCESS_WRITEONLY;

  if(fdwShareMode == 0)
    sharetype |= OPEN_SHARE_DENYREADWRITE;
  else
  if(fdwShareMode & (FILE_SHARE_READ | FILE_SHARE_WRITE))
    sharetype |= OPEN_SHARE_DENYNONE;
  else
  if(fdwShareMode & FILE_SHARE_WRITE)
    sharetype |= OPEN_SHARE_DENYREAD;
  else
  if(fdwShareMode & FILE_SHARE_READ)
    sharetype |= OPEN_SHARE_DENYWRITE;

  rc = DosOpen(	drvname,                        /* File path name */
               	&hfFileHandle,                  /* File handle */
               	&ulAction,                      /* Action taken */
	       	0,
         	FILE_NORMAL,
           	FILE_OPEN,
           	sharetype,
               	0L);                            /* No extended attribute */

  dprintf(("DosOpen %s returned %d\n", drvname, rc));

  if(rc == NO_ERROR) {
    knownDriver[i].hDevice = (DWORD)hfFileHandle;
    return((HANDLE)hfFileHandle);
  }
  else  return(0);
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
  dprintf(("OS2QueryPerformanceFrequency returned 0x%X%X\n", lpFrequency->u.HighPart, lpFrequency->u.LowPart));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************

