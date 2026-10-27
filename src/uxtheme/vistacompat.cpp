/*
 * UXTHEME shims for the Vista+ buffered-paint / text-effect APIs.
 * Odin32 has no visual-styles engine, so these report "not supported"
 * (E_NOTIMPL / NULL / FALSE) and callers fall back to classic drawing.
 */
#include <os2win.h>

#define VC_E_NOTIMPL    ((LONG)0x80004001)

extern "C" {

LONG WINAPI DrawThemeTextEx(HANDLE hTheme, HDC hdc, int iPartId, int iStateId,
                            LPCWSTR pszText, int iCharCount, DWORD dwFlags,
                            LPRECT pRect, LPVOID pOptions)
{
    return VC_E_NOTIMPL;
}

LONG WINAPI BufferedPaintStopAllAnimations(HWND hwnd)
{
    return 0;   //S_OK: nothing to stop
}

HANDLE WINAPI BeginBufferedAnimation(HWND hwnd, HDC hdcTarget, const RECT *prcTarget,
                                     DWORD dwFormat, LPVOID pPaintParams,
                                     LPVOID pAnimationParams, HDC *phdcFrom, HDC *phdcTo)
{
    if(phdcFrom) *phdcFrom = NULL;
    if(phdcTo)   *phdcTo   = NULL;
    return NULL;
}

LONG WINAPI EndBufferedAnimation(HANDLE hbpAnimation, BOOL fUpdateTarget)
{
    return VC_E_NOTIMPL;
}

BOOL WINAPI BufferedPaintRenderAnimation(HWND hwnd, HDC hdcTarget)
{
    return FALSE;   //no animation running: caller paints normally
}

LONG WINAPI GetThemeTransitionDuration(HANDLE hTheme, int iPartId, int iStateIdFrom,
                                       int iStateIdTo, int iPropId, DWORD *pdwDuration)
{
    if(pdwDuration) *pdwDuration = 0;
    return VC_E_NOTIMPL;
}

} // extern "C"
