/*
 * KERNEL32 compatibility shims for APIs introduced after Windows Vista
 * (Windows 7 .. 10) that modern Win32 programs import directly.  The Odin32 PE
 * loader resolves every import when the module is loaded and an import that
 * cannot be resolved ends in the "non-existing api" message box (or in a
 * program that never starts), so each function the Microsoft C/C++ runtimes
 * and the OpenJDK 17 binaries import has to exist even if only as a cleanly
 * failing stub.  Added together with vistacompat.cpp.
 *
 * Real implementations: GlobalMemoryStatusEx, GetComputerNameExA,
 * GetDynamicTimeZoneInformation, TzSpecificLocalTimeToSystemTime (fixed
 * bias, no DST rules), CreateEventExW/CreateSemaphoreExW, GetQueuedCompletionStatusEx
 * (one entry per call), SetFileInformationByHandle (end-of-file and basic
 * times only), the NUMA queries (a single node) and the small constants
 * (IsWow64Process, GetLargePageMinimum, ...).
 *
 * Stubs that fail with ERROR_CALL_NOT_IMPLEMENTED: symbolic links, file
 * streams, ReOpenFile, the thread pool timer/wait objects, PSAPI module
 * enumeration.
 */
#include <os2win.h>
#include <winnls.h>
#include "odintrace.h"
#include <string.h>
#include <stdlib.h>
#include <win/thread.h>
#include <wprocess.h>
typedef VOID (WINAPI *LPFIBER_START_ROUTINE)(LPVOID);
#ifndef ERROR_ALREADY_FIBER
#define ERROR_ALREADY_FIBER 1280
#define ERROR_ALREADY_THREAD 1281
#endif

#define W7_ERROR_NOT_IMPL   ERROR_CALL_NOT_IMPLEMENTED

typedef struct {
    DWORD     dwLength;
    DWORD     dwMemoryLoad;
    ULONGLONG ullTotalPhys;
    ULONGLONG ullAvailPhys;
    ULONGLONG ullTotalPageFile;
    ULONGLONG ullAvailPageFile;
    ULONGLONG ullTotalVirtual;
    ULONGLONG ullAvailVirtual;
    ULONGLONG ullAvailExtendedVirtual;
} W7_MEMORYSTATUSEX;

typedef struct {
    LONG       Bias;
    WCHAR      StandardName[32];
    SYSTEMTIME StandardDate;
    LONG       StandardBias;
    WCHAR      DaylightName[32];
    SYSTEMTIME DaylightDate;
    LONG       DaylightBias;
    WCHAR      TimeZoneKeyName[128];
    BOOLEAN    DynamicDaylightTimeDisabled;
} W7_DYNAMIC_TZI;

typedef struct {
    DWORD cb;
    DWORD PageFaultCount;
    DWORD PeakWorkingSetSize;
    DWORD WorkingSetSize;
    DWORD QuotaPeakPagedPoolUsage;
    DWORD QuotaPagedPoolUsage;
    DWORD QuotaPeakNonPagedPoolUsage;
    DWORD QuotaNonPagedPoolUsage;
    DWORD PagefileUsage;
    DWORD PeakPagefileUsage;
} W7_PROCESS_MEMORY_COUNTERS;

typedef struct {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union { struct { DWORD Offset; DWORD OffsetHigh; }; PVOID Pointer; };
    HANDLE    hEvent;
} W7_OVERLAPPED;

typedef struct {
    ULONG_PTR     lpCompletionKey;
    W7_OVERLAPPED *lpOverlapped;
    ULONG_PTR     Internal;
    DWORD         dwNumberOfBytesTransferred;
} W7_OVERLAPPED_ENTRY;

