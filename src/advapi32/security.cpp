/* $Id: security.cpp,v 1.1 1999-11-30 19:41:45 sandervl Exp $ */
/*
 * Win32 security API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (OS/2 Port)
 *
 * Based on Wine code (dlls\advapi32\security.c)
 *
 * Copyright original Wine author(s) (????)
 *
 * dlls/advapi32/security.c
 *  FIXME: for all functions thunking down to Rtl* functions:  implement SetLastError()
 */
#ifdef __WIN32OS2__
#include <os2win.h>
#include <heapstring.h>
#endif

#include <string.h>

#include "windef.h"
#include "winerror.h"
#include "heap.h"
#include "ntddk.h"
#include "ntsecapi.h"
#include "debugtools.h"

DECLARE_DEBUG_CHANNEL(advapi)
DECLARE_DEBUG_CHANNEL(security)

static BOOL fInitSecurity = FALSE;
static BOOL fHasSecurity = FALSE;

static BOOL Wine_HasSecurity(void)
{
    //SvL: Let's not check this for every single call please...
    if(!fInitSecurity) 
    {
    	OSVERSIONINFOA osi;
    	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA); 
    	GetVersionExA(&osi);
    	if (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) { 
		fHasSecurity = TRUE;
	}
        fInitSecurity = TRUE;
    }
    if(!fHasSecurity) {
       	SetLastError(ERROR_CALL_NOT_IMPLEMENTED); 
        return FALSE;
    }
    return TRUE;
}  


#define CallWin32ToNt(func) \
	{ NTSTATUS ret; \
	  ret = (func); \
	  if (ret !=STATUS_SUCCESS) \
	  { SetLastError (RtlNtStatusToDosError(ret)); return FALSE; } \
	  return TRUE; \
	}

/*	##############################
	######	TOKEN FUNCTIONS ######
	##############################
*/

/******************************************************************************
 * OpenProcessToken			[ADVAPI32.109]
 * Opens the access token associated with a process
 *
 * PARAMS
 *   ProcessHandle [I] Handle to process
 *   DesiredAccess [I] Desired access to process
 *   TokenHandle   [O] Pointer to handle of open access token
 *
 * RETURNS STD
 */
BOOL WINAPI
OpenProcessToken( HANDLE ProcessHandle, DWORD DesiredAccess, 
                  HANDLE *TokenHandle )
{
        if (!Wine_HasSecurity()) return FALSE;
	CallWin32ToNt(NtOpenProcessToken( ProcessHandle, DesiredAccess, TokenHandle ));
}

/******************************************************************************
 * OpenThreadToken [ADVAPI32.114]
 *
 * PARAMS
 *   thread        []
 *   desiredaccess []
 *   openasself    []
 *   thandle       []
 */
BOOL WINAPI
OpenThreadToken( HANDLE ThreadHandle, DWORD DesiredAccess, 
		 BOOL OpenAsSelf, HANDLE *TokenHandle)
{
        if (!Wine_HasSecurity()) return FALSE;
	CallWin32ToNt (NtOpenThreadToken(ThreadHandle, DesiredAccess, OpenAsSelf, TokenHandle));
}

/******************************************************************************
 * AdjustTokenPrivileges [ADVAPI32.10]
 *
 * PARAMS
 *   TokenHandle          []
 *   DisableAllPrivileges []
 *   NewState             []
 *   BufferLength         []
 *   PreviousState        []
 *   ReturnLength         []
 */
BOOL WINAPI
AdjustTokenPrivileges( HANDLE TokenHandle, BOOL DisableAllPrivileges,
                       PTOKEN_PRIVILEGES NewState, DWORD BufferLength, 
                       PTOKEN_PRIVILEGES PreviousState, LPDWORD ReturnLength )
{
	CallWin32ToNt(NtAdjustPrivilegesToken(TokenHandle, DisableAllPrivileges, NewState, BufferLength, PreviousState, ReturnLength));
}

