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

BOOL WINAPI CreateWellKnownSid(DWORD type, PVOID domainSid, PVOID sid, PDWORD cb)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
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
