/* $Id: ADVAPI32.CPP,v 1.17 2001-04-22 10:39:26 sandervl Exp $ */

/*
 * Win32 advanced API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * @(#) ADVAPI32.C            1.0.1 1998/06/14 PH added stubs
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include "misc.h"
#include "advapi32.h"
#include "unicode.h"
#include "winreg.h"
#include <heapstring.h>

ODINDEBUGCHANNEL(ADVAPI32-ADVAPI32)


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1


/*****************************************************************************
 * Name      : SetTokenInformation
 * Purpose   : The SetTokenInformation function sets various types of
 *             information for a specified access token. The information it
 *             sets replaces existing information. The calling process must
 *             have appropriate access rights to set the information.
 * Parameters: HANDLE                  hToken         handle of access token
 *             TOKEN_INFORMATION_CLASS tic            type of information to set
 *             LPVOID                  lpvInformation address of information to set
 *             DWORD                   cbInformation  size of information buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetTokenInformation(HANDLE                  hToken,
                                     TOKEN_INFORMATION_CLASS tic,
                                     LPVOID                  lpvInformation,
                                     DWORD                   cbInformation)
{
  dprintf(("ADVAPI32: SetTokenInformation(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hToken,
           tic,
           lpvInformation,
           cbInformation));

  return (FALSE); /* signal failure */
}

//******************************************************************************
//******************************************************************************
BOOL WIN32API GetUserNameA(  /*PLF Wed  98-02-11 13:33:39*/
    LPTSTR lpBuffer,        /* address of name buffer       */
    LPDWORD lpcchBuffer)    /* address of size of name buffer       */


    /* The GetUserName function retrieves the user name of the current
     *  thread. This is the name of the user currently logged onto the
     *  system.
     */

{
    #define USERNAME "USER"
    if(*lpcchBuffer < sizeof(USERNAME))
        return FALSE;
    strcpy(lpBuffer, USERNAME);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetUserNameW( /*KSO Thu 21.05.1998 */
   LPWSTR lpBuffer,
   LPDWORD lpccBuffer
   )
{

   if ( *lpccBuffer >= sizeof(USERNAME)*2 )
   {
      AsciiToUnicode(USERNAME, lpBuffer);
      return TRUE;
   }
      return FALSE;
}
//******************************************************************************
//******************************************************************************



/*PLF Sat  98-03-07 02:59:20*/

/*****************************************************************************
 * Name      : AbortSystemShutdownA
 * Purpose   : The AbortSystemShutdown function stops a system shutdown started
 *             by using the InitiateSystemShutdown function.
 * Parameters: LPTSTR  lpMachineName  address of name of computer to stop shutting down
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AbortSystemShutdownA(LPTSTR lpMachineName)
{
  dprintf(("ADVAPI32: AbortSystemShutdownA(%s) not implemented.\n",
           lpMachineName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AbortSystemShutdownW
 * Purpose   : The AbortSystemShutdown function stops a system shutdown started
 *             by using the InitiateSystemShutdown function.
 * Parameters: LPWSTR  lpMachineName  address of name of computer to stop shutting down
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AbortSystemShutdownW(LPWSTR lpMachineName)
{
  dprintf(("ADVAPI32: AbortSystemShutdownW(%s) not implemented.\n",
           lpMachineName));

  return (FALSE);
}




/*****************************************************************************
 * Name      : AccessCheckAndAuditAlarmA
 * Purpose   : The AccessCheckAndAuditAlarm function performs an access
 *             validation and generates corresponding audit messages. An
 *             application can also use this function to determine whether
 *             necessary privileges are held by a client process. This function
 *             is generally used by a server application impersonating a client
 *             process. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR              SubsystemName      address of string for subsystem name
 *             LPVOID               HandleId           address of handle identifier
 *             LPTSTR               ObjectTypeName     address of string for object type
 *             LPTSTR               ObjectName         address of string for object name
 *             PSECURITY_DESCRIPTOR SecurityDescriptor address of security descriptor
 *             DWORD                DesiredAccess      mask for requested access rights
 *             PGENERIC_MAPPING     GenericMapping     address of GENERIC_MAPPING
 *             BOOL                 ObjectCreation     object-creation flag
 *             LPDWORD              GrantedAccess      address of mask for granted rights
 *             LPBOOL               AccessStatus       address of flag for results
 *             LPBOOL               pfGenerateOnClose  address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AccessCheckAndAuditAlarmA(LPCSTR              SubsystemName,
                                           LPVOID               HandleId,
                                           LPTSTR               ObjectTypeName,
                                           LPTSTR               ObjectName,
                                           PSECURITY_DESCRIPTOR SecurityDescriptor,
                                           DWORD                DesiredAccess,
                                           PGENERIC_MAPPING     GenericMapping,
                                           BOOL                 ObjectCreation,
                                           LPDWORD              GrantedAccess,
                                           LPBOOL               AccessStatus,
                                           LPBOOL               pfGenerateOnClose)
{
  dprintf(("ADVAPI32: AccessCheckAndAuditAlarmA(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           SecurityDescriptor,
           DesiredAccess,
           GenericMapping,
           ObjectCreation,
           GrantedAccess,
           AccessStatus,
           pfGenerateOnClose));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AccessCheckAndAuditAlarmW
 * Purpose   : The AccessCheckAndAuditAlarm function performs an access
 *             validation and generates corresponding audit messages. An
 *             application can also use this function to determine whether
 *             necessary privileges are held by a client process. This function
 *             is generally used by a server application impersonating a client
 *             process. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR              SubsystemName      address of string for subsystem name
 *             LPVOID               HandleId           address of handle identifier
 *             LPTSTR               ObjectTypeName     address of string for object type
 *             LPTSTR               ObjectName         address of string for object name
 *             PSECURITY_DESCRIPTOR SecurityDescriptor address of security descriptor
 *             DWORD                DesiredAccess      mask for requested access rights
 *             PGENERIC_MAPPING     GenericMapping     address of GENERIC_MAPPING
 *             BOOL                 ObjectCreation     object-creation flag
 *             LPDWORD              GrantedAccess      address of mask for granted rights
 *             LPBOOL               AccessStatus       address of flag for results
 *             LPBOOL               pfGenerateOnClose  address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AccessCheckAndAuditAlarmW(LPCWSTR              SubsystemName,
                                           LPVOID               HandleId,
                                           LPWSTR               ObjectTypeName,
                                           LPWSTR               ObjectName,
                                           PSECURITY_DESCRIPTOR SecurityDescriptor,
                                           DWORD                DesiredAccess,
                                           PGENERIC_MAPPING     GenericMapping,
                                           BOOL                 ObjectCreation,
                                           LPDWORD              GrantedAccess,
                                           LPBOOL               AccessStatus,
                                           LPBOOL               pfGenerateOnClose)
{
  dprintf(("ADVAPI32: AccessCheckAndAuditAlarmW(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           SecurityDescriptor,
           DesiredAccess,
           GenericMapping,
           ObjectCreation,
           GrantedAccess,
           AccessStatus,
           pfGenerateOnClose));

  return (FALSE);
}

/*****************************************************************************
 * Name      : AddAccessDeniedAce
 * Purpose   : The AddAccessDeniedAce function adds an access-denied ACE to an
 *             ACL. The access is denied to a specified SID. An ACE is an
 *             access-control entry. An ACL is an access-control list. A SID
 *             is a security identifier.
 * Parameters: PACL  pAcl           address of access-control list
 *             DWORD dwAceRevision  ACL revision level
 *             DWORD AccessMask     access mask
 *             PSID  pSid           address of security identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AddAccessDeniedAce(PACL  pAcl,
                                    DWORD dwAceRevision,
                                    DWORD AccessMask,
                                    PSID  pSid)
{
  dprintf(("ADVAPI32: AddAccessDeniedAce(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceRevision,
           AccessMask,
           pSid));

  return (FALSE);
}

/*****************************************************************************
 * Name      : AddAuditAccessAce
 * Purpose   : The AddAuditAccessAce function adds a system-audit ACE to a
 *             system ACL. The access of a specified SID is audited. An ACE is
 *             an access-control entry. An ACL is an access-control list. A SID
 *             is a security identifier.
 * Parameters: PACL  pAcl           address of access-control list
 *             DWORD dwAceRevision  ACL revision level
 *             DWORD dwAccessMask   access mask
 *             PSID  pSid           address of security identifier
 *             BOOL  bAuditSuccess  flag for auditing successful access
 *             BOOL  bAuditFailure  flag for auditing unsuccessful access attempts
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AddAuditAccessAce(PACL  pAcl,
                                   DWORD dwAceRevision,
                                   DWORD dwAccessMask,
                                   PSID  pSid,
                                   BOOL  bAuditSuccess,
                                   BOOL  bAuditFailure)
{
  dprintf(("ADVAPI32: AddAuditAccessAce(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceRevision,
           dwAccessMask,
           pSid,
           bAuditSuccess,
           bAuditFailure));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AdjustTokenGroups
 * Purpose   : The AdjustTokenGroups function adjusts groups in the specified
 *             access token. TOKEN_ADJUST_GROUPS access is required to enable
 *             or disable groups in an access token.
 * Parameters: HANDLE         TokenHandle     handle of token that contains groups
 *             BOOL           ResetToDefault  flag for default settings
 *             PTOKEN_GROUPS  NewState        address of new group information
 *             DWORD          BufferLength    size of buffer for previous information
 *             PTOKEN_GROUPS  PreviousState   address of previous group information
 *             LPDWORD         ReturnLength    address of required buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AdjustTokenGroups(HANDLE         TokenHandle,
                                   BOOL           ResetToDefault,
                                   PTOKEN_GROUPS  NewState,
                                   DWORD          BufferLength,
                                   PTOKEN_GROUPS  PreviousState,
                                   LPDWORD         ReturnLength)
{
  dprintf(("ADVAPI32: AdjustTokenGroups(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           TokenHandle,
           ResetToDefault,
           NewState,
           BufferLength,
           PreviousState,
           ReturnLength));

  return (FALSE); /* signal failure */
}



