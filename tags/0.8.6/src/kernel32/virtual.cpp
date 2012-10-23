/* $Id: virtual.cpp,v 1.55 2003-04-02 11:03:32 sandervl Exp $ */

/*
 * Win32 virtual memory functions
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 * Copyright 1998 Peter FitzSimmons
 * Copyright 2002-2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Parts (VIRTUAL_MapFileA/W) based on Wine code (memory\virtual.c):
 *
 * Copyright 1997 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <win/virtual.h>
#include <win/thread.h>
#include <heapstring.h>
#include <handlemanager.h>
#include "mmap.h"
#include "oslibdos.h"
#include "oslibmem.h"

#define DBG_LOCALLOG    DBG_virtual
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-VIRTUAL)

#define PAGE_SHIFT 12

/***********************************************************************
 *             CreateFileMapping32A   (KERNEL32.46)
 * Creates a named or unnamed file-mapping object for the specified file
 *
 * RETURNS
 *  Handle: Success
 *  0: Mapping object does not exist
 *  NULL: Failure
 */
HANDLE WINAPI CreateFileMappingA(
                HANDLE hFile,   /* [in] Handle of file to map */
                SECURITY_ATTRIBUTES *sa, /* [in] Optional security attributes*/
                DWORD protect,   /* [in] Protection for mapping object */
                DWORD size_high, /* [in] High-order 32 bits of object size */
                DWORD size_low,  /* [in] Low-order 32 bits of object size */
                LPCSTR name      /* [in] Name of file-mapping object */ )
{
 HANDLE hFileMap;

   dprintf(("CreateFileMappingA: %x %x %x%x %s", hFile, protect, size_high, size_low, name));
   hFileMap = HMCreateFileMapping(hFile, sa, protect, size_high, size_low, name);
   dprintf(("CreateFileMappingA returned %x", hFileMap));
   return hFileMap;
}


/***********************************************************************
 *             CreateFileMapping32W   (KERNEL32.47)
 * See CreateFileMapping32A
 */
HANDLE WINAPI CreateFileMappingW( HANDLE hFile, LPSECURITY_ATTRIBUTES attr,
                                      DWORD protect, DWORD size_high,
                                      DWORD size_low, LPCWSTR name )
{
    LPSTR nameA = HEAP_strdupWtoA( GetProcessHeap(), 0, name );
    HANDLE ret = CreateFileMappingA( hFile, attr, protect,
                                         size_high, size_low, nameA );
    HeapFree( GetProcessHeap(), 0, nameA );
    return ret;
}


/***********************************************************************
 *             OpenFileMapping32A   (KERNEL32.397)
 * Opens a named file-mapping object.
 *
 * RETURNS
 *  Handle: Success
 *  NULL: Failure
 */
HANDLE WINAPI OpenFileMappingA(
                DWORD access,   /* [in] Access mode */
                BOOL inherit, /* [in] Inherit flag */
                LPCSTR name )   /* [in] Name of file-mapping object */
{
    dprintf(("OpenFileMappingA: %x %d %s", access, inherit, name));
    return HMOpenFileMapping(access, inherit, name);
}


/***********************************************************************
 *             OpenFileMapping32W   (KERNEL32.398)
 * See OpenFileMapping32A
 */
HANDLE WINAPI OpenFileMappingW( DWORD access, BOOL inherit, LPCWSTR name)
{
    LPSTR nameA = HEAP_strdupWtoA( GetProcessHeap(), 0, name );
    HANDLE ret = OpenFileMappingA( access, inherit, nameA );
    HeapFree( GetProcessHeap(), 0, nameA );
    return ret;
}


/***********************************************************************
 *             MapViewOfFile   (KERNEL32.385)
 * Maps a view of a file into the address space
 *
 * RETURNS
 *  Starting address of mapped view
 *  NULL: Failure
 */
LPVOID WINAPI MapViewOfFile(
              HANDLE mapping,  /* [in] File-mapping object to map */
              DWORD access,      /* [in] Access mode */
              DWORD offset_high, /* [in] High-order 32 bits of file offset */
              DWORD offset_low,  /* [in] Low-order 32 bits of file offset */
              DWORD count        /* [in] Number of bytes to map */
)
{
    return MapViewOfFileEx( mapping, access, offset_high,
                            offset_low, count, NULL );
}