extern "C" {

/*****************************************************************************
 * Memory of other processes is not accessible: only the calling process works
 *****************************************************************************/
LPVOID WINAPI VirtualAllocEx(HANDLE hProcess, LPVOID addr, DWORD size, DWORD type, DWORD protect)
{
    if(hProcess != GetCurrentProcess() && hProcess != (HANDLE)0) {
        SetLastError(W7_ERROR_NOT_IMPL);
        return NULL;
    }
    return VirtualAlloc(addr, size, type, protect);
}

BOOL WINAPI VirtualFreeEx(HANDLE hProcess, LPVOID addr, DWORD size, DWORD type)
{
    if(hProcess != GetCurrentProcess() && hProcess != (HANDLE)0) {
        SetLastError(W7_ERROR_NOT_IMPL);
        return FALSE;
    }
    return VirtualFree(addr, size, type);
}

/*****************************************************************************
 * Fibers
 *****************************************************************************/
/* A fiber is a user-mode stack plus saved registers.  The current fiber is kept in the TEB's FiberData slot (NT
 * offset 0x10, the unused "selector manager list" field here); the first dword of a FIBER is the fiber parameter,
 * as on Windows.  Switching saves ebx/esi/edi/ebp on the old stack, stores esp in the fiber, and also exchanges the
 * TEB's exception chain and stack limits so that SEH and stack checks follow the fiber. */
typedef struct _ODINFIBER {
    PVOID   param;              /* 00 FiberData (must be first) */
    PVOID   except;             /* 04 saved exception chain */
    PVOID   stackTop;           /* 08 saved stack base (high) */
    PVOID   stackLow;           /* 0c saved stack limit (low) */
    DWORD   esp;                /* 10 saved stack pointer while not running */
    PVOID   stackAlloc;         /* 14 VirtualAlloc'ed stack (NULL for a converted thread) */
    LPFIBER_START_ROUTINE start;/* 18 */
    BOOL    isThread;           /* 1c */
} ODINFIBER;

extern "C" void odinSwitchStacks(DWORD *saveEsp, DWORD newEsp);
__asm__(".text\n"
        ".globl _odinSwitchStacks\n"
        "_odinSwitchStacks:\n"
        "    movl 4(%esp), %eax\n"
        "    movl 8(%esp), %edx\n"
        "    pushl %ebx\n"
        "    pushl %esi\n"
        "    pushl %edi\n"
        "    pushl %ebp\n"
        "    movl %esp, (%eax)\n"
        "    movl %edx, %esp\n"
        "    popl %ebp\n"
        "    popl %edi\n"
        "    popl %esi\n"
        "    popl %ebx\n"
        "    ret\n");

static ODINFIBER *currentFiber(void)
{
    TEB *teb = GetThreadTEB();
    return teb ? (ODINFIBER *)teb->selman_list : NULL;
}

static void WINAPI odinFiberEntry(void)
{
    ODINFIBER *f = currentFiber();
    OdinTrace("FIBER-entry", NULL, (DWORD)f, f ? (DWORD)f->start : 0);
    if(f && f->start)
        f->start(f->param);
    OdinTrace("FIBER-return", NULL, 0, 0);
    ExitThread(0);                  /* returning from a fiber function ends the thread */
}

BOOL WINAPI IsThreadAFiber(void)
{
    static int noFiber = -1;
    if(noFiber < 0) noFiber = getenv("ODIN_NOFIBER") ? 1 : 0;   /* programs that only use a fiber for a bigger stack (Chromium) then run on the thread stack */
    if(noFiber) return TRUE;
    return currentFiber() != NULL;
}

LPVOID WINAPI ConvertThreadToFiberEx(LPVOID param, DWORD flags)
{
    TEB *teb = GetThreadTEB();
    ODINFIBER *f;
    if(!teb)
        return NULL;
    if(teb->selman_list) {
        SetLastError(ERROR_ALREADY_FIBER);
        return NULL;
    }
    f = (ODINFIBER *)LocalAlloc(LPTR, sizeof(ODINFIBER));
    if(!f)
        return NULL;
    f->param = param;
    f->isThread = TRUE;
    f->except = teb->except;
    f->stackTop = teb->stack_top;
    f->stackLow = teb->stack_low;
    teb->selman_list = (DWORD)f;
    return f;
}

LPVOID WINAPI ConvertThreadToFiber(LPVOID param)
{
    return ConvertThreadToFiberEx(param, 0);
}

BOOL WINAPI ConvertFiberToThread(void)
{
    TEB *teb = GetThreadTEB();
    ODINFIBER *f = currentFiber();
    if(!teb || !f || !f->isThread) {
        SetLastError(ERROR_ALREADY_THREAD);
        return FALSE;
    }
    teb->selman_list = 0;
    LocalFree((HLOCAL)f);
    return TRUE;
}

LPVOID WINAPI CreateFiberEx(SIZE_T commit, SIZE_T reserve, DWORD flags, LPFIBER_START_ROUTINE start, LPVOID param)
{
    ODINFIBER *f;
    DWORD *sp;
    SIZE_T size = reserve ? reserve : (commit ? commit : 0x100000);
    BYTE *stack;
    if(size < 0x10000)
        size = 0x10000;
    f = (ODINFIBER *)LocalAlloc(LPTR, sizeof(ODINFIBER));
    if(!f)
        return NULL;
    stack = (BYTE *)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(!stack) {
        LocalFree((HLOCAL)f);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    f->param = param;
    f->start = start;
    f->stackAlloc = stack;
    f->stackLow = stack;
    f->stackTop = stack + size;
    f->except = (PVOID)-1;
    /* initial frame as odinSwitchStacks expects it: ebp edi esi ebx, then the "return" address */
    sp = (DWORD *)(stack + size) - 4;           /* leave room above for the dummy return address */
    *--sp = 0;                                  /* dummy return address of odinFiberEntry */
    *--sp = (DWORD)odinFiberEntry;
    *--sp = 0;                                  /* ebx */
    *--sp = 0;                                  /* esi */
    *--sp = 0;                                  /* edi */
    *--sp = 0;                                  /* ebp */
    f->esp = (DWORD)sp;
    OdinTrace("FIBER-create", NULL, (DWORD)f, (DWORD)stack);
    return f;
}

LPVOID WINAPI CreateFiber(DWORD stackSize, LPFIBER_START_ROUTINE start, LPVOID param)
{
    return CreateFiberEx(stackSize, 0, 0, start, param);
}

VOID WINAPI SwitchToFiber(LPVOID fiber)
{
    TEB *teb = GetThreadTEB();
    ODINFIBER *cur = currentFiber(), *to = (ODINFIBER *)fiber;
    if(!teb || !cur || !to || cur == to)
        return;
    OdinTrace("FIBER-switch", NULL, (DWORD)cur, (DWORD)to);
    cur->except = teb->except;
    cur->stackTop = teb->stack_top;
    cur->stackLow = teb->stack_low;
    teb->except = to->except;
    teb->stack_top = to->stackTop;
    teb->stack_low = to->stackLow;
    teb->selman_list = (DWORD)to;
    odinSwitchStacks(&cur->esp, to->esp);
}

VOID WINAPI DeleteFiber(LPVOID fiber)
{
    ODINFIBER *f = (ODINFIBER *)fiber;
    if(!f)
        return;
    if(f == currentFiber())
        ExitThread(0);              /* deleting the running fiber ends the thread */
    if(f->stackAlloc)
        VirtualFree(f->stackAlloc, 0, MEM_RELEASE);
    LocalFree((HLOCAL)f);
}

/*****************************************************************************
 * Time zones
 *****************************************************************************/
BOOL WINAPI TzSpecificLocalTimeToSystemTime(const TIME_ZONE_INFORMATION *tzi,
                                            const SYSTEMTIME *local, SYSTEMTIME *utc)
{
    TIME_ZONE_INFORMATION cur;
    FILETIME   ft;
    ULONGLONG  t;
    LONG       bias;
    DWORD      id;

    if(!local || !utc) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(!tzi) {
        id  = GetTimeZoneInformation(&cur);
        tzi = &cur;
    }
    else id = TIME_ZONE_ID_STANDARD;

    bias = tzi->Bias;
    //no transition rules evaluated: use the daylight bias only when the active zone says so
    if(id == TIME_ZONE_ID_DAYLIGHT) bias += tzi->DaylightBias;
    else                            bias += tzi->StandardBias;

    if(!SystemTimeToFileTime(local, &ft))
        return FALSE;
    t  = ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    t += (LONGLONG)bias * 60 * 10000000;       //UTC = local + bias
    ft.dwLowDateTime  = (DWORD)t;
    ft.dwHighDateTime = (DWORD)(t >> 32);
    return FileTimeToSystemTime(&ft, utc);
}

DWORD WINAPI GetDynamicTimeZoneInformation(W7_DYNAMIC_TZI *dtzi)
{
    TIME_ZONE_INFORMATION tzi;
    DWORD id;

    if(!dtzi) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return TIME_ZONE_ID_INVALID;
    }
    id = GetTimeZoneInformation(&tzi);
    memset(dtzi, 0, sizeof(*dtzi));
    dtzi->Bias         = tzi.Bias;
    dtzi->StandardBias = tzi.StandardBias;
    dtzi->DaylightBias = tzi.DaylightBias;
    dtzi->StandardDate = tzi.StandardDate;
    dtzi->DaylightDate = tzi.DaylightDate;
    memcpy(dtzi->StandardName, tzi.StandardName, sizeof(dtzi->StandardName));
    memcpy(dtzi->DaylightName, tzi.DaylightName, sizeof(dtzi->DaylightName));
    return id;
}

/*****************************************************************************
 * Memory
 *****************************************************************************/
BOOL WINAPI GlobalMemoryStatusEx(W7_MEMORYSTATUSEX *ms)
{
    MEMORYSTATUS old;

    if(!ms || ms->dwLength != sizeof(W7_MEMORYSTATUSEX)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    old.dwLength = sizeof(old);
    GlobalMemoryStatus(&old);
    ms->dwMemoryLoad            = old.dwMemoryLoad;
    ms->ullTotalPhys            = old.dwTotalPhys;
    ms->ullAvailPhys            = old.dwAvailPhys;
    ms->ullTotalPageFile        = old.dwTotalPageFile;
    ms->ullAvailPageFile        = old.dwAvailPageFile;
    ms->ullTotalVirtual         = old.dwTotalVirtual;
    ms->ullAvailVirtual         = old.dwAvailVirtual;
    ms->ullAvailExtendedVirtual = 0;
    return TRUE;
}

SIZE_T WINAPI GetLargePageMinimum(void)
{
    return 0;                                   //large pages are not supported
}

BOOL WINAPI GetNumaHighestNodeNumber(PULONG highest)
{
    if(!highest) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *highest = 0;                               //one NUMA node
    return TRUE;
}

BOOL WINAPI GetNumaNodeProcessorMask(UCHAR node, ULONGLONG *mask)
{
    DWORD procmask = 0, sysmask = 0;

    if(node != 0 || !mask) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    GetProcessAffinityMask(GetCurrentProcess(), &procmask, &sysmask);
    *mask = sysmask;
    return TRUE;
}

LPVOID WINAPI VirtualAllocExNuma(HANDLE hProcess, LPVOID addr, SIZE_T size,
                                 DWORD type, DWORD protect, DWORD node)
{
    return VirtualAllocEx(hProcess, addr, size, type, protect);
}

/*****************************************************************************
 * Process / system information
 *****************************************************************************/
BOOL WINAPI IsWow64Process(HANDLE hProcess, PBOOL wow64)
{
    if(!wow64) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *wow64 = FALSE;
    return TRUE;
}

BOOL WINAPI ProcessIdToSessionId(DWORD pid, DWORD *session)
{
    if(!session) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *session = 0;
    return TRUE;
}

DWORD WINAPI GetProcessId(HANDLE hProcess)
{
    if(hProcess == GetCurrentProcess())
        return GetCurrentProcessId();
    SetLastError(ERROR_INVALID_HANDLE);         //process handles carry no id here
    return 0;
}

DWORD WINAPI GetCurrentProcessorNumber(void)
{
    return 0;
}

VOID WINAPI FlushProcessWriteBuffers(void)
{
    __sync_synchronize();
}

HWND WINAPI GetConsoleWindow(void)
{
    return 0;
}

BOOL WINAPI GetComputerNameExA(DWORD nameType, LPSTR buf, LPDWORD size)
{
    char  tmp[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD len = sizeof(tmp);

    if(!size) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(!GetComputerNameA(tmp, &len))
        return FALSE;
    //NetBIOS, DNS host name, DNS domain, FQDN ...: the single name we know
    if(*size <= len) {
        *size = len + 1;
        SetLastError(ERROR_MORE_DATA);
        return FALSE;
    }
    if(buf) {
        memcpy(buf, tmp, len + 1);
    }
    *size = len;
    return TRUE;
}

int WINAPI GetGeoInfoA(DWORD geoId, DWORD geoType, LPSTR data, int cch, WORD lang)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return 0;
}

DWORD WINAPI GetUserGeoID(DWORD geoClass)
{
    return 0xFFFFFFFF;                          //GEOID_NOT_AVAILABLE
}

/*****************************************************************************
 * PSAPI entry points that moved into kernel32 (K32*)
 *****************************************************************************/
BOOL WINAPI K32EnumProcessModules(HANDLE hProcess, HMODULE *mods, DWORD cb, LPDWORD needed)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return FALSE;
}

DWORD WINAPI K32GetModuleFileNameExA(HANDLE hProcess, HMODULE hModule, LPSTR name, DWORD size)
{
    if(hProcess != GetCurrentProcess()) {
        SetLastError(W7_ERROR_NOT_IMPL);
        return 0;
    }
    return GetModuleFileNameA(hModule, name, size);
}

BOOL WINAPI K32GetModuleInformation(HANDLE hProcess, HMODULE hModule, PVOID info, DWORD cb)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return FALSE;
}

