/* $Id: rtl.cpp,v 1.10 2000-08-02 16:31:34 bird Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * NT basis DLL
 *
 * This file contains the Rtl* API functions. These should be implementable.
 *
 * Copyright 1996-1998 Marcus Meissner
 *      1999 Alex Korobka
 */

#include <stdlib.h>
#include <string.h>
#include <odinwrap.h>

#include "ntdll.h"

#include "winuser.h"
#include "winerror.h"
#include "winreg.h"
#include "heapstring.h"

ODINDEBUGCHANNEL(NTDLL-RTL)

/*
 * resource functions
 */

/***********************************************************************
 *           RtlInitializeResource       (NTDLL.409)
 *
 * xxxResource() functions implement multiple-reader-single-writer lock.
 * The code is based on information published in WDJ January 1999 issue.
 */
void WINAPI RtlInitializeResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlInitializeResource(%08xh)\n",
           rwl));

  if( rwl )
  {
    rwl->iNumberActive     = 0;
    rwl->uExclusiveWaiters = 0;
    rwl->uSharedWaiters    = 0;
    rwl->hOwningThreadId   = 0;
    rwl->dwTimeoutBoost    = 0; /* no info on this one, default value is 0 */
    InitializeCriticalSection( &rwl->rtlCS );
    rwl->hExclusiveReleaseSemaphore = CreateSemaphoreA( NULL, 0, 65535, NULL );
    rwl->hSharedReleaseSemaphore    = CreateSemaphoreA( NULL, 0, 65535, NULL );
  }
}


/***********************************************************************
 *           RtlDeleteResource              (NTDLL.330)
 */
void WINAPI RtlDeleteResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlDeleteResource(%08xh)\n",
           rwl));

  if( rwl )
  {
    EnterCriticalSection( &rwl->rtlCS );
    if( rwl->iNumberActive || rwl->uExclusiveWaiters || rwl->uSharedWaiters )
       dprintf(("NTDLL: RtlDeleteResource active MRSW lock (%p), expect failure\n",
                rwl));

    rwl->hOwningThreadId   = 0;
    rwl->uExclusiveWaiters = rwl->uSharedWaiters = 0;
    rwl->iNumberActive     = 0;
    CloseHandle( rwl->hExclusiveReleaseSemaphore );
    CloseHandle( rwl->hSharedReleaseSemaphore );
    LeaveCriticalSection( &rwl->rtlCS );
    DeleteCriticalSection( &rwl->rtlCS );
  }
}


/***********************************************************************
 *          RtlAcquireResourceExclusive  (NTDLL.256)
 */
BYTE WINAPI RtlAcquireResourceExclusive(LPRTL_RWLOCK rwl,
                                        BYTE         fWait)
{
  BYTE retVal = 0;

  if( !rwl )
    return 0;

  dprintf(("NTDLL: RtlAcquireResourceExclusive(%08xh,%08xh)\n",
           rwl,
           fWait));

start:
    EnterCriticalSection( &rwl->rtlCS );
    if( rwl->iNumberActive == 0 ) /* lock is free */
    {
   rwl->iNumberActive = -1;
   retVal = 1;
    }
    else if( rwl->iNumberActive < 0 ) /* exclusive lock in progress */
    {
    if( rwl->hOwningThreadId == GetCurrentThreadId() )
    {
        retVal = 1;
        rwl->iNumberActive--;
        goto done;
    }
wait:
    if( fWait )
    {
        rwl->uExclusiveWaiters++;

        LeaveCriticalSection( &rwl->rtlCS );
        if( WaitForSingleObject( rwl->hExclusiveReleaseSemaphore, INFINITE ) == WAIT_FAILED )
       goto done;
        goto start; /* restart the acquisition to avoid deadlocks */
    }
    }
    else  /* one or more shared locks are in progress */
    if( fWait )
        goto wait;

    if( retVal == 1 )
   rwl->hOwningThreadId = GetCurrentThreadId();
done:
    LeaveCriticalSection( &rwl->rtlCS );
    return retVal;
}

/***********************************************************************
 *          RtlAcquireResourceShared     (NTDLL.257)
 */
