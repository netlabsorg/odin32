/*
 * Copyright 1999, 2000 Juergen Schmied
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <string.h>

#include "wine/debug.h"
#include "ntdll_misc.h"

#include "ntddk.h"
#include "winioctl.h"

WINE_DEFAULT_DEBUG_CHANNEL(ntdll);

/**************************************************************************
 *                 NtOpenFile				[NTDLL.@]
 *                 ZwOpenFile				[NTDLL.@]
 * FUNCTION: Opens a file
 * ARGUMENTS:
 *  FileHandle		Variable that receives the file handle on return
 *  DesiredAccess	Access desired by the caller to the file
 *  ObjectAttributes	Structue describing the file to be opened
 *  IoStatusBlock	Receives details about the result of the operation
 *  ShareAccess		Type of shared access the caller requires
 *  OpenOptions		Options for the file open
 */
NTSTATUS WINAPI NtOpenFile(
	OUT PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions)
{
	FIXME("(%p,0x%08lx,%p,%p,0x%08lx,0x%08lx) stub\n",
	FileHandle, DesiredAccess, ObjectAttributes, 
	IoStatusBlock, ShareAccess, OpenOptions);
	dump_ObjectAttributes (ObjectAttributes);
	return 0;
}

/**************************************************************************
 *		NtCreateFile				[NTDLL.@]
 *		ZwCreateFile				[NTDLL.@]
 * FUNCTION: Either causes a new file or directory to be created, or it opens
 *  an existing file, device, directory or volume, giving the caller a handle
 *  for the file object. This handle can be used by subsequent calls to
 *  manipulate data within the file or the file object's state of attributes.
 * ARGUMENTS:
 *	FileHandle		Points to a variable which receives the file handle on return
 *	DesiredAccess		Desired access to the file
 *	ObjectAttributes	Structure describing the file
 *	IoStatusBlock		Receives information about the operation on return
 *	AllocationSize		Initial size of the file in bytes
 *	FileAttributes		Attributes to create the file with
 *	ShareAccess		Type of shared access the caller would like to the file
 *	CreateDisposition	Specifies what to do, depending on whether the file already exists
 *	CreateOptions		Options for creating a new file
 *	EaBuffer		Undocumented
 *	EaLength		Undocumented
 */
NTSTATUS WINAPI NtCreateFile(
	OUT PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocateSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength)  
{
	FIXME("(%p,0x%08lx,%p,%p,%p,0x%08lx,0x%08lx,0x%08lx,0x%08lx,%p,0x%08lx) stub\n",
	FileHandle,DesiredAccess,ObjectAttributes,
	IoStatusBlock,AllocateSize,FileAttributes,
	ShareAccess,CreateDisposition,CreateOptions,EaBuffer,EaLength);
	dump_ObjectAttributes (ObjectAttributes);
	return 0;
}

/******************************************************************************
 *  NtReadFile					[NTDLL.@]
 *  ZwReadFile					[NTDLL.@]
 *
 * Parameters
 *   HANDLE32 		FileHandle
 *   HANDLE32 		Event 		OPTIONAL
 *   PIO_APC_ROUTINE 	ApcRoutine 	OPTIONAL
 *   PVOID 		ApcContext 	OPTIONAL
 *   PIO_STATUS_BLOCK 	IoStatusBlock
 *   PVOID 		Buffer
 *   ULONG 		Length
 *   PLARGE_INTEGER 	ByteOffset 	OPTIONAL
 *   PULONG 		Key 		OPTIONAL
 */
NTSTATUS WINAPI NtReadFile (
	HANDLE FileHandle,
	HANDLE EventHandle,
	PIO_APC_ROUTINE ApcRoutine,
	PVOID ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER ByteOffset,
	PULONG Key)
{
	FIXME("(0x%08x,0x%08x,%p,%p,%p,%p,0x%08lx,%p,%p),stub!\n",
	FileHandle,EventHandle,ApcRoutine,ApcContext,IoStatusBlock,Buffer,Length,ByteOffset,Key);
	return 0;
}

/**************************************************************************
 *		NtDeviceIoControlFile			[NTDLL.@]
 *		ZwDeviceIoControlFile			[NTDLL.@]
 */
NTSTATUS WINAPI NtDeviceIoControlFile(
	IN HANDLE DeviceHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE UserApcRoutine OPTIONAL,
	IN PVOID UserApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG IoControlCode,
	IN PVOID InputBuffer,
	IN ULONG InputBufferSize,
	OUT PVOID OutputBuffer,
	IN ULONG OutputBufferSize)
{
	FIXME("(0x%08x,0x%08x,%p,%p,%p,0x%08lx,%p,0x%08lx,%p,0x%08lx): empty stub\n",
	DeviceHandle, Event, UserApcRoutine, UserApcContext,
	IoStatusBlock, IoControlCode, InputBuffer, InputBufferSize, OutputBuffer, OutputBufferSize);
	return 0;
}

/******************************************************************************
 * NtFsControlFile [NTDLL.@]
 * ZwFsControlFile [NTDLL.@]
 */
