/*
 * KERNEL32 compatibility functions imported by the Mozilla Firefox 160+ binaries
 * (firefox.exe, mozglue.dll, xul.dll, plugin-container.exe, the Microsoft C++
 * runtime).  The Odin32 PE loader stops with the "non-existing api" message box
 * for an import it cannot resolve, so every function must exist; where OS/2 has no
 * equivalent the function reports "not supported" or does nothing.
 *
 * Real implementations: QueueUserWorkItem (a thread per call), SignalObjectAndWait,
 * GetLogicalProcessorInformation (one entry per CPU), ReplaceFileA, GetSystemTimes
 * (zeros), SetCriticalSectionSpinCount.
 * Job objects are faked with an event handle so that firefox.exe's launcher can
 * "create" and "close" one.
 */
#include <os2win.h>
#include <string.h>
#include "odintrace.h"

#define FF_NOT_IMPL   ERROR_CALL_NOT_IMPLEMENTED

typedef struct {
    LPTHREAD_START_ROUTINE func;
    PVOID                  ctx;
} FFWORKITEM;

static DWORD WINAPI ffWorkItemThread(LPVOID p)
{
    FFWORKITEM item = *(FFWORKITEM *)p;
    HeapFree(GetProcessHeap(), 0, p);
    return item.func(item.ctx);
}

