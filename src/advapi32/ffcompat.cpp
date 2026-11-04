/*
 * ADVAPI32: security, credential and event tracing functions imported by the
 * Mozilla Firefox 160+ binaries.  Odin has no security model and no ETW, so the
 * functions fail cleanly (or succeed without doing anything for tracing).
 */
#include <os2win.h>
#include <string.h>

extern "C" {

BOOL WINAPI AddMandatoryAce(PVOID acl, DWORD rev, DWORD flags, DWORD policy, PVOID labelSid)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

VOID WINAPI BuildTrusteeWithSidW(PVOID trustee, PVOID sid)
{
    if(trustee) {
        memset(trustee, 0, 20);          /* TRUSTEE_W: 5 DWORDs */
        ((DWORD *)trustee)[4] = (DWORD)sid;
    }
}

BOOL WINAPI CreatePrivateObjectSecurityEx(PVOID parent, PVOID creator, PVOID *newDesc, PVOID objType,
                                          BOOL isContainer, ULONG autoInherit, HANDLE token, PVOID genericMapping)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

/* CreateWellKnownSid: the SIDs of the common well known accounts and mandatory labels */
typedef struct {
    int   type;            /* WELL_KNOWN_SID_TYPE */
    BYTE  authority;       /* last byte of the identifier authority */
    BYTE  count;           /* number of sub authorities */
    DWORD sub[2];
} WKSID;

static const WKSID wkSids[] = {
    {  0, 0, 1, { 0, 0 } },            /* WinNullSid                    S-1-0-0 */
    {  1, 1, 1, { 0, 0 } },            /* WinWorldSid                   S-1-1-0 */
    {  2, 2, 1, { 0, 0 } },            /* WinLocalSid                   S-1-2-0 */
    {  3, 3, 1, { 0, 0 } },            /* WinCreatorOwnerSid            S-1-3-0 */
    {  4, 3, 1, { 1, 0 } },            /* WinCreatorGroupSid            S-1-3-1 */
    {  7, 5, 0, { 0, 0 } },            /* WinNtAuthoritySid             S-1-5 */
    {  8, 5, 1, { 1, 0 } },            /* WinDialupSid                  S-1-5-1 */
    {  9, 5, 1, { 2, 0 } },            /* WinNetworkSid                 S-1-5-2 */
    { 10, 5, 1, { 3, 0 } },            /* WinBatchSid                   S-1-5-3 */
    { 11, 5, 1, { 4, 0 } },            /* WinInteractiveSid             S-1-5-4 */
    { 12, 5, 1, { 6, 0 } },            /* WinServiceSid                 S-1-5-6 */
    { 13, 5, 1, { 7, 0 } },            /* WinAnonymousSid               S-1-5-7 */
    { 15, 5, 1, { 9, 0 } },            /* WinEnterpriseControllersSid   S-1-5-9 */
    { 16, 5, 1, { 10, 0 } },           /* WinSelfSid                    S-1-5-10 */
    { 17, 5, 1, { 11, 0 } },           /* WinAuthenticatedUserSid       S-1-5-11 */
    { 18, 5, 1, { 12, 0 } },           /* WinRestrictedCodeSid          S-1-5-12 */
    { 19, 5, 1, { 13, 0 } },           /* WinTerminalServerSid          S-1-5-13 */
    { 20, 5, 1, { 14, 0 } },           /* WinRemoteLogonIdSid           S-1-5-14 */
    { 22, 5, 1, { 18, 0 } },           /* WinLocalSystemSid             S-1-5-18 */
    { 23, 5, 1, { 19, 0 } },           /* WinLocalServiceSid            S-1-5-19 */
    { 24, 5, 1, { 20, 0 } },           /* WinNetworkServiceSid          S-1-5-20 */
    { 25, 5, 1, { 32, 0 } },           /* WinBuiltinDomainSid           S-1-5-32 */
    { 26, 5, 2, { 32, 544 } },         /* WinBuiltinAdministratorsSid   S-1-5-32-544 */
    { 27, 5, 2, { 32, 545 } },         /* WinBuiltinUsersSid            S-1-5-32-545 */
    { 28, 5, 2, { 32, 546 } },         /* WinBuiltinGuestsSid           S-1-5-32-546 */
    { 29, 5, 2, { 32, 547 } },         /* WinBuiltinPowerUsersSid       S-1-5-32-547 */
    { 51, 5, 1, { 33, 0 } },           /* WinWriteRestrictedCodeSid     S-1-5-33 */
    { 64, 5, 2, { 64, 10 } },          /* WinNTLMAuthenticationSid      S-1-5-64-10 */
    { 65, 16, 1, { 0, 0 } },           /* WinUntrustedLabelSid          S-1-16-0 */
    { 66, 16, 1, { 4096, 0 } },        /* WinLowLabelSid                S-1-16-4096 */
    { 67, 16, 1, { 8192, 0 } },        /* WinMediumLabelSid             S-1-16-8192 */
    { 68, 16, 1, { 12288, 0 } },       /* WinHighLabelSid               S-1-16-12288 */
    { 69, 16, 1, { 16384, 0 } },       /* WinSystemLabelSid             S-1-16-16384 */
};

BOOL WINAPI CreateWellKnownSid(DWORD type, PVOID domainSid, PVOID sid, PDWORD cb)
{
    unsigned i;
    DWORD    need;
    BYTE    *p = (BYTE *)sid;

    if(cb == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    for(i = 0; i < sizeof(wkSids) / sizeof(wkSids[0]); i++) {
        if((DWORD)wkSids[i].type != type)
            continue;
        need = 8 + 4 * wkSids[i].count;
        if(*cb < need || p == NULL) {
            *cb = need;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        memset(p, 0, need);
        p[0] = 1;                                 /* revision */
        p[1] = wkSids[i].count;
        p[7] = wkSids[i].authority;               /* big endian identifier authority */
        memcpy(p + 8, wkSids[i].sub, 4 * wkSids[i].count);
        *cb = need;
        return TRUE;
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
}

/* CredDeleteA, CredReadA and CredWriteA are in cred.c (exported through advapi32.def) */

DWORD WINAPI GetNamedSecurityInfoW(LPCWSTR name, DWORD objType, DWORD info, PVOID *owner, PVOID *group,
                                   PVOID *dacl, PVOID *sacl, PVOID *desc)
{
    return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD WINAPI GetSecurityInfo(HANDLE h, DWORD objType, DWORD info, PVOID *owner, PVOID *group,
                             PVOID *dacl, PVOID *sacl, PVOID *desc)
{
    return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD WINAPI SetNamedSecurityInfoW(LPWSTR name, DWORD objType, DWORD info, PVOID owner, PVOID group,
                                   PVOID dacl, PVOID sacl)
{
    return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD WINAPI SetSecurityInfo(HANDLE h, DWORD objType, DWORD info, PVOID owner, PVOID group,
                             PVOID dacl, PVOID sacl)
{
    return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD WINAPI SetEntriesInAclW(ULONG count, PVOID entries, PVOID oldAcl, PVOID *newAcl)
{
    if(newAcl)
        *newAcl = NULL;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI RegDisablePredefinedCache(void)
{
    return ERROR_SUCCESS;
}

/* event tracing: do nothing, successfully */
DWORD WINAPI RegisterTraceGuidsW(PVOID cb, PVOID ctx, PVOID ctrlGuid, ULONG count, PVOID guids,
                                 LPCWSTR mof, LPCWSTR mofRes, ULONGLONG *handle)
{
    if(handle)
        *handle = 0;
    return ERROR_SUCCESS;
}

DWORD WINAPI UnregisterTraceGuids(ULONGLONG handle)
{
    return ERROR_SUCCESS;
}

DWORD WINAPI TraceEvent(ULONGLONG handle, PVOID header)
{
    return ERROR_SUCCESS;
}

ULONGLONG WINAPI GetTraceLoggerHandle(PVOID buffer)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return (ULONGLONG)0xFFFFFFFF;
}

} // extern "C"