/***********************************************************************
 *             MapViewOfFileEx   (KERNEL32.386)
 * Maps a view of a file into the address space
 *
 * RETURNS
 *  Starting address of mapped view
 *  NULL: Failure
 */
LPVOID WINAPI MapViewOfFileEx(
              HANDLE handle,   /* [in] File-mapping object to map */
              DWORD access,      /* [in] Access mode */
              DWORD offset_high, /* [in] High-order 32 bits of file offset */
              DWORD offset_low,  /* [in] Low-order 32 bits of file offset */
              DWORD count,       /* [in] Number of bytes to map */
              LPVOID addr        /* [in] Suggested starting address for mapped view */
)
{
  return HMMapViewOfFileEx(handle, access, offset_high, offset_low, count, addr);
}


/***********************************************************************
 *             FlushViewOfFile   (KERNEL32.262)
 * Writes to the disk a byte range within a mapped view of a file
 *
 * RETURNS
 *  TRUE: Success
 *  FALSE: Failure
 */
BOOL WINAPI FlushViewOfFile(
              LPCVOID base, /* [in] Start address of byte range to flush */
              DWORD cbFlush /* [in] Number of bytes in range */
)
{
 Win32MemMap *map;
 DWORD offset;
 BOOL ret;

    if (!base)
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    map = Win32MemMapView::findMapByView((ULONG)base, &offset, MEMMAP_ACCESS_READ);
    if(map == NULL) {
        //This is what NT4, SP6 returns for an invalid view address
        SetLastError( ERROR_INVALID_ADDRESS );
        return FALSE;
    }
    ret = map->flushView((ULONG)base, offset, cbFlush);
    map->Release();
    return ret;
}


/***********************************************************************
 *             UnmapViewOfFile   (KERNEL32.540)
 * Unmaps a mapped view of a file.
 *
 * NOTES
 *  Should addr be an LPCVOID?
 *
 * RETURNS
 *  TRUE: Success
 *  FALSE: Failure
 */
BOOL WINAPI UnmapViewOfFile(LPVOID addr /* [in] Address where mapped view begins */
)
{
    Win32MemMap *map;
    BOOL  ret;

    if (!addr)
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    map = Win32MemMapView::findMapByView((ULONG)addr);
    if(map == NULL) {
        SetLastError( ERROR_FILE_NOT_FOUND );
        return FALSE;
    }
    ret = map->unmapViewOfFile(addr);
    map->Release();
    return ret;
}

/***********************************************************************
 *             VIRTUAL_MapFileW
 *
 * Helper function to map a file to memory:
 *  name            -   file name
 *  [RETURN] ptr        -   pointer to mapped file
 */
HANDLE WINAPI VIRTUAL_MapFileW( LPCWSTR name , LPVOID *lpMapping, BOOL fReadIntoMemory)
{
    HANDLE hFile, hMapping = -1;

    hFile = CreateFileW( name, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                           OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hMapping = CreateFileMappingA( hFile, NULL, PAGE_READONLY | ((fReadIntoMemory) ? SEC_COMMIT : 0), 0, 0, NULL );
        CloseHandle( hFile );
        if (hMapping)
        {
            *lpMapping = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );
        }
    }
    return hMapping;
}

/***********************************************************************
 *             VIRTUAL_MapFileA
 *
 * Helper function to map a file to memory:
 *  name            -   file name
 *  [RETURN] ptr        -   pointer to mapped file
 */