NTSTATUS WINAPI NtFsControlFile(
	IN HANDLE DeviceHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG IoControlCode,
	IN PVOID InputBuffer,
	IN ULONG InputBufferSize,
	OUT PVOID OutputBuffer,
	IN ULONG OutputBufferSize)
{
	FIXME("(0x%08x,0x%08x,%p,%p,%p,0x%08lx,%p,0x%08lx,%p,0x%08lx): stub\n",
	DeviceHandle,Event,ApcRoutine,ApcContext,IoStatusBlock,IoControlCode,
	InputBuffer,InputBufferSize,OutputBuffer,OutputBufferSize);
	return 0;
}

/******************************************************************************
 *  NtSetVolumeInformationFile		[NTDLL.@]
 *  ZwSetVolumeInformationFile		[NTDLL.@]
 */
NTSTATUS WINAPI NtSetVolumeInformationFile(
	IN HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FsInformation,
        ULONG Length,
	FS_INFORMATION_CLASS FsInformationClass) 
{
	FIXME("(0x%08x,%p,%p,0x%08lx,0x%08x) stub\n",
	FileHandle,IoStatusBlock,FsInformation,Length,FsInformationClass);
	return 0;
}

/******************************************************************************
 *  NtQueryInformationFile		[NTDLL.@]
 *  ZwQueryInformationFile		[NTDLL.@]
 */
NTSTATUS WINAPI NtQueryInformationFile(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInformation,
	ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass)
{
	FIXME("(0x%08x,%p,%p,0x%08lx,0x%08x),stub!\n",
	FileHandle,IoStatusBlock,FileInformation,Length,FileInformationClass);
	return 0;
}

/******************************************************************************
 *  NtSetInformationFile		[NTDLL.@]
 *  ZwSetInformationFile		[NTDLL.@]
 */
NTSTATUS WINAPI NtSetInformationFile(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInformation,
	ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass)
{
	FIXME("(0x%08x,%p,%p,0x%08lx,0x%08x)\n",
	FileHandle,IoStatusBlock,FileInformation,Length,FileInformationClass);
	return 0;
}

/******************************************************************************
 *  NtQueryDirectoryFile	[NTDLL.@]
 *  ZwQueryDirectoryFile	[NTDLL.@]
 *  ZwQueryDirectoryFile
 */
NTSTATUS WINAPI NtQueryDirectoryFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	IN BOOLEAN ReturnSingleEntry,
	IN PUNICODE_STRING FileName OPTIONAL,
	IN BOOLEAN RestartScan)
{
	FIXME("(0x%08x 0x%08x %p %p %p %p 0x%08lx 0x%08x 0x%08x %p 0x%08x\n",
	FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation,
	Length, FileInformationClass, ReturnSingleEntry, 
	debugstr_us(FileName),RestartScan);
	return 0;
}

/******************************************************************************
 *  NtQueryVolumeInformationFile		[NTDLL.@]
 *  ZwQueryVolumeInformationFile		[NTDLL.@]
 */
NTSTATUS WINAPI NtQueryVolumeInformationFile (
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FSInformation,
	IN ULONG Length,
	IN FS_INFORMATION_CLASS FSInformationClass)
{
	ULONG len = 0;

	FIXME("(0x%08x %p %p 0x%08lx 0x%08x) stub!\n",
	FileHandle, IoStatusBlock, FSInformation, Length, FSInformationClass);

	switch ( FSInformationClass )
	{
	  case FileFsVolumeInformation:
	    len = sizeof( FILE_FS_VOLUME_INFORMATION );
	    break;
	  case FileFsLabelInformation:
	    len = 0;
	    break;

	  case FileFsSizeInformation:
	    len = sizeof( FILE_FS_SIZE_INFORMATION );
	    break;

	  case FileFsDeviceInformation:	
	    len = sizeof( FILE_FS_DEVICE_INFORMATION );
	    break;
	  case FileFsAttributeInformation:
	    len = sizeof( FILE_FS_ATTRIBUTE_INFORMATION );
	    break;

	  case FileFsControlInformation:
	    len = 0;
	    break;

	  case FileFsFullSizeInformation:
	    len = 0;
	    break;

	  case FileFsObjectIdInformation:
	    len = 0;
	    break;

	  case FileFsMaximumInformation:
	    len = 0;
	    break;
	}

	if (Length < len)
	  return STATUS_BUFFER_TOO_SMALL;

	switch ( FSInformationClass )
	{
	  case FileFsVolumeInformation:
	    break;
	  case FileFsLabelInformation:
	    break;

	  case FileFsSizeInformation:
	    break;

	  case FileFsDeviceInformation:	
	    if (FSInformation)
	    {
	      FILE_FS_DEVICE_INFORMATION * DeviceInfo = FSInformation;
	      DeviceInfo->DeviceType = FILE_DEVICE_DISK; 
	      DeviceInfo->Characteristics = 0;
	      break;
	    };
	  case FileFsAttributeInformation:
	    break;

	  case FileFsControlInformation:
	    break;

	  case FileFsFullSizeInformation:
	    break;

	  case FileFsObjectIdInformation:
	    break;

	  case FileFsMaximumInformation:
	    break;
	}
	IoStatusBlock->DUMMYUNIONNAME_DOT Status = STATUS_SUCCESS;
	IoStatusBlock->Information = len;
	return STATUS_SUCCESS;
}
