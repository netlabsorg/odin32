/* diagnostic: FS-selector tracking hooks for -finstrument-functions */
#include <os2win.h>

extern "C" VOID WIN32API OdinFSInstrExt(DWORD kind, DWORD fn, DWORD cs);

extern "C" void __attribute__((no_instrument_function)) __cyg_profile_func_enter(void *fn, void *cs)
{
    OdinFSInstrExt(1, (DWORD)fn, (DWORD)cs);
}

extern "C" void __attribute__((no_instrument_function)) __cyg_profile_func_exit(void *fn, void *cs)
{
    OdinFSInstrExt(0, (DWORD)fn, (DWORD)cs);
}
