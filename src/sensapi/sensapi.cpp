/*
 * sensapi - System Event Notification (network state) stub
 * Odin32 compatibility DLL: provides the exports that current Windows programs
 * import so that they can be loaded; see the individual functions for what is
 * implemented and what merely reports "not supported".
 */
#include <os2win.h>

/* Network state is not tracked: report "not connected". */
extern "C" {

BOOL WINAPI IsNetworkAlive(LPDWORD lpdwFlags)
{
    if(lpdwFlags) *lpdwFlags = 0;
    return FALSE;
}

BOOL WINAPI IsDestinationReachableA(LPCSTR lpszDestination, LPVOID lpQOCInfo)
{
    return FALSE;
}

BOOL WINAPI IsDestinationReachableW(LPCWSTR lpszDestination, LPVOID lpQOCInfo)
{
    return FALSE;
}

} // extern "C"
