#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winreg.h>
#include <winnt.h>
#include <wincon.h>
#include <winthunk.h>
#include <winuser.h>
#include <ddeml.h>
#include <dde.h>
#include <winnls.h>
#include <ntddk.h>
#include <heapstring.h>

#define DBG_LOCALLOG    DBG_trace
#include <dbglocal.h>

#define DBGWRAP_MODULE "USER32"
#include <dbgwrap.h>



LRESULT WINAPI SendIMEMessageExA(HWND hwnd, LPARAM lparam);
LRESULT WINAPI SendIMEMessageExW(HWND hwnd, LPARAM lparam);
BOOL WIN32API CloseDesktop(HDESK hDesktop);
BOOL WIN32API CloseWindowStation(HWINSTA hWinSta);
HDESK WIN32API CreateDesktopA(LPCTSTR               lpszDesktop,
                              LPCTSTR               lpszDevice,
                              LPDEVMODEA            pDevMode,
                              DWORD                 dwFlags,
                              DWORD                 dwDesiredAccess,
                              LPSECURITY_ATTRIBUTES lpsa);
HDESK WIN32API CreateDesktopW(LPCTSTR               lpszDesktop,
                              LPCTSTR               lpszDevice,
                              LPDEVMODEW            pDevMode,
                              DWORD                 dwFlags,
                              DWORD                 dwDesiredAccess,
                              LPSECURITY_ATTRIBUTES lpsa);

HWINSTA WIN32API CreateWindowStationA(LPTSTR lpWinSta,
                                      DWORD  dwReserved,
                                      DWORD  dwDesiredAccess,
                                      LPSECURITY_ATTRIBUTES lpsa);
HWINSTA WIN32API CreateWindowStationW(LPWSTR lpWinSta,
                                      DWORD  dwReserved,
                                      DWORD  dwDesiredAccess,
                                      LPSECURITY_ATTRIBUTES lpsa);

BOOL WIN32API EnumDesktopWindows(HDESK       hDesktop,
                                 WNDENUMPROC lpfn,
                                 LPARAM      lParam);

BOOL WIN32API EnumWindowStationsA(WINSTAENUMPROCA lpEnumFunc,
                                  LPARAM          lParam);
BOOL WIN32API EnumWindowStationsW(WINSTAENUMPROCW lpEnumFunc,
                                  LPARAM          lParam);

BOOL WIN32API GetNextQueueWindow(DWORD x1, DWORD x2);
HWINSTA WIN32API GetProcessWindowStation(VOID);
HRESULT WIN32API GetProgmanWindow ( );
HRESULT WIN32API SetProgmanWindow ( HWND hwnd );
HRESULT WIN32API GetTaskmanWindow ( );
HRESULT WIN32API SetTaskmanWindow ( HWND hwnd );
BOOL    WIN32API SetShellWindow(DWORD x1);
HRESULT WIN32API SetShellWindowEx (HWND hwndProgman,
                                   HWND hwndListView);

BOOL WIN32API IsHungAppWindow(HWND  hwnd,
                              ULONG ulDummy);

BOOL WIN32API GetUserObjectInformationA(HANDLE  hObj,
                                        int     nIndex,
                                        PVOID   pvInfo,
                                        DWORD   nLength,
                                        LPDWORD lpnLengthNeeded);
BOOL WIN32API GetUserObjectInformationW(HANDLE  hObj,
                                        int     nIndex,
                                        PVOID   pvInfo,
                                        DWORD   nLength,
                                        LPDWORD lpnLengthNeeded);
BOOL WIN32API GetUserObjectSecurity(HANDLE                hObj,
                                    PSECURITY_INFORMATION pSIRequested,
                                    PSECURITY_DESCRIPTOR  pSID,
                                    DWORD                 nLength,
                                    LPDWORD               lpnLengthNeeded);


HDESK WIN32API OpenDesktopA(LPCTSTR lpszDesktopName,
                            DWORD   dwFlags,
                            BOOL    fInherit,
                            DWORD   dwDesiredAccess);
HDESK WIN32API OpenDesktopW(LPCTSTR lpszDesktopName,
                            DWORD   dwFlags,
                            BOOL    fInherit,
                            DWORD   dwDesiredAccess);

HDESK WIN32API OpenInputDesktop(DWORD dwFlags,
                                BOOL  fInherit,
                                DWORD dwDesiredAccess);


HWINSTA WIN32API OpenWindowStationA(LPCTSTR lpszWinStaName,
                                    BOOL    fInherit,
                                    DWORD   dwDesiredAccess);
