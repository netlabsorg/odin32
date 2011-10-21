#ifndef __ODINCRT_H__
#define __ODINCRT_H__

#include <win32type.h>

#ifndef SEM_INDEFINITE_WAIT
#define SEM_INDEFINITE_WAIT -1
#endif

typedef struct {
        LONG    LockCount;
        LONG    RecursionCount;
        ULONG   OwningThread;
        ULONG   hevLock;
        ULONG   Reserved;
        LONG    CreationCount;
} CRITICAL_SECTION_OS2, *PCRITICAL_SECTION_OS2;

#ifndef OS2_INCLUDED
typedef const char *PCSZ;
#endif

ULONG WIN32API DosValidateCriticalSection (CRITICAL_SECTION_OS2 *crit);
ULONG WIN32API DosDeleteCriticalSection(CRITICAL_SECTION_OS2 *);
ULONG WIN32API DosEnterCriticalSection(CRITICAL_SECTION_OS2 *, ULONG ulTimeout = SEM_INDEFINITE_WAIT);
ULONG WIN32API DosInitializeCriticalSection(CRITICAL_SECTION_OS2 *, PCSZ pszSemName, BOOL fShared = FALSE);
ULONG WIN32API DosAccessCriticalSection(CRITICAL_SECTION_OS2 *, PCSZ pszSemName);
ULONG WIN32API DosLeaveCriticalSection(CRITICAL_SECTION_OS2 *);

LONG WIN32API DosInterlockedCompareExchange( PLONG dest, LONG xchg, LONG compare );
LONG WIN32API DosInterlockedDecrement(LPLONG);
LONG WIN32API DosInterlockedExchange(LPLONG,LONG);
LONG WIN32API DosInterlockedExchangeAdd( PLONG dest, LONG incr );
LONG WIN32API DosInterlockedIncrement(LPLONG);

#ifdef OS2_INCLUDED
APIRET WIN32API DosQueryModuleHandleStrict(PCSZ pszModname, PHMODULE pHmod);
#endif

#endif //__ODINCRT_H__
