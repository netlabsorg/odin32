/* $Id: mmap.cpp,v 1.7 1999-08-24 12:23:54 sandervl Exp $ */

/*
 * Win32 Memory mapped file class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * NOTE: Memory mapping DOES NOT work when kernel-mode code causes
 *       a pagefault in the memory mapped object. (exceptions aren't
 *       dispatched to our exception handler until after the kernel mode
 *       call returns (too late))
 *
 * NOTE: There's no easy way to determine which pages were modified. Temporary
 *       solution is to write all paged-in memory to disk.
 *
 * NOTE: Are apps allowed to change the protection flags of memory mapped pages?
 *       I'm assuming they aren't for now.
 *
 * TODO: Handles returned should be usable by all apis that accept file handles
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <win\virtual.h>
#include <vmutex.h>
#include <handlemanager.h>
#include "mmap.h"

VMutex globalmapMutex;

//******************************************************************************
//TODO: sharing between processes
//******************************************************************************
Win32MemMap::Win32MemMap(HFILE hfile, ULONG size, ULONG fdwProtect, LPSTR lpszName)
               : fMapped(FALSE), pMapping(NULL), mMapAccess(0)
{
  globalmapMutex.enter();
  memmaps = this;
  next    = memmaps;
  globalmapMutex.leave();

  hMemFile   = hfile;

  mSize      = size;
  mProtFlags = fdwProtect;

  if(lpszName) {
	lpszMapName = (char *)malloc(strlen(lpszName)+1);
	strcpy(lpszMapName, lpszName);
  }
  else	lpszMapName = NULL;
}
//******************************************************************************
//******************************************************************************
HANDLE Win32MemMap::Init()
{
  mapMutex.enter();
  if(hMemFile != -1) 
  {
     	if(DuplicateHandle(GetCurrentProcess(), hMemFile, GetCurrentProcess(),
                           &hMemFile, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) 
     	{
		dprintf(("Win32MemMap::Init: DuplicateHandle failed!"));
		goto fail;
     	}
  }
  if(HMHandleAllocate(&hMemMap, (ULONG)this) != 0)
  {
	dprintf(("Win32MemMap::Init HMHandleAllocate failed!!"));
	DebugInt3();
	goto fail;
  }
  mapMutex.leave();
  return hMemMap;
fail:
  mapMutex.leave();
  return 0;
}
//******************************************************************************
//******************************************************************************
Win32MemMap::~Win32MemMap()
{
  unmapFileView();
  if(lpszMapName) {
	free(lpszMapName);
  }
  mapMutex.enter();
  if(pMapping) {
	VirtualFree(pMapping, mSize, MEM_RELEASE);
	pMapping = NULL;
  }
  if(hMemFile != -1) {
  	CloseHandle(hMemFile);
	hMemFile = -1;
  }
  mapMutex.leave();

  globalmapMutex.enter();
  Win32MemMap *map = memmaps;

  if(map == this) {
	memmaps = next;
  }
  else {
  	while(map->next) {
		if(map->next == this)
			break;
		map = map->next;
	}
	if(map->next) {
		map->next = next;
	}
	else	dprintf(("Win32MemMap::~Win32MemMap: map not found!! (%x)", this));
  }
  globalmapMutex.leave();
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMap::unmapFileView()
{
  if(fMapped == FALSE)
	return FALSE;

  flushView(pMapping, mSize);
  mapMutex.enter();
  if(pMapping) {
	VirtualFree(pMapping, mSize, MEM_RELEASE);
  }
  pMapping = NULL;
  fMapped = FALSE;
  mapMutex.leave();
  return TRUE;
}
//******************************************************************************
//******************************************************************************
LPVOID Win32MemMap::mapFileView(ULONG size, ULONG offset, ULONG fdwAccess)
{
  mapMutex.enter();
  ULONG memFlags = (mProtFlags & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY));
  ULONG fAlloc   = 0;

  if(fdwAccess & FILE_MAP_WRITE && mProtFlags & (PAGE_READONLY|PAGE_WRITECOPY)) 
	goto parmfail;
  if(fdwAccess & FILE_MAP_READ && mProtFlags & PAGE_WRITECOPY) 
	goto parmfail;
  if(fdwAccess & FILE_MAP_ALL_ACCESS && mProtFlags & (PAGE_READONLY|PAGE_WRITECOPY)) 
	goto parmfail;
  if(fdwAccess & FILE_MAP_COPY && mProtFlags & PAGE_WRITECOPY) 
	goto parmfail;
  
  if(mProtFlags & SEC_COMMIT)
	fAlloc |= MEM_COMMIT;
  if(mProtFlags & SEC_RESERVE)
 	fAlloc |= MEM_RESERVE;

  pMapping = VirtualAlloc(0, mSize, fAlloc, memFlags);
  if(pMapping == NULL) {
	dprintf(("Win32MemMap::mapFileView: VirtualAlloc %x %x %x failed!", mSize, fAlloc, memFlags));
	goto fail;
  }
  mapMutex.leave();
  return pMapping;

parmfail:
  dprintf(("Win32MemMap::mapFileView: ERROR_INVALID_PARAMETER"));
  SetLastError(ERROR_INVALID_PARAMETER);
fail:
  mapMutex.leave();
  return 0;
}
//******************************************************************************
// NOTE: There's no easy way to determine which pages were modified. Temporary
//       solution is to write all paged-in memory to disk.
//TODO: Are apps allowed to change the protection flags of memory mapped pages?
//      I'm assuming they aren't for now.
//******************************************************************************
BOOL Win32MemMap::flushView(LPVOID lpvBase, ULONG cbFlush)
{
 MEMORY_BASIC_INFORMATION memInfo;
 ULONG nrpages, nrBytesWritten;
 int   i;

  mapMutex.enter();
  if(fMapped == FALSE)
	goto parmfail;

  if(lpvBase < pMapping || (ULONG)lpvBase+cbFlush > (ULONG)pMapping+mSize)
	goto parmfail;

  if(mProtFlags & PAGE_READONLY)
	goto parmfail;

  nrpages = cbFlush/PAGE_SIZE;
  if(cbFlush & 0xFFF)  nrpages++;

  for(i=0;i<nrpages;i++) {
	if(VirtualQuery((LPSTR)lpvBase+i*PAGE_SIZE, &memInfo, PAGE_SIZE) == 0) {
		dprintf(("Win32MemMap::flushView: VirtualQuery (%x,%x) failed for %x", lpvBase, cbFlush, (ULONG)lpvBase+i*PAGE_SIZE));
		goto fail;
	}
	//If a page is reserved or write protected, we won't bother flushing it to disk
	if(memInfo.State & MEM_COMMIT && 
           memInfo.AllocationProtect & (PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)) 
        {//committed and allowed for writing?
		if(WriteFile(hMemFile, (LPSTR)lpvBase+i*PAGE_SIZE, PAGE_SIZE, &nrBytesWritten, NULL) == FALSE) {
			dprintf(("Win32MemMap::flushView: WriteFile failed for %x", (ULONG)lpvBase+i*PAGE_SIZE));
			goto fail;
		}
		if(nrBytesWritten != PAGE_SIZE) {
			dprintf(("Win32MemMap::flushView: WriteFile didn't write all bytes for %x", (ULONG)lpvBase+i*PAGE_SIZE));
			goto fail;
		}
	}
  }
  mapMutex.leave();
  return TRUE;
parmfail:
  SetLastError(ERROR_INVALID_PARAMETER);
  mapMutex.leave();
  return FALSE;
fail:
  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::findMap(LPSTR lpszName)
{
  globalmapMutex.enter();
  Win32MemMap *map = memmaps;

  if(map != NULL) {
  	while(map) {
		if(map->lpszMapName && !strcmp(map->lpszMapName, lpszName))
			break;
		map = map->next;
	}
  }
  globalmapMutex.leave();
  dprintf(("Win32MemMap::findMap: couldn't find map %s", lpszName));
  return map;
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::findMap(ULONG address)
{
  globalmapMutex.enter();
  Win32MemMap *map = memmaps;

  if(map != NULL) {
  	while(map) {
		if(map->pMapping && (ULONG)map->pMapping <= address &&
                   (ULONG)map->pMapping + map->mSize > address)
		{
			break;
		}
		map = map->next;
	}
  }
  globalmapMutex.leave();
  return map;
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::memmaps = NULL;
