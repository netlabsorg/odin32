/*
 * dlls/advapi32/security.c
 *  FIXME: for all functions thunking down to Rtl* functions:  implement SetLastError()
 */
#include <string.h>

#include "windef.h"
#include "winerror.h"
#include "heap.h"
#include "ntddk.h"
#include "ntsecapi.h"
#include "debugtools.h"

#ifdef __WIN32OS2__
#include <heapstring.h>
#endif

DEFAULT_DEBUG_CHANNEL(advapi);

#define CallWin32ToNt(func) \
	{ NTSTATUS ret; \
	  ret = (func); \
	  if (ret !=STATUS_SUCCESS) \
	  { SetLastError (RtlNtStatusToDosError(ret)); return FALSE; } \
	  return TRUE; \
	}

static void dumpLsaAttributes( PLSA_OBJECT_ATTRIBUTES oa )
{
	if (oa)
	{
	  TRACE("\n\tlength=%lu, rootdir=0x%08x, objectname=%s\n\tattr=0x%08lx, sid=%p qos=%p\n",
		oa->Length, oa->RootDirectory,
		oa->ObjectName?debugstr_w(oa->ObjectName->Buffer):"null",
     		oa->Attributes, oa->SecurityDescriptor, oa->SecurityQualityOfService);
	}
}

/*	##############################
	######	TOKEN FUNCTIONS ######
	##############################
*/

/******************************************************************************
 * OpenProcessToken			[ADVAPI32.@]
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
	CallWin32ToNt(NtOpenProcessToken( ProcessHandle, DesiredAccess, TokenHandle ));
}

/******************************************************************************
 * OpenThreadToken [ADVAPI32.@]
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
	CallWin32ToNt (NtOpenThreadToken(ThreadHandle, DesiredAccess, OpenAsSelf, TokenHandle));
}

/******************************************************************************
 * AdjustTokenPrivileges [ADVAPI32.@]
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
                       LPVOID NewState, DWORD BufferLength, 
                       LPVOID PreviousState, LPDWORD ReturnLength )
{
	CallWin32ToNt(NtAdjustPrivilegesToken(TokenHandle, DisableAllPrivileges, NewState, BufferLength, PreviousState, ReturnLength));
}

/******************************************************************************
 * CheckTokenMembership [ADVAPI32.@]
 *
 * PARAMS
 *   TokenHandle []
 *   SidToCheck  []
 *   IsMember    []
 */
BOOL WINAPI
CheckTokenMembership( HANDLE TokenHandle, PSID SidToCheck,
                      PBOOL IsMember )
{
  FIXME("(0x%08x %p %p) stub!\n", TokenHandle, SidToCheck, IsMember);

  *IsMember = TRUE;
  return(TRUE);
}

/******************************************************************************
 * GetTokenInformation [ADVAPI32.@]
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

/*************************************************************************
 * SetThreadToken [ADVAPI32.@]
 *
 * Assigns an "impersonation token" to a thread so it can assume the
 * security privledges of another thread or process.  Can also remove
 * a previously assigned token.  Only supported on NT - it's a stub 
 * exactly like this one on Win9X.
 *
 */

BOOL WINAPI SetThreadToken(PHANDLE thread, HANDLE token)
{
#ifdef __WIN32OS2__

    dprintf(("SetThreadToken %x %x NOT IMPLEMENTED (FAKED)", thread, token));
    return TRUE; //pretend it succeeded
#else
    FIXME("(%p, %x): stub (NT impl. only)\n", thread, token);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
#endif
}

/*	##############################
	######	SID FUNCTIONS	######
	##############################
*/

/******************************************************************************
 * AllocateAndInitializeSid [ADVAPI32.@]
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
	return RtlAllocateAndInitializeSid(
		pIdentifierAuthority, nSubAuthorityCount, 
		nSubAuthority0, nSubAuthority1,	nSubAuthority2, nSubAuthority3,
		nSubAuthority4, nSubAuthority5, nSubAuthority6, nSubAuthority7,
		pSid );
}

/******************************************************************************
 * FreeSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
PVOID WINAPI
FreeSid( PSID pSid )
{
    	RtlFreeSid(pSid); 
	return NULL; /* is documented like this */
}

