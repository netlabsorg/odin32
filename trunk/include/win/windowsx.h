/* $Id: windowsx.h,v 1.2 1999-08-28 07:36:39 sandervl Exp $ */

/* Copyright (C) 1999 Corel Corporation (Paul Quinn) */

#ifndef __WINE_WINDOWSX_H
#define __WINE_WINDOWSX_H

#ifdef __cplusplus
extern "C" {
#endif

#define _INC_WINDOWSX
	
#define GET_WM_VSCROLL_CODE(wp, lp)         LOWORD(wp)
#define GET_WM_VSCROLL_POS(wp, lp)          HIWORD(wp)
#define GET_WM_VSCROLL_HWND(wp, lp)         (HWND)(lp)
#define GET_WM_VSCROLL_MPS(code, pos, hwnd) (WPARAM)MAKELONG(code, pos), (LONG)(hwnd)


#define GET_WM_COMMAND_ID(wp, lp)           LOWORD(wp)
#define GET_WM_COMMAND_HWND(wp, lp)         (HWND)(lp)
#define GET_WM_COMMAND_CMD(wp, lp)          HIWORD(wp)
#define GET_WM_COMMAND_MPS(id, hwnd, cmd)   (WPARAM)MAKELONG(id, cmd), (LONG)(hwnd)

#define WM_CTLCOLOR                             0x0019

#define GET_WM_CTLCOLOR_HDC(wp, lp, msg)        (HDC)(wp)
#define GET_WM_CTLCOLOR_HWND(wp, lp, msg)       (HWND)(lp)
#define GET_WM_CTLCOLOR_TYPE(wp, lp, msg)       (WORD)(msg - WM_CTLCOLORMSGBOX)
#define GET_WM_CTLCOLOR_MSG(type)               (WORD)(WM_CTLCOLORMSGBOX+(type))
#define GET_WM_CTLCOLOR_MPS(hdc, hwnd, type)    (WPARAM)(hdc), (LONG)(hwnd)

#define GET_WM_VKEYTOITEM_CODE(wp, lp)          (int)(short)LOWORD(wp)
#define GET_WM_VKEYTOITEM_ITEM(wp, lp)          HIWORD(wp)
#define GET_WM_VKEYTOITEM_HWND(wp, lp)          (HWND)(lp)
#define GET_WM_VKEYTOITEM_MPS(code, item, hwnd) (WPARAM)MAKELONG(item, code), (LONG)(hwnd)

/****** KERNEL Macro APIs ****************************************************/

#define     GetInstanceModule(hInstance) (HMODULE)(hInstance)

#define     GlobalPtrHandle(lp)         \
                ((HGLOBAL)GlobalHandle(lp))

#define     GlobalLockPtr(lp)                \
                ((BOOL)GlobalLock(GlobalPtrHandle(lp)))
#define     GlobalUnlockPtr(lp)      \
                GlobalUnlock(GlobalPtrHandle(lp))

#define     GlobalAllocPtr(flags, cb)        \
                (GlobalLock(GlobalAlloc((flags), (cb))))
#define     GlobalReAllocPtr(lp, cbNew, flags)       \
                (GlobalUnlockPtr(lp), GlobalLock(GlobalReAlloc(GlobalPtrHandle(lp) , (cbNew), (flags))))
#define     GlobalFreePtr(lp)                \
                (GlobalUnlockPtr(lp), (BOOL)GlobalFree(GlobalPtrHandle(lp)))

/****** USER Macro APIs ******************************************************/

#define     GetWindowInstance(hwnd) ((HMODULE)GetWindowLong(hwnd, GWL_HINSTANCE))

#define     GetWindowStyle(hwnd)    ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#define     GetWindowExStyle(hwnd)  ((DWORD)GetWindowLong(hwnd, GWL_EXSTYLE))

#define     GetWindowOwner(hwnd)    GetWindow(hwnd, GW_OWNER)

#define     GetFirstChild(hwnd)     GetTopWindow(hwnd)
#define     GetFirstSibling(hwnd)   GetWindow(hwnd, GW_HWNDFIRST)
#define     GetLastSibling(hwnd)    GetWindow(hwnd, GW_HWNDLAST)
#define     GetNextSibling(hwnd)    GetWindow(hwnd, GW_HWNDNEXT)
#define     GetPrevSibling(hwnd)    GetWindow(hwnd, GW_HWNDPREV)

#define     GetWindowID(hwnd)       GetDlgCtrlID(hwnd)

#define     SetWindowRedraw(hwnd, fRedraw)  \
	        ((void)SendMessage(hwnd, WM_SETREDRAW, (WPARAM)(BOOL)(fRedraw), 0L))
#define     SubclassWindow(hwnd, lpfn)      \
		((WNDPROC)SetWindowLong((hwnd), GWL_WNDPROC, (LPARAM)(WNDPROC)(lpfn)))

#define     IsMinimized(hwnd)        IsIconic(hwnd)
#define     IsMaximized(hwnd)        IsZoomed(hwnd)
#define     IsRestored(hwnd)    ((GetWindowStyle(hwnd) & (WS_MINIMIZE | WS_MAXIMIZE)) == 0L)
#define     SetWindowFont(hwnd, hfont, fRedraw) \
		FORWARD_WM_SETFONT((hwnd), (hfont), (fRedraw), SendMessage)

#define     GetWindowFont(hwnd)      FORWARD_WM_GETFONT((hwnd), SendMessage)

#define     MapWindowRect(hwndFrom, hwndTo, lprc) \
                MapWindowPoints((hwndFrom), (hwndTo), (POINT *)(lprc), 2)

#define     IsLButtonDown()  (GetKeyState(VK_LBUTTON) < 0)
#define     IsRButtonDown()  (GetKeyState(VK_RBUTTON) < 0)
#define     IsMButtonDown()  (GetKeyState(VK_MBUTTON) < 0)

#define     SubclassDialog(hwndDlg, lpfn) \
		((DLGPROC)SetWindowLong(hwndDlg, DWL_DLGPROC, (LPARAM)(DLGPROC)(lpfn)))

#define     DeletePen(hpen)      DeleteObject((HGDIOBJ)(HPEN)(hpen))
#define     SelectPen(hdc, hpen)    ((HPEN)SelectObject((hdc), (HGDIOBJ)(HPEN)(hpen)))
#define     GetStockPen(i)       ((HPEN)GetStockObject(i))

#define     DeleteBrush(hbr)     DeleteObject((HGDIOBJ)(HBRUSH)(hbr))
#define     SelectBrush(hdc, hbr)   ((HBRUSH)SelectObject((hdc), (HGDIOBJ)(HBRUSH)(hbr)))
#define     GetStockBrush(i)     ((HBRUSH)GetStockObject(i))

#define     DeletePalette(hpal)     DeleteObject((HGDIOBJ)(HPALETTE)(hpal))

#define     DeleteFont(hfont)       DeleteObject((HGDIOBJ)(HFONT)(hfont))
#define     SelectFont(hdc, hfont)  ((HFONT)SelectObject((hdc), (HGDIOBJ)(HFONT) (hfont)))
#define     GetStockFont(i)         ((HFONT)GetStockObject(i))
#define     DeleteBitmap(hbm)       DeleteObject((HGDIOBJ)(HBITMAP)(hbm))
#define     SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))
#define     InsetRect(lprc, dx, dy) InflateRect((lprc), -(dx), -(dy))


#define     SetDlgMsgResult(hwnd, msg, result) (( \
        (msg) == WM_CTLCOLORMSGBOX      || \
        (msg) == WM_CTLCOLOREDIT        || \
        (msg) == WM_CTLCOLORLISTBOX     || \
        (msg) == WM_CTLCOLORBTN         || \
        (msg) == WM_CTLCOLORDLG         || \
        (msg) == WM_CTLCOLORSCROLLBAR   || \
        (msg) == WM_CTLCOLORSTATIC      || \
        (msg) == WM_COMPAREITEM         || \
        (msg) == WM_VKEYTOITEM          || \
        (msg) == WM_CHARTOITEM          || \
        (msg) == WM_QUERYDRAGICON       || \
        (msg) == WM_INITDIALOG             \
    ) ? (BOOL)(result) : (SetWindowLong((hwnd), DWL_MSGRESULT, (LPARAM)(LRESULT)(result)), TRUE))

#define     DefDlgProcEx(hwnd, msg, wParam, lParam, pfRecursion) \
    (*(pfRecursion) = TRUE, DefDlgProc(hwnd, msg, wParam, lParam))

#define     CheckDefDlgRecursion(pfRecursion) \
    if (*(pfRecursion)) { *(pfRecursion) = FALSE; return FALSE; }

/****** Message crackers ****************************************************/

#define HANDLE_MSG(hwnd, message, fn)    \
    case (message): return HANDLE_##message((hwnd), (wParam), (lParam), (fn))

/* void Cls_OnCompacting(HWND hwnd, UINT compactRatio) */
#define HANDLE_WM_COMPACTING(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam)), 0L)
#define FORWARD_WM_COMPACTING(hwnd, compactRatio, fn) \
    (void)(fn)((hwnd), WM_COMPACTING, (WPARAM)(UINT)(compactRatio), 0L)

