/* $Id: om.cpp,v 1.3 2000-02-03 21:39:12 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * Object management functions
 */

#include <stdlib.h>
#include <string.h>

#include "ntdll.h"

/* move to somewhere */
typedef void * POBJDIR_INFORMATION;

/*
 * Generic object functions
 */

/******************************************************************************
 * NtQueryObject [NTDLL.161]
 */
NTSTATUS WINAPI NtQueryObject(HANDLE                   ObjectHandle,
                              OBJECT_INFORMATION_CLASS ObjectInformationClass,
                              PVOID                    ObjectInformation,
                              ULONG                    Length,
                              PULONG                   ResultLength)
{
  dprintf(("NTDLL: NtQueryObject(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ObjectHandle,
           ObjectInformationClass,
           ObjectInformation,
           Length,
           ResultLength));

  return 0;
}


/******************************************************************************
 *  NtQuerySecurityObject                [NTDLL]
 */
NTSTATUS WINAPI NtQuerySecurityObject(
	IN HANDLE Object,
	IN SECURITY_INFORMATION RequestedInformation,
	OUT PSECURITY_DESCRIPTOR pSecurityDesriptor,
	IN ULONG Length,
	OUT PULONG ResultLength)
{
	static SID_IDENTIFIER_AUTHORITY localSidAuthority = {SECURITY_NT_AUTHORITY};
	static SID_IDENTIFIER_AUTHORITY worldSidAuthority = {SECURITY_WORLD_SID_AUTHORITY};
	BYTE Buffer[256];
	PISECURITY_DESCRIPTOR_RELATIVE psd = (PISECURITY_DESCRIPTOR_RELATIVE)Buffer;
	UINT BufferIndex = sizeof(SECURITY_DESCRIPTOR_RELATIVE);
	
	dprintf(("(0x%08x,0x%08lx,%p,0x%08lx,%p) stub!\n",
	Object, RequestedInformation, pSecurityDesriptor, Length, ResultLength));

	RequestedInformation &= 0x0000000f;

	if (RequestedInformation & SACL_SECURITY_INFORMATION) return STATUS_ACCESS_DENIED;

	ZeroMemory(Buffer, 256);
	RtlCreateSecurityDescriptor((PSECURITY_DESCRIPTOR)psd, SECURITY_DESCRIPTOR_REVISION);
	psd->Control = SE_SELF_RELATIVE | 
	  ((RequestedInformation & DACL_SECURITY_INFORMATION) ? SE_DACL_PRESENT:0);

	/* owner: administrator S-1-5-20-220*/
	if (OWNER_SECURITY_INFORMATION & RequestedInformation)
	{
	  PSID psid = (PSID)&(Buffer[BufferIndex]);

	  psd->Owner = BufferIndex;
	  BufferIndex += RtlLengthRequiredSid(2);

	  psid->Revision = SID_REVISION;
	  psid->SubAuthorityCount = 2;
	  psid->IdentifierAuthority = localSidAuthority;
	  psid->SubAuthority[0] = SECURITY_BUILTIN_DOMAIN_RID;
	  psid->SubAuthority[1] = DOMAIN_ALIAS_RID_ADMINS;
	}
	
	/* group: built in domain S-1-5-12 */
	if (GROUP_SECURITY_INFORMATION & RequestedInformation)
	{
	  PSID psid = (PSID) &(Buffer[BufferIndex]);

	  psd->Group = BufferIndex;
	  BufferIndex += RtlLengthRequiredSid(1);

	  psid->Revision = SID_REVISION;
	  psid->SubAuthorityCount = 1;
	  psid->IdentifierAuthority = localSidAuthority;
	  psid->SubAuthority[0] = SECURITY_LOCAL_SYSTEM_RID;
	}

	/* discretionary ACL */
	if (DACL_SECURITY_INFORMATION & RequestedInformation)
	{
	  /* acl header */
	  PACL pacl = (PACL)&(Buffer[BufferIndex]);
	  PACCESS_ALLOWED_ACE pace;
	  PSID psid;
	  	  	  
	  psd->Dacl = BufferIndex;

	  pacl->AclRevision = MIN_ACL_REVISION;
	  pacl->AceCount = 3;
	  pacl->AclSize = BufferIndex; /* storing the start index temporary */

	  BufferIndex += sizeof(ACL);
	  
	  /* ACE System - full access */
	  pace = (PACCESS_ALLOWED_ACE)&(Buffer[BufferIndex]);
	  BufferIndex += sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD);

	  pace->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
	  pace->Header.AceFlags = CONTAINER_INHERIT_ACE;
	  pace->Header.AceSize = sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD) + RtlLengthRequiredSid(1);
	  pace->Mask = DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER  | 0x3f;
	  pace->SidStart = BufferIndex;

	  /* SID S-1-5-12 (System) */
	  psid = (PSID)&(Buffer[BufferIndex]);

	  BufferIndex += RtlLengthRequiredSid(1);

	  psid->Revision = SID_REVISION;
	  psid->SubAuthorityCount = 1;
	  psid->IdentifierAuthority = localSidAuthority;
	  psid->SubAuthority[0] = SECURITY_LOCAL_SYSTEM_RID;
	  
	  /* ACE Administrators - full access*/
	  pace = (PACCESS_ALLOWED_ACE) &(Buffer[BufferIndex]);
	  BufferIndex += sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD);

	  pace->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
	  pace->Header.AceFlags = CONTAINER_INHERIT_ACE;
	  pace->Header.AceSize = sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD) + RtlLengthRequiredSid(2);
	  pace->Mask = DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER  | 0x3f;
	  pace->SidStart = BufferIndex;

	  /* S-1-5-12 (Administrators) */
	  psid = (PSID)&(Buffer[BufferIndex]);

	  BufferIndex += RtlLengthRequiredSid(2);

	  psid->Revision = SID_REVISION;
	  psid->SubAuthorityCount = 2;
	  psid->IdentifierAuthority = localSidAuthority;
	  psid->SubAuthority[0] = SECURITY_BUILTIN_DOMAIN_RID;
	  psid->SubAuthority[1] = DOMAIN_ALIAS_RID_ADMINS;
	 
	  /* ACE Everyone - read access */
	  pace = (PACCESS_ALLOWED_ACE)&(Buffer[BufferIndex]);
	  BufferIndex += sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD);

	  pace->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
	  pace->Header.AceFlags = CONTAINER_INHERIT_ACE;
	  pace->Header.AceSize = sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD) + RtlLengthRequiredSid(1);
	  pace->Mask = READ_CONTROL| 0x19;
	  pace->SidStart = BufferIndex;

	  /* SID S-1-1-0 (Everyone) */
	  psid = (PSID)&(Buffer[BufferIndex]);

	  BufferIndex += RtlLengthRequiredSid(1);

	  psid->Revision = SID_REVISION;
	  psid->SubAuthorityCount = 1;
	  psid->IdentifierAuthority = worldSidAuthority;
	  psid->SubAuthority[0] = 0;

	  /* calculate used bytes */
	  pacl->AclSize = BufferIndex - pacl->AclSize;
	}
	*ResultLength = BufferIndex;
	dprintf(("len=%lu\n", *ResultLength));
	if (Length < *ResultLength) return STATUS_BUFFER_TOO_SMALL;
	memcpy(pSecurityDesriptor, Buffer, *ResultLength);

	return STATUS_SUCCESS;
}