/******************************************************************************
 * CopySid [ADVAPI32.@]
 *
 * PARAMS
 *   nDestinationSidLength []
 *   pDestinationSid       []
 *   pSourceSid            []
 */
BOOL WINAPI
CopySid( DWORD nDestinationSidLength, PSID pDestinationSid, PSID pSourceSid )
{
	return RtlCopySid(nDestinationSidLength, pDestinationSid, pSourceSid);
}

/******************************************************************************
 * IsValidSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
BOOL WINAPI
IsValidSid( PSID pSid )
{
	return RtlValidSid( pSid );
}

/******************************************************************************
 * EqualSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid1 []
 *   pSid2 []
 */
BOOL WINAPI
EqualSid( PSID pSid1, PSID pSid2 )
{
	return RtlEqualSid( pSid1, pSid2 );
}

/******************************************************************************
 * EqualPrefixSid [ADVAPI32.@]
 */
BOOL WINAPI EqualPrefixSid (PSID pSid1, PSID pSid2) 
{
	return RtlEqualPrefixSid(pSid1, pSid2);
}

/******************************************************************************
 * GetSidLengthRequired [ADVAPI32.@]
 *
 * PARAMS
 *   nSubAuthorityCount []
 */
DWORD WINAPI
GetSidLengthRequired( BYTE nSubAuthorityCount )
{
	return RtlLengthRequiredSid(nSubAuthorityCount);
}

/******************************************************************************
 * InitializeSid [ADVAPI32.@]
 *
 * PARAMS
 *   pIdentifierAuthority []
 */
BOOL WINAPI
InitializeSid (
	PSID pSid,
	PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
	BYTE nSubAuthorityCount)
{
	return RtlInitializeSid(pSid, pIdentifierAuthority, nSubAuthorityCount);
}

/******************************************************************************
 * GetSidIdentifierAuthority [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
PSID_IDENTIFIER_AUTHORITY WINAPI
GetSidIdentifierAuthority( PSID pSid )
{
	return RtlIdentifierAuthoritySid(pSid);
}

/******************************************************************************
 * GetSidSubAuthority [ADVAPI32.@]
 *
 * PARAMS
 *   pSid          []
 *   nSubAuthority []
 */
PDWORD WINAPI
GetSidSubAuthority( PSID pSid, DWORD nSubAuthority )
{
	return RtlSubAuthoritySid(pSid, nSubAuthority);
}

/******************************************************************************
 * GetSidSubAuthorityCount [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
PUCHAR WINAPI
GetSidSubAuthorityCount (PSID pSid)
{
	return RtlSubAuthorityCountSid(pSid);
}

/******************************************************************************
 * GetLengthSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
DWORD WINAPI
GetLengthSid (PSID pSid)
{
        if(!IsValidSid(pSid)) return 0;

	return RtlLengthSid(pSid);
}

/*	##############################################
	######	SECURITY DESCRIPTOR FUNCTIONS	######
	##############################################
*/
	
/******************************************************************************
 * InitializeSecurityDescriptor [ADVAPI32.@]
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
 * GetSecurityDescriptorLength [ADVAPI32.@]
 */
DWORD WINAPI GetSecurityDescriptorLength( SECURITY_DESCRIPTOR *pDescr)
{
	return (RtlLengthSecurityDescriptor(pDescr));
}

/******************************************************************************
 * GetSecurityDescriptorOwner [ADVAPI32.@]
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
 * SetSecurityDescriptorOwner [ADVAPI32.@]
 *
 * PARAMS
 */
BOOL WINAPI SetSecurityDescriptorOwner( PSECURITY_DESCRIPTOR pSecurityDescriptor, 
				   PSID pOwner, BOOL bOwnerDefaulted)
{
	CallWin32ToNt (RtlSetOwnerSecurityDescriptor(pSecurityDescriptor, pOwner, bOwnerDefaulted));
}
/******************************************************************************
 * GetSecurityDescriptorGroup			[ADVAPI32.@]
 */
