/* $Id: mmapview.cpp,v 1.2 2003-12-12 11:09:38 sandervl Exp $ */

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
#include <win\virtual.h>
#include <odincrt.h>
#include <handlemanager.h>
#include "mmap.h"
#include "oslibdos.h"
#include "oslibmem.h"
#include <winimagepeldr.h>
#include <custombuild.h> 

#define DBG_LOCALLOG    DBG_mmapview
#include "dbglocal.h"

Win32MemMapView *Win32MemMapView::mapviews = NULL;

//******************************************************************************
// Class Win32MemMapView
//
// Memory map view
//
// View parent = memory map that contains the original memory map
// View owner  = duplicate memory map that created this view (can be NULL)
//
//******************************************************************************
Win32MemMapView::Win32MemMapView(Win32MemMap *map, ULONG offset, ULONG size, 
                                 ULONG fdwAccess, Win32MemMap *owner)
{
 LPVOID           viewaddr = (LPVOID)((ULONG)map->getMappingAddr()+offset);
 ULONG            accessAttr = 0;
 Win32MemMapView *tmpview  = mapviews;

    errorState      = 0;
    mParentMap      = map;
    mOwnerMap       = NULL;
    pCOWBitmap      = NULL;
    mSize           = size;
    mOffset         = offset;
    mProcessId      = GetCurrentProcessId();
    pShareViewAddr  = NULL;

    switch(fdwAccess) {
    case FILE_MAP_READ:
        accessAttr  = PAG_READ;
        mfAccess    = MEMMAP_ACCESS_READ;
        break;
    case FILE_MAP_ALL_ACCESS:
    case FILE_MAP_WRITE:
    case FILE_MAP_WRITE|FILE_MAP_READ:
        accessAttr  = (PAG_READ|PAG_WRITE);
        mfAccess    = MEMMAP_ACCESS_READ | MEMMAP_ACCESS_WRITE;
        break;
    case FILE_MAP_COPY:
        accessAttr  = (PAG_READ|PAG_WRITE);
        mfAccess    = MEMMAP_ACCESS_READ | MEMMAP_ACCESS_WRITE | MEMMAP_ACCESS_COPYONWRITE;
        break;
    }
    //Named file mappings from other processes are always shared;
    //map into our address space
    if(map->getMemName() != NULL && map->getProcessId() != mProcessId)
    {
        //shared memory map, so map it into our address space
        if(OSLibDosGetNamedSharedMem((LPVOID *)&viewaddr, map->getMemName()) != OSLIB_NOERROR) 
        {
            dprintf(("new OSLibDosGetNamedSharedMem FAILED"));
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            errorState = 1;
            return;
        }
        pShareViewAddr = viewaddr;
        viewaddr += mOffset;
    }

    //view == memory mapping for executable images (only used internally)
    if(map->getImage()) {
        pMapView  = map->getMappingAddr();
        pMapView += mOffset;
    }
    else {
        if(mfAccess & MEMMAP_ACCESS_COPYONWRITE) 
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            errorState = 1;
            return;
        }
        else 
        if(OSLibDosAliasMem(viewaddr, size, &pMapView, accessAttr) != OSLIB_NOERROR) {
            dprintf(("new OSLibDosAliasMem FAILED"));
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            errorState = 1;
            return;
        }
    }

    dprintf(("Win32MemMapView::Win32MemMapView: created %x (alias for %x), size %d", pMapView, viewaddr, size));
    mParentMap->AddRef();
    mParentMap->AddView();

    DosEnterCriticalSection(&globalmapcritsect);
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
    DosLeaveCriticalSection(&globalmapcritsect);

    if(owner) {
        mOwnerMap = owner;
        mOwnerMap->AddRef();
        mOwnerMap->AddView();
    }
}
//******************************************************************************
//******************************************************************************
Win32MemMapView::~Win32MemMapView()
{
    if(errorState != 0)
        return;

    dprintf(("Win32MemMapView dtor: deleting view %x %x", mOffset, mSize));

    if(mfAccess & MEMMAP_ACCESS_WRITE)
        mParentMap->flushView(MMAP_FLUSHVIEW_ALL, mOffset, mSize);

    //Don't free memory for executable image map views (only used internally)
    if(!mParentMap->getImage())
        OSLibDosFreeMem(pMapView);

    if(pShareViewAddr) {
        OSLibDosFreeMem(pShareViewAddr);
    }
    if(pCOWBitmap) free(pCOWBitmap);

    DosEnterCriticalSection(&globalmapcritsect);
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
        else    dprintf(("Win32MemMapView::~Win32MemMapView: map not found!! (%x)", this));
    }
    DosLeaveCriticalSection(&globalmapcritsect);

    mParentMap->RemoveView();
    mParentMap->Release();
    if(mOwnerMap) {
        mOwnerMap->RemoveView();
        mOwnerMap->Release();
    }
}
//******************************************************************************
// Win32MemMapView::markCOWPages
//
// Mark pages as private in the COW page bitmap
//
// Parameters:
//
//   int startpage              - start page
//   int nrpages                - number of pages
//
//
//******************************************************************************
void Win32MemMapView::markCOWPages(int startpage, int nrpages)
{
    int viewpagestart, nrviewpages;

    if(pCOWBitmap == NULL) {
        //not a COW view; ignore
        return;
    }
    //check if this page is part of our view
    viewpagestart = mOffset >> PAGE_SHIFT;
    nrviewpages = mSize >> PAGE_SHIFT;
    if(mSize & 0xFFF) 
        nrviewpages++;

    if(startpage < viewpagestart || startpage >= viewpagestart+nrviewpages) {
        return; //outside this view
    }
    if(startpage + nrpages > viewpagestart + nrviewpages) {  
        nrpages -= ((startpage + nrpages) - (viewpagestart + nrviewpages));
    }

    for(int i=startpage;i<startpage+nrpages;i++) {
        set_bit(i, pCOWBitmap);
    }
}
//******************************************************************************
// Win32MemMapView::changePageFlags
//
// Change the protection flags of our alias. Called when a range of pages has
// been committed.
//
// Parameters:
//
//   ULONG offset               - offset in memory map (page aligned!)
//   ULONG size                 - size of committed page range
//   PAGEVIEW flags             - page flags
//       PAGEVIEW_READONLY      -> set page flags to readonly
//       PAGEVIEW_VIEW          -> set page flags to view default
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
//******************************************************************************
BOOL Win32MemMapView::changePageFlags(ULONG offset, ULONG size, PAGEVIEW flags)
{
    ULONG accessAttr = 0, rc;

    //offset must be page aligned
    if(offset & 0xFFF) {
        DebugInt3();
        return FALSE;
    }

    if( ( (mfAccess & MEMMAP_ACCESS_COPYONWRITE) && (flags != PAGEVIEW_GUARD)  ) || 
        ( (flags == PAGEVIEW_GUARD) && !(mfAccess & MEMMAP_ACCESS_COPYONWRITE) ) ) 
    {
        //PAGEVIEW_VIEW/READONLY does not apply to COW views
        return TRUE; 
    }
    if(mOffset + mSize <= offset || mOffset >= offset + size) {
        return TRUE; //not part of this view
    }
    if(offset < mOffset) {
        size  -= mOffset - offset;
        offset = mOffset;
    }
    if(mOffset + mSize < offset + size) {
        size -= ((offset + size) - (mOffset + mSize));
    }

    if(flags == PAGEVIEW_READONLY) {
        accessAttr = PAG_READ;
    }
    else
    {//use view attributes
        if(mfAccess & MEMMAP_ACCESS_READ) {
            accessAttr |= PAG_READ;
        }
        if(mfAccess & MEMMAP_ACCESS_WRITE) {
            accessAttr |= PAG_WRITE;
        }
    }

    {
        rc = OSLibDosSetMem((char *)pMapView+(offset - mOffset), size, accessAttr);
        if(rc) {
            dprintf(("Win32MemMapView::changePageFlags: OSLibDosSetMem %x %x %x failed with %d", (char *)pMapView+(offset - mOffset), size, accessAttr, rc));
            return FALSE;
        }
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
int Win32MemMapView::findViews(Win32MemMap *map, int nrViews, 
                               Win32MemMapView *viewarray[])
{
  int i=0;

  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMapView *view = mapviews, *nextview;

  if(view != NULL) 
  {
      while(view && i < nrViews) 
      {
          if(view->getParentMap() == map)
          {
              viewarray[i] = view;
              i++;
          }
          view = view->next;
      }
  }
  DosLeaveCriticalSection(&globalmapcritsect);
  return i;
}
//******************************************************************************
//******************************************************************************
void Win32MemMapView::deleteViews(Win32MemMap *map)
{
  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMapView *view = mapviews, *nextview;

  if(view != NULL) 
  {
      while(view) 
      {
          nextview = view->next;
          if(view->getParentMap() == map)
          {
              DosLeaveCriticalSection(&globalmapcritsect);
              delete view;
              DosEnterCriticalSection(&globalmapcritsect);
          }
          view = nextview;
      }
  }
  DosLeaveCriticalSection(&globalmapcritsect);
}
//******************************************************************************
//******************************************************************************
// Win32MemMap::findMapByView
//
// Find the map of the view that contains the specified starting address 
// and has the specified access type
//
// Parameters:
//
//   ULONG address              - view address
//   ULONG *offset              - address of ULONG that receives the offset
//                                in the returned memory map
//   ULONG accessType           - access type:
//                                MEMMAP_ACCESS_READ
//                                MEMMAP_ACCESS_WRITE
//                                MEMMAP_ACCESS_EXECUTE
//
// Returns:
//   <> NULL                    - success, address of parent map object
//   NULL                       - failure
//
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMapView::findMapByView(ULONG address,
                                            ULONG *offset,
                                            ULONG accessType)
{
  Win32MemMap *map = NULL;
  ULONG ulOffset;

  if(mapviews == NULL) return NULL;

  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMapView *view = mapviews;
  ULONG ulViewAddr;

  if(!offset)  offset = &ulOffset;

  *offset = 0;

  if(view != NULL)
  {
    do
    {
      ulViewAddr = (ULONG)view->getViewAddr();

      // if ulViewAddr is > address, we've exhausted
      // the sorted list already and can abort search.
      if(ulViewAddr <= address)
      {
        if(ulViewAddr + view->getSize() > address &&
           view->getAccessFlags() >= accessType)
        {
          *offset = view->getOffset() + (address - ulViewAddr);
          goto success;
        }

        // Not found yet, continue search with next map
        view = view->next;
      }
      else
      {
          // list is exhausted, abort loop
          view = NULL;
      }
    }
    while(view);

    //failure if we get here
    view = NULL;
  }
success:
#ifdef DEBUG
  if(view && !view->getParentMap()->isImageMap())
      dprintf(("findMapByView %x %x -> %x off %x",
               address,
               accessType,
               view->getViewAddr(),
               *offset));
#endif

  if(view) {
      //first look at the owner (duplicate map), then the real parent
      map = view->getOwnerMap();
      if(!map) map = view->getParentMap();

      if(map) map->AddRef();
  }

  DosLeaveCriticalSection(&globalmapcritsect);

  return map;
}
//******************************************************************************
// Win32MemMap::findView
//
// Find the view that contains the specified starting address
//
// Parameters:
//
//   LPVOID address             - view address
//
// Returns:
//   <> NULL                    - success, address view object
//   NULL                       - failure
//
//******************************************************************************
Win32MemMapView *Win32MemMapView::findView(ULONG address)
{
  ULONG ulViewAddr;

  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMapView *view = mapviews;

  if(view != NULL) {
    while(view) {
        ulViewAddr = (ULONG)view->getViewAddr();
        if(ulViewAddr <= address && ulViewAddr + view->getSize() > address)
        {
            break;
        }
        view = view->next;
    }
  }
  DosLeaveCriticalSection(&globalmapcritsect);
  return view;
}
//******************************************************************************
//******************************************************************************
