/* $Id: reg.cpp,v 1.2 1999-06-10 17:06:46 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * registry functions
 */

#include "winreg.h"

#include "ntdll.h"


/******************************************************************************
 * NtCreateKey [NTDLL]
 * ZwCreateKey
 */
NTSTATUS WINAPI NtCreateKey(PHANDLE            KeyHandle,
                            ACCESS_MASK        DesiredAccess,
                            POBJECT_ATTRIBUTES ObjectAttributes,
                            ULONG              TitleIndex,
                            PUNICODE_STRING    Class,
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
                                 PUNICODE_STRING ValueName)
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
NTSTATUS WINAPI NtLoadKey(PHANDLE            KeyHandle,
                          POBJECT_ATTRIBUTES ObjectAttributes)
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
                          POBJECT_ATTRIBUTES ObjectAttributes)
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
                                PUNICODE_STRING             ValueName,
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
                              PUNICODE_STRING ValueName,
                              ULONG           TitleIndex,
                              ULONG           Type,
                              PVOID           Data,
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
