/* $Id: oslibdos.cpp,v 1.12 1999-12-01 18:40:48 sandervl Exp $ */

/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include "initterm.h"
#include "oslibdos.h"
#include "dosqss.h"

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
	dprintf(("OSLibDosAliasMem: DosAliasMem %x %x return %d", pb, cb, rc));
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
	char *env = getenv(path);
	if(env == NULL)
		return 0;

	while(*env != '=') env++;
	env++;
	while(*env == ' ') env++;
	if(DosSearchPath(SEARCH_IGNORENETERRS | SEARCH_ENVIRONMENT, env,
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

#define GENERIC_READ               0x80000000
#define GENERIC_WRITE              0x40000000
    if(dwAccess == (GENERIC_READ | GENERIC_WRITE))
      os2Flags |= OPEN_ACCESS_READWRITE;
    else if(dwAccess & GENERIC_WRITE)
      os2Flags |= OPEN_ACCESS_WRITEONLY;
    else if(dwAccess & GENERIC_READ)
      os2Flags |= OPEN_ACCESS_READONLY;

#define FILE_SHARE_READ         0x00000001L
#define FILE_SHARE_WRITE        0x00000002L
    if(dwShare == 0)
      os2Flags |= OPEN_SHARE_DENYREADWRITE;
    else if(dwShare == (FILE_SHARE_READ | FILE_SHARE_WRITE))
      os2Flags |= OPEN_SHARE_DENYNONE;
    else if(dwShare & FILE_SHARE_READ)
      os2Flags |= OPEN_SHARE_DENYWRITE;
    else if(dwShare & FILE_SHARE_WRITE)
      os2Flags |= OPEN_SHARE_DENYREAD;

#define CREATE_NEW              1
#define CREATE_ALWAYS           2
#define OPEN_EXISTING           3
#define OPEN_ALWAYS             4
#define TRUNCATE_EXISTING       5
    if(dwCreation == CREATE_NEW)
      os2Open = OPEN_ACTION_FAIL_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW;
    else if(dwCreation == CREATE_ALWAYS)
      os2Open = OPEN_ACTION_REPLACE_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW;
    else if(dwCreation == OPEN_EXISTING)
      os2Open = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW;
    else if(dwCreation == OPEN_ALWAYS)
      os2Open = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW;
    else if(dwCreation == TRUNCATE_EXISTING)
      os2Open = OPEN_ACTION_REPLACE_IF_EXISTS;// |OPEN_ACTION_FAIL_IF_NEW;

#define FILE_ATTRIBUTE_READONLY         0x00000001L
#define FILE_ATTRIBUTE_HIDDEN           0x00000002L
#define FILE_ATTRIBUTE_SYSTEM           0x00000004L
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010L
#define FILE_ATTRIBUTE_ARCHIVE          0x00000020L
#define FILE_ATTRIBUTE_NORMAL           0x00000080L
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100L
    if(dwFlags & FILE_ATTRIBUTE_READONLY)
      os2Attrib |= FILE_READONLY;
    if(dwFlags & FILE_ATTRIBUTE_HIDDEN)
      os2Attrib |= FILE_HIDDEN;
    if(dwFlags & FILE_ATTRIBUTE_SYSTEM)
      os2Attrib |= FILE_SYSTEM;
    if(dwFlags & FILE_ATTRIBUTE_DIRECTORY)
      os2Attrib |= FILE_DIRECTORY;
    if(dwFlags & FILE_ATTRIBUTE_ARCHIVE)
      os2Attrib |= FILE_ARCHIVED;
    if(dwFlags & FILE_ATTRIBUTE_NORMAL)
      os2Attrib |= FILE_NORMAL;

#define FILE_FLAG_WRITE_THROUGH    0x80000000UL
#define FILE_FLAG_OVERLAPPED       0x40000000L
#define FILE_FLAG_NO_BUFFERING     0x20000000L
#define FILE_FLAG_RANDOM_ACCESS    0x10000000L
#define FILE_FLAG_SEQUENTIAL_SCAN  0x08000000L
#define FILE_FLAG_DELETE_ON_CLOSE  0x04000000L
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000L
#define FILE_FLAG_POSIX_SEMANTICS  0x01000000L
    if(dwFlags & FILE_FLAG_WRITE_THROUGH)
      os2Flags |= OPEN_FLAGS_WRITE_THROUGH;
    if(dwFlags & FILE_FLAG_NO_BUFFERING)
      os2Flags |= OPEN_FLAGS_NO_CACHE;
    if(dwFlags & FILE_FLAG_RANDOM_ACCESS)
      os2Flags |= OPEN_FLAGS_RANDOM;
    if(dwFlags & FILE_FLAG_SEQUENTIAL_SCAN)
      os2Flags |= OPEN_FLAGS_SEQUENTIAL;

    // TODO:
    // if(dwFlags & FILE_FLAG_OVERLAPPED)
    // if(dwFlags & FILE_FLAG_DELETE_ON_CLOSE

    rc = DosOpen(lpFileName, &hFile, &ulAction, 0,
                 os2Attrib, os2Open, os2Flags, 0);

    if(rc)
    {
      // TODO: TEST TEST
      dprintf(("DosOpen Error rc:%d, try without GENERIC_WRITE", rc));
      if(dwAccess & GENERIC_WRITE)
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
