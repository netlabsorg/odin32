/*
 * Win32 mailslot APIs
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * This is not a complete implementation. Just a partial implementation
 * using named pipes.
 *
 * TODO: Doesn't work for remote mailslots (on other machines)
 * TODO: Writing to multiple mailslot servers doesn't work (\\*\mailslot\xxx)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <string.h>
#include "HandleManager.h"
#include "HMMailslot.h"

#define DBG_LOCALLOG    DBG_hmmailslot
#include "dbglocal.h"

HMMailSlotInfo::HMMailSlotInfo(LPCSTR lpszName, HANDLE hPipe, DWORD nMaxMessageSize, 
                               DWORD lReadTimeout, BOOL fServer,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  this->lpszName = (LPSTR)malloc(strlen(lpszName)+1);
  if(!this->lpszName) {
    DebugInt3();
  }
  strcpy(this->lpszName, lpszName);
  this->lpSecurityAttributes = lpSecurityAttributes;
  lpMessage = NULL;
  dwMessageCount = 0;

  this->nMaxMessageSize = nMaxMessageSize;
  this->lReadTimeout = lReadTimeout;
  this->fServer      = fServer;
  this->hPipe        = hPipe;
}

HMMailSlotInfo::~HMMailSlotInfo()
{
    if(lpszName) free(lpszName);
    if(hPipe)    CloseHandle(hPipe);
}


HMMailslotClass::HMMailslotClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName)
{
    HMDeviceRegisterEx("\\\\.\\MAILSLOT", this, NULL);
}

/*****************************************************************************
 * Name      : HMMailslotClass::FindDevice
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
BOOL HMMailslotClass::FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength)
{
    //\\*\mailslot\      length 13
    if((lstrncmpiA(lpDeviceName, "\\\\*\\mailslot\\", 13) == 0))
    {
        return TRUE;
    }
    if((lstrncmpiA(lpDeviceName, "\\\\.\\mailslot\\", 13) == 0))
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 * Name      : HANDLE WIN32API CreateMailslotA
 * Purpose   : The CreateMailslot function creates a mailslot with the specified
 *             name and returns a handle that a mailslot server can use to
 *             perform operations on the mailslot. The mailslot is local to the
 *             computer that creates it. An error occurs if a mailslot with
 *             the specified name already exists.
 * Parameters: LPCSTR lpName              pointer to string for mailslot name
 *             DWORD nMaxMessageSize      maximum message size
 *             DWORD lReadTimeout         milliseconds before read time-out
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes pointer to security structure
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to
 *             the mailslot, for use in server mailslot operations.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMMailslotClass::CreateMailslotA(PHMHANDLEDATA pHMHandleData,
                                      LPCSTR lpName, DWORD nMaxMessageSize,
                                      DWORD lReadTimeout,
                                      LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  HMMailSlotInfo *mailslot;
  HANDLE hPipe;

  dprintf(("KERNEL32: CreateMailslotA(%s,%08x,%08x,%08x) partially implemented",
           lpName, nMaxMessageSize, lReadTimeout, lpSecurityAttributes));


  pHMHandleData->dwUserData = 0;

  if(lpName == NULL) {
      SetLastError(ERROR_PATH_NOT_FOUND);
      return FALSE;
  }

  char *pipename = (char *)alloca(strlen(lpName)+16);
  if(pipename == NULL) {
      DebugInt3();
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
  }
  strcpy(pipename, "\\\\.\\pipe\\");
  strcat(pipename, lpName);
  //TODO: lookup name and fail if exists
  hPipe = CreateNamedPipeA(pipename, PIPE_ACCESS_INBOUND, 
                           PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, 
                           PIPE_UNLIMITED_INSTANCES, MAILSLOT_SIZE, 
                           (nMaxMessageSize) ? nMaxMessageSize : MAILSLOT_SIZE, 
                           lReadTimeout, lpSecurityAttributes);

  if(hPipe == INVALID_HANDLE_VALUE) { 
      dprintf(("CreateMailslotA: unable to create pipe %s", pipename));
      return FALSE;
  }
  ::ConnectNamedPipe(hPipe, NULL);
  SetLastError(0);
  if(lReadTimeout == MAILSLOT_WAIT_FOREVER) {
     DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
     ::SetNamedPipeHandleState(hPipe, &mode, NULL, NULL);
  }
  else
  if(lReadTimeout != 0) { 
     dprintf(("WARNING: timeout %x not supported", lReadTimeout));
  }
  mailslot = new HMMailSlotInfo(lpName, hPipe, nMaxMessageSize, lReadTimeout, TRUE, lpSecurityAttributes);
  if(mailslot == NULL) {
      DebugInt3();
      return FALSE;
  }
  pHMHandleData->dwUserData = (DWORD)mailslot;
  return TRUE;
}

/*****************************************************************************
 * Name      : DWORD HMMailslotClass::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMMailslotClass::CreateFile (LPCSTR        lpFileName,
                                   PHMHANDLEDATA pHMHandleData,
                                   PVOID         lpSecurityAttributes,
                                   PHMHANDLEDATA pHMHandleDataTemplate)
{
  HMMailSlotInfo *mailslot;
  HANDLE hPipe;

  dprintf(("HMMailslotClass::CreateFile: %s", lpFileName));

  pHMHandleData->dwUserData = 0;

  char *pipename = (char *)alloca(strlen(lpFileName)+16);
  if(pipename == NULL) {
      DebugInt3();
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
  }
  strcpy(pipename, "\\\\.\\pipe\\");
  strcat(pipename, lpFileName);

tryagain:
  hPipe = ::CreateFileA(pipename, pHMHandleData->dwAccess,
                        pHMHandleData->dwShare, (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                        pHMHandleData->dwCreation,
                        pHMHandleData->dwFlags, 0);

  if(hPipe == INVALID_HANDLE_VALUE) { 
      if(pipename[11] == '*') {
          dprintf(("pipename with asterix not supported; connect only to one mailslot"));
          pipename[11] = '.';
          goto tryagain;
      }
      dprintf(("CreateMailslotA: unable to create pipe %s", pipename));
      return GetLastError();
  }
  //todo: lookup name and fail if exists
  mailslot = new HMMailSlotInfo(lpFileName, hPipe, -1, 0, FALSE, (LPSECURITY_ATTRIBUTES)lpSecurityAttributes);
  if(mailslot == NULL) {
      DebugInt3();
      return ERROR_NOT_ENOUGH_MEMORY;
  }
  pHMHandleData->dwUserData = (DWORD)mailslot;
  return NO_ERROR;
}
/*****************************************************************************
 * Name      : DWORD HMMailslotClass::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMMailslotClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  HMMailSlotInfo *mailslot = (HMMailSlotInfo *)pHMHandleData->dwUserData;

  dprintf(("KERNEL32: HMMailslotClass::CloseHandle(%08x)", pHMHandleData->hHMHandle));

  if(mailslot) {
      delete mailslot;
  }
  return TRUE;
}

/*****************************************************************************
 * Name      : BOOL GetMailslotInfo
 * Purpose   : The GetMailslotInfo function retrieves information about the
 *             specified mailslot.
 * Parameters: HANDLE  hMailslot        mailslot handle
 *             LPDWORD lpMaxMessageSize address of maximum message size
 *             LPDWORD lpNextSize       address of size of next message
 *             LPDWORD lpMessageCount   address of number of messages
 *             LPDWORD lpReadTimeout    address of read time-out
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMMailslotClass::GetMailslotInfo(PHMHANDLEDATA pHMHandleData,
                                      LPDWORD lpMaxMessageSize,
                                      LPDWORD lpNextSize,
                                      LPDWORD lpMessageCount,
                                      LPDWORD lpReadTimeout)
{
    DWORD bytesread, bytesavail, msgsize;
    HMMailSlotInfo *mailslot = (HMMailSlotInfo *)pHMHandleData->dwUserData;

    dprintf(("KERNEL32: GetMailslotInfo(%08xh,%08xh,%08xh,%08xh,%08xh) partially implemented",
             pHMHandleData->hHMHandle,
             lpMaxMessageSize,
             lpNextSize,
             lpMessageCount,
             lpReadTimeout));

    if(mailslot == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(mailslot->fServer == FALSE) {
        dprintf(("GetMailslotInfo not allowed with client handle"));
        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
    }
    if(::PeekNamedPipe(mailslot->hPipe, NULL, 0, &bytesread, &bytesavail, &msgsize) == FALSE) {
        dprintf(("ERROR: GetMailslotInfo: PeekNamedPipe failed!"));
        return FALSE;
    }
    if(lpMaxMessageSize)   *lpMaxMessageSize = mailslot->nMaxMessageSize;
    if(lpReadTimeout)      *lpReadTimeout    = mailslot->lReadTimeout;

    //TODO: get correct number of messages!
    if(bytesavail) {
        if(lpNextSize)         *lpNextSize       = msgsize;
        if(lpMessageCount)     *lpMessageCount   = 1;
    }
    else {
        if(lpNextSize)         *lpNextSize       = 0;
        if(lpMessageCount)     *lpMessageCount   = 0;
    }
    return TRUE;
}

/*****************************************************************************
 * Name      : BOOL SetMailslotInfo
 * Purpose   : The SetMailslotInfo function sets the time-out value used by the
 *             specified mailslot for a read operation.
 * Parameters: HANDLE hObject       handle to a mailslot object
 *             DWORD  dwReadTimeout read time-out
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMMailslotClass::SetMailslotInfo(PHMHANDLEDATA pHMHandleData,
                                      DWORD  dwReadTimeout)
{
    HMMailSlotInfo *mailslot = (HMMailSlotInfo *)pHMHandleData->dwUserData;

    dprintf(("KERNEL32: SetMailslotInfo(%08xh,%08xh) partially implemented",
              pHMHandleData->hHMHandle, dwReadTimeout));

    if(mailslot == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(mailslot->fServer == FALSE) {
        dprintf(("SetMailslotInfo not allowed with client handle"));
        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
    }
    mailslot->lReadTimeout = dwReadTimeout;
    return(FALSE);
}

/*****************************************************************************
 * Name      : BOOL HMMailslotClass::ReadFile
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

BOOL HMMailslotClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                               LPCVOID       lpBuffer,
                               DWORD         nNumberOfBytesToRead,
                               LPDWORD       lpNumberOfBytesRead,
                               LPOVERLAPPED  lpOverlapped)
{
    HMMailSlotInfo *mailslot = (HMMailSlotInfo *)pHMHandleData->dwUserData;
    dprintf(("KERNEL32: HMMailslotClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)",
             lpHMDeviceName,  pHMHandleData, lpBuffer, nNumberOfBytesToRead,
             lpNumberOfBytesRead, lpOverlapped));

    if(lpNumberOfBytesRead)
        *lpNumberOfBytesRead = 0;

    if(mailslot == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(mailslot->fServer == FALSE) {
        dprintf(("ReadFile not allowed with client handle"));
        SetLastError(ERROR_INVALID_FUNCTION); //TODO: right error?
        return FALSE;
    }
    return ::ReadFile(mailslot->hPipe, (LPVOID)lpBuffer, nNumberOfBytesToRead,
                      lpNumberOfBytesRead, lpOverlapped);
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
BOOL HMMailslotClass::ReadFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToRead,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    HMMailSlotInfo *mailslot = (HMMailSlotInfo *)pHMHandleData->dwUserData;

    dprintf(("HMMailslotClass::ReadFileEx(%08xh,%08xh,%08xh,%08xh,%08xh)",  pHMHandleData->hHMHandle,
             lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine));

    if(mailslot == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(mailslot->fServer == FALSE) {
        dprintf(("ReadFile not allowed with client handle"));
        SetLastError(ERROR_INVALID_FUNCTION); //TODO: right error?
        return FALSE;
    }
    return ::ReadFileEx(mailslot->hPipe, lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);
}


/*****************************************************************************
 * Name      : BOOL HMMailslotClass::WriteFile
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

BOOL HMMailslotClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToWrite,
                                    LPDWORD       lpNumberOfBytesWritten,
                                    LPOVERLAPPED  lpOverlapped)
{
    HMMailSlotInfo *mailslot = (HMMailSlotInfo *)pHMHandleData->dwUserData;

    dprintf(("KERNEL32: HMMailslotClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x)",
             lpHMDeviceName, pHMHandleData, lpBuffer, nNumberOfBytesToWrite,
             lpNumberOfBytesWritten, lpOverlapped));

    if(lpNumberOfBytesWritten)
        *lpNumberOfBytesWritten = 0;

    if(mailslot == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(mailslot->fServer == TRUE) {
        dprintf(("ReadFile not allowed with server handle"));
        SetLastError(ERROR_INVALID_FUNCTION); //TODO: right error?
        return FALSE;
    }

    return ::WriteFile(mailslot->hPipe, lpBuffer, nNumberOfBytesToWrite,
                       lpNumberOfBytesWritten, lpOverlapped);
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

BOOL HMMailslotClass::WriteFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToWrite,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    HMMailSlotInfo *mailslot = (HMMailSlotInfo *)pHMHandleData->dwUserData;

    dprintf(("HMMailslotClass::WriteFileEx(%08xh,%08xh,%08xh,%08xh,%08xh)",
             pHMHandleData->hHMHandle, lpBuffer, nNumberOfBytesToWrite,
             lpOverlapped,lpCompletionRoutine));

    if(mailslot == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(mailslot->fServer == TRUE) {
        dprintf(("ReadFile not allowed with server handle"));
        SetLastError(ERROR_INVALID_FUNCTION); //TODO: right error?
        return FALSE;
    }
    return ::WriteFileEx(mailslot->hPipe, lpBuffer, nNumberOfBytesToWrite,lpOverlapped,lpCompletionRoutine);
}