/******************************************************************************
 * GetTokenInformation [ADVAPI32.66]
 *
 * PARAMS
 *   token           []
 *   tokeninfoclass  []
 *   tokeninfo       []
 *   tokeninfolength []
 *   retlen          []
 *
 */
BOOL WINAPI
GetTokenInformation( HANDLE token, TOKEN_INFORMATION_CLASS tokeninfoclass, 
		     LPVOID tokeninfo, DWORD tokeninfolength, LPDWORD retlen )
{
	CallWin32ToNt (NtQueryInformationToken( token, tokeninfoclass, tokeninfo, tokeninfolength, retlen));
}

/*	##############################
	######	SID FUNCTIONS	######
	##############################
*/

/******************************************************************************
 * AllocateAndInitializeSid [ADVAPI32.11]
 *
 * PARAMS
 *   pIdentifierAuthority []
 *   nSubAuthorityCount   []
 *   nSubAuthority0       []
 *   nSubAuthority1       []
 *   nSubAuthority2       []
 *   nSubAuthority3       []
 *   nSubAuthority4       []
 *   nSubAuthority5       []
 *   nSubAuthority6       []
 *   nSubAuthority7       []
 *   pSid                 []
 */
BOOL WINAPI
AllocateAndInitializeSid( PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
                          BYTE nSubAuthorityCount,
                          DWORD nSubAuthority0, DWORD nSubAuthority1,
                          DWORD nSubAuthority2, DWORD nSubAuthority3,
                          DWORD nSubAuthority4, DWORD nSubAuthority5,
                          DWORD nSubAuthority6, DWORD nSubAuthority7,
                          PSID *pSid )
{
    if (!(*pSid = (PSID)HeapAlloc(GetProcessHeap(), 0,
                                  GetSidLengthRequired(nSubAuthorityCount))))
        return FALSE;

    (*pSid)->Revision = SID_REVISION;
    if (pIdentifierAuthority)
        memcpy(&(*pSid)->IdentifierAuthority, pIdentifierAuthority,
               sizeof (SID_IDENTIFIER_AUTHORITY));
    *GetSidSubAuthorityCount(*pSid) = nSubAuthorityCount;

    if (nSubAuthorityCount > 0)
        *GetSidSubAuthority(*pSid, 0) = nSubAuthority0;
    if (nSubAuthorityCount > 1)
        *GetSidSubAuthority(*pSid, 1) = nSubAuthority1;
    if (nSubAuthorityCount > 2)
        *GetSidSubAuthority(*pSid, 2) = nSubAuthority2;
    if (nSubAuthorityCount > 3)
        *GetSidSubAuthority(*pSid, 3) = nSubAuthority3;
    if (nSubAuthorityCount > 4)
        *GetSidSubAuthority(*pSid, 4) = nSubAuthority4;
    if (nSubAuthorityCount > 5)
        *GetSidSubAuthority(*pSid, 5) = nSubAuthority5;
    if (nSubAuthorityCount > 6)
        *GetSidSubAuthority(*pSid, 6) = nSubAuthority6;
    if (nSubAuthorityCount > 7)
        *GetSidSubAuthority(*pSid, 7) = nSubAuthority7;

    return TRUE;
}

/******************************************************************************
 * FreeSid [ADVAPI32.42]
 *
 * PARAMS
 *   pSid []
 */
PVOID WINAPI
FreeSid( PSID pSid )
{
    HeapFree( GetProcessHeap(), 0, pSid );
    return NULL;
}

/******************************************************************************
 * CopySid [ADVAPI32.24]
 *
 * PARAMS
 *   nDestinationSidLength []
 *   pDestinationSid       []
 *   pSourceSid            []
 */
BOOL WINAPI
CopySid( DWORD nDestinationSidLength, PSID pDestinationSid, PSID pSourceSid )
{

    if (!IsValidSid(pSourceSid))
        return FALSE;

    if (nDestinationSidLength < GetLengthSid(pSourceSid))
        return FALSE;

    memcpy(pDestinationSid, pSourceSid, GetLengthSid(pSourceSid));

    return TRUE;
}

