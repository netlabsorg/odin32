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
#include <os2wrap.h>
#include <string.h>
#include <stdlib.h>

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
    for(i = 0; i < 2; i++) {
        unsigned long v = i ? b : a;
        int j;
        for(j = 7; j >= 0; j--) buf[n++] = "0123456789ABCDEF"[(v >> (j * 4)) & 15];
        buf[n++] = ' ';
    }
    buf[n++] = 13; buf[n++] = 10;
    if(DosOpen((PSZ)fn, &hf, &act, 0, 0, OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_CREATE_IF_NEW,
               OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE, NULL) == 0) {
        DosSetFilePtr(hf, 0, FILE_END, &wr);
        DosWrite(hf, buf, n, &wr);
        DosClose(hf);
    }
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
                OdinRawTrace(tag, NULL, ctx.ctx_RegEip, ctx.ctx_RegEsp);
                for(f = 0; f < 10 && hangReadable(ebp) && hangReadable(ebp + 4); f++) {
                    ULONG ra = ((ULONG *)ebp)[1], next = ((ULONG *)ebp)[0];
                    OdinRawTrace("HANG-ra", NULL, ra, ebp);
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