BOOL WINAPI GetSecurityDescriptorGroup(
	PSECURITY_DESCRIPTOR SecurityDescriptor,
	PSID *Group,
	LPBOOL GroupDefaulted)
{
	CallWin32ToNt (RtlGetGroupSecurityDescriptor(SecurityDescriptor, Group, (PBOOLEAN)GroupDefaulted));
}	
/******************************************************************************
 * SetSecurityDescriptorGroup [ADVAPI32.@]
 */
BOOL WINAPI SetSecurityDescriptorGroup ( PSECURITY_DESCRIPTOR SecurityDescriptor,
					   PSID Group, BOOL GroupDefaulted)
{
	CallWin32ToNt (RtlSetGroupSecurityDescriptor( SecurityDescriptor, Group, GroupDefaulted));
}

/******************************************************************************
 * IsValidSecurityDescriptor [ADVAPI32.@]
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
 *  GetSecurityDescriptorDacl			[ADVAPI32.@]
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
 *  SetSecurityDescriptorDacl			[ADVAPI32.@]
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
 *  GetSecurityDescriptorSacl			[ADVAPI32.@]
 */
BOOL WINAPI GetSecurityDescriptorSacl(
	IN PSECURITY_DESCRIPTOR lpsd,
	OUT LPBOOL lpbSaclPresent,
	OUT PACL *pSacl,
	OUT LPBOOL lpbSaclDefaulted)
{
	CallWin32ToNt (RtlGetSaclSecurityDescriptor(lpsd,
	   (PBOOLEAN)lpbSaclPresent, pSacl, (PBOOLEAN)lpbSaclDefaulted));
}	

/**************************************************************************
 * SetSecurityDescriptorSacl			[ADVAPI32.@]
 */
BOOL WINAPI SetSecurityDescriptorSacl (
	PSECURITY_DESCRIPTOR lpsd,
	BOOL saclpresent,
	PACL lpsacl,
	BOOL sacldefaulted)
{
	CallWin32ToNt (RtlSetSaclSecurityDescriptor(lpsd, saclpresent, lpsacl, sacldefaulted));
}
/******************************************************************************
 * MakeSelfRelativeSD [ADVAPI32.@]
 *
 * PARAMS
 *   lpabssecdesc  []
 *   lpselfsecdesc []
 *   lpbuflen      []
 */
BOOL WINAPI
MakeSelfRelativeSD(
	IN PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor,
	IN PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
	IN OUT LPDWORD lpdwBufferLength)
{
	CallWin32ToNt (RtlMakeSelfRelativeSD(pAbsoluteSecurityDescriptor,pSelfRelativeSecurityDescriptor, lpdwBufferLength));
}

/******************************************************************************
 * GetSecurityDescriptorControl			[ADVAPI32.@]
 */

BOOL WINAPI GetSecurityDescriptorControl ( PSECURITY_DESCRIPTOR  pSecurityDescriptor,
		 PSECURITY_DESCRIPTOR_CONTROL pControl, LPDWORD lpdwRevision)
{
	CallWin32ToNt (RtlGetControlSecurityDescriptor(pSecurityDescriptor,pControl,lpdwRevision));
}		

/*	##############################
	######	ACL FUNCTIONS	######
	##############################
*/

/*************************************************************************
 * InitializeAcl [ADVAPI32.@]
 */
DWORD WINAPI InitializeAcl(PACL acl, DWORD size, DWORD rev)
{
	CallWin32ToNt (RtlCreateAcl(acl, size, rev));
}

/*	##############################
	######	MISC FUNCTIONS	######
	##############################
*/

/******************************************************************************
 * LookupPrivilegeValueW			[ADVAPI32.@]
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
    FIXME("(%s,%s,%p): stub\n",debugstr_w(lpSystemName), 
        debugstr_w(lpName), lpLuid);
    return TRUE;
}

/******************************************************************************
 * LookupPrivilegeValueA			[ADVAPI32.@]
 */
BOOL WINAPI
LookupPrivilegeValueA( LPCSTR lpSystemName, LPCSTR lpName, LPVOID lpLuid )
{
    LPWSTR lpSystemNameW = HEAP_strdupAtoW(GetProcessHeap(), 0, lpSystemName);
    LPWSTR lpNameW = HEAP_strdupAtoW(GetProcessHeap(), 0, lpName);
    BOOL ret;

    ret = LookupPrivilegeValueW( lpSystemNameW, lpNameW, lpLuid);
    HeapFree(GetProcessHeap(), 0, lpNameW);
    HeapFree(GetProcessHeap(), 0, lpSystemNameW);
    return ret;
}

