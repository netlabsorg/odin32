/* $Id: hmfile.cpp,v 1.42 2003-04-02 12:58:29 sandervl Exp $ */

/*
 * File IO win32 apis
 *
 * Copyright 1999-2000 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
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
#include "HMFile.h"
#include "mmap.h"
#include "oslibdos.h"

#define DBG_LOCALLOG    DBG_hmfile
#include "dbglocal.h"

/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::CreateFile
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

DWORD HMDeviceFileClass::CreateFile (LPCSTR        lpFileName,
                                     PHMHANDLEDATA pHMHandleData,
                                     PVOID         lpSecurityAttributes,
                                     PHMHANDLEDATA pHMHandleDataTemplate)
{
  HFILE hFile;
  HFILE hTemplate;
  char  filepath[260];

  dprintfl(("KERNEL32: HMDeviceFileClass::CreateFile %s(%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData,
           lpSecurityAttributes,
           pHMHandleDataTemplate));
  
  ParsePath(lpFileName, filepath, sizeof(filepath));

  //convert to long file name if in 8.3 hashed format
  GetLongPathNameA(filepath, filepath, sizeof(filepath));
  lpFileName = filepath;

  // create from template
  if (pHMHandleDataTemplate != NULL)
     hTemplate = pHMHandleDataTemplate->hHMHandle;
  else
     hTemplate = 0;

  //TODO: FILE_SHARE_DELETE
  hFile = OSLibDosCreateFile((LPSTR)lpFileName,
                             pHMHandleData->dwAccess,
                             pHMHandleData->dwShare,
                             (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                             pHMHandleData->dwCreation,
                             pHMHandleData->dwFlags,
                             hTemplate);

  if (hFile != INVALID_HANDLE_ERROR)
  {
        pHMHandleData->dwUserData = (DWORD) new HMFileInfo((LPSTR)lpFileName, lpSecurityAttributes);
        pHMHandleData->hHMHandle  = hFile;
        return (NO_ERROR);
  }
  else {
        dprintf(("CreateFile failed; error %d", GetLastError()));
        return(GetLastError());
  }
}

//*****************************************************************************
//Parses and copies path
//OpenFile in NT4, SP6 accepts double (or more) backslashes as separators for directories!
//(OS/2 doesn't)
//Girotel 2.0 (Dutch banking app) seems to depend on this behaviour
//*****************************************************************************
void HMDeviceFileClass::ParsePath(LPCSTR lpszFileName, LPSTR lpszParsedFileName, DWORD length)
{
    int i=0;

    while(*lpszFileName != 0 && i < length-1) {
        *lpszParsedFileName++ = *lpszFileName;
        if(*lpszFileName == '\\') {
            while(*lpszFileName == '\\') {
                 lpszFileName++;
            }
        }
        else {
            lpszFileName++;
        }
        i++;
    }
    *lpszParsedFileName = 0;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::OpenFile
 * Purpose   : this is called from the handle manager if a OpenFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    : TODO: Check if this implementation is complete and 100% correct
 *		             UTC Time or Localtime ?
 *                   GetFileTime is changed, Returns UTC-time yet !!!!! 
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceFileClass::OpenFile (LPCSTR        lpszFileName,
                                   PHMHANDLEDATA pHMHandleData,
                                   OFSTRUCT      *pOFStruct,
                                   UINT          fuMode)
{
    HFILE hFile;
    FILETIME filetime;
    WORD filedatetime[2];
    char filepath[260];
    LPSTR lpFileName = (LPSTR)lpszFileName;

    SetLastError(ERROR_SUCCESS);

    //Re-open using name in OFSTRUCT
    if(fuMode & OF_REOPEN)
         lpFileName = (LPSTR)pOFStruct->szPathName;
    else memset(pOFStruct, 0, sizeof(OFSTRUCT));

    dprintf(("KERNEL32: HMDeviceFileClass::OpenFile %s(%s,%08x,%08x,%08x)", lpHMDeviceName,
             lpFileName, pHMHandleData, pOFStruct, fuMode));

    if(lpFileName == NULL) {
        dprintf(("Invalid name (NULL)!"));
        SetLastError(ERROR_INVALID_NAME);
        return HFILE_ERROR;
    }

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
           && !(fuMode & OF_CREATE) )
        {
            pOFStruct->nErrCode = ERROR_FILE_NOT_FOUND;
            SetLastError(ERROR_FILE_NOT_FOUND);
            return HFILE_ERROR;
        }
        lpFileName = filepath;
    }
    else {
        ParsePath(lpFileName, filepath, sizeof(filepath));

        //convert to long file name if in 8.3 hashed format
        GetLongPathNameA(filepath, filepath, sizeof(filepath));

        lpFileName = filepath;
    }

    // filling OFSTRUCT
    pOFStruct->cBytes = sizeof(OFSTRUCT);
    pOFStruct->nErrCode = 0;
    strncpy((char *)pOFStruct->szPathName, lpFileName, OFS_MAXPATHNAME);
    pOFStruct->szPathName[OFS_MAXPATHNAME-1] = 0;

    hFile = OSLibDosOpenFile((LPSTR)lpFileName, fuMode);

    if(hFile != INVALID_HANDLE_ERROR)
    {
        //Needed for GetFileTime
        pHMHandleData->hHMHandle = hFile;
        GetFileTime(pHMHandleData,
                    NULL,
                    NULL,
                    &filetime );

		/* UTC Time or Localtime ? GetFileTime Returns UTC-time yet ? !!!!! */ 
        FileTimeToDosDateTime(&filetime,
                              &filedatetime[0],
                              &filedatetime[1] );
        memcpy(pOFStruct->reserved, filedatetime, sizeof(pOFStruct->reserved));

        if(fuMode & OF_DELETE)
        {
            OSLibDosClose(hFile);
            OSLibDosDelete((LPSTR)lpFileName);
        }
        else
        if(fuMode & OF_EXIST)
        {
            OSLibDosClose(hFile);
            hFile = HFILE_ERROR;
        }
        if(fuMode & OF_PARSE)
        {
            CHAR drive[4];

            drive[0] = pOFStruct->szPathName[0];
            drive[1] = pOFStruct->szPathName[1];
            drive[2] = pOFStruct->szPathName[2];
            drive[3] = 0;

            pOFStruct->fFixedDisk = (GetDriveTypeA(drive) != DRIVE_REMOVABLE);

            OSLibDosClose(hFile);
            hFile = HFILE_ERROR;
        }

        if((fuMode & OF_VERIFY))
        {
            if(memcmp(pOFStruct->reserved, filedatetime, sizeof(pOFStruct->reserved)))
            {
                OSLibDosClose(hFile);
                SetLastError(ERROR_FILE_NOT_FOUND);
            }
            hFile = HFILE_ERROR;
        }

        pOFStruct->nErrCode = GetLastError();
        pHMHandleData->hHMHandle = hFile;

        if(hFile != HFILE_ERROR) {
            pHMHandleData->dwUserData = (DWORD) new HMFileInfo((LPSTR)lpFileName, NULL);
        }
        return (NO_ERROR);
    }
    else {
        DWORD rc = GetLastError();

        if(fuMode & OF_EXIST)
        {
            if(rc == ERROR_OPEN_FAILED) {
                SetLastError(ERROR_FILE_NOT_FOUND);
            }
        }
        //todo: OF_PROMPT handling (pop up message box)
    }
    // error branch
    pOFStruct->nErrCode = GetLastError();
    dprintf(("KERNEL32: HMDeviceFileClass::OpenFile Error %08xh\n",
             pOFStruct->nErrCode));

    // return != NO_ERROR => error code
    return(hFile);
}

