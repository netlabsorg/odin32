/* $Id: file.cpp,v 1.3 2000-02-03 21:46:42 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 */

#include <stdlib.h>
#include <string.h>

#include "ntdll.h"


/**************************************************************************
 *                 NtOpenFile                     [NTDLL.127]
 * FUNCTION: Opens a file
 * ARGUMENTS:
 *  FileHandle    Variable that receives the file handle on return
 *  DesiredAccess Access desired by the caller to the file
 *  ObjectAttributes                     Structue describing the file to be opened
 *  IoStatusBlock Receives details about the result of the operation
 *  ShareAccess                          Type of shared access the caller requires
 *  OpenOptions                          Options for the file open
 */
NTSTATUS WIN32API NtOpenFile(PHANDLE            FileHandle,
                             ACCESS_MASK        DesiredAccess,
                             POBJECT_ATTRIBUTES ObjectAttributes,
                             PIO_STATUS_BLOCK   IoStatusBlock,
                             ULONG              ShareAccess,
                             ULONG              OpenOptions)
{
  dprintf(("NTDLL: NtOpenFile(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           FileHandle,
           DesiredAccess,
           ObjectAttributes,
           IoStatusBlock,
           ShareAccess,
           OpenOptions));

  return 0;
}


/**************************************************************************
 *    NtCreateFile                                [NTDLL.73]
 * FUNCTION: Either causes a new file or directory to be created, or it opens
 *  an existing file, device, directory or volume, giving the caller a handle
 *  for the file object. This handle can be used by subsequent calls to
 *  manipulate data within the file or the file object's state of attributes.
 * ARGUMENTS:
 * FileHandle     Points to a variable which receives the file handle on return
 * DesiredAccess                         Desired access to the file
 * ObjectAttributes                      Structure describing the file
 * IoStatusBlock                         Receives information about the operation on return
 * AllocationSize                        Initial size of the file in bytes
 * FileAttributes                        Attributes to create the file with
 * ShareAccess    Type of shared access the caller would like to the file
 * CreateDisposition                     Specifies what to do, depending on whether the file already exists
 * CreateOptions                         Options for creating a new file
 * EaBuffer    Undocumented
 * EaLength    Undocumented
 */
NTSTATUS WIN32API NtCreateFile(PHANDLE            FileHandle,
                               ACCESS_MASK        DesiredAccess,
                               POBJECT_ATTRIBUTES ObjectAttributes,
                               PIO_STATUS_BLOCK   IoStatusBlock,
                               PLARGE_INTEGER     AllocateSize,
                               ULONG              FileAttributes,
                               ULONG              ShareAccess,
                               ULONG              CreateDisposition,
                               ULONG              CreateOptions,
                               PVOID              EaBuffer,
                               ULONG              EaLength)
{
  dprintf(("NTDLL: NtCreateFile(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           FileHandle,
           DesiredAccess,
           ObjectAttributes,
           IoStatusBlock,
           AllocateSize,
           FileAttributes,
           ShareAccess,
           CreateDisposition,
           CreateOptions,
           EaBuffer,
           EaLength));

  return 0;
}


/******************************************************************************
 *  NtReadFile                                 [NTDLL]
 *  ZwReadFile
 *
 * Parameters
 *   HANDLE32     FileHandle
 *   HANDLE32     Event                     OPTIONAL
 *   PIO_APC_ROUTINE                     ApcRoutine  OPTIONAL
 *   PVOID     ApcContext                OPTIONAL
 *   PIO_STATUS_BLOCK                    IoStatusBlock
 *   PVOID     Buffer
 *   ULONG     Length
 *   PLARGE_INTEGER                      ByteOffset  OPTIONAL
 *   PULONG       Key                       OPTIONAL
 */
NTSTATUS WIN32API NtReadFile (HANDLE           FileHandle,
                              HANDLE           EventHandle,
                              PIO_APC_ROUTINE  ApcRoutine,
                              PVOID            ApcContext,
                              PIO_STATUS_BLOCK IoStatusBlock,
                              PVOID            Buffer,
                              ULONG            Length,
                              PLARGE_INTEGER   ByteOffset,
                              PULONG           Key)
{
  dprintf(("NTDLL: NtReadFile(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           FileHandle,
           EventHandle,
           ApcRoutine,
           ApcContext,
           IoStatusBlock,
           Buffer,
           Length,
           ByteOffset,
           Key));

  return 0;
}


/**************************************************************************
 *    NtDeviceIoControlFile                    [NTDLL.94]
 */
