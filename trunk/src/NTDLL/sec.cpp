/* $Id: sec.cpp,v 1.8 2001-04-22 10:38:59 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * Security functions
 *
 * Copyright 1996-1998 Marcus Meissner
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#include <os2win.h>
#include "ntdll.h"


ODINDEBUGCHANNEL(NTDLL-SEC)

/*
 * SID FUNCTIONS
 */

/******************************************************************************
 *  RtlAllocateAndInitializeSid             [NTDLL.265]
 *
 */
BOOLEAN WINAPI RtlAllocateAndInitializeSid ( PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
					     BYTE nSubAuthorityCount,
					     DWORD nSubAuthority0,
					     DWORD nSubAuthority1,
					     DWORD nSubAuthority2,
					     DWORD nSubAuthority3,
					     DWORD nSubAuthority4,
					     DWORD nSubAuthority5,
					     DWORD nSubAuthority6,
					     DWORD nSubAuthority7,
					     PSID *pSid)
{
  dprintf(("NTDLL: RtlAllocateAndInitializeSid(%08xh,%08xh,%08xh,"
           "%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)",
           pIdentifierAuthority,
           nSubAuthorityCount,
           nSubAuthority0,
           nSubAuthority1,
           nSubAuthority2,
           nSubAuthority3,
           nSubAuthority4,
           nSubAuthority5,
           nSubAuthority6,
           nSubAuthority7,
           pSid));

  *pSid = (PSID)Heap_Alloc(sizeof(SID)+nSubAuthorityCount*sizeof(DWORD));
  if(*pSid == NULL) {
	SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	return FALSE;
  }
  (*pSid)->Revision          = SID_REVISION;
  (*pSid)->SubAuthorityCount = nSubAuthorityCount;

  if (nSubAuthorityCount > 0)
        (*pSid)->SubAuthority[0] = nSubAuthority0;
  if (nSubAuthorityCount > 1)
        (*pSid)->SubAuthority[1] = nSubAuthority1;
  if (nSubAuthorityCount > 2)
        (*pSid)->SubAuthority[2] = nSubAuthority2;
  if (nSubAuthorityCount > 3)
        (*pSid)->SubAuthority[3] = nSubAuthority3;
  if (nSubAuthorityCount > 4)
        (*pSid)->SubAuthority[4] = nSubAuthority4;
  if (nSubAuthorityCount > 5)
        (*pSid)->SubAuthority[5] = nSubAuthority5;
  if (nSubAuthorityCount > 6)
        (*pSid)->SubAuthority[6] = nSubAuthority6;
  if (nSubAuthorityCount > 7)
        (*pSid)->SubAuthority[7] = nSubAuthority7;

  if(pIdentifierAuthority) 
  	memcpy((PVOID)&(*pSid)->IdentifierAuthority, (PVOID)pIdentifierAuthority, sizeof(SID_IDENTIFIER_AUTHORITY));
  return TRUE;
}


/******************************************************************************
 *  RtlEqualSid                          [NTDLL.352]
 *
 */
BOOL WINAPI RtlEqualSid(PSID pSid1, PSID pSid2)
{
   dprintf(("NTDLL: RtlEqualSid(%08x, %08x)",
            pSid1,
            pSid2));

   if (!RtlValidSid(pSid1) || !RtlValidSid(pSid2))
        return FALSE;

   if (*RtlSubAuthorityCountSid(pSid1) != *RtlSubAuthorityCountSid(pSid2))
        return FALSE;

   if (memcmp(pSid1, pSid2, RtlLengthSid(pSid1)) != 0)
        return FALSE;

   return TRUE;
}


/******************************************************************************
 * RtlEqualPrefixSid [NTDLL.351]
 */
BOOL WINAPI RtlEqualPrefixSid (PSID pSid1, PSID pSid2) 
{
    if (!RtlValidSid(pSid1) || !RtlValidSid(pSid2))
        return FALSE;

    if (*RtlSubAuthorityCountSid(pSid1) != *RtlSubAuthorityCountSid(pSid2))
        return FALSE;

    if (memcmp(pSid1, pSid2, RtlLengthRequiredSid(pSid1->SubAuthorityCount - 1)) != 0)
        return FALSE;

    return TRUE;
}