/* void Cls_OnWinIniChange(HWND hwnd, LPCTSTR lpszSectionName) */
#define HANDLE_WM_WININICHANGE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (LPCTSTR)(lParam)), 0L)
#define FORWARD_WM_WININICHANGE(hwnd, lpszSectionName, fn) \
    (void)(fn)((hwnd), WM_WININICHANGE, 0L, (LPARAM)(LPCTSTR)(lpszSectionName))

/* void Cls_OnSysColorChange(HWND hwnd) */
#define HANDLE_WM_SYSCOLORCHANGE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_SYSCOLORCHANGE(hwnd, fn) \
    (void)(fn)((hwnd), WM_SYSCOLORCHANGE, 0L, 0L)

/* BOOL Cls_OnQueryNewPalette(HWND hwnd) */
#define HANDLE_WM_QUERYNEWPALETTE(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(hwnd), 0L)
#define FORWARD_WM_QUERYNEWPALETTE(hwnd, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_QUERYNEWPALETTE, 0L, 0L)

/* void Cls_OnPaletteIsChanging(HWND hwnd, HWND hwndPaletteChange) */
#define HANDLE_WM_PALETTEISCHANGING(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define FORWARD_WM_PALETTEISCHANGING(hwnd, hwndPaletteChange, fn) \
    (void)(fn)((hwnd), WM_PALETTEISCHANGING, (WPARAM)(HWND)(hwndPaletteChange), 0L)

/* void Cls_OnPaletteChanged(HWND hwnd, HWND hwndPaletteChange) */
#define HANDLE_WM_PALETTECHANGED(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define FORWARD_WM_PALETTECHANGED(hwnd, hwndPaletteChange, fn) \
    (void)(fn)((hwnd), WM_PALETTECHANGED, (WPARAM)(HWND)(hwndPaletteChange), 0L)

/* void Cls_OnFontChange(HWND hwnd) */
#define HANDLE_WM_FONTCHANGE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_FONTCHANGE(hwnd, fn) \
    (void)(fn)((hwnd), WM_FONTCHANGE, 0L, 0L)

/* void Cls_OnSpoolerStatus(HWND hwnd, UINT status, int cJobInQueue) */
#define HANDLE_WM_SPOOLERSTATUS(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define FORWARD_WM_SPOOLERSTATUS(hwnd, status, cJobInQueue, fn) \
    (void)(fn)((hwnd), WM_SPOOLERSTATUS, (WPARAM)(status), MAKELPARAM((cJobInQueue), 0))

/* void Cls_OnDevModeChange(HWND hwnd, LPCTSTR lpszDeviceName) */
#define HANDLE_WM_DEVMODECHANGE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (LPCTSTR)(lParam)), 0L)
#define FORWARD_WM_DEVMODECHANGE(hwnd, lpszDeviceName, fn) \
    (void)(fn)((hwnd), WM_DEVMODECHANGE, 0L, (LPARAM)(LPCTSTR)(lpszDeviceName))

/* void Cls_OnTimeChange(HWND hwnd) */
#define HANDLE_WM_TIMECHANGE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_TIMECHANGE(hwnd, fn) \
    (void)(fn)((hwnd), WM_TIMECHANGE, 0L, 0L)

/* void Cls_OnPower(HWND hwnd, int code) */
#define HANDLE_WM_POWER(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam)), 0L)
#define FORWARD_WM_POWER(hwnd, code, fn) \
    (void)(fn)((hwnd), WM_POWER, (WPARAM)(int)(code), 0L)

/* BOOL Cls_OnQueryEndSession(HWND hwnd) */
#define HANDLE_WM_QUERYENDSESSION(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(hwnd), 0L)
#define FORWARD_WM_QUERYENDSESSION(hwnd, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_QUERYENDSESSION, 0L, 0L)

/* void Cls_OnEndSession(HWND hwnd, BOOL fEnding) */
#define HANDLE_WM_ENDSESSION(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(wParam)), 0L)
#define FORWARD_WM_ENDSESSION(hwnd, fEnding, fn) \
    (void)(fn)((hwnd), WM_ENDSESSION, (WPARAM)(BOOL)(fEnding), 0L)

/* void Cls_OnQuit(HWND hwnd, int exitCode) */
#define HANDLE_WM_QUIT(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam)), 0L)
#define FORWARD_WM_QUIT(hwnd, exitCode, fn) \
    (void)(fn)((hwnd), WM_QUIT, (WPARAM)(exitCode), 0L)

/* This message is in Windows 3.1 only */
/* void Cls_OnSystemError(HWND hwnd, int errCode) */
#define HANDLE_WM_SYSTEMERROR(hwnd, wParam, lParam, fn) 0L
#define FORWARD_WM_SYSTEMERROR(hwnd, errCode, fn) 0L

/* BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) */
#define HANDLE_WM_CREATE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (LPCREATESTRUCT)(lParam)) ? 0L : (LRESULT)-1L)
#define FORWARD_WM_CREATE(hwnd, lpCreateStruct, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_CREATE, 0L, (LPARAM)(LPCREATESTRUCT)(lpCreateStruct))

/* BOOL Cls_OnNCCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) */
#define HANDLE_WM_NCCREATE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((hwnd), (LPCREATESTRUCT)(lParam))
#define FORWARD_WM_NCCREATE(hwnd, lpCreateStruct, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_NCCREATE, 0L, (LPARAM)(LPCREATESTRUCT)(lpCreateStruct))

/* void Cls_OnDestroy(HWND hwnd) */
#define HANDLE_WM_DESTROY(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_DESTROY(hwnd, fn) \
    (void)(fn)((hwnd), WM_DESTROY, 0L, 0L)

/* void Cls_OnNCDestroy(HWND hwnd) */
#define HANDLE_WM_NCDESTROY(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_NCDESTROY(hwnd, fn) \
    (void)(fn)((hwnd), WM_NCDESTROY, 0L, 0L)

/* void Cls_OnShowWindow(HWND hwnd, BOOL fShow, UINT status) */
#define HANDLE_WM_SHOWWINDOW(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(wParam), (UINT)(lParam)), 0L)
#define FORWARD_WM_SHOWWINDOW(hwnd, fShow, status, fn) \
    (void)(fn)((hwnd), WM_SHOWWINDOW, (WPARAM)(BOOL)(fShow), (LPARAM)(UINT)(status))

/* void Cls_OnSetRedraw(HWND hwnd, BOOL fRedraw) */
#define HANDLE_WM_SETREDRAW(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(wParam)), 0L)
#define FORWARD_WM_SETREDRAW(hwnd, fRedraw, fn) \
    (void)(fn)((hwnd), WM_SETREDRAW, (WPARAM)(BOOL)(fRedraw), 0L)

/* void Cls_OnEnable(HWND hwnd, BOOL fEnable) */
#define HANDLE_WM_ENABLE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(wParam)), 0L)
#define FORWARD_WM_ENABLE(hwnd, fEnable, fn) \
    (void)(fn)((hwnd), WM_ENABLE, (WPARAM)(BOOL)(fEnable), 0L)

/* void Cls_OnSetText(HWND hwnd, LPCTSTR lpszText) */
#define HANDLE_WM_SETTEXT(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (LPCTSTR)(lParam)), 0L)
#define FORWARD_WM_SETTEXT(hwnd, lpszText, fn) \
    (void)(fn)((hwnd), WM_SETTEXT, 0L, (LPARAM)(LPCTSTR)(lpszText))

/* INT Cls_OnGetText(HWND hwnd, int cchTextMax, LPTSTR lpszText) */
#define HANDLE_WM_GETTEXT(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((hwnd), (int)(wParam), (LPTSTR)(lParam))
#define FORWARD_WM_GETTEXT(hwnd, cchTextMax, lpszText, fn) \
    (int)(DWORD)(fn)((hwnd), WM_GETTEXT, (WPARAM)(int)(cchTextMax), (LPARAM)(LPTSTR)(lpszText))

/* INT Cls_OnGetTextLength(HWND hwnd) */
#define HANDLE_WM_GETTEXTLENGTH(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)(hwnd)
#define FORWARD_WM_GETTEXTLENGTH(hwnd, fn) \
    (int)(DWORD)(fn)((hwnd), WM_GETTEXTLENGTH, 0L, 0L)

/* BOOL Cls_OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos) */
#define HANDLE_WM_WINDOWPOSCHANGING(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((hwnd), (LPWINDOWPOS)(lParam))
#define FORWARD_WM_WINDOWPOSCHANGING(hwnd, lpwpos, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_WINDOWPOSCHANGING, 0L, (LPARAM)(LPWINDOWPOS)(lpwpos))

/* void Cls_OnWindowPosChanged(HWND hwnd, const LPWINDOWPOS lpwpos) */
#define HANDLE_WM_WINDOWPOSCHANGED(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (const LPWINDOWPOS)(lParam)), 0L)
#define FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, fn) \
    (void)(fn)((hwnd), WM_WINDOWPOSCHANGED, 0L, (LPARAM)(const LPWINDOWPOS)(lpwpos))

