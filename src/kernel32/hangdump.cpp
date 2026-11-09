/*
 * Hang diagnosis for Win32 programs running under Odin: ODIN_HANGDUMP=<seconds>.
 *
 * Together with ODIN_TRACE=<file>: a helper thread wakes up after the given number of
 * seconds and writes, for every thread of the process, the instruction pointer and a
 * few return addresses (HANG-t<tid> and HANG-ra lines) to the trace file.  It repeats
 * the dump every 10 seconds, three times.  Only OS/2 native calls are used.
 */
#define INCL_DOSPROCESS
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSFILEMGR
#define INCL_DOSMODULEMGR
#include <os2wrap.h>
#include <string.h>
#include <stdlib.h>

/* serializes writes to the ODIN_TRACE file between threads (bounded spin: the owner may be suspended) */
volatile int g_odinTraceLock = 0;
volatile int g_odinTraceNoLock = 0;      /* set while threads are being suspended for a dump: the lock owner may be one of them */
void OdinTraceLockAcquire(void)
{
    int i;
    if(g_odinTraceNoLock) return;
    for(i = 0; i < 1500; i++) {
        if(!__sync_lock_test_and_set(&g_odinTraceLock, 1)) return;
        DosSleep(i < 20 ? 0 : 1);
    }
}
void OdinTraceLockRelease(void)
{
    __sync_lock_release(&g_odinTraceLock);
}

void OdinRawTrace(const char *tag, const char *str, unsigned long a, unsigned long b)
{
    /* plain OS/2 file write: this thread has no Win32 TIB, so the normal OdinTrace cannot be used */
    const char *fn = getenv("ODIN_TRACE");
    char buf[96];
    HFILE hf;
    ULONG act, n = 0, wr, i;
    if(!fn) return;
    if(tag[0] == 'T' && tag[1] == 'E' && tag[2] == 'R' && getenv("ODIN_NOTERMTRACE")) return;
    while(*tag && n < 40) buf[n++] = *tag++;
    buf[n++] = ' ';
    if(str) while(*str && n < 60) buf[n++] = *str++;
    buf[n++] = ' ';
    for(i = 0; i < 2; i++) {
        unsigned long v = i ? b : a;
        int j;
        for(j = 7; j >= 0; j--) buf[n++] = "0123456789ABCDEF"[(v >> (j * 4)) & 15];
        buf[n++] = ' ';
    }
    buf[n++] = 13; buf[n++] = 10;
    OdinTraceLockAcquire();
    if(DosOpen((PSZ)fn, &hf, &act, 0, 0, OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
               OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE, NULL) == 0) {
        DosSetFilePtr(hf, 0, FILE_END, &wr);
        DosWrite(hf, buf, n, &wr);
        DosClose(hf);
    }
    {
        OdinTraceLockRelease();
    }
}

extern "C" int OdinPeModOfAddr(ULONG addr, char *out, int max, ULONG *poff);
extern "C" void OdinPeModList(void (*cb)(const char *name, ULONG base, ULONG size));
static void hangModCb(const char *name, ULONG base, ULONG size) { OdinRawTrace("HANG-mod", name, base, size); }

/* "MODULE+offset" of an address (empty when it is in no module) */
static void hangModName(ULONG addr, char *out)
{
    HMODULE hmod;
    ULONG obj = 0, off = 0, n = 0;
    char name[40];
    int j;
    out[0] = 0;
    if(DosQueryModFromEIP(&hmod, &obj, sizeof(name), name, &off, (PVOID)addr) != 0) return;
    {   char *p = name, *base = name;
        for(; *p; p++) if(*p == 0x5c) base = p + 1;
        while(*base && n < 24) out[n++] = *base++;
    }
    out[n++] = 0x2b;
    for(j = 7; j >= 0; j--) out[n++] = "0123456789ABCDEF"[(off >> (j * 4)) & 15];
    out[n] = 0;
}

