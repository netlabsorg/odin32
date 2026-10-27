/*
 * dbghelp - DBGHELP (ImageNtHeader only)
 * Odin32 compatibility DLL: provides the exports that current Windows programs
 * import so that they can be loaded; see the individual functions for what is
 * implemented and what merely reports "not supported".
 */
#include <os2win.h>

extern "C" LPVOID WINAPI ImageNtHeader(LPVOID Base)
{
    BYTE *p = (BYTE *)Base;

    if(p == NULL || p[0] != 'M' || p[1] != 'Z') {
        return NULL;
    }
    LONG off = *(LONG *)(p + 0x3C);                 //IMAGE_DOS_HEADER.e_lfanew
    if(off <= 0 || off > 0x10000000) {
        return NULL;
    }
    BYTE *nt = p + off;
    if(nt[0] != 'P' || nt[1] != 'E' || nt[2] != 0 || nt[3] != 0) {
        return NULL;
    }
    return nt;
}