BYTE WINAPI RtlAcquireResourceShared(LPRTL_RWLOCK rwl,
                                     BYTE         fWait)
{
  DWORD dwWait = WAIT_FAILED;
  BYTE retVal = 0;

  if( !rwl )
    return 0;

  dprintf(("NTDLL: RtlAcquireResourceShared(%08xh,%08xh)\n",
           rwl,
           fWait));

start:
    EnterCriticalSection( &rwl->rtlCS );
    if( rwl->iNumberActive < 0 )
    {
   if( rwl->hOwningThreadId == GetCurrentThreadId() )
   {
       rwl->iNumberActive--;
       retVal = 1;
       goto done;
   }

   if( fWait )
   {
       rwl->uSharedWaiters++;
       LeaveCriticalSection( &rwl->rtlCS );
       if( (dwWait = WaitForSingleObject( rwl->hSharedReleaseSemaphore, INFINITE )) == WAIT_FAILED )
      goto done;
       goto start;
   }
    }
    else
    {
   if( dwWait != WAIT_OBJECT_0 ) /* otherwise RtlReleaseResource() has already done it */
       rwl->iNumberActive++;
   retVal = 1;
    }
done:
    LeaveCriticalSection( &rwl->rtlCS );
    return retVal;
}


/***********************************************************************
 *           RtlReleaseResource             (NTDLL.471)
 */
void WINAPI RtlReleaseResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlReleaseResource(%08xh)\n",
           rwl));

    EnterCriticalSection( &rwl->rtlCS );

    if( rwl->iNumberActive > 0 ) /* have one or more readers */
    {
   if( --rwl->iNumberActive == 0 )
   {
       if( rwl->uExclusiveWaiters )
       {
wake_exclusive:
      rwl->uExclusiveWaiters--;
      ReleaseSemaphore( rwl->hExclusiveReleaseSemaphore, 1, NULL );
       }
   }
    }
    else
    if( rwl->iNumberActive < 0 ) /* have a writer, possibly recursive */
    {
   if( ++rwl->iNumberActive == 0 )
   {
       rwl->hOwningThreadId = 0;
       if( rwl->uExclusiveWaiters )
      goto wake_exclusive;
       else
      if( rwl->uSharedWaiters )
      {
          UINT n = rwl->uSharedWaiters;
          rwl->iNumberActive = rwl->uSharedWaiters; /* prevent new writers from joining until
                                                * all queued readers have done their thing */
          rwl->uSharedWaiters = 0;
          ReleaseSemaphore( rwl->hSharedReleaseSemaphore, n, NULL );
      }
   }
    }
    LeaveCriticalSection( &rwl->rtlCS );
}


/***********************************************************************
 *           RtlDumpResource                (NTDLL.340)
 */
void WINAPI RtlDumpResource(LPRTL_RWLOCK rwl)
{
  dprintf(("NTDLL: RtlDumpResource(%08x)\n",
           rwl));

  if( rwl )
  {
    dprintf(("NTDLL: RtlDumpResource(%p):\n\tactive count = %i\n\twaiting readers = %i\n\twaiting writers = %i\n",
             rwl,
             rwl->iNumberActive,
             rwl->uSharedWaiters,
             rwl->uExclusiveWaiters));

    if( rwl->iNumberActive )
       dprintf(("NTDLL: \towner thread = %08x\n",
                rwl->hOwningThreadId ));
  }
}


/*
 * heap functions
 */

/******************************************************************************
 *  RtlCreateHeap                        [NTDLL]
 */
HANDLE WINAPI RtlCreateHeap(ULONG                Flags,
                            PVOID                BaseAddress,
                            ULONG                SizeToReserve,
                            ULONG                SizeToCommit,
                            PVOID                Unknown,
                            PRTL_HEAP_DEFINITION Definition)
{
  dprintf(("NTDLL: RtlCreateHeap(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh).\n",
           Flags,
           BaseAddress,
           SizeToReserve,
           SizeToCommit,
           Unknown,
           Definition));

  return HeapCreate(Flags,
                    SizeToCommit,
                    SizeToReserve);
}


/******************************************************************************
 *  RtlAllocateHeap                         [NTDLL]
 */
PVOID WINAPI RtlAllocateHeap(HANDLE Heap,
                             ULONG  Flags,
                             ULONG  Size)
{
  dprintf(("NTDLL: RtlAllocateHeap(%08xh,%08xh,%08xh).\n",
           Heap,
           Flags,
           Size));

  return HeapAlloc(Heap,
                   Flags,
                   Size);
}


/******************************************************************************
 *  RtlFreeHeap                          [NTDLL]
 */
BOOLEAN WINAPI RtlFreeHeap(HANDLE Heap,
                           ULONG Flags,
                           PVOID Address)
{
  dprintf(("NTDLL: RtlFreeHeap(%08xh,%08xh,%08xh)\n",
           Heap,
           Flags,
           Address));

  return HeapFree(Heap, Flags, Address);
}


