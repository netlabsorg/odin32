/* $Id: oslibdos.cpp,v 1.17 2000-01-27 21:51:17 sandervl Exp $ */
/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999-2000 Edgar Buerkle <Edgar.Buerkle@gmx.net>
 * Copyright 2000 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>
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
#include <win32type.h>
#include <winconst.h>
#include <misc.h>
#include "initterm.h"
#include "oslibdos.h"
#include "dosqss.h"

/***********************************
 * PH: fixups for missing os2win.h *
 ***********************************/

void _System SetLastError(ULONG ulError);

APIRET APIENTRY DosAliasMem(PVOID pb, ULONG cb, PPVOID ppbAlias, ULONG fl);

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
  else	flags |= OBJ_GETTABLE;
 
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
  else	rc = DosGetSharedMem((LPVOID)*(DWORD *)lplpMemAddr, PAG_READ|PAG_WRITE);

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
  else	return hFile;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosClose(DWORD hFile)
{
  return DosClose(hFile);
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
	os2method = FILE_BEGIN	;
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
  else	return newoffset;
}
//******************************************************************************
//******************************************************************************
//@@@PH Note: this routine is nothing but a QUICK'N'DIRTY HACK!
//@@@PH       this function should be implemented accordingly to NTDLL's
//            RtlSecondsSince1980ToTime
//            RtlTimeToSecondsSince1980
static void iFDATEFTIME2FILETIME(FDATE fdOS2, FTIME ftOS2, LPFILETIME pftWin32)
{
  float f;
  #define facSECOND 2                // as encoded in OS/2
  #define facMINUTE 60
  #define facHOUR   3600
  #define facDAY    86400
  #define facMONTH  facDAY * 30      // cough, cough :)
  #define facYEAR   facDAY * 365

  /* pftWin32 is 100ns based from 01.01.1601 00:00:00 */
  f =   (fdOS2.year  +  379) * facYEAR        // 1980 - 1601
      + (fdOS2.month - 0   ) * facMONTH
      + (fdOS2.day   - 1   ) * facDAY
      + (ftOS2.hours       ) * facHOUR
      + (ftOS2.minutes     ) * facMINUTE
      + (ftOS2.twosecs     ) * facSECOND;

  f *= 10000; // convert to 100ns base
  pftWin32->dwHighDateTime = (f / (float)(0xffffffff) );
  pftWin32->dwLowDateTime  = (f - (float)((float)pftWin32->dwHighDateTime *
                                          (float)0xffffffff) );
}

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
  lpFad->dwFileAttributes = fs3.attrFile; // directly interchangeable
  iFDATEFTIME2FILETIME(fs3.fdateCreation,   fs3.ftimeCreation,   &lpFad->ftCreationTime);
  iFDATEFTIME2FILETIME(fs3.fdateLastAccess, fs3.ftimeLastAccess, &lpFad->ftLastAccessTime);
  iFDATEFTIME2FILETIME(fs3.fdateLastWrite,  fs3.ftimeLastWrite,  &lpFad->ftLastWriteTime);

  /* @@@PH we might add Aurora support ...
  lpFad->nFileSizeHigh    = info.nFileSizeHigh;
  */
  lpFad->nFileSizeHigh    = 0;
  lpFad->nFileSizeLow     = fs3.cbFile;

  return TRUE;
}
//******************************************************************************
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
	if(DosSearchPath(SEARCH_IGNORENETERRS | SEARCH_ENVIRONMENT, path,
                         name, full_name, length_fullname) != 0) {
		return 0;
	}
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
  else	return newoffset;
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
    SetLastError(87); // ERROR_INVALID_PARAMETER
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
     if ( rc == ERROR_PIPE_BUSY         ) SetLastError(ERROR_PIPE_BUSY_W);
       else
     if ( rc == ERROR_PATH_NOT_FOUND    ) SetLastError(ERROR_PATH_NOT_FOUND_W);
       else
     if ( rc == ERROR_NOT_ENOUGH_MEMORY ) SetLastError(ERROR_NOT_ENOUGH_MEMORY_W);
       else
     if ( rc == ERROR_INVALID_PARAMETER ) SetLastError(ERROR_INVALID_PARAMETER_W);
       else
     if ( rc == ERROR_OUT_OF_STRUCTURES ) SetLastError(ERROR_OUT_OF_STRUCTURES_W);
       else
         // Unknown error
         SetLastError(ERROR_INVALID_PARAMETER_W); // fixme!
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
    else
  if (rc==ERROR_BROKEN_PIPE) SetLastError(ERROR_BROKEN_PIPE_W);    
    else
  if (rc==ERROR_BAD_PIPE) SetLastError(ERROR_BAD_PIPE_W);
    else
  if (rc==ERROR_PIPE_NOT_CONNECTED) SetLastError(ERROR_PIPE_NOT_CONNECTED_W);
    else
  // TODO: Implemnt this using Windows Errors
  // if (rc==ERROR_INTERRUPT)
  SetLastError(ERROR_PIPE_NOT_CONNECTED_W);
  
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
   else
  if ( rc==ERROR_FILE_NOT_FOUND     ) SetLastError(ERROR_FILE_NOT_FOUND_W);    
    else
  if ( rc==ERROR_PATH_NOT_FOUND     ) SetLastError(ERROR_PATH_NOT_FOUND_W);    
    else
  if ( rc==ERROR_ACCESS_DENIED      ) SetLastError(ERROR_ACCESS_DENIED_W);    
    else
  if ( rc==ERROR_MORE_DATA          ) SetLastError(ERROR_MORE_DATA_W);    
    else
  if ( rc==ERROR_PIPE_BUSY          ) SetLastError(ERROR_PIPE_BUSY_W);    
    else
  if ( rc==ERROR_BAD_FORMAT         ) SetLastError(ERROR_BAD_FORMAT_W);    
    else
  if ( rc==ERROR_BROKEN_PIPE        ) SetLastError(ERROR_BROKEN_PIPE_W);    
    else
  if ( rc==ERROR_BAD_PIPE           ) SetLastError(ERROR_BAD_PIPE_W);
    else
  if ( rc==ERROR_PIPE_NOT_CONNECTED ) SetLastError(ERROR_PIPE_NOT_CONNECTED_W);
    else
  // TODO: Implemnt this using Windows Errors
  // if (rc==ERROR_INTERRUPT)
  SetLastError(233);

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
   else
  if ( rc==ERROR_ACCESS_DENIED      ) SetLastError(ERROR_ACCESS_DENIED_W);    
    else
  if ( rc==ERROR_MORE_DATA          ) SetLastError(ERROR_MORE_DATA_W);    
    else
  if ( rc==ERROR_PIPE_BUSY          ) SetLastError(ERROR_PIPE_BUSY_W);    
    else
  if ( rc==ERROR_BAD_FORMAT         ) SetLastError(ERROR_BAD_FORMAT_W);    
    else
  if ( rc==ERROR_BROKEN_PIPE        ) SetLastError(ERROR_BROKEN_PIPE_W);    
    else
  if ( rc==ERROR_BAD_PIPE           ) SetLastError(ERROR_BAD_PIPE_W);
    else
  if ( rc==ERROR_PIPE_NOT_CONNECTED ) SetLastError(ERROR_PIPE_NOT_CONNECTED_W);
    else
  // Unknown error
  SetLastError(ERROR_PIPE_NOT_CONNECTED_W);

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
   else
  if ( rc==ERROR_ACCESS_DENIED      ) SetLastError(ERROR_ACCESS_DENIED_W);    
    else
  if ( rc==ERROR_PIPE_BUSY          ) SetLastError(ERROR_PIPE_BUSY_W);    
    else
  if ( rc==ERROR_BAD_PIPE           ) SetLastError(ERROR_BAD_PIPE_W);
    else
  if ( rc==ERROR_PIPE_NOT_CONNECTED ) SetLastError(ERROR_PIPE_NOT_CONNECTED_W);
    else
  // Unknown error
  SetLastError(ERROR_PIPE_NOT_CONNECTED_W);

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
    else
  if ( rc==ERROR_BROKEN_PIPE        ) SetLastError(ERROR_BROKEN_PIPE_W);    
    else
  if ( rc==ERROR_BAD_PIPE           ) SetLastError(ERROR_BAD_PIPE_W);
    else
     // Unknown error
     SetLastError(ERROR_PIPE_NOT_CONNECTED_W); // Maybe another?

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
    else
  if ( rc == ERROR_PATH_NOT_FOUND ) SetLastError(ERROR_PATH_NOT_FOUND_W);
    else
  if ( rc==ERROR_BAD_PIPE         ) SetLastError(ERROR_BAD_PIPE_W);
    else
  if ( rc == ERROR_PIPE_BUSY      ) SetLastError(ERROR_PIPE_BUSY_W);
    else
  if ( rc == ERROR_SEM_TIMEOUT_W  ) SetLastError(ERROR_SEM_TIMEOUT_W);
    else
  // TODO: Implemnt this using Windows Errors
  // if (rc==ERROR_INTERRUPT)
  SetLastError(ERROR_PIPE_NOT_CONNECTED_W);

  return (FALSE);
}