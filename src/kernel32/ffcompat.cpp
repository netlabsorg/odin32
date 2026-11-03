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

} // extern "C"
