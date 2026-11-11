/*
 * ADVAPI32: RegGetValueW (Windows Vista / XP SP3 and later).
 * Open the optional sub key, query the value, apply the RRF_RT_* type filter.
 * REG_EXPAND_SZ values are returned unexpanded; RRF_ZEROONFAILURE honoured.
 */
#include <os2win.h>
#include <winreg.h>
#include <winnt.h>
#include <string.h>
#include <stdlib.h>

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

/*****************************************************************************
 * Security descriptor / token functions imported by java.nio's file attribute
 * code.  Odin has no security model, so they fail cleanly.
 *****************************************************************************/
BOOL WINAPI AddAccessDeniedAceEx(PACL acl, DWORD rev, DWORD flags, DWORD mask, PSID sid)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

/* "S-1-5-32-544" or an SDDL alias ("WD" = Everyone, "SY", "BA" ...) to a SID in LocalAlloc'ed memory.  Chromium's logging
 * set-up asks for L"WD" and treats a failure as fatal. */
static const struct { char a[3]; const char *sid; } sddlSids[] = {
    { "WD", "S-1-1-0" },     { "CO", "S-1-3-0" },       { "CG", "S-1-3-1" },       { "OW", "S-1-3-4" },
    { "NU", "S-1-5-2" },     { "IU", "S-1-5-4" },       { "SU", "S-1-5-6" },       { "AN", "S-1-5-7" },
    { "PS", "S-1-5-10" },    { "AU", "S-1-5-11" },      { "RC", "S-1-5-12" },      { "SY", "S-1-5-18" },
    { "LS", "S-1-5-19" },    { "NS", "S-1-5-20" },      { "BA", "S-1-5-32-544" },  { "BU", "S-1-5-32-545" },
    { "BG", "S-1-5-32-546" },{ "PU", "S-1-5-32-547" },  { "AO", "S-1-5-32-548" },  { "SO", "S-1-5-32-549" },
    { "PO", "S-1-5-32-550" },{ "BO", "S-1-5-32-551" },  { "RE", "S-1-5-32-552" },  { "RU", "S-1-5-32-554" },
    { "RD", "S-1-5-32-555" },{ "NO", "S-1-5-32-556" },  { "LU", "S-1-5-32-559" },  { "WR", "S-1-5-33" },
    { "AC", "S-1-15-2-1" },  { "ED", "S-1-5-9" },       { "RS", "S-1-5-32-575" },  { "ES", "S-1-5-32-573" }
};

static BOOL parseSidString(const char *s, PSID *sid)
{
    DWORD auth[6], subs[15];
    ULONGLONG idauth = 0;
    int   nsub = 0, i;
    char *e;
    PBYTE p;

    if((s[0] != 'S' && s[0] != 's') || s[1] != '-')
        return FALSE;
    s += 2;
    if(strtoul(s, &e, 10) != 1 || *e != '-')       /* revision 1 */
        return FALSE;
    s = e + 1;
    idauth = strtoul(s, &e, 0);
    if(e == s)
        return FALSE;
    s = e;
    while(*s == '-' && nsub < 15) {
        subs[nsub++] = strtoul(s + 1, &e, 10);
        if(e == s + 1)
            return FALSE;
        s = e;
    }
    if(*s || nsub == 0)
        return FALSE;
    p = (PBYTE)LocalAlloc(LPTR, 8 + 4 * nsub);
    if(!p) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    p[0] = 1;
    p[1] = (BYTE)nsub;
    for(i = 0; i < 6; i++)
        p[2 + i] = (BYTE)(idauth >> (8 * (5 - i)));
    for(i = 0; i < nsub; i++)
        memcpy(p + 8 + 4 * i, &subs[i], 4);
    *sid = (PSID)p;
    return TRUE;
}

BOOL WINAPI ConvertStringSidToSidW(LPCWSTR str, PSID *sid)
{
    char buf[100];
    int  n = 0, i;

    if(!str || !sid) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    while(str[n] && n < 90) {
        buf[n] = (str[n] < 128) ? (char)str[n] : '?';
        n++;
    }
    buf[n] = 0;
    if(str[n]) {
        SetLastError(ERROR_INVALID_SID);
        return FALSE;
    }
    if(n == 2) {
        for(i = 0; i < (int)(sizeof(sddlSids) / sizeof(sddlSids[0])); i++) {
            if(sddlSids[i].a[0] == buf[0] && sddlSids[i].a[1] == buf[1]) {
                if(parseSidString(sddlSids[i].sid, sid)) {
                    SetLastError(ERROR_SUCCESS);
                    return TRUE;
                }
                return FALSE;
            }
        }
    }
    else if(parseSidString(buf, sid)) {
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    SetLastError(ERROR_INVALID_SID);
    return FALSE;
}

/* Odin keeps no real ACLs: hand out a minimal self-relative descriptor (revision 1, no owner/group/ACLs) so callers
 * that only pass it on (Chromium's chrome_elf.dll builds one for its shared objects) work. */
static BOOL minimalSecurityDescriptor(PVOID *sd, PULONG size)
{
    BYTE *p;
    if(!sd) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    p = (BYTE *)LocalAlloc(LPTR, 20);
    if(!p) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    p[0] = 1;                       /* Revision */
    p[3] = 0x80;                    /* Control: SE_SELF_RELATIVE */
    *sd = p;
    if(size)
        *size = 20;
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}

BOOL WINAPI ConvertStringSecurityDescriptorToSecurityDescriptorA(LPCSTR str, DWORD rev, PVOID *sd, PULONG size)
{
    return minimalSecurityDescriptor(sd, size);
}

BOOL WINAPI ConvertStringSecurityDescriptorToSecurityDescriptorW(LPCWSTR str, DWORD rev, PVOID *sd, PULONG size)
{
    return minimalSecurityDescriptor(sd, size);
}

BOOL WINAPI DuplicateTokenEx(HANDLE token, DWORD access, LPSECURITY_ATTRIBUTES attr,
                             DWORD level, DWORD type, PHANDLE newToken)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI SaferiIsExecutableFileType(LPCWSTR path, BOOLEAN fromShell)
{
    return FALSE;
}