/******************************************************************************
 *  RtlDestroyHeap                          [NTDLL]
 *
 * FIXME: prototype guessed
 */
BOOLEAN WINAPI RtlDestroyHeap(HANDLE Heap)
{
  dprintf(("NTDLL: RtlDestroyHeap(%08xh)\n",
           Heap));

  return HeapDestroy(Heap);
}



/******************************************************************************
 *  RtlSizeHeap                             [NTDLL]
 */

ODINFUNCTION3(DWORD,RtlSizeHeap,HANDLE,hHeap,
                                DWORD, dwFlags,
                                PVOID, pAddress)
{
  return HeapSize(hHeap,
                  dwFlags,
                  pAddress);
}



/*
 * misc functions
 */


/******************************************************************************
 *  RtlAcquirePebLock                       [NTDLL]
 */
VOID WINAPI RtlAcquirePebLock(void)
{
  dprintf(("NTDLL: RtlAcquirePebLock() not implemented.\n"));

  /* enter critical section ? */
}


/******************************************************************************
 *  RtlReleasePebLock                       [NTDLL]
 */
VOID WINAPI RtlReleasePebLock(void)
{
  dprintf(("NTDLL: RtlReleasePebLock() not implemented.\n"));

  /* leave critical section ? */
}


/******************************************************************************
 *  RtlIntegerToChar                     [NTDLL]
 */
