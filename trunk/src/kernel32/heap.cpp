/* $Id: heap.cpp,v 1.14 1999-08-21 16:29:26 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 heap API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#define  _WIN32
#include "os2heap.h"
#include <heap.h>

static HANDLE processheap = NULL;
OS2Heap *OS2ProcessHeap = NULL;

//******************************************************************************
//******************************************************************************
LPVOID WIN32API HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
 OS2Heap *curheap = OS2Heap::find(hHeap);

//  dprintf(("HeapAlloc %X bytes", dwBytes));
  if(curheap == NULL)
        return(NULL);

  return(curheap->Alloc(dwFlags, dwBytes));
}
//******************************************************************************
//******************************************************************************
LPVOID WIN32API HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, DWORD dwBytes)
{
 OS2Heap *curheap = OS2Heap::find(hHeap);

  if(curheap == NULL)
        return(NULL);

  return(curheap->ReAlloc(dwFlags, lpMem, dwBytes));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
 OS2Heap *curheap = OS2Heap::find(hHeap);

  if(curheap == NULL)
        return(FALSE);

  return(curheap->Free(dwFlags, lpMem));
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API HeapCreate(DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize)
{
 OS2Heap *curheap;

  //Create Open32 heap for it's handle
  HANDLE hHeap = O32_HeapCreate(flOptions, 0, 4096);
  if(hHeap == NULL)
    return(NULL);

  curheap = new OS2Heap(hHeap, flOptions, dwInitialSize, dwMaximumSize);

  if(curheap == NULL)
  {
      	O32_HeapDestroy(hHeap);
        return(NULL);
  }

  if(curheap->getHeapHandle() == NULL)
  {
    O32_HeapDestroy(hHeap);
    delete(curheap);
    return(NULL);
  }
  return(curheap->getHeapHandle());
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HeapDestroy(HANDLE hHeap)
{
 OS2Heap *curheap = OS2Heap::find(hHeap);

  if(curheap == NULL)
        return(FALSE);

  O32_HeapDestroy(curheap->getHeapHandle());
  delete(curheap);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API HeapSize(HANDLE hHeap, DWORD arg2, PVOID  arg3)
{
 OS2Heap *curheap = OS2Heap::find(hHeap);

  if(curheap == NULL)
        return(0);
  return curheap->Size(arg2, arg3);
}
//******************************************************************************
//TODO: Check this!!!
//******************************************************************************
DWORD WIN32API HeapCompact(HANDLE hHeap, DWORD dwFlags)
{
  dprintf(("KERNEL32:  HeapCompact: Unknown API - stub\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
  dprintf(("KERNEL32:  HeapValidate - stub\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HeapUnlock(HANDLE hHeap)
{
  dprintf(("KERNEL32:  HeapUnlock - stub (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HeapLock(HANDLE hHeap)
{
  dprintf(("KERNEL32:  HeapLock - stub (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//    LPPROCESS_HEAP_ENTRY lpEntry
//******************************************************************************
BOOL WIN32API HeapWalk(HANDLE hHeap, void *lpEntry)
{
  dprintf(("KERNEL32:  HeapWalk - stub (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetProcessHeap(VOID)
{
 HANDLE hHeap;

//    dprintf(("KERNEL32:  OS2GetProcessHeap\n"));
    //SvL: Only one process heap per process
    if(processheap == NULL) {
      //TODO: I haven't thought real hard about this.  I added it just to make "hdr.exe" happy.
      hHeap = O32_HeapCreate(HEAP_GENERATE_EXCEPTIONS, 1, 0x4000);

      OS2ProcessHeap = new OS2Heap(hHeap, HEAP_GENERATE_EXCEPTIONS, 0x4000, 0);

      if(OS2ProcessHeap == NULL) {
         	O32_HeapDestroy(hHeap);
               	return(NULL);
      }
      processheap = hHeap;
    }
    return(processheap);
}
//******************************************************************************
//******************************************************************************
HLOCAL WIN32API LocalAlloc(UINT fuFlags, DWORD cbBytes)
{
 HLOCAL lmem;
 DWORD  dwFlags = 0;

  if(processheap == NULL) {
      if(GetProcessHeap() == NULL)
         return(NULL);
  }
  if(fuFlags & LMEM_ZEROINIT)
    dwFlags = HEAP_ZERO_MEMORY;

  lmem = (HLOCAL)OS2ProcessHeap->Alloc(dwFlags, cbBytes, fuFlags);

  dprintf(("KERNEL32:  LocalAlloc flags %X, size %d returned %X\n", dwFlags, cbBytes, lmem));

  return(lmem);
}
//******************************************************************************
//******************************************************************************
HLOCAL WIN32API LocalDiscard(HLOCAL hMem)
{
    dprintf(("KERNEL32:  LocalDiscard\n"));

//    return O32_LocalDiscard(arg1);
    return(hMem);   //TODO: Possible memory leak
}
//******************************************************************************
//******************************************************************************
UINT WIN32API LocalFlags(HLOCAL hMem)
{
    dprintf(("KERNEL32: LocalFlags %X\n", hMem));

    return OS2ProcessHeap->GetFlags((LPVOID)hMem);
}
//******************************************************************************
//******************************************************************************
HLOCAL WIN32API LocalFree(HLOCAL hMem)
{
    dprintf(("KERNEL32: LocalFree %X\n", hMem));

    if(OS2ProcessHeap->GetLockCnt((LPVOID)hMem) != 0) {
      dprintf(("LocalFree, lock count != 0\n"));
      return(hMem);   //TODO: SetLastError
    }
    if(OS2ProcessHeap->Free(0, (LPVOID)hMem) == FALSE) {
      return(hMem);   //TODO: SetLastError
    }
    return NULL; //success
}
//******************************************************************************
//******************************************************************************
HLOCAL WIN32API LocalHandle(PCVOID lpMem)
{
    dprintf(("KERNEL32:  LocalHandle\n"));

    return (HLOCAL)lpMem;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LocalUnlock(HLOCAL hMem)
{
    dprintf(("KERNEL32: LocalUnlock %X\n", hMem));

    return OS2ProcessHeap->Unlock((LPVOID)hMem);
}
//******************************************************************************
//TODO: cbBytes==0 && fuFlags & LMEM_MOVEABLE not handled!!
//******************************************************************************
HLOCAL WIN32API LocalReAlloc(HLOCAL hMem, DWORD cbBytes, UINT fuFlags)
{
  HLOCAL hLocalNew;
  LPVOID lpMem;

  dprintf(("KERNEL32: LocalReAlloc %X %d %X\n", hMem, cbBytes, fuFlags));

  //SvL: 8-8-'98: Notepad bugfix (assumes address is identical when new size < old size)
  if(OS2ProcessHeap->Size(0, (LPVOID)hMem) > cbBytes)
    return hMem;

  hLocalNew = LocalAlloc(fuFlags, cbBytes);
  if (hLocalNew != 0)
  {
    lpMem = LocalLock(hLocalNew);

    if (lpMem != NULL) /* copy memory if successful */
      memcpy(lpMem,
             (LPVOID)hMem,
             min(cbBytes, OS2ProcessHeap->Size(0, (LPVOID)hMem))
            );

    LocalUnlock(hLocalNew);
    OS2ProcessHeap->Free(0, (LPVOID)hMem);
  }
  return(hLocalNew);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API LocalSize(HLOCAL hMem)
{
    dprintf(("KERNEL32: LocalSize %X\n", hMem));

    return OS2ProcessHeap->Size(0, (PVOID)hMem);
}
//******************************************************************************
//******************************************************************************
PVOID WIN32API LocalLock(HLOCAL hMem)
{
    dprintf(("KERNEL32:  LocalLock %X\n", hMem));

    OS2ProcessHeap->Lock((LPVOID)hMem);
    return (PVOID)hMem;
}
//******************************************************************************