/*****************************************************************************
 * Name      : AllocateLocallyUniqueId
 * Purpose   : The AllocateLocallyUniqueId function allocates a locally unique
 *             identifier (LUID).
 * Parameters: PLUID  Luid  address of locally unique identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AllocateLocallyUniqueId(PLUID  Luid)
{
  dprintf(("ADVAPI32: AllocateLocallyUniqueId(%08xh) not implemented.\n",
           Luid));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : AreAllAccessesGranted
 * Purpose   : The AreAllAccessesGranted function checks whether a set of
 *             requested access rights has been granted. The access rights are
 *             represented as bit flags in a 32-bit access mask.
 * Parameters: DWORD GrantedAccess  access mask for granted access rights
 *             DWORD DesiredAccess  access mask for requested access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AreAllAccessesGranted(DWORD  GrantedAccess,
                                       DWORD  DesiredAccess)
{
  dprintf(("ADVAPI32: AreAllAccessesGranted(%08xh,%08xh) not implemented.\n",
           GrantedAccess,
           DesiredAccess));

  return (TRUE); /* grant all access */
}


/*****************************************************************************
 * Name      : AreAnyAccessesGranted
 * Purpose   : The AreAnyAccessesGranted function tests whether any of a set of
 *             requested access rights has been granted. The access rights are
 *             represented as bit flags in a 32-bit access mask.
 * Parameters: DWORD GrantedAccess  access mask for granted access rights
 *             DWORD DesiredAccess  access mask for requested access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AreAnyAccessesGranted(DWORD  GrantedAccess,
                                       DWORD  DesiredAccess)
{
  dprintf(("ADVAPI32: AreAnyAccessesGranted(%08xh,%08xh) not implemented.\n",
           GrantedAccess,
           DesiredAccess));

  return (TRUE); /* grant all access */
}