/******************************************************************************
 * IsValidSid [ADVAPI32.80]
 *
 * PARAMS
 *   pSid []
 */
BOOL WINAPI
IsValidSid( PSID pSid )
{
    if (IsBadReadPtr(pSid, 4))
    {
        WARN_(security)("(%p): invalid pointer!", pSid);
        return FALSE;
    }

    if (pSid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES)
        return FALSE;

    if (!pSid || pSid->Revision != SID_REVISION)
        return FALSE;

    return TRUE;
}

/******************************************************************************
 * EqualSid [ADVAPI32.40]
 *
 * PARAMS
 *   pSid1 []
 *   pSid2 []
 */
BOOL WINAPI
EqualSid( PSID pSid1, PSID pSid2 )
{
    if (!IsValidSid(pSid1) || !IsValidSid(pSid2))
        return FALSE;

    if (*GetSidSubAuthorityCount(pSid1) != *GetSidSubAuthorityCount(pSid2))
        return FALSE;

    if (memcmp(pSid1, pSid2, GetLengthSid(pSid1)) != 0)
        return FALSE;

    return TRUE;
}

/******************************************************************************
 * EqualPrefixSid [ADVAPI32.39]
 */
BOOL WINAPI EqualPrefixSid (PSID pSid1, PSID pSid2) {
    if (!IsValidSid(pSid1) || !IsValidSid(pSid2))
        return FALSE;

    if (*GetSidSubAuthorityCount(pSid1) != *GetSidSubAuthorityCount(pSid2))
        return FALSE;

    if (memcmp(pSid1, pSid2, GetSidLengthRequired(pSid1->SubAuthorityCount - 1))
 != 0)
        return FALSE;

    return TRUE;
}

/******************************************************************************
 * GetSidLengthRequired [ADVAPI32.63]
 *
 * PARAMS
 *   nSubAuthorityCount []
 */
DWORD WINAPI
GetSidLengthRequired( BYTE nSubAuthorityCount )
{
    return sizeof (SID) + (nSubAuthorityCount - 1) * sizeof (DWORD);
}

/******************************************************************************
 * InitializeSid [ADVAPI32.74]
 *
 * PARAMS
 *   pIdentifierAuthority []
 */
BOOL WINAPI
InitializeSid (PSID pSid, PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
                    BYTE nSubAuthorityCount)
{
    int i;

    pSid->Revision = SID_REVISION;
    if (pIdentifierAuthority)
        memcpy(&pSid->IdentifierAuthority, pIdentifierAuthority,
               sizeof (SID_IDENTIFIER_AUTHORITY));
    *GetSidSubAuthorityCount(pSid) = nSubAuthorityCount;

    for (i = 0; i < nSubAuthorityCount; i++)
        *GetSidSubAuthority(pSid, i) = 0;

    return TRUE;
}

/******************************************************************************
 * GetSidIdentifierAuthority [ADVAPI32.62]
 *
 * PARAMS
 *   pSid []
 */
PSID_IDENTIFIER_AUTHORITY WINAPI
GetSidIdentifierAuthority( PSID pSid )
{
    return &pSid->IdentifierAuthority;
}

/******************************************************************************
 * GetSidSubAuthority [ADVAPI32.64]
 *
 * PARAMS
 *   pSid          []
 *   nSubAuthority []
 */
PDWORD WINAPI
GetSidSubAuthority( PSID pSid, DWORD nSubAuthority )
{
    return &pSid->SubAuthority[nSubAuthority];
}

/******************************************************************************
 * GetSidSubAuthorityCount [ADVAPI32.65]
 *
 * PARAMS
 *   pSid []
 */
PUCHAR WINAPI
GetSidSubAuthorityCount (PSID pSid)
{
    return &pSid->SubAuthorityCount;
}

