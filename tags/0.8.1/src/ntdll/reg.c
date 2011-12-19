/* $Id: reg.c,v 1.5 2003-04-08 12:47:07 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * registry functions
 */
#include <windows.h>
#include <ntddk.h>
#include <winnt.h>
#include <ntdef.h>
#include <winreg.h>

#include <heapstring.h>
#include "debugtools.h"

/*RLW - this isn't used & generates a gcc warning */
#if 0

/* translates predefined paths to HKEY_ constants */
static BOOLEAN _NtKeyToWinKey(
                              IN POBJECT_ATTRIBUTES ObjectAttributes,
                              OUT UINT * Offset,/* offset within ObjectName */
                              OUT HKEY * KeyHandle)/* translated handle */
{
  static const WCHAR KeyPath_HKLM[] = {
    '\\','R','E','G','I','S','T','R','Y',
      '\\','M','A','C','H','I','N','E',0};
  static const WCHAR KeyPath_HKU [] = {
    '\\','R','E','G','I','S','T','R','Y',
      '\\','U','S','E','R',0};
  static const WCHAR KeyPath_HCC [] = {
    '\\','R','E','G','I','S','T','R','Y',
      '\\','M','A','C','H','I','N','E',
      '\\','S','Y','S','T','E','M',
      '\\','C','U','R','R','E','N','T','C','O','N','T','R','O','L','S','E','T',
      '\\','H','A','R','D','W','A','R','E','P','R','O','F','I','L','E','S',
      '\\','C','U','R','R','E','N','T',0};
  static const WCHAR KeyPath_HCR [] = {
    '\\','R','E','G','I','S','T','R','Y',
      '\\','M','A','C','H','I','N','E',
      '\\','S','O','F','T','W','A','R','E',
      '\\','C','L','A','S','S','E','S',0};
  int len;
  PUNICODE_STRING ObjectName = ObjectAttributes->ObjectName;
  
  if(ObjectAttributes->RootDirectory)
  {
    len = 0;
    *KeyHandle = ObjectAttributes->RootDirectory;
  }
  else if((ObjectName->Length > (len=lstrlenW(KeyPath_HKLM)))
          && (0==lstrncmpiW(ObjectName->Buffer,KeyPath_HKLM,len)))
  {  *KeyHandle = HKEY_LOCAL_MACHINE;
  }
  else if((ObjectName->Length > (len=lstrlenW(KeyPath_HKU)))
          && (0==lstrncmpiW(ObjectName->Buffer,KeyPath_HKU,len)))
  {  *KeyHandle = HKEY_USERS;
  }
  else if((ObjectName->Length > (len=lstrlenW(KeyPath_HCR)))
          && (0==lstrncmpiW(ObjectName->Buffer,KeyPath_HCR,len)))
  {  *KeyHandle = HKEY_CLASSES_ROOT;
  }
  else if((ObjectName->Length > (len=lstrlenW(KeyPath_HCC)))
          && (0==lstrncmpiW(ObjectName->Buffer,KeyPath_HCC,len)))
  {  *KeyHandle = HKEY_CURRENT_CONFIG;
  }
  else
  {
    *KeyHandle = 0;
    *Offset = 0;
    return FALSE;
  }
  
  if (len > 0 && ObjectName->Buffer[len] == (WCHAR)'\\') len++;
  *Offset = len;
  
  TRACE("off=%u hkey=0x%08x\n", *Offset, *KeyHandle);
  return TRUE;
}

#endif

/******************************************************************************
 * NtCreateKey [NTDLL]
 * ZwCreateKey
 */
NTSTATUS WINAPI NtCreateKey(PHANDLE            KeyHandle,
                            ACCESS_MASK        DesiredAccess,
                            const OBJECT_ATTRIBUTES* ObjectAttributes,
                            ULONG              TitleIndex,
                            const UNICODE_STRING*  Class,
                            ULONG              CreateOptions,
                            PULONG             Disposition)
{
  dprintf (("NTDLL: NtCreateKey(%08xh,  %08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
            KeyHandle,
            DesiredAccess,
            ObjectAttributes,
            TitleIndex,
            Class,
            CreateOptions,
            Disposition));

  return (0);
}

/******************************************************************************
 * NtDeleteKey [NTDLL]
 * ZwDeleteKey
 */
