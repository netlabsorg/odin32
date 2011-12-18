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

#ifdef DEBUG

#include "heapshared.h"
#include <vmutex.h>

typedef struct _CritSectDebug
{
    struct _CritSectDebug *next;
    struct _CritSectDebug *prev;
    CRITICAL_SECTION *ThisSect;
    HANDLE CreatingThread;
    LONG EnteredCount;
} CritSectDebug;

CritSectDebug *csfirst = NULL;
CritSectDebug *cslast = NULL;
ULONG ulCritSectCount = 0;

VMutex csmutex;

void printCriticalSectionStatistic (void)
{
    ULONG leakedSectionsCount = 0;
    CritSectDebug *iter = csfirst;

    while (iter)
    {
        dprintf(("%d: Section %p, created by thread %8.8X, entered %d\n", leakedSectionsCount, iter->ThisSect, iter->CreatingThread, iter->EnteredCount));
        leakedSectionsCount++;

        CritSectDebug *next = iter->next;
        _sfree(iter);
        iter = next;
    }

    dprintf(("Total leaked sections %d, section count is %d\n", leakedSectionsCount, ulCritSectCount));
}

CritSectDebug *checkCritSectDebug (void *DebugInfo)
{
    CritSectDebug *iter = csfirst;

    while (iter)
    {
        if (iter == DebugInfo)
        {
            break;
        }

        iter = iter->next;
    }

    if (!iter)
    {
        dprintf(("DebugInfo is not in our list: %p!!!\n", DebugInfo));
    }

    return iter;
}

void InitializeDebugInfo (CRITICAL_SECTION *crit)
{
    csmutex.enter();
    if (checkCritSectDebug (crit->DebugInfo) != NULL)
    {
        dprintf(("InitializeDebugInfo already initialized: DebugInfo %p", crit->DebugInfo));
    }
    else
    {
        CritSectDebug *pdbg = (CritSectDebug *)_smalloc(sizeof (CritSectDebug));
        if (!pdbg)
        {
            dprintf(("InitializeDebugInfo failed to allocate debug info!!!"));
        }
        else
        {
            dprintf(("InitializeDebugInfo DebugInfo: %p", pdbg));
            pdbg->ThisSect = crit;
            pdbg->CreatingThread = ODIN_GetCurrentThreadId();
            pdbg->EnteredCount = 0;

            pdbg->next = NULL;
            pdbg->prev = cslast;
            if (cslast)
            {
                cslast->next = pdbg;
            }

            cslast = pdbg;

            if (!csfirst)
            {
                csfirst = pdbg;
            }
        }

        crit->DebugInfo = pdbg;

        ulCritSectCount++;
    }

    csmutex.leave();
}

void DeleteDebugInfo (CRITICAL_SECTION *crit)
{
    csmutex.enter();

    CritSectDebug *pdbg = checkCritSectDebug (crit->DebugInfo);

    if (pdbg)
    {
        dprintf(("Deleting critical section with DebugInfo == %p\n", pdbg));

        if (pdbg->next)
        {
            pdbg->next->prev = pdbg->prev;
        }
        else
        {
            cslast = pdbg->prev;
        }

        if (pdbg->prev)
        {
            pdbg->prev->next = pdbg->next;
        }
        else
        {
            csfirst = pdbg->next;
        }

        _sfree (pdbg);

        ulCritSectCount--;

        crit->DebugInfo = NULL;
    }

    csmutex.leave();
}

void DebugEnterCritSect (CRITICAL_SECTION *crit)
{
    csmutex.enter();

    CritSectDebug *pdbg = checkCritSectDebug (crit->DebugInfo);

    if (!pdbg)
    {
        InitializeDebugInfo (crit);
        pdbg = checkCritSectDebug (crit->DebugInfo);
    }

    if (pdbg)
    {
        pdbg->EnteredCount++;
    }

    csmutex.leave();
}

void DebugLeaveCritSect (CRITICAL_SECTION *crit)
{
    csmutex.enter();

    CritSectDebug *pdbg = checkCritSectDebug (crit->DebugInfo);

    if (pdbg)
    {
        pdbg->EnteredCount--;
    }

    csmutex.leave();
}
#else
#define InitializeDebugInfo(a)
#define DeleteDebugInfo(a)
#define DebugEnterCritSect(a)
#define DebugLeaveCritSect(a)
#endif



/***********************************************************************
 *           get_semaphore (allocate semaphore only when needed)
 *           Assign the semaphore atomically
 */
static inline HANDLE get_semaphore (CRITICAL_SECTION *crit)
{
    HANDLE ret = crit->LockSemaphore;

    if (!ret)
    {
        HANDLE sem = CreateSemaphoreA (NULL, 0, 1, NULL);

        if (sem)
        {
            ret = InterlockedCompareExchange ((PLONG)&crit->LockSemaphore, sem, NULL);

            if (ret)
            {
                // semaphore already assigned
                CloseHandle (sem);
            }
            else
            {
                ret = sem;
            }
        }
    }

    return ret;
}

extern "C" {

/***********************************************************************
 *           InitializeCriticalSection   (KERNEL32.472) (NTDLL.406)
 */
void WINAPI InitializeCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("InitializeCriticalSection %x", crit));

    InitializeDebugInfo (crit);

    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;
    crit->LockSemaphore  = 0;
    crit->Reserved       = 0;
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
    if (spincount) dprintf(("critsection=%p: spincount=%ld not supported\n", crit, spincount));
    InitializeCriticalSection( crit );
    crit->Reserved = spincount;
    return TRUE;
}

