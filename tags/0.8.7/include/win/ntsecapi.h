#ifndef __WINE_NTSECAPI_H
#define __WINE_NTSECAPI_H

#include <winnt.h>
#include <ntdef.h>

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;
#endif

/* Policy access rights */
#define POLICY_VIEW_LOCAL_INFORMATION           0x00000001L
#define POLICY_VIEW_AUDIT_INFORMATION           0x00000002L
#define POLICY_GET_PRIVATE_INFORMATION          0x00000004L
#define POLICY_TRUST_ADMIN                      0x00000008L
#define POLICY_CREATE_ACCOUNT                   0x00000010L
#define POLICY_CREATE_SECRET                    0x00000020L
#define POLICY_CREATE_PRIVILEGE                 0x00000040L
#define POLICY_SET_DEFAULT_QUOTA_LIMITS         0x00000080L
#define POLICY_SET_AUDIT_REQUIREMENTS           0x00000100L
#define POLICY_AUDIT_LOG_ADMIN                  0x00000200L
#define POLICY_SERVER_ADMIN                     0x00000400L
#define POLICY_LOOKUP_NAMES                     0x00000800L
#define POLICY_NOTIFICATION                     0x00001000L

#define POLICY_ALL_ACCESS                       ( \
    STANDARD_RIGHTS_REQUIRED | \
    POLICY_VIEW_LOCAL_INFORMATION | \
    POLICY_VIEW_AUDIT_INFORMATION | \
    POLICY_GET_PRIVATE_INFORMATION | \
    POLICY_TRUST_ADMIN | \
    POLICY_CREATE_ACCOUNT | \
    POLICY_CREATE_SECRET | \
    POLICY_CREATE_PRIVILEGE | \
    POLICY_SET_DEFAULT_QUOTA_LIMITS | \
    POLICY_SET_AUDIT_REQUIREMENTS | \
    POLICY_AUDIT_LOG_ADMIN | \
    POLICY_SERVER_ADMIN | \
    POLICY_LOOKUP_NAMES)


#define POLICY_READ                             ( \
    STANDARD_RIGHTS_READ | \
    POLICY_VIEW_AUDIT_INFORMATION | \
    POLICY_GET_PRIVATE_INFORMATION)

#define POLICY_WRITE                            ( \
   STANDARD_RIGHTS_WRITE | \
   POLICY_TRUST_ADMIN | \
   POLICY_CREATE_ACCOUNT | \
   POLICY_CREATE_SECRET | \
   POLICY_CREATE_PRIVILEGE | \
   POLICY_SET_DEFAULT_QUOTA_LIMITS | \
   POLICY_SET_AUDIT_REQUIREMENTS | \
   POLICY_AUDIT_LOG_ADMIN | \
   POLICY_SERVER_ADMIN)

#define POLICY_EXECUTE                          ( \
   STANDARD_RIGHTS_EXECUTE | \
   POLICY_VIEW_LOCAL_INFORMATION | \
   POLICY_LOOKUP_NAMES)

#define POLICY_AUDIT_EVENT_UNCHANGED 0x00000000L
#define POLICY_AUDIT_EVENT_SUCCESS   0x00000001L
#define POLICY_AUDIT_EVENT_FAILURE   0x00000002L
#define POLICY_AUDIT_EVENT_NONE      0x00000004L

#define POLICY_AUDIT_EVENT_MASK (POLICY_AUDIT_EVENT_SUCCESS | \
                                 POLICY_AUDIT_EVENT_FAILURE | \
                                 POLICY_AUDIT_EVENT_NONE)

typedef UNICODE_STRING LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;
typedef STRING LSA_STRING, *PLSA_STRING;
typedef OBJECT_ATTRIBUTES LSA_OBJECT_ATTRIBUTES, *PLSA_OBJECT_ATTRIBUTES;
typedef ULONG LSA_OPERATIONAL_MODE, *PLSA_OPERATIONAL_MODE;

