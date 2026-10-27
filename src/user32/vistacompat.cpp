/*
 * USER32 compatibility shims for APIs introduced after Windows 2000.
 * Added so that current Windows programs, which import these functions
 * directly, can be loaded by Odin32 (all imports are resolved at load time).
 *
 *  SetLayeredWindowAttributes   accepted, no effect (PM has no per-window
 *                               alpha / colour key; the window stays opaque)
 *  Add/RemoveClipboardFormatListener
 *                               accepted; WM_CLIPBOARDUPDATE is never sent
 *  GetMenuBarInfo               not implemented, fails with
 *                               ERROR_CALL_NOT_IMPLEMENTED
 */
#include <os2win.h>

extern "C" {

BOOL WINAPI SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
    if(!IsWindow(hwnd)) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI AddClipboardFormatListener(HWND hwnd)
{
    if(!IsWindow(hwnd)) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI RemoveClipboardFormatListener(HWND hwnd)
{
    if(!IsWindow(hwnd)) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI GetMenuBarInfo(HWND hwnd, LONG idObject, LONG idItem, LPVOID pmbi)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

} // extern "C"
