/* $Id: virtual.cpp,v 1.1 1999-09-15 23:33:01 sandervl Exp $ */

/*
 * Win32 virtual memory functions
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 * Copyright 1998 Peter FitzSimmons
 *
 * Parts (VIRTUAL_MapFileA/W) based on Wine code (memory\virtual.c):
 *
 * Copyright 1997 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <win\virtual.h>
#include <heapstring.h>
#include <handlemanager.h>
#include "mmap.h"
#include "oslibdos.h"

/***********************************************************************
 *             CreateFileMapping32A   (KERNEL32.46)
 * Creates a named or unnamed file-mapping object for the specified file
 *
 * RETURNS
 *	Handle: Success
 *	0: Mapping object does not exist
 *	NULL: Failure
 */
HANDLE WINAPI CreateFileMappingA(
                HFILE hFile,   /* [in] Handle of file to map */
                SECURITY_ATTRIBUTES *sa, /* [in] Optional security attributes*/
                DWORD protect,   /* [in] Protection for mapping object */
                DWORD size_high, /* [in] High-order 32 bits of object size */
                DWORD size_low,  /* [in] Low-order 32 bits of object size */
                LPCSTR name      /* [in] Name of file-mapping object */ )
{
   return HMCreateFileMapping(hFile, sa, protect, size_high, size_low, name);
}


/***********************************************************************
 *             CreateFileMapping32W   (KERNEL32.47)
 * See CreateFileMapping32A
 */
