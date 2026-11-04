/*
 * wtsapi32 - terminal services stub (Mozilla Firefox registers for session change notifications).
 * There is only one session and it never changes, so registering succeeds and nothing is ever sent;
 * querying session information fails.
 */
#include <os2win.h>

extern "C" {

BOOL WINAPI WTSRegisterSessionNotification(HWND hwnd, DWORD flags)
{
    return TRUE;
}

BOOL WINAPI WTSUnRegisterSessionNotification(HWND hwnd)
{
    return TRUE;
}

BOOL WINAPI WTSQuerySessionInformationW(HANDLE server, DWORD sessionId, int infoClass, LPWSTR *buffer, DWORD *bytes)
{
    if(buffer) *buffer = NULL;
    if(bytes)  *bytes = 0;
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL WINAPI WTSQuerySessionInformationA(HANDLE server, DWORD sessionId, int infoClass, LPSTR *buffer, DWORD *bytes)
{
    if(buffer) *buffer = NULL;
    if(bytes)  *bytes = 0;
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

VOID WINAPI WTSFreeMemory(PVOID memory)
{
}

} // extern "C"
