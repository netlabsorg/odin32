/* $Id: virtual.cpp,v 1.2 1999-08-24 12:23:25 sandervl Exp $ */

/*
 * Win32 virtual memory functions
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include "mmap.h"

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
 HANDLE dupHandle;

   if((hFile == -1 && size_low == 0) || size_high ||
       protect & (PAGE_READONLY|PAGE_READWRITE|PAGE_WRITECOPY|SEC_COMMIT|SEC_IMAGE|SEC_RESERVE|SEC_NOCACHE) ||
       ((protect & SEC_COMMIT) && (protect & SEC_RESERVE))) 
   {

	dprintf(("CreateFileMappingA: invalid parameter (combination)!"));
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;	
   }
   if(DuplicateHandle(GetCurrentProcess(), hFile, GetCurrentProcess(),
                        &dupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) 
   {
	dprintf(("CreateFileMappingA: DuplicateHandle failed!"));
	return 0;
   }
   return dupHandle;
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
    dprintf(("OpenFileMappingA: NOT IMPLEMENTED"));
    return 0;
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
 DWORD filesize, bytesread;
 LPSTR lpBuffer;

   filesize = GetFileSize(handle, 0);
   if(filesize == 0) {
	dprintf(("MapViewOfFileEx: filesize = 0!"));
	return 0;
   }
   lpBuffer = (LPSTR)VirtualAlloc(0, filesize, MEM_COMMIT, PAGE_READWRITE);
   if(lpBuffer == 0) {
	dprintf(("MapViewOfFileEx: VirtualAlloc failed (%d bytes)!", filesize));
	return 0;
   }
   if(ReadFile(handle, lpBuffer, filesize, &bytesread, NULL) == FALSE) {
	dprintf(("MapViewOfFileEx: ReadFile failed (%d bytes)!", filesize));
	VirtualFree(lpBuffer, 0, MEM_RELEASE);
	return 0;
   }
   return lpBuffer;
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
    dprintf(("FlushViewOfFile: NOT IMPLEMENTED"));
    return TRUE;
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
    if (!addr)
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    return VirtualFree(addr, 0, MEM_RELEASE);
}

/***********************************************************************
 *             VIRTUAL_MapFileW
 *
 * Helper function to map a file to memory:
 *  name			-	file name 
 *  [RETURN] ptr		-	pointer to mapped file
 */
LPVOID WINAPI VIRTUAL_MapFileW( LPCWSTR name )
{
    HANDLE hFile, hMapping;
    LPVOID ptr = NULL;

    hFile = CreateFileW( name, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hMapping = CreateFileMappingA( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
        CloseHandle( hFile );
        if (hMapping)
        {
            ptr = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );
            CloseHandle( hMapping );
        }
    }
    return ptr;
}

/***********************************************************************
 *             VIRTUAL_MapFileA
 *
 * Helper function to map a file to memory:
 *  name			-	file name 
 *  [RETURN] ptr		-	pointer to mapped file
 */
LPVOID WINAPI VIRTUAL_MapFileA( LPCSTR name )
{
    HANDLE hFile, hMapping;
    LPVOID ptr = NULL;

    hFile = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, NULL, 
                        OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hMapping = CreateFileMappingA( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
        CloseHandle( hFile );
        if (hMapping)
        {
            ptr = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );
            CloseHandle( hMapping );
        }
    }
    return ptr;
}