NTSTATUS WINAPI NtDeleteKey(HANDLE KeyHandle)
{
  dprintf(("NTDLL: NtDeleteKey(%08xh) not implemented\n",
           KeyHandle));

  return (0);
}


/******************************************************************************
 * NtDeleteValueKey [NTDLL]
 * ZwDeleteValueKey
 */
NTSTATUS WINAPI NtDeleteValueKey(HANDLE          KeyHandle,
                                 const UNICODE_STRING* ValueName)
{
  dprintf(("NTDLL: NtDeleteValueKey(%08xh, %08xh) not implemented\n",
           KeyHandle,
           ValueName));

  return(0);
}


/******************************************************************************
 * NtEnumerateKey [NTDLL]
 * ZwEnumerateKey
 */
NTSTATUS WINAPI NtEnumerateKey(HANDLE                KeyHandle,
                               ULONG                 Index,
                               KEY_INFORMATION_CLASS KeyInformationClass,
                               PVOID                 KeyInformation,
                               ULONG                 Length,
                               PULONG                ResultLength)
{
  dprintf(("NTDLL: NtEnumerateKey(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           Index,
           KeyInformationClass,
           KeyInformation,
           Length,
           ResultLength));

  return 0;
}


/******************************************************************************
 *  NtEnumerateValueKey                  [NTDLL]
 *  ZwEnumerateValueKey
 */
NTSTATUS WINAPI NtEnumerateValueKey(HANDLE                      KeyHandle,
                                    ULONG                       Index,
                                    KEY_VALUE_INFORMATION_CLASS KeyInformationClass,
                                    PVOID                       KeyInformation,
                                    ULONG                       Length,
                                    PULONG                      ResultLength)
{
  dprintf(("NTDLL: NtEnumerateValueKey(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           Index,
           KeyInformationClass,
           KeyInformation,
           Length,
           ResultLength));

  return 0;
}


/******************************************************************************
 *  NtFlushKey [NTDLL]
 *  ZwFlushKey
 */
NTSTATUS WINAPI NtFlushKey(HANDLE KeyHandle)
{
  dprintf(("NTDLL: NtFlushKey(%08xh)\n",
           KeyHandle));

  return ERROR_SUCCESS;
}


/******************************************************************************
 *  NtLoadKey  [NTDLL]
 *  ZwLoadKey
 */
NTSTATUS WINAPI NtLoadKey(const OBJECT_ATTRIBUTES *KeyHandle,
                          const OBJECT_ATTRIBUTES *ObjectAttributes)
{
  dprintf(("NTDLL: NtLoadKey(%08xh,%08xh) not implemented.\n",
           KeyHandle,
           ObjectAttributes));

  return 0;
}


/******************************************************************************
 *  NtNotifyChangeKey                    [NTDLL]
 *  ZwNotifyChangeKey
 */
NTSTATUS WINAPI NtNotifyChangeKey(HANDLE           KeyHandle,
                                  HANDLE           Event,
                                  PIO_APC_ROUTINE  ApcRoutine,
                                  PVOID            ApcContext,
                                  PIO_STATUS_BLOCK IoStatusBlock,
                                  ULONG            CompletionFilter,
                                  BOOLEAN          Asynchronous,
                                  PVOID            ChangeBuffer,
                                  ULONG            Length,
                                  BOOLEAN          WatchSubTree)
{
  dprintf(("NTDLL: NtNotifyChangeKey(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           Event,
           ApcRoutine,
           ApcContext,
           IoStatusBlock,
           CompletionFilter,
           Asynchronous,
           ChangeBuffer,
           Length,
           WatchSubTree));

  return 0;
}


/******************************************************************************
 * NtOpenKey [NTDLL.129]
 * ZwOpenKey
 *   OUT PHANDLE                            KeyHandle,
 *   IN     ACCESS_MASK                     DesiredAccess,
 *   IN     POBJECT_ATTRIBUTES           ObjectAttributes
 */
NTSTATUS WINAPI NtOpenKey(PHANDLE            KeyHandle,
                          ACCESS_MASK        DesiredAccess,
                          const OBJECT_ATTRIBUTES *ObjectAttributes)
{
  dprintf(("NTDLL: NtOpenKey(%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           DesiredAccess,
           ObjectAttributes));
  return 0;
}


/******************************************************************************
 * NtQueryKey [NTDLL]
 * ZwQueryKey
 */