/******************************************************************************
 *  RtlFreeSid    [NTDLL.376]
 */
VOID* WINAPI RtlFreeSid(PSID pSid)
{
  dprintf(("NTDLL: RtlFreeSid(%08xh)", pSid));

  Heap_Free(pSid);
  return NULL;
}


/**************************************************************************
 *                 RtlLengthRequiredSid        [NTDLL.427]
 */
DWORD WINAPI RtlLengthRequiredSid(DWORD nrofsubauths)
{
  dprintf (("NTDLL: RtlLengthRequiredSid(%08xh)\n",
            nrofsubauths));

  return sizeof(DWORD)*nrofsubauths+sizeof(SID);
}


/**************************************************************************
 *                 RtlLengthSid                   [NTDLL.429]
 */
DWORD WINAPI RtlLengthSid(PSID sid)
{
  dprintf(("NTDLL: RtlLengthSid(%08xh)\n",
           sid));

  if (!sid)
    return FALSE;

  return sizeof(DWORD)*sid->SubAuthorityCount+sizeof(SID);
}


/**************************************************************************
 *                 RtlInitializeSid            [NTDLL.410]
 */
ODINFUNCTION3(DWORD, RtlInitializeSid,
              PSID, psid,
              PSID_IDENTIFIER_AUTHORITY, psidauth,
              DWORD, c)
{
  BYTE  a = c & 0xff;

  if (a>=SID_MAX_SUB_AUTHORITIES)
    return a;

  psid->SubAuthorityCount = a;
  psid->Revision          = SID_REVISION;
  memcpy(&(psid->IdentifierAuthority),
         psidauth,
         sizeof(SID_IDENTIFIER_AUTHORITY));

  return STATUS_SUCCESS;
}


/******************************************************************************
 * RtlIdentifierAuthoritySid [NTDLL.395]
 *
 * PARAMS
 *   pSid []
 */
PSID_IDENTIFIER_AUTHORITY WINAPI RtlIdentifierAuthoritySid( PSID pSid )
{
    return &pSid->IdentifierAuthority;
}

/**************************************************************************
 *                 RtlSubAuthoritySid          [NTDLL.497]
 */
ODINFUNCTION2(LPDWORD, RtlSubAuthoritySid,
              PSID, psid,
              DWORD, nr)
{
  return &(psid->SubAuthority[nr]);
}


/**************************************************************************
 *                 RtlSubAuthorityCountSid     [NTDLL.496]
 */

LPBYTE WINAPI RtlSubAuthorityCountSid(PSID psid)
{
  dprintf2(("NTDLL: RtlSubAUthorityCountSid(%08xh)\n",
            psid));

  return ((LPBYTE)psid)+1;
}


/**************************************************************************
 *                 RtlCopySid                     [NTDLL.302]
 */
DWORD WINAPI RtlCopySid(DWORD nDestinationSidLength,
                        PSID  pDestinationSid,
                        PSID  pSourceSid)
{
  dprintf(("NTDLL: RtlCopySid(%08xh,%08xh,%08xh)\n",
           nDestinationSidLength,
           pDestinationSid,
           pSourceSid));

  if (!RtlValidSid(pSourceSid))
        return STATUS_INVALID_PARAMETER;

  if (nDestinationSidLength < RtlLengthSid(pSourceSid))
        return STATUS_BUFFER_TOO_SMALL;

  memcpy(pDestinationSid, pSourceSid, RtlLengthSid(pSourceSid));

  return STATUS_SUCCESS;
}


/******************************************************************************
 * RtlValidSid [NTDLL.523]
 *
 * PARAMS
 *   pSid []
 */
BOOL WINAPI RtlValidSid( PSID pSid )
{
    if (IsBadReadPtr(pSid, 4))
    {
        return FALSE;
    }

    if (pSid->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES)
        return FALSE;

    if (!pSid || pSid->Revision != SID_REVISION)
        return FALSE;

    return TRUE;
}

