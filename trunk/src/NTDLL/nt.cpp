/* $Id: nt.cpp,v 1.3 1999-12-18 20:01:13 sandervl Exp $ */


/*
 * NT basis DLL
 *
 * This file contains the Nt* API functions of NTDLL.DLL.
 * In the original ntdll.dll they all seem to just call int 0x2e (down to the HAL)
 *
 * Copyright 1996-1998 Marcus Meissner
 * Copyright 1999      Patrick Haller
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ntdll.h"


/* move to winbase.h */

// @@@PH can't compile this WINE code
//typedef VOID (CALLBACK *PTIMERAPCROUTINE)(LPVOID lpArgToCompletionRoutine,DWORD dwTimerLowValue,DWORD dwTimerHighValue);
typedef PVOID PTIMERAPCROUTINE;


/*
 * Timer object
 */

/**************************************************************************
 *    NtCreateTimer                               [NTDLL.87]
 */
NTSTATUS WINAPI NtCreateTimer(PHANDLE            TimerHandle,
                              ACCESS_MASK        DesiredAccess,
                              POBJECT_ATTRIBUTES ObjectAttributes,
                              TIMER_TYPE         TimerType)
{
  dprintf(("NTDLL: NtCreateTimer(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           TimerHandle,
           DesiredAccess,
           ObjectAttributes,
           TimerType));

  return 0;
}


/**************************************************************************
 *    NtSetTimer                               [NTDLL.221]
 */
NTSTATUS WINAPI NtSetTimer(HANDLE           TimerHandle,
                           PLARGE_INTEGER   DueTime,
                           PTIMERAPCROUTINE TimerApcRoutine,
                           PVOID            TimerContext,
                           BOOLEAN          WakeTimer,
                           ULONG            Period,
                           PBOOLEAN         PreviousState)
{
  dprintf(("NTDLL: NtSetTimer(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           TimerHandle,
           DueTime,
           TimerApcRoutine,
           TimerContext,
           WakeTimer,
           Period,
           PreviousState));

  return 0;
}


/******************************************************************************
 * NtQueryTimerResolution [NTDLL.129]
 */
