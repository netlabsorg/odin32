/*
 * ADVAPI32: RegGetValueW (Windows Vista / XP SP3 and later).
 * Open the optional sub key, query the value, apply the RRF_RT_* type filter.
 * REG_EXPAND_SZ values are returned unexpanded; RRF_ZEROONFAILURE honoured.
 */
#include <os2win.h>
#include <winreg.h>
#include <winnt.h>
#include <string.h>

#define VC_RRF_RT_REG_NONE        0x00000001
#define VC_RRF_RT_REG_SZ          0x00000002
#define VC_RRF_RT_REG_EXPAND_SZ   0x00000004
#define VC_RRF_RT_REG_BINARY      0x00000008
#define VC_RRF_RT_REG_DWORD       0x00000010
#define VC_RRF_RT_REG_MULTI_SZ    0x00000020
#define VC_RRF_RT_REG_QWORD       0x00000040
#define VC_RRF_RT_ANY             0x0000FFFF
#define VC_RRF_ZEROONFAILURE      0x20000000

extern "C" LONG WINAPI RegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue,
                                    DWORD dwFlags, LPDWORD pdwType, PVOID pvData,
                                    LPDWORD pcbData)
{
    HKEY   hSub = hkey;
    DWORD  type = 0, cb = 0;
    LONG   rc;
    DWORD  filter = dwFlags & VC_RRF_RT_ANY;

    if(pvData && !pcbData) {
        return ERROR_INVALID_PARAMETER;
    }
    rc = ERROR_SUCCESS;
    if(lpSubKey && lpSubKey[0]) {
        rc = RegOpenKeyExW(hkey, lpSubKey, 0, KEY_QUERY_VALUE, &hSub);
    }
    if(rc == ERROR_SUCCESS) {
        if(pcbData) cb = *pcbData;
        rc = RegQueryValueExW(hSub, lpValue, NULL, &type, (LPBYTE)pvData, pcbData ? &cb : NULL);
        if(rc == ERROR_SUCCESS || rc == ERROR_MORE_DATA) {
            if(pcbData) *pcbData = cb;
            if(pdwType) *pdwType = type;
            if(rc == ERROR_SUCCESS && filter && filter != VC_RRF_RT_ANY) {
                DWORD bit;
                switch(type) {
                case REG_NONE:       bit = VC_RRF_RT_REG_NONE; break;
                case REG_SZ:         bit = VC_RRF_RT_REG_SZ; break;
                case REG_EXPAND_SZ:  bit = VC_RRF_RT_REG_EXPAND_SZ; break;
                case REG_BINARY:     bit = VC_RRF_RT_REG_BINARY; break;
                case REG_DWORD:      bit = VC_RRF_RT_REG_DWORD; break;
                case REG_MULTI_SZ:   bit = VC_RRF_RT_REG_MULTI_SZ; break;
                case 11:             bit = VC_RRF_RT_REG_QWORD; break;
                default:             bit = 0; break;
                }
                if(!(filter & bit)) {
                    rc = ERROR_UNSUPPORTED_TYPE;
                }
            }
        }
        if(hSub != hkey) {
            RegCloseKey(hSub);
        }
    }
    if(rc != ERROR_SUCCESS && (dwFlags & VC_RRF_ZEROONFAILURE) && pvData && pcbData) {
        memset(pvData, 0, *pcbData);
    }
    return rc;
}
