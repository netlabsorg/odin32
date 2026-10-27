/*
 * SHLWAPI: ColorAdjustLuma.  Like Wine, the luminance adjustment is
 * accepted but not applied: the colour is returned unchanged.
 */
#include <os2win.h>

extern "C" COLORREF WINAPI ColorAdjustLuma(COLORREF clrRGB, int n, BOOL fScale)
{
    return clrRGB;
}
