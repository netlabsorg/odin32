/* $Id: oslibdos.cpp,v 1.27 2000-05-22 19:07:58 sandervl Exp $ */
/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999-2000 Edgar Buerkle <Edgar.Buerkle@gmx.net>
 * Copyright 2000 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_NPIPES
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <win32api.h>
#include <winconst.h>
#include <misc.h>
#include "initterm.h"
#include "oslibdos.h"
#include "dosqss.h"

#define DBG_LOCALLOG    DBG_oslibdos
#include "dbglocal.h"

//******************************************************************************
// translate OS/2 error codes to Windows codes
// NOTE: add all codes you need, list is not complete!
//******************************************************************************
DWORD error2WinError(APIRET rc,DWORD defaultCode = ERROR_NOT_ENOUGH_MEMORY_W)
{
  switch (rc)
  {
    case NO_ERROR: //0
      return ERROR_SUCCESS_W;

    case ERROR_FILE_NOT_FOUND: //2
      return ERROR_FILE_NOT_FOUND_W;

    case ERROR_PATH_NOT_FOUND: //3
      return ERROR_PATH_NOT_FOUND_W;

    case ERROR_ACCESS_DENIED: //5
      return ERROR_ACCESS_DENIED_W;

    case ERROR_INVALID_HANDLE: //6
      return ERROR_INVALID_HANDLE_W;

    case ERROR_NOT_ENOUGH_MEMORY: //8
      return ERROR_NOT_ENOUGH_MEMORY_W;

    case ERROR_BAD_FORMAT: //11
      return ERROR_BAD_FORMAT_W;

    case ERROR_NO_MORE_FILES: //18
      return ERROR_NO_MORE_FILES_W;

    case ERROR_NOT_DOS_DISK: //26
      return ERROR_NOT_DOS_DISK_W;

    case ERROR_OUT_OF_STRUCTURES: //84
      return ERROR_OUT_OF_STRUCTURES_W;

    case ERROR_INVALID_PARAMETER: //87
      return ERROR_INVALID_PARAMETER_W;

    case ERROR_INTERRUPT: //95
      return ERROR_INVALID_AT_INTERRUPT_TIME_W; //CB: right???

    case ERROR_DRIVE_LOCKED: //108
      return ERROR_DRIVE_LOCKED_W;

    case ERROR_BROKEN_PIPE: //109
      return ERROR_BROKEN_PIPE_W;

    case ERROR_BUFFER_OVERFLOW: //111
      return ERROR_BUFFER_OVERFLOW_W;

    case ERROR_NO_MORE_SEARCH_HANDLES: //113
      return ERROR_NO_MORE_SEARCH_HANDLES_W;

    case ERROR_SEM_TIMEOUT: //121
      return ERROR_SEM_TIMEOUT_W;

    case ERROR_DISCARDED: //157
      return ERROR_DISCARDED_W;

    case ERROR_FILENAME_EXCED_RANGE: //206
      return ERROR_FILENAME_EXCED_RANGE_W;

    case ERROR_META_EXPANSION_TOO_LONG: //208
      return ERROR_META_EXPANSION_TOO_LONG_W;

    case ERROR_BAD_PIPE: //230
      return ERROR_BAD_PIPE_W;

    case ERROR_PIPE_BUSY: //231
      return ERROR_PIPE_BUSY_W;

    case ERROR_PIPE_NOT_CONNECTED: //233
      return ERROR_PIPE_NOT_CONNECTED_W;

    case ERROR_MORE_DATA: //234
      return ERROR_MORE_DATA_W;

    case ERROR_INVALID_EA_NAME: //254
      return ERROR_INVALID_EA_NAME_W;

    case ERROR_EA_LIST_INCONSISTENT: //255
      return ERROR_EA_LIST_INCONSISTENT_W;

    case ERROR_EAS_DIDNT_FIT: //275
      return ERROR_EAS_DIDNT_FIT;

    default:
      return defaultCode;
  }
}
//******************************************************************************
//TODO: Assumes entire memory range has the same protection flags!
//TODO: Check if this works for code aliases...
//******************************************************************************
DWORD OSLibDosAliasMem(LPVOID pb, ULONG cb, LPVOID *ppbAlias, ULONG fl)
{
 DWORD rc;
 DWORD attr;
 DWORD size = cb;

  cb = (cb-1) & ~0xfff;
  cb+= PAGE_SIZE;

  rc = DosQueryMem(pb, &size, &attr);
  if(rc) {
        dprintf(("OSLibDosAliasMem: DosQueryMem %x %x return %d", pb, size, rc));
        return rc;
  }
  size = (size-1) & ~0xfff;
  size+= PAGE_SIZE;
  if(size != cb) {
        dprintf(("ERROR: OSLibDosAliasMem: size != cb (%x!=%x)!!!!!!!!", size, cb));
        //ignore this and continue return 5;
        attr = fl; //just use original protection flags (NOT CORRECT)
  }
  attr &= (PAG_READ|PAG_WRITE|PAG_EXECUTE|PAG_GUARD|PAG_DEFAULT);
  if(attr != fl) {
        rc = DosSetMem(pb, size, fl);
        if(rc) {
                dprintf(("OSLibDosAliasMem: DosSetMem %x %x return %d", pb, size, rc));
                attr = fl;
                //just continue for now
                //return rc;
        }
  }
  rc = DosAliasMem(pb, cb, ppbAlias, 2);
  if(rc) {
        dprintf(("OSLibDosAliasMem: DosAliasMem %x %x returned %d", pb, cb, rc));
        return rc;
  }
  if(attr != fl) {
        rc = DosSetMem(pb, size, attr);
        if(rc) {
                dprintf(("OSLibDosAliasMem: DosSetMem (2) %x %x return %d", pb, size, rc));
                return rc;
        }
  }
  return 0;
}
//******************************************************************************
//NT returns addresses aligned at 64k, so we do too.
//******************************************************************************
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags)
{
 LPVOID memaddr;
 DWORD  offset;
 APIRET rc;

  rc = DosAllocMem(&memaddr, size, PAG_READ | flAllocMem);
  if(rc) {
        return rc;
  }
  DosEnterCritSec();
  DosFreeMem(memaddr);
  offset = (DWORD)memaddr & 0xFFFF;
  if(offset) {
        DosAllocMem(&memaddr, 64*1024 - offset, PAG_READ | flAllocMem);
  }
  rc = DosAllocMem(lplpMemAddr, size, flags | flAllocMem);
  DosExitCritSec();
  if((DWORD)*lplpMemAddr & 0xFFFF) {//still not at 64k boundary?
        DosFreeMem(*lplpMemAddr);
        rc = OSLibDosAllocMem(lplpMemAddr, size, flags);
  }
  if(offset) {
        DosFreeMem(memaddr);
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosFreeMem(LPVOID lpMemAddr)
{
  return DosFreeMem(lpMemAddr);
}
//******************************************************************************
//NOTE: If name == NULL, allocated gettable unnamed shared memory
//******************************************************************************
DWORD OSLibDosAllocSharedMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags, LPSTR name)
{
 APIRET rc;
 char  *sharedmemname = NULL;

  if(name) {
        sharedmemname = (char *)malloc(strlen(name) + 16);
        strcpy(sharedmemname, "\\SHAREMEM\\");
        strcat(sharedmemname, name);
  }
  else  flags |= OBJ_GETTABLE;

  rc = DosAllocSharedMem(lplpMemAddr, sharedmemname, size, flags);
  if(name) {
        free(sharedmemname);
  }
  return rc;
}
//******************************************************************************
//NOTE: If name == NULL, assume gettable unnamed shared memory
//******************************************************************************
DWORD OSLibDosGetNamedSharedMem(LPVOID *lplpMemAddr, LPSTR name)
{
 APIRET rc;
 char  *sharedmemname = NULL;

  if(name) {
        sharedmemname = (char *)malloc(strlen(name) + 16);
        strcpy(sharedmemname, "\\SHAREMEM\\");
        strcat(sharedmemname, name);
        rc = DosGetNamedSharedMem(lplpMemAddr, sharedmemname, PAG_READ|PAG_WRITE);
        if(name) {
                free(sharedmemname);
        }
  }
  else  rc = DosGetSharedMem((LPVOID)*(DWORD *)lplpMemAddr, PAG_READ|PAG_WRITE);

  return rc;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosQueryMem(LPVOID lpMemAddr, DWORD *lpRangeSize, DWORD *lpAttr)
{
  return DosQueryMem(lpMemAddr, lpRangeSize, lpAttr);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetMem(LPVOID lpMemAddr, DWORD size, DWORD flags)
{
 APIRET rc;

  rc = DosSetMem(lpMemAddr, size, flags);
  switch(rc) {
        case ERROR_INVALID_ADDRESS:
                return OSLIB_ERROR_INVALID_ADDRESS;
        case ERROR_ACCESS_DENIED:
                return OSLIB_ERROR_ACCESS_DENIED;
        default:
                return rc;
  }
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosOpen(char *lpszFileName, DWORD flags)
{
 APIRET rc;
 HFILE  hFile;
 ULONG  ulAction;
 DWORD  os2flags = OPEN_FLAGS_NOINHERIT;


  if(flags & OSLIB_ACCESS_READONLY)
        os2flags |= OPEN_ACCESS_READONLY;
  else
  if(flags & OSLIB_ACCESS_READWRITE)
        os2flags |= OPEN_ACCESS_READWRITE;

  if(flags & OSLIB_ACCESS_SHAREDENYNONE)
        os2flags |= OPEN_SHARE_DENYNONE;
  else
  if(flags & OSLIB_ACCESS_SHAREDENYREAD)
        os2flags |= OPEN_SHARE_DENYREAD;
  else
  if(flags & OSLIB_ACCESS_SHAREDENYWRITE)
        os2flags |= OPEN_SHARE_DENYWRITE;

tryopen:
  rc = DosOpen(lpszFileName,                     /* File path name */
               &hFile,                   /* File handle */
               &ulAction,                      /* Action taken */
               0L,                             /* File primary allocation */
               0L,                     /* File attribute */
               OPEN_ACTION_FAIL_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
               os2flags,
               0L);                            /* No extended attribute */

  if(rc) {
        if(rc == ERROR_TOO_MANY_OPEN_FILES) {
         ULONG CurMaxFH;
         LONG  ReqCount = 32;

                rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
                if(rc) {
                        dprintf(("DosSetRelMaxFH returned %d", rc));
                        return 0;
                }
                dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
                goto tryopen;
        }
        return 0;
  }
  else  return hFile;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosClose(DWORD hFile)
{
  return DosClose(hFile);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosChangeMaxFileHandles()
{
 ULONG  CurMaxFH;
 LONG   ReqCount = 0;
 APIRET rc;

        rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
        if(rc) {
                dprintf(("DosSetRelMaxFH returned %d", rc));
                return rc;
        }
        if(ReqCount + ODIN_INCREMENT_MAX_FILEHANDLES > CurMaxFH) {
                ReqCount = CurMaxFH + ODIN_INCREMENT_MAX_FILEHANDLES;
                rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
                if(rc) {
                        dprintf(("DosSetRelMaxFH returned %d", rc));
                        return rc;
                }
        }
        return 0;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetInitialMaxFileHandles(DWORD maxhandles)
{
 ULONG  CurMaxFH;
 LONG   ReqCount = 0;
 APIRET rc;

        rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
        if(rc) {
                dprintf(("DosSetRelMaxFH returned %d", rc));
                return rc;
        }
        if(CurMaxFH < maxhandles) {
                rc = DosSetMaxFH(maxhandles);
                if(rc) {
                        dprintf(("DosSetMaxFH returned %d", rc));
                        return rc;
                }
        }
        return 0;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosRead(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesRead)
{
  return DosRead(hFile, lpBuffer, size, nrBytesRead);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosWrite(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesWritten)
{
  return DosWrite(hFile, lpBuffer, size, nrBytesWritten);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetFilePtr(DWORD hFile, DWORD offset, DWORD method)
{
 DWORD  os2method;
 DWORD  newoffset;
 APIRET rc;

  switch(method) {
  case OSLIB_SETPTR_FILE_CURRENT:
        os2method = FILE_CURRENT;
        break;
  case OSLIB_SETPTR_FILE_BEGIN:
        os2method = FILE_BEGIN  ;
        break;
  case OSLIB_SETPTR_FILE_END:
        os2method = FILE_END;
        break;
  default:
        return OSLIB_ERROR_INVALID_PARAMETER;
  }
  rc = DosSetFilePtr(hFile, offset, os2method, &newoffset);
  if(rc) {
        return -1;
  }
  else  return newoffset;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosDelete(char *lpszFileName)
{
  return DosDelete(lpszFileName);
}
//******************************************************************************
//******************************************************************************
BOOL pmDateTimeToFileTime(FDATE *pDate,FTIME *pTime,FILETIME *pFT)
{
  USHORT dosTime, dosDate;

  dosTime = *(USHORT*)pTime;
  dosDate = *(USHORT*)pDate;

  return DosDateTimeToFileTime(dosDate,dosTime,pFT);
}
//******************************************************************************
//******************************************************************************
#define NOT_NORMAL (FILE_ATTRIBUTE_READONLY_W | \
                    FILE_ATTRIBUTE_HIDDEN_W |   \
                    FILE_ATTRIBUTE_SYSTEM_W |   \
                    FILE_ATTRIBUTE_ARCHIVE_W)

inline DWORD pm2WinFileAttributes(DWORD attrFile)
{
  DWORD res = 0;

  if (!(attrFile & NOT_NORMAL))
    res |= FILE_ATTRIBUTE_NORMAL_W;
  if (attrFile & FILE_READONLY)
    res |= FILE_ATTRIBUTE_READONLY_W;
  if (attrFile & FILE_HIDDEN)
    res |= FILE_ATTRIBUTE_HIDDEN_W;
  if (attrFile & FILE_SYSTEM)
    res |= FILE_ATTRIBUTE_SYSTEM_W;
  if (attrFile & FILE_DIRECTORY)
    res |= FILE_ATTRIBUTE_DIRECTORY_W;
  if (attrFile & FILE_ARCHIVED)
    res |= FILE_ATTRIBUTE_ARCHIVE_W;

  //CB: not used: FILE_ATTRIBUTE_COMPRESSED_W

  return res;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosGetFileAttributesEx(PSZ   pszName,
                                 ULONG ulDummy,
                                 PVOID pBuffer)
{
  APIRET      rc;                                         /* API return code */
  FILESTATUS3 fs3;                             /* file information structure */
  LPWIN32_FILE_ATTRIBUTE_DATA lpFad = (LPWIN32_FILE_ATTRIBUTE_DATA) pBuffer;

  // Note: we only handle standard "GetFileExInfoStandard" requests
  rc = DosQueryPathInfo(pszName,               /* query the file information */
                        FIL_STANDARD,
                        &fs3,
                        sizeof(fs3));
  if (rc != NO_ERROR)                                    /* check for errors */
    return FALSE;                                   /* raise error condition */

  // convert structure
  lpFad->dwFileAttributes = pm2WinFileAttributes(fs3.attrFile);
  pmDateTimeToFileTime(&fs3.fdateCreation,   &fs3.ftimeCreation,   &lpFad->ftCreationTime);
  pmDateTimeToFileTime(&fs3.fdateLastAccess, &fs3.ftimeLastAccess, &lpFad->ftLastAccessTime);
  pmDateTimeToFileTime(&fs3.fdateLastWrite,  &fs3.ftimeLastWrite,  &lpFad->ftLastWriteTime);

  /* @@@PH we might add Aurora support ...
  lpFad->nFileSizeHigh    = info.nFileSizeHigh;
  */
  lpFad->nFileSizeHigh    = 0;
  lpFad->nFileSizeLow     = fs3.cbFile;

  return TRUE;
}
//******************************************************************************
DWORD WIN32API GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD );
//******************************************************************************
DWORD OSLibDosSearchPath(DWORD cmd, char *path, char *name, char *full_name,
                         DWORD length_fullname)
{
  switch(cmd) {
  case OSLIB_SEARCHDIR:
        if(DosSearchPath(SEARCH_IGNORENETERRS, path,
                         name, full_name, length_fullname) != 0) {
                return 0;
        }
        return strlen(full_name);


  case OSLIB_SEARCHCURDIR:
        if(DosSearchPath(SEARCH_IGNORENETERRS | SEARCH_CUR_DIRECTORY, path,
                         name, full_name, length_fullname) != 0) {
                return 0;
        }
        return strlen(full_name);

  case OSLIB_SEARCHFILE:
  {
    FILESTATUS3 fileinfo;

        if(DosQueryPathInfo(name, FIL_STANDARD, &fileinfo, sizeof(fileinfo)) != 0) {
                return 0;
        }
        strncpy(full_name, name, length_fullname);
        return strlen(full_name);
  }

  case OSLIB_SEARCHENV:
  {
   LPSTR envstring;
   int   envsize;
   CHAR  szResult[CCHMAXPATH];

        envsize = GetEnvironmentVariableA(path, NULL, 0);
        envstring = (LPSTR)malloc(envsize+1);
        GetEnvironmentVariableA(path, envstring, envsize);
        if(DosSearchPath(SEARCH_IGNORENETERRS, envstring,
                         name, szResult, sizeof(szResult)) != 0) {
                free(envstring);
                return 0;
        }
        free(envstring);
        strcpy(full_name, szResult);
        return strlen(full_name);
  }
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosCreate(CHAR *lpFileName,
                     DWORD dwAccess,
                     DWORD dwShare,
                     LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                     DWORD dwCreation,
                     DWORD dwFlags,
                     HANDLE hTemplate,
                     DWORD *dwFile)
{
   APIRET rc;
   HFILE  hFile;
   ULONG  ulAction=0;
   DWORD  os2Attrib=0;
   DWORD  os2Flags = 0; //OPEN_FLAGS_NOINHERIT;
   DWORD  os2Open=0;

    if(dwAccess == (GENERIC_READ_W | GENERIC_WRITE_W))
      os2Flags |= OPEN_ACCESS_READWRITE;
    else if(dwAccess & GENERIC_WRITE_W)
      os2Flags |= OPEN_ACCESS_WRITEONLY;
    else if(dwAccess & GENERIC_READ_W)
      os2Flags |= OPEN_ACCESS_READONLY;

    if(dwShare == 0)
      os2Flags |= OPEN_SHARE_DENYREADWRITE;
    else if(dwShare == (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W))
      os2Flags |= OPEN_SHARE_DENYNONE;
    else if(dwShare & FILE_SHARE_READ_W)
      os2Flags |= OPEN_SHARE_DENYWRITE;
    else if(dwShare & FILE_SHARE_WRITE_W)
      os2Flags |= OPEN_SHARE_DENYREAD;

    if(dwCreation == CREATE_NEW_W)
      os2Open = OPEN_ACTION_FAIL_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW;
    else if(dwCreation == CREATE_ALWAYS_W)
      os2Open = OPEN_ACTION_REPLACE_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW;
    else if(dwCreation == OPEN_EXISTING_W)
      os2Open = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW;
    else if(dwCreation == OPEN_ALWAYS_W)
      os2Open = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW;
    else if(dwCreation == TRUNCATE_EXISTING_W)
      os2Open = OPEN_ACTION_REPLACE_IF_EXISTS;// |OPEN_ACTION_FAIL_IF_NEW;

    if(dwFlags & FILE_ATTRIBUTE_READONLY_W)
      os2Attrib |= FILE_READONLY;
    if(dwFlags & FILE_ATTRIBUTE_HIDDEN_W)
      os2Attrib |= FILE_HIDDEN;
    if(dwFlags & FILE_ATTRIBUTE_SYSTEM_W)
      os2Attrib |= FILE_SYSTEM;
    if(dwFlags & FILE_ATTRIBUTE_DIRECTORY_W)
      os2Attrib |= FILE_DIRECTORY;
    if(dwFlags & FILE_ATTRIBUTE_ARCHIVE_W)
      os2Attrib |= FILE_ARCHIVED;
    if(dwFlags & FILE_ATTRIBUTE_NORMAL_W)
      os2Attrib |= FILE_NORMAL;

    if(dwFlags & FILE_FLAG_WRITE_THROUGH_W)
      os2Flags |= OPEN_FLAGS_WRITE_THROUGH;
    if(dwFlags & FILE_FLAG_NO_BUFFERING_W)
      os2Flags |= OPEN_FLAGS_NO_CACHE;
    if(dwFlags & FILE_FLAG_RANDOM_ACCESS_W)
      os2Flags |= OPEN_FLAGS_RANDOM;
    if(dwFlags & FILE_FLAG_SEQUENTIAL_SCAN_W)
      os2Flags |= OPEN_FLAGS_SEQUENTIAL;

    // TODO:
    // if(dwFlags & FILE_FLAG_OVERLAPPED_W)
    // if(dwFlags & FILE_FLAG_DELETE_ON_CLOSE_W

    rc = DosOpen(lpFileName, &hFile, &ulAction, 0,
                 os2Attrib, os2Open, os2Flags, 0);

    if(rc)
    {
      // TODO: TEST TEST
      dprintf(("DosOpen Error rc:%d, try without GENERIC_WRITE_W", rc));
      if(dwAccess & GENERIC_WRITE_W)
        os2Flags &= ~(OPEN_ACCESS_READWRITE | OPEN_ACCESS_WRITEONLY);
      rc = DosOpen(lpFileName, &hFile, &ulAction, 0,
                   os2Attrib, os2Open, os2Flags, 0);
      if(rc)
      {
        dprintf(("DosOpen Error rc:%d os2Attrib:%X os2Open:%X os2Flags:%X",
                 rc, os2Attrib, os2Open, os2Flags));
        hFile = -1;
      }
    }

    *dwFile = hFile;
    return rc;
}
//******************************************************************************
//(without changing file pointer)
//******************************************************************************
DWORD OSLibDosGetFileSize(DWORD hFile)
{
  FILESTATUS3  fsts3ConfigInfo = {{0}};
  ULONG        ulBufSize     = sizeof(FILESTATUS3);

  DosQueryFileInfo(hFile, FIL_STANDARD, &fsts3ConfigInfo, ulBufSize);
  return fsts3ConfigInfo.cbFile;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetFilePtr2(DWORD hFile, DWORD offset, DWORD method)
{
 DWORD  newoffset;
 APIRET rc;


  rc = DosSetFilePtr(hFile, offset, method, &newoffset);
  if(rc) {
    dprintf(("DosSetFilePtr Error rc:%d", rc));
    return -1;
  }
  else  return newoffset;
}
//******************************************************************************
//(FlushBuffer)
//******************************************************************************
DWORD OSLibDosResetBuffer(DWORD hFile)
{
  return DosResetBuffer(hFile);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosDupHandle(DWORD hFile, DWORD *hNew)
{
  *hNew = -1;
  return DosDupHandle(hFile, hNew);
}
//******************************************************************************
//******************************************************************************
void OSLibDosDisableHardError(BOOL fTurnOff)
{
  DosError((fTurnOff) ? FERR_DISABLEHARDERR : FERR_ENABLEHARDERR);
}
//******************************************************************************
//Returns time spent in kernel & user mode in milliseconds
//******************************************************************************
BOOL OSLibDosQueryProcTimes(DWORD procid, ULONG *kerneltime, ULONG *usertime)
{
 APIRET rc;
 char *buf;
 ULONG size;
 ULONG nrthreads = 4;

tryagain:
  size = sizeof(QTOPLEVEL)+sizeof(QGLOBAL)+sizeof(QPROCESS) + nrthreads*sizeof(QTHREAD);
  buf = (char *)malloc(size);
  rc = DosQuerySysState(0x1, RESERVED, procid, RESERVED, (PCHAR)buf, size);

  if(rc) {
        free(buf);
        if(rc == ERROR_BUFFER_OVERFLOW) {
                nrthreads += 4;
                goto tryagain;
        }
        return FALSE;
  }
  PQTOPLEVEL top = (PQTOPLEVEL)buf;

  *kerneltime = 0;
  *usertime = 0;
  for(int i=0;i<top->procdata->threadcnt;i++) {
        *kerneltime += top->procdata->threads[i].systime;
        *usertime   += top->procdata->threads[i].usertime;
  }
  free(buf);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
// TODO: implement SecurityAttributes parameter
DWORD OSLibDosCreateNamedPipe(LPCTSTR lpName,
                               DWORD   dwOpenMode,
                               DWORD   dwPipeMode,
                               DWORD   nMaxInstances,
                               DWORD   nOutBufferSize,
                               DWORD   nInBufferSize,
                               DWORD   nDefaultTimeOut,
                               void*   lpSecurityAttributes)
{  DWORD dwOS2Mode     = 0;
   DWORD dwOS2PipeMode = 0;
   LPSTR lpOS2Name;
   DWORD hPipe;
   DWORD rc;

  if (dwOpenMode & PIPE_ACCESS_DUPLEX_W)
    dwOS2Mode |= NP_ACCESS_DUPLEX;
  else
  if (dwOpenMode & PIPE_ACCESS_INBOUND_W)
    dwOS2Mode |= NP_ACCESS_INBOUND;
  else
  if (dwOpenMode & PIPE_ACCESS_OUTBOUND_W)
    dwOS2Mode |= NP_ACCESS_OUTBOUND;
  // TODO:
  // if(dwOpenMode & FILE_FLAG_OVERLAPPED)
  // if(dwOpenMode & WRITE_DAC)
  // if(dwOpenMode & WRITE_OWNER)
  // if(dwOpenMode & ACCESS_SYSTEM_SECURITY)
  if(dwOpenMode & FILE_FLAG_WRITE_THROUGH_W)
    dwOS2Mode |= NP_WRITEBEHIND; // FIXME: I'm not sure!

  if (dwPipeMode & PIPE_WAIT_W)
    dwOS2PipeMode |= NP_WAIT;
  if (dwPipeMode & PIPE_NOWAIT_W)
    dwOS2PipeMode |= NP_NOWAIT;
  if (dwPipeMode & PIPE_READMODE_BYTE_W)
    dwOS2PipeMode |= NP_READMODE_BYTE;
  if (dwPipeMode & PIPE_READMODE_MESSAGE_W)
    dwOS2PipeMode |= NP_READMODE_MESSAGE;
  if (dwPipeMode & PIPE_TYPE_BYTE_W)
    dwOS2PipeMode |= NP_TYPE_BYTE;
  if (dwPipeMode & PIPE_TYPE_MESSAGE_W)
    dwOS2PipeMode |= NP_TYPE_MESSAGE;

  if (nMaxInstances>0xff)
  {
    SetLastError(ERROR_INVALID_PARAMETER_W); // ERROR_INVALID_PARAMETER
    return -1; // INVALID_HANDLE_VALUE
  }
  dwOS2PipeMode |= nMaxInstances;

  if (strstr(lpName,"\\\\."))
  {
    // If pipe is created on the local machine
    // we must delete string \\. because
    // in Windows named pipes scheme is a \\.\PIPE\pipename
    // but in OS/2 only \PIPE\pipename
    lpOS2Name = (LPSTR)lpName + 3;
  }
  else lpOS2Name = (LPSTR)lpName;

  dprintf(("DosCreateNPipe(%s,%x,%x,%x,%x,%x)",lpOS2Name,dwOS2Mode,dwOS2PipeMode,nInBufferSize,nOutBufferSize,nDefaultTimeOut));
  rc=DosCreateNPipe(lpOS2Name,
                    &hPipe,
                    dwOS2Mode,
                    dwOS2PipeMode,
                    nInBufferSize,
                    nInBufferSize,
                    nDefaultTimeOut); // Timeouts must be tested!

  dprintf(("DosCreateNPipe rc=%d",rc));
  if (rc)
  {
    SetLastError(error2WinError(rc,ERROR_INVALID_PARAMETER_W));
    return -1; // INVALID_HANDLE_VALUE
  }
  return hPipe;
}

//******************************************************************************
//******************************************************************************
// TODO: implement lpOverlapped parameter!
BOOL OSLibDosConnectNamedPipe(DWORD hNamedPipe, LPOVERLAPPED lpOverlapped)
{
 DWORD rc;

  rc=DosConnectNPipe(hNamedPipe);
  dprintf(("DosConnectNPipe rc=%d",rc));

  if (!rc) return (TRUE);
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));

  return (FALSE);
}

//******************************************************************************
//******************************************************************************
BOOL OSLibDosCallNamedPipe( LPCTSTR lpNamedPipeName,
                            LPVOID  lpInBuffer,
                            DWORD   nInBufferSize,
                            LPVOID  lpOutBuffer,
                            DWORD   nOutBufferSize,
                            LPDWORD lpBytesRead,
                            DWORD   nTimeOut )
{
  LPSTR lpOS2Name;
  DWORD rc;

  if (strstr(lpNamedPipeName,"\\\\."))
  {
    // If pipe is created on the local machine
    // we must delete string \\. because
    // in Windows named pipes scheme is a \\.\PIPE\pipename
    // but in OS/2 only \PIPE\pipename
    lpOS2Name = (LPSTR)lpNamedPipeName + 3;
  }
  else lpOS2Name = (LPSTR)lpNamedPipeName;

  rc=DosCallNPipe(lpOS2Name,
                  lpInBuffer,
                  nInBufferSize,
                  lpOutBuffer,
                  nOutBufferSize,
                  lpBytesRead,
                  nTimeOut );


  if (!rc) return (TRUE);
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));

  return (FALSE);
}

//******************************************************************************
//******************************************************************************
BOOL OSLibDosTransactNamedPipe( DWORD  hNamedPipe,
                                LPVOID lpInBuffer,
                                DWORD nInBufferSize,
                                LPVOID lpOutBuffer,
                                DWORD nOutBufferSize,
                                LPDWORD lpBytesRead,
                                LPOVERLAPPED lpOverlapped)
{
  DWORD rc;

  rc=DosTransactNPipe(hNamedPipe,
                      lpOutBuffer,
                      nOutBufferSize,
                      lpInBuffer,
                      nInBufferSize,
                      lpBytesRead);

  dprintf(("DosTransactNPipe returned rc=%d");)
  if (!rc) return (TRUE);
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));

  return (FALSE);
}

//******************************************************************************
//******************************************************************************
BOOL OSLibDosPeekNamedPipe(DWORD   hPipe,
                        LPVOID  lpvBuffer,
                        DWORD   cbBuffer,
                        LPDWORD lpcbRead,
                        LPDWORD lpcbAvail,
                        LPDWORD lpcbMessage)
{
  DWORD     rc;
  AVAILDATA availData ={0};
  ULONG     ulDummy;

  rc=DosPeekNPipe(hPipe,lpvBuffer,cbBuffer,lpcbRead,&availData,&ulDummy);

  dprintf(("DosPeekNPipe returned rc=%d",rc));

  if (!rc)
  {
    *lpcbAvail   = availData.cbpipe;
    *lpcbMessage = availData.cbmessage;
    return (TRUE);
  }
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));

  return (FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosDisconnectNamedPipe(DWORD hPipe)
{
  DWORD     rc;

  rc=DosDisConnectNPipe(hPipe);

  dprintf(("DosDisConnectNPipe returned rc=%d",rc));

  if (!rc) return TRUE;
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));

  return (FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosWaitNamedPipe(LPCSTR lpszNamedPipeName,
                            DWORD   dwTimeout)
{
  LPSTR lpOS2Name;
  DWORD rc;

  if (strstr(lpszNamedPipeName,"\\\\."))
  {
    // If pipe is created on the local machine
    // we must delete string \\. because
    // in Windows named pipes scheme is a \\.\PIPE\pipename
    // but in OS/2 only \PIPE\pipename
    lpOS2Name = (LPSTR)lpszNamedPipeName + 3;
  }
  else lpOS2Name = (LPSTR)lpszNamedPipeName;

  rc=DosWaitNPipe(lpOS2Name,dwTimeout);

  dprintf(("DosWaitNPipe returned rc=%d",rc));

  if (!rc) return TRUE;
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));

  return (FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL isRoot(CHAR* name)
{
  if (name[1] == ':')
  {
    //local name (x:\)
    return (name[2] == 0) || !strchr(&name[3],'\\');
  } else if (name[0] == '\\')
  {
    //UNC name (\\resource\drive\)
    CHAR *drive,*dir;

    drive = strchr(&name[2],'\\');
    if (!drive) return FALSE;
    dir = strchr(&drive[1],'\\');
    if (!dir) return TRUE;
    return !strchr(&dir[1],'\\');
  } else return FALSE; //unknown
}
//******************************************************************************
//******************************************************************************
inline CHAR system2DOSCharacter(CHAR ch)
{
  switch(ch)
  {
    case ' ':
    case '.':
    case '~':
      return '_';

    default:
      return toupper(ch);
  }
}

VOID long2ShortName(CHAR* longName,CHAR* shortName)
{
  INT x;
  CHAR *source = longName,*dest = shortName,*ext = strrchr(longName,'.');

  if ((strcmp(longName,".") == 0) || (strcmp(longName,"..") == 0))
  {
    strcpy(shortName,longName);
    return;
  }

  //CB: quick and dirty, real FILE~12.EXT is too slow

  //8 character file name
  for (x = 0;x < 8;x++)
  {
    if ((source == ext) || (source[0] == 0)) break;
    dest[0] = system2DOSCharacter(source[0]);
    source++;
    dest++;
  }

  if (source[0] == 0)
  {
    dest[0] = 0;
    return;
  }

  if (source != ext)
  {
    //longName > 8 characters, insert ~1
    shortName[6] = '~';
    shortName[7] = '1';
  }

  if (ext)
  {
    //add extension, 3 characters
    dest[0] = ext[0];
    dest++;
    ext++;
    for (x = 0;x < 3;x++)
    {
      if (ext[0] == 0) break;
      dest[0] = system2DOSCharacter(ext[0]);
      ext++;
      dest++;
    }
  }
  dest[0] = 0;
}
//******************************************************************************
//******************************************************************************
VOID translateFileResults(FILESTATUS3 *pResult,LPWIN32_FIND_DATAA pFind,CHAR* achName)
{
  CHAR* name;

  pFind->dwFileAttributes = pm2WinFileAttributes(pResult->attrFile);

  pmDateTimeToFileTime(&pResult->fdateCreation,&pResult->ftimeCreation,&pFind->ftCreationTime);
  pmDateTimeToFileTime(&pResult->fdateLastAccess,&pResult->ftimeLastAccess,&pFind->ftLastAccessTime);
  pmDateTimeToFileTime(&pResult->fdateLastWrite,&pResult->ftimeLastWrite,&pFind->ftLastWriteTime);

  pFind->nFileSizeHigh = 0; //CB: fixme
  pFind->nFileSizeLow = pResult->cbFile;
  name = strrchr(achName,'\\');
  if (name)
  {
    name++;
    strcpy(pFind->cFileName,name);
  } else pFind->cFileName[0] = 0;
  long2ShortName(pFind->cFileName,pFind->cAlternateFileName);
}

VOID translateFindResults(FILEFINDBUF3 *pResult,LPWIN32_FIND_DATAA pFind)
{
  pFind->dwFileAttributes = pm2WinFileAttributes(pResult->attrFile);

  pmDateTimeToFileTime(&pResult->fdateCreation,&pResult->ftimeCreation,&pFind->ftCreationTime);
  pmDateTimeToFileTime(&pResult->fdateLastAccess,&pResult->ftimeLastAccess,&pFind->ftLastAccessTime);
  pmDateTimeToFileTime(&pResult->fdateLastWrite,&pResult->ftimeLastWrite,&pFind->ftLastWriteTime);

  pFind->nFileSizeHigh = 0; //CB: fixme
  pFind->nFileSizeLow = pResult->cbFile;
  strcpy(pFind->cFileName,pResult->achName);
  long2ShortName(pFind->cFileName,pFind->cAlternateFileName);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosFindFirst(LPCSTR lpFileName,WIN32_FIND_DATAA* lpFindFileData)
{
  HDIR hDir = -1;
  ULONG attrs;
  FILEFINDBUF3 result;
  ULONG searchCount = 1;

  attrs = FILE_READONLY | FILE_HIDDEN | FILE_SYSTEM | FILE_DIRECTORY | FILE_ARCHIVED;
  result.achName[0] = 0;

  DosError(FERR_DISABLEHARDERR | FERR_DISABLEEXCEPTION);
  APIRET rc = DosFindFirst((PSZ)lpFileName,&hDir,attrs,&result,sizeof(result),&searchCount,FIL_STANDARD);
  DosError(FERR_ENABLEHARDERR | FERR_ENABLEEXCEPTION);

  //check root: skip "." and ".." (HPFS, not on FAT)
  //check in OSLibDosFindNext not necessary: "." and ".." are the first two entries
  if ((rc == 0) && isRoot((LPSTR)lpFileName))
  {
    while ((strcmp(result.achName,".") == 0) || (strcmp(result.achName,"..") == 0))
    {
      result.achName[0] = 0;
      DosError(FERR_DISABLEHARDERR | FERR_DISABLEEXCEPTION);
      searchCount = 1;
      APIRET rc = DosFindNext(hDir,&result,sizeof(result),&searchCount);
      DosError(FERR_ENABLEHARDERR | FERR_ENABLEEXCEPTION);
      if (rc)
      {
        DosFindClose(hDir);
        SetLastError(error2WinError(rc));

        return INVALID_HANDLE_VALUE_W;
      }
    }
  }
  if(rc) {
        DosFindClose(hDir);
        SetLastError(error2WinError(rc));
	return INVALID_HANDLE_VALUE_W;
  }
  translateFindResults(&result,lpFindFileData);
  return hDir;
}
//******************************************************************************
// NOTE: returns "." and ".." in root
//******************************************************************************
DWORD OSLibDosFindFirstMulti(LPCSTR lpFileName,WIN32_FIND_DATAA *lpFindFileData,DWORD *count)
{
  HDIR hDir = -1;
  ULONG attrs;
  FILEFINDBUF3 *result,*data;
  ULONG searchCount = *count;

  attrs = FILE_READONLY | FILE_HIDDEN | FILE_SYSTEM | FILE_DIRECTORY | FILE_ARCHIVED;
  result = (FILEFINDBUF3*)malloc(searchCount*sizeof(FILEFINDBUF3));

  DosError(FERR_DISABLEHARDERR | FERR_DISABLEEXCEPTION);
  APIRET rc = DosFindFirst((PSZ)lpFileName,&hDir,attrs,result,searchCount*sizeof(FILEFINDBUF3),&searchCount,FIL_STANDARD);
  DosError(FERR_ENABLEHARDERR | FERR_ENABLEEXCEPTION);
  if (rc)
  {
    free(result);
    *count = 0;
    SetLastError(error2WinError(rc));

    return INVALID_HANDLE_VALUE_W;
  }

  data = result;
  for (int x = 0;x < searchCount;x++)
  {
    translateFindResults(data,&lpFindFileData[x]);
    data = (FILEFINDBUF3*)(((DWORD)data)+data->oNextEntryOffset);
  }
  free(result);
  *count = searchCount;

  return hDir;
}
//******************************************************************************
//******************************************************************************
BOOL  OSLibDosFindNext(DWORD hFindFile,WIN32_FIND_DATAA *lpFindFileData)
{
  FILEFINDBUF3 result;
  ULONG searchCount = 1;

  APIRET rc = DosFindNext((HDIR)hFindFile,&result,sizeof(result),&searchCount);
  if (rc)
  {
    SetLastError(error2WinError(rc));

    return FALSE;
  }

  translateFindResults(&result,lpFindFileData);

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL  OSLibDosFindNextMulti(DWORD hFindFile,WIN32_FIND_DATAA *lpFindFileData,DWORD *count)
{
  FILEFINDBUF3 *result,*data;
  ULONG searchCount = *count;

  result = (FILEFINDBUF3*)malloc(searchCount*sizeof(FILEFINDBUF3));
  APIRET rc = DosFindNext((HDIR)hFindFile,result,searchCount*sizeof(FILEFINDBUF3),&searchCount);
  if (rc)
  {
    free(result);
    *count = 0;
    SetLastError(error2WinError(rc));

    return FALSE;
  }

  data = result;
  for (int x = 0;x < searchCount;x++)
  {
    translateFindResults(data,&lpFindFileData[x]);
    data = (FILEFINDBUF3*)(((DWORD)data)+data->oNextEntryOffset);
  }
  free(result);
  *count = searchCount;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL  OSLibDosFindClose(DWORD hFindFile)
{
  APIRET rc = DosFindClose((HDIR)hFindFile);
  if (rc)
  {
    SetLastError(error2WinError(rc));

    return FALSE;
  }

  return TRUE;
}
