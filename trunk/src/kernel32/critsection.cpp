/* $Id: critsection.cpp,v 1.10 2003-08-01 16:25:47 sandervl Exp $ */
/*
 * Win32 critical sections
 * 
 *
 *
 * Copyright 1998 Alexandre Julliard (991031 Port)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <assert.h>
#include <stdio.h>
#include <dbglog.h>
#include <debugtools.h>
#include <odinwrap.h>
#include <wprocess.h>

#define DBG_LOCALLOG	DBG_critsection
#include "dbglocal.h"

DECLARE_DEBUG_CHANNEL(relay)



/***********************************************************************
 *           InitializeCriticalSection   (KERNEL32.472) (NTDLL.406)
 */
void WINAPI InitializeCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("InitializeCriticalSection %x", crit));
    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;
    crit->LockSemaphore  = CreateSemaphoreA( NULL, 0, 1, NULL );
    crit->Reserved       = ODIN_GetCurrentProcessId();
}

/***********************************************************************
 *           InitializeCriticalSectionAndSpinCount   (NTDLL.@)
 * The InitializeCriticalSectionAndSpinCount (KERNEL32) function is
 * available on NT4SP3 or later, and Win98 or later.
 * I am assuming that this is the correct definition given the MSDN
 * docs for the kernel32 functions.
 */
BOOL WINAPI InitializeCriticalSectionAndSpinCount( CRITICAL_SECTION *crit, DWORD spincount )
{
    if(spincount) dprintf(("critsection=%p: spincount=%ld not supported\n", crit, spincount));
    crit->Reserved = spincount;
    InitializeCriticalSection( crit );
    return TRUE;
}

/***********************************************************************
 *           DeleteCriticalSection   (KERNEL32.185) (NTDLL.327)
 */
void WINAPI DeleteCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("DeleteCriticalSection %x", crit));

    if (crit->LockSemaphore)
    {
        if (crit->RecursionCount)  /* Should not happen */
            dprintf(("Deleting owned critical section (%p)\n", crit ));

        crit->LockCount      = -1;
        crit->RecursionCount = 0;
        crit->OwningThread   = 0;
        CloseHandle( crit->LockSemaphore );
        crit->LockSemaphore  = 0;
        crit->Reserved       = (DWORD)-1;
    }
}


/***********************************************************************
 *           EnterCriticalSection   (KERNEL32.195) (NTDLL.344)
 */
void WINAPI EnterCriticalSection( CRITICAL_SECTION *crit )
{
    DWORD res;

    dprintf2(("EnterCriticalSection %x", crit));
    if (!crit->LockSemaphore)
    {
    	dprintf(("entering uninitialized section(%p)?\n",crit));
    	InitializeCriticalSection(crit);
    }
    if (InterlockedIncrement( &crit->LockCount ))
    {
        if (crit->OwningThread == ODIN_GetCurrentThreadId())
        {
            crit->RecursionCount++;
            return;
        }

        /* Now wait for it */
        for (;;)
        {
            res = WaitForSingleObject( crit->LockSemaphore, 5000L );
            if ( res == WAIT_TIMEOUT )
            {
                dprintf(("Critical section %p wait timed out, retrying (60 sec)\n", crit ));
                res = WaitForSingleObject( crit->LockSemaphore, 60000L );
                if ( res == WAIT_TIMEOUT && TRACE_ON(relay) )
                {
                    dprintf(("Critical section %p wait timed out, retrying (5 min)\n", crit ));
                    res = WaitForSingleObject( crit->LockSemaphore, 300000L );
                }
            }
            if (res == STATUS_WAIT_0) break;

#if 0
            EXCEPTION_RECORD rec;

            rec.ExceptionCode    = EXCEPTION_CRITICAL_SECTION_WAIT;
            rec.ExceptionFlags   = 0;
            rec.ExceptionRecord  = NULL;
            rec.ExceptionAddress = RaiseException;  /* sic */
            rec.NumberParameters = 1;
            rec.ExceptionInformation[0] = (DWORD)crit;
            RtlRaiseException( &rec );
#endif
            dprintf(("ERROR: EnterCritSection: WaitForSingleObject returned %d -> RaiseException", res));
            RaiseException(EXCEPTION_CRITICAL_SECTION_WAIT, 0, 1, (DWORD *)crit);
        }
    }
    crit->OwningThread   = ODIN_GetCurrentThreadId();
    crit->RecursionCount = 1;
}


/***********************************************************************
 *           TryEnterCriticalSection   (KERNEL32.898) (NTDLL.969)
 */
BOOL WINAPI TryEnterCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf2(("TryEnterCriticalSection %x", crit));
    if (InterlockedIncrement( &crit->LockCount ))
    {
        if (crit->OwningThread == ODIN_GetCurrentThreadId())
        {
            crit->RecursionCount++;
            return TRUE;
        }
        /* FIXME: this doesn't work */
        InterlockedDecrement( &crit->LockCount );
        return FALSE;
    }
    crit->OwningThread   = ODIN_GetCurrentThreadId();
    crit->RecursionCount = 1;
    return TRUE;
}


/***********************************************************************
 *           LeaveCriticalSection   (KERNEL32.494) (NTDLL.426)
 */
void WINAPI LeaveCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf2(("LeaveCriticalSection %x", crit));
    if (crit->OwningThread != ODIN_GetCurrentThreadId()) return;
       
    if (--crit->RecursionCount)
    {
        InterlockedDecrement( &crit->LockCount );
        return;
    }
    crit->OwningThread = 0;
    if (InterlockedDecrement( &crit->LockCount ) >= 0)
    {
        /* Someone is waiting */
        ReleaseSemaphore( crit->LockSemaphore, 1, NULL );
    }
}


/***********************************************************************
 *           MakeCriticalSectionGlobal   (KERNEL32.515)
 */
void WINAPI MakeCriticalSectionGlobal( CRITICAL_SECTION *crit )
{
    dprintf(("MakeCriticalSectionGlobal %x", crit));
    crit->LockSemaphore = ConvertToGlobalHandle( crit->LockSemaphore );
    crit->Reserved      = 0L;
}


/***********************************************************************
 *           ReinitializeCriticalSection   (KERNEL32.581)
 */
void WINAPI ReinitializeCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("ReinitializeCriticalSection %x", crit));

    if ( !crit->LockSemaphore )
        InitializeCriticalSection( crit );

    else if ( crit->Reserved && crit->Reserved != ODIN_GetCurrentProcessId() )
    {
        dprintf(("(%p) called for %08lx first, %08lx now: making global\n", 
               crit, crit->Reserved, ODIN_GetCurrentProcessId() ));

        MakeCriticalSectionGlobal( crit );
    }
}


/***********************************************************************
 *           UninitializeCriticalSection   (KERNEL32.703)
 */
void WINAPI UninitializeCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("UninitializeCriticalSection %x", crit));
    if ( crit->LockSemaphore )
    {
        if ( crit->Reserved )  /* not global */
            DeleteCriticalSection( crit );
        else
            dprintf(("(%p) for %08lx: Crst is global, don't know whether to delete\n", 
                     crit, ODIN_GetCurrentProcessId() ));
    }
}

