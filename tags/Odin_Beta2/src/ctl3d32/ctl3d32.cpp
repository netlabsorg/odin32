#include <windows.h>
#include <ctl3d.h>
#include <misc.h>

//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dSubclassDlg(HWND hwnd, WORD flags)
{
    dprintf(("Ctl3dSubclassDlg %x %x", hwnd, flags));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dSubclassDlgEx(HWND hwnd, DWORD flags)
{
    dprintf(("Ctl3dSubclassDlg %x %x", hwnd, flags));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
WORD WINAPI Ctl3dGetVer(void)
{
    dprintf(("Ctl3dGetVer"));
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dEnabled(void)
{
    dprintf(("Ctl3dEnabled"));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
HBRUSH WINAPI Ctl3dCtlColor(HDC hdc, LONG flags)
{
    dprintf(("Ctl3dCtlColor %x %x", hdc, flags));
    return 0;
}
//******************************************************************************
//******************************************************************************
HBRUSH WINAPI Ctl3dCtlColorEx(UINT wm, WPARAM wParam, LPARAM lParam)
{
    dprintf(("Ctl3dCtlColorEx %x %x %x", wm, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dColorChange()
{
    dprintf(("Ctl3dColorChange"));
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dSubclassCtl(HWND hwnd)
{
    dprintf(("Ctl3dSubclassCtl %x", hwnd));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dSubclassCtlEx(HWND hwnd, int flags)
{
    dprintf(("Ctl3dSubclassCtlEx %x %x", hwnd, flags));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dUnsubclassCtl(HWND hwnd)
{
    dprintf(("Ctl3dUnsubclassCtl %x", hwnd));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
LONG WINAPI Ctl3dDlgFramePaint(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    dprintf(("Ctl3dDlgFramePaint %x %x %x %x", hwnd, Msg, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dAutoSubclass(HINSTANCE hInstance)
{
    dprintf(("Ctl3dAutoSubclass %x", hInstance));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dAutoSubclassEx(HINSTANCE hInstance, DWORD flags)
{
    dprintf(("Ctl3dAutoSubclassEx %x %x", hInstance, flags));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dIsAutoSubclass()
{
    dprintf(("Ctl3dIsAutoSubclass"));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dUnAutoSubclass()
{
    dprintf(("Ctl3dUnAutoSubclass"));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dRegister(HINSTANCE hInstance)
{
    dprintf(("Ctl3dRegister %x", hInstance));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Ctl3dUnregister(HINSTANCE hInstance)
{
    dprintf(("Ctl3dUnRegister %x", hInstance));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
VOID WINAPI Ctl3dWinIniChange()
{
    dprintf(("Ctl3dWinIniChange"));
    return;
}
//******************************************************************************
//******************************************************************************
LONG WINAPI BtnWndProc3d(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    dprintf(("BtnWndProc3d %x %x %x %x", hwnd, Msg, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WINAPI EditWndProc3d(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    dprintf(("EditWndProc3d %x %x %x %x", hwnd, Msg, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WINAPI ListWndProc3d(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    dprintf(("ListWndProc3d %x %x %x %x", hwnd, Msg, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WINAPI ComboWndProc3d(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    dprintf(("ComboWndProc3d %x %x %x %x", hwnd, Msg, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WINAPI StaticWndProc3d(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    dprintf(("StaticWndProc3d %x %x %x %x", hwnd, Msg, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WINAPI Ctl3dDlgProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    dprintf(("Ctl3dDlgProc %x %x %x %x", hwnd, Msg, wParam, lParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
