
/*
 * Win32 heap API functions for OS/2
 *
 * Copyright 1999-2001 Sander van Leeuwen
 *
 * Global memory functions ported from Wine
 * Copyright 1995 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#define  _WIN32
#include "os2heap.h"
#include <heap.h>
#include "os2heap.h"
#include <odinwrap.h>
#include "initterm.h"

#define DBG_LOCALLOG	DBG_heap
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-HEAP)

//******************************************************************************
// Local variables
//******************************************************************************

static OS2Heap *OS2ProcessHeap = NULL;

static HANDLE i_initializeProcessHeap()
{
  HANDLE _processheap;
  
  //SvL: Only one process heap per process
  OS2ProcessHeap = new OS2Heap(HEAP_GENERATE_EXCEPTIONS, 0x4000, 0);

  if(OS2ProcessHeap == NULL)
    return(NULL);
  _processheap = OS2ProcessHeap->getHeapHandle();
  
  return _processheap;
}

static HANDLE processheap = i_initializeProcessHeap();

// This macro could be mapped to GetProcessHeap() if there
// is any change in functionality
#define GETPROCESSHEAP processheap


//******************************************************************************
// Fast Heap Handle Management
//******************************************************************************

extern HANDLE   fhhm_lastHandle;
extern OS2Heap* fhhm_lastHeap;

#define FINDHEAP(curheap, hHeap)     \
  if (fhhm_lastHeap &&               \
      hHeap == fhhm_lastHandle)      \
    curheap = fhhm_lastHeap;         \
  else                               \
    curheap = OS2Heap::find(hHeap);  \
  


//******************************************************************************
//******************************************************************************
LPVOID WIN32API HeapAlloc(HANDLE hHeap, 
                          DWORD dwFlags,
                          DWORD dwBytes)
{
  OS2Heap *curheap;
  FINDHEAP(curheap,hHeap)
  LPVOID   rc;

  if(curheap == NULL)
        return(NULL);

  rc = curheap->Alloc(dwFlags, dwBytes);
  dprintf2(("HeapAlloc %d bytes -> %x", dwBytes, rc));
  return rc;
}
//******************************************************************************
//******************************************************************************
LPVOID HeapReAlloc(HANDLE hHeap, 
                   DWORD dwFlags,
                   LPVOID lpMem, 
                   DWORD dwBytes)
{
  OS2Heap *curheap;
  FINDHEAP(curheap,hHeap)

  dprintf2(("HeapReAlloc %x %x %x %X bytes", hHeap, dwFlags, lpMem, dwBytes));
  if(curheap == NULL)
        return(NULL);

  //Some apps (i.e. nt's cmd.exe) assume the returned pointer is the same as the old one!
  if(lpMem && curheap->Size(0, lpMem) > dwBytes)
        return lpMem;

  return(curheap->ReAlloc(dwFlags, lpMem, dwBytes));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HeapFree(HANDLE hHeap, 
                       DWORD dwFlags,
                       LPVOID lpMem)
{
  OS2Heap *curheap;
  FINDHEAP(curheap,hHeap)

  dprintf2(("HeapFree %X", lpMem));
  if(curheap == NULL)
    return(FALSE);

  BOOL fResult = curheap->Free(dwFlags, lpMem);
  if (fResult == FALSE)
    SetLastError(ERROR_INVALID_HANDLE); /// @@@PH possibly wrong return code!
  
  return fResult;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG3(HANDLE, HeapCreate, DWORD, flOptions, DWORD, dwInitialSize, 
                   DWORD, dwMaximumSize)
{
 OS2Heap *curheap;

  //Create Open32 heap for it's handle
  dprintf2(("HeapCreate dwInitialSize %X", dwInitialSize));

  curheap = new OS2Heap(flOptions, dwInitialSize, dwMaximumSize);

  if(curheap == NULL)
  {
        return(NULL);
  }

  if(curheap->getHeapHandle() == NULL)
  {
    delete(curheap);
    return(NULL);
  }
  return(curheap->getHeapHandle());
}
//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG1(BOOL, HeapDestroy, HANDLE, hHeap)
{
  OS2Heap *curheap;
  FINDHEAP(curheap,hHeap)

  dprintf2(("HeapDestroy %X", hHeap));
  if(curheap == NULL)
        return(FALSE);

  delete(curheap);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG3(DWORD, HeapSize, HANDLE, hHeap, DWORD, arg2, PVOID, lpMem)
{
  OS2Heap *curheap;
  FINDHEAP(curheap,hHeap)

  dprintf2(("HeapSize %X %x %x", hHeap, arg2, lpMem));
  if(curheap == NULL)
        return(0);
  return curheap->Size(arg2, lpMem);
}
//******************************************************************************
//TODO: Check this!!!
//******************************************************************************
ODINFUNCTIONNODBG2(DWORD, HeapCompact, HANDLE, hHeap, DWORD, dwFlags)
{
  dprintf(("KERNEL32:  HeapCompact: Unknown API - stub\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG3(BOOL, HeapValidate, HANDLE, hHeap, DWORD, dwFlags, LPCVOID, lpMem)
{
  OS2Heap *curheap;
  FINDHEAP(curheap,hHeap)

  dprintf2(("KERNEL32: HeapValidate %x %x %x", hHeap, dwFlags, lpMem));
  if(curheap == NULL)
        return FALSE;

  return curheap->Validate(dwFlags, lpMem);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG1(BOOL, HeapUnlock, HANDLE, hHeap)
{
  dprintf(("KERNEL32:  HeapUnlock - stub (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG1(BOOL, HeapLock, HANDLE, hHeap)
{
  dprintf(("KERNEL32:  HeapLock - stub (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//    LPPROCESS_HEAP_ENTRY lpEntry
//******************************************************************************
ODINFUNCTIONNODBG2(BOOL, HeapWalk, HANDLE, hHeap, LPVOID, lpEntry)
{
  dprintf(("KERNEL32:  HeapWalk - stub (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetProcessHeap()
{
//    dprintf2(("KERNEL32: GetProcessHeap\n"));
    return(processheap);
}
#if 1
/*
 * Win32 Global heap functions (GlobalXXX).
 * These functions included in Win32 for compatibility with 16 bit Windows
 * Especially the moveable blocks and handles are oldish. 
 * But the ability to directly allocate memory with GPTR and LPTR is widely
 * used.
 *
 * The handle stuff looks horrible, but it's implemented almost like Win95
 * does it. 
 *
 */