/******************************************************************************
 *  NtDuplicateObject                       [NTDLL]
 */
NTSTATUS WINAPI NtDuplicateObject(HANDLE      SourceProcessHandle,
                                  PHANDLE     SourceHandle,
                                  HANDLE      TargetProcessHandle,
                                  PHANDLE     TargetHandle,
                                  ACCESS_MASK DesiredAccess,
                                  BOOLEAN     InheritHandle,
                                  ULONG       Options)
{
  dprintf (("NTDLL: NtDuplicateObject(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
            SourceProcessHandle,
            SourceHandle,
            TargetProcessHandle,
            TargetHandle,
            DesiredAccess,
            InheritHandle,
            Options));

  *TargetHandle = 0;
  return 0;
}


/**************************************************************************
 *                 NtClose                        [NTDLL.65]
 * FUNCTION: Closes a handle reference to an object
 * ARGUMENTS:
 * Handle   handle to close
 */
NTSTATUS WINAPI NtClose(
	HANDLE Handle) 
{
	dprintf(("(0x%08x)\n",Handle));
	if (CloseHandle(Handle))
	  return STATUS_SUCCESS;
	return STATUS_UNSUCCESSFUL; /*fixme*/
}


/******************************************************************************
 *  NtWaitForSingleObject                   [NTDLL]
 */
NTSTATUS WINAPI NtWaitForSingleObject(PHANDLE        Object,
                                      BOOLEAN        Alertable,
                                      PLARGE_INTEGER Time)
{
  dprintf(("NTDLL: NtWaitForSingleObject(%08xh,%08xh,%08xh) not implemented.\n",
           Object,
           Alertable,
           Time));

  return 0;
}


/*
 * Directory functions
 */

