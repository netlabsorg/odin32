/* $Id: hmopen32.cpp,v 1.21 2000-05-09 18:56:08 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 */


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */

//#define DEBUG_LOCAL

#ifdef DEBUG_LOCAL
#  define dprintfl(a) dprintf(a)
#else
inline void ignore_dprintf(...){}
#  define dprintfl(a) ignore_dprintf(a)
#endif

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <string.h>
#include "HandleManager.h"
#include "HMOpen32.h"
#include "mmap.h"

#define DBG_LOCALLOG	DBG_hmopen32
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

/*****************************************************************************
 * Name      : HMDeviceOpen32Class::_DeviceRequest
 * Purpose   : entry method for special request functions
 * Parameters: ULONG ulRequestCode
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : the standard behaviour is to return an error code for non-
 *             existant request codes
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/
DWORD  HMDeviceOpen32Class::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                        ULONG         ulRequestCode,
                                        ULONG         arg1,
                                        ULONG         arg2,
                                        ULONG         arg3,
                                        ULONG         arg4)
{
  dprintfl(("KERNEL32: HandleManager::Open32::_DeviceRequest %s(%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           ulRequestCode));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMDeviceHandler::DuplicateHandle
 * Purpose   : dummy version
 * Parameters: 
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : the standard behaviour is to return an error code for non-
 *             existant request codes
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/
BOOL HMDeviceOpen32Class::DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                       	       DWORD   fdwAccess,
                       	       BOOL    fInherit,
                               DWORD   fdwOptions)
{
 BOOL rc;

  dprintf(("KERNEL32:HandleManager::Open32::DuplicateHandle %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData,
           srcprocess, pHMSrcHandle->hHMHandle, destprocess, desthandle));

#if 1
  rc = O32_DuplicateHandle(srcprocess, pHMSrcHandle->hHMHandle, destprocess, desthandle, fdwAccess, fInherit, fdwOptions);

  if(rc == TRUE) {
     	pHMHandleData->hHMHandle = *desthandle;
     	return (NO_ERROR);
  }
  else  return(O32_GetLastError());
#else
  rc = OSLibDosDupHandle(pHMSrcHandle->hHMHandle, desthandle);
  if(rc == NO_ERROR)
  {
    pHMHandleData->hHMHandle = *desthandle;
    return (NO_ERROR);
  }
  else
  {
    dprintfl(("KERNEL32: HandleManager::Open32::DuplicateHandle Error %d\n",rc));
    O32_SetLastError(rc);
    return(rc);
  }
#endif
}

/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::CreateFile
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
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::CreateFile (LPCSTR        lpFileName,
                                       PHMHANDLEDATA pHMHandleData,
                                       PVOID         lpSecurityAttributes,
                                       PHMHANDLEDATA pHMHandleDataTemplate)
{
  HFILE hFile;
  HFILE hTemplate;

  dprintfl(("KERNEL32: HandleManager::Open32::CreateFile %s(%s,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData,
           lpSecurityAttributes,
           pHMHandleDataTemplate));

  if (strncmp(lpFileName,       // "support" for local unc names
              "\\\\.\\",
              4) == 0) 
  {
  	lpFileName+=4;
  }

  // create from template
  if (pHMHandleDataTemplate != NULL)
     hTemplate = pHMHandleDataTemplate->hHMHandle;
  else
     hTemplate = 0;

#if 1
  //SvL: Open32 doesn't like this flag
  if(pHMHandleData->dwShare & FILE_SHARE_DELETE) {
	pHMHandleData->dwShare &= ~FILE_SHARE_DELETE;
  }

  hFile = O32_CreateFile(lpFileName,
                         pHMHandleData->dwAccess,
                         pHMHandleData->dwShare,
                         //(LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                         NULL,
                         pHMHandleData->dwCreation,
                         pHMHandleData->dwFlags,
                         hTemplate);
  if (hFile != INVALID_HANDLE_ERROR)
  {
     pHMHandleData->hHMHandle = hFile;
     return (NO_ERROR);
  }
  else
    return(O32_GetLastError());
#else

  rc = OSLibDosCreate((char *)lpFileName,
                       pHMHandleData->dwAccess,
                       pHMHandleData->dwShare,
                       //(LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                       NULL,
                       pHMHandleData->dwCreation,
                       pHMHandleData->dwFlags,
                       hTemplate, &hFile);
  if(rc)
  {
    dprintfl(("KERNEL32: HandleManager::Open32::CreateFile Error %d\n",rc));
    O32_SetLastError(rc);
    return(rc);
  }
  else
  {
    pHMHandleData->hHMHandle = hFile;
    return (NO_ERROR);
  }
#endif
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  BOOL bRC;

  dprintfl(("KERNEL32: HandleManager::Open32::CloseHandle(%08x)\n",
           pHMHandleData->hHMHandle));

#if 1
  bRC = O32_CloseHandle(pHMHandleData->hHMHandle);

  dprintfl(("KERNEL32: HandleManager::Open32::CloseHandle returned %08xh\n",
           bRC));

  return (DWORD)bRC;
#else
  bRC = OSLibDosClose(pHMHandleData->hHMHandle);
  if(bRC)
  {
    dprintfl(("KERNEL32: HandleManager::Open32::CloseHandle Error %d\n",bRC));
    O32_SetLastError(bRC);
    return TRUE;      // MUTEX Problem
    return FALSE;
  }
  else
  {
    return TRUE;
  }
#endif
}


/*****************************************************************************
 * Name      : BOOL HMDeviceOpen32Class::ReadFile
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

BOOL HMDeviceOpen32Class::ReadFile(PHMHANDLEDATA pHMHandleData,
                                   LPCVOID       lpBuffer,
                                   DWORD         nNumberOfBytesToRead,
                                   LPDWORD       lpNumberOfBytesRead,
                                   LPOVERLAPPED  lpOverlapped)
{
  BOOL bRC;
  LPVOID lpRealBuf;

  dprintfl(("KERNEL32: HandleManager::Open32::ReadFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));

#if 1
  Win32MemMap *map;
  DWORD offset;

  //SvL: DosRead doesn't like writing to memory addresses returned by 
  //     DosAliasMem -> search for original memory mapped pointer and use 
  //     that one
  map = Win32MemMapView::findMapByView((ULONG)lpBuffer, &offset, MEMMAP_ACCESS_READ);
  if(map) {
	lpRealBuf = (LPVOID)((ULONG)map->getMappingAddr() + offset);
	DWORD nrpages = nNumberOfBytesToRead/4096;
	if(offset & 0xfff) 
		nrpages++;
	else
	if(nNumberOfBytesToRead & 0xfff) 
		nrpages++;

	map->commitPage(offset & ~0xfff, TRUE, nrpages);
  }
  else  lpRealBuf = (LPVOID)lpBuffer;
  
  bRC = O32_ReadFile(pHMHandleData->hHMHandle,
                     (PVOID)lpRealBuf,
                     nNumberOfBytesToRead,
                     lpNumberOfBytesRead,
                     lpOverlapped);

  if(bRC == 0) {
     	dprintf(("KERNEL32: HandleManager::Open32::ReadFile returned %08xh %x\n",
                  bRC, GetLastError()));
	dprintf(("%x -> %d", lpBuffer, IsBadWritePtr((LPVOID)lpBuffer, nNumberOfBytesToRead)));
  }
  else 	dprintfl(("KERNEL32: HandleManager::Open32::ReadFile returned %08xh\n",
                   bRC));

  return bRC;
#else
  rc = OSLibDosRead(pHMHandleData->hHMHandle,
                    (PVOID) lpBuffer,
                    nNumberOfBytesToRead,
                    lpNumberOfBytesRead);
  if(rc)
  {
    dprintfl(("KERNEL32: HandleManager::Open32::ReadFile Error %d\n",rc));
    O32_SetLastError(rc);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
#endif
}


/*****************************************************************************
 * Name      : BOOL HMDeviceOpen32Class::WriteFile
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

BOOL HMDeviceOpen32Class::WriteFile(PHMHANDLEDATA pHMHandleData,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToWrite,
                                    LPDWORD       lpNumberOfBytesWritten,
                                    LPOVERLAPPED  lpOverlapped)
{
  BOOL bRC;

  dprintfl(("KERNEL32: HandleManager::Open32::WriteFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

#if 1
  bRC = O32_WriteFile(pHMHandleData->hHMHandle,
                      lpBuffer,
                      nNumberOfBytesToWrite,
                      lpNumberOfBytesWritten,
                      lpOverlapped);

  dprintfl(("KERNEL32: HandleManager::Open32::WriteFile returned %08xh\n",
           bRC));

  return bRC;
#else
  rc = OSLibDosWrite(pHMHandleData->hHMHandle,
                     (PVOID) lpBuffer,
                     nNumberOfBytesToWrite,
                     lpNumberOfBytesWritten);
  if(rc)
  {
    dprintfl(("KERNEL32: HandleManager::Open32::WriteFile Error %d\n",rc));
    O32_SetLastError(rc);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
#endif
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HandleManager::Open32::GetFileType %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData));

#if 1
  return O32_GetFileType(pHMHandleData->hHMHandle);
#else
  // TODO: FILE_TYPE_DISK
  return FILE_TYPE_CHAR;
#endif
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::GetFileInformationByHandle
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA               pHMHandleData
 *             BY_HANDLE_FILE_INFORMATION* pHFI
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::GetFileInformationByHandle(PHMHANDLEDATA               pHMHandleData,
                                                      BY_HANDLE_FILE_INFORMATION* pHFI)
{
  dprintfl(("KERNEL32: HandleManager::Open32::GetFileInformationByHandle %s(%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pHFI));

  return O32_GetFileInformationByHandle(pHMHandleData->hHMHandle,
                                        pHFI);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceOpen32Class::SetEndOfFile
 * Purpose   : set end of file marker
 * Parameters: PHMHANDLEDATA              pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceOpen32Class::SetEndOfFile(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HandleManager::Open32::SetEndOfFile %s(%08xh)\n",
           lpHMDeviceName,
           pHMHandleData));

  return O32_SetEndOfFile(pHMHandleData->hHMHandle);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceOpen32Class::SetFileTime
 * Purpose   : set file time
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PFILETIME     pFT1
 *             PFILETIME     pFT2
 *             PFILETIME     pFT3
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceOpen32Class::SetFileTime(PHMHANDLEDATA pHMHandleData,
                                      LPFILETIME pFT1,
                                      LPFILETIME pFT2,
                                      LPFILETIME pFT3)
{
  dprintfl(("KERNEL32: HandleManager::Open32::SetFileTime %s(%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pFT1,
           pFT2,
           pFT3));

  return O32_SetFileTime(pHMHandleData->hHMHandle,
                         pFT1,
                         pFT2,
                         pFT3);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::GetFileSize
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

DWORD HMDeviceOpen32Class::GetFileSize(PHMHANDLEDATA pHMHandleData,
                                       PDWORD        lpdwFileSizeHigh)
{
  dprintfl(("KERNEL32: HandleManager::Open32::GetFileSize %s(%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           lpdwFileSizeHigh));

#if 1
  if(lpdwFileSizeHigh)
    *lpdwFileSizeHigh = 0;

  return O32_GetFileSize(pHMHandleData->hHMHandle,
                         lpdwFileSizeHigh);
#else
  size = OSLibDosGetFileSize(pHMHandleData->hHMHandle);
  if(pSize)
    *pSize = 0;
  return size;
#endif
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::SetFilePointer
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

DWORD HMDeviceOpen32Class::SetFilePointer(PHMHANDLEDATA pHMHandleData,
                                          LONG          lDistanceToMove,
                                          PLONG         lpDistanceToMoveHigh,
                                          DWORD         dwMoveMethod)
{
  dprintfl(("KERNEL32: HandleManager::Open32::SetFilePointer %s(%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           lDistanceToMove,
           lpDistanceToMoveHigh,
           dwMoveMethod));

#if 1
  DWORD ret;

  if(lpDistanceToMoveHigh)
    *lpDistanceToMoveHigh = 0;

  ret = O32_SetFilePointer(pHMHandleData->hHMHandle,
                           lDistanceToMove,
                           lpDistanceToMoveHigh,
                           dwMoveMethod);

//  if(ret == -1) {
//	dprintf(("current position %x (error = %x)", O32_SetFilePointer(pHMHandleData->hHMHandle,0, 0, 1), GetLastError()));
//  }
  return ret;
#else

  if(lpDistanceToMoveHigh)
    *lpDistanceToMoveHigh = 0;
  pos = OSLibDosSetFilePtr2(pHMHandleData->hHMHandle, lDistanceToMove, dwMoveMethod);
  return pos;
#endif
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::LockFile
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD arg2
 *             DWORD arg3
 *             DWORD arg4
 *             DWORD arg5
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::LockFile(PHMHANDLEDATA pHMHandleData,
                                    DWORD         arg2,
                                    DWORD         arg3,
                                    DWORD         arg4,
                                    DWORD         arg5)
{
  dprintfl(("KERNEL32: HandleManager::Open32::LockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           arg2,
           arg3,
           arg4,
           arg5));

  return O32_LockFile(pHMHandleData->hHMHandle,
                      arg2,
                      arg3,
                      arg4,
                      arg5);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::LockFileEx
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwFlags
 *             DWORD dwReserved
 *             DWORD nNumberOfBytesToLockLow
 *             DWORD nNumberOfBytesToLockHigh
 *             LPOVERLAPPED lpOverlapped
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::LockFileEx(PHMHANDLEDATA pHMHandleData,
                                      DWORD         dwFlags,
                                      DWORD         dwReserved,
                                      DWORD         nNumberOfBytesToLockLow,
                                      DWORD         nNumberOfBytesToLockHigh,
                                      LPOVERLAPPED  lpOverlapped)
{

  dprintfl(("KERNEL32: HandleManager::Open32::LockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwFlags,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));


  return(O32_LockFile(pHMHandleData->hHMHandle,
                      lpOverlapped->Offset,
                      lpOverlapped->OffsetHigh,
                      nNumberOfBytesToLockLow,
                      nNumberOfBytesToLockHigh));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::OpenFile
 * Purpose   : this is called from the handle manager if a OpenFile() is
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
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::OpenFile (LPCSTR        lpFileName,
                                     PHMHANDLEDATA pHMHandleData,
                                     OFSTRUCT      *pOFStruct,
                                     UINT          fuMode)
{
  HFILE hFile;
  FILETIME filetime;
  WORD filedatetime[2];
  char filepath[260];

  dprintfl(("KERNEL32: HandleManager::Open32::OpenFile %s(%s,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData,
           pOFStruct,
           fuMode));

  if(strcmp(lpFileName,       // "support" for local unc names
             "\\\\.\\") == 0)
  {
    lpFileName+=4;
  }
  else 
  if(!strchr(lpFileName, ':') && !strchr(lpFileName, '\\')) 
  {
	//filename only; search for file in following order
	//1: dir from which the app loaded
	//2: current dir
	//3: windows system dir
	//4: windows dir
	//5: dirs in path path environment variable
	//SearchPath does exactly that
	LPSTR filenameinpath;

	if(SearchPathA(NULL, lpFileName, NULL, sizeof(filepath), filepath, &filenameinpath) == 0
           && !(fuMode & OF_CREATE) ) {
		SetLastError(ERROR_FILE_NOT_FOUND);
		return HFILE_ERROR;
	}
	lpFileName = filepath;
  }
  // filling OFSTRUCT
  memset(pOFStruct, 0, sizeof(OFSTRUCT));
  pOFStruct->cBytes = sizeof(OFSTRUCT);
  pOFStruct->nErrCode = 0;
  strncpy((char *)pOFStruct->szPathName, lpFileName, OFS_MAXPATHNAME - 1);

  hFile = O32_OpenFile(lpFileName,
                       pOFStruct,
                       fuMode);
  if (hFile != INVALID_HANDLE_ERROR)
  {
    pHMHandleData->hHMHandle = hFile;

    GetFileTime(hFile,
                NULL,
                NULL,
                &filetime );
    FileTimeToDosDateTime(&filetime,
                          &filedatetime[0],
                          &filedatetime[1] );
    memcpy(pOFStruct->reserved,
           filedatetime,
           sizeof(pOFStruct->reserved) );

    return (NO_ERROR);
  }

  // error branch
  pOFStruct->nErrCode = O32_GetLastError();
  dprintf(("KERNEL32: HandleManager::Open32::OpenFile Error %08xh\n",
            pOFStruct->nErrCode));

  // return != NO_ERROR => error code
  return(hFile);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::UnlockFile
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD arg2
 *             DWORD arg3
 *             DWORD arg4
 *             DWORD arg5
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::UnlockFile(PHMHANDLEDATA pHMHandleData,
                                      DWORD         arg2,
                                      DWORD         arg3,
                                      DWORD         arg4,
                                      DWORD         arg5)
{
  dprintfl(("KERNEL32: HandleManager::Open32::UnlockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           arg2,
           arg3,
           arg4,
           arg5));

  return O32_UnlockFile(pHMHandleData->hHMHandle,
                        arg2,
                        arg3,
                        arg4,
                        arg5);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::UnlockFileEx
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwFlags
 *             DWORD dwReserved
 *             DWORD nNumberOfBytesToLockLow
 *             DWORD nNumberOfBytesToLockHigh
 *             LPOVERLAPPED lpOverlapped
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::UnlockFileEx(PHMHANDLEDATA pHMHandleData,
                                        DWORD         dwFlags,
                                        DWORD         dwReserved,
                                        DWORD         nNumberOfBytesToLockLow,
                                        DWORD         nNumberOfBytesToLockHigh,
                                        LPOVERLAPPED  lpOverlapped)
{

  dprintfl(("KERNEL32: HandleManager::Open32::UnlockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwFlags,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));

  return(O32_UnlockFile(pHMHandleData->hHMHandle,
                   lpOverlapped->Offset,
                   lpOverlapped->OffsetHigh,
                   nNumberOfBytesToLockLow,
                   nNumberOfBytesToLockHigh));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::WaitForSingleObject
 * Purpose   : object synchronization
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwTimeout
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::WaitForSingleObject(PHMHANDLEDATA pHMHandleData,
                                               DWORD         dwTimeout)
{
 DWORD rc, starttime, endtime;

  dprintfl(("KERNEL32: HandleManager::Open32::WaitForSingleObject(%08xh,%08h)\n",
            pHMHandleData->hHMHandle,
            dwTimeout));
 
  if(dwTimeout) {
  	starttime = O32_GetCurrentTime();
  }
  rc =  (O32_WaitForSingleObject(pHMHandleData->hHMHandle,
                                 dwTimeout));
  if(dwTimeout) {
  	endtime = O32_GetCurrentTime();
  	dprintf(("KERNEL32: HandleManager::WaitForSingleObject %x delta = %x (rc=%x)", pHMHandleData->hHMHandle, endtime - starttime, rc));
  }
  else dprintf(("KERNEL32: HandleManager::WaitForSingleObject %x rc=%x", pHMHandleData->hHMHandle, rc));
  return rc;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::WaitForSingleObjectEx
 * Purpose   : object synchronization
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwTimeout
 *             BOOL  fAlertable
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                                 DWORD         dwTimeout,
                                                 BOOL          fAlertable)
{
  dprintfl(("KERNEL32: HandleManager::Open32::WaitForSingleObjectEx(%08xh,%08h,%08xh) not implemented correctly.\n",
            pHMHandleData->hHMHandle,
            dwTimeout,
            fAlertable));

  //@@@PH: WARNING mapped to WaitForSingleObject simply. fAlertable missing!
  return (O32_WaitForSingleObject(pHMHandleData->hHMHandle,
                                  dwTimeout));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::FlushFileBuffers
 * Purpose   : flush the buffers of a file
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceOpen32Class::FlushFileBuffers(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HandleManager::Open32:FlushFileBuffers(%08xh)\n",
            pHMHandleData->hHMHandle));

  return(O32_FlushFileBuffers(pHMHandleData->hHMHandle));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::GetOverlappedResult
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
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceOpen32Class::GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                              LPOVERLAPPED  arg2,
                                              LPDWORD       arg3,
                                              BOOL          arg4)
{
  dprintfl(("KERNEL32: HandleManager::Open32::GetOverlappedResult(%08xh,%08xh,%08xh,%08xh)\n",
            pHMHandleData->hHMHandle,
            arg2,
            arg3,
            arg4));

  return(O32_GetOverlappedResult(pHMHandleData->hHMHandle,
                                 arg2,
                                 arg3,
                                 arg4));
}

