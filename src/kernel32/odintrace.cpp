/*
 * Lightweight API trace for release builds of Odin32 (the normal dprintf
 * logging only exists in DEBUG builds, which do not build with GCC 9.2 yet).
 *
 * Enabled by setting the environment variable ODIN_TRACE to a file name.  One
 * line per event is appended to that file:
 *      <tag> <string> <hex> <hex>
 * The file is opened, appended to and closed for every line (slow but robust
 * across crashes).  Uses only OS/2 file calls and own formatting, so it is safe
 * to call while the Win32 TIB (FS) is active.
 */
#include <os2win.h>
#include <string.h>
#include <stdlib.h>
#include "oslibdos.h"
#include <thread.h>
#include <wprocess.h>
#include "odintrace.h"
extern BOOL fExeStarted;

static volatile int fFsBusy = 0;     //>0: inside tracing code, FS checks are suppressed
static int  fTraceInit = 0;
static char szTraceFile[260];

static void traceInit(void)
{
    USHORT sel = RestoreOS2FS();
    const char *p = getenv("ODIN_TRACE");
    szTraceFile[0] = 0;
    if(p) {
        strncpy(szTraceFile, p, sizeof(szTraceFile) - 1);
        szTraceFile[sizeof(szTraceFile) - 1] = 0;
    }
    SetFS(sel);
    fTraceInit = 1;
}

static void OdinTraceInner(const char *tag, const char *str, DWORD a, DWORD b)
{
    char  buf[400];
    int   n = 0, i, j;
    DWORD wr, v[2], h;

    if(!fTraceInit) traceInit();
    if(!szTraceFile[0]) return;

    while(*tag && n < 60) buf[n++] = *tag++;
    buf[n++] = ' ';
    if(str) {
        for(i = 0; str[i] && n < 330; i++) buf[n++] = str[i];
    }
    v[0] = a; v[1] = b;
    buf[n++] = ' '; buf[n++] = 't'; buf[n++] = '=';
    {   DWORD tk = GetTickCount();
        for(j = 7; j >= 0; j--) buf[n++] = "0123456789ABCDEF"[(tk >> (j * 4)) & 15];
    }
    for(i = 0; i < 2; i++) {
        buf[n++] = ' ';
        for(j = 7; j >= 0; j--) buf[n++] = "0123456789ABCDEF"[(v[i] >> (j * 4)) & 15];
    }
    buf[n++] = 13; buf[n++] = 10;

    h = OSLibDosCreateFile(szTraceFile, 0x40000000 /*GENERIC_WRITE*/, 3 /*share r/w*/, NULL,
                           4 /*OPEN_ALWAYS*/, 0x80 /*NORMAL*/, 0);
    if(h == 0 || h == (DWORD)-1) return;
    OSLibDosSetFilePtr(h, 0, 2 /*FILE_END*/);
    OSLibDosWrite(h, buf, n, &wr);
    OSLibDosClose(h);
}

void OdinTrace(const char *tag, const char *str, DWORD a, DWORD b)
{
    fFsBusy++;
    OdinTraceInner(tag, str, a, b);
    fFsBusy--;
}

void OdinTraceW(const char *tag, const WCHAR *wstr, DWORD a, DWORD b)
{
    char tmp[200];
    int  i;

    for(i = 0; wstr && wstr[i] && i < 190; i++) {
        tmp[i] = (wstr[i] < 128) ? (char)wstr[i] : '?';
    }
    tmp[i] = 0;
    OdinTrace(tag, tmp, a, b);
}
extern "C" VOID WIN32API OdinTraceExt(const char *tag, DWORD a, DWORD b)
{
    OdinTrace(tag, NULL, a, b);
}

//Logs (at most 60 times) when the FS selector is not the thread's Win32 TIB, i.e. an
//earlier call returned to Win32 code with the OS/2 TIB still loaded.
void OdinFSCheck(const char *where, DWORD ra)
{
    static int cnt = 0;
    static int fBusy = 0;
    TEB *teb;

    if(!fExeStarted || fBusy) return;     //too early: no TEB yet
    if(!fTraceInit) traceInit();
    if(!szTraceFile[0] || cnt > 60) return;
    fBusy = 1;
    teb = GetThreadTEB();
    if(teb && GetFS() != (USHORT)teb->teb_sel) {
        cnt++;
        OdinTrace(where, NULL, GetFS(), ra);
    }
    fBusy = 0;
}

extern "C" VOID WIN32API OdinFSCheckExt(const char *where, DWORD ra)
{
    OdinFSCheck(where, ra);
}

//---- FS tracking via -finstrument-functions (diagnostic) ---------------------------
static volatile int fFsArmed = 0;
static volatile int cFsLogged = 0;

extern "C" VOID WIN32API __attribute__((no_instrument_function)) OdinFSArmExt(DWORD on)
{
    DWORD w32, os2; USHORT sel = GetFS();
    __asm__ __volatile__("movl %%fs:0x2c, %0" : "=r"(w32));
    RestoreOS2FS();
    __asm__ __volatile__("movl %%fs:0x2c, %0" : "=r"(os2));
    SetFS(sel);
    OdinTrace("TLSPTR-win32-os2", NULL, w32, os2);
    fFsArmed = on;
}

extern "C" VOID WIN32API __attribute__((no_instrument_function)) OdinFSInstrExt(DWORD kind, DWORD fn, DWORD cs)
{
    TEB *teb;
    char fsstr[16];
    USHORT fs;
    int i;

    if(!fFsArmed || fFsBusy || !fExeStarted || cFsLogged > 6000) return;
    fFsBusy++;
    teb = GetThreadTEB();
    fs = GetFS();
    cFsLogged++;
    for(i = 0; i < 4; i++) fsstr[i] = "0123456789ABCDEF"[(fs >> ((3 - i) * 4)) & 15];
    fsstr[4] = (teb && fs != (USHORT)teb->teb_sel) ? '!' : ' ';
    fsstr[5] = 0;
    OdinTrace(kind ? "FSI-E" : "FSI-X", fsstr, fn, cs);
    fFsBusy--;
}

extern "C" void __attribute__((no_instrument_function)) __cyg_profile_func_enter(void *fn, void *cs)
{
    OdinFSInstrExt(1, (DWORD)fn, (DWORD)cs);
}

extern "C" void __attribute__((no_instrument_function)) __cyg_profile_func_exit(void *fn, void *cs)
{
    OdinFSInstrExt(0, (DWORD)fn, (DWORD)cs);
}