extern "C" {

/*****************************************************************************
 * Vectored exception handling: Firefox only registers a crash reporting hook.
 *****************************************************************************/
PVOID WINAPI AddVectoredExceptionHandler(ULONG first, PVOID handler)
{
    return handler ? handler : (PVOID)1;
}

ULONG WINAPI RemoveVectoredExceptionHandler(PVOID handle)
{
    return 1;
}

/*****************************************************************************
 * Job objects
 *****************************************************************************/
HANDLE WINAPI CreateJobObjectW(LPSECURITY_ATTRIBUTES sa, LPCWSTR name)
{
    return CreateEventA(NULL, TRUE, FALSE, NULL);
}

BOOL WINAPI SetInformationJobObject(HANDLE job, DWORD infoClass, PVOID info, DWORD cb)
{
    return TRUE;
}

BOOL WINAPI AssignProcessToJobObject(HANDLE job, HANDLE process)
{
    return TRUE;
}

BOOL WINAPI TerminateJobObject(HANDLE job, UINT exitCode)
{
    return TRUE;
}

/*****************************************************************************
 * Console, session, power
 *****************************************************************************/
BOOL WINAPI AttachConsole(DWORD pid)
{
    SetLastError(ERROR_ACCESS_DENIED);
    return FALSE;
}

DWORD WINAPI WTSGetActiveConsoleSessionId(void)
{
    return 0;
}

DWORD WINAPI SetThreadExecutionState(DWORD state)
{
    return 0x80000000; /* ES_CONTINUOUS */
}

DWORD WINAPI SetThreadIdealProcessor(HANDLE hThread, DWORD proc)
{
    return 0;
}

BOOL WINAPI SetDllDirectoryW(LPCWSTR path)
{
    return TRUE;
}

DWORD WINAPI SetCriticalSectionSpinCount(CRITICAL_SECTION *cs, DWORD spin)
{
    return 0;
}

HANDLE WINAPI CreateMemoryResourceNotification(DWORD type)
{
    return CreateEventA(NULL, TRUE, FALSE, NULL);
}

/*****************************************************************************
 * Timer queues (only used by the Microsoft concurrency runtime): not supported
 *****************************************************************************/
HANDLE WINAPI CreateTimerQueue(void)
{
    SetLastError(FF_NOT_IMPL);
    return NULL;
}

BOOL WINAPI CreateTimerQueueTimer(PHANDLE timer, HANDLE queue, PVOID cb, PVOID param,
                                  DWORD due, DWORD period, ULONG flags)
{
    SetLastError(FF_NOT_IMPL);
    return FALSE;
}

BOOL WINAPI ChangeTimerQueueTimer(HANDLE queue, HANDLE timer, ULONG due, ULONG period)
{
    SetLastError(FF_NOT_IMPL);
    return FALSE;
}

BOOL WINAPI DeleteTimerQueueTimer(HANDLE queue, HANDLE timer, HANDLE completion)
{
    SetLastError(FF_NOT_IMPL);
    return FALSE;
}

BOOL WINAPI UnregisterWait(HANDLE wait)
{
    return TRUE;
}

/*****************************************************************************
 * Work items and synchronisation
 *****************************************************************************/
BOOL WINAPI QueueUserWorkItem(LPTHREAD_START_ROUTINE func, PVOID ctx, ULONG flags)
{
    FFWORKITEM *item;
    HANDLE      h;

    if(func == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    item = (FFWORKITEM *)HeapAlloc(GetProcessHeap(), 0, sizeof(FFWORKITEM));
    if(item == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    item->func = func;
    item->ctx  = ctx;
    h = CreateThread(NULL, 0, ffWorkItemThread, item, 0, NULL);
    if(h == NULL) {
        HeapFree(GetProcessHeap(), 0, item);
        return FALSE;
    }
    CloseHandle(h);
    return TRUE;
}

DWORD WINAPI SignalObjectAndWait(HANDLE hSignal, HANDLE hWait, DWORD timeout, BOOL alertable)
{
    if(!SetEvent(hSignal)) {
        if(!ReleaseSemaphore(hSignal, 1, NULL)) {
            if(!ReleaseMutex(hSignal)) {
                SetLastError(ERROR_INVALID_HANDLE);
                return WAIT_FAILED;
            }
        }
    }
    return WaitForSingleObjectEx(hWait, timeout, alertable);
}

/*****************************************************************************
 * System information
 *****************************************************************************/
BOOL WINAPI GetLogicalProcessorInformation(PVOID buffer, PDWORD plen)
{
    SYSTEM_INFO si;
    DWORD       i, n, need;
    DWORD      *p = (DWORD *)buffer;  /* SYSTEM_LOGICAL_PROCESSOR_INFORMATION is 24 bytes */

    if(plen == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    GetSystemInfo(&si);
    n    = si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 1;
    need = n * 24;
    if(buffer == NULL || *plen < need) {
        *plen = need;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }
    memset(buffer, 0, need);
    for(i = 0; i < n; i++, p += 6) {
        p[0] = 1u << (i & 31);   /* ProcessorMask */
        p[1] = 0;                /* RelationProcessorCore */
        ((BYTE *)p)[8] = 0;      /* Flags */
    }
    *plen = need;
    return TRUE;
}

BOOL WINAPI GetProcessHandleCount(HANDLE hProcess, PDWORD count)
{
    if(count == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *count = 64;
    return TRUE;
}

BOOL WINAPI GetSystemTimes(LPFILETIME idle, LPFILETIME kernel, LPFILETIME user)
{
    if(idle)   memset(idle, 0, sizeof(FILETIME));
    if(kernel) memset(kernel, 0, sizeof(FILETIME));
    if(user)   memset(user, 0, sizeof(FILETIME));
    return TRUE;
}

int WINAPI GetGeoInfoW(DWORD geoId, DWORD geoType, LPWSTR data, int cch, WORD lang)
{
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}

/*****************************************************************************
 * Files
 *****************************************************************************/
BOOL WINAPI ReplaceFileA(LPCSTR replaced, LPCSTR replacement, LPCSTR backup, DWORD flags,
                         PVOID exclude, PVOID reserved)
{
    if(backup != NULL) {
        if(!CopyFileA(replaced, backup, FALSE))
            return FALSE;
    }
    return MoveFileExA(replacement, replaced, MOVEFILE_REPLACE_EXISTING);
}

/*****************************************************************************
 * Table based unwinding (not used on x86): the functions only have to exist
 *****************************************************************************/
PVOID WINAPI RtlLookupFunctionEntry(DWORD pc, PDWORD imageBase, PVOID historyTable)
{
    if(imageBase)
        *imageBase = 0;
    return NULL;
}

PVOID WINAPI RtlPcToFileHeader(PVOID pc, PVOID *base)
{
    MEMORY_BASIC_INFORMATION mbi;

    if(VirtualQuery(pc, &mbi, sizeof(mbi)) == 0) {
        if(base)
            *base = NULL;
        return NULL;
    }
    if(base)
        *base = mbi.AllocationBase;
    return mbi.AllocationBase;
}

VOID WINAPI RtlUnwindEx(PVOID targetFrame, PVOID targetIp, PVOID record, PVOID retValue, PVOID context,
                        PVOID history)
{
}

PVOID WINAPI RtlVirtualUnwind(DWORD type, DWORD imageBase, DWORD pc, PVOID entry, PVOID context,
                              PVOID *handlerData, PDWORD establisher, PVOID ctxPointers)
{
    return NULL;
}

} // extern "C"

/* Waitable timers: an event handle plus a helper thread per SetWaitableTimer call.  A newer
 * SetWaitableTimer or CancelWaitableTimer call (generation counter) stops the older thread.
 * Completion routines are not supported. */
extern "C" {

#define FF_MAXTIMERS 256

static struct { HANDLE h; volatile LONG gen; } ffTimers[FF_MAXTIMERS];
static volatile LONG ffTimerLock = 0;

typedef struct {
    int   idx;
    LONG  gen;
    DWORD due;
    DWORD period;
} FFTIMERARG;

static void ffTimerLockAcquire(void) { while(InterlockedExchange((LPLONG)&ffTimerLock, 1)) Sleep(0); }
static void ffTimerLockRelease(void) { InterlockedExchange((LPLONG)&ffTimerLock, 0); }

static int ffTimerFind(HANDLE h)
{
    int i;
    for(i = 0; i < FF_MAXTIMERS; i++)
        if(ffTimers[i].h == h && h) return i;
    return -1;
}

/* sleeps in slices; returns FALSE when the timer was set again or cancelled meanwhile */
static BOOL ffTimerSleep(int idx, LONG gen, DWORD ms)
{
    while(ms) {
        DWORD slice = ms > 50 ? 50 : ms;
        Sleep(slice);
        ms -= slice;
        if(ffTimers[idx].gen != gen) return FALSE;
    }
    return ffTimers[idx].gen == gen;
}

static DWORD WINAPI ffTimerThread(LPVOID p)
{
    FFTIMERARG a = *(FFTIMERARG *)p;
    HeapFree(GetProcessHeap(), 0, p);

    if(!ffTimerSleep(a.idx, a.gen, a.due)) return 0;
    for(;;) {
        SetEvent(ffTimers[a.idx].h);
        if(!a.period) break;
        if(!ffTimerSleep(a.idx, a.gen, a.period)) break;
    }
    return 0;
}

HANDLE FF_CreateWaitableTimer(BOOL manual)
{
    HANDLE h = CreateEventA(NULL, manual, FALSE, NULL);
    int i;

    if(!h) return NULL;
    ffTimerLockAcquire();
    for(i = 0; i < FF_MAXTIMERS; i++)
        if(!ffTimers[i].h) { ffTimers[i].h = h; ffTimers[i].gen = 0; break; }
    ffTimerLockRelease();
    if(i == FF_MAXTIMERS) {
        CloseHandle(h);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    SetLastError(ERROR_SUCCESS);
    return h;
}

BOOL FF_CancelWaitableTimer(HANDLE h)
{
    int i = ffTimerFind(h);
    if(i < 0) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    InterlockedIncrement((LPLONG)&ffTimers[i].gen);
    return TRUE;
}

BOOL FF_SetWaitableTimer(HANDLE h, const LARGE_INTEGER *when, LONG period, PVOID callback, LPVOID arg, BOOL resume)
{
    int i = ffTimerFind(h);
    FFTIMERARG *a;
    LONGLONG q;
    DWORD due;
    HANDLE th;
    DWORD tid;

    if(i < 0 || !when) {
        SetLastError(i < 0 ? ERROR_INVALID_HANDLE : ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    q = ((LONGLONG)when->HighPart << 32) | when->LowPart;
    if(q < 0) {
        LONGLONG ms = (-q) / 10000;
        due = ms > 0x7FFFFFFE ? 0x7FFFFFFE : (DWORD)ms;
    }
    else {
        FILETIME ft;
        LONGLONG now, ms;
        GetSystemTimeAsFileTime(&ft);
        now = ((LONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
        ms = q > now ? (q - now) / 10000 : 0;
        due = ms > 0x7FFFFFFE ? 0x7FFFFFFE : (DWORD)ms;
    }
    ResetEvent(h);
    a = (FFTIMERARG *)HeapAlloc(GetProcessHeap(), 0, sizeof(*a));
    if(!a) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    a->idx = i;
    a->gen = InterlockedIncrement((LPLONG)&ffTimers[i].gen);
    a->due = due;
    a->period = period > 0 ? (DWORD)period : 0;
    th = CreateThread(NULL, 0, ffTimerThread, a, 0, &tid);
    if(!th) {
        HeapFree(GetProcessHeap(), 0, a);
        return FALSE;
    }
    CloseHandle(th);
    return TRUE;
}

} // extern "C"

/* Windows 8 era memory functions: Firefox read-ahead of xul.dll (without PrefetchVirtualMemory it
 * reads the whole 170 MB library through ReadFile at start-up, which takes minutes) and a precise
 * system time (the system time of OS/2 has the resolution of the timer tick anyway). */
extern "C" {

BOOL WINAPI PrefetchVirtualMemory(HANDLE process, ULONG count, PVOID entries, ULONG flags)
{
    return TRUE;                  /* a hint; the pages are loaded on demand */
}

VOID WINAPI GetSystemTimePreciseAsFileTime(LPFILETIME ft)
{
    GetSystemTimeAsFileTime(ft);
}

/* Windows 8+: process mitigation policies.  Odin has none, so setting one is accepted and ignored and
 * reading one reports "nothing enabled". */
BOOL WINAPI SetProcessMitigationPolicy(DWORD policy, PVOID buffer, ULONG length)
{
    return TRUE;
}

BOOL WINAPI GetProcessMitigationPolicy(HANDLE process, DWORD policy, PVOID buffer, ULONG length)
{
    if(buffer && length)
        memset(buffer, 0, length);
    return TRUE;
}

/* Windows Vista+: process/thread attribute lists (CreateProcess with STARTUPINFOEX).  Odin's CreateProcess ignores the
 * attributes (handle lists, parent process, mitigation policies), but callers - Chromium's crashpad - need the
 * documented size negotiation: the first call with a NULL list fails with ERROR_INSUFFICIENT_BUFFER. */
BOOL WINAPI InitializeProcThreadAttributeList(LPVOID list, DWORD count, DWORD flags, PSIZE_T size)
{
    SIZE_T need = 16 + (SIZE_T)count * 24;
    if(!size) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(!list || *size < need) {
        *size = need;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }
    memset(list, 0, need);
    ((DWORD *)list)[1] = count;
    return TRUE;
}

BOOL WINAPI UpdateProcThreadAttribute(LPVOID list, DWORD flags, DWORD_PTR attribute, PVOID value, SIZE_T size,
                                      PVOID previous, PSIZE_T returnSize)
{
    return list ? TRUE : FALSE;
}

VOID WINAPI DeleteProcThreadAttributeList(LPVOID list)
{
}

//Approximation: process CPU time (100ns units from GetProcessTimes) at an assumed 3 GHz.
BOOL WINAPI QueryProcessCycleTime(HANDLE hProcess, PULONG64 cycles)
{
    FILETIME c, e, k, u;
    ULONG64 t;

    if(!cycles) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if(!GetProcessTimes(hProcess, &c, &e, &k, &u)) return FALSE;
    t = (((ULONG64)k.dwHighDateTime << 32) | k.dwLowDateTime) + (((ULONG64)u.dwHighDateTime << 32) | u.dwLowDateTime);
    *cycles = t * 300;
    return TRUE;
}

} // extern "C"