static int hangReadable(ULONG addr)
{
    ULONG size = 4, flags = 0;
    if(addr < 0x10000) return 0;
    if(DosQueryMem((PVOID)addr, &size, &flags) != 0) return 0;
    return (flags & PAG_READ) && (flags & PAG_COMMIT) && size >= 4;
}

static void APIENTRY hangDumpThread(ULONG secs)
{
    PTIB ptib;
    PPIB ppib;
    TID  self;
    int  round, tid, f;
    CONTEXTRECORD ctx;
    char tag[16];

    DosGetInfoBlocks(&ptib, &ppib);
    self = ptib->tib_ptib2->tib2_ultid;
    DosSleep(secs * 1000);
    OdinPeModList(hangModCb);
    g_odinTraceNoLock = 1;
    for(round = 0; round < 3; round++) {
        for(tid = 1; tid < 60; tid++) {
            if((TID)tid == self) continue;
            if(DosSuspendThread(tid) != 0) continue;
            memset(&ctx, 0, sizeof(ctx));
            ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
            if(DosQueryThreadContext(tid, 1, &ctx) == 0) {
                ULONG ebp = ctx.ctx_RegEbp;
                tag[0] = 'H'; tag[1] = 'A'; tag[2] = 'N'; tag[3] = 'G'; tag[4] = '-'; tag[5] = 't';
                tag[6] = (char)('0' + (tid / 10) % 10); tag[7] = (char)('0' + tid % 10); tag[8] = 0;
                {   char mn[48]; hangModName(ctx.ctx_RegEip, mn); OdinRawTrace(tag, mn, ctx.ctx_RegEip, ctx.ctx_RegEsp); }
                for(f = 0; f < 40 && hangReadable(ebp) && hangReadable(ebp + 4); f++) {
                    ULONG ra = ((ULONG *)ebp)[1], next = ((ULONG *)ebp)[0];
                    char mn[48];
                    hangModName(ra, mn);
                    OdinRawTrace("HANG-ra", mn, ra, ebp);
                    if(next <= ebp) break;
                    ebp = next;
                }
            }
            DosResumeThread(tid);
        }
        OdinRawTrace("HANG-round", NULL, round, 0);
        DosSleep(10000);
    }
    /* a test run with ODIN_HANGDUMP set must not leave a hung process behind */
    OdinRawTrace("HANG-exit", NULL, 0, 0);
    DosExit(EXIT_PROCESS, 99);
}

extern "C" void OdinHangDumpStart(void)
{
    const char *p = getenv("ODIN_HANGDUMP");
    TID tid;
    if(p && atoi(p) > 0)
        DosCreateThread(&tid, (PFNTHREAD)hangDumpThread, (ULONG)atoi(p), 0, 65536);
}

/* Called from the exception handler for XCPT_PROCESS_TERMINATE / XCPT_ASYNC_PROCESS_TERMINATE: logs the
 * thread, the exception code and the call chain of the (first few) threads that see the process end, to find
 * out who ended it. */