#define MAGIC_GLOBAL_USED 0x5342
#define GLOBAL_LOCK_MAX   0xFF
#define HANDLE_TO_INTERN(h)  ((PGLOBAL32_INTERN)(((char *)(h))-2))
#define INTERN_TO_HANDLE(i)  ((HGLOBAL) &((i)->Pointer))
#define ISHANDLE(h)          (((DWORD)(h)&2)!=0)
#define ISPOINTER(h)         (((DWORD)(h)&2)==0)

//SvL: -2 for 8 byte alignment
#define POINTER_TO_HANDLE(p) (*(((HGLOBAL *)(p))-2))
#define HGLOBAL_SIZE         2*sizeof(HGLOBAL)

#pragma pack(1)

typedef struct __GLOBAL32_INTERN
{
   WORD         Magic;
   LPVOID       Pointer;
   BYTE         Flags;
   BYTE         LockCount;
} GLOBAL32_INTERN, *PGLOBAL32_INTERN;

#pragma pack()

/***********************************************************************
 *           GlobalAlloc   (KERNEL32.@)
 * RETURNS
 *	Handle: Success
 *	NULL: Failure
 */
ODINFUNCTION2(HGLOBAL, GlobalAlloc,
              UINT, flags,
              DWORD, size)
{
   PGLOBAL32_INTERN     pintern;
   DWORD		hpflags;
   LPVOID               palloc;

   if(flags&GMEM_ZEROINIT)
      hpflags=HEAP_ZERO_MEMORY;
   else
      hpflags=0;
    
   if((flags & GMEM_MOVEABLE)==0) /* POINTER */
   {
      palloc=HeapAlloc(GETPROCESSHEAP, hpflags, size);
      return (HGLOBAL) palloc;
   }
   else  /* HANDLE */
   {
      /* HeapLock(heap); */

      pintern=(PGLOBAL32_INTERN)HeapAlloc(GETPROCESSHEAP, 0,  sizeof(GLOBAL32_INTERN));
      if (!pintern) return 0;
      if(size)
      {
         //SvL: 2*sizeof for 8 byte alignment
	 if (!(palloc=HeapAlloc(GETPROCESSHEAP, hpflags, size+HGLOBAL_SIZE))) {
	    HeapFree(GETPROCESSHEAP, 0, pintern);
	    return 0;
	 }
	 *(HGLOBAL *)palloc=INTERN_TO_HANDLE(pintern);
         //SvL: 2*sizeof for 8 byte alignment
	 pintern->Pointer=(char *) palloc+HGLOBAL_SIZE;
      }
      else
	 pintern->Pointer=NULL;
      pintern->Magic=MAGIC_GLOBAL_USED;
      pintern->Flags=flags>>8;
      pintern->LockCount=0;
      
      /* HeapUnlock(heap); */
       
      return INTERN_TO_HANDLE(pintern);
   }
}
/***********************************************************************
 *           GlobalLock   (KERNEL32.@)
 * RETURNS
 *	Pointer to first byte of block
 *	NULL: Failure
 */
