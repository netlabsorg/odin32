/* $Id: sync.c,v 1.4 2003-04-07 18:40:52 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * Process synchronisation
 */

#include <windows.h>
#include <dbglog.h>
#include "ntddk.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>



/*
 * Semaphore
 */

/******************************************************************************
 *  NtCreateSemaphore                    [NTDLL]
 */
NTSTATUS WINAPI NtCreateSemaphore(PHANDLE            SemaphoreHandle,
                                  ACCESS_MASK        DesiredAccess,
                                  const OBJECT_ATTRIBUTES *ObjectAttributes,
                                  ULONG              InitialCount,
                                  ULONG              MaximumCount)
{
  dprintf(("NTDLL: NtCreateSemaphore(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SemaphoreHandle,
           DesiredAccess,
           ObjectAttributes,
           InitialCount,
           MaximumCount));

 
  return 0;
}


/******************************************************************************
 *  NtOpenSemaphore                      [NTDLL]
 */
NTSTATUS WINAPI NtOpenSemaphore(HANDLE             SemaphoreHandle,
                                ACCESS_MASK        DesiredAccess,
                                POBJECT_ATTRIBUTES ObjectAttributes)
{
  dprintf(("NTDLL: NtOpenSemaphore(%08xh,%08xh,%08xh) not implemented.\n",
           SemaphoreHandle,
           DesiredAccess,
           ObjectAttributes));

  return 0;
}


/******************************************************************************
 *  NtQuerySemaphore                     [NTDLL]
 */
NTSTATUS WINAPI NtQuerySemaphore(HANDLE SemaphoreHandle,
                                 PVOID  SemaphoreInformationClass,
                                 PVOID SemaphoreInformation,
                                 ULONG Length,
                                 PULONG ReturnLength)
{
  dprintf(("NTDLL: NtQuerySemaphore(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SemaphoreHandle,
           SemaphoreInformationClass,
           SemaphoreInformation,
           Length,
           ReturnLength));

  return 0;
}


/******************************************************************************
 *  NtReleaseSemaphore                   [NTDLL]
 */
NTSTATUS WINAPI NtReleaseSemaphore(HANDLE SemaphoreHandle,
                                   ULONG  ReleaseCount,
                                   PULONG PreviousCount)
{
  dprintf(("NTDLL: NtReleaseSemaphore(%08xh,%08xh,%08xh) not implemented.\n",
           SemaphoreHandle,
           ReleaseCount,
           PreviousCount));

  return 0;
}


/*
 * Event
 */

/**************************************************************************
 *    NtCreateEvent                               [NTDLL.71]
 */
NTSTATUS WINAPI NtCreateEvent(PHANDLE            EventHandle,
                              ACCESS_MASK        DesiredAccess,
                              const OBJECT_ATTRIBUTES * ObjectAttributes,
                              BOOLEAN            ManualReset,
                              BOOLEAN            InitialState)
{
  dprintf(("NTDLL: NtCreateEvent(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           EventHandle,
           DesiredAccess,
           ObjectAttributes,
           ManualReset,
           InitialState));

  return 0;
}


/******************************************************************************
 *  NtOpenEvent                          [NTDLL]
 */
NTSTATUS WINAPI NtOpenEvent(PHANDLE            EventHandle,
                            ACCESS_MASK        DesiredAccess,
                            const OBJECT_ATTRIBUTES *ObjectAttributes)
{
  dprintf(("NTDLL: NtOpenEvent(%08xh,%08xh,%08xh) not implemented.\n",
           EventHandle,
           DesiredAccess,
           ObjectAttributes));

  return 0;
}


/******************************************************************************
 *  NtSetEvent    [NTDLL]
 */
NTSTATUS WINAPI NtSetEvent(HANDLE EventHandle,
                           PULONG NumberOfThreadsReleased)
{
  dprintf(("NTDLL: NtSetEvent(%08xh,%08xh) not implemented.\n",
           EventHandle,
           NumberOfThreadsReleased));

  return 0;
}

/**************************************************************************
 *                 NtResetEvent                   [NTDLL.?]
 */
NTSTATUS WIN32API NtResetEvent(HANDLE hEvent, PULONG UnknownVariable)
{
  dprintf(("NTDLL: NtResetEvent(%08xh) not implemented.\n",
           hEvent));

  return 0;
}

/**************************************************************************
 *                 NtClearEvent                   [NTDLL.?]
 */
NTSTATUS WIN32API NtClearEvent(HANDLE hEvent)
{
  dprintf(("NTDLL: NtClearEvent(%08xh) not implemented.\n",
           hEvent));

  return 0;
}

/**************************************************************************
 *                 NtPulseEvent                   [NTDLL.?]
 */
NTSTATUS WIN32API NtPulseEvent(HANDLE hEvent,
                               PULONG PulseCount)
{
  dprintf(("NTDLL: NtPulseEvent(%08xh,%08xh) not implemented.\n",
           hEvent,
          PulseCount));

  return 0;
}

/**************************************************************************
 *                 NtQueryEvent                   [NTDLL.?]
 */
NTSTATUS WIN32API NtQueryEvent(HANDLE hEvent,
                               UINT   EventInformationClass,
                               PVOID  EventInformation,
                               ULONG  EventInformationLength,
                               PULONG ReturnLength)
{
  dprintf(("NTDLL: NtQueryEvent() not implemented.\n"));

  return 0;
}
