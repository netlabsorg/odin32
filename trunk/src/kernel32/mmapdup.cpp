/* $Id: mmapdup.cpp,v 1.1 2003-03-27 14:13:11 sandervl Exp $ */

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

#define DBG_LOCALLOG    DBG_mmapdup
#include "dbglocal.h"


//******************************************************************************
// Class Win32MemMapDup
//
// Duplicate memory mapping class (duplicate map with different protection flags
// associated with an existing memory map)
//
//******************************************************************************
Win32MemMapDup::Win32MemMapDup(Win32MemMap *parent, HFILE hFile, ULONG size, 
                               ULONG fdwProtect, LPSTR lpszName) :
          Win32MemMap(hFile, size, fdwProtect, lpszName)
{
    this->parent = parent;
    hOrgMemFile = -1;	//we're a duplicate
    parent->AddRef();
}
//******************************************************************************
//******************************************************************************
Win32MemMapDup::~Win32MemMapDup()
{
    parent->Release();
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMapDup::Init(DWORD aMSize)
{
    //copy values from original map
    mSize    = parent->getMapSize();
    hMemFile = parent->getFileHandle();

    //If the parent is a readonly map and we allow write access, then we must
    //override our parent's protection flags
    if((getProtFlags() & PAGE_READWRITE) && (parent->getProtFlags() == PAGE_READONLY))
    {
        parent->setProtFlags(PAGE_READWRITE);
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMapDup::flushView(ULONG viewaddr, ULONG offset, ULONG cbFlush)
{
    return parent->flushView(viewaddr, offset, cbFlush);
}
//******************************************************************************
// Win32MemMapDup::mapViewOfFile
//
// Map the view identified by addr. Create a view with our parent as the parent
// map and ourselves as the ower.
//
// View parent = memory map that contains the original memory map
// View owner  = duplicate memory map that created this view (can be NULL)
//
// Parameters:
//
//   ULONG size                 - size of view
//   ULONG offset               - offset in memory map
//   ULONG fdwAccess            - access flags
//                                FILE_MAP_WRITE, FILE_MAP_READ, FILE_MAP_COPY
//                                FILE_MAP_ALL_ACCESS
//
// Returns:
//   <>NULL                     - success, view address
//   NULL                       - failure
//
//******************************************************************************
LPVOID Win32MemMapDup::mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess)
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
    if((fdwAccess & FILE_MAP_READ) && !(mProtFlags & (PAGE_READWRITE|PAGE_READONLY)))
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


    if(parent->allocateMap() == FALSE) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto fail;
    }

    mapview = new Win32MemMapView(parent, offset, (size == 0) ? (mSize - offset) : size, fdwAccess, this);
    if(mapview == NULL) {
        goto fail;
    }
    if(mapview->everythingOk() == FALSE) {
        dprintf(("Win32MemMapDup::mapViewOfFile: !mapview->everythingOk"));
        delete mapview;
        goto fail;
    }

    mapMutex.leave();
    SetLastError(ERROR_SUCCESS);
    return mapview->getViewAddr();

parmfail:
    dprintf(("Win32MemMapDup::mapViewOfFile: invalid parameter (ERROR_ACCESS_DENIED)"));
    //NT4 SP6 returns ERROR_ACCESS_DENIED for most invalid parameters
    SetLastError(ERROR_ACCESS_DENIED);
fail:
    mapMutex.leave();
    return 0;
}
//******************************************************************************
// Win32MemMapDup::unmapViewOfFile
//
// Unmap the view identified by addr by calling the parent and cleaning up
// the duplicate object
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
BOOL Win32MemMapDup::unmapViewOfFile(LPVOID addr)
{
    BOOL ret;

    dprintf(("Win32MemMapDup::unmapViewOfFile %x (nrmaps=%d)", addr, nrMappings));

    mapMutex.enter();

    if(nrMappings == 0)
        goto fail;

    ret = parent->unmapViewOfFile(addr);

    mapMutex.leave();

    SetLastError(ERROR_SUCCESS);
    return TRUE;
fail:
    mapMutex.leave();
    SetLastError(ERROR_INVALID_ADDRESS);
    return FALSE;
}
//******************************************************************************
// Win32MemMapDup::commitPage
//
// Handle a pagefault for a duplicate view
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
BOOL Win32MemMapDup::commitPage(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess, int nrpages)
{
    return parent->commitPage(ulFaultAddr, offset, fWriteAccess, nrpages);
}
//******************************************************************************
// Win32MemMapDup::commitGuardPage
//
// Handle a guard page exception
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
BOOL Win32MemMapDup::commitGuardPage(ULONG ulFaultAddr, ULONG ulOffset, 
                                     BOOL fWriteAccess)
{
   return parent->commitGuardPage(ulFaultAddr, ulOffset, fWriteAccess);
}
//******************************************************************************
// Win32MemMapDup::invalidatePages
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
BOOL Win32MemMapDup::invalidatePages(ULONG offset, ULONG size)
{
    return parent->invalidatePages(offset, size);
}
//******************************************************************************
//******************************************************************************