typedef PVOID LSA_HANDLE, *PLSA_HANDLE;

NTSTATUS WINAPI LsaOpenPolicy(PLSA_UNICODE_STRING,PLSA_OBJECT_ATTRIBUTES,ACCESS_MASK,PLSA_HANDLE);

typedef enum 
{
	PolicyAuditLogInformation = 1,
	PolicyAuditEventsInformation,
	PolicyPrimaryDomainInformation,
	PolicyPdAccountInformation,
	PolicyAccountDomainInformation,
	PolicyLsaServerRoleInformation,
	PolicyReplicaSourceInformation,
	PolicyDefaultQuotaInformation,
	PolicyModificationInformation,
	PolicyAuditFullSetInformation,
	PolicyAuditFullQueryInformation,
	PolicyDnsDomainInformation
} POLICY_INFORMATION_CLASS, *PPOLICY_INFORMATION_CLASS;

typedef enum _SECURITY_LOGON_TYPE
{
    Interactive = 2,
    Network,
    Batch,
    Service,
    Proxy
} SECURITY_LOGON_TYPE, *PSECURITY_LOGON_TYPE;

typedef enum _POLICY_AUDIT_EVENT_TYPE
{
    AuditCategorySystem,
    AuditCategoryLogon,
    AuditCategoryObjectAccess,
    AuditCategoryPrivilegeUse,
    AuditCategoryDetailedTracking,
    AuditCategoryPolicyChange,
    AuditCategoryAccountManagement
} POLICY_AUDIT_EVENT_TYPE, *PPOLICY_AUDIT_EVENT_TYPE;

typedef ULONG POLICY_AUDIT_EVENT_OPTIONS, *PPOLICY_AUDIT_EVENT_OPTIONS;

typedef struct
{
	BOOLEAN AuditingMode;
	PPOLICY_AUDIT_EVENT_OPTIONS EventAuditingOptions;
	ULONG MaximumAuditEventCount;
} POLICY_AUDIT_EVENTS_INFO, *PPOLICY_AUDIT_EVENTS_INFO;

typedef struct
{
    LSA_UNICODE_STRING Name;
    PSID Sid;
} POLICY_PRIMARY_DOMAIN_INFO, *PPOLICY_PRIMARY_DOMAIN_INFO;

typedef struct _POLICY_ACCOUNT_DOMAIN_INFO
{
    LSA_UNICODE_STRING DomainName;
    PSID DomainSid;
} POLICY_ACCOUNT_DOMAIN_INFO, *PPOLICY_ACCOUNT_DOMAIN_INFO;

typedef struct _POLICY_DNS_DOMAIN_INFO
{
    LSA_UNICODE_STRING Name;
    LSA_UNICODE_STRING DnsDomainName;
    LSA_UNICODE_STRING DnsForestName;
    GUID DomainGuid;
    PSID Sid;
} POLICY_DNS_DOMAIN_INFO, *PPOLICY_DNS_DOMAIN_INFO;

typedef enum _POLICY_LSA_SERVER_ROLE
{
    PolicyServerRoleBackup = 2,
    PolicyServerRolePrimary
} POLICY_LSA_SERVER_ROLE, *PPOLICY_LSA_SERVER_ROLE;

typedef struct _POLICY_LSA_SERVER_ROLE_INFO
{
    POLICY_LSA_SERVER_ROLE LsaServerRole;
} POLICY_LSA_SERVER_ROLE_INFO, *PPOLICY_LSA_SERVER_ROLE_INFO;

typedef struct _POLICY_MODIFICATION_INFO
{
    LARGE_INTEGER ModifiedId;
    LARGE_INTEGER DatabaseCreationTime;
} POLICY_MODIFICATION_INFO, *PPOLICY_MODIFICATION_INFO;

