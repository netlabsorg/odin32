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
NTSTATUS WINAPI NtQuerySecurityObject(DWORD x1,
                                      DWORD x2,
                                      DWORD x3,
                                      DWORD x4,
                                      DWORD x5)
{
  dprintf(("NTDLL: NtQuerySecurityObject(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return 0;
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
//NTSTATUS WINAPI NtClose(
//   HANDLE Handle)
//{
//   FIXME(ntdll,"(0x%08x),stub!\n",Handle);
//   return 1;
//}


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

