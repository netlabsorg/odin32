/* $Id: os2wrap.h,v 1.16 2003-08-08 14:55:15 sandervl Exp $ */
#ifndef __OS2WRAP_H__
#define __OS2WRAP_H__

#ifdef __EMX__

#define OS2EMX_PLAIN_CHAR

#ifndef USE_OS2_TOOLKIT_HEADERS

// be compatible with the toolkit
#define OS2_INCLUDED
#define OS2DEF_INCLUDED
#define __OS2_H__

// add missing declarations
typedef unsigned short APIRET16;
#define APIENTRY16 _Far16 _Pascal
#define _LNK_CONV _Optlink

#define INCL_LONGLONG_STRUCTS

#endif // ifndef USE_OS2_TOOLKIT_HEADERS

#endif // ifdef __EMX__

#include <os2.h>
#include <os2sel.h>
#include <os2newapi.h>

// VAC: inline is a C++ keyword, must be translated to _Inline in C code
#if (defined(__IBMCPP__) || defined(__IBMC__))
#ifndef __cplusplus
#define inline  _Inline
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef INCL_DOSMEMMGR

APIRET APIENTRY _DosAliasMem(PVOID pb, ULONG cb, PPVOID ppbAlias, ULONG fl);

#undef  DosAliasMem
#define DosAliasMem _DosAliasMem

ULONG  APIENTRY _DosAllocMem(PPVOID a, ULONG b, ULONG c);

#undef  DosAllocMem
#define DosAllocMem _DosAllocMem

ULONG APIENTRY _DosAllocSharedMem(PPVOID a, PCSZ b, ULONG c, ULONG d);

#undef  DosAllocSharedMem
#define DosAllocSharedMem _DosAllocSharedMem

ULONG APIENTRY _DosFreeMem(PVOID a);

#undef  DosFreeMem
#define DosFreeMem _DosFreeMem

ULONG APIENTRY _DosGetNamedSharedMem(PPVOID a, PCSZ b, ULONG c);

#undef  DosGetNamedSharedMem
#define DosGetNamedSharedMem _DosGetNamedSharedMem

ULONG APIENTRY _DosGetSharedMem(PVOID a, ULONG b);

#undef  DosGetSharedMem
#define DosGetSharedMem _DosGetSharedMem

ULONG APIENTRY _DosGiveSharedMem(PVOID a, PID b, ULONG c);

#undef  DosGiveSharedMem
#define DosGiveSharedMem _DosGiveSharedMem

ULONG APIENTRY _DosQueryMem(PVOID a, PULONG b, PULONG c);

#undef  DosQueryMem
#define DosQueryMem _DosQueryMem

ULONG APIENTRY _DosSetMem(PVOID a, ULONG b, ULONG c);

#undef  DosSetMem
#define DosSetMem _DosSetMem

ULONG APIENTRY _DosSubAllocMem(PVOID a, PPVOID b, ULONG c);

#undef  DosSubAllocMem
#define DosSubAllocMem _DosSubAllocMem

ULONG APIENTRY _DosSubFreeMem(PVOID a, PVOID b, ULONG c);

#undef  DosSubFreeMem
#define DosSubFreeMem _DosSubFreeMem

ULONG APIENTRY _DosSubSetMem(PVOID a, ULONG b, ULONG c);

#undef  DosSubSetMem
#define DosSubSetMem _DosSubSetMem

ULONG APIENTRY _DosSubUnsetMem(PVOID a);

#undef  DosSubUnsetMem
#define DosSubUnsetMem _DosSubUnsetMem

#endif
#ifdef INCL_DOSFILEMGR
ULONG APIENTRY _DosCancelLockRequest(HFILE a, PFILELOCK b);

#undef  DosCancelLockRequest
#define DosCancelLockRequest _DosCancelLockRequest

ULONG APIENTRY _DosClose(HFILE a);

#undef  DosClose
#define DosClose _DosClose

ULONG APIENTRY _DosCopy(PCSZ a, PCSZ b, ULONG c);

#undef  DosCopy
#define DosCopy _DosCopy

ULONG APIENTRY _DosCreateDir(PCSZ a, PEAOP2 b);

#undef  DosCreateDir
#define DosCreateDir _DosCreateDir

ULONG APIENTRY _DosDelete(PCSZ a);

#undef  DosDelete
#define DosDelete _DosDelete

ULONG APIENTRY _DosDeleteDir(PCSZ a);

#undef  DosDeleteDir
#define DosDeleteDir _DosDeleteDir

ULONG APIENTRY _DosDupHandle(HFILE a, PHFILE b);

#undef  DosDupHandle
#define DosDupHandle _DosDupHandle

ULONG APIENTRY _DosEditName(ULONG a, PCSZ b, PCSZ c, PBYTE d, ULONG e);

#undef  DosEditName
#define DosEditName _DosEditName

ULONG APIENTRY _DosEnumAttribute(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g);

#undef  DosEnumAttribute
#define DosEnumAttribute _DosEnumAttribute

ULONG APIENTRY _DosFindClose(HDIR a);

#undef  DosFindClose
#define DosFindClose _DosFindClose

ULONG APIENTRY _DosFindFirst(PCSZ a, PHDIR b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g);

#undef  DosFindFirst
#define DosFindFirst _DosFindFirst

ULONG APIENTRY _DosFindNext(HDIR a, PVOID b, ULONG c, PULONG d);

#undef  DosFindNext
#define DosFindNext _DosFindNext

ULONG APIENTRY _DosForceDelete(PCSZ a);

#undef  DosForceDelete
#define DosForceDelete _DosForceDelete

ULONG APIENTRY _DosFSAttach(PCSZ a, PCSZ b, PVOID c, ULONG d, ULONG e);

#undef  DosFSAttach
#define DosFSAttach _DosFSAttach

ULONG APIENTRY _DosFSCtl(PVOID a, ULONG b, PULONG c, PVOID d, ULONG e, PULONG f, ULONG g, PCSZ h, HFILE i, ULONG j);

#undef  DosFSCtl
#define DosFSCtl _DosFSCtl

ULONG APIENTRY _DosMove(PCSZ a, PCSZ b);

#undef  DosMove
#define DosMove _DosMove

ULONG APIENTRY _DosOpen(PCSZ a, PHFILE b, PULONG c, ULONG d, ULONG e, ULONG f, ULONG g, PEAOP2 h);

#undef  DosOpen
#define DosOpen _DosOpen

ULONG APIENTRY _DosProtectClose(HFILE a, FHLOCK b);

#undef  DosProtectClose
#define DosProtectClose _DosProtectClose

ULONG APIENTRY _DosProtectEnumAttribute(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g, FHLOCK h);

#undef  DosProtectEnumAttribute
#define DosProtectEnumAttribute _DosProtectEnumAttribute

ULONG APIENTRY _DosProtectOpen(PCSZ a, PHFILE b, PULONG c, ULONG d, ULONG e, ULONG f, ULONG g, PEAOP2 h, PFHLOCK i);

#undef  DosProtectOpen
#define DosProtectOpen _DosProtectOpen

ULONG APIENTRY _DosProtectQueryFHState(HFILE a, PULONG b, FHLOCK c);

#undef  DosProtectQueryFHState
#define DosProtectQueryFHState _DosProtectQueryFHState

ULONG APIENTRY _DosProtectQueryFileInfo(HFILE a, ULONG b, PVOID c, ULONG d, FHLOCK e);

#undef  DosProtectQueryFileInfo
#define DosProtectQueryFileInfo _DosProtectQueryFileInfo

ULONG APIENTRY _DosProtectRead(HFILE a, PVOID b, ULONG c, PULONG d, FHLOCK e);

#undef  DosProtectRead
#define DosProtectRead _DosProtectRead

ULONG APIENTRY _DosProtectSetFHState(HFILE a, ULONG b, FHLOCK c);

#undef  DosProtectSetFHState
#define DosProtectSetFHState _DosProtectSetFHState

ULONG APIENTRY _DosProtectSetFileInfo(HFILE a, ULONG b, PVOID c, ULONG d, FHLOCK e);

#undef  DosProtectSetFileInfo
#define DosProtectSetFileInfo _DosProtectSetFileInfo

ULONG APIENTRY _DosProtectSetFileLocks(HFILE a, PFILELOCK b, PFILELOCK c, ULONG d, ULONG e, FHLOCK f);

#undef  DosProtectSetFileLocks
#define DosProtectSetFileLocks _DosProtectSetFileLocks

ULONG APIENTRY _DosProtectSetFilePtr(HFILE a, LONG b, ULONG c, PULONG d, FHLOCK e);

#undef  DosProtectSetFilePtr
#define DosProtectSetFilePtr _DosProtectSetFilePtr

ULONG APIENTRY _DosProtectSetFileSize(HFILE a, ULONG b, FHLOCK c);

#undef  DosProtectSetFileSize
#define DosProtectSetFileSize _DosProtectSetFileSize

ULONG APIENTRY _DosProtectWrite(HFILE a, PVOID b, ULONG c, PULONG d, FHLOCK e);

#undef  DosProtectWrite
#define DosProtectWrite _DosProtectWrite

ULONG APIENTRY _DosQueryCurrentDir(ULONG a, PBYTE b, PULONG c);

#undef  DosQueryCurrentDir
#define DosQueryCurrentDir _DosQueryCurrentDir

ULONG APIENTRY _DosQueryCurrentDisk(PULONG a, PULONG b);

#undef  DosQueryCurrentDisk
#define DosQueryCurrentDisk _DosQueryCurrentDisk

ULONG APIENTRY _DosQueryFHState(HFILE a, PULONG b);

#undef  DosQueryFHState
#define DosQueryFHState _DosQueryFHState

ULONG APIENTRY _DosQueryFileInfo(HFILE a, ULONG b, PVOID c, ULONG d);

#undef  DosQueryFileInfo
#define DosQueryFileInfo _DosQueryFileInfo

ULONG APIENTRY _DosQueryFSAttach(PCSZ a, ULONG b, ULONG c, PFSQBUFFER2 d, PULONG e);

#undef  DosQueryFSAttach
#define DosQueryFSAttach _DosQueryFSAttach

ULONG APIENTRY _DosQueryFSInfo(ULONG a, ULONG b, PVOID c, ULONG d);

#undef  DosQueryFSInfo
#define DosQueryFSInfo _DosQueryFSInfo

ULONG APIENTRY _DosQueryHType(HFILE a, PULONG b, PULONG c);

#undef  DosQueryHType
#define DosQueryHType _DosQueryHType

ULONG APIENTRY _DosQueryPathInfo(PCSZ a, ULONG b, PVOID c, ULONG d);

#undef  DosQueryPathInfo
#define DosQueryPathInfo _DosQueryPathInfo

ULONG APIENTRY _DosQueryVerify(PBOOL32 a);

#undef  DosQueryVerify
#define DosQueryVerify _DosQueryVerify

ULONG APIENTRY _DosRead(HFILE a, PVOID b, ULONG c, PULONG d);

#undef  DosRead
#define DosRead _DosRead

ULONG APIENTRY _DosResetBuffer(HFILE a);

#undef  DosResetBuffer
#define DosResetBuffer _DosResetBuffer

ULONG APIENTRY _DosSetCurrentDir(PCSZ a);

#undef  DosSetCurrentDir
#define DosSetCurrentDir _DosSetCurrentDir

ULONG APIENTRY _DosSetDefaultDisk(ULONG a);

#undef  DosSetDefaultDisk
#define DosSetDefaultDisk _DosSetDefaultDisk

ULONG APIENTRY _DosSetFHState(HFILE a, ULONG b);

#undef  DosSetFHState
#define DosSetFHState _DosSetFHState

ULONG APIENTRY _DosSetFileInfo(HFILE a, ULONG b, PVOID c, ULONG d);

#undef  DosSetFileInfo
#define DosSetFileInfo _DosSetFileInfo

ULONG APIENTRY _DosSetFileLocks(HFILE a, PFILELOCK b, PFILELOCK c, ULONG d, ULONG e);

#undef  DosSetFileLocks
#define DosSetFileLocks _DosSetFileLocks

ULONG APIENTRY _DosSetFilePtr(HFILE a, LONG b, ULONG c, PULONG d);

#undef  DosSetFilePtr
#define DosSetFilePtr _DosSetFilePtr

ULONG APIENTRY _DosSetFileSize(HFILE a, ULONG b);

#undef  DosSetFileSize
#define DosSetFileSize _DosSetFileSize

ULONG APIENTRY _DosSetFSInfo(ULONG a, ULONG b, PVOID c, ULONG d);

#undef  DosSetFSInfo
#define DosSetFSInfo _DosSetFSInfo

ULONG APIENTRY _DosSetMaxFH(ULONG a);

#undef  DosSetMaxFH
#define DosSetMaxFH _DosSetMaxFH

ULONG APIENTRY _DosSetPathInfo(PCSZ a, ULONG b, PVOID c, ULONG d, ULONG e);

#undef  DosSetPathInfo
#define DosSetPathInfo _DosSetPathInfo

ULONG APIENTRY _DosSetRelMaxFH(PLONG a, PULONG b);

#undef  DosSetRelMaxFH
#define DosSetRelMaxFH _DosSetRelMaxFH

ULONG APIENTRY _DosSetVerify(BOOL32 a);

#undef  DosSetVerify
#define DosSetVerify _DosSetVerify

ULONG APIENTRY _DosShutdown(ULONG a);

#undef  DosShutdown
#define DosShutdown _DosShutdown

ULONG APIENTRY _DosWrite(HFILE a, PVOID b, ULONG c, PULONG d);

#undef  DosWrite
#define DosWrite _DosWrite

#endif
#ifdef INCL_DOSMISC
ULONG APIENTRY _DosSearchPath(ULONG a, PCSZ b, PCSZ c, PBYTE d, ULONG e);

#undef  DosSearchPath
#define DosSearchPath _DosSearchPath

ULONG APIENTRY  _DosError(ULONG error);

#undef  DosError
#define DosError _DosError

#endif
#ifdef INCL_DOSDEVICES
ULONG APIENTRY _DosDevConfig(PVOID a, ULONG b);

#undef  DosDevConfig
#define DosDevConfig _DosDevConfig

ULONG APIENTRY _DosDevIOCtl(HFILE a, ULONG b, ULONG c, PVOID d, ULONG e, PULONG f, PVOID g, ULONG h, PULONG i);

#undef  DosDevIOCtl
#define DosDevIOCtl _DosDevIOCtl

ULONG APIENTRY _DosPhysicalDisk(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e);

#undef  DosPhysicalDisk
#define DosPhysicalDisk _DosPhysicalDisk

#endif
#ifdef INCL_DOSNLS
ULONG APIENTRY _DosMapCase(ULONG a, PCOUNTRYCODE b, PCHAR c);

#undef  DosMapCase
#define DosMapCase _DosMapCase

ULONG APIENTRY _DosQueryCollate(ULONG a, PCOUNTRYCODE b, PCHAR c, PULONG d);

#undef  DosQueryCollate
#define DosQueryCollate _DosQueryCollate

ULONG APIENTRY _DosQueryCp(ULONG a, PULONG b, PULONG c);

#undef  DosQueryCp
#define DosQueryCp _DosQueryCp

ULONG APIENTRY _DosQueryCtryInfo(ULONG a, PCOUNTRYCODE b, PCOUNTRYINFO c, PULONG d);

#undef  DosQueryCtryInfo
#define DosQueryCtryInfo _DosQueryCtryInfo

ULONG APIENTRY _DosQueryDBCSEnv(ULONG a, PCOUNTRYCODE b, PCHAR c);

#undef  DosQueryDBCSEnv
#define DosQueryDBCSEnv _DosQueryDBCSEnv

ULONG APIENTRY _DosSetProcessCp(ULONG a);

#undef  DosSetProcessCp
#define DosSetProcessCp _DosSetProcessCp

#endif
#ifdef INCL_DOSMODULEMGR
ULONG APIENTRY _DosFreeModule(HMODULE a);

#undef  DosFreeModule
#define DosFreeModule _DosFreeModule

ULONG APIENTRY _DosLoadModule(PSZ a, ULONG b, PCSZ c, PHMODULE d);

#undef  DosLoadModule
#define DosLoadModule _DosLoadModule

ULONG APIENTRY _DosQueryModFromEIP(PHMODULE a, PULONG b, ULONG c, PCHAR d, PULONG e, ULONG f);

#undef  DosQueryModFromEIP
#define DosQueryModFromEIP _DosQueryModFromEIP

ULONG APIENTRY _DosQueryModuleHandle(PCSZ a, PHMODULE b);

#undef  DosQueryModuleHandle
#define DosQueryModuleHandle _DosQueryModuleHandle

ULONG APIENTRY _DosQueryModuleName(HMODULE a, ULONG b, PCHAR c);

#undef  DosQueryModuleName
#define DosQueryModuleName _DosQueryModuleName

ULONG APIENTRY _DosQueryProcAddr(HMODULE a, ULONG b, PCSZ c, PFN *d);

#undef  DosQueryProcAddr
#define DosQueryProcAddr _DosQueryProcAddr

ULONG APIENTRY _DosQueryProcType(HMODULE a, ULONG b, PCSZ c, PULONG d);

#undef  DosQueryProcType
#define DosQueryProcType _DosQueryProcType

#endif
#ifdef INCL_DOSRESOURCES
ULONG APIENTRY _DosFreeResource(PVOID a);

#undef  DosFreeResource
#define DosFreeResource _DosFreeResource

ULONG APIENTRY _DosGetResource(HMODULE a, ULONG b, ULONG c, PPVOID d);

#undef  DosGetResource
#define DosGetResource _DosGetResource

ULONG APIENTRY _DosQueryResourceSize(HMODULE a, ULONG b, ULONG c, PULONG d);

#undef  DosQueryResourceSize
#define DosQueryResourceSize _DosQueryResourceSize

#endif
#ifdef INCL_DOSPROCESS
ULONG APIENTRY _DosBeep(ULONG a, ULONG b);

#undef  DosBeep
#define DosBeep _DosBeep

VOID APIENTRY _DosExit(ULONG a, ULONG b);

#undef  DosExit
#define DosExit _DosExit

ULONG APIENTRY _DosAllocThreadLocalMemory(ULONG a, PULONG *b);

#undef  DosAllocThreadLocalMemory
#define DosAllocThreadLocalMemory _DosAllocThreadLocalMemory

ULONG APIENTRY _DosCreateThread(PTID a, PFNTHREAD b, ULONG c, ULONG d, ULONG e);

#undef  DosCreateThread
#define DosCreateThread _DosCreateThread

ULONG APIENTRY _DosEnterCritSec();

#undef  DosEnterCritSec
#define DosEnterCritSec _DosEnterCritSec

ULONG APIENTRY _DosExecPgm(PCHAR a, LONG b, ULONG c, PCSZ d, PCSZ e, PRESULTCODES f, PCSZ g);

#undef  DosExecPgm
#define DosExecPgm _DosExecPgm

ULONG APIENTRY _DosExitCritSec();

#undef  DosExitCritSec
#define DosExitCritSec _DosExitCritSec

ULONG APIENTRY _DosExitList(ULONG a, PFNEXITLIST b);

#undef  DosExitList
#define DosExitList _DosExitList

ULONG APIENTRY _DosFreeThreadLocalMemory(ULONG *a);

#undef  DosFreeThreadLocalMemory
#define DosFreeThreadLocalMemory _DosFreeThreadLocalMemory

ULONG APIENTRY _DosGetInfoBlocks(PTIB *a, PPIB *b);

#undef  DosGetInfoBlocks
#define DosGetInfoBlocks _DosGetInfoBlocks

ULONG APIENTRY _DosKillProcess(ULONG a, PID b);

#undef  DosKillProcess
#define DosKillProcess _DosKillProcess

ULONG APIENTRY _DosKillThread(TID a);

#undef  DosKillThread
#define DosKillThread _DosKillThread

ULONG APIENTRY _DosResumeThread(TID a);

#undef  DosResumeThread
#define DosResumeThread _DosResumeThread

ULONG APIENTRY _DosSetPriority(ULONG a, ULONG b, LONG c, ULONG d);

#undef  DosSetPriority
#define DosSetPriority _DosSetPriority

ULONG APIENTRY _DosSleep(ULONG a);

#undef  DosSleep
#define DosSleep _DosSleep

ULONG APIENTRY _DosSuspendThread(TID a);

#undef  DosSuspendThread
#define DosSuspendThread _DosSuspendThread

ULONG APIENTRY _DosWaitChild(ULONG a, ULONG b, PRESULTCODES c, PPID d, PID e);

#undef  DosWaitChild
#define DosWaitChild _DosWaitChild

ULONG APIENTRY _DosWaitThread(PTID a, ULONG b);

#undef  DosWaitThread
#define DosWaitThread _DosWaitThread

#endif

#ifdef INCL_DOSSESMGR
ULONG APIENTRY _DosQueryAppType(PCSZ a, PULONG b);

#undef  DosQueryAppType
#define DosQueryAppType _DosQueryAppType

ULONG APIENTRY _DosSelectSession(ULONG a);

#undef  DosSelectSession
#define DosSelectSession _DosSelectSession

ULONG APIENTRY _DosSetSession(ULONG a, PSTATUSDATA b);

#undef  DosSetSession
#define DosSetSession _DosSetSession

ULONG APIENTRY _DosStartSession(PSTARTDATA a, PULONG b, PPID c);

#undef  DosStartSession
#define DosStartSession _DosStartSession

ULONG APIENTRY _DosStopSession(ULONG a, ULONG b);

#undef  DosStopSession
#define DosStopSession _DosStopSession

#endif
#ifdef INCL_DOSSEMAPHORES
ULONG APIENTRY _DosCloseEventSem(HEV a);

#undef  DosCloseEventSem
#define DosCloseEventSem _DosCloseEventSem

ULONG APIENTRY _DosCreateEventSem(PCSZ a, PHEV b, ULONG c, BOOL32 d);

#undef  DosCreateEventSem
#define DosCreateEventSem _DosCreateEventSem

ULONG APIENTRY _DosOpenEventSem(PCSZ a, PHEV b);

#undef  DosOpenEventSem
#define DosOpenEventSem _DosOpenEventSem

ULONG APIENTRY _DosPostEventSem(HEV a);

#undef  DosPostEventSem
#define DosPostEventSem _DosPostEventSem

ULONG APIENTRY _DosQueryEventSem(HEV a, PULONG b);

#undef  DosQueryEventSem
#define DosQueryEventSem _DosQueryEventSem

ULONG APIENTRY _DosResetEventSem(HEV a, PULONG b);

#undef  DosResetEventSem
#define DosResetEventSem _DosResetEventSem

ULONG APIENTRY _DosWaitEventSem(HEV a, ULONG b);

#undef  DosWaitEventSem
#define DosWaitEventSem _DosWaitEventSem

ULONG APIENTRY _DosCloseMutexSem(HMTX a);

#undef  DosCloseMutexSem
#define DosCloseMutexSem _DosCloseMutexSem

ULONG APIENTRY _DosCreateMutexSem(PCSZ a, PHMTX b, ULONG c, BOOL32 d);

#undef  DosCreateMutexSem
#define DosCreateMutexSem _DosCreateMutexSem

ULONG APIENTRY _DosOpenMutexSem(PCSZ a, PHMTX b);

#undef  DosOpenMutexSem
#define DosOpenMutexSem _DosOpenMutexSem

ULONG APIENTRY _DosQueryMutexSem(HMTX a, PPID b, PTID c, PULONG d);

#undef  DosQueryMutexSem
#define DosQueryMutexSem _DosQueryMutexSem

ULONG APIENTRY _DosReleaseMutexSem(HMTX a);

#undef  DosReleaseMutexSem
#define DosReleaseMutexSem _DosReleaseMutexSem

ULONG APIENTRY _DosRequestMutexSem(HMTX a, ULONG b);

#undef  DosRequestMutexSem
#define DosRequestMutexSem _DosRequestMutexSem

ULONG APIENTRY _DosAddMuxWaitSem(HMUX a, PSEMRECORD b);

#undef  DosAddMuxWaitSem
#define DosAddMuxWaitSem _DosAddMuxWaitSem

ULONG APIENTRY _DosCloseMuxWaitSem(HMUX a);

#undef  DosCloseMuxWaitSem
#define DosCloseMuxWaitSem _DosCloseMuxWaitSem