BOOL WINAPI K32GetProcessMemoryInfo(HANDLE hProcess, W7_PROCESS_MEMORY_COUNTERS *pmc, DWORD cb)
{
    if(!pmc || cb < sizeof(DWORD)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    memset(pmc, 0, cb < sizeof(*pmc) ? cb : sizeof(*pmc));
    pmc->cb = cb < sizeof(*pmc) ? cb : sizeof(*pmc);
    return TRUE;                                //counters are not tracked: report zero
}

USHORT WINAPI RtlCaptureStackBackTrace(ULONG skip, ULONG count, PVOID *frames, PULONG hash)
{
    if(hash) *hash = 0;
    return 0;                                   //no frames captured
}

/*****************************************************************************
 * Synchronisation objects with access/flags parameters
 *****************************************************************************/
HANDLE WINAPI CreateEventExW(LPSECURITY_ATTRIBUTES attr, LPCWSTR name, DWORD flags, DWORD access)
{
    return CreateEventW(attr, (flags & 0x1) != 0, (flags & 0x2) != 0, name);
}

HANDLE WINAPI CreateSemaphoreExW(LPSECURITY_ATTRIBUTES attr, LONG initial, LONG maxcount,
                                 LPCWSTR name, DWORD flags, DWORD access)
{
    return CreateSemaphoreW(attr, initial, maxcount, name);
}

/*****************************************************************************
 * I/O completion: one packet per call
 *****************************************************************************/
BOOL WINAPI GetQueuedCompletionStatusEx(HANDLE port, W7_OVERLAPPED_ENTRY *entries, ULONG count,
                                        PULONG removed, DWORD timeout, BOOL alertable)
{
    DWORD       bytes = 0;
    DWORD       key = 0;
    LPOVERLAPPED ov = NULL;
    BOOL        ok;

    if(!entries || count == 0 || !removed) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    ok = GetQueuedCompletionStatus(port, &bytes, &key, &ov, timeout);
    if(!ok && ov == NULL)
        return FALSE;                           //timeout or error: nothing dequeued
    entries[0].lpCompletionKey            = key;
    entries[0].lpOverlapped               = (W7_OVERLAPPED *)ov;
    entries[0].Internal                   = 0;
    entries[0].dwNumberOfBytesTransferred = bytes;
    *removed = 1;
    return TRUE;
}

BOOL WINAPI SetFileCompletionNotificationModes(HANDLE hFile, UCHAR flags)
{
    return TRUE;                                //packets are always queued: the only mode we have
}

/*****************************************************************************
 * Files
 *****************************************************************************/
BOOL WINAPI SetFileInformationByHandle(HANDLE hFile, DWORD infoClass, LPVOID info, DWORD size)
{
    switch(infoClass)
    {
    case 6:     //FileEndOfFileInfo
        if(size >= sizeof(LONGLONG)) {
            LONG lo = (LONG)((LONGLONG *)info)[0];
            LONG hi = (LONG)(((LONGLONG *)info)[0] >> 32);
            if(SetFilePointer(hFile, lo, &hi, FILE_BEGIN) == 0xFFFFFFFF && GetLastError() != NO_ERROR)
                return FALSE;
            return SetEndOfFile(hFile);
        }
        break;
    case 0: {   //FileBasicInfo: CreationTime, LastAccessTime, LastWriteTime, ChangeTime, Attributes
        if(size >= 4 * sizeof(LONGLONG)) {
            LONGLONG *t = (LONGLONG *)info;
            FILETIME c, a, w;
            c.dwLowDateTime = (DWORD)t[0]; c.dwHighDateTime = (DWORD)(t[0] >> 32);
            a.dwLowDateTime = (DWORD)t[1]; a.dwHighDateTime = (DWORD)(t[1] >> 32);
            w.dwLowDateTime = (DWORD)t[2]; w.dwHighDateTime = (DWORD)(t[2] >> 32);
            return SetFileTime(hFile, t[0] ? &c : NULL, t[1] ? &a : NULL, t[2] ? &w : NULL);
        }
        break;
    }
    default:
        break;
    }
    SetLastError(W7_ERROR_NOT_IMPL);
    return FALSE;
}

HANDLE WINAPI ReOpenFile(HANDLE hOriginal, DWORD access, DWORD share, DWORD flags)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return INVALID_HANDLE_VALUE;
}