/*****************************************************************************
 * Name      : HMDeviceFileClass::DuplicateHandle
 * Purpose   :
 * Parameters:
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : DUPLICATE_CLOSE_SOURCE flag handled in HMDuplicateHandle
 *
 * Status    : partially implemented
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceFileClass::DuplicateHandle(HANDLE srchandle, PHMHANDLEDATA pHMHandleData,
                                        HANDLE  srcprocess,
                                        PHMHANDLEDATA pHMSrcHandle,
                                        HANDLE  destprocess,
                                        PHANDLE desthandle,
                                        DWORD   fdwAccess,
                                        BOOL    fInherit,
                                        DWORD   fdwOptions,
                                        DWORD   fdwOdinOptions)
{
 HMFileInfo *srcfileinfo = (HMFileInfo *)pHMSrcHandle->dwUserData;
 DWORD rc;

  dprintf(("KERNEL32:HMDeviceFileClass::DuplicateHandle (%08x,%08x,%08x,%08x,%08x)",
           pHMHandleData,
           srcprocess,
           pHMSrcHandle->hHMHandle,
           destprocess,
           desthandle));

  //TODO: Inheritance of file handles won't work!

  if(destprocess != srcprocess)
  {
    //TODO:!!!!
    dprintf(("ERROR: DuplicateHandle; different processes not yet supported!!"));
    return FALSE;
  }

  if(srcfileinfo)
  {
    //SvL: Special duplicatehandle option used by memory mapped class to duplicate
    //     file handle
    //     Can't use DosDupHandle or else there can be a sharing violation
    //     when an app tries to access the same file again
    if(fdwOdinOptions)
    {
        HMHANDLEDATA duphdata;

        memcpy(&duphdata, pHMHandleData, sizeof(duphdata));
        duphdata.dwCreation = OPEN_EXISTING;

        if(CreateFile(srcfileinfo->lpszFileName, &duphdata,
                      srcfileinfo->lpSecurityAttributes, NULL) == NO_ERROR)
        {
            memcpy(pHMHandleData, &duphdata, sizeof(duphdata));

            if(fInherit) SetHandleInformation(pHMHandleData, ~HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

            SetLastError(ERROR_SUCCESS);
            return TRUE;
        }
        dprintf(("ERROR: DuplicateHandle; CreateFile %s failed -> trying DosDupHandle instead!",
                  srcfileinfo->lpszFileName));
        //SvL: IE5 setup opens file with DENYREADWRITE, so CreateFile can't
        //     be used for duplicating the handle; try DosDupHandle instead
    }

    if(!(fdwOptions & DUPLICATE_SAME_ACCESS) && fdwAccess != pHMSrcHandle->dwAccess) {
        dprintf(("WARNING: DuplicateHandle; app wants different access permission; Not supported!! (%x, %x)", fdwAccess, pHMSrcHandle->dwAccess));
    }

    rc = OSLibDosDupHandle(pHMSrcHandle->hHMHandle,
                           desthandle);
    if (rc)
    {
      dprintf(("ERROR: DuplicateHandle: OSLibDosDupHandle(%s) failed = %u",
               srcfileinfo->lpszFileName, rc));
      SetLastError(rc);
      return FALSE;   // ERROR
    }
    else {
      if(fInherit) SetHandleInformation(pHMHandleData, ~HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

      SetLastError(ERROR_SUCCESS);
      pHMHandleData->hHMHandle = *desthandle;
      return TRUE;    // OK
    }
  }
  else
  {
    dprintf(("ERROR: DuplicateHandle; invalid parameter!!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
}

/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::SetHandleInformation
 * Purpose   : The SetHandleInformation function sets certain properties of an
 *             object handle. The information is specified as a set of bit flags.
 * Parameters: HANDLE hObject  handle to an object
 *             DWORD  dwMask   specifies flags to change
 *             DWORD  dwFlags  specifies new values for flags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : 
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceFileClass::SetHandleInformation(PHMHANDLEDATA pHMHandleData,
                                             DWORD  dwMask,
                                             DWORD  dwFlags)
{
    DWORD rc;

    pHMHandleData->dwHandleInformation &= ~dwMask;
    pHMHandleData->dwHandleInformation |= (dwFlags & dwMask);

    rc = OSLibDosSetFHState(pHMHandleData->hHMHandle,
                            pHMHandleData->dwHandleInformation);
    if (rc)
    {
        dprintf(("ERROR: SetHandleInformation: OSLibDosSetFHState failed with %d", rc));

        SetLastError(rc);
        return FALSE;
    }

    SetLastError(ERROR_SUCCESS);
    return TRUE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceFileClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  HMFileInfo *fileInfo = (HMFileInfo *)pHMHandleData->dwUserData;
  BOOL        bRC;

  dprintfl(("KERNEL32: HMDeviceFileClass::CloseHandle(%08x)\n",
           pHMHandleData->hHMHandle));

  bRC = OSLibDosClose(pHMHandleData->hHMHandle);

  if(pHMHandleData->dwFlags & FILE_FLAG_DELETE_ON_CLOSE) {
    //TODO: should only do this after all handles have been closed
    if(fileInfo) {
        DeleteFileA(fileInfo->lpszFileName);
    }
  }
  if(fileInfo) {
    delete fileInfo;
  }
  dprintf(("KERNEL32: HMDeviceFileClass::CloseHandle returned %08xh\n",
           bRC));

  return (DWORD)bRC;
}


/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::ReadFile
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

BOOL HMDeviceFileClass::ReadFile(PHMHANDLEDATA pHMHandleData,
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

  dprintfl(("KERNEL32: HMDeviceFileClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
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
  if(lpCompletionRoutine) {
      dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
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
  else lpRealBuf = (LPVOID)lpBuffer;

  //If this file is also used in a memory map somewhere, then we need
  //to tell the map to flush all modified contents to disk right NOW
  map = Win32MemMap::findMapByFile(pHMHandleData->hWin32Handle);
  if(map) {
      DWORD curpos = SetFilePointer(pHMHandleData, 0, NULL, FILE_CURRENT);

      dprintf(("Flush memory maps to disk before reading!!"));
      map->flushView(MMAP_FLUSHVIEW_ALL, curpos, nNumberOfBytesToRead);
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
        dprintf(("KERNEL32: HMDeviceFileClass::ReadFile returned %08xh %x\n",
                  bRC, GetLastError()));
    dprintf(("%x -> %d", lpBuffer, IsBadWritePtr((LPVOID)lpBuffer, nNumberOfBytesToRead)));
  }

  return bRC;
}


/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::WriteFile
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

BOOL HMDeviceFileClass::WriteFile(PHMHANDLEDATA pHMHandleData,
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

  dprintfl(("KERNEL32: HMDeviceFileClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

  //SvL: It's legal for this pointer to be NULL
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
  else lpRealBuf = (LPVOID)lpBuffer;

  //If this file is also used in a memory map somewhere, then we need
  //to tell the map to flush all modified contents to disk right NOW
  DWORD curfilepos;
  map = Win32MemMap::findMapByFile(pHMHandleData->hWin32Handle);
  if(map) {
      curfilepos = SetFilePointer(pHMHandleData, 0, NULL, FILE_CURRENT);

      dprintf(("Flush memory maps to disk before writing!!"));
      map->flushView(MMAP_FLUSHVIEW_ALL, curfilepos, nNumberOfBytesToWrite);
  }

  if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) {
    dprintf(("ERROR: Overlapped IO not yet implememented!!"));
  }
//  else {
    bRC = OSLibDosWrite(pHMHandleData->hHMHandle,
                            (PVOID)lpRealBuf,
                            nNumberOfBytesToWrite,
                            lpNumberOfBytesWritten);
//  }

  if(map && bRC) {
      dprintf(("Invalidate memory map after file writing!!"));
      map->invalidatePages(curfilepos, *lpNumberOfBytesWritten);
  }

  dprintf(("KERNEL32: HMDeviceFileClass::WriteFile returned %08xh\n",
           bRC));

  return bRC;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceFileClass::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HMDeviceFileClass::GetFileType %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData));

  return FILE_TYPE_DISK;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::GetFileInformationByHandle
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

DWORD HMDeviceFileClass::GetFileInformationByHandle(PHMHANDLEDATA               pHMHandleData,
                                                    BY_HANDLE_FILE_INFORMATION* pHFI)
{
    dprintfl(("KERNEL32: HMDeviceFileClass::GetFileInformationByHandle %s(%08xh,%08xh)\n",
              lpHMDeviceName, pHMHandleData, pHFI));

    if(OSLibDosGetFileInformationByHandle(pHMHandleData->hHMHandle,
                                          pHFI))
    {
        return TRUE;
    }
    dprintf(("GetFileInformationByHandle failed with error %d", GetLastError()));
    return FALSE;

}


/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::SetEndOfFile
 * Purpose   : set end of file marker
 * Parameters: PHMHANDLEDATA              pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceFileClass::SetEndOfFile(PHMHANDLEDATA pHMHandleData)
{
    dprintfl(("KERNEL32: HMDeviceFileClass::SetEndOfFile %s(%08xh)\n",
             lpHMDeviceName,
             pHMHandleData));

    if(OSLibDosSetEndOfFile(pHMHandleData->hHMHandle)) {
        return TRUE;
    }
    dprintf(("SetEndOfFile failed with error %d", GetLastError()));
    return FALSE;
}

//******************************************************************************
/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::SetFileTime
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
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44] mod. DT
 *****************************************************************************/

