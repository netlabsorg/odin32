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

/* More Windows 7 era user32 functions that Firefox imports with delay loading (a missing one aborts the
 * program).  Device notifications and touch/gesture input never happen here; the display configuration
 * API reports "not supported". */
extern "C" {

HDEVNOTIFY WINAPI RegisterDeviceNotificationW(HANDLE hRecipient, LPVOID filter, DWORD flags)
{
    return (HDEVNOTIFY)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, 16);
}

HDEVNOTIFY WINAPI RegisterDeviceNotificationA(HANDLE hRecipient, LPVOID filter, DWORD flags)
{
    return RegisterDeviceNotificationW(hRecipient, filter, flags);
}

BOOL WINAPI UnregisterDeviceNotification(HDEVNOTIFY handle)
{
    if(!handle) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    GlobalFree((HGLOBAL)handle);
    return TRUE;
}

BOOL WINAPI SetProcessDPIAware(void)
{
    return TRUE;
}

BOOL WINAPI RegisterTouchWindow(HWND hwnd, ULONG flags)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI UnregisterTouchWindow(HWND hwnd)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI GetGestureInfo(HANDLE hGestureInfo, LPVOID pGestureInfo)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI SetGestureConfig(HWND hwnd, DWORD reserved, UINT cIDs, LPVOID pGestureConfig, UINT cbSize)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

LONG WINAPI GetDisplayConfigBufferSizes(UINT32 flags, UINT32 *numPathArrayElements, UINT32 *numModeInfoArrayElements)
{
    return ERROR_NOT_SUPPORTED;
}

LONG WINAPI QueryDisplayConfig(UINT32 flags, UINT32 *numPathArrayElements, LPVOID pathArray,
                               UINT32 *numModeInfoArrayElements, LPVOID modeInfoArray, LPVOID currentTopologyId)
{
    return ERROR_NOT_SUPPORTED;
}

BOOL WINAPI GetLastInputInfo(LPVOID plii)
{
    /* LASTINPUTINFO { UINT cbSize; DWORD dwTime; }: report "input just now" */
    if(!plii || *(UINT *)plii != 8) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    ((DWORD *)plii)[1] = GetTickCount();
    return TRUE;
}

} // extern "C"