HWINSTA WIN32API OpenWindowStationW(LPCTSTR lpszWinStaName,
                                    BOOL    fInherit,
                                    DWORD   dwDesiredAccess);

BOOL WIN32API SetProcessWindowStation(HWINSTA hWinSta);
BOOL WIN32API SetThreadDesktop(HDESK hDesktop);
BOOL WIN32API SetUserObjectInformationA(HANDLE hObject,
                                        int    nIndex,
                                        PVOID  lpvInfo,
                                        DWORD  cbInfo);
BOOL WIN32API SetUserObjectInformationW(HANDLE hObject,
                                        int    nIndex,
                                        PVOID  lpvInfo,
                                        DWORD  cbInfo);
BOOL WIN32API SetUserObjectSecurity(HANDLE hObject,
                                    PSECURITY_INFORMATION psi,
                                    PSECURITY_DESCRIPTOR  psd);

BOOL WIN32API SwitchDesktop(HDESK hDesktop);
BOOL WIN32API PlaySoundEvent(DWORD x1);
BOOL WIN32API SetSysColorsTemp(void);
BOOL WIN32API RegisterNetworkCapabilities(DWORD x1, DWORD x2);
BOOL WIN32API RegisterSystemThread(DWORD x1, DWORD x2);
BOOL WIN32API YieldTask(void);
BOOL WIN32API WinOldAppHackoMatic(DWORD x1);
BOOL WIN32API IsHungThread(DWORD x1);
BOOL WIN32API UserSignalProc(DWORD x1, DWORD x2, DWORD x3, DWORD x4);
DWORD WIN32API RegisterTasklist (DWORD x);
HDESK WIN32API GetThreadDesktop(DWORD dwThreadId);
BOOL WINAPI WINNLSEnableIME(HWND hwnd, BOOL bOpen);
UINT WINAPI WINNLSGetIMEHotkey(HWND hwnd);
BOOL WINAPI WINNLSGetEnableStatus(HWND hwnd);
DWORD WIN32API SetLogonNotifyWindow(HWINSTA hwinsta,HWND hwnd);
BOOL WIN32API SetProcessWindowStation(HWINSTA hWinSta);
void WIN32API NotifyWinEvent(DWORD event, HWND hwnd, LONG idObjectType, LONG idObject);
DWORD WIN32API UnhookWinEvent(DWORD arg1);
DWORD WIN32API SetWinEventHook(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7);
BOOL WIN32API SysErrorBox(DWORD x1, DWORD x2, DWORD x3);
BOOL WIN32API EndTask(DWORD x1, DWORD x2, DWORD x3);
LRESULT WINAPI EditWndProcA( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );


DEBUGWRAP12(AttachThreadInput)
DEBUGWRAP_LVL2_20(CallWindowProcA)
DEBUGWRAP_LVL2_20(CallWindowProcW)
DEBUGWRAP16(CheckRadioButton)


DEBUGWRAP4(CloseDesktop)
DEBUGWRAP4(CloseWindowStation)
DEBUGWRAP20(CopyImage)
DEBUGWRAP24(CreateDesktopA)
DEBUGWRAP24(CreateDesktopW)
DEBUGWRAP16(CreateWindowStationA)
DEBUGWRAP16(CreateWindowStationW)

DEBUGWRAP40(CreateMDIWindowA)
DEBUGWRAP40(CreateMDIWindowW)
DEBUGWRAP4(ArrangeIconicWindows)
DEBUGWRAP8(CascadeChildWindows)
DEBUGWRAP20(CascadeWindows)

DEBUGWRAP12(DragDetect)
DEBUGWRAP20(DragObject)
DEBUGWRAP16(EditWndProcA)
DEBUGWRAP12(EndTask)
DEBUGWRAP12(EnumDesktopWindows)
DEBUGWRAP12(EnumDesktopsA)
DEBUGWRAP12(EnumDesktopsW)
DEBUGWRAP8(EnumWindowStationsA)
DEBUGWRAP8(EnumWindowStationsW)
DEBUGWRAP8(ExitWindowsEx)


DEBUGWRAP0(GetInputState)
DEBUGWRAP12(GetInternalWindowPos)
DEBUGWRAP0(GetProcessWindowStation)
DEBUGWRAP0(GetProgmanWindow)
DEBUGWRAP4(GetQueueStatus)
DEBUGWRAP0(GetShellWindow)
DEBUGWRAP4(GetSysColor)
DEBUGWRAP4(GetSysColorBrush)
DEBUGWRAP4(GetSystemMetrics)
DEBUGWRAP0(GetTaskmanWindow)
DEBUGWRAP4(GetThreadDesktop)

