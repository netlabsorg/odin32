/* $Id: mmap.cpp,v 1.72 2004-04-06 14:20:20 sandervl Exp $ */

/*
 * Win32 Memory mapped file & view classes
 *
 * Copyright 1999-2003 Sander van Leeuwen (sandervl@xs4all.nl)
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
 * TODO: Memory mapped files with views that extend the file (not 100% correct now)
 * TODO: Suspend all threads when a page is committed (possible that another thread
 *       accesses the same memory before the page is read from disk
 * TODO: File maps for new files (must select an initial size)!
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include <win/virtual.h>
#include <odincrt.h>
#include <handlemanager.h>
#include "mmap.h"
#include "oslibdos.h"
#include "oslibmem.h"
#include "winimagepeldr.h"
#include <custombuild.h>
#include "asmutil.h"

#define DBG_LOCALLOG    DBG_mmap
#include "dbglocal.h"

static LPCSTR pszMMapSemName = MEMMAP_CRITSECTION_NAME;

//******************************************************************************
//******************************************************************************
void WIN32API SetCustomMMapSemName(LPSTR pszSemName)
{
    pszMMapSemName = pszSemName;
}
//******************************************************************************
//******************************************************************************
void InitializeMemMaps()
{
    if(globalmapcritsect.hevLock == 0) {
         dprintf(("InitializeMemMaps -> create shared critical section"));
    }
    else {
         dprintf(("InitializeMemMaps -> access shared critical section"));
    }
    DosAccessCriticalSection(&globalmapcritsect, pszMMapSemName);
}
//******************************************************************************
//******************************************************************************
void FinalizeMemMaps()
{
    DosDeleteCriticalSection(&globalmapcritsect);
}
//******************************************************************************
//******************************************************************************
Win32MemMap::Win32MemMap(HANDLE hfile, ULONG size, ULONG fdwProtect, LPSTR lpszName)
               : nrMappings(0), pMapping(NULL), mMapAccess(0), referenced(0),
                 image(0), pWriteBitmap(NULL), lpszFileName(NULL)
{
    DosEnterCriticalSection(&globalmapcritsect);
    next    = memmaps;
    memmaps = this;
    DosLeaveCriticalSection(&globalmapcritsect);

    hMemFile = hOrgMemFile = hfile;

    mSize      = size;
    mProtFlags = fdwProtect;

    mProcessId  = GetCurrentProcessId();

    if(lpszName) {
        lpszMapName = (char *)_smalloc(strlen(lpszName)+1);
        strcpy(lpszMapName, lpszName);
    }
    else  lpszMapName = NULL;
    AddRef();
}
//******************************************************************************
//Map constructor used for executable image maps (only used internally)
//******************************************************************************
Win32MemMap::Win32MemMap(Win32PeLdrImage *pImage, ULONG baseAddress, ULONG size)
               : nrMappings(0), pMapping(NULL), mMapAccess(0), referenced(0),
                 image(0), pWriteBitmap(NULL), lpszFileName(NULL)
{
    DosEnterCriticalSection(&globalmapcritsect);
    next    = memmaps;
    memmaps = this;
    DosLeaveCriticalSection(&globalmapcritsect);

    hMemFile = hOrgMemFile = -1;

    mSize      = size;
    mProtFlags = PAGE_READWRITE;
    mProcessId = GetCurrentProcessId();

    pMapping   = (LPVOID)baseAddress;

    image      = pImage;
    lpszMapName= NULL;
    AddRef();
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMap::Init(DWORD aMSize)
{
    mapMutex.enter();
    if(hMemFile != -1)
    {
        lpszFileName = (char *)_smalloc(MMAP_MAX_FILENAME_LENGTH);
        if(HMGetFileNameFromHandle(hMemFile, lpszFileName, MMAP_MAX_FILENAME_LENGTH) == FALSE) {
            return FALSE;
        }
#if 0
        if(DuplicateHandle(GetCurrentProcess(), hMemFile, GetCurrentProcess(),
                           &hMemFile, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE)
#else
        if(HMDuplicateHandle(GetCurrentProcess(), hMemFile, GetCurrentProcess(),
                           &hMemFile, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE)
#endif
        {
            dprintf(("Win32MemMap::Init: DuplicateHandle failed!"));
            goto fail;
        }
        mSize = SetFilePointer(hMemFile, 0, NULL, FILE_BEGIN);
        mSize = SetFilePointer(hMemFile, 0, NULL, FILE_END);
        if(mSize == -1) {
            dprintf(("Win32MemMap::init: SetFilePointer failed to set pos end"));
            goto fail;
        }
        if (mSize < aMSize)
        {
            dprintf(("Win32MemMap::init: file size %d, memory map size %d", mSize, aMSize));
            //Froloff: Need to check if exist the possibility of file to memory
            //         mapping not from the beginning of file
            mSize = SetFilePointer(hMemFile, aMSize, NULL, FILE_BEGIN);
            // Commit filesize changes onto disk
            SetEndOfFile(hMemFile);
        }
#if 0
        //SvL: Temporary limitation of size (Warp Server Advanced doesn't allow
        //     one to reserve more than 450 MB (unless you override the virtual
        //     memory max limit) of continuous memory; (Warp 4 much less))
        if(mSize > 64*1024*1024) {
            mSize = 64*1024*1024;
        }
#endif
    }
    // Allocate the memory for the map right now
    if(allocateMap() == FALSE) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto fail;
    }

    dprintf(("CreateFileMappingA for file %x, prot %x size %d, name %s", hMemFile, mProtFlags, mSize, lpszMapName));
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
    Win32MemMapView::deleteViews(this); //delete all views of our memory mapped file

    dprintf(("Win32MemMap dtor: deleting map %x %x", pMapping, mSize));

    mapMutex.enter();
    if(lpszMapName) {
        free(lpszMapName);
    }
    if(lpszFileName) {
        free(lpszFileName);
    }
    if(pMapping && !image) {
        dprintf(("Free map memory"));
        if(lpszMapName) {
                OSLibDosFreeMem(pMapping);
        }
        else    VirtualFree(pMapping, 0, MEM_RELEASE);

        pMapping = NULL;
    }
    if(hMemFile != -1) {
        dprintf(("Win32MemMap dtor: closing memory file %x", hMemFile));
        CloseHandle(hMemFile);
        hMemFile = -1;
    }
    if(pWriteBitmap) free(pWriteBitmap);

    mapMutex.leave();

    DosEnterCriticalSection(&globalmapcritsect);
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
        else dprintf(("Win32MemMap::~Win32MemMap: map not found!! (%x)", this));
    }
    DosLeaveCriticalSection(&globalmapcritsect);
}
//******************************************************************************
//******************************************************************************
int Win32MemMap::Release()
{
    dprintf(("Win32MemMap::Release %s (%d)", lpszMapName, referenced-1));
    --referenced;
    if(referenced == 0) {
        delete this;
        return 0;
    }
    return referenced;
}
//******************************************************************************
// Win32MemMap::commitRange
//
// Commit a range of pages
//
// Parameters:
//
//   ULONG ulFaultAddr          - exception address
//   ULONG ulOffset             - offset in memory map
//   BOOL  fWriteAccess         - TRUE  -> write exception
//                                FALSE -> read exception
//   int   nrpages              - number of pages
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
//******************************************************************************
BOOL Win32MemMap::commitRange(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess, int nrpages)
{
    LPVOID lpPageFaultAddr = (LPVOID)((ULONG)pMapping + offset);
    DWORD  pageAddr        = (DWORD)lpPageFaultAddr & ~0xFFF;

    dprintf(("Win32MemMap::commitRange %x (faultaddr %x)", pageAddr, lpPageFaultAddr));

    if(fWriteAccess)
    {//writes are handled on a per-page basis
        for(int i=0;i<nrpages;i++)
        {
            if(commitPage(ulFaultAddr, offset, TRUE, 1) == FALSE) {
                dprintf(("Win32MemMap::commit: commitPage failed!!"));
                return FALSE;
            }
            ulFaultAddr += PAGE_SIZE;
            offset      += PAGE_SIZE;
        }
        return TRUE;
    }
    else    return commitPage(ulFaultAddr, offset, FALSE, nrpages);
}
//******************************************************************************
// Win32MemMap::commitPage
//
// Handle a pagefault for a memory map
//
// Parameters:
//
//   ULONG ulFaultAddr          - exception address
//   ULONG ulOffset             - offset in memory map
//   BOOL  fWriteAccess         - TRUE  -> write exception
//                                FALSE -> read exception
//   int   nrpages              - number of pages
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
// NOTE:
//   We handle only one pages for write access!
//
// REMARKS:
//   We determine whether a page has been modified by checking it's protection flags
//   If the write flag is set, this means commitPage had to enable this due to a pagefault
//   (all pages are readonly until the app tries to write to it)
//******************************************************************************
BOOL Win32MemMap::commitPage(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess, int nrpages)
{
 MEMORY_BASIC_INFORMATION memInfo;
 LPVOID lpPageFaultAddr = (LPVOID)((ULONG)pMapping + offset);
 DWORD pageAddr         = (DWORD)lpPageFaultAddr & ~0xFFF;
 DWORD oldProt, newProt, nrBytesRead;
 int i;

//  mapMutex.enter();

  if(image) {
      return image->commitPage(pageAddr, fWriteAccess);
  }

  if(fWriteAccess && (mProtFlags & PAGE_WRITECOPY))
  {//this is a COW map, call commitGuardPage to handle write faults
      return commitGuardPage(ulFaultAddr, offset, fWriteAccess);
  }

  dprintf(("Win32MemMap::commitPage %x (faultaddr %x)", pageAddr, lpPageFaultAddr));

  //align at page boundary
  offset &= ~0xFFF;

  //If it's a write access violation and the view is readonly, then fail
  if(fWriteAccess) {
      Win32MemMapView *view = Win32MemMapView::findView(ulFaultAddr);
      if(view) {
          if(!(view->getAccessFlags() & MEMMAP_ACCESS_WRITE)) {
              dprintf(("Write access for a readonly view!!"));
              return FALSE;
          }
      }
      else {
          DebugInt3(); //can't happen
          return FALSE;
      }
  }

  int faultsize = nrpages*PAGE_SIZE;

  if(fWriteAccess)
  {//write access needs special care, so do that on a per page basis
      dprintf(("Write access -> handle only one page"));
      faultsize = PAGE_SIZE;
  }

  offset = pageAddr - (ULONG)pMapping;
  if(offset + faultsize > mSize) {
      faultsize = mSize - offset;
  }

  while(faultsize)
  {
        if(VirtualQuery((LPSTR)pageAddr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
            dprintf(("Win32MemMap::commitPage: VirtualQuery (%x,%x) failed for %x", pageAddr, nrpages*PAGE_SIZE));
            goto fail;
        }
        memInfo.RegionSize = std::min<unsigned int>(memInfo.RegionSize, faultsize);
        //Only changes the state of the pages with the same attribute flags
        //(returned in memInfo.RegionSize)
        //If it's smaller than the mNrPages, it simply means one or more of the
        //other pages have already been committed
        if(!(memInfo.State & MEM_COMMIT))
        {
            if(VirtualAlloc((LPVOID)pageAddr, memInfo.RegionSize, MEM_COMMIT, PAGE_READWRITE) == FALSE) {
                goto fail;
            }

            //Part of the memory map has been committed, so now we can change
            //the protection flags of the aliases (DosSetMem fails for reserved pages)
            updateViewPages(offset, memInfo.RegionSize, (fWriteAccess) ? PAGEVIEW_VIEW : PAGEVIEW_READONLY);

            if(hMemFile != -1)
            {//now read the page(s) from disk
                DWORD size;

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
            }
            //We set the protection flags to PAGE_READONLY, unless this pagefault
            //was due to a write access
            //This way we can track dirty pages which need to be flushed to
            //disk when FlushViewOfFile is called or the map is closed.
            if(!fWriteAccess)
            {
                if(VirtualProtect((LPVOID)pageAddr, memInfo.RegionSize, PAGE_READONLY, &oldProt) == FALSE) {
                    dprintf(("VirtualProtect %x %x PAGE_READWRITE failed with %d!!", pageAddr, memInfo.RegionSize, GetLastError()));
                    goto fail;
                }
            }
            else
            {//make these pages as dirty
                ULONG startPage  = (pageAddr - (ULONG)pMapping) >> PAGE_SHIFT;
                ULONG nrPages    = memInfo.RegionSize >> PAGE_SHIFT;

                if(memInfo.RegionSize & 0xFFF)
                    nrPages++;

                dprintf(("Mark %d page(s) starting at %x as dirty", nrPages, pageAddr));
                markDirtyPages(startPage, nrPages);

                //Write access means that the next time the corresponding COW page
                //is touched, we need to reload it. So set the GUARD flags.
                updateViewPages(offset, memInfo.RegionSize, PAGEVIEW_GUARD);
            }
        }
        else
        if(fWriteAccess)
        {
            //mark these pages as dirty
            ULONG startPage  = (pageAddr - (ULONG)pMapping) >> PAGE_SHIFT;
            ULONG nrPages    = memInfo.RegionSize >> PAGE_SHIFT;

            if(memInfo.RegionSize & 0xFFF)
                nrPages++;

            dprintf(("Mark %d page(s) starting at %x as dirty", nrPages, pageAddr));
            markDirtyPages(startPage, nrPages);

            //and turn on a write access
            if(VirtualProtect((LPVOID)pageAddr, memInfo.RegionSize, PAGE_READWRITE, &oldProt) == FALSE) {
                dprintf(("VirtualProtect %x %x PAGE_READWRITE failed with %d!!", pageAddr, memInfo.RegionSize, GetLastError()));
                goto fail;
            }
            //Now change all the aliased pages according to their view protection flags
            updateViewPages(offset, memInfo.RegionSize, PAGEVIEW_VIEW);

            //Write access means that the next time the corresponding COW page
            //is touched, we need to reload it. So set the GUARD flags.
            updateViewPages(offset, memInfo.RegionSize, PAGEVIEW_GUARD);
        }
        faultsize -= memInfo.RegionSize;
        pageAddr  += memInfo.RegionSize;
  }

//  mapMutex.leave();
  return TRUE;
fail:
//  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
// Win32MemMap::commitGuardPage
//
// Handle a guard page exception for a copy-on-write view (one page only)
//
// Parameters:
//
//   ULONG ulFaultAddr          - exception address
//   ULONG ulOffset             - offset in memory map
//   BOOL  fWriteAccess         - TRUE  -> write exception
//                                FALSE -> read exception
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
//******************************************************************************
BOOL Win32MemMap::commitGuardPage(ULONG ulFaultAddr, ULONG ulOffset, BOOL fWriteAccess)
{
   MEMORY_BASIC_INFORMATION memInfo;
   BOOL   ret;
   DWORD  pageAddr = ulFaultAddr & ~0xFFF;
   DWORD  dwNewProt, dwOldProt;

   dprintf(("Win32MemMap::commitGuardPage %x (faultaddr %x)", pageAddr, ulFaultAddr));

   //align at page boundary
   ulOffset &= ~0xFFF;

   //commit a single page in the original mapping (pretend read access)
   ret = commitPage(ulFaultAddr, ulOffset, FALSE, 1);
   if(ret == FALSE) {
       DebugInt3();
       goto fail;
   }
   //clear PAGE_GUARD flag, since this page must now be made accessible
   dwNewProt = mProtFlags & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY);
   if(dwNewProt & PAGE_WRITECOPY) {
       dwNewProt |= PAGE_GUARD;
   }
   dwNewProt &= ~PAGE_GUARD;
   if(VirtualProtect((LPVOID)pageAddr, PAGE_SIZE, dwNewProt, &dwOldProt) == FALSE) {
       dprintf(("Win32MemMap::commitGuardPage: VirtualProtect %x 0x1000 %x failed!!", pageAddr, dwNewProt));
       goto fail;
   }
   //copy the data from the original mapping to the COW page
   memcpy((LPVOID)pageAddr, (char *)pMapping+ulOffset, PAGE_SIZE);

   if(fWriteAccess)
   {//copy on write; mark pages as private
        DWORD startpage = (ulOffset) >> PAGE_SHIFT;

        dprintf(("Win32MemMap::commitGuardPage: write access -> mark page as private"));

        Win32MemMapView *view = Win32MemMapView::findView(ulFaultAddr);
        if(view)
        {
            view->markCOWPages(startpage, 1);
        }
        else DebugInt3(); //oh, oh
   }
   else
   {//read access; must set the map + all views to READONLY to track write access

       dprintf(("Win32MemMap::commitGuardPage: read access -> set page as READONLY in map + all views"));

       //first the memory map page
       if(VirtualProtect((LPVOID)pageAddr, PAGE_SIZE, PAGE_READONLY, &dwOldProt) == FALSE) {
           dprintf(("Win32MemMap::commitGuardPage: VirtualProtect %x 0x1000 %x failed!!", pageAddr, dwNewProt));
           goto fail;
       }
       //now for any views that exist
       updateViewPages(ulOffset, PAGE_SIZE, PAGEVIEW_READONLY);
   }

   return TRUE;
fail:
   return FALSE;
}
//******************************************************************************
// Win32MemMap::updateViewPages
//
// Update the page flags of all views
//
// Parameters:
//
//   ULONG offset               - offset in memory map
//   ULONG size                 - range size
//   PAGEVIEW flags             - page flags
//       PAGEVIEW_READONLY      -> set page flags to readonly
//       PAGEVIEW_VIEW          -> set page flags to view default
//       PAGEVIEW_GUARD         -> set page flags of COW view to GUARD
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
//******************************************************************************
BOOL Win32MemMap::updateViewPages(ULONG offset, ULONG size, PAGEVIEW flags)
{
    Win32MemMapView **views = (Win32MemMapView **)alloca(sizeof(Win32MemMapView*)*nrMappings);
    int localmaps;

    if(views)
    {
        localmaps = Win32MemMapView::findViews(this, nrMappings, views);
        if(localmaps <= nrMappings)
        {
            for(int i=0;i<localmaps;i++)
            {
                views[i]->changePageFlags(offset, size, flags);
            }
        }
        else {
            dprintf(("unexpected number of views %d vs %d", localmaps, nrMappings));
            DebugInt3(); //oh, oh
        }
    }
    return TRUE;
}
//******************************************************************************
// Win32MemMap::invalidatePages
//
// Invalidate map pages. (called by WriteFile)
//
// Parameters:
//
//   ULONG offset               - offset in memory map
//   ULONG size                 - invalid range size
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
//******************************************************************************
BOOL Win32MemMap::invalidatePages(ULONG offset, ULONG size)
{
    ULONG diff = offset & 0xFFF;
    BOOL ret;

    offset &= ~0xFFF;
    size   += diff;

    dprintf(("Win32MemMap::invalidatePages %x %x", offset, size));
    ret = VirtualFree((LPSTR)pMapping + offset, size, MEM_DECOMMIT);
    if(ret == FALSE) {
        dprintf(("ERROR: Win32MemMap::invalidatePages: VirtualFree failed!!"));
    }
    //invalidate all shared COW pages too by setting the GUARD flag
    //(which forces a resync the next time the app touches them)
    return updateViewPages(offset, size, PAGEVIEW_GUARD);
}
//******************************************************************************
// Win32MemMap::allocateMap
//
// Allocate memory for the map if not yet already done.
//
// Returns:
//   FALSE                      - success
//   TRUE                       - failure
//
//******************************************************************************
BOOL Win32MemMap::allocateMap()
{
    ULONG fAlloc   = 0;

    fAlloc = MEM_RESERVE;

    //Memory has already been allocated for executable image maps (only used internally)
    if(!pMapping && nrMappings == 0)
    {//if not mapped, reserve/commit entire view
        //SvL: Always read/write access or else ReadFile will crash once we
        //     start committing pages.
        //     This is most likely an OS/2 bug and doesn't happen in Aurora
        //     when allocating memory with the PAG_ANY bit set. (without this
        //     flag it will also crash)
        //NOTE: If this is ever changed, then we must update setProtFlags!!!!

        //All named file mappings are shared (files & memory only)
        if(lpszMapName) {
            pMapping = VirtualAllocShared(mSize, fAlloc, PAGE_READWRITE, lpszMapName);
        }
        else {
            pMapping = VirtualAlloc(0, mSize, fAlloc, PAGE_READWRITE);
        }
        if(pMapping == NULL) {
            dprintf(("Win32MemMap::mapViewOfFile: VirtualAlloc %x %x failed!", mSize, fAlloc));
            goto fail;
        }
        //Windows NT seems to commit memory for memory maps, regardsless of the SEC_COMMIT flag
        if((hMemFile == -1 && !image)) {//commit memory
            VirtualAlloc(pMapping, mSize, MEM_COMMIT, PAGE_READWRITE);
        }

        DWORD nrPages = mSize >> PAGE_SHIFT;
        if(mSize & 0xFFF)
           nrPages++;

        if(hMemFile != -1 && (mProtFlags & SEC_COMMIT)) {
            commitPage((ULONG)pMapping, 0, FALSE, nrPages);
        }
        //Allocate bitmap for all pages to keep track of write access (file maps only)
        //Necessary for FlushViewOfFile.
        if(hMemFile != -1) {
            int sizebitmap = nrPages/8 + 1;

            pWriteBitmap = (char *)_smalloc(sizebitmap);
            if(pWriteBitmap == NULL) {
                DebugInt3();
                goto fail;
            }
            memset(pWriteBitmap, 0, sizebitmap);
        }
    }
    return TRUE;

fail:
    return FALSE;
}
//******************************************************************************
// Win32MemMap::mapViewOfFile
//
// Map the view identified by addr
//
// Parameters:
//
//   ULONG size                 - size of view
//   ULONG offset               - offset in memory map
//   ULONG fdwAccess            - access flags
//                                FILE_MAP_WRITE, FILE_MAP_READ, FILE_MAP_COPY
//                                FILE_MAP_ALL_ACCESS
//
//
// Returns:
//   <>NULL                     - success, view address
//   NULL                       - failure
//
//******************************************************************************
LPVOID Win32MemMap::mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess)
{
    DWORD processId = GetCurrentProcessId();

    mapMutex.enter();
    ULONG memFlags = (mProtFlags & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY));
    Win32MemMapView *mapview;

    //@@@PH: if(fdwAccess & ~(FILE_MAP_WRITE|FILE_MAP_READ|FILE_MAP_COPY))
    // Docs say FILE_MAP_ALL_ACCESS is same as FILE_MAP_WRITE. Doesn't match reality though.
    if(fdwAccess & ~FILE_MAP_ALL_ACCESS)
        goto parmfail;
    if((fdwAccess & FILE_MAP_WRITE) && !(mProtFlags & PAGE_READWRITE))
        goto parmfail;
    // CreateFileMapping docs say that PAGE_WRITECOPY is equivalent to PAGE_READONLY and thus
    // is okay for FILE_MAP_READ too
    if((fdwAccess & FILE_MAP_READ) && !(mProtFlags & (PAGE_READWRITE|PAGE_READONLY|PAGE_WRITECOPY)))
        goto parmfail;

    if (fdwAccess != FILE_MAP_ALL_ACCESS)
        if((fdwAccess & FILE_MAP_COPY) && !(mProtFlags & PAGE_WRITECOPY))
             goto parmfail;

    if(offset+size > mSize && (!(fdwAccess & FILE_MAP_WRITE) || hMemFile == -1))
        goto parmfail;

    //SvL: TODO: Doesn't work for multiple views
    if(offset+size > mSize) {
        mSize = offset+size;
    }

    mapview = new Win32MemMapView(this, offset, (size == 0) ? (mSize - offset) : size, fdwAccess);
    if(mapview == NULL) {
        goto fail;
    }
    if(mapview->everythingOk() == FALSE) {
        dprintf(("Win32MemMap::mapViewOfFile: !mapview->everythingOk"));
        delete mapview;
        goto fail;
    }
    mapMutex.leave();
    SetLastError(ERROR_SUCCESS);
    return mapview->getViewAddr();

parmfail:
    dprintf(("Win32MemMap::mapViewOfFile: invalid parameter (ERROR_ACCESS_DENIED)"));
    //NT4 SP6 returns ERROR_ACCESS_DENIED for most invalid parameters
    SetLastError(ERROR_ACCESS_DENIED);
fail:
    mapMutex.leave();
    return 0;
}
//******************************************************************************
// Win32MemMap::unmapViewOfFile
//
// Unmap the view identified by addr
//
// Parameters:
//
//   LPVOID addr                - view address; doesn't need to be the address
//                                returned by MapViewOfFile(Ex) (as MSDN clearly says);
//                                can be any address within the view range
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
//******************************************************************************
BOOL Win32MemMap::unmapViewOfFile(LPVOID addr)
{
    Win32MemMapView *view;

    dprintf(("Win32MemMap::unmapViewOfFile %x (nrmaps=%d)", addr, nrMappings));
    mapMutex.enter();

    if(nrMappings == 0)
        goto fail;

    view = Win32MemMapView::findView((ULONG)addr);
    if(view == NULL)
        goto fail;

    delete view;

    mapMutex.leave();

    SetLastError(ERROR_SUCCESS);
    return TRUE;
fail:
    mapMutex.leave();
    SetLastError(ERROR_INVALID_ADDRESS);
    return FALSE;
}
//******************************************************************************
//We determine whether a page has been modified by checking it's protection flags
//If the write flag is set, this means commitPage had to enable this due to a pagefault
//(all pages are readonly until the app tries to modify the contents of the page)
//
//TODO: Are apps allowed to change the protection flags of memory mapped pages?
//      I'm assuming they aren't for now.
//******************************************************************************
BOOL Win32MemMap::flushView(ULONG viewaddr, ULONG offset, ULONG cbFlush)
{
  ULONG nrBytesWritten, size, accessflags, oldProt;
  Win32MemMapView *view;
  int i;

  dprintf(("Win32MemMap::flushView: %x %x", (ULONG)pMapping+offset, cbFlush));

  if(image) //no flushing for image maps
      return TRUE;

  if(hMemFile == -1)
      goto success; //TODO: Return an error here?

  if(offset > mSize)
      goto parmfail;

  if(viewaddr != MMAP_FLUSHVIEW_ALL)
  {
      view = Win32MemMapView::findView(viewaddr);
      if(nrMappings == 0 || view == NULL) {
          DebugInt3(); //should never happen
          goto parmfail;
      }
      accessflags = view->getAccessFlags();
  }
  else {
      //force a flush to disk; only those pages marked dirty are flushed anyway
      accessflags = FILE_MAP_WRITE;
  }
  //If the view is readonly or copy on write, then the flush is ignored
  if(!(accessflags & MEMMAP_ACCESS_WRITE) || (accessflags & MEMMAP_ACCESS_COPYONWRITE))
  {
      dprintf(("Readonly or Copy-On-Write memory map -> ignore flush"));
      //this is not a failure; NT4 SP6 returns success
      goto success;
  }

  if(cbFlush == 0)
      cbFlush = mSize;

  if(offset + cbFlush > mSize) {
      cbFlush -= (offset + cbFlush - mSize);
  }

  //Check the write page bitmap for dirty pages and write them to disk
  while(cbFlush)
  {
      int startPage = offset >> PAGE_SHIFT;
      size = PAGE_SIZE;

      if(isDirtyPage(startPage))
      {
          if(size > cbFlush) {
              size = cbFlush;
          }
          dprintf(("Win32MemMap::flushView for offset %x, size %d", offset, size));

          if(SetFilePointer(hMemFile, offset, NULL, FILE_BEGIN) != offset) {
              dprintf(("Win32MemMap::flushView: SetFilePointer failed to set pos to %x", offset));
              goto fail;
          }
          if(WriteFile(hMemFile, (LPSTR)((ULONG)pMapping + offset), size, &nrBytesWritten, NULL) == FALSE) {
              dprintf(("Win32MemMap::flushView: WriteFile failed for %x", (ULONG)pMapping + offset));
              goto fail;
          }
          if(nrBytesWritten != size) {
              dprintf(("Win32MemMap::flushView: WriteFile didn't write all bytes for %x", (ULONG)pMapping + offset));
              goto fail;
          }
          clearDirtyPages(startPage, 1);

          //We've just flushed the page to disk, so we need to track future writes
          //again; Set page to readonly (first memory map, then alias(es))
          if(VirtualProtect((LPVOID)((ULONG)pMapping + offset), size, PAGE_READONLY, &oldProt) == FALSE) {
              dprintf(("VirtualProtect %x %x PAGE_READWRITE failed with %d!!", (ULONG)pMapping + offset, size, GetLastError()));
              goto fail;
          }
          updateViewPages(offset, size, PAGEVIEW_READONLY);
      }

      if(cbFlush < size)
          break;

      cbFlush -= size;
      offset  += size;
  }
success:
  SetLastError(ERROR_SUCCESS);
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
  if(lpszName == NULL)
    return NULL;

  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMap *map = memmaps;

  if(map != NULL) {
    while(map) {
        if(map->lpszMapName && !strcmp(map->lpszMapName, lpszName))
            break;
        map = map->next;
    }
  }
  if(map) map->AddRef();

  DosLeaveCriticalSection(&globalmapcritsect);
  if(!map) dprintf(("Win32MemMap::findMap: couldn't find map %s", lpszName));
  return map;
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::findMapByFile(HANDLE hFile)
{
  DWORD processId = GetCurrentProcessId();
  char  szFileName[260];

  if(hFile == -1)
    return NULL;

  if(HMGetFileNameFromHandle(hFile, szFileName, sizeof(szFileName)) == FALSE)
    return NULL;

  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMap *map = memmaps;

  if(map != NULL)
  {
    while(map) {
        //TODO: we currently don't support sharing file maps between processes
        if(map->mProcessId == processId && map->lpszFileName)
        {
            if(!strcmp(map->lpszFileName, szFileName) && map->getFileHandle() != hFile)
                break;
        }
        map = map->next;
    }
  }
  if(map) map->AddRef();
  DosLeaveCriticalSection(&globalmapcritsect);
  if(!map) dprintf2(("Win32MemMap::findMapByFile: couldn't find map with file handle %x", hFile));
  return map;
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::findMap(ULONG address)
{
  DosEnterCriticalSection(&globalmapcritsect);
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
  if(map) map->AddRef();
  DosLeaveCriticalSection(&globalmapcritsect);
  return map;
}
//******************************************************************************
//******************************************************************************
void Win32MemMap::deleteAll()
{
 Win32MemMap *map, *nextmap;
 DWORD processId = GetCurrentProcessId();

  //delete all maps created by this process
  DosEnterCriticalSection(&globalmapcritsect);

startdeleteviews:
  map = memmaps;
  while(map) {
      map->AddRef(); //make sure it doesn't get deleted

      //delete all views created by this process for this map
      Win32MemMapView::deleteViews(map);

      nextmap = map->next;

      //map->Release can delete multiple objects (duplicate memory map), so make
      //sure our nextmap pointer remains valid by increasing the refcount
      if(nextmap) nextmap->AddRef();
      map->Release();

      if(nextmap && nextmap->Release() == 0) {
          //oops, nextmap was just deleted and is no longer valid
          //can't continue from here, so let's start again
          dprintf(("oops, nextmap is invalid -> start again (1)"));
          goto startdeleteviews;
      }

      map = nextmap;
  }
startdelete:
  map = memmaps;
  while(map) {
      nextmap = map->next;
      if(map->getProcessId() == processId)
      {
          //delete map can delete multiple objects (duplicate memory map), so make
          //sure our nextmap pointer remains valid by increasing the refcount
          if(nextmap) nextmap->AddRef();
          delete map;

          if(nextmap && nextmap->Release() == 0) {
              //oops, nextmap was just deleted and is no longer valid
              //can't continue from here, so let's start again
              dprintf(("oops, nextmap is invalid -> start again (2)"));
              goto startdelete;
          }
      }
      map = nextmap;
  }
  DosLeaveCriticalSection(&globalmapcritsect);
}
//******************************************************************************
// Win32MemMapView::markDirtyPages
//
// Mark pages as dirty (changed) in the write page bitmap
//
// Parameters:
//
//   int startpage              - start page
//   int nrpages                - number of pages
//
//
//******************************************************************************
void Win32MemMap::markDirtyPages(int startpage, int nrpages)
{
    if(pWriteBitmap == NULL) return; //can be NULL for non-file mappings

    for(int i=startpage;i<startpage+nrpages;i++) {
        set_bit(i, pWriteBitmap);
    }
}
//******************************************************************************
// Win32MemMapView::clearDirtyPages
//
// Mark pages as clean in the write page bitmap
//
// Parameters:
//
//   int startpage              - start page
//   int nrpages                - number of pages
//
//
//******************************************************************************
void Win32MemMap::clearDirtyPages(int startpage, int nrpages)
{
    if(pWriteBitmap == NULL) return; //can be NULL for non-file mappings

    for(int i=startpage;i<startpage+nrpages;i++) {
        clear_bit(i, pWriteBitmap);
    }
}
//******************************************************************************
//******************************************************************************
