/* $Id: sync.c,v 1.6 2005-01-23 18:10:22 sao2l02 Exp $ */

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
#include "wine/debug.h"
#include "ntddk.h"
#include "ntdll_misc.h"
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

/*
 *	Timers
 */

/**************************************************************************
 *		NtCreateTimer				[NTDLL.@]
 *		ZwCreateTimer				[NTDLL.@]
 */
NTSTATUS WINAPI NtCreateTimer(
	OUT PHANDLE TimerHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN TIMER_TYPE TimerType)
{
	FIXME("(%p,0x%08lx,%p,0x%08x) stub\n",
	TimerHandle,DesiredAccess,ObjectAttributes, TimerType);
	dump_ObjectAttributes(ObjectAttributes);
	return 0;
}
/**************************************************************************
 *		NtSetTimer				[NTDLL.@]
 *		ZwSetTimer				[NTDLL.@]
 */
NTSTATUS WINAPI NtSetTimer(
	IN HANDLE TimerHandle,
	IN PLARGE_INTEGER DueTime,
	IN PTIMERAPCROUTINE TimerApcRoutine,
	IN PVOID TimerContext,
	IN BOOLEAN WakeTimer,
	IN ULONG Period OPTIONAL,
	OUT PBOOLEAN PreviousState OPTIONAL)
{
	FIXME("(0x%08x,%p,%p,%p,%08x,0x%08lx,%p) stub\n",
	TimerHandle,DueTime,TimerApcRoutine,TimerContext,WakeTimer,Period,PreviousState);
	return 0;
}

/******************************************************************************
/******************************************************************************
 * NtQueryTimerResolution [NTDLL.@]
 */
NTSTATUS WINAPI NtQueryTimerResolution(OUT ULONG* min_resolution,
                                       OUT ULONG* max_resolution,
                                       OUT ULONG* current_resolution)
{
    FIXME("(%p,%p,%p), stub!\n",
          min_resolution, max_resolution, current_resolution);

    return STATUS_NOT_IMPLEMENTED;
}

/*
 *	Process object
 */