HANDLE WINAPI CreateFileMappingW( HFILE hFile, LPSECURITY_ATTRIBUTES attr,
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
 *	Handle: Success
 *	NULL: Failure
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
 *	Starting address of mapped view
 *	NULL: Failure
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
 *	Starting address of mapped view
 *	NULL: Failure
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
 *	TRUE: Success
 *	FALSE: Failure
 */
BOOL WINAPI FlushViewOfFile(
              LPCVOID base, /* [in] Start address of byte range to flush */
              DWORD cbFlush /* [in] Number of bytes in range */
)
{
 Win32MemMap *map;
 DWORD offset;

    if (!base)
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    map = Win32MemMapView::findMapByView((ULONG)base, &offset, MEMMAP_ACCESS_READ);
    if(map == NULL) {
        SetLastError( ERROR_FILE_NOT_FOUND );
        return FALSE;
    }
    return map->flushView(offset, cbFlush);
}


/***********************************************************************
 *             UnmapViewOfFile   (KERNEL32.540)
 * Unmaps a mapped view of a file.
 *
 * NOTES
 *	Should addr be an LPCVOID?
 *
 * RETURNS
 *	TRUE: Success
 *	FALSE: Failure
 */
BOOL WINAPI UnmapViewOfFile(LPVOID addr /* [in] Address where mapped view begins */
)
{
 Win32MemMap *map;
 Win32MemMapView *view;

 DWORD offset;

    if (!addr)
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    map = Win32MemMapView::findMapByView((ULONG)addr, &offset, MEMMAP_ACCESS_READ, &view);
    if(map == NULL) {
        SetLastError( ERROR_FILE_NOT_FOUND );
        return FALSE;
    }
    return map->unmapViewOfFile(view);
}

/***********************************************************************
 *             VIRTUAL_MapFileW
 *
 * Helper function to map a file to memory:
 *  name			-	file name
 *  [RETURN] ptr		-	pointer to mapped file
 */
HANDLE WINAPI VIRTUAL_MapFileW( LPCWSTR name , LPVOID *lpMapping)
{
    HANDLE hFile, hMapping = -1;

    hFile = CreateFileW( name, GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hMapping = CreateFileMappingA( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
        CloseHandle( hFile );
        if (hMapping != INVALID_HANDLE_VALUE)
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
 *  name			-	file name
 *  [RETURN] ptr		-	pointer to mapped file
 */
HANDLE WINAPI VIRTUAL_MapFileA( LPCSTR name , LPVOID *lpMapping)
{
    HANDLE hFile, hMapping = -1;

    hFile = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hMapping = CreateFileMappingA( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
        CloseHandle( hFile );
        if (hMapping != INVALID_HANDLE_VALUE)
        {
            *lpMapping = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );
        }
    }
    return hMapping;
}

//******************************************************************************
//******************************************************************************
LPVOID WIN32API VirtualAlloc(LPVOID lpvAddress, DWORD cbSize, DWORD fdwAllocationType,
                             DWORD fdwProtect)
{
 PVOID Address = lpvAddress;
 ULONG flag = 0, base;
 DWORD rc;

  dprintf(("VirtualAlloc at %X; %d bytes, fAlloc %d, fProtect %d\n", (int)lpvAddress, cbSize, fdwAllocationType, fdwProtect));

  if (cbSize > 0x7fc00000)  /* 2Gb - 4Mb */
  {
   	dprintf(("VirtualAlloc: size too large"));
        SetLastError( ERROR_OUTOFMEMORY );
        return NULL;
  }

  if (!(fdwAllocationType & (MEM_COMMIT | MEM_RESERVE)) ||
       (fdwAllocationType & ~(MEM_COMMIT | MEM_RESERVE)))
  {
   	dprintf(("VirtualAlloc: Invalid parameter"));
        SetLastError( ERROR_INVALID_PARAMETER );
        return NULL;
  }

  if(fdwAllocationType & MEM_COMMIT) {
        dprintf(("VirtualAlloc: commit\n"));
        flag = PAG_COMMIT;
  }
  if(fdwProtect & PAGE_READONLY)     	  flag |= PAG_READ;
  if(fdwProtect & PAGE_READWRITE)    	  flag |= (PAG_READ | PAG_WRITE);
  if(fdwProtect & PAGE_WRITECOPY)    	  flag |= (PAG_READ | PAG_WRITE);

  if(fdwProtect & PAGE_EXECUTE_READWRITE) flag |= (PAG_EXECUTE | PAG_WRITE | PAG_READ);
  if(fdwProtect & PAGE_EXECUTE_READ) 	  flag |= (PAG_EXECUTE | PAG_READ);
  if(fdwProtect & PAGE_EXECUTE)           flag |= PAG_EXECUTE;

  if(fdwProtect & PAGE_GUARD)        	  flag |= PAG_GUARD;

  //just do this if other options are used
  if(!(flag & (PAG_READ | PAG_WRITE | PAG_EXECUTE)) || flag == 0)
  {
    	dprintf(("VirtualAlloc: Unknown protection flags, default to read/write"));
      	flag |= PAG_READ | PAG_WRITE;
  }

  if(fdwAllocationType & MEM_COMMIT && lpvAddress != NULL)
  {
    Address = lpvAddress;

    rc = OSLibDosSetMem(lpvAddress, cbSize, flag);

    //might try to commit larger part with same base address
    if(rc == OSLIB_ERROR_ACCESS_DENIED && cbSize > 4096 )
    { //knut: AND more than one page
      char *newbase = (char *)lpvAddress + ((cbSize-1) & 0xFFFFF000); //knut: lets not start after the last page!
      ULONG size, os2flags;

      while(newbase >= (char *)lpvAddress)
      {  //knut: should check first page to!!
        size     = 4096;
        os2flags = 0;
        rc = OSLibDosQueryMem(newbase, &size, &os2flags);
        if(rc)
          break;

        if(os2flags & PAG_COMMIT)
        {
          newbase += 4096;
          break;
        }
        newbase -= 4096;
      }

      if(rc == 0)
      {
        //In case it wants to commit bytes that fall into the last
        //page of the previous commit command
        if(cbSize > ((int)newbase - (int)lpvAddress))
          	rc = OSLibDosSetMem(newbase, cbSize - ((int)newbase - (int)lpvAddress), flag);
      }
      else      return(NULL);

    }
    else
    {
      	if(rc == OSLIB_ERROR_INVALID_ADDRESS) {
        	rc = OSLibDosAllocMem(&Address, cbSize, flag );
	}
      	else
        if(rc)  dprintf(("Unexpected DosSetMem error %x", rc));
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
//******************************************************************************
BOOL WIN32API VirtualFree(LPVOID lpvAddress, DWORD cbSize, DWORD FreeType)
{
 DWORD rc;

  dprintf(("VirtualFree at %d; %d bytes, freetype %d\n", (int)lpvAddress, cbSize, FreeType));

  if(lpvAddress == NULL || cbSize == 0) {
	SetLastError(ERROR_INVALID_PARAMETER);
    	return(FALSE);
  }
  if(FreeType & MEM_DECOMMIT) {
        rc = OSLibDosSetMem(lpvAddress, cbSize, PAG_DECOMMIT);
  }
  else  rc = OSLibDosFreeMem(lpvAddress);    //MEM_RELEASE, cbSize == 0 (or should be)

  if(rc) {
	SetLastError(ERROR_GEN_FAILURE);
    	return(FALSE);
  }
  return(TRUE);
}
//******************************************************************************
//LPVOID lpvAddress;            /* address of region of committed pages */
//DWORD  cbSize;                /* size of the region   */
//DWORD  fdwNewProtect;         /* desired access protection    */
//PDWORD pfdwOldProtect;        /* address of variable to get old protection    */
//TODO: Not 100% complete
//TODO: SetLastError on failure
//******************************************************************************
BOOL WIN32API VirtualProtect(LPVOID lpvAddress, DWORD cbSize, DWORD fdwNewProtect,
                             DWORD *pfdwOldProtect)
{
 DWORD rc;
 ULONG  pageFlags = 0;
 int npages;

  dprintf(("VirtualProtect %X; %d bytes, new flags %X (%X)\n", (int)lpvAddress, cbSize, fdwNewProtect, pfdwOldProtect));
  if(pfdwOldProtect == NULL)
        return(FALSE);

  rc = OSLibDosQueryMem(lpvAddress, &cbSize, &pageFlags);
  if(rc) {
        dprintf(("DosQueryMem returned %d\n", rc));
        return(FALSE);
  }
  dprintf(("Old memory flags %X\n", pageFlags));
  *pfdwOldProtect = 0;
  if(pageFlags & PAG_READ && !(pageFlags & PAG_WRITE))
        *pfdwOldProtect |= PAGE_READONLY;
  if(pageFlags & (PAG_WRITE))
        *pfdwOldProtect |= PAGE_READWRITE;

  if((pageFlags & (PAG_WRITE | PAG_EXECUTE)) == (PAG_WRITE | PAG_EXECUTE))
        *pfdwOldProtect |= PAGE_EXECUTE_READWRITE;
  else
  if(pageFlags & PAG_EXECUTE)
        *pfdwOldProtect |= PAGE_EXECUTE_READ;

  if(pageFlags & PAG_GUARD)
        *pfdwOldProtect |= PAGE_GUARD;
  pageFlags = 0;

  if(fdwNewProtect & PAGE_READONLY)     pageFlags |= PAG_READ;
  if(fdwNewProtect & PAGE_READWRITE)    pageFlags |= (PAG_READ | PAG_WRITE);
  if(fdwNewProtect & PAGE_WRITECOPY)    pageFlags |= (PAG_READ | PAG_WRITE);
  if(fdwNewProtect & PAGE_EXECUTE_READ) pageFlags |= (PAG_EXECUTE | PAG_READ);
  if(fdwNewProtect & PAGE_EXECUTE_READWRITE)
        pageFlags |= (PAG_EXECUTE | PAG_WRITE | PAG_READ);
  if(fdwNewProtect & PAGE_EXECUTE_WRITECOPY)
        pageFlags |= (PAG_EXECUTE | PAG_WRITE | PAG_READ);
  if(fdwNewProtect & PAGE_GUARD)        pageFlags |= PAG_GUARD;
//Not supported in OS/2??
//  if(fdwNewProtect & PAGE_NOACCESS)

  dprintf(("New memory flags %X\n", pageFlags));
  if(pageFlags == 0) {
        dprintf(("pageFlags == 0\n"));
        return(TRUE);   //nothing to do
  }
  ULONG offset = ((ULONG)lpvAddress & 0xFFF);
  npages = (cbSize >> 12);
  if(cbSize & 0xFFF + offset) {
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
DWORD WIN32API VirtualQuery(LPCVOID lpvAddress, LPMEMORY_BASIC_INFORMATION pmbiBuffer,
                            DWORD cbLength)
{
 ULONG  cbRangeSize, dAttr;
 DWORD rc;

  if(lpvAddress == NULL || pmbiBuffer == NULL || cbLength == 0) {
   	return 0;
  }

  cbRangeSize = cbLength & ~0xFFF;
  if(cbLength & 0xFFF) {
	cbRangeSize += PAGE_SIZE;
  }
  rc = OSLibDosQueryMem((LPVOID)lpvAddress, &cbRangeSize, &dAttr);
  if(rc) {
   	dprintf(("VirtualQuery - DosQueryMem %x %x returned %d\n", lpvAddress, cbLength, rc));
   	return 0;
  }
  memset(pmbiBuffer, 0, sizeof(MEMORY_BASIC_INFORMATION));
  pmbiBuffer->BaseAddress = (LPVOID)lpvAddress;
  pmbiBuffer->RegionSize  = cbRangeSize;
  if(dAttr & PAG_READ && !(dAttr & PAG_WRITE))
        pmbiBuffer->Protect |= PAGE_READONLY;
  if(dAttr & PAG_WRITE)
        pmbiBuffer->Protect |= PAGE_READWRITE;

  if((dAttr & (PAG_WRITE | PAG_EXECUTE)) == (PAG_WRITE | PAG_EXECUTE))
        pmbiBuffer->Protect |= PAGE_EXECUTE_READWRITE;
  else
  if(dAttr & PAG_EXECUTE)
        pmbiBuffer->Protect |= PAGE_EXECUTE_READ;

  if(dAttr & PAG_GUARD)
        pmbiBuffer->Protect |= PAGE_GUARD;

  if(dAttr & PAG_FREE)
   	pmbiBuffer->State = MEM_FREE;
  else
  if(dAttr & PAG_COMMIT)
   	pmbiBuffer->State = MEM_COMMIT;
  else  pmbiBuffer->State = MEM_RESERVE;

  if(!(dAttr & PAG_SHARED))
   	pmbiBuffer->Type = MEM_PRIVATE;

  //TODO: This is not correct: AllocationProtect should contain the protection
  //      flags used in the initial call to VirtualAlloc
  pmbiBuffer->AllocationProtect = pmbiBuffer->Protect;
  if(dAttr & PAG_BASE) {
   	pmbiBuffer->AllocationBase = (LPVOID)lpvAddress;
  }
  else {
   while(lpvAddress > 0) {
      rc = OSLibDosQueryMem((LPVOID)lpvAddress, &cbRangeSize, &dAttr);
      if(rc) {
         dprintf(("VirtualQuery - OSLibDosQueryMem %x %x returned %d\n", lpvAddress, cbLength, rc));
         break;
      }
      if(dAttr & PAG_BASE) {
         pmbiBuffer->AllocationBase = (LPVOID)lpvAddress;
         break;
      }
      lpvAddress = (LPVOID)((ULONG)lpvAddress - PAGE_SIZE);
   }
  }
  return sizeof(MEMORY_BASIC_INFORMATION);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API VirtualLock( LPVOID lpAddress, DWORD dwSize )
{
  dprintf(("VirtualLock at %d; %d bytes - stub (TRUE)\n", (int)lpAddress, dwSize));
  return TRUE;
}

//******************************************************************************
BOOL WIN32API VirtualUnlock( LPVOID lpAddress, DWORD dwSize )
{
  dprintf(("VirtualUnlock at %d; %d bytes - stub (TRUE)\n", (int)lpAddress, dwSize));
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

BOOL WIN32API VirtualProtectEx(HANDLE hProcess,
                                  LPVOID lpvAddress,
                                  DWORD  cbSize,
                                  DWORD  fdwNewProtect,
                                  LPDWORD pfdwOldProtect)
{
  dprintf(("KERNEL32: VirtualProtectEx(%08x,%08xh,%08xh,%08xh,%08xh) not implemented for different processes.\n",
           hProcess,
           lpvAddress,
           cbSize,
           fdwNewProtect,
           pfdwOldProtect));

  return VirtualProtect(lpvAddress, cbSize, fdwNewProtect, pfdwOldProtect);
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

DWORD WIN32API VirtualQueryEx(HANDLE  hProcess,
                                 LPCVOID  lpvAddress,
                                 LPMEMORY_BASIC_INFORMATION pmbiBuffer,
                                 DWORD   cbLength)
{
  dprintf(("KERNEL32: VirtualQueryEx(%08x,%08xh,%08xh,%08xh) not implemented for different processes.\n",
           hProcess,
           lpvAddress,
           pmbiBuffer,
           cbLength));

  return VirtualQuery(lpvAddress, pmbiBuffer, cbLength);
}
//******************************************************************************
//******************************************************************************