/******************************************************************************
 * GetFileSecurityA [ADVAPI32.@]
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
  FIXME("(%s) : stub\n", debugstr_a(lpFileName));
  return TRUE;
}

/******************************************************************************
 * GetFileSecurityW [ADVAPI32.@]
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
  FIXME("(%s) : stub\n", debugstr_w(lpFileName) ); 
  return TRUE;
}


/******************************************************************************
 * LookupAccountSidA [ADVAPI32.@]
 */
BOOL WINAPI
LookupAccountSidA(
	IN LPCSTR system,
	IN PSID sid,
	OUT LPSTR account,
	IN OUT LPDWORD accountSize,
	OUT LPSTR domain,
	IN OUT LPDWORD domainSize,
	OUT PSID_NAME_USE name_use )
{
	static const char ac[] = "Administrator";
	static const char dm[] = "DOMAIN";
	FIXME("(%s,sid=%p,%p,%p(%lu),%p,%p(%lu),%p): semi-stub\n",
	      debugstr_a(system),sid,
	      account,accountSize,accountSize?*accountSize:0,
	      domain,domainSize,domainSize?*domainSize:0,
	      name_use);

	if (accountSize) *accountSize = strlen(ac)+1;
	if (account && (*accountSize > strlen(ac)))
	  strcpy(account, ac);

	if (domainSize) *domainSize = strlen(dm)+1;
	if (domain && (*domainSize > strlen(dm)))
	  strcpy(domain,dm);

	if (name_use) *name_use = SidTypeUser;
	return TRUE;
}

/******************************************************************************
 * LookupAccountSidW [ADVAPI32.@]
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
LookupAccountSidW(
	IN LPCWSTR system,
	IN PSID sid,
	OUT LPWSTR account,
	IN OUT LPDWORD accountSize,
	OUT LPWSTR domain,
	IN OUT LPDWORD domainSize,
	OUT PSID_NAME_USE name_use )
{
    static const WCHAR ac[] = {'A','d','m','i','n','i','s','t','r','a','t','o','r',0};
    static const WCHAR dm[] = {'D','O','M','A','I','N',0};
	FIXME("(%s,sid=%p,%p,%p(%lu),%p,%p(%lu),%p): semi-stub\n",
	      debugstr_w(system),sid,
	      account,accountSize,accountSize?*accountSize:0,
	      domain,domainSize,domainSize?*domainSize:0,
	      name_use);

	if (accountSize) *accountSize = strlenW(ac)+1;
	if (account && (*accountSize > strlenW(ac)))
            strcpyW(account, ac);

	if (domainSize) *domainSize = strlenW(dm)+1;
	if (domain && (*domainSize > strlenW(dm)))
            strcpyW(domain,dm);

	if (name_use) *name_use = SidTypeUser;
	return TRUE;
}

/******************************************************************************
 * SetFileSecurityA [ADVAPI32.@]
 * Sets the security of a file or directory
 */
BOOL WINAPI SetFileSecurityA( LPCSTR lpFileName,
                                SECURITY_INFORMATION RequestedInformation,
                                PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
  FIXME("(%s) : stub\n", debugstr_a(lpFileName));
  return TRUE;
}

/******************************************************************************
 * SetFileSecurityW [ADVAPI32.@]
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
  FIXME("(%s) : stub\n", debugstr_w(lpFileName) ); 
  return TRUE;
}

/******************************************************************************
 * QueryWindows31FilesMigration [ADVAPI32.@]
 *
 * PARAMS
 *   x1 []
 */
BOOL WINAPI
QueryWindows31FilesMigration( DWORD x1 )
{
	FIXME("(%ld):stub\n",x1);
	return TRUE;
}

/******************************************************************************
 * SynchronizeWindows31FilesAndWindowsNTRegistry [ADVAPI32.@]
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
	FIXME("(0x%08lx,0x%08lx,0x%08lx,0x%08lx):stub\n",x1,x2,x3,x4);
	return TRUE;
}

/******************************************************************************
 * LsaOpenPolicy [ADVAPI32.@]
 *
 * PARAMS
 *   x1 []
 *   x2 []
 *   x3 []
 *   x4 []
 */
