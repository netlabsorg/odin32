/* $Id: mmap.cpp,v 1.2 1999-09-25 19:06:17 sandervl Exp $ */

/*
 * Win32 Memory mapped file & view classes
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * NOTE: Memory mapping DOES NOT work when kernel-mode code causes
 *       a pagefault in the memory mapped object. (exceptions aren't
 *       dispatched to our exception handler until after the kernel mode
 *       call returns (too late))
 *
 * NOTE: Are apps allowed to change the protection flags of memory mapped pages?
 *       I'm assuming they aren't for now.
 *
 * TODO: Handles returned should be usable by all apis that accept file handles
 * TODO: Sharing memory mapped files between multiple processes
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <virtual.h>
#include <vmutex.h>
#include <handlemanager.h>
#include "mmap.h"
#include "oslibdos.h"

VMutex globalmapMutex;
VMutex globalviewMutex;

//******************************************************************************
//TODO: sharing between processes
//******************************************************************************
Win32MemMap::Win32MemMap(HFILE hfile, ULONG size, ULONG fdwProtect, LPSTR lpszName)
               : nrMappings(0), pMapping(NULL), mMapAccess(0), referenced(0)
{
  globalmapMutex.enter();
  next    = memmaps;
  memmaps = this;
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
BOOL Win32MemMap::Init(HANDLE hMemMap)
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
	mSize = SetFilePointer(hMemFile, 0, NULL, FILE_END);
	if(mSize == -1) {
		dprintf(("Win32MemMap::init: SetFilePointer failed to set pos end"));
		goto fail;
	}
  }

  dprintf(("CreateFileMappingA for file %x, prot %x size %d, name %s", hMemFile, mProtFlags, mSize, lpszMapName));
  this->hMemMap = hMemMap;
  mapMutex.leave();
  return TRUE;
fail:
  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
//******************************************************************************
Win32MemMap::~Win32MemMap()
{
  for(int i=0;i<nrMappings;i++) {
	Win32MemMapView::deleteView(this); //delete all views of our memory mapped file
  }
  mapMutex.enter();
  if(lpszMapName) {
	free(lpszMapName);
  }
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
//We determine whether a page has been modified by checking it's protection flags
//If the write flag is set, this means commitPage had to enable this due to a pagefault
//(all pages are readonly until the app tries to write to it)
//******************************************************************************
BOOL Win32MemMap::commitPage(ULONG offset, BOOL fWriteAccess)
{
 MEMORY_BASIC_INFORMATION memInfo;
 LPVOID lpPageFaultAddr = (LPVOID)((ULONG)pMapping + offset);
 DWORD pageAddr         = (DWORD)lpPageFaultAddr & ~0xFFF;
 DWORD oldProt, newProt, nrBytesRead, size;

//  mapMutex.enter();

  newProt  = mProtFlags & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY);

  dprintf(("Win32MemMap::commitPage %x (faultaddr %x)", pageAddr, lpPageFaultAddr));
  if(hMemFile != -1) {
	if(VirtualQuery((LPSTR)pageAddr, &memInfo, NRPAGES_TOCOMMIT*PAGE_SIZE) == 0) {
		dprintf(("Win32MemMap::commitPage: VirtualQuery (%x,%x) failed for %x", pageAddr, NRPAGES_TOCOMMIT*PAGE_SIZE));
		goto fail;
	}
	//Only changes the state of the pages with the same attribute flags
	//(returned in memInfo.RegionSize)
	//If it's smaller than the mNrPages, it simply means one or more of the
        //other pages have already been committed
	if(memInfo.State & MEM_COMMIT)
        {//if it's already committed, then the app tried to write to it
		if(!fWriteAccess) {
			dprintf(("Win32MemMap::commitPage: Huh? Already committed and not trying to write (%x,%x) failed for %x", pageAddr, memInfo.RegionSize));
			goto fail;
		}
		if(VirtualProtect((LPVOID)pageAddr, memInfo.RegionSize, newProt, &oldProt) == FALSE) {
			dprintf(("Win32MemMap::commitPage: Failed to set write flag on page (%x,%x) failed for %x", pageAddr, memInfo.RegionSize));
			goto fail;
		}
	}
	else {
  		if(VirtualAlloc((LPVOID)pageAddr, memInfo.RegionSize, MEM_COMMIT, PAGE_READWRITE) == FALSE) {
			goto fail;
  		}
		offset = pageAddr - (ULONG)pMapping;
		size   = memInfo.RegionSize;
		if(offset + size > mSize) {
			dprintf(("Adjusting size from %d to %d", size, mSize - offset));
			size = mSize - offset;
		}
		if(SetFilePointer(hMemFile, offset, NULL, FILE_BEGIN) != offset) {
			dprintf(("Win32MemMap::commitPage: SetFilePointer failed to set pos to %x", offset));
			goto fail;
		}
		if(ReadFile(hMemFile, (LPSTR)pageAddr, size, &nrBytesRead, NULL) == FALSE) {
			dprintf(("Win32MemMap::commitPage: ReadFile failed for %x", pageAddr));
			goto fail;
		}
		if(nrBytesRead != size) {
			dprintf(("Win32MemMap::commitPage: ReadFile didn't read all bytes for %x", pageAddr));
			goto fail;
		}
		if(mProtFlags & PAGE_READONLY) {
  			if(VirtualProtect((LPVOID)pageAddr, memInfo.RegionSize, newProt, &oldProt) == FALSE) {
				goto fail;
			}
		}
	}
  }
  else {
	if(VirtualQuery((LPSTR)pageAddr, &memInfo, NRPAGES_TOCOMMIT*PAGE_SIZE) == 0) {
		dprintf(("Win32MemMap::commitPage: VirtualQuery (%x,%x) failed for %x", pageAddr, NRPAGES_TOCOMMIT*PAGE_SIZE));
		goto fail;
	}
  	if(VirtualAlloc((LPVOID)pageAddr, memInfo.RegionSize, MEM_COMMIT, newProt) == FALSE) {
		goto fail;
  	}
  }

//  mapMutex.leave();
  return TRUE;
fail:
//  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
//todo: unalias memory
//******************************************************************************
BOOL Win32MemMap::unmapViewOfFile(Win32MemMapView *view)
{
  mapMutex.enter();

  if(nrMappings == 0)
	goto fail;

  delete view;

  if(--nrMappings) {
	VirtualFree(pMapping, mSize, MEM_RELEASE);
	pMapping = NULL;
  }
  mapMutex.leave();
  return TRUE;
fail:
  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
//******************************************************************************
LPVOID Win32MemMap::mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess)
{
  mapMutex.enter();
  ULONG memFlags = (mProtFlags & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY));
  ULONG fAlloc   = 0;
  Win32MemMapView *mapview;

  //@@@PH: if(fdwAccess & ~(FILE_MAP_WRITE|FILE_MAP_READ|FILE_MAP_COPY))
  // Docs say FILE_MAP_ALL_ACCESS is same as FILE_MAP_WRITE. Doesn't match reality though.
  if(fdwAccess & ~FILE_MAP_ALL_ACCESS)
	goto parmfail;
  if((fdwAccess & FILE_MAP_WRITE) && !(mProtFlags & PAGE_READWRITE))
	goto parmfail;
  if((fdwAccess & FILE_MAP_READ) && !(mProtFlags & (PAGE_READWRITE|PAGE_READONLY)))
	goto parmfail;

  //@@@PH
  if (fdwAccess != FILE_MAP_ALL_ACCESS)
    if((fdwAccess & FILE_MAP_COPY) && !(mProtFlags & PAGE_WRITECOPY))
      goto parmfail;

  if(offset+size > mSize)
	goto parmfail;

//TODO: If committed, read file into memory
#if 0
  if(mProtFlags & SEC_COMMIT)
	fAlloc |= MEM_COMMIT;
  else
  if(mProtFlags & SEC_RESERVE)
 	fAlloc |= MEM_RESERVE;
#else
  fAlloc = MEM_RESERVE;
#endif

  if(nrMappings == 0) {//if not mapped, reserve/commit entire view
	//SvL: Always read/write access or else ReadFile will crash once we
 	//     start committing pages.
	//     This is most likely an OS/2 bug and doesn't happen in Aurora
        //     when allocating memory with the PAG_ANY bit set. (without this
        //     flag it will also crash)
  	pMapping = VirtualAlloc(0, mSize, fAlloc, PAGE_READWRITE);
  	if(pMapping == NULL) {
		dprintf(("Win32MemMap::mapFileView: VirtualAlloc %x %x %x failed!", mSize, fAlloc, memFlags));
		goto fail;
  	}
  }
  mapview = new Win32MemMapView(this, offset, (size == 0) ? mSize : size, fdwAccess);
  if(mapview == NULL) {
	goto fail;
  }
  if(mapview->everythingOk() == FALSE) {
	delete mapview;
	goto fail;
  }
  nrMappings++;
  mapMutex.leave();
  return mapview->getViewAddr();

parmfail:
  dprintf(("Win32MemMap::mapFileView: ERROR_INVALID_PARAMETER"));
  SetLastError(ERROR_INVALID_PARAMETER);
fail:
  mapMutex.leave();
  return 0;
}
//******************************************************************************
//We determine whether a page has been modified by checking it's protection flags
//If the write flag is set, this means commitPage had to enable this due to a pagefault
//(all pages are readonly until the app tries to modify the contents of the page)
//
//TODO: Are apps allowed to change the protection flags of memory mapped pages?
//      I'm assuming they aren't for now.
//******************************************************************************
BOOL Win32MemMap::flushView(ULONG offset, ULONG cbFlush)
{
 LPVOID lpvBase = (LPVOID)((ULONG)pMapping+offset);
 MEMORY_BASIC_INFORMATION memInfo;
 ULONG nrBytesWritten, size;
 int   i;

  dprintf(("Win32MemMap::flushView: %x %x", lpvBase, cbFlush));
  if(nrMappings == 0)
	goto parmfail;

  if(cbFlush == 0)
 	cbFlush = mSize;

  if(lpvBase < pMapping || (ULONG)lpvBase+cbFlush > (ULONG)pMapping+mSize)
	goto parmfail;

  if(mProtFlags & PAGE_READONLY)
	goto parmfail;

  if(hMemFile == -1)
	goto success; //TODO: Return an error here?

  while(cbFlush) {
	if(VirtualQuery((LPSTR)lpvBase, &memInfo, cbFlush) == 0) {
		dprintf(("Win32MemMap::flushView: VirtualQuery (%x,%x) failed for %x", lpvBase, cbFlush, (ULONG)lpvBase+i*PAGE_SIZE));
		goto fail;
	}
	//If a page (or range of pages) is reserved or write protected, we
        //won't bother flushing it to disk
	if(memInfo.State & MEM_COMMIT &&
           memInfo.AllocationProtect & (PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY))
        {//committed and allowed for writing?
		offset = (ULONG)lpvBase - (ULONG)pMapping;
		size   = memInfo.RegionSize;
		if(size > cbFlush) {
			size = cbFlush;
		}
		dprintf(("Win32MemMap::flushView for offset %x, size %d", offset, size));

		if(SetFilePointer(hMemFile, offset, NULL, FILE_BEGIN) != offset) {
			dprintf(("Win32MemMap::flushView: SetFilePointer failed to set pos to %x", offset));
			goto fail;
		}
		if(WriteFile(hMemFile, (LPSTR)lpvBase, size, &nrBytesWritten, NULL) == FALSE) {
			dprintf(("Win32MemMap::flushView: WriteFile failed for %x", (ULONG)lpvBase));
			goto fail;
		}
		if(nrBytesWritten != size) {
			dprintf(("Win32MemMap::flushView: WriteFile didn't write all bytes for %x", (ULONG)lpvBase));
			goto fail;
		}
	}
	lpvBase = (LPVOID)((ULONG)lpvBase + memInfo.RegionSize);

	if(cbFlush < memInfo.RegionSize)
		break;

	cbFlush -= memInfo.RegionSize;
  }
success:
  return TRUE;
parmfail:
  SetLastError(ERROR_INVALID_PARAMETER);
  return FALSE;
fail:
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
//Assumes mutex has been acquired
//******************************************************************************
void Win32MemMap::deleteAll()
{
  while(memmaps) {
	CloseHandle(memmaps->hMemMap);
  }
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::memmaps = NULL;

//******************************************************************************
//******************************************************************************
Win32MemMapView::Win32MemMapView(Win32MemMap *map, ULONG offset, ULONG size,
                                 ULONG fdwAccess)
{
 LPVOID           viewaddr = (LPVOID)((ULONG)map->getMappingAddr()+offset);
 ULONG            accessAttr = 0;
 Win32MemMapView *tmpview  = mapviews;

  errorState = 0;
  mParentMap = map;
  mSize    = size;
  mOffset  = offset;

  switch(fdwAccess) {
  case FILE_MAP_READ:
	accessAttr = PAG_READ;
	mfAccess   = MEMMAP_ACCESS_READ;
	break;
  case FILE_MAP_WRITE:
  case FILE_MAP_COPY:
	accessAttr = (PAG_READ|PAG_WRITE);
	mfAccess   = MEMMAP_ACCESS_WRITE;
	break;
  }
  if(OSLibDosAliasMem(viewaddr, size, &pMapView, accessAttr) != OSLIB_NOERROR) {
	dprintf(("new OSLibDosAliasMem FAILED"));
	SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	errorState = 1;
  	return;
  }

  dprintf(("Win32MemMapView::Win32MemMapView: created %x (alias for %x), size %d", pMapView, viewaddr, size));

  globalviewMutex.enter();
  if(tmpview == NULL || tmpview->getViewAddr() > pMapView) {
	next     = mapviews;
	mapviews = this;
  }
  else {
	while(tmpview->next) {
		if(tmpview->next->getViewAddr() > pMapView) {
			break;
		}
		tmpview = tmpview->next;
	}
	next          = tmpview->next;
	tmpview->next = this;
  }
  globalviewMutex.leave();
}
//******************************************************************************
//******************************************************************************
Win32MemMapView::~Win32MemMapView()
{
  if(errorState != 0)
	return;

  if(mfAccess != MEMMAP_ACCESS_READ)
  	mParentMap->flushView(mOffset, mSize);

  OSLibDosFreeMem(pMapView);

  globalviewMutex.enter();
  Win32MemMapView *view = mapviews;

  if(view == this) {
	mapviews = next;
  }
  else {
  	while(view->next) {
		if(view->next == this)
			break;
		view = view->next;
	}
	if(view->next) {
		view->next = next;
	}
	else	dprintf(("Win32MemMapView::~Win32MemMapView: map not found!! (%x)", this));
  }
  globalviewMutex.leave();
}
//******************************************************************************
//******************************************************************************
void Win32MemMapView::deleteView(Win32MemMap *map)
{
  globalviewMutex.enter();
  Win32MemMapView *view = mapviews;

  if(view != NULL) {
  	while(view) {
		if(view->getParentMap() == map)
		{
			globalviewMutex.leave();
			delete view;
			return;
		}
		view = view->next;
	}
  }
  globalviewMutex.leave();
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMapView::findMapByView(ULONG address, ULONG *offset,
                                            ULONG accessType,
                                            Win32MemMapView **pView)
{
  globalviewMutex.enter();
  Win32MemMapView *view = mapviews;

  *offset = 0;

  if(view != NULL) {
  	while(view && (ULONG)view->getViewAddr() <= address) {
		if((ULONG)view->getViewAddr() <= address &&
                   (ULONG)view->getViewAddr() + view->getSize() >= address &&
                   view->getAccessFlags() >= accessType)
		{
			*offset = view->getOffset() + (address - (ULONG)view->getViewAddr());
			goto success;
		}
		view = view->next;
	}
	//failure if we get here
	view = NULL;
  }
success:
  globalviewMutex.leave();
  if(pView) *pView = view;
  return (view) ? view->getParentMap() : NULL;
}
//******************************************************************************
//******************************************************************************
Win32MemMapView *Win32MemMapView::findView(LPVOID address)
{
  Win32MemMapView *view = mapviews;

  if(view != NULL) {
  	while(view) {
		if(view->getViewAddr() == address)
		{
			break;
		}
		view = view->next;
	}
  }
  return view;
}
//******************************************************************************
//******************************************************************************
Win32MemMapView *Win32MemMapView::mapviews = NULL;