BOOL WINAPI FindNextStreamW(HANDLE hFind, LPVOID data)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return FALSE;
}

BOOLEAN WINAPI CreateSymbolicLinkW(LPCWSTR link, LPCWSTR target, DWORD flags)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return FALSE;
}

/*****************************************************************************
 * Thread pool timer and wait objects (the C++ runtime imports them)
 *****************************************************************************/
PVOID WINAPI CreateThreadpoolTimer(PVOID callback, PVOID context, PVOID env)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return NULL;
}

VOID WINAPI SetThreadpoolTimer(PVOID timer, PFILETIME due, DWORD period, DWORD window)
{
}

VOID WINAPI WaitForThreadpoolTimerCallbacks(PVOID timer, BOOL cancelPending)
{
}

VOID WINAPI CloseThreadpoolTimer(PVOID timer)
{
}

PVOID WINAPI CreateThreadpoolWait(PVOID callback, PVOID context, PVOID env)
{
    SetLastError(W7_ERROR_NOT_IMPL);
    return NULL;
}

VOID WINAPI SetThreadpoolWait(PVOID wait, HANDLE h, PFILETIME timeout)
{
}

VOID WINAPI CloseThreadpoolWait(PVOID wait)
{
}

/*****************************************************************************
 * Processor groups and jobs (queried by the Java 21 VM; Odin has neither)
 *****************************************************************************/
DWORD WINAPI GetActiveProcessorCount(WORD group)
{
    SYSTEM_INFO si;
    if(group != 0 && group != 0xFFFF) {
        return 0;
    }
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}

BOOL WINAPI GetProcessGroupAffinity(HANDLE hProcess, USHORT *pGroupCount, USHORT *pGroupArray)
{
    if(!pGroupCount || *pGroupCount < 1 || !pGroupArray) {
        if(pGroupCount) *pGroupCount = 1;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }
    *pGroupCount = 1;
    pGroupArray[0] = 0;
    return TRUE;
}

BOOL WINAPI IsProcessInJob(HANDLE hProcess, HANDLE hJob, PBOOL pResult)
{
    if(pResult) *pResult = FALSE;
    return TRUE;
}

BOOL WINAPI QueryInformationJobObject(HANDLE hJob, DWORD infoClass, PVOID info, DWORD cb, PDWORD pcbReturned)
{
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
}

VOID WINAPI RaiseFailFastException(PVOID record, PVOID context, DWORD flags)
{
    ExitProcess(0xC0000409);
}

} // extern "C"
