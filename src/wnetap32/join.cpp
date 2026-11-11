/*
 * NETAPI32: domain / workgroup join information
 *
 * Modern programs (Chromium, Firefox) query the domain-join state at start-up through delay-loaded imports
 * (NetGetJoinInformation, NetGetAadJoinInformation, DsRoleGetPrimaryDomainInformation).  If an import is
 * missing, the delay-load helper treats it as fatal.  This machine is never in a domain: report that.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2win.h>
#include <misc.h>
#include <string.h>
#include <heapstring.h>

#include "lanman.h"

#define NetSetupWorkgroupName_  2

extern "C" {

//NET_API_STATUS NetGetJoinInformation(LPCWSTR server, LPWSTR *name, PNETSETUP_JOIN_STATUS status)
DWORD WIN32API NetGetJoinInformation(LPCWSTR lpServer, LPWSTR *lpNameBuffer, DWORD *BufferType)
{
    static const WCHAR wszWorkgroup[] = { 'W','O','R','K','G','R','O','U','P', 0 };
    LPWSTR p;

    if(!lpNameBuffer || !BufferType)
        return ERROR_INVALID_PARAMETER;

    p = (LPWSTR)HEAP_malloc(sizeof(wszWorkgroup));   //freed with NetApiBufferFree
    if(!p)
        return ERROR_NOT_ENOUGH_MEMORY;
    memcpy(p, wszWorkgroup, sizeof(wszWorkgroup));
    *lpNameBuffer = p;
    *BufferType   = NetSetupWorkgroupName_;
    return 0;
}

//HRESULT NetGetAadJoinInformation(LPCWSTR tenantId, PDSREG_JOIN_INFO *joinInfo): S_OK with no info = not joined
DWORD WIN32API NetGetAadJoinInformation(LPCWSTR lpTenantId, PVOID *ppJoinInfo)
{
    if(ppJoinInfo)
        *ppJoinInfo = NULL;
    return 0;
}

VOID WIN32API NetFreeAadJoinInformation(PVOID pJoinInfo)
{
}

//DWORD DsRoleGetPrimaryDomainInformation(LPCWSTR server, level, PBYTE *buffer)
DWORD WIN32API DsRoleGetPrimaryDomainInformation(LPCWSTR lpServer, DWORD InfoLevel, PBYTE *Buffer)
{
    if(Buffer)
        *Buffer = NULL;
    return ERROR_NOT_SUPPORTED;
}

VOID WIN32API DsRoleFreeMemory(PVOID Buffer)
{
    if(Buffer)
        HEAP_free(Buffer);
}

} // extern "C"