NTSTATUS WINAPI NtQueryKey(HANDLE                KeyHandle,
                           KEY_INFORMATION_CLASS KeyInformationClass,
                           PVOID                 KeyInformation,
                           ULONG                 Length,
                           PULONG                ResultLength)
{
  dprintf(("NTDLL: NtQueryKey(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           KeyInformationClass,
           KeyInformation,
           Length,
           ResultLength));

  return 0;
}


/******************************************************************************
 * NtQueryMultipleValueKey [NTDLL]
 * ZwQueryMultipleValueKey
 */

NTSTATUS WINAPI NtQueryMultipleValueKey(HANDLE KeyHandle,
                                        PVALENTW ListOfValuesToQuery,
                                        ULONG NumberOfItems,
                                        PVOID MultipleValueInformation,
                                        ULONG Length,
                                        PULONG  ReturnLength)
{
  dprintf(("NTDLL: NtQueryMultipleValueKey(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           ListOfValuesToQuery,
           NumberOfItems,
           MultipleValueInformation,
           Length,
           ReturnLength));

  return 0;
}


/******************************************************************************
 * NtQueryValueKey [NTDLL]
 * ZwQueryValueKey
 */
NTSTATUS WINAPI NtQueryValueKey(HANDLE                      KeyHandle,
                                const UNICODE_STRING        *ValueName,
                                KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
                                PVOID                       KeyValueInformation,
                                ULONG                       Length,
                                PULONG                      ResultLength)
{
  dprintf(("NTDLL: NtQueryValueKey(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           ValueName,
           KeyValueInformationClass,
           KeyValueInformation,
           Length,
           ResultLength));

  return 0;
}


/******************************************************************************
 * NtReplaceKey [NTDLL]
 * ZwReplaceKey
 */
NTSTATUS WINAPI NtReplaceKey(POBJECT_ATTRIBUTES ObjectAttributes,
                             HANDLE             Key,
                             POBJECT_ATTRIBUTES ReplacedObjectAttributes)
{
  dprintf(("NTDLL: NtReplaceKey(%08xh,%08xh,%08xh) not implemented.\n",
           ObjectAttributes,
           Key,
           ReplacedObjectAttributes));

  return 0;
}


/******************************************************************************
 * NtRestoreKey [NTDLL]
 * ZwRestoreKey
 */
NTSTATUS WINAPI NtRestoreKey(HANDLE KeyHandle,
                             HANDLE FileHandle,
                             ULONG RestoreFlags)
{
  dprintf(("NTDLL: NtRestoreKey(%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           FileHandle,
           RestoreFlags));

  return 0;
}


/******************************************************************************
 * NtSaveKey [NTDLL]
 * ZwSaveKey
 */
NTSTATUS WINAPI NtSaveKey(HANDLE KeyHandle,
                          HANDLE FileHandle)
{
  dprintf(("NTDLL NtSaveKey(%08xh,%08xh) not implemented.\n",
           KeyHandle,
           FileHandle));

  return 0;
}


/******************************************************************************
 * NtSetInformationKey [NTDLL]
 * ZwSetInformationKey
 */
NTSTATUS WINAPI NtSetInformationKey(HANDLE    KeyHandle,
                                    const int KeyInformationClass,
                                    PVOID     KeyInformation,
                                    ULONG     KeyInformationLength)
{
  dprintf(("NTDLL: NtSetInformationKey(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           KeyInformationClass,
           KeyInformation,
           KeyInformationLength));

  return 0;
}


/******************************************************************************
 * NtSetValueKey [NTDLL]
 * ZwSetValueKey
 */
NTSTATUS WINAPI NtSetValueKey(HANDLE          KeyHandle,
                              const UNICODE_STRING *ValueName,
                              ULONG           TitleIndex,
                              ULONG           Type,
                              const VOID      *Data,
                              ULONG           DataSize)
{
  dprintf(("NTDLL: NtSetValueKey(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           KeyHandle,
           ValueName,
           TitleIndex,
           Type,
           Data,
           DataSize));

  return (0);
}


/******************************************************************************
 * NtUnloadKey [NTDLL]
 * ZwUnloadKey
 */
NTSTATUS WINAPI NtUnloadKey(HANDLE KeyHandle)
{
  dprintf(("NTDLL: NtUnloadKey(%08xh) not implemented.\n",
           KeyHandle));

  return 0;
}