NTSTATUS WINAPI
LsaOpenPolicy(
	IN PLSA_UNICODE_STRING SystemName,
	IN PLSA_OBJECT_ATTRIBUTES ObjectAttributes,
	IN ACCESS_MASK DesiredAccess,
	IN OUT PLSA_HANDLE PolicyHandle)
{
	FIXME("(%s,%p,0x%08lx,%p):stub\n",
              SystemName?debugstr_w(SystemName->Buffer):"null",
	      ObjectAttributes, DesiredAccess, PolicyHandle);
	dumpLsaAttributes(ObjectAttributes);
	if(PolicyHandle) *PolicyHandle = (LSA_HANDLE)0xcafe;
	return TRUE;
}

/******************************************************************************
 * LsaQueryInformationPolicy [ADVAPI32.@]
 */
NTSTATUS WINAPI
LsaQueryInformationPolicy( 
	IN LSA_HANDLE PolicyHandle,
        IN POLICY_INFORMATION_CLASS InformationClass,
	OUT PVOID *Buffer)
{
	FIXME("(%p,0x%08x,%p):stub\n",
              PolicyHandle, InformationClass, Buffer);

	if(!Buffer) return FALSE;
	switch (InformationClass)
	{
	  case PolicyAuditEventsInformation: /* 2 */
	    {
	      PPOLICY_AUDIT_EVENTS_INFO p = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(POLICY_AUDIT_EVENTS_INFO));
	      p->AuditingMode = FALSE; /* no auditing */
	      *Buffer = p;
	    }
	    break;
	  case PolicyPrimaryDomainInformation: /* 3 */
	  case PolicyAccountDomainInformation: /* 5 */
	    {
	      struct di
	      { POLICY_PRIMARY_DOMAIN_INFO ppdi;
		SID sid;
	      };
	      SID_IDENTIFIER_AUTHORITY localSidAuthority = {SECURITY_NT_AUTHORITY};
		
	      struct di * xdi = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(xdi));
	      RtlInitUnicodeString(&(xdi->ppdi.Name), HEAP_strdupAtoW(GetProcessHeap(),0,"DOMAIN"));
	      xdi->ppdi.Sid = &(xdi->sid);
	      xdi->sid.Revision = SID_REVISION;
	      xdi->sid.SubAuthorityCount = 1;
	      xdi->sid.IdentifierAuthority = localSidAuthority;
	      xdi->sid.SubAuthority[0] = SECURITY_LOCAL_SYSTEM_RID;
	      *Buffer = xdi;
	    }
	    break;
	  case 	PolicyAuditLogInformation:	
	  case 	PolicyPdAccountInformation:	
	  case 	PolicyLsaServerRoleInformation:
	  case 	PolicyReplicaSourceInformation:
	  case 	PolicyDefaultQuotaInformation:
	  case 	PolicyModificationInformation:
	  case 	PolicyAuditFullSetInformation:
	  case 	PolicyAuditFullQueryInformation:
	  case 	PolicyDnsDomainInformation:
	    {
	      FIXME("category not implemented\n");
	      return FALSE;
	    }
	}
	return TRUE; 
}			  

/******************************************************************************
 * LsaLookupSids [ADVAPI32.@]
 */
typedef struct
{
	SID_NAME_USE Use;
	LSA_UNICODE_STRING Name;
	LONG DomainIndex;
} LSA_TRANSLATED_NAME, *PLSA_TRANSLATED_NAME;

typedef struct 
{
	LSA_UNICODE_STRING Name;
	PSID Sid;
} LSA_TRUST_INFORMATION, *PLSA_TRUST_INFORMATION;

typedef struct 
{
	ULONG Entries;
	PLSA_TRUST_INFORMATION Domains;
} LSA_REFERENCED_DOMAIN_LIST, *PLSA_REFERENCED_DOMAIN_LIST;

