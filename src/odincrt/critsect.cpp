/* $Id: critsect.cpp,v 1.6 2002-10-07 16:28:13 sandervl Exp $ */
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

#include <assert.h>
#include <stdio.h>

#include <odincrt.h>

#ifdef DEBUG
#define DebugInt3()	_interrupt(3)
#else
#define DebugInt3()
#endif

//******************************************************************************
// This is an OS/2 implementation of what Win32 treats as "critical sections"
// It is an implementation that is highly optimized for the case where there is
// only one thread trying to access the critical section, i.e. it is available
// most of the time. Therefore we can use these critical sections for all our
// serialization and not lose any performance when concurrent access is unlikely.
//
// In case there is multiple access, we use the OS/2 kernel even semaphores.
//******************************************************************************


// encode PID and TID into one 32bit value
#define MAKE_THREADID(processid, threadid) 	((processid << 16) | threadid)

//******************************************************************************
//******************************************************************************
inline ULONG GetCurrentThreadId()
{
    PTIB   ptib;
    PPIB   ppib;
    APIRET rc;
 
    rc = DosGetInfoBlocks(&ptib, &ppib);
    if(rc == NO_ERROR) {
        return MAKE_THREADID(ppib->pib_ulpid, ptib->tib_ptib2->tib2_ultid);
    }
    DebugInt3();
    return 0;
}
//******************************************************************************
//******************************************************************************
inline ULONG GetCurrentProcessId()
{
    PTIB   ptib;
    PPIB   ppib;
    APIRET rc;
 
    rc = DosGetInfoBlocks(&ptib, &ppib);
    if(rc == NO_ERROR) {
        return ppib->pib_ulpid;
    }
    DebugInt3();
    return 0;
}

/***********************************************************************
 *           DosInitializeCriticalSection
 */
ULONG WIN32API DosInitializeCriticalSection(CRITICAL_SECTION_OS2 *crit, PSZ pszSemName)
{
    APIRET rc;

    // initialize lock count with special value -1, meaning noone posesses it
    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;

    rc = DosCreateEventSem(pszSemName, &crit->hmtxLock, (pszSemName) ? DC_SEM_SHARED : 0, 0);
    if(rc != NO_ERROR) {
        DebugInt3();
        crit->hmtxLock = 0;
        return rc;
    }
    crit->Reserved       = GetCurrentProcessId();
    return NO_ERROR;
}


/***********************************************************************
 *           DosAccessCriticalSection
 */
ULONG WIN32API DosAccessCriticalSection(CRITICAL_SECTION_OS2 *, PSZ pszSemName)
{
    HMTX   hmtxLock = 0;
    APIRET rc;

    if(pszSemName == NULL) {
        DebugInt3();
        return ERROR_INVALID_PARAMETER;
    }

    rc = DosOpenEventSem(pszSemName, &hmtxLock);
    if(rc != NO_ERROR) {
        DebugInt3();
        return rc;
    }
    return NO_ERROR;
}
/***********************************************************************
 *           DosDeleteCriticalSection
 */
ULONG WIN32API DosDeleteCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    if (crit->hmtxLock)
    {
        if (crit->RecursionCount)  /* Should not happen */
        {
           DebugInt3();
        }
        crit->LockCount      = -1;
        crit->RecursionCount = 0;
        crit->OwningThread   = 0;
        DosCloseEventSem(crit->hmtxLock);
        crit->hmtxLock       = 0;
        crit->Reserved       = (DWORD)-1;
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
    if (!crit->hmtxLock)
    {
    	DosInitializeCriticalSection(crit, NULL);
    }
    // do an atomic increase of the lockcounter and see if it is > 0
    // (i.e. it is already posessed)
    if (DosInterlockedIncrement(&crit->LockCount))
    {
testenter:
        // if the same thread is requesting it again, memorize it
        if (crit->OwningThread == threadid)
        {
            crit->RecursionCount++;
            return NO_ERROR;
        }
        // do an atomic operation where we compare the owning thread id with 0
        // and if this is true, exchange it with the id of the current thread.
        if(DosInterlockedCompareExchange((PLONG)&crit->OwningThread, threadid, 0))
        {
            // the compare did not return equal, i.e. the crit sect is in use

            ULONG ulnrposts;

            /* Now wait for it */
            APIRET rc = DosWaitEventSem(crit->hmtxLock, ulTimeout);
            if(rc != NO_ERROR) {
                DebugInt3();
                return rc;
            }
            DosResetEventSem(crit->hmtxLock, &ulnrposts);
            // multiple waiters could be running now. Repeat the logic so that
            // only one actually can get the critical section
            goto testenter;
        }
    }
    crit->OwningThread   = GetCurrentThreadId();
    crit->RecursionCount = 1;
    return NO_ERROR;
}


/***********************************************************************
 *           DosTryEnterCriticalSection
 */
BOOL WIN32API DosTryEnterCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    if (DosInterlockedIncrement( &crit->LockCount ))
    {
        if (crit->OwningThread == GetCurrentThreadId())
        {
            crit->RecursionCount++;
            return TRUE;
        }
        DosInterlockedDecrement( &crit->LockCount );
        return FALSE;
    }
    crit->OwningThread   = GetCurrentThreadId();
    crit->RecursionCount = 1;
    return TRUE;
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
        DosInterlockedDecrement( &crit->LockCount );
        return NO_ERROR;
    }
    crit->OwningThread = 0;
    if (DosInterlockedDecrement( &crit->LockCount ) >= 0)
    {
        /* Someone is waiting */
        DosPostEventSem(crit->hmtxLock);
    }
    return NO_ERROR;
}
