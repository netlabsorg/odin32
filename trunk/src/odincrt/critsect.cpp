/* $Id: critsect.cpp,v 1.4 2002-07-19 11:06:45 sandervl Exp $ */
/*
 * Critical sections
 * 
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl) 
 * OS/2 port
 *
 * Based on Wine code
 *
 * Copyright 1998 Alexandre Julliard (991031 Port)
 *
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
VOID WIN32API DosInitializeCriticalSection(CRITICAL_SECTION_OS2 *crit, PSZ pszSemName)
{
    APIRET rc;

    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;

    rc = DosCreateEventSem(pszSemName, &crit->hmtxLock, (pszSemName) ? DC_SEM_SHARED : 0, 0);
    if(rc != NO_ERROR) {
        DebugInt3();
        crit->hmtxLock = 0;
    }
    crit->Reserved       = GetCurrentProcessId();
}


/***********************************************************************
 *           DosAccessCriticalSection
 */
VOID WIN32API DosAccessCriticalSection(CRITICAL_SECTION_OS2 *, PSZ pszSemName)
{
    HMTX   hmtxLock = 0;
    APIRET rc;

    if(pszSemName == NULL) {
        DebugInt3();
        return;
    }

    rc = DosOpenEventSem(pszSemName, &hmtxLock);
    if(rc != NO_ERROR) {
        DebugInt3();
    }
}
/***********************************************************************
 *           DosDeleteCriticalSection
 */
void WIN32API DosDeleteCriticalSection( CRITICAL_SECTION_OS2 *crit )
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
}


/***********************************************************************
 *           DosEnterCriticalSection
 */
void WIN32API DosEnterCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    DWORD res;
    DWORD threadid = GetCurrentThreadId();

    if (!crit->hmtxLock)
    {
    	DosInitializeCriticalSection(crit, NULL);
    }
    if (DosInterlockedIncrement( &crit->LockCount ))
    {
testenter:
        if (crit->OwningThread == threadid)
        {
            crit->RecursionCount++;
            return;
        }

        if(DosInterlockedCompareExchange((PLONG)&crit->OwningThread, threadid, 0)) 
        {
            ULONG ulnrposts;

            /* Now wait for it */
            APIRET rc = DosWaitEventSem(crit->hmtxLock, SEM_INDEFINITE_WAIT);
            if(rc != NO_ERROR) {
                DebugInt3();
                return;
            }
            DosResetEventSem(crit->hmtxLock, &ulnrposts);
            goto testenter;
        }
    }
    crit->OwningThread   = GetCurrentThreadId();
    crit->RecursionCount = 1;
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
void WIN32API DosLeaveCriticalSection( CRITICAL_SECTION_OS2 *crit )
{
    if (crit->OwningThread != GetCurrentThreadId()) return;
       
    if (--crit->RecursionCount)
    {
        DosInterlockedDecrement( &crit->LockCount );
        return;
    }
    crit->OwningThread = 0;
    if (DosInterlockedDecrement( &crit->LockCount ) >= 0)
    {
        /* Someone is waiting */
        DosPostEventSem(crit->hmtxLock);
    }
}