DEBUGWRAP20(GetUserObjectInformationA)
DEBUGWRAP20(GetUserObjectInformationW)
DEBUGWRAP20(GetUserObjectSecurity)



DEBUGWRAP4(IsHungThread)
DEBUGWRAP8(IsHungAppWindow)
DEBUGWRAP4(MessageBeep)


DEBUGWRAP16(OpenDesktopA)
DEBUGWRAP16(OpenDesktopW)
DEBUGWRAP12(OpenInputDesktop)
DEBUGWRAP12(OpenWindowStationA)
DEBUGWRAP12(OpenWindowStationW)
DEBUGWRAP4(PaintDesktop)
DEBUGWRAP4(PlaySoundEvent)
DEBUGWRAP8(RegisterNetworkCapabilities)
DEBUGWRAP8(RegisterSystemThread)
DEBUGWRAP4(RegisterTasklist)
DEBUGWRAP4(SetDebugErrorLevel)
DEBUGWRAP4(SetDeskWallPaper)
DEBUGWRAP8(SetLastErrorEx)
DEBUGWRAP8(SetLogonNotifyWindow)
DEBUGWRAP4(SetProcessWindowStation)
DEBUGWRAP4(SetProgmanWindow)
DEBUGWRAP4(SetShellWindow)
DEBUGWRAP8(SetShellWindowEx)
DEBUGWRAP12(SetSysColors)
DEBUGWRAP0(SetSysColorsTemp)


DEBUGWRAP4(SetTaskmanWindow)
DEBUGWRAP4(SetThreadDesktop)
DEBUGWRAP16(SetUserObjectInformationA)
DEBUGWRAP16(SetUserObjectInformationW)
DEBUGWRAP12(SetUserObjectSecurity)



DEBUGWRAP4(SwitchDesktop)
DEBUGWRAP8(SwitchToThisWindow)
DEBUGWRAP12(SysErrorBox)
DEBUGWRAP16(SystemParametersInfoA)
DEBUGWRAP16(SystemParametersInfoW)
DEBUGWRAP8(TileChildWindows)
DEBUGWRAP20(TileWindows)

DEBUGWRAP16(UserSignalProc)
DEBUGWRAP8(WaitForInputIdle)
DEBUGWRAP16(WinHelpA)
DEBUGWRAP16(WinHelpW)

DEBUGWRAP4(WinOldAppHackoMatic)

DEBUGWRAP0(YieldTask)
DEBUGWRAP12(wvsprintfA)
DEBUGWRAP12(wvsprintfW)
DEBUGWRAP8(GetGUIThreadInfo)
DEBUGWRAP16(NotifyWinEvent)
DEBUGWRAP4(UnhookWinEvent)
DEBUGWRAP28(SetWinEventHook)
DEBUGWRAP8(WINNLSEnableIME)
DEBUGWRAP4(WINNLSGetEnableStatus)
DEBUGWRAP4(WINNLSGetIMEHotkey)




