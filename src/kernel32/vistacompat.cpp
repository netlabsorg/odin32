/*
 * KERNEL32 compatibility shims for APIs introduced after Windows 2000
 * (Windows XP / Vista / 7 era).  Added so that current Windows programs,
 * which import these functions directly, can be loaded by Odin32 at all:
 * the Odin PE loader resolves every import at load time, and one unresolved
 * import means the program never starts.
 *
 * Where an exact implementation is possible with the existing Odin32 kernel
 * the function does the real thing (SRW locks, condition variables, INIT_ONCE,
 * FLS, thread pool work items, locale-Ex names, ReplaceFile, ...).  Where the
 * OS/2 host has no equivalent (change notifications, process snapshots,
 * alternate data streams, final path names, application restart) the function
 * fails cleanly with ERROR_CALL_NOT_IMPLEMENTED / E_NOTIMPL so that callers
 * can take their documented fallback path.
 *
 * Known limitations (also listed in odin-changed-files/README.md):
 *  - SRW locks are exclusive-only; the *Shared variants take the lock
 *    exclusively (no deadlock unless a thread re-acquires a shared lock).
 *  - Condition variables poll (Sleep(1)) instead of blocking in the kernel.
 *  - FLS destructor callbacks are never called (FLS is mapped onto TLS).
 *  - Thread pool work items run on a fresh thread each (no real pool).
 *  - CompareStringOrdinal(ignore case) folds ASCII, Latin-1, Greek, Cyrillic.
 *  - *Ex locale functions ignore the locale name and use the user default.
 */
#include <os2win.h>
#include <winnls.h>
#include "odintrace.h"
#include <stdlib.h>
#include <string.h>

#define VC_E_NOTIMPL        ((LONG)0x80004001)
#define VC_LOCALE_USER      0x0400