/* void Cls_OnMove(HWND hwnd, int x, int y) */
#define HANDLE_WM_MOVE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define FORWARD_WM_MOVE(hwnd, x, y, fn) \
    (void)(fn)((hwnd), WM_MOVE, 0L, MAKELPARAM((x), (y)))

/* void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy) */
#define HANDLE_WM_SIZE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define FORWARD_WM_SIZE(hwnd, state, cx, cy, fn) \
    (void)(fn)((hwnd), WM_SIZE, (WPARAM)(UINT)(state), MAKELPARAM((cx), (cy)))

/* void Cls_OnClose(HWND hwnd) */
#define HANDLE_WM_CLOSE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_CLOSE(hwnd, fn) \
    (void)(fn)((hwnd), WM_CLOSE, 0L, 0L)

/* BOOL Cls_OnQueryOpen(HWND hwnd) */
#define HANDLE_WM_QUERYOPEN(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(hwnd), 0L)
#define FORWARD_WM_QUERYOPEN(hwnd, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_QUERYOPEN, 0L, 0L)

/* void Cls_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo) */
#define HANDLE_WM_GETMINMAXINFO(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (LPMINMAXINFO)(lParam)), 0L)
#define FORWARD_WM_GETMINMAXINFO(hwnd, lpMinMaxInfo, fn) \
    (void)(fn)((hwnd), WM_GETMINMAXINFO, 0L, (LPARAM)(LPMINMAXINFO)(lpMinMaxInfo))

/* void Cls_OnPaint(HWND hwnd) */
#define HANDLE_WM_PAINT(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_PAINT(hwnd, fn) \
    (void)(fn)((hwnd), WM_PAINT, 0L, 0L)

/* BOOL Cls_OnEraseBkgnd(HWND hwnd, HDC hdc) */
#define HANDLE_WM_ERASEBKGND(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((hwnd), (HDC)(wParam))
#define FORWARD_WM_ERASEBKGND(hwnd, hdc, fn) \
   (BOOL)(DWORD)(fn)((hwnd), WM_ERASEBKGND, (WPARAM)(HDC)(hdc), 0L)

/* BOOL Cls_OnIconEraseBkgnd(HWND hwnd, HDC hdc) */
#define HANDLE_WM_ICONERASEBKGND(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((hwnd), (HDC)(wParam))
#define FORWARD_WM_ICONERASEBKGND(hwnd, hdc, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_ICONERASEBKGND, (WPARAM)(HDC)(hdc), 0L)

/* void Cls_OnNCPaint(HWND hwnd, HRGN hrgn) */
#define HANDLE_WM_NCPAINT(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HRGN)(wParam)), 0L)
#define FORWARD_WM_NCPAINT(hwnd, hrgn, fn) \
    (void)(fn)((hwnd), WM_NCPAINT, (WPARAM)(HRGN)(hrgn), 0L)

/* UINT Cls_OnNCCalcSize(HWND hwnd, BOOL fCalcValidRects, NCCALCSIZE_PARAMS * lpcsp) */
#define HANDLE_WM_NCCALCSIZE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((hwnd), (BOOL)(0), (NCCALCSIZE_PARAMS *)(lParam))
#define FORWARD_WM_NCCALCSIZE(hwnd, fCalcValidRects, lpcsp, fn) \
    (UINT)(DWORD)(fn)((hwnd), WM_NCCALCSIZE, 0L, (LPARAM)(NCCALCSIZE_PARAMS *)(lpcsp))

/* UINT Cls_OnNCHitTest(HWND hwnd, int x, int y) */
#define HANDLE_WM_NCHITTEST(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define FORWARD_WM_NCHITTEST(hwnd, x, y, fn) \
    (UINT)(DWORD)(fn)((hwnd), WM_NCHITTEST, 0L, MAKELPARAM((x), (y)))

/* HICON Cls_OnQueryDragIcon(HWND hwnd) */
#define HANDLE_WM_QUERYDRAGICON(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)(hwnd)
#define FORWARD_WM_QUERYDRAGICON(hwnd, fn) \
    (HICON)(UINT)(DWORD)(fn)((hwnd), WM_QUERYDRAGICON, 0L, 0L)

#ifdef _INC_SHELLAPI
/* void Cls_OnDropFiles(HWND hwnd, HDROP hdrop) */
#define HANDLE_WM_DROPFILES(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HDROP)(wParam)), 0L)
#define FORWARD_WM_DROPFILES(hwnd, hdrop, fn) \
    (void)(fn)((hwnd), WM_DROPFILES, (WPARAM)(HDROP)(hdrop), 0L)
#endif  /* _INC_SHELLAPI */

/* void Cls_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized) */
#define HANDLE_WM_ACTIVATE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)LOWORD(wParam), (HWND)(lParam), (BOOL)HIWORD(wParam)), 0L)
#define FORWARD_WM_ACTIVATE(hwnd, state, hwndActDeact, fMinimized, fn) \
    (void)(fn)((hwnd), WM_ACTIVATE, MAKEWPARAM((state), (fMinimized)), (LPARAM)(HWND)(hwndActDeact))

/* void Cls_OnActivateApp(HWND hwnd, BOOL fActivate, DWORD dwThreadId) */
#define HANDLE_WM_ACTIVATEAPP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(wParam), (DWORD)(lParam)), 0L)
#define FORWARD_WM_ACTIVATEAPP(hwnd, fActivate, dwThreadId, fn) \
    (void)(fn)((hwnd), WM_ACTIVATEAPP, (WPARAM)(BOOL)(fActivate), (LPARAM)(dwThreadId))

/* BOOL Cls_OnNCActivate(HWND hwnd, BOOL fActive, HWND hwndActDeact, BOOL fMinimized) */
#define HANDLE_WM_NCACTIVATE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((hwnd), (BOOL)(wParam), 0L, 0L)
#define FORWARD_WM_NCACTIVATE(hwnd, fActive, hwndActDeact, fMinimized, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_NCACTIVATE, (WPARAM)(BOOL)(fActive), 0L)

/* void Cls_OnSetFocus(HWND hwnd, HWND hwndOldFocus) */
#define HANDLE_WM_SETFOCUS(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define FORWARD_WM_SETFOCUS(hwnd, hwndOldFocus, fn) \
    (void)(fn)((hwnd), WM_SETFOCUS, (WPARAM)(HWND)(hwndOldFocus), 0L)

/* void Cls_OnKillFocus(HWND hwnd, HWND hwndNewFocus) */
#define HANDLE_WM_KILLFOCUS(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define FORWARD_WM_KILLFOCUS(hwnd, hwndNewFocus, fn) \
    (void)(fn)((hwnd), WM_KILLFOCUS, (WPARAM)(HWND)(hwndNewFocus), 0L)

/* void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define HANDLE_WM_KEYDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), TRUE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
#define FORWARD_WM_KEYDOWN(hwnd, vk, cRepeat, flags, fn) \
    (void)(fn)((hwnd), WM_KEYDOWN, (WPARAM)(UINT)(vk), MAKELPARAM((cRepeat), (flags)))

/* void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define HANDLE_WM_KEYUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), FALSE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
#define FORWARD_WM_KEYUP(hwnd, vk, cRepeat, flags, fn) \
    (void)(fn)((hwnd), WM_KEYUP, (WPARAM)(UINT)(vk), MAKELPARAM((cRepeat), (flags)))

/* void Cls_OnChar(HWND hwnd, TCHAR ch, int cRepeat) */
#define HANDLE_WM_CHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define FORWARD_WM_CHAR(hwnd, ch, cRepeat, fn) \
    (void)(fn)((hwnd), WM_CHAR, (WPARAM)(TCHAR)(ch), MAKELPARAM((cRepeat),0))

/* void Cls_OnDeadChar(HWND hwnd, TCHAR ch, int cRepeat) */
#define HANDLE_WM_DEADCHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define FORWARD_WM_DEADCHAR(hwnd, ch, cRepeat, fn) \
    (void)(fn)((hwnd), WM_DEADCHAR, (WPARAM)(TCHAR)(ch), MAKELPARAM((cRepeat),0))

/* void Cls_OnSysKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define HANDLE_WM_SYSKEYDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), TRUE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
#define FORWARD_WM_SYSKEYDOWN(hwnd, vk, cRepeat, flags, fn) \
    (void)(fn)((hwnd), WM_SYSKEYDOWN, (WPARAM)(UINT)(vk), MAKELPARAM((cRepeat), (flags)))

/* void Cls_OnSysKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) */
#define HANDLE_WM_SYSKEYUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), FALSE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
#define FORWARD_WM_SYSKEYUP(hwnd, vk, cRepeat, flags, fn) \
    (void)(fn)((hwnd), WM_SYSKEYUP, (WPARAM)(UINT)(vk), MAKELPARAM((cRepeat), (flags)))

/* void Cls_OnSysChar(HWND hwnd, TCHAR ch, int cRepeat) */
#define HANDLE_WM_SYSCHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define FORWARD_WM_SYSCHAR(hwnd, ch, cRepeat, fn) \
    (void)(fn)((hwnd), WM_SYSCHAR, (WPARAM)(TCHAR)(ch), MAKELPARAM((cRepeat), 0))