ULONG APIENTRY _DosCreateMuxWaitSem(PCSZ a, PHMUX b, ULONG c, PSEMRECORD d, ULONG e);

#undef  DosCreateMuxWaitSem
#define DosCreateMuxWaitSem _DosCreateMuxWaitSem

ULONG APIENTRY _DosDeleteMuxWaitSem(HMUX a, HSEM b);

#undef  DosDeleteMuxWaitSem
#define DosDeleteMuxWaitSem _DosDeleteMuxWaitSem

ULONG APIENTRY _DosOpenMuxWaitSem(PCSZ a, PHMUX b);

#undef  DosOpenMuxWaitSem
#define DosOpenMuxWaitSem _DosOpenMuxWaitSem

ULONG APIENTRY _DosQueryMuxWaitSem(HMUX a, PULONG b, PSEMRECORD c, PULONG d);

#undef  DosQueryMuxWaitSem
#define DosQueryMuxWaitSem _DosQueryMuxWaitSem

ULONG APIENTRY _DosWaitMuxWaitSem(HMUX a, ULONG b, PULONG c);

#undef  DosWaitMuxWaitSem
#define DosWaitMuxWaitSem _DosWaitMuxWaitSem

#endif
#ifdef INCL_DOSNMPIPES
ULONG APIENTRY _DosCallNPipe(PCSZ a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g);

#undef  DosCallNPipe
#define DosCallNPipe _DosCallNPipe

ULONG APIENTRY _DosConnectNPipe(HPIPE a);

#undef  DosConnectNPipe
#define DosConnectNPipe _DosConnectNPipe

ULONG APIENTRY _DosCreateNPipe(PCSZ a, PHPIPE b, ULONG c, ULONG d, ULONG e, ULONG f, ULONG g);

#undef  DosCreateNPipe
#define DosCreateNPipe _DosCreateNPipe

ULONG APIENTRY _DosDisConnectNPipe(HPIPE a);

#undef  DosDisConnectNPipe
#define DosDisConnectNPipe _DosDisConnectNPipe

ULONG APIENTRY _DosPeekNPipe(HPIPE a, PVOID b, ULONG c, PULONG d, PAVAILDATA e, PULONG f);

#undef  DosPeekNPipe
#define DosPeekNPipe _DosPeekNPipe

ULONG APIENTRY _DosQueryNPHState(HPIPE a, PULONG b);

#undef  DosQueryNPHState
#define DosQueryNPHState _DosQueryNPHState

ULONG APIENTRY _DosQueryNPipeInfo(HPIPE a, ULONG b, PVOID c, ULONG d);

#undef  DosQueryNPipeInfo
#define DosQueryNPipeInfo _DosQueryNPipeInfo

ULONG APIENTRY _DosQueryNPipeSemState(HSEM a, PPIPESEMSTATE b, ULONG c);

#undef  DosQueryNPipeSemState
#define DosQueryNPipeSemState _DosQueryNPipeSemState

ULONG APIENTRY _DosSetNPHState(HPIPE a, ULONG b);

#undef  DosSetNPHState
#define DosSetNPHState _DosSetNPHState

ULONG APIENTRY _DosSetNPipeSem(HPIPE a, HSEM b, ULONG c);

#undef  DosSetNPipeSem
#define DosSetNPipeSem _DosSetNPipeSem

ULONG APIENTRY _DosTransactNPipe(HPIPE a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f);

#undef  DosTransactNPipe
#define DosTransactNPipe _DosTransactNPipe

ULONG APIENTRY _DosWaitNPipe(PCSZ a, ULONG b);

#undef  DosWaitNPipe
#define DosWaitNPipe _DosWaitNPipe

ULONG APIENTRY _DosCreatePipe(PHFILE a, PHFILE b, ULONG c);

#undef  DosCreatePipe
#define DosCreatePipe _DosCreatePipe

#endif
#ifdef INCL_DOSQUEUES
ULONG APIENTRY _DosCloseQueue(HQUEUE a);

#undef  DosCloseQueue
#define DosCloseQueue _DosCloseQueue

ULONG APIENTRY _DosCreateQueue(PHQUEUE a, ULONG b, PCSZ c);

#undef  DosCreateQueue
#define DosCreateQueue _DosCreateQueue

ULONG APIENTRY _DosOpenQueue(PPID a, PHQUEUE b, PCSZ c);

#undef  DosOpenQueue
#define DosOpenQueue _DosOpenQueue

ULONG APIENTRY _DosPeekQueue(HQUEUE a, PREQUESTDATA b, PULONG c, PPVOID d, PULONG e, BOOL32 f, PBYTE g, HEV h);

#undef  DosPeekQueue
#define DosPeekQueue _DosPeekQueue

ULONG APIENTRY _DosPurgeQueue(HQUEUE a);

#undef  DosPurgeQueue
#define DosPurgeQueue _DosPurgeQueue

ULONG APIENTRY _DosQueryQueue(HQUEUE a, PULONG b);

#undef  DosQueryQueue
#define DosQueryQueue _DosQueryQueue

ULONG APIENTRY _DosReadQueue(HQUEUE a, PREQUESTDATA b, PULONG c, PPVOID d, ULONG e, BOOL32 f, PBYTE g, HEV h);

#undef  DosReadQueue
#define DosReadQueue _DosReadQueue

ULONG APIENTRY _DosWriteQueue(HQUEUE a, ULONG b, ULONG c, PVOID d, ULONG e);

#undef  DosWriteQueue
#define DosWriteQueue _DosWriteQueue

#endif
#ifdef INCL_DOSEXCEPTIONS
ULONG APIENTRY _DosAcknowledgeSignalException(ULONG a);

#undef  DosAcknowledgeSignalException
#define DosAcknowledgeSignalException _DosAcknowledgeSignalException

ULONG APIENTRY _DosEnterMustComplete(PULONG a);

#undef  DosEnterMustComplete
#define DosEnterMustComplete _DosEnterMustComplete

ULONG APIENTRY _DosExitMustComplete(PULONG a);

#undef  DosExitMustComplete
#define DosExitMustComplete _DosExitMustComplete

ULONG APIENTRY _DosQueryThreadContext(TID a, ULONG b, PCONTEXTRECORD c);

#undef  DosQueryThreadContext
#define DosQueryThreadContext _DosQueryThreadContext

ULONG APIENTRY _DosRaiseException(PEXCEPTIONREPORTRECORD a);

#undef  DosRaiseException
#define DosRaiseException _DosRaiseException

ULONG APIENTRY _DosSendSignalException(PID a, ULONG b);

#undef  DosSendSignalException
#define DosSendSignalException _DosSendSignalException

ULONG APIENTRY _DosSetExceptionHandler(PEXCEPTIONREGISTRATIONRECORD a);

#undef  DosSetExceptionHandler
#define DosSetExceptionHandler _DosSetExceptionHandler

ULONG APIENTRY _DosSetSignalExceptionFocus(BOOL32 a, PULONG b);

#undef  DosSetSignalExceptionFocus
#define DosSetSignalExceptionFocus _DosSetSignalExceptionFocus

ULONG APIENTRY _DosUnsetExceptionHandler(PEXCEPTIONREGISTRATIONRECORD a);

#undef  DosUnsetExceptionHandler
#define DosUnsetExceptionHandler _DosUnsetExceptionHandler

ULONG APIENTRY _DosUnwindException(PEXCEPTIONREGISTRATIONRECORD a, PVOID b, PEXCEPTIONREPORTRECORD c);

#undef  DosUnwindException
#define DosUnwindException _DosUnwindException

#endif
#ifdef INCL_DOSMISC
ULONG APIENTRY _DosQuerySysInfo(ULONG a, ULONG b, PVOID c, ULONG d);

#undef  DosQuerySysInfo
#define DosQuerySysInfo _DosQuerySysInfo

ULONG APIENTRY _DosScanEnv(PCSZ a, PCSZ *b);

#undef  DosScanEnv
#define DosScanEnv _DosScanEnv

ULONG APIENTRY _DosQueryRASInfo(ULONG a, PPVOID b);

#undef  DosQueryRASInfo
#define DosQueryRASInfo _DosQueryRASInfo

#endif
#ifdef INCL_DOSDATETIME
ULONG APIENTRY _DosGetDateTime(PDATETIME a);

#undef  DosGetDateTime
#define DosGetDateTime _DosGetDateTime

ULONG APIENTRY _DosSetDateTime(PDATETIME a);

#undef  DosSetDateTime
#define DosSetDateTime _DosSetDateTime

ULONG APIENTRY _DosAsyncTimer(ULONG a, HSEM b, PHTIMER c);

#undef  DosAsyncTimer
#define DosAsyncTimer _DosAsyncTimer

ULONG APIENTRY _DosStartTimer(ULONG a, HSEM b, PHTIMER c);

#undef  DosStartTimer
#define DosStartTimer _DosStartTimer

ULONG APIENTRY _DosStopTimer(HTIMER a);

#undef  DosStopTimer
#define DosStopTimer _DosStopTimer

#endif
#ifdef DosTmrQueryFreq
ULONG APIENTRY _DosTmrQueryFreq(PULONG a);

#undef  DosTmrQueryFreq
#define DosTmrQueryFreq _DosTmrQueryFreq

ULONG APIENTRY _DosTmrQueryTime(PQWORD a);

#undef  DosTmrQueryTime
#define DosTmrQueryTime _DosTmrQueryTime

#endif
#ifdef INCL_DOSMVDM
ULONG APIENTRY _DosCloseVDD(HVDD a);

#undef  DosCloseVDD
#define DosCloseVDD _DosCloseVDD

ULONG APIENTRY _DosOpenVDD(PCSZ a, PHVDD b);

#undef  DosOpenVDD
#define DosOpenVDD _DosOpenVDD

ULONG APIENTRY _DosQueryDOSProperty(SGID a, PCSZ b, ULONG c, PSZ d);

#undef  DosQueryDOSProperty
#define DosQueryDOSProperty _DosQueryDOSProperty

ULONG APIENTRY _DosRequestVDD(HVDD a, SGID b, ULONG c, ULONG d, PVOID e, ULONG f, PVOID g);

#undef  DosRequestVDD
#define DosRequestVDD _DosRequestVDD

ULONG APIENTRY _DosSetDOSProperty(SGID a, PCSZ b, ULONG c, PCSZ d);

#undef  DosSetDOSProperty
#define DosSetDOSProperty _DosSetDOSProperty

#endif
#ifdef INCL_DOSPROCESS
ULONG APIENTRY _DosDebug(uDB_t *a);

#undef  DosDebug
#define DosDebug _DosDebug

#endif
#ifdef INCL_DOSMISC
ULONG APIENTRY _DosGetMessage(PCHAR *a, ULONG b, PCHAR c, ULONG d, ULONG e, PCSZ f, PULONG g);

#undef  DosGetMessage
#define DosGetMessage _DosGetMessage

ULONG APIENTRY _DosInsertMessage(PCHAR *a, ULONG b, PCSZ c, ULONG d, PCHAR e, ULONG f, PULONG g);

#undef  DosInsertMessage
#define DosInsertMessage _DosInsertMessage

ULONG APIENTRY _DosPutMessage(HFILE a, ULONG b, PCHAR c);

#undef  DosPutMessage
#define DosPutMessage _DosPutMessage

ULONG APIENTRY _DosQueryMessageCP(PCHAR a, ULONG b, PCSZ c, PULONG d);

#undef  DosQueryMessageCP
#define DosQueryMessageCP _DosQueryMessageCP

ULONG APIENTRY _DosVerifyPidTid(PID Pid, TID Tid);
#undef  DosVerifyPidTid
#define DosVerifyPidTid _DosVerifyPidTid

#endif
#ifdef INCL_DOSRAS
ULONG APIENTRY _DosDumpProcess(ULONG a, ULONG b, PID c);

#undef  DosDumpProcess
#define DosDumpProcess _DosDumpProcess

ULONG APIENTRY _DosForceSystemDump(ULONG a);

#undef  DosForceSystemDump
#define DosForceSystemDump _DosForceSystemDump

ULONG APIENTRY _DosQueryRASInfo(ULONG a, PPVOID b);

#undef  DosQueryRASInfo
#define DosQueryRASInfo _DosQueryRASInfo

ULONG APIENTRY _DosSuppressPopUps(ULONG a, ULONG b);

#undef  DosSuppressPopUps
#define DosSuppressPopUps _DosSuppressPopUps

#endif
#ifdef INCL_DOSPROFILE
ULONG APIENTRY _DosQuerySysState(ULONG a, ULONG b, PID c, TID d, PVOID e, ULONG f);

#undef  DosQuerySysState
#define DosQuerySysState _DosQuerySysState

#endif
#ifdef INCL_RXSUBCOM
ULONG APIENTRY _RexxDeregisterSubcom(PCSZ a, PCSZ b);

#undef  RexxDeregisterSubcom
#define RexxDeregisterSubcom _RexxDeregisterSubcom

ULONG APIENTRY _RexxQuerySubcom(PCSZ a, PCSZ b, PUSHORT c, PUCHAR d);

#undef  RexxQuerySubcom
#define RexxQuerySubcom _RexxQuerySubcom

ULONG APIENTRY _RexxRegisterSubcomDll(PCSZ a, PCSZ b, PCSZ c, PUCHAR d, ULONG e);

#undef  RexxRegisterSubcomDll
#define RexxRegisterSubcomDll _RexxRegisterSubcomDll

ULONG APIENTRY _RexxRegisterSubcomExe(PCSZ a, PFN b, PUCHAR c);

#undef  RexxRegisterSubcomExe
#define RexxRegisterSubcomExe _RexxRegisterSubcomExe

#endif
#ifdef INCL_RXSHV
ULONG APIENTRY _RexxVariablePool(PSHVBLOCK a);

#undef  RexxVariablePool
#define RexxVariablePool _RexxVariablePool

#endif
#ifdef INCL_RXFUNC
ULONG APIENTRY _RexxDeregisterFunction(PCSZ a);

#undef  RexxDeregisterFunction
#define RexxDeregisterFunction _RexxDeregisterFunction

ULONG APIENTRY _RexxQueryFunction(PCSZ a);

#undef  RexxQueryFunction
#define RexxQueryFunction _RexxQueryFunction

ULONG APIENTRY _RexxRegisterFunctionDll(PCSZ a, PCSZ b, PCSZ c);

#undef  RexxRegisterFunctionDll
#define RexxRegisterFunctionDll _RexxRegisterFunctionDll

ULONG APIENTRY _RexxRegisterFunctionExe(PCSZ a, RexxFunctionHandler *b);

#undef  RexxRegisterFunctionExe
#define RexxRegisterFunctionExe _RexxRegisterFunctionExe

#endif
#ifdef INCL_RXSYSEXIT
ULONG APIENTRY _RexxDeregisterExit(PCSZ a, PCSZ b);

#undef  RexxDeregisterExit
#define RexxDeregisterExit _RexxDeregisterExit

ULONG APIENTRY _RexxQueryExit(PCSZ a, PCSZ b, PUSHORT c, PUCHAR d);

#undef  RexxQueryExit
#define RexxQueryExit _RexxQueryExit

ULONG APIENTRY _RexxRegisterExitDll(PCSZ a, PCSZ b, PCSZ c, PUCHAR d, ULONG e);

#undef  RexxRegisterExitDll
#define RexxRegisterExitDll _RexxRegisterExitDll

ULONG APIENTRY _RexxRegisterExitExe(PCSZ a, PFN b, PUCHAR c);

#undef  RexxRegisterExitExe
#define RexxRegisterExitExe _RexxRegisterExitExe

#endif
#ifdef INCL_RXARI
ULONG APIENTRY _RexxResetTrace(PID a, TID b);

#undef  RexxResetTrace
#define RexxResetTrace _RexxResetTrace

ULONG APIENTRY _RexxSetHalt(PID a, TID b);

#undef  RexxSetHalt
#define RexxSetHalt _RexxSetHalt

ULONG APIENTRY _RexxSetTrace(PID a, TID b);

#undef  RexxSetTrace
#define RexxSetTrace _RexxSetTrace

#endif
#ifdef INCL_RXMACRO
ULONG APIENTRY _RexxAddMacro(PCSZ a, PCSZ b, ULONG c);

#undef  RexxAddMacro
#define RexxAddMacro _RexxAddMacro

ULONG APIENTRY _RexxClearMacroSpace();

#undef  RexxClearMacroSpace
#define RexxClearMacroSpace _RexxClearMacroSpace

ULONG APIENTRY _RexxDropMacro(PCSZ a);

#undef  RexxDropMacro
#define RexxDropMacro _RexxDropMacro

ULONG APIENTRY _RexxLoadMacroSpace(ULONG a, PCSZ *b, PCSZ c);

#undef  RexxLoadMacroSpace
#define RexxLoadMacroSpace _RexxLoadMacroSpace

ULONG APIENTRY _RexxQueryMacro(PCSZ a, PUSHORT b);

#undef  RexxQueryMacro
#define RexxQueryMacro _RexxQueryMacro

ULONG APIENTRY _RexxReorderMacro(PCSZ a, ULONG b);

#undef  RexxReorderMacro
#define RexxReorderMacro _RexxReorderMacro

ULONG APIENTRY _RexxSaveMacroSpace(ULONG a, PCSZ *b, PCSZ c);

#undef  RexxSaveMacroSpace
#define RexxSaveMacroSpace _RexxSaveMacroSpace

LONG APIENTRY _RexxStart(LONG a, PRXSTRING b, PCSZ c, PRXSTRING d, PCSZ e, LONG f, PRXSYSEXIT g, PSHORT h, PRXSTRING i);

#undef  RexxStart
#define RexxStart _RexxStart

#endif
#ifdef INCL_WIN
#ifdef INCL_WINMESSAGEMGR
BOOL APIENTRY _WinCancelShutdown(HMQ a, BOOL b);

#undef  WinCancelShutdown
#define WinCancelShutdown _WinCancelShutdown

HMQ APIENTRY _WinCreateMsgQueue(HAB a, LONG b);

#undef  WinCreateMsgQueue
#define WinCreateMsgQueue _WinCreateMsgQueue

BOOL APIENTRY _WinDestroyMsgQueue(HMQ a);

#undef  WinDestroyMsgQueue
#define WinDestroyMsgQueue _WinDestroyMsgQueue

MRESULT APIENTRY _WinDispatchMsg(HAB a, PQMSG b);

#undef  WinDispatchMsg
#define WinDispatchMsg _WinDispatchMsg

BOOL APIENTRY _WinGetMsg(HAB a, PQMSG b, HWND c, ULONG d, ULONG e);

#undef  WinGetMsg
#define WinGetMsg _WinGetMsg

BOOL APIENTRY _WinLockInput(HMQ a, ULONG b);

#undef  WinLockInput
#define WinLockInput _WinLockInput

BOOL APIENTRY _WinPeekMsg(HAB a, PQMSG b, HWND c, ULONG d, ULONG e, ULONG f);

#undef  WinPeekMsg
#define WinPeekMsg _WinPeekMsg

BOOL APIENTRY _WinPostMsg(HWND a, ULONG b, MPARAM c, MPARAM d);

#undef  WinPostMsg
#define WinPostMsg _WinPostMsg

HMQ APIENTRY _WinQueueFromID(HAB a, PID b, TID c);

#undef  WinQueueFromID
#define WinQueueFromID _WinQueueFromID

BOOL APIENTRY _WinQueryQueueInfo(HMQ a, PMQINFO b, ULONG c);

#undef  WinQueryQueueInfo
#define WinQueryQueueInfo _WinQueryQueueInfo

HMQ APIENTRY _WinQuerySendMsg(HAB a, HMQ b, HMQ c, PQMSG d);

#undef  WinQuerySendMsg
#define WinQuerySendMsg _WinQuerySendMsg

BOOL APIENTRY _WinRegisterUserDatatype(HAB a, LONG b, LONG c, PLONG d);

#undef  WinRegisterUserDatatype
#define WinRegisterUserDatatype _WinRegisterUserDatatype

BOOL APIENTRY _WinRegisterUserMsg(HAB a, ULONG b, LONG c, LONG d, LONG e, LONG f, LONG g);

#undef  WinRegisterUserMsg
#define WinRegisterUserMsg _WinRegisterUserMsg

BOOL APIENTRY _WinReplyMsg(HAB a, HMQ b, HMQ c, MRESULT d);

#undef  WinReplyMsg
#define WinReplyMsg _WinReplyMsg

MRESULT APIENTRY _WinSendMsg(HWND a, ULONG b, MPARAM c, MPARAM d);

#undef  WinSendMsg
#define WinSendMsg _WinSendMsg

BOOL APIENTRY _WinSetMsgMode(HAB a, PCSZ b, LONG c);

#undef  WinSetMsgMode
#define WinSetMsgMode _WinSetMsgMode

BOOL APIENTRY _WinSetSynchroMode(HAB a, LONG b);

#undef  WinSetSynchroMode
#define WinSetSynchroMode _WinSetSynchroMode

BOOL APIENTRY _WinThreadAssocQueue(HAB a, HMQ b);

#undef  WinThreadAssocQueue
#define WinThreadAssocQueue _WinThreadAssocQueue

BOOL APIENTRY _WinWakeThread(HMQ a);

#undef  WinWakeThread
#define WinWakeThread _WinWakeThread

#endif
HWND APIENTRY _WinCreateWindow(HWND a, PCSZ b, PCSZ c, ULONG d, LONG e, LONG f, LONG g, LONG h, HWND i, HWND j, ULONG k, PVOID l, PVOID m);

#undef  WinCreateWindow
#define WinCreateWindow _WinCreateWindow

BOOL APIENTRY _WinDrawBitmap(HPS a, HBITMAP b, PRECTL c, PPOINTL d, LONG e, LONG f, ULONG g);

#undef  WinDrawBitmap
#define WinDrawBitmap _WinDrawBitmap

BOOL APIENTRY _WinDrawBorder(HPS a, PRECTL b, LONG c, LONG d, LONG e, LONG f, ULONG g);

#undef  WinDrawBorder
#define WinDrawBorder _WinDrawBorder

LONG APIENTRY _WinDrawText(HPS a, LONG b, PCH c, PRECTL d, LONG e, LONG f, ULONG g);

#undef  WinDrawText
#define WinDrawText _WinDrawText

BOOL APIENTRY _WinEnableWindow(HWND a, BOOL b);

#undef  WinEnableWindow
#define WinEnableWindow _WinEnableWindow

BOOL APIENTRY _WinEnableWindowUpdate(HWND a, BOOL b);

#undef  WinEnableWindowUpdate
#define WinEnableWindowUpdate _WinEnableWindowUpdate

BOOL APIENTRY _WinInvalidateRect(HWND a, PRECTL b, BOOL c);

#undef  WinInvalidateRect
#define WinInvalidateRect _WinInvalidateRect

BOOL APIENTRY _WinInvalidateRegion(HWND a, HRGN b, BOOL c);

#undef  WinInvalidateRegion
#define WinInvalidateRegion _WinInvalidateRegion

BOOL APIENTRY _WinInvertRect(HPS a, PRECTL b);

#undef  WinInvertRect
#define WinInvertRect _WinInvertRect

BOOL APIENTRY _WinIsChild(HWND a, HWND b);

#undef  WinIsChild
#define WinIsChild _WinIsChild

BOOL APIENTRY _WinIsWindow(HAB a, HWND b);

#undef  WinIsWindow
#define WinIsWindow _WinIsWindow

BOOL APIENTRY _WinIsWindowEnabled(HWND a);

#undef  WinIsWindowEnabled
#define WinIsWindowEnabled _WinIsWindowEnabled

BOOL APIENTRY _WinIsWindowVisible(HWND a);

#undef  WinIsWindowVisible
#define WinIsWindowVisible _WinIsWindowVisible

LONG APIENTRY _WinLoadMessage(HAB a, HMODULE b, ULONG c, LONG d, PSZ e);

#undef  WinLoadMessage
#define WinLoadMessage _WinLoadMessage

LONG APIENTRY _WinLoadString(HAB a, HMODULE b, ULONG c, LONG d, PSZ e);

#undef  WinLoadString
#define WinLoadString _WinLoadString

LONG APIENTRY _WinMultWindowFromIDs(HWND a, PHWND b, ULONG c, ULONG d);

#undef  WinMultWindowFromIDs
#define WinMultWindowFromIDs _WinMultWindowFromIDs

HWND APIENTRY _WinQueryDesktopWindow(HAB a, HDC b);

