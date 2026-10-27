/*
 * dwmapi - Desktop Window Manager API stub (no composition on OS/2)
 * Odin32 compatibility DLL: provides the exports that current Windows programs
 * import so that they can be loaded; see the individual functions for what is
 * implemented and what merely reports "not supported".
 */
#include <os2win.h>

#define VC_E_NOTIMPL    ((LONG)0x80004001)

extern "C" {

LONG WINAPI DwmIsCompositionEnabled(BOOL *pfEnabled)
{
    if(pfEnabled) *pfEnabled = FALSE;
    return 0;
}

LONG WINAPI DwmGetColorizationColor(DWORD *pcrColorization, BOOL *pfOpaqueBlend)
{
    if(pcrColorization) *pcrColorization = 0xFF3C78D7;      //a neutral blue accent
    if(pfOpaqueBlend)   *pfOpaqueBlend   = TRUE;
    return 0;
}

LONG WINAPI DwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    return VC_E_NOTIMPL;
}

LONG WINAPI DwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPVOID pvAttribute, DWORD cbAttribute)
{
    return VC_E_NOTIMPL;
}

LONG WINAPI DwmExtendFrameIntoClientArea(HWND hwnd, LPVOID pMarInset)
{
    return VC_E_NOTIMPL;
}

} // extern "C"