/**************************************************************************
 * NtOpenDirectoryObject [NTDLL.124]
 * FUNCTION: Opens a namespace directory object
 * ARGUMENTS:
 *  DirectoryHandle                      Variable which receives the directory handle
 *  DesiredAccess Desired access to the directory
 *  ObjectAttributes                     Structure describing the directory
 * RETURNS: Status
 */
NTSTATUS WINAPI NtOpenDirectoryObject(PHANDLE            DirectoryHandle,
                                      ACCESS_MASK        DesiredAccess,
                                      POBJECT_ATTRIBUTES ObjectAttributes)
{
  dprintf(("NTDLL: NtOpenDirectoryObject(%08xh,%08xh,%08xh) not implemented.\n",
           DirectoryHandle,
           DesiredAccess,
           ObjectAttributes));

  return 0;
}


/******************************************************************************
 *  NtCreateDirectoryObject              [NTDLL]
 */
NTSTATUS WINAPI NtCreateDirectoryObject(PHANDLE            DirectoryHandle,
                                        ACCESS_MASK        DesiredAccess,
                                        POBJECT_ATTRIBUTES ObjectAttributes)
{
  dprintf(("NTDLL: NtCreateDirectoryObject(%08xh,%08xh,%08xh) not implemented.\n",
           DirectoryHandle,
           DesiredAccess,
           ObjectAttributes));

  return 0;
}


/******************************************************************************
 * NtQueryDirectoryObject [NTDLL.149]
 * FUNCTION: Reads information from a namespace directory
 * ARGUMENTS:
 *  DirObjInformation                    Buffer to hold the data read
 *  BufferLength  Size of the buffer in bytes
 *  GetNextIndex  If TRUE then set ObjectIndex to the index of the next object
 *       If FALSE then set ObjectIndex to the number of objects in the directory
 *  IgnoreInputIndex                     If TRUE start reading at index 0
 *       If FALSE start reading at the index specified by object index
 *  ObjectIndex                          Zero based index into the directory, interpretation depends on IgnoreInputIndex and GetNextIndex
 *  DataWritten                          Caller supplied storage for the number of bytes written (or NULL)
 */
NTSTATUS WINAPI NtQueryDirectoryObject(HANDLE              DirObjHandle,
                                       POBJDIR_INFORMATION DirObjInformation,
                                       ULONG               BufferLength,
                                       BOOLEAN             GetNextIndex,
                                       BOOLEAN             IgnoreInputIndex,
                                       PULONG              ObjectIndex,
                                       PULONG              DataWritten)
{
  dprintf(("NTDLL: NtQueryDirectoryObject(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           DirObjHandle,
           DirObjInformation,
           BufferLength,
           GetNextIndex,
           IgnoreInputIndex,
           ObjectIndex,
           DataWritten));

  return 0xc0000000; /* We don't have any. Whatever. (Yet.) */
}


/*
 * Link objects
 */

/******************************************************************************
 *  NtOpenSymbolicLinkObject             [NTDLL]
 */
NTSTATUS WINAPI NtOpenSymbolicLinkObject(PHANDLE            LinkHandle,
                                         ACCESS_MASK        DesiredAccess,
                                         POBJECT_ATTRIBUTES ObjectAttributes)
{
  dprintf(("NTDLL: NtOpenSymbolicLinkObject(%08xh,%08xh,%08xh) not implemented.\n",
           LinkHandle,
           DesiredAccess,
           ObjectAttributes));

  return 0;
}


/******************************************************************************
 *  NtCreateSymbolicLinkObject           [NTDLL]
 */
NTSTATUS WINAPI NtCreateSymbolicLinkObject(PHANDLE            SymbolicLinkHandle,
                                           ACCESS_MASK        DesiredAccess,
                                           POBJECT_ATTRIBUTES ObjectAttributes,
                                           PUNICODE_STRING    Name)
{
  dprintf(("NTDLL: NtCreateSymbolicLinkObject(%08xh,%08xh,%08xh,%08x) not implemented.\n",
           SymbolicLinkHandle,
           DesiredAccess,
           ObjectAttributes,
           Name));

  return 0;
}


/******************************************************************************
 *  NtQuerySymbolicLinkObject            [NTDLL]
 */
NTSTATUS WINAPI NtQuerySymbolicLinkObject(HANDLE          LinkHandle,
                                          PUNICODE_STRING LinkTarget,
                                          PULONG          ReturnedLength)
{
  dprintf(("NTDLL: NtQuerySymbolicLinkObject(%08xh,%08xh,%08xh) not implemented.\n",
           LinkHandle,
           LinkTarget,
           ReturnedLength));

  return 0;
}