NTSTATUS WINAPI NtQueryTimerResolution(DWORD x1,
                                       DWORD x2,
                                       DWORD x3)
{
  dprintf(("NTDLL: NtQueryTimerResolution(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 1;
}


/*
 * Process object
 */

/******************************************************************************
 *  NtTerminateProcess                         [NTDLL.]
 *
 *  Native applications must kill themselves when done
 * FIXME: return value 0-success
 */
NTSTATUS WINAPI NtTerminateProcess(HANDLE ProcessHandle,
                                   LONG   ExitStatus)
{
  dprintf(("NTDLL: NtTerminateProcess(%08xh,%08xh)",
           ProcessHandle,
           ExitStatus));

  /* win32 (0x7fffffff) to nt (-1) */
  if ( NtCurrentProcess() == ProcessHandle )
    ProcessHandle = GetCurrentProcess();

  /* @@@PH return code looks suspicious ! */
  return (! TerminateProcess( ProcessHandle, ExitStatus ));
}


/******************************************************************************
*  NtQueryInformationProcess                [NTDLL.]
*
*/
NTSTATUS WINAPI NtQueryInformationProcess(HANDLE           ProcessHandle,
                                          PROCESSINFOCLASS ProcessInformationClass,
                                          PVOID            ProcessInformation,
                                          ULONG            ProcessInformationLength,
                                          PULONG           ReturnLength)
{
  dprintf(("NTDLL: NtQueryInformationProcess(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ProcessHandle,
           ProcessInformationClass,
           ProcessInformation,
           ProcessInformationLength,
           ReturnLength));

  return 0;
}

/******************************************************************************
 * NtSetInformationProcess [NTDLL.207]
 */
NTSTATUS WINAPI NtSetInformationProcess(HANDLE           ProcessHandle,
                                        PROCESSINFOCLASS ProcessInformationClass,
                                        PVOID            ProcessInformation,
                                        ULONG            ProcessInformationLength)
{
  dprintf(("NTDLL: NtSetInformationProcess(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ProcessHandle,
           ProcessInformationClass,
           ProcessInformation,
           ProcessInformationLength));

  return 0;
}


/*
 * Thread
 */

/******************************************************************************
 *  NtResumeThread                       [NTDLL]
 */
NTSTATUS WINAPI NtResumeThread(HANDLE ThreadHandle,
                               PULONG SuspendCount)
{
  dprintf(("NTDLL: NtResumeThread(%08xh,%08x) not implemented.\n",
           ThreadHandle,
           SuspendCount));

  return 0;
}


/******************************************************************************
 *  NtTerminateThread                    [NTDLL]
 */
NTSTATUS WINAPI NtTerminateThread(HANDLE ThreadHandle,
                                  NTSTATUS ExitStatus)
{
  dprintf(("NTDLL: NtTerminateThread(%08xh,%08xh) not correctly implemented.\n",
           ThreadHandle,
           ExitStatus));

  if ( TerminateThread(ThreadHandle,ExitStatus) )
    return 0;

  return 0xc0000000; /* FIXME: lasterror->ntstatus */
}


/******************************************************************************
*  NtQueryInformationThread                 [NTDLL.]
*
*/
NTSTATUS WINAPI NtQueryInformationThread(HANDLE          ThreadHandle,
                                         THREADINFOCLASS ThreadInformationClass,
                                         PVOID           ThreadInformation,
                                         ULONG           ThreadInformationLength,
                                         PULONG          ReturnLength)
{
  dprintf(("NTDLL: NtQueryInformationThread(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ThreadHandle,
           ThreadInformationClass,
           ThreadInformation,
           ThreadInformationLength,
           ReturnLength));

  return 0;
}


/******************************************************************************
 *  NtSetInformationThread                  [NTDLL]
 */
NTSTATUS WINAPI NtSetInformationThread(HANDLE          ThreadHandle,
                                       THREADINFOCLASS ThreadInformationClass,
                                       PVOID           ThreadInformation,
                                       ULONG           ThreadInformationLength)
{
  dprintf(("NTDLL: NtSetInformationThread(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ThreadHandle,
           ThreadInformationClass,
           ThreadInformation,
           ThreadInformationLength));

  return 0;
}


/*
 * Token
 */

/******************************************************************************
 *  NtDuplicateToken                        [NTDLL]
 */
NTSTATUS WINAPI NtDuplicateToken(HANDLE                       ExistingToken,
                                 ACCESS_MASK                  DesiredAccess,
                                 POBJECT_ATTRIBUTES           ObjectAttributes,
                                 SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
                                 TOKEN_TYPE                   TokenType,
                                 PHANDLE                      NewToken)
{
  dprintf(("NTDLL: NtDuplicateToken(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ExistingToken,
           DesiredAccess,
           ObjectAttributes,
           ImpersonationLevel,
           TokenType,
           NewToken));

  return 0;
}

/******************************************************************************
 *  NtOpenProcessToken                      [NTDLL]
 */
NTSTATUS WINAPI NtOpenProcessToken(HANDLE  ProcessHandle,
                                   DWORD   DesiredAccess,
                                   PHANDLE TokenHandle)
{
  dprintf(("NTDLL: NtOpenProcessToken(%08xh,%08xh,%08xh) not implemented.\n",
           ProcessHandle,
           DesiredAccess,
           TokenHandle));

  *TokenHandle = 0xcafe;
  return 0;
}


/******************************************************************************
 *  NtOpenThreadToken                       [NTDLL]
 */
NTSTATUS WINAPI NtOpenThreadToken(HANDLE  ThreadHandle,
                                  DWORD   DesiredAccess,
                                  BOOLEAN OpenAsSelf,
                                  PHANDLE TokenHandle)
{
  dprintf(("NTDLL: NtOpenThreadToken(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ThreadHandle,
           DesiredAccess,
           OpenAsSelf,
           TokenHandle));

  *TokenHandle = 0xcafe;
  return 0;
}


/******************************************************************************
 *  NtAdjustPrivilegesToken                 [NTDLL]
 *
 * FIXME: parameters unsafe
 */
NTSTATUS WINAPI NtAdjustPrivilegesToken(HANDLE            TokenHandle,
                                        BOOLEAN           DisableAllPrivileges,
                                        PTOKEN_PRIVILEGES NewState,
                                        DWORD             BufferLength,
                                        PTOKEN_PRIVILEGES PreviousState,
                                        PDWORD            ReturnLength)
{
  dprintf(("NTDLL: NtAdjustPrivilegesToken(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           TokenHandle,
           DisableAllPrivileges,
           NewState,
           BufferLength,
           PreviousState,
           ReturnLength));

  return 0;
}


/******************************************************************************
*  NtQueryInformationToken                  [NTDLL.156]
*
*/
NTSTATUS WINAPI NtQueryInformationToken(HANDLE  Token,
                                        DWORD   TokenInformationClass,
                                        LPVOID  TokenInformation,
                                        DWORD   TokenInformationLength,
                                        LPDWORD ReturnLength)
{
  dprintf(("NTDLL: NtQueryInformationToken(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           Token,
           TokenInformationClass,
           TokenInformation,
           TokenInformationLength,
           ReturnLength));

  switch (TokenInformationClass)
  {
    case TokenGroups:                    /* 2 */
      	*ReturnLength = sizeof (TOKEN_GROUPS);
	if(TokenInformationLength < sizeof (TOKEN_GROUPS)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_GROUPS));
	break;
    case TokenUser:                     /* 1 */
      	*ReturnLength = sizeof (TOKEN_USER);
	if(TokenInformationLength < sizeof (TOKEN_USER)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_USER));
	break;
    case TokenPrivileges:
      	*ReturnLength = sizeof (TOKEN_PRIVILEGES);
	if(TokenInformationLength < sizeof (TOKEN_PRIVILEGES)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_PRIVILEGES));
	break;
    case TokenOwner:
      	*ReturnLength = sizeof (TOKEN_OWNER);
	if(TokenInformationLength < sizeof (TOKEN_OWNER)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_OWNER));
	break;
    case TokenPrimaryGroup:
      	*ReturnLength = sizeof (TOKEN_PRIMARY_GROUP);
	if(TokenInformationLength < sizeof (TOKEN_PRIMARY_GROUP)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_PRIMARY_GROUP));
	break;
    case TokenDefaultDacl:
      	*ReturnLength = sizeof (TOKEN_DEFAULT_DACL);
	if(TokenInformationLength < sizeof (TOKEN_DEFAULT_DACL)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_DEFAULT_DACL));
	break;
    case TokenSource:
      	*ReturnLength = sizeof (TOKEN_SOURCE);
	if(TokenInformationLength < sizeof (TOKEN_SOURCE)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_SOURCE));
	break;
    case TokenType:
      	*ReturnLength = sizeof (TOKEN_TYPE);
	if(TokenInformationLength < sizeof (TOKEN_TYPE)) {
		return STATUS_BUFFER_TOO_SMALL;
	}
	memset(TokenInformation, 0, sizeof(TOKEN_TYPE));
	break;