ODINFUNCTION1(LPVOID, GlobalLock,
              HGLOBAL, hmem)
{
   PGLOBAL32_INTERN pintern;
   LPVOID           palloc;


   if(hmem == NULL || ISPOINTER(hmem)) {
      dprintf(("KERNEL32: GlobalLock %x returned %x", hmem, hmem));
      return (LPVOID) hmem;
   }

   /* verify lpMem address */
   if (hmem >= (HGLOBAL)ulMaxAddr || hmem < (HGLOBAL)0x10000)
   {
    	dprintf(("GlobalLock ERROR BAD HEAP POINTER:%X\n", hmem));
        DebugInt3();
    	return 0;
   }

   /* HeapLock(GETPROCESSHEAP); */
   
   pintern=HANDLE_TO_INTERN(hmem);
   if(pintern->Magic==MAGIC_GLOBAL_USED)
   {
      if(pintern->LockCount<GLOBAL_LOCK_MAX)
	 pintern->LockCount++;
      palloc=pintern->Pointer;
   }
   else
   {
      dprintf(("ERROR: GlobalLock invalid handle %x", hmem));
      palloc=(LPVOID) NULL;
      SetLastError(ERROR_INVALID_HANDLE);
   }
   /* HeapUnlock(GETPROCESSHEAP); */;

   dprintf(("KERNEL32: GlobalLock %x returned %x", hmem, palloc));
   return palloc;
}


/***********************************************************************
 *           GlobalUnlock   (KERNEL32.@)
 * RETURNS
 *	TRUE: Object is still locked
 *	FALSE: Object is unlocked
 */
ODINFUNCTION1(BOOL, GlobalUnlock,
              HGLOBAL, hmem)
{
   PGLOBAL32_INTERN       pintern;
   BOOL                 locked;

   if(hmem == NULL || ISPOINTER(hmem))
      return FALSE;

   /* verify lpMem address */
   if (hmem >= (HGLOBAL)ulMaxAddr || hmem < (HGLOBAL)0x10000)
   {
    	dprintf(("GlobalUnlock ERROR BAD HEAP POINTER:%X\n", hmem));
        DebugInt3();
    	return 0;
   }

   /* HeapLock(GETPROCESSHEAP); */
   pintern=HANDLE_TO_INTERN(hmem);
   
   if(pintern->Magic==MAGIC_GLOBAL_USED)
   {
      if((pintern->LockCount<GLOBAL_LOCK_MAX)&&(pintern->LockCount>0))
	 pintern->LockCount--;

      locked = (pintern->LockCount != 0);
      if (!locked) SetLastError(NO_ERROR);
   }
   else
   {
      dprintf(("ERROR: GlobalUnlock invalid handle %x", hmem));
      SetLastError(ERROR_INVALID_HANDLE);
      locked=FALSE;
   }
   /* HeapUnlock(GETPROCESSHEAP); */
   return locked;
}


