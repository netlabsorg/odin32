/*
 * winhttp - WinHTTP stub.  Java asks it for the proxy configuration; every
 * call reports that no WinHTTP session can be opened, which makes callers fall
 * back to a direct connection.
 */
#include <os2win.h>

extern "C" {

PVOID WINAPI WinHttpOpen(LPCWSTR agent, DWORD access, LPCWSTR proxy, LPCWSTR bypass, DWORD flags)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
}

BOOL WINAPI WinHttpGetIEProxyConfigForCurrentUser(PVOID config)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI WinHttpGetProxyForUrl(PVOID session, LPCWSTR url, PVOID options, PVOID proxyInfo)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI WinHttpCloseHandle(PVOID h)
{
    return TRUE;
}

} // extern "C"