#undef  WinQueryDesktopWindow
#define WinQueryDesktopWindow _WinQueryDesktopWindow

HWND APIENTRY _WinQueryObjectWindow(HWND a);

#undef  WinQueryObjectWindow
#define WinQueryObjectWindow _WinQueryObjectWindow

HPOINTER APIENTRY _WinQueryPointer(HWND a);

#undef  WinQueryPointer
#define WinQueryPointer _WinQueryPointer

HWND APIENTRY _WinQueryWindow(HWND a, LONG b);

#undef  WinQueryWindow
#define WinQueryWindow _WinQueryWindow

BOOL APIENTRY _WinQueryWindowPos(HWND a, PSWP b);

#undef  WinQueryWindowPos
#define WinQueryWindowPos _WinQueryWindowPos

BOOL APIENTRY _WinQueryWindowProcess(HWND a, PPID b, PTID c);

#undef  WinQueryWindowProcess
#define WinQueryWindowProcess _WinQueryWindowProcess

LONG APIENTRY _WinQueryWindowText(HWND a, LONG b, PCH c);

#undef  WinQueryWindowText
#define WinQueryWindowText _WinQueryWindowText

LONG APIENTRY _WinQueryWindowTextLength(HWND a);

#undef  WinQueryWindowTextLength
#define WinQueryWindowTextLength _WinQueryWindowTextLength

BOOL APIENTRY _WinSetMultWindowPos(HAB a, PSWP b, ULONG c);

#undef  WinSetMultWindowPos
#define WinSetMultWindowPos _WinSetMultWindowPos

BOOL APIENTRY _WinSetOwner(HWND a, HWND b);

#undef  WinSetOwner
#define WinSetOwner _WinSetOwner

BOOL APIENTRY _WinSetParent(HWND a, HWND b, BOOL c);

#undef  WinSetParent
#define WinSetParent _WinSetParent

BOOL APIENTRY _WinSetWindowPos(HWND a, HWND b, LONG c, LONG d, LONG e, LONG f, ULONG g);

#undef  WinSetWindowPos
#define WinSetWindowPos _WinSetWindowPos

BOOL APIENTRY _WinSetWindowText(HWND a, PCSZ b);

#undef  WinSetWindowText
#define WinSetWindowText _WinSetWindowText

BOOL APIENTRY _WinUpdateWindow(HWND a);

#undef  WinUpdateWindow
#define WinUpdateWindow _WinUpdateWindow

HWND APIENTRY _WinWindowFromID(HWND a, ULONG b);

#undef  WinWindowFromID
#define WinWindowFromID _WinWindowFromID

#ifdef INCL_WINFRAMEMGR
HWND APIENTRY _WinCreateStdWindow(HWND a, ULONG b, PULONG c, PCSZ d, PCSZ e, ULONG f, HMODULE g, ULONG h, PHWND i);

#undef  WinCreateStdWindow
#define WinCreateStdWindow _WinCreateStdWindow

BOOL APIENTRY _WinCalcFrameRect(HWND a, PRECTL b, BOOL c);

#undef  WinCalcFrameRect
#define WinCalcFrameRect _WinCalcFrameRect

BOOL APIENTRY _WinCreateFrameControls(HWND a, PFRAMECDATA b, PCSZ c);

#undef  WinCreateFrameControls
#define WinCreateFrameControls _WinCreateFrameControls

BOOL APIENTRY _WinFlashWindow(HWND a, BOOL b);

#undef  WinFlashWindow
#define WinFlashWindow _WinFlashWindow

BOOL APIENTRY _WinGetMaxPosition(HWND a, PSWP b);

#undef  WinGetMaxPosition
#define WinGetMaxPosition _WinGetMaxPosition

BOOL APIENTRY _WinGetMinPosition(HWND a, PSWP b, PPOINTL c);

#undef  WinGetMinPosition
#define WinGetMinPosition _WinGetMinPosition

BOOL APIENTRY _WinSaveWindowPos(HSAVEWP a, PSWP b, ULONG c);

#undef  WinSaveWindowPos
#define WinSaveWindowPos _WinSaveWindowPos

#endif
#ifdef INCL_WINWINDOWMGR
HPS APIENTRY _WinBeginPaint(HWND a, HPS b, PRECTL c);

#undef  WinBeginPaint
#define WinBeginPaint _WinBeginPaint

MRESULT APIENTRY _WinDefWindowProc(HWND a, ULONG b, MPARAM c, MPARAM d);

#undef  WinDefWindowProc
#define WinDefWindowProc _WinDefWindowProc

BOOL APIENTRY _WinDestroyWindow(HWND a);

#undef  WinDestroyWindow
#define WinDestroyWindow _WinDestroyWindow

BOOL APIENTRY _WinEndPaint(HPS a);

#undef  WinEndPaint
#define WinEndPaint _WinEndPaint

BOOL APIENTRY _WinFillRect(HPS a, PRECTL b, LONG c);

#undef  WinFillRect
#define WinFillRect _WinFillRect

HPS APIENTRY _WinGetClipPS(HWND a, HWND b, ULONG c);

#undef  WinGetClipPS
#define WinGetClipPS _WinGetClipPS

HPS APIENTRY _WinGetPS(HWND a);

#undef  WinGetPS
#define WinGetPS _WinGetPS

HAB APIENTRY _WinInitialize(ULONG a);

#undef  WinInitialize
#define WinInitialize _WinInitialize

BOOL APIENTRY _WinIsWindowShowing(HWND a);

#undef  WinIsWindowShowing
#define WinIsWindowShowing _WinIsWindowShowing

HDC APIENTRY _WinOpenWindowDC(HWND a);

#undef  WinOpenWindowDC
#define WinOpenWindowDC _WinOpenWindowDC

HAB APIENTRY _WinQueryAnchorBlock(HWND a);

#undef  WinQueryAnchorBlock
#define WinQueryAnchorBlock _WinQueryAnchorBlock

ULONG APIENTRY _WinQueryVersion(HAB a);

#undef  WinQueryVersion
#define WinQueryVersion _WinQueryVersion

BOOL APIENTRY _WinQueryWindowRect(HWND a, PRECTL b);

#undef  WinQueryWindowRect
#define WinQueryWindowRect _WinQueryWindowRect

BOOL APIENTRY _WinRegisterClass(HAB a, PCSZ b, PFNWP c, ULONG d, ULONG e);

#undef  WinRegisterClass
#define WinRegisterClass _WinRegisterClass

BOOL APIENTRY _WinReleasePS(HPS a);

#undef  WinReleasePS
#define WinReleasePS _WinReleasePS

LONG APIENTRY _WinScrollWindow(HWND a, LONG b, LONG c, PRECTL d, PRECTL e, HRGN f, PRECTL g, ULONG h);

#undef  WinScrollWindow
#define WinScrollWindow _WinScrollWindow

BOOL APIENTRY _WinSetActiveWindow(HWND a, HWND b);

#undef  WinSetActiveWindow
#define WinSetActiveWindow _WinSetActiveWindow

BOOL APIENTRY _WinShowWindow(HWND a, BOOL b);

#undef  WinShowWindow
#define WinShowWindow _WinShowWindow

BOOL APIENTRY _WinTerminate(HAB a);

#undef  WinTerminate
#define WinTerminate _WinTerminate

#endif
#ifdef INCL_WINWINDOWMGR
HENUM APIENTRY _WinBeginEnumWindows(HWND a);

#undef  WinBeginEnumWindows
#define WinBeginEnumWindows _WinBeginEnumWindows

BOOL APIENTRY _WinEndEnumWindows(HENUM a);

#undef  WinEndEnumWindows
#define WinEndEnumWindows _WinEndEnumWindows

LONG APIENTRY _WinExcludeUpdateRegion(HPS a, HWND b);

#undef  WinExcludeUpdateRegion
#define WinExcludeUpdateRegion _WinExcludeUpdateRegion

HWND APIENTRY _WinGetNextWindow(HENUM a);

#undef  WinGetNextWindow
#define WinGetNextWindow _WinGetNextWindow

HPS APIENTRY _WinGetScreenPS(HWND a);

#undef  WinGetScreenPS
#define WinGetScreenPS _WinGetScreenPS

BOOL APIENTRY _WinIsThreadActive(HAB a);

#undef  WinIsThreadActive
#define WinIsThreadActive _WinIsThreadActive

BOOL APIENTRY _WinLockVisRegions(HWND a, BOOL b);

#undef  WinLockVisRegions
#define WinLockVisRegions _WinLockVisRegions

BOOL APIENTRY _WinLockWindowUpdate(HWND a, HWND b);

#undef  WinLockWindowUpdate
#define WinLockWindowUpdate _WinLockWindowUpdate

BOOL APIENTRY _WinMapWindowPoints(HWND a, HWND b, PPOINTL c, LONG d);

#undef  WinMapWindowPoints
#define WinMapWindowPoints _WinMapWindowPoints

HWND APIENTRY _WinQueryActiveWindow(HWND a);

#undef  WinQueryActiveWindow
#define WinQueryActiveWindow _WinQueryActiveWindow

BOOL APIENTRY _WinQueryClassInfo(HAB a, PCSZ b, PCLASSINFO c);

#undef  WinQueryClassInfo
#define WinQueryClassInfo _WinQueryClassInfo

LONG APIENTRY _WinQueryClassName(HWND a, LONG b, PCH c);

#undef  WinQueryClassName
#define WinQueryClassName _WinQueryClassName

BOOL APIENTRY _WinQueryUpdateRect(HWND a, PRECTL b);

#undef  WinQueryUpdateRect
#define WinQueryUpdateRect _WinQueryUpdateRect

LONG APIENTRY _WinQueryUpdateRegion(HWND a, HRGN b);

#undef  WinQueryUpdateRegion
#define WinQueryUpdateRegion _WinQueryUpdateRegion

HWND APIENTRY _WinQuerySysModalWindow(HWND a);

#undef  WinQuerySysModalWindow
#define WinQuerySysModalWindow _WinQuerySysModalWindow

HDC APIENTRY _WinQueryWindowDC(HWND a);

#undef  WinQueryWindowDC
#define WinQueryWindowDC _WinQueryWindowDC

PVOID APIENTRY _WinQueryWindowPtr(HWND a, LONG b);

#undef  WinQueryWindowPtr
#define WinQueryWindowPtr _WinQueryWindowPtr

ULONG APIENTRY _WinQueryWindowULong(HWND a, LONG b);

#undef  WinQueryWindowULong
#define WinQueryWindowULong _WinQueryWindowULong

USHORT APIENTRY _WinQueryWindowUShort(HWND a, LONG b);

#undef  WinQueryWindowUShort
#define WinQueryWindowUShort _WinQueryWindowUShort

BOOL APIENTRY _WinSetSysModalWindow(HWND a, HWND b);

#undef  WinSetSysModalWindow
#define WinSetSysModalWindow _WinSetSysModalWindow

BOOL APIENTRY _WinSetWindowBits(HWND a, LONG b, ULONG c, ULONG d);

#undef  WinSetWindowBits
#define WinSetWindowBits _WinSetWindowBits

BOOL APIENTRY _WinSetWindowPtr(HWND a, LONG b, PVOID c);

#undef  WinSetWindowPtr
#define WinSetWindowPtr _WinSetWindowPtr

BOOL APIENTRY _WinSetWindowULong(HWND a, LONG b, ULONG c);

#undef  WinSetWindowULong
#define WinSetWindowULong _WinSetWindowULong

BOOL APIENTRY _WinSetWindowUShort(HWND a, LONG b, USHORT c);

#undef  WinSetWindowUShort
#define WinSetWindowUShort _WinSetWindowUShort

PFNWP APIENTRY _WinSubclassWindow(HWND a, PFNWP b);

#undef  WinSubclassWindow
#define WinSubclassWindow _WinSubclassWindow

BOOL APIENTRY _WinValidateRect(HWND a, PRECTL b, BOOL c);

#undef  WinValidateRect
#define WinValidateRect _WinValidateRect

BOOL APIENTRY _WinValidateRegion(HWND a, HRGN b, BOOL c);

#undef  WinValidateRegion
#define WinValidateRegion _WinValidateRegion

HWND APIENTRY _WinWindowFromDC(HDC a);

#undef  WinWindowFromDC
#define WinWindowFromDC _WinWindowFromDC

HWND APIENTRY _WinWindowFromPoint(HWND a, PPOINTL b, BOOL c);

#undef  WinWindowFromPoint
#define WinWindowFromPoint _WinWindowFromPoint

#endif
#ifdef INCL_WINACCELERATORS
ULONG APIENTRY _WinCopyAccelTable(HACCEL a, PACCELTABLE b, ULONG c);

#undef  WinCopyAccelTable
#define WinCopyAccelTable _WinCopyAccelTable

HACCEL APIENTRY _WinCreateAccelTable(HAB a, PACCELTABLE b);

#undef  WinCreateAccelTable
#define WinCreateAccelTable _WinCreateAccelTable

BOOL APIENTRY _WinDestroyAccelTable(HACCEL a);

#undef  WinDestroyAccelTable
#define WinDestroyAccelTable _WinDestroyAccelTable

HACCEL APIENTRY _WinLoadAccelTable(HAB a, HMODULE b, ULONG c);

#undef  WinLoadAccelTable
#define WinLoadAccelTable _WinLoadAccelTable

HACCEL APIENTRY _WinQueryAccelTable(HAB a, HWND b);

#undef  WinQueryAccelTable
#define WinQueryAccelTable _WinQueryAccelTable

BOOL APIENTRY _WinSetAccelTable(HAB a, HACCEL b, HWND c);

#undef  WinSetAccelTable
#define WinSetAccelTable _WinSetAccelTable

BOOL APIENTRY _WinTranslateAccel(HAB a, HWND b, HACCEL c, PQMSG d);

#undef  WinTranslateAccel
#define WinTranslateAccel _WinTranslateAccel

#endif
#ifdef INCL_WINATOM
ATOM APIENTRY _WinAddAtom(HATOMTBL a, PCSZ b);

#undef  WinAddAtom
#define WinAddAtom _WinAddAtom

HATOMTBL APIENTRY _WinCreateAtomTable(ULONG a, ULONG b);

#undef  WinCreateAtomTable
#define WinCreateAtomTable _WinCreateAtomTable

ATOM APIENTRY _WinDeleteAtom(HATOMTBL a, ATOM b);

#undef  WinDeleteAtom
#define WinDeleteAtom _WinDeleteAtom

HATOMTBL APIENTRY _WinDestroyAtomTable(HATOMTBL a);

#undef  WinDestroyAtomTable
#define WinDestroyAtomTable _WinDestroyAtomTable

ATOM APIENTRY _WinFindAtom(HATOMTBL a, PCSZ b);

#undef  WinFindAtom
#define WinFindAtom _WinFindAtom

ULONG APIENTRY _WinQueryAtomLength(HATOMTBL a, ATOM b);

#undef  WinQueryAtomLength
#define WinQueryAtomLength _WinQueryAtomLength

ULONG APIENTRY _WinQueryAtomName(HATOMTBL a, ATOM b, PSZ c, ULONG d);

#undef  WinQueryAtomName
#define WinQueryAtomName _WinQueryAtomName

ULONG APIENTRY _WinQueryAtomUsage(HATOMTBL a, ATOM b);

#undef  WinQueryAtomUsage
#define WinQueryAtomUsage _WinQueryAtomUsage

HATOMTBL APIENTRY _WinQuerySystemAtomTable();

#undef  WinQuerySystemAtomTable
#define WinQuerySystemAtomTable _WinQuerySystemAtomTable

#endif
#ifdef INCL_WINCLIPBOARD
BOOL APIENTRY _WinCloseClipbrd(HAB a);

#undef  WinCloseClipbrd
#define WinCloseClipbrd _WinCloseClipbrd

BOOL APIENTRY _WinEmptyClipbrd(HAB a);

#undef  WinEmptyClipbrd
#define WinEmptyClipbrd _WinEmptyClipbrd

ULONG APIENTRY _WinEnumClipbrdFmts(HAB a, ULONG b);

#undef  WinEnumClipbrdFmts
#define WinEnumClipbrdFmts _WinEnumClipbrdFmts

BOOL APIENTRY _WinOpenClipbrd(HAB a);

#undef  WinOpenClipbrd
#define WinOpenClipbrd _WinOpenClipbrd

ULONG APIENTRY _WinQueryClipbrdData(HAB a, ULONG b);

#undef  WinQueryClipbrdData
#define WinQueryClipbrdData _WinQueryClipbrdData

BOOL APIENTRY _WinQueryClipbrdFmtInfo(HAB a, ULONG b, PULONG c);

#undef  WinQueryClipbrdFmtInfo
#define WinQueryClipbrdFmtInfo _WinQueryClipbrdFmtInfo

HWND APIENTRY _WinQueryClipbrdOwner(HAB a);

#undef  WinQueryClipbrdOwner
#define WinQueryClipbrdOwner _WinQueryClipbrdOwner

HWND APIENTRY _WinQueryClipbrdViewer(HAB a);

#undef  WinQueryClipbrdViewer
#define WinQueryClipbrdViewer _WinQueryClipbrdViewer

BOOL APIENTRY _WinSetClipbrdData(HAB a, ULONG b, ULONG c, ULONG d);

#undef  WinSetClipbrdData
#define WinSetClipbrdData _WinSetClipbrdData

BOOL APIENTRY _WinSetClipbrdOwner(HAB a, HWND b);

#undef  WinSetClipbrdOwner
#define WinSetClipbrdOwner _WinSetClipbrdOwner

BOOL APIENTRY _WinSetClipbrdViewer(HAB a, HWND b);

#undef  WinSetClipbrdViewer
#define WinSetClipbrdViewer _WinSetClipbrdViewer


BOOL APIENTRY __OpenClipbrd(HAB a, HWND b);

#undef  _OpenClipbrd
#define _OpenClipbrd __OpenClipbrd

BOOL APIENTRY _Win32AddClipbrdViewer(HWND a);

#undef  Win32AddClipbrdViewer
#define Win32AddClipbrdViewer _Win32AddClipbrdViewer

BOOL APIENTRY _Win32RemoveClipbrdViewer(HWND a);

#undef  Win32RemoveClipbrdViewer
#define Win32RemoveClipbrdViewer _Win32RemoveClipbrdViewer

HWND APIENTRY _Win32QueryClipbrdViewerChain();

#undef  Win32QueryClipbrdViewerChain
#define Win32QueryClipbrdViewerChain _Win32QueryClipbrdViewerChain

HWND APIENTRY _Win32QueryOpenClipbrdWindow();

#undef  Win32QueryOpenClipbrdWindow
#define Win32QueryOpenClipbrdWindow _Win32QueryOpenClipbrdWindow

#endif
#ifdef INCL_WINDDE
BOOL APIENTRY _WinDdeInitiate(HWND a, PCSZ b, PCSZ c, PCONVCONTEXT d);

#undef  WinDdeInitiate
#define WinDdeInitiate _WinDdeInitiate

BOOL APIENTRY _WinDdePostMsg(HWND a, HWND b, ULONG c, PDDESTRUCT d, ULONG e);

#undef  WinDdePostMsg
#define WinDdePostMsg _WinDdePostMsg

MRESULT APIENTRY _WinDdeRespond(HWND a, HWND b, PCSZ c, PCSZ d, PCONVCONTEXT e);

#undef  WinDdeRespond
#define WinDdeRespond _WinDdeRespond

#endif
#ifdef INCL_WINCOUNTRY
ULONG APIENTRY _WinCompareStrings(HAB a, ULONG b, ULONG c, PCSZ d, PCSZ e, ULONG f);

#undef  WinCompareStrings
#define WinCompareStrings _WinCompareStrings

UCHAR APIENTRY _WinCpTranslateChar(HAB a, ULONG b, UCHAR c, ULONG d);

#undef  WinCpTranslateChar
#define WinCpTranslateChar _WinCpTranslateChar

BOOL APIENTRY _WinCpTranslateString(HAB a, ULONG b, PCSZ c, ULONG d, ULONG e, PSZ f);

#undef  WinCpTranslateString
#define WinCpTranslateString _WinCpTranslateString

PCSZ APIENTRY _WinNextChar(HAB a, ULONG b, ULONG c, PCSZ d);

#undef  WinNextChar
#define WinNextChar _WinNextChar

PCSZ APIENTRY _WinPrevChar(HAB a, ULONG b, ULONG c, PCSZ d, PCSZ e);

#undef  WinPrevChar
#define WinPrevChar _WinPrevChar

ULONG APIENTRY _WinQueryCp(HMQ a);

#undef  WinQueryCp
#define WinQueryCp _WinQueryCp

ULONG APIENTRY _WinQueryCpList(HAB a, ULONG b, PULONG c);

#undef  WinQueryCpList
#define WinQueryCpList _WinQueryCpList

BOOL APIENTRY _WinSetCp(HMQ a, ULONG b);

#undef  WinSetCp
#define WinSetCp _WinSetCp

ULONG APIENTRY _WinUpper(HAB a, ULONG b, ULONG c, PSZ d);

#undef  WinUpper
#define WinUpper _WinUpper

ULONG APIENTRY _WinUpperChar(HAB a, ULONG b, ULONG c, ULONG d);

#undef  WinUpperChar
#define WinUpperChar _WinUpperChar

#endif
#ifdef INCL_WINCURSORS
BOOL APIENTRY _WinCreateCursor(HWND a, LONG b, LONG c, LONG d, LONG e, ULONG f, PRECTL g);

#undef  WinCreateCursor
#define WinCreateCursor _WinCreateCursor

BOOL APIENTRY _WinDestroyCursor(HWND a);

#undef  WinDestroyCursor
#define WinDestroyCursor _WinDestroyCursor

BOOL APIENTRY _WinShowCursor(HWND a, BOOL b);

#undef  WinShowCursor
#define WinShowCursor _WinShowCursor

BOOL APIENTRY _WinQueryCursorInfo(HWND a, PCURSORINFO b);

#undef  WinQueryCursorInfo
#define WinQueryCursorInfo _WinQueryCursorInfo

#endif
#ifdef INCL_WINDESKTOP
BOOL APIENTRY _WinQueryDesktopBkgnd(HWND a, PDESKTOP b);

#undef  WinQueryDesktopBkgnd
#define WinQueryDesktopBkgnd _WinQueryDesktopBkgnd

HBITMAP APIENTRY _WinSetDesktopBkgnd(HWND a, PDESKTOP b);

#undef  WinSetDesktopBkgnd
#define WinSetDesktopBkgnd _WinSetDesktopBkgnd

#endif
#ifdef INCL_WINDIALOGS
BOOL APIENTRY _WinAlarm(HWND a, ULONG b);

#undef  WinAlarm
#define WinAlarm _WinAlarm

MRESULT APIENTRY _WinDefDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d);

#undef  WinDefDlgProc
#define WinDefDlgProc _WinDefDlgProc

BOOL APIENTRY _WinDismissDlg(HWND a, ULONG b);

#undef  WinDismissDlg
#define WinDismissDlg _WinDismissDlg

ULONG APIENTRY _WinDlgBox(HWND a, HWND b, PFNWP c, HMODULE d, ULONG e, PVOID f);

#undef  WinDlgBox
#define WinDlgBox _WinDlgBox

BOOL APIENTRY _WinGetDlgMsg(HWND a, PQMSG b);

#undef  WinGetDlgMsg
#define WinGetDlgMsg _WinGetDlgMsg

HWND APIENTRY _WinLoadDlg(HWND a, HWND b, PFNWP c, HMODULE d, ULONG e, PVOID f);

#undef  WinLoadDlg
#define WinLoadDlg _WinLoadDlg

ULONG APIENTRY _WinMessageBox(HWND a, HWND b, PCSZ c, PCSZ d, ULONG e, ULONG f);

#undef  WinMessageBox
#define WinMessageBox _WinMessageBox

ULONG APIENTRY _WinMessageBox2(HWND a, HWND b, PSZ c, PSZ d, ULONG e, PMB2INFO f);

#undef  WinMessageBox2
#define WinMessageBox2 _WinMessageBox2

BOOL APIENTRY _WinQueryDlgItemShort(HWND a, ULONG b, PSHORT c, BOOL d);

#undef  WinQueryDlgItemShort
#define WinQueryDlgItemShort _WinQueryDlgItemShort

ULONG APIENTRY _WinQueryDlgItemText(HWND a, ULONG b, LONG c, PSZ d);