#if 0
    case TokenImpersonationLevel:
    case TokenStatistics:
#endif /* 0 */
  }

  return STATUS_SUCCESS;
}


/*
 * Section
 */

/******************************************************************************
 *  NtCreateSection                      [NTDLL]
 */
NTSTATUS WINAPI NtCreateSection(PHANDLE SectionHandle,
                                ACCESS_MASK DesiredAccess,
                                POBJECT_ATTRIBUTES ObjectAttributes,
                                PLARGE_INTEGER MaximumSize,
                                ULONG SectionPageProtection,
                                ULONG AllocationAttributes,
                                HANDLE FileHandle)
{
  dprintf(("NTDLL: NtCreateSection(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SectionHandle,
           DesiredAccess,
           ObjectAttributes,
           MaximumSize,
           SectionPageProtection,
           AllocationAttributes,
           FileHandle));

  return 0;
}


/******************************************************************************
 *  NtOpenSection [NTDLL]
 */
NTSTATUS WINAPI NtOpenSection(PHANDLE SectionHandle,
                              ACCESS_MASK DesiredAccess,
                              POBJECT_ATTRIBUTES ObjectAttributes)
{
  dprintf(("NTDLL: NtOpenSection(%08xh,%08xh,%08xh) not implemented.\n",
           SectionHandle,
           DesiredAccess,
           ObjectAttributes));

  return 0;
}


