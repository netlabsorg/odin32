/* $Id: security.cpp,v 1.4 2000-01-06 20:23:47 sandervl Exp $ */
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

DECLARE_DEBUG_CHANNEL(advapi-security)

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
    FIXME(__FUNCTION__"(%p, %x): stub\n", thread, token);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
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
	CallWin32ToNt (RtlCopySid( nDestinationSidLength, pDestinationSid, pSourceSid));
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
	CallWin32ToNt (RtlValidSid( pSid));
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
	CallWin32ToNt (RtlEqualSid( pSid1, pSid2));
}

/******************************************************************************
 * EqualPrefixSid [ADVAPI32.39]
 */
BOOL WINAPI EqualPrefixSid (PSID pSid1, PSID pSid2) 
{
	CallWin32ToNt (RtlEqualPrefixSid( pSid1, pSid2));
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
	CallWin32ToNt (RtlInitializeSid( pSid, pIdentifierAuthority, nSubAuthorityCount));
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
	FIXME(__FUNCTION__"(%p,%p,%p),stub!\n",lpabssecdesc,lpselfsecdesc,lpbuflen);
	return TRUE;
}

/******************************************************************************
 * GetSecurityDescriptorControl32			[ADVAPI32]
 */

BOOL WINAPI GetSecurityDescriptorControl ( PSECURITY_DESCRIPTOR  pSecurityDescriptor,
		 PSECURITY_DESCRIPTOR_CONTROL pControl, LPDWORD lpdwRevision)
{	FIXME(__FUNCTION__"(%p,%p,%p),stub!\n",pSecurityDescriptor,pControl,lpdwRevision);
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
    FIXME(__FUNCTION__"(%s,%s,%p): stub\n",debugstr_w(lpSystemName), 
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
  FIXME(__FUNCTION__"(%s) : stub\n", debugstr_a(lpFileName));
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
  FIXME(__FUNCTION__"(%s) : stub\n", debugstr_w(lpFileName) ); 
  return TRUE;
}

/******************************************************************************
 * SetFileSecurity32A [ADVAPI32.182]
 * Sets the security of a file or directory
 */
BOOL WINAPI SetFileSecurityA( LPCSTR lpFileName,
                                SECURITY_INFORMATION RequestedInformation,
                                PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
  FIXME(__FUNCTION__"(%s) : stub\n", debugstr_a(lpFileName));
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
  FIXME(__FUNCTION__"(%s) : stub\n", debugstr_w(lpFileName) ); 
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
	FIXME(__FUNCTION__"(%s,%p,%p,%p,%p,%p,%p): stub\n",
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
	FIXME(__FUNCTION__"(%p,%p,%p,%p,%p,%p,%p): stub\n",
	      system,sid,account,accountSize,domain,domainSize,name_use);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
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
	FIXME(__FUNCTION__"(%ld):stub\n",x1);
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
	FIXME(__FUNCTION__"(0x%08lx,0x%08lx,0x%08lx,0x%08lx):stub\n",x1,x2,x3,x4);
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
	FIXME(__FUNCTION__"(%p,%p,0x%08lx,%p):stub\n",
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
	FIXME(__FUNCTION__"(0x%08lx):stub\n",x1);
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
	FIXME(__FUNCTION__"(), stub\n");
	return TRUE;
}

/******************************************************************************
 * ImpersonateSelf [ADVAPI32.71]
 */
BOOL WINAPI
ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL ImpersonationLevel)
{
    FIXME(__FUNCTION__"(%08x), stub\n", ImpersonationLevel);
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
    FIXME(__FUNCTION__"(%p, %04x, %08lx, %p, %p, %p, %p, %p), stub\n",
		   pSecurityDescriptor, ClientToken, DesiredAccess, GenericMapping, 
		   PrivilegeSet, PrivilegeSetLength, GrantedAccess, AccessStatus);
    *AccessStatus = TRUE;
    return TRUE;
}

/*****************************************************************************
 * Name      : InitializeAcl
 * Purpose   : The InitializeAcl function creates a new ACL structure.
 *             An ACL is an access-control list.
 * Parameters: PACL  pAcl          address of access-control list
 *             DWORD nAclLength    size of access-control list
 *             DWORD dwAclRevision revision level of access-control list
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API InitializeAcl(PACL  pAcl,
                            DWORD nAclLength,
                            DWORD dwAclRevision)
{
  dprintf(("ADVAPI32: InitializeAcl(%08xh,%08xh,%08xh)",
           pAcl,
           nAclLength,
           dwAclRevision));

  CallWin32ToNt (RtlCreateAcl(pAcl, nAclLength, dwAclRevision));
}