#undef  WinQueryDlgItemText
#define WinQueryDlgItemText _WinQueryDlgItemText

LONG APIENTRY _WinQueryDlgItemTextLength(HWND a, ULONG b);

#undef  WinQueryDlgItemTextLength
#define WinQueryDlgItemTextLength _WinQueryDlgItemTextLength

BOOL APIENTRY _WinSetDlgItemShort(HWND a, ULONG b, USHORT c, BOOL d);

#undef  WinSetDlgItemShort
#define WinSetDlgItemShort _WinSetDlgItemShort

BOOL APIENTRY _WinSetDlgItemText(HWND a, ULONG b, PCSZ c);

#undef  WinSetDlgItemText
#define WinSetDlgItemText _WinSetDlgItemText

HWND APIENTRY _WinCreateDlg(HWND a, HWND b, PFNWP c, PDLGTEMPLATE d, PVOID e);

#undef  WinCreateDlg
#define WinCreateDlg _WinCreateDlg

HWND APIENTRY _WinEnumDlgItem(HWND a, HWND b, ULONG c);

#undef  WinEnumDlgItem
#define WinEnumDlgItem _WinEnumDlgItem

BOOL APIENTRY _WinMapDlgPoints(HWND a, PPOINTL b, ULONG c, BOOL d);

#undef  WinMapDlgPoints
#define WinMapDlgPoints _WinMapDlgPoints

ULONG APIENTRY _WinProcessDlg(HWND a);

#undef  WinProcessDlg
#define WinProcessDlg _WinProcessDlg

MRESULT APIENTRY _WinSendDlgItemMsg(HWND a, ULONG b, ULONG c, MPARAM d, MPARAM e);

#undef  WinSendDlgItemMsg
#define WinSendDlgItemMsg _WinSendDlgItemMsg

LONG APIENTRY _WinSubstituteStrings(HWND a, PCSZ b, LONG c, PSZ d);

#undef  WinSubstituteStrings
#define WinSubstituteStrings _WinSubstituteStrings

#endif
#ifdef INCL_WINERRORS
ERRORID APIENTRY _WinGetLastError(HAB a);

#undef  WinGetLastError
#define WinGetLastError _WinGetLastError

BOOL APIENTRY _WinFreeErrorInfo(PERRINFO a);

#undef  WinFreeErrorInfo
#define WinFreeErrorInfo _WinFreeErrorInfo

PERRINFO APIENTRY _WinGetErrorInfo(HAB a);

#undef  WinGetErrorInfo
#define WinGetErrorInfo _WinGetErrorInfo

#endif
#ifdef INCL_WINHOOKS
BOOL APIENTRY _WinCallMsgFilter(HAB a, PQMSG b, ULONG c);

#undef  WinCallMsgFilter
#define WinCallMsgFilter _WinCallMsgFilter

BOOL APIENTRY _WinReleaseHook(HAB a, HMQ b, LONG c, PFN d, HMODULE e);

#undef  WinReleaseHook
#define WinReleaseHook _WinReleaseHook

BOOL APIENTRY _WinSetHook(HAB a, HMQ b, LONG c, PFN d, HMODULE e);

#undef  WinSetHook
#define WinSetHook _WinSetHook

#endif
#ifdef INCL_WININPUT
BOOL APIENTRY _WinFocusChange(HWND a, HWND b, ULONG c);

#undef  WinFocusChange
#define WinFocusChange _WinFocusChange

BOOL APIENTRY _WinLockupSystem(HAB a);

#undef  WinLockupSystem
#define WinLockupSystem _WinLockupSystem

BOOL APIENTRY _WinSetFocus(HWND a, HWND b);

#undef  WinSetFocus
#define WinSetFocus _WinSetFocus

BOOL APIENTRY _WinUnlockSystem(HAB a, PSZ b);

#undef  WinUnlockSystem
#define WinUnlockSystem _WinUnlockSystem

BOOL APIENTRY _WinCheckInput(HAB a);

#undef  WinCheckInput
#define WinCheckInput _WinCheckInput

BOOL APIENTRY _WinEnablePhysInput(HWND a, BOOL b);

#undef  WinEnablePhysInput
#define WinEnablePhysInput _WinEnablePhysInput

LONG APIENTRY _WinGetKeyState(HWND a, LONG b);

#undef  WinGetKeyState
#define WinGetKeyState _WinGetKeyState

LONG APIENTRY _WinGetPhysKeyState(HWND a, LONG b);

#undef  WinGetPhysKeyState
#define WinGetPhysKeyState _WinGetPhysKeyState

BOOL APIENTRY _WinIsPhysInputEnabled(HWND a);

#undef  WinIsPhysInputEnabled
#define WinIsPhysInputEnabled _WinIsPhysInputEnabled

HWND APIENTRY _WinQueryCapture(HWND a);

#undef  WinQueryCapture
#define WinQueryCapture _WinQueryCapture

HWND APIENTRY _WinQueryFocus(HWND a);

#undef  WinQueryFocus
#define WinQueryFocus _WinQueryFocus

ULONG APIENTRY _WinQueryVisibleRegion(HWND a, HRGN b);

#undef  WinQueryVisibleRegion
#define WinQueryVisibleRegion _WinQueryVisibleRegion

BOOL APIENTRY _WinSetCapture(HWND a, HWND b);

#undef  WinSetCapture
#define WinSetCapture _WinSetCapture

BOOL APIENTRY _WinSetKeyboardStateTable(HWND a, PBYTE b, BOOL c);

#undef  WinSetKeyboardStateTable
#define WinSetKeyboardStateTable _WinSetKeyboardStateTable

BOOL APIENTRY _WinSetVisibleRegionNotify(HWND a, BOOL b);

#undef  WinSetVisibleRegionNotify
#define WinSetVisibleRegionNotify _WinSetVisibleRegionNotify

#endif
#ifdef INCL_WINLOAD
BOOL APIENTRY _WinDeleteLibrary(HAB a, HLIB b);

#undef  WinDeleteLibrary
#define WinDeleteLibrary _WinDeleteLibrary

BOOL APIENTRY _WinDeleteProcedure(HAB a, PFNWP b);

#undef  WinDeleteProcedure
#define WinDeleteProcedure _WinDeleteProcedure

HLIB APIENTRY _WinLoadLibrary(HAB a, PCSZ b);

#undef  WinLoadLibrary
#define WinLoadLibrary _WinLoadLibrary

PFNWP APIENTRY _WinLoadProcedure(HAB a, HLIB b, PSZ c);

#undef  WinLoadProcedure
#define WinLoadProcedure _WinLoadProcedure

#endif
#ifdef INCL_WINMENUS
HWND APIENTRY _WinCreateMenu(HWND a, PVOID b);

#undef  WinCreateMenu
#define WinCreateMenu _WinCreateMenu

HWND APIENTRY _WinLoadMenu(HWND a, HMODULE b, ULONG c);

#undef  WinLoadMenu
#define WinLoadMenu _WinLoadMenu

BOOL APIENTRY _WinPopupMenu(HWND a, HWND b, HWND c, LONG d, LONG e, LONG f, ULONG g);

#undef  WinPopupMenu
#define WinPopupMenu _WinPopupMenu

#endif
#ifdef INCL_WINMESSAGEMGR
BOOL APIENTRY _WinBroadcastMsg(HWND a, ULONG b, MPARAM c, MPARAM d, ULONG e);

#undef  WinBroadcastMsg
#define WinBroadcastMsg _WinBroadcastMsg

BOOL APIENTRY _WinInSendMsg(HAB a);

#undef  WinInSendMsg
#define WinInSendMsg _WinInSendMsg

BOOL APIENTRY _WinPostQueueMsg(HMQ a, ULONG b, MPARAM c, MPARAM d);

#undef  WinPostQueueMsg
#define WinPostQueueMsg _WinPostQueueMsg

BOOL APIENTRY _WinQueryMsgPos(HAB a, PPOINTL b);

#undef  WinQueryMsgPos
#define WinQueryMsgPos _WinQueryMsgPos

ULONG APIENTRY _WinQueryMsgTime(HAB a);

#undef  WinQueryMsgTime
#define WinQueryMsgTime _WinQueryMsgTime

ULONG APIENTRY _WinQueryQueueStatus(HWND a);

#undef  WinQueryQueueStatus
#define WinQueryQueueStatus _WinQueryQueueStatus

ULONG APIENTRY _WinRequestMutexSem(HMTX a, ULONG b);

#undef  WinRequestMutexSem
#define WinRequestMutexSem _WinRequestMutexSem

BOOL APIENTRY _WinSetClassMsgInterest(HAB a, PCSZ b, ULONG c, LONG d);

#undef  WinSetClassMsgInterest
#define WinSetClassMsgInterest _WinSetClassMsgInterest

BOOL APIENTRY _WinSetMsgInterest(HWND a, ULONG b, LONG c);

#undef  WinSetMsgInterest
#define WinSetMsgInterest _WinSetMsgInterest

ULONG APIENTRY _WinWaitEventSem(HEV a, ULONG b);

#undef  WinWaitEventSem
#define WinWaitEventSem _WinWaitEventSem

BOOL APIENTRY _WinWaitMsg(HAB a, ULONG b, ULONG c);

#undef  WinWaitMsg
#define WinWaitMsg _WinWaitMsg

ULONG APIENTRY _WinWaitMuxWaitSem(HMUX a, ULONG b, PULONG c);

#undef  WinWaitMuxWaitSem
#define WinWaitMuxWaitSem _WinWaitMuxWaitSem

#endif
#ifdef INCL_WINPALETTE
LONG APIENTRY _WinRealizePalette(HWND a, HPS b, PULONG c);

#undef  WinRealizePalette
#define WinRealizePalette _WinRealizePalette

#endif
#ifdef INCL_WINPOINTERS
HPOINTER APIENTRY _WinCreatePointer(HWND a, HBITMAP b, BOOL c, LONG d, LONG e);

#undef  WinCreatePointer
#define WinCreatePointer _WinCreatePointer

HPOINTER APIENTRY _WinCreatePointerIndirect(HWND a, PPOINTERINFO b);

#undef  WinCreatePointerIndirect
#define WinCreatePointerIndirect _WinCreatePointerIndirect

BOOL APIENTRY _WinDestroyPointer(HPOINTER a);

#undef  WinDestroyPointer
#define WinDestroyPointer _WinDestroyPointer

BOOL APIENTRY _WinDrawPointer(HPS a, LONG b, LONG c, HPOINTER d, ULONG e);

#undef  WinDrawPointer
#define WinDrawPointer _WinDrawPointer

HBITMAP APIENTRY _WinGetSysBitmap(HWND a, ULONG b);

#undef  WinGetSysBitmap
#define WinGetSysBitmap _WinGetSysBitmap

HPOINTER APIENTRY _WinLoadPointer(HWND a, HMODULE b, ULONG c);

#undef  WinLoadPointer
#define WinLoadPointer _WinLoadPointer

BOOL APIENTRY _WinLockPointerUpdate(HWND a, HPOINTER b, ULONG c);

#undef  WinLockPointerUpdate
#define WinLockPointerUpdate _WinLockPointerUpdate

BOOL APIENTRY _WinQueryPointerPos(HWND a, PPOINTL b);

#undef  WinQueryPointerPos
#define WinQueryPointerPos _WinQueryPointerPos

BOOL APIENTRY _WinQueryPointerInfo(HPOINTER a, PPOINTERINFO b);

#undef  WinQueryPointerInfo
#define WinQueryPointerInfo _WinQueryPointerInfo

HPOINTER APIENTRY _WinQuerySysPointer(HWND a, LONG b, BOOL c);

#undef  WinQuerySysPointer
#define WinQuerySysPointer _WinQuerySysPointer

BOOL APIENTRY _WinQuerySysPointerData(HWND a, ULONG b, PICONINFO c);

#undef  WinQuerySysPointerData
#define WinQuerySysPointerData _WinQuerySysPointerData

BOOL APIENTRY _WinSetPointer(HWND a, HPOINTER b);

#undef  WinSetPointer
#define WinSetPointer _WinSetPointer

BOOL APIENTRY _WinSetPointerOwner(HPOINTER a, PID b, BOOL c);

#undef  WinSetPointerOwner
#define WinSetPointerOwner _WinSetPointerOwner

BOOL APIENTRY _WinSetPointerPos(HWND a, LONG b, LONG c);

#undef  WinSetPointerPos
#define WinSetPointerPos _WinSetPointerPos

BOOL APIENTRY _WinSetSysPointerData(HWND a, ULONG b, PICONINFO c);

#undef  WinSetSysPointerData
#define WinSetSysPointerData _WinSetSysPointerData

BOOL APIENTRY _WinShowPointer(HWND a, BOOL b);

#undef  WinShowPointer
#define WinShowPointer _WinShowPointer

#endif
#ifdef INCL_WINRECTANGLES
BOOL APIENTRY _WinCopyRect(HAB a, PRECTL b, PRECTL c);

#undef  WinCopyRect
#define WinCopyRect _WinCopyRect

BOOL APIENTRY _WinEqualRect(HAB a, PRECTL b, PRECTL c);

#undef  WinEqualRect
#define WinEqualRect _WinEqualRect

BOOL APIENTRY _WinInflateRect(HAB a, PRECTL b, LONG c, LONG d);

#undef  WinInflateRect
#define WinInflateRect _WinInflateRect

BOOL APIENTRY _WinIntersectRect(HAB a, PRECTL b, PRECTL c, PRECTL d);

#undef  WinIntersectRect
#define WinIntersectRect _WinIntersectRect

BOOL APIENTRY _WinIsRectEmpty(HAB a, PRECTL b);

#undef  WinIsRectEmpty
#define WinIsRectEmpty _WinIsRectEmpty

BOOL APIENTRY _WinMakePoints(HAB a, PPOINTL b, ULONG c);

#undef  WinMakePoints
#define WinMakePoints _WinMakePoints

BOOL APIENTRY _WinMakeRect(HAB a, PRECTL b);

#undef  WinMakeRect
#define WinMakeRect _WinMakeRect

BOOL APIENTRY _WinOffsetRect(HAB a, PRECTL b, LONG c, LONG d);

#undef  WinOffsetRect
#define WinOffsetRect _WinOffsetRect

BOOL APIENTRY _WinPtInRect(HAB a, PRECTL b, PPOINTL c);

#undef  WinPtInRect
#define WinPtInRect _WinPtInRect

BOOL APIENTRY _WinSetRect(HAB a, PRECTL b, LONG c, LONG d, LONG e, LONG f);

#undef  WinSetRect
#define WinSetRect _WinSetRect

BOOL APIENTRY _WinSetRectEmpty(HAB a, PRECTL b);

#undef  WinSetRectEmpty
#define WinSetRectEmpty _WinSetRectEmpty

BOOL APIENTRY _WinSubtractRect(HAB a, PRECTL b, PRECTL c, PRECTL d);

#undef  WinSubtractRect
#define WinSubtractRect _WinSubtractRect

BOOL APIENTRY _WinUnionRect(HAB a, PRECTL b, PRECTL c, PRECTL d);

#undef  WinUnionRect
#define WinUnionRect _WinUnionRect

#endif
#ifdef INCL_WINSYS
LONG APIENTRY _WinQueryControlColors(HWND a, LONG b, ULONG c, ULONG d, PCTLCOLOR e);

#undef  WinQueryControlColors
#define WinQueryControlColors _WinQueryControlColors

ULONG APIENTRY _WinQueryPresParam(HWND a, ULONG b, ULONG c, PULONG d, ULONG e, PVOID f, ULONG g);

#undef  WinQueryPresParam
#define WinQueryPresParam _WinQueryPresParam

LONG APIENTRY _WinQuerySysColor(HWND a, LONG b, LONG c);

#undef  WinQuerySysColor
#define WinQuerySysColor _WinQuerySysColor

LONG APIENTRY _WinQuerySysValue(HWND a, LONG b);

#undef  WinQuerySysValue
#define WinQuerySysValue _WinQuerySysValue

BOOL APIENTRY _WinRemovePresParam(HWND a, ULONG b);

#undef  WinRemovePresParam
#define WinRemovePresParam _WinRemovePresParam

LONG APIENTRY _WinSetControlColors(HWND a, LONG b, ULONG c, ULONG d, PCTLCOLOR e);

#undef  WinSetControlColors
#define WinSetControlColors _WinSetControlColors

BOOL APIENTRY _WinSetPresParam(HWND a, ULONG b, ULONG c, PVOID d);

#undef  WinSetPresParam
#define WinSetPresParam _WinSetPresParam

BOOL APIENTRY _WinSetSysColors(HWND a, ULONG b, ULONG c, LONG d, ULONG e, PLONG f);

#undef  WinSetSysColors
#define WinSetSysColors _WinSetSysColors

BOOL APIENTRY _WinSetSysValue(HWND a, LONG b, LONG c);

#undef  WinSetSysValue
#define WinSetSysValue _WinSetSysValue

#endif
#ifdef INCL_WINTHUNKAPI
PFN APIENTRY _WinQueryClassThunkProc(PCSZ a);

#undef  WinQueryClassThunkProc
#define WinQueryClassThunkProc _WinQueryClassThunkProc

LONG APIENTRY _WinQueryWindowModel(HWND a);

#undef  WinQueryWindowModel
#define WinQueryWindowModel _WinQueryWindowModel

PFN APIENTRY _WinQueryWindowThunkProc(HWND a);

#undef  WinQueryWindowThunkProc
#define WinQueryWindowThunkProc _WinQueryWindowThunkProc

BOOL APIENTRY _WinSetClassThunkProc(PCSZ a, PFN b);

#undef  WinSetClassThunkProc
#define WinSetClassThunkProc _WinSetClassThunkProc

BOOL APIENTRY _WinSetWindowThunkProc(HWND a, PFN b);

#undef  WinSetWindowThunkProc
#define WinSetWindowThunkProc _WinSetWindowThunkProc

#endif
#ifdef INCL_WINTIMER
ULONG APIENTRY _WinGetCurrentTime(HAB a);

#undef  WinGetCurrentTime
#define WinGetCurrentTime _WinGetCurrentTime

ULONG APIENTRY _WinStartTimer(HAB a, HWND b, ULONG c, ULONG d);

#undef  WinStartTimer
#define WinStartTimer _WinStartTimer

BOOL APIENTRY _WinStopTimer(HAB a, HWND b, ULONG c);

#undef  WinStopTimer
#define WinStopTimer _WinStopTimer

#endif
#ifdef INCL_WINTRACKRECT
BOOL APIENTRY _WinShowTrackRect(HWND a, BOOL b);

#undef  WinShowTrackRect
#define WinShowTrackRect _WinShowTrackRect

BOOL APIENTRY _WinTrackRect(HWND a, HPS b, PTRACKINFO c);

#undef  WinTrackRect
#define WinTrackRect _WinTrackRect

#endif
#endif
#ifdef INCL_GPI
LONG APIENTRY _GpiAnimatePalette(HPAL a, ULONG b, ULONG c, ULONG d, PULONG e);

#undef  GpiAnimatePalette
#define GpiAnimatePalette _GpiAnimatePalette

BOOL APIENTRY _GpiBeginArea(HPS a, ULONG b);

#undef  GpiBeginArea
#define GpiBeginArea _GpiBeginArea

BOOL APIENTRY _GpiBeginElement(HPS a, LONG b, PCSZ c);

#undef  GpiBeginElement
#define GpiBeginElement _GpiBeginElement

BOOL APIENTRY _GpiBeginPath(HPS a, LONG b);

#undef  GpiBeginPath
#define GpiBeginPath _GpiBeginPath

LONG APIENTRY _GpiBox(HPS a, LONG b, PPOINTL c, LONG d, LONG e);

#undef  GpiBox
#define GpiBox _GpiBox

LONG APIENTRY _GpiCallSegmentMatrix(HPS a, LONG b, LONG c, PMATRIXLF d, LONG e);

#undef  GpiCallSegmentMatrix
#define GpiCallSegmentMatrix _GpiCallSegmentMatrix

LONG APIENTRY _GpiCharString(HPS a, LONG b, PCH c);

#undef  GpiCharString
#define GpiCharString _GpiCharString

LONG APIENTRY _GpiCharStringAt(HPS a, PPOINTL b, LONG c, PCH d);

#undef  GpiCharStringAt
#define GpiCharStringAt _GpiCharStringAt

LONG APIENTRY _GpiCharStringPos(HPS a, PRECTL b, ULONG c, LONG d, PCH e, PLONG f);

#undef  GpiCharStringPos
#define GpiCharStringPos _GpiCharStringPos

LONG APIENTRY _GpiCharStringPosAt(HPS a, PPOINTL b, PRECTL c, ULONG d, LONG e, PCH f, PLONG g);

#undef  GpiCharStringPosAt
#define GpiCharStringPosAt _GpiCharStringPosAt

BOOL APIENTRY _GpiCloseFigure(HPS a);

#undef  GpiCloseFigure
#define GpiCloseFigure _GpiCloseFigure

LONG APIENTRY _GpiCombineRegion(HPS a, HRGN b, HRGN c, HRGN d, LONG e);

#undef  GpiCombineRegion
#define GpiCombineRegion _GpiCombineRegion

BOOL APIENTRY _GpiComment(HPS a, LONG b, PBYTE c);

#undef  GpiComment
#define GpiComment _GpiComment

BOOL APIENTRY _GpiConvert(HPS a, LONG b, LONG c, LONG d, PPOINTL e);

#undef  GpiConvert
#define GpiConvert _GpiConvert

BOOL APIENTRY _GpiConvertWithMatrix(HPS a, LONG b, PPOINTL c, LONG d, PMATRIXLF e);

#undef  GpiConvertWithMatrix
#define GpiConvertWithMatrix _GpiConvertWithMatrix

HMF APIENTRY _GpiCopyMetaFile(HMF a);

#undef  GpiCopyMetaFile
#define GpiCopyMetaFile _GpiCopyMetaFile

BOOL APIENTRY _GpiCreateLogColorTable(HPS a, ULONG b, LONG c, LONG d, LONG e, PLONG f);

#undef  GpiCreateLogColorTable
#define GpiCreateLogColorTable _GpiCreateLogColorTable

LONG APIENTRY _GpiCreateLogFont(HPS a, STR8 *b, LONG c, PFATTRS d);

#undef  GpiCreateLogFont
#define GpiCreateLogFont _GpiCreateLogFont

HPAL APIENTRY _GpiCreatePalette(HAB a, ULONG b, ULONG c, ULONG d, PULONG e);

#undef  GpiCreatePalette
#define GpiCreatePalette _GpiCreatePalette

HRGN APIENTRY _GpiCreateRegion(HPS a, LONG b, PRECTL c);

#undef  GpiCreateRegion
#define GpiCreateRegion _GpiCreateRegion

BOOL APIENTRY _GpiDeleteElement(HPS a);

#undef  GpiDeleteElement
#define GpiDeleteElement _GpiDeleteElement

BOOL APIENTRY _GpiDeleteElementRange(HPS a, LONG b, LONG c);

#undef  GpiDeleteElementRange
#define GpiDeleteElementRange _GpiDeleteElementRange

BOOL APIENTRY _GpiDeleteElementsBetweenLabels(HPS a, LONG b, LONG c);

#undef  GpiDeleteElementsBetweenLabels
#define GpiDeleteElementsBetweenLabels _GpiDeleteElementsBetweenLabels

BOOL APIENTRY _GpiDeleteMetaFile(HMF a);

#undef  GpiDeleteMetaFile
#define GpiDeleteMetaFile _GpiDeleteMetaFile

BOOL APIENTRY _GpiDeletePalette(HPAL a);

#undef  GpiDeletePalette
#define GpiDeletePalette _GpiDeletePalette

BOOL APIENTRY _GpiDeleteSetId(HPS a, LONG b);

#undef  GpiDeleteSetId
#define GpiDeleteSetId _GpiDeleteSetId

BOOL APIENTRY _GpiDestroyRegion(HPS a, HRGN b);

#undef  GpiDestroyRegion
#define GpiDestroyRegion _GpiDestroyRegion

LONG APIENTRY _GpiElement(HPS a, LONG b, PCSZ c, LONG d, PBYTE e);

#undef  GpiElement
#define GpiElement _GpiElement

LONG APIENTRY _GpiEndArea(HPS a);

#undef  GpiEndArea
#define GpiEndArea _GpiEndArea

