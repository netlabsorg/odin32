/* $Id: oslibdos.cpp,v 1.1 1999-09-15 23:32:59 sandervl Exp $ */

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
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include "initterm.h"
#include "oslibdos.h"

APIRET APIENTRY DosAliasMem(PVOID pb, ULONG cb, PPVOID ppbAlias, ULONG fl);

//******************************************************************************
//******************************************************************************
DWORD OSLibDosAliasMem(LPVOID pb, ULONG cb, LPVOID *ppbAlias, ULONG fl)
{
  return DosAliasMem(pb, cb, ppbAlias, fl);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags)
{
  return DosAllocMem(lplpMemAddr, size, flags | flAllocMem);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosFreeMem(LPVOID lpMemAddr)
{
  return DosFreeMem(lpMemAddr);
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
DWORD OSLibDosGetFileSize(DWORD hFile)
{
 ULONG  ulLocal, filesize = 0;

  DosSetFilePtr(hFile, 0L, FILE_BEGIN, &ulLocal);
  DosSetFilePtr(hFile, 0L, FILE_END, &filesize);
  DosSetFilePtr(hFile, 0L, FILE_BEGIN, &ulLocal);
  return filesize;
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