NTSTATUS WINAPI
LsaLookupSids(
	IN LSA_HANDLE PolicyHandle,
	IN ULONG Count,
	IN PSID *Sids,
	OUT PLSA_REFERENCED_DOMAIN_LIST *ReferencedDomains,
	OUT PLSA_TRANSLATED_NAME *Names )
{
	FIXME("%p %lu %p %p %p\n",
	  PolicyHandle, Count, Sids, ReferencedDomains, Names);
	return FALSE;
}

/******************************************************************************
 * LsaFreeMemory [ADVAPI32.@]
 */
NTSTATUS WINAPI
LsaFreeMemory(IN PVOID Buffer)
{
	TRACE("(%p)\n",Buffer);
	return HeapFree(GetProcessHeap(), 0, Buffer);
}
/******************************************************************************
 * LsaClose [ADVAPI32.@]
 */
NTSTATUS WINAPI
LsaClose(IN LSA_HANDLE ObjectHandle)
{
	FIXME("(%p):stub\n",ObjectHandle);
	return 0xc0000000;
}
/******************************************************************************
 * NotifyBootConfigStatus [ADVAPI32.@]
 *
 * PARAMS
 *   x1 []
 */
BOOL WINAPI
NotifyBootConfigStatus( DWORD x1 )
{
	FIXME("(0x%08lx):stub\n",x1);
	return 1;
}

/******************************************************************************
 * RevertToSelf [ADVAPI32.@]
 *
 * PARAMS
 *   void []
 */
BOOL WINAPI
RevertToSelf( void )
{
	FIXME("(), stub\n");
	return TRUE;
}

/******************************************************************************
 * ImpersonateSelf [ADVAPI32.@]
 */
BOOL WINAPI
ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL ImpersonationLevel)
{
	return RtlImpersonateSelf(ImpersonationLevel);
}

/******************************************************************************
 * AccessCheck [ADVAPI32.@]
 *
 * FIXME check cast LPBOOL to PBOOLEAN
 */
BOOL WINAPI
AccessCheck(
	PSECURITY_DESCRIPTOR SecurityDescriptor,
	HANDLE ClientToken,
	DWORD DesiredAccess,
	PGENERIC_MAPPING GenericMapping,
	PPRIVILEGE_SET PrivilegeSet,
	LPDWORD PrivilegeSetLength,
	LPDWORD GrantedAccess,
	LPBOOL AccessStatus)
{
	CallWin32ToNt (NtAccessCheck(SecurityDescriptor, ClientToken, DesiredAccess,
	  GenericMapping, PrivilegeSet, PrivilegeSetLength, GrantedAccess, (PBOOLEAN)AccessStatus));
}

/*************************************************************************
 * SetKernelObjectSecurity [ADVAPI32.@]
 */
BOOL WINAPI SetKernelObjectSecurity (
	IN HANDLE Handle,
	IN SECURITY_INFORMATION SecurityInformation,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor )
{
	CallWin32ToNt (NtSetSecurityObject (Handle, SecurityInformation, SecurityDescriptor));
}

/******************************************************************************
 *  AddAccessAllowedAce [ADVAPI32.@]
 */
BOOL WINAPI AddAccessAllowedAce(
        IN OUT PACL pAcl,
        IN DWORD dwAceRevision,
        IN DWORD AccessMask,
        IN PSID pSid)
{
        return RtlAddAccessAllowedAce(pAcl, dwAceRevision, AccessMask, pSid);
}

/******************************************************************************
 * LookupAccountNameA [ADVAPI32.@]
 */
BOOL WINAPI
LookupAccountNameA(
	IN LPCSTR system,
	IN LPCSTR account,
	OUT PSID sid,
	OUT LPDWORD cbSid,
	LPSTR ReferencedDomainName,
	IN OUT LPDWORD cbReferencedDomainName,
	OUT PSID_NAME_USE name_use )
{
    FIXME("(%s,%s,%p,%p,%p,%p,%p), stub.\n",system,account,sid,cbSid,ReferencedDomainName,cbReferencedDomainName,name_use);
    return FALSE;
}

/******************************************************************************
 * GetAce [ADVAPI32.@]
 */
BOOL WINAPI GetAce(PACL pAcl,DWORD dwAceIndex,LPVOID *pAce )
{
    CallWin32ToNt(RtlGetAce(pAcl, dwAceIndex, pAce));
}