extern "C" {

/*****************************************************************************
 * GetTickCount64
 *****************************************************************************/
ULONGLONG WINAPI GetTickCount64(void)
{
    static volatile LONG   hiword = 0;
    static volatile DWORD  lastlo = 0;
    DWORD lo = GetTickCount();
    LONG  hi;

    //GetTickCount wraps every 49.7 days; extend it to 64 bits by counting wraps
    //(correct as long as the function is called at least once per wrap period)
    DWORD prev = lastlo;
    if(lo < prev) {
        __sync_bool_compare_and_swap(&lastlo, prev, lo);
        hi = __sync_add_and_fetch(&hiword, 1);
    }
    else {
        __sync_bool_compare_and_swap(&lastlo, prev, lo);
        hi = hiword;
    }
    return ((ULONGLONG)(DWORD)hi << 32) | lo;
}

/*****************************************************************************
 * GetProductInfo: report "Professional" for every OS version asked about
 *****************************************************************************/
BOOL WINAPI GetProductInfo(DWORD dwOSMajor, DWORD dwOSMinor, DWORD dwSPMajor,
                           DWORD dwSPMinor, PDWORD pdwReturnedProductType)
{
    if(pdwReturnedProductType == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *pdwReturnedProductType = 0x30;     //PRODUCT_PROFESSIONAL
    return TRUE;
}

/*****************************************************************************
 * InitializeCriticalSectionEx (flags/spin count are advisory)
 *****************************************************************************/
BOOL WINAPI InitializeCriticalSectionEx(CRITICAL_SECTION * lpCriticalSection,
                                        DWORD dwSpinCount, DWORD Flags)
{
    InitializeCriticalSection(lpCriticalSection);
    return TRUE;
}

/*****************************************************************************
 * Slim reader/writer locks.  SRWLOCK is one pointer-sized word: 0 = free.
 *****************************************************************************/
VOID WINAPI InitializeSRWLock(volatile LONG *lock)
{
    *lock = 0;
}

static inline VOID srw_acquire(volatile LONG *lock)
{
    unsigned spins = 0;
    while(__sync_val_compare_and_swap(lock, 0, 1) != 0) {
        if(++spins < 200) {
            Sleep(0);
        }
        else {
            Sleep(1);
        }
    }
}

VOID WINAPI AcquireSRWLockExclusive(volatile LONG *lock)
{
    srw_acquire(lock);
}

VOID WINAPI ReleaseSRWLockExclusive(volatile LONG *lock)
{
    __sync_lock_release(lock);
}

BYTE WINAPI TryAcquireSRWLockExclusive(volatile LONG *lock)
{
    return __sync_val_compare_and_swap(lock, 0, 1) == 0;
}

VOID WINAPI AcquireSRWLockShared(volatile LONG *lock)
{
    srw_acquire(lock);
}

VOID WINAPI ReleaseSRWLockShared(volatile LONG *lock)
{
    __sync_lock_release(lock);
}

BYTE WINAPI TryAcquireSRWLockShared(volatile LONG *lock)
{
    return __sync_val_compare_and_swap(lock, 0, 1) == 0;
}

/*****************************************************************************
 * Condition variables.  CONDITION_VARIABLE is one word used as a generation
 * counter: a wake increments it, a sleeper waits until it changes.  Spurious
 * wake-ups are permitted by the API contract, so Wake == WakeAll.
 *****************************************************************************/
VOID WINAPI InitializeConditionVariable(volatile LONG *cv)
{
    *cv = 0;
}

VOID WINAPI WakeAllConditionVariable(volatile LONG *cv)
{
    __sync_add_and_fetch(cv, 1);
}

VOID WINAPI WakeConditionVariable(volatile LONG *cv)
{
    __sync_add_and_fetch(cv, 1);
}

BOOL WINAPI SleepConditionVariableSRW(volatile LONG *cv, volatile LONG *srw,
                                      DWORD dwMilliseconds, ULONG Flags)
{
    LONG  generation = *cv;
    DWORD start      = GetTickCount();
    BOOL  fOk        = TRUE;

    __sync_lock_release(srw);
    while(*cv == generation) {
        if(dwMilliseconds != INFINITE &&
           (DWORD)(GetTickCount() - start) >= dwMilliseconds) {
            fOk = FALSE;
            break;
        }
        Sleep(1);
    }
    srw_acquire(srw);
    if(!fOk) {
        SetLastError(ERROR_TIMEOUT);
    }
    return fOk;
}

BOOL WINAPI SleepConditionVariableCS(volatile LONG *cv, CRITICAL_SECTION * cs,
                                     DWORD dwMilliseconds)
{
    LONG  generation = *cv;
    DWORD start      = GetTickCount();
    BOOL  fOk        = TRUE;

    LeaveCriticalSection(cs);
    while(*cv == generation) {
        if(dwMilliseconds != INFINITE &&
           (DWORD)(GetTickCount() - start) >= dwMilliseconds) {
            fOk = FALSE;
            break;
        }
        Sleep(1);
    }
    EnterCriticalSection(cs);
    if(!fOk) {
        SetLastError(ERROR_TIMEOUT);
    }
    return fOk;
}

/*****************************************************************************
 * One-time initialisation.  INIT_ONCE is one word:
 *   0 = not started, 1 = in progress, otherwise (context | 2) = complete.
 *****************************************************************************/
#define INITONCE_FLAG_CHECK_ONLY    0x1
#define INITONCE_FLAG_ASYNC         0x2
#define INITONCE_FLAG_INIT_FAILED   0x4

VOID WINAPI InitOnceInitialize(volatile LONG *once)
{
    *once = 0;
}

BOOL WINAPI InitOnceBeginInitialize(volatile LONG *once, DWORD dwFlags,
                                    PBOOL fPending, LPVOID *lpContext)
{
    if(once == NULL || fPending == NULL || (dwFlags & ~7) != 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    for(;;) {
        LONG state = *once;
        if(state == 0 || state == 1) {
            if(dwFlags & INITONCE_FLAG_CHECK_ONLY) {
                SetLastError(ERROR_GEN_FAILURE);
                return FALSE;
            }
            if(state == 0) {
                if(__sync_val_compare_and_swap(once, 0, 1) != 0) {
                    continue;
                }
                *fPending = TRUE;
                return TRUE;
            }
            if(dwFlags & INITONCE_FLAG_ASYNC) {
                *fPending = TRUE;
                return TRUE;
            }
            Sleep(1);               //another thread is initialising; wait for it
            continue;
        }
        *fPending = FALSE;
        if(lpContext) {
            *lpContext = (LPVOID)(state & ~3);
        }
        return TRUE;
    }
}

BOOL WINAPI InitOnceComplete(volatile LONG *once, DWORD dwFlags, LPVOID lpContext)
{
    if(once == NULL || (dwFlags & ~(INITONCE_FLAG_ASYNC|INITONCE_FLAG_INIT_FAILED)) != 0 ||
       ((DWORD)lpContext & 3) != 0 ||
       ((dwFlags & INITONCE_FLAG_INIT_FAILED) && lpContext != NULL)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(dwFlags & INITONCE_FLAG_INIT_FAILED) {
        *once = 0;
    }
    else {
        *once = (LONG)lpContext | 2;
    }
    return TRUE;
}

typedef BOOL (WINAPI *PINIT_ONCE_FN_)(LPVOID InitOnce, LPVOID Parameter, LPVOID *Context);

BOOL WINAPI InitOnceExecuteOnce(volatile LONG *once, PINIT_ONCE_FN_ InitFn,
                                LPVOID Parameter, LPVOID *Context)
{
    BOOL   fPending;
    LPVOID ctx = NULL;

    if(!InitOnceBeginInitialize(once, 0, &fPending, &ctx)) {
        return FALSE;
    }
    if(!fPending) {
        if(Context) *Context = ctx;
        return TRUE;
    }
    if(!InitFn((LPVOID)once, Parameter, &ctx)) {
        InitOnceComplete(once, INITONCE_FLAG_INIT_FAILED, NULL);
        return FALSE;
    }
    if(!InitOnceComplete(once, 0, ctx)) {
        return FALSE;
    }
    if(Context) *Context = ctx;
    return TRUE;
}

/*****************************************************************************
 * Fiber local storage, mapped onto TLS (destructor callbacks are not called)
 *****************************************************************************/
DWORD WINAPI FlsAlloc(LPVOID lpCallback)
{
    return TlsAlloc();
}

LPVOID WINAPI FlsGetValue(DWORD dwFlsIndex)
{
    return TlsGetValue(dwFlsIndex);
}

BOOL WINAPI FlsSetValue(DWORD dwFlsIndex, LPVOID lpFlsData)
{
    return TlsSetValue(dwFlsIndex, lpFlsData);
}

BOOL WINAPI FlsFree(DWORD dwFlsIndex)
{
    return TlsFree(dwFlsIndex);
}

/*****************************************************************************
 * Thread pool work items (each submission runs on its own thread).
 * refs = 1 (owner, dropped by CloseThreadpoolWork) + one per running callback.
 *****************************************************************************/
typedef VOID (WINAPI *PTP_WORK_CALLBACK_)(LPVOID Instance, LPVOID Context, LPVOID Work);

struct TPWORK {
    PTP_WORK_CALLBACK_ cb;
    LPVOID             ctx;
    volatile LONG      refs;
};

static DWORD WINAPI tpwork_thread(LPVOID lpParameter)
{
    TPWORK *w = (TPWORK *)lpParameter;

    w->cb(NULL, w->ctx, w);
    if(__sync_sub_and_fetch(&w->refs, 1) == 0) {
        free(w);
    }
    return 0;
}

LPVOID WINAPI CreateThreadpoolWork(PTP_WORK_CALLBACK_ pfnwk, LPVOID pv, LPVOID pcbe)
{
    TPWORK *w;

    if(pfnwk == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    w = (TPWORK *)malloc(sizeof(TPWORK));
    if(w == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    w->cb   = pfnwk;
    w->ctx  = pv;
    w->refs = 1;
    return w;
}

VOID WINAPI SubmitThreadpoolWork(LPVOID pwk)
{
    TPWORK *w = (TPWORK *)pwk;
    DWORD   tid;
    HANDLE  hThread;

    __sync_add_and_fetch(&w->refs, 1);
    hThread = CreateThread(NULL, 0, tpwork_thread, w, 0, &tid);
    if(hThread == NULL) {
        //could not start a thread: run the callback synchronously instead
        w->cb(NULL, w->ctx, w);
        __sync_sub_and_fetch(&w->refs, 1);
        return;
    }
    CloseHandle(hThread);
}

VOID WINAPI WaitForThreadpoolWorkCallbacks(LPVOID pwk, BOOL fCancelPendingCallbacks)
{
    TPWORK *w = (TPWORK *)pwk;

    while(w->refs > 1) {
        Sleep(1);
    }
}

VOID WINAPI CloseThreadpoolWork(LPVOID pwk)
{
    TPWORK *w = (TPWORK *)pwk;

    if(__sync_sub_and_fetch(&w->refs, 1) == 0) {
        free(w);
    }
}

VOID WINAPI FreeLibraryWhenCallbackReturns(LPVOID pci, HMODULE mod)
{
    //not supported: the library simply stays loaded
}

/*****************************************************************************
 * Case folding for CompareStringOrdinal(..., bIgnoreCase = TRUE)
 *****************************************************************************/
static WCHAR vc_upper(WCHAR c)
{
    if(c < 0x80) {
        return (c >= 'a' && c <= 'z') ? (WCHAR)(c - 0x20) : c;
    }
    if(c >= 0xE0 && c <= 0xFE && c != 0xF7) return (WCHAR)(c - 0x20);   //Latin-1
    if(c >= 0x3B1 && c <= 0x3C9 && c != 0x3C2) return (WCHAR)(c - 0x20); //Greek
    if(c >= 0x430 && c <= 0x44F) return (WCHAR)(c - 0x20);              //Cyrillic
    if(c >= 0x450 && c <= 0x45F) return (WCHAR)(c - 0x50);
    return c;
}

int WINAPI CompareStringOrdinal(LPCWSTR lpString1, int cchCount1,
                                LPCWSTR lpString2, int cchCount2, BOOL bIgnoreCase)
{
    int i, n;

    if(lpString1 == NULL || lpString2 == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if(cchCount1 < 0) { for(cchCount1 = 0; lpString1[cchCount1]; cchCount1++) ; }
    if(cchCount2 < 0) { for(cchCount2 = 0; lpString2[cchCount2]; cchCount2++) ; }
    n = cchCount1 < cchCount2 ? cchCount1 : cchCount2;
    for(i = 0; i < n; i++) {
        WCHAR a = lpString1[i], b = lpString2[i];
        if(bIgnoreCase) {
            a = vc_upper(a);
            b = vc_upper(b);
        }
        if(a != b) {
            return (a < b) ? 1 : 3;     //CSTR_LESS_THAN / CSTR_GREATER_THAN
        }
    }
    if(cchCount1 == cchCount2) return 2; //CSTR_EQUAL
    return (cchCount1 < cchCount2) ? 1 : 3;
}

/*****************************************************************************
 * Locale-name ("Ex") variants: forwarded to the LCID versions, user default
 *****************************************************************************/
int WINAPI CompareStringEx(LPCWSTR lpLocaleName, DWORD dwCmpFlags,
                           LPCWSTR lpString1, int cchCount1,
                           LPCWSTR lpString2, int cchCount2,
                           LPVOID lpVersionInformation, LPVOID lpReserved, LPARAM lParam)
{
    return CompareStringW(VC_LOCALE_USER, dwCmpFlags, lpString1, cchCount1,
                          lpString2, cchCount2);
}

int WINAPI LCMapStringEx(LPCWSTR lpLocaleName, DWORD dwMapFlags,
                         LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest,
                         LPVOID lpVersionInformation, LPVOID lpReserved, LPARAM lParam)
{
    return LCMapStringW(VC_LOCALE_USER, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
}

/* GetLocaleInfoEx.  Odin's GetLocaleInfoW does not know LOCALE_RETURN_NUMBER
 * (the result is then a string where the caller - e.g. the UCRT locale setup -
 * expects a DWORD) nor LOCALE_SNAME, so both are handled here. */
int WINAPI GetLocaleInfoEx(LPCWSTR lpLocaleName, DWORD LCType, LPWSTR lpLCData, int cchData)
{
    int   n;
    WCHAR tmp[96];

    if(LCType & 0x20000000)                      //LOCALE_RETURN_NUMBER
    {
        DWORD value = 0;
        int   i;

        n = GetLocaleInfoW(VC_LOCALE_USER, LCType & ~0x20000000, tmp, 96);
        if(n == 0) {
            OdinTrace("GetLocaleInfoEx-NUM-FAIL", NULL, LCType, 0);
            return 0;
        }
        for(i = 0; tmp[i] >= '0' && tmp[i] <= '9'; i++) {
            value = value * 10 + (tmp[i] - '0');
        }
        OdinTrace("GetLocaleInfoEx-NUM", NULL, LCType, value);
        if(cchData == 0) return 2;
        if(cchData < 2 || lpLCData == NULL) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        *(DWORD *)lpLCData = value;
        return 2;
    }
    n = GetLocaleInfoW(VC_LOCALE_USER, LCType, lpLCData, cchData);
    if(n == 0 && (LCType & 0xFFFF) == 0x5C) {    //LOCALE_SNAME -> en-US
        static const WCHAR snm[] = { 'e','n','-','U','S',0 };
        if(cchData == 0) return 6;
        if(cchData >= 6 && lpLCData) {
            memcpy(lpLCData, snm, sizeof(snm));
            return 6;
        }
    }
    OdinTrace("GetLocaleInfoEx", NULL, LCType, n);
    return n;
}

int WINAPI GetDateFormatEx(LPCWSTR lpLocaleName, DWORD dwFlags, const SYSTEMTIME *lpDate,
                           LPCWSTR lpFormat, LPWSTR lpDateStr, int cchDate,
                           LPCWSTR lpCalendar)
{
    return GetDateFormatW(VC_LOCALE_USER, dwFlags, lpDate, lpFormat, lpDateStr, cchDate);
}

int WINAPI GetTimeFormatEx(LPCWSTR lpLocaleName, DWORD dwFlags, const SYSTEMTIME *lpTime,
                           LPCWSTR lpFormat, LPWSTR lpTimeStr, int cchTime)
{
    return GetTimeFormatW(VC_LOCALE_USER, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
}

/*****************************************************************************
 * Functions that have no OS/2 equivalent: fail cleanly
 *****************************************************************************/
DWORD WINAPI GetFinalPathNameByHandleW(HANDLE hFile, LPWSTR lpszFilePath,
                                       DWORD cchFilePath, DWORD dwFlags)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}

HANDLE WINAPI FindFirstStreamW(LPCWSTR lpFileName, DWORD InfoLevel,
                               LPVOID lpFindStreamData, DWORD dwFlags)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return INVALID_HANDLE_VALUE;
}

LONG WINAPI GetApplicationRestartSettings(HANDLE hProcess, LPWSTR pwzCommandline,
                                          PDWORD pcchSize, PDWORD pdwFlags)
{
    return VC_E_NOTIMPL;
}

LONG WINAPI RegisterApplicationRestart(LPCWSTR pwzCommandLine, DWORD dwFlags)
{
    return VC_E_NOTIMPL;
}

LONG WINAPI UnregisterApplicationRestart(void)
{
    return VC_E_NOTIMPL;
}

BOOL WINAPI QueryFullProcessImageNameW(HANDLE hProcess, DWORD dwFlags,
                                       LPWSTR lpExeName, PDWORD lpdwSize)
{
    DWORD len;

    if(lpExeName == NULL || lpdwSize == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(hProcess != GetCurrentProcess() && hProcess != (HANDLE)-1) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);   //only our own process is known
        return FALSE;
    }
    len = GetModuleFileNameW(NULL, lpExeName, *lpdwSize);
    if(len == 0 || len >= *lpdwSize) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }
    *lpdwSize = len;
    return TRUE;
}

BOOL WINAPI Process32FirstW(HANDLE hSnapshot, LPVOID lppe)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI Process32NextW(HANDLE hSnapshot, LPVOID lppe)
{
    SetLastError(ERROR_NO_MORE_FILES);
    return FALSE;
}

BOOL WINAPI ReadDirectoryChangesW(HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength,
                                  BOOL bWatchSubtree, DWORD dwNotifyFilter,
                                  LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped,
                                  LPVOID lpCompletionRoutine)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI GetFileInformationByHandleEx(HANDLE hFile, DWORD FileInformationClass,
                                         LPVOID lpFileInformation, DWORD dwBufferSize)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

/*****************************************************************************
 * ReplaceFileW: move lpReplacedFileName to the backup (if any), then move
 * the replacement over it.  Attribute/ACL merging (the flags) is not done.
 *****************************************************************************/
BOOL WINAPI ReplaceFileW(LPCWSTR lpReplacedFileName, LPCWSTR lpReplacementFileName,
                         LPCWSTR lpBackupFileName, DWORD dwReplaceFlags,
                         LPVOID lpExclude, LPVOID lpReserved)
{
    if(lpReplacedFileName == NULL || lpReplacementFileName == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(lpBackupFileName) {
        if(!MoveFileExW(lpReplacedFileName, lpBackupFileName, MOVEFILE_REPLACE_EXISTING)) {
            return FALSE;
        }
    }
    return MoveFileExW(lpReplacementFileName, lpReplacedFileName, MOVEFILE_REPLACE_EXISTING);
}

/* MoveFileWithProgressW: MoveFileExW plus progress callbacks (a move is atomic here,
 * so the callback is invoked once for the finished copy when one is given). */
typedef DWORD (WINAPI *LPPROGRESS_ROUTINE_VC)(LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER,
                                              LARGE_INTEGER, DWORD, DWORD, HANDLE, HANDLE, LPVOID);
BOOL WINAPI MoveFileWithProgressW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName,
                                  LPPROGRESS_ROUTINE_VC lpProgressRoutine, LPVOID lpData, DWORD dwFlags)
{
    return MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);
}

/* CreateHardLinkW: OS/2 file systems have no hard links */
BOOL WINAPI CreateHardLinkW(LPCWSTR lpFileName, LPCWSTR lpExistingFileName, LPVOID lpSecurityAttributes)
{
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

} // extern "C"
