/* $Id: lsa.cpp,v 1.7 2002-02-21 22:51:59 sandervl Exp $ */

/*
 * Win32 Security Database API functions for OS/2
 *
 * Copyright 1999 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <os2win.h>
#include <misc.h>
#include "advapi32.h"

ODINDEBUGCHANNEL(ADVAPI32-LSA)


// still unknown functions
//ADVAPI32.SystemFunction007
//ADVAPI32.SystemFunction028
//ADVAPI32.SystemFunction022
//ADVAPI32.SystemFunction006
//ADVAPI32.SystemFunction014
//ADVAPI32.SystemFunction020
//ADVAPI32.SystemFunction012
//ADVAPI32.LsaSetInformationTrustedDomain
//ADVAPI32.LsaGetSystemAccessAccount
//ADVAPI32.LsaEnumeratePrivilegesOfAccount
//ADVAPI32.EnumServiceGroupW
//ADVAPI32.LsaOpenAccount
//ADVAPI32.LsaQueryInfoTrustedDomain
//ADVAPI32.LsaCreateTrustedDomain
//ADVAPI32.LsaOpenTrustedDomain
//ADVAPI32.LsaSetSecret
//ADVAPI32.LsaQuerySecret
//ADVAPI32.LsaOpenSecret
//ADVAPI32.LsaCreateSecret
//ADVAPI32.LsaEnumerateAccounts
//ADVAPI32.LsaEnumeratePrivileges
//ADVAPI32.LsaLookupPrivilegeDisplayName
//ADVAPI32.LsaRemovePrivilegesFromAccount
//ADVAPI32.LsaSetSystemAccessAccount
//ADVAPI32.LsaDelete
//ADVAPI32.LsaCreateAccount
//ADVAPI32.LsaAddPrivilegesToAccount


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1

// @@@PH: adopt to NTSTATUS.H
typedef DWORD NTSTATUS;

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS                 0 // ERROR_SUCCESS
#endif

typedef DWORD ACCESS_MASK;


typedef PVOID LSA_HANDLE, *PLSA_HANDLE;


typedef struct _LSA_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;


typedef struct _LSA_OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PLSA_UNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} LSA_OBJECT_ATTRIBUTES, *PLSA_OBJECT_ATTRIBUTES;


typedef struct _LSA_TRANSLATED_NAME {
    SID_NAME_USE Use;
    LSA_UNICODE_STRING Name;
    LONG DomainIndex;
} LSA_TRANSLATED_NAME, *PLSA_TRANSLATED_NAME;


typedef struct _LSA_TRANSLATED_SID {
    SID_NAME_USE Use;
    ULONG RelativeId;
    LONG DomainIndex;
} LSA_TRANSLATED_SID, *PLSA_TRANSLATED_SID;


typedef struct _LSA_TRUST_INFORMATION {
    LSA_UNICODE_STRING Name;
    PSID Sid;
} LSA_TRUST_INFORMATION, *PLSA_TRUST_INFORMATION;


typedef struct _LSA_REFERENCED_DOMAIN_LIST {
    ULONG Entries;
    PLSA_TRUST_INFORMATION Domains;
} LSA_REFERENCED_DOMAIN_LIST, *PLSA_REFERENCED_DOMAIN_LIST;


typedef enum _POLICY_INFORMATION_CLASS {
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
    PolicyDnsDomainInformation,
    PolicyMachinePasswordInformation
} POLICY_INFORMATION_CLASS, *PPOLICY_INFORMATION_CLASS;


typedef enum _POLICY_AUDIT_EVENT_TYPE {
    AuditCategorySystem,
    AuditCategoryLogon,
    AuditCategoryObjectAccess,
    AuditCategoryPrivilegeUse,
    AuditCategoryDetailedTracking,
    AuditCategoryPolicyChange,
    AuditCategoryAccountManagement,
    AuditCategoryDirectoryServiceAccess,
    AuditCategoryAccountLogon
} POLICY_AUDIT_EVENT_TYPE, *PPOLICY_AUDIT_EVENT_TYPE;


typedef struct _POLICY_AUDIT_EVENTS_INFO {
    BOOLEAN AuditingMode;
    //PPOLICY_AUDIT_EVENT_OPTIONS EventAuditingOptions;
    //PH: not documented
    DWORD EventAuditingOptions;
    ULONG MaximumAuditEventCount;
} POLICY_AUDIT_EVENTS_INFO, *PPOLICY_AUDIT_EVENTS_INFO;


typedef struct _POLICY_PRIMARY_DOMAIN_INFO {
    LSA_UNICODE_STRING Name;
    PSID Sid;
} POLICY_PRIMARY_DOMAIN_INFO, *PPOLICY_PRIMARY_DOMAIN_INFO;


typedef struct _POLICY_DNS_DOMAIN_INFO {
    LSA_UNICODE_STRING Name;
    LSA_UNICODE_STRING DnsDomainName;
    LSA_UNICODE_STRING DnsForestName;
    //GUID DomainGuid;
    // PH: not used yet
    DWORD DomainGuid;
    PSID Sid;
} POLICY_DNS_DOMAIN_INFO, *PPOLICY_DNS_DOMAIN_INFO;


typedef struct _POLICY_ACCOUNT_DOMAIN_INFO {
    LSA_UNICODE_STRING DomainName;
    PSID DomainSid;
} POLICY_ACCOUNT_DOMAIN_INFO, *PPOLICY_ACCOUNT_DOMAIN_INFO;


typedef enum _POLICY_LSA_SERVER_ROLE {
    PolicyServerRoleBackup = 2,
    PolicyServerRolePrimary
} POLICY_LSA_SERVER_ROLE, *PPOLICY_LSA_SERVER_ROLE;


typedef struct _POLICY_LSA_SERVER_ROLE_INFO {
    POLICY_LSA_SERVER_ROLE LsaServerRole;
} POLICY_LSA_SERVER_ROLE_INFO, *PPOLICY_LSA_SERVER_ROLE_INFO;


typedef struct _POLICY_MODIFICATION_INFO {
    LARGE_INTEGER ModifiedId;
    LARGE_INTEGER DatabaseCreationTime;
} POLICY_MODIFICATION_INFO, *PPOLICY_MODIFICATION_INFO;


typedef struct _POLICY_MACHINE_PASSWORD_INFO {
    LARGE_INTEGER PasswordChangeInterval;
} POLICY_MACHINE_PASSWORD_INFO, *PPOLICY_MACHINE_PASSWORD_INFO;


typedef ULONG LSA_ENUMERATION_HANDLE, *PLSA_ENUMERATION_HANDLE;


typedef struct _LSA_AUTH_INFORMATION {
    LARGE_INTEGER LastUpdateTime;
    ULONG AuthType;
    ULONG AuthInfoLength;
    PUCHAR AuthInfo;
} LSA_AUTH_INFORMATION, *PLSA_AUTH_INFORMATION;


typedef struct _TRUSTED_DOMAIN_AUTH_INFORMATION {
  ULONG IncomingAuthInfos;
  PLSA_AUTH_INFORMATION IncomingAuthenticationInformation;
  PLSA_AUTH_INFORMATION IncomingPreviousAuthenticationInformation;
  ULONG OutgoingAuthInfos;
  PLSA_AUTH_INFORMATION OutgoingAuthenticationInformation;
  PLSA_AUTH_INFORMATION OutgoingPreviousAuthenticationInformation;
} TRUSTED_DOMAIN_AUTH_INFORMATION, *PTRUSTED_DOMAIN_AUTH_INFORMATION;


typedef enum _TRUSTED_INFORMATION_CLASS {
    TrustedDomainNameInformation = 1,
    TrustedControllersInformation,
    TrustedPosixOffsetInformation,
    TrustedPasswordInformation,
    TrustedDomainInformationBasic,
    TrustedDomainInformationEx,
    TrustedDomainAuthInformation,
    TrustedDomainFullInformation
} TRUSTED_INFORMATION_CLASS, *PTRUSTED_INFORMATION_CLASS;


typedef struct _TRUSTED_DOMAIN_NAME_INFO {
    LSA_UNICODE_STRING Name;
} TRUSTED_DOMAIN_NAME_INFO, *PTRUSTED_DOMAIN_NAME_INFO;


typedef struct _TRUSTED_POSIX_OFFSET_INFO {
    ULONG Offset;
} TRUSTED_POSIX_OFFSET_INFO, *PTRUSTED_POSIX_OFFSET_INFO;


typedef struct _TRUSTED_PASSWORD_INFO {
    LSA_UNICODE_STRING Password;
    LSA_UNICODE_STRING OldPassword;
} TRUSTED_PASSWORD_INFO, *PTRUSTED_PASSWORD_INFO;


typedef struct _TRUSTED_DOMAIN_INFORMATION_EX {
    LSA_UNICODE_STRING Name;
    LSA_UNICODE_STRING FlatName;
    PSID  Sid;
    ULONG TrustDirection;
    ULONG TrustType;
    ULONG TrustAttributes;
} TRUSTED_DOMAIN_INFORMATION_EX, *PTRUSTED_DOMAIN_INFORMATION_EX;


typedef struct _TRUSTED_DOMAIN_FULL_INFORMATION {
    TRUSTED_DOMAIN_INFORMATION_EX   Information;
    TRUSTED_POSIX_OFFSET_INFO       PosixOffset;
    TRUSTED_DOMAIN_AUTH_INFORMATION AuthInformation;
} TRUSTED_DOMAIN_FULL_INFORMATION, *PTRUSTED_DOMAIN_FULL_INFORMATION;


// undocumented structures
typedef DWORD POLICY_NOTIFICATION_INFORMATION_CLASS;



/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

extern "C"
{
NTSTATUS WIN32API LsaAddAccountRights(LSA_HANDLE           PolicyHandle,
                                      PSID                 AccountSid,
                                      PLSA_UNICODE_STRING  UserRights,
                                      ULONG                CountOfRights);


NTSTATUS WIN32API LsaCreateTrustedDomainEx(LSA_HANDLE                       PolicyHandle,
                                           PTRUSTED_DOMAIN_INFORMATION_EX   TrustedDomainInformation,
                                           PTRUSTED_DOMAIN_AUTH_INFORMATION AuthenticationInformation,
                                           ACCESS_MASK                      DesiredAccess,
                                           PLSA_HANDLE                      TrustedDomainHandle);

NTSTATUS WIN32API LsaDeleteTrustedDomain(LSA_HANDLE  PolicyHandle,
                                         PSID        TrustedDomainSid);

NTSTATUS WIN32API LsaEnumerateAccountRights(LSA_HANDLE           PolicyHandle,
                                            PSID                 AccountSid,
                                            PLSA_UNICODE_STRING* UserRights,
                                            PULONG               CountOfRights);

NTSTATUS WIN32API LsaEnumerateAccountsWithUserRight(LSA_HANDLE           PolicyHandle,
                                                    PLSA_UNICODE_STRING  UserRight,
                                                    PVOID *              EnumerationBuffer,
                                                    PULONG               CountReturned);

NTSTATUS WIN32API LsaEnumerateTrustedDomains(LSA_HANDLE               PolicyHandle,
                                             PLSA_ENUMERATION_HANDLE  EnumerationContext,
                                             PVOID *                  Buffer,
                                             ULONG                    PreferedMaximumLength,
                                             PULONG                   CountReturned);

NTSTATUS WIN32API LsaEnumerateTrustedDomainsEx(LSA_HANDLE               PolicyHandle,
                                               PLSA_ENUMERATION_HANDLE  EnumerationContext,
                                               PVOID *                  Buffer,
                                               ULONG                    PreferredMaximumLength,
                                               PULONG                   CountReturned);


NTSTATUS WIN32API LsaLookupNames(LSA_HANDLE                   PolicyHandle,
                                 ULONG                        Count,
                                 PLSA_UNICODE_STRING          Names,
                                 PLSA_REFERENCED_DOMAIN_LIST* ReferencedDomains,
                                 PLSA_TRANSLATED_SID*         Sids);


ULONG WIN32API LsaNtStatusToWinError(NTSTATUS Status);

NTSTATUS WIN32API LsaOpenPolicy(PLSA_UNICODE_STRING     SystemName,
                                PLSA_OBJECT_ATTRIBUTES  ObjectAttributes,
                                ACCESS_MASK             DesiredAccess,
                                PLSA_HANDLE             PolicyHandle);

NTSTATUS WIN32API LsaOpenTrustedDomainByName(LSA_HANDLE          PolicyHandle,
                                             PLSA_UNICODE_STRING TrustedDomainName,
                                             ACCESS_MASK         DesiredAccess,
                                             PLSA_HANDLE         TrustedDomainHandle);


NTSTATUS WIN32API LsaQueryTrustedDomainInfo(LSA_HANDLE                 PolicyHandle,
                                            PSID                       TrustedDomainSid,
                                            TRUSTED_INFORMATION_CLASS  InformationClass,
                                            PVOID *                    Buffer);

NTSTATUS WIN32API LsaQueryTrustedDomainInfoByName(LSA_HANDLE                PolicyHandle,
                                                  PLSA_UNICODE_STRING       TrustedDomainName,
                                                  TRUSTED_INFORMATION_CLASS InformationClass,
                                                  PVOID *                   Buffer);

NTSTATUS WIN32API LsaRegisterPolicyChangeNotification(
              POLICY_NOTIFICATION_INFORMATION_CLASS InformationClass,
              HANDLE NotificationEventHandle);

NTSTATUS WIN32API LsaRemoveAccountRights(LSA_HANDLE          PolicyHandle,
                                         PSID                AccountSid,
                                         BOOLEAN             AllRights,
                                         PLSA_UNICODE_STRING UserRights,
                                         ULONG               CountOfRights);

NTSTATUS WIN32API LsaRetrievePrivateData(LSA_HANDLE           PolicyHandle,
                                         PLSA_UNICODE_STRING  KeyName,
                                         PLSA_UNICODE_STRING* PrivateData);

NTSTATUS WIN32API LsaSetInformationPolicy(LSA_HANDLE               PolicyHandle,
                                          POLICY_INFORMATION_CLASS InformationClass,
                                          PVOID *                  Buffer);

NTSTATUS WIN32API LsaSetTrustedDomainInfo(LSA_HANDLE                PolicyHandle,
                                          PSID                      TrustedDomainSid,
                                          TRUSTED_INFORMATION_CLASS InformationClass,
                                          PVOID                     Buffer);

NTSTATUS WIN32API LsaStorePrivateData(LSA_HANDLE          PolicyHandle,
                                      PLSA_UNICODE_STRING KeyName,
                                      PLSA_UNICODE_STRING PrivateData);

NTSTATUS WIN32API LsaUnregisterPolicyChangeNotification(
              POLICY_NOTIFICATION_INFORMATION_CLASS InformationClass,
              HANDLE   NotificationEventHandle);

}


/*****************************************************************************
 * Name      : LsaAddAccountRights
 * Purpose   : The LsaAddAccountRights function assigns one or more
 *             privileges to an account. The function ignores any specified
 *             privileges that are already held by the account. If the
 *             account does not exist, LsaAddAccountRights creates it.
 * Parameters: LSA_HANDLE PolicyHandle,
 *             PSID AccountSid,
 *             PLSA_UNICODE_STRING UserRights,
 *             ULONG CountOfRights
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,            LsaAddAccountRights,
              LSA_HANDLE,          PolicyHandle,
              PSID,                AccountSid,
              PLSA_UNICODE_STRING, UserRights,
              ULONG,               CountOfRights)
{
  dprintf(("ADVAPI32:Lsa: LsaAddAccountRights not implemented.\n"));
  return 0;
}




/*****************************************************************************
 * Name      : LsaCreateTrustedDomainEx
 * Purpose   : The LsaCreateTrustedDomainEx function creates a new
 *             TrustedDomain object.
 * Parameters: LSA_HANDLE                       PolicyHandle,
 *             PTRUSTED_DOMAIN_INFORMATION_EX   TrustedDomainInformation,
 *             PTRUSTED_DOMAIN_AUTH_INFORMATION AuthenticationInformation,
 *             ACCESS_MASK                      DesiredAccess,
 *             PLSA_HANDLE                      TrustedDomainHandle
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION5(NTSTATUS,                        LsaCreateTrustedDomainEx,
              LSA_HANDLE,                      PolicyHandle,
              PTRUSTED_DOMAIN_INFORMATION_EX,  TrustedDomainInformation,
              PTRUSTED_DOMAIN_AUTH_INFORMATION,AuthenticationInformation,
              ACCESS_MASK,                     DesiredAccess,
              PLSA_HANDLE,                     TrustedDomainHandle)
{
  dprintf(("ADVAPI32:Lsa: LsaCreateTrustedDomainEx not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaDeleteTrustedDomain
 * Purpose   : The LsaDeleteTrustedDomain function deletes a trusted
 *             domain from list of trusted domains for an LSA policy object.
 * Parameters: LSA_HANDLE PolicyHandle
 *             PSID       TrustedDomainSid
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION2(NTSTATUS,LsaDeleteTrustedDomain, LSA_HANDLE, PolicyHandle,
                                               PSID,       TrustedDomainSid)
{
  dprintf(("ADVAPI32:Lsa: LsaDeleteTrustedDomain not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaEnumerateAccountRights
 * Purpose   : The LsaEnumerateAccountRights function enumerates
 *             the privileges assigned to an account.
 * Parameters: LSA_HANDLE PolicyHandle,
 *             PSID AccountSid,
 *             PLSA_UNICODE_STRING *UserRights,
 *             PULONG CountOfRights
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,            LsaEnumerateAccountRights,
              LSA_HANDLE,          PolicyHandle,
              PSID,                AccountSid,
              PLSA_UNICODE_STRING*,UserRights,
              PULONG,              CountOfRights)
{
  dprintf(("ADVAPI32:Lsa: LsaEnumerateAccountRights not implemented.\n"));
  return 0;
}



/*****************************************************************************
 * Name      : LsaEnumerateAccountsWithUserRight
 * Purpose   : The LsaEnumerateAccountsWithUserRight
 *             function returns an array of pointers to SIDs that
 *             identify the accounts in an LSA policy object's
 *             database that hold a specified privilege.
 * Parameters: LSA_HANDLE           PolicyHandle
 *             PLSA_UNICODE_STRING  UserRight
 *             PVOID *              EnumerationBuffer
 *             PULONG               CountReturned
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,            LsaEnumerateAccountsWithUserRight,
              LSA_HANDLE,          PolicyHandle,
              PLSA_UNICODE_STRING, UserRight,
              PVOID *,             EnumerationBuffer,
              PULONG,              CountReturned)
{
  dprintf(("ADVAPI32:Lsa: LsaEnumerateAccountsWithUserRight not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaEnumerateTrustedDomains
 * Purpose   : The LsaEnumerateTrustedDomains function retrieves
 *             the names and SIDs of domains trusted by an LSA policy
 *             object.
 * Parameters: LSA_HANDLE              PolicyHandle,
 *             PLSA_ENUMERATION_HANDLE EnumerationContext,
 *             PVOID *                 Buffer,
 *             ULONG                   PreferedMaximumLength,
 *             PULONG                  CountReturned
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION5(NTSTATUS,                LsaEnumerateTrustedDomains,
              LSA_HANDLE,              PolicyHandle,
              PLSA_ENUMERATION_HANDLE, EnumerationContext,
              PVOID *,                 Buffer,
              ULONG,                   PreferedMaximumLength,
              PULONG,                  CountReturned)
{
  dprintf(("ADVAPI32:Lsa: LsaEnumerateTrustedDomains not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaEnumerateTrustedDomainsEx
 * Purpose   : The LsaEnumerateTrustedDomainsEx function returns
 *             information about the domains trusted by the local system.
 *             This function returns more information than
 *             LsaEnumerateTrustedDomains.
 * Parameters: LSA_HANDLE              PolicyHandle,
 *             PLSA_ENUMERATION_HANDLE EnumerationContext,
 *             PVOID *                 Buffer,
 *             ULONG                   PrefefredMaximumLength,
 *             PULONG                  CountReturned
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION5(NTSTATUS,                LsaEnumerateTrustedDomainsEx,
              LSA_HANDLE,              PolicyHandle,
              PLSA_ENUMERATION_HANDLE, EnumerationContext,
              PVOID *,                 Buffer,
              ULONG,                   PreferredMaximumLength,
              PULONG,                  CountReturned)
{
  dprintf(("ADVAPI32:Lsa: LsaEnumerateTrustedDomains not implemented.\n"));
  return 0;
}




/*****************************************************************************
 * Name      : LsaLookupNames
 * Purpose   : The LsaLookupNames function looks up the SIDs
 *             that correspond to an array of user, group, or local
 *             group names.
 * Parameters: LSA_HANDLE                  PolicyHandle
 *             ULONG                       Count
 *             PLSA_UNICODE_STRING         Names
 *             PLSA_REFERENCED_DOMAIN_LIST *ReferencedDomains
 *             PLSA_TRANSLATED_SID         *Sids
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION5(NTSTATUS,                    LsaLookupNames,
              LSA_HANDLE,                  PolicyHandle,
              ULONG,                       Count,
              PLSA_UNICODE_STRING,         Names,
              PLSA_REFERENCED_DOMAIN_LIST*, ReferencedDomains,
              PLSA_TRANSLATED_SID*,        Sids)
{
  dprintf(("ADVAPI32:Lsa: LsaLookupNames not implemented.\n"));
  return 0;
}




/*****************************************************************************
 * Name      : LsaNtStatusToWinError
 * Purpose   : The LsaNtStatusToWinError function converts an
 *             NTSTATUS code returned by an LSA function to a Windows
 *             error code
 * Parameters: NTSTATUS Status
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION1(ULONG,    LsaNtStatusToWinError,
              NTSTATUS, Status)
{
  dprintf(("ADVAPI32:Lsa: LsaNtStatusToWinError not implemented.\n"));
  return Status;
}



/*****************************************************************************
 * Name      : LsaOpenTrustedDomainByName
 * Purpose   : The LsaOpenTrustedDomainByName function opens the
 *             LSA policy handle on a remote trusted domain. You can use
 *             pass this handle into LSA function calls in order to query
 *             and/or manage the LSA policy of the remote machine.
 * Parameters: LSA_HANDLE          PolicyHandle,
 *             PLSA_UNICODE_STRING TrustedDomainName,
 *             ACCESS_MASK         DesiredAccess,
 *             PLSA_HANDLE         TrustedDomainHandle
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,            LsaOpenTrustedDomainByName,
              LSA_HANDLE,          PolicyHandle,
              PLSA_UNICODE_STRING, TrustedDomainName,
              ACCESS_MASK,         DesiredAccess,
              PLSA_HANDLE,         TrustedDomainHandle)
{
  dprintf(("ADVAPI32:Lsa: LsaOpenTrustedDomainByName not implemented.\n"));
  return 0;
}



/*****************************************************************************
 * Name      : LsaQueryTrustedDomainInfo
 * Purpose   : The LsaQueryTrustedDomainInfo function retrieves
 *             information about a trusted domain object.
 * Parameters: LSA_HANDLE                PolicyHandle,
 *             PSID                      TrustedDomainSid,
 *             TRUSTED_INFORMATION_CLASS InformationClass,
 *             PVOID *                   Buffer
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,                  LsaQueryTrustedDomainInfo,
              LSA_HANDLE,                PolicyHandle,
              PSID,                      TrustedDomainSid,
              TRUSTED_INFORMATION_CLASS, InformationClass,
              PVOID *,                   Buffer)
{
  dprintf(("ADVAPI32:Lsa: LsaQueryTrustedDomainInfo not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaQueryTrustedDomainInfoByName
 * Purpose   : The LsaQueryTrustedDomainInfo function retrieves
 *             information about a trusted domain object.
 * Parameters: LSA_HANDLE                PolicyHandle,
 *             PLSA_UNICODE_STRING       TrustedDomainName,
 *             TRUSTED_INFORMATION_CLASS InformationClass,
 *             PVOID *                   Buffer
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,                  LsaQueryTrustedDomainInfoByName,
              LSA_HANDLE,                PolicyHandle,
              PLSA_UNICODE_STRING,       TrustedDomainName,
              TRUSTED_INFORMATION_CLASS, InformationClass,
              PVOID *,                   Buffer)
{
  dprintf(("ADVAPI32:Lsa: LsaQueryTrustedDomainInfoByName not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaRegisterPolicyChangeNotification
 * Purpose   : The LsaRegisterPolicyChangeNotification function
 *             registers an event handle with the local security authority
 *             (LSA). This event handle is signaled whenever the indicated
 *             LSA policy is modified.
 * Parameters: POLICY_NOTIFICATION_INFORMATION_CLASS InformationClass
 *             HANDLE                                NotificationEventHandle
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION2(NTSTATUS,  LsaRegisterPolicyChangeNotification,
              POLICY_NOTIFICATION_INFORMATION_CLASS, InformationClass,
              HANDLE,   NotificationEventHandle)
{
  dprintf(("ADVAPI32:Lsa: LsaRegisterPolicyChangeNotification not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaRemoveAccountRights
 * Purpose   : The LsaRemoveAccountRights function removes one or
 *             more privileges from an account. You can specify the
 *             privileges to be removed, or you can set a flag to remove all
 *             privileges. If you specify privileges not held by the account,
 *             the function ignores them. The function deletes the account
 *             if you remove all privileges.
 * Parameters: LSA_HANDLE PolicyHandle,
 *             PSID AccountSid,
 *             BOOLEAN AllRights,
 *             PLSA_UNICODE_STRING UserRights,
 *             ULONG CountOfRights
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION5(NTSTATUS,            LsaRemoveAccountRights,
              LSA_HANDLE,          PolicyHandle,
              PSID,                AccountSid,
              BOOLEAN,             AllRights,
              PLSA_UNICODE_STRING, UserRights,
              ULONG,               CountOfRights)
{
  dprintf(("ADVAPI32:Lsa: LsaRemoveAccountRights not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaRetrievePrivateData
 * Purpose   : The LsaRetrievePrivateData function retrieves private
 *             data that was stored by the LsaStorePrivateData
 *             function.
 * Parameters: LSA_HANDLE          PolicyHandle,
 *             PLSA_UNICODE_STRING KeyName,
 *             PLSA_UNICODE_STRING *PrivateData
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION3(NTSTATUS,            LsaRetrievePrivateData,
              LSA_HANDLE,          PolicyHandle,
              PLSA_UNICODE_STRING, KeyName,
              PLSA_UNICODE_STRING*,PrivateData)
{
  dprintf(("ADVAPI32:Lsa: LsaRetrievePrivateData not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaSetInformationPolicy
 * Purpose   : The LsaSetInformationPolicy function modifies
 *             information in an LSA policy object.
 * Parameters: LSA_HANDLE PolicyHandle,
 *             POLICY_INFORMATION_CLASS InformationClass,
 *             PVOID *Buffer
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION3(NTSTATUS,                 LsaSetInformationPolicy,
              LSA_HANDLE,               PolicyHandle,
              POLICY_INFORMATION_CLASS, InformationClass,
              PVOID *,                  Buffer)
{
  dprintf(("ADVAPI32:Lsa: LsaSetInformationPolicy not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaSetTrustedDomainInfo
 * Purpose   : The LsaSetTrustedDomainInformation function
 *             modifies an LSA policy object's information about a trusted
 *             domain object.
 * Parameters: LSA_HANDLE                PolicyHandle,
 *             PSID                      TrustedDomainSid,
 *             TRUSTED_INFORMATION_CLASS InformationClass,
 *             PVOID                     Buffer
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,                  LsaSetTrustedDomainInfo,
              LSA_HANDLE,                PolicyHandle,
              PSID,                      TrustedDomainSid,
              TRUSTED_INFORMATION_CLASS, InformationClass,
              PVOID,                     Buffer)
{
  dprintf(("ADVAPI32:Lsa: LsaSetTrustedDomainInfo not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaSetTrustedDomainInfoByName
 * Purpose   : The LsaSetTrustedDomainInformation function
 *             modifies an LSA policy object's information about a trusted
 *             domain object.
 * Parameters: LSA_HANDLE                PolicyHandle,
 *             PLSA_UNICODE_STRING       TrustedDomainName,
 *             TRUSTED_INFORMATION_CLASS InformationClass,
 *             PVOID                     Buffer
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION4(NTSTATUS,                  LsaSetTrustedDomainInfoByName,
              LSA_HANDLE,                PolicyHandle,
              PLSA_UNICODE_STRING,       TrustedDomainName,
              TRUSTED_INFORMATION_CLASS, InformationClass,
              PVOID,                     Buffer)
{
  dprintf(("ADVAPI32:Lsa: LsaSetTrustedDomainInfoByName not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaStorePrivateData
 * Purpose   : The LsaStorePrivateData function stores or deletes an
 *             LSA policy object's private data under a specified registry
 *             key.
 * Parameters: LSA_HANDLE          PolicyHandle,
 *             PLSA_UNICODE_STRING KeyName,
 *             PLSA_UNICODE_STRING PrivateData
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION3(NTSTATUS,           LsaStorePrivateData,
              LSA_HANDLE,         PolicyHandle,
              PLSA_UNICODE_STRING,KeyName,
              PLSA_UNICODE_STRING,PrivateData)
{
  dprintf(("ADVAPI32:Lsa: LsaStorePrivateData not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : LsaUnregisterPolicyChangeNotification
 * Purpose   : The LsaUnregisterPolicyChangeNotification function
 *             disables the previously registered notification event.
 * Parameters: POLICY_NOTIFICATION_INFORMATION_CLASS InformationClass
 *             HANDLE                                NotificationEventHandle
 * Variables :
 * Result    : NTSTATUS
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 23:00]
 *****************************************************************************/

ODINFUNCTION2(NTSTATUS,  LsaUnregisterPolicyChangeNotification,
              POLICY_NOTIFICATION_INFORMATION_CLASS, InformationClass,
              HANDLE,   NotificationEventHandle)
{
  dprintf(("ADVAPI32:Lsa: LsaUnregisterPolicyChangeNotification not implemented.\n"));
  return 0;
}

