/*
 * msimg32 - MSIMG32 (AlphaBlend forwards to GDI32!GdiAlphaBlend)
 * Odin32 compatibility DLL: provides the exports that current Windows programs
 * import so that they can be loaded; see the individual functions for what is
 * implemented and what merely reports "not supported".
 */
#include <os2win.h>

extern "C" BOOL WINAPI GdiAlphaBlend(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);

extern "C" BOOL WINAPI AlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
                                  HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc,
                                  BLENDFUNCTION ftn)
{
    return GdiAlphaBlend(hdcDest, xoriginDest, yoriginDest, wDest, hDest,
                         hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, *(DWORD *)&ftn);
}
