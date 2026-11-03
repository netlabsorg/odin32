/*
 * PSAPI: functions imported by Mozilla Firefox 160+.
 */
#include <os2win.h>

extern "C" {

DWORD WINAPI GetProcessImageFileNameW(HANDLE hProcess, LPWSTR name, DWORD size)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}

} // extern "C"