/******************************************************************************
 * GetLengthSid [ADVAPI32.48]
 *
 * PARAMS
 *   pSid []
 */
DWORD WINAPI
GetLengthSid (PSID pSid)
{
    return GetSidLengthRequired( * GetSidSubAuthorityCount(pSid) );
}

/*	##############################################
	######	SECURITY DESCRIPTOR FUNCTIONS	######
	##############################################
*/
	
/******************************************************************************
 * InitializeSecurityDescriptor [ADVAPI32.73]
 *
 * PARAMS
 *   pDescr   []
 *   revision []
 */
BOOL WINAPI
InitializeSecurityDescriptor( SECURITY_DESCRIPTOR *pDescr, DWORD revision )
{
	CallWin32ToNt (RtlCreateSecurityDescriptor(pDescr, revision ));
}

/******************************************************************************
 * GetSecurityDescriptorLength [ADVAPI32.55]
 */
DWORD WINAPI GetSecurityDescriptorLength( SECURITY_DESCRIPTOR *pDescr)
{
	return (RtlLengthSecurityDescriptor(pDescr));
}

/******************************************************************************
 * GetSecurityDescriptorOwner [ADVAPI32.56]
 *
 * PARAMS
 *   pOwner            []
 *   lpbOwnerDefaulted []
 */
BOOL WINAPI
GetSecurityDescriptorOwner( SECURITY_DESCRIPTOR *pDescr, PSID *pOwner,
			    LPBOOL lpbOwnerDefaulted )
{
	CallWin32ToNt (RtlGetOwnerSecurityDescriptor( pDescr, pOwner, (PBOOLEAN)lpbOwnerDefaulted ));
}

/******************************************************************************
 * SetSecurityDescriptorOwner [ADVAPI32]
 *
 * PARAMS
 */
BOOL WINAPI SetSecurityDescriptorOwner( PSECURITY_DESCRIPTOR pSecurityDescriptor, 
				   PSID pOwner, BOOL bOwnerDefaulted)
{
	CallWin32ToNt (RtlSetOwnerSecurityDescriptor(pSecurityDescriptor, pOwner, bOwnerDefaulted));
}
/******************************************************************************
 * GetSecurityDescriptorGroup			[ADVAPI32.54]
 */
BOOL WINAPI GetSecurityDescriptorGroup(
	PSECURITY_DESCRIPTOR SecurityDescriptor,
	PSID *Group,
	LPBOOL GroupDefaulted)
{
	CallWin32ToNt (RtlGetGroupSecurityDescriptor(SecurityDescriptor, Group, (PBOOLEAN)GroupDefaulted));
}	
/******************************************************************************
 * SetSecurityDescriptorGroup
 */
BOOL WINAPI SetSecurityDescriptorGroup ( PSECURITY_DESCRIPTOR SecurityDescriptor,
					   PSID Group, BOOL GroupDefaulted)
{
	CallWin32ToNt (RtlSetGroupSecurityDescriptor( SecurityDescriptor, Group, GroupDefaulted));
}

/******************************************************************************
 * IsValidSecurityDescriptor [ADVAPI32.79]
 *
 * PARAMS
 *   lpsecdesc []
 */
BOOL WINAPI
IsValidSecurityDescriptor( PSECURITY_DESCRIPTOR SecurityDescriptor )
{
	CallWin32ToNt (RtlValidSecurityDescriptor(SecurityDescriptor));
}

/******************************************************************************
 *  GetSecurityDescriptorDacl			[ADVAPI.91]
 */
BOOL WINAPI GetSecurityDescriptorDacl(
	IN PSECURITY_DESCRIPTOR pSecurityDescriptor,
	OUT LPBOOL lpbDaclPresent,
	OUT PACL *pDacl,
	OUT LPBOOL lpbDaclDefaulted)
{
	CallWin32ToNt (RtlGetDaclSecurityDescriptor(pSecurityDescriptor, (PBOOLEAN)lpbDaclPresent,
					       pDacl, (PBOOLEAN)lpbDaclDefaulted));
}	

