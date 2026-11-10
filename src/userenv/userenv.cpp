/*
 * userenv - user environment / profile / group-policy API.  Chromium delay-loads
 * these (CreateEnvironmentBlock for launching children, GetProfileType, the
 * critical policy section and GP notification calls); without the DLL its
 * delay-load failure handler crashes the browser at start-up.
 */
#include <os2win.h>
#include <string.h>

extern "C" {

BOOL WINAPI CreateEnvironmentBlock(LPVOID *lpEnvironment, HANDLE hToken, BOOL bInherit)
{
    LPWSTR env, p;
    SIZE_T cb;
    LPVOID copy;

    if(!lpEnvironment) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    env = GetEnvironmentStringsW();
    if(!env) { SetLastError(ERROR_OUTOFMEMORY); return FALSE; }
    p = env;
    while(*p) p += lstrlenW(p) + 1;
    cb = (p - env + 1) * sizeof(WCHAR);
    copy = HeapAlloc(GetProcessHeap(), 0, cb);
    if(copy) memcpy(copy, env, cb);
    FreeEnvironmentStringsW(env);
    if(!copy) { SetLastError(ERROR_OUTOFMEMORY); return FALSE; }
    *lpEnvironment = copy;
    return TRUE;
}

BOOL WINAPI DestroyEnvironmentBlock(LPVOID lpEnvironment)
{
    if(!lpEnvironment) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    return HeapFree(GetProcessHeap(), 0, lpEnvironment);
}

LONG WINAPI DeriveAppContainerSidFromAppContainerName(LPCWSTR name, PVOID *sid)
{
    if(sid) *sid = NULL;
    return (LONG)0x80004001;   //E_NOTIMPL
}

HANDLE WINAPI EnterCriticalPolicySection(BOOL bMachine)
{
    return (HANDLE)1;
}

BOOL WINAPI LeaveCriticalPolicySection(HANDLE hSection)
{
    return TRUE;
}

BOOL WINAPI GetProfileType(DWORD *pdwFlags)
{
    if(!pdwFlags) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    *pdwFlags = 0;      //local profile
    return TRUE;
}

BOOL WINAPI RegisterGPNotification(HANDLE hEvent, BOOL bMachine)
{
    return TRUE;
}

BOOL WINAPI UnregisterGPNotification(HANDLE hEvent)
{
    return TRUE;
}

static BOOL profileDirW(LPWSTR buf, LPDWORD len)
{
    WCHAR tmp[MAX_PATH];
    DWORD n;

    if(!len) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    n = GetEnvironmentVariableW(L"USERPROFILE", tmp, MAX_PATH);
    if(n == 0 || n >= MAX_PATH) { lstrcpyW(tmp, L"C:\\Users\\Default"); n = lstrlenW(tmp); }
    if(!buf || *len < n + 1) { *len = n + 1; SetLastError(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
    lstrcpyW(buf, tmp);
    *len = n + 1;
    return TRUE;
}

BOOL WINAPI GetUserProfileDirectoryW(HANDLE hToken, LPWSTR lpProfileDir, LPDWORD lpcchSize)
{
    return profileDirW(lpProfileDir, lpcchSize);
}

BOOL WINAPI GetUserProfileDirectoryA(HANDLE hToken, LPSTR lpProfileDir, LPDWORD lpcchSize)
{
    WCHAR w[MAX_PATH];
    DWORD n = MAX_PATH;
    if(!profileDirW(w, &n)) return FALSE;
    if(!lpProfileDir || !lpcchSize || *lpcchSize < n) {
        if(lpcchSize) *lpcchSize = n;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }
    WideCharToMultiByte(0, 0, w, -1, lpProfileDir, *lpcchSize, NULL, NULL);
    *lpcchSize = n;
    return TRUE;
}

} // extern "C"