#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_menu
DEBUGWRAP16(AppendMenuA)
DEBUGWRAP16(AppendMenuW)
DEBUGWRAP20(ChangeMenuA)
DEBUGWRAP20(ChangeMenuW)
DEBUGWRAP12(CheckMenuItem)
DEBUGWRAP20(CheckMenuRadioItem)
DEBUGWRAP0(CreateMenu)
DEBUGWRAP0(CreatePopupMenu)
DEBUGWRAP12(DeleteMenu)
DEBUGWRAP4(DestroyMenu)
DEBUGWRAP12(EnableMenuItem)
DEBUGWRAP4(DrawMenuBar)
DEBUGWRAP4(GetMenu)
DEBUGWRAP0(GetMenuCheckMarkDimensions)
DEBUGWRAP4(GetMenuContextHelpId)
DEBUGWRAP12(GetMenuDefaultItem)
DEBUGWRAP4(GetMenuItemCount)
DEBUGWRAP8(GetMenuItemID)
DEBUGWRAP16(GetMenuItemInfoA)
DEBUGWRAP16(GetMenuItemInfoW)
DEBUGWRAP16(GetMenuItemRect)
DEBUGWRAP12(GetMenuState)
DEBUGWRAP20(GetMenuStringA)
DEBUGWRAP20(GetMenuStringW)
DEBUGWRAP8(GetSubMenu)
DEBUGWRAP8(GetSystemMenu)
DEBUGWRAP16(HiliteMenuItem)
DEBUGWRAP20(InsertMenuA)
DEBUGWRAP16(InsertMenuItemA)
DEBUGWRAP16(InsertMenuItemW)
DEBUGWRAP20(InsertMenuW)
DEBUGWRAP4(IsMenu)
DEBUGWRAP8(LoadMenuA)
DEBUGWRAP4(LoadMenuIndirectW)
DEBUGWRAP8(LoadMenuW)
DEBUGWRAP16(MenuItemFromPoint)
DEBUGWRAP20(ModifyMenuA)
DEBUGWRAP20(ModifyMenuW)
DEBUGWRAP12(RemoveMenu)
DEBUGWRAP8(SetMenu)
DEBUGWRAP8(SetMenuContextHelpId)
DEBUGWRAP12(SetMenuDefaultItem)
DEBUGWRAP20(SetMenuItemBitmaps)
DEBUGWRAP16(SetMenuItemInfoA)
DEBUGWRAP16(SetMenuItemInfoW)
DEBUGWRAP28(TrackPopupMenu)
DEBUGWRAP24(TrackPopupMenuEx)
DEBUGWRAP8(GetMenuInfo)
DEBUGWRAP8(SetMenuInfo)
DEBUGWRAP_LVL2_12(TranslateAcceleratorA)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_windowclass
DEBUGWRAP12(GetClassInfoA)
DEBUGWRAP12(GetClassInfoExA)
DEBUGWRAP12(GetClassInfoExW)
DEBUGWRAP12(GetClassInfoW)
DEBUGWRAP8(GetClassLongA)
DEBUGWRAP8(GetClassLongW)
DEBUGWRAP12(GetClassNameA)
DEBUGWRAP12(GetClassNameW)
DEBUGWRAP8(GetClassWord)
DEBUGWRAP4(RegisterClassA)
DEBUGWRAP4(RegisterClassExA)
DEBUGWRAP4(RegisterClassExW)
DEBUGWRAP4(RegisterClassW)
DEBUGWRAP12(SetClassLongA)
DEBUGWRAP12(SetClassLongW)
DEBUGWRAP12(SetClassWord)
DEBUGWRAP8(UnregisterClassA)
DEBUGWRAP8(UnregisterClassW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_clipboard
DEBUGWRAP8(ChangeClipboardChain)
DEBUGWRAP0(CloseClipboard)
DEBUGWRAP0(CountClipboardFormats)
DEBUGWRAP0(EmptyClipboard)
DEBUGWRAP4(EnumClipboardFormats)
DEBUGWRAP4(GetClipboardData)
DEBUGWRAP12(GetClipboardFormatNameA)
DEBUGWRAP12(GetClipboardFormatNameW)
DEBUGWRAP0(GetClipboardOwner)
DEBUGWRAP0(GetClipboardViewer)
DEBUGWRAP0(GetOpenClipboardWindow)
DEBUGWRAP8(GetPriorityClipboardFormat)
DEBUGWRAP4(IsClipboardFormatAvailable)
DEBUGWRAP4(OpenClipboard)
DEBUGWRAP4(RegisterClipboardFormatA)
DEBUGWRAP4(RegisterClipboardFormatW)
DEBUGWRAP8(SetClipboardData)
DEBUGWRAP4(SetClipboardViewer)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_winkeyboard
DEBUGWRAP12(SendInput)
DEBUGWRAP8(ActivateKeyboardLayout)
DEBUGWRAP4(GetKeyboardLayout)
DEBUGWRAP8(GetKeyboardLayoutList)
DEBUGWRAP4(GetKeyboardLayoutNameA)
DEBUGWRAP4(GetKeyboardLayoutNameW)
DEBUGWRAP4(GetKeyboardState)
DEBUGWRAP4(GetKeyboardType)
DEBUGWRAP8(LoadKeyboardLayoutA)
DEBUGWRAP8(LoadKeyboardLayoutW)
DEBUGWRAP4(SetKeyboardState)
DEBUGWRAP4(UnloadKeyboardLayout)
DEBUGWRAP16(keybd_event)
DEBUGWRAP4(VkKeyScanA)
DEBUGWRAP8(VkKeyScanExA)
DEBUGWRAP8(VkKeyScanExW)
DEBUGWRAP4(VkKeyScanW)
DEBUGWRAP0(GetKBCodePage)
DEBUGWRAP12(GetKeyNameTextA)
DEBUGWRAP12(GetKeyNameTextW)
DEBUGWRAP4(GetKeyState)
DEBUGWRAP4(GetAsyncKeyState)
DEBUGWRAP8(MapVirtualKeyA)
DEBUGWRAP12(MapVirtualKeyExA)
DEBUGWRAP12(MapVirtualKeyExW)
DEBUGWRAP8(MapVirtualKeyW)
DEBUGWRAP4(OemKeyScan)
DEBUGWRAP16(RegisterHotKey)
DEBUGWRAP8(UnregisterHotKey)
DEBUGWRAP20(ToAscii)
DEBUGWRAP24(ToAsciiEx)
DEBUGWRAP24(ToUnicode)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_winicon
DEBUGWRAP28(CreateCursor)
DEBUGWRAP4(ClipCursor)
DEBUGWRAP4(GetClipCursor)
DEBUGWRAP0(GetCursor)
DEBUGWRAP4(SetCursor)
DEBUGWRAP8(SetSystemCursor)
DEBUGWRAP4(ShowCursor)
DEBUGWRAP4(DestroyCursor)
DEBUGWRAP4(CopyIcon)
DEBUGWRAP28(CreateIcon)
DEBUGWRAP16(CreateIconFromResource)
DEBUGWRAP28(CreateIconFromResourceEx)
DEBUGWRAP4(CreateIconIndirect)
DEBUGWRAP4(DestroyIcon)
DEBUGWRAP16(DrawIcon)
DEBUGWRAP36(DrawIconEx)
DEBUGWRAP8(GetIconInfo)
DEBUGWRAP8(LoadIconA)
DEBUGWRAP8(LoadIconW)
DEBUGWRAP8(LookupIconIdFromDirectory)
DEBUGWRAP20(LookupIconIdFromDirectoryEx)
DEBUGWRAP4(OpenIcon)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_property
DEBUGWRAP8(EnumPropsA)
DEBUGWRAP12(EnumPropsExA)
DEBUGWRAP12(EnumPropsExW)
DEBUGWRAP8(EnumPropsW)
DEBUGWRAP8(GetPropA)
DEBUGWRAP8(GetPropW)
DEBUGWRAP8(RemovePropA)
DEBUGWRAP8(RemovePropW)
DEBUGWRAP12(SetPropA)
DEBUGWRAP12(SetPropW)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_rect
DEBUGWRAP8(CopyRect)
DEBUGWRAP8(EqualRect)
DEBUGWRAP12(SubtractRect)
DEBUGWRAP12(InflateRect)
DEBUGWRAP12(IntersectRect)
DEBUGWRAP4(IsRectEmpty)
DEBUGWRAP12(OffsetRect)
DEBUGWRAP12(PtInRect)
DEBUGWRAP20(SetRect)
DEBUGWRAP4(SetRectEmpty)
DEBUGWRAP12(UnionRect)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_display
DEBUGWRAP8(GetMonitorInfoW)
DEBUGWRAP8(GetMonitorInfoA)
DEBUGWRAP8(MonitorFromWindow)
DEBUGWRAP8(MonitorFromRect)
DEBUGWRAP12(MonitorFromPoint)
DEBUGWRAP8(ChangeDisplaySettingsA)
DEBUGWRAP8(ChangeDisplaySettingsW)
DEBUGWRAP12(EnumDisplaySettingsA)
DEBUGWRAP12(EnumDisplaySettingsW)
DEBUGWRAP20(ChangeDisplaySettingsExA)
DEBUGWRAP20(ChangeDisplaySettingsExW)
DEBUGWRAP16(EnumDisplayMonitors)
DEBUGWRAP16(EnumDisplayDevicesA)
DEBUGWRAP16(EnumDisplayDevicesW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_scroll
DEBUGWRAP12(EnableScrollBar)
DEBUGWRAP12(GetScrollBarInfo)
DEBUGWRAP12(GetScrollInfo)
DEBUGWRAP8(GetScrollPos)
DEBUGWRAP16(GetScrollRange)
DEBUGWRAP16(SetScrollInfo)
DEBUGWRAP16(SetScrollPos)
DEBUGWRAP20(SetScrollRange)
DEBUGWRAP12(ShowScrollBar)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_hook
DEBUGWRAP16(CallNextHookEx)
DEBUGWRAP8(SetWindowsHookA)
DEBUGWRAP16(SetWindowsHookExA)
DEBUGWRAP16(SetWindowsHookExW)
DEBUGWRAP8(SetWindowsHookW)
DEBUGWRAP8(UnhookWindowsHook)
DEBUGWRAP4(UnhookWindowsHookEx)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_windowmsg
DEBUGWRAP20(MsgWaitForMultipleObjects)
DEBUGWRAP20(BroadcastSystemMessage)
DEBUGWRAP8(CallMsgFilterA)
DEBUGWRAP8(CallMsgFilterW)
DEBUGWRAP4(DispatchMessageA)
DEBUGWRAP4(DispatchMessageW)
DEBUGWRAP16(GetMessageA)
DEBUGWRAP0(GetMessageExtraInfo)
DEBUGWRAP0(GetMessagePos)
DEBUGWRAP0(GetMessageTime)
DEBUGWRAP16(GetMessageW)
DEBUGWRAP0(InSendMessage)
DEBUGWRAP20(PeekMessageA)
DEBUGWRAP20(PeekMessageW)
DEBUGWRAP16(PostMessageA)
DEBUGWRAP16(PostMessageW)
DEBUGWRAP4(PostQuitMessage)
DEBUGWRAP16(PostThreadMessageA)
DEBUGWRAP16(PostThreadMessageW)
DEBUGWRAP4(RegisterWindowMessageA)
DEBUGWRAP4(RegisterWindowMessageW)
DEBUGWRAP4(ReplyMessage)
DEBUGWRAP16(SendMessageA)
DEBUGWRAP24(SendMessageCallbackA)
DEBUGWRAP24(SendMessageCallbackW)
DEBUGWRAP28(SendMessageTimeoutA)
DEBUGWRAP28(SendMessageTimeoutW)
DEBUGWRAP16(SendMessageW)
DEBUGWRAP16(SendNotifyMessageA)
DEBUGWRAP16(SendNotifyMessageW)
DEBUGWRAP4(SetMessageExtraInfo)
DEBUGWRAP4(SetMessageQueue)
DEBUGWRAP4(TranslateMessage)
DEBUGWRAP0(WaitMessage)
DEBUGWRAP8(SendIMEMessageExA)
DEBUGWRAP8(SendIMEMessageExW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_windlgmsg
DEBUGWRAP20(SendDlgItemMessageA)
DEBUGWRAP20(SendDlgItemMessageW)
DEBUGWRAP8(IsDialogMessageA)
DEBUGWRAP8(IsDialogMessageW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_msgbox
DEBUGWRAP16(MessageBoxA)
DEBUGWRAP20(MessageBoxExA)
DEBUGWRAP20(MessageBoxExW)
DEBUGWRAP4(MessageBoxIndirectA)
DEBUGWRAP4(MessageBoxIndirectW)
DEBUGWRAP16(MessageBoxW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_defwndproc

DEBUGWRAP_LVL2_16(DefDlgProcA)
DEBUGWRAP_LVL2_16(DefDlgProcW)
DEBUGWRAP_LVL2_20(DefFrameProcA)
DEBUGWRAP_LVL2_20(DefFrameProcW)
DEBUGWRAP_LVL2_16(DefMDIChildProcA)
DEBUGWRAP_LVL2_16(DefMDIChildProcW)
DEBUGWRAP_LVL2_16(DefWindowProcA)
DEBUGWRAP_LVL2_16(DefWindowProcW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_windlg
DEBUGWRAP16(SetDlgItemInt)
DEBUGWRAP12(SetDlgItemTextA)
DEBUGWRAP12(SetDlgItemTextW)
DEBUGWRAP12(CheckDlgButton)
DEBUGWRAP20(DlgDirListA)
DEBUGWRAP20(DlgDirListComboBoxA)
DEBUGWRAP20(DlgDirListComboBoxW)
DEBUGWRAP20(DlgDirListW)
DEBUGWRAP16(DlgDirSelectComboBoxExA)
DEBUGWRAP16(DlgDirSelectComboBoxExW)
DEBUGWRAP16(DlgDirSelectExA)
DEBUGWRAP16(DlgDirSelectExW)
DEBUGWRAP4(GetDlgCtrlID)
DEBUGWRAP8(GetDlgItem)
DEBUGWRAP16(GetDlgItemInt)
DEBUGWRAP16(GetDlgItemTextA)
DEBUGWRAP16(GetDlgItemTextW)
DEBUGWRAP12(GetNextDlgGroupItem)
DEBUGWRAP12(GetNextDlgTabItem)
DEBUGWRAP8(IsDlgButtonChecked)
DEBUGWRAP20(CreateDialogIndirectParamA)
DEBUGWRAP20(CreateDialogIndirectParamW)
DEBUGWRAP20(CreateDialogParamA)
DEBUGWRAP20(CreateDialogParamW)
DEBUGWRAP20(DialogBoxIndirectParamA)
DEBUGWRAP20(DialogBoxIndirectParamW)
DEBUGWRAP20(DialogBoxParamA)
DEBUGWRAP20(DialogBoxParamW)
DEBUGWRAP8(EndDialog)
DEBUGWRAP0(GetDialogBaseUnits)
DEBUGWRAP8(MapDialogRect)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_loadres
DEBUGWRAP8(LoadBitmapA)
DEBUGWRAP8(LoadBitmapW)
DEBUGWRAP8(LoadCursorA)
DEBUGWRAP4(LoadCursorFromFileA)
DEBUGWRAP4(LoadCursorFromFileW)
DEBUGWRAP8(LoadCursorW)
DEBUGWRAP24(LoadImageA)
DEBUGWRAP24(LoadImageW)
DEBUGWRAP16(LoadStringA)
DEBUGWRAP16(LoadStringW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_winmouse
DEBUGWRAP4(GetCursorPos)
DEBUGWRAP8(SetCursorPos)
DEBUGWRAP4(SwapMouseButton)
DEBUGWRAP20(mouse_event)
DEBUGWRAP4(TrackMouseEvent)
DEBUGWRAP0(GetCapture)
DEBUGWRAP0(ReleaseCapture)
DEBUGWRAP4(SetCapture)
DEBUGWRAP0(GetDoubleClickTime)
DEBUGWRAP4(SetDoubleClickTime)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_caret
DEBUGWRAP16(CreateCaret)
DEBUGWRAP0(DestroyCaret)
DEBUGWRAP0(GetCaretBlinkTime)
DEBUGWRAP4(GetCaretPos)
DEBUGWRAP4(HideCaret)
DEBUGWRAP4(SetCaretBlinkTime)
DEBUGWRAP8(SetCaretPos)
DEBUGWRAP4(ShowCaret)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_uitools
DEBUGWRAP16(DrawAnimatedRects)
DEBUGWRAP16(DrawCaption)
DEBUGWRAP28(DrawCaptionTempA)
DEBUGWRAP28(DrawCaptionTempW)
DEBUGWRAP16(DrawEdge)
DEBUGWRAP8(DrawFocusRect)
DEBUGWRAP16(DrawFrameControl)
DEBUGWRAP40(DrawStateA)
DEBUGWRAP40(DrawStateW)
DEBUGWRAP20(DrawTextA)
DEBUGWRAP24(DrawTextExA)
DEBUGWRAP24(DrawTextExW)
DEBUGWRAP20(DrawTextW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_text
DEBUGWRAP20(GetTabbedTextExtentA)
DEBUGWRAP20(GetTabbedTextExtentW)
DEBUGWRAP32(TabbedTextOutA)
DEBUGWRAP32(TabbedTextOutW)
DEBUGWRAP36(GrayStringA)
DEBUGWRAP36(GrayStringW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_timer
DEBUGWRAP8(KillTimer)
DEBUGWRAP16(SetTimer)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_window
DEBUGWRAP8(FindWindowA)
DEBUGWRAP16(FindWindowExA)
DEBUGWRAP16(FindWindowExW)
DEBUGWRAP8(FindWindowW)
DEBUGWRAP8(FlashWindow)
DEBUGWRAP12(AdjustWindowRect)
DEBUGWRAP16(AdjustWindowRectEx)
DEBUGWRAP0(AnyPopup)
DEBUGWRAP4(BeginDeferWindowPos)
DEBUGWRAP4(BringWindowToTop)
DEBUGWRAP12(ChildWindowFromPoint)
DEBUGWRAP16(ChildWindowFromPointEx)
DEBUGWRAP4(CloseWindow)
DEBUGWRAP48(CreateWindowExA)
DEBUGWRAP48(CreateWindowExW)
DEBUGWRAP32(DeferWindowPos)
DEBUGWRAP4(DestroyWindow)
DEBUGWRAP8(EnableWindow)
DEBUGWRAP4(EndDeferWindowPos)
DEBUGWRAP12(EnumThreadWindows)
DEBUGWRAP8(EnumWindows)
DEBUGWRAP12(EnumChildWindows)
DEBUGWRAP0(GetActiveWindow)
DEBUGWRAP8(GetClientRect)
DEBUGWRAP0(GetDesktopWindow)
DEBUGWRAP0(GetFocus)
DEBUGWRAP0(GetForegroundWindow)
DEBUGWRAP4(GetLastActivePopup)
DEBUGWRAP8(GetNextQueueWindow)
DEBUGWRAP4(GetParent)
DEBUGWRAP4(GetTopWindow)
DEBUGWRAP8(GetWindow)
DEBUGWRAP4(GetWindowContextHelpId)
DEBUGWRAP8(GetWindowRect)
DEBUGWRAP12(GetWindowTextA)
DEBUGWRAP4(GetWindowTextLengthA)
DEBUGWRAP4(GetWindowTextLengthW)
DEBUGWRAP12(GetWindowTextW)
DEBUGWRAP8(GetWindowThreadProcessId)
DEBUGWRAP8(GetWindowPlacement)
DEBUGWRAP8(SetWindowPlacement)
DEBUGWRAP28(SetWindowPos)
DEBUGWRAP8(SetWindowTextA)
DEBUGWRAP8(SetWindowTextW)
DEBUGWRAP8(ShowOwnedPopups)
DEBUGWRAP8(ShowWindow)
DEBUGWRAP8(ShowWindowAsync)
DEBUGWRAP24(MoveWindow)
DEBUGWRAP4(IsIconic)
DEBUGWRAP8(IsChild)
DEBUGWRAP_LVL2_4(IsWindow)
DEBUGWRAP4(IsWindowEnabled)
DEBUGWRAP4(IsWindowUnicode)
DEBUGWRAP4(IsWindowVisible)
DEBUGWRAP4(IsZoomed)
DEBUGWRAP4(LockWindowUpdate)
DEBUGWRAP16(MapWindowPoints)
DEBUGWRAP4(SetActiveWindow)
DEBUGWRAP4(SetFocus)
DEBUGWRAP4(SetForegroundWindow)
DEBUGWRAP16(SetInternalWindowPos)
DEBUGWRAP8(SetParent)
DEBUGWRAP8(SetWindowContextHelpId)
DEBUGWRAP8(WindowFromPoint)
DEBUGWRAP8(ScreenToClient)
DEBUGWRAP4(WindowFromDC)
DEBUGWRAP8(ClientToScreen)
DEBUGWRAP12(InternalGetWindowText)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_windowword
DEBUGWRAP8(GetWindowLongA)
DEBUGWRAP8(GetWindowLongW)
DEBUGWRAP8(GetWindowWord)
DEBUGWRAP12(SetWindowLongA)
DEBUGWRAP12(SetWindowLongW)
DEBUGWRAP12(SetWindowWord)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_paint
DEBUGWRAP4(GetDC)
DEBUGWRAP12(GetDCEx)
DEBUGWRAP12(FillRect)
DEBUGWRAP12(FrameRect)
DEBUGWRAP4(GetWindowDC)
DEBUGWRAP8(ReleaseDC)
DEBUGWRAP8(CalcChildScroll)
DEBUGWRAP28(ScrollDC)
DEBUGWRAP20(ScrollWindow)
DEBUGWRAP32(ScrollWindowEx)
DEBUGWRAP12(InvalidateRect)
DEBUGWRAP12(InvalidateRgn)
DEBUGWRAP8(InvertRect)
DEBUGWRAP8(ValidateRect)
DEBUGWRAP8(ValidateRgn)
DEBUGWRAP16(RedrawWindow)
DEBUGWRAP8(EndPaint)
DEBUGWRAP12(GetUpdateRect)
DEBUGWRAP12(GetUpdateRgn)
DEBUGWRAP8(GetWindowRgn)
DEBUGWRAP12(SetWindowRgn)
DEBUGWRAP4(UpdateWindow)
DEBUGWRAP8(BeginPaint)
DEBUGWRAP8(ExcludeUpdateRgn)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_winaccel
DEBUGWRAP12(CopyAcceleratorTableA)
DEBUGWRAP12(CopyAcceleratorTableW)
DEBUGWRAP8(CreateAcceleratorTableA)
DEBUGWRAP8(CreateAcceleratorTableW)
DEBUGWRAP4(DestroyAcceleratorTable)
DEBUGWRAP8(TranslateMDISysAccel)
DEBUGWRAP8(LoadAcceleratorsA)
DEBUGWRAP8(LoadAcceleratorsW)