/******************************************************************************
 *  SetSecurityDescriptorDacl			[ADVAPI.224]
 */
BOOL WINAPI 
SetSecurityDescriptorDacl (
	PSECURITY_DESCRIPTOR lpsd,
	BOOL daclpresent,
	PACL dacl,
	BOOL dacldefaulted )
{
	CallWin32ToNt (RtlSetDaclSecurityDescriptor (lpsd, daclpresent, dacl, dacldefaulted ));
}
/******************************************************************************
 *  GetSecurityDescriptorSacl			[ADVAPI.]
 */
BOOL WINAPI GetSecurityDescriptorSacl(
	IN PSECURITY_DESCRIPTOR lpsd,
	OUT LPBOOL lpbSaclPresent,
	OUT PACL *pSacl,
	OUT LPBOOL lpbSaclDefaulted)
{
	CallWin32ToNt (RtlGetSaclSecurityDescriptor(lpsd, (PBOOLEAN)lpbSaclPresent,
					       pSacl, (PBOOLEAN)lpbSaclDefaulted));
}	

/**************************************************************************
 * SetSecurityDescriptorSacl			[NTDLL.488]
 */
BOOL  WINAPI SetSecurityDescriptorSacl (
	PSECURITY_DESCRIPTOR lpsd,
	BOOL saclpresent,
	PACL lpsacl,
	BOOL sacldefaulted)
{
	CallWin32ToNt (RtlSetSaclSecurityDescriptor(lpsd, saclpresent, lpsacl, sacldefaulted));
}
/******************************************************************************
 * MakeSelfRelativeSD [ADVAPI32.95]
 *
 * PARAMS
 *   lpabssecdesc  []
 *   lpselfsecdesc []
 *   lpbuflen      []
 */
BOOL WINAPI
MakeSelfRelativeSD( PSECURITY_DESCRIPTOR lpabssecdesc,
                    PSECURITY_DESCRIPTOR lpselfsecdesc, LPDWORD lpbuflen )
{
	FIXME_(advapi)("(%p,%p,%p),stub!\n",lpabssecdesc,lpselfsecdesc,lpbuflen);
	return TRUE;
}

/******************************************************************************
 * GetSecurityDescriptorControl32			[ADVAPI32]
 */

BOOL WINAPI GetSecurityDescriptorControl ( PSECURITY_DESCRIPTOR  pSecurityDescriptor,
		 PSECURITY_DESCRIPTOR_CONTROL pControl, LPDWORD lpdwRevision)
{	FIXME_(advapi)("(%p,%p,%p),stub!\n",pSecurityDescriptor,pControl,lpdwRevision);
	return 1;
}		

/*	##############################
	######	MISC FUNCTIONS	######
	##############################
*/

/******************************************************************************
 * LookupPrivilegeValue32W			[ADVAPI32.93]
 * Retrieves LUID used on a system to represent the privilege name.
 *
 * NOTES
 *   lpLuid should be PLUID
 *
 * PARAMS
 *   lpSystemName [I] Address of string specifying the system
 *   lpName       [I] Address of string specifying the privilege
 *   lpLuid       [I] Address of locally unique identifier
 *
 * RETURNS STD
 */
BOOL WINAPI
LookupPrivilegeValueW( LPCWSTR lpSystemName, LPCWSTR lpName, LPVOID lpLuid )
{
    FIXME_(advapi)("(%s,%s,%p): stub\n",debugstr_w(lpSystemName), 
                  debugstr_w(lpName), lpLuid);
    return TRUE;
}

/******************************************************************************
 * LookupPrivilegeValue32A			[ADVAPI32.92]
 */