/******************************************************************************
 *  NtQuerySection                       [NTDLL]
 */
NTSTATUS WINAPI NtQuerySection(HANDLE SectionHandle,
                               PVOID SectionInformationClass,
                               PVOID SectionInformation,
                               ULONG Length,
                               PULONG ResultLength)
{
  dprintf(("NTDLL: NtQuerySection(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SectionHandle,
           SectionInformationClass,
           SectionInformation,
           Length,
           ResultLength));

  return 0;
}


/******************************************************************************
 * NtMapViewOfSection                    [NTDLL]
 * FUNCTION: Maps a view of a section into the virtual address space of a process
 *
 * ARGUMENTS:
 *  SectionHandle Handle of the section
 *  ProcessHandle Handle of the process
 *  BaseAddress                          Desired base address (or NULL) on entry
 *       Actual base address of the view on exit
 *  ZeroBits      Number of high order address bits that must be zero
 *  CommitSize    Size bytes of the initially committed section of the view
 *  SectionOffset Offset bytes from the beginning of the section to the beginning of the view
 *  ViewSize      Desired length of map (or zero to map all) on entry
         Actual length mapped on exit
 *  InheritDisposition                   Specified how the view is to be shared with
 *       child processes
 *  AllocateType  Type of allocation for the pages
 *  Protect    Protection for the committed region of the view
 */
NTSTATUS WINAPI NtMapViewOfSection(HANDLE          SectionHandle,
                                   HANDLE          ProcessHandle,
                                   PVOID*          BaseAddress,
                                   ULONG           ZeroBits,
                                   ULONG           CommitSize,
                                   PLARGE_INTEGER  SectionOffset,
                                   PULONG          ViewSize,
                                   SECTION_INHERIT InheritDisposition,
                                   ULONG           AllocationType,
                                   ULONG           Protect)
{
  dprintf(("NTDLL: NtMapViewOfSection(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SectionHandle,
           ProcessHandle,
           BaseAddress,
           ZeroBits,
           CommitSize,
           SectionOffset,
           ViewSize,
           InheritDisposition,
           AllocationType,
           Protect));

  return 0;
}


/*
 * ports
 */

/******************************************************************************
 *  NtCreatePort                         [NTDLL]
 */