BOOL HMDeviceFileClass::SetFileTime(PHMHANDLEDATA pHMHandleData,
                                    LPFILETIME pFT1,
                                    LPFILETIME pFT2,
                                    LPFILETIME pFT3)
{
    dprintfl(("KERNEL32: HMDeviceFileClass::SetFileTime %s(%08xh,%08xh,%08xh,%08xh)\n",
              lpHMDeviceName, pHMHandleData, pFT1, pFT2, pFT3));

    if(OSLibDosSetFileTime(pHMHandleData->hHMHandle, pFT1, pFT2, pFT3)) return TRUE;

    dprintf(("SetFileTime failed with error %d", GetLastError()));
    return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::GetFileTime
 * Purpose   : get file time
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PFILETIME     pFT1
 *             PFILETIME     pFT2
 *             PFILETIME     pFT3
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL mod. DT
 *****************************************************************************/

BOOL HMDeviceFileClass::GetFileTime(PHMHANDLEDATA pHMHandleData,
                                    LPFILETIME pFT1,
                                    LPFILETIME pFT2,
                                    LPFILETIME pFT3)
{
  if(!pFT1 && !pFT2 && !pFT3) {//TODO: does NT do this?
    dprintf(("ERROR: GetFileTime: invalid parameter!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  if(OSLibDosGetFileTime(pHMHandleData->hHMHandle, pFT1, pFT2, pFT3)) return TRUE;
  dprintf(("GetFileTime failed with error %d", GetLastError()));
  return FALSE;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::GetFileSize
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

DWORD HMDeviceFileClass::GetFileSize(PHMHANDLEDATA pHMHandleData,
                                     PDWORD        lpdwFileSizeHigh)
{
  dprintfl(("KERNEL32: HMDeviceFileClass::GetFileSize %s(%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           lpdwFileSizeHigh));

  if(lpdwFileSizeHigh)
    *lpdwFileSizeHigh = 0;

  return OSLibDosGetFileSize(pHMHandleData->hHMHandle, lpdwFileSizeHigh);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::SetFilePointer
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

DWORD HMDeviceFileClass::SetFilePointer(PHMHANDLEDATA pHMHandleData,
                                        LONG          lDistanceToMove,
                                        PLONG         lpDistanceToMoveHigh,
                                        DWORD         dwMoveMethod)
{
  DWORD ret;

  dprintfl(("KERNEL32: HMDeviceFileClass::SetFilePointer %s(%08xh,%08xh,%08xh,%08xh)\n",
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


/*****************************************************************************
 * Name      : BOOL HMDeviceFileClass::LockFile
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

BOOL HMDeviceFileClass::LockFile(PHMHANDLEDATA pHMHandleData,
                                  DWORD         dwFileOffsetLow,
                                  DWORD         dwFileOffsetHigh,
                                  DWORD         cbLockLow,
                                  DWORD         cbLockHigh)
{
  dprintfl(("KERNEL32: HMDeviceFileClass::LockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwFileOffsetLow,
           dwFileOffsetHigh,
           cbLockLow,
           cbLockHigh));

  return OSLibDosLockFile(pHMHandleData->hHMHandle,
                          LOCKFILE_EXCLUSIVE_LOCK,
                          dwFileOffsetLow,
                          dwFileOffsetHigh,
                          cbLockLow,
                          cbLockHigh,
                          NULL);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::LockFileEx
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

BOOL HMDeviceFileClass::LockFileEx(PHMHANDLEDATA pHMHandleData,
                                    DWORD         dwFlags,
                                    DWORD         dwReserved,
                                    DWORD         nNumberOfBytesToLockLow,
                                    DWORD         nNumberOfBytesToLockHigh,
                                    LPOVERLAPPED  lpOverlapped)
{

  dprintfl(("KERNEL32: HMDeviceFileClass::LockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not completely implemented!",
           lpHMDeviceName,
           pHMHandleData,
           dwFlags,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));


  return(OSLibDosLockFile(pHMHandleData->hHMHandle,
                          dwFlags,
                          lpOverlapped->Offset,
                          lpOverlapped->OffsetHigh,
                          nNumberOfBytesToLockLow,
                          nNumberOfBytesToLockHigh,
                          lpOverlapped));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::UnlockFile
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

BOOL HMDeviceFileClass::UnlockFile(PHMHANDLEDATA pHMHandleData,
                                    DWORD         dwFileOffsetLow,
                                    DWORD         dwFileOffsetHigh,
                                    DWORD         cbLockLow,
                                    DWORD         cbLockHigh)
{
  dprintfl(("KERNEL32: HMDeviceFileClass::UnlockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwFileOffsetLow,
           dwFileOffsetHigh,
           cbLockLow,
           cbLockHigh));

  return OSLibDosUnlockFile(pHMHandleData->hHMHandle,
                            dwFileOffsetLow,
                            dwFileOffsetHigh,
                            cbLockLow,
                            cbLockHigh,
                            NULL);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::UnlockFileEx
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

BOOL HMDeviceFileClass::UnlockFileEx(PHMHANDLEDATA pHMHandleData,
                                     DWORD         dwReserved,
                                     DWORD         nNumberOfBytesToLockLow,
                                     DWORD         nNumberOfBytesToLockHigh,
                                     LPOVERLAPPED  lpOverlapped)
{

  dprintfl(("KERNEL32: HMDeviceFileClass::UnlockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh) not completely implemented!",
           lpHMDeviceName,
           pHMHandleData,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));


  return(OSLibDosUnlockFile(pHMHandleData->hHMHandle,
                            lpOverlapped->Offset,
                            lpOverlapped->OffsetHigh,
                            nNumberOfBytesToLockLow,
                            nNumberOfBytesToLockHigh,
                            lpOverlapped));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::FlushFileBuffers
 * Purpose   : flush the buffers of a file
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceFileClass::FlushFileBuffers(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HMDeviceFileClass:FlushFileBuffers(%08xh)\n",
            pHMHandleData->hHMHandle));

  return(OSLibDosFlushFileBuffers(pHMHandleData->hHMHandle));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::GetOverlappedResult
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

BOOL HMDeviceFileClass::GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                              LPOVERLAPPED  arg2,
                                              LPDWORD       arg3,
                                              BOOL          arg4)
{
  dprintfl(("KERNEL32-WARNING: HMDeviceFileClass::GetOverlappedResult(%08xh,%08xh,%08xh,%08xh) STUB!!",
            pHMHandleData->hHMHandle,
            arg2,
            arg3,
            arg4));

  return FALSE;
//  return(O32_GetOverlappedResult(pHMHandleData->hHMHandle,
//                                 arg2,
//                                 arg3,
//                                 arg4));
}
//******************************************************************************
//******************************************************************************
HMFileInfo::HMFileInfo(LPSTR lpszFileName, PVOID lpSecurityAttributes)
{
  this->lpszFileName = (LPSTR)malloc(strlen(lpszFileName)+1);
  if(!this->lpszFileName) {
    DebugInt3();
  }
  strcpy(this->lpszFileName, lpszFileName);
  this->lpSecurityAttributes = lpSecurityAttributes;
}
//******************************************************************************
//******************************************************************************
HMFileInfo::~HMFileInfo()
{
  if(lpszFileName) {
    free(lpszFileName);
    lpszFileName = NULL;
  }
}
//******************************************************************************
//******************************************************************************