/*****************************************************************************
 * Name      : CreatePrivateObjectSecurity
 * Purpose   : The CreatePrivateObjectSecurity function allocates and initializes
 *             a self-relative security descriptor for a new protected server's
 *             object. This function is called when a new protected server object is being created.
 * Parameters: PSECURITY_DESCRIPTOR  ParentDescriptor  address of parent directory SD
 *             PSECURITY_DESCRIPTOR  CreatorDescriptor address of creator SD
 *             PSECURITY_DESCRIPTOR *NewDescriptor     address of pointer to new SD
 *             BOOL                  IsDirectoryObject container flag for new SD
 *             HANDLE                Token             handle of client's access token
 *             PGENERIC_MAPPING      GenericMapping    address of access-rights structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CreatePrivateObjectSecurity(PSECURITY_DESCRIPTOR  ParentDescriptor,
                                             PSECURITY_DESCRIPTOR  CreatorDescriptor,
                                             PSECURITY_DESCRIPTOR *NewDescriptor,
                                             BOOL                  IsDirectoryObject,
                                             HANDLE                Token,
                                             PGENERIC_MAPPING      GenericMapping)
{
  dprintf(("ADVAPI32: CreatePrivateObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ParentDescriptor,
           CreatorDescriptor,
           NewDescriptor,
           IsDirectoryObject,
           Token,
           GenericMapping));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CreateProcessAsUserA
 * Purpose   : The CreateProcessAsUser function creates a new process and its
 *             primary thread. The new process then executes a specified executable
 *             file. The CreateProcessAsUser function behaves just like the
 *             CreateProcess function, with one important difference: the
 *             created process runs in a context in which the system sees the
 *             user represented by the hToken parameter as if that user had
 *             logged on to the system and then called the CreateProcess function.
 * Parameters: HANDLE                 hToken               handle to a token that represents a logged-on user
 *             LPCSTR                lpApplicationName    pointer to name of executable module
 *             LPTSTR                 lpCommandLine        pointer to command line string
 *             LPSECURITY_ATTRIBUTES  lpProcessAttributes  pointer to process security attributes
 *             LPSECURITY_ATTRIBUTES  lpThreadAttributes   pointer to thread security attributes
 *             BOOL                   bInheritHandles      new process inherits handles
 *             DWORD                  dwCreationFlags      creation flags
 *             LPVOID                 lpEnvironment        pointer to new environment block
 *             LPCSTR                lpCurrentDirectory   pointer to current directory name
 *             LPSTARTUPINFO          lpStartupInfo        pointer to STARTUPINFO
 *             LPPROCESS_INFORMATION  lpProcessInformation pointer to PROCESS_INFORMATION
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CreateProcessAsUserA(HANDLE                 hToken,
                                      LPCSTR                lpApplicationName,
                                      LPTSTR                 lpCommandLine,
                                      LPSECURITY_ATTRIBUTES  lpProcessAttributes,
                                      LPSECURITY_ATTRIBUTES  lpThreadAttributes,
                                      BOOL                   bInheritHandles,
                                      DWORD                  dwCreationFlags,
                                      LPVOID                 lpEnvironment,
                                      LPCSTR                 lpCurrentDirectory,
                                      LPSTARTUPINFOA         lpStartupInfo,
                                      LPPROCESS_INFORMATION  lpProcessInformation)
{
  dprintf(("ADVAPI32: CreateProcessAsUserA(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           hToken,
           lpApplicationName,
           lpCommandLine,
           lpProcessAttributes,
           lpThreadAttributes,
           bInheritHandles,
           dwCreationFlags,
           lpEnvironment,
           lpCurrentDirectory,
           lpStartupInfo,
           lpProcessInformation));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CreateProcessAsUserW
 * Purpose   : The CreateProcessAsUser function creates a new process and its
 *             primary thread. The new process then executes a specified executable
 *             file. The CreateProcessAsUser function behaves just like the
 *             CreateProcess function, with one important difference: the
 *             created process runs in a context in which the system sees the
 *             user represented by the hToken parameter as if that user had
 *             logged on to the system and then called the CreateProcess function.
 * Parameters: HANDLE                 hToken               handle to a token that represents a logged-on user
 *             LPCWSTR                lpApplicationName    pointer to name of executable module
 *             LPWSTR                 lpCommandLine        pointer to command line string
 *             LPSECURITY_ATTRIBUTES  lpProcessAttributes  pointer to process security attributes
 *             LPSECURITY_ATTRIBUTES  lpThreadAttributes   pointer to thread security attributes
 *             BOOL                   bInheritHandles      new process inherits handles
 *             DWORD                  dwCreationFlags      creation flags
 *             LPVOID                 lpEnvironment        pointer to new environment block
 *             LPCWSTR                lpCurrentDirectory   pointer to current directory name
 *             LPSTARTUPINFO          lpStartupInfo        pointer to STARTUPINFO
 *             LPPROCESS_INFORMATION  lpProcessInformation pointer to PROCESS_INFORMATION
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CreateProcessAsUserW(HANDLE                 hToken,
                                      LPCWSTR                lpApplicationName,
                                      LPWSTR                 lpCommandLine,
                                      LPSECURITY_ATTRIBUTES  lpProcessAttributes,
                                      LPSECURITY_ATTRIBUTES  lpThreadAttributes,
                                      BOOL                   bInheritHandles,
                                      DWORD                  dwCreationFlags,
                                      LPVOID                 lpEnvironment,
                                      LPCWSTR                lpCurrentDirectory,
                                      LPSTARTUPINFOA         lpStartupInfo,
                                      LPPROCESS_INFORMATION  lpProcessInformation)
{
  dprintf(("ADVAPI32: CreateProcessAsUserW(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           hToken,
           lpApplicationName,
           lpCommandLine,
           lpProcessAttributes,
           lpThreadAttributes,
           bInheritHandles,
           dwCreationFlags,
           lpEnvironment,
           lpCurrentDirectory,
           lpStartupInfo,
           lpProcessInformation));

  return (FALSE); /* signal failure */
}



/*****************************************************************************
 * Name      : DeleteAce
 * Purpose   : The DeleteAce function deletes an ACE from an ACL.
 *             An ACE is an access-control entry. An ACL is an access-control list.
 * Parameters: PACL  pAcl        address of access-control list
 *             DWORD dwAceIndex  index of ACE position in ACL
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API DeleteAce(PACL  pAcl,
                           DWORD dwAceIndex)
{
  dprintf(("ADVAPI32: DeleteAce(%08xh, %08xh) not implemented.\n",
           pAcl,
           dwAceIndex));

  return (FALSE); /* signal failure */
}