/* void Cls_OnSysDeadChar(HWND hwnd, TCHAR ch, int cRepeat) */
#define HANDLE_WM_SYSDEADCHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define FORWARD_WM_SYSDEADCHAR(hwnd, ch, cRepeat, fn) \
    (void)(fn)((hwnd), WM_SYSDEADCHAR, (WPARAM)(TCHAR)(ch), MAKELPARAM((cRepeat), 0))

/* void Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags) */
#define HANDLE_WM_MOUSEMOVE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_MOUSEMOVE(hwnd, x, y, keyFlags, fn) \
    (void)(fn)((hwnd), WM_MOUSEMOVE, (WPARAM)(UINT)(keyFlags), MAKELPARAM((x), (y)))

/* void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define HANDLE_WM_LBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, keyFlags, fn) \
    (void)(fn)((hwnd), (fDoubleClick) ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN, (WPARAM)(UINT)(keyFlags), MAKELPARAM((x), (y)))

/* void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define HANDLE_WM_LBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags) */
#define HANDLE_WM_LBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_LBUTTONUP(hwnd, x, y, keyFlags, fn) \
    (void)(fn)((hwnd), WM_LBUTTONUP, (WPARAM)(UINT)(keyFlags), MAKELPARAM((x), (y)))

/* void Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define HANDLE_WM_RBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_RBUTTONDOWN(hwnd, fDoubleClick, x, y, keyFlags, fn) \
    (void)(fn)((hwnd), (fDoubleClick) ? WM_RBUTTONDBLCLK : WM_RBUTTONDOWN, (WPARAM)(UINT)(keyFlags), MAKELPARAM((x), (y)))

/* void Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define HANDLE_WM_RBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls_OnRButtonUp(HWND hwnd, int x, int y, UINT flags) */
#define HANDLE_WM_RBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_RBUTTONUP(hwnd, x, y, keyFlags, fn) \
    (void)(fn)((hwnd), WM_RBUTTONUP, (WPARAM)(UINT)(keyFlags), MAKELPARAM((x), (y)))

/* void Cls_OnMButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define HANDLE_WM_MBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_MBUTTONDOWN(hwnd, fDoubleClick, x, y, keyFlags, fn) \
    (void)(fn)((hwnd), (fDoubleClick) ? WM_MBUTTONDBLCLK : WM_MBUTTONDOWN, (WPARAM)(UINT)(keyFlags), MAKELPARAM((x), (y)))

/* void Cls_OnMButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) */
#define HANDLE_WM_MBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls_OnMButtonUp(HWND hwnd, int x, int y, UINT flags) */
#define HANDLE_WM_MBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_MBUTTONUP(hwnd, x, y, keyFlags, fn) \
    (void)(fn)((hwnd), WM_MBUTTONUP, (WPARAM)(UINT)(keyFlags), MAKELPARAM((x), (y)))

/* void Cls_OnNCMouseMove(HWND hwnd, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCMOUSEMOVE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_NCMOUSEMOVE(hwnd, x, y, codeHitTest, fn) \
    (void)(fn)((hwnd), WM_NCMOUSEMOVE, (WPARAM)(UINT)(codeHitTest), MAKELPARAM((x), (y)))

/* void Cls_OnNCLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCLBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_NCLBUTTONDOWN(hwnd, fDoubleClick, x, y, codeHitTest, fn) \
    (void)(fn)((hwnd), (fDoubleClick) ? WM_NCLBUTTONDBLCLK : WM_NCLBUTTONDOWN, (WPARAM)(UINT)(codeHitTest), MAKELPARAM((x), (y)))

/* void Cls_OnNCLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCLBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls_OnNCLButtonUp(HWND hwnd, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCLBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_NCLBUTTONUP(hwnd, x, y, codeHitTest, fn) \
    (void)(fn)((hwnd), WM_NCLBUTTONUP, (WPARAM)(UINT)(codeHitTest), MAKELPARAM((x), (y)))

/* void Cls_OnNCRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCRBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_NCRBUTTONDOWN(hwnd, fDoubleClick, x, y, codeHitTest, fn) \
    (void)(fn)((hwnd), (fDoubleClick) ? WM_NCRBUTTONDBLCLK : WM_NCRBUTTONDOWN, (WPARAM)(UINT)(codeHitTest), MAKELPARAM((x), (y)) )

/* void Cls_OnNCRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCRBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls_OnNCRButtonUp(HWND hwnd, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCRBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_NCRBUTTONUP(hwnd, x, y, codeHitTest, fn) \
    (void)(fn)((hwnd), WM_NCRBUTTONUP, (WPARAM)(UINT)(codeHitTest), MAKELPARAM((x), (y)) )

/* void Cls_OnNCMButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCMBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_NCMBUTTONDOWN(hwnd, fDoubleClick, x, y, codeHitTest, fn) \
    (void)(fn)((hwnd), (fDoubleClick) ? WM_NCMBUTTONDBLCLK : WM_NCMBUTTONDOWN, (WPARAM)(UINT)(codeHitTest), MAKELPARAM((x), (y)) )

/* void Cls_OnNCMButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCMBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls_OnNCMButtonUp(HWND hwnd, int x, int y, UINT codeHitTest) */
#define HANDLE_WM_NCMBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define FORWARD_WM_NCMBUTTONUP(hwnd, x, y, codeHitTest, fn) \
    (void)(fn)((hwnd), WM_NCMBUTTONUP, (WPARAM)(UINT)(codeHitTest), MAKELPARAM((x), (y)) )

/* int Cls_OnMouseActivate(HWND hwnd, HWND hwndTopLevel, UINT codeHitTest, UINT msg) */
#define HANDLE_WM_MOUSEACTIVATE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((hwnd), (HWND)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam))
#define FORWARD_WM_MOUSEACTIVATE(hwnd, hwndTopLevel, codeHitTest, msg, fn) \
    (int)(DWORD)(fn)((hwnd), WM_MOUSEACTIVATE, (WPARAM)(HWND)(hwndTopLevel), MAKELPARAM((codeHitTest), (msg)))

/* void Cls_OnCancelMode(HWND hwnd) */
#define HANDLE_WM_CANCELMODE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_CANCELMODE(hwnd, fn) \
    (void)(fn)((hwnd), WM_CANCELMODE, 0L, 0L)

/* void Cls_OnTimer(HWND hwnd, UINT id) */
#define HANDLE_WM_TIMER(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam)), 0L)
#define FORWARD_WM_TIMER(hwnd, id, fn) \
    (void)(fn)((hwnd), WM_TIMER, (WPARAM)(UINT)(id), 0L)

/* void Cls_OnInitMenu(HWND hwnd, HMENU hMenu) */
#define HANDLE_WM_INITMENU(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HMENU)(wParam)), 0L)
#define FORWARD_WM_INITMENU(hwnd, hMenu, fn) \
    (void)(fn)((hwnd), WM_INITMENU, (WPARAM)(HMENU)(hMenu), 0L)

/* void Cls_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu) */
#define HANDLE_WM_INITMENUPOPUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HMENU)(wParam), (UINT)LOWORD(lParam), (BOOL)HIWORD(lParam)), 0L)
#define FORWARD_WM_INITMENUPOPUP(hwnd, hMenu, item, fSystemMenu, fn) \
    (void)(fn)((hwnd), WM_INITMENUPOPUP, (WPARAM)(HMENU)(hMenu), MAKELPARAM((item),(fSystemMenu)))

/* void Cls_OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags) */
#define HANDLE_WM_MENUSELECT(hwnd, wParam, lParam, fn)                  \
    ((fn)((hwnd), (HMENU)(lParam),  \
    (int)(LOWORD(wParam)),          \
    (HIWORD(wParam) & MF_POPUP) ? GetSubMenu((HMENU)lParam, LOWORD(wParam)) : 0L, \
    (UINT)(((short)HIWORD(wParam) == -1) ? 0xFFFFFFFF : HIWORD(wParam))), 0L)
#define FORWARD_WM_MENUSELECT(hwnd, hmenu, item, hmenuPopup, flags, fn) \
    (void)(fn)((hwnd), WM_MENUSELECT, MAKEWPARAM((item), (flags)), (LPARAM)(HMENU)((hmenu) ? (hmenu) : (hmenuPopup)))

/* DWORD Cls_OnMenuChar(HWND hwnd, UINT ch, UINT flags, HMENU hmenu) */
#define HANDLE_WM_MENUCHAR(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((hwnd), (UINT)(LOWORD(wParam)), (UINT)HIWORD(wParam), (HMENU)(lParam))
#define FORWARD_WM_MENUCHAR(hwnd, ch, flags, hmenu, fn) \
    (DWORD)(fn)((hwnd), WM_MENUCHAR, MAKEWPARAM(flags, (WORD)(ch)), (LPARAM)(HMENU)(hmenu))

/* void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) */
#define HANDLE_WM_COMMAND(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(LOWORD(wParam)), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)
#define FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, fn) \
    (void)(fn)((hwnd), WM_COMMAND, MAKEWPARAM((UINT)(id),(UINT)(codeNotify)), (LPARAM)(HWND)(hwndCtl))