HANDLE WINAPI VIRTUAL_MapFileA( LPCSTR name , LPVOID *lpMapping, BOOL fReadIntoMemory)
{
    HANDLE hFile, hMapping = -1;

    hFile = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                        OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hMapping = CreateFileMappingA( hFile, NULL, PAGE_READONLY | ((fReadIntoMemory) ? SEC_COMMIT : 0), 0, 0, NULL );
        CloseHandle( hFile );
        if (hMapping)
        {
            *lpMapping = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );
        }
    }
    return hMapping;
}
//******************************************************************************
// Translate OS2 page attributes to Windows attribute, state and type values
//******************************************************************************
void TranslateOS2PageAttr(DWORD os2attr, DWORD *lpdwWinProtect, DWORD *lpdwWinState,
                          DWORD *lpdwWinType)
{
    DWORD State, Type;

    if(!lpdwWinState) lpdwWinState = &State;
    if(!lpdwWinType)  lpdwWinType = &Type;

    *lpdwWinProtect  = 0;
    *lpdwWinState    = 0;
    *lpdwWinType     = 0;

    if(os2attr & PAG_READ && !(os2attr & PAG_WRITE))
        *lpdwWinProtect |= PAGE_READONLY;
    else
    if(os2attr & PAG_WRITE)
        *lpdwWinProtect |= PAGE_READWRITE;

    if((os2attr & (PAG_WRITE | PAG_EXECUTE)) == (PAG_WRITE | PAG_EXECUTE))
        *lpdwWinProtect |= PAGE_EXECUTE_READWRITE;
    else
    if(os2attr & PAG_EXECUTE)
        *lpdwWinProtect |= PAGE_EXECUTE_READ;

    if(os2attr & PAG_GUARD)
        *lpdwWinProtect |= PAGE_GUARD;

    if(os2attr & PAG_FREE)
        *lpdwWinState = MEM_FREE;
    else
    if(os2attr & PAG_COMMIT)
        *lpdwWinState = MEM_COMMIT;
    else
        *lpdwWinState = MEM_RESERVE;

    //TODO: MEM_MAPPED & MEM_IMAGE (==SEC_IMAGE)
    if(!(os2attr & PAG_SHARED))
        *lpdwWinType = MEM_PRIVATE;

    // Pages can be committed but not necessarily accessible!!
    if (!(os2attr & (PAG_READ | PAG_WRITE | PAG_EXECUTE | PAG_GUARD)))
        *lpdwWinProtect = PAGE_NOACCESS;

}
//******************************************************************************
// Translate Windows page attributes to OS/2 page attributes
//******************************************************************************
void TranslateWinPageAttr(DWORD dwProtect, DWORD *lpdwOS2Attr)
{
    *lpdwOS2Attr = 0;

    if(dwProtect & PAGE_READONLY)     *lpdwOS2Attr |= PAG_READ;
    if(dwProtect & PAGE_READWRITE)    *lpdwOS2Attr |= (PAG_READ | PAG_WRITE);
    if(dwProtect & PAGE_WRITECOPY)    *lpdwOS2Attr |= (PAG_READ | PAG_WRITE);

    if(dwProtect & PAGE_EXECUTE)      *lpdwOS2Attr |= (PAG_EXECUTE | PAG_READ);
    if(dwProtect & PAGE_EXECUTE_READ) *lpdwOS2Attr |= (PAG_EXECUTE | PAG_READ);
    if(dwProtect & PAGE_EXECUTE_READWRITE)
        *lpdwOS2Attr |= (PAG_EXECUTE | PAG_WRITE | PAG_READ);
    if(dwProtect & PAGE_EXECUTE_WRITECOPY)
        *lpdwOS2Attr |= (PAG_EXECUTE | PAG_WRITE | PAG_READ);

    if(dwProtect & PAGE_GUARD)
        *lpdwOS2Attr |= PAG_GUARD;

    if(dwProtect & PAGE_NOACCESS)     *lpdwOS2Attr |= PAG_READ; //can't do this in OS/2
}