/*****************************************************************************
 * Name      : DestroyPrivateObjectSecurity
 * Purpose   : The DestroyPrivateObjectSecurity function deletes a protected
 *             server object's security descriptor. This security descriptor
 *             must have been created by a call to the CreatePrivateObjectSecurity function.
 * Parameters: PSECURITY_DESCRIPTOR  * ObjectDescriptor  address of pointer to SECURITY_DESCRIPTOR
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API DestroyPrivateObjectSecurity(PSECURITY_DESCRIPTOR *ObjectDescriptor)
{
  dprintf(("ADVAPI32: DestroyPrivateObjectSecurity(%08xh) not implemented.\n",
           ObjectDescriptor));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : DuplicateToken
 * Purpose   : The DuplicateToken function creates a new access token that
 *             duplicates one already in existence.
 * Parameters: HANDLE                       ExistingTokenHandle  handle of token to duplicate
 *             SECURITY_IMPERSONATION_LEVEL ImpersonationLevel   impersonation level
 *             PHANDLE                      DuplicateTokenHandle handle of duplicated token
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API DuplicateToken(HANDLE                       ExistingTokenHandle,
                                SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
                                PHANDLE                      DuplicateTokenHandle)
{
  dprintf(("ADVAPI32: DuplicateToken(%08x,%08xh,%08xh) not implemented.\n",
           ExistingTokenHandle,
           ImpersonationLevel,
           DuplicateTokenHandle));

  return (FALSE); /* signal failure */
}




/*****************************************************************************
 * Name      : GetAclInformation
 * Purpose   : The GetAclInformation function retrieves information about an
 *             access-control list (ACL).
 * Parameters: PACL                  pAcl                  address of access-control list
 *             LPVOID                pAclInformation       address of ACL information
 *             DWORD                 nAclInformationLength size of ACL information
 *             ACL_INFORMATION_CLASS dwAclInformationClass class of requested information
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetAclInformation(PACL                  pAcl,
                                   LPVOID                pAclInformation,
                                   DWORD                 nAclInformationLength,
                                   ACL_INFORMATION_CLASS dwAclInformationClass)
{
  dprintf(("ADVAPI32: GetAclInformation(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           pAclInformation,
           nAclInformationLength,
           dwAclInformationClass));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetKernelObjectSecurity
 * Purpose   : The GetKernelObjectSecurity function retrieves a copy of the
 *             security descriptor protecting a kernel object.
 * Parameters: HANDLE                Handle                handle of object to query
 *             SECURITY_INFORMATION  RequestedInformation  requested information
 *             PSECURITY_DESCRIPTOR  pSecurityDescriptor   address of security descriptor
 *             DWORD                 nLength               size of buffer for security descriptor
 *             LPDWORD               lpnLengthNeeded      address of required size of buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetKernelObjectSecurity(HANDLE               Handle,
                                         SECURITY_INFORMATION RequestedInformation,
                                         PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                         DWORD                nLength,
                                         LPDWORD              lpnLengthNeeded)
{
  dprintf(("ADVAPI32: GetKernelObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           Handle,
           RequestedInformation,
           pSecurityDescriptor,
           nLength,
           lpnLengthNeeded));

  return (FALSE); /* signal failure */
}




/*****************************************************************************
 * Name      : GetPrivateObjectSecurity
 * Purpose   : The GetPrivateObjectSecurity retrieves information from a
 *             protected server object's security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR ObjectDescriptor    address of SD to query
 *             SECURITY_INFORMATION SecurityInformation requested information
 *             PSECURITY_DESCRIPTOR ResultantDescriptor address of retrieved SD
 *             DWORD                DescriptorLength    size of buffer for retrieved SD
 *             LPDWORD               ReturnLength        address of buffer size required for SD
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetPrivateObjectSecurity(PSECURITY_DESCRIPTOR ObjectDescriptor,
                                          SECURITY_INFORMATION SecurityInformation,
                                          PSECURITY_DESCRIPTOR ResultantDescriptor,
                                          DWORD                DescriptorLength,
                                          LPDWORD               ReturnLength)
{
  dprintf(("ADVAPI32: GetPrivateObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ObjectDescriptor,
           SecurityInformation,
           ResultantDescriptor,
           DescriptorLength,
           ReturnLength));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ImpersonateLoggedOnUser
 * Purpose   : The ImpersonateLoggedOnUser function lets the calling thread
 *             impersonate a user. The user is represented by a token handle
 *             obtained by calling the LogonUser function
 * Parameters: HANDLE hToken
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ImpersonateLoggedOnUser(HANDLE hToken)
{
  dprintf(("ADVAPI32: ImpersonateLoggedOnUser(%08xh) not implemented.\n",
           hToken));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : ImpersonateNamedPipeClient
 * Purpose   : The ImpersonateNamedPipeClient function impersonates a named-pipe
 *             client application.
 * Parameters: HANDLE  hNamedPipe  handle of a named pipe
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ImpersonateNamedPipeClient(HANDLE hNamedPipe)
{
  dprintf(("ADVAPI32: ImpersonateNamedPipeClient(%08xh) not implemented.\n",
           hNamedPipe));

  return (TRUE); /* signal OK */
}

