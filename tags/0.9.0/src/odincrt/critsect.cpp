/* $Id: critsect.cpp,v 1.12 2004-04-14 08:45:45 sandervl Exp $ */
/*
 * Critical sections in the Win32 sense
 *
 * Copyright 2002 Sander van Leeuwen <sandervl@innotek.de>
 *
 */
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2wrap.h>
#include <win32type.h>
#include <win32api.h>
#include <FastInfoBlocks.h>

#include <assert.h>
#include <stdio.h>

#include <odincrt.h>


#undef fibGetPid

#undef dprintf
#undef DebugInt3

#define dprintf(a)
#define DebugInt3()


ULONG WIN32API DosValidateCriticalSection (CRITICAL_SECTION_OS2 *crit)
{
    if (crit->hevLock != NULLHANDLE)
    {
        return NO_ERROR;
    }

    return ERROR_INVALID_PARAMETER;
}

//******************************************************************************
// This is an OS/2 implementation of what Win32 treats as "critical sections"
// It is an implementation that is highly optimized for the case where there is
// only one thread trying to access the critical section, i.e. it is available
// most of the time. Therefore we can use these critical sections for all our
// serialization and not lose any performance when concurrent access is unlikely.
//
// In case there is multiple access, we use the OS/2 kernel event semaphores.
//******************************************************************************


// encode PID and TID into one 32bit value
#define MAKE_THREADID(processid, threadid) 	((processid << 16) | threadid)

//******************************************************************************
//******************************************************************************
inline ULONG GetCurrentThreadId()
{
#ifdef fibGetPid
    return MAKE_THREADID(fibGetPid(), fibGetTid());
#else
    PTIB   ptib;
    PPIB   ppib;
    APIRET rc;

    rc = DosGetInfoBlocks(&ptib, &ppib);
    if(rc == NO_ERROR) {
#ifdef DEBUG
        if(MAKE_THREADID(ppib->pib_ulpid, ptib->tib_ptib2->tib2_ultid) == 0) {
            DebugInt3();
        }
#endif
        return MAKE_THREADID(ppib->pib_ulpid, ptib->tib_ptib2->tib2_ultid);
    }
    DebugInt3();
    return 0;
#endif
}
//******************************************************************************
//******************************************************************************
inline ULONG GetCurrentProcessId()
{
#ifdef fibGetPid
    return fibGetPid();
#else
    PTIB   ptib;
    PPIB   ppib;
    APIRET rc;

    rc = DosGetInfoBlocks(&ptib, &ppib);
    if(rc == NO_ERROR) {
        return ppib->pib_ulpid;
    }
    DebugInt3();
    return 0;
#endif
}

/***********************************************************************
 *           DosInitializeCriticalSection
 */
ULONG WIN32API DosInitializeCriticalSection(CRITICAL_SECTION_OS2 *crit,
                                            PCSZ pszSemName, BOOL fShared)
{
    APIRET rc;

    // initialize lock count with special value -1, meaning noone posesses it
    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;

    rc = DosCreateEventSem(pszSemName, &crit->hevLock, (pszSemName || fShared) ? DC_SEM_SHARED : 0, 0);
    if(rc != NO_ERROR) {
        DebugInt3();
        crit->hevLock = 0;
        return rc;
    }
    crit->CreationCount  = 1;
    crit->Reserved       = GetCurrentProcessId();
    return NO_ERROR;
}


/***********************************************************************
 *           DosAccessCriticalSection
 */
ULONG WIN32API DosAccessCriticalSection(CRITICAL_SECTION_OS2 *crit, PCSZ pszSemName)
{
    APIRET rc = NO_ERROR;

    // Increment creation counter to prevent the section to be destroyed while
    // we are checking it. Assume that an unitialized section has the counter == 0
    DosInterlockedIncrement(&crit->CreationCount);

    if (DosValidateCriticalSection (crit) == NO_ERROR)
    {
        // the section already initialized, use it
        HEV hevLock = NULLHANDLE;

        if (pszSemName == NULL)
        {
            hevLock = crit->hevLock;
        }

        rc = DosOpenEventSem(pszSemName, &hevLock);

        if (rc != NO_ERROR)
        {
            DosInterlockedDecrement(&crit->CreationCount);
            DebugInt3();
        }
    }
    else
    {
        rc = DosInitializeCriticalSection (crit, pszSemName, TRUE);
    }

    return NO_ERROR;
}
/***********************************************************************
 *           DosDeleteCriticalSection
 */
ULONG WIN32API DosDeleteCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    if (crit->hevLock)
    {
#ifdef DEBUG
        if (  (crit->LockCount != -1 && crit->CreationCount == 1)
            || crit->OwningThread
            || crit->RecursionCount)  /* Should not happen */
        {
           DebugInt3();
        }
#endif
        DosCloseEventSem(crit->hevLock);
        if(DosInterlockedDecrement(&crit->CreationCount) == 0)
        {
            crit->LockCount      = -1;
            crit->RecursionCount = 0;
            crit->OwningThread   = 0;
            crit->hevLock       = 0;
            crit->Reserved       = (DWORD)-1;
        }
    }
    return NO_ERROR;
}


/***********************************************************************
 *           DosEnterCriticalSection
 */
ULONG WIN32API DosEnterCriticalSection( CRITICAL_SECTION_OS2 *crit, ULONG ulTimeout )
{
    DWORD res;
    DWORD threadid = GetCurrentThreadId();

    // create crit sect just in time...
    if (!crit->hevLock)
    {
    	DosInitializeCriticalSection(crit, NULL);
    }
    // if the same thread is requesting it again, memorize it
    if (crit->OwningThread == threadid)
    {
        crit->RecursionCount++;
        return NO_ERROR;
    }

    // do an atomic increase of the lockcounter
    DosInterlockedIncrement(&crit->LockCount);

    // do an atomic operation where we compare the owning thread id with 0
    // and if this is true, exchange it with the id of the current thread.
testenter:
    if(DosInterlockedCompareExchange((PLONG)&crit->OwningThread, threadid, 0))
    {
        // the crit sect is in use
        ULONG ulnrposts;

        // now wait for it
        APIRET rc = DosWaitEventSem(crit->hevLock, ulTimeout);
        if(rc != NO_ERROR) {
            DebugInt3();
            return rc;
        }
        DosResetEventSem(crit->hevLock, &ulnrposts);
        // multiple waiters could be running now. Repeat the logic so that
        // only one actually can get the critical section
        goto testenter;
    }
    crit->RecursionCount = 1;
    return NO_ERROR;
}


/***********************************************************************
 *           DosLeaveCriticalSection
 */
ULONG WIN32API DosLeaveCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    if (crit->OwningThread != GetCurrentThreadId()) {
        DebugInt3();
        return ERROR_INVALID_PARAMETER;
    }

    if (--crit->RecursionCount)
    {
        //just return
        return NO_ERROR;
    }
    crit->OwningThread = 0;
    if (DosInterlockedDecrement( &crit->LockCount ) >= 0)
    {
        /* Someone is waiting */
        DosPostEventSem(crit->hevLock);
    }
    return NO_ERROR;
}
