/* $Id: critsect.cpp,v 1.11 2004-03-16 17:24:53 sandervl Exp $ */
/*
 * Critical sections in the Win32 sense
 *
 * Copyright 2002 Sander van Leeuwen <sandervl@innotek.de>
 *
 */
#define INCL_DOS
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2.h>

#include "dbglog.h"

#include <odincrt.h>
#include <FastInfoBlocks.h>

#undef fibGetPid

#undef dprintf
#undef DebugInt3

#define dprintf(a)
#define DebugInt3()

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
ULONG WIN32API DosInitializeCriticalSection(CRITICAL_SECTION_OS2 *crit, char *pszSemName, BOOL fShared)
{
    APIRET rc;

    rc = DosCreateEventSem(pszSemName, &crit->hevLock, (pszSemName || fShared)? DC_SEM_SHARED: 0, 0);
    
    if(rc != NO_ERROR) 
    {
        crit->hevLock = 0;
        return rc;
    }
    
    // initialize lock count with special value -1, meaning noone posesses it
    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;

    crit->CreationCount  = 1;
    crit->Reserved       = 0;
    
    return NO_ERROR;
}

ULONG WIN32API DosValidateCriticalSection (CRITICAL_SECTION_OS2 *crit)
{
    if (crit->hevLock != NULLHANDLE)
    {
        return NO_ERROR;
    }
    
    return ERROR_INVALID_PARAMETER;
}

// Initializes or opens a critical section
ULONG WIN32API DosAccessCriticalSection(CRITICAL_SECTION_OS2 *crit, char *pszSemName)
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

ULONG WIN32API DosDeleteCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    if (DosValidateCriticalSection (crit))
    {
        DosCloseEventSem (crit->hevLock);
        
        if(DosInterlockedDecrement(&crit->CreationCount) == 0)
        {
            crit->LockCount      = -1;
            crit->RecursionCount = 0;
            crit->OwningThread   = 0;
            crit->hevLock        = 0;
            crit->Reserved       = 0;
        }
    }
    return NO_ERROR;
}


ULONG WIN32API DosEnterCriticalSection( CRITICAL_SECTION_OS2 *crit, ULONG ulTimeout )
{
    APIRET rc = NO_ERROR;
            
    ULONG threadid = GetCurrentThreadId();
            
    if (!crit->hevLock)
    {
        rc = DosInitializeCriticalSection (crit, NULL, FALSE);
        if (rc != NO_ERROR)
        {
            return rc;
        }
    }

    dprintf(("Entering the section: owner = %8.8X\n", crit->OwningThread));

    // We want to acquire the section, count the entering
    DosInterlockedIncrement (&crit->LockCount);
    
    // try to acquire the section
    for (;;)
    {
        // try to assign owning thread id atomically
        if (DosInterlockedCompareExchange((PLONG)&crit->OwningThread, threadid, 0) == 0)
        {
            ULONG ulnrposts = 0;

            dprintf(("Acquired the section: owner = %8.8X\n", crit->OwningThread));
            DosResetEventSem (crit->hevLock, &ulnrposts);
            break;
        }
        
        if (crit->OwningThread == threadid)
        {
            // This thread already owns the section
            crit->RecursionCount++;
            dprintf(("Recursion: %d\n", crit->RecursionCount));
            return NO_ERROR;
        }

        // Arise any timing problems and let others to run
        DosSleep (0);

        dprintf(("Waiting on sem: owner = %8.8X\n", crit->OwningThread));
        rc = DosWaitEventSem (crit->hevLock, ulTimeout);
        dprintf(("Returned from wait: owner = %8.8X, rc = %d\n", crit->OwningThread, rc));
       
        if (rc != NO_ERROR) 
        {
            dprintf(("Returned from wait: FAILED!!!\n"));
            // We fail, deregister itself
            DosInterlockedDecrement (&crit->LockCount);
            return rc;
        }
    }
    
    // the section was successfully aquired
    crit->RecursionCount = 1;
    
    if (crit->Reserved != 0)
    {
        // the section already entered!!!!
        DosBeep (2000, 200);
    }
    
    crit->Reserved = 1;
    
    return NO_ERROR;
}


ULONG WIN32API DosLeaveCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    dprintf(("Leaving the section\n"));
    if (crit->OwningThread != GetCurrentThreadId()) {
        dprintf(("WRONG THREAD ID!!! owner is %8.8X\n", crit->OwningThread));
        return ERROR_INVALID_PARAMETER;
    }

    if (--crit->RecursionCount)
    {
        dprintf(("Recursion exit: %d\n", crit->RecursionCount));
        DosInterlockedDecrement( &crit->LockCount );
        return NO_ERROR;
    }
    
    crit->Reserved = 0;
    crit->OwningThread = 0;

    dprintf(("Released the section\n"));

    if (DosInterlockedDecrement( &crit->LockCount ) >= 0)
    {
        dprintf(("Posted the semaphore\n"));
        DosPostEventSem(crit->hevLock);
    }
    
    return NO_ERROR;
}


/**
 * Checks if the current thread is in the critical section or now.
 *
 * @returns NO_ERROR if in the critical section.
 * @returns ERROR_NOT_OWNER if not in the critical section.
 * @param   pCrit   Pointer to the critical section.
 */
ULONG WIN32API DosIsInCriticalSection( CRITICAL_SECTION_OS2 *pCrit )
{
    return  (pCrit->hevLock && pCrit->OwningThread == GetCurrentThreadId() ? NO_ERROR : ERROR_NOT_OWNER);
}