DWORD WINAPI RtlIntegerToChar(DWORD x1,
                              DWORD x2,
                              DWORD x3,
                              DWORD x4)
{
  dprintf(("NTDLL: RtlIntegerToChar(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/******************************************************************************
 *  RtlSetEnvironmentVariable               [NTDLL]
 */
DWORD WINAPI RtlSetEnvironmentVariable(DWORD           x1,
                                       PUNICODE_STRING key,
                                       PUNICODE_STRING val)
{
  dprintf(("NTDLL: RtlSetEnvironmentVariable(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           key,
           val));

  return 0;
}


/******************************************************************************
 *  RtlNewSecurityObject                    [NTDLL]
 */
DWORD WINAPI RtlNewSecurityObject(DWORD x1,
                                  DWORD x2,
                                  DWORD x3,
                                  DWORD x4,
                                  DWORD x5,
                                  DWORD x6)
{
  dprintf(("NTDLL: RtlNewSecurityObject(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return 0;
}


/******************************************************************************
 *  RtlDeleteSecurityObject                 [NTDLL]
 */
DWORD WINAPI RtlDeleteSecurityObject(DWORD x1)
{
  dprintf(("NTDLL: RtlDeleteSecurityObject(%08xh) not implemented.\n",
           x1));

  return 0;
}


/**************************************************************************
 *                 RtlNormalizeProcessParams      [NTDLL.441]
 */
LPVOID WINAPI RtlNormalizeProcessParams(LPVOID x)
{
  dprintf(("NTDLL: RtlNormalizeProcessParams(%08xh) not implemented.\n",
           x));

  return x;
}


/**************************************************************************
 *                 RtlNtStatusToDosError       [NTDLL.442]
 */
DWORD WINAPI RtlNtStatusToDosError(DWORD error)
{
  dprintf(("NTDLL: RtlNtStatusToDosError(%08xh) partially implemented.\n",
           error));

  switch (error)
  {
    case STATUS_SUCCESS:                return ERROR_SUCCESS;
    case STATUS_INVALID_PARAMETER:      return ERROR_BAD_ARGUMENTS;
    case STATUS_BUFFER_TOO_SMALL:       return ERROR_INSUFFICIENT_BUFFER;
/*   case STATUS_INVALID_SECURITY_DESCR: return ERROR_INVALID_SECURITY_DESCR;*/
    case STATUS_NO_MEMORY:              return ERROR_NOT_ENOUGH_MEMORY;
/*   case STATUS_UNKNOWN_REVISION:
     case STATUS_BUFFER_OVERFLOW:*/
  }

  dprintf(("NTDLL: RtlNtStatusToDosError(%08xh is unknown !)\n",
           error));

  return ERROR_SUCCESS;
}


/**************************************************************************
 *                 RtlGetNtProductType         [NTDLL.390]
 */
BOOLEAN WINAPI RtlGetNtProductType(LPDWORD type)
{
  dprintf(("NTDLL: RtlGetNtProductType(%08xh) not correctly implemented.\n",
           type));

  *type=3; /* dunno. 1 for client, 3 for server? */
  return 1;
}


/******************************************************************************
 * RtlEnlargedIntegerMultiply [NTDLL.341]
 * Note: This even works, since gcc returns 64bit values in eax/edx just like
 * the caller expects. However... The relay code won't grok this I think.
 *
 * @@@PH: Parameters are unknown
 */
/* longlong in VAC++ ? */

extern INT WINAPI RtlEnlargedIntegerMultiply(INT factor1,
                                             INT factor2);


/******************************************************************************
 * RtlExtendedLargeIntegerDivide [NTDLL.359]
 */
extern INT WINAPI RtlExtendedLargeIntegerDivide(LARGE_INTEGER dividend,
                                                DWORD         divisor,
                                                LPDWORD       rest);

/******************************************************************************
 * RtlExtendedLargeIntegerMultiply [NTDLL.358]
 * Note: This even works, since gcc returns 64bit values in eax/edx just like
 * the caller expects. However... The relay code won't grok this I think.
 */
/* longlong in VAC++ ? */

extern LARGE_INTEGER WINAPI RtlExtendedIntegerMultiply(LARGE_INTEGER factor1,
                                                       INT           factor2);

/******************************************************************************
 *  RtlFormatCurrentUserKeyPath             [NTDLL.371]
 */
DWORD WINAPI RtlFormatCurrentUserKeyPath(DWORD x)
{
  dprintf(("NTDLL: RtlFormatCurrentUserKeyPath(%08xh) not implemented.\n",
           x));

  return 1;
}


/******************************************************************************
 *  RtlOpenCurrentUser                      [NTDLL]
 */
DWORD WINAPI RtlOpenCurrentUser(DWORD x1,
                                DWORD *x2)
{
  /* Note: this is not the correct solution,
   * But this works pretty good on wine and NT4.0 binaries
   */
  if (x1 == 0x2000000 )
  {
    *x2 = HKEY_CURRENT_USER;
    return TRUE;
  }

  return FALSE;
}


/**************************************************************************
 *                 RtlDosPathNameToNtPathName_U      [NTDLL.338]
 *
 * FIXME: convert to UNC or whatever is expected here
 */
BOOLEAN  WINAPI RtlDosPathNameToNtPathName_U(LPWSTR          from,
                                             PUNICODE_STRING us,
                                             DWORD           x2,
                                             DWORD           x3)
{
  LPSTR fromA = HEAP_strdupWtoA(GetProcessHeap(),0,from);

  dprintf(("NTDLL: RtlDosPathNameToNtPathName_U(%08xh,%08h,%08xh,%08xh) not implemented.\n",
           from,
           us,
           x2,
           x3));

  if (us)
    RtlInitUnicodeString(us,HEAP_strdupW(GetProcessHeap(),0,from));

  return TRUE;
}


/******************************************************************************
 *  RtlCreateEnvironment                    [NTDLL]
 */
DWORD WINAPI RtlCreateEnvironment(DWORD x1,
                                  DWORD x2)
{
  dprintf(("NTDLL: RtlCreateEnvironment(%08xh, %08xh) not implemented.\n",
           x1,
           x2));

  return 0;
}


/******************************************************************************
 *  RtlDestroyEnvironment                   [NTDLL]
 */
DWORD WINAPI RtlDestroyEnvironment(DWORD x)
{
  dprintf(("NTDLL: RtlDestroyEnvironment(%08xh) not implemented.\n",
           x));

  return 0;
}


/******************************************************************************
 *  RtlQueryEnvironmentVariable_U           [NTDLL]
 */
DWORD WINAPI RtlQueryEnvironmentVariable_U(DWORD           x1,
                                           PUNICODE_STRING key,
                                           PUNICODE_STRING val)
{
  dprintf(("NTDLL: RtlQueryEnvironmentVariable_U(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           key,
           val));

  return 0;
}


/*****************************************************************************
 * Name      : RtlCopyLuid
 * Purpose   : copy local unique identifier?
 * Parameters: PLUID pluid1
 *             PLUID pluid2
 * Variables :
 * Result    :
 * Remark    : NTDLL.321
 * Status    : COMPLETELY ? IMPLEMENTED TESTED ?
 *
 * Author    : Patrick Haller [Tue, 1999/11/09 09:00]
 *****************************************************************************/

PLUID WINAPI RtlCopyLuid(PLUID pluid1,
                         PLUID pluid2)
{
  pluid2->LowPart  = pluid1->LowPart;
  pluid2->HighPart = pluid1->HighPart;
  return (pluid1);
}