/* void Cls_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos) */
#define HANDLE_WM_HSCROLL(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(lParam), (UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam)), 0L)
#define FORWARD_WM_HSCROLL(hwnd, hwndCtl, code, pos, fn) \
    (void)(fn)((hwnd), WM_HSCROLL, MAKEWPARAM((UINT)(int)(code),(UINT)(int)(pos)), (LPARAM)(UINT)(hwndCtl))

/* void Cls_OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos) */
#define HANDLE_WM_VSCROLL(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(lParam), (UINT)(LOWORD(wParam)),  (int)(short)HIWORD(wParam)), 0L)
#define FORWARD_WM_VSCROLL(hwnd, hwndCtl, code, pos, fn) \
    (void)(fn)((hwnd), WM_VSCROLL, MAKEWPARAM((UINT)(int)(code),(UINT)(int)(pos)), (LPARAM)(HWND)(hwndCtl))

/* void Cls_OnCut(HWND hwnd) */
#define HANDLE_WM_CUT(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_CUT(hwnd, fn) \
    (void)(fn)((hwnd), WM_CUT, 0L, 0L)

/* void Cls_OnCopy(HWND hwnd) */
#define HANDLE_WM_COPY(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_COPY(hwnd, fn) \
    (void)(fn)((hwnd), WM_COPY, 0L, 0L)

/* void Cls_OnPaste(HWND hwnd) */
#define HANDLE_WM_PASTE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_PASTE(hwnd, fn) \
    (void)(fn)((hwnd), WM_PASTE, 0L, 0L)

/* void Cls_OnClear(HWND hwnd) */
#define HANDLE_WM_CLEAR(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_CLEAR(hwnd, fn) \
    (void)(fn)((hwnd), WM_CLEAR, 0L, 0L)

/* void Cls_OnUndo(HWND hwnd) */
#define HANDLE_WM_UNDO(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_UNDO(hwnd, fn) \
    (void)(fn)((hwnd), WM_UNDO, 0L, 0L)

/* HANDLE Cls_OnRenderFormat(HWND hwnd, UINT fmt) */
#define HANDLE_WM_RENDERFORMAT(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HANDLE)(fn)((hwnd), (UINT)(wParam))
#define FORWARD_WM_RENDERFORMAT(hwnd, fmt, fn) \
    (HANDLE)(UINT)(DWORD)(fn)((hwnd), WM_RENDERFORMAT, (WPARAM)(UINT)(fmt), 0L)

/* void Cls_OnRenderAllFormats(HWND hwnd) */
#define HANDLE_WM_RENDERALLFORMATS(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_RENDERALLFORMATS(hwnd, fn) \
    (void)(fn)((hwnd), WM_RENDERALLFORMATS, 0L, 0L)

/* void Cls_OnDestroyClipboard(HWND hwnd) */
#define HANDLE_WM_DESTROYCLIPBOARD(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_DESTROYCLIPBOARD(hwnd, fn) \
    (void)(fn)((hwnd), WM_DESTROYCLIPBOARD, 0L, 0L)

/* void Cls_OnDrawClipboard(HWND hwnd) */
#define HANDLE_WM_DRAWCLIPBOARD(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_DRAWCLIPBOARD(hwnd, fn) \
    (void)(fn)((hwnd), WM_DRAWCLIPBOARD, 0L, 0L)

/* void Cls_OnPaintClipboard(HWND hwnd, HWND hwndCBViewer, const LPPAINTSTRUCT lpPaintStruct) */
#define HANDLE_WM_PAINTCLIPBOARD(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), (const LPPAINTSTRUCT)GlobalLock((HGLOBAL)(lParam))), GlobalUnlock((HGLOBAL)(lParam)), 0L)
#define FORWARD_WM_PAINTCLIPBOARD(hwnd, hwndCBViewer, lpPaintStruct, fn) \
    (void)(fn)((hwnd), WM_PAINTCLIPBOARD, (WPARAM)(HWND)(hwndCBViewer), (LPARAM)(LPPAINTSTRUCT)(lpPaintStruct))

/* void Cls_OnSizeClipboard(HWND hwnd, HWND hwndCBViewer, const LPRECT lprc) */
#define HANDLE_WM_SIZECLIPBOARD(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), (const LPRECT)GlobalLock((HGLOBAL)(lParam))), GlobalUnlock((HGLOBAL)(lParam)), 0L)
#define FORWARD_WM_SIZECLIPBOARD(hwnd, hwndCBViewer, lprc, fn) \
    (void)(fn)((hwnd), WM_SIZECLIPBOARD, (WPARAM)(HWND)(hwndCBViewer), (LPARAM)(LPRECT)(lprc))

/* void Cls_OnVScrollClipboard(HWND hwnd, HWND hwndCBViewer, UINT code, int pos) */
#define HANDLE_WM_VSCROLLCLIPBOARD(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), (UINT)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define FORWARD_WM_VSCROLLCLIPBOARD(hwnd, hwndCBViewer, code, pos, fn) \
    (void)(fn)((hwnd), WM_VSCROLLCLIPBOARD, (WPARAM)(HWND)(hwndCBViewer), MAKELPARAM((code), (pos)))

/* void Cls_OnHScrollClipboard(HWND hwnd, HWND hwndCBViewer, UINT code, int pos) */
#define HANDLE_WM_HSCROLLCLIPBOARD(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), (UINT)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define FORWARD_WM_HSCROLLCLIPBOARD(hwnd, hwndCBViewer, code, pos, fn) \
    (void)(fn)((hwnd), WM_HSCROLLCLIPBOARD, (WPARAM)(HWND)(hwndCBViewer), MAKELPARAM((code), (pos)))

/* void Cls_OnAskCBFormatName(HWND hwnd, int cchMax, LPTSTR rgchName) */
#define HANDLE_WM_ASKCBFORMATNAME(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam), (LPTSTR)(lParam)), 0L)
#define FORWARD_WM_ASKCBFORMATNAME(hwnd, cchMax, rgchName, fn) \
    (void)(fn)((hwnd), WM_ASKCBFORMATNAME, (WPARAM)(int)(cchMax), (LPARAM)(rgchName))

/* void Cls_OnChangeCBChain(HWND hwnd, HWND hwndRemove, HWND hwndNext) */
#define HANDLE_WM_CHANGECBCHAIN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), (HWND)(lParam)), 0L)
#define FORWARD_WM_CHANGECBCHAIN(hwnd, hwndRemove, hwndNext, fn) \
    (void)(fn)((hwnd), WM_CHANGECBCHAIN, (WPARAM)(HWND)(hwndRemove), (LPARAM)(HWND)(hwndNext))

/* BOOL Cls_OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg) */
#define HANDLE_WM_SETCURSOR(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((hwnd), (HWND)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam))
#define FORWARD_WM_SETCURSOR(hwnd, hwndCursor, codeHitTest, msg, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_SETCURSOR, (WPARAM)(HWND)(hwndCursor), MAKELPARAM((codeHitTest), (msg)))

/* void Cls_OnSysCommand(HWND hwnd, UINT cmd, int x, int y) */
#define HANDLE_WM_SYSCOMMAND(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define FORWARD_WM_SYSCOMMAND(hwnd, cmd, x, y, fn) \
    (void)(fn)((hwnd), WM_SYSCOMMAND, (WPARAM)(UINT)(cmd), MAKELPARAM((x), (y)))

/* HWND Cls_MDICreate(HWND hwnd, const LPMDICREATESTRUCT lpmcs) */
#define HANDLE_WM_MDICREATE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((hwnd), (LPMDICREATESTRUCT)(lParam))
#define FORWARD_WM_MDICREATE(hwnd, lpmcs, fn) \
    (HWND)(UINT)(DWORD)(fn)((hwnd), WM_MDICREATE, 0L, (LPARAM)(LPMDICREATESTRUCT)(lpmcs))

/* void Cls_MDIDestroy(HWND hwnd, HWND hwndDestroy) */
#define HANDLE_WM_MDIDESTROY(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define FORWARD_WM_MDIDESTROY(hwnd, hwndDestroy, fn) \
    (void)(fn)((hwnd), WM_MDIDESTROY, (WPARAM)(hwndDestroy), 0L)

/* NOTE: Usable only by MDI client windows */
/* void Cls_MDIActivate(HWND hwnd, BOOL fActive, HWND hwndActivate, HWND hwndDeactivate) */
#define HANDLE_WM_MDIACTIVATE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(lParam == (LPARAM)hwnd), (HWND)(lParam), (HWND)(wParam)), 0L)
#define FORWARD_WM_MDIACTIVATE(hwnd, fActive, hwndActivate, hwndDeactivate, fn) \
    (void)(fn)(hwnd, WM_MDIACTIVATE, (WPARAM)(hwndDeactivate), (LPARAM)(hwndActivate))

/* void Cls_MDIRestore(HWND hwnd, HWND hwndRestore) */
#define HANDLE_WM_MDIRESTORE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define FORWARD_WM_MDIRESTORE(hwnd, hwndRestore, fn) \
    (void)(fn)((hwnd), WM_MDIRESTORE, (WPARAM)(hwndRestore), 0L)