NTSTATUS WIN32API NtDeviceIoControlFile(HANDLE           DeviceHandle,
                                        HANDLE           Event,
                                        PIO_APC_ROUTINE  UserApcRoutine,
                                        PVOID            UserApcContext,
                                        PIO_STATUS_BLOCK IoStatusBlock,
                                        ULONG            IoControlCode,
                                        PVOID            InputBuffer,
                                        ULONG            InputBufferSize,
                                        PVOID            OutputBuffer,
                                        ULONG            OutputBufferSize)
{
  dprintf(("NTDLL: NtDeviceIoControlFile(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           DeviceHandle,
           Event,
           UserApcRoutine,
           UserApcContext,
           IoStatusBlock,
           IoControlCode,
           InputBuffer,
           InputBufferSize,
           OutputBuffer,
           OutputBufferSize));

  return 0;
}


/******************************************************************************
 * NtFsControlFile [NTDLL.108]
 */
NTSTATUS WIN32API NtFsControlFile(HANDLE           DeviceHandle,
                                  HANDLE           Event,
                                  PIO_APC_ROUTINE  UserApcRoutine,
                                  PVOID            UserApcContext,
                                  PIO_STATUS_BLOCK IoStatusBlock,
                                  ULONG            IoControlCode,
                                  PVOID            InputBuffer,
                                  ULONG            InputBufferSize,
                                  PVOID            OutputBuffer,
                                  ULONG            OutputBufferSize)
{
  dprintf(("NTDLL: NtFsControlFile(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           DeviceHandle,
           Event,
           UserApcRoutine,
           UserApcContext,
           IoStatusBlock,
           IoControlCode,
           InputBuffer,
           InputBufferSize,
           OutputBuffer,
           OutputBufferSize));

  return 0;
}


/******************************************************************************
 *  NtSetVolumeInformationFile              [NTDLL]
 */
NTSTATUS WIN32API NtSetVolumeInformationFile(HANDLE FileHandle,
                                             PVOID  VolumeInformationClass,
                                             PVOID  VolumeInformation,
                                             ULONG  Length)
{
  dprintf(("NTDLL: NtSetVolumeInformationFile(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           FileHandle,
           VolumeInformationClass,
           VolumeInformation,
           Length));

  return 0;
}


/******************************************************************************
 *  NtQueryInformationFile                  [NTDLL]
 */
NTSTATUS WIN32API NtQueryInformationFile(HANDLE                 FileHandle,
                                         PIO_STATUS_BLOCK       IoStatusBlock,
                                         PVOID                  FileInformation,
                                         ULONG                  Length,
                                         FILE_INFORMATION_CLASS FileInformationClass)
{
  dprintf(("NTDLL: NtQueryInformationFile(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           FileHandle,
           IoStatusBlock,
           FileInformation,
           Length,
           FileInformationClass));

  return 0;
}


/******************************************************************************
 *  NtSetInformationFile                    [NTDLL]
 */
NTSTATUS WIN32API NtSetInformationFile(HANDLE                 FileHandle,
                                       PIO_STATUS_BLOCK       IoStatusBlock,
                                       PVOID                  FileInformation,
                                       ULONG                  Length,
                                       FILE_INFORMATION_CLASS FileInformationClass)
{
  dprintf(("NTDLL: NtSetInformationFile(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           FileHandle,
           IoStatusBlock,
           FileInformation,
           Length,
           FileInformationClass));

  return 0;
}


/******************************************************************************
 *  NtQueryDirectoryFile                 [NTDLL]
 *  ZwQueryDirectoryFile
 */
NTSTATUS WIN32API NtQueryDirectoryFile(HANDLE                 FileHandle,
                                       HANDLE                 Event,
                                       PIO_APC_ROUTINE        ApcRoutine,
                                       PVOID                  ApcContext,
                                       PIO_STATUS_BLOCK       IoStatusBlock,
                                       PVOID                  FileInformation,
                                       ULONG                  Length,
                                       FILE_INFORMATION_CLASS FileInformationClass,
                                       BOOLEAN                ReturnSingleEntry,
                                       PUNICODE_STRING        FileName,
                                       BOOLEAN                RestartScan)
{
  dprintf(("NTDLL: NtQueryDirectoryFile(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           FileHandle,
           Event,
           ApcRoutine,
           ApcContext,
           IoStatusBlock,
           FileInformation,
           Length,
           FileInformationClass,
           ReturnSingleEntry,
           FileName,
           RestartScan));

  return 0;
}


