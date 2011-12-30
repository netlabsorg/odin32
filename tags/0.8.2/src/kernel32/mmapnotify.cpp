/* $Id: mmapnotify.cpp,v 1.1 2004-01-11 11:52:18 sandervl Exp $ */

/*
 * Win32 Memory mapped notification class
 *
 * Calls notification handler when an exception occurs in the memory range of
 * the map.
 *
 * Copyright 2003 Sander van Leeuwen (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <win/virtual.h>
#include <odincrt.h>
#include <handlemanager.h>
#include "mmap.h"
#include "oslibdos.h"
#include "oslibmem.h"
#include "winimagepeldr.h"
#include <custombuild.h>
#include <memmap.h>

#define DBG_LOCALLOG    DBG_mmapnotify
#include "dbglocal.h"

Win32MemMapNotify *Win32MemMapNotify::mapviews = NULL;

//******************************************************************************
//******************************************************************************
BOOL WIN32API MMAP_RegisterMemoryRange(PFNEXCEPTIONNOTIFY pfnNotify, LPVOID lpViewAddr, DWORD size, DWORD dwUserData)
{
   Win32MemMapNotify *map;

   dprintf(("MMAP_RegisterMemoryRange %x %x %d %x", pfnNotify, lpViewAddr, size, dwUserData));

   map = new Win32MemMapNotify(pfnNotify, lpViewAddr, size, dwUserData);
   if(map == NULL) DebugInt3();

   return (map != NULL);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MMAP_UnregisterMemoryRange(LPVOID lpViewAddr)
{
   Win32MemMapNotify *map;

   dprintf(("MMAP_UnregisterMemoryRange %x", lpViewAddr));

   map = Win32MemMapNotify::findView((ULONG)lpViewAddr);
   if(map) delete map;

   return (map != NULL);
}
//******************************************************************************

//******************************************************************************
// Class Win32MemMapNotify
//
//
//
//******************************************************************************
Win32MemMapNotify::Win32MemMapNotify(PFNEXCEPTIONNOTIFY pfnNotify, LPVOID lpViewAddr, ULONG size, ULONG dwUserData)
{
 Win32MemMapNotify *tmpview  = mapviews;

    dprintf(("Win32MemMapNotify::Win32MemMapNotify: created %x, size %d", lpViewAddr, size));

    pMapView = lpViewAddr;
    mSize    = size;
    this->pfnNotify = pfnNotify;
    this->dwUserData = dwUserData;

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
}
//******************************************************************************
//******************************************************************************
Win32MemMapNotify::~Win32MemMapNotify()
{
    dprintf(("Win32MemMapNotify dtor: deleting view %x %x", pMapView, mSize));

    DosEnterCriticalSection(&globalmapcritsect);
    Win32MemMapNotify *view = mapviews;

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
        else    dprintf(("Win32MemMapNotify::~Win32MemMapNotify: map not found!! (%x)", this));
    }
    DosLeaveCriticalSection(&globalmapcritsect);
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMapNotify::notify(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess)
{
    return pfnNotify(pMapView, offset, fWriteAccess, mSize, dwUserData);
}
//******************************************************************************
//******************************************************************************
// Win32MemMapNotify::findMapByView
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
Win32MemMapNotify *Win32MemMapNotify::findMapByView(ULONG address,
                                                    ULONG *offset,
                                                    ULONG accessType)
{
  ULONG ulOffset;

  if(mapviews == NULL) return NULL;

  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMapNotify *view = mapviews;
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
        if(ulViewAddr + view->getSize() > address)
        {
          *offset = (address - ulViewAddr);
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

  DosLeaveCriticalSection(&globalmapcritsect);

  return view;
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
Win32MemMapNotify *Win32MemMapNotify::findView(ULONG address)
{
  ULONG ulViewAddr;

  DosEnterCriticalSection(&globalmapcritsect);
  Win32MemMapNotify *view = mapviews;

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