NTSTATUS WINAPI NtCreatePort(DWORD x1,
                             DWORD x2,
                             DWORD x3,
                             DWORD x4,
                             DWORD x5)
{
  dprintf(("NTDLL: NtCreatePort(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return 0;
}


/******************************************************************************
 *  NtConnectPort                        [NTDLL]
 */
NTSTATUS WINAPI NtConnectPort(DWORD           x1,
                              PUNICODE_STRING uni,
                              DWORD           x3,
                              DWORD           x4,
                              DWORD           x5,
                              DWORD           x6,
                              DWORD           x7,
                              DWORD           x8)
{
  dprintf(("NTDLL: NtConnectPort(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           uni,
           x3,
           x4,
           x5,
           x6,
           x7,
           x8));

  return 0;
}


/******************************************************************************
 *  NtListenPort                         [NTDLL]
 */
NTSTATUS WINAPI NtListenPort(DWORD x1,
                             DWORD x2)
{
  dprintf(("NTDLL: NtListenPort(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return 0;
}


/******************************************************************************
 *  NtAcceptConnectPort                  [NTDLL]
 */
NTSTATUS WINAPI NtAcceptConnectPort(DWORD x1,
                                    DWORD x2,
                                    DWORD x3,
                                    DWORD x4,
                                    DWORD x5,
                                    DWORD x6)
{
  dprintf(("NTDLL: NtAcceptConnectPort(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return 0;
}


/******************************************************************************
 *  NtCompleteConnectPort                [NTDLL]
 */
NTSTATUS WINAPI NtCompleteConnectPort(DWORD x1)
{
  dprintf(("NTDLL: NtCompleteConnectPort(%08xh) not implemented.\n",
           x1));

  return 0;
}


/******************************************************************************
 *  NtRegisterThreadTerminatePort        [NTDLL]
 */
NTSTATUS WINAPI NtRegisterThreadTerminatePort(DWORD x1)
{
  dprintf(("NTDLL: NtRegisterThreadTerminatePort(%08xh) not implemented.\n",
           x1));

  return 0;
}


/******************************************************************************
 *  NtRequestWaitReplyPort                  [NTDLL]
 */
NTSTATUS WINAPI NtRequestWaitReplyPort(DWORD x1,
                                       DWORD x2,
                                       DWORD x3)
{
  dprintf(("NTDLL: NtRequestWaitReplyPort(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 0;
}


/******************************************************************************
 *  NtReplyWaitReceivePort               [NTDLL]
 */
NTSTATUS WINAPI NtReplyWaitReceivePort(DWORD x1,
                                       DWORD x2,
                                       DWORD x3,
                                       DWORD x4)
{
  dprintf(("NTDLL: NtReplyWaitReceivePort(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/*
 * Misc
 */

 /******************************************************************************
 *  NtSetIntervalProfile                 [NTDLL]
 */
NTSTATUS WINAPI NtSetIntervalProfile(DWORD x1,
                                     DWORD x2)
{
  dprintf(("NTDLL: NtSetIntervalProfile(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return 0;
}


/******************************************************************************
 *  NtQueryPerformanceCounter            [NTDLL]
 */
NTSTATUS WINAPI NtQueryPerformanceCounter(PLARGE_INTEGER Counter,
                                          PLARGE_INTEGER Frequency)
{
  dprintf(("NTDLL: NtQueryPerformanceCounter(%08xh,%08xh) not implemented.\n",
           Counter,
           Frequency));

  return 0;
}


/******************************************************************************
 *  NtCreateMailSlotFile                 [NTDLL]
 */
NTSTATUS WINAPI NtCreateMailslotFile(DWORD x1,
                                     DWORD x2,
                                     DWORD x3,
                                     DWORD x4,
                                     DWORD x5,
                                     DWORD x6,
                                     DWORD x7,
                                     DWORD x8)
{
  dprintf(("NTDLL: NtCreateMailslotFile(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6,
           x7,
           x8));

  return 0;
}


/******************************************************************************
 * NtQuerySystemInformation [NTDLL.168]
 *
 * ARGUMENTS:
 *  SystemInformationClass               Index to a certain information structure
 * SystemTimeAdjustmentInformation       SYSTEM_TIME_ADJUSTMENT
 * SystemCacheInformation                   SYSTEM_CACHE_INFORMATION
 * SystemConfigurationInformation        CONFIGURATION_INFORMATION
 * observed (class/len):
 *    0x0/0x2c
 *    0x12/0x18
 *    0x2/0x138
 *    0x8/0x600
 *  SystemInformation                    caller supplies storage for the information structure
 *  Length     size of the structure
 *  ResultLength  Data written
 */
NTSTATUS WINAPI NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                         PVOID                    SystemInformation,
                                         ULONG                    Length,
                                         PULONG                   ResultLength)
{
  dprintf(("NTDLL: NtQuerySystemInformation(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SystemInformationClass,
           SystemInformation,
           Length,
           ResultLength));

  ZeroMemory (SystemInformation, Length);
  return 0;
}


/******************************************************************************
 *  NtCreatePagingFile                      [NTDLL]
 */
NTSTATUS WINAPI NtCreatePagingFile(PUNICODE_STRING PageFileName,
                                   ULONG MinimumSize,
                                   ULONG MaximumSize,
                                   PULONG ActualSize)
{
  dprintf(("NTDLL: NtCreatePagingFile(%08xh, %08xh, %08xh, %08xh) not implemented.\n",
           PageFileName,
           MinimumSize,
           MaximumSize,
           ActualSize));

  return 0;
}


/******************************************************************************
 *  NtDisplayString                               [NTDLL.95]
 *
 * writes a string to the nt-textmode screen eg. during startup
 */
NTSTATUS WINAPI NtDisplayString (PUNICODE_STRING string)
{
  dprintf(("NTDLL: NtDisplayString(%08xh\n",
           string));

  WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),
                string->Buffer,
                string->Length,
                0,
                0);

  return 0;
}
