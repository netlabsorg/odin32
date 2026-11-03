/*
 * DBGHELP: SymFromAddr (the Firefox stack walker); there are no symbols to give.
 */
#include <os2win.h>

extern "C" {

BOOL WINAPI SymFromAddr(HANDLE hProcess, ULONGLONG addr, ULONGLONG *displacement, PVOID symbol)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

} // extern "C"