/*****************************************************************************
 * Name      : InitiateSystemShutdownA
 * Purpose   : The InitiateSystemShutdown function initiates a shutdown and
 *             optional restart of the specified computer.
 * Parameters: LPTSTR lpMachineName        address of name of computer to shut down
 *             LPTSTR lpMessage            address of message to display in dialog box
 *             DWORD  dwTimeout            time to display dialog box
 *             BOOL   bForceAppsClosed     force applications with unsaved changes flag
 *             BOOL   bRebootAfterShutdown reboot flag
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API InitiateSystemShutdownA(LPTSTR lpMachineName,
                                         LPTSTR lpMessage,
                                         DWORD  dwTimeout,
                                         BOOL   bForceAppsClosed,
                                         BOOL   bRebootAfterShutdown)
{
  dprintf(("ADVAPI32: InitiateSystemShutdownA(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpMachineName,
           lpMessage,
           dwTimeout,
           bForceAppsClosed,
           bRebootAfterShutdown));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : InitiateSystemShutdownW
 * Purpose   : The InitiateSystemShutdown function initiates a shutdown and
 *             optional restart of the specified computer.
 * Parameters: LPWSTR lpMachineName        address of name of computer to shut down
 *             LPWSTR lpMessage            address of message to display in dialog box
 *             DWORD  dwTimeout            time to display dialog box
 *             BOOL   bForceAppsClosed     force applications with unsaved changes flag
 *             BOOL   bRebootAfterShutdown reboot flag
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API InitiateSystemShutdownW(LPWSTR lpMachineName,
                                         LPWSTR lpMessage,
                                         DWORD  dwTimeout,
                                         BOOL   bForceAppsClosed,
                                         BOOL   bRebootAfterShutdown)
{
  dprintf(("ADVAPI32: InitiateSystemShutdownW(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpMachineName,
           lpMessage,
           dwTimeout,
           bForceAppsClosed,
           bRebootAfterShutdown));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : IsTextUnicode
 * Purpose   : The IsTextUnicode function determines whether a buffer probably
 *             contains a form of Unicode text. The function uses various
 *             statistical and deterministic methods to make its determination,
 *             under the control of flags passed via lpi. When the function
 *             returns, the results of such tests are reported via lpi. If all
 *             specified tests are passed, the function returns TRUE; otherwise,
 *             it returns FALSE.
 * Parameters: CONST LPVOID lpBuffer pointer to an input buffer to be examined
 *             int          cb       the size in bytes of the input buffer
 *             LPINT        lpi      pointer to flags that condition text examination and receive results
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

DWORD WIN32API IsTextUnicode(CONST LPVOID  lpBuffer,
                                int           cb,
                                LPINT         lpi)
{
  DWORD dwResult = 0;

  dprintf(("ADVAPI32: IsTextUnicode(%08xh,%08xh,%08xh) not implemented.\n",
           lpBuffer,
           cb,
           lpi));

  if (cb & 0x0001) dwResult |= IS_TEXT_UNICODE_ODD_LENGTH;

  return (dwResult); /* signal failure */
}


/*****************************************************************************
 * Name      : IsValidAcl
 * Purpose   : The IsValidAcl function validates an access-control list (ACL).
 * Parameters: PACL  pAcl  address of access-control list
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API IsValidAcl(PACL pAcl)
{
  dprintf(("ADVAPI32: IsValidAcl(%08xh) not implemented.\n",
           pAcl));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : LogonUserA
 * Purpose   : The LogonUser function attempts to perform a user logon
 *             operation. You specify the user with a user name and domain,
 *             and authenticate the user with a clear-text password. If the
 *             function succeeds, you receive a handle to a token that
 *             represents the logged-on user. You can then use this token
 *             handle to impersonate the specified user, or to create a process
 *             running in the context of the specified user.
 * Parameters: LPTSTR  lpszUsername    string that specifies the user name
 *             LPTSTR  lpszDomain      string that specifies the domain or servero
 *             LPTSTR  lpszPassword    string that specifies the password
 *             DWORD   dwLogonType     specifies the type of logon operation
 *             DWORD   dwLogonProvider specifies the logon provider
 *             PHANDLE phToken         pointer to variable to receive token handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LogonUserA(LPTSTR  lpszUsername,
                            LPTSTR  lpszDomain,
                            LPTSTR  lpszPassword,
                            DWORD   dwLogonType,
                            DWORD   dwLogonProvider,
                            PHANDLE phToken)
{
  dprintf(("ADVAPI32: LogonUserA(%s,%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszUsername,
           lpszDomain,
           lpszPassword,
           dwLogonType,
           dwLogonProvider,
           phToken));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : LogonUserW
 * Purpose   : The LogonUser function attempts to perform a user logon
 *             operation. You specify the user with a user name and domain,
 *             and authenticate the user with a clear-text password. If the
 *             function succeeds, you receive a handle to a token that
 *             represents the logged-on user. You can then use this token
 *             handle to impersonate the specified user, or to create a process
 *             running in the context of the specified user.
 * Parameters: LPWSTR  lpszUsername    string that specifies the user name
 *             LPWSTR  lpszDomain      string that specifies the domain or servero
 *             LPWSTR  lpszPassword    string that specifies the password
 *             DWORD   dwLogonType     specifies the type of logon operation
 *             DWORD   dwLogonProvider specifies the logon provider
 *             PHANDLE phToken         pointer to variable to receive token handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LogonUserW(LPWSTR  lpszUsername,
                            LPWSTR  lpszDomain,
                            LPWSTR  lpszPassword,
                            DWORD   dwLogonType,
                            DWORD   dwLogonProvider,
                            PHANDLE phToken)
{
  dprintf(("ADVAPI32: LogonUserW(%s,%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszUsername,
           lpszDomain,
           lpszPassword,
           dwLogonType,
           dwLogonProvider,
           phToken));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : LookupAccountNameA
 * Purpose   : The LookupAccountName function accepts the name of a system and
 *             an account as input. It retrieves a security identifier (SID)
 *             for the account and the name of the domain on which the account was found.
 * Parameters: LPCSTR       lpSystemName           address of string for system name
 *             LPCSTR       lpAccountName          address of string for account name
 *             PSID          Sid                    address of security identifier
 *             LPDWORD       cbSid                  address of size of security identifier
 *             LPTSTR        ReferencedDomainName   address of string for referenced domain
 *             LPDWORD       cbReferencedDomainName address of size of domain string
 *             PSID_NAME_USE peUse                  address of SID-type indicator
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupAccountNameA(LPCSTR       lpSystemName,
                                    LPCSTR       lpAccountName,
                                    PSID          Sid,
                                    LPDWORD       cbSid,
                                    LPTSTR        ReferencedDomainName,
                                    LPDWORD       cbReferencedDomainName,
                                    PSID_NAME_USE peUse)
{
  dprintf(("ADVAPI32: LookupAccountNameA(%s,%s,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpAccountName,
           Sid,
           cbSid,
           ReferencedDomainName,
           cbReferencedDomainName,
           peUse));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupAccountNameW
 * Purpose   : The LookupAccountName function accepts the name of a system and
 *             an account as input. It retrieves a security identifier (SID)
 *             for the account and the name of the domain on which the account was found.
 * Parameters: LPCWSTR       lpSystemName           address of string for system name
 *             LPCWSTR       lpAccountName          address of string for account name
 *             PSID          Sid                    address of security identifier
 *             LPDWORD       cbSid                  address of size of security identifier
 *             LPWSTR        ReferencedDomainName   address of string for referenced domain
 *             LPDWORD       cbReferencedDomainName address of size of domain string
 *             PSID_NAME_USE peUse                  address of SID-type indicator
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupAccountNameW(LPCWSTR       lpSystemName,
                                    LPCWSTR       lpAccountName,
                                    PSID          Sid,
                                    LPDWORD       cbSid,
                                    LPWSTR        ReferencedDomainName,
                                    LPDWORD       cbReferencedDomainName,
                                    PSID_NAME_USE peUse)
{
  dprintf(("ADVAPI32: LookupAccountNameW(%s,%s,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpAccountName,
           Sid,
           cbSid,
           ReferencedDomainName,
           cbReferencedDomainName,
           peUse));

  return (FALSE); /* signal failure */
}