/*
 * security descriptor functions
 */

/**************************************************************************
 * RtlCreateSecurityDescriptor                 [NTDLL.313]
 *
 * RETURNS:
 *  0 success,
 *  STATUS_INVALID_OWNER, STATUS_PRIVILEGE_NOT_HELD, STATUS_NO_INHERITANCE,
 *  STATUS_NO_MEMORY
 */
NTSTATUS WINAPI RtlCreateSecurityDescriptor(PSECURITY_DESCRIPTOR lpsd,
                                            DWORD                rev)
{
  dprintf(("NTDLL: RtlCreateSecurityDescriptor(%08xh,%08xh)\n",
           lpsd,
           rev));

  if (rev!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;

  memset(lpsd,
         '\0',
         sizeof(*lpsd));

  lpsd->Revision = SECURITY_DESCRIPTOR_REVISION;

  return STATUS_SUCCESS;
}


/**************************************************************************
 * RtlValidSecurityDescriptor                  [NTDLL.313]
 *
 */
NTSTATUS WINAPI RtlValidSecurityDescriptor(PSECURITY_DESCRIPTOR SecurityDescriptor)
{
  dprintf(("NTDLL: RtlValidSecurityDescriptor(%08xh)\n",
           SecurityDescriptor));

  if ( ! SecurityDescriptor )
    return STATUS_INVALID_SECURITY_DESCR;

  if ( SecurityDescriptor->Revision != SECURITY_DESCRIPTOR_REVISION )
    return STATUS_UNKNOWN_REVISION;

  return STATUS_SUCCESS;
}


/**************************************************************************
 *  RtlLengthSecurityDescriptor                [NTDLL]
 */
ULONG WINAPI RtlLengthSecurityDescriptor(PSECURITY_DESCRIPTOR SecurityDescriptor)
{
  ULONG Size;

  dprintf(("NTDLL: RtlLengthSecurityDescriptor(%08xh)\n",
           SecurityDescriptor));

  Size = SECURITY_DESCRIPTOR_MIN_LENGTH;
  if ( SecurityDescriptor == NULL )
    return 0;

  if ( SecurityDescriptor->Owner != NULL )
    Size += SecurityDescriptor->Owner->SubAuthorityCount;
  if ( SecurityDescriptor->Group != NULL )
    Size += SecurityDescriptor->Group->SubAuthorityCount;


  if ( SecurityDescriptor->Sacl != NULL )
    Size += SecurityDescriptor->Sacl->AclSize;
  if ( SecurityDescriptor->Dacl != NULL )
    Size += SecurityDescriptor->Dacl->AclSize;

  return Size;
}

/******************************************************************************
 *  RtlGetDaclSecurityDescriptor            [NTDLL]
 *
 */
NTSTATUS WINAPI RtlGetDaclSecurityDescriptor(PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                             PBOOLEAN             lpbDaclPresent,
                                             PACL                 *pDacl,
                                             PBOOLEAN             lpbDaclDefaulted)
{
  dprintf(("NTDLL: RtlGetDaclSecurityDescriptor(%08xh,%08xh,%08xh,%08xh)\n",
           pSecurityDescriptor,
           lpbDaclPresent,
           pDacl,
           lpbDaclDefaulted));

  if (pSecurityDescriptor->Revision != SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION ;

  *lpbDaclPresent = (SE_DACL_PRESENT & pSecurityDescriptor->Control);
  if (*lpbDaclPresent ? 1 : 0)
  {
    if ( SE_SELF_RELATIVE & pSecurityDescriptor->Control)
    {
      *pDacl = (PACL) ((LPBYTE)pSecurityDescriptor + (DWORD)pSecurityDescriptor->Dacl);
    }
    else
    {
      *pDacl = pSecurityDescriptor->Dacl;
    }
  }

  *lpbDaclDefaulted = (( SE_DACL_DEFAULTED & pSecurityDescriptor->Control ) ? 1 : 0);

  return STATUS_SUCCESS;
}


/**************************************************************************
 *  RtlSetDaclSecurityDescriptor            [NTDLL.483]
 */
NTSTATUS WINAPI RtlSetDaclSecurityDescriptor (PSECURITY_DESCRIPTOR lpsd,
                                              BOOLEAN              daclpresent,
                                              PACL                 dacl,
                                              BOOLEAN              dacldefaulted)
{
  dprintf(("NTDLL: RtlSetDaclSecurityDescriptor(%08xh,%08xh,%08xh,%08xh)\n",
           lpsd,
           daclpresent,
           dacl,
           dacldefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;
  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  if (!daclpresent)
  {
    lpsd->Control &= ~SE_DACL_PRESENT;
    return TRUE;
  }

  lpsd->Control |= SE_DACL_PRESENT;
  lpsd->Dacl = dacl;

  if (dacldefaulted)
    lpsd->Control |= SE_DACL_DEFAULTED;
  else
    lpsd->Control &= ~SE_DACL_DEFAULTED;

  return STATUS_SUCCESS;
}


/******************************************************************************
 *  RtlGetSaclSecurityDescriptor            [NTDLL]
 *
 */
NTSTATUS WINAPI RtlGetSaclSecurityDescriptor(PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                             PBOOLEAN             lpbSaclPresent,
                                             PACL                 *pSacl,
                                             PBOOLEAN             lpbSaclDefaulted)
{
  dprintf(("NTDLL: RtlGetSaclSecurityDescriptor(%08xh,%08xh,%08xh,%08xh)\n",
           pSecurityDescriptor,
           lpbSaclPresent,
           pSacl,
           lpbSaclDefaulted));

  if (pSecurityDescriptor->Revision != SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION ;

  *lpbSaclPresent = (SE_SACL_PRESENT & pSecurityDescriptor->Control);
  if (*lpbSaclPresent ? 1 : 0)
  {
    if ( SE_SELF_RELATIVE & pSecurityDescriptor->Control)
    {
      *pSacl = (PACL) ((LPBYTE)pSecurityDescriptor + (DWORD)pSecurityDescriptor->Sacl);
    }
    else
    {
      *pSacl = pSecurityDescriptor->Sacl;
    }
  }

  *lpbSaclDefaulted = (( SE_SACL_DEFAULTED & pSecurityDescriptor->Control ) ? 1 : 0);

  return STATUS_SUCCESS;
}


/**************************************************************************
 * RtlSetSaclSecurityDescriptor                [NTDLL.488]
 */
NTSTATUS WINAPI RtlSetSaclSecurityDescriptor (PSECURITY_DESCRIPTOR lpsd,
                                              BOOLEAN              saclpresent,
                                              PACL                 sacl,
                                              BOOLEAN              sacldefaulted)
{
  dprintf(("NTDLL: RtlSetSaclSecurityDescriptor(%08xh,%08xh,%08xh,%08xh)\n",
           lpsd,
           saclpresent,
           sacl,
           sacldefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;

  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  if (!saclpresent)
  {
    lpsd->Control &= ~SE_SACL_PRESENT;
    return 0;
  }

  lpsd->Control |= SE_SACL_PRESENT;
  lpsd->Sacl = sacl;

  if (sacldefaulted)
    lpsd->Control |= SE_SACL_DEFAULTED;
  else
    lpsd->Control &= ~SE_SACL_DEFAULTED;

  return STATUS_SUCCESS;
}


/**************************************************************************
 * RtlGetOwnerSecurityDescriptor            [NTDLL.488]
 */
NTSTATUS WINAPI RtlGetOwnerSecurityDescriptor(PSECURITY_DESCRIPTOR SecurityDescriptor,
                                              PSID                 *Owner,
                                              PBOOLEAN             OwnerDefaulted)
{
  dprintf(("NTDLL: RtlGetOwnerSecurityDescriptor(%08xh,%08xh,%08xh)\n",
           SecurityDescriptor,
           Owner,
           OwnerDefaulted));

  if ( !SecurityDescriptor  || !Owner || !OwnerDefaulted )
    return STATUS_INVALID_PARAMETER;

  *Owner = SecurityDescriptor->Owner;
  if ( *Owner != NULL )
  {
    if ( SecurityDescriptor->Control & SE_OWNER_DEFAULTED )
       *OwnerDefaulted = TRUE;
    else
       *OwnerDefaulted = FALSE;
  }

  return STATUS_SUCCESS;
}


/**************************************************************************
 *                 RtlSetOwnerSecurityDescriptor     [NTDLL.487]
 */
NTSTATUS WINAPI RtlSetOwnerSecurityDescriptor(PSECURITY_DESCRIPTOR lpsd,
                                              PSID                 owner,
                                              BOOLEAN              ownerdefaulted)
{
  dprintf(("NTDLL: RtlSetOwnerSecurityDescriptor(%08x,%08xh,%08xh)\n",
           lpsd,
           owner,
           ownerdefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;
  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  lpsd->Owner = owner;
  if (ownerdefaulted)
    lpsd->Control |= SE_OWNER_DEFAULTED;
  else
    lpsd->Control &= ~SE_OWNER_DEFAULTED;

  return STATUS_SUCCESS;
}


/**************************************************************************
 *                 RtlSetGroupSecurityDescriptor     [NTDLL.485]
 */
NTSTATUS WINAPI RtlSetGroupSecurityDescriptor (PSECURITY_DESCRIPTOR lpsd,
                                               PSID                 group,
                                               BOOLEAN              groupdefaulted)
{
  dprintf(("NTDLL: RtlSetGroupSecurityDescriptor(%08xh,%08xh,%08xh)\n",
           lpsd,
           group,
           groupdefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;
  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  lpsd->Group = group;
  if (groupdefaulted)
    lpsd->Control |= SE_GROUP_DEFAULTED;
  else
    lpsd->Control &= ~SE_GROUP_DEFAULTED;

  return STATUS_SUCCESS;
}


/**************************************************************************
 *                 RtlGetGroupSecurityDescriptor     [NTDLL]
 */
NTSTATUS WINAPI RtlGetGroupSecurityDescriptor(PSECURITY_DESCRIPTOR SecurityDescriptor,
                                              PSID                 *Group,
                                              PBOOLEAN             GroupDefaulted)
{
  dprintf(("NTDLL: RtlGetGroupSecurityDescriptor(%08xh,%08xh,%08xh)\n",
           SecurityDescriptor,
           Group,
           GroupDefaulted));

  if ( !SecurityDescriptor || !Group || !GroupDefaulted )
    return STATUS_INVALID_PARAMETER;

  *Group = SecurityDescriptor->Group;
  if ( *Group != NULL )
  {
    if ( SecurityDescriptor->Control & SE_GROUP_DEFAULTED )
      *GroupDefaulted = TRUE;
    else
      *GroupDefaulted = FALSE;
  }

  return STATUS_SUCCESS;
}


/*
 * access control list's
 */

/**************************************************************************
 *                 RtlCreateAcl                   [NTDLL.306]
 *
 * NOTES
 *    This should return NTSTATUS
 */
DWORD WINAPI RtlCreateAcl(PACL  acl,
                          DWORD size,
                          DWORD rev)
{
  dprintf(("NTDLL: RtlCreateAcl(%08xh,%08xh,%08xh)\n",
           acl,
           size,
           rev));

  if (rev!=ACL_REVISION)
    return STATUS_INVALID_PARAMETER;
  if (size<sizeof(ACL))
    return STATUS_BUFFER_TOO_SMALL;
  if (size>0xFFFF)
    return STATUS_INVALID_PARAMETER;

  memset(acl,'\0',sizeof(ACL));
  acl->AclRevision                      = rev;
  acl->AclSize                          = size;
  acl->AceCount                         = 0;

  return 0;
}


/**************************************************************************
 *                 RtlFirstFreeAce             [NTDLL.370]
 * looks for the AceCount+1 ACE, and if it is still within the alloced
 * ACL, return a pointer to it
 */
BOOLEAN WINAPI RtlFirstFreeAce(PACL        acl,
                               PACE_HEADER *x)
{
  PACE_HEADER ace;
  int      i;

  dprintf(("NTDLL: RtlFirstFreeAct(%08xh,%08xh)\n",
           acl,
           x));

  *x = 0;
  ace = (PACE_HEADER)(acl+1);
  for (i=0;
       i<acl->AceCount;
       i++)
  {
    if ((DWORD)ace>=(((DWORD)acl)+acl->AclSize))
       return 0;

    ace = (PACE_HEADER)(((BYTE*)ace)+ace->AceSize);
  }

  if ((DWORD)ace>=(((DWORD)acl)+acl->AclSize))
     return 0;

  *x = ace;
  return 1;
}


/**************************************************************************
 *                 RtlAddAce                      [NTDLL.260]
 */
NTSTATUS WINAPI RtlAddAce(PACL acl,
                          DWORD rev,
                          DWORD xnrofaces,
                          PACE_HEADER acestart,
                          DWORD acelen)
{
  PACE_HEADER ace,targetace;
  int      nrofaces;

  dprintf(("NTDLL: RtlAddAce(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           acl,
           rev,
           xnrofaces,
           acestart,
           acelen));

  if (acl->AclRevision != ACL_REVISION)
    return STATUS_INVALID_PARAMETER;

  if (!RtlFirstFreeAce(acl,&targetace))
    return STATUS_INVALID_PARAMETER;

  nrofaces=0;
  ace=acestart;

  while (((DWORD)ace-(DWORD)acestart)<acelen)
  {
    nrofaces++;
    ace = (PACE_HEADER)(((BYTE*)ace)+ace->AceSize);
  }

  if ((DWORD)targetace+acelen>(DWORD)acl+acl->AclSize) /* too much aces */
    return STATUS_INVALID_PARAMETER;

  memcpy((LPBYTE)targetace,acestart,acelen);
  acl->AceCount+=nrofaces;

  return STATUS_SUCCESS;
}


/******************************************************************************
 *  RtlAddAccessAllowedAce                  [NTDLL]
 */
NTSTATUS WINAPI RtlAddAccessAllowedAce(IN OUT PACL pAcl, IN DWORD dwAceRevision, 
                                       IN DWORD AccessMask, IN PSID pSid)
{
  dprintf(("NTDLL: RtlAddAccessAllowedAce(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceRevision,
           AccessMask,
           pSid));

  return 0;
}


/******************************************************************************
 *  RtlGetAce     [NTDLL]
 */
NTSTATUS WINAPI RtlGetAce(PACL  pAcl,
                          DWORD dwAceIndex,
                          LPVOID *pAce )
{
  PACE_HEADER ace;
  int      i;

  dprintf(("NTDLL: RtlGetAce(%08x,%08x,%08x) test implementation",
           pAcl,
           dwAceIndex,
           pAce));


  *pAce = 0;
  if(dwAceIndex > pAcl->AceCount) {
      dprintf(("index out of range"));
      return STATUS_INVALID_PARAMETER;
  }
  ace = (PACE_HEADER)(pAcl+1);

  for (i=0;
       i<dwAceIndex;
       i++)
  {
    if ((DWORD)ace>=(((DWORD)pAcl)+pAcl->AclSize))
       return STATUS_BUFFER_OVERFLOW;

    ace = (PACE_HEADER)(((BYTE*)ace)+ace->AceSize);
  }

  if ((DWORD)ace>=(((DWORD)pAcl)+pAcl->AclSize))
     return STATUS_BUFFER_OVERFLOW;

  *pAce = ace;
  return STATUS_SUCCESS;
}


/*
 * misc
 */

/******************************************************************************
 *  RtlAdjustPrivilege                      [NTDLL]
 */
DWORD WINAPI RtlAdjustPrivilege(DWORD x1,
                                DWORD x2,
                                DWORD x3,
                                DWORD x4)
{
  dprintf(("NTDLL: RtlAdjustPrivilege(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}