/* HWND Cls_MDINext(HWND hwnd, HWND hwndCur, BOOL fPrev) */
#define HANDLE_WM_MDINEXT(hwnd, wParam, lParam, fn) \
    (LRESULT)(HWND)(fn)((hwnd), (HWND)(wParam), (BOOL)lParam)
#define FORWARD_WM_MDINEXT(hwnd, hwndCur, fPrev, fn) \
    (HWND)(UINT)(DWORD)(fn)((hwnd), WM_MDINEXT, (WPARAM)(hwndCur), (LPARAM)(fPrev))

/* void Cls_MDIMaximize(HWND hwnd, HWND hwndMaximize) */
#define HANDLE_WM_MDIMAXIMIZE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define FORWARD_WM_MDIMAXIMIZE(hwnd, hwndMaximize, fn) \
    (void)(fn)((hwnd), WM_MDIMAXIMIZE, (WPARAM)(hwndMaximize), 0L)

/* BOOL Cls_MDITile(HWND hwnd, UINT cmd) */
#define HANDLE_WM_MDITILE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((hwnd), (UINT)(wParam))
#define FORWARD_WM_MDITILE(hwnd, cmd, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_MDITILE, (WPARAM)(cmd), 0L)

/* BOOL Cls_MDICascade(HWND hwnd, UINT cmd) */
#define HANDLE_WM_MDICASCADE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((hwnd), (UINT)(wParam))
#define FORWARD_WM_MDICASCADE(hwnd, cmd, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_MDICASCADE, (WPARAM)(cmd), 0L)

/* void Cls_MDIIconArrange(HWND hwnd) */
#define HANDLE_WM_MDIICONARRANGE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_MDIICONARRANGE(hwnd, fn) \
    (void)(fn)((hwnd), WM_MDIICONARRANGE, 0L, 0L)

/* HWND Cls_MDIGetActive(HWND hwnd) */
#define HANDLE_WM_MDIGETACTIVE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)(hwnd)
#define FORWARD_WM_MDIGETACTIVE(hwnd, fn) \
    (HWND)(UINT)(DWORD)(fn)((hwnd), WM_MDIGETACTIVE, 0L, 0L)

/* HMENU Cls_MDISetMenu(HWND hwnd, BOOL fRefresh, HMENU hmenuFrame, HMENU hmenuWindow) */
#define HANDLE_WM_MDISETMENU(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((hwnd), (BOOL)(wParam), (HMENU)(wParam), (HMENU)(lParam))
#define FORWARD_WM_MDISETMENU(hwnd, fRefresh, hmenuFrame, hmenuWindow, fn) \
    (HMENU)(UINT)(DWORD)(fn)((hwnd), WM_MDISETMENU, (WPARAM)((fRefresh) ? (hmenuFrame) : 0), (LPARAM)(hmenuWindow))

/* void Cls_OnChildActivate(HWND hwnd) */
#define HANDLE_WM_CHILDACTIVATE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_CHILDACTIVATE(hwnd, fn) \
    (void)(fn)((hwnd), WM_CHILDACTIVATE, 0L, 0L)

/* BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) */
#define HANDLE_WM_INITDIALOG(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(BOOL)(fn)((hwnd), (HWND)(wParam), lParam)
#define FORWARD_WM_INITDIALOG(hwnd, hwndFocus, lParam, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_INITDIALOG, (WPARAM)(HWND)(hwndFocus), (lParam))

/* HWND Cls_OnNextDlgCtl(HWND hwnd, HWND hwndSetFocus, BOOL fNext) */
#define HANDLE_WM_NEXTDLGCTL(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HWND)(fn)((hwnd), (HWND)(wParam), (BOOL)(lParam))
#define FORWARD_WM_NEXTDLGCTL(hwnd, hwndSetFocus, fNext, fn) \
    (HWND)(UINT)(DWORD)(fn)((hwnd), WM_NEXTDLGCTL, (WPARAM)(HWND)(hwndSetFocus), (LPARAM)(fNext))

/* void Cls_OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild) */
#define HANDLE_WM_PARENTNOTIFY(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)LOWORD(wParam), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)
#define FORWARD_WM_PARENTNOTIFY(hwnd, msg, hwndChild, idChild, fn) \
    (void)(fn)((hwnd), WM_PARENTNOTIFY, MAKEWPARAM(msg, idChild), (LPARAM)(hwndChild))

/* void Cls_OnEnterIdle(HWND hwnd, UINT source, HWND hwndSource) */
#define HANDLE_WM_ENTERIDLE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam), (HWND)(lParam)), 0L)
#define FORWARD_WM_ENTERIDLE(hwnd, source, hwndSource, fn) \
    (void)(fn)((hwnd), WM_ENTERIDLE, (WPARAM)(UINT)(source), (LPARAM)(HWND)(hwndSource))

/* UINT Cls_OnGetDlgCode(HWND hwnd, LPMSG lpmsg) */
#define HANDLE_WM_GETDLGCODE(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)(hwnd, (LPMSG)(lParam))
#define FORWARD_WM_GETDLGCODE(hwnd, lpmsg, fn) \
    (UINT)(DWORD)(fn)((hwnd), WM_GETDLGCODE, (lpmsg ? lpmsg->wParam : 0), (LPARAM)(LPMSG)(lpmsg))

/* HBRUSH Cls_OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type) */
#define HANDLE_WM_CTLCOLORMSGBOX(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)(lParam), CTLCOLOR_MSGBOX)
#define FORWARD_WM_CTLCOLORMSGBOX(hwnd, hdc, hwndChild, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLORMSGBOX, (WPARAM)(HDC)(hdc), (LPARAM)(HWND)(hwndChild))

#define HANDLE_WM_CTLCOLOREDIT(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)(lParam), CTLCOLOR_EDIT)
#define FORWARD_WM_CTLCOLOREDIT(hwnd, hdc, hwndChild, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLOREDIT, (WPARAM)(HDC)(hdc), (LPARAM)(HWND)(hwndChild))

#define HANDLE_WM_CTLCOLORLISTBOX(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)(lParam), CTLCOLOR_LISTBOX)
#define FORWARD_WM_CTLCOLORLISTBOX(hwnd, hdc, hwndChild, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLORLISTBOX, (WPARAM)(HDC)(hdc), (LPARAM)(HWND)(hwndChild))

#define HANDLE_WM_CTLCOLORBTN(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)(lParam), CTLCOLOR_BTN)
#define FORWARD_WM_CTLCOLORBTN(hwnd, hdc, hwndChild, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLORBTN, (WPARAM)(HDC)(hdc), (LPARAM)(HWND)(hwndChild))

#define HANDLE_WM_CTLCOLORDLG(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)(lParam), CTLCOLOR_DLG)
#define FORWARD_WM_CTLCOLORDLG(hwnd, hdc, hwndChild, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLORDLG, (WPARAM)(HDC)(hdc), (LPARAM)(HWND)(hwndChild))

#define HANDLE_WM_CTLCOLORSCROLLBAR(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)(lParam), CTLCOLOR_SCROLLBAR)
#define FORWARD_WM_CTLCOLORSCROLLBAR(hwnd, hdc, hwndChild, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLORSCROLLBAR, (WPARAM)(HDC)(hdc), (LPARAM)(HWND)(hwndChild))

#define HANDLE_WM_CTLCOLORSTATIC(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((hwnd), (HDC)(wParam), (HWND)(lParam), CTLCOLOR_STATIC)
#define FORWARD_WM_CTLCOLORSTATIC(hwnd, hdc, hwndChild, fn) \
    (HBRUSH)(UINT)(DWORD)(fn)((hwnd), WM_CTLCOLORSTATIC, (WPARAM)(HDC)(hdc), (LPARAM)(HWND)(hwndChild))

/* void Cls_OnSetFont(HWND hwndCtl, HFONT hfont, BOOL fRedraw) */
#define HANDLE_WM_SETFONT(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HFONT)(wParam), (BOOL)(lParam)), 0L)
#define FORWARD_WM_SETFONT(hwnd, hfont, fRedraw, fn) \
    (void)(fn)((hwnd), WM_SETFONT, (WPARAM)(HFONT)(hfont), (LPARAM)(BOOL)(fRedraw))

/* HFONT Cls_OnGetFont(HWND hwnd) */
#define HANDLE_WM_GETFONT(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HFONT)(fn)(hwnd)
#define FORWARD_WM_GETFONT(hwnd, fn) \
    (HFONT)(UINT)(DWORD)(fn)((hwnd), WM_GETFONT, 0L, 0L)

/* void Cls_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem) */
#define HANDLE_WM_DRAWITEM(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (const DRAWITEMSTRUCT *)(lParam)), 0L)
#define FORWARD_WM_DRAWITEM(hwnd, lpDrawItem, fn) \
    (void)(fn)((hwnd), WM_DRAWITEM, (WPARAM)(((const DRAWITEMSTRUCT *)lpDrawItem)->CtlID), (LPARAM)(const DRAWITEMSTRUCT *)(lpDrawItem))