BOOL APIENTRY _GpiEndElement(HPS a);

#undef  GpiEndElement
#define GpiEndElement _GpiEndElement

BOOL APIENTRY _GpiEndPath(HPS a);

#undef  GpiEndPath
#define GpiEndPath _GpiEndPath

LONG APIENTRY _GpiEqualRegion(HPS a, HRGN b, HRGN c);

#undef  GpiEqualRegion
#define GpiEqualRegion _GpiEqualRegion

LONG APIENTRY _GpiExcludeClipRectangle(HPS a, PRECTL b);

#undef  GpiExcludeClipRectangle
#define GpiExcludeClipRectangle _GpiExcludeClipRectangle

LONG APIENTRY _GpiFillPath(HPS a, LONG b, LONG c);

#undef  GpiFillPath
#define GpiFillPath _GpiFillPath

LONG APIENTRY _GpiFrameRegion(HPS a, HRGN b, PSIZEL c);

#undef  GpiFrameRegion
#define GpiFrameRegion _GpiFrameRegion

LONG APIENTRY _GpiFullArc(HPS a, LONG b, FIXED c);

#undef  GpiFullArc
#define GpiFullArc _GpiFullArc

LONG APIENTRY _GpiImage(HPS a, LONG b, PSIZEL c, LONG d, PBYTE e);

#undef  GpiImage
#define GpiImage _GpiImage

LONG APIENTRY _GpiIntersectClipRectangle(HPS a, PRECTL b);

#undef  GpiIntersectClipRectangle
#define GpiIntersectClipRectangle _GpiIntersectClipRectangle

BOOL APIENTRY _GpiLabel(HPS a, LONG b);

#undef  GpiLabel
#define GpiLabel _GpiLabel

LONG APIENTRY _GpiLine(HPS a, PPOINTL b);

#undef  GpiLine
#define GpiLine _GpiLine

BOOL APIENTRY _GpiLoadFonts(HAB a, PCSZ b);

#undef  GpiLoadFonts
#define GpiLoadFonts _GpiLoadFonts

HMF APIENTRY _GpiLoadMetaFile(HAB a, PCSZ b);

#undef  GpiLoadMetaFile
#define GpiLoadMetaFile _GpiLoadMetaFile

BOOL APIENTRY _GpiLoadPublicFonts(HAB a, PCSZ b);

#undef  GpiLoadPublicFonts
#define GpiLoadPublicFonts _GpiLoadPublicFonts

LONG APIENTRY _GpiMarker(HPS a, PPOINTL b);

#undef  GpiMarker
#define GpiMarker _GpiMarker

BOOL APIENTRY _GpiModifyPath(HPS a, LONG b, LONG c);

#undef  GpiModifyPath
#define GpiModifyPath _GpiModifyPath

BOOL APIENTRY _GpiMove(HPS a, PPOINTL b);

#undef  GpiMove
#define GpiMove _GpiMove

LONG APIENTRY _GpiOffsetClipRegion(HPS a, PPOINTL b);

#undef  GpiOffsetClipRegion
#define GpiOffsetClipRegion _GpiOffsetClipRegion

BOOL APIENTRY _GpiOffsetElementPointer(HPS a, LONG b);

#undef  GpiOffsetElementPointer
#define GpiOffsetElementPointer _GpiOffsetElementPointer

BOOL APIENTRY _GpiOffsetRegion(HPS a, HRGN b, PPOINTL c);

#undef  GpiOffsetRegion
#define GpiOffsetRegion _GpiOffsetRegion

LONG APIENTRY _GpiOutlinePath(HPS a, LONG b, LONG c);

#undef  GpiOutlinePath
#define GpiOutlinePath _GpiOutlinePath

LONG APIENTRY _GpiPaintRegion(HPS a, HRGN b);

#undef  GpiPaintRegion
#define GpiPaintRegion _GpiPaintRegion

LONG APIENTRY _GpiPartialArc(HPS a, PPOINTL b, FIXED c, FIXED d, FIXED e);

#undef  GpiPartialArc
#define GpiPartialArc _GpiPartialArc

HRGN APIENTRY _GpiPathToRegion(HPS a, LONG b, LONG c);

#undef  GpiPathToRegion
#define GpiPathToRegion _GpiPathToRegion

LONG APIENTRY _GpiPlayMetaFile(HPS a, HMF b, LONG c, PLONG d, PLONG e, LONG f, PSZ g);

#undef  GpiPlayMetaFile
#define GpiPlayMetaFile _GpiPlayMetaFile

LONG APIENTRY _GpiPointArc(HPS a, PPOINTL b);

#undef  GpiPointArc
#define GpiPointArc _GpiPointArc

LONG APIENTRY _GpiPolyFillet(HPS a, LONG b, PPOINTL c);

#undef  GpiPolyFillet
#define GpiPolyFillet _GpiPolyFillet

LONG APIENTRY _GpiPolyFilletSharp(HPS a, LONG b, PPOINTL c, PFIXED d);

#undef  GpiPolyFilletSharp
#define GpiPolyFilletSharp _GpiPolyFilletSharp

HRGN APIENTRY _GpiCreatePolygonRegion(HPS a, ULONG b, PPOLYGON c, ULONG d);

#undef  GpiCreatePolygonRegion
#define GpiCreatePolygonRegion _GpiCreatePolygonRegion

HRGN APIENTRY _GpiCreateEllipticRegion(HPS a, PRECTL b);

#undef  GpiCreateEllipticRegion
#define GpiCreateEllipticRegion _GpiCreateEllipticRegion

HRGN APIENTRY _GpiCreateRoundRectRegion(HPS a, PPOINTL b, LONG c, LONG d);

#undef  GpiCreateRoundRectRegion
#define GpiCreateRoundRectRegion _GpiCreateRoundRectRegion

LONG APIENTRY _GpiPolygons(HPS a, ULONG b, PPOLYGON c, ULONG d, ULONG e);

#undef  GpiPolygons
#define GpiPolygons _GpiPolygons

LONG APIENTRY _GpiPolyLine(HPS a, LONG b, PPOINTL c);

#undef  GpiPolyLine
#define GpiPolyLine _GpiPolyLine

LONG APIENTRY _GpiPolyLineDisjoint(HPS a, LONG b, PPOINTL c);

#undef  GpiPolyLineDisjoint
#define GpiPolyLineDisjoint _GpiPolyLineDisjoint

LONG APIENTRY _GpiPolyMarker(HPS a, LONG b, PPOINTL c);

#undef  GpiPolyMarker
#define GpiPolyMarker _GpiPolyMarker

LONG APIENTRY _GpiPolySpline(HPS a, LONG b, PPOINTL c);

#undef  GpiPolySpline
#define GpiPolySpline _GpiPolySpline

BOOL APIENTRY _GpiPop(HPS a, LONG b);

#undef  GpiPop
#define GpiPop _GpiPop

LONG APIENTRY _GpiPtInRegion(HPS a, HRGN b, PPOINTL c);

#undef  GpiPtInRegion
#define GpiPtInRegion _GpiPtInRegion

LONG APIENTRY _GpiPtVisible(HPS a, PPOINTL b);

#undef  GpiPtVisible
#define GpiPtVisible _GpiPtVisible

BOOL APIENTRY _GpiQueryArcParams(HPS a, PARCPARAMS b);

#undef  GpiQueryArcParams
#define GpiQueryArcParams _GpiQueryArcParams

LONG APIENTRY _GpiQueryAttrMode(HPS a);

#undef  GpiQueryAttrMode
#define GpiQueryAttrMode _GpiQueryAttrMode

LONG APIENTRY _GpiQueryAttrs(HPS a, LONG b, ULONG c, PBUNDLE d);

#undef  GpiQueryAttrs
#define GpiQueryAttrs _GpiQueryAttrs

LONG APIENTRY _GpiQueryBackColor(HPS a);

#undef  GpiQueryBackColor
#define GpiQueryBackColor _GpiQueryBackColor

LONG APIENTRY _GpiQueryBackMix(HPS a);

#undef  GpiQueryBackMix
#define GpiQueryBackMix _GpiQueryBackMix

BOOL APIENTRY _GpiQueryCharAngle(HPS a, PGRADIENTL b);

#undef  GpiQueryCharAngle
#define GpiQueryCharAngle _GpiQueryCharAngle

BOOL APIENTRY _GpiQueryCharBox(HPS a, PSIZEF b);

#undef  GpiQueryCharBox
#define GpiQueryCharBox _GpiQueryCharBox

BOOL APIENTRY _GpiQueryCharBreakExtra(HPS a, PFIXED b);

#undef  GpiQueryCharBreakExtra
#define GpiQueryCharBreakExtra _GpiQueryCharBreakExtra

LONG APIENTRY _GpiQueryCharDirection(HPS a);

#undef  GpiQueryCharDirection
#define GpiQueryCharDirection _GpiQueryCharDirection

BOOL APIENTRY _GpiQueryCharExtra(HPS a, PFIXED b);

#undef  GpiQueryCharExtra
#define GpiQueryCharExtra _GpiQueryCharExtra

LONG APIENTRY _GpiQueryCharMode(HPS a);

#undef  GpiQueryCharMode
#define GpiQueryCharMode _GpiQueryCharMode

LONG APIENTRY _GpiQueryCharSet(HPS a);

#undef  GpiQueryCharSet
#define GpiQueryCharSet _GpiQueryCharSet

BOOL APIENTRY _GpiQueryCharShear(HPS a, PPOINTL b);

#undef  GpiQueryCharShear
#define GpiQueryCharShear _GpiQueryCharShear

BOOL APIENTRY _GpiQueryCharStringPos(HPS a, ULONG b, LONG c, PCH d, PLONG e, PPOINTL f);

#undef  GpiQueryCharStringPos
#define GpiQueryCharStringPos _GpiQueryCharStringPos

BOOL APIENTRY _GpiQueryCharStringPosAt(HPS a, PPOINTL b, ULONG c, LONG d, PCH e, PLONG f, PPOINTL g);

#undef  GpiQueryCharStringPosAt
#define GpiQueryCharStringPosAt _GpiQueryCharStringPosAt

LONG APIENTRY _GpiQueryClipBox(HPS a, PRECTL b);

#undef  GpiQueryClipBox
#define GpiQueryClipBox _GpiQueryClipBox

HRGN APIENTRY _GpiQueryClipRegion(HPS a);

#undef  GpiQueryClipRegion
#define GpiQueryClipRegion _GpiQueryClipRegion

LONG APIENTRY _GpiQueryColor(HPS a);

#undef  GpiQueryColor
#define GpiQueryColor _GpiQueryColor

BOOL APIENTRY _GpiQueryColorData(HPS a, LONG b, PLONG c);

#undef  GpiQueryColorData
#define GpiQueryColorData _GpiQueryColorData

LONG APIENTRY _GpiQueryColorIndex(HPS a, ULONG b, LONG c);

#undef  GpiQueryColorIndex
#define GpiQueryColorIndex _GpiQueryColorIndex

ULONG APIENTRY _GpiQueryCp(HPS a);

#undef  GpiQueryCp
#define GpiQueryCp _GpiQueryCp

BOOL APIENTRY _GpiQueryCurrentPosition(HPS a, PPOINTL b);

#undef  GpiQueryCurrentPosition
#define GpiQueryCurrentPosition _GpiQueryCurrentPosition

BOOL APIENTRY _GpiQueryDefArcParams(HPS a, PARCPARAMS b);

#undef  GpiQueryDefArcParams
#define GpiQueryDefArcParams _GpiQueryDefArcParams

BOOL APIENTRY _GpiQueryDefAttrs(HPS a, LONG b, ULONG c, PBUNDLE d);

#undef  GpiQueryDefAttrs
#define GpiQueryDefAttrs _GpiQueryDefAttrs

BOOL APIENTRY _GpiQueryDefCharBox(HPS a, PSIZEL b);

#undef  GpiQueryDefCharBox
#define GpiQueryDefCharBox _GpiQueryDefCharBox

BOOL APIENTRY _GpiQueryDefTag(HPS a, PLONG b);

#undef  GpiQueryDefTag
#define GpiQueryDefTag _GpiQueryDefTag

BOOL APIENTRY _GpiQueryDefViewingLimits(HPS a, PRECTL b);

#undef  GpiQueryDefViewingLimits
#define GpiQueryDefViewingLimits _GpiQueryDefViewingLimits

BOOL APIENTRY _GpiQueryDefaultViewMatrix(HPS a, LONG b, PMATRIXLF c);

#undef  GpiQueryDefaultViewMatrix
#define GpiQueryDefaultViewMatrix _GpiQueryDefaultViewMatrix

LONG APIENTRY _GpiQueryEditMode(HPS a);

#undef  GpiQueryEditMode
#define GpiQueryEditMode _GpiQueryEditMode

LONG APIENTRY _GpiQueryElement(HPS a, LONG b, LONG c, PBYTE d);

#undef  GpiQueryElement
#define GpiQueryElement _GpiQueryElement

LONG APIENTRY _GpiQueryElementPointer(HPS a);

#undef  GpiQueryElementPointer
#define GpiQueryElementPointer _GpiQueryElementPointer

LONG APIENTRY _GpiQueryElementType(HPS a, PLONG b, LONG c, PSZ d);

#undef  GpiQueryElementType
#define GpiQueryElementType _GpiQueryElementType

ULONG APIENTRY _GpiQueryFaceString(HPS a, PCSZ b, PFACENAMEDESC c, LONG d, PSZ e);

#undef  GpiQueryFaceString
#define GpiQueryFaceString _GpiQueryFaceString

ULONG APIENTRY _GpiQueryFontAction(HAB a, ULONG b);

#undef  GpiQueryFontAction
#define GpiQueryFontAction _GpiQueryFontAction

LONG APIENTRY _GpiQueryFontFileDescriptions(HAB a, PCSZ b, PLONG c, PFFDESCS d);

#undef  GpiQueryFontFileDescriptions
#define GpiQueryFontFileDescriptions _GpiQueryFontFileDescriptions

BOOL APIENTRY _GpiQueryFontMetrics(HPS a, LONG b, PFONTMETRICS c);

#undef  GpiQueryFontMetrics
#define GpiQueryFontMetrics _GpiQueryFontMetrics

LONG APIENTRY _GpiQueryFonts(HPS a, ULONG b, PCSZ c, PLONG d, LONG e, PFONTMETRICS f);

#undef  GpiQueryFonts
#define GpiQueryFonts _GpiQueryFonts

LONG APIENTRY _GpiQueryFullFontFileDescs(HAB a, PCSZ b, PLONG c, PVOID d, PLONG e);

#undef  GpiQueryFullFontFileDescs
#define GpiQueryFullFontFileDescs _GpiQueryFullFontFileDescs

BOOL APIENTRY _GpiQueryGraphicsField(HPS a, PRECTL b);

#undef  GpiQueryGraphicsField
#define GpiQueryGraphicsField _GpiQueryGraphicsField

LONG APIENTRY _GpiQueryKerningPairs(HPS a, LONG b, PKERNINGPAIRS c);

#undef  GpiQueryKerningPairs
#define GpiQueryKerningPairs _GpiQueryKerningPairs

LONG APIENTRY _GpiQueryLineEnd(HPS a);

#undef  GpiQueryLineEnd
#define GpiQueryLineEnd _GpiQueryLineEnd

LONG APIENTRY _GpiQueryLineJoin(HPS a);

#undef  GpiQueryLineJoin
#define GpiQueryLineJoin _GpiQueryLineJoin

LONG APIENTRY _GpiQueryLineType(HPS a);

#undef  GpiQueryLineType
#define GpiQueryLineType _GpiQueryLineType

FIXED APIENTRY _GpiQueryLineWidth(HPS a);

#undef  GpiQueryLineWidth
#define GpiQueryLineWidth _GpiQueryLineWidth

LONG APIENTRY _GpiQueryLineWidthGeom(HPS a);

#undef  GpiQueryLineWidthGeom
#define GpiQueryLineWidthGeom _GpiQueryLineWidthGeom

LONG APIENTRY _GpiQueryLogColorTable(HPS a, ULONG b, LONG c, LONG d, PLONG e);

#undef  GpiQueryLogColorTable
#define GpiQueryLogColorTable _GpiQueryLogColorTable

BOOL APIENTRY _GpiQueryLogicalFont(HPS a, LONG b, PSTR8 c, PFATTRS d, LONG e);

#undef  GpiQueryLogicalFont
#define GpiQueryLogicalFont _GpiQueryLogicalFont

LONG APIENTRY _GpiQueryMarker(HPS a);

#undef  GpiQueryMarker
#define GpiQueryMarker _GpiQueryMarker

BOOL APIENTRY _GpiQueryMarkerBox(HPS a, PSIZEF b);

#undef  GpiQueryMarkerBox
#define GpiQueryMarkerBox _GpiQueryMarkerBox

LONG APIENTRY _GpiQueryMarkerSet(HPS a);

#undef  GpiQueryMarkerSet
#define GpiQueryMarkerSet _GpiQueryMarkerSet

BOOL APIENTRY _GpiQueryMetaFileBits(HMF a, LONG b, LONG c, PBYTE d);

#undef  GpiQueryMetaFileBits
#define GpiQueryMetaFileBits _GpiQueryMetaFileBits

LONG APIENTRY _GpiQueryMetaFileLength(HMF a);

#undef  GpiQueryMetaFileLength
#define GpiQueryMetaFileLength _GpiQueryMetaFileLength

LONG APIENTRY _GpiQueryMix(HPS a);

#undef  GpiQueryMix
#define GpiQueryMix _GpiQueryMix

BOOL APIENTRY _GpiQueryModelTransformMatrix(HPS a, LONG b, PMATRIXLF c);

#undef  GpiQueryModelTransformMatrix
#define GpiQueryModelTransformMatrix _GpiQueryModelTransformMatrix

LONG APIENTRY _GpiQueryNearestColor(HPS a, ULONG b, LONG c);

#undef  GpiQueryNearestColor
#define GpiQueryNearestColor _GpiQueryNearestColor

LONG APIENTRY _GpiQueryNumberSetIds(HPS a);

#undef  GpiQueryNumberSetIds
#define GpiQueryNumberSetIds _GpiQueryNumberSetIds

BOOL APIENTRY _GpiQueryPageViewport(HPS a, PRECTL b);

#undef  GpiQueryPageViewport
#define GpiQueryPageViewport _GpiQueryPageViewport

HPAL APIENTRY _GpiQueryPalette(HPS a);

#undef  GpiQueryPalette
#define GpiQueryPalette _GpiQueryPalette

LONG APIENTRY _GpiQueryPaletteInfo(HPAL a, HPS b, ULONG c, ULONG d, ULONG e, PULONG f);

#undef  GpiQueryPaletteInfo
#define GpiQueryPaletteInfo _GpiQueryPaletteInfo

LONG APIENTRY _GpiQueryPattern(HPS a);

#undef  GpiQueryPattern
#define GpiQueryPattern _GpiQueryPattern

BOOL APIENTRY _GpiQueryPatternRefPoint(HPS a, PPOINTL b);

#undef  GpiQueryPatternRefPoint
#define GpiQueryPatternRefPoint _GpiQueryPatternRefPoint

LONG APIENTRY _GpiQueryPatternSet(HPS a);

#undef  GpiQueryPatternSet
#define GpiQueryPatternSet _GpiQueryPatternSet

LONG APIENTRY _GpiQueryRealColors(HPS a, ULONG b, LONG c, LONG d, PLONG e);

#undef  GpiQueryRealColors
#define GpiQueryRealColors _GpiQueryRealColors

LONG APIENTRY _GpiQueryRegionBox(HPS a, HRGN b, PRECTL c);

#undef  GpiQueryRegionBox
#define GpiQueryRegionBox _GpiQueryRegionBox

BOOL APIENTRY _GpiQueryRegionRects(HPS a, HRGN b, PRECTL c, PRGNRECT d, PRECTL e);

#undef  GpiQueryRegionRects
#define GpiQueryRegionRects _GpiQueryRegionRects

LONG APIENTRY _GpiQueryRGBColor(HPS a, ULONG b, LONG c);

#undef  GpiQueryRGBColor
#define GpiQueryRGBColor _GpiQueryRGBColor

BOOL APIENTRY _GpiQuerySegmentTransformMatrix(HPS a, LONG b, LONG c, PMATRIXLF d);

#undef  GpiQuerySegmentTransformMatrix
#define GpiQuerySegmentTransformMatrix _GpiQuerySegmentTransformMatrix

BOOL APIENTRY _GpiQuerySetIds(HPS a, LONG b, PLONG c, PSTR8 d, PLONG e);

#undef  GpiQuerySetIds
#define GpiQuerySetIds _GpiQuerySetIds

BOOL APIENTRY _GpiQueryTextAlignment(HPS a, PLONG b, PLONG c);

#undef  GpiQueryTextAlignment
#define GpiQueryTextAlignment _GpiQueryTextAlignment

BOOL APIENTRY _GpiQueryTextBox(HPS a, LONG b, PCH c, LONG d, PPOINTL e);

#undef  GpiQueryTextBox
#define GpiQueryTextBox _GpiQueryTextBox

BOOL APIENTRY _GpiQueryViewingLimits(HPS a, PRECTL b);

#undef  GpiQueryViewingLimits
#define GpiQueryViewingLimits _GpiQueryViewingLimits

BOOL APIENTRY _GpiQueryViewingTransformMatrix(HPS a, LONG b, PMATRIXLF c);

#undef  GpiQueryViewingTransformMatrix
#define GpiQueryViewingTransformMatrix _GpiQueryViewingTransformMatrix

BOOL APIENTRY _GpiQueryWidthTable(HPS a, LONG b, LONG c, PLONG d);

#undef  GpiQueryWidthTable
#define GpiQueryWidthTable _GpiQueryWidthTable

LONG APIENTRY _GpiRectInRegion(HPS a, HRGN b, PRECTL c);

#undef  GpiRectInRegion
#define GpiRectInRegion _GpiRectInRegion

LONG APIENTRY _GpiRectVisible(HPS a, PRECTL b);

#undef  GpiRectVisible
#define GpiRectVisible _GpiRectVisible

BOOL APIENTRY _GpiRotate(HPS a, PMATRIXLF b, LONG c, FIXED d, PPOINTL e);

#undef  GpiRotate
#define GpiRotate _GpiRotate

BOOL APIENTRY _GpiSaveMetaFile(HMF a, PCSZ b);

#undef  GpiSaveMetaFile
#define GpiSaveMetaFile _GpiSaveMetaFile

BOOL APIENTRY _GpiScale(HPS a, PMATRIXLF b, LONG c, PFIXED d, PPOINTL e);

#undef  GpiScale
#define GpiScale _GpiScale

HPAL APIENTRY _GpiSelectPalette(HPS a, HPAL b);

#undef  GpiSelectPalette
#define GpiSelectPalette _GpiSelectPalette

BOOL APIENTRY _GpiSetArcParams(HPS a, PARCPARAMS b);

#undef  GpiSetArcParams
#define GpiSetArcParams _GpiSetArcParams

BOOL APIENTRY _GpiSetAttrMode(HPS a, LONG b);

#undef  GpiSetAttrMode
#define GpiSetAttrMode _GpiSetAttrMode

BOOL APIENTRY _GpiSetAttrs(HPS a, LONG b, ULONG c, ULONG d, PVOID e);

#undef  GpiSetAttrs
#define GpiSetAttrs _GpiSetAttrs

BOOL APIENTRY _GpiSetBackColor(HPS a, LONG b);

#undef  GpiSetBackColor
#define GpiSetBackColor _GpiSetBackColor

BOOL APIENTRY _GpiSetBackMix(HPS a, LONG b);

#undef  GpiSetBackMix
#define GpiSetBackMix _GpiSetBackMix

BOOL APIENTRY _GpiSetCharAngle(HPS a, PGRADIENTL b);

#undef  GpiSetCharAngle
#define GpiSetCharAngle _GpiSetCharAngle

BOOL APIENTRY _GpiSetCharBox(HPS a, PSIZEF b);

#undef  GpiSetCharBox
#define GpiSetCharBox _GpiSetCharBox

BOOL APIENTRY _GpiSetCharBreakExtra(HPS a, FIXED b);

#undef  GpiSetCharBreakExtra
#define GpiSetCharBreakExtra _GpiSetCharBreakExtra

BOOL APIENTRY _GpiSetCharDirection(HPS a, LONG b);