typedef struct _SECURITY_LOGON_SESSION_DATA {
    ULONG Size;
    LUID LogonId;
    LSA_UNICODE_STRING UserName;
    LSA_UNICODE_STRING LogonDomain;
    LSA_UNICODE_STRING AuthenticationPackage;
    ULONG LogonType;
    ULONG Session;
    PSID Sid;
    LARGE_INTEGER LogonTime;
    LSA_UNICODE_STRING LogonServer;
    LSA_UNICODE_STRING DnsDomainName;
    LSA_UNICODE_STRING Upn;
} SECURITY_LOGON_SESSION_DATA, *PSECURITY_LOGON_SESSION_DATA;

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

typedef struct _LSA_TRANSLATED_SID
{
    SID_NAME_USE Use;
    ULONG RelativeId;
    LONG DomainIndex;
} LSA_TRANSLATED_SID, *PLSA_TRANSLATED_SID;

typedef struct _TRUSTED_DOMAIN_INFORMATION_EX
{
    LSA_UNICODE_STRING Name;
    LSA_UNICODE_STRING FlatName;
    PSID Sid;
    ULONG TrustDirection;
    ULONG TrustType;
    ULONG TrustAttributes;
} TRUSTED_DOMAIN_INFORMATION_EX, *PTRUSTED_DOMAIN_INFORMATION_EX;

typedef struct _LSA_AUTH_INFORMATION
{
    LARGE_INTEGER LastUpdateTime;
    ULONG AuthType;
    ULONG AuthInfoLength;
    PUCHAR AuthInfo;
} LSA_AUTH_INFORMATION, *PLSA_AUTH_INFORMATION;

typedef struct _TRUSTED_DOMAIN_AUTH_INFORMATION
{
    ULONG IncomingAuthInfos;
    PLSA_AUTH_INFORMATION IncomingAuthenticationInformation;
    PLSA_AUTH_INFORMATION IncomingPreviousAuthenticationInformation;
    ULONG OutgoingAuthInfos;
    PLSA_AUTH_INFORMATION OutgoingAuthenticationInformation;
    PLSA_AUTH_INFORMATION OutgoingPreviousAuthenticationInformation;
} TRUSTED_DOMAIN_AUTH_INFORMATION, *PTRUSTED_DOMAIN_AUTH_INFORMATION;

typedef struct _LSA_TRANSLATED_SID2
{
    SID_NAME_USE Use;
    PSID Sid;
    LONG DomainIndex;
    ULONG Flags;
} LSA_TRANSLATED_SID2, *PLSA_TRANSLATED_SID2;

typedef enum _TRUSTED_INFORMATION_CLASS
{
    TrustedDomainNameInformation = 1,
    TrustedControllersInformation,
    TrustedPosixOffsetInformation,
    TrustedPasswordInformation,
    TrustedDomainInformationBasic,
    TrustedDomainInformationEx,
    TrustedDomainAuthInformation,
    TrustedDomainFullInformation
} TRUSTED_INFORMATION_CLASS, *PTRUSTED_INFORMATION_CLASS;

typedef enum _POLICY_NOTIFICATION_INFORMATION_CLASS
{
    PolicyNotifyAuditEventsInformation = 1,
    PolicyNotifyAccountDomainInformation,
    PolicyNotifyServerRoleInformation,
    PolicyNotifyDnsDomainInformation,
    PolicyNotifyDomainEfsInformation,
    PolicyNotifyDomainKerberosTicketInformation,
    PolicyNotifyMachineAccountPasswordInformation
} POLICY_NOTIFICATION_INFORMATION_CLASS, *PPOLICY_NOTIFICATION_INFORMATION_CLASS;

NTSTATUS WINAPI LsaQueryInformationPolicy(LSA_HANDLE,POLICY_INFORMATION_CLASS,PVOID*);

NTSTATUS WINAPI LsaFreeMemory(PVOID);
NTSTATUS WINAPI LsaClose(IN LSA_HANDLE ObjectHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* !defined(__WINE_NTSECAPI_H) */