//******************************************************************************
//* this function is here for completeness, some stupid software requires it.
UINT WIN32API LocalShrink(HLOCAL hMem,
                             UINT   cbNewSize)
{
    dprintf(("KERNEL32:  LocalShrink %X, %08xh - stub (cbNewSize)\n",
             hMem,
             cbNewSize));

    return cbNewSize;
}
//******************************************************************************

//******************************************************************************
//* this function is here for completeness, mIRC/32 requires it.
UINT WIN32API LocalCompact(UINT cbNewSize)
{
    dprintf(("KERNEL32:  LocalCompact %08xh - stub (cbNewSize)\n",
             cbNewSize));

    return cbNewSize;
}
//******************************************************************************
//******************************************************************************
HGLOBAL WIN32API GlobalAlloc(UINT fuFlags, DWORD dwBytes)
{
    dprintf(("KERNEL32:  GlobalAlloc %d\n", dwBytes));

    return O32_GlobalAlloc(fuFlags, dwBytes);
}
//******************************************************************************
//******************************************************************************
HGLOBAL WIN32API GlobalFree( HGLOBAL arg1)
{
    dprintf(("KERNEL32:  GlobalFree\n"));

    return O32_GlobalFree(arg1);
}
//******************************************************************************
//******************************************************************************
HGLOBAL WIN32API GlobalHandle( LPCVOID arg1)
{
    dprintf(("KERNEL32:  OS2GlobalHandle\n"));

    return O32_GlobalHandle((LPVOID)arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GlobalFlags(HGLOBAL arg1)
{
    dprintf(("KERNEL32:  OS2GlobalFlags\n"));

    return O32_GlobalFlags(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GlobalCompact(DWORD dwMinFree)
{
  dprintf(("KERNEL32:  GlobalCompact, OBSOLETE - stub\n"));

  return(0);
}
//******************************************************************************
//******************************************************************************
PVOID WIN32API GlobalLock(HGLOBAL arg1)
{
    dprintf(("KERNEL32: GlobalLock\n"));

    return O32_GlobalLock(arg1);
}
//******************************************************************************
//******************************************************************************
HGLOBAL WIN32API GlobalReAlloc(HGLOBAL arg1, DWORD arg2, UINT  arg3)
{
    dprintf(("KERNEL32:  GlobalReAlloc\n"));

    return O32_GlobalReAlloc(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GlobalSize(HGLOBAL arg1)
{
    dprintf(("KERNEL32:  GlobalSize\n"));

    return O32_GlobalSize(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GlobalUnlock(HGLOBAL arg1)
{
    dprintf(("KERNEL32:  GlobalUnlock\n"));

    return O32_GlobalUnlock(arg1);
}
//******************************************************************************
//******************************************************************************