#undef  GpiSetCharDirection
#define GpiSetCharDirection _GpiSetCharDirection

BOOL APIENTRY _GpiSetCharExtra(HPS a, FIXED b);

#undef  GpiSetCharExtra
#define GpiSetCharExtra _GpiSetCharExtra

BOOL APIENTRY _GpiSetCharMode(HPS a, LONG b);

#undef  GpiSetCharMode
#define GpiSetCharMode _GpiSetCharMode

BOOL APIENTRY _GpiSetCharSet(HPS a, LONG b);

#undef  GpiSetCharSet
#define GpiSetCharSet _GpiSetCharSet

BOOL APIENTRY _GpiSetCharShear(HPS a, PPOINTL b);

#undef  GpiSetCharShear
#define GpiSetCharShear _GpiSetCharShear

BOOL APIENTRY _GpiSetClipPath(HPS a, LONG b, LONG c);

#undef  GpiSetClipPath
#define GpiSetClipPath _GpiSetClipPath

LONG APIENTRY _GpiSetClipRegion(HPS a, HRGN b, PHRGN c);

#undef  GpiSetClipRegion
#define GpiSetClipRegion _GpiSetClipRegion

BOOL APIENTRY _GpiSetColor(HPS a, LONG b);

#undef  GpiSetColor
#define GpiSetColor _GpiSetColor

BOOL APIENTRY _GpiSetCp(HPS a, ULONG b);

#undef  GpiSetCp
#define GpiSetCp _GpiSetCp

BOOL APIENTRY _GpiSetCurrentPosition(HPS a, PPOINTL b);

#undef  GpiSetCurrentPosition
#define GpiSetCurrentPosition _GpiSetCurrentPosition

BOOL APIENTRY _GpiSetDefArcParams(HPS a, PARCPARAMS b);

#undef  GpiSetDefArcParams
#define GpiSetDefArcParams _GpiSetDefArcParams

BOOL APIENTRY _GpiSetDefAttrs(HPS a, LONG b, ULONG c, PVOID d);

#undef  GpiSetDefAttrs
#define GpiSetDefAttrs _GpiSetDefAttrs

BOOL APIENTRY _GpiSetDefaultViewMatrix(HPS a, LONG b, PMATRIXLF c, LONG d);

#undef  GpiSetDefaultViewMatrix
#define GpiSetDefaultViewMatrix _GpiSetDefaultViewMatrix

BOOL APIENTRY _GpiSetDefTag(HPS a, LONG b);

#undef  GpiSetDefTag
#define GpiSetDefTag _GpiSetDefTag

BOOL APIENTRY _GpiSetDefViewingLimits(HPS a, PRECTL b);

#undef  GpiSetDefViewingLimits
#define GpiSetDefViewingLimits _GpiSetDefViewingLimits

BOOL APIENTRY _GpiSetEditMode(HPS a, LONG b);

#undef  GpiSetEditMode
#define GpiSetEditMode _GpiSetEditMode

BOOL APIENTRY _GpiSetElementPointer(HPS a, LONG b);

#undef  GpiSetElementPointer
#define GpiSetElementPointer _GpiSetElementPointer

BOOL APIENTRY _GpiSetElementPointerAtLabel(HPS a, LONG b);

#undef  GpiSetElementPointerAtLabel
#define GpiSetElementPointerAtLabel _GpiSetElementPointerAtLabel

BOOL APIENTRY _GpiSetGraphicsField(HPS a, PRECTL b);

#undef  GpiSetGraphicsField
#define GpiSetGraphicsField _GpiSetGraphicsField

BOOL APIENTRY _GpiSetLineEnd(HPS a, LONG b);

#undef  GpiSetLineEnd
#define GpiSetLineEnd _GpiSetLineEnd

BOOL APIENTRY _GpiSetLineJoin(HPS a, LONG b);

#undef  GpiSetLineJoin
#define GpiSetLineJoin _GpiSetLineJoin

BOOL APIENTRY _GpiSetLineType(HPS a, LONG b);

#undef  GpiSetLineType
#define GpiSetLineType _GpiSetLineType

BOOL APIENTRY _GpiSetLineWidth(HPS a, FIXED b);

#undef  GpiSetLineWidth
#define GpiSetLineWidth _GpiSetLineWidth

BOOL APIENTRY _GpiSetLineWidthGeom(HPS a, LONG b);

#undef  GpiSetLineWidthGeom
#define GpiSetLineWidthGeom _GpiSetLineWidthGeom

BOOL APIENTRY _GpiSetMarker(HPS a, LONG b);

#undef  GpiSetMarker
#define GpiSetMarker _GpiSetMarker

BOOL APIENTRY _GpiSetMarkerBox(HPS a, PSIZEF b);

#undef  GpiSetMarkerBox
#define GpiSetMarkerBox _GpiSetMarkerBox

BOOL APIENTRY _GpiSetMarkerSet(HPS a, LONG b);

#undef  GpiSetMarkerSet
#define GpiSetMarkerSet _GpiSetMarkerSet

BOOL APIENTRY _GpiSetMetaFileBits(HMF a, LONG b, LONG c, PBYTE d);

#undef  GpiSetMetaFileBits
#define GpiSetMetaFileBits _GpiSetMetaFileBits

BOOL APIENTRY _GpiSetMix(HPS a, LONG b);

#undef  GpiSetMix
#define GpiSetMix _GpiSetMix

BOOL APIENTRY _GpiSetModelTransformMatrix(HPS a, LONG b, PMATRIXLF c, LONG d);

#undef  GpiSetModelTransformMatrix
#define GpiSetModelTransformMatrix _GpiSetModelTransformMatrix

BOOL APIENTRY _GpiSetPageViewport(HPS a, PRECTL b);

#undef  GpiSetPageViewport
#define GpiSetPageViewport _GpiSetPageViewport

BOOL APIENTRY _GpiSetPaletteEntries(HPAL a, ULONG b, ULONG c, ULONG d, ULONG *e);

#undef  GpiSetPaletteEntries
#define GpiSetPaletteEntries _GpiSetPaletteEntries

BOOL APIENTRY _GpiSetPattern(HPS a, LONG b);

#undef  GpiSetPattern
#define GpiSetPattern _GpiSetPattern

BOOL APIENTRY _GpiSetPatternRefPoint(HPS a, PPOINTL b);

#undef  GpiSetPatternRefPoint
#define GpiSetPatternRefPoint _GpiSetPatternRefPoint

BOOL APIENTRY _GpiSetPatternSet(HPS a, LONG b);

#undef  GpiSetPatternSet
#define GpiSetPatternSet _GpiSetPatternSet

BOOL APIENTRY _GpiSetRegion(HPS a, HRGN b, LONG c, PRECTL d);

#undef  GpiSetRegion
#define GpiSetRegion _GpiSetRegion

BOOL APIENTRY _GpiSetSegmentTransformMatrix(HPS a, LONG b, LONG c, MATRIXLF *d, LONG e);

#undef  GpiSetSegmentTransformMatrix
#define GpiSetSegmentTransformMatrix _GpiSetSegmentTransformMatrix

BOOL APIENTRY _GpiSetTextAlignment(HPS a, LONG b, LONG c);

#undef  GpiSetTextAlignment
#define GpiSetTextAlignment _GpiSetTextAlignment

BOOL APIENTRY _GpiSetViewingLimits(HPS a, PRECTL b);

#undef  GpiSetViewingLimits
#define GpiSetViewingLimits _GpiSetViewingLimits

BOOL APIENTRY _GpiSetViewingTransformMatrix(HPS a, LONG b, MATRIXLF *c, LONG d);

#undef  GpiSetViewingTransformMatrix
#define GpiSetViewingTransformMatrix _GpiSetViewingTransformMatrix

LONG APIENTRY _GpiStrokePath(HPS a, LONG b, ULONG c);

#undef  GpiStrokePath
#define GpiStrokePath _GpiStrokePath

BOOL APIENTRY _GpiTranslate(HPS a, PMATRIXLF b, LONG c, PPOINTL d);

#undef  GpiTranslate
#define GpiTranslate _GpiTranslate

BOOL APIENTRY _GpiUnloadFonts(HAB a, PCSZ b);

#undef  GpiUnloadFonts
#define GpiUnloadFonts _GpiUnloadFonts

BOOL APIENTRY _GpiUnloadPublicFonts(HAB a, PCSZ b);

#undef  GpiUnloadPublicFonts
#define GpiUnloadPublicFonts _GpiUnloadPublicFonts

#ifdef INCL_GPIBITMAPS
LONG APIENTRY _GpiBitBlt(HPS a, HPS b, LONG c, PPOINTL d, LONG e, ULONG f);

#undef  GpiBitBlt
#define GpiBitBlt _GpiBitBlt

BOOL APIENTRY _GpiDeleteBitmap(HBITMAP a);

#undef  GpiDeleteBitmap
#define GpiDeleteBitmap _GpiDeleteBitmap

HBITMAP APIENTRY _GpiLoadBitmap(HPS a, HMODULE b, ULONG c, LONG d, LONG e);

#undef  GpiLoadBitmap
#define GpiLoadBitmap _GpiLoadBitmap

HBITMAP APIENTRY _GpiSetBitmap(HPS a, HBITMAP b);

#undef  GpiSetBitmap
#define GpiSetBitmap _GpiSetBitmap

LONG APIENTRY _GpiWCBitBlt(HPS a, HBITMAP b, LONG c, PPOINTL d, LONG e, ULONG f);

#undef  GpiWCBitBlt
#define GpiWCBitBlt _GpiWCBitBlt

HBITMAP APIENTRY _GpiCreateBitmap(HPS a, BITMAPINFOHEADER2 *b, ULONG c, PBYTE d, BITMAPINFO2 *e);

#undef  GpiCreateBitmap
#define GpiCreateBitmap _GpiCreateBitmap

LONG APIENTRY _GpiDrawBits(HPS a, PVOID b, BITMAPINFO2 *c, LONG d, PPOINTL e, LONG f, ULONG g);

#undef  GpiDrawBits
#define GpiDrawBits _GpiDrawBits

LONG APIENTRY _GpiFloodFill(HPS a, LONG b, LONG c);

#undef  GpiFloodFill
#define GpiFloodFill _GpiFloodFill

LONG APIENTRY _GpiQueryBitmapBits(HPS a, LONG b, LONG c, PBYTE d, PBITMAPINFO2 e);

#undef  GpiQueryBitmapBits
#define GpiQueryBitmapBits _GpiQueryBitmapBits

BOOL APIENTRY _GpiQueryBitmapDimension(HBITMAP a, PSIZEL b);

#undef  GpiQueryBitmapDimension
#define GpiQueryBitmapDimension _GpiQueryBitmapDimension

HBITMAP APIENTRY _GpiQueryBitmapHandle(HPS a, LONG b);

#undef  GpiQueryBitmapHandle
#define GpiQueryBitmapHandle _GpiQueryBitmapHandle

BOOL APIENTRY _GpiQueryBitmapInfoHeader(HBITMAP a, PBITMAPINFOHEADER2 b);

#undef  GpiQueryBitmapInfoHeader
#define GpiQueryBitmapInfoHeader _GpiQueryBitmapInfoHeader

BOOL APIENTRY _GpiQueryBitmapParameters(HBITMAP a, PBITMAPINFOHEADER b);

#undef  GpiQueryBitmapParameters
#define GpiQueryBitmapParameters _GpiQueryBitmapParameters

BOOL APIENTRY _GpiQueryDeviceBitmapFormats(HPS a, LONG b, PLONG c);

#undef  GpiQueryDeviceBitmapFormats
#define GpiQueryDeviceBitmapFormats _GpiQueryDeviceBitmapFormats

LONG APIENTRY _GpiSetBitmapBits(HPS a, LONG b, LONG c, PBYTE d, BITMAPINFO2 *e);

#undef  GpiSetBitmapBits
#define GpiSetBitmapBits _GpiSetBitmapBits

LONG APIENTRY _GpiQueryPel(HPS a, PPOINTL b);

#undef  GpiQueryPel
#define GpiQueryPel _GpiQueryPel

BOOL APIENTRY _GpiSetBitmapDimension(HBITMAP a, SIZEL *b);

#undef  GpiSetBitmapDimension
#define GpiSetBitmapDimension _GpiSetBitmapDimension

BOOL APIENTRY _GpiSetBitmapId(HPS a, HBITMAP b, LONG c);

#undef  GpiSetBitmapId
#define GpiSetBitmapId _GpiSetBitmapId

LONG APIENTRY _GpiSetPel(HPS a, PPOINTL b);

#undef  GpiSetPel
#define GpiSetPel _GpiSetPel

#endif
#ifdef INCL_GPICONTROL
BOOL APIENTRY _GpiAssociate(HPS a, HDC b);

#undef  GpiAssociate
#define GpiAssociate _GpiAssociate

HPS APIENTRY _GpiCreatePS(HAB a, HDC b, PSIZEL c, ULONG d);

#undef  GpiCreatePS
#define GpiCreatePS _GpiCreatePS

BOOL APIENTRY _GpiDestroyPS(HPS a);

#undef  GpiDestroyPS
#define GpiDestroyPS _GpiDestroyPS

BOOL APIENTRY _GpiErase(HPS a);

#undef  GpiErase
#define GpiErase _GpiErase

HDC APIENTRY _GpiQueryDevice(HPS a);

#undef  GpiQueryDevice
#define GpiQueryDevice _GpiQueryDevice

BOOL APIENTRY _GpiRestorePS(HPS a, LONG b);

#undef  GpiRestorePS
#define GpiRestorePS _GpiRestorePS

LONG APIENTRY _GpiSavePS(HPS a);

#undef  GpiSavePS
#define GpiSavePS _GpiSavePS

LONG APIENTRY _GpiErrorSegmentData(HPS a, PLONG b, PLONG c);

#undef  GpiErrorSegmentData
#define GpiErrorSegmentData _GpiErrorSegmentData

LONG APIENTRY _GpiQueryDrawControl(HPS a, LONG b);

#undef  GpiQueryDrawControl
#define GpiQueryDrawControl _GpiQueryDrawControl

LONG APIENTRY _GpiQueryDrawingMode(HPS a);

#undef  GpiQueryDrawingMode
#define GpiQueryDrawingMode _GpiQueryDrawingMode

ULONG APIENTRY _GpiQueryPS(HPS a, PSIZEL b);

#undef  GpiQueryPS
#define GpiQueryPS _GpiQueryPS

BOOL APIENTRY _GpiResetPS(HPS a, ULONG b);

#undef  GpiResetPS
#define GpiResetPS _GpiResetPS

LONG APIENTRY _GpiQueryStopDraw(HPS a);

#undef  GpiQueryStopDraw
#define GpiQueryStopDraw _GpiQueryStopDraw

BOOL APIENTRY _GpiSetDrawControl(HPS a, LONG b, LONG c);

#undef  GpiSetDrawControl
#define GpiSetDrawControl _GpiSetDrawControl

BOOL APIENTRY _GpiSetDrawingMode(HPS a, LONG b);

#undef  GpiSetDrawingMode
#define GpiSetDrawingMode _GpiSetDrawingMode

BOOL APIENTRY _GpiSetPS(HPS a, SIZEL *b, ULONG c);

#undef  GpiSetPS
#define GpiSetPS _GpiSetPS

BOOL APIENTRY _GpiSetStopDraw(HPS a, LONG b);

#undef  GpiSetStopDraw
#define GpiSetStopDraw _GpiSetStopDraw

#endif
#ifdef INCL_GPICORRELATION
LONG APIENTRY _GpiCorrelateChain(HPS a, LONG b, PPOINTL c, LONG d, LONG e, PLONG f);

#undef  GpiCorrelateChain
#define GpiCorrelateChain _GpiCorrelateChain

LONG APIENTRY _GpiCorrelateFrom(HPS a, LONG b, LONG c, LONG d, PPOINTL e, LONG f, LONG g, PLONG h);

#undef  GpiCorrelateFrom
#define GpiCorrelateFrom _GpiCorrelateFrom

LONG APIENTRY _GpiCorrelateSegment(HPS a, LONG b, LONG c, PPOINTL d, LONG e, LONG f, PLONG g);

#undef  GpiCorrelateSegment
#define GpiCorrelateSegment _GpiCorrelateSegment

BOOL APIENTRY _GpiQueryBoundaryData(HPS a, PRECTL b);

#undef  GpiQueryBoundaryData
#define GpiQueryBoundaryData _GpiQueryBoundaryData

BOOL APIENTRY _GpiQueryPickAperturePosition(HPS a, PPOINTL b);

#undef  GpiQueryPickAperturePosition
#define GpiQueryPickAperturePosition _GpiQueryPickAperturePosition

BOOL APIENTRY _GpiQueryPickApertureSize(HPS a, PSIZEL b);

#undef  GpiQueryPickApertureSize
#define GpiQueryPickApertureSize _GpiQueryPickApertureSize

BOOL APIENTRY _GpiQueryTag(HPS a, PLONG b);

#undef  GpiQueryTag
#define GpiQueryTag _GpiQueryTag

BOOL APIENTRY _GpiResetBoundaryData(HPS a);

#undef  GpiResetBoundaryData
#define GpiResetBoundaryData _GpiResetBoundaryData

BOOL APIENTRY _GpiSetPickAperturePosition(HPS a, PPOINTL b);

#undef  GpiSetPickAperturePosition
#define GpiSetPickAperturePosition _GpiSetPickAperturePosition

BOOL APIENTRY _GpiSetPickApertureSize(HPS a, LONG b, SIZEL *c);

#undef  GpiSetPickApertureSize
#define GpiSetPickApertureSize _GpiSetPickApertureSize

BOOL APIENTRY _GpiSetTag(HPS a, LONG b);

#undef  GpiSetTag
#define GpiSetTag _GpiSetTag

#endif
#ifdef INCL_GPIINK
BOOL APIENTRY _GpiBeginInkPath(HPS a, LONG b, ULONG c);

#undef  GpiBeginInkPath
#define GpiBeginInkPath _GpiBeginInkPath

BOOL APIENTRY _GpiEndInkPath(HPS a, ULONG b);

#undef  GpiEndInkPath
#define GpiEndInkPath _GpiEndInkPath

LONG APIENTRY _GpiStrokeInkPath(HPS a, LONG b, LONG c, PPOINTL d, ULONG e);

#undef  GpiStrokeInkPath
#define GpiStrokeInkPath _GpiStrokeInkPath

#endif
#ifdef INCL_GPISEGMENTS
BOOL APIENTRY _GpiCloseSegment(HPS a);

#undef  GpiCloseSegment
#define GpiCloseSegment _GpiCloseSegment

BOOL APIENTRY _GpiDeleteSegment(HPS a, LONG b);

#undef  GpiDeleteSegment
#define GpiDeleteSegment _GpiDeleteSegment

BOOL APIENTRY _GpiDeleteSegments(HPS a, LONG b, LONG c);

#undef  GpiDeleteSegments
#define GpiDeleteSegments _GpiDeleteSegments

BOOL APIENTRY _GpiDrawChain(HPS a);

#undef  GpiDrawChain
#define GpiDrawChain _GpiDrawChain

BOOL APIENTRY _GpiDrawDynamics(HPS a);

#undef  GpiDrawDynamics
#define GpiDrawDynamics _GpiDrawDynamics

BOOL APIENTRY _GpiDrawFrom(HPS a, LONG b, LONG c);

#undef  GpiDrawFrom
#define GpiDrawFrom _GpiDrawFrom

BOOL APIENTRY _GpiDrawSegment(HPS a, LONG b);

#undef  GpiDrawSegment
#define GpiDrawSegment _GpiDrawSegment

LONG APIENTRY _GpiGetData(HPS a, LONG b, PLONG c, LONG d, LONG e, PBYTE f);

#undef  GpiGetData
#define GpiGetData _GpiGetData

BOOL APIENTRY _GpiOpenSegment(HPS a, LONG b);

#undef  GpiOpenSegment
#define GpiOpenSegment _GpiOpenSegment

LONG APIENTRY _GpiPutData(HPS a, LONG b, PLONG c, PBYTE d);

#undef  GpiPutData
#define GpiPutData _GpiPutData

LONG APIENTRY _GpiQueryInitialSegmentAttrs(HPS a, LONG b);

#undef  GpiQueryInitialSegmentAttrs
#define GpiQueryInitialSegmentAttrs _GpiQueryInitialSegmentAttrs

LONG APIENTRY _GpiQuerySegmentAttrs(HPS a, LONG b, LONG c);

#undef  GpiQuerySegmentAttrs
#define GpiQuerySegmentAttrs _GpiQuerySegmentAttrs

LONG APIENTRY _GpiQuerySegmentNames(HPS a, LONG b, LONG c, LONG d, PLONG e);

#undef  GpiQuerySegmentNames
#define GpiQuerySegmentNames _GpiQuerySegmentNames

LONG APIENTRY _GpiQuerySegmentPriority(HPS a, LONG b, LONG c);

#undef  GpiQuerySegmentPriority
#define GpiQuerySegmentPriority _GpiQuerySegmentPriority

BOOL APIENTRY _GpiRemoveDynamics(HPS a, LONG b, LONG c);

#undef  GpiRemoveDynamics
#define GpiRemoveDynamics _GpiRemoveDynamics

BOOL APIENTRY _GpiSetInitialSegmentAttrs(HPS a, LONG b, LONG c);

#undef  GpiSetInitialSegmentAttrs
#define GpiSetInitialSegmentAttrs _GpiSetInitialSegmentAttrs

BOOL APIENTRY _GpiSetSegmentAttrs(HPS a, LONG b, LONG c, LONG d);

#undef  GpiSetSegmentAttrs
#define GpiSetSegmentAttrs _GpiSetSegmentAttrs

BOOL APIENTRY _GpiSetSegmentPriority(HPS a, LONG b, LONG c, LONG d);

#undef  GpiSetSegmentPriority
#define GpiSetSegmentPriority _GpiSetSegmentPriority

#endif
#ifdef INCL_DEV
LONG APIENTRY _DevEscape(HDC a, LONG b, LONG c, PBYTE d, PLONG e, PBYTE f);

#undef  DevEscape
#define DevEscape _DevEscape

LONG APIENTRY _DevPostEscape(PCSZ a, PCSZ b, PCSZ c, PCSZ d, ULONG e, ULONG f, PBYTE g, ULONG h, PBYTE i);

#undef  DevPostEscape
#define DevPostEscape _DevPostEscape

LONG APIENTRY _DevPostDeviceModes(HAB a, PDRIVDATA b, PCSZ c, PCSZ d, PCSZ e, ULONG f);

#undef  DevPostDeviceModes
#define DevPostDeviceModes _DevPostDeviceModes

BOOL APIENTRY _DevQueryDeviceNames(HAB a, PCSZ b, PLONG c, PSTR32 d, PSTR64 e, PLONG f, PSTR16 g);

#undef  DevQueryDeviceNames
#define DevQueryDeviceNames _DevQueryDeviceNames

LONG APIENTRY _DevQueryHardcopyCaps(HDC a, LONG b, LONG c, PHCINFO d);

#undef  DevQueryHardcopyCaps
#define DevQueryHardcopyCaps _DevQueryHardcopyCaps

#endif
HMF APIENTRY _DevCloseDC(HDC a);

#undef  DevCloseDC
#define DevCloseDC _DevCloseDC

HDC APIENTRY _DevOpenDC(HAB a, LONG b, PCSZ c, LONG d, PDEVOPENDATA e, HDC f);

#undef  DevOpenDC
#define DevOpenDC _DevOpenDC

BOOL APIENTRY _DevQueryCaps(HDC a, LONG b, LONG c, PLONG d);

#undef  DevQueryCaps
#define DevQueryCaps _DevQueryCaps

#endif
#ifdef INCL_WINPROGRAMLIST
HPROGRAM APIENTRY _PrfAddProgram(HINI a, PPROGDETAILS b, HPROGRAM c);

#undef  PrfAddProgram
#define PrfAddProgram _PrfAddProgram

BOOL APIENTRY _PrfChangeProgram(HINI a, HPROGRAM b, PPROGDETAILS c);

#undef  PrfChangeProgram
#define PrfChangeProgram _PrfChangeProgram

HPROGRAM APIENTRY _PrfCreateGroup(HINI a, PCSZ b, UCHAR c);

#undef  PrfCreateGroup
#define PrfCreateGroup _PrfCreateGroup