#define OBJ_LOCSPECIFIC 0x1000
//******************************************************************************
//NOTE: Do NOT set the last error to ERROR_SUCCESS if successful. Windows
//      does not do this either!
//******************************************************************************
LPVOID WIN32API VirtualAlloc(LPVOID lpvAddress,
                             DWORD  cbSize,
                             DWORD  fdwAllocationType,
                             DWORD  fdwProtect)
{
    PVOID Address;
    ULONG flag = 0, base;
    ULONG remainder;
    DWORD rc;

    if (cbSize > 0x7fc00000)  /* 2Gb - 4Mb */
    {
        dprintf(("VirtualAlloc: size too large"));
        SetLastError( ERROR_OUTOFMEMORY );
        return NULL;
    }

    // We're ignoring MEM_TOP_DOWN for now
    if (!(fdwAllocationType & (MEM_COMMIT | MEM_RESERVE)) ||
       (fdwAllocationType & ~(MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN)))
    {
        dprintf(("VirtualAlloc: Invalid parameter"));
        SetLastError( ERROR_INVALID_PARAMETER );
        return NULL;
    }

    //round address and size to page boundaries
    remainder  = (ULONG)lpvAddress & 0xFFF;
    lpvAddress = (LPVOID)((ULONG)lpvAddress & ~0xFFF);
    Address    = lpvAddress;

    cbSize    += remainder;
    remainder  = cbSize & 0xFFF;
    cbSize    &= ~0xFFF;
    if(remainder)
        cbSize += PAGE_SIZE;

    //Translate windows page attributes (flag is reset to 0!!)
    TranslateWinPageAttr(fdwProtect, &flag);

    if(fdwAllocationType & MEM_COMMIT)
    {
        dprintf(("VirtualAlloc: commit\n"));
        flag |= PAG_COMMIT;
    }

    // The below doesn't seem to be true any longer, at least when it comes to
    // Odin and Java. Need to do more testing to make sure
#if 1
    if(fdwAllocationType & MEM_RESERVE) {
        //SvL: DosRead crashes if memory is initially reserved with write
        //     access disabled (OS/2 bug) even if the commit sets the page
        //     flags to read/write:
        // DosSetMem does not alter the 16 bit selectors so if you change memory
        // attributes and then access the memory with a 16 bit API (such as DosRead),
        // it will have the old (alloc time) attributes
        flag |= PAG_READ|PAG_WRITE;
    }
#endif

    //just do this if other options are used
    if(!(flag & (PAG_READ | PAG_WRITE | PAG_EXECUTE)) || flag == 0)
    {
        dprintf(("VirtualAlloc: Unknown protection flags, default to read/write"));
        flag |= PAG_READ | PAG_WRITE;
    }

    if(lpvAddress)
    {
        Win32MemMap *map;
        ULONG offset, nrpages, accessflags = 0;

        nrpages = cbSize >> PAGE_SHIFT;
        if(cbSize & 0xFFF)
            nrpages++;

        if(flag & PAG_READ) {
            accessflags |= MEMMAP_ACCESS_READ;
        }
        if(flag & PAG_WRITE) {
            accessflags |= MEMMAP_ACCESS_WRITE;
        }
        if(flag & PAG_EXECUTE) {
            accessflags |= MEMMAP_ACCESS_EXECUTE;
        }
        map = Win32MemMapView::findMapByView((ULONG)lpvAddress, &offset, accessflags);
        if(map) {
            //TODO: We don't allow protection flag changes for mmaped files now
            map->commitPage((ULONG)lpvAddress, offset, FALSE, nrpages);
            map->Release();
            return lpvAddress;
        }
        /* trying to allocate memory at specified address */
        if(fdwAllocationType & MEM_RESERVE)
        {
            rc = OSLibDosAllocMem(&Address, cbSize, flag |OBJ_LOCSPECIFIC);
            dprintf(("Allocation at specified address: %x. rc: %i", Address, rc));
            if (rc)
            {
                SetLastError(ERROR_OUTOFMEMORY);
                return NULL;
            }
            dprintf(("Allocated at specified address: %x. rc: %i", Address, rc));
            return(Address);
        }
    }

    // commit memory
    if(fdwAllocationType & MEM_COMMIT)
    {
        Address = lpvAddress;

        //try to commit the pages
        rc = OSLibDosSetMem(lpvAddress, cbSize, flag);

        //might try to commit larger part with same base address
        if(rc == OSLIB_ERROR_ACCESS_DENIED && cbSize > PAGE_SIZE )
        {
            while(cbSize)
            {
                //check if the app tries to commit an already commited part of memory or change the protection flags
                ULONG size = cbSize, os2flags, newrc;
                newrc = OSLibDosQueryMem(lpvAddress, &size, &os2flags);
                if(newrc == 0)
                {
                    if(os2flags & PAG_COMMIT)
                    {
                        dprintf(("VirtualAlloc: commit on committed memory"));
                        if((flag & (PAG_READ|PAG_WRITE|PAG_EXECUTE)) != (os2flags & (PAG_READ|PAG_WRITE|PAG_EXECUTE)))
                        {   //change protection flags
                            DWORD tmp;
                            if(VirtualProtect(lpvAddress, size, fdwProtect, &tmp) == FALSE)
                            {
                                dprintf(("ERROR: VirtualAlloc: commit on committed memory -> VirtualProtect failed!!"));
                                return NULL;
                            }
                        }
                    }
                    else
                    {   //commit this page (or range of pages)
                        rc = OSLibDosSetMem(lpvAddress, size, flag);
                        if(rc) {
                            dprintf(("Unexpected DosSetMem error %x", rc));
                            break;
                        }
                    }
                }
                else {
                    dprintf(("Unexpected DosQueryMem error %x", newrc));
                    rc = newrc;
                    break;
                }
                cbSize -= size;

                lpvAddress = (LPVOID)((char *)lpvAddress + size);
            }//while(cbSize)

            rc = 0; //success
        }
        else
        {
            if(rc == OSLIB_ERROR_INVALID_ADDRESS) {
                rc = OSLibDosAllocMem(&Address, cbSize, flag );
            }
            else {
                if(rc) {
                    //check if the app tries to commit an already commited part of memory or change the protection flags
                    ULONG size = cbSize, os2flags, newrc;
                    newrc = OSLibDosQueryMem(lpvAddress, &size, &os2flags);
                    if(newrc == 0) {
                        if(size >= cbSize && (os2flags & PAG_COMMIT)) {
                                dprintf(("VirtualAlloc: commit on committed memory"));
                                if((flag & (PAG_READ|PAG_WRITE|PAG_EXECUTE)) != (os2flags & (PAG_READ|PAG_WRITE|PAG_EXECUTE)))
                                {   //change protection flags
                                    DWORD tmp;
                                    if(VirtualProtect(lpvAddress, cbSize, fdwProtect, &tmp) == TRUE) {
                                        return lpvAddress;
                                    }
                                    dprintf(("ERROR: VirtualAlloc: commit on committed memory -> VirtualProtect failed!!"));
                                    return NULL;
                                }
                                //else everything ok
                                return lpvAddress;
                        }
                        else    dprintf(("Unexpected DosSetMem error %x", rc));
                    }
                    else {
                        dprintf(("Unexpected DosQueryMem error %x", newrc));
                    }
                }
            }
        }
    }
    else
    {
        rc = OSLibDosAllocMem(&Address, cbSize, flag);
    }

    if(rc)
    {
        dprintf(("DosSetMem returned %d\n", rc));
        SetLastError( ERROR_OUTOFMEMORY );
        return(NULL);
    }

    dprintf(("VirtualAlloc returned %X\n", Address));
    return(Address);
}
//******************************************************************************
//NOTE: Do NOT set the last error to ERROR_SUCCESS if successful. Windows
//      does not do this either!
//******************************************************************************
BOOL WIN32API VirtualFree(LPVOID lpvAddress,
                          DWORD  cbSize,
                          DWORD  FreeType)
{
    DWORD rc;

    // verify parameters
    if((FreeType & MEM_RELEASE) && (cbSize != 0))
    {
        dprintf(("WARNING: VirtualFree: invalid parameter!!"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }

    if((FreeType & MEM_DECOMMIT) && (FreeType & MEM_RELEASE))
    {
        dprintf(("WARNING: VirtualFree: invalid parameter!!"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }

    /* Assuming that we don't allocate memory in the first 64kb. */
    if ((unsigned)lpvAddress < 0x10000)
    {
        if (!lpvAddress)
            dprintf(("WARNING: VirtualFree: bogus address %p!!", lpvAddress));
        SetLastError(ERROR_INVALID_ADDRESS);
        return FALSE;
    }

    // decommit memory
    if (FreeType &  MEM_DECOMMIT)
    {
        // check if app wants to decommit stack pages -> don't allow that!
        // (VAC runtime uses last stack page to store some internal
        //  data; if freed pe/pec will crash during exit)

        TEB *teb = GetThreadTEB();
        if(teb) {
            DWORD stacktop    = (DWORD)teb->stack_top;
            DWORD stackbottom = (DWORD)teb->stack_low;

            stackbottom = stackbottom & ~0xFFFF;   //round down to 64kb boundary
            stacktop    = stacktop & ~0xFFF;

            if(lpvAddress >= (PVOID)stackbottom && lpvAddress < (PVOID)stacktop) {
                //pretend we did was was asked
                dprintf(("WARNING: app tried to decommit stack pages; pretend success"));
                return TRUE;
            }
        }
        // decommit memory block
        rc = OSLibDosSetMem(lpvAddress, cbSize, PAG_DECOMMIT);
        if(rc)
        {
            if(rc == 32803) { //SvL: ERROR_ALIAS
                dprintf(("KERNEL32:VirtualFree:OsLibSetMem rc = #%d; app tries to decommit aliased memory; ignore", rc));
                return(TRUE);
            }
            dprintf(("KERNEL32:VirtualFree:OsLibSetMem rc = #%d\n", rc));
            SetLastError(ERROR_INVALID_ADDRESS);
            return(FALSE);
        }
    }

    // release memory
    if (FreeType &  MEM_RELEASE)
    {
        rc = OSLibDosFreeMem(lpvAddress); // free the memory block
        if(rc)
        {
            dprintf(("KERNEL32:VirtualFree:OsLibFreeMem rc = #%d\n", rc));
            SetLastError(ERROR_INVALID_ADDRESS);
            return(FALSE);
        }
    }
    return TRUE;
}
//******************************************************************************
//LPVOID lpvAddress;            /* address of region of committed pages       */
//DWORD  cbSize;                /* size of the region                         */
//DWORD  fdwNewProtect;         /* desired access protection                  */
//PDWORD pfdwOldProtect;        /* address of variable to get old protection  */
//TODO: Not 100% complete
//TODO: SetLastError on failure
//******************************************************************************

BOOL WIN32API VirtualProtect(LPVOID lpvAddress,
                             DWORD  cbSize,
                             DWORD  fdwNewProtect,
                             DWORD* pfdwOldProtect)
{
    DWORD rc;
    DWORD  cb = cbSize;
    ULONG  pageFlags = 0;
    int npages;

    if(pfdwOldProtect == NULL) {
        dprintf(("WARNING: pfdwOldProtect == NULL"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }

    SetLastError(ERROR_SUCCESS);

    rc = OSLibDosQueryMem(lpvAddress, &cb, &pageFlags);
    if(rc) {
        dprintf(("DosQueryMem returned %d\n", rc));
        return(FALSE);
    }
    dprintf(("Old memory flags %X\n", pageFlags));
    TranslateOS2PageAttr(pageFlags, pfdwOldProtect, NULL, NULL);

    TranslateWinPageAttr(fdwNewProtect, &pageFlags);

    dprintf(("New memory flags %X\n", pageFlags));
    if(pageFlags == 0) {
        dprintf(("pageFlags == 0\n"));
        return(TRUE);   //nothing to do
    }
    ULONG offset = ((ULONG)lpvAddress & 0xFFF);
    npages = (cbSize >> 12);

    cb = (cbSize & 0xFFF) + offset; // !!! added, some optimization :)
    if( cb > 0 ) { // changed
        npages++;
    }
    if( cb > 4096 ) { // changed, note '>' sign ( not '>=' ) 4096 is exactly one page
        npages++;
    }

    lpvAddress = (LPVOID)((int)lpvAddress & ~0xFFF);
    cbSize     = npages*4096;
    dprintf(("lpvAddress = %X, cbSize = %d\n", lpvAddress, cbSize));

    rc = OSLibDosSetMem(lpvAddress, cbSize, pageFlags);
    if(rc) {
        dprintf(("DosSetMem returned %d\n", rc));
        return(FALSE);
    }
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API VirtualQuery(LPCVOID lpvAddress,
                            LPMEMORY_BASIC_INFORMATION pmbiBuffer,
                            DWORD   cbLength)
{
    ULONG  cbRangeSize, dAttr;
    DWORD  rc;
    LPVOID lpBase;

    SetLastError(ERROR_SUCCESS);

    if(pmbiBuffer == NULL || cbLength != sizeof(MEMORY_BASIC_INFORMATION)) // check parameters
    {
        dprintf(("WARNING: invalid parameter"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;                             // nothing to return
    }

    // determine exact page range
    lpBase = (LPVOID)((ULONG)lpvAddress & 0xFFFFF000);
    cbRangeSize = -1;

    rc = OSLibDosQueryMem(lpBase, &cbRangeSize, &dAttr);
    if(rc==487)
    {
        // On OS/2, ERROR_INVALID_ADDRESS (478) is returned for unallocated or
        // freed private memory (for unallocated shared memory DosQueryMem
        // succeeds with PAG_FREE). However on Windows (per MSDN), VirtualQuery
        // succeeds with MEM_FREE for any address that is within the addressable
        // space of the process. Provide a rough simulation here.

        dprintf(("WARNING: VirtualQuery: OSLibDosQueryMem(0x%08X,%d) returned %d, "
                 "returning MEM_FREE for one page!",
                 lpBase, cbLength, rc));

        memset(pmbiBuffer, 0, sizeof(MEMORY_BASIC_INFORMATION));
        pmbiBuffer->BaseAddress = lpBase;
        pmbiBuffer->RegionSize  = 0x1000;
        pmbiBuffer->State       = MEM_FREE;
        return sizeof(MEMORY_BASIC_INFORMATION);
    }
    if(rc)
    {
        dprintf(("VirtualQuery - OSLibDosQueryMem %x %x returned %d\n",
                  lpBase, cbLength, rc));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    memset(pmbiBuffer, 0, sizeof(MEMORY_BASIC_INFORMATION));

    pmbiBuffer->BaseAddress = lpBase;
    //round to next page boundary
    pmbiBuffer->RegionSize  = (cbRangeSize + 0xFFF) & 0xFFFFF000;

    TranslateOS2PageAttr(dAttr, &pmbiBuffer->Protect, &pmbiBuffer->State, &pmbiBuffer->Type);

    //TODO: This is not correct: AllocationProtect should contain the protection
    //      flags used in the initial call to VirtualAlloc
    pmbiBuffer->AllocationProtect = pmbiBuffer->Protect;
    pmbiBuffer->AllocationBase    = OSLibDosFindMemBase(lpBase, &dAttr);
    if(dAttr) {
        TranslateOS2PageAttr(dAttr, &pmbiBuffer->AllocationProtect, NULL, NULL);
    }
#if 0
    dprintf(("Memory region alloc base          0x%08x", pmbiBuffer->AllocationBase));
    dprintf(("Memory region alloc protect flags %x", pmbiBuffer->AllocationProtect));
    dprintf(("Memory region base                0x%08x", pmbiBuffer->BaseAddress));
    dprintf(("Memory region protect flags       %x", pmbiBuffer->Protect));
    dprintf(("Memory region region size         0x%08x", pmbiBuffer->RegionSize));
    dprintf(("Memory region state               0x%08x", pmbiBuffer->State));
    dprintf(("Memory region type                0x%08x", pmbiBuffer->Type));
#endif
    return sizeof(MEMORY_BASIC_INFORMATION);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API VirtualLock(LPVOID lpAddress, DWORD dwSize)
{
    dprintf(("VirtualLock at %d; %d bytes - stub (TRUE)\n", (int)lpAddress, dwSize));
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API VirtualUnlock(LPVOID lpAddress, DWORD dwSize)
{
    dprintf(("VirtualUnlock at %d; %d bytes - stub (TRUE)\n", (int)lpAddress, dwSize));
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}

/*****************************************************************************
 * Name      : BOOL VirtualProtectEx
 * Purpose   : The VirtualProtectEx function changes the access protection on
 *             a region of committed pages in the virtual address space of a specified
 *             process. Note that this function differs from VirtualProtect,
 *             which changes the access protection on the calling process only.
 * Parameters: HANDLE hProcess       handle of process
 *             LPVOID lpvAddress     address of region of committed pages
 *             DWORD  cbSize         size of region
 *             DWORD  fdwNewProtect  desired access protection
 *             PDWORD pfdwOldProtect address of variable to get old protection
 * Variables :
 * Result    : size of target buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API VirtualProtectEx(HANDLE  hProcess,
                               LPVOID  lpvAddress,
                               DWORD   cbSize,
                               DWORD   fdwNewProtect,
                               LPDWORD pfdwOldProtect)
{
  // only execute API, if this is the current process !
  if (GetCurrentProcess() == hProcess)
    return VirtualProtect(lpvAddress, cbSize, fdwNewProtect, pfdwOldProtect);
  else
  {
    SetLastError(ERROR_ACCESS_DENIED); // deny access to other processes
    return FALSE;
  }
}


/*****************************************************************************
 * Name      : DWORD VirtualQueryEx
 * Purpose   : The VirtualQueryEx function provides information about a range
 *             of pages within the virtual address space of a specified process.
 * Parameters: HANDLE  hProcess                     handle of process
 *             LPCVOID  lpvAddress                  address of region
 *             LPMEMORY_BASIC_INFORMATION pmbiBuffer address of information buffer
 *             DWORD  cbLength                      size of buffer
 * Variables :
 * Result    : number of bytes returned in buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API VirtualQueryEx(HANDLE hProcess,
                              LPCVOID lpvAddress,
                              LPMEMORY_BASIC_INFORMATION pmbiBuffer,
                              DWORD   cbLength)
{
  // only execute API, if this is the current process !
  if (GetCurrentProcess() == hProcess)
    return VirtualQuery(lpvAddress, pmbiBuffer, cbLength);
  else
  {
    SetLastError(ERROR_ACCESS_DENIED); // deny access to other processes
    return FALSE;
  }
}

//******************************************************************************
// Private Odin api
//******************************************************************************
LPVOID VirtualAllocShared(DWORD cbSize, DWORD  fdwAllocationType,
                          DWORD fdwProtect, LPSTR name)
{
  LPVOID Address;
  ULONG flag = 0, base;
  DWORD rc;

  dprintf(("VirtualAllocShared: %x %x %x %s", cbSize, fdwAllocationType, fdwProtect, name));

  if (cbSize > 0x7fc00000)  /* 2Gb - 4Mb */
  {
    dprintf(("VirtualAllocShared: size too large"));
        SetLastError( ERROR_OUTOFMEMORY );
        return NULL;
  }

  if (!(fdwAllocationType & (MEM_COMMIT | MEM_RESERVE)) ||
       (fdwAllocationType & ~(MEM_COMMIT | MEM_RESERVE)))
  {
    dprintf(("VirtualAllocShared: Invalid parameter"));
        SetLastError( ERROR_INVALID_PARAMETER );
        return NULL;
  }

  //Translate windows page attributes (flag is reset to 0!!)
  TranslateWinPageAttr(fdwProtect, &flag);

  if(fdwAllocationType & MEM_COMMIT)
  {
        dprintf(("VirtualAllocShared: commit\n"));
      flag |= PAG_COMMIT;
  }

  //just do this if other options are used
  if(!(flag & (PAG_READ | PAG_WRITE | PAG_EXECUTE)) || flag == 0)
  {
    dprintf(("VirtualAllocShared: Unknown protection flags, default to read/write"));
    flag |= PAG_READ | PAG_WRITE;
  }

  rc = OSLibDosAllocSharedMem(&Address, cbSize, flag, name);

  if(rc)
  {
    dprintf(("DosAllocSharedMem returned %d\n", rc));
    SetLastError( ERROR_OUTOFMEMORY );
    return(NULL);
  }

  dprintf(("VirtualAllocShared returned %X\n", Address));
  return(Address);
}
//******************************************************************************
//******************************************************************************
