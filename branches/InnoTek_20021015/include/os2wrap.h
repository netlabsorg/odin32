/* $Id: os2wrap.h,v 1.15 2000-06-17 12:31:55 sandervl Exp $ */
#ifndef __OS2WRAP_H__
#define __OS2WRAP_H__

#include <os2.h>
#include <os2sel.h>
#include <os2newapi.h>

#if (__IBMC__ > 300)
// VA3.6: inline is a C++ keyword, must be translated to _inline or _Inline or __inline in C code
#define inline _inline
#endif

#ifdef INCL_DOSMEMMGR

APIRET APIENTRY DosAliasMem(PVOID pb, ULONG cb, PPVOID ppbAlias, ULONG fl);

inline APIRET _DosAliasMem(PVOID pb, ULONG cb, PPVOID ppbAlias, ULONG fl)
{
 APIRET yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosAliasMem(pb, cb, ppbAlias, fl);
    SetFS(sel);

    return yyrc;
} 

#undef  DosAliasMem
#define DosAliasMem _DosAliasMem

inline ULONG _DosAllocMem(PPVOID a, ULONG b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosAllocMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosAllocMem
#define DosAllocMem _DosAllocMem

inline ULONG _DosAllocSharedMem(PPVOID a, PCSZ b, ULONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosAllocSharedMem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosAllocSharedMem
#define DosAllocSharedMem _DosAllocSharedMem

inline ULONG _DosFreeMem(PVOID a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFreeMem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFreeMem
#define DosFreeMem _DosFreeMem

inline ULONG _DosGetNamedSharedMem(PPVOID a, PCSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosGetNamedSharedMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosGetNamedSharedMem
#define DosGetNamedSharedMem _DosGetNamedSharedMem

inline ULONG _DosGetSharedMem(PVOID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosGetSharedMem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosGetSharedMem
#define DosGetSharedMem _DosGetSharedMem

inline ULONG _DosGiveSharedMem(PVOID a, PID b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosGiveSharedMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosGiveSharedMem
#define DosGiveSharedMem _DosGiveSharedMem

inline ULONG _DosQueryMem(PVOID a, PULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryMem
#define DosQueryMem _DosQueryMem

inline ULONG _DosSetMem(PVOID a, ULONG b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetMem
#define DosSetMem _DosSetMem

inline ULONG _DosSubAllocMem(PVOID a, PPVOID b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSubAllocMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSubAllocMem
#define DosSubAllocMem _DosSubAllocMem

inline ULONG _DosSubFreeMem(PVOID a, PVOID b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSubFreeMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSubFreeMem
#define DosSubFreeMem _DosSubFreeMem

inline ULONG _DosSubSetMem(PVOID a, ULONG b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSubSetMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSubSetMem
#define DosSubSetMem _DosSubSetMem

inline ULONG _DosSubUnsetMem(PVOID a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSubUnsetMem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSubUnsetMem
#define DosSubUnsetMem _DosSubUnsetMem

#endif
#ifdef INCL_DOSFILEMGR
inline ULONG _DosCancelLockRequest(HFILE a, PFILELOCK b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCancelLockRequest(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCancelLockRequest
#define DosCancelLockRequest _DosCancelLockRequest

inline ULONG _DosClose(HFILE a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosClose(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosClose
#define DosClose _DosClose

inline ULONG _DosCopy(PCSZ a, PCSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCopy(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCopy
#define DosCopy _DosCopy

inline ULONG _DosCreateDir(PCSZ a, PEAOP2 b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreateDir(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreateDir
#define DosCreateDir _DosCreateDir

inline ULONG _DosDelete(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDelete(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDelete
#define DosDelete _DosDelete

inline ULONG _DosDeleteDir(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDeleteDir(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDeleteDir
#define DosDeleteDir _DosDeleteDir

inline ULONG _DosDupHandle(HFILE a, PHFILE b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDupHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDupHandle
#define DosDupHandle _DosDupHandle

inline ULONG _DosEditName(ULONG a, PCSZ b, PCSZ c, PBYTE d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosEditName(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosEditName
#define DosEditName _DosEditName

inline ULONG _DosEnumAttribute(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosEnumAttribute(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosEnumAttribute
#define DosEnumAttribute _DosEnumAttribute

inline ULONG _DosFindClose(HDIR a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFindClose(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFindClose
#define DosFindClose _DosFindClose

inline ULONG _DosFindFirst(PCSZ a, PHDIR b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFindFirst(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFindFirst
#define DosFindFirst _DosFindFirst

inline ULONG _DosFindNext(HDIR a, PVOID b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFindNext(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFindNext
#define DosFindNext _DosFindNext

inline ULONG _DosForceDelete(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosForceDelete(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosForceDelete
#define DosForceDelete _DosForceDelete

inline ULONG _DosFSAttach(PCSZ a, PCSZ b, PVOID c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFSAttach(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFSAttach
#define DosFSAttach _DosFSAttach

inline ULONG _DosFSCtl(PVOID a, ULONG b, PULONG c, PVOID d, ULONG e, PULONG f, ULONG g, PCSZ h, HFILE i, ULONG j)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFSCtl(a, b, c, d, e, f, g, h, i, j);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFSCtl
#define DosFSCtl _DosFSCtl

inline ULONG _DosMove(PCSZ a, PCSZ b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosMove(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosMove
#define DosMove _DosMove

inline ULONG _DosOpen(PCSZ a, PHFILE b, PULONG c, ULONG d, ULONG e, ULONG f, ULONG g, PEAOP2 h)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosOpen(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  DosOpen
#define DosOpen _DosOpen

inline ULONG _DosProtectClose(HFILE a, FHLOCK b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectClose(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectClose
#define DosProtectClose _DosProtectClose

inline ULONG _DosProtectEnumAttribute(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g, FHLOCK h)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectEnumAttribute(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectEnumAttribute
#define DosProtectEnumAttribute _DosProtectEnumAttribute

inline ULONG _DosProtectOpen(PCSZ a, PHFILE b, PULONG c, ULONG d, ULONG e, ULONG f, ULONG g, PEAOP2 h, PFHLOCK i)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectOpen(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectOpen
#define DosProtectOpen _DosProtectOpen

inline ULONG _DosProtectQueryFHState(HFILE a, PULONG b, FHLOCK c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectQueryFHState(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectQueryFHState
#define DosProtectQueryFHState _DosProtectQueryFHState

inline ULONG _DosProtectQueryFileInfo(HFILE a, ULONG b, PVOID c, ULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectQueryFileInfo(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectQueryFileInfo
#define DosProtectQueryFileInfo _DosProtectQueryFileInfo

inline ULONG _DosProtectRead(HFILE a, PVOID b, ULONG c, PULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectRead(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectRead
#define DosProtectRead _DosProtectRead

inline ULONG _DosProtectSetFHState(HFILE a, ULONG b, FHLOCK c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectSetFHState(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectSetFHState
#define DosProtectSetFHState _DosProtectSetFHState

inline ULONG _DosProtectSetFileInfo(HFILE a, ULONG b, PVOID c, ULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectSetFileInfo(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectSetFileInfo
#define DosProtectSetFileInfo _DosProtectSetFileInfo

inline ULONG _DosProtectSetFileLocks(HFILE a, PFILELOCK b, PFILELOCK c, ULONG d, ULONG e, FHLOCK f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectSetFileLocks(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectSetFileLocks
#define DosProtectSetFileLocks _DosProtectSetFileLocks

inline ULONG _DosProtectSetFilePtr(HFILE a, LONG b, ULONG c, PULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectSetFilePtr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectSetFilePtr
#define DosProtectSetFilePtr _DosProtectSetFilePtr

inline ULONG _DosProtectSetFileSize(HFILE a, ULONG b, FHLOCK c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectSetFileSize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectSetFileSize
#define DosProtectSetFileSize _DosProtectSetFileSize

inline ULONG _DosProtectWrite(HFILE a, PVOID b, ULONG c, PULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosProtectWrite(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosProtectWrite
#define DosProtectWrite _DosProtectWrite

inline ULONG _DosQueryCurrentDir(ULONG a, PBYTE b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryCurrentDir(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryCurrentDir
#define DosQueryCurrentDir _DosQueryCurrentDir

inline ULONG _DosQueryCurrentDisk(PULONG a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryCurrentDisk(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryCurrentDisk
#define DosQueryCurrentDisk _DosQueryCurrentDisk

inline ULONG _DosQueryFHState(HFILE a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryFHState(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryFHState
#define DosQueryFHState _DosQueryFHState

inline ULONG _DosQueryFileInfo(HFILE a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryFileInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryFileInfo
#define DosQueryFileInfo _DosQueryFileInfo

inline ULONG _DosQueryFSAttach(PCSZ a, ULONG b, ULONG c, PFSQBUFFER2 d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryFSAttach(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryFSAttach
#define DosQueryFSAttach _DosQueryFSAttach

inline ULONG _DosQueryFSInfo(ULONG a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryFSInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryFSInfo
#define DosQueryFSInfo _DosQueryFSInfo

inline ULONG _DosQueryHType(HFILE a, PULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryHType(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryHType
#define DosQueryHType _DosQueryHType

inline ULONG _DosQueryPathInfo(PCSZ a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryPathInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryPathInfo
#define DosQueryPathInfo _DosQueryPathInfo

inline ULONG _DosQueryVerify(PBOOL32 a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryVerify(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryVerify
#define DosQueryVerify _DosQueryVerify

inline ULONG _DosRead(HFILE a, PVOID b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosRead(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosRead
#define DosRead _DosRead

inline ULONG _DosResetBuffer(HFILE a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosResetBuffer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosResetBuffer
#define DosResetBuffer _DosResetBuffer

inline ULONG _DosSetCurrentDir(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetCurrentDir(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetCurrentDir
#define DosSetCurrentDir _DosSetCurrentDir

inline ULONG _DosSetDefaultDisk(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetDefaultDisk(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetDefaultDisk
#define DosSetDefaultDisk _DosSetDefaultDisk

inline ULONG _DosSetFHState(HFILE a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFHState(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetFHState
#define DosSetFHState _DosSetFHState

inline ULONG _DosSetFileInfo(HFILE a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFileInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetFileInfo
#define DosSetFileInfo _DosSetFileInfo

inline ULONG _DosSetFileLocks(HFILE a, PFILELOCK b, PFILELOCK c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFileLocks(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetFileLocks
#define DosSetFileLocks _DosSetFileLocks

inline ULONG _DosSetFilePtr(HFILE a, LONG b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFilePtr(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetFilePtr
#define DosSetFilePtr _DosSetFilePtr

inline ULONG _DosSetFileSize(HFILE a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFileSize(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetFileSize
#define DosSetFileSize _DosSetFileSize

inline ULONG _DosSetFSInfo(ULONG a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFSInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetFSInfo
#define DosSetFSInfo _DosSetFSInfo

inline ULONG _DosSetMaxFH(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetMaxFH(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetMaxFH
#define DosSetMaxFH _DosSetMaxFH

inline ULONG _DosSetPathInfo(PCSZ a, ULONG b, PVOID c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetPathInfo(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetPathInfo
#define DosSetPathInfo _DosSetPathInfo

inline ULONG _DosSetRelMaxFH(PLONG a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetRelMaxFH(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetRelMaxFH
#define DosSetRelMaxFH _DosSetRelMaxFH

inline ULONG _DosSetVerify(BOOL32 a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetVerify(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetVerify
#define DosSetVerify _DosSetVerify

inline ULONG _DosShutdown(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosShutdown(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosShutdown
#define DosShutdown _DosShutdown

inline ULONG _DosWrite(HFILE a, PVOID b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosWrite(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosWrite
#define DosWrite _DosWrite

#endif
#ifdef INCL_DOSMISC
inline ULONG _DosSearchPath(ULONG a, PCSZ b, PCSZ c, PBYTE d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSearchPath(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSearchPath
#define DosSearchPath _DosSearchPath

inline ULONG  _DosError(ULONG error)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosError(error);
    SetFS(sel);

    return yyrc;
} 

#undef  DosError
#define DosError _DosError

#endif
#ifdef INCL_DOSDEVICES
inline ULONG _DosDevConfig(PVOID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDevConfig(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDevConfig
#define DosDevConfig _DosDevConfig

inline ULONG _DosDevIOCtl(HFILE a, ULONG b, ULONG c, PVOID d, ULONG e, PULONG f, PVOID g, ULONG h, PULONG i)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDevIOCtl(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDevIOCtl
#define DosDevIOCtl _DosDevIOCtl

inline ULONG _DosPhysicalDisk(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosPhysicalDisk(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosPhysicalDisk
#define DosPhysicalDisk _DosPhysicalDisk

#endif
#ifdef INCL_DOSNLS
inline ULONG _DosMapCase(ULONG a, PCOUNTRYCODE b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosMapCase(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosMapCase
#define DosMapCase _DosMapCase

inline ULONG _DosQueryCollate(ULONG a, PCOUNTRYCODE b, PCHAR c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryCollate(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryCollate
#define DosQueryCollate _DosQueryCollate

inline ULONG _DosQueryCp(ULONG a, PULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryCp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryCp
#define DosQueryCp _DosQueryCp

inline ULONG _DosQueryCtryInfo(ULONG a, PCOUNTRYCODE b, PCOUNTRYINFO c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryCtryInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryCtryInfo
#define DosQueryCtryInfo _DosQueryCtryInfo

inline ULONG _DosQueryDBCSEnv(ULONG a, PCOUNTRYCODE b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryDBCSEnv(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryDBCSEnv
#define DosQueryDBCSEnv _DosQueryDBCSEnv

inline ULONG _DosSetProcessCp(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetProcessCp(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetProcessCp
#define DosSetProcessCp _DosSetProcessCp

#endif
#ifdef INCL_DOSMODULEMGR
inline ULONG _DosFreeModule(HMODULE a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFreeModule(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFreeModule
#define DosFreeModule _DosFreeModule

inline ULONG _DosLoadModule(PSZ a, ULONG b, PCSZ c, PHMODULE d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosLoadModule(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosLoadModule
#define DosLoadModule _DosLoadModule

inline ULONG _DosQueryModuleHandle(PCSZ a, PHMODULE b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryModuleHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryModuleHandle
#define DosQueryModuleHandle _DosQueryModuleHandle

inline ULONG _DosQueryModuleName(HMODULE a, ULONG b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryModuleName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryModuleName
#define DosQueryModuleName _DosQueryModuleName

inline ULONG _DosQueryProcAddr(HMODULE a, ULONG b, PCSZ c, PFN *d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryProcAddr(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryProcAddr
#define DosQueryProcAddr _DosQueryProcAddr

inline ULONG _DosQueryProcType(HMODULE a, ULONG b, PCSZ c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryProcType(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryProcType
#define DosQueryProcType _DosQueryProcType

#endif
#ifdef INCL_DOSRESOURCES
inline ULONG _DosFreeResource(PVOID a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFreeResource(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFreeResource
#define DosFreeResource _DosFreeResource

inline ULONG _DosGetResource(HMODULE a, ULONG b, ULONG c, PPVOID d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosGetResource(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosGetResource
#define DosGetResource _DosGetResource

inline ULONG _DosQueryResourceSize(HMODULE a, ULONG b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryResourceSize(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryResourceSize
#define DosQueryResourceSize _DosQueryResourceSize

#endif
#ifdef INCL_DOSPROCESS
inline ULONG _DosBeep(ULONG a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosBeep(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosBeep
#define DosBeep _DosBeep

inline VOID _DosExit(ULONG a, ULONG b)
{
 USHORT sel = RestoreOS2FS();

    DosExit(a, b);
    SetFS(sel);
} 

#undef  DosExit
#define DosExit _DosExit

inline ULONG _DosAllocThreadLocalMemory(ULONG a, PULONG *b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosAllocThreadLocalMemory(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosAllocThreadLocalMemory
#define DosAllocThreadLocalMemory _DosAllocThreadLocalMemory

inline ULONG _DosCreateThread(PTID a, PFNTHREAD b, ULONG c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreateThread(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreateThread
#define DosCreateThread _DosCreateThread

inline ULONG _DosEnterCritSec()
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosEnterCritSec();
    SetFS(sel);

    return yyrc;
} 

#undef  DosEnterCritSec
#define DosEnterCritSec _DosEnterCritSec

inline ULONG _DosExecPgm(PCHAR a, LONG b, ULONG c, PCSZ d, PCSZ e, PRESULTCODES f, PCSZ g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosExecPgm(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosExecPgm
#define DosExecPgm _DosExecPgm

inline ULONG _DosExitCritSec()
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosExitCritSec();
    SetFS(sel);

    return yyrc;
} 

#undef  DosExitCritSec
#define DosExitCritSec _DosExitCritSec

inline ULONG _DosExitList(ULONG a, PFNEXITLIST b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosExitList(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosExitList
#define DosExitList _DosExitList

inline ULONG _DosFreeThreadLocalMemory(ULONG *a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosFreeThreadLocalMemory(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosFreeThreadLocalMemory
#define DosFreeThreadLocalMemory _DosFreeThreadLocalMemory

inline ULONG _DosGetInfoBlocks(PTIB *a, PPIB *b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosGetInfoBlocks(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosGetInfoBlocks
#define DosGetInfoBlocks _DosGetInfoBlocks

inline ULONG _DosKillProcess(ULONG a, PID b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosKillProcess(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosKillProcess
#define DosKillProcess _DosKillProcess

inline ULONG _DosKillThread(TID a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosKillThread(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosKillThread
#define DosKillThread _DosKillThread

inline ULONG _DosResumeThread(TID a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosResumeThread(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosResumeThread
#define DosResumeThread _DosResumeThread

inline ULONG _DosSetPriority(ULONG a, ULONG b, LONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetPriority(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetPriority
#define DosSetPriority _DosSetPriority

inline ULONG _DosSleep(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSleep(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSleep
#define DosSleep _DosSleep

inline ULONG _DosSuspendThread(TID a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSuspendThread(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSuspendThread
#define DosSuspendThread _DosSuspendThread

inline ULONG _DosWaitChild(ULONG a, ULONG b, PRESULTCODES c, PPID d, PID e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosWaitChild(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosWaitChild
#define DosWaitChild _DosWaitChild

inline ULONG _DosWaitThread(PTID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosWaitThread(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosWaitThread
#define DosWaitThread _DosWaitThread

#endif

#ifdef INCL_DOSSESMGR
inline ULONG _DosQueryAppType(PCSZ a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryAppType(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryAppType
#define DosQueryAppType _DosQueryAppType

inline ULONG _DosSelectSession(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSelectSession(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSelectSession
#define DosSelectSession _DosSelectSession

inline ULONG _DosSetSession(ULONG a, PSTATUSDATA b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetSession(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetSession
#define DosSetSession _DosSetSession

inline ULONG _DosStartSession(PSTARTDATA a, PULONG b, PPID c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosStartSession(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosStartSession
#define DosStartSession _DosStartSession

inline ULONG _DosStopSession(ULONG a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosStopSession(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosStopSession
#define DosStopSession _DosStopSession

#endif
#ifdef INCL_DOSSEMAPHORES
inline ULONG _DosCloseEventSem(HEV a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCloseEventSem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCloseEventSem
#define DosCloseEventSem _DosCloseEventSem

inline ULONG _DosCreateEventSem(PCSZ a, PHEV b, ULONG c, BOOL32 d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreateEventSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreateEventSem
#define DosCreateEventSem _DosCreateEventSem

inline ULONG _DosOpenEventSem(PCSZ a, PHEV b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosOpenEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosOpenEventSem
#define DosOpenEventSem _DosOpenEventSem

inline ULONG _DosPostEventSem(HEV a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosPostEventSem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosPostEventSem
#define DosPostEventSem _DosPostEventSem

inline ULONG _DosQueryEventSem(HEV a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryEventSem
#define DosQueryEventSem _DosQueryEventSem

inline ULONG _DosResetEventSem(HEV a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosResetEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosResetEventSem
#define DosResetEventSem _DosResetEventSem

inline ULONG _DosWaitEventSem(HEV a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosWaitEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosWaitEventSem
#define DosWaitEventSem _DosWaitEventSem

inline ULONG _DosCloseMutexSem(HMTX a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCloseMutexSem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCloseMutexSem
#define DosCloseMutexSem _DosCloseMutexSem

inline ULONG _DosCreateMutexSem(PCSZ a, PHMTX b, ULONG c, BOOL32 d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreateMutexSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreateMutexSem
#define DosCreateMutexSem _DosCreateMutexSem

inline ULONG _DosOpenMutexSem(PCSZ a, PHMTX b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosOpenMutexSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosOpenMutexSem
#define DosOpenMutexSem _DosOpenMutexSem

inline ULONG _DosQueryMutexSem(HMTX a, PPID b, PTID c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryMutexSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryMutexSem
#define DosQueryMutexSem _DosQueryMutexSem

inline ULONG _DosReleaseMutexSem(HMTX a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosReleaseMutexSem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosReleaseMutexSem
#define DosReleaseMutexSem _DosReleaseMutexSem

inline ULONG _DosRequestMutexSem(HMTX a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosRequestMutexSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosRequestMutexSem
#define DosRequestMutexSem _DosRequestMutexSem

inline ULONG _DosAddMuxWaitSem(HMUX a, PSEMRECORD b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosAddMuxWaitSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosAddMuxWaitSem
#define DosAddMuxWaitSem _DosAddMuxWaitSem

inline ULONG _DosCloseMuxWaitSem(HMUX a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCloseMuxWaitSem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCloseMuxWaitSem
#define DosCloseMuxWaitSem _DosCloseMuxWaitSem

inline ULONG _DosCreateMuxWaitSem(PCSZ a, PHMUX b, ULONG c, PSEMRECORD d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreateMuxWaitSem(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreateMuxWaitSem
#define DosCreateMuxWaitSem _DosCreateMuxWaitSem

inline ULONG _DosDeleteMuxWaitSem(HMUX a, HSEM b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDeleteMuxWaitSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDeleteMuxWaitSem
#define DosDeleteMuxWaitSem _DosDeleteMuxWaitSem

inline ULONG _DosOpenMuxWaitSem(PCSZ a, PHMUX b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosOpenMuxWaitSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosOpenMuxWaitSem
#define DosOpenMuxWaitSem _DosOpenMuxWaitSem

inline ULONG _DosQueryMuxWaitSem(HMUX a, PULONG b, PSEMRECORD c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryMuxWaitSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryMuxWaitSem
#define DosQueryMuxWaitSem _DosQueryMuxWaitSem

inline ULONG _DosWaitMuxWaitSem(HMUX a, ULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosWaitMuxWaitSem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosWaitMuxWaitSem
#define DosWaitMuxWaitSem _DosWaitMuxWaitSem

#endif
#ifdef INCL_DOSNMPIPES
inline ULONG _DosCallNPipe(PCSZ a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCallNPipe(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCallNPipe
#define DosCallNPipe _DosCallNPipe

inline ULONG _DosConnectNPipe(HPIPE a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosConnectNPipe(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosConnectNPipe
#define DosConnectNPipe _DosConnectNPipe

inline ULONG _DosCreateNPipe(PCSZ a, PHPIPE b, ULONG c, ULONG d, ULONG e, ULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreateNPipe(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreateNPipe
#define DosCreateNPipe _DosCreateNPipe

inline ULONG _DosDisConnectNPipe(HPIPE a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDisConnectNPipe(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDisConnectNPipe
#define DosDisConnectNPipe _DosDisConnectNPipe

inline ULONG _DosPeekNPipe(HPIPE a, PVOID b, ULONG c, PULONG d, PAVAILDATA e, PULONG f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosPeekNPipe(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DosPeekNPipe
#define DosPeekNPipe _DosPeekNPipe

inline ULONG _DosQueryNPHState(HPIPE a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryNPHState(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryNPHState
#define DosQueryNPHState _DosQueryNPHState

inline ULONG _DosQueryNPipeInfo(HPIPE a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryNPipeInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryNPipeInfo
#define DosQueryNPipeInfo _DosQueryNPipeInfo

inline ULONG _DosQueryNPipeSemState(HSEM a, PPIPESEMSTATE b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryNPipeSemState(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryNPipeSemState
#define DosQueryNPipeSemState _DosQueryNPipeSemState

//AH: Different definition in 4.5 toolkit headers!!!
//we don't need it so far, so let's just remove it for now
#if 0
inline ULONG _DosRawReadNPipe(PCSZ a, ULONG b, PULONG c, PVOID d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosRawReadNPipe(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosRawReadNPipe
#define DosRawReadNPipe _DosRawReadNPipe

inline ULONG _DosRawWriteNPipe(PCSZ a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosRawWriteNPipe(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosRawWriteNPipe
#define DosRawWriteNPipe _DosRawWriteNPipe

#endif

inline ULONG _DosSetNPHState(HPIPE a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetNPHState(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetNPHState
#define DosSetNPHState _DosSetNPHState

inline ULONG _DosSetNPipeSem(HPIPE a, HSEM b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetNPipeSem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetNPipeSem
#define DosSetNPipeSem _DosSetNPipeSem

inline ULONG _DosTransactNPipe(HPIPE a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosTransactNPipe(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DosTransactNPipe
#define DosTransactNPipe _DosTransactNPipe

inline ULONG _DosWaitNPipe(PCSZ a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosWaitNPipe(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosWaitNPipe
#define DosWaitNPipe _DosWaitNPipe

inline ULONG _DosCreatePipe(PHFILE a, PHFILE b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreatePipe(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreatePipe
#define DosCreatePipe _DosCreatePipe

#endif
#ifdef INCL_DOSQUEUES
inline ULONG _DosCloseQueue(HQUEUE a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCloseQueue(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCloseQueue
#define DosCloseQueue _DosCloseQueue

inline ULONG _DosCreateQueue(PHQUEUE a, ULONG b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCreateQueue(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCreateQueue
#define DosCreateQueue _DosCreateQueue

inline ULONG _DosOpenQueue(PPID a, PHQUEUE b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosOpenQueue(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosOpenQueue
#define DosOpenQueue _DosOpenQueue

inline ULONG _DosPeekQueue(HQUEUE a, PREQUESTDATA b, PULONG c, PPVOID d, PULONG e, BOOL32 f, PBYTE g, HEV h)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosPeekQueue(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  DosPeekQueue
#define DosPeekQueue _DosPeekQueue

inline ULONG _DosPurgeQueue(HQUEUE a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosPurgeQueue(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosPurgeQueue
#define DosPurgeQueue _DosPurgeQueue

inline ULONG _DosQueryQueue(HQUEUE a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryQueue
#define DosQueryQueue _DosQueryQueue

inline ULONG _DosReadQueue(HQUEUE a, PREQUESTDATA b, PULONG c, PPVOID d, ULONG e, BOOL32 f, PBYTE g, HEV h)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosReadQueue(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  DosReadQueue
#define DosReadQueue _DosReadQueue

inline ULONG _DosWriteQueue(HQUEUE a, ULONG b, ULONG c, PVOID d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosWriteQueue(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DosWriteQueue
#define DosWriteQueue _DosWriteQueue

#endif
#ifdef INCL_DOSEXCEPTIONS
inline ULONG _DosAcknowledgeSignalException(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosAcknowledgeSignalException(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosAcknowledgeSignalException
#define DosAcknowledgeSignalException _DosAcknowledgeSignalException

inline ULONG _DosEnterMustComplete(PULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosEnterMustComplete(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosEnterMustComplete
#define DosEnterMustComplete _DosEnterMustComplete

inline ULONG _DosExitMustComplete(PULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosExitMustComplete(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosExitMustComplete
#define DosExitMustComplete _DosExitMustComplete

inline ULONG _DosQueryThreadContext(TID a, ULONG b, PCONTEXTRECORD c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryThreadContext(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryThreadContext
#define DosQueryThreadContext _DosQueryThreadContext

inline ULONG _DosRaiseException(PEXCEPTIONREPORTRECORD a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosRaiseException(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosRaiseException
#define DosRaiseException _DosRaiseException

inline ULONG _DosSendSignalException(PID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSendSignalException(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSendSignalException
#define DosSendSignalException _DosSendSignalException

inline ULONG _DosSetExceptionHandler(PEXCEPTIONREGISTRATIONRECORD a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetExceptionHandler(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetExceptionHandler
#define DosSetExceptionHandler _DosSetExceptionHandler

inline ULONG _DosSetSignalExceptionFocus(BOOL32 a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetSignalExceptionFocus(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetSignalExceptionFocus
#define DosSetSignalExceptionFocus _DosSetSignalExceptionFocus

inline ULONG _DosUnsetExceptionHandler(PEXCEPTIONREGISTRATIONRECORD a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosUnsetExceptionHandler(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosUnsetExceptionHandler
#define DosUnsetExceptionHandler _DosUnsetExceptionHandler

inline ULONG _DosUnwindException(PEXCEPTIONREGISTRATIONRECORD a, PVOID b, PEXCEPTIONREPORTRECORD c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosUnwindException(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosUnwindException
#define DosUnwindException _DosUnwindException

#endif
#ifdef INCL_DOSMISC
inline ULONG _DosQuerySysInfo(ULONG a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQuerySysInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQuerySysInfo
#define DosQuerySysInfo _DosQuerySysInfo

inline ULONG _DosScanEnv(PCSZ a, PCSZ *b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosScanEnv(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosScanEnv
#define DosScanEnv _DosScanEnv

inline ULONG _DosQueryRASInfo(ULONG a, PPVOID b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryRASInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryRASInfo
#define DosQueryRASInfo _DosQueryRASInfo

#endif
#ifdef INCL_DOSDATETIME
inline ULONG _DosGetDateTime(PDATETIME a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosGetDateTime(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosGetDateTime
#define DosGetDateTime _DosGetDateTime

inline ULONG _DosSetDateTime(PDATETIME a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetDateTime(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetDateTime
#define DosSetDateTime _DosSetDateTime

inline ULONG _DosAsyncTimer(ULONG a, HSEM b, PHTIMER c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosAsyncTimer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosAsyncTimer
#define DosAsyncTimer _DosAsyncTimer

inline ULONG _DosStartTimer(ULONG a, HSEM b, PHTIMER c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosStartTimer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosStartTimer
#define DosStartTimer _DosStartTimer

inline ULONG _DosStopTimer(HTIMER a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosStopTimer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosStopTimer
#define DosStopTimer _DosStopTimer

#endif
#ifdef DosTmrQueryFreq
inline ULONG _DosTmrQueryFreq(PULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosTmrQueryFreq(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosTmrQueryFreq
#define DosTmrQueryFreq _DosTmrQueryFreq

inline ULONG _DosTmrQueryTime(PQWORD a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosTmrQueryTime(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosTmrQueryTime
#define DosTmrQueryTime _DosTmrQueryTime

#endif
#ifdef INCL_DOSMVDM
inline ULONG _DosCloseVDD(HVDD a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosCloseVDD(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosCloseVDD
#define DosCloseVDD _DosCloseVDD

inline ULONG _DosOpenVDD(PCSZ a, PHVDD b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosOpenVDD(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosOpenVDD
#define DosOpenVDD _DosOpenVDD

inline ULONG _DosQueryDOSProperty(SGID a, PCSZ b, ULONG c, PSZ d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryDOSProperty(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryDOSProperty
#define DosQueryDOSProperty _DosQueryDOSProperty

inline ULONG _DosRequestVDD(HVDD a, SGID b, ULONG c, ULONG d, PVOID e, ULONG f, PVOID g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosRequestVDD(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosRequestVDD
#define DosRequestVDD _DosRequestVDD

inline ULONG _DosSetDOSProperty(SGID a, PCSZ b, ULONG c, PCSZ d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetDOSProperty(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSetDOSProperty
#define DosSetDOSProperty _DosSetDOSProperty

#endif
#ifdef INCL_DOSPROCESS
inline ULONG _DosDebug(uDB_t *a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDebug(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDebug
#define DosDebug _DosDebug

#endif
#ifdef INCL_DOSMISC
inline ULONG _DosGetMessage(PCHAR *a, ULONG b, PCHAR c, ULONG d, ULONG e, PCSZ f, PULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosGetMessage(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosGetMessage
#define DosGetMessage _DosGetMessage

inline ULONG _DosInsertMessage(PCHAR *a, ULONG b, PCSZ c, ULONG d, PCHAR e, ULONG f, PULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosInsertMessage(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DosInsertMessage
#define DosInsertMessage _DosInsertMessage

inline ULONG _DosPutMessage(HFILE a, ULONG b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosPutMessage(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosPutMessage
#define DosPutMessage _DosPutMessage

inline ULONG _DosQueryMessageCP(PCHAR a, ULONG b, PCSZ c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryMessageCP(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryMessageCP
#define DosQueryMessageCP _DosQueryMessageCP

#endif
#ifdef INCL_DOSRAS
inline ULONG _DosDumpProcess(ULONG a, ULONG b, PID c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosDumpProcess(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DosDumpProcess
#define DosDumpProcess _DosDumpProcess

inline ULONG _DosForceSystemDump(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosForceSystemDump(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DosForceSystemDump
#define DosForceSystemDump _DosForceSystemDump

inline ULONG _DosQueryRASInfo(ULONG a, PPVOID b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosQueryRASInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosQueryRASInfo
#define DosQueryRASInfo _DosQueryRASInfo

inline ULONG _DosSuppressPopUps(ULONG a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSuppressPopUps(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DosSuppressPopUps
#define DosSuppressPopUps _DosSuppressPopUps

#endif
#ifdef INCL_RXSUBCOM
inline ULONG _RexxDeregisterSubcom(PCSZ a, PCSZ b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxDeregisterSubcom(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxDeregisterSubcom
#define RexxDeregisterSubcom _RexxDeregisterSubcom

inline ULONG _RexxQuerySubcom(PCSZ a, PCSZ b, PUSHORT c, PUCHAR d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxQuerySubcom(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxQuerySubcom
#define RexxQuerySubcom _RexxQuerySubcom

inline ULONG _RexxRegisterSubcomDll(PCSZ a, PCSZ b, PCSZ c, PUCHAR d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxRegisterSubcomDll(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxRegisterSubcomDll
#define RexxRegisterSubcomDll _RexxRegisterSubcomDll

inline ULONG _RexxRegisterSubcomExe(PCSZ a, PFN b, PUCHAR c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxRegisterSubcomExe(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxRegisterSubcomExe
#define RexxRegisterSubcomExe _RexxRegisterSubcomExe

#endif
#ifdef INCL_RXSHV
inline ULONG _RexxVariablePool(PSHVBLOCK a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxVariablePool(a);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxVariablePool
#define RexxVariablePool _RexxVariablePool

#endif
#ifdef INCL_RXFUNC
inline ULONG _RexxDeregisterFunction(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxDeregisterFunction(a);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxDeregisterFunction
#define RexxDeregisterFunction _RexxDeregisterFunction

inline ULONG _RexxQueryFunction(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxQueryFunction(a);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxQueryFunction
#define RexxQueryFunction _RexxQueryFunction

inline ULONG _RexxRegisterFunctionDll(PCSZ a, PCSZ b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxRegisterFunctionDll(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxRegisterFunctionDll
#define RexxRegisterFunctionDll _RexxRegisterFunctionDll

inline ULONG _RexxRegisterFunctionExe(PCSZ a, RexxFunctionHandler *b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxRegisterFunctionExe(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxRegisterFunctionExe
#define RexxRegisterFunctionExe _RexxRegisterFunctionExe

#endif
#ifdef INCL_RXSYSEXIT
inline ULONG _RexxDeregisterExit(PCSZ a, PCSZ b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxDeregisterExit(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxDeregisterExit
#define RexxDeregisterExit _RexxDeregisterExit

inline ULONG _RexxQueryExit(PCSZ a, PCSZ b, PUSHORT c, PUCHAR d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxQueryExit(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxQueryExit
#define RexxQueryExit _RexxQueryExit

inline ULONG _RexxRegisterExitDll(PCSZ a, PCSZ b, PCSZ c, PUCHAR d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxRegisterExitDll(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxRegisterExitDll
#define RexxRegisterExitDll _RexxRegisterExitDll

inline ULONG _RexxRegisterExitExe(PCSZ a, PFN b, PUCHAR c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxRegisterExitExe(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxRegisterExitExe
#define RexxRegisterExitExe _RexxRegisterExitExe

#endif
#ifdef INCL_RXARI
inline ULONG _RexxResetTrace(PID a, TID b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxResetTrace(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxResetTrace
#define RexxResetTrace _RexxResetTrace

inline ULONG _RexxSetHalt(PID a, TID b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxSetHalt(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxSetHalt
#define RexxSetHalt _RexxSetHalt

inline ULONG _RexxSetTrace(PID a, TID b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxSetTrace(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxSetTrace
#define RexxSetTrace _RexxSetTrace

#endif
#ifdef INCL_RXMACRO
inline ULONG _RexxAddMacro(PCSZ a, PCSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxAddMacro(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxAddMacro
#define RexxAddMacro _RexxAddMacro

inline ULONG _RexxClearMacroSpace()
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxClearMacroSpace();
    SetFS(sel);

    return yyrc;
} 

#undef  RexxClearMacroSpace
#define RexxClearMacroSpace _RexxClearMacroSpace

inline ULONG _RexxDropMacro(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxDropMacro(a);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxDropMacro
#define RexxDropMacro _RexxDropMacro

inline ULONG _RexxLoadMacroSpace(ULONG a, PCSZ *b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxLoadMacroSpace(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxLoadMacroSpace
#define RexxLoadMacroSpace _RexxLoadMacroSpace

inline ULONG _RexxQueryMacro(PCSZ a, PUSHORT b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxQueryMacro(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxQueryMacro
#define RexxQueryMacro _RexxQueryMacro

inline ULONG _RexxReorderMacro(PCSZ a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxReorderMacro(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxReorderMacro
#define RexxReorderMacro _RexxReorderMacro

inline ULONG _RexxSaveMacroSpace(ULONG a, PCSZ *b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxSaveMacroSpace(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxSaveMacroSpace
#define RexxSaveMacroSpace _RexxSaveMacroSpace

inline LONG _RexxStart(LONG a, PRXSTRING b, PCSZ c, PRXSTRING d, PCSZ e, LONG f, PRXSYSEXIT g, PSHORT h, PRXSTRING i)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = RexxStart(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  RexxStart
#define RexxStart _RexxStart

#endif
#ifdef INCL_WIN
#ifdef INCL_WINMESSAGEMGR
inline BOOL _WinCancelShutdown(HMQ a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCancelShutdown(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCancelShutdown
#define WinCancelShutdown _WinCancelShutdown

inline HMQ _WinCreateMsgQueue(HAB a, LONG b)
{
 HMQ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateMsgQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateMsgQueue
#define WinCreateMsgQueue _WinCreateMsgQueue

inline BOOL _WinDestroyMsgQueue(HMQ a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyMsgQueue(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyMsgQueue
#define WinDestroyMsgQueue _WinDestroyMsgQueue

inline MRESULT _WinDispatchMsg(HAB a, PQMSG b)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDispatchMsg(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDispatchMsg
#define WinDispatchMsg _WinDispatchMsg

inline BOOL _WinGetMsg(HAB a, PQMSG b, HWND c, ULONG d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetMsg
#define WinGetMsg _WinGetMsg

inline BOOL _WinLockInput(HMQ a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLockInput(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLockInput
#define WinLockInput _WinLockInput

inline BOOL _WinPeekMsg(HAB a, PQMSG b, HWND c, ULONG d, ULONG e, ULONG f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinPeekMsg(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinPeekMsg
#define WinPeekMsg _WinPeekMsg

inline BOOL _WinPostMsg(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinPostMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinPostMsg
#define WinPostMsg _WinPostMsg

inline HMQ _WinQueueFromID(HAB a, PID b, TID c)
{
 HMQ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueueFromID(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueueFromID
#define WinQueueFromID _WinQueueFromID

inline BOOL _WinQueryQueueInfo(HMQ a, PMQINFO b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryQueueInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryQueueInfo
#define WinQueryQueueInfo _WinQueryQueueInfo

inline HMQ _WinQuerySendMsg(HAB a, HMQ b, HMQ c, PQMSG d)
{
 HMQ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySendMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySendMsg
#define WinQuerySendMsg _WinQuerySendMsg

inline BOOL _WinRegisterUserDatatype(HAB a, LONG b, LONG c, PLONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRegisterUserDatatype(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRegisterUserDatatype
#define WinRegisterUserDatatype _WinRegisterUserDatatype

inline BOOL _WinRegisterUserMsg(HAB a, ULONG b, LONG c, LONG d, LONG e, LONG f, LONG g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRegisterUserMsg(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRegisterUserMsg
#define WinRegisterUserMsg _WinRegisterUserMsg

inline BOOL _WinReplyMsg(HAB a, HMQ b, HMQ c, MRESULT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinReplyMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinReplyMsg
#define WinReplyMsg _WinReplyMsg

inline MRESULT _WinSendMsg(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSendMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSendMsg
#define WinSendMsg _WinSendMsg

inline BOOL _WinSetMsgMode(HAB a, PCSZ b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetMsgMode(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetMsgMode
#define WinSetMsgMode _WinSetMsgMode

inline BOOL _WinSetSynchroMode(HAB a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetSynchroMode(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetSynchroMode
#define WinSetSynchroMode _WinSetSynchroMode

inline BOOL _WinThreadAssocQueue(HAB a, HMQ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinThreadAssocQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinThreadAssocQueue
#define WinThreadAssocQueue _WinThreadAssocQueue

inline BOOL _WinWakeThread(HMQ a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinWakeThread(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinWakeThread
#define WinWakeThread _WinWakeThread

#endif
inline HWND _WinCreateWindow(HWND a, PCSZ b, PCSZ c, ULONG d, LONG e, LONG f, LONG g, LONG h, HWND i, HWND j, ULONG k, PVOID l, PVOID m)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateWindow(a, b, c, d, e, f, g, h, i, j, k, l, m);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateWindow
#define WinCreateWindow _WinCreateWindow

inline BOOL _WinDrawBitmap(HPS a, HBITMAP b, PRECTL c, PPOINTL d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDrawBitmap(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDrawBitmap
#define WinDrawBitmap _WinDrawBitmap

inline BOOL _WinDrawBorder(HPS a, PRECTL b, LONG c, LONG d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDrawBorder(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDrawBorder
#define WinDrawBorder _WinDrawBorder

inline LONG _WinDrawText(HPS a, LONG b, PCH c, PRECTL d, LONG e, LONG f, ULONG g)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDrawText(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDrawText
#define WinDrawText _WinDrawText

inline BOOL _WinEnableWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEnableWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEnableWindow
#define WinEnableWindow _WinEnableWindow

inline BOOL _WinEnableWindowUpdate(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEnableWindowUpdate(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEnableWindowUpdate
#define WinEnableWindowUpdate _WinEnableWindowUpdate

inline BOOL _WinInvalidateRect(HWND a, PRECTL b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinInvalidateRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinInvalidateRect
#define WinInvalidateRect _WinInvalidateRect

inline BOOL _WinInvalidateRegion(HWND a, HRGN b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinInvalidateRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinInvalidateRegion
#define WinInvalidateRegion _WinInvalidateRegion

inline BOOL _WinInvertRect(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinInvertRect(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinInvertRect
#define WinInvertRect _WinInvertRect

inline BOOL _WinIsChild(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsChild(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsChild
#define WinIsChild _WinIsChild

inline BOOL _WinIsWindow(HAB a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsWindow
#define WinIsWindow _WinIsWindow

inline BOOL _WinIsWindowEnabled(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsWindowEnabled(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsWindowEnabled
#define WinIsWindowEnabled _WinIsWindowEnabled

inline BOOL _WinIsWindowVisible(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsWindowVisible(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsWindowVisible
#define WinIsWindowVisible _WinIsWindowVisible

inline LONG _WinLoadMessage(HAB a, HMODULE b, ULONG c, LONG d, PSZ e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadMessage(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadMessage
#define WinLoadMessage _WinLoadMessage

inline LONG _WinLoadString(HAB a, HMODULE b, ULONG c, LONG d, PSZ e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadString
#define WinLoadString _WinLoadString

inline LONG _WinMultWindowFromIDs(HWND a, PHWND b, ULONG c, ULONG d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMultWindowFromIDs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMultWindowFromIDs
#define WinMultWindowFromIDs _WinMultWindowFromIDs

inline HWND _WinQueryDesktopWindow(HAB a, HDC b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryDesktopWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryDesktopWindow
#define WinQueryDesktopWindow _WinQueryDesktopWindow

inline HWND _WinQueryObjectWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryObjectWindow(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryObjectWindow
#define WinQueryObjectWindow _WinQueryObjectWindow

inline HPOINTER _WinQueryPointer(HWND a)
{
 HPOINTER yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryPointer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryPointer
#define WinQueryPointer _WinQueryPointer

inline HWND _WinQueryWindow(HWND a, LONG b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindow
#define WinQueryWindow _WinQueryWindow

inline BOOL _WinQueryWindowPos(HWND a, PSWP b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowPos(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowPos
#define WinQueryWindowPos _WinQueryWindowPos

inline BOOL _WinQueryWindowProcess(HWND a, PPID b, PTID c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowProcess(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowProcess
#define WinQueryWindowProcess _WinQueryWindowProcess

inline LONG _WinQueryWindowText(HWND a, LONG b, PCH c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowText(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowText
#define WinQueryWindowText _WinQueryWindowText

inline LONG _WinQueryWindowTextLength(HWND a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowTextLength(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowTextLength
#define WinQueryWindowTextLength _WinQueryWindowTextLength

inline BOOL _WinSetMultWindowPos(HAB a, PSWP b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetMultWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetMultWindowPos
#define WinSetMultWindowPos _WinSetMultWindowPos

inline BOOL _WinSetOwner(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetOwner(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetOwner
#define WinSetOwner _WinSetOwner

inline BOOL _WinSetParent(HWND a, HWND b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetParent(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetParent
#define WinSetParent _WinSetParent

inline BOOL _WinSetWindowPos(HWND a, HWND b, LONG c, LONG d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetWindowPos(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetWindowPos
#define WinSetWindowPos _WinSetWindowPos

inline BOOL _WinSetWindowText(HWND a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetWindowText(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetWindowText
#define WinSetWindowText _WinSetWindowText

inline BOOL _WinUpdateWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinUpdateWindow(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinUpdateWindow
#define WinUpdateWindow _WinUpdateWindow

inline HWND _WinWindowFromID(HWND a, ULONG b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinWindowFromID(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinWindowFromID
#define WinWindowFromID _WinWindowFromID

#ifdef INCL_WINFRAMEMGR
inline HWND _WinCreateStdWindow(HWND a, ULONG b, PULONG c, PCSZ d, PCSZ e, ULONG f, HMODULE g, ULONG h, PHWND i)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateStdWindow(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateStdWindow
#define WinCreateStdWindow _WinCreateStdWindow

inline BOOL _WinCalcFrameRect(HWND a, PRECTL b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCalcFrameRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCalcFrameRect
#define WinCalcFrameRect _WinCalcFrameRect

inline BOOL _WinCreateFrameControls(HWND a, PFRAMECDATA b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateFrameControls(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateFrameControls
#define WinCreateFrameControls _WinCreateFrameControls

inline BOOL _WinFlashWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFlashWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFlashWindow
#define WinFlashWindow _WinFlashWindow

inline BOOL _WinGetMaxPosition(HWND a, PSWP b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetMaxPosition(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetMaxPosition
#define WinGetMaxPosition _WinGetMaxPosition

inline BOOL _WinGetMinPosition(HWND a, PSWP b, PPOINTL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetMinPosition(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetMinPosition
#define WinGetMinPosition _WinGetMinPosition

inline BOOL _WinSaveWindowPos(HSAVEWP a, PSWP b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSaveWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSaveWindowPos
#define WinSaveWindowPos _WinSaveWindowPos

#endif
#ifdef INCL_WINWINDOWMGR
inline HPS _WinBeginPaint(HWND a, HPS b, PRECTL c)
{
 HPS yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinBeginPaint(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinBeginPaint
#define WinBeginPaint _WinBeginPaint

inline MRESULT _WinDefWindowProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDefWindowProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDefWindowProc
#define WinDefWindowProc _WinDefWindowProc

inline BOOL _WinDestroyWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyWindow(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyWindow
#define WinDestroyWindow _WinDestroyWindow

inline BOOL _WinEndPaint(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEndPaint(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEndPaint
#define WinEndPaint _WinEndPaint

inline BOOL _WinFillRect(HPS a, PRECTL b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFillRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFillRect
#define WinFillRect _WinFillRect

inline HPS _WinGetClipPS(HWND a, HWND b, ULONG c)
{
 HPS yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetClipPS(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetClipPS
#define WinGetClipPS _WinGetClipPS

inline HPS _WinGetPS(HWND a)
{
 HPS yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetPS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetPS
#define WinGetPS _WinGetPS

inline HAB _WinInitialize(ULONG a)
{
 HAB yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinInitialize(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinInitialize
#define WinInitialize _WinInitialize

inline BOOL _WinIsWindowShowing(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsWindowShowing(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsWindowShowing
#define WinIsWindowShowing _WinIsWindowShowing

inline HDC _WinOpenWindowDC(HWND a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinOpenWindowDC(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinOpenWindowDC
#define WinOpenWindowDC _WinOpenWindowDC

inline HAB _WinQueryAnchorBlock(HWND a)
{
 HAB yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryAnchorBlock(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryAnchorBlock
#define WinQueryAnchorBlock _WinQueryAnchorBlock

inline ULONG _WinQueryVersion(HAB a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryVersion(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryVersion
#define WinQueryVersion _WinQueryVersion

inline BOOL _WinQueryWindowRect(HWND a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowRect(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowRect
#define WinQueryWindowRect _WinQueryWindowRect

inline BOOL _WinRegisterClass(HAB a, PCSZ b, PFNWP c, ULONG d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRegisterClass(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRegisterClass
#define WinRegisterClass _WinRegisterClass

inline BOOL _WinReleasePS(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinReleasePS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinReleasePS
#define WinReleasePS _WinReleasePS

inline LONG _WinScrollWindow(HWND a, LONG b, LONG c, PRECTL d, PRECTL e, HRGN f, PRECTL g, ULONG h)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinScrollWindow(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  WinScrollWindow
#define WinScrollWindow _WinScrollWindow

inline BOOL _WinSetActiveWindow(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetActiveWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetActiveWindow
#define WinSetActiveWindow _WinSetActiveWindow

inline BOOL _WinShowWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinShowWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinShowWindow
#define WinShowWindow _WinShowWindow

inline BOOL _WinTerminate(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinTerminate(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinTerminate
#define WinTerminate _WinTerminate

#endif
#ifdef INCL_WINWINDOWMGR
inline HENUM _WinBeginEnumWindows(HWND a)
{
 HENUM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinBeginEnumWindows(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinBeginEnumWindows
#define WinBeginEnumWindows _WinBeginEnumWindows

inline BOOL _WinEndEnumWindows(HENUM a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEndEnumWindows(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEndEnumWindows
#define WinEndEnumWindows _WinEndEnumWindows

inline LONG _WinExcludeUpdateRegion(HPS a, HWND b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinExcludeUpdateRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinExcludeUpdateRegion
#define WinExcludeUpdateRegion _WinExcludeUpdateRegion

inline HWND _WinGetNextWindow(HENUM a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetNextWindow(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetNextWindow
#define WinGetNextWindow _WinGetNextWindow

inline HPS _WinGetScreenPS(HWND a)
{
 HPS yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetScreenPS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetScreenPS
#define WinGetScreenPS _WinGetScreenPS

inline BOOL _WinIsThreadActive(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsThreadActive(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsThreadActive
#define WinIsThreadActive _WinIsThreadActive

inline BOOL _WinLockVisRegions(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLockVisRegions(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLockVisRegions
#define WinLockVisRegions _WinLockVisRegions

inline BOOL _WinLockWindowUpdate(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLockWindowUpdate(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLockWindowUpdate
#define WinLockWindowUpdate _WinLockWindowUpdate

inline BOOL _WinMapWindowPoints(HWND a, HWND b, PPOINTL c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMapWindowPoints(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMapWindowPoints
#define WinMapWindowPoints _WinMapWindowPoints

inline HWND _WinQueryActiveWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryActiveWindow(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryActiveWindow
#define WinQueryActiveWindow _WinQueryActiveWindow

inline BOOL _WinQueryClassInfo(HAB a, PCSZ b, PCLASSINFO c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryClassInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryClassInfo
#define WinQueryClassInfo _WinQueryClassInfo

inline LONG _WinQueryClassName(HWND a, LONG b, PCH c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryClassName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryClassName
#define WinQueryClassName _WinQueryClassName

inline BOOL _WinQueryUpdateRect(HWND a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryUpdateRect(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryUpdateRect
#define WinQueryUpdateRect _WinQueryUpdateRect

inline LONG _WinQueryUpdateRegion(HWND a, HRGN b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryUpdateRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryUpdateRegion
#define WinQueryUpdateRegion _WinQueryUpdateRegion

inline HWND _WinQuerySysModalWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySysModalWindow(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySysModalWindow
#define WinQuerySysModalWindow _WinQuerySysModalWindow

inline HDC _WinQueryWindowDC(HWND a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowDC(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowDC
#define WinQueryWindowDC _WinQueryWindowDC

inline PVOID _WinQueryWindowPtr(HWND a, LONG b)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowPtr(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowPtr
#define WinQueryWindowPtr _WinQueryWindowPtr

inline ULONG _WinQueryWindowULong(HWND a, LONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowULong(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowULong
#define WinQueryWindowULong _WinQueryWindowULong

inline USHORT _WinQueryWindowUShort(HWND a, LONG b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowUShort(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowUShort
#define WinQueryWindowUShort _WinQueryWindowUShort

inline BOOL _WinSetSysModalWindow(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetSysModalWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetSysModalWindow
#define WinSetSysModalWindow _WinSetSysModalWindow

inline BOOL _WinSetWindowBits(HWND a, LONG b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetWindowBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetWindowBits
#define WinSetWindowBits _WinSetWindowBits

inline BOOL _WinSetWindowPtr(HWND a, LONG b, PVOID c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetWindowPtr(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetWindowPtr
#define WinSetWindowPtr _WinSetWindowPtr

inline BOOL _WinSetWindowULong(HWND a, LONG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetWindowULong(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetWindowULong
#define WinSetWindowULong _WinSetWindowULong

inline BOOL _WinSetWindowUShort(HWND a, LONG b, USHORT c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetWindowUShort(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetWindowUShort
#define WinSetWindowUShort _WinSetWindowUShort

inline PFNWP _WinSubclassWindow(HWND a, PFNWP b)
{
 PFNWP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSubclassWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSubclassWindow
#define WinSubclassWindow _WinSubclassWindow

inline BOOL _WinValidateRect(HWND a, PRECTL b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinValidateRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinValidateRect
#define WinValidateRect _WinValidateRect

inline BOOL _WinValidateRegion(HWND a, HRGN b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinValidateRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinValidateRegion
#define WinValidateRegion _WinValidateRegion

inline HWND _WinWindowFromDC(HDC a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinWindowFromDC(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinWindowFromDC
#define WinWindowFromDC _WinWindowFromDC

inline HWND _WinWindowFromPoint(HWND a, PPOINTL b, BOOL c)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinWindowFromPoint(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinWindowFromPoint
#define WinWindowFromPoint _WinWindowFromPoint

#endif
#ifdef INCL_WINACCELERATORS
inline ULONG _WinCopyAccelTable(HACCEL a, PACCELTABLE b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCopyAccelTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCopyAccelTable
#define WinCopyAccelTable _WinCopyAccelTable

inline HACCEL _WinCreateAccelTable(HAB a, PACCELTABLE b)
{
 HACCEL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateAccelTable(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateAccelTable
#define WinCreateAccelTable _WinCreateAccelTable

inline BOOL _WinDestroyAccelTable(HACCEL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyAccelTable(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyAccelTable
#define WinDestroyAccelTable _WinDestroyAccelTable

inline HACCEL _WinLoadAccelTable(HAB a, HMODULE b, ULONG c)
{
 HACCEL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadAccelTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadAccelTable
#define WinLoadAccelTable _WinLoadAccelTable

inline HACCEL _WinQueryAccelTable(HAB a, HWND b)
{
 HACCEL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryAccelTable(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryAccelTable
#define WinQueryAccelTable _WinQueryAccelTable

inline BOOL _WinSetAccelTable(HAB a, HACCEL b, HWND c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetAccelTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetAccelTable
#define WinSetAccelTable _WinSetAccelTable

inline BOOL _WinTranslateAccel(HAB a, HWND b, HACCEL c, PQMSG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinTranslateAccel(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinTranslateAccel
#define WinTranslateAccel _WinTranslateAccel

#endif
#ifdef INCL_WINATOM
inline ATOM _WinAddAtom(HATOMTBL a, PCSZ b)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinAddAtom(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinAddAtom
#define WinAddAtom _WinAddAtom

inline HATOMTBL _WinCreateAtomTable(ULONG a, ULONG b)
{
 HATOMTBL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateAtomTable(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateAtomTable
#define WinCreateAtomTable _WinCreateAtomTable

inline ATOM _WinDeleteAtom(HATOMTBL a, ATOM b)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDeleteAtom(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDeleteAtom
#define WinDeleteAtom _WinDeleteAtom

inline HATOMTBL _WinDestroyAtomTable(HATOMTBL a)
{
 HATOMTBL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyAtomTable(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyAtomTable
#define WinDestroyAtomTable _WinDestroyAtomTable

inline ATOM _WinFindAtom(HATOMTBL a, PCSZ b)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFindAtom(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFindAtom
#define WinFindAtom _WinFindAtom

inline ULONG _WinQueryAtomLength(HATOMTBL a, ATOM b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryAtomLength(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryAtomLength
#define WinQueryAtomLength _WinQueryAtomLength

inline ULONG _WinQueryAtomName(HATOMTBL a, ATOM b, PSZ c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryAtomName(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryAtomName
#define WinQueryAtomName _WinQueryAtomName

inline ULONG _WinQueryAtomUsage(HATOMTBL a, ATOM b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryAtomUsage(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryAtomUsage
#define WinQueryAtomUsage _WinQueryAtomUsage

inline HATOMTBL _WinQuerySystemAtomTable()
{
 HATOMTBL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySystemAtomTable();
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySystemAtomTable
#define WinQuerySystemAtomTable _WinQuerySystemAtomTable

#endif
#ifdef INCL_WINCLIPBOARD
inline BOOL _WinCloseClipbrd(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCloseClipbrd(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCloseClipbrd
#define WinCloseClipbrd _WinCloseClipbrd

inline BOOL _WinEmptyClipbrd(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEmptyClipbrd(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEmptyClipbrd
#define WinEmptyClipbrd _WinEmptyClipbrd

inline ULONG _WinEnumClipbrdFmts(HAB a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEnumClipbrdFmts(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEnumClipbrdFmts
#define WinEnumClipbrdFmts _WinEnumClipbrdFmts

inline BOOL _WinOpenClipbrd(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinOpenClipbrd(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinOpenClipbrd
#define WinOpenClipbrd _WinOpenClipbrd

inline ULONG _WinQueryClipbrdData(HAB a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryClipbrdData(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryClipbrdData
#define WinQueryClipbrdData _WinQueryClipbrdData

inline BOOL _WinQueryClipbrdFmtInfo(HAB a, ULONG b, PULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryClipbrdFmtInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryClipbrdFmtInfo
#define WinQueryClipbrdFmtInfo _WinQueryClipbrdFmtInfo

inline HWND _WinQueryClipbrdOwner(HAB a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryClipbrdOwner(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryClipbrdOwner
#define WinQueryClipbrdOwner _WinQueryClipbrdOwner

inline HWND _WinQueryClipbrdViewer(HAB a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryClipbrdViewer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryClipbrdViewer
#define WinQueryClipbrdViewer _WinQueryClipbrdViewer

inline BOOL _WinSetClipbrdData(HAB a, ULONG b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetClipbrdData(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetClipbrdData
#define WinSetClipbrdData _WinSetClipbrdData

inline BOOL _WinSetClipbrdOwner(HAB a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetClipbrdOwner(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetClipbrdOwner
#define WinSetClipbrdOwner _WinSetClipbrdOwner

inline BOOL _WinSetClipbrdViewer(HAB a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetClipbrdViewer(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetClipbrdViewer
#define WinSetClipbrdViewer _WinSetClipbrdViewer

#endif
#ifdef INCL_WINDDE
inline BOOL _WinDdeInitiate(HWND a, PCSZ b, PCSZ c, PCONVCONTEXT d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDdeInitiate(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDdeInitiate
#define WinDdeInitiate _WinDdeInitiate

inline BOOL _WinDdePostMsg(HWND a, HWND b, ULONG c, PDDESTRUCT d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDdePostMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDdePostMsg
#define WinDdePostMsg _WinDdePostMsg

inline MRESULT _WinDdeRespond(HWND a, HWND b, PCSZ c, PCSZ d, PCONVCONTEXT e)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDdeRespond(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDdeRespond
#define WinDdeRespond _WinDdeRespond

#endif
#ifdef INCL_WINCOUNTRY
inline ULONG _WinCompareStrings(HAB a, ULONG b, ULONG c, PCSZ d, PCSZ e, ULONG f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCompareStrings(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCompareStrings
#define WinCompareStrings _WinCompareStrings

inline UCHAR _WinCpTranslateChar(HAB a, ULONG b, UCHAR c, ULONG d)
{
 UCHAR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCpTranslateChar(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCpTranslateChar
#define WinCpTranslateChar _WinCpTranslateChar

inline BOOL _WinCpTranslateString(HAB a, ULONG b, PCSZ c, ULONG d, ULONG e, PSZ f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCpTranslateString(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCpTranslateString
#define WinCpTranslateString _WinCpTranslateString

inline PCSZ _WinNextChar(HAB a, ULONG b, ULONG c, PCSZ d)
{
 PCSZ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinNextChar(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinNextChar
#define WinNextChar _WinNextChar

inline PCSZ _WinPrevChar(HAB a, ULONG b, ULONG c, PCSZ d, PCSZ e)
{
 PCSZ yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinPrevChar(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinPrevChar
#define WinPrevChar _WinPrevChar

inline ULONG _WinQueryCp(HMQ a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryCp(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryCp
#define WinQueryCp _WinQueryCp

inline ULONG _WinQueryCpList(HAB a, ULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryCpList(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryCpList
#define WinQueryCpList _WinQueryCpList

inline BOOL _WinSetCp(HMQ a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetCp(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetCp
#define WinSetCp _WinSetCp

inline ULONG _WinUpper(HAB a, ULONG b, ULONG c, PSZ d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinUpper(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinUpper
#define WinUpper _WinUpper

inline ULONG _WinUpperChar(HAB a, ULONG b, ULONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinUpperChar(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinUpperChar
#define WinUpperChar _WinUpperChar

#endif
#ifdef INCL_WINCURSORS
inline BOOL _WinCreateCursor(HWND a, LONG b, LONG c, LONG d, LONG e, ULONG f, PRECTL g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateCursor(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateCursor
#define WinCreateCursor _WinCreateCursor

inline BOOL _WinDestroyCursor(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyCursor(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyCursor
#define WinDestroyCursor _WinDestroyCursor

inline BOOL _WinShowCursor(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinShowCursor(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinShowCursor
#define WinShowCursor _WinShowCursor

inline BOOL _WinQueryCursorInfo(HWND a, PCURSORINFO b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryCursorInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryCursorInfo
#define WinQueryCursorInfo _WinQueryCursorInfo

#endif
#ifdef INCL_WINDESKTOP
inline BOOL _WinQueryDesktopBkgnd(HWND a, PDESKTOP b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryDesktopBkgnd(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryDesktopBkgnd
#define WinQueryDesktopBkgnd _WinQueryDesktopBkgnd

inline HBITMAP _WinSetDesktopBkgnd(HWND a, PDESKTOP b)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetDesktopBkgnd(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetDesktopBkgnd
#define WinSetDesktopBkgnd _WinSetDesktopBkgnd

#endif
#ifdef INCL_WINDIALOGS
inline BOOL _WinAlarm(HWND a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinAlarm(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinAlarm
#define WinAlarm _WinAlarm

inline MRESULT _WinDefDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDefDlgProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDefDlgProc
#define WinDefDlgProc _WinDefDlgProc

inline BOOL _WinDismissDlg(HWND a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDismissDlg(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDismissDlg
#define WinDismissDlg _WinDismissDlg

inline ULONG _WinDlgBox(HWND a, HWND b, PFNWP c, HMODULE d, ULONG e, PVOID f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDlgBox(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDlgBox
#define WinDlgBox _WinDlgBox

inline BOOL _WinGetDlgMsg(HWND a, PQMSG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetDlgMsg(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetDlgMsg
#define WinGetDlgMsg _WinGetDlgMsg

inline HWND _WinLoadDlg(HWND a, HWND b, PFNWP c, HMODULE d, ULONG e, PVOID f)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadDlg(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadDlg
#define WinLoadDlg _WinLoadDlg

inline ULONG _WinMessageBox(HWND a, HWND b, PCSZ c, PCSZ d, ULONG e, ULONG f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMessageBox(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMessageBox
#define WinMessageBox _WinMessageBox

inline ULONG _WinMessageBox2(HWND a, HWND b, PSZ c, PSZ d, ULONG e, PMB2INFO f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMessageBox2(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMessageBox2
#define WinMessageBox2 _WinMessageBox2

inline BOOL _WinQueryDlgItemShort(HWND a, ULONG b, PSHORT c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryDlgItemShort(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryDlgItemShort
#define WinQueryDlgItemShort _WinQueryDlgItemShort

inline ULONG _WinQueryDlgItemText(HWND a, ULONG b, LONG c, PSZ d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryDlgItemText(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryDlgItemText
#define WinQueryDlgItemText _WinQueryDlgItemText

inline LONG _WinQueryDlgItemTextLength(HWND a, ULONG b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryDlgItemTextLength(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryDlgItemTextLength
#define WinQueryDlgItemTextLength _WinQueryDlgItemTextLength

inline BOOL _WinSetDlgItemShort(HWND a, ULONG b, USHORT c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetDlgItemShort(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetDlgItemShort
#define WinSetDlgItemShort _WinSetDlgItemShort

inline BOOL _WinSetDlgItemText(HWND a, ULONG b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetDlgItemText(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetDlgItemText
#define WinSetDlgItemText _WinSetDlgItemText

inline HWND _WinCreateDlg(HWND a, HWND b, PFNWP c, PDLGTEMPLATE d, PVOID e)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateDlg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateDlg
#define WinCreateDlg _WinCreateDlg

inline HWND _WinEnumDlgItem(HWND a, HWND b, ULONG c)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEnumDlgItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEnumDlgItem
#define WinEnumDlgItem _WinEnumDlgItem

inline BOOL _WinMapDlgPoints(HWND a, PPOINTL b, ULONG c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMapDlgPoints(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMapDlgPoints
#define WinMapDlgPoints _WinMapDlgPoints

inline ULONG _WinProcessDlg(HWND a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinProcessDlg(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinProcessDlg
#define WinProcessDlg _WinProcessDlg

inline MRESULT _WinSendDlgItemMsg(HWND a, ULONG b, ULONG c, MPARAM d, MPARAM e)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSendDlgItemMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSendDlgItemMsg
#define WinSendDlgItemMsg _WinSendDlgItemMsg

inline LONG _WinSubstituteStrings(HWND a, PCSZ b, LONG c, PSZ d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSubstituteStrings(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSubstituteStrings
#define WinSubstituteStrings _WinSubstituteStrings

#endif
#ifdef INCL_WINERRORS
inline ERRORID _WinGetLastError(HAB a)
{
 ERRORID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetLastError(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetLastError
#define WinGetLastError _WinGetLastError

inline BOOL _WinFreeErrorInfo(PERRINFO a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFreeErrorInfo(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFreeErrorInfo
#define WinFreeErrorInfo _WinFreeErrorInfo

inline PERRINFO _WinGetErrorInfo(HAB a)
{
 PERRINFO yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetErrorInfo(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetErrorInfo
#define WinGetErrorInfo _WinGetErrorInfo

#endif
#ifdef INCL_WINHOOKS
inline BOOL _WinCallMsgFilter(HAB a, PQMSG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCallMsgFilter(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCallMsgFilter
#define WinCallMsgFilter _WinCallMsgFilter

inline BOOL _WinReleaseHook(HAB a, HMQ b, LONG c, PFN d, HMODULE e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinReleaseHook(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinReleaseHook
#define WinReleaseHook _WinReleaseHook

inline BOOL _WinSetHook(HAB a, HMQ b, LONG c, PFN d, HMODULE e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetHook(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetHook
#define WinSetHook _WinSetHook

#endif
#ifdef INCL_WININPUT
inline BOOL _WinFocusChange(HWND a, HWND b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFocusChange(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFocusChange
#define WinFocusChange _WinFocusChange

inline BOOL _WinLockupSystem(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLockupSystem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLockupSystem
#define WinLockupSystem _WinLockupSystem

inline BOOL _WinSetFocus(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetFocus(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetFocus
#define WinSetFocus _WinSetFocus

inline BOOL _WinUnlockSystem(HAB a, PSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinUnlockSystem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinUnlockSystem
#define WinUnlockSystem _WinUnlockSystem

inline BOOL _WinCheckInput(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCheckInput(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCheckInput
#define WinCheckInput _WinCheckInput

inline BOOL _WinEnablePhysInput(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEnablePhysInput(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEnablePhysInput
#define WinEnablePhysInput _WinEnablePhysInput

inline LONG _WinGetKeyState(HWND a, LONG b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetKeyState(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetKeyState
#define WinGetKeyState _WinGetKeyState

inline LONG _WinGetPhysKeyState(HWND a, LONG b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetPhysKeyState(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetPhysKeyState
#define WinGetPhysKeyState _WinGetPhysKeyState

inline BOOL _WinIsPhysInputEnabled(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsPhysInputEnabled(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsPhysInputEnabled
#define WinIsPhysInputEnabled _WinIsPhysInputEnabled

inline HWND _WinQueryCapture(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryCapture(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryCapture
#define WinQueryCapture _WinQueryCapture

inline HWND _WinQueryFocus(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryFocus(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryFocus
#define WinQueryFocus _WinQueryFocus

inline ULONG _WinQueryVisibleRegion(HWND a, HRGN b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryVisibleRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryVisibleRegion
#define WinQueryVisibleRegion _WinQueryVisibleRegion

inline BOOL _WinSetCapture(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetCapture(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetCapture
#define WinSetCapture _WinSetCapture

inline BOOL _WinSetKeyboardStateTable(HWND a, PBYTE b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetKeyboardStateTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetKeyboardStateTable
#define WinSetKeyboardStateTable _WinSetKeyboardStateTable

inline BOOL _WinSetVisibleRegionNotify(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetVisibleRegionNotify(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetVisibleRegionNotify
#define WinSetVisibleRegionNotify _WinSetVisibleRegionNotify

#endif
#ifdef INCL_WINLOAD
inline BOOL _WinDeleteLibrary(HAB a, HLIB b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDeleteLibrary(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDeleteLibrary
#define WinDeleteLibrary _WinDeleteLibrary

inline BOOL _WinDeleteProcedure(HAB a, PFNWP b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDeleteProcedure(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDeleteProcedure
#define WinDeleteProcedure _WinDeleteProcedure

inline HLIB _WinLoadLibrary(HAB a, PCSZ b)
{
 HLIB yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadLibrary(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadLibrary
#define WinLoadLibrary _WinLoadLibrary

inline PFNWP _WinLoadProcedure(HAB a, HLIB b, PSZ c)
{
 PFNWP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadProcedure(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadProcedure
#define WinLoadProcedure _WinLoadProcedure

#endif
#ifdef INCL_WINMENUS
inline HWND _WinCreateMenu(HWND a, PVOID b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateMenu(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateMenu
#define WinCreateMenu _WinCreateMenu

inline HWND _WinLoadMenu(HWND a, HMODULE b, ULONG c)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadMenu(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadMenu
#define WinLoadMenu _WinLoadMenu

inline BOOL _WinPopupMenu(HWND a, HWND b, HWND c, LONG d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinPopupMenu(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinPopupMenu
#define WinPopupMenu _WinPopupMenu

#endif
#ifdef INCL_WINMESSAGEMGR
inline BOOL _WinBroadcastMsg(HWND a, ULONG b, MPARAM c, MPARAM d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinBroadcastMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinBroadcastMsg
#define WinBroadcastMsg _WinBroadcastMsg

inline BOOL _WinInSendMsg(HAB a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinInSendMsg(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinInSendMsg
#define WinInSendMsg _WinInSendMsg

inline BOOL _WinPostQueueMsg(HMQ a, ULONG b, MPARAM c, MPARAM d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinPostQueueMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinPostQueueMsg
#define WinPostQueueMsg _WinPostQueueMsg

inline BOOL _WinQueryMsgPos(HAB a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryMsgPos(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryMsgPos
#define WinQueryMsgPos _WinQueryMsgPos

inline ULONG _WinQueryMsgTime(HAB a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryMsgTime(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryMsgTime
#define WinQueryMsgTime _WinQueryMsgTime

inline ULONG _WinQueryQueueStatus(HWND a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryQueueStatus(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryQueueStatus
#define WinQueryQueueStatus _WinQueryQueueStatus

inline ULONG _WinRequestMutexSem(HMTX a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRequestMutexSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRequestMutexSem
#define WinRequestMutexSem _WinRequestMutexSem

inline BOOL _WinSetClassMsgInterest(HAB a, PCSZ b, ULONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetClassMsgInterest(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetClassMsgInterest
#define WinSetClassMsgInterest _WinSetClassMsgInterest

inline BOOL _WinSetMsgInterest(HWND a, ULONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetMsgInterest(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetMsgInterest
#define WinSetMsgInterest _WinSetMsgInterest

inline ULONG _WinWaitEventSem(HEV a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinWaitEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinWaitEventSem
#define WinWaitEventSem _WinWaitEventSem

inline BOOL _WinWaitMsg(HAB a, ULONG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinWaitMsg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinWaitMsg
#define WinWaitMsg _WinWaitMsg

inline ULONG _WinWaitMuxWaitSem(HMUX a, ULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinWaitMuxWaitSem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinWaitMuxWaitSem
#define WinWaitMuxWaitSem _WinWaitMuxWaitSem

#endif
#ifdef INCL_WINPALETTE
inline LONG _WinRealizePalette(HWND a, HPS b, PULONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRealizePalette(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRealizePalette
#define WinRealizePalette _WinRealizePalette

#endif
#ifdef INCL_WINPOINTERS
inline HPOINTER _WinCreatePointer(HWND a, HBITMAP b, BOOL c, LONG d, LONG e)
{
 HPOINTER yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreatePointer(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreatePointer
#define WinCreatePointer _WinCreatePointer

inline HPOINTER _WinCreatePointerIndirect(HWND a, PPOINTERINFO b)
{
 HPOINTER yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreatePointerIndirect(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreatePointerIndirect
#define WinCreatePointerIndirect _WinCreatePointerIndirect

inline BOOL _WinDestroyPointer(HPOINTER a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyPointer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyPointer
#define WinDestroyPointer _WinDestroyPointer

inline BOOL _WinDrawPointer(HPS a, LONG b, LONG c, HPOINTER d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDrawPointer(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDrawPointer
#define WinDrawPointer _WinDrawPointer

inline HBITMAP _WinGetSysBitmap(HWND a, ULONG b)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetSysBitmap(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetSysBitmap
#define WinGetSysBitmap _WinGetSysBitmap

inline HPOINTER _WinLoadPointer(HWND a, HMODULE b, ULONG c)
{
 HPOINTER yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadPointer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadPointer
#define WinLoadPointer _WinLoadPointer

inline BOOL _WinLockPointerUpdate(HWND a, HPOINTER b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLockPointerUpdate(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLockPointerUpdate
#define WinLockPointerUpdate _WinLockPointerUpdate

inline BOOL _WinQueryPointerPos(HWND a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryPointerPos(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryPointerPos
#define WinQueryPointerPos _WinQueryPointerPos

inline BOOL _WinQueryPointerInfo(HPOINTER a, PPOINTERINFO b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryPointerInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryPointerInfo
#define WinQueryPointerInfo _WinQueryPointerInfo

inline HPOINTER _WinQuerySysPointer(HWND a, LONG b, BOOL c)
{
 HPOINTER yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySysPointer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySysPointer
#define WinQuerySysPointer _WinQuerySysPointer

inline BOOL _WinQuerySysPointerData(HWND a, ULONG b, PICONINFO c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySysPointerData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySysPointerData
#define WinQuerySysPointerData _WinQuerySysPointerData

inline BOOL _WinSetPointer(HWND a, HPOINTER b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetPointer
#define WinSetPointer _WinSetPointer

inline BOOL _WinSetPointerOwner(HPOINTER a, PID b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetPointerOwner(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetPointerOwner
#define WinSetPointerOwner _WinSetPointerOwner

inline BOOL _WinSetPointerPos(HWND a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetPointerPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetPointerPos
#define WinSetPointerPos _WinSetPointerPos

inline BOOL _WinSetSysPointerData(HWND a, ULONG b, PICONINFO c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetSysPointerData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetSysPointerData
#define WinSetSysPointerData _WinSetSysPointerData

inline BOOL _WinShowPointer(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinShowPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinShowPointer
#define WinShowPointer _WinShowPointer

#endif
#ifdef INCL_WINRECTANGLES
inline BOOL _WinCopyRect(HAB a, PRECTL b, PRECTL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCopyRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCopyRect
#define WinCopyRect _WinCopyRect

inline BOOL _WinEqualRect(HAB a, PRECTL b, PRECTL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEqualRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEqualRect
#define WinEqualRect _WinEqualRect

inline BOOL _WinInflateRect(HAB a, PRECTL b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinInflateRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinInflateRect
#define WinInflateRect _WinInflateRect

inline BOOL _WinIntersectRect(HAB a, PRECTL b, PRECTL c, PRECTL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIntersectRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIntersectRect
#define WinIntersectRect _WinIntersectRect

inline BOOL _WinIsRectEmpty(HAB a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsRectEmpty(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsRectEmpty
#define WinIsRectEmpty _WinIsRectEmpty

inline BOOL _WinMakePoints(HAB a, PPOINTL b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMakePoints(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMakePoints
#define WinMakePoints _WinMakePoints

inline BOOL _WinMakeRect(HAB a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMakeRect(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMakeRect
#define WinMakeRect _WinMakeRect

inline BOOL _WinOffsetRect(HAB a, PRECTL b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinOffsetRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinOffsetRect
#define WinOffsetRect _WinOffsetRect

inline BOOL _WinPtInRect(HAB a, PRECTL b, PPOINTL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinPtInRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinPtInRect
#define WinPtInRect _WinPtInRect

inline BOOL _WinSetRect(HAB a, PRECTL b, LONG c, LONG d, LONG e, LONG f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetRect(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetRect
#define WinSetRect _WinSetRect

inline BOOL _WinSetRectEmpty(HAB a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetRectEmpty(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetRectEmpty
#define WinSetRectEmpty _WinSetRectEmpty

inline BOOL _WinSubtractRect(HAB a, PRECTL b, PRECTL c, PRECTL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSubtractRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSubtractRect
#define WinSubtractRect _WinSubtractRect

inline BOOL _WinUnionRect(HAB a, PRECTL b, PRECTL c, PRECTL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinUnionRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinUnionRect
#define WinUnionRect _WinUnionRect

#endif
#ifdef INCL_WINSYS
inline LONG _WinQueryControlColors(HWND a, LONG b, ULONG c, ULONG d, PCTLCOLOR e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryControlColors(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryControlColors
#define WinQueryControlColors _WinQueryControlColors

inline ULONG _WinQueryPresParam(HWND a, ULONG b, ULONG c, PULONG d, ULONG e, PVOID f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryPresParam(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryPresParam
#define WinQueryPresParam _WinQueryPresParam

inline LONG _WinQuerySysColor(HWND a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySysColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySysColor
#define WinQuerySysColor _WinQuerySysColor

inline LONG _WinQuerySysValue(HWND a, LONG b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySysValue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySysValue
#define WinQuerySysValue _WinQuerySysValue

inline BOOL _WinRemovePresParam(HWND a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRemovePresParam(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRemovePresParam
#define WinRemovePresParam _WinRemovePresParam

inline LONG _WinSetControlColors(HWND a, LONG b, ULONG c, ULONG d, PCTLCOLOR e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetControlColors(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetControlColors
#define WinSetControlColors _WinSetControlColors

inline BOOL _WinSetPresParam(HWND a, ULONG b, ULONG c, PVOID d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetPresParam(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetPresParam
#define WinSetPresParam _WinSetPresParam

inline BOOL _WinSetSysColors(HWND a, ULONG b, ULONG c, LONG d, ULONG e, PLONG f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetSysColors(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetSysColors
#define WinSetSysColors _WinSetSysColors

inline BOOL _WinSetSysValue(HWND a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetSysValue(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetSysValue
#define WinSetSysValue _WinSetSysValue

#endif
#ifdef INCL_WINTHUNKAPI
inline PFN _WinQueryClassThunkProc(PCSZ a)
{
 PFN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryClassThunkProc(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryClassThunkProc
#define WinQueryClassThunkProc _WinQueryClassThunkProc

inline LONG _WinQueryWindowModel(HWND a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowModel(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowModel
#define WinQueryWindowModel _WinQueryWindowModel

inline PFN _WinQueryWindowThunkProc(HWND a)
{
 PFN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryWindowThunkProc(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryWindowThunkProc
#define WinQueryWindowThunkProc _WinQueryWindowThunkProc

inline BOOL _WinSetClassThunkProc(PCSZ a, PFN b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetClassThunkProc(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetClassThunkProc
#define WinSetClassThunkProc _WinSetClassThunkProc

inline BOOL _WinSetWindowThunkProc(HWND a, PFN b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetWindowThunkProc(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetWindowThunkProc
#define WinSetWindowThunkProc _WinSetWindowThunkProc

#endif
#ifdef INCL_WINTIMER
inline ULONG _WinGetCurrentTime(HAB a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetCurrentTime(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetCurrentTime
#define WinGetCurrentTime _WinGetCurrentTime

inline ULONG _WinStartTimer(HAB a, HWND b, ULONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinStartTimer(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinStartTimer
#define WinStartTimer _WinStartTimer

inline BOOL _WinStopTimer(HAB a, HWND b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinStopTimer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinStopTimer
#define WinStopTimer _WinStopTimer

#endif
#ifdef INCL_WINTRACKRECT
inline BOOL _WinShowTrackRect(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinShowTrackRect(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinShowTrackRect
#define WinShowTrackRect _WinShowTrackRect

inline BOOL _WinTrackRect(HWND a, HPS b, PTRACKINFO c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinTrackRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinTrackRect
#define WinTrackRect _WinTrackRect

#endif
#endif
#ifdef INCL_GPI
inline LONG _GpiAnimatePalette(HPAL a, ULONG b, ULONG c, ULONG d, PULONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiAnimatePalette(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiAnimatePalette
#define GpiAnimatePalette _GpiAnimatePalette

inline BOOL _GpiBeginArea(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiBeginArea(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiBeginArea
#define GpiBeginArea _GpiBeginArea

inline BOOL _GpiBeginElement(HPS a, LONG b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiBeginElement(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiBeginElement
#define GpiBeginElement _GpiBeginElement

inline BOOL _GpiBeginPath(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiBeginPath(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiBeginPath
#define GpiBeginPath _GpiBeginPath

inline LONG _GpiBox(HPS a, LONG b, PPOINTL c, LONG d, LONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiBox(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiBox
#define GpiBox _GpiBox

inline LONG _GpiCallSegmentMatrix(HPS a, LONG b, LONG c, PMATRIXLF d, LONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCallSegmentMatrix(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCallSegmentMatrix
#define GpiCallSegmentMatrix _GpiCallSegmentMatrix

inline LONG _GpiCharString(HPS a, LONG b, PCH c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCharString(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCharString
#define GpiCharString _GpiCharString

inline LONG _GpiCharStringAt(HPS a, PPOINTL b, LONG c, PCH d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCharStringAt(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCharStringAt
#define GpiCharStringAt _GpiCharStringAt

inline LONG _GpiCharStringPos(HPS a, PRECTL b, ULONG c, LONG d, PCH e, PLONG f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCharStringPos(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCharStringPos
#define GpiCharStringPos _GpiCharStringPos

inline LONG _GpiCharStringPosAt(HPS a, PPOINTL b, PRECTL c, ULONG d, LONG e, PCH f, PLONG g)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCharStringPosAt(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCharStringPosAt
#define GpiCharStringPosAt _GpiCharStringPosAt

inline BOOL _GpiCloseFigure(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCloseFigure(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCloseFigure
#define GpiCloseFigure _GpiCloseFigure

inline LONG _GpiCombineRegion(HPS a, HRGN b, HRGN c, HRGN d, LONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCombineRegion(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCombineRegion
#define GpiCombineRegion _GpiCombineRegion

inline BOOL _GpiComment(HPS a, LONG b, PBYTE c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiComment(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiComment
#define GpiComment _GpiComment

inline BOOL _GpiConvert(HPS a, LONG b, LONG c, LONG d, PPOINTL e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiConvert(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiConvert
#define GpiConvert _GpiConvert

inline BOOL _GpiConvertWithMatrix(HPS a, LONG b, PPOINTL c, LONG d, PMATRIXLF e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiConvertWithMatrix(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiConvertWithMatrix
#define GpiConvertWithMatrix _GpiConvertWithMatrix

inline HMF _GpiCopyMetaFile(HMF a)
{
 HMF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCopyMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCopyMetaFile
#define GpiCopyMetaFile _GpiCopyMetaFile

inline BOOL _GpiCreateLogColorTable(HPS a, ULONG b, LONG c, LONG d, LONG e, PLONG f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreateLogColorTable(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreateLogColorTable
#define GpiCreateLogColorTable _GpiCreateLogColorTable

inline LONG _GpiCreateLogFont(HPS a, STR8 *b, LONG c, PFATTRS d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreateLogFont(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreateLogFont
#define GpiCreateLogFont _GpiCreateLogFont

inline HPAL _GpiCreatePalette(HAB a, ULONG b, ULONG c, ULONG d, PULONG e)
{
 HPAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreatePalette(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreatePalette
#define GpiCreatePalette _GpiCreatePalette

inline HRGN _GpiCreateRegion(HPS a, LONG b, PRECTL c)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreateRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreateRegion
#define GpiCreateRegion _GpiCreateRegion

inline BOOL _GpiDeleteElement(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteElement(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteElement
#define GpiDeleteElement _GpiDeleteElement

inline BOOL _GpiDeleteElementRange(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteElementRange(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteElementRange
#define GpiDeleteElementRange _GpiDeleteElementRange

inline BOOL _GpiDeleteElementsBetweenLabels(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteElementsBetweenLabels(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteElementsBetweenLabels
#define GpiDeleteElementsBetweenLabels _GpiDeleteElementsBetweenLabels

inline BOOL _GpiDeleteMetaFile(HMF a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteMetaFile
#define GpiDeleteMetaFile _GpiDeleteMetaFile

inline BOOL _GpiDeletePalette(HPAL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeletePalette(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeletePalette
#define GpiDeletePalette _GpiDeletePalette

inline BOOL _GpiDeleteSetId(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteSetId(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteSetId
#define GpiDeleteSetId _GpiDeleteSetId

inline BOOL _GpiDestroyRegion(HPS a, HRGN b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDestroyRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDestroyRegion
#define GpiDestroyRegion _GpiDestroyRegion

inline LONG _GpiElement(HPS a, LONG b, PCSZ c, LONG d, PBYTE e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiElement(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiElement
#define GpiElement _GpiElement

inline LONG _GpiEndArea(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiEndArea(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiEndArea
#define GpiEndArea _GpiEndArea

inline BOOL _GpiEndElement(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiEndElement(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiEndElement
#define GpiEndElement _GpiEndElement

inline BOOL _GpiEndPath(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiEndPath(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiEndPath
#define GpiEndPath _GpiEndPath

inline LONG _GpiEqualRegion(HPS a, HRGN b, HRGN c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiEqualRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiEqualRegion
#define GpiEqualRegion _GpiEqualRegion

inline LONG _GpiExcludeClipRectangle(HPS a, PRECTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiExcludeClipRectangle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiExcludeClipRectangle
#define GpiExcludeClipRectangle _GpiExcludeClipRectangle

inline LONG _GpiFillPath(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiFillPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiFillPath
#define GpiFillPath _GpiFillPath

inline LONG _GpiFrameRegion(HPS a, HRGN b, PSIZEL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiFrameRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiFrameRegion
#define GpiFrameRegion _GpiFrameRegion

inline LONG _GpiFullArc(HPS a, LONG b, FIXED c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiFullArc(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiFullArc
#define GpiFullArc _GpiFullArc

inline LONG _GpiImage(HPS a, LONG b, PSIZEL c, LONG d, PBYTE e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiImage(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiImage
#define GpiImage _GpiImage

inline LONG _GpiIntersectClipRectangle(HPS a, PRECTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiIntersectClipRectangle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiIntersectClipRectangle
#define GpiIntersectClipRectangle _GpiIntersectClipRectangle

inline BOOL _GpiLabel(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiLabel(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiLabel
#define GpiLabel _GpiLabel

inline LONG _GpiLine(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiLine(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiLine
#define GpiLine _GpiLine

inline BOOL _GpiLoadFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiLoadFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiLoadFonts
#define GpiLoadFonts _GpiLoadFonts

inline HMF _GpiLoadMetaFile(HAB a, PCSZ b)
{
 HMF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiLoadMetaFile(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiLoadMetaFile
#define GpiLoadMetaFile _GpiLoadMetaFile

inline BOOL _GpiLoadPublicFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiLoadPublicFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiLoadPublicFonts
#define GpiLoadPublicFonts _GpiLoadPublicFonts

inline LONG _GpiMarker(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiMarker(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiMarker
#define GpiMarker _GpiMarker

inline BOOL _GpiModifyPath(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiModifyPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiModifyPath
#define GpiModifyPath _GpiModifyPath

inline BOOL _GpiMove(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiMove(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiMove
#define GpiMove _GpiMove

inline LONG _GpiOffsetClipRegion(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiOffsetClipRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiOffsetClipRegion
#define GpiOffsetClipRegion _GpiOffsetClipRegion

inline BOOL _GpiOffsetElementPointer(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiOffsetElementPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiOffsetElementPointer
#define GpiOffsetElementPointer _GpiOffsetElementPointer

inline BOOL _GpiOffsetRegion(HPS a, HRGN b, PPOINTL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiOffsetRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiOffsetRegion
#define GpiOffsetRegion _GpiOffsetRegion

inline LONG _GpiOutlinePath(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiOutlinePath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiOutlinePath
#define GpiOutlinePath _GpiOutlinePath

inline LONG _GpiPaintRegion(HPS a, HRGN b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPaintRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPaintRegion
#define GpiPaintRegion _GpiPaintRegion

inline LONG _GpiPartialArc(HPS a, PPOINTL b, FIXED c, FIXED d, FIXED e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPartialArc(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPartialArc
#define GpiPartialArc _GpiPartialArc

inline HRGN _GpiPathToRegion(HPS a, LONG b, LONG c)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPathToRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPathToRegion
#define GpiPathToRegion _GpiPathToRegion

inline LONG _GpiPlayMetaFile(HPS a, HMF b, LONG c, PLONG d, PLONG e, LONG f, PSZ g)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPlayMetaFile(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPlayMetaFile
#define GpiPlayMetaFile _GpiPlayMetaFile

inline LONG _GpiPointArc(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPointArc(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPointArc
#define GpiPointArc _GpiPointArc

inline LONG _GpiPolyFillet(HPS a, LONG b, PPOINTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPolyFillet(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPolyFillet
#define GpiPolyFillet _GpiPolyFillet

inline LONG _GpiPolyFilletSharp(HPS a, LONG b, PPOINTL c, PFIXED d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPolyFilletSharp(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPolyFilletSharp
#define GpiPolyFilletSharp _GpiPolyFilletSharp

inline LONG _GpiPolygons(HPS a, ULONG b, PPOLYGON c, ULONG d, ULONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPolygons(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HRGN _GpiCreatePolygonRegion(HPS a, ULONG b, PPOLYGON c, ULONG d)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreatePolygonRegion(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreatePolygonRegion
#define GpiCreatePolygonRegion _GpiCreatePolygonRegion

inline HRGN _GpiCreateEllipticRegion(HPS a, PRECTL b)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreateEllipticRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreateEllipticRegion
#define GpiCreateEllipticRegion _GpiCreateEllipticRegion

inline HRGN _GpiCreateRoundRectRegion(HPS a, PPOINTL b, LONG c, LONG d)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreateRoundRectRegion(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreateRoundRectRegion
#define GpiCreateRoundRectRegion _GpiCreateRoundRectRegion

#undef  GpiPolygons
#define GpiPolygons _GpiPolygons

inline LONG _GpiPolyLine(HPS a, LONG b, PPOINTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPolyLine(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPolyLine
#define GpiPolyLine _GpiPolyLine

inline LONG _GpiPolyLineDisjoint(HPS a, LONG b, PPOINTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPolyLineDisjoint(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPolyLineDisjoint
#define GpiPolyLineDisjoint _GpiPolyLineDisjoint

inline LONG _GpiPolyMarker(HPS a, LONG b, PPOINTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPolyMarker(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPolyMarker
#define GpiPolyMarker _GpiPolyMarker

inline LONG _GpiPolySpline(HPS a, LONG b, PPOINTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPolySpline(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPolySpline
#define GpiPolySpline _GpiPolySpline

inline BOOL _GpiPop(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPop(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPop
#define GpiPop _GpiPop

inline LONG _GpiPtInRegion(HPS a, HRGN b, PPOINTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPtInRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPtInRegion
#define GpiPtInRegion _GpiPtInRegion

inline LONG _GpiPtVisible(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPtVisible(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPtVisible
#define GpiPtVisible _GpiPtVisible

inline BOOL _GpiQueryArcParams(HPS a, PARCPARAMS b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryArcParams
#define GpiQueryArcParams _GpiQueryArcParams

inline LONG _GpiQueryAttrMode(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryAttrMode(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryAttrMode
#define GpiQueryAttrMode _GpiQueryAttrMode

inline LONG _GpiQueryAttrs(HPS a, LONG b, ULONG c, PBUNDLE d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryAttrs
#define GpiQueryAttrs _GpiQueryAttrs

inline LONG _GpiQueryBackColor(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBackColor(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBackColor
#define GpiQueryBackColor _GpiQueryBackColor

inline LONG _GpiQueryBackMix(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBackMix(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBackMix
#define GpiQueryBackMix _GpiQueryBackMix

inline BOOL _GpiQueryCharAngle(HPS a, PGRADIENTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharAngle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharAngle
#define GpiQueryCharAngle _GpiQueryCharAngle

inline BOOL _GpiQueryCharBox(HPS a, PSIZEF b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharBox(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharBox
#define GpiQueryCharBox _GpiQueryCharBox

inline BOOL _GpiQueryCharBreakExtra(HPS a, PFIXED b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharBreakExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharBreakExtra
#define GpiQueryCharBreakExtra _GpiQueryCharBreakExtra

inline LONG _GpiQueryCharDirection(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharDirection(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharDirection
#define GpiQueryCharDirection _GpiQueryCharDirection

inline BOOL _GpiQueryCharExtra(HPS a, PFIXED b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharExtra
#define GpiQueryCharExtra _GpiQueryCharExtra

inline LONG _GpiQueryCharMode(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharMode(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharMode
#define GpiQueryCharMode _GpiQueryCharMode

inline LONG _GpiQueryCharSet(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharSet(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharSet
#define GpiQueryCharSet _GpiQueryCharSet

inline BOOL _GpiQueryCharShear(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharShear(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharShear
#define GpiQueryCharShear _GpiQueryCharShear

inline BOOL _GpiQueryCharStringPos(HPS a, ULONG b, LONG c, PCH d, PLONG e, PPOINTL f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharStringPos(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharStringPos
#define GpiQueryCharStringPos _GpiQueryCharStringPos

inline BOOL _GpiQueryCharStringPosAt(HPS a, PPOINTL b, ULONG c, LONG d, PCH e, PLONG f, PPOINTL g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCharStringPosAt(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCharStringPosAt
#define GpiQueryCharStringPosAt _GpiQueryCharStringPosAt

inline LONG _GpiQueryClipBox(HPS a, PRECTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryClipBox(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryClipBox
#define GpiQueryClipBox _GpiQueryClipBox

inline HRGN _GpiQueryClipRegion(HPS a)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryClipRegion(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryClipRegion
#define GpiQueryClipRegion _GpiQueryClipRegion

inline LONG _GpiQueryColor(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryColor(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryColor
#define GpiQueryColor _GpiQueryColor

inline BOOL _GpiQueryColorData(HPS a, LONG b, PLONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryColorData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryColorData
#define GpiQueryColorData _GpiQueryColorData

inline LONG _GpiQueryColorIndex(HPS a, ULONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryColorIndex(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryColorIndex
#define GpiQueryColorIndex _GpiQueryColorIndex

inline ULONG _GpiQueryCp(HPS a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCp(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCp
#define GpiQueryCp _GpiQueryCp

inline BOOL _GpiQueryCurrentPosition(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryCurrentPosition(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryCurrentPosition
#define GpiQueryCurrentPosition _GpiQueryCurrentPosition

inline BOOL _GpiQueryDefArcParams(HPS a, PARCPARAMS b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDefArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDefArcParams
#define GpiQueryDefArcParams _GpiQueryDefArcParams

inline BOOL _GpiQueryDefAttrs(HPS a, LONG b, ULONG c, PBUNDLE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDefAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDefAttrs
#define GpiQueryDefAttrs _GpiQueryDefAttrs

inline BOOL _GpiQueryDefCharBox(HPS a, PSIZEL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDefCharBox(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDefCharBox
#define GpiQueryDefCharBox _GpiQueryDefCharBox

inline BOOL _GpiQueryDefTag(HPS a, PLONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDefTag(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDefTag
#define GpiQueryDefTag _GpiQueryDefTag

inline BOOL _GpiQueryDefViewingLimits(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDefViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDefViewingLimits
#define GpiQueryDefViewingLimits _GpiQueryDefViewingLimits

inline BOOL _GpiQueryDefaultViewMatrix(HPS a, LONG b, PMATRIXLF c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDefaultViewMatrix(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDefaultViewMatrix
#define GpiQueryDefaultViewMatrix _GpiQueryDefaultViewMatrix

inline LONG _GpiQueryEditMode(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryEditMode(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryEditMode
#define GpiQueryEditMode _GpiQueryEditMode

inline LONG _GpiQueryElement(HPS a, LONG b, LONG c, PBYTE d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryElement(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryElement
#define GpiQueryElement _GpiQueryElement

inline LONG _GpiQueryElementPointer(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryElementPointer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryElementPointer
#define GpiQueryElementPointer _GpiQueryElementPointer

inline LONG _GpiQueryElementType(HPS a, PLONG b, LONG c, PSZ d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryElementType(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryElementType
#define GpiQueryElementType _GpiQueryElementType

inline ULONG _GpiQueryFaceString(HPS a, PCSZ b, PFACENAMEDESC c, LONG d, PSZ e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryFaceString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryFaceString
#define GpiQueryFaceString _GpiQueryFaceString

inline ULONG _GpiQueryFontAction(HAB a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryFontAction(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryFontAction
#define GpiQueryFontAction _GpiQueryFontAction

inline LONG _GpiQueryFontFileDescriptions(HAB a, PCSZ b, PLONG c, PFFDESCS d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryFontFileDescriptions(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryFontFileDescriptions
#define GpiQueryFontFileDescriptions _GpiQueryFontFileDescriptions

inline BOOL _GpiQueryFontMetrics(HPS a, LONG b, PFONTMETRICS c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryFontMetrics(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryFontMetrics
#define GpiQueryFontMetrics _GpiQueryFontMetrics

inline LONG _GpiQueryFonts(HPS a, ULONG b, PCSZ c, PLONG d, LONG e, PFONTMETRICS f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryFonts(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryFonts
#define GpiQueryFonts _GpiQueryFonts

inline LONG _GpiQueryFullFontFileDescs(HAB a, PCSZ b, PLONG c, PVOID d, PLONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryFullFontFileDescs(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryFullFontFileDescs
#define GpiQueryFullFontFileDescs _GpiQueryFullFontFileDescs

inline BOOL _GpiQueryGraphicsField(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryGraphicsField(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryGraphicsField
#define GpiQueryGraphicsField _GpiQueryGraphicsField

inline LONG _GpiQueryKerningPairs(HPS a, LONG b, PKERNINGPAIRS c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryKerningPairs(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryKerningPairs
#define GpiQueryKerningPairs _GpiQueryKerningPairs

inline LONG _GpiQueryLineEnd(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryLineEnd(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryLineEnd
#define GpiQueryLineEnd _GpiQueryLineEnd

inline LONG _GpiQueryLineJoin(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryLineJoin(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryLineJoin
#define GpiQueryLineJoin _GpiQueryLineJoin

inline LONG _GpiQueryLineType(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryLineType(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryLineType
#define GpiQueryLineType _GpiQueryLineType

inline FIXED _GpiQueryLineWidth(HPS a)
{
 FIXED yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryLineWidth(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryLineWidth
#define GpiQueryLineWidth _GpiQueryLineWidth

inline LONG _GpiQueryLineWidthGeom(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryLineWidthGeom(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryLineWidthGeom
#define GpiQueryLineWidthGeom _GpiQueryLineWidthGeom

inline LONG _GpiQueryLogColorTable(HPS a, ULONG b, LONG c, LONG d, PLONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryLogColorTable(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryLogColorTable
#define GpiQueryLogColorTable _GpiQueryLogColorTable

inline BOOL _GpiQueryLogicalFont(HPS a, LONG b, PSTR8 c, PFATTRS d, LONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryLogicalFont(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryLogicalFont
#define GpiQueryLogicalFont _GpiQueryLogicalFont

inline LONG _GpiQueryMarker(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryMarker(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryMarker
#define GpiQueryMarker _GpiQueryMarker

inline BOOL _GpiQueryMarkerBox(HPS a, PSIZEF b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryMarkerBox(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryMarkerBox
#define GpiQueryMarkerBox _GpiQueryMarkerBox

inline LONG _GpiQueryMarkerSet(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryMarkerSet(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryMarkerSet
#define GpiQueryMarkerSet _GpiQueryMarkerSet

inline BOOL _GpiQueryMetaFileBits(HMF a, LONG b, LONG c, PBYTE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryMetaFileBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryMetaFileBits
#define GpiQueryMetaFileBits _GpiQueryMetaFileBits

inline LONG _GpiQueryMetaFileLength(HMF a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryMetaFileLength(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryMetaFileLength
#define GpiQueryMetaFileLength _GpiQueryMetaFileLength

inline LONG _GpiQueryMix(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryMix(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryMix
#define GpiQueryMix _GpiQueryMix

inline BOOL _GpiQueryModelTransformMatrix(HPS a, LONG b, PMATRIXLF c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryModelTransformMatrix(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryModelTransformMatrix
#define GpiQueryModelTransformMatrix _GpiQueryModelTransformMatrix

inline LONG _GpiQueryNearestColor(HPS a, ULONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryNearestColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryNearestColor
#define GpiQueryNearestColor _GpiQueryNearestColor

inline LONG _GpiQueryNumberSetIds(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryNumberSetIds(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryNumberSetIds
#define GpiQueryNumberSetIds _GpiQueryNumberSetIds

inline BOOL _GpiQueryPageViewport(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPageViewport(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPageViewport
#define GpiQueryPageViewport _GpiQueryPageViewport

inline HPAL _GpiQueryPalette(HPS a)
{
 HPAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPalette(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPalette
#define GpiQueryPalette _GpiQueryPalette

inline LONG _GpiQueryPaletteInfo(HPAL a, HPS b, ULONG c, ULONG d, ULONG e, PULONG f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPaletteInfo(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPaletteInfo
#define GpiQueryPaletteInfo _GpiQueryPaletteInfo

inline LONG _GpiQueryPattern(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPattern(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPattern
#define GpiQueryPattern _GpiQueryPattern

inline BOOL _GpiQueryPatternRefPoint(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPatternRefPoint(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPatternRefPoint
#define GpiQueryPatternRefPoint _GpiQueryPatternRefPoint

inline LONG _GpiQueryPatternSet(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPatternSet(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPatternSet
#define GpiQueryPatternSet _GpiQueryPatternSet

inline LONG _GpiQueryRealColors(HPS a, ULONG b, LONG c, LONG d, PLONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryRealColors(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryRealColors
#define GpiQueryRealColors _GpiQueryRealColors

inline LONG _GpiQueryRegionBox(HPS a, HRGN b, PRECTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryRegionBox(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryRegionBox
#define GpiQueryRegionBox _GpiQueryRegionBox

inline BOOL _GpiQueryRegionRects(HPS a, HRGN b, PRECTL c, PRGNRECT d, PRECTL e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryRegionRects(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryRegionRects
#define GpiQueryRegionRects _GpiQueryRegionRects

inline LONG _GpiQueryRGBColor(HPS a, ULONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryRGBColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryRGBColor
#define GpiQueryRGBColor _GpiQueryRGBColor

inline BOOL _GpiQuerySegmentTransformMatrix(HPS a, LONG b, LONG c, PMATRIXLF d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQuerySegmentTransformMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQuerySegmentTransformMatrix
#define GpiQuerySegmentTransformMatrix _GpiQuerySegmentTransformMatrix

inline BOOL _GpiQuerySetIds(HPS a, LONG b, PLONG c, PSTR8 d, PLONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQuerySetIds(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQuerySetIds
#define GpiQuerySetIds _GpiQuerySetIds

inline BOOL _GpiQueryTextAlignment(HPS a, PLONG b, PLONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryTextAlignment(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryTextAlignment
#define GpiQueryTextAlignment _GpiQueryTextAlignment

inline BOOL _GpiQueryTextBox(HPS a, LONG b, PCH c, LONG d, PPOINTL e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryTextBox(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryTextBox
#define GpiQueryTextBox _GpiQueryTextBox

inline BOOL _GpiQueryViewingLimits(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryViewingLimits
#define GpiQueryViewingLimits _GpiQueryViewingLimits

inline BOOL _GpiQueryViewingTransformMatrix(HPS a, LONG b, PMATRIXLF c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryViewingTransformMatrix(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryViewingTransformMatrix
#define GpiQueryViewingTransformMatrix _GpiQueryViewingTransformMatrix

inline BOOL _GpiQueryWidthTable(HPS a, LONG b, LONG c, PLONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryWidthTable(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryWidthTable
#define GpiQueryWidthTable _GpiQueryWidthTable

inline LONG _GpiRectInRegion(HPS a, HRGN b, PRECTL c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiRectInRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiRectInRegion
#define GpiRectInRegion _GpiRectInRegion

inline LONG _GpiRectVisible(HPS a, PRECTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiRectVisible(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiRectVisible
#define GpiRectVisible _GpiRectVisible

inline BOOL _GpiRotate(HPS a, PMATRIXLF b, LONG c, FIXED d, PPOINTL e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiRotate(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiRotate
#define GpiRotate _GpiRotate

inline BOOL _GpiSaveMetaFile(HMF a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSaveMetaFile(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSaveMetaFile
#define GpiSaveMetaFile _GpiSaveMetaFile

inline BOOL _GpiScale(HPS a, PMATRIXLF b, LONG c, PFIXED d, PPOINTL e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiScale(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiScale
#define GpiScale _GpiScale

inline HPAL _GpiSelectPalette(HPS a, HPAL b)
{
 HPAL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSelectPalette(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSelectPalette
#define GpiSelectPalette _GpiSelectPalette

inline BOOL _GpiSetArcParams(HPS a, PARCPARAMS b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetArcParams
#define GpiSetArcParams _GpiSetArcParams

inline BOOL _GpiSetAttrMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetAttrMode(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetAttrMode
#define GpiSetAttrMode _GpiSetAttrMode

inline BOOL _GpiSetAttrs(HPS a, LONG b, ULONG c, ULONG d, PVOID e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetAttrs(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetAttrs
#define GpiSetAttrs _GpiSetAttrs

inline BOOL _GpiSetBackColor(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetBackColor(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetBackColor
#define GpiSetBackColor _GpiSetBackColor

inline BOOL _GpiSetBackMix(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetBackMix(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetBackMix
#define GpiSetBackMix _GpiSetBackMix

inline BOOL _GpiSetCharAngle(HPS a, PGRADIENTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharAngle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharAngle
#define GpiSetCharAngle _GpiSetCharAngle

inline BOOL _GpiSetCharBox(HPS a, PSIZEF b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharBox(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharBox
#define GpiSetCharBox _GpiSetCharBox

inline BOOL _GpiSetCharBreakExtra(HPS a, FIXED b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharBreakExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharBreakExtra
#define GpiSetCharBreakExtra _GpiSetCharBreakExtra

inline BOOL _GpiSetCharDirection(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharDirection(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharDirection
#define GpiSetCharDirection _GpiSetCharDirection

inline BOOL _GpiSetCharExtra(HPS a, FIXED b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharExtra
#define GpiSetCharExtra _GpiSetCharExtra

inline BOOL _GpiSetCharMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharMode(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharMode
#define GpiSetCharMode _GpiSetCharMode

inline BOOL _GpiSetCharSet(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharSet(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharSet
#define GpiSetCharSet _GpiSetCharSet

inline BOOL _GpiSetCharShear(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCharShear(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCharShear
#define GpiSetCharShear _GpiSetCharShear

inline BOOL _GpiSetClipPath(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetClipPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetClipPath
#define GpiSetClipPath _GpiSetClipPath

inline LONG _GpiSetClipRegion(HPS a, HRGN b, PHRGN c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetClipRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetClipRegion
#define GpiSetClipRegion _GpiSetClipRegion

inline BOOL _GpiSetColor(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetColor(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetColor
#define GpiSetColor _GpiSetColor

inline BOOL _GpiSetCp(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCp(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCp
#define GpiSetCp _GpiSetCp

inline BOOL _GpiSetCurrentPosition(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetCurrentPosition(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetCurrentPosition
#define GpiSetCurrentPosition _GpiSetCurrentPosition

inline BOOL _GpiSetDefArcParams(HPS a, PARCPARAMS b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetDefArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetDefArcParams
#define GpiSetDefArcParams _GpiSetDefArcParams

inline BOOL _GpiSetDefAttrs(HPS a, LONG b, ULONG c, PVOID d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetDefAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetDefAttrs
#define GpiSetDefAttrs _GpiSetDefAttrs

inline BOOL _GpiSetDefaultViewMatrix(HPS a, LONG b, PMATRIXLF c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetDefaultViewMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetDefaultViewMatrix
#define GpiSetDefaultViewMatrix _GpiSetDefaultViewMatrix

inline BOOL _GpiSetDefTag(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetDefTag(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetDefTag
#define GpiSetDefTag _GpiSetDefTag

inline BOOL _GpiSetDefViewingLimits(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetDefViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetDefViewingLimits
#define GpiSetDefViewingLimits _GpiSetDefViewingLimits

inline BOOL _GpiSetEditMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetEditMode(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetEditMode
#define GpiSetEditMode _GpiSetEditMode

inline BOOL _GpiSetElementPointer(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetElementPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetElementPointer
#define GpiSetElementPointer _GpiSetElementPointer

inline BOOL _GpiSetElementPointerAtLabel(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetElementPointerAtLabel(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetElementPointerAtLabel
#define GpiSetElementPointerAtLabel _GpiSetElementPointerAtLabel

inline BOOL _GpiSetGraphicsField(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetGraphicsField(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetGraphicsField
#define GpiSetGraphicsField _GpiSetGraphicsField

inline BOOL _GpiSetLineEnd(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetLineEnd(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetLineEnd
#define GpiSetLineEnd _GpiSetLineEnd

inline BOOL _GpiSetLineJoin(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetLineJoin(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetLineJoin
#define GpiSetLineJoin _GpiSetLineJoin

inline BOOL _GpiSetLineType(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetLineType(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetLineType
#define GpiSetLineType _GpiSetLineType

inline BOOL _GpiSetLineWidth(HPS a, FIXED b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetLineWidth(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetLineWidth
#define GpiSetLineWidth _GpiSetLineWidth

inline BOOL _GpiSetLineWidthGeom(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetLineWidthGeom(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetLineWidthGeom
#define GpiSetLineWidthGeom _GpiSetLineWidthGeom

inline BOOL _GpiSetMarker(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetMarker(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetMarker
#define GpiSetMarker _GpiSetMarker

inline BOOL _GpiSetMarkerBox(HPS a, PSIZEF b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetMarkerBox(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetMarkerBox
#define GpiSetMarkerBox _GpiSetMarkerBox

inline BOOL _GpiSetMarkerSet(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetMarkerSet(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetMarkerSet
#define GpiSetMarkerSet _GpiSetMarkerSet

inline BOOL _GpiSetMetaFileBits(HMF a, LONG b, LONG c, PBYTE d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetMetaFileBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetMetaFileBits
#define GpiSetMetaFileBits _GpiSetMetaFileBits

inline BOOL _GpiSetMix(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetMix(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetMix
#define GpiSetMix _GpiSetMix

inline BOOL _GpiSetModelTransformMatrix(HPS a, LONG b, PMATRIXLF c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetModelTransformMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetModelTransformMatrix
#define GpiSetModelTransformMatrix _GpiSetModelTransformMatrix

inline BOOL _GpiSetPageViewport(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPageViewport(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPageViewport
#define GpiSetPageViewport _GpiSetPageViewport

inline BOOL _GpiSetPaletteEntries(HPAL a, ULONG b, ULONG c, ULONG d, ULONG *e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPaletteEntries(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPaletteEntries
#define GpiSetPaletteEntries _GpiSetPaletteEntries

inline BOOL _GpiSetPattern(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPattern(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPattern
#define GpiSetPattern _GpiSetPattern

inline BOOL _GpiSetPatternRefPoint(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPatternRefPoint(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPatternRefPoint
#define GpiSetPatternRefPoint _GpiSetPatternRefPoint

inline BOOL _GpiSetPatternSet(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPatternSet(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPatternSet
#define GpiSetPatternSet _GpiSetPatternSet

inline BOOL _GpiSetRegion(HPS a, HRGN b, LONG c, PRECTL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetRegion(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetRegion
#define GpiSetRegion _GpiSetRegion

inline BOOL _GpiSetSegmentTransformMatrix(HPS a, LONG b, LONG c, MATRIXLF *d, LONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetSegmentTransformMatrix(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetSegmentTransformMatrix
#define GpiSetSegmentTransformMatrix _GpiSetSegmentTransformMatrix

inline BOOL _GpiSetTextAlignment(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetTextAlignment(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetTextAlignment
#define GpiSetTextAlignment _GpiSetTextAlignment

inline BOOL _GpiSetViewingLimits(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetViewingLimits
#define GpiSetViewingLimits _GpiSetViewingLimits

inline BOOL _GpiSetViewingTransformMatrix(HPS a, LONG b, MATRIXLF *c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetViewingTransformMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetViewingTransformMatrix
#define GpiSetViewingTransformMatrix _GpiSetViewingTransformMatrix

inline LONG _GpiStrokePath(HPS a, LONG b, ULONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiStrokePath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiStrokePath
#define GpiStrokePath _GpiStrokePath

inline BOOL _GpiTranslate(HPS a, PMATRIXLF b, LONG c, PPOINTL d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiTranslate(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiTranslate
#define GpiTranslate _GpiTranslate

inline BOOL _GpiUnloadFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiUnloadFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiUnloadFonts
#define GpiUnloadFonts _GpiUnloadFonts

inline BOOL _GpiUnloadPublicFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiUnloadPublicFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiUnloadPublicFonts
#define GpiUnloadPublicFonts _GpiUnloadPublicFonts

#ifdef INCL_GPIBITMAPS
inline LONG _GpiBitBlt(HPS a, HPS b, LONG c, PPOINTL d, LONG e, ULONG f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiBitBlt(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiBitBlt
#define GpiBitBlt _GpiBitBlt

inline BOOL _GpiDeleteBitmap(HBITMAP a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteBitmap(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteBitmap
#define GpiDeleteBitmap _GpiDeleteBitmap

inline HBITMAP _GpiLoadBitmap(HPS a, HMODULE b, ULONG c, LONG d, LONG e)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiLoadBitmap(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiLoadBitmap
#define GpiLoadBitmap _GpiLoadBitmap

inline HBITMAP _GpiSetBitmap(HPS a, HBITMAP b)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetBitmap(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetBitmap
#define GpiSetBitmap _GpiSetBitmap

inline LONG _GpiWCBitBlt(HPS a, HBITMAP b, LONG c, PPOINTL d, LONG e, ULONG f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiWCBitBlt(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiWCBitBlt
#define GpiWCBitBlt _GpiWCBitBlt

inline HBITMAP _GpiCreateBitmap(HPS a, BITMAPINFOHEADER2 *b, ULONG c, PBYTE d, BITMAPINFO2 *e)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreateBitmap(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreateBitmap
#define GpiCreateBitmap _GpiCreateBitmap

inline LONG _GpiDrawBits(HPS a, PVOID b, BITMAPINFO2 *c, LONG d, PPOINTL e, LONG f, ULONG g)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDrawBits(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDrawBits
#define GpiDrawBits _GpiDrawBits

inline LONG _GpiFloodFill(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiFloodFill(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiFloodFill
#define GpiFloodFill _GpiFloodFill

inline LONG _GpiQueryBitmapBits(HPS a, LONG b, LONG c, PBYTE d, PBITMAPINFO2 e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBitmapBits(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBitmapBits
#define GpiQueryBitmapBits _GpiQueryBitmapBits

inline BOOL _GpiQueryBitmapDimension(HBITMAP a, PSIZEL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBitmapDimension(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBitmapDimension
#define GpiQueryBitmapDimension _GpiQueryBitmapDimension

inline HBITMAP _GpiQueryBitmapHandle(HPS a, LONG b)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBitmapHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBitmapHandle
#define GpiQueryBitmapHandle _GpiQueryBitmapHandle

inline BOOL _GpiQueryBitmapInfoHeader(HBITMAP a, PBITMAPINFOHEADER2 b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBitmapInfoHeader(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBitmapInfoHeader
#define GpiQueryBitmapInfoHeader _GpiQueryBitmapInfoHeader

inline BOOL _GpiQueryBitmapParameters(HBITMAP a, PBITMAPINFOHEADER b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBitmapParameters(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBitmapParameters
#define GpiQueryBitmapParameters _GpiQueryBitmapParameters

inline BOOL _GpiQueryDeviceBitmapFormats(HPS a, LONG b, PLONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDeviceBitmapFormats(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDeviceBitmapFormats
#define GpiQueryDeviceBitmapFormats _GpiQueryDeviceBitmapFormats

inline LONG _GpiSetBitmapBits(HPS a, LONG b, LONG c, PBYTE d, BITMAPINFO2 *e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetBitmapBits(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetBitmapBits
#define GpiSetBitmapBits _GpiSetBitmapBits

inline LONG _GpiQueryPel(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPel(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPel
#define GpiQueryPel _GpiQueryPel

inline BOOL _GpiSetBitmapDimension(HBITMAP a, SIZEL *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetBitmapDimension(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetBitmapDimension
#define GpiSetBitmapDimension _GpiSetBitmapDimension

inline BOOL _GpiSetBitmapId(HPS a, HBITMAP b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetBitmapId(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetBitmapId
#define GpiSetBitmapId _GpiSetBitmapId

inline LONG _GpiSetPel(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPel(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPel
#define GpiSetPel _GpiSetPel

#endif
#ifdef INCL_GPICONTROL
inline BOOL _GpiAssociate(HPS a, HDC b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiAssociate(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiAssociate
#define GpiAssociate _GpiAssociate

inline HPS _GpiCreatePS(HAB a, HDC b, PSIZEL c, ULONG d)
{
 HPS yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCreatePS(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCreatePS
#define GpiCreatePS _GpiCreatePS

inline BOOL _GpiDestroyPS(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDestroyPS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDestroyPS
#define GpiDestroyPS _GpiDestroyPS

inline BOOL _GpiErase(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiErase(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiErase
#define GpiErase _GpiErase

inline HDC _GpiQueryDevice(HPS a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDevice(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDevice
#define GpiQueryDevice _GpiQueryDevice

inline BOOL _GpiRestorePS(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiRestorePS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiRestorePS
#define GpiRestorePS _GpiRestorePS

inline LONG _GpiSavePS(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSavePS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSavePS
#define GpiSavePS _GpiSavePS

inline LONG _GpiErrorSegmentData(HPS a, PLONG b, PLONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiErrorSegmentData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiErrorSegmentData
#define GpiErrorSegmentData _GpiErrorSegmentData

inline LONG _GpiQueryDrawControl(HPS a, LONG b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDrawControl(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDrawControl
#define GpiQueryDrawControl _GpiQueryDrawControl

inline LONG _GpiQueryDrawingMode(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryDrawingMode(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryDrawingMode
#define GpiQueryDrawingMode _GpiQueryDrawingMode

inline ULONG _GpiQueryPS(HPS a, PSIZEL b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPS
#define GpiQueryPS _GpiQueryPS

inline BOOL _GpiResetPS(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiResetPS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiResetPS
#define GpiResetPS _GpiResetPS

inline LONG _GpiQueryStopDraw(HPS a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryStopDraw(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryStopDraw
#define GpiQueryStopDraw _GpiQueryStopDraw

inline BOOL _GpiSetDrawControl(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetDrawControl(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetDrawControl
#define GpiSetDrawControl _GpiSetDrawControl

inline BOOL _GpiSetDrawingMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetDrawingMode(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetDrawingMode
#define GpiSetDrawingMode _GpiSetDrawingMode

inline BOOL _GpiSetPS(HPS a, SIZEL *b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPS(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPS
#define GpiSetPS _GpiSetPS

inline BOOL _GpiSetStopDraw(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetStopDraw(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetStopDraw
#define GpiSetStopDraw _GpiSetStopDraw

#endif
#ifdef INCL_GPICORRELATION
inline LONG _GpiCorrelateChain(HPS a, LONG b, PPOINTL c, LONG d, LONG e, PLONG f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCorrelateChain(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCorrelateChain
#define GpiCorrelateChain _GpiCorrelateChain

inline LONG _GpiCorrelateFrom(HPS a, LONG b, LONG c, LONG d, PPOINTL e, LONG f, LONG g, PLONG h)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCorrelateFrom(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCorrelateFrom
#define GpiCorrelateFrom _GpiCorrelateFrom

inline LONG _GpiCorrelateSegment(HPS a, LONG b, LONG c, PPOINTL d, LONG e, LONG f, PLONG g)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCorrelateSegment(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCorrelateSegment
#define GpiCorrelateSegment _GpiCorrelateSegment

inline BOOL _GpiQueryBoundaryData(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryBoundaryData(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryBoundaryData
#define GpiQueryBoundaryData _GpiQueryBoundaryData

inline BOOL _GpiQueryPickAperturePosition(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPickAperturePosition(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPickAperturePosition
#define GpiQueryPickAperturePosition _GpiQueryPickAperturePosition

inline BOOL _GpiQueryPickApertureSize(HPS a, PSIZEL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryPickApertureSize(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryPickApertureSize
#define GpiQueryPickApertureSize _GpiQueryPickApertureSize

inline BOOL _GpiQueryTag(HPS a, PLONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryTag(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryTag
#define GpiQueryTag _GpiQueryTag

inline BOOL _GpiResetBoundaryData(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiResetBoundaryData(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiResetBoundaryData
#define GpiResetBoundaryData _GpiResetBoundaryData

inline BOOL _GpiSetPickAperturePosition(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPickAperturePosition(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPickAperturePosition
#define GpiSetPickAperturePosition _GpiSetPickAperturePosition

inline BOOL _GpiSetPickApertureSize(HPS a, LONG b, SIZEL *c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetPickApertureSize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetPickApertureSize
#define GpiSetPickApertureSize _GpiSetPickApertureSize

inline BOOL _GpiSetTag(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetTag(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetTag
#define GpiSetTag _GpiSetTag

#endif
#ifdef INCL_GPIINK
inline BOOL _GpiBeginInkPath(HPS a, LONG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiBeginInkPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiBeginInkPath
#define GpiBeginInkPath _GpiBeginInkPath

inline BOOL _GpiEndInkPath(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiEndInkPath(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiEndInkPath
#define GpiEndInkPath _GpiEndInkPath

inline LONG _GpiStrokeInkPath(HPS a, LONG b, LONG c, PPOINTL d, ULONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiStrokeInkPath(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiStrokeInkPath
#define GpiStrokeInkPath _GpiStrokeInkPath

#endif
#ifdef INCL_GPISEGMENTS
inline BOOL _GpiCloseSegment(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiCloseSegment(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiCloseSegment
#define GpiCloseSegment _GpiCloseSegment

inline BOOL _GpiDeleteSegment(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteSegment(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteSegment
#define GpiDeleteSegment _GpiDeleteSegment

inline BOOL _GpiDeleteSegments(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDeleteSegments(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDeleteSegments
#define GpiDeleteSegments _GpiDeleteSegments

inline BOOL _GpiDrawChain(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDrawChain(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDrawChain
#define GpiDrawChain _GpiDrawChain

inline BOOL _GpiDrawDynamics(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDrawDynamics(a);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDrawDynamics
#define GpiDrawDynamics _GpiDrawDynamics

inline BOOL _GpiDrawFrom(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDrawFrom(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDrawFrom
#define GpiDrawFrom _GpiDrawFrom

inline BOOL _GpiDrawSegment(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiDrawSegment(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiDrawSegment
#define GpiDrawSegment _GpiDrawSegment

inline LONG _GpiGetData(HPS a, LONG b, PLONG c, LONG d, LONG e, PBYTE f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiGetData(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiGetData
#define GpiGetData _GpiGetData

inline BOOL _GpiOpenSegment(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiOpenSegment(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiOpenSegment
#define GpiOpenSegment _GpiOpenSegment

inline LONG _GpiPutData(HPS a, LONG b, PLONG c, PBYTE d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiPutData(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiPutData
#define GpiPutData _GpiPutData

inline LONG _GpiQueryInitialSegmentAttrs(HPS a, LONG b)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQueryInitialSegmentAttrs(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQueryInitialSegmentAttrs
#define GpiQueryInitialSegmentAttrs _GpiQueryInitialSegmentAttrs

inline LONG _GpiQuerySegmentAttrs(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQuerySegmentAttrs(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQuerySegmentAttrs
#define GpiQuerySegmentAttrs _GpiQuerySegmentAttrs

inline LONG _GpiQuerySegmentNames(HPS a, LONG b, LONG c, LONG d, PLONG e)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQuerySegmentNames(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQuerySegmentNames
#define GpiQuerySegmentNames _GpiQuerySegmentNames

inline LONG _GpiQuerySegmentPriority(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiQuerySegmentPriority(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiQuerySegmentPriority
#define GpiQuerySegmentPriority _GpiQuerySegmentPriority

inline BOOL _GpiRemoveDynamics(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiRemoveDynamics(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiRemoveDynamics
#define GpiRemoveDynamics _GpiRemoveDynamics

inline BOOL _GpiSetInitialSegmentAttrs(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetInitialSegmentAttrs(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetInitialSegmentAttrs
#define GpiSetInitialSegmentAttrs _GpiSetInitialSegmentAttrs

inline BOOL _GpiSetSegmentAttrs(HPS a, LONG b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetSegmentAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetSegmentAttrs
#define GpiSetSegmentAttrs _GpiSetSegmentAttrs

inline BOOL _GpiSetSegmentPriority(HPS a, LONG b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = GpiSetSegmentPriority(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  GpiSetSegmentPriority
#define GpiSetSegmentPriority _GpiSetSegmentPriority

#endif
#ifdef INCL_DEV
inline LONG _DevEscape(HDC a, LONG b, LONG c, PBYTE d, PLONG e, PBYTE f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevEscape(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DevEscape
#define DevEscape _DevEscape

inline LONG _DevPostEscape(PCSZ a, PCSZ b, PCSZ c, PCSZ d, ULONG e, ULONG f, PBYTE g, ULONG h, PBYTE i)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevPostEscape(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  DevPostEscape
#define DevPostEscape _DevPostEscape

inline LONG _DevPostDeviceModes(HAB a, PDRIVDATA b, PCSZ c, PCSZ d, PCSZ e, ULONG f)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevPostDeviceModes(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DevPostDeviceModes
#define DevPostDeviceModes _DevPostDeviceModes

inline BOOL _DevQueryDeviceNames(HAB a, PCSZ b, PLONG c, PSTR32 d, PSTR64 e, PLONG f, PSTR16 g)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevQueryDeviceNames(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  DevQueryDeviceNames
#define DevQueryDeviceNames _DevQueryDeviceNames

inline LONG _DevQueryHardcopyCaps(HDC a, LONG b, LONG c, PHCINFO d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevQueryHardcopyCaps(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DevQueryHardcopyCaps
#define DevQueryHardcopyCaps _DevQueryHardcopyCaps

#endif
inline HMF _DevCloseDC(HDC a)
{
 HMF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevCloseDC(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DevCloseDC
#define DevCloseDC _DevCloseDC

inline HDC _DevOpenDC(HAB a, LONG b, PCSZ c, LONG d, PDEVOPENDATA e, HDC f)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevOpenDC(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DevOpenDC
#define DevOpenDC _DevOpenDC

inline BOOL _DevQueryCaps(HDC a, LONG b, LONG c, PLONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DevQueryCaps(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DevQueryCaps
#define DevQueryCaps _DevQueryCaps

#endif
#ifdef INCL_WINPROGRAMLIST
inline HPROGRAM _PrfAddProgram(HINI a, PPROGDETAILS b, HPROGRAM c)
{
 HPROGRAM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfAddProgram(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfAddProgram
#define PrfAddProgram _PrfAddProgram

inline BOOL _PrfChangeProgram(HINI a, HPROGRAM b, PPROGDETAILS c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfChangeProgram(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfChangeProgram
#define PrfChangeProgram _PrfChangeProgram

inline HPROGRAM _PrfCreateGroup(HINI a, PCSZ b, UCHAR c)
{
 HPROGRAM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfCreateGroup(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfCreateGroup
#define PrfCreateGroup _PrfCreateGroup

inline BOOL _PrfDestroyGroup(HINI a, HPROGRAM b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfDestroyGroup(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfDestroyGroup
#define PrfDestroyGroup _PrfDestroyGroup

inline PROGCATEGORY _PrfQueryProgramCategory(HINI a, PCSZ b)
{
 PROGCATEGORY yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProgramCategory(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProgramCategory
#define PrfQueryProgramCategory _PrfQueryProgramCategory

inline ULONG _PrfQueryProgramHandle(HINI a, PCSZ b, PHPROGARRAY c, ULONG d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProgramHandle(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProgramHandle
#define PrfQueryProgramHandle _PrfQueryProgramHandle

inline ULONG _PrfQueryProgramTitles(HINI a, HPROGRAM b, PPROGTITLE c, ULONG d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProgramTitles(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProgramTitles
#define PrfQueryProgramTitles _PrfQueryProgramTitles

inline ULONG _PrfQueryDefinition(HINI a, HPROGRAM b, PPROGDETAILS c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryDefinition(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryDefinition
#define PrfQueryDefinition _PrfQueryDefinition

inline BOOL _PrfRemoveProgram(HINI a, HPROGRAM b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfRemoveProgram(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfRemoveProgram
#define PrfRemoveProgram _PrfRemoveProgram

inline HAPP _WinStartApp(HWND a, PPROGDETAILS b, PCSZ c, PVOID d, ULONG e)
{
 HAPP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinStartApp(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinStartApp
#define WinStartApp _WinStartApp

inline BOOL _WinTerminateApp(HAPP a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinTerminateApp(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinTerminateApp
#define WinTerminateApp _WinTerminateApp

#endif
#ifdef INCL_WINSWITCHLIST
inline HSWITCH _WinAddSwitchEntry(PSWCNTRL a)
{
 HSWITCH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinAddSwitchEntry(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinAddSwitchEntry
#define WinAddSwitchEntry _WinAddSwitchEntry

inline ULONG _WinRemoveSwitchEntry(HSWITCH a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRemoveSwitchEntry(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRemoveSwitchEntry
#define WinRemoveSwitchEntry _WinRemoveSwitchEntry

inline ULONG _WinChangeSwitchEntry(HSWITCH a, PSWCNTRL b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinChangeSwitchEntry(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinChangeSwitchEntry
#define WinChangeSwitchEntry _WinChangeSwitchEntry

inline HSWITCH _WinCreateSwitchEntry(HAB a, PSWCNTRL b)
{
 HSWITCH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateSwitchEntry(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateSwitchEntry
#define WinCreateSwitchEntry _WinCreateSwitchEntry

inline ULONG _WinQuerySessionTitle(HAB a, ULONG b, PSZ c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySessionTitle(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySessionTitle
#define WinQuerySessionTitle _WinQuerySessionTitle

inline ULONG _WinQuerySwitchEntry(HSWITCH a, PSWCNTRL b)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySwitchEntry(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySwitchEntry
#define WinQuerySwitchEntry _WinQuerySwitchEntry

inline HSWITCH _WinQuerySwitchHandle(HWND a, PID b)
{
 HSWITCH yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySwitchHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySwitchHandle
#define WinQuerySwitchHandle _WinQuerySwitchHandle

inline ULONG _WinQuerySwitchList(HAB a, PSWBLOCK b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQuerySwitchList(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQuerySwitchList
#define WinQuerySwitchList _WinQuerySwitchList

inline ULONG _WinQueryTaskSizePos(HAB a, ULONG b, PSWP c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryTaskSizePos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryTaskSizePos
#define WinQueryTaskSizePos _WinQueryTaskSizePos

inline ULONG _WinQueryTaskTitle(ULONG a, PSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryTaskTitle(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryTaskTitle
#define WinQueryTaskTitle _WinQueryTaskTitle

inline ULONG _WinSwitchToProgram(HSWITCH a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSwitchToProgram(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSwitchToProgram
#define WinSwitchToProgram _WinSwitchToProgram

#endif
#ifdef INCL_WINSHELLDATA
inline BOOL _PrfCloseProfile(HINI a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfCloseProfile(a);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfCloseProfile
#define PrfCloseProfile _PrfCloseProfile

inline HINI _PrfOpenProfile(HAB a, PCSZ b)
{
 HINI yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfOpenProfile(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfOpenProfile
#define PrfOpenProfile _PrfOpenProfile

inline BOOL _PrfQueryProfile(HAB a, PPRFPROFILE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProfile(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProfile
#define PrfQueryProfile _PrfQueryProfile

inline BOOL _PrfQueryProfileData(HINI a, PCSZ b, PCSZ c, PVOID d, PULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProfileData(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProfileData
#define PrfQueryProfileData _PrfQueryProfileData

inline LONG _PrfQueryProfileInt(HINI a, PCSZ b, PCSZ c, LONG d)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProfileInt(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProfileInt
#define PrfQueryProfileInt _PrfQueryProfileInt

inline BOOL _PrfQueryProfileSize(HINI a, PCSZ b, PCSZ c, PULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProfileSize(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProfileSize
#define PrfQueryProfileSize _PrfQueryProfileSize

inline ULONG _PrfQueryProfileString(HINI a, PCSZ b, PCSZ c, PCSZ d, PVOID e, ULONG f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfQueryProfileString(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfQueryProfileString
#define PrfQueryProfileString _PrfQueryProfileString

inline BOOL _PrfReset(HAB a, PPRFPROFILE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfReset(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfReset
#define PrfReset _PrfReset

inline BOOL _PrfWriteProfileData(HINI a, PCSZ b, PCSZ c, PVOID d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfWriteProfileData(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfWriteProfileData
#define PrfWriteProfileData _PrfWriteProfileData

inline BOOL _PrfWriteProfileString(HINI a, PCSZ b, PCSZ c, PCSZ d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = PrfWriteProfileString(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  PrfWriteProfileString
#define PrfWriteProfileString _PrfWriteProfileString

#endif
#ifdef INCL_WINSTDFILE
inline MRESULT _WinDefFileDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDefFileDlgProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDefFileDlgProc
#define WinDefFileDlgProc _WinDefFileDlgProc

inline HWND _WinFileDlg(HWND a, HWND b, PFILEDLG c)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFileDlg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFileDlg
#define WinFileDlg _WinFileDlg

inline BOOL _WinFreeFileDlgList(PAPSZ a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFreeFileDlgList(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFreeFileDlgList
#define WinFreeFileDlgList _WinFreeFileDlgList

#endif
#ifdef INCL_WINSTDFONT
inline HWND _WinFontDlg(HWND a, HWND b, PFONTDLG c)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFontDlg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFontDlg
#define WinFontDlg _WinFontDlg

inline MRESULT _WinDefFontDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDefFontDlgProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDefFontDlgProc
#define WinDefFontDlgProc _WinDefFontDlgProc

#endif
#ifdef INCL_WINSTDDRAG
inline BOOL _DrgAcceptDroppedFiles(HWND a, PCSZ b, PCSZ c, ULONG d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgAcceptDroppedFiles(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgAcceptDroppedFiles
#define DrgAcceptDroppedFiles _DrgAcceptDroppedFiles

inline BOOL _DrgAccessDraginfo(PDRAGINFO a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgAccessDraginfo(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgAccessDraginfo
#define DrgAccessDraginfo _DrgAccessDraginfo

inline HSTR _DrgAddStrHandle(PCSZ a)
{
 HSTR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgAddStrHandle(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgAddStrHandle
#define DrgAddStrHandle _DrgAddStrHandle

inline PDRAGINFO _DrgAllocDraginfo(ULONG a)
{
 PDRAGINFO yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgAllocDraginfo(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgAllocDraginfo
#define DrgAllocDraginfo _DrgAllocDraginfo

inline PDRAGTRANSFER _DrgAllocDragtransfer(ULONG a)
{
 PDRAGTRANSFER yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgAllocDragtransfer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgAllocDragtransfer
#define DrgAllocDragtransfer _DrgAllocDragtransfer

inline BOOL _DrgCancelLazyDrag()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgCancelLazyDrag();
    SetFS(sel);

    return yyrc;
} 

#undef  DrgCancelLazyDrag
#define DrgCancelLazyDrag _DrgCancelLazyDrag

inline BOOL _DrgDeleteDraginfoStrHandles(PDRAGINFO a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgDeleteDraginfoStrHandles(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgDeleteDraginfoStrHandles
#define DrgDeleteDraginfoStrHandles _DrgDeleteDraginfoStrHandles

inline BOOL _DrgDeleteStrHandle(HSTR a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgDeleteStrHandle(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgDeleteStrHandle
#define DrgDeleteStrHandle _DrgDeleteStrHandle

inline HWND _DrgDrag(HWND a, PDRAGINFO b, PDRAGIMAGE c, ULONG d, LONG e, PVOID f)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgDrag(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgDrag
#define DrgDrag _DrgDrag

inline BOOL _DrgDragFiles(HWND a, PCSZ *b, PCSZ *c, PCSZ *d, ULONG e, HPOINTER f, ULONG g, BOOL h, ULONG i)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgDragFiles(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgDragFiles
#define DrgDragFiles _DrgDragFiles

inline BOOL _DrgFreeDraginfo(PDRAGINFO a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgFreeDraginfo(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgFreeDraginfo
#define DrgFreeDraginfo _DrgFreeDraginfo

inline BOOL _DrgFreeDragtransfer(PDRAGTRANSFER a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgFreeDragtransfer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgFreeDragtransfer
#define DrgFreeDragtransfer _DrgFreeDragtransfer

inline HPS _DrgGetPS(HWND a)
{
 HPS yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgGetPS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgGetPS
#define DrgGetPS _DrgGetPS

inline BOOL _DrgLazyDrag(HWND a, PDRAGINFO b, PDRAGIMAGE c, ULONG d, PVOID e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgLazyDrag(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgLazyDrag
#define DrgLazyDrag _DrgLazyDrag

inline BOOL _DrgLazyDrop(HWND a, ULONG b, PPOINTL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgLazyDrop(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgLazyDrop
#define DrgLazyDrop _DrgLazyDrop

inline BOOL _DrgPostTransferMsg(HWND a, ULONG b, PDRAGTRANSFER c, ULONG d, ULONG e, BOOL f)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgPostTransferMsg(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgPostTransferMsg
#define DrgPostTransferMsg _DrgPostTransferMsg

inline BOOL _DrgPushDraginfo(PDRAGINFO a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgPushDraginfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgPushDraginfo
#define DrgPushDraginfo _DrgPushDraginfo

inline PDRAGINFO _DrgQueryDraginfoPtr(PDRAGINFO a)
{
 PDRAGINFO yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryDraginfoPtr(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryDraginfoPtr
#define DrgQueryDraginfoPtr _DrgQueryDraginfoPtr

inline PDRAGINFO _DrgQueryDraginfoPtrFromHwnd(HWND a)
{
 PDRAGINFO yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryDraginfoPtrFromHwnd(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryDraginfoPtrFromHwnd
#define DrgQueryDraginfoPtrFromHwnd _DrgQueryDraginfoPtrFromHwnd

inline PDRAGINFO _DrgQueryDraginfoPtrFromDragitem(PDRAGITEM a)
{
 PDRAGINFO yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryDraginfoPtrFromDragitem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryDraginfoPtrFromDragitem
#define DrgQueryDraginfoPtrFromDragitem _DrgQueryDraginfoPtrFromDragitem

inline BOOL _DrgQueryDragitem(PDRAGINFO a, ULONG b, PDRAGITEM c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryDragitem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryDragitem
#define DrgQueryDragitem _DrgQueryDragitem

inline ULONG _DrgQueryDragitemCount(PDRAGINFO a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryDragitemCount(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryDragitemCount
#define DrgQueryDragitemCount _DrgQueryDragitemCount

inline PDRAGITEM _DrgQueryDragitemPtr(PDRAGINFO a, ULONG b)
{
 PDRAGITEM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryDragitemPtr(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryDragitemPtr
#define DrgQueryDragitemPtr _DrgQueryDragitemPtr

inline ULONG _DrgQueryDragStatus()
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryDragStatus();
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryDragStatus
#define DrgQueryDragStatus _DrgQueryDragStatus

inline BOOL _DrgQueryNativeRMF(PDRAGITEM a, ULONG b, PCHAR c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryNativeRMF(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryNativeRMF
#define DrgQueryNativeRMF _DrgQueryNativeRMF

inline ULONG _DrgQueryNativeRMFLen(PDRAGITEM a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryNativeRMFLen(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryNativeRMFLen
#define DrgQueryNativeRMFLen _DrgQueryNativeRMFLen

inline ULONG _DrgQueryStrName(HSTR a, ULONG b, PSZ c)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryStrName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryStrName
#define DrgQueryStrName _DrgQueryStrName

inline ULONG _DrgQueryStrNameLen(HSTR a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryStrNameLen(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryStrNameLen
#define DrgQueryStrNameLen _DrgQueryStrNameLen

inline BOOL _DrgQueryTrueType(PDRAGITEM a, ULONG b, PSZ c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryTrueType(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryTrueType
#define DrgQueryTrueType _DrgQueryTrueType

inline ULONG _DrgQueryTrueTypeLen(PDRAGITEM a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgQueryTrueTypeLen(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgQueryTrueTypeLen
#define DrgQueryTrueTypeLen _DrgQueryTrueTypeLen

inline PDRAGINFO _DrgReallocDraginfo(PDRAGINFO a, ULONG b)
{
 PDRAGINFO yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgReallocDraginfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgReallocDraginfo
#define DrgReallocDraginfo _DrgReallocDraginfo

inline BOOL _DrgReleasePS(HPS a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgReleasePS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgReleasePS
#define DrgReleasePS _DrgReleasePS

inline MRESULT _DrgSendTransferMsg(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgSendTransferMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgSendTransferMsg
#define DrgSendTransferMsg _DrgSendTransferMsg

inline BOOL _DrgSetDragImage(PDRAGINFO a, PDRAGIMAGE b, ULONG c, PVOID d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgSetDragImage(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgSetDragImage
#define DrgSetDragImage _DrgSetDragImage

inline BOOL _DrgSetDragitem(PDRAGINFO a, PDRAGITEM b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgSetDragitem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgSetDragitem
#define DrgSetDragitem _DrgSetDragitem

inline BOOL _DrgSetDragPointer(PDRAGINFO a, HPOINTER b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgSetDragPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgSetDragPointer
#define DrgSetDragPointer _DrgSetDragPointer

inline BOOL _DrgVerifyNativeRMF(PDRAGITEM a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgVerifyNativeRMF(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgVerifyNativeRMF
#define DrgVerifyNativeRMF _DrgVerifyNativeRMF

inline BOOL _DrgVerifyRMF(PDRAGITEM a, PCSZ b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgVerifyRMF(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgVerifyRMF
#define DrgVerifyRMF _DrgVerifyRMF

inline BOOL _DrgVerifyTrueType(PDRAGITEM a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgVerifyTrueType(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgVerifyTrueType
#define DrgVerifyTrueType _DrgVerifyTrueType

inline BOOL _DrgVerifyType(PDRAGITEM a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgVerifyType(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgVerifyType
#define DrgVerifyType _DrgVerifyType

inline BOOL _DrgVerifyTypeSet(PDRAGITEM a, PCSZ b, ULONG c, PSZ d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DrgVerifyTypeSet(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DrgVerifyTypeSet
#define DrgVerifyTypeSet _DrgVerifyTypeSet

#endif
#ifdef INCL_WPCLASS
inline HOBJECT _WinCopyObject(HOBJECT a, HOBJECT b, ULONG c)
{
 HOBJECT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCopyObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCopyObject
#define WinCopyObject _WinCopyObject

inline HOBJECT _WinCreateObject(PCSZ a, PCSZ b, PCSZ c, PCSZ d, ULONG e)
{
 HOBJECT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateObject(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateObject
#define WinCreateObject _WinCreateObject

inline HOBJECT _WinCreateShadow(HOBJECT a, HOBJECT b, ULONG c)
{
 HOBJECT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateShadow(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateShadow
#define WinCreateShadow _WinCreateShadow

inline BOOL _WinDeregisterObjectClass(PCSZ a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDeregisterObjectClass(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDeregisterObjectClass
#define WinDeregisterObjectClass _WinDeregisterObjectClass

inline BOOL _WinDestroyObject(HOBJECT a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyObject(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyObject
#define WinDestroyObject _WinDestroyObject

inline BOOL _WinEnumObjectClasses(POBJCLASS a, PULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinEnumObjectClasses(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinEnumObjectClasses
#define WinEnumObjectClasses _WinEnumObjectClasses

inline BOOL _WinIsSOMDDReady()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsSOMDDReady();
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsSOMDDReady
#define WinIsSOMDDReady _WinIsSOMDDReady

inline BOOL _WinIsWPDServerReady()
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinIsWPDServerReady();
    SetFS(sel);

    return yyrc;
} 

#undef  WinIsWPDServerReady
#define WinIsWPDServerReady _WinIsWPDServerReady

inline HOBJECT _WinMoveObject(HOBJECT a, HOBJECT b, ULONG c)
{
 HOBJECT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinMoveObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinMoveObject
#define WinMoveObject _WinMoveObject

inline BOOL _WinOpenObject(HOBJECT a, ULONG b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinOpenObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinOpenObject
#define WinOpenObject _WinOpenObject

inline BOOL _WinQueryActiveDesktopPathname(PSZ a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryActiveDesktopPathname(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryActiveDesktopPathname
#define WinQueryActiveDesktopPathname _WinQueryActiveDesktopPathname

inline HOBJECT _WinQueryObject(PCSZ a)
{
 HOBJECT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryObject(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryObject
#define WinQueryObject _WinQueryObject

inline BOOL _WinQueryObjectPath(HOBJECT a, PSZ b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryObjectPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryObjectPath
#define WinQueryObjectPath _WinQueryObjectPath

inline BOOL _WinRegisterObjectClass(PCSZ a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRegisterObjectClass(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRegisterObjectClass
#define WinRegisterObjectClass _WinRegisterObjectClass

inline BOOL _WinReplaceObjectClass(PCSZ a, PCSZ b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinReplaceObjectClass(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinReplaceObjectClass
#define WinReplaceObjectClass _WinReplaceObjectClass

inline ULONG _WinRestartSOMDD(BOOL a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRestartSOMDD(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRestartSOMDD
#define WinRestartSOMDD _WinRestartSOMDD

inline ULONG _WinRestartWPDServer(BOOL a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRestartWPDServer(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRestartWPDServer
#define WinRestartWPDServer _WinRestartWPDServer

inline BOOL _WinSaveObject(HOBJECT a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSaveObject(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSaveObject
#define WinSaveObject _WinSaveObject

inline BOOL _WinSetObjectData(HOBJECT a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetObjectData(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetObjectData
#define WinSetObjectData _WinSetObjectData

inline BOOL _WinFreeFileIcon(HPOINTER a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinFreeFileIcon(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinFreeFileIcon
#define WinFreeFileIcon _WinFreeFileIcon

inline HPOINTER _WinLoadFileIcon(PCSZ a, BOOL b)
{
 HPOINTER yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadFileIcon(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadFileIcon
#define WinLoadFileIcon _WinLoadFileIcon

inline BOOL _WinRestoreWindowPos(PCSZ a, PCSZ b, HWND c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinRestoreWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinRestoreWindowPos
#define WinRestoreWindowPos _WinRestoreWindowPos

inline BOOL _WinSetFileIcon(PCSZ a, PICONINFO b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinSetFileIcon(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinSetFileIcon
#define WinSetFileIcon _WinSetFileIcon

inline BOOL _WinShutdownSystem(HAB a, HMQ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinShutdownSystem(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinShutdownSystem
#define WinShutdownSystem _WinShutdownSystem

inline BOOL _WinStoreWindowPos(PCSZ a, PCSZ b, HWND c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinStoreWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinStoreWindowPos
#define WinStoreWindowPos _WinStoreWindowPos

#endif
#ifdef INCL_SPL
inline BOOL _SplStdClose(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplStdClose(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplStdClose
#define SplStdClose _SplStdClose

inline BOOL _SplStdDelete(HSTD a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplStdDelete(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplStdDelete
#define SplStdDelete _SplStdDelete

inline BOOL _SplStdGetBits(HSTD a, LONG b, LONG c, PCH d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplStdGetBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  SplStdGetBits
#define SplStdGetBits _SplStdGetBits

inline BOOL _SplStdOpen(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplStdOpen(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplStdOpen
#define SplStdOpen _SplStdOpen

inline LONG _SplStdQueryLength(HSTD a)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplStdQueryLength(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplStdQueryLength
#define SplStdQueryLength _SplStdQueryLength

inline BOOL _SplStdStart(HDC a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplStdStart(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplStdStart
#define SplStdStart _SplStdStart

inline HSTD _SplStdStop(HDC a)
{
 HSTD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplStdStop(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplStdStop
#define SplStdStop _SplStdStop

inline SPLERR _SplControlDevice(PSZ a, PSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplControlDevice(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  SplControlDevice
#define SplControlDevice _SplControlDevice

inline SPLERR _SplCopyJob(PCSZ a, PCSZ b, ULONG c, PCSZ d, PCSZ e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplCopyJob(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplCopyJob
#define SplCopyJob _SplCopyJob

inline SPLERR _SplCreateDevice(PSZ a, ULONG b, PVOID c, ULONG d)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplCreateDevice(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  SplCreateDevice
#define SplCreateDevice _SplCreateDevice

inline SPLERR _SplCreatePort(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplCreatePort(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplCreatePort
#define SplCreatePort _SplCreatePort

inline SPLERR _SplCreateQueue(PSZ a, ULONG b, PVOID c, ULONG d)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplCreateQueue(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  SplCreateQueue
#define SplCreateQueue _SplCreateQueue

inline SPLERR _SplDeleteDevice(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplDeleteDevice(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplDeleteDevice
#define SplDeleteDevice _SplDeleteDevice

inline SPLERR _SplDeleteJob(PSZ a, PSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplDeleteJob(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  SplDeleteJob
#define SplDeleteJob _SplDeleteJob

inline SPLERR _SplDeletePort(PCSZ a, PCSZ b)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplDeletePort(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplDeletePort
#define SplDeletePort _SplDeletePort

inline SPLERR _SplDeleteQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplDeleteQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplDeleteQueue
#define SplDeleteQueue _SplDeleteQueue

inline SPLERR _SplEnumDevice(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplEnumDevice(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  SplEnumDevice
#define SplEnumDevice _SplEnumDevice

inline SPLERR _SplEnumDriver(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplEnumDriver(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  SplEnumDriver
#define SplEnumDriver _SplEnumDriver

inline SPLERR _SplEnumJob(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f, PULONG g, PULONG h, PVOID i)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplEnumJob(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  SplEnumJob
#define SplEnumJob _SplEnumJob

inline SPLERR _SplEnumPort(PCSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplEnumPort(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  SplEnumPort
#define SplEnumPort _SplEnumPort

inline SPLERR _SplEnumPrinter(PSZ a, ULONG b, ULONG c, PVOID d, ULONG e, PULONG f, PULONG g, PULONG h, PVOID i)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplEnumPrinter(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#undef  SplEnumPrinter
#define SplEnumPrinter _SplEnumPrinter

inline SPLERR _SplEnumQueue(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplEnumQueue(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  SplEnumQueue
#define SplEnumQueue _SplEnumQueue

inline SPLERR _SplEnumQueueProcessor(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplEnumQueueProcessor(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

#undef  SplEnumQueueProcessor
#define SplEnumQueueProcessor _SplEnumQueueProcessor

inline SPLERR _SplHoldJob(PCSZ a, PCSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplHoldJob(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  SplHoldJob
#define SplHoldJob _SplHoldJob

inline SPLERR _SplHoldQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplHoldQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplHoldQueue
#define SplHoldQueue _SplHoldQueue

inline SPLERR _SplPurgeQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplPurgeQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplPurgeQueue
#define SplPurgeQueue _SplPurgeQueue

inline SPLERR _SplQueryDevice(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQueryDevice(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQueryDevice
#define SplQueryDevice _SplQueryDevice

inline SPLERR _SplQueryDriver(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f, PULONG g)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQueryDriver(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQueryDriver
#define SplQueryDriver _SplQueryDriver

inline SPLERR _SplQueryJob(PSZ a, PSZ b, ULONG c, ULONG d, PVOID e, ULONG f, PULONG g)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQueryJob(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQueryJob
#define SplQueryJob _SplQueryJob

inline SPLERR _SplQueryPort(PCSZ a, PCSZ b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQueryPort(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQueryPort
#define SplQueryPort _SplQueryPort

inline SPLERR _SplQueryQueue(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQueryQueue(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQueryQueue
#define SplQueryQueue _SplQueryQueue

inline SPLERR _SplReleaseJob(PCSZ a, PCSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplReleaseJob(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  SplReleaseJob
#define SplReleaseJob _SplReleaseJob

inline SPLERR _SplReleaseQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplReleaseQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplReleaseQueue
#define SplReleaseQueue _SplReleaseQueue

inline SPLERR _SplSetDevice(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplSetDevice(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplSetDevice
#define SplSetDevice _SplSetDevice

inline SPLERR _SplSetDriver(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f, ULONG g)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplSetDriver(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  SplSetDriver
#define SplSetDriver _SplSetDriver

inline SPLERR _SplSetJob(PSZ a, PSZ b, ULONG c, ULONG d, PVOID e, ULONG f, ULONG g)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplSetJob(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  SplSetJob
#define SplSetJob _SplSetJob

inline SPLERR _SplSetPort(PCSZ a, PCSZ b, ULONG c, PVOID d, ULONG e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplSetPort(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplSetPort
#define SplSetPort _SplSetPort

inline SPLERR _SplSetQueue(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplSetQueue(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  SplSetQueue
#define SplSetQueue _SplSetQueue

inline ULONG _SplMessageBox(PSZ a, ULONG b, ULONG c, PSZ d, PSZ e, ULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplMessageBox(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  SplMessageBox
#define SplMessageBox _SplMessageBox

inline BOOL _SplQmAbort(HSPL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmAbort(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmAbort
#define SplQmAbort _SplQmAbort

inline BOOL _SplQmAbortDoc(HSPL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmAbortDoc(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmAbortDoc
#define SplQmAbortDoc _SplQmAbortDoc

inline BOOL _SplQmClose(HSPL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmClose(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmClose
#define SplQmClose _SplQmClose

inline BOOL _SplQmEndDoc(HSPL a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmEndDoc(a);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmEndDoc
#define SplQmEndDoc _SplQmEndDoc

inline ULONG _SplQmGetJobID(HSPL a, ULONG b, PVOID c, ULONG d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmGetJobID(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmGetJobID
#define SplQmGetJobID _SplQmGetJobID

inline BOOL _SplQmNewPage(HSPL a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmNewPage(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmNewPage
#define SplQmNewPage _SplQmNewPage

inline HSPL _SplQmOpen(PSZ a, LONG b, PQMOPENDATA c)
{
 HSPL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmOpen(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmOpen
#define SplQmOpen _SplQmOpen

inline BOOL _SplQmStartDoc(HSPL a, PSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmStartDoc(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmStartDoc
#define SplQmStartDoc _SplQmStartDoc

inline BOOL _SplQmWrite(HSPL a, LONG b, PVOID c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = SplQmWrite(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  SplQmWrite
#define SplQmWrite _SplQmWrite

#endif
#ifdef INCL_WINHELP
inline BOOL _WinAssociateHelpInstance(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinAssociateHelpInstance(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinAssociateHelpInstance
#define WinAssociateHelpInstance _WinAssociateHelpInstance

inline HWND _WinCreateHelpInstance(HAB a, PHELPINIT b)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateHelpInstance(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateHelpInstance
#define WinCreateHelpInstance _WinCreateHelpInstance

inline BOOL _WinCreateHelpTable(HWND a, PHELPTABLE b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinCreateHelpTable(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  WinCreateHelpTable
#define WinCreateHelpTable _WinCreateHelpTable

inline BOOL _WinDestroyHelpInstance(HWND a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDestroyHelpInstance(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinDestroyHelpInstance
#define WinDestroyHelpInstance _WinDestroyHelpInstance

inline BOOL _WinLoadHelpTable(HWND a, ULONG b, HMODULE c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinLoadHelpTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  WinLoadHelpTable
#define WinLoadHelpTable _WinLoadHelpTable

inline HWND _WinQueryHelpInstance(HWND a)
{
 HWND yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinQueryHelpInstance(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinQueryHelpInstance
#define WinQueryHelpInstance _WinQueryHelpInstance

#endif
#ifdef INCL_DDF
inline BOOL _DdfBeginList(HDDF a, ULONG b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfBeginList(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfBeginList
#define DdfBeginList _DdfBeginList

inline BOOL _DdfBitmap(HDDF a, HBITMAP b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfBitmap(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfBitmap
#define DdfBitmap _DdfBitmap

inline BOOL _DdfEndList(HDDF a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfEndList(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfEndList
#define DdfEndList _DdfEndList

inline BOOL _DdfHyperText(HDDF a, PCSZ b, PCSZ c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfHyperText(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfHyperText
#define DdfHyperText _DdfHyperText

inline BOOL _DdfInform(HDDF a, PCSZ b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfInform(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfInform
#define DdfInform _DdfInform

inline HDDF _DdfInitialize(HWND a, ULONG b, ULONG c)
{
 HDDF yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfInitialize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfInitialize
#define DdfInitialize _DdfInitialize

inline BOOL _DdfListItem(HDDF a, PCSZ b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfListItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfListItem
#define DdfListItem _DdfListItem

inline BOOL _DdfMetafile(HDDF a, HMF b, PRECTL c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfMetafile(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfMetafile
#define DdfMetafile _DdfMetafile

inline BOOL _DdfPara(HDDF a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfPara(a);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfPara
#define DdfPara _DdfPara

inline BOOL _DdfSetColor(HDDF a, COLOR b, COLOR c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfSetColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfSetColor
#define DdfSetColor _DdfSetColor

inline BOOL _DdfSetFont(HDDF a, PCSZ b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfSetFont(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfSetFont
#define DdfSetFont _DdfSetFont

inline BOOL _DdfSetFontStyle(HDDF a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfSetFontStyle(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfSetFontStyle
#define DdfSetFontStyle _DdfSetFontStyle

inline BOOL _DdfSetFormat(HDDF a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfSetFormat(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfSetFormat
#define DdfSetFormat _DdfSetFormat

inline BOOL _DdfSetTextAlign(HDDF a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfSetTextAlign(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfSetTextAlign
#define DdfSetTextAlign _DdfSetTextAlign

inline BOOL _DdfText(HDDF a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DdfText(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  DdfText
#define DdfText _DdfText

#endif
#ifdef INCL_AVIO
inline USHORT _VioAssociate(HDC a, HVPS b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16ASSOCIATE(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioAssociate
#define VioAssociate _VioAssociate

inline USHORT _VioCreateLogFont(PFATTRS a, LONG b, PSTR8 c, HVPS d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16CREATELOGFONT(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  VioCreateLogFont
#define VioCreateLogFont _VioCreateLogFont

inline USHORT _VioCreatePS(PHVPS a, SHORT b, SHORT c, SHORT d, SHORT e, HVPS f)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16CREATEPS(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  VioCreatePS
#define VioCreatePS _VioCreatePS

inline USHORT _VioDeleteSetId(LONG a, HVPS b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16DELETESETID(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioDeleteSetId
#define VioDeleteSetId _VioDeleteSetId

inline USHORT _VioDestroyPS(HVPS a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16DESTROYPS(a);
    SetFS(sel);

    return yyrc;
} 

#undef  VioDestroyPS
#define VioDestroyPS _VioDestroyPS

inline USHORT _VioGetDeviceCellSize(PSHORT a, PSHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETDEVICECELLSIZE(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetDeviceCellSize
#define VioGetDeviceCellSize _VioGetDeviceCellSize

inline USHORT _VioGetOrg(PSHORT a, PSHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETORG(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetOrg
#define VioGetOrg _VioGetOrg

inline USHORT _VioQueryFonts(PLONG a, PFONTMETRICS b, LONG c, PLONG d, PSZ e, ULONG f, HVPS g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16QUERYFONTS(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  VioQueryFonts
#define VioQueryFonts _VioQueryFonts

inline USHORT _VioQuerySetIds(PLONG a, PSTR8 b, PLONG c, LONG d, HVPS e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16QUERYSETIDS(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioQuerySetIds
#define VioQuerySetIds _VioQuerySetIds

inline USHORT _VioSetDeviceCellSize(SHORT a, SHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETDEVICECELLSIZE(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetDeviceCellSize
#define VioSetDeviceCellSize _VioSetDeviceCellSize

inline USHORT _VioSetOrg(SHORT a, SHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETORG(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetOrg
#define VioSetOrg _VioSetOrg

inline USHORT _VioShowPS(SHORT a, SHORT b, SHORT c, HVPS d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SHOWPS(a, b, c, d);
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

    yyrc = VIO16CHECKCHARTYPE(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  VioCheckCharType
#define VioCheckCharType _VioCheckCharType

inline USHORT _VioDeRegister()
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16DEREGISTER();
    SetFS(sel);

    return yyrc;
} 

#undef  VioDeRegister
#define VioDeRegister _VioDeRegister

inline USHORT _VioEndPopUp(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16ENDPOPUP(a);
    SetFS(sel);

    return yyrc;
} 

#undef  VioEndPopUp
#define VioEndPopUp _VioEndPopUp

inline USHORT _VioGetAnsi(PUSHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETANSI(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetAnsi
#define VioGetAnsi _VioGetAnsi

inline USHORT _VioGetBuf(PULONG a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETBUF(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetBuf
#define VioGetBuf _VioGetBuf

inline USHORT _VioGetConfig(USHORT a, PVIOCONFIGINFO b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETCONFIG(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetConfig
#define VioGetConfig _VioGetConfig

inline USHORT _VioGetCp(USHORT a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETCP(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetCp
#define VioGetCp _VioGetCp

inline USHORT _VioGetCurPos(PUSHORT a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETCURPOS(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetCurPos
#define VioGetCurPos _VioGetCurPos

inline USHORT _VioGetCurType(PVIOCURSORINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETCURTYPE(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetCurType
#define VioGetCurType _VioGetCurType

inline USHORT _VioGetFont(PVIOFONTINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETFONT(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetFont
#define VioGetFont _VioGetFont

inline USHORT _VioGetMode(PVIOMODEINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETMODE(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetMode
#define VioGetMode _VioGetMode

inline USHORT _VioGetPhysBuf(PVIOPHYSBUF a, USHORT b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETPHYSBUF(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetPhysBuf
#define VioGetPhysBuf _VioGetPhysBuf

inline USHORT _VioGetState(PVOID a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16GETSTATE(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioGetState
#define VioGetState _VioGetState

inline USHORT _VioModeUndo(USHORT a, USHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16MODEUNDO(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioModeUndo
#define VioModeUndo _VioModeUndo

inline USHORT _VioModeWait(USHORT a, PUSHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16MODEWAIT(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioModeWait
#define VioModeWait _VioModeWait

inline USHORT _VioPopUp(PUSHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16POPUP(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioPopUp
#define VioPopUp _VioPopUp

inline USHORT _VioPrtSc(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16PRTSC(a);
    SetFS(sel);

    return yyrc;
} 

#undef  VioPrtSc
#define VioPrtSc _VioPrtSc

inline USHORT _VioPrtScToggle(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16PRTSCTOGGLE(a);
    SetFS(sel);

    return yyrc;
} 

#undef  VioPrtScToggle
#define VioPrtScToggle _VioPrtScToggle

inline USHORT _VioReadCellStr(PCH a, PUSHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16READCELLSTR(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioReadCellStr
#define VioReadCellStr _VioReadCellStr

inline USHORT _VioReadCharStr(PCH a, PUSHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16READCHARSTR(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioReadCharStr
#define VioReadCharStr _VioReadCharStr

inline USHORT _VioRegister(PSZ a, PSZ b, ULONG c, ULONG d)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16REGISTER(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  VioRegister
#define VioRegister _VioRegister

inline USHORT _VioSavRedrawUndo(USHORT a, USHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SAVREDRAWUNDO(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSavRedrawUndo
#define VioSavRedrawUndo _VioSavRedrawUndo

inline USHORT _VioSavRedrawWait(USHORT a, PUSHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SAVREDRAWWAIT(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSavRedrawWait
#define VioSavRedrawWait _VioSavRedrawWait

inline USHORT _VioScrLock(USHORT a, PUCHAR b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SCRLOCK(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioScrLock
#define VioScrLock _VioScrLock

inline USHORT _VioScrollDn(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SCROLLDN(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  VioScrollDn
#define VioScrollDn _VioScrollDn

inline USHORT _VioScrollLf(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SCROLLLF(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  VioScrollLf
#define VioScrollLf _VioScrollLf

inline USHORT _VioScrollRt(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SCROLLRT(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  VioScrollRt
#define VioScrollRt _VioScrollRt

inline USHORT _VioScrollUp(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SCROLLUP(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#undef  VioScrollUp
#define VioScrollUp _VioScrollUp

inline USHORT _VioScrUnLock(HVIO a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SCRUNLOCK(a);
    SetFS(sel);

    return yyrc;
} 

#undef  VioScrUnLock
#define VioScrUnLock _VioScrUnLock

inline USHORT _VioSetAnsi(USHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETANSI(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetAnsi
#define VioSetAnsi _VioSetAnsi

inline USHORT _VioSetCp(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETCP(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetCp
#define VioSetCp _VioSetCp

inline USHORT _VioSetCurPos(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETCURPOS(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetCurPos
#define VioSetCurPos _VioSetCurPos

inline USHORT _VioSetCurType(PVIOCURSORINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETCURTYPE(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetCurType
#define VioSetCurType _VioSetCurType

inline USHORT _VioSetFont(PVIOFONTINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETFONT(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetFont
#define VioSetFont _VioSetFont

inline USHORT _VioSetMode(PVIOMODEINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETMODE(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetMode
#define VioSetMode _VioSetMode

inline USHORT _VioSetState(PVOID a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SETSTATE(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  VioSetState
#define VioSetState _VioSetState

inline USHORT _VioShowBuf(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16SHOWBUF(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  VioShowBuf
#define VioShowBuf _VioShowBuf

inline USHORT _VioWrtCellStr(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16WRTCELLSTR(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioWrtCellStr
#define VioWrtCellStr _VioWrtCellStr

inline USHORT _VioWrtCharStr(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16WRTCHARSTR(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioWrtCharStr
#define VioWrtCharStr _VioWrtCharStr

inline USHORT _VioWrtCharStrAtt(PCH a, USHORT b, USHORT c, USHORT d, PBYTE e, HVIO f)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16WRTCHARSTRATT(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  VioWrtCharStrAtt
#define VioWrtCharStrAtt _VioWrtCharStrAtt

inline USHORT _VioWrtNAttr(PBYTE a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16WRTNATTR(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioWrtNAttr
#define VioWrtNAttr _VioWrtNAttr

inline USHORT _VioWrtNCell(PBYTE a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16WRTNCELL(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioWrtNCell
#define VioWrtNCell _VioWrtNCell

inline USHORT _VioWrtNChar(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16WRTNCHAR(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  VioWrtNChar
#define VioWrtNChar _VioWrtNChar

inline USHORT _VioWrtTTY(PCH a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = VIO16WRTTTY(a, b, c);
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

    yyrc = MOU16CLOSE(a);
    SetFS(sel);

    return yyrc;
} 

#undef  MouClose
#define MouClose _MouClose

inline USHORT _MouDeRegister()
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16DEREGISTER();
    SetFS(sel);

    return yyrc;
} 

#undef  MouDeRegister
#define MouDeRegister _MouDeRegister

inline USHORT _MouDrawPtr(HMOU a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16DRAWPTR(a);
    SetFS(sel);

    return yyrc;
} 

#undef  MouDrawPtr
#define MouDrawPtr _MouDrawPtr

inline USHORT _MouFlushQue(HMOU a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16FLUSHQUE(a);
    SetFS(sel);

    return yyrc;
} 

#undef  MouFlushQue
#define MouFlushQue _MouFlushQue

inline USHORT _MouGetDevStatus(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETDEVSTATUS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetDevStatus
#define MouGetDevStatus _MouGetDevStatus

inline USHORT _MouGetEventMask(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETEVENTMASK(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetEventMask
#define MouGetEventMask _MouGetEventMask

inline USHORT _MouGetNumButtons(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETNUMBUTTONS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetNumButtons
#define MouGetNumButtons _MouGetNumButtons

inline USHORT _MouGetNumMickeys(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETNUMMICKEYS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetNumMickeys
#define MouGetNumMickeys _MouGetNumMickeys

inline USHORT _MouGetNumQueEl(PMOUQUEINFO a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETNUMQUEEL(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetNumQueEl
#define MouGetNumQueEl _MouGetNumQueEl

inline USHORT _MouGetPtrPos(PPTRLOC a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETPTRPOS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetPtrPos
#define MouGetPtrPos _MouGetPtrPos

inline USHORT _MouGetPtrShape(PBYTE a, PPTRSHAPE b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETPTRSHAPE(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetPtrShape
#define MouGetPtrShape _MouGetPtrShape

inline USHORT _MouGetScaleFact(PSCALEFACT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETSCALEFACT(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetScaleFact
#define MouGetScaleFact _MouGetScaleFact

inline USHORT _MouGetThreshold(PTHRESHOLD a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16GETTHRESHOLD(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouGetThreshold
#define MouGetThreshold _MouGetThreshold

inline USHORT _MouInitReal(PSZ a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16INITREAL(a);
    SetFS(sel);

    return yyrc;
} 

#undef  MouInitReal
#define MouInitReal _MouInitReal

inline USHORT _MouOpen(PSZ a, PHMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16OPEN(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouOpen
#define MouOpen _MouOpen

inline USHORT _MouReadEventQue(PMOUEVENTINFO a, PUSHORT b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16READEVENTQUE(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  MouReadEventQue
#define MouReadEventQue _MouReadEventQue

inline USHORT _MouRegister(PSZ a, PSZ b, ULONG c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16REGISTER(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  MouRegister
#define MouRegister _MouRegister

inline USHORT _MouRemovePtr(PNOPTRRECT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16REMOVEPTR(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouRemovePtr
#define MouRemovePtr _MouRemovePtr

inline USHORT _MouSetDevStatus(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16SETDEVSTATUS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouSetDevStatus
#define MouSetDevStatus _MouSetDevStatus

inline USHORT _MouSetEventMask(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16SETEVENTMASK(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouSetEventMask
#define MouSetEventMask _MouSetEventMask

inline USHORT _MouSetPtrPos(PPTRLOC a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16SETPTRPOS(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouSetPtrPos
#define MouSetPtrPos _MouSetPtrPos

inline USHORT _MouSetPtrShape(PBYTE a, PPTRSHAPE b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16SETPTRSHAPE(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  MouSetPtrShape
#define MouSetPtrShape _MouSetPtrShape

inline USHORT _MouSetScaleFact(PSCALEFACT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16SETSCALEFACT(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouSetScaleFact
#define MouSetScaleFact _MouSetScaleFact

inline USHORT _MouSetThreshold(PTHRESHOLD a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16SETTHRESHOLD(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  MouSetThreshold
#define MouSetThreshold _MouSetThreshold

inline USHORT _MouSynch(USHORT a)
{
 USHORT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = MOU16SYNCH(a);
    SetFS(sel);

    return yyrc;
} 

#undef  MouSynch
#define MouSynch _MouSynch

#endif

#endif