/***********************************************************************
 *           GlobalHandle   (KERNEL32.@)
 * Returns the handle associated with the specified pointer.
 *
 * RETURNS
 *	Handle: Success
 *	NULL: Failure
 */
ODINFUNCTION1(HGLOBAL, GlobalHandle,
              LPCVOID, pmem)
{
    HGLOBAL handle;
    PGLOBAL32_INTERN  maybe_intern;
    LPCVOID test;

    if (!pmem)
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

#ifdef __WIN32OS2__
        handle = 0;

        /* note that if pmem is a pointer to a a block allocated by        */
        /* GlobalAlloc with GMEM_MOVEABLE then magic test in HeapValidate  */
        /* will fail.                                                      */
        if (ISPOINTER(pmem)) {
            if (HeapValidate( GETPROCESSHEAP, 0, pmem )) {
                handle = (HGLOBAL)pmem;  /* valid fixed block */
                return handle;
            }
            handle = POINTER_TO_HANDLE(pmem);
        } else
            handle = (HGLOBAL)pmem;

        /* Now test handle either passed in or retrieved from pointer */
        maybe_intern = HANDLE_TO_INTERN( handle );
        if(IsBadReadPtr(maybe_intern, sizeof(GLOBAL32_INTERN))) {
            SetLastError( ERROR_INVALID_HANDLE );
            return 0;
        }

        if (maybe_intern->Magic == MAGIC_GLOBAL_USED) {
            test = maybe_intern->Pointer;
            //SvL: -2 for 8 byte alignment
            if (HeapValidate( GETPROCESSHEAP, 0, ((HGLOBAL *)test)-2 ) && /* obj(-handle) valid arena? */
                HeapValidate( GETPROCESSHEAP, 0, maybe_intern ))  /* intern valid arena? */
            {
                return handle;
            }
        }
        handle = 0;
        SetLastError( ERROR_INVALID_HANDLE );
#else
    __TRY
    {
        handle = 0;

        /* note that if pmem is a pointer to a a block allocated by        */
        /* GlobalAlloc with GMEM_MOVEABLE then magic test in HeapValidate  */
        /* will fail.                                                      */
        if (ISPOINTER(pmem)) {
            if (HeapValidate( GETPROCESSHEAP, 0, pmem )) {
                handle = (HGLOBAL)pmem;  /* valid fixed block */
                break;
            }
            handle = POINTER_TO_HANDLE(pmem);
        } else
            handle = (HGLOBAL)pmem;

        /* Now test handle either passed in or retrieved from pointer */
        maybe_intern = HANDLE_TO_INTERN( handle );
        if (maybe_intern->Magic == MAGIC_GLOBAL_USED) {
            test = maybe_intern->Pointer;
            if (HeapValidate( GETPROCESSHEAP, 0, ((HGLOBAL *)test)-1 ) && /* obj(-handle) valid arena? */
                HeapValidate( GETPROCESSHEAP, 0, maybe_intern ))  /* intern valid arena? */
                break;  /* valid moveable block */
        }
        handle = 0;
        SetLastError( ERROR_INVALID_HANDLE );
    }
    __EXCEPT(page_fault)
    {
        SetLastError( ERROR_INVALID_HANDLE );
        return 0;
    }
    __ENDTRY
#endif
    return handle;
}


/***********************************************************************
 *           GlobalReAlloc   (KERNEL32.@)
 * RETURNS
 *	Handle: Success
 *	NULL: Failure
 */
