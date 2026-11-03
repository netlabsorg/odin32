/*
 * SHLWAPI: SHRegGetValueW, used by Mozilla Firefox 160+.  It is RegGetValueW with
 * the SRRF_* flags (which have the same values as RRF_*).
 */
#include <os2win.h>

extern "C" {

LONG WINAPI RegGetValueW(HKEY hkey, LPCWSTR subKey, LPCWSTR value, DWORD flags,
                         LPDWORD type, PVOID data, LPDWORD cbData);

LONG WINAPI SHRegGetValueW(HKEY hkey, LPCWSTR subKey, LPCWSTR value, DWORD flags,
                           LPDWORD type, PVOID data, LPDWORD cbData)
{
    return RegGetValueW(hkey, subKey, value, flags, type, data, cbData);
}

} // extern "C"