BOOL APIENTRY _PrfDestroyGroup(HINI a, HPROGRAM b);

#undef  PrfDestroyGroup
#define PrfDestroyGroup _PrfDestroyGroup

PROGCATEGORY APIENTRY _PrfQueryProgramCategory(HINI a, PCSZ b);

#undef  PrfQueryProgramCategory
#define PrfQueryProgramCategory _PrfQueryProgramCategory

ULONG APIENTRY _PrfQueryProgramHandle(HINI a, PCSZ b, PHPROGARRAY c, ULONG d, PULONG e);

#undef  PrfQueryProgramHandle
#define PrfQueryProgramHandle _PrfQueryProgramHandle

ULONG APIENTRY _PrfQueryProgramTitles(HINI a, HPROGRAM b, PPROGTITLE c, ULONG d, PULONG e);

#undef  PrfQueryProgramTitles
#define PrfQueryProgramTitles _PrfQueryProgramTitles

ULONG APIENTRY _PrfQueryDefinition(HINI a, HPROGRAM b, PPROGDETAILS c, ULONG d);

#undef  PrfQueryDefinition
#define PrfQueryDefinition _PrfQueryDefinition

BOOL APIENTRY _PrfRemoveProgram(HINI a, HPROGRAM b);

#undef  PrfRemoveProgram
#define PrfRemoveProgram _PrfRemoveProgram

HAPP APIENTRY _WinStartApp(HWND a, PPROGDETAILS b, PCSZ c, PVOID d, ULONG e);

#undef  WinStartApp
#define WinStartApp _WinStartApp

BOOL APIENTRY _WinTerminateApp(HAPP a);

#undef  WinTerminateApp
#define WinTerminateApp _WinTerminateApp

#endif
#ifdef INCL_WINSWITCHLIST
HSWITCH APIENTRY _WinAddSwitchEntry(PSWCNTRL a);

#undef  WinAddSwitchEntry
#define WinAddSwitchEntry _WinAddSwitchEntry

ULONG APIENTRY _WinRemoveSwitchEntry(HSWITCH a);

#undef  WinRemoveSwitchEntry
#define WinRemoveSwitchEntry _WinRemoveSwitchEntry

ULONG APIENTRY _WinChangeSwitchEntry(HSWITCH a, PSWCNTRL b);

#undef  WinChangeSwitchEntry
#define WinChangeSwitchEntry _WinChangeSwitchEntry

HSWITCH APIENTRY _WinCreateSwitchEntry(HAB a, PSWCNTRL b);

#undef  WinCreateSwitchEntry
#define WinCreateSwitchEntry _WinCreateSwitchEntry

ULONG APIENTRY _WinQuerySessionTitle(HAB a, ULONG b, PSZ c, ULONG d);

#undef  WinQuerySessionTitle
#define WinQuerySessionTitle _WinQuerySessionTitle

ULONG APIENTRY _WinQuerySwitchEntry(HSWITCH a, PSWCNTRL b);

#undef  WinQuerySwitchEntry
#define WinQuerySwitchEntry _WinQuerySwitchEntry

HSWITCH APIENTRY _WinQuerySwitchHandle(HWND a, PID b);

#undef  WinQuerySwitchHandle
#define WinQuerySwitchHandle _WinQuerySwitchHandle

ULONG APIENTRY _WinQuerySwitchList(HAB a, PSWBLOCK b, ULONG c);

#undef  WinQuerySwitchList
#define WinQuerySwitchList _WinQuerySwitchList

ULONG APIENTRY _WinQueryTaskSizePos(HAB a, ULONG b, PSWP c);

#undef  WinQueryTaskSizePos
#define WinQueryTaskSizePos _WinQueryTaskSizePos

ULONG APIENTRY _WinQueryTaskTitle(ULONG a, PSZ b, ULONG c);

#undef  WinQueryTaskTitle
#define WinQueryTaskTitle _WinQueryTaskTitle

ULONG APIENTRY _WinSwitchToProgram(HSWITCH a);

#undef  WinSwitchToProgram
#define WinSwitchToProgram _WinSwitchToProgram

#endif
#ifdef INCL_WINSHELLDATA
BOOL APIENTRY _PrfCloseProfile(HINI a);

#undef  PrfCloseProfile
#define PrfCloseProfile _PrfCloseProfile

HINI APIENTRY _PrfOpenProfile(HAB a, PCSZ b);

#undef  PrfOpenProfile
#define PrfOpenProfile _PrfOpenProfile

BOOL APIENTRY _PrfQueryProfile(HAB a, PPRFPROFILE b);

#undef  PrfQueryProfile
#define PrfQueryProfile _PrfQueryProfile

BOOL APIENTRY _PrfQueryProfileData(HINI a, PCSZ b, PCSZ c, PVOID d, PULONG e);

#undef  PrfQueryProfileData
#define PrfQueryProfileData _PrfQueryProfileData

LONG APIENTRY _PrfQueryProfileInt(HINI a, PCSZ b, PCSZ c, LONG d);

#undef  PrfQueryProfileInt
#define PrfQueryProfileInt _PrfQueryProfileInt

BOOL APIENTRY _PrfQueryProfileSize(HINI a, PCSZ b, PCSZ c, PULONG d);

#undef  PrfQueryProfileSize
#define PrfQueryProfileSize _PrfQueryProfileSize

ULONG APIENTRY _PrfQueryProfileString(HINI a, PCSZ b, PCSZ c, PCSZ d, PVOID e, ULONG f);

#undef  PrfQueryProfileString
#define PrfQueryProfileString _PrfQueryProfileString

BOOL APIENTRY _PrfReset(HAB a, PPRFPROFILE b);

#undef  PrfReset
#define PrfReset _PrfReset

BOOL APIENTRY _PrfWriteProfileData(HINI a, PCSZ b, PCSZ c, PVOID d, ULONG e);

#undef  PrfWriteProfileData
#define PrfWriteProfileData _PrfWriteProfileData

BOOL APIENTRY _PrfWriteProfileString(HINI a, PCSZ b, PCSZ c, PCSZ d);

#undef  PrfWriteProfileString
#define PrfWriteProfileString _PrfWriteProfileString

#endif
#ifdef INCL_WINSTDFILE
MRESULT APIENTRY _WinDefFileDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d);

#undef  WinDefFileDlgProc
#define WinDefFileDlgProc _WinDefFileDlgProc

HWND APIENTRY _WinFileDlg(HWND a, HWND b, PFILEDLG c);

#undef  WinFileDlg
#define WinFileDlg _WinFileDlg

BOOL APIENTRY _WinFreeFileDlgList(PAPSZ a);

#undef  WinFreeFileDlgList
#define WinFreeFileDlgList _WinFreeFileDlgList

#endif
#ifdef INCL_WINSTDFONT
HWND APIENTRY _WinFontDlg(HWND a, HWND b, PFONTDLG c);

#undef  WinFontDlg
#define WinFontDlg _WinFontDlg

MRESULT APIENTRY _WinDefFontDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d);

#undef  WinDefFontDlgProc
#define WinDefFontDlgProc _WinDefFontDlgProc

#endif
#ifdef INCL_WINSTDDRAG
BOOL APIENTRY _DrgAcceptDroppedFiles(HWND a, PCSZ b, PCSZ c, ULONG d, ULONG e);

#undef  DrgAcceptDroppedFiles
#define DrgAcceptDroppedFiles _DrgAcceptDroppedFiles

BOOL APIENTRY _DrgAccessDraginfo(PDRAGINFO a);

#undef  DrgAccessDraginfo
#define DrgAccessDraginfo _DrgAccessDraginfo

HSTR APIENTRY _DrgAddStrHandle(PCSZ a);

#undef  DrgAddStrHandle
#define DrgAddStrHandle _DrgAddStrHandle

PDRAGINFO APIENTRY _DrgAllocDraginfo(ULONG a);

#undef  DrgAllocDraginfo
#define DrgAllocDraginfo _DrgAllocDraginfo

PDRAGTRANSFER APIENTRY _DrgAllocDragtransfer(ULONG a);

#undef  DrgAllocDragtransfer
#define DrgAllocDragtransfer _DrgAllocDragtransfer

BOOL APIENTRY _DrgCancelLazyDrag();

#undef  DrgCancelLazyDrag
#define DrgCancelLazyDrag _DrgCancelLazyDrag

BOOL APIENTRY _DrgDeleteDraginfoStrHandles(PDRAGINFO a);

#undef  DrgDeleteDraginfoStrHandles
#define DrgDeleteDraginfoStrHandles _DrgDeleteDraginfoStrHandles

BOOL APIENTRY _DrgDeleteStrHandle(HSTR a);

#undef  DrgDeleteStrHandle
#define DrgDeleteStrHandle _DrgDeleteStrHandle

HWND APIENTRY _DrgDrag(HWND a, PDRAGINFO b, PDRAGIMAGE c, ULONG d, LONG e, PVOID f);

#undef  DrgDrag
#define DrgDrag _DrgDrag

BOOL APIENTRY _DrgDragFiles(HWND a, PCSZ *b, PCSZ *c, PCSZ *d, ULONG e, HPOINTER f, ULONG g, BOOL h, ULONG i);

#undef  DrgDragFiles
#define DrgDragFiles _DrgDragFiles

BOOL APIENTRY _DrgFreeDraginfo(PDRAGINFO a);

#undef  DrgFreeDraginfo
#define DrgFreeDraginfo _DrgFreeDraginfo

BOOL APIENTRY _DrgFreeDragtransfer(PDRAGTRANSFER a);

#undef  DrgFreeDragtransfer
#define DrgFreeDragtransfer _DrgFreeDragtransfer

HPS APIENTRY _DrgGetPS(HWND a);

#undef  DrgGetPS
#define DrgGetPS _DrgGetPS

BOOL APIENTRY _DrgLazyDrag(HWND a, PDRAGINFO b, PDRAGIMAGE c, ULONG d, PVOID e);

#undef  DrgLazyDrag
#define DrgLazyDrag _DrgLazyDrag

BOOL APIENTRY _DrgLazyDrop(HWND a, ULONG b, PPOINTL c);

#undef  DrgLazyDrop
#define DrgLazyDrop _DrgLazyDrop

BOOL APIENTRY _DrgPostTransferMsg(HWND a, ULONG b, PDRAGTRANSFER c, ULONG d, ULONG e, BOOL f);

#undef  DrgPostTransferMsg
#define DrgPostTransferMsg _DrgPostTransferMsg

BOOL APIENTRY _DrgPushDraginfo(PDRAGINFO a, HWND b);

#undef  DrgPushDraginfo
#define DrgPushDraginfo _DrgPushDraginfo

PDRAGINFO APIENTRY _DrgQueryDraginfoPtr(PDRAGINFO a);

#undef  DrgQueryDraginfoPtr
#define DrgQueryDraginfoPtr _DrgQueryDraginfoPtr

PDRAGINFO APIENTRY _DrgQueryDraginfoPtrFromHwnd(HWND a);

#undef  DrgQueryDraginfoPtrFromHwnd
#define DrgQueryDraginfoPtrFromHwnd _DrgQueryDraginfoPtrFromHwnd

PDRAGINFO APIENTRY _DrgQueryDraginfoPtrFromDragitem(PDRAGITEM a);

#undef  DrgQueryDraginfoPtrFromDragitem
#define DrgQueryDraginfoPtrFromDragitem _DrgQueryDraginfoPtrFromDragitem

BOOL APIENTRY _DrgQueryDragitem(PDRAGINFO a, ULONG b, PDRAGITEM c, ULONG d);

#undef  DrgQueryDragitem
#define DrgQueryDragitem _DrgQueryDragitem

ULONG APIENTRY _DrgQueryDragitemCount(PDRAGINFO a);

#undef  DrgQueryDragitemCount
#define DrgQueryDragitemCount _DrgQueryDragitemCount

PDRAGITEM APIENTRY _DrgQueryDragitemPtr(PDRAGINFO a, ULONG b);

#undef  DrgQueryDragitemPtr
#define DrgQueryDragitemPtr _DrgQueryDragitemPtr

ULONG APIENTRY _DrgQueryDragStatus();

#undef  DrgQueryDragStatus
#define DrgQueryDragStatus _DrgQueryDragStatus

BOOL APIENTRY _DrgQueryNativeRMF(PDRAGITEM a, ULONG b, PCHAR c);

#undef  DrgQueryNativeRMF
#define DrgQueryNativeRMF _DrgQueryNativeRMF

ULONG APIENTRY _DrgQueryNativeRMFLen(PDRAGITEM a);

#undef  DrgQueryNativeRMFLen
#define DrgQueryNativeRMFLen _DrgQueryNativeRMFLen

ULONG APIENTRY _DrgQueryStrName(HSTR a, ULONG b, PSZ c);

#undef  DrgQueryStrName
#define DrgQueryStrName _DrgQueryStrName

ULONG APIENTRY _DrgQueryStrNameLen(HSTR a);

#undef  DrgQueryStrNameLen
#define DrgQueryStrNameLen _DrgQueryStrNameLen

BOOL APIENTRY _DrgQueryTrueType(PDRAGITEM a, ULONG b, PSZ c);

#undef  DrgQueryTrueType
#define DrgQueryTrueType _DrgQueryTrueType

ULONG APIENTRY _DrgQueryTrueTypeLen(PDRAGITEM a);

#undef  DrgQueryTrueTypeLen
#define DrgQueryTrueTypeLen _DrgQueryTrueTypeLen

PDRAGINFO APIENTRY _DrgReallocDraginfo(PDRAGINFO a, ULONG b);

#undef  DrgReallocDraginfo
#define DrgReallocDraginfo _DrgReallocDraginfo

BOOL APIENTRY _DrgReleasePS(HPS a);

#undef  DrgReleasePS
#define DrgReleasePS _DrgReleasePS

MRESULT APIENTRY _DrgSendTransferMsg(HWND a, ULONG b, MPARAM c, MPARAM d);

#undef  DrgSendTransferMsg
#define DrgSendTransferMsg _DrgSendTransferMsg

BOOL APIENTRY _DrgSetDragImage(PDRAGINFO a, PDRAGIMAGE b, ULONG c, PVOID d);

#undef  DrgSetDragImage
#define DrgSetDragImage _DrgSetDragImage

BOOL APIENTRY _DrgSetDragitem(PDRAGINFO a, PDRAGITEM b, ULONG c, ULONG d);

#undef  DrgSetDragitem
#define DrgSetDragitem _DrgSetDragitem

BOOL APIENTRY _DrgSetDragPointer(PDRAGINFO a, HPOINTER b);

#undef  DrgSetDragPointer
#define DrgSetDragPointer _DrgSetDragPointer

BOOL APIENTRY _DrgVerifyNativeRMF(PDRAGITEM a, PCSZ b);

#undef  DrgVerifyNativeRMF
#define DrgVerifyNativeRMF _DrgVerifyNativeRMF

BOOL APIENTRY _DrgVerifyRMF(PDRAGITEM a, PCSZ b, PCSZ c);

#undef  DrgVerifyRMF
#define DrgVerifyRMF _DrgVerifyRMF

BOOL APIENTRY _DrgVerifyTrueType(PDRAGITEM a, PCSZ b);

#undef  DrgVerifyTrueType
#define DrgVerifyTrueType _DrgVerifyTrueType

BOOL APIENTRY _DrgVerifyType(PDRAGITEM a, PCSZ b);

#undef  DrgVerifyType
#define DrgVerifyType _DrgVerifyType

BOOL APIENTRY _DrgVerifyTypeSet(PDRAGITEM a, PCSZ b, ULONG c, PSZ d);

#undef  DrgVerifyTypeSet
#define DrgVerifyTypeSet _DrgVerifyTypeSet

#endif
#ifdef INCL_WPCLASS
HOBJECT APIENTRY _WinCopyObject(HOBJECT a, HOBJECT b, ULONG c);

#undef  WinCopyObject
#define WinCopyObject _WinCopyObject

HOBJECT APIENTRY _WinCreateObject(PCSZ a, PCSZ b, PCSZ c, PCSZ d, ULONG e);

#undef  WinCreateObject
#define WinCreateObject _WinCreateObject

HOBJECT APIENTRY _WinCreateShadow(HOBJECT a, HOBJECT b, ULONG c);

#undef  WinCreateShadow
#define WinCreateShadow _WinCreateShadow

BOOL APIENTRY _WinDeregisterObjectClass(PCSZ a);

#undef  WinDeregisterObjectClass
#define WinDeregisterObjectClass _WinDeregisterObjectClass

BOOL APIENTRY _WinDestroyObject(HOBJECT a);

#undef  WinDestroyObject
#define WinDestroyObject _WinDestroyObject

BOOL APIENTRY _WinEnumObjectClasses(POBJCLASS a, PULONG b);

#undef  WinEnumObjectClasses
#define WinEnumObjectClasses _WinEnumObjectClasses

BOOL APIENTRY _WinIsSOMDDReady();

#undef  WinIsSOMDDReady
#define WinIsSOMDDReady _WinIsSOMDDReady

BOOL APIENTRY _WinIsWPDServerReady();

#undef  WinIsWPDServerReady
#define WinIsWPDServerReady _WinIsWPDServerReady

HOBJECT APIENTRY _WinMoveObject(HOBJECT a, HOBJECT b, ULONG c);

#undef  WinMoveObject
#define WinMoveObject _WinMoveObject

BOOL APIENTRY _WinOpenObject(HOBJECT a, ULONG b, BOOL c);

#undef  WinOpenObject
#define WinOpenObject _WinOpenObject

BOOL APIENTRY _WinQueryActiveDesktopPathname(PSZ a, ULONG b);

#undef  WinQueryActiveDesktopPathname
#define WinQueryActiveDesktopPathname _WinQueryActiveDesktopPathname

HOBJECT APIENTRY _WinQueryObject(PCSZ a);

#undef  WinQueryObject
#define WinQueryObject _WinQueryObject

BOOL APIENTRY _WinQueryObjectPath(HOBJECT a, PSZ b, ULONG c);

#undef  WinQueryObjectPath
#define WinQueryObjectPath _WinQueryObjectPath

BOOL APIENTRY _WinRegisterObjectClass(PCSZ a, PCSZ b);

#undef  WinRegisterObjectClass
#define WinRegisterObjectClass _WinRegisterObjectClass

BOOL APIENTRY _WinReplaceObjectClass(PCSZ a, PCSZ b, BOOL c);

#undef  WinReplaceObjectClass
#define WinReplaceObjectClass _WinReplaceObjectClass

ULONG APIENTRY _WinRestartSOMDD(BOOL a);

#undef  WinRestartSOMDD
#define WinRestartSOMDD _WinRestartSOMDD

ULONG APIENTRY _WinRestartWPDServer(BOOL a);

#undef  WinRestartWPDServer
#define WinRestartWPDServer _WinRestartWPDServer

BOOL APIENTRY _WinSaveObject(HOBJECT a, BOOL b);

#undef  WinSaveObject
#define WinSaveObject _WinSaveObject

BOOL APIENTRY _WinSetObjectData(HOBJECT a, PCSZ b);

#undef  WinSetObjectData
#define WinSetObjectData _WinSetObjectData

BOOL APIENTRY _WinFreeFileIcon(HPOINTER a);

#undef  WinFreeFileIcon
#define WinFreeFileIcon _WinFreeFileIcon

HPOINTER APIENTRY _WinLoadFileIcon(PCSZ a, BOOL b);

#undef  WinLoadFileIcon
#define WinLoadFileIcon _WinLoadFileIcon

BOOL APIENTRY _WinRestoreWindowPos(PCSZ a, PCSZ b, HWND c);

#undef  WinRestoreWindowPos
#define WinRestoreWindowPos _WinRestoreWindowPos

BOOL APIENTRY _WinSetFileIcon(PCSZ a, PICONINFO b);

#undef  WinSetFileIcon
#define WinSetFileIcon _WinSetFileIcon

BOOL APIENTRY _WinShutdownSystem(HAB a, HMQ b);

#undef  WinShutdownSystem
#define WinShutdownSystem _WinShutdownSystem

BOOL APIENTRY _WinStoreWindowPos(PCSZ a, PCSZ b, HWND c);

#undef  WinStoreWindowPos
#define WinStoreWindowPos _WinStoreWindowPos

#endif
#ifdef INCL_SPL
BOOL APIENTRY _SplStdClose(HDC a);

#undef  SplStdClose
#define SplStdClose _SplStdClose

BOOL APIENTRY _SplStdDelete(HSTD a);

#undef  SplStdDelete
#define SplStdDelete _SplStdDelete

BOOL APIENTRY _SplStdGetBits(HSTD a, LONG b, LONG c, PCH d);

#undef  SplStdGetBits
#define SplStdGetBits _SplStdGetBits

BOOL APIENTRY _SplStdOpen(HDC a);

#undef  SplStdOpen
#define SplStdOpen _SplStdOpen

LONG APIENTRY _SplStdQueryLength(HSTD a);

#undef  SplStdQueryLength
#define SplStdQueryLength _SplStdQueryLength

BOOL APIENTRY _SplStdStart(HDC a);

#undef  SplStdStart
#define SplStdStart _SplStdStart

HSTD APIENTRY _SplStdStop(HDC a);

#undef  SplStdStop
#define SplStdStop _SplStdStop

SPLERR APIENTRY _SplControlDevice(PSZ a, PSZ b, ULONG c);

#undef  SplControlDevice
#define SplControlDevice _SplControlDevice

SPLERR APIENTRY _SplCopyJob(PCSZ a, PCSZ b, ULONG c, PCSZ d, PCSZ e, PULONG f);

#undef  SplCopyJob
#define SplCopyJob _SplCopyJob

SPLERR APIENTRY _SplCreateDevice(PSZ a, ULONG b, PVOID c, ULONG d);

#undef  SplCreateDevice
#define SplCreateDevice _SplCreateDevice

SPLERR APIENTRY _SplCreatePort(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f);

#undef  SplCreatePort
#define SplCreatePort _SplCreatePort

SPLERR APIENTRY _SplCreateQueue(PSZ a, ULONG b, PVOID c, ULONG d);

#undef  SplCreateQueue
#define SplCreateQueue _SplCreateQueue

SPLERR APIENTRY _SplDeleteDevice(PSZ a, PSZ b);

#undef  SplDeleteDevice
#define SplDeleteDevice _SplDeleteDevice

SPLERR APIENTRY _SplDeleteJob(PSZ a, PSZ b, ULONG c);

#undef  SplDeleteJob
#define SplDeleteJob _SplDeleteJob

SPLERR APIENTRY _SplDeletePort(PCSZ a, PCSZ b);

#undef  SplDeletePort
#define SplDeletePort _SplDeletePort

SPLERR APIENTRY _SplDeleteQueue(PSZ a, PSZ b);

#undef  SplDeleteQueue
#define SplDeleteQueue _SplDeleteQueue

SPLERR APIENTRY _SplEnumDevice(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h);

#undef  SplEnumDevice
#define SplEnumDevice _SplEnumDevice

SPLERR APIENTRY _SplEnumDriver(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h);

#undef  SplEnumDriver
#define SplEnumDriver _SplEnumDriver

SPLERR APIENTRY _SplEnumJob(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f, PULONG g, PULONG h, PVOID i);

#undef  SplEnumJob
#define SplEnumJob _SplEnumJob

SPLERR APIENTRY _SplEnumPort(PCSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h);

#undef  SplEnumPort
#define SplEnumPort _SplEnumPort

SPLERR APIENTRY _SplEnumPrinter(PSZ a, ULONG b, ULONG c, PVOID d, ULONG e, PULONG f, PULONG g, PULONG h, PVOID i);

#undef  SplEnumPrinter
#define SplEnumPrinter _SplEnumPrinter

SPLERR APIENTRY _SplEnumQueue(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h);

#undef  SplEnumQueue
#define SplEnumQueue _SplEnumQueue

SPLERR APIENTRY _SplEnumQueueProcessor(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h);

#undef  SplEnumQueueProcessor
#define SplEnumQueueProcessor _SplEnumQueueProcessor

SPLERR APIENTRY _SplHoldJob(PCSZ a, PCSZ b, ULONG c);

#undef  SplHoldJob
#define SplHoldJob _SplHoldJob

SPLERR APIENTRY _SplHoldQueue(PSZ a, PSZ b);

#undef  SplHoldQueue
#define SplHoldQueue _SplHoldQueue