ODINFUNCTION3(HGLOBAL, GlobalReAlloc,
                 HGLOBAL, hmem,
                 DWORD, size,
                 UINT, flags)
{
   LPVOID               palloc;
   HGLOBAL            hnew;
   PGLOBAL32_INTERN     pintern;
   DWORD heap_flags = (flags & GMEM_ZEROINIT) ? HEAP_ZERO_MEMORY : 0;

#ifdef __WIN32OS2__
/* @@@PH 20011017
  convert a handle to a handle ???
  hmem = GlobalHandle((LPCVOID)hmem);
  */
#endif

   hnew = 0;
   /* HeapLock(heap); */
   if(flags & GMEM_MODIFY) /* modify flags */
   {
      if( ISPOINTER(hmem) && (flags & GMEM_MOVEABLE))
      {
	 /* make a fixed block moveable
	  * actually only NT is able to do this. But it's soo simple
	  */
         if (hmem == 0)
         {
	     dprintf(("ERROR: GlobalReAlloc32 with null handle!\n"));
             SetLastError( ERROR_NOACCESS );
    	     return 0;
         }
	 size=HeapSize(GETPROCESSHEAP, 0, (LPVOID) hmem);
	 hnew=GlobalAlloc( flags, size);
	 palloc=GlobalLock(hnew);
	 memcpy(palloc, (LPVOID) hmem, size);
	 GlobalUnlock(hnew);
	 GlobalFree(hmem);
      }
      else if( ISPOINTER(hmem) &&(flags & GMEM_DISCARDABLE))
      {
	 /* change the flags to make our block "discardable" */
	 pintern=HANDLE_TO_INTERN(hmem);
	 pintern->Flags = pintern->Flags | (GMEM_DISCARDABLE >> 8);
	 hnew=hmem;
      }
      else
      {
	 SetLastError(ERROR_INVALID_PARAMETER);
	 hnew = 0;
      }
   }
   else
   {
      if(ISPOINTER(hmem))
      {
	 /* reallocate fixed memory */
	 hnew=(HGLOBAL)HeapReAlloc(GETPROCESSHEAP, heap_flags, (LPVOID) hmem, size);
      }
      else
      {
	 /* reallocate a moveable block */
	 pintern=HANDLE_TO_INTERN(hmem);

#if 0
/* Apparently Windows doesn't care whether the handle is locked at this point */
/* See also the same comment in GlobalFree() */
	 if(pintern->LockCount>1) {
	    ERR("handle 0x%08lx is still locked, cannot realloc!\n",(DWORD)hmem);
	    SetLastError(ERROR_INVALID_HANDLE);
	 } else
#endif
         if(size!=0)
	 {
	    hnew=hmem;
	    if(pintern->Pointer)
	    {
	       if((palloc = HeapReAlloc(GETPROCESSHEAP, heap_flags,
				   (char *) pintern->Pointer-HGLOBAL_SIZE,
				   size+HGLOBAL_SIZE)) == NULL)
		   return 0; /* Block still valid */
	       pintern->Pointer=(char *) palloc+HGLOBAL_SIZE;
	    }
	    else
	    {
	        if((palloc=HeapAlloc(GETPROCESSHEAP, heap_flags, size+HGLOBAL_SIZE))
		   == NULL)
		    return 0;
	       *(HGLOBAL *)palloc=hmem;
	       pintern->Pointer=(char *) palloc+HGLOBAL_SIZE;
	    }
	 }
	 else
	 {
	    if(pintern->Pointer)
	    {
	       HeapFree(GETPROCESSHEAP, 0, (char *) pintern->Pointer-HGLOBAL_SIZE);
	       pintern->Pointer=NULL;
	    }
	 }
      }
   }
   /* HeapUnlock(heap); */
   return hnew;
}


/***********************************************************************
 *           GlobalFree   (KERNEL32.@)
 * RETURNS
 *	NULL: Success
 *	Handle: Failure
 */
