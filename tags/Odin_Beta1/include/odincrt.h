#ifndef __ODINCRT_H__
#define __ODINCRT_H__

#include <win32type.h>

typedef struct {
        LONG    LockCount;
        LONG    RecursionCount;
        ULONG   OwningThread;
        ULONG   hmtxLock;
        ULONG   Reserved;
} CRITICAL_SECTION_OS2, *PCRITICAL_SECTION_OS2;

VOID WIN32API DosDeleteCriticalSection(CRITICAL_SECTION_OS2 *);
VOID WIN32API DosEnterCriticalSection(CRITICAL_SECTION_OS2 *);
VOID WIN32API DosInitializeCriticalSection(CRITICAL_SECTION_OS2 *, char *pszSemName);
VOID WIN32API DosAccessCriticalSection(CRITICAL_SECTION_OS2 *, char *pszSemName);
VOID WIN32API DosLeaveCriticalSection(CRITICAL_SECTION_OS2 *);

LONG WIN32API DosInterlockedCompareExchange( PLONG dest, LONG xchg, LONG compare );
LONG WIN32API DosInterlockedDecrement(LPLONG);
LONG WIN32API DosInterlockedExchange(LPLONG,LONG);
LONG WIN32API DosInterlockedExchangeAdd( PLONG dest, LONG incr );
LONG WIN32API DosInterlockedIncrement(LPLONG);


#endif //__ODINCRT_H__