SPLERR APIENTRY _SplPurgeQueue(PSZ a, PSZ b);

#undef  SplPurgeQueue
#define SplPurgeQueue _SplPurgeQueue

SPLERR APIENTRY _SplQueryDevice(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f);

#undef  SplQueryDevice
#define SplQueryDevice _SplQueryDevice

SPLERR APIENTRY _SplQueryDriver(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f, PULONG g);

#undef  SplQueryDriver
#define SplQueryDriver _SplQueryDriver

SPLERR APIENTRY _SplQueryJob(PSZ a, PSZ b, ULONG c, ULONG d, PVOID e, ULONG f, PULONG g);

#undef  SplQueryJob
#define SplQueryJob _SplQueryJob

SPLERR APIENTRY _SplQueryPort(PCSZ a, PCSZ b, ULONG c, PVOID d, ULONG e, PULONG f);

#undef  SplQueryPort
#define SplQueryPort _SplQueryPort

SPLERR APIENTRY _SplQueryQueue(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f);

#undef  SplQueryQueue
#define SplQueryQueue _SplQueryQueue

SPLERR APIENTRY _SplReleaseJob(PCSZ a, PCSZ b, ULONG c);

#undef  SplReleaseJob
#define SplReleaseJob _SplReleaseJob

SPLERR APIENTRY _SplReleaseQueue(PSZ a, PSZ b);

#undef  SplReleaseQueue
#define SplReleaseQueue _SplReleaseQueue

SPLERR APIENTRY _SplSetDevice(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, ULONG f);

#undef  SplSetDevice
#define SplSetDevice _SplSetDevice

SPLERR APIENTRY _SplSetDriver(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f, ULONG g);

#undef  SplSetDriver
#define SplSetDriver _SplSetDriver

SPLERR APIENTRY _SplSetJob(PSZ a, PSZ b, ULONG c, ULONG d, PVOID e, ULONG f, ULONG g);

#undef  SplSetJob
#define SplSetJob _SplSetJob

SPLERR APIENTRY _SplSetPort(PCSZ a, PCSZ b, ULONG c, PVOID d, ULONG e, ULONG f);

#undef  SplSetPort
#define SplSetPort _SplSetPort

SPLERR APIENTRY _SplSetQueue(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, ULONG f);

#undef  SplSetQueue
#define SplSetQueue _SplSetQueue

ULONG APIENTRY _SplMessageBox(PSZ a, ULONG b, ULONG c, PSZ d, PSZ e, ULONG f, ULONG g);

#undef  SplMessageBox
#define SplMessageBox _SplMessageBox

BOOL APIENTRY _SplQmAbort(HSPL a);

#undef  SplQmAbort
#define SplQmAbort _SplQmAbort

BOOL APIENTRY _SplQmAbortDoc(HSPL a);

#undef  SplQmAbortDoc
#define SplQmAbortDoc _SplQmAbortDoc

BOOL APIENTRY _SplQmClose(HSPL a);

#undef  SplQmClose
#define SplQmClose _SplQmClose

BOOL APIENTRY _SplQmEndDoc(HSPL a);

#undef  SplQmEndDoc
#define SplQmEndDoc _SplQmEndDoc

ULONG APIENTRY _SplQmGetJobID(HSPL a, ULONG b, PVOID c, ULONG d, PULONG e);

#undef  SplQmGetJobID
#define SplQmGetJobID _SplQmGetJobID

BOOL APIENTRY _SplQmNewPage(HSPL a, ULONG b);

#undef  SplQmNewPage
#define SplQmNewPage _SplQmNewPage

HSPL APIENTRY _SplQmOpen(PSZ a, LONG b, PQMOPENDATA c);

#undef  SplQmOpen
#define SplQmOpen _SplQmOpen

BOOL APIENTRY _SplQmStartDoc(HSPL a, PSZ b);

#undef  SplQmStartDoc
#define SplQmStartDoc _SplQmStartDoc

BOOL APIENTRY _SplQmWrite(HSPL a, LONG b, PVOID c);

#undef  SplQmWrite
#define SplQmWrite _SplQmWrite

#endif
#ifdef INCL_WINHELP
BOOL APIENTRY _WinAssociateHelpInstance(HWND a, HWND b);

#undef  WinAssociateHelpInstance
#define WinAssociateHelpInstance _WinAssociateHelpInstance

HWND APIENTRY _WinCreateHelpInstance(HAB a, PHELPINIT b);

#undef  WinCreateHelpInstance
#define WinCreateHelpInstance _WinCreateHelpInstance

BOOL APIENTRY _WinCreateHelpTable(HWND a, PHELPTABLE b);

#undef  WinCreateHelpTable
#define WinCreateHelpTable _WinCreateHelpTable

BOOL APIENTRY _WinDestroyHelpInstance(HWND a);

#undef  WinDestroyHelpInstance
#define WinDestroyHelpInstance _WinDestroyHelpInstance

BOOL APIENTRY _WinLoadHelpTable(HWND a, ULONG b, HMODULE c);

#undef  WinLoadHelpTable
#define WinLoadHelpTable _WinLoadHelpTable

HWND APIENTRY _WinQueryHelpInstance(HWND a);

#undef  WinQueryHelpInstance
#define WinQueryHelpInstance _WinQueryHelpInstance

#endif
#ifdef INCL_DDF
BOOL APIENTRY _DdfBeginList(HDDF a, ULONG b, ULONG c, ULONG d);

#undef  DdfBeginList
#define DdfBeginList _DdfBeginList

BOOL APIENTRY _DdfBitmap(HDDF a, HBITMAP b, ULONG c);

#undef  DdfBitmap
#define DdfBitmap _DdfBitmap

BOOL APIENTRY _DdfEndList(HDDF a);

#undef  DdfEndList
#define DdfEndList _DdfEndList

BOOL APIENTRY _DdfHyperText(HDDF a, PCSZ b, PCSZ c, ULONG d);

#undef  DdfHyperText
#define DdfHyperText _DdfHyperText

BOOL APIENTRY _DdfInform(HDDF a, PCSZ b, ULONG c);

#undef  DdfInform
#define DdfInform _DdfInform

HDDF APIENTRY _DdfInitialize(HWND a, ULONG b, ULONG c);

#undef  DdfInitialize
#define DdfInitialize _DdfInitialize

BOOL APIENTRY _DdfListItem(HDDF a, PCSZ b, PCSZ c);

#undef  DdfListItem
#define DdfListItem _DdfListItem

BOOL APIENTRY _DdfMetafile(HDDF a, HMF b, PRECTL c);

#undef  DdfMetafile
#define DdfMetafile _DdfMetafile

BOOL APIENTRY _DdfPara(HDDF a);

#undef  DdfPara
#define DdfPara _DdfPara

BOOL APIENTRY _DdfSetColor(HDDF a, COLOR b, COLOR c);

#undef  DdfSetColor
#define DdfSetColor _DdfSetColor

BOOL APIENTRY _DdfSetFont(HDDF a, PCSZ b, ULONG c, ULONG d);

#undef  DdfSetFont
#define DdfSetFont _DdfSetFont

BOOL APIENTRY _DdfSetFontStyle(HDDF a, ULONG b);

#undef  DdfSetFontStyle
#define DdfSetFontStyle _DdfSetFontStyle

BOOL APIENTRY _DdfSetFormat(HDDF a, ULONG b);

#undef  DdfSetFormat
#define DdfSetFormat _DdfSetFormat

BOOL APIENTRY _DdfSetTextAlign(HDDF a, ULONG b);

#undef  DdfSetTextAlign
#define DdfSetTextAlign _DdfSetTextAlign

BOOL APIENTRY _DdfText(HDDF a, PCSZ b);

#undef  DdfText
#define DdfText _DdfText

#endif

#ifdef INCL_AVIO
inline USHORT _VioAssociate(HDC a, HVPS b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioAssociate(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioAssociate
#define VioAssociate _VioAssociate

inline USHORT _VioCreateLogFont(PFATTRS a, LONG b, PSTR8 c, HVPS d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioCreateLogFont(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  VioCreateLogFont
#define VioCreateLogFont _VioCreateLogFont

inline USHORT _VioCreatePS(PHVPS a, SHORT b, SHORT c, SHORT d, SHORT e, HVPS f)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioCreatePS(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
}

#undef  VioCreatePS
#define VioCreatePS _VioCreatePS

inline USHORT _VioDeleteSetId(LONG a, HVPS b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioDeleteSetId(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioDeleteSetId
#define VioDeleteSetId _VioDeleteSetId

inline USHORT _VioDestroyPS(HVPS a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioDestroyPS(a);
    SetFS(sel);

    return yyrc;
}

#undef  VioDestroyPS
#define VioDestroyPS _VioDestroyPS

inline USHORT _VioGetDeviceCellSize(PSHORT a, PSHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetDeviceCellSize(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetDeviceCellSize
#define VioGetDeviceCellSize _VioGetDeviceCellSize

inline USHORT _VioGetOrg(PSHORT a, PSHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetOrg(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetOrg
#define VioGetOrg _VioGetOrg

inline USHORT _VioQueryFonts(PLONG a, PFONTMETRICS b, LONG c, PLONG d, PSZ e, ULONG f, HVPS g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioQueryFonts(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
}

#undef  VioQueryFonts
#define VioQueryFonts _VioQueryFonts

inline USHORT _VioQuerySetIds(PLONG a, PSTR8 b, PLONG c, LONG d, HVPS e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioQuerySetIds(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioQuerySetIds
#define VioQuerySetIds _VioQuerySetIds

inline USHORT _VioSetDeviceCellSize(SHORT a, SHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetDeviceCellSize(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetDeviceCellSize
#define VioSetDeviceCellSize _VioSetDeviceCellSize

inline USHORT _VioSetOrg(SHORT a, SHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetOrg(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetOrg
#define VioSetOrg _VioSetOrg

inline USHORT _VioShowPS(SHORT a, SHORT b, SHORT c, HVPS d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioShowPS(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  VioShowPS
#define VioShowPS _VioShowPS

inline MRESULT _WinDefAVioWindowProc(HWND a, USHORT b, ULONG c, ULONG d)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDefAVioWindowProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  WinDefAVioWindowProc
#define WinDefAVioWindowProc _WinDefAVioWindowProc

#endif
#ifdef INCL_KBD
inline USHORT _KbdCharIn(PKBDKEYINFO a, USHORT b, HKBD c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdCharIn(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  KbdCharIn
#define KbdCharIn _KbdCharIn

inline USHORT _KbdClose(HKBD a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdClose(a);
    SetFS(sel);

    return yyrc;
}

#undef  KbdClose
#define KbdClose _KbdClose

inline USHORT _KbdDeRegister()
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdDeRegister();
    SetFS(sel);

    return yyrc;
}

#undef  KbdDeRegister
#define KbdDeRegister _KbdDeRegister

inline USHORT _KbdFlushBuffer(HKBD a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdFlushBuffer(a);
    SetFS(sel);

    return yyrc;
}

#undef  KbdFlushBuffer
#define KbdFlushBuffer _KbdFlushBuffer

inline USHORT _KbdFreeFocus(HKBD a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdFreeFocus(a);
    SetFS(sel);

    return yyrc;
}

#undef  KbdFreeFocus
#define KbdFreeFocus _KbdFreeFocus

inline USHORT _KbdGetCp(ULONG a, PUSHORT b, HKBD c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdGetCp(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  KbdGetCp
#define KbdGetCp _KbdGetCp

inline USHORT _KbdGetFocus(USHORT a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdGetFocus(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdGetFocus
#define KbdGetFocus _KbdGetFocus

inline USHORT _KbdGetHWID(PKBDHWID a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdGetHWID(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdGetHWID
#define KbdGetHWID _KbdGetHWID

inline USHORT _KbdGetStatus(PKBDINFO a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdGetStatus(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdGetStatus
#define KbdGetStatus _KbdGetStatus

inline USHORT _KbdOpen(PHKBD a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdOpen(a);
    SetFS(sel);

    return yyrc;
}

#undef  KbdOpen
#define KbdOpen _KbdOpen

inline USHORT _KbdPeek(PKBDKEYINFO a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdPeek(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdPeek
#define KbdPeek _KbdPeek

inline USHORT _KbdRegister(PSZ a, PSZ b, ULONG c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdRegister(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  KbdRegister
#define KbdRegister _KbdRegister

inline USHORT _KbdSetCp(USHORT a, USHORT b, HKBD c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdSetCp(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  KbdSetCp
#define KbdSetCp _KbdSetCp

inline USHORT _KbdSetCustXt(PUSHORT a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdSetCustXt(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdSetCustXt
#define KbdSetCustXt _KbdSetCustXt

inline USHORT _KbdSetFgnd()
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdSetFgnd();
    SetFS(sel);

    return yyrc;
}

#undef  KbdSetFgnd
#define KbdSetFgnd _KbdSetFgnd

inline USHORT _KbdSetHWID(PKBDHWID a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdSetHWID(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdSetHWID
#define KbdSetHWID _KbdSetHWID

inline USHORT _KbdSetStatus(PKBDINFO a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdSetStatus(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdSetStatus
#define KbdSetStatus _KbdSetStatus

inline USHORT _KbdStringIn(PCH a, PSTRINGINBUF b, USHORT c, HKBD d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdStringIn(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  KbdStringIn
#define KbdStringIn _KbdStringIn

inline USHORT _KbdSynch(USHORT a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdSynch(a);
    SetFS(sel);

    return yyrc;
}

#undef  KbdSynch
#define KbdSynch _KbdSynch

inline USHORT _KbdXlate(PKBDTRANS a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = KbdXlate(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  KbdXlate
#define KbdXlate _KbdXlate

#endif
#ifdef INCL_VIO
inline USHORT _VioCheckCharType(PUSHORT a, USHORT b, USHORT c, HVIO d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioCheckCharType(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  VioCheckCharType
#define VioCheckCharType _VioCheckCharType

inline USHORT _VioDeRegister()
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioDeRegister();
    SetFS(sel);

    return yyrc;
}

#undef  VioDeRegister
#define VioDeRegister _VioDeRegister

inline USHORT _VioEndPopUp(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioEndPopUp(a);
    SetFS(sel);

    return yyrc;
}

#undef  VioEndPopUp
#define VioEndPopUp _VioEndPopUp

inline USHORT _VioGetAnsi(PUSHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetAnsi(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetAnsi
#define VioGetAnsi _VioGetAnsi

inline USHORT _VioGetBuf(PULONG a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetBuf(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetBuf
#define VioGetBuf _VioGetBuf

inline USHORT _VioGetConfig(USHORT a, PVIOCONFIGINFO b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetConfig(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetConfig
#define VioGetConfig _VioGetConfig

inline USHORT _VioGetCp(USHORT a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetCp(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetCp
#define VioGetCp _VioGetCp

inline USHORT _VioGetCurPos(PUSHORT a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetCurPos(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetCurPos
#define VioGetCurPos _VioGetCurPos

inline USHORT _VioGetCurType(PVIOCURSORINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetCurType(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetCurType
#define VioGetCurType _VioGetCurType

inline USHORT _VioGetFont(PVIOFONTINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetFont(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetFont
#define VioGetFont _VioGetFont

inline USHORT _VioGetMode(PVIOMODEINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetMode(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetMode
#define VioGetMode _VioGetMode

inline USHORT _VioGetPhysBuf(PVIOPHYSBUF a, USHORT b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetPhysBuf(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetPhysBuf
#define VioGetPhysBuf _VioGetPhysBuf

inline USHORT _VioGetState(PVOID a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioGetState(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioGetState
#define VioGetState _VioGetState

inline USHORT _VioModeUndo(USHORT a, USHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioModeUndo(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioModeUndo
#define VioModeUndo _VioModeUndo

inline USHORT _VioModeWait(USHORT a, PUSHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioModeWait(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioModeWait
#define VioModeWait _VioModeWait

inline USHORT _VioPopUp(PUSHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioPopUp(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioPopUp
#define VioPopUp _VioPopUp

inline USHORT _VioPrtSc(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioPrtSc(a);
    SetFS(sel);

    return yyrc;
}

#undef  VioPrtSc
#define VioPrtSc _VioPrtSc

inline USHORT _VioPrtScToggle(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioPrtScToggle(a);
    SetFS(sel);

    return yyrc;
}

#undef  VioPrtScToggle
#define VioPrtScToggle _VioPrtScToggle

inline USHORT _VioReadCellStr(PCH a, PUSHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioReadCellStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioReadCellStr
#define VioReadCellStr _VioReadCellStr

inline USHORT _VioReadCharStr(PCH a, PUSHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioReadCharStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioReadCharStr
#define VioReadCharStr _VioReadCharStr

inline USHORT _VioRegister(PSZ a, PSZ b, ULONG c, ULONG d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioRegister(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  VioRegister
#define VioRegister _VioRegister

inline USHORT _VioSavRedrawUndo(USHORT a, USHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSavRedrawUndo(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioSavRedrawUndo
#define VioSavRedrawUndo _VioSavRedrawUndo

inline USHORT _VioSavRedrawWait(USHORT a, PUSHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSavRedrawWait(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioSavRedrawWait
#define VioSavRedrawWait _VioSavRedrawWait

inline USHORT _VioScrLock(USHORT a, PUCHAR b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioScrLock(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioScrLock
#define VioScrLock _VioScrLock

inline USHORT _VioScrollDn(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioScrollDn(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
}

#undef  VioScrollDn
#define VioScrollDn _VioScrollDn

inline USHORT _VioScrollLf(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioScrollLf(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
}

#undef  VioScrollLf
#define VioScrollLf _VioScrollLf

inline USHORT _VioScrollRt(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioScrollRt(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
}

#undef  VioScrollRt
#define VioScrollRt _VioScrollRt

inline USHORT _VioScrollUp(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioScrollUp(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
}

#undef  VioScrollUp
#define VioScrollUp _VioScrollUp

inline USHORT _VioScrUnLock(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioScrUnLock(a);
    SetFS(sel);

    return yyrc;
}

#undef  VioScrUnLock
#define VioScrUnLock _VioScrUnLock

inline USHORT _VioSetAnsi(USHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetAnsi(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetAnsi
#define VioSetAnsi _VioSetAnsi

inline USHORT _VioSetCp(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetCp(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetCp
#define VioSetCp _VioSetCp

inline USHORT _VioSetCurPos(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetCurPos(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetCurPos
#define VioSetCurPos _VioSetCurPos

inline USHORT _VioSetCurType(PVIOCURSORINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetCurType(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetCurType
#define VioSetCurType _VioSetCurType

inline USHORT _VioSetFont(PVIOFONTINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetFont(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetFont
#define VioSetFont _VioSetFont

inline USHORT _VioSetMode(PVIOMODEINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetMode(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetMode
#define VioSetMode _VioSetMode

inline USHORT _VioSetState(PVOID a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioSetState(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  VioSetState
#define VioSetState _VioSetState

inline USHORT _VioShowBuf(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioShowBuf(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioShowBuf
#define VioShowBuf _VioShowBuf

inline USHORT _VioWrtCellStr(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioWrtCellStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioWrtCellStr
#define VioWrtCellStr _VioWrtCellStr

inline USHORT _VioWrtCharStr(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioWrtCharStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioWrtCharStr
#define VioWrtCharStr _VioWrtCharStr

inline USHORT _VioWrtCharStrAtt(PCH a, USHORT b, USHORT c, USHORT d, PBYTE e, HVIO f)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioWrtCharStrAtt(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
}

#undef  VioWrtCharStrAtt
#define VioWrtCharStrAtt _VioWrtCharStrAtt

inline USHORT _VioWrtNAttr(PBYTE a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioWrtNAttr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioWrtNAttr
#define VioWrtNAttr _VioWrtNAttr

inline USHORT _VioWrtNCell(PBYTE a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioWrtNCell(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioWrtNCell
#define VioWrtNCell _VioWrtNCell

inline USHORT _VioWrtNChar(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioWrtNChar(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  VioWrtNChar
#define VioWrtNChar _VioWrtNChar

inline USHORT _VioWrtTTY(PCH a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VioWrtTTY(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  VioWrtTTY
#define VioWrtTTY _VioWrtTTY

#endif
#ifdef INCL_MOU
inline USHORT _MouClose(HMOU a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouClose(a);
    SetFS(sel);

    return yyrc;
}

#undef  MouClose
#define MouClose _MouClose

inline USHORT _MouDeRegister()
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouDeRegister();
    SetFS(sel);

    return yyrc;
}

#undef  MouDeRegister
#define MouDeRegister _MouDeRegister

inline USHORT _MouDrawPtr(HMOU a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouDrawPtr(a);
    SetFS(sel);

    return yyrc;
}

#undef  MouDrawPtr
#define MouDrawPtr _MouDrawPtr

inline USHORT _MouFlushQue(HMOU a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouFlushQue(a);
    SetFS(sel);

    return yyrc;
}

#undef  MouFlushQue
#define MouFlushQue _MouFlushQue

inline USHORT _MouGetDevStatus(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetDevStatus(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetDevStatus
#define MouGetDevStatus _MouGetDevStatus

inline USHORT _MouGetEventMask(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetEventMask(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetEventMask
#define MouGetEventMask _MouGetEventMask

inline USHORT _MouGetNumButtons(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetNumButtons(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetNumButtons
#define MouGetNumButtons _MouGetNumButtons

inline USHORT _MouGetNumMickeys(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetNumMickeys(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetNumMickeys
#define MouGetNumMickeys _MouGetNumMickeys

inline USHORT _MouGetNumQueEl(PMOUQUEINFO a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetNumQueEl(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetNumQueEl
#define MouGetNumQueEl _MouGetNumQueEl

inline USHORT _MouGetPtrPos(PPTRLOC a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetPtrPos(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetPtrPos
#define MouGetPtrPos _MouGetPtrPos

inline USHORT _MouGetPtrShape(PBYTE a, PPTRSHAPE b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetPtrShape(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetPtrShape
#define MouGetPtrShape _MouGetPtrShape

inline USHORT _MouGetScaleFact(PSCALEFACT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetScaleFact(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetScaleFact
#define MouGetScaleFact _MouGetScaleFact

inline USHORT _MouGetThreshold(PTHRESHOLD a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouGetThreshold(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouGetThreshold
#define MouGetThreshold _MouGetThreshold

inline USHORT _MouInitReal(PSZ a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouInitReal(a);
    SetFS(sel);

    return yyrc;
}

#undef  MouInitReal
#define MouInitReal _MouInitReal

inline USHORT _MouOpen(PSZ a, PHMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouOpen(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouOpen
#define MouOpen _MouOpen

inline USHORT _MouReadEventQue(PMOUEVENTINFO a, PUSHORT b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouReadEventQue(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  MouReadEventQue
#define MouReadEventQue _MouReadEventQue

inline USHORT _MouRegister(PSZ a, PSZ b, ULONG c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouRegister(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  MouRegister
#define MouRegister _MouRegister

inline USHORT _MouRemovePtr(PNOPTRRECT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouRemovePtr(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouRemovePtr
#define MouRemovePtr _MouRemovePtr

inline USHORT _MouSetDevStatus(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouSetDevStatus(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouSetDevStatus
#define MouSetDevStatus _MouSetDevStatus

inline USHORT _MouSetEventMask(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouSetEventMask(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouSetEventMask
#define MouSetEventMask _MouSetEventMask

inline USHORT _MouSetPtrPos(PPTRLOC a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouSetPtrPos(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouSetPtrPos
#define MouSetPtrPos _MouSetPtrPos

inline USHORT _MouSetPtrShape(PBYTE a, PPTRSHAPE b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouSetPtrShape(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  MouSetPtrShape
#define MouSetPtrShape _MouSetPtrShape

inline USHORT _MouSetScaleFact(PSCALEFACT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouSetScaleFact(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouSetScaleFact
#define MouSetScaleFact _MouSetScaleFact

inline USHORT _MouSetThreshold(PTHRESHOLD a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouSetThreshold(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  MouSetThreshold
#define MouSetThreshold _MouSetThreshold

inline USHORT _MouSynch(USHORT a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MouSynch(a);
    SetFS(sel);

    return yyrc;
}

#undef  MouSynch
#define MouSynch _MouSynch

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