BOOL WINAPI
LookupPrivilegeValueA( LPCSTR lpSystemName, LPCSTR lpName, LPVOID lpLuid )
{
    LPWSTR lpSystemNameW = HEAP_strdupAtoW(GetProcessHeap(), 0, lpSystemName);
    LPWSTR lpNameW = HEAP_strdupAtoW(GetProcessHeap(), 0, lpName);
    BOOL ret = LookupPrivilegeValueW( lpSystemNameW, lpNameW, lpLuid);
    HeapFree(GetProcessHeap(), 0, lpNameW);
    HeapFree(GetProcessHeap(), 0, lpSystemNameW);
    return ret;
}

/******************************************************************************
 * GetFileSecurity32A [ADVAPI32.45]
 *
 * Obtains Specified information about the security of a file or directory
 * The information obtained is constrained by the callers access rights and
 * privileges
 */
BOOL WINAPI
GetFileSecurityA( LPCSTR lpFileName, 
                    SECURITY_INFORMATION RequestedInformation,
                    PSECURITY_DESCRIPTOR pSecurityDescriptor,
                    DWORD nLength, LPDWORD lpnLengthNeeded )
{
  FIXME_(advapi)("(%s) : stub\n", debugstr_a(lpFileName));
  return TRUE;
}

/******************************************************************************
 * GetFileSecurity32W [ADVAPI32.46]
 *
 * Obtains Specified information about the security of a file or directory
 * The information obtained is constrained by the callers access rights and
 * privileges
 *
 * PARAMS
 *   lpFileName           []
 *   RequestedInformation []
 *   pSecurityDescriptor  []
 *   nLength              []
 *   lpnLengthNeeded      []
 */
BOOL WINAPI
GetFileSecurityW( LPCWSTR lpFileName, 
                    SECURITY_INFORMATION RequestedInformation,
                    PSECURITY_DESCRIPTOR pSecurityDescriptor,
                    DWORD nLength, LPDWORD lpnLengthNeeded )
{
  FIXME_(advapi)("(%s) : stub\n", debugstr_w(lpFileName) ); 
  return TRUE;
}


/******************************************************************************
 * LookupAccountSid32A [ADVAPI32.86]
 */