/*****************************************************************************
 * Name      : LookupPrivilegeDisplayNameA
 * Purpose   : The LookupPrivilegeDisplayName function retrieves a displayable
 *             name representing a specified privilege.
 * Parameters: LPCSTR lpSystemName   address of string specifying the system
 *             LPCSTR lpName         address of string specifying the privilege
 *             LPTSTR  lpDisplayName  address of string receiving the displayable name
 *             LPDWORD cbDisplayName  address of size of string for displayable name
 *             LPDWORD lpLanguageId   address of language identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeDisplayNameA(LPCSTR lpSystemName,
                                             LPCSTR lpName,
                                             LPTSTR  lpDisplayName,
                                             LPDWORD cbDisplayName,
                                             LPDWORD lpLanguageId)
{
  dprintf(("ADVAPI32: LookupPrivilegeDisplayNameA(%s,%s,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpName,
           lpDisplayName,
           cbDisplayName,
           lpLanguageId));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupPrivilegeDisplayNameW
 * Purpose   : The LookupPrivilegeDisplayName function retrieves a displayable
 *             name representing a specified privilege.
 * Parameters: LPCWSTR lpSystemName   address of string specifying the system
 *             LPCWSTR lpName         address of string specifying the privilege
 *             LPWSTR  lpDisplayName  address of string receiving the displayable name
 *             LPDWORD cbDisplayName  address of size of string for displayable name
 *             LPDWORD lpLanguageId   address of language identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeDisplayNameW(LPCWSTR lpSystemName,
                                             LPCWSTR lpName,
                                             LPWSTR  lpDisplayName,
                                             LPDWORD cbDisplayName,
                                             LPDWORD lpLanguageId)
{
  dprintf(("ADVAPI32: LookupPrivilegeDisplayNameW(%s,%s,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpName,
           lpDisplayName,
           cbDisplayName,
           lpLanguageId));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupPrivilegeNameA
 * Purpose   : The LookupPrivilegeName function retrieves the name corresponding
 *             to the privilege represented on a specific system by a specified
 *             locally unique identifier (LUID).
 * Parameters: LPCSTR lpSystemName address of string specifying the system
 *             PLUID   lpLuid       address of locally unique identifier
 *             LPTSTR  lpName       address of string specifying the privilege
 *             LPDWORD cbName       address of size of string for displayable name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeNameA(LPCSTR lpSystemName,
                                      PLUID   lpLuid,
                                      LPTSTR  lpName,
                                      LPDWORD cbName)
{
  dprintf(("ADVAPI32: LookupPrivilegeNameA(%s,%08xh,%s,%08xh) not implemented.\n",
           lpSystemName,
           lpLuid,
           lpName,
           cbName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupPrivilegeNameW
 * Purpose   : The LookupPrivilegeName function retrieves the name corresponding
 *             to the privilege represented on a specific system by a specified
 *             locally unique identifier (LUID).
 * Parameters: LPCWSTR lpSystemName address of string specifying the system
 *             PLUID   lpLuid       address of locally unique identifier
 *             LPWSTR  lpName       address of string specifying the privilege
 *             LPDWORD cbName       address of size of string for displayable name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeNameW(LPCWSTR lpSystemName,
                                      PLUID   lpLuid,
                                      LPWSTR  lpName,
                                      LPDWORD cbName)
{
  dprintf(("ADVAPI32: LookupPrivilegeNameW(%s,%08xh,%s,%08xh) not implemented.\n",
           lpSystemName,
           lpLuid,
           lpName,
           cbName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : MakeAbsoluteSD
 * Purpose   : The MakeAbsoluteSD function creates a security descriptor in
 *             absolute format by using a security descriptor in self-relative
 *             format as a template.
 * Parameters: PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor    address self-relative SD
 *             PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor        address of absolute SD
 *             LPDWORD              lpdwAbsoluteSecurityDescriptorSize address of size of absolute SD
 *             PACL                 pDacl                              address of discretionary ACL
 *             LPDWORD              lpdwDaclSize                       address of size of discretionary ACL
 *             PACL                 pSacl                              address of system ACL
 *             LPDWORD              lpdwSaclSize                       address of size of system ACL
 *             PSID                 pOwner                             address of owner SID
 *             LPDWORD              lpdwOwnerSize                      address of size of owner SID
 *             PSID                 pPrimaryGroup                      address of primary-group SID
 *             LPDWORD              lpdwPrimaryGroupSize               address of size of group SID
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API MakeAbsoluteSD(PSECURITY_DESCRIPTOR  pSelfRelativeSecurityDescriptor,
                                PSECURITY_DESCRIPTOR  pAbsoluteSecurityDescriptor,
                                LPDWORD               lpdwAbsoluteSecurityDescriptorSize,
                                PACL                  pDacl,
                                LPDWORD               lpdwDaclSize,
                                PACL                  pSacl,
                                LPDWORD               lpdwSaclSize,
                                PSID                  pOwner,
                                LPDWORD               lpdwOwnerSize,
                                PSID                  pPrimaryGroup,
                                LPDWORD               lpdwPrimaryGroupSize)
{
  dprintf(("ADVAPI32: MakeAbsoluteSD(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pSelfRelativeSecurityDescriptor,
           pAbsoluteSecurityDescriptor,
           lpdwAbsoluteSecurityDescriptorSize,
           pDacl,
           lpdwDaclSize,
           pSacl,
           lpdwSaclSize,
           pOwner,
           lpdwOwnerSize,
           pPrimaryGroup,
           lpdwPrimaryGroupSize));

  return (FALSE); /* signal failure */
}




