/* $Id: testdrv.cpp,v 1.3 2001-09-05 12:52:27 bird Exp $ */
//******************************************************************************
//******************************************************************************
// Test sample device driver dll
//
// The dll needs to be registered:
// HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Services\\Driver_Name
//     string key 'DllName' with path of driver dll
//******************************************************************************
//******************************************************************************
#define INCL_DOSPROFILE
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_GPI
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#include <os2wrap.h>             //Odin32 OS/2 api wrappers

#include <winconst.h>
#include <win32type.h>
#include <win32api.h>
#include <misc.h>
#include <win\winioctl.h>

#define TESTDRV_CATEGORY    0x40

//******************************************************************************
//******************************************************************************
HANDLE WIN32API DrvOpen(DWORD dwAccess, DWORD dwShare)
{
 APIRET rc;
 HFILE  hfFileHandle   = 0L;     /* Handle for file being manipulated */
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  sharetype = 0;

  if(dwAccess & (GENERIC_READ_W | GENERIC_WRITE_W))
    sharetype |= OPEN_ACCESS_READWRITE;
  else
  if(dwAccess & GENERIC_WRITE_W)
    sharetype |= OPEN_ACCESS_WRITEONLY;

  if(dwShare == 0)
    sharetype |= OPEN_SHARE_DENYREADWRITE;
  else
  if(dwShare & (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W))
    sharetype |= OPEN_SHARE_DENYNONE;
  else
  if(dwShare & FILE_SHARE_WRITE_W)
    sharetype |= OPEN_SHARE_DENYREAD;
  else
  if(dwShare & FILE_SHARE_READ_W)
    sharetype |= OPEN_SHARE_DENYWRITE;

tryopen:
  rc = DosOpen( "TESTDRV$",                     /* File path name */
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

  if(rc == NO_ERROR) {
        return hfFileHandle;
  }
  else  return -1;
}
//******************************************************************************
//******************************************************************************
void WIN32API DrvClose(HANDLE hDevice)
{
  DosClose(hDevice);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DrvIOCtl(HANDLE hDevice, DWORD dwIoControlCode,
                       LPVOID lpInBuffer, DWORD nInBufferSize,
                       LPVOID lpOutBuffer, DWORD nOutBufferSize,
                       LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
  APIRET rc;
  ULONG ioctl;

  switch(dwIoControlCode) {
  default:
        return FALSE;
  }
  dprintf(("DrvIOCtl func %x: %x %d %x %d %x %x", dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped));

  *lpBytesReturned = nOutBufferSize;

  rc = DosDevIOCtl(hDevice, TESTDRV_CATEGORY, ioctl,
                   lpInBuffer, nInBufferSize, &nInBufferSize,
                   lpOutBuffer, nOutBufferSize, lpBytesReturned);

  dprintf(("DrvIOCtl returned %d bytes returned %d", rc , (lpBytesReturned) ? *lpBytesReturned : 0));
  return rc == NO_ERROR;
}
//******************************************************************************
//******************************************************************************

