#ifndef __OS2WRAP_H__
#define __OS2WRAP_H__

#include <os2sel.h>

#ifdef INCL_DOSMEMMGR
inline ULONG APIENTRY DosAllocMem(PPVOID a, ULONG b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosAllocMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosAllocSharedMem(PPVOID a, PCSZ b, ULONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosAllocSharedMem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosFreeMem(PVOID a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFreeMem(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosGetNamedSharedMem(PPVOID a, PCSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosGetNamedSharedMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosGetSharedMem(PVOID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosGetSharedMem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosGiveSharedMem(PVOID a, PID b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosGiveSharedMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryMem(PVOID a, PULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetMem(PVOID a, ULONG b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSubAllocMem(PVOID a, PPVOID b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSubAllocMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSubFreeMem(PVOID a, PVOID b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSubFreeMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSubSetMem(PVOID a, ULONG b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSubSetMem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSubUnsetMem(PVOID a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSubUnsetMem(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSFILEMGR
inline ULONG APIENTRY DosCancelLockRequest(HFILE a, CONST FILELOCK *b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCancelLockRequest(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosClose(HFILE a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosClose(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCopy(PCSZ a, PCSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCopy(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreateDir(PCSZ a, PEAOP2 b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreateDir(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosDelete(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDelete(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosDeleteDir(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDeleteDir(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosDupHandle(HFILE a, PHFILE b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDupHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosEditName(ULONG a, PCSZ b, PCSZ c, PBYTE d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosEditName(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosEnumAttribute(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosEnumAttribute(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosFindClose(HDIR a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFindClose(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosFindFirst(PCSZ a, PHDIR b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFindFirst(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosFindNext(HDIR a, PVOID b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFindNext(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosForceDelete(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosForceDelete(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosFSAttach(PCSZ a, PCSZ b, CONST VOID *c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFSAttach(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosFSCtl(PVOID a, ULONG b, PULONG c, PVOID d, ULONG e, PULONG f, ULONG g, PCSZ h, HFILE i, ULONG j)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFSCtl(a, b, c, d, e, f, g, h, i, j);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosMove(PCSZ a, PCSZ b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosMove(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosOpen(PCSZ a, PHFILE b, PULONG c, ULONG d, ULONG e, ULONG f, ULONG g, PEAOP2 h)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosOpen(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectClose(HFILE a, FHLOCK b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectClose(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectEnumAttribute(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g, FHLOCK h)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectEnumAttribute(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectOpen(PCSZ a, PHFILE b, PULONG c, ULONG d, ULONG e, ULONG f, ULONG g, PEAOP2 h, PFHLOCK i)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectOpen(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectQueryFHState(HFILE a, PULONG b, FHLOCK c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectQueryFHState(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectQueryFileInfo(HFILE a, ULONG b, PVOID c, ULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectQueryFileInfo(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectRead(HFILE a, PVOID b, ULONG c, PULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectRead(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectSetFHState(HFILE a, ULONG b, FHLOCK c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectSetFHState(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectSetFileInfo(HFILE a, ULONG b, PVOID c, ULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectSetFileInfo(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectSetFileLocks(HFILE a, CONST FILELOCK *b, CONST FILELOCK *c, ULONG d, ULONG e, FHLOCK f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectSetFileLocks(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectSetFilePtr(HFILE a, LONG b, ULONG c, PULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectSetFilePtr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectSetFileSize(HFILE a, ULONG b, FHLOCK c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectSetFileSize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosProtectWrite(HFILE a, PVOID b, ULONG c, PULONG d, FHLOCK e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosProtectWrite(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryCurrentDir(ULONG a, PBYTE b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryCurrentDir(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryCurrentDisk(PULONG a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryCurrentDisk(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryFHState(HFILE a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryFHState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryFileInfo(HFILE a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryFileInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryFSAttach(PCSZ a, ULONG b, ULONG c, PFSQBUFFER2 d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryFSAttach(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryFSInfo(ULONG a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryFSInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryHType(HFILE a, PULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryHType(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryPathInfo(PCSZ a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryPathInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryVerify(PBOOL32 a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryVerify(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosRead(HFILE a, PVOID b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosRead(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosResetBuffer(HFILE a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosResetBuffer(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetCurrentDir(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetCurrentDir(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetDefaultDisk(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetDefaultDisk(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetFHState(HFILE a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetFHState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetFileInfo(HFILE a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetFileInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetFileLocks(HFILE a, CONST FILELOCK *b, CONST FILELOCK *c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetFileLocks(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetFilePtr(HFILE a, LONG b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetFilePtr(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetFileSize(HFILE a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetFileSize(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetFSInfo(ULONG a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetFSInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetMaxFH(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetMaxFH(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetPathInfo(PCSZ a, ULONG b, PVOID c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetPathInfo(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetRelMaxFH(PLONG a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetRelMaxFH(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetVerify(BOOL32 a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetVerify(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosShutdown(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosShutdown(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosWrite(HFILE a, PVOID b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosWrite(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSMISC
inline ULONG APIENTRY DosSearchPath(ULONG a, PCSZ b, PCSZ c, PBYTE d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSearchPath(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSDEVICES
inline ULONG APIENTRY DosDevConfig(PVOID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDevConfig(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosDevIOCtl(HFILE a, ULONG b, ULONG c, PVOID d, ULONG e, PULONG f, PVOID g, ULONG h, PULONG i)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDevIOCtl(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosPhysicalDisk(ULONG a, PVOID b, ULONG c, PVOID d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosPhysicalDisk(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSNLS
inline ULONG APIENTRY DosMapCase(ULONG a, CONST COUNTRYCODE *b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosMapCase(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryCollate(ULONG a, CONST COUNTRYCODE *b, PCHAR c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryCollate(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryCp(ULONG a, PULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryCp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryCtryInfo(ULONG a, PCOUNTRYCODE b, PCOUNTRYINFO c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryCtryInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryDBCSEnv(ULONG a, PCOUNTRYCODE b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryDBCSEnv(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetProcessCp(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetProcessCp(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSMODULEMGR
inline ULONG APIENTRY DosFreeModule(HMODULE a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFreeModule(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosLoadModule(PSZ a, ULONG b, PCSZ c, PHMODULE d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosLoadModule(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryModuleHandle(PCSZ a, PHMODULE b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryModuleHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryModuleName(HMODULE a, ULONG b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryModuleName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryProcAddr(HMODULE a, ULONG b, PCSZ c, PFN *d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryProcAddr(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryProcType(HMODULE a, ULONG b, PCSZ c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryProcType(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSRESOURCES
inline ULONG APIENTRY DosFreeResource(PVOID a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFreeResource(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosGetResource(HMODULE a, ULONG b, ULONG c, PPVOID d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosGetResource(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryResourceSize(HMODULE a, ULONG b, ULONG c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryResourceSize(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSPROCESS
inline ULONG APIENTRY DosBeep(ULONG a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosBeep(a, b);
    SetFS(sel);

    return yyrc;
} 

inline VOID APIENTRY DosExit(ULONG a, ULONG b)
{
 USHORT sel = GetFS();

    DosExit(a, b);
    SetFS(sel);
} 

inline ULONG APIENTRY DosAllocThreadLocalMemory(ULONG a, PULONG *b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosAllocThreadLocalMemory(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreateThread(PTID a, PFNTHREAD b, ULONG c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreateThread(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosEnterCritSec()
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosEnterCritSec();
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosExecPgm(PCHAR a, LONG b, ULONG c, PCSZ d, PCSZ e, PRESULTCODES f, PCSZ g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosExecPgm(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosExitCritSec()
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosExitCritSec();
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosExitList(ULONG a, PFNEXITLIST b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosExitList(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosFreeThreadLocalMemory(ULONG *a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosFreeThreadLocalMemory(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosGetInfoBlocks(PTIB *a, PPIB *b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosGetInfoBlocks(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosKillProcess(ULONG a, PID b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosKillProcess(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosKillThread(TID a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosKillThread(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosResumeThread(TID a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosResumeThread(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetPriority(ULONG a, ULONG b, LONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetPriority(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSleep(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSleep(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSuspendThread(TID a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSuspendThread(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosVerifyPidTid(PID a, TID b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosVerifyPidTid(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosWaitChild(ULONG a, ULONG b, PRESULTCODES c, PPID d, PID e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosWaitChild(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosWaitThread(PTID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosWaitThread(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSSESMGR
inline ULONG APIENTRY DosQueryAppType(PCSZ a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryAppType(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSelectSession(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSelectSession(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetSession(ULONG a, PSTATUSDATA b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetSession(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosStartSession(PSTARTDATA a, PULONG b, PPID c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosStartSession(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosStopSession(ULONG a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosStopSession(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSSEMAPHORES
inline ULONG APIENTRY DosCloseEventSem(HEV a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCloseEventSem(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreateEventSem(PCSZ a, PHEV b, ULONG c, BOOL32 d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreateEventSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosOpenEventSem(PCSZ a, PHEV b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosOpenEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosPostEventSem(HEV a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosPostEventSem(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryEventSem(HEV a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosResetEventSem(HEV a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosResetEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosWaitEventSem(HEV a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosWaitEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCloseMutexSem(HMTX a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCloseMutexSem(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreateMutexSem(PCSZ a, PHMTX b, ULONG c, BOOL32 d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreateMutexSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosOpenMutexSem(PCSZ a, PHMTX b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosOpenMutexSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryMutexSem(HMTX a, PPID b, PTID c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryMutexSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosReleaseMutexSem(HMTX a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosReleaseMutexSem(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosRequestMutexSem(HMTX a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosRequestMutexSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosAddMuxWaitSem(HMUX a, PSEMRECORD b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosAddMuxWaitSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCloseMuxWaitSem(HMUX a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCloseMuxWaitSem(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreateMuxWaitSem(PCSZ a, PHMUX b, ULONG c, PSEMRECORD d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreateMuxWaitSem(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosDeleteMuxWaitSem(HMUX a, HSEM b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDeleteMuxWaitSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosOpenMuxWaitSem(PCSZ a, PHMUX b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosOpenMuxWaitSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryMuxWaitSem(HMUX a, PULONG b, PSEMRECORD c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryMuxWaitSem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosWaitMuxWaitSem(HMUX a, ULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosWaitMuxWaitSem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSNMPIPES
inline ULONG APIENTRY DosCallNPipe(PCSZ a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCallNPipe(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosConnectNPipe(HPIPE a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosConnectNPipe(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreateNPipe(PCSZ a, PHPIPE b, ULONG c, ULONG d, ULONG e, ULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreateNPipe(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosDisConnectNPipe(HPIPE a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDisConnectNPipe(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosPeekNPipe(HPIPE a, PVOID b, ULONG c, PULONG d, PAVAILDATA e, PULONG f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosPeekNPipe(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryNPHState(HPIPE a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryNPHState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryNPipeInfo(HPIPE a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryNPipeInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryNPipeSemState(HSEM a, PPIPESEMSTATE b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryNPipeSemState(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosRawReadNPipe(PCSZ a, ULONG b, PULONG c, PVOID d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosRawReadNPipe(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosRawWriteNPipe(PCSZ a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosRawWriteNPipe(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetNPHState(HPIPE a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetNPHState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetNPipeSem(HPIPE a, HSEM b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetNPipeSem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosTransactNPipe(HPIPE a, PVOID b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosTransactNPipe(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosWaitNPipe(PCSZ a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosWaitNPipe(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreatePipe(PHFILE a, PHFILE b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreatePipe(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSQUEUES
inline ULONG APIENTRY DosCloseQueue(HQUEUE a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCloseQueue(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosCreateQueue(PHQUEUE a, ULONG b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCreateQueue(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosOpenQueue(PPID a, PHQUEUE b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosOpenQueue(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosPeekQueue(HQUEUE a, PREQUESTDATA b, PULONG c, PPVOID d, PULONG e, BOOL32 f, PBYTE g, HEV h)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosPeekQueue(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosPurgeQueue(HQUEUE a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosPurgeQueue(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryQueue(HQUEUE a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosReadQueue(HQUEUE a, PREQUESTDATA b, PULONG c, PPVOID d, ULONG e, BOOL32 f, PBYTE g, HEV h)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosReadQueue(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosWriteQueue(HQUEUE a, ULONG b, ULONG c, PVOID d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosWriteQueue(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSEXCEPTIONS
inline ULONG APIENTRY DosAcknowledgeSignalException(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosAcknowledgeSignalException(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosEnterMustComplete(PULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosEnterMustComplete(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosExitMustComplete(PULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosExitMustComplete(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryThreadContext(TID a, ULONG b, PCONTEXTRECORD c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryThreadContext(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosRaiseException(PEXCEPTIONREPORTRECORD a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosRaiseException(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSendSignalException(PID a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSendSignalException(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetExceptionHandler(PEXCEPTIONREGISTRATIONRECORD a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetExceptionHandler(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetSignalExceptionFocus(BOOL32 a, PULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetSignalExceptionFocus(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosUnsetExceptionHandler(PEXCEPTIONREGISTRATIONRECORD a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosUnsetExceptionHandler(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosUnwindException(PEXCEPTIONREGISTRATIONRECORD a, PVOID b, PEXCEPTIONREPORTRECORD c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosUnwindException(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSMISC
inline ULONG APIENTRY DosQuerySysInfo(ULONG a, ULONG b, PVOID c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQuerySysInfo(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosScanEnv(PCSZ a, PSZ *b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosScanEnv(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryRASInfo(ULONG a, PPVOID b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryRASInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSDATETIME
inline ULONG APIENTRY DosGetDateTime(PDATETIME a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosGetDateTime(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetDateTime(CONST DATETIME *a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetDateTime(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosAsyncTimer(ULONG a, HSEM b, PHTIMER c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosAsyncTimer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosStartTimer(ULONG a, HSEM b, PHTIMER c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosStartTimer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosStopTimer(HTIMER a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosStopTimer(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef DosTmrQueryFreq
inline ULONG APIENTRY DosTmrQueryFreq(PULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosTmrQueryFreq(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosTmrQueryTime(PQWORD a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosTmrQueryTime(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSMVDM
inline ULONG APIENTRY DosCloseVDD(HVDD a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosCloseVDD(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosOpenVDD(PCSZ a, PHVDD b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosOpenVDD(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryDOSProperty(SGID a, PCSZ b, ULONG c, PSZ d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryDOSProperty(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosRequestVDD(HVDD a, SGID b, ULONG c, ULONG d, PVOID e, ULONG f, PVOID g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosRequestVDD(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSetDOSProperty(SGID a, PCSZ b, ULONG c, PCSZ d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSetDOSProperty(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSPROCESS
inline ULONG APIENTRY DosDebug(uDB_t *a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDebug(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSMISC
inline ULONG APIENTRY DosGetMessage(PCHAR *a, ULONG b, PCHAR c, ULONG d, ULONG e, PCSZ f, PULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosGetMessage(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosInsertMessage(PCHAR *a, ULONG b, PCSZ c, ULONG d, PCHAR e, ULONG f, PULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosInsertMessage(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosPutMessage(HFILE a, ULONG b, PCHAR c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosPutMessage(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryMessageCP(PCHAR a, ULONG b, PCSZ c, PULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryMessageCP(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DOSRAS
inline ULONG APIENTRY DosDumpProcess(ULONG a, ULONG b, PID c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosDumpProcess(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosForceSystemDump(ULONG a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosForceSystemDump(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosQueryRASInfo(ULONG a, PPVOID b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosQueryRASInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DosSuppressPopUps(ULONG a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DosSuppressPopUps(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_RXSUBCOM
inline ULONG APIENTRY RexxDeregisterSubcom(PCSZ a, PCSZ b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxDeregisterSubcom(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxQuerySubcom(PCSZ a, PCSZ b, PUSHORT c, PUCHAR d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxQuerySubcom(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxRegisterSubcomDll(PCSZ a, PCSZ b, PCSZ c, PUCHAR d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxRegisterSubcomDll(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxRegisterSubcomExe(PCSZ a, PFN b, PUCHAR c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxRegisterSubcomExe(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_RXSHV
inline ULONG APIENTRY RexxVariablePool(PSHVBLOCK a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxVariablePool(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_RXFUNC
inline ULONG APIENTRY RexxDeregisterFunction(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxDeregisterFunction(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxQueryFunction(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxQueryFunction(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxRegisterFunctionDll(PCSZ a, PCSZ b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxRegisterFunctionDll(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxRegisterFunctionExe(PCSZ a, RexxFunctionHandler *b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxRegisterFunctionExe(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_RXSYSEXIT
inline ULONG APIENTRY RexxDeregisterExit(PCSZ a, PCSZ b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxDeregisterExit(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxQueryExit(PCSZ a, PCSZ b, PUSHORT c, PUCHAR d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxQueryExit(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxRegisterExitDll(PCSZ a, PCSZ b, PCSZ c, PUCHAR d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxRegisterExitDll(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxRegisterExitExe(PCSZ a, PFN b, PUCHAR c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxRegisterExitExe(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_RXARI
inline ULONG APIENTRY RexxResetTrace(PID a, TID b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxResetTrace(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxSetHalt(PID a, TID b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxSetHalt(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxSetTrace(PID a, TID b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxSetTrace(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_RXMACRO
inline ULONG APIENTRY RexxAddMacro(PCSZ a, PCSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxAddMacro(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxClearMacroSpace()
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxClearMacroSpace();
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxDropMacro(PCSZ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxDropMacro(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxLoadMacroSpace(ULONG a, PCSZ *b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxLoadMacroSpace(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxQueryMacro(PCSZ a, PUSHORT b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxQueryMacro(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxReorderMacro(PCSZ a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxReorderMacro(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY RexxSaveMacroSpace(ULONG a, PCSZ *b, PCSZ c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxSaveMacroSpace(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY RexxStart(LONG a, PRXSTRING b, PCSZ c, PRXSTRING d, PCSZ e, LONG f, PRXSYSEXIT g, PSHORT h, PRXSTRING i)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = RexxStart(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WIN
#ifdef INCL_WINMESSAGEMGR
inline BOOL APIENTRY WinCancelShutdown(HMQ a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCancelShutdown(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HMQ APIENTRY WinCreateMsgQueue(HAB a, LONG b)
{
 HMQ yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateMsgQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDestroyMsgQueue(HMQ a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyMsgQueue(a);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY WinDispatchMsg(HAB a, PQMSG b)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinDispatchMsg(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinGetMsg(HAB a, PQMSG b, HWND c, ULONG d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinLockInput(HMQ a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinLockInput(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinPeekMsg(HAB a, PQMSG b, HWND c, ULONG d, ULONG e, ULONG f)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinPeekMsg(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinPostMsg(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinPostMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HMQ APIENTRY WinQueueFromID(HAB a, PID b, TID c)
{
 HMQ yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueueFromID(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryQueueInfo(HMQ a, PMQINFO b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryQueueInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HMQ APIENTRY WinQuerySendMsg(HAB a, HMQ b, HMQ c, PQMSG d)
{
 HMQ yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySendMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinRegisterUserDatatype(HAB a, LONG b, LONG c, PLONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinRegisterUserDatatype(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinRegisterUserMsg(HAB a, ULONG b, LONG c, LONG d, LONG e, LONG f, LONG g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinRegisterUserMsg(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinReplyMsg(HAB a, HMQ b, HMQ c, MRESULT d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinReplyMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY WinSendMsg(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinSendMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetMsgMode(HAB a, PCSZ b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetMsgMode(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetSynchroMode(HAB a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetSynchroMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinThreadAssocQueue(HAB a, HMQ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinThreadAssocQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinWakeThread(HMQ a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinWakeThread(a);
    SetFS(sel);

    return yyrc;
} 

#endif
inline HWND APIENTRY WinCreateWindow(HWND a, PCSZ b, PCSZ c, ULONG d, LONG e, LONG f, LONG g, LONG h, HWND i, HWND j, ULONG k, PVOID l, PVOID m)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateWindow(a, b, c, d, e, f, g, h, i, j, k, l, m);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDrawBitmap(HPS a, HBITMAP b, CONST RECTL *c, CONST POINTL *d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDrawBitmap(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDrawBorder(HPS a, CONST RECTL *b, LONG c, LONG d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDrawBorder(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinDrawText(HPS a, LONG b, PCH c, PRECTL d, LONG e, LONG f, ULONG g)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinDrawText(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEnableWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEnableWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEnableWindowUpdate(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEnableWindowUpdate(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinInvalidateRect(HWND a, CONST RECTL *b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinInvalidateRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinInvalidateRegion(HWND a, HRGN b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinInvalidateRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinInvertRect(HPS a, CONST RECTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinInvertRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsChild(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsChild(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsWindow(HAB a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsWindowEnabled(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsWindowEnabled(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsWindowVisible(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsWindowVisible(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinLoadMessage(HAB a, HMODULE b, ULONG c, LONG d, PSZ e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadMessage(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinLoadString(HAB a, HMODULE b, ULONG c, LONG d, PSZ e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinMultWindowFromIDs(HWND a, PHWND b, ULONG c, ULONG d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinMultWindowFromIDs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryDesktopWindow(HAB a, HDC b)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryDesktopWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryObjectWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryObjectWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline HPOINTER APIENTRY WinQueryPointer(HWND a)
{
 HPOINTER yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryPointer(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryWindow(HWND a, LONG b)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryWindowPos(HWND a, PSWP b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryWindowProcess(HWND a, PPID b, PTID c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowProcess(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQueryWindowText(HWND a, LONG b, PCH c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowText(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQueryWindowTextLength(HWND a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowTextLength(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetMultWindowPos(HAB a, CONST SWP *b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetMultWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetOwner(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetOwner(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetParent(HWND a, HWND b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetParent(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetWindowPos(HWND a, HWND b, LONG c, LONG d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetWindowPos(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetWindowText(HWND a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetWindowText(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinUpdateWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinUpdateWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinWindowFromID(HWND a, ULONG b)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinWindowFromID(a, b);
    SetFS(sel);

    return yyrc;
} 

#ifdef INCL_WINFRAMEMGR
inline HWND APIENTRY WinCreateStdWindow(HWND a, ULONG b, PULONG c, PCSZ d, PCSZ e, ULONG f, HMODULE g, ULONG h, PHWND i)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateStdWindow(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinCalcFrameRect(HWND a, PRECTL b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCalcFrameRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinCreateFrameControls(HWND a, PFRAMECDATA b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateFrameControls(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinFlashWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinFlashWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinGetMaxPosition(HWND a, PSWP b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetMaxPosition(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinGetMinPosition(HWND a, PSWP b, CONST POINTL *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetMinPosition(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSaveWindowPos(HSAVEWP a, PSWP b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSaveWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINWINDOWMGR
inline HPS APIENTRY WinBeginPaint(HWND a, HPS b, PRECTL c)
{
 HPS yyrc;
 USHORT sel = GetFS();

    yyrc = WinBeginPaint(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY WinDefWindowProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinDefWindowProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDestroyWindow(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEndPaint(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEndPaint(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinFillRect(HPS a, CONST RECTL *b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinFillRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HPS APIENTRY WinGetClipPS(HWND a, HWND b, ULONG c)
{
 HPS yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetClipPS(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HPS APIENTRY WinGetPS(HWND a)
{
 HPS yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetPS(a);
    SetFS(sel);

    return yyrc;
} 

inline HAB APIENTRY WinInitialize(ULONG a)
{
 HAB yyrc;
 USHORT sel = GetFS();

    yyrc = WinInitialize(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsWindowShowing(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsWindowShowing(a);
    SetFS(sel);

    return yyrc;
} 

inline HDC APIENTRY WinOpenWindowDC(HWND a)
{
 HDC yyrc;
 USHORT sel = GetFS();

    yyrc = WinOpenWindowDC(a);
    SetFS(sel);

    return yyrc;
} 

inline HAB APIENTRY WinQueryAnchorBlock(HWND a)
{
 HAB yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryAnchorBlock(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryVersion(HAB a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryVersion(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryWindowRect(HWND a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinRegisterClass(HAB a, PCSZ b, PFNWP c, ULONG d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinRegisterClass(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinReleasePS(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinReleasePS(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinScrollWindow(HWND a, LONG b, LONG c, CONST RECTL *d, CONST RECTL *e, HRGN f, PRECTL g, ULONG h)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinScrollWindow(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetActiveWindow(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetActiveWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinShowWindow(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinShowWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinTerminate(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinTerminate(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINWINDOWMGR
inline HENUM APIENTRY WinBeginEnumWindows(HWND a)
{
 HENUM yyrc;
 USHORT sel = GetFS();

    yyrc = WinBeginEnumWindows(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEndEnumWindows(HENUM a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEndEnumWindows(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinExcludeUpdateRegion(HPS a, HWND b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinExcludeUpdateRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinGetNextWindow(HENUM a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetNextWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline HPS APIENTRY WinGetScreenPS(HWND a)
{
 HPS yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetScreenPS(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsThreadActive(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsThreadActive(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinLockVisRegions(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinLockVisRegions(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinLockWindowUpdate(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinLockWindowUpdate(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinMapWindowPoints(HWND a, HWND b, PPOINTL c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinMapWindowPoints(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryActiveWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryActiveWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryClassInfo(HAB a, PCSZ b, PCLASSINFO c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryClassInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQueryClassName(HWND a, LONG b, PCH c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryClassName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryUpdateRect(HWND a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryUpdateRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQueryUpdateRegion(HWND a, HRGN b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryUpdateRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQuerySysModalWindow(HWND a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySysModalWindow(a);
    SetFS(sel);

    return yyrc;
} 

inline HDC APIENTRY WinQueryWindowDC(HWND a)
{
 HDC yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowDC(a);
    SetFS(sel);

    return yyrc;
} 

inline PVOID APIENTRY WinQueryWindowPtr(HWND a, LONG b)
{
 PVOID yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowPtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryWindowULong(HWND a, LONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowULong(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY WinQueryWindowUShort(HWND a, LONG b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowUShort(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetSysModalWindow(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetSysModalWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetWindowBits(HWND a, LONG b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetWindowBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetWindowPtr(HWND a, LONG b, PVOID c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetWindowPtr(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetWindowULong(HWND a, LONG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetWindowULong(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetWindowUShort(HWND a, LONG b, USHORT c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetWindowUShort(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline PFNWP APIENTRY WinSubclassWindow(HWND a, PFNWP b)
{
 PFNWP yyrc;
 USHORT sel = GetFS();

    yyrc = WinSubclassWindow(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinValidateRect(HWND a, CONST RECTL *b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinValidateRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinValidateRegion(HWND a, HRGN b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinValidateRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinWindowFromDC(HDC a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinWindowFromDC(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinWindowFromPoint(HWND a, CONST POINTL *b, BOOL c)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinWindowFromPoint(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINACCELERATORS
inline ULONG APIENTRY WinCopyAccelTable(HACCEL a, PACCELTABLE b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinCopyAccelTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HACCEL APIENTRY WinCreateAccelTable(HAB a, PACCELTABLE b)
{
 HACCEL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateAccelTable(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDestroyAccelTable(HACCEL a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyAccelTable(a);
    SetFS(sel);

    return yyrc;
} 

inline HACCEL APIENTRY WinLoadAccelTable(HAB a, HMODULE b, ULONG c)
{
 HACCEL yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadAccelTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HACCEL APIENTRY WinQueryAccelTable(HAB a, HWND b)
{
 HACCEL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryAccelTable(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetAccelTable(HAB a, HACCEL b, HWND c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetAccelTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinTranslateAccel(HAB a, HWND b, HACCEL c, PQMSG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinTranslateAccel(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINATOM
inline ATOM APIENTRY WinAddAtom(HATOMTBL a, PCSZ b)
{
 ATOM yyrc;
 USHORT sel = GetFS();

    yyrc = WinAddAtom(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HATOMTBL APIENTRY WinCreateAtomTable(ULONG a, ULONG b)
{
 HATOMTBL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateAtomTable(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ATOM APIENTRY WinDeleteAtom(HATOMTBL a, ATOM b)
{
 ATOM yyrc;
 USHORT sel = GetFS();

    yyrc = WinDeleteAtom(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HATOMTBL APIENTRY WinDestroyAtomTable(HATOMTBL a)
{
 HATOMTBL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyAtomTable(a);
    SetFS(sel);

    return yyrc;
} 

inline ATOM APIENTRY WinFindAtom(HATOMTBL a, PCSZ b)
{
 ATOM yyrc;
 USHORT sel = GetFS();

    yyrc = WinFindAtom(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryAtomLength(HATOMTBL a, ATOM b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryAtomLength(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryAtomName(HATOMTBL a, ATOM b, PSZ c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryAtomName(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryAtomUsage(HATOMTBL a, ATOM b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryAtomUsage(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HATOMTBL APIENTRY WinQuerySystemAtomTable()
{
 HATOMTBL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySystemAtomTable();
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINCLIPBOARD
inline BOOL APIENTRY WinCloseClipbrd(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCloseClipbrd(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEmptyClipbrd(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEmptyClipbrd(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinEnumClipbrdFmts(HAB a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinEnumClipbrdFmts(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinOpenClipbrd(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinOpenClipbrd(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryClipbrdData(HAB a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryClipbrdData(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryClipbrdFmtInfo(HAB a, ULONG b, PULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryClipbrdFmtInfo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryClipbrdOwner(HAB a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryClipbrdOwner(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryClipbrdViewer(HAB a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryClipbrdViewer(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetClipbrdData(HAB a, ULONG b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetClipbrdData(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetClipbrdOwner(HAB a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetClipbrdOwner(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetClipbrdViewer(HAB a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetClipbrdViewer(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINDDE
inline BOOL APIENTRY WinDdeInitiate(HWND a, PCSZ b, PCSZ c, CONST CONVCONTEXT *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDdeInitiate(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDdePostMsg(HWND a, HWND b, ULONG c, CONST DDESTRUCT *d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDdePostMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY WinDdeRespond(HWND a, HWND b, PCSZ c, PCSZ d, CONST CONVCONTEXT *e)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinDdeRespond(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINCOUNTRY
inline ULONG APIENTRY WinCompareStrings(HAB a, ULONG b, ULONG c, PCSZ d, PCSZ e, ULONG f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinCompareStrings(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline UCHAR APIENTRY WinCpTranslateChar(HAB a, ULONG b, UCHAR c, ULONG d)
{
 UCHAR yyrc;
 USHORT sel = GetFS();

    yyrc = WinCpTranslateChar(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinCpTranslateString(HAB a, ULONG b, PCSZ c, ULONG d, ULONG e, PSZ f)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCpTranslateString(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline PSZ APIENTRY WinNextChar(HAB a, ULONG b, ULONG c, PCSZ d)
{
 PSZ yyrc;
 USHORT sel = GetFS();

    yyrc = WinNextChar(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline PSZ APIENTRY WinPrevChar(HAB a, ULONG b, ULONG c, PCSZ d, PCSZ e)
{
 PSZ yyrc;
 USHORT sel = GetFS();

    yyrc = WinPrevChar(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryCp(HMQ a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryCp(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryCpList(HAB a, ULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryCpList(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetCp(HMQ a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetCp(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinUpper(HAB a, ULONG b, ULONG c, PSZ d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinUpper(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinUpperChar(HAB a, ULONG b, ULONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinUpperChar(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINCURSORS
inline BOOL APIENTRY WinCreateCursor(HWND a, LONG b, LONG c, LONG d, LONG e, ULONG f, PRECTL g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateCursor(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDestroyCursor(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyCursor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinShowCursor(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinShowCursor(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryCursorInfo(HWND a, PCURSORINFO b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryCursorInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINDESKTOP
inline BOOL APIENTRY WinQueryDesktopBkgnd(HWND a, PDESKTOP b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryDesktopBkgnd(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP APIENTRY WinSetDesktopBkgnd(HWND a, CONST DESKTOP *b)
{
 HBITMAP yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetDesktopBkgnd(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINDIALOGS
inline BOOL APIENTRY WinAlarm(HWND a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinAlarm(a, b);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY WinDefDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinDefDlgProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDismissDlg(HWND a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDismissDlg(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinDlgBox(HWND a, HWND b, PFNWP c, HMODULE d, ULONG e, PVOID f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinDlgBox(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinGetDlgMsg(HWND a, PQMSG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetDlgMsg(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinLoadDlg(HWND a, HWND b, PFNWP c, HMODULE d, ULONG e, PVOID f)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadDlg(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinMessageBox(HWND a, HWND b, PCSZ c, PCSZ d, ULONG e, ULONG f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinMessageBox(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinMessageBox2(HWND a, HWND b, PCSZ c, PCSZ d, ULONG e, PMB2INFO f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinMessageBox2(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryDlgItemShort(HWND a, ULONG b, PSHORT c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryDlgItemShort(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryDlgItemText(HWND a, ULONG b, LONG c, PSZ d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryDlgItemText(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQueryDlgItemTextLength(HWND a, ULONG b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryDlgItemTextLength(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetDlgItemShort(HWND a, ULONG b, USHORT c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetDlgItemShort(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetDlgItemText(HWND a, ULONG b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetDlgItemText(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinCreateDlg(HWND a, HWND b, PFNWP c, PDLGTEMPLATE d, PVOID e)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateDlg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinEnumDlgItem(HWND a, HWND b, ULONG c)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinEnumDlgItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinMapDlgPoints(HWND a, PPOINTL b, ULONG c, BOOL d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinMapDlgPoints(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinProcessDlg(HWND a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinProcessDlg(a);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY WinSendDlgItemMsg(HWND a, ULONG b, ULONG c, MPARAM d, MPARAM e)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinSendDlgItemMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinSubstituteStrings(HWND a, PCSZ b, LONG c, PSZ d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinSubstituteStrings(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINERRORS
inline ERRORID APIENTRY WinGetLastError(HAB a)
{
 ERRORID yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetLastError(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinFreeErrorInfo(PERRINFO a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinFreeErrorInfo(a);
    SetFS(sel);

    return yyrc;
} 

inline PERRINFO APIENTRY WinGetErrorInfo(HAB a)
{
 PERRINFO yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetErrorInfo(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINHOOKS
inline BOOL APIENTRY WinCallMsgFilter(HAB a, PQMSG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCallMsgFilter(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinReleaseHook(HAB a, HMQ b, LONG c, PFN d, HMODULE e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinReleaseHook(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetHook(HAB a, HMQ b, LONG c, PFN d, HMODULE e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetHook(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WININPUT
inline BOOL APIENTRY WinFocusChange(HWND a, HWND b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinFocusChange(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinLockupSystem(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinLockupSystem(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetFocus(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetFocus(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinUnlockSystem(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinUnlockSystem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinCheckInput(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCheckInput(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEnablePhysInput(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEnablePhysInput(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinGetKeyState(HWND a, LONG b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetKeyState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinGetPhysKeyState(HWND a, LONG b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetPhysKeyState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsPhysInputEnabled(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsPhysInputEnabled(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryCapture(HWND a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryCapture(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryFocus(HWND a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryFocus(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryVisibleRegion(HWND a, HRGN b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryVisibleRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetCapture(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetCapture(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetKeyboardStateTable(HWND a, PBYTE b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetKeyboardStateTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetVisibleRegionNotify(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetVisibleRegionNotify(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINLOAD
inline BOOL APIENTRY WinDeleteLibrary(HAB a, HLIB b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDeleteLibrary(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDeleteProcedure(HAB a, PFNWP b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDeleteProcedure(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HLIB APIENTRY WinLoadLibrary(HAB a, PCSZ b)
{
 HLIB yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadLibrary(a, b);
    SetFS(sel);

    return yyrc;
} 

inline PFNWP APIENTRY WinLoadProcedure(HAB a, HLIB b, PSZ c)
{
 PFNWP yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadProcedure(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINMENUS
inline HWND APIENTRY WinCreateMenu(HWND a, PVOID b)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateMenu(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinLoadMenu(HWND a, HMODULE b, ULONG c)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadMenu(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinPopupMenu(HWND a, HWND b, HWND c, LONG d, LONG e, LONG f, ULONG g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinPopupMenu(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINMESSAGEMGR
inline BOOL APIENTRY WinBroadcastMsg(HWND a, ULONG b, MPARAM c, MPARAM d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinBroadcastMsg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinInSendMsg(HAB a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinInSendMsg(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinPostQueueMsg(HMQ a, ULONG b, MPARAM c, MPARAM d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinPostQueueMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryMsgPos(HAB a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryMsgPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryMsgTime(HAB a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryMsgTime(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryQueueStatus(HWND a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryQueueStatus(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinRequestMutexSem(HMTX a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinRequestMutexSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetClassMsgInterest(HAB a, PCSZ b, ULONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetClassMsgInterest(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetMsgInterest(HWND a, ULONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetMsgInterest(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinWaitEventSem(HEV a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinWaitEventSem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinWaitMsg(HAB a, ULONG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinWaitMsg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinWaitMuxWaitSem(HMUX a, ULONG b, PULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinWaitMuxWaitSem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINPALETTE
inline LONG APIENTRY WinRealizePalette(HWND a, HPS b, PULONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinRealizePalette(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINPOINTERS
inline HPOINTER APIENTRY WinCreatePointer(HWND a, HBITMAP b, BOOL c, LONG d, LONG e)
{
 HPOINTER yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreatePointer(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HPOINTER APIENTRY WinCreatePointerIndirect(HWND a, CONST POINTERINFO *b)
{
 HPOINTER yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreatePointerIndirect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDestroyPointer(HPOINTER a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyPointer(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDrawPointer(HPS a, LONG b, LONG c, HPOINTER d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDrawPointer(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP APIENTRY WinGetSysBitmap(HWND a, ULONG b)
{
 HBITMAP yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetSysBitmap(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HPOINTER APIENTRY WinLoadPointer(HWND a, HMODULE b, ULONG c)
{
 HPOINTER yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadPointer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinLockPointerUpdate(HWND a, HPOINTER b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinLockPointerUpdate(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryPointerPos(HWND a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryPointerPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryPointerInfo(HPOINTER a, PPOINTERINFO b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryPointerInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HPOINTER APIENTRY WinQuerySysPointer(HWND a, LONG b, BOOL c)
{
 HPOINTER yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySysPointer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQuerySysPointerData(HWND a, ULONG b, PICONINFO c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySysPointerData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetPointer(HWND a, HPOINTER b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetPointerOwner(HPOINTER a, PID b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetPointerOwner(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetPointerPos(HWND a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetPointerPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetSysPointerData(HWND a, ULONG b, CONST ICONINFO *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetSysPointerData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinShowPointer(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinShowPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINRECTANGLES
inline BOOL APIENTRY WinCopyRect(HAB a, PRECTL b, CONST RECTL *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCopyRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEqualRect(HAB a, CONST RECTL *b, CONST RECTL *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEqualRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinInflateRect(HAB a, PRECTL b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinInflateRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIntersectRect(HAB a, PRECTL b, CONST RECTL *c, CONST RECTL *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIntersectRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsRectEmpty(HAB a, CONST RECTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsRectEmpty(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinMakePoints(HAB a, PPOINTL b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinMakePoints(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinMakeRect(HAB a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinMakeRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinOffsetRect(HAB a, PRECTL b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinOffsetRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinPtInRect(HAB a, CONST RECTL *b, CONST POINTL *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinPtInRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetRect(HAB a, PRECTL b, LONG c, LONG d, LONG e, LONG f)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetRect(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetRectEmpty(HAB a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetRectEmpty(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSubtractRect(HAB a, PRECTL b, CONST RECTL *c, CONST RECTL *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSubtractRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinUnionRect(HAB a, PRECTL b, CONST RECTL *c, CONST RECTL *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinUnionRect(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINSYS
inline LONG APIENTRY WinQueryControlColors(HWND a, LONG b, ULONG c, ULONG d, PCTLCOLOR e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryControlColors(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryPresParam(HWND a, ULONG b, ULONG c, PULONG d, ULONG e, PVOID f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryPresParam(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQuerySysColor(HWND a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySysColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQuerySysValue(HWND a, LONG b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySysValue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinRemovePresParam(HWND a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinRemovePresParam(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinSetControlColors(HWND a, LONG b, ULONG c, ULONG d, PCTLCOLOR e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetControlColors(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetPresParam(HWND a, ULONG b, ULONG c, PVOID d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetPresParam(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetSysColors(HWND a, ULONG b, ULONG c, LONG d, ULONG e, CONST LONG *f)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetSysColors(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetSysValue(HWND a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetSysValue(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINTHUNKAPI
inline PFN APIENTRY WinQueryClassThunkProc(PCSZ a)
{
 PFN yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryClassThunkProc(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY WinQueryWindowModel(HWND a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowModel(a);
    SetFS(sel);

    return yyrc;
} 

inline PFN APIENTRY WinQueryWindowThunkProc(HWND a)
{
 PFN yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryWindowThunkProc(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetClassThunkProc(PCSZ a, PFN b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetClassThunkProc(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetWindowThunkProc(HWND a, PFN b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetWindowThunkProc(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINTIMER
inline ULONG APIENTRY WinGetCurrentTime(HAB a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinGetCurrentTime(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinStartTimer(HAB a, HWND b, ULONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinStartTimer(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinStopTimer(HAB a, HWND b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinStopTimer(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINTRACKRECT
inline BOOL APIENTRY WinShowTrackRect(HWND a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinShowTrackRect(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinTrackRect(HWND a, HPS b, PTRACKINFO c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinTrackRect(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#endif
#ifdef INCL_GPI
inline LONG APIENTRY GpiAnimatePalette(HPAL a, ULONG b, ULONG c, ULONG d, CONST ULONG *e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiAnimatePalette(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiBeginArea(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiBeginArea(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiBeginElement(HPS a, LONG b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiBeginElement(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiBeginPath(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiBeginPath(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiBox(HPS a, LONG b, CONST POINTL *c, LONG d, LONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiBox(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCallSegmentMatrix(HPS a, LONG b, LONG c, CONST MATRIXLF *d, LONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCallSegmentMatrix(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCharString(HPS a, LONG b, PCH c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCharString(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCharStringAt(HPS a, CONST POINTL *b, LONG c, PCH d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCharStringAt(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCharStringPos(HPS a, CONST RECTL *b, ULONG c, LONG d, PCH e, CONST LONG *f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCharStringPos(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCharStringPosAt(HPS a, CONST POINTL *b, CONST RECTL *c, ULONG d, LONG e, PCH f, CONST LONG *g)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCharStringPosAt(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiCloseFigure(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCloseFigure(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCombineRegion(HPS a, HRGN b, HRGN c, HRGN d, LONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCombineRegion(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiComment(HPS a, LONG b, CONST BYTE *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiComment(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiConvert(HPS a, LONG b, LONG c, LONG d, PPOINTL e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiConvert(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiConvertWithMatrix(HPS a, LONG b, PPOINTL c, LONG d, CONST MATRIXLF *e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiConvertWithMatrix(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HMF APIENTRY GpiCopyMetaFile(HMF a)
{
 HMF yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCopyMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiCreateLogColorTable(HPS a, ULONG b, LONG c, LONG d, LONG e, CONST LONG *f)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCreateLogColorTable(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCreateLogFont(HPS a, CONST STR8 *b, LONG c, CONST FATTRS *d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCreateLogFont(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HPAL APIENTRY GpiCreatePalette(HAB a, ULONG b, ULONG c, ULONG d, CONST ULONG *e)
{
 HPAL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCreatePalette(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HRGN APIENTRY GpiCreateRegion(HPS a, LONG b, CONST RECTL *c)
{
 HRGN yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCreateRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteElement(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteElement(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteElementRange(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteElementRange(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteElementsBetweenLabels(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteElementsBetweenLabels(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteMetaFile(HMF a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteMetaFile(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeletePalette(HPAL a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeletePalette(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteSetId(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteSetId(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDestroyRegion(HPS a, HRGN b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDestroyRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiElement(HPS a, LONG b, PCSZ c, LONG d, CONST BYTE *e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiElement(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiEndArea(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiEndArea(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiEndElement(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiEndElement(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiEndPath(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiEndPath(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiEqualRegion(HPS a, HRGN b, HRGN c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiEqualRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiExcludeClipRectangle(HPS a, CONST RECTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiExcludeClipRectangle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiFillPath(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiFillPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiFrameRegion(HPS a, HRGN b, CONST SIZEL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiFrameRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiFullArc(HPS a, LONG b, FIXED c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiFullArc(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiImage(HPS a, LONG b, CONST SIZEL *c, LONG d, CONST BYTE *e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiImage(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiIntersectClipRectangle(HPS a, CONST RECTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiIntersectClipRectangle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiLabel(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiLabel(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiLine(HPS a, CONST POINTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiLine(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiLoadFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiLoadFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HMF APIENTRY GpiLoadMetaFile(HAB a, PCSZ b)
{
 HMF yyrc;
 USHORT sel = GetFS();

    yyrc = GpiLoadMetaFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiLoadPublicFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiLoadPublicFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiMarker(HPS a, CONST POINTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiMarker(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiModifyPath(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiModifyPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiMove(HPS a, CONST POINTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiMove(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiOffsetClipRegion(HPS a, CONST POINTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiOffsetClipRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiOffsetElementPointer(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiOffsetElementPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiOffsetRegion(HPS a, HRGN b, CONST POINTL *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiOffsetRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiOutlinePath(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiOutlinePath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPaintRegion(HPS a, HRGN b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPaintRegion(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPartialArc(HPS a, CONST POINTL *b, FIXED c, FIXED d, FIXED e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPartialArc(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HRGN APIENTRY GpiPathToRegion(HPS a, LONG b, LONG c)
{
 HRGN yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPathToRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPlayMetaFile(HPS a, HMF b, LONG c, CONST LONG *d, PLONG e, LONG f, PSZ g)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPlayMetaFile(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPointArc(HPS a, CONST POINTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPointArc(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPolyFillet(HPS a, LONG b, CONST POINTL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPolyFillet(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPolyFilletSharp(HPS a, LONG b, CONST POINTL *c, CONST FIXED *d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPolyFilletSharp(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPolygons(HPS a, ULONG b, CONST POLYGON *c, ULONG d, ULONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPolygons(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPolyLine(HPS a, LONG b, CONST POINTL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPolyLine(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPolyLineDisjoint(HPS a, LONG b, CONST POINTL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPolyLineDisjoint(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPolyMarker(HPS a, LONG b, CONST POINTL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPolyMarker(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPolySpline(HPS a, LONG b, CONST POINTL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPolySpline(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiPop(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPop(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPtInRegion(HPS a, HRGN b, CONST POINTL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPtInRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPtVisible(HPS a, CONST POINTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPtVisible(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryArcParams(HPS a, PARCPARAMS b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryAttrMode(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryAttrMode(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryAttrs(HPS a, LONG b, ULONG c, PBUNDLE d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryBackColor(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBackColor(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryBackMix(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBackMix(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCharAngle(HPS a, PGRADIENTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharAngle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCharBox(HPS a, PSIZEF b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCharBreakExtra(HPS a, PFIXED b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharBreakExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryCharDirection(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharDirection(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCharExtra(HPS a, PFIXED b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryCharMode(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharMode(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryCharSet(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharSet(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCharShear(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharShear(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCharStringPos(HPS a, ULONG b, LONG c, PCH d, PLONG e, PPOINTL f)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharStringPos(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCharStringPosAt(HPS a, PPOINTL b, ULONG c, LONG d, PCH e, PLONG f, PPOINTL g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCharStringPosAt(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryClipBox(HPS a, PRECTL b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryClipBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HRGN APIENTRY GpiQueryClipRegion(HPS a)
{
 HRGN yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryClipRegion(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryColor(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryColor(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryColorData(HPS a, LONG b, PLONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryColorData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryColorIndex(HPS a, ULONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryColorIndex(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY GpiQueryCp(HPS a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCp(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryCurrentPosition(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryCurrentPosition(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryDefArcParams(HPS a, PARCPARAMS b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDefArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryDefAttrs(HPS a, LONG b, ULONG c, PBUNDLE d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDefAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryDefCharBox(HPS a, PSIZEL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDefCharBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryDefTag(HPS a, PLONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDefTag(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryDefViewingLimits(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDefViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryDefaultViewMatrix(HPS a, LONG b, PMATRIXLF c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDefaultViewMatrix(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryEditMode(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryEditMode(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryElement(HPS a, LONG b, LONG c, PBYTE d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryElement(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryElementPointer(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryElementPointer(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryElementType(HPS a, PLONG b, LONG c, PSZ d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryElementType(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY GpiQueryFaceString(HPS a, PCSZ b, PFACENAMEDESC c, LONG d, PSZ e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryFaceString(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY GpiQueryFontAction(HAB a, ULONG b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryFontAction(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryFontFileDescriptions(HAB a, PCSZ b, PLONG c, PFFDESCS d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryFontFileDescriptions(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryFontMetrics(HPS a, LONG b, PFONTMETRICS c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryFontMetrics(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryFonts(HPS a, ULONG b, PCSZ c, PLONG d, LONG e, PFONTMETRICS f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryFonts(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryFullFontFileDescs(HAB a, PCSZ b, PLONG c, PVOID d, PLONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryFullFontFileDescs(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryGraphicsField(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryGraphicsField(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryKerningPairs(HPS a, LONG b, PKERNINGPAIRS c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryKerningPairs(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryLineEnd(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryLineEnd(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryLineJoin(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryLineJoin(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryLineType(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryLineType(a);
    SetFS(sel);

    return yyrc;
} 

inline FIXED APIENTRY GpiQueryLineWidth(HPS a)
{
 FIXED yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryLineWidth(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryLineWidthGeom(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryLineWidthGeom(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryLogColorTable(HPS a, ULONG b, LONG c, LONG d, PLONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryLogColorTable(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryLogicalFont(HPS a, LONG b, PSTR8 c, PFATTRS d, LONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryLogicalFont(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryMarker(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryMarker(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryMarkerBox(HPS a, PSIZEF b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryMarkerBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryMarkerSet(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryMarkerSet(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryMetaFileBits(HMF a, LONG b, LONG c, PBYTE d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryMetaFileBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryMetaFileLength(HMF a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryMetaFileLength(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryMix(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryMix(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryModelTransformMatrix(HPS a, LONG b, PMATRIXLF c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryModelTransformMatrix(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryNearestColor(HPS a, ULONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryNearestColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryNumberSetIds(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryNumberSetIds(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryPageViewport(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPageViewport(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HPAL APIENTRY GpiQueryPalette(HPS a)
{
 HPAL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPalette(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryPaletteInfo(HPAL a, HPS b, ULONG c, ULONG d, ULONG e, PULONG f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPaletteInfo(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryPattern(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPattern(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryPatternRefPoint(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPatternRefPoint(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryPatternSet(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPatternSet(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryRealColors(HPS a, ULONG b, LONG c, LONG d, PLONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryRealColors(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryRegionBox(HPS a, HRGN b, PRECTL c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryRegionBox(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryRegionRects(HPS a, HRGN b, PRECTL c, PRGNRECT d, PRECTL e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryRegionRects(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryRGBColor(HPS a, ULONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryRGBColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQuerySegmentTransformMatrix(HPS a, LONG b, LONG c, PMATRIXLF d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQuerySegmentTransformMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQuerySetIds(HPS a, LONG b, PLONG c, PSTR8 d, PLONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQuerySetIds(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryTextAlignment(HPS a, PLONG b, PLONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryTextAlignment(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryTextBox(HPS a, LONG b, PCH c, LONG d, PPOINTL e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryTextBox(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryViewingLimits(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryViewingTransformMatrix(HPS a, LONG b, PMATRIXLF c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryViewingTransformMatrix(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryWidthTable(HPS a, LONG b, LONG c, PLONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryWidthTable(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiRectInRegion(HPS a, HRGN b, CONST RECTL *c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiRectInRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiRectVisible(HPS a, CONST RECTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiRectVisible(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiRotate(HPS a, PMATRIXLF b, LONG c, FIXED d, CONST POINTL *e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiRotate(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSaveMetaFile(HMF a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSaveMetaFile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiScale(HPS a, PMATRIXLF b, LONG c, CONST FIXED *d, CONST POINTL *e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiScale(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HPAL APIENTRY GpiSelectPalette(HPS a, HPAL b)
{
 HPAL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSelectPalette(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetArcParams(HPS a, CONST ARCPARAMS *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetAttrMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetAttrMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetAttrs(HPS a, LONG b, ULONG c, ULONG d, CONST VOID *e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetAttrs(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetBackColor(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetBackColor(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetBackMix(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetBackMix(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharAngle(HPS a, CONST GRADIENTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharAngle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharBox(HPS a, CONST SIZEF *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharBreakExtra(HPS a, FIXED b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharBreakExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharDirection(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharDirection(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharExtra(HPS a, FIXED b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharExtra(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharSet(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharSet(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCharShear(HPS a, CONST POINTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCharShear(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetClipPath(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetClipPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiSetClipRegion(HPS a, HRGN b, PHRGN c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetClipRegion(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetColor(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetColor(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCp(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCp(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetCurrentPosition(HPS a, CONST POINTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetCurrentPosition(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetDefArcParams(HPS a, CONST ARCPARAMS *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetDefArcParams(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetDefAttrs(HPS a, LONG b, ULONG c, CONST VOID *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetDefAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetDefaultViewMatrix(HPS a, LONG b, CONST MATRIXLF *c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetDefaultViewMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetDefTag(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetDefTag(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetDefViewingLimits(HPS a, CONST RECTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetDefViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetEditMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetEditMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetElementPointer(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetElementPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetElementPointerAtLabel(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetElementPointerAtLabel(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetGraphicsField(HPS a, CONST RECTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetGraphicsField(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetLineEnd(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetLineEnd(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetLineJoin(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetLineJoin(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetLineType(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetLineType(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetLineWidth(HPS a, FIXED b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetLineWidth(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetLineWidthGeom(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetLineWidthGeom(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetMarker(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetMarker(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetMarkerBox(HPS a, CONST SIZEF *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetMarkerBox(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetMarkerSet(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetMarkerSet(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetMetaFileBits(HMF a, LONG b, LONG c, CONST BYTE *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetMetaFileBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetMix(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetMix(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetModelTransformMatrix(HPS a, LONG b, CONST MATRIXLF *c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetModelTransformMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPageViewport(HPS a, CONST RECTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPageViewport(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPaletteEntries(HPAL a, ULONG b, ULONG c, ULONG d, CONST ULONG *e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPaletteEntries(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPattern(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPattern(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPatternRefPoint(HPS a, CONST POINTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPatternRefPoint(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPatternSet(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPatternSet(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetRegion(HPS a, HRGN b, LONG c, CONST RECTL *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetRegion(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetSegmentTransformMatrix(HPS a, LONG b, LONG c, CONST MATRIXLF *d, LONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetSegmentTransformMatrix(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetTextAlignment(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetTextAlignment(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetViewingLimits(HPS a, CONST RECTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetViewingLimits(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetViewingTransformMatrix(HPS a, LONG b, CONST MATRIXLF *c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetViewingTransformMatrix(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiStrokePath(HPS a, LONG b, ULONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiStrokePath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiTranslate(HPS a, PMATRIXLF b, LONG c, CONST POINTL *d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiTranslate(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiUnloadFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiUnloadFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiUnloadPublicFonts(HAB a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiUnloadPublicFonts(a, b);
    SetFS(sel);

    return yyrc;
} 

#ifdef INCL_GPIBITMAPS
inline LONG APIENTRY GpiBitBlt(HPS a, HPS b, LONG c, CONST POINTL *d, LONG e, ULONG f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiBitBlt(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteBitmap(HBITMAP a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteBitmap(a);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP APIENTRY GpiLoadBitmap(HPS a, HMODULE b, ULONG c, LONG d, LONG e)
{
 HBITMAP yyrc;
 USHORT sel = GetFS();

    yyrc = GpiLoadBitmap(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP APIENTRY GpiSetBitmap(HPS a, HBITMAP b)
{
 HBITMAP yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetBitmap(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiWCBitBlt(HPS a, HBITMAP b, LONG c, CONST POINTL *d, LONG e, ULONG f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiWCBitBlt(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP APIENTRY GpiCreateBitmap(HPS a, CONST BITMAPINFOHEADER2 *b, ULONG c, CONST BYTE *d, CONST BITMAPINFO2 *e)
{
 HBITMAP yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCreateBitmap(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiDrawBits(HPS a, CONST VOID *b, CONST BITMAPINFO2 *c, LONG d, CONST POINTL *e, LONG f, ULONG g)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDrawBits(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiFloodFill(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiFloodFill(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryBitmapBits(HPS a, LONG b, LONG c, PBYTE d, PBITMAPINFO2 e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBitmapBits(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryBitmapDimension(HBITMAP a, PSIZEL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBitmapDimension(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HBITMAP APIENTRY GpiQueryBitmapHandle(HPS a, LONG b)
{
 HBITMAP yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBitmapHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryBitmapInfoHeader(HBITMAP a, PBITMAPINFOHEADER2 b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBitmapInfoHeader(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryBitmapParameters(HBITMAP a, PBITMAPINFOHEADER b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBitmapParameters(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryDeviceBitmapFormats(HPS a, LONG b, PLONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDeviceBitmapFormats(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiSetBitmapBits(HPS a, LONG b, LONG c, CONST BYTE *d, CONST BITMAPINFO2 *e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetBitmapBits(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryPel(HPS a, PPOINTL b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPel(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetBitmapDimension(HBITMAP a, CONST SIZEL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetBitmapDimension(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetBitmapId(HPS a, HBITMAP b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetBitmapId(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiSetPel(HPS a, CONST POINTL *b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPel(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_GPICONTROL
inline BOOL APIENTRY GpiAssociate(HPS a, HDC b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiAssociate(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HPS APIENTRY GpiCreatePS(HAB a, HDC b, PSIZEL c, ULONG d)
{
 HPS yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCreatePS(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDestroyPS(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDestroyPS(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiErase(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiErase(a);
    SetFS(sel);

    return yyrc;
} 

inline HDC APIENTRY GpiQueryDevice(HPS a)
{
 HDC yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDevice(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiRestorePS(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiRestorePS(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiSavePS(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSavePS(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiErrorSegmentData(HPS a, PLONG b, PLONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiErrorSegmentData(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryDrawControl(HPS a, LONG b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDrawControl(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryDrawingMode(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryDrawingMode(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY GpiQueryPS(HPS a, PSIZEL b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPS(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiResetPS(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiResetPS(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryStopDraw(HPS a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryStopDraw(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetDrawControl(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetDrawControl(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetDrawingMode(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetDrawingMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPS(HPS a, CONST SIZEL *b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPS(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetStopDraw(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetStopDraw(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_GPICORRELATION
inline LONG APIENTRY GpiCorrelateChain(HPS a, LONG b, CONST POINTL *c, LONG d, LONG e, PLONG f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCorrelateChain(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCorrelateFrom(HPS a, LONG b, LONG c, LONG d, CONST POINTL *e, LONG f, LONG g, PLONG h)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCorrelateFrom(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiCorrelateSegment(HPS a, LONG b, LONG c, CONST POINTL *d, LONG e, LONG f, PLONG g)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCorrelateSegment(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryBoundaryData(HPS a, PRECTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryBoundaryData(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryPickAperturePosition(HPS a, PPOINTL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPickAperturePosition(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryPickApertureSize(HPS a, PSIZEL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryPickApertureSize(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiQueryTag(HPS a, PLONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryTag(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiResetBoundaryData(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiResetBoundaryData(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPickAperturePosition(HPS a, CONST POINTL *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPickAperturePosition(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetPickApertureSize(HPS a, LONG b, CONST SIZEL *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetPickApertureSize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetTag(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetTag(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_GPIINK
inline BOOL APIENTRY GpiBeginInkPath(HPS a, LONG b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiBeginInkPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiEndInkPath(HPS a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiEndInkPath(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiStrokeInkPath(HPS a, LONG b, LONG c, CONST POINTL *d, ULONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiStrokeInkPath(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_GPISEGMENTS
inline BOOL APIENTRY GpiCloseSegment(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiCloseSegment(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteSegment(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteSegment(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDeleteSegments(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDeleteSegments(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDrawChain(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDrawChain(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDrawDynamics(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDrawDynamics(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDrawFrom(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDrawFrom(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiDrawSegment(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiDrawSegment(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiGetData(HPS a, LONG b, PLONG c, LONG d, LONG e, PBYTE f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiGetData(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiOpenSegment(HPS a, LONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiOpenSegment(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiPutData(HPS a, LONG b, PLONG c, CONST BYTE *d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiPutData(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQueryInitialSegmentAttrs(HPS a, LONG b)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQueryInitialSegmentAttrs(a, b);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQuerySegmentAttrs(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQuerySegmentAttrs(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQuerySegmentNames(HPS a, LONG b, LONG c, LONG d, PLONG e)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQuerySegmentNames(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY GpiQuerySegmentPriority(HPS a, LONG b, LONG c)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = GpiQuerySegmentPriority(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiRemoveDynamics(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiRemoveDynamics(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetInitialSegmentAttrs(HPS a, LONG b, LONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetInitialSegmentAttrs(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetSegmentAttrs(HPS a, LONG b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetSegmentAttrs(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY GpiSetSegmentPriority(HPS a, LONG b, LONG c, LONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = GpiSetSegmentPriority(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
inline HMF APIENTRY DevCloseDC(HDC a)
{
 HMF yyrc;
 USHORT sel = GetFS();

    yyrc = DevCloseDC(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY DevEscape(HDC a, LONG b, LONG c, PBYTE d, PLONG e, PBYTE f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = DevEscape(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline HDC APIENTRY DevOpenDC(HAB a, LONG b, PCSZ c, LONG d, PDEVOPENDATA e, HDC f)
{
 HDC yyrc;
 USHORT sel = GetFS();

    yyrc = DevOpenDC(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY DevPostDeviceModes(HAB a, PDRIVDATA b, PCSZ c, PCSZ d, PCSZ e, ULONG f)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = DevPostDeviceModes(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DevQueryCaps(HDC a, LONG b, LONG c, PLONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DevQueryCaps(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DevQueryDeviceNames(HAB a, PCSZ b, PLONG c, PSTR32 d, PSTR64 e, PLONG f, PSTR16 g)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DevQueryDeviceNames(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY DevQueryHardcopyCaps(HDC a, LONG b, LONG c, PHCINFO d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = DevQueryHardcopyCaps(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINPROGRAMLIST
inline HPROGRAM APIENTRY PrfAddProgram(HINI a, PPROGDETAILS b, HPROGRAM c)
{
 HPROGRAM yyrc;
 USHORT sel = GetFS();

    yyrc = PrfAddProgram(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfChangeProgram(HINI a, HPROGRAM b, PPROGDETAILS c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfChangeProgram(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HPROGRAM APIENTRY PrfCreateGroup(HINI a, PCSZ b, UCHAR c)
{
 HPROGRAM yyrc;
 USHORT sel = GetFS();

    yyrc = PrfCreateGroup(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfDestroyGroup(HINI a, HPROGRAM b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfDestroyGroup(a, b);
    SetFS(sel);

    return yyrc;
} 

inline PROGCATEGORY APIENTRY PrfQueryProgramCategory(HINI a, PCSZ b)
{
 PROGCATEGORY yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProgramCategory(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY PrfQueryProgramHandle(HINI a, PCSZ b, PHPROGARRAY c, ULONG d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProgramHandle(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY PrfQueryProgramTitles(HINI a, HPROGRAM b, PPROGTITLE c, ULONG d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProgramTitles(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY PrfQueryDefinition(HINI a, HPROGRAM b, PPROGDETAILS c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryDefinition(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfRemoveProgram(HINI a, HPROGRAM b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfRemoveProgram(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HAPP APIENTRY WinStartApp(HWND a, PPROGDETAILS b, PCSZ c, PVOID d, ULONG e)
{
 HAPP yyrc;
 USHORT sel = GetFS();

    yyrc = WinStartApp(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinTerminateApp(HAPP a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinTerminateApp(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINSWITCHLIST
inline HSWITCH APIENTRY WinAddSwitchEntry(CONST SWCNTRL *a)
{
 HSWITCH yyrc;
 USHORT sel = GetFS();

    yyrc = WinAddSwitchEntry(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinRemoveSwitchEntry(HSWITCH a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinRemoveSwitchEntry(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinChangeSwitchEntry(HSWITCH a, CONST SWCNTRL *b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinChangeSwitchEntry(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HSWITCH APIENTRY WinCreateSwitchEntry(HAB a, CONST SWCNTRL *b)
{
 HSWITCH yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateSwitchEntry(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQuerySessionTitle(HAB a, ULONG b, PSZ c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySessionTitle(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQuerySwitchEntry(HSWITCH a, PSWCNTRL b)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySwitchEntry(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HSWITCH APIENTRY WinQuerySwitchHandle(HWND a, PID b)
{
 HSWITCH yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySwitchHandle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQuerySwitchList(HAB a, PSWBLOCK b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQuerySwitchList(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryTaskSizePos(HAB a, ULONG b, PSWP c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryTaskSizePos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinQueryTaskTitle(ULONG a, PSZ b, ULONG c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryTaskTitle(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinSwitchToProgram(HSWITCH a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinSwitchToProgram(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINSHELLDATA
inline BOOL APIENTRY PrfCloseProfile(HINI a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfCloseProfile(a);
    SetFS(sel);

    return yyrc;
} 

inline HINI APIENTRY PrfOpenProfile(HAB a, PCSZ b)
{
 HINI yyrc;
 USHORT sel = GetFS();

    yyrc = PrfOpenProfile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfQueryProfile(HAB a, PPRFPROFILE b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProfile(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfQueryProfileData(HINI a, PCSZ b, PCSZ c, PVOID d, PULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProfileData(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY PrfQueryProfileInt(HINI a, PCSZ b, PCSZ c, LONG d)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProfileInt(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfQueryProfileSize(HINI a, PCSZ b, PCSZ c, PULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProfileSize(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY PrfQueryProfileString(HINI a, PCSZ b, PCSZ c, PCSZ d, PVOID e, ULONG f)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = PrfQueryProfileString(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfReset(HAB a, CONST PrfPROFILE *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfReset(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfWriteProfileData(HINI a, PCSZ b, PCSZ c, PVOID d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfWriteProfileData(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY PrfWriteProfileString(HINI a, PCSZ b, PCSZ c, PCSZ d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = PrfWriteProfileString(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINSTDFILE
inline MRESULT APIENTRY WinDefFileDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinDefFileDlgProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinFileDlg(HWND a, HWND b, PFILEDLG c)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinFileDlg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinFreeFileDlgList(PAPSZ a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinFreeFileDlgList(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINSTDFONT
inline HWND APIENTRY WinFontDlg(HWND a, HWND b, PFONTDLG c)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinFontDlg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY WinDefFontDlgProc(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinDefFontDlgProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINSTDDRAG
inline BOOL APIENTRY DrgAcceptDroppedFiles(HWND a, PCSZ b, PCSZ c, ULONG d, ULONG e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgAcceptDroppedFiles(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgAccessDraginfo(PDRAGINFO a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgAccessDraginfo(a);
    SetFS(sel);

    return yyrc;
} 

inline HSTR APIENTRY DrgAddStrHandle(PCSZ a)
{
 HSTR yyrc;
 USHORT sel = GetFS();

    yyrc = DrgAddStrHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline PDRAGINFO APIENTRY DrgAllocDraginfo(ULONG a)
{
 PDRAGINFO yyrc;
 USHORT sel = GetFS();

    yyrc = DrgAllocDraginfo(a);
    SetFS(sel);

    return yyrc;
} 

inline PDRAGTRANSFER APIENTRY DrgAllocDragtransfer(ULONG a)
{
 PDRAGTRANSFER yyrc;
 USHORT sel = GetFS();

    yyrc = DrgAllocDragtransfer(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgCancelLazyDrag()
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgCancelLazyDrag();
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgDeleteDraginfoStrHandles(PDRAGINFO a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgDeleteDraginfoStrHandles(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgDeleteStrHandle(HSTR a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgDeleteStrHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY DrgDrag(HWND a, PDRAGINFO b, PDRAGIMAGE c, ULONG d, LONG e, PVOID f)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = DrgDrag(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgDragFiles(HWND a, PSZ *b, PSZ *c, PSZ *d, ULONG e, HPOINTER f, ULONG g, BOOL h, ULONG i)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgDragFiles(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgFreeDraginfo(PDRAGINFO a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgFreeDraginfo(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgFreeDragtransfer(PDRAGTRANSFER a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgFreeDragtransfer(a);
    SetFS(sel);

    return yyrc;
} 

inline HPS APIENTRY DrgGetPS(HWND a)
{
 HPS yyrc;
 USHORT sel = GetFS();

    yyrc = DrgGetPS(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgLazyDrag(HWND a, PDRAGINFO b, PDRAGIMAGE c, ULONG d, PVOID e)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgLazyDrag(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgLazyDrop(HWND a, ULONG b, PPOINTL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgLazyDrop(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgPostTransferMsg(HWND a, ULONG b, PDRAGTRANSFER c, ULONG d, ULONG e, BOOL f)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgPostTransferMsg(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgPushDraginfo(PDRAGINFO a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgPushDraginfo(a, b);
    SetFS(sel);

    return yyrc;
} 

inline PDRAGINFO APIENTRY DrgQueryDraginfoPtr(PDRAGINFO a)
{
 PDRAGINFO yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryDraginfoPtr(a);
    SetFS(sel);

    return yyrc;
} 

inline PDRAGINFO APIENTRY DrgQueryDraginfoPtrFromHwnd(HWND a)
{
 PDRAGINFO yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryDraginfoPtrFromHwnd(a);
    SetFS(sel);

    return yyrc;
} 

inline PDRAGINFO APIENTRY DrgQueryDraginfoPtrFromDragitem(CONST DRAGITEM *a)
{
 PDRAGINFO yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryDraginfoPtrFromDragitem(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgQueryDragitem(PDRAGINFO a, ULONG b, PDRAGITEM c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryDragitem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DrgQueryDragitemCount(PDRAGINFO a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryDragitemCount(a);
    SetFS(sel);

    return yyrc;
} 

inline PDRAGITEM APIENTRY DrgQueryDragitemPtr(PDRAGINFO a, ULONG b)
{
 PDRAGITEM yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryDragitemPtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DrgQueryDragStatus()
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryDragStatus();
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgQueryNativeRMF(PDRAGITEM a, ULONG b, PCHAR c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryNativeRMF(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DrgQueryNativeRMFLen(PDRAGITEM a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryNativeRMFLen(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DrgQueryStrName(HSTR a, ULONG b, PSZ c)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryStrName(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DrgQueryStrNameLen(HSTR a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryStrNameLen(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgQueryTrueType(PDRAGITEM a, ULONG b, PSZ c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryTrueType(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY DrgQueryTrueTypeLen(PDRAGITEM a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = DrgQueryTrueTypeLen(a);
    SetFS(sel);

    return yyrc;
} 

inline PDRAGINFO APIENTRY DrgReallocDraginfo(PDRAGINFO a, ULONG b)
{
 PDRAGINFO yyrc;
 USHORT sel = GetFS();

    yyrc = DrgReallocDraginfo(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgReleasePS(HPS a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgReleasePS(a);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY DrgSendTransferMsg(HWND a, ULONG b, MPARAM c, MPARAM d)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = DrgSendTransferMsg(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgSetDragImage(PDRAGINFO a, PDRAGIMAGE b, ULONG c, PVOID d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgSetDragImage(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgSetDragitem(PDRAGINFO a, PDRAGITEM b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgSetDragitem(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgSetDragPointer(PDRAGINFO a, HPOINTER b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgSetDragPointer(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgVerifyNativeRMF(PDRAGITEM a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgVerifyNativeRMF(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgVerifyRMF(PDRAGITEM a, PCSZ b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgVerifyRMF(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgVerifyTrueType(PDRAGITEM a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgVerifyTrueType(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgVerifyType(PDRAGITEM a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgVerifyType(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DrgVerifyTypeSet(PDRAGITEM a, PCSZ b, ULONG c, PSZ d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DrgVerifyTypeSet(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WPCLASS
inline HOBJECT APIENTRY WinCopyObject(HOBJECT a, HOBJECT b, ULONG c)
{
 HOBJECT yyrc;
 USHORT sel = GetFS();

    yyrc = WinCopyObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HOBJECT APIENTRY WinCreateObject(PCSZ a, PCSZ b, PCSZ c, PCSZ d, ULONG e)
{
 HOBJECT yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateObject(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline HOBJECT APIENTRY WinCreateShadow(HOBJECT a, HOBJECT b, ULONG c)
{
 HOBJECT yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateShadow(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDeregisterObjectClass(PCSZ a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDeregisterObjectClass(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDestroyObject(HOBJECT a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyObject(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinEnumObjectClasses(POBJCLASS a, PULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinEnumObjectClasses(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsSOMDDReady()
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsSOMDDReady();
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinIsWPDServerReady()
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinIsWPDServerReady();
    SetFS(sel);

    return yyrc;
} 

inline HOBJECT APIENTRY WinMoveObject(HOBJECT a, HOBJECT b, ULONG c)
{
 HOBJECT yyrc;
 USHORT sel = GetFS();

    yyrc = WinMoveObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinOpenObject(HOBJECT a, ULONG b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinOpenObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryActiveDesktopPathname(PSZ a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryActiveDesktopPathname(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HOBJECT APIENTRY WinQueryObject(PCSZ a)
{
 HOBJECT yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryObject(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinQueryObjectPath(HOBJECT a, PSZ b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryObjectPath(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinRegisterObjectClass(PCSZ a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinRegisterObjectClass(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinReplaceObjectClass(PCSZ a, PCSZ b, BOOL c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinReplaceObjectClass(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinRestartSOMDD(BOOL a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinRestartSOMDD(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY WinRestartWPDServer(BOOL a)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = WinRestartWPDServer(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSaveObject(HOBJECT a, BOOL b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSaveObject(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetObjectData(HOBJECT a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetObjectData(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinFreeFileIcon(HPOINTER a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinFreeFileIcon(a);
    SetFS(sel);

    return yyrc;
} 

inline HPOINTER APIENTRY WinLoadFileIcon(PCSZ a, BOOL b)
{
 HPOINTER yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadFileIcon(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinRestoreWindowPos(PCSZ a, PCSZ b, HWND c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinRestoreWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinSetFileIcon(PCSZ a, CONST ICONINFO *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinSetFileIcon(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinShutdownSystem(HAB a, HMQ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinShutdownSystem(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinStoreWindowPos(PCSZ a, PCSZ b, HWND c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinStoreWindowPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_SPL
inline BOOL APIENTRY SplStdClose(HDC a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplStdClose(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplStdDelete(HSTD a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplStdDelete(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplStdGetBits(HSTD a, LONG b, LONG c, PCH d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplStdGetBits(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplStdOpen(HDC a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplStdOpen(a);
    SetFS(sel);

    return yyrc;
} 

inline LONG APIENTRY SplStdQueryLength(HSTD a)
{
 LONG yyrc;
 USHORT sel = GetFS();

    yyrc = SplStdQueryLength(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplStdStart(HDC a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplStdStart(a);
    SetFS(sel);

    return yyrc;
} 

inline HSTD APIENTRY SplStdStop(HDC a)
{
 HSTD yyrc;
 USHORT sel = GetFS();

    yyrc = SplStdStop(a);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplControlDevice(PSZ a, PSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplControlDevice(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplCopyJob(PCSZ a, PCSZ b, ULONG c, PCSZ d, PCSZ e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplCopyJob(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplCreateDevice(PSZ a, ULONG b, PVOID c, ULONG d)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplCreateDevice(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplCreatePort(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplCreatePort(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplCreateQueue(PSZ a, ULONG b, PVOID c, ULONG d)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplCreateQueue(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplDeleteDevice(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplDeleteDevice(a, b);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplDeleteJob(PSZ a, PSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplDeleteJob(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplDeletePort(PCSZ a, PCSZ b)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplDeletePort(a, b);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplDeleteQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplDeleteQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplEnumDevice(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplEnumDevice(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplEnumDriver(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplEnumDriver(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplEnumJob(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f, PULONG g, PULONG h, PVOID i)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplEnumJob(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplEnumPort(PCSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplEnumPort(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplEnumPrinter(PSZ a, ULONG b, ULONG c, PVOID d, ULONG e, PULONG f, PULONG g, PULONG h, PVOID i)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplEnumPrinter(a, b, c, d, e, f, g, h, i);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplEnumQueue(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplEnumQueue(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplEnumQueueProcessor(PSZ a, ULONG b, PVOID c, ULONG d, PULONG e, PULONG f, PULONG g, PVOID h)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplEnumQueueProcessor(a, b, c, d, e, f, g, h);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplHoldJob(PCSZ a, PCSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplHoldJob(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplHoldQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplHoldQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplPurgeQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplPurgeQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplQueryDevice(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplQueryDevice(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplQueryDriver(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f, PULONG g)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplQueryDriver(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplQueryJob(PSZ a, PSZ b, ULONG c, ULONG d, PVOID e, ULONG f, PULONG g)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplQueryJob(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplQueryPort(PCSZ a, PCSZ b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplQueryPort(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplQueryQueue(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, PULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplQueryQueue(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplReleaseJob(PCSZ a, PCSZ b, ULONG c)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplReleaseJob(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplReleaseQueue(PSZ a, PSZ b)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplReleaseQueue(a, b);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplSetDevice(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplSetDevice(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplSetDriver(PCSZ a, PCSZ b, PCSZ c, ULONG d, PVOID e, ULONG f, ULONG g)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplSetDriver(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplSetJob(PSZ a, PSZ b, ULONG c, ULONG d, PVOID e, ULONG f, ULONG g)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplSetJob(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplSetPort(PCSZ a, PCSZ b, ULONG c, PVOID d, ULONG e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplSetPort(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline SPLERR APIENTRY SplSetQueue(PSZ a, PSZ b, ULONG c, PVOID d, ULONG e, ULONG f)
{
 SPLERR yyrc;
 USHORT sel = GetFS();

    yyrc = SplSetQueue(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY SplMessageBox(PSZ a, ULONG b, ULONG c, PSZ d, PSZ e, ULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = SplMessageBox(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplQmAbort(HSPL a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmAbort(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplQmAbortDoc(HSPL a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmAbortDoc(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplQmClose(HSPL a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmClose(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplQmEndDoc(HSPL a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmEndDoc(a);
    SetFS(sel);

    return yyrc;
} 

inline ULONG APIENTRY SplQmGetJobID(HSPL a, ULONG b, PVOID c, ULONG d, PULONG e)
{
 ULONG yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmGetJobID(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplQmNewPage(HSPL a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmNewPage(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HSPL APIENTRY SplQmOpen(PSZ a, LONG b, PQMOPENDATA c)
{
 HSPL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmOpen(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplQmStartDoc(HSPL a, PSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmStartDoc(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY SplQmWrite(HSPL a, LONG b, PVOID c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = SplQmWrite(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_WINHELP
inline BOOL APIENTRY WinAssociateHelpInstance(HWND a, HWND b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinAssociateHelpInstance(a, b);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinCreateHelpInstance(HAB a, PHELPINIT b)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateHelpInstance(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinCreateHelpTable(HWND a, CONST HELPTABLE *b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinCreateHelpTable(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinDestroyHelpInstance(HWND a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinDestroyHelpInstance(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY WinLoadHelpTable(HWND a, ULONG b, HMODULE c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = WinLoadHelpTable(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HWND APIENTRY WinQueryHelpInstance(HWND a)
{
 HWND yyrc;
 USHORT sel = GetFS();

    yyrc = WinQueryHelpInstance(a);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_DDF
inline BOOL APIENTRY DdfBeginList(HDDF a, ULONG b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfBeginList(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfBitmap(HDDF a, HBITMAP b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfBitmap(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfEndList(HDDF a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfEndList(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfHyperText(HDDF a, PCSZ b, PCSZ c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfHyperText(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfInform(HDDF a, PCSZ b, ULONG c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfInform(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline HDDF APIENTRY DdfInitialize(HWND a, ULONG b, ULONG c)
{
 HDDF yyrc;
 USHORT sel = GetFS();

    yyrc = DdfInitialize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfListItem(HDDF a, PCSZ b, PCSZ c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfListItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfMetafile(HDDF a, HMF b, CONST RECTL *c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfMetafile(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfPara(HDDF a)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfPara(a);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfSetColor(HDDF a, COLOR b, COLOR c)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfSetColor(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfSetFont(HDDF a, PCSZ b, ULONG c, ULONG d)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfSetFont(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfSetFontStyle(HDDF a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfSetFontStyle(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfSetFormat(HDDF a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfSetFormat(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfSetTextAlign(HDDF a, ULONG b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfSetTextAlign(a, b);
    SetFS(sel);

    return yyrc;
} 

inline BOOL APIENTRY DdfText(HDDF a, PCSZ b)
{
 BOOL yyrc;
 USHORT sel = GetFS();

    yyrc = DdfText(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_AVIO
inline USHORT APIENTRY VioAssociate(HDC a, HVPS b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioAssociate(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioCreateLogFont(PFATTRS a, LONG b, PSTR8 c, HVPS d)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioCreateLogFont(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioCreatePS(PHVPS a, SHORT b, SHORT c, SHORT d, SHORT e, HVPS f)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioCreatePS(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioDeleteSetId(LONG a, HVPS b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioDeleteSetId(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioDestroyPS(HVPS a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioDestroyPS(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetDeviceCellSize(PSHORT a, PSHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetDeviceCellSize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetOrg(PSHORT a, PSHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetOrg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioQueryFonts(PLONG a, PFONTMETRICS b, LONG c, PLONG d, PSZ e, ULONG f, HVPS g)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioQueryFonts(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioQuerySetIds(PLONG a, PSTR8 b, PLONG c, LONG d, HVPS e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioQuerySetIds(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetDeviceCellSize(SHORT a, SHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetDeviceCellSize(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetOrg(SHORT a, SHORT b, HVPS c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetOrg(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioShowPS(SHORT a, SHORT b, SHORT c, HVPS d)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioShowPS(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline MRESULT APIENTRY16 WinDefAVioWindowProc(HWND a, USHORT b, ULONG c, ULONG d)
{
 MRESULT yyrc;
 USHORT sel = GetFS();

    yyrc = WinDefAVioWindowProc(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_KBD
inline USHORT APIENTRY KbdCharIn(PKBDKEYINFO a, USHORT b, HKBD c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdCharIn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdClose(HKBD a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdClose(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdDeRegister()
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdDeRegister();
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdFlushBuffer(HKBD a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdFlushBuffer(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdFreeFocus(HKBD a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdFreeFocus(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdGetCp(ULONG a, PUSHORT b, HKBD c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdGetCp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdGetFocus(USHORT a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdGetFocus(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdGetHWID(PKBDHWID a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdGetHWID(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdGetStatus(PKBDINFO a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdGetStatus(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdOpen(PHKBD a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdOpen(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdPeek(PKBDKEYINFO a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdPeek(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdRegister(PCSZ a, PCSZ b, ULONG c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdRegister(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdSetCp(USHORT a, USHORT b, HKBD c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdSetCp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdSetCustXt(PUSHORT a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdSetCustXt(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdSetFgnd()
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdSetFgnd();
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdSetHWID(PKBDHWID a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdSetHWID(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdSetStatus(PKBDINFO a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdSetStatus(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdStringIn(PCH a, PSTRINGINBUF b, USHORT c, HKBD d)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdStringIn(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdSynch(USHORT a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdSynch(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 KbdXlate(PKBDTRANS a, HKBD b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = KbdXlate(a, b);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_VIO
inline USHORT APIENTRY16 VioCheckCharType(PUSHORT a, USHORT b, USHORT c, HVIO d)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioCheckCharType(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioDeRegister()
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioDeRegister();
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioEndPopUp(HVIO a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioEndPopUp(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetAnsi(PUSHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetAnsi(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetBuf(PULONG a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetBuf(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetConfig(USHORT a, PVIOCONFIGINFO b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetConfig(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetCp(USHORT a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetCp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetCurPos(PUSHORT a, PUSHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetCurPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetCurType(PVIOCURSORINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetCurType(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetFont(PVIOFONTINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetFont(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetMode(PVIOMODEINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetPhysBuf(PVIOPHYSBUF a, USHORT b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetPhysBuf(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGetState(PVOID a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGetState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioGlobalReg(PCSZ a, PCSZ b, ULONG c, ULONG d, USHORT e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioGlobalReg(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioModeUndo(USHORT a, USHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioModeUndo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioModeWait(USHORT a, PUSHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioModeWait(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioPopUp(PUSHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioPopUp(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioPrtSc(HVIO a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioPrtSc(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioPrtScToggle(HVIO a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioPrtScToggle(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioReadCellStr(PCH a, PUSHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioReadCellStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioReadCharStr(PCH a, PUSHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioReadCharStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioRegister(PCSZ a, PCSZ b, ULONG c, ULONG d)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioRegister(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSavRedrawUndo(USHORT a, USHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSavRedrawUndo(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSavRedrawWait(USHORT a, PUSHORT b, USHORT c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSavRedrawWait(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioScrLock(USHORT a, PUCHAR b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioScrLock(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioScrollDn(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioScrollDn(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioScrollLf(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioScrollLf(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioScrollRt(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioScrollRt(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioScrollUp(USHORT a, USHORT b, USHORT c, USHORT d, USHORT e, PBYTE f, HVIO g)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioScrollUp(a, b, c, d, e, f, g);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioScrUnLock(HVIO a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioScrUnLock(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetAnsi(USHORT a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetAnsi(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetCp(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetCp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetCurPos(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetCurPos(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetCurType(PVIOCURSORINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetCurType(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetFont(PVIOFONTINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetFont(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetMode(PVIOMODEINFO a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetMode(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioSetState(PVOID a, HVIO b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioSetState(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioShowBuf(USHORT a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioShowBuf(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioWrtCellStr(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioWrtCellStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioWrtCharStr(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioWrtCharStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioWrtCharStrAtt(PCH a, USHORT b, USHORT c, USHORT d, PBYTE e, HVIO f)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioWrtCharStrAtt(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioWrtNAttr(CONST BYTE *a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioWrtNAttr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioWrtNCell(CONST BYTE *a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioWrtNCell(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioWrtNChar(PCH a, USHORT b, USHORT c, USHORT d, HVIO e)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioWrtNChar(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 VioWrtTTY(PCH a, USHORT b, HVIO c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = VioWrtTTY(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#ifdef INCL_MOU
inline USHORT APIENTRY16 MouClose(HMOU a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouClose(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouDeRegister()
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouDeRegister();
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouDrawPtr(HMOU a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouDrawPtr(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouFlushQue(HMOU a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouFlushQue(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetDevStatus(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetDevStatus(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetEventMask(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetEventMask(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetNumButtons(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetNumButtons(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetNumMickeys(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetNumMickeys(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetNumQueEl(PMOUQUEINFO a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetNumQueEl(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetPtrPos(PPTRLOC a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetPtrPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetPtrShape(PBYTE a, PPTRSHAPE b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetPtrShape(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetScaleFact(PSCALEFACT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetScaleFact(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouGetThreshold(PTHRESHOLD a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouGetThreshold(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouInitReal(PCSZ a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouInitReal(a);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouOpen(PCSZ a, PHMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouOpen(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouReadEventQue(PMOUEVENTINFO a, PUSHORT b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouReadEventQue(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouRegister(PCSZ a, PCSZ b, ULONG c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouRegister(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouRemovePtr(PNOPTRRECT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouRemovePtr(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouSetDevStatus(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouSetDevStatus(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouSetEventMask(PUSHORT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouSetEventMask(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouSetPtrPos(PPTRLOC a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouSetPtrPos(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouSetPtrShape(PBYTE a, PPTRSHAPE b, HMOU c)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouSetPtrShape(a, b, c);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouSetScaleFact(PSCALEFACT a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouSetScaleFact(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouSetThreshold(PTHRESHOLD a, HMOU b)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouSetThreshold(a, b);
    SetFS(sel);

    return yyrc;
} 

inline USHORT APIENTRY16 MouSynch(USHORT a)
{
 USHORT yyrc;
 USHORT sel = GetFS();

    yyrc = MouSynch(a);
    SetFS(sel);

    return yyrc;
} 

#endif

#endif