/* void Cls_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem) */
#define HANDLE_WM_MEASUREITEM(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (MEASUREITEMSTRUCT *)(lParam)), 0L)
#define FORWARD_WM_MEASUREITEM(hwnd, lpMeasureItem, fn) \
    (void)(fn)((hwnd), WM_MEASUREITEM, (WPARAM)(((MEASUREITEMSTRUCT *)lpMeasureItem)->CtlID), (LPARAM)(MEASUREITEMSTRUCT *)(lpMeasureItem))

/* void Cls_OnDeleteItem(HWND hwnd, const DELETEITEMSTRUCT * lpDeleteItem) */
#define HANDLE_WM_DELETEITEM(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (const DELETEITEMSTRUCT *)(lParam)), 0L)
#define FORWARD_WM_DELETEITEM(hwnd, lpDeleteItem, fn) \
    (void)(fn)((hwnd), WM_DELETEITEM, (WPARAM)(((const DELETEITEMSTRUCT *)(lpDeleteItem))->CtlID), (LPARAM)(const DELETEITEMSTRUCT *)(lpDeleteItem))

/* int Cls_OnCompareItem(HWND hwnd, const COMPAREITEMSTRUCT * lpCompareItem) */
#define HANDLE_WM_COMPAREITEM(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((hwnd), (const COMPAREITEMSTRUCT *)(lParam))
#define FORWARD_WM_COMPAREITEM(hwnd, lpCompareItem, fn) \
    (int)(DWORD)(fn)((hwnd), WM_COMPAREITEM, (WPARAM)(((const COMPAREITEMSTRUCT *)(lpCompareItem))->CtlID), (LPARAM)(const COMPAREITEMSTRUCT *)(lpCompareItem))

/* int Cls_OnVkeyToItem(HWND hwnd, UINT vk, HWND hwndListbox, int iCaret) */
#define HANDLE_WM_VKEYTOITEM(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((hwnd), (UINT)LOWORD(wParam), (HWND)(lParam), (int)(short)HIWORD(wParam))
#define FORWARD_WM_VKEYTOITEM(hwnd, vk, hwndListBox, iCaret, fn) \
    (int)(DWORD)(fn)((hwnd), WM_VKEYTOITEM, MAKEWPARAM((vk), (iCaret)), (LPARAM)(hwndListBox))

/* int Cls_OnCharToItem(HWND hwnd, UINT ch, HWND hwndListbox, int iCaret) */
#define HANDLE_WM_CHARTOITEM(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((hwnd), (UINT)LOWORD(wParam), (HWND)(lParam), (int)(short)HIWORD(wParam))
#define FORWARD_WM_CHARTOITEM(hwnd, ch, hwndListBox, iCaret, fn) \
    (int)(DWORD)(fn)((hwnd), WM_CHARTOITEM, MAKEWPARAM((UINT)(iCaret), (UINT)(ch)), (LPARAM)(hwndListBox))

/* void Cls_OnQueueSync(HWND hwnd) */
#define HANDLE_WM_QUEUESYNC(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define FORWARD_WM_QUEUESYNC(hwnd, fn) \
    (void)(fn)((hwnd), WM_QUEUESYNC, 0L, 0L)
#if (WINVER >= 0x030a)
/* void Cls_OnCommNotify(HWND hwnd, int cid, UINT flags) */
#define HANDLE_WM_COMMNOTIFY(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam), (UINT)LOWORD(lParam)), 0L)
#define FORWARD_WM_COMMNOTIFY(hwnd, cid, flags, fn) \
    (void)(fn)((hwnd), WM_COMMNOTIFY, (WPARAM)(cid), MAKELPARAM((flags), 0))
#endif

/****** COMBOBOX control message APIs *****************************************/
#define ComboBox_GetCount(hwndCtl)            \
		((int)(DWORD)SendMessage((hwndCtl), CB_GETCOUNT, 0L, 0L))

#define ComboBox_GetCurSel(hwndCtl)           \
		((int)(DWORD)SendMessage((hwndCtl), CB_GETCURSEL, 0L, 0L))

#define ComboBox_SetCurSel(hwndCtl, index)    \
		((int)(DWORD)SendMessage((hwndCtl), CB_SETCURSEL, (WPARAM)(int)(index), 0L))

#define ComboBox_GetLBTextLen(hwndCtl, index) \
		((int)(DWORD)SendMessage((hwndCtl), CB_GETLBTEXTLEN, (WPARAM)(int)(index), 0L))

#define ComboBox_DeleteString(hwndCtl, index) \
		((int)(DWORD)SendMessage((hwndCtl), CB_DELETESTRING, (WPARAM)(int)(index), 0L))

#define ComboBox_FindString(hwndCtl, indexStart, lpszFind) \
		((int)(DWORD)SendMessage((hwndCtl), CB_FINDSTRING, (WPARAM)(int)(indexStart), (LPARAM)(LPCTSTR)(lpszFind)))

#define ComboBox_ResetContent(hwndCtl)      \
		((int)(DWORD)SendMessage((hwndCtl), CB_RESETCONTENT, 0L, 0L))
		
#define ComboBox_AddString(hwndCtl, lpsz)   \
		((int)(DWORD)SendMessage((hwndCtl), CB_ADDSTRING, 0L, (LPARAM)(LPCTSTR)(lpsz)))
#define ComboBox_GetLBTextLen(hwndCtl, index)       \
		((int)(DWORD)SendMessage((hwndCtl), CB_GETLBTEXTLEN, (WPARAM)(int)(index), 0L))
#define ComboBox_GetLBText(hwndCtl, index, lpszBuffer) \
		((int)(DWORD)SendMessage((hwndCtl), CB_GETLBTEXT, (WPARAM)(int)(index), (LPARAM)(LPCTSTR)(lpszBuffer)))

#define ComboBox_GetDroppedState(hwndCtl)             \
           ((BOOL)(DWORD)SendMessage((hwndCtl), CB_GETDROPPEDSTATE, 0L, 0L))
#define ComboBox_GetDroppedControlRect(hwndCtl, lprc) \
	   ((void)SendMessage((hwndCtl), CB_GETDROPPEDCONTROLRECT, 0L, (LPARAM)(RECT *)(lprc)))

/****** ListBox control message APIs *****************************************/

#define ListBox_Enable(hwndCtl, fEnable)            EnableWindow((hwndCtl), (fEnable))
#define ListBox_GetCount(hwndCtl)          \
	   ((int)(DWORD)SendMessage((hwndCtl), LB_GETCOUNT, 0L, 0L))
#define ListBox_ResetContent(hwndCtl)      \
	   ((BOOL)(DWORD)SendMessage((hwndCtl), LB_RESETCONTENT, 0L, 0L))

#define ListBox_GetItemData(hwndCtl, index)     \
	((LRESULT)(DWORD)SendMessage((hwndCtl), LB_GETITEMDATA, (WPARAM)(int)(index), 0L))

#define ListBox_GetCurSel(hwndCtl)        \
	((int)(DWORD)SendMessage((hwndCtl), LB_GETCURSEL, 0L, 0L))
#define ListBox_SetCurSel(hwndCtl, index) \
	((int)(DWORD)SendMessage((hwndCtl), LB_SETCURSEL, (WPARAM)(int)(index), 0L))

#define ListBox_AddString(hwndCtl, lpsz)           \
	((int)(DWORD)SendMessage((hwndCtl), LB_ADDSTRING, 0L, (LPARAM)(LPCTSTR)(lpsz)))
#define ListBox_InsertString(hwndCtl, index, lpsz) \
	((int)(DWORD)SendMessage((hwndCtl), LB_INSERTSTRING, (WPARAM)(int)(index), (LPARAM)(LPCTSTR)(lpsz)))
	
#define ListBox_AddItemData(hwndCtl, data) \
	((int)(DWORD)SendMessage((hwndCtl), LB_ADDSTRING, 0L, (LPARAM)(data)))
#define ListBox_InsertItemData(hwndCtl, index, data) \
	((int)(DWORD)SendMessage((hwndCtl), LB_INSERTSTRING, (WPARAM)(int)(index), (LPARAM)(data)))

#define ListBox_FindString(hwndCtl, indexStart, lpszFind) \
	((int)(DWORD)SendMessage((hwndCtl), LB_FINDSTRING, (WPARAM)(int)(indexStart), (LPARAM)(LPCTSTR)(lpszFind)))
#define ListBox_FindStringExact(hwndCtl, indexStart, lpszFind) \
	((int)(DWORD)SendMessage((hwndCtl), LB_FINDSTRINGEXACT, (WPARAM)(int)(indexStart), (LPARAM)(LPCTSTR)(lpszFind)))

#define Edit_LineFromChar(hwndCtl, ich)   \
	((int)(DWORD)SendMessage((hwndCtl), EM_LINEFROMCHAR, (WPARAM)(int)(ich), 0L))
#define Edit_LineIndex(hwndCtl, line)     \
	((int)(DWORD)SendMessage((hwndCtl), EM_LINEINDEX, (WPARAM)(int)(line), 0L))
#define Edit_LineLength(hwndCtl, line)    \
	((int)(DWORD)SendMessage((hwndCtl), EM_LINELENGTH, (WPARAM)(int)(line), 0L))

/****** Static control message APIs ******************************************/

#define Static_Enable(hwndCtl, fEnable)         EnableWindow((hwndCtl), (fEnable))

#define Static_GetText(hwndCtl, lpch, cchMax)   GetWindowText((hwndCtl), (lpch), (cchMax))
#define Static_GetTextLength(hwndCtl)           GetWindowTextLength(hwndCtl)
#define Static_SetText(hwndCtl, lpsz)           SetWindowText((hwndCtl), (lpsz))

#define Static_SetIcon(hwndCtl, hIcon)          ((HICON)(UINT)(DWORD)SendMessage((hwndCtl), STM_SETICON, (WPARAM)(HICON)(hIcon), 0L))
#define Static_GetIcon(hwndCtl, hIcon)          ((HICON)(UINT)(DWORD)SendMessage((hwndCtl), STM_GETICON, 0L, 0L))

/****** Button control message APIs ******************************************/

#define Button_Enable(hwndCtl, fEnable)         EnableWindow((hwndCtl), (fEnable))

#define Button_GetText(hwndCtl, lpch, cchMax)   GetWindowText((hwndCtl), (lpch), (cchMax))
#define Button_GetTextLength(hwndCtl)           GetWindowTextLength(hwndCtl)
#define Button_SetText(hwndCtl, lpsz)           SetWindowText((hwndCtl), (lpsz))

#define Button_GetCheck(hwndCtl)            ((int)(DWORD)SendMessage((hwndCtl), BM_GETCHECK, 0L, 0L))
#define Button_SetCheck(hwndCtl, check)     ((void)SendMessage((hwndCtl), BM_SETCHECK, (WPARAM)(int)(check), 0L))

#define Button_GetState(hwndCtl)            ((int)(DWORD)SendMessage((hwndCtl), BM_GETSTATE, 0L, 0L))
#define Button_SetState(hwndCtl, state)     ((UINT)(DWORD)SendMessage((hwndCtl), BM_SETSTATE, (WPARAM)(int)(state), 0L))

#define Button_SetStyle(hwndCtl, style, fRedraw) ((void)SendMessage((hwndCtl), BM_SETSTYLE, (WPARAM)LOWORD(style), MAKELPARAM(((fRedraw) ? TRUE : FALSE), 0)))

/****** Edit control message APIs ********************************************/

#define Edit_SetSel(hwndCtl, ichStart, ichEnd)   \
	((void)SendMessage((hwndCtl), EM_SETSEL, (ichStart), (ichEnd)))

#define Edit_GetText(hwndCtl, lpch, cchMax)     GetWindowText((hwndCtl), (lpch), (cchMax))
#define Edit_GetTextLength(hwndCtl)             GetWindowTextLength(hwndCtl)
#define Edit_SetText(hwndCtl, lpsz)             SetWindowText((hwndCtl), (lpsz))
	
#define Edit_GetModify(hwndCtl)            \
	((BOOL)(DWORD)SendMessage((hwndCtl), EM_GETMODIFY, 0L, 0L))
#define Edit_SetModify(hwndCtl, fModified) \
	((void)SendMessage((hwndCtl), EM_SETMODIFY, (WPARAM)(UINT)(fModified), 0L))
	
/* void Cls_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem) */
#define HANDLE_WM_MEASUREITEM(hwnd, wParam, lParam, fn) \
	    ((fn)((hwnd), (MEASUREITEMSTRUCT *)(lParam)), 0L)
#define FORWARD_WM_MEASUREITEM(hwnd, lpMeasureItem, fn) \
	    (void)(fn)((hwnd), WM_MEASUREITEM, (WPARAM)(((MEASUREITEMSTRUCT *)lpMeasureItem)->CtlID), (LPARAM)(MEASUREITEMSTRUCT *)(lpMeasureItem))

/* void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) */
#define HANDLE_WM_COMMAND(hwnd, wParam, lParam, fn) \
	    ((fn)((hwnd), (int)(LOWORD(wParam)), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)
#define FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, fn) \
	    (void)(fn)((hwnd), WM_COMMAND, MAKEWPARAM((UINT)(id),(UINT)(codeNotify)), (LPARAM)(HWND)(hwndCtl))

/* void Cls_OnTimer(HWND hwnd, UINT id) */
#define HANDLE_WM_TIMER(hwnd, wParam, lParam, fn) ((fn)((hwnd), (UINT)(wParam)), 0L)
#define FORWARD_WM_TIMER(hwnd, id, fn) (void)(fn)((hwnd), WM_TIMER, (WPARAM)(UINT)(id), 0L)

/* void Cls_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu) */
#define HANDLE_WM_INITMENUPOPUP(hwnd, wParam, lParam, fn) \
        ((fn)((hwnd), (HMENU)(wParam), (UINT)LOWORD(lParam), (BOOL)HIWORD(lParam)), 0L)
#define FORWARD_WM_INITMENUPOPUP(hwnd, hMenu, item, fSystemMenu, fn) \
        (void)(fn)((hwnd), WM_INITMENUPOPUP, (WPARAM)(HMENU)(hMenu), MAKELPARAM((item),(fSystemMenu)))

/* UINT Cls_OnNCHitTest(HWND hwnd, int x, int y) */
#define HANDLE_WM_NCHITTEST(hwnd, wParam, lParam, fn) \
	(LRESULT)(DWORD)(UINT)(fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))
#define FORWARD_WM_NCHITTEST(hwnd, x, y, fn) \
	(UINT)(DWORD)(fn)((hwnd), WM_NCHITTEST, 0L, MAKELPARAM((x), (y)))

/* HFONT Cls_OnGetFont(HWND hwnd) */
#define HANDLE_WM_GETFONT(hwnd, wParam, lParam, fn)  (LRESULT)(DWORD)(UINT)(HFONT)(fn)(hwnd)
#define FORWARD_WM_GETFONT(hwnd, fn)  (HFONT)(UINT)(DWORD)(fn)((hwnd), WM_GETFONT, 0L, 0L)

/* void Cls_OnSetFont(HWND hwndCtl, HFONT hfont, BOOL fRedraw) */
#define HANDLE_WM_SETFONT(hwnd, wParam, lParam, fn) \
	    ((fn)((hwnd), (HFONT)(wParam), (BOOL)(lParam)), 0L)
#define FORWARD_WM_SETFONT(hwnd, hfont, fRedraw, fn) \
			 (void)(fn)((hwnd), WM_SETFONT, (WPARAM)(HFONT)(hfont), (LPARAM)(BOOL)(fRedraw))

/* void Cls_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo) */
#define HANDLE_WM_GETMINMAXINFO(hwnd, wParam, lParam, fn) \
			 ((fn)((hwnd), (LPMINMAXINFO)(lParam)), 0L)
#define FORWARD_WM_GETMINMAXINFO(hwnd, lpMinMaxInfo, fn) \
			(void)(fn)((hwnd), WM_GETMINMAXINFO, 0L, (LPARAM)(LPMINMAXINFO)(lpMinMaxInfo))

/* void Cls_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT lpDrawItem) */
#define HANDLE_WM_DRAWITEM(hwnd, wParam, lParam, fn) \
	    ((fn)((hwnd), (const DRAWITEMSTRUCT *)(lParam)), 0L)
#define FORWARD_WM_DRAWITEM(hwnd, lpDrawItem, fn) \
        (void)(fn)((hwnd), WM_DRAWITEM, (WPARAM)(((const DRAWITEMSTRUCT *)lpDrawItem)->CtlID), (LPARAM)(const DRAWITEMSTRUCT *)(lpDrawItem))
				
/****** C runtime porting macros ****************************************/

#if (__IBMC__ >= 400) || (__IBMCPP__ >= 360)
   // defines already in compiler includes
#else
#define _ncalloc    calloc
#define _nexpand    _expand
#define _ffree      free
#define _fmalloc    malloc
#define _fmemccpy   _memccpy
#define _fmemchr    memchr
#define _fmemcmp    memcmp
#define _fmemcpy    memcpy
#define _fmemicmp   _memicmp
#define _fmemmove   memmove
#define _fmemset    memset
#define _fmsize     _msize
#define _frealloc   realloc
#define _fstrcat    strcat
#define _fstrchr    strchr
#define _fstrcmp    strcmp
#define _fstrcpy    strcpy
#define _fstrcspn   strcspn
#define _fstrdup    _strdup
#define _fstricmp   _stricmp
#define _fstrlen    strlen
#define _fstrlwr    _strlwr
#define _fstrncat   strncat
#define _fstrncmp   strncmp
#define _fstrncpy   strncpy
#define _fstrnicmp  _strnicmp
#define _fstrnset   _strnset
#define _fstrpbrk   strpbrk
#define _fstrrchr   strrchr
#define _fstrrev    _strrev
#define _fstrset    _strset
#define _fstrspn    strspn
#define _fstrstr    strstr
#define _fstrtok    strtok
#define _fstrupr    _strupr
#define _nfree      free
#define _nmalloc    malloc
#define _nmsize     _msize
#define _nrealloc   realloc
#define _nstrdup    _strdup
#define hmemcpy16     MoveMemory
#endif

#ifdef __cplusplus
}
#endif
				
#endif