ODINFUNCTION1(HGLOBAL, GlobalFree,
              HGLOBAL, hmem)
{
   PGLOBAL32_INTERN pintern;
   HGLOBAL        hreturned = 0;
  
  // 2001-10-17 PH
  // Note: we do have a *HANDLE* here still ...
  // any may not terminate w/o setting SetLastError()
  // plus returning 0 means "OK"!
#if 0
   /* verify lpMem address */
   if (hmem >= (HGLOBAL)ulMaxAddr || hmem < (HGLOBAL)0x10000)
   {
    	dprintf(("GlobalFree ERROR BAD HEAP POINTER:%X\n", hmem));
//        DebugInt3();
    	return 0;
   }
#endif
  
   if(ISPOINTER(hmem)) /* POINTER */
   {
     if(HeapFree(GETPROCESSHEAP, 0, (LPVOID) hmem) == TRUE)
       hreturned = 0;    // success
     else
       hreturned = hmem; // failure
   }
   else  /* HANDLE */
   {
      /* HeapLock(heap); */
      pintern=HANDLE_TO_INTERN(hmem);
      
      if(pintern->Magic==MAGIC_GLOBAL_USED)
      {	 

/* WIN98 does not make this test. That is you can free a */
/* block you have not unlocked. Go figure!!              */
      /* if(pintern->LockCount!=0)  */
      /*    SetLastError(ERROR_INVALID_HANDLE);  */

	 if(pintern->Pointer)
	    if(!HeapFree(GETPROCESSHEAP, 0, (char *)(pintern->Pointer)-HGLOBAL_SIZE))
	       hreturned=hmem;
	 if(!HeapFree(GETPROCESSHEAP, 0, pintern))
	    hreturned=hmem;
      }      
      else
      {
        // this was not a heap handle!
        SetLastError(ERROR_INVALID_HANDLE);
        hreturned = hmem;
      }
     
      /* HeapUnlock(heap); */
   }
  
   return hreturned;
}


/***********************************************************************
 *           GlobalSize   (KERNEL32.@)
 * RETURNS
 *	Size in bytes of the global memory object
 *	0: Failure
 */
ODINFUNCTION1(DWORD, GlobalSize,
              HGLOBAL, hmem)
{
   DWORD                retval;
   PGLOBAL32_INTERN     pintern;

   if(ISPOINTER(hmem)) 
   {
      retval=HeapSize(GETPROCESSHEAP, 0,  (LPVOID) hmem);
   }
   else
   {
      /* HeapLock(heap); */
      pintern=HANDLE_TO_INTERN(hmem);
      
      if(pintern->Magic==MAGIC_GLOBAL_USED)
      {
        if (!pintern->Pointer) /* handle case of GlobalAlloc( ??,0) */
            return 0;
	 retval=HeapSize(GETPROCESSHEAP, 0,
	                 (char *)(pintern->Pointer)-HGLOBAL_SIZE)-HGLOBAL_SIZE;
         //SvL: ???
	 if (retval == 0xffffffff-HGLOBAL_SIZE) retval = 0;
      }
      else
      {
         dprintf(("ERROR: GlobalSize invalid handle %x", hmem));
	 retval=0;
      }
      /* HeapUnlock(heap); */
   }
   /* HeapSize returns 0xffffffff on failure */
   if (retval == 0xffffffff) retval = 0;
   dprintf(("KERNEL32: GlobalSize %x = %d", hmem, retval));
   return retval;
}


/***********************************************************************
 *           GlobalWire   (KERNEL32.@)
 */
ODINFUNCTION1(LPVOID, GlobalWire,
              HGLOBAL, hmem)
{
   return GlobalLock( hmem );
}


/***********************************************************************
 *           GlobalUnWire   (KERNEL32.@)
 */
ODINFUNCTION1(BOOL, GlobalUnWire,
              HGLOBAL, hmem)
{
   return GlobalUnlock( hmem);
}


/***********************************************************************
 *           GlobalFix   (KERNEL32.@)
 */
ODINPROCEDURE1(GlobalFix,
               HGLOBAL, hmem)
{
    GlobalLock( hmem );
}


/***********************************************************************
 *           GlobalUnfix   (KERNEL32.@)
 */
ODINPROCEDURE1(GlobalUnfix,
               HGLOBAL, hmem)
{
   GlobalUnlock( hmem);
}


/***********************************************************************
 *           GlobalFlags   (KERNEL32.@)
 * Returns information about the specified global memory object
 *
 * NOTES
 *	Should this return GMEM_INVALID_HANDLE on invalid handle?
 *
 * RETURNS
 *	Value specifying allocation flags and lock count
 *	GMEM_INVALID_HANDLE: Failure
 */