BOOL WINAPI
LookupAccountSidA( LPCSTR system, PSID sid, LPCSTR account,
                     LPDWORD accountSize, LPCSTR domain, LPDWORD domainSize,
                     PSID_NAME_USE name_use )
{
	FIXME_(security)("(%s,%p,%p,%p,%p,%p,%p): stub\n",
	      system,sid,account,accountSize,domain,domainSize,name_use);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/******************************************************************************
 * LookupAccountSid32W [ADVAPI32.87]
 *
 * PARAMS
 *   system      []
 *   sid         []
 *   account     []
 *   accountSize []
 *   domain      []
 *   domainSize  []
 *   name_use    []
 */
BOOL WINAPI
LookupAccountSidW( LPCWSTR system, PSID sid, LPCWSTR account, 
                     LPDWORD accountSize, LPCWSTR domain, LPDWORD domainSize,
                     PSID_NAME_USE name_use )
{
	FIXME_(security)("(%p,%p,%p,%p,%p,%p,%p): stub\n",
	      system,sid,account,accountSize,domain,domainSize,name_use);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/******************************************************************************
 * SetFileSecurity32A [ADVAPI32.182]
 * Sets the security of a file or directory
 */
BOOL WINAPI SetFileSecurityA( LPCSTR lpFileName,
                                SECURITY_INFORMATION RequestedInformation,
                                PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
  FIXME_(advapi)("(%s) : stub\n", debugstr_a(lpFileName));
  return TRUE;
}

/******************************************************************************
 * SetFileSecurity32W [ADVAPI32.183]
 * Sets the security of a file or directory
 *
 * PARAMS
 *   lpFileName           []
 *   RequestedInformation []
 *   pSecurityDescriptor  []
 */
BOOL WINAPI
SetFileSecurityW( LPCWSTR lpFileName, 
                    SECURITY_INFORMATION RequestedInformation,
                    PSECURITY_DESCRIPTOR pSecurityDescriptor )
{
  FIXME_(advapi)("(%s) : stub\n", debugstr_w(lpFileName) ); 
  return TRUE;
}

/******************************************************************************
 * QueryWindows31FilesMigration [ADVAPI32.266]
 *
 * PARAMS
 *   x1 []
 */
BOOL WINAPI
QueryWindows31FilesMigration( DWORD x1 )
{
	FIXME_(advapi)("(%ld):stub\n",x1);
	return TRUE;
}

/******************************************************************************
 * SynchronizeWindows31FilesAndWindowsNTRegistry [ADVAPI32.265]
 *
 * PARAMS
 *   x1 []
 *   x2 []
 *   x3 []
 *   x4 []
 */
BOOL WINAPI
SynchronizeWindows31FilesAndWindowsNTRegistry( DWORD x1, DWORD x2, DWORD x3,
                                               DWORD x4 )
{
	FIXME_(advapi)("(0x%08lx,0x%08lx,0x%08lx,0x%08lx):stub\n",x1,x2,x3,x4);
	return TRUE;
}

/******************************************************************************
 * LsaOpenPolicy [ADVAPI32.200]
 *
 * PARAMS
 *   x1 []
 *   x2 []
 *   x3 []
 *   x4 []
 */
NTSTATUS WINAPI
LsaOpenPolicy(PLSA_UNICODE_STRING SystemName,
	      PLSA_OBJECT_ATTRIBUTES ObjectAttributes,
	      ACCESS_MASK DesiredAccess,
	      PLSA_HANDLE PolicyHandle)
{
	FIXME_(advapi)("(%p,%p,0x%08lx,%p):stub\n",
		       SystemName, ObjectAttributes,
		       DesiredAccess, PolicyHandle);
	return 0xc0000000; /* generic error */
}

/******************************************************************************
 * NotifyBootConfigStatus [ADVAPI32.97]
 *
 * PARAMS
 *   x1 []
 */
BOOL WINAPI
NotifyBootConfigStatus( DWORD x1 )
{
	FIXME_(advapi)("(0x%08lx):stub\n",x1);
	return 1;
}

/******************************************************************************
 * RevertToSelf [ADVAPI32.180]
 *
 * PARAMS
 *   void []
 */
BOOL WINAPI
RevertToSelf( void )
{
	FIXME_(advapi)("(), stub\n");
	return TRUE;
}

/******************************************************************************
 * ImpersonateSelf [ADVAPI32.71]
 */
BOOL WINAPI
ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL ImpersonationLevel)
{
    FIXME_(advapi)("(%08x), stub\n", ImpersonationLevel);
    return TRUE;
}

/******************************************************************************
 * AccessCheck32 [ADVAPI32.71]
 */
BOOL WINAPI
AccessCheck(PSECURITY_DESCRIPTOR pSecurityDescriptor, HANDLE ClientToken,
	    DWORD DesiredAccess, PGENERIC_MAPPING GenericMapping, PPRIVILEGE_SET PrivilegeSet,
	    LPDWORD PrivilegeSetLength, LPDWORD GrantedAccess, LPBOOL AccessStatus)
{
    FIXME_(advapi)("(%p, %04x, %08lx, %p, %p, %p, %p, %p), stub\n",
		   pSecurityDescriptor, ClientToken, DesiredAccess, GenericMapping, 
		   PrivilegeSet, PrivilegeSetLength, GrantedAccess, AccessStatus);
    *AccessStatus = TRUE;
    return TRUE;
}

/*************************************************************************
 * SetThreadToken [ADVAPI32.231]
 *
 * Assigns an "impersonation token" to a thread so it can assume the
 * security privledges of another thread or process.  Can also remove
 * a previously assigned token.  Only supported on NT - it's a stub 
 * exactly like this one on Win9X.
 *
 */

BOOL WINAPI SetThreadToken(PHANDLE thread, HANDLE token)
{
    FIXME_(advapi)("(%p, %x): stub\n", thread, token);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}