extern "C" void OdinTermDump(ULONG code, ULONG eip, ULONG ebp, ULONG info0)
{
    static volatile LONG cnt = 0;
    PTIB ptib;
    PPIB ppib;
    char mn[48];
    int f;

    if(!getenv("ODIN_TRACE") || cnt >= 3) return;
    DosGetInfoBlocks(&ptib, &ppib);
    if(code != 0xC0010001) {                            /* async: only note which thread ended the process */
        OdinRawTrace("TERMDUMP-async", NULL, ptib->tib_ptib2->tib2_ultid, info0);
        {   /* call chain of the thread that started the termination (first notification only) */
            static volatile LONG once = 0;
            if(info0 != ptib->tib_ptib2->tib2_ultid && !__sync_lock_test_and_set(&once, 1)) {
                CONTEXTRECORD ctx;
                memset(&ctx, 0, sizeof(ctx));
                ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
                int susp = (DosSuspendThread((TID)info0) == 0);
                if(DosQueryThreadContext((TID)info0, 1, &ctx) == 0) {
                    ULONG ebp = ctx.ctx_RegEbp;
                    hangModName(ctx.ctx_RegEip, mn);
                    OdinRawTrace("TERMINIT-eip", mn, ctx.ctx_RegEip, ctx.ctx_RegEsp);
                    for(f = 0; f < 30 && hangReadable(ebp) && hangReadable(ebp + 4); f++) {
                        ULONG ra = ((ULONG *)ebp)[1], next = ((ULONG *)ebp)[0];
                        char pn[40]; ULONG poff;
                        hangModName(ra, mn);
                        OdinRawTrace("TERMINIT-ra", mn, ra, ebp);
                        if(OdinPeModOfAddr(ra, pn, sizeof(pn), &poff)) OdinRawTrace("TERMINIT-pe", pn, poff, ra);
                        if(next <= ebp) break;
                        ebp = next;
                    }
                } else OdinRawTrace("TERMINIT-noctx", NULL, info0, susp);
                if(susp) DosResumeThread((TID)info0);
            }
        }
        return;
    }
    cnt++;
    DosGetInfoBlocks(&ptib, &ppib);
    hangModName(eip, mn);
    OdinRawTrace("TERMDUMP", mn, code, info0);
    OdinRawTrace("TERMDUMP-tid", NULL, ptib->tib_ptib2->tib2_ultid, 0);
    OdinRawTrace("TERMDUMP-eip", mn, eip, ebp);
    {   /* printable text found in the stack above the terminating frame (panic / assert messages) */
        ULONG a = ebp, end = ebp + 0x800, n = 0;
        while(a < end && n < 16) {
            ULONG s = a;
            char buf[72];
            int len = 0;
            while(a < end && hangReadable(a) && ((unsigned char *)a)[0] >= 0x20 && ((unsigned char *)a)[0] < 0x7f) { if(len < 70) buf[len++] = ((char *)a)[0]; a++; }
            if(len >= 10) { buf[len] = 0; OdinRawTrace("TERMDUMP-str", buf, s, len); n++; }
            a++;
        }
    }
    for(f = 0; f < 12 && hangReadable(ebp) && hangReadable(ebp + 4); f++) {
        ULONG ra = ((ULONG *)ebp)[1], next = ((ULONG *)ebp)[0];
        hangModName(ra, mn);
        {   char pn[40]; ULONG poff;
            if(OdinPeModOfAddr(ra, pn, sizeof(pn), &poff)) { OdinRawTrace("TERMDUMP-pe", pn, poff, ra); }
        }
        OdinRawTrace("TERMDUMP-ra", mn, ra, ebp);
        if(next <= ebp) break;
        ebp = next;
    }
}

/* exit list routine: runs in the thread that ends the process, so it tells which thread did it */
static void APIENTRY hangExitList(ULONG reason)
{
    PTIB ptib;
    PPIB ppib;
    DosGetInfoBlocks(&ptib, &ppib);
    OdinRawTrace("EXITLIST", NULL, ptib->tib_ptib2->tib2_ultid, reason);
    DosExitList(EXLST_EXIT, (PFNEXITLIST)hangExitList);
}

extern "C" void OdinExitListStart(void)
{
    if(getenv("ODIN_TRACE"))
        DosExitList(EXLST_ADD | 0x0000FF00, (PFNEXITLIST)hangExitList);
}

/* OS/2 thread id of the calling thread (for the trace lines) */
extern "C" unsigned long OdinCurrentTid(void)
{
    PTIB ptib;
    PPIB ppib;
    if(DosGetInfoBlocks(&ptib, &ppib) != 0 || !ptib || !ptib->tib_ptib2) return 0;
    return ptib->tib_ptib2->tib2_ultid;
}