/*****************************************************************************
 * Name      : MapGenericMask
 * Purpose   : The MapGenericMask function maps the generic access rights in
 *             an access mask to specific and standard access rights. The function
 *             applies a mapping supplied in a GENERIC_MAPPING structure.
 * Parameters: LPDWORD            AccessMask     address of access mask
 *             PGENERIC_MAPPING  GenericMapping address of GENERIC_MAPPING structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

VOID WIN32API MapGenericMask(LPDWORD           AccessMask,
                                PGENERIC_MAPPING GenericMapping)
{
  dprintf(("ADVAPI32: MapGenericMask(%08xh,%08xh) not implemented.\n",
           AccessMask,
           GenericMapping));
}





/*****************************************************************************
 * Name      : ObjectCloseAuditAlarmA
 * Purpose   : The ObjectCloseAuditAlarm function generates audit messages when
 *             a handle of an object is deleted. Alarms are not supported in the
 *             current version of Windows NT.
 * Parameters: LPCSTR SubsystemName   address of string for subsystem name
 *             LPVOID  HandleId        address of handle identifier
 *             BOOL    GenerateOnClose flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectCloseAuditAlarmA(LPCSTR SubsystemName,
                                        LPVOID  HandleId,
                                        BOOL    GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectCloseAuditAlarmA(%s,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectCloseAuditAlarmW
 * Purpose   : The ObjectCloseAuditAlarm function generates audit messages when
 *             a handle of an object is deleted. Alarms are not supported in the
 *             current version of Windows NT.
 * Parameters: LPCWSTR SubsystemName   address of string for subsystem name
 *             LPVOID  HandleId        address of handle identifier
 *             BOOL    GenerateOnClose flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectCloseAuditAlarmW(LPCWSTR SubsystemName,
                                        LPVOID  HandleId,
                                        BOOL    GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectCloseAuditAlarmW(%s,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}



/*****************************************************************************
 * Name      : ObjectOpenAuditAlarmA
 * Purpose   : The ObjectOpenAuditAlarm function generates audit messages when
 *             a client application attempts to gain access to an object or to
 *             create a new one. Alarms are not supported in the current version
 *             of Windows NT.
 * Parameters: LPCSTR              SubsystemName       address of string for subsystem name
 *             LPVOID               HandleId            address of handle identifier
 *             LPTSTR               ObjectTypeName      address of string for object type
 *             LPTSTR               ObjectName          address of string for object name
 *             PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             HANDLE               ClientToken         handle of client's access token
 *             DWORD                DesiredAccess       mask for desired access rights
 *             DWORD                GrantedAccess       mask for granted access rights
 *             PPRIVILEGE_SET       Privileges          address of privileges
 *             BOOL                 ObjectCreation      flag for object creation
 *             BOOL                 AccessGranted       flag for results
 *             LPBOOL               GenerateOnClose     address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectOpenAuditAlarmA(LPCSTR              SubsystemName,
                                       LPVOID               HandleId,
                                       LPTSTR               ObjectTypeName,
                                       LPTSTR               ObjectName,
                                       PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                       HANDLE               ClientToken,
                                       DWORD                DesiredAccess,
                                       DWORD                GrantedAccess,
                                       PPRIVILEGE_SET       Privileges,
                                       BOOL                 ObjectCreation,
                                       BOOL                 AccessGranted,
                                       LPBOOL               GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectOpenAuditAlarmA(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           pSecurityDescriptor,
           ClientToken,
           DesiredAccess,
           GrantedAccess,
           Privileges,
           ObjectCreation,
           AccessGranted,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectOpenAuditAlarmW
 * Purpose   : The ObjectOpenAuditAlarm function generates audit messages when
 *             a client application attempts to gain access to an object or to
 *             create a new one. Alarms are not supported in the current version
 *             of Windows NT.
 * Parameters: LPCWSTR              SubsystemName       address of string for subsystem name
 *             LPVOID               HandleId            address of handle identifier
 *             LPWSTR               ObjectTypeName      address of string for object type
 *             LPWSTR               ObjectName          address of string for object name
 *             PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             HANDLE               ClientToken         handle of client's access token
 *             DWORD                DesiredAccess       mask for desired access rights
 *             DWORD                GrantedAccess       mask for granted access rights
 *             PPRIVILEGE_SET       Privileges          address of privileges
 *             BOOL                 ObjectCreation      flag for object creation
 *             BOOL                 AccessGranted       flag for results
 *             LPBOOL               GenerateOnClose     address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectOpenAuditAlarmW(LPCWSTR              SubsystemName,
                                       LPVOID               HandleId,
                                       LPWSTR               ObjectTypeName,
                                       LPWSTR               ObjectName,
                                       PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                       HANDLE               ClientToken,
                                       DWORD                DesiredAccess,
                                       DWORD                GrantedAccess,
                                       PPRIVILEGE_SET       Privileges,
                                       BOOL                 ObjectCreation,
                                       BOOL                 AccessGranted,
                                       LPBOOL               GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectOpenAuditAlarmW(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           pSecurityDescriptor,
           ClientToken,
           DesiredAccess,
           GrantedAccess,
           Privileges,
           ObjectCreation,
           AccessGranted,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectPrivilegeAuditAlarmA
 * Purpose   : The ObjectPrivilegeAuditAlarm function generates audit messages
 *             as a result of a client's attempt to perform a privileged operation
 *             on a server application object using an already opened handle of
 *             that object. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR        lpszSubsystem   address of string for subsystem name
 *             LPVOID         lpvHandleId     address of handle identifier
 *             HANDLE         hClientToken    handle of client's access token
 *             DWORD          dwDesiredAccess mask for desired access rights
 *             PPRIVILEGE_SET pps             address of privileges
 *             BOOL           fAccessGranted  flag for results
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectPrivilegeAuditAlarmA(LPCSTR        lpszSubsystem,
                                            LPVOID         lpvHandleId,
                                            HANDLE         hClientToken,
                                            DWORD          dwDesiredAccess,
                                            PPRIVILEGE_SET pps,
                                            BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: ObjectPrivilegeAuditAlarmA(%s,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpvHandleId,
           hClientToken,
           dwDesiredAccess,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectPrivilegeAuditAlarmW
 * Purpose   : The ObjectPrivilegeAuditAlarm function generates audit messages
 *             as a result of a client's attempt to perform a privileged operation
 *             on a server application object using an already opened handle of
 *             that object. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCWSTR        lpszSubsystem   address of string for subsystem name
 *             LPVOID         lpvHandleId     address of handle identifier
 *             HANDLE         hClientToken    handle of client's access token
 *             DWORD          dwDesiredAccess mask for desired access rights
 *             PPRIVILEGE_SET pps             address of privileges
 *             BOOL           fAccessGranted  flag for results
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectPrivilegeAuditAlarmW(LPCWSTR        lpszSubsystem,
                                            LPVOID         lpvHandleId,
                                            HANDLE         hClientToken,
                                            DWORD          dwDesiredAccess,
                                            PPRIVILEGE_SET pps,
                                            BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: ObjectPrivilegeAuditAlarmW(%s,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpvHandleId,
           hClientToken,
           dwDesiredAccess,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : PrivilegeCheck
 * Purpose   : The PrivilegeCheck function tests the security context represented
 *             by a specific access token to discover whether it contains the
 *             specified privileges. This function is typically called by a server
 *             application to check the privileges of a client's access token.
 * Parameters: HANDLE          hClientToken handle of client's access token
 *             PPRIVILEGE_SET  pps          address of privileges
 *             LPBOOL          lpfResult    address of flag for result
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API PrivilegeCheck(HANDLE         hClientToken,
                                PPRIVILEGE_SET pps,
                                LPBOOL         lpfResult)
{
  dprintf(("ADVAPI32: PrivilegeCheck(%08xh,%08xh,%08xh) not implemented.\n",
           hClientToken,
           pps,
           lpfResult));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : PrivilegedServiceAuditAlarmA
 * Purpose   : The PrivilegedServiceAuditAlarm function generates audit messages
 *             when an attempt is made to perform privileged system service
 *             operations. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR        lpszSubsystem  address of string for subsystem name
 *             LPCSTR        lpszService    address of string for service name
 *             HANDLE         hClientToken   handle of access token
 *             PPRIVILEGE_SET pps            address of privileges
 *             BOOL           fAccessGranted flag for granted access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API PrivilegedServiceAuditAlarmA(LPCSTR        lpszSubsystem,
                                              LPCSTR        lpszService,
                                              HANDLE         hClientToken,
                                              PPRIVILEGE_SET pps,
                                              BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: PrivilegedServiceAuditAlarmA(%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpszService,
           hClientToken,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : PrivilegedServiceAuditAlarmW
 * Purpose   : The PrivilegedServiceAuditAlarm function generates audit messages
 *             when an attempt is made to perform privileged system service
 *             operations. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCWSTR        lpszSubsystem  address of string for subsystem name
 *             LPCWSTR        lpszService    address of string for service name
 *             HANDLE         hClientToken   handle of access token
 *             PPRIVILEGE_SET pps            address of privileges
 *             BOOL           fAccessGranted flag for granted access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API PrivilegedServiceAuditAlarmW(LPCWSTR        lpszSubsystem,
                                              LPCWSTR        lpszService,
                                              HANDLE         hClientToken,
                                              PPRIVILEGE_SET pps,
                                              BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: PrivilegedServiceAuditAlarmW(%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpszService,
           hClientToken,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : SetAclInformation
 * Purpose   : The SetAclInformation function sets information about an access-control list (ACL).
 * Parameters: PACL                  pAcl       address of access-control list
 *             LPVOID                lpvAclInfo address of ACL information
 *             DWORD                 cbAclInfo  size of ACL information
 *             ACL_INFORMATION_CLASS aclic      specifies class of requested info
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetAclInformation(PACL                  pAcl,
                                   LPVOID                lpvAclInfo,
                                   DWORD                 cbAclInfo,
                                   ACL_INFORMATION_CLASS aclic)
{
  dprintf(("ADVAPI32: SetAclInformation(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           lpvAclInfo,
           cbAclInfo,
           aclic));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetKernelObjectSecurity
 * Purpose   : The SetKernelObjectSecurity function sets the security of a kernel
 *             object. For example, this can be a process, thread, or event.
 * Parameters: HANDLE               hObject handle of object
 *             SECURITY_INFORMATION si      type of information to set
 *             PSECURITY_DESCRIPTOR psd     address of security descriptor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetKernelObjectSecurity(HANDLE               hObject,
                                         SECURITY_INFORMATION si,
                                         PSECURITY_DESCRIPTOR psd)
{
  dprintf(("ADVAPI32: SetKernelObjectSecurity(%08xh,%08xh,%08xh) not implemented.\n",
           hObject,
           si,
           psd));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetPrivateObjectSecurity
 * Purpose   : The SetPrivateObjectSecurity function modifies a private
 *             object's security descriptor.
 * Parameters: SECURITY_INFORMATION  si           type of security information
 *             PSECURITY_DESCRIPTOR  psdSource    address of SD to apply to object
 *             PSECURITY_DESCRIPTOR  *lppsdTarget address of object's SD
 *             PGENERIC_MAPPING      pgm          address of access-mapping structure
 *             HANDLE                hClientToken handle of client access token
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/


BOOL WIN32API    SetPrivateObjectSecurity(SECURITY_INFORMATION  si,
                                          PSECURITY_DESCRIPTOR  psdSource,
                                          PSECURITY_DESCRIPTOR  *lppsdTarget,
                                          PGENERIC_MAPPING      pgm,
                                          HANDLE                hClientToken)
{
  dprintf(("ADVAPI32: SetPrivateObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           si,
           psdSource,
           lppsdTarget,
           pgm,
           hClientToken));

  return (FALSE); /* signal failure */
}