ODINFUNCTION1(UINT, GlobalFlags,
              HGLOBAL, hmem)
{
   DWORD                retval;
   PGLOBAL32_INTERN     pintern;

   if(ISPOINTER(hmem))
   {
      retval=0;
   }
   else
   {
      /* HeapLock(GETPROCESSHEAP); */
      pintern=HANDLE_TO_INTERN(hmem);
      if(pintern->Magic==MAGIC_GLOBAL_USED)
      {               
	 retval=pintern->LockCount + (pintern->Flags<<8);
	 if(pintern->Pointer==0)
	    retval|= GMEM_DISCARDED;
      }
      else
      {
         dprintf(("ERROR: GlobalFlags invalid handle %x", hmem));
	 retval=0;
      }
      /* HeapUnlock(GETPROCESSHEAP); */
   }
   return retval;
}


/***********************************************************************
 *           GlobalCompact   (KERNEL32.@)
 */
ODINFUNCTION1(DWORD, GlobalCompact,
              DWORD, minfree )
{
    return 0;  /* GlobalCompact does nothing in Win32 */
}
#else
//******************************************************************************
#ifdef DEBUG
static ULONG totalGlobalAlloc = 0;
#endif
//******************************************************************************
ODINFUNCTION2(HGLOBAL, GlobalAlloc,
              UINT, fuFlags, 
              DWORD, dwBytes)
{
 HGLOBAL ret;

    ret = O32_GlobalAlloc(fuFlags, dwBytes);
#ifdef DEBUG
    totalGlobalAlloc += dwBytes;
#endif
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HGLOBAL, GlobalFree,
              HGLOBAL, arg1)
{
 HGLOBAL ret;

#ifdef DEBUG
    totalGlobalAlloc -= O32_GlobalSize(arg1);
#endif
    ret = O32_GlobalFree(arg1);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HGLOBAL, GlobalHandle,
              LPCVOID, lpMem)
{
    return O32_GlobalHandle((LPVOID)lpMem);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(UINT, GlobalFlags,
              HGLOBAL, hMem)
{
    return O32_GlobalFlags(hMem);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GlobalCompact,
              DWORD, dwMinFree)
{
  return(0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(PVOID, GlobalLock,
              HGLOBAL, hMem)
{
  return O32_GlobalLock(hMem);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(HGLOBAL, GlobalReAlloc,
              HGLOBAL, arg1,
              DWORD, arg2,
              UINT,  arg3)
{
    return O32_GlobalReAlloc(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GlobalSize,
              HGLOBAL, arg1)
{
    return O32_GlobalSize(arg1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, GlobalUnlock,
              HGLOBAL, arg1)
{
    return O32_GlobalUnlock(arg1);
}
/***********************************************************************
 *           GlobalWire
 *
 * The GlobalWire function is obsolete. It is provided only for compatibility 
 * with 16-bit versions of Windows. Applications that need to lock a global 
 * memory object should use the GlobalLock and GlobalUnlock functions. 
 *
 */
ODINFUNCTION1(LPVOID, GlobalWire,
              HGLOBAL, hmem)
{
   return GlobalLock( hmem );
}
/***********************************************************************
 *           GlobalUnWire
 *
 * The GlobalUnWire function is obsolete. It is provided only for compatibility 
 * with 16-bit versions of Windows. Applications that need to lock a global 
 * memory object should use the GlobalLock and GlobalUnlock functions. 
 *
 */
ODINFUNCTION1(BOOL, GlobalUnWire,
              HGLOBAL, hmem)
{
   return GlobalUnlock( hmem);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HGLOBAL, GlobalDiscard,
              HGLOBAL, hGlobal)
{
    return O32_GlobalDiscard(hGlobal);
}
/***********************************************************************
 *           GlobalFix   (KERNEL32.@)
 */
ODINPROCEDURE1(GlobalFix,
               HGLOBAL, hmem)
{
    GlobalLock( hmem );
}
/***********************************************************************
 *           GlobalUnfix   (KERNEL32.@)
 */
ODINPROCEDURE1(GlobalUnfix,
               HGLOBAL, hmem)
{
   GlobalUnlock( hmem);
}
#endif
//******************************************************************************
//******************************************************************************
HLOCAL WIN32API LocalAlloc(UINT fuFlags, DWORD cbBytes)
{
    HLOCAL hLocal;

    // Check flags
    if(fuFlags & (~(LMEM_MOVEABLE | LMEM_DISCARDABLE | LMEM_NOCOMPACT |
                    LMEM_NODISCARD | LMEM_ZEROINIT)))
    {
       dprintf(("LocalAlloc %x %x: INVALID flags!", fuFlags, cbBytes));
       SetLastError(ERROR_INVALID_PARAMETER);
       return 0;
    }
    // Note: local & global flags are the same (the ones used here), so no need for conversion
    hLocal = GlobalAlloc(fuFlags, cbBytes);
    dprintf(("KERNEL32: LocalAlloc flags %X, size %d returned %X", fuFlags, cbBytes, hLocal));
    return hLocal;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API LocalFlags(HLOCAL hMem)
{
    UINT ret, retG;
    dprintf(("KERNEL32: LocalFlags %X\n", hMem));

    retG = GlobalFlags(hMem);

    if(retG == GMEM_INVALID_HANDLE)
    {
       return LMEM_INVALID_HANDLE;
    }
    // Low byte holds lock count.
    // Hi byte of low word contains some flags.
    ret = retG & 0xff; // Keep lock count.
    if (retG & GMEM_DISCARDABLE)  ret |= LMEM_DISCARDABLE;
    if (retG & GMEM_DISCARDED)    ret |= LMEM_DISCARDED;

    return ret;
}
//******************************************************************************
//******************************************************************************
HLOCAL WIN32API LocalFree(HLOCAL hMem)
{
  dprintf(("KERNEL32: LocalFree %X", hMem));

  return GlobalFree(hMem);
}
//******************************************************************************
//******************************************************************************
HLOCAL WIN32API LocalHandle(PCVOID lpMem)
{
    dprintf(("KERNEL32: LocalHandle %x", lpMem));

    return GlobalHandle(lpMem);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LocalUnlock(HLOCAL hMem)
{
    dprintf(("KERNEL32: LocalUnlock %X", hMem));

    return GlobalUnlock(hMem);
}
//******************************************************************************
//TODO: cbBytes==0 && fuFlags & LMEM_MOVEABLE not handled!!
//******************************************************************************
HLOCAL WIN32API LocalReAlloc(HLOCAL hMem, DWORD cbBytes, UINT fuFlags)
{
  HLOCAL hLocalNew, hLocalOld;
  LPVOID lpMem, lpMemOld;
  DWORD  cbOldSize;

    dprintf(("KERNEL32: LocalReAlloc %X %d %X\n", hMem, cbBytes, fuFlags));

    // Check flags
    if(fuFlags & (~(LMEM_MOVEABLE | LMEM_DISCARDABLE | LMEM_NOCOMPACT |
                    LMEM_MODIFY | LMEM_ZEROINIT)))
    {
       dprintf(("LocalAlloc %x %x: INVALID flags!", fuFlags, cbBytes));
       SetLastError(ERROR_INVALID_PARAMETER);
       return 0;
    }

    //SvL: 8-8-'98: Notepad bugfix (assumes address is identical when new size < old size)
    cbOldSize = LocalSize(hMem);
    if(cbOldSize > cbBytes)
        return hMem;

    return GlobalReAlloc(hMem, cbBytes, fuFlags);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API LocalSize(HLOCAL hMem)
{
    dprintf(("KERNEL32: LocalSize %X", hMem));

    return GlobalSize(hMem);
}
//******************************************************************************
//******************************************************************************
PVOID WIN32API LocalLock(HLOCAL hMem)
{
    dprintf(("KERNEL32:  LocalLock %X\n", hMem));

    return GlobalLock(hMem);
}
//******************************************************************************
//* this function is here for completeness, some stupid software requires it.
//******************************************************************************
UINT WIN32API LocalShrink(HLOCAL hMem, UINT cbNewSize)
{
    dprintf(("KERNEL32:  LocalShrink %X, %08xh - OBSOLETE", hMem, cbNewSize));

    return cbNewSize;
}
//******************************************************************************
//* this function is here for completeness, mIRC/32 requires it.
//******************************************************************************
UINT WIN32API LocalCompact(UINT cbNewSize)
{
    dprintf(("KERNEL32:  LocalCompact %08xh - OBSOLETE", cbNewSize));

    return cbNewSize;
}
//******************************************************************************
//******************************************************************************