/***********************************************************************
 *           DeleteCriticalSection   (KERNEL32.185) (NTDLL.327)
 */
void WINAPI DeleteCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("DeleteCriticalSection %x", crit));

    DeleteDebugInfo (crit);

        if (crit->RecursionCount)  /* Should not happen */
            dprintf(("Deleting owned critical section (%p)\n", crit ));

        crit->LockCount      = -1;
        crit->RecursionCount = 0;
        crit->OwningThread   = 0;
    if (crit->LockSemaphore) CloseHandle( crit->LockSemaphore );
        crit->LockSemaphore  = 0;
    crit->Reserved       = 0;
}


/***********************************************************************
 *           EnterCriticalSection   (KERNEL32.195) (NTDLL.344)
 */
void WINAPI EnterCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf2(("EnterCriticalSection %x", crit));

#if 0
    /* bird: this is bullcrap, memset(&MyCritSect, 0, sizeof(MyCritSect)) doesn't work in w2k (at least not here).
     * besides this isn't safe. The leave code first sets owner=0 then decrements the lock, heavens knows what
     * will happen if the thread is preemted there...
     */
    // check for uninitialized critical section created with memset (, 0, )
    if (crit->LockCount != -1 && crit->OwningThread == 0)
    {
        dprintf(("Uninitialized section!!!"));
        __interrupt(3);
        InitializeCriticalSection (crit);
    }
#endif

    DebugEnterCritSect (crit);

    if (InterlockedIncrement (&crit->LockCount))
    {
        if (crit->OwningThread == ODIN_GetCurrentThreadId())
        {
            crit->RecursionCount++;
            return;
        }

        /* Now wait for it */
        for (;;)
        {
            HANDLE sem = get_semaphore (crit);
            DWORD res = WaitForSingleObject (sem, 5000L);

            if (res == WAIT_TIMEOUT)
            {
                dprintf(("Critical section %p wait timed out, retrying (60 sec)\n", crit));
                res = WaitForSingleObject (sem, 60000L);
                if (res == WAIT_TIMEOUT && TRACE_ON(relay))
                {
                    dprintf(("Critical section %p wait timed out, retrying (5 min)\n", crit));
                    res = WaitForSingleObject (sem, 300000L);
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
            RaiseException(EXCEPTION_CRITICAL_SECTION_WAIT, 0, 1, (DWORD *)&crit);
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
    BOOL ret = FALSE;

    dprintf2(("TryEnterCriticalSection %x", crit));

    if (InterlockedCompareExchange (&crit->LockCount, 0, -1) == -1)
    {
        // LockCount was successfully changed from -1 to 0 and we own the section now
        crit->OwningThread   = ODIN_GetCurrentThreadId();
        crit->RecursionCount = 1;
        ret = TRUE;
    }
    else
    {
        if (crit->OwningThread == ODIN_GetCurrentThreadId())
        {
            InterlockedIncrement (&crit->LockCount);
            crit->RecursionCount++;
            ret = TRUE;
        }
    }

    if (ret)  DebugEnterCritSect (crit);

    return ret;
}


/***********************************************************************
 *           LeaveCriticalSection   (KERNEL32.494) (NTDLL.426)
 */
void WINAPI LeaveCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf2(("LeaveCriticalSection %x", crit));

    DebugLeaveCritSect (crit);

    /*
     * Wine and Windows don't check if the caller is the owner.
     * We leave this check in for debugging since odin32 may use this
     * internally and it's good to know if we mess up.
     */
#ifdef DEBUG
    if (crit->OwningThread != ODIN_GetCurrentThreadId())
    {
        dprintf(("LeaveCriticalSection: Not owner!! OwningThread=%d CurThread=%d\n", crit->OwningThread, ODIN_GetCurrentThreadId()));
        DebugInt3();
        return;
    }
#endif

    if (--crit->RecursionCount)
    {
        InterlockedDecrement(&crit->LockCount);
        return;
    }

    crit->OwningThread = 0;

    if (InterlockedDecrement( &crit->LockCount ) >= 0)
    {
        /* Someone is waiting */
        HANDLE sem = get_semaphore (crit);
        ReleaseSemaphore (sem, 1, NULL);
    }
}


/***********************************************************************
 *           MakeCriticalSectionGlobal   (KERNEL32.515)
 */
void WINAPI MakeCriticalSectionGlobal( CRITICAL_SECTION *crit )
{
    dprintf(("MakeCriticalSectionGlobal %x", crit));

    HANDLE sem = get_semaphore(crit);

    crit->LockSemaphore = ConvertToGlobalHandle (sem);
}


/***********************************************************************
 *           ReinitializeCriticalSection   (KERNEL32.581)
 */
void WINAPI ReinitializeCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("ReinitializeCriticalSection %x", crit));

    if (!crit->LockSemaphore)
    {
        InitializeCriticalSection (crit);
    }
}


/***********************************************************************
 *           UninitializeCriticalSection   (KERNEL32.703)
 */
void WINAPI UninitializeCriticalSection( CRITICAL_SECTION *crit )
{
    dprintf(("UninitializeCriticalSection %x", crit));
    DeleteCriticalSection (crit);
}

} // extern "C"

