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
DWORD WIN32API GetGUIThreadInfo(DWORD arg1, DWORD arg2);
HDESK WIN32API GetThreadDesktop(DWORD dwThreadId);
LRESULT WINAPI SendIMEMessageExA(HWND hwnd, LPARAM lparam);
LRESULT WINAPI SendIMEMessageExW(HWND hwnd, LPARAM lparam);
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
LRESULT WINAPI EditWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );


DEBUGWRAP8(ActivateKeyboardLayout)
DEBUGWRAP12(AdjustWindowRect)
DEBUGWRAP16(AdjustWindowRectEx)
DEBUGWRAP0(AnyPopup)
DEBUGWRAP16(AppendMenuA)
DEBUGWRAP16(AppendMenuW)
DEBUGWRAP4(ArrangeIconicWindows)
DEBUGWRAP12(AttachThreadInput)
DEBUGWRAP4(BeginDeferWindowPos)
DEBUGWRAP8(BeginPaint)
DEBUGWRAP4(BringWindowToTop)
DEBUGWRAP20(BroadcastSystemMessage)
DEBUGWRAP8(CalcChildScroll)
DEBUGWRAP8(CallMsgFilterA)
DEBUGWRAP8(CallMsgFilterW)
DEBUGWRAP16(CallNextHookEx)
DEBUGWRAP20(CallWindowProcA)
DEBUGWRAP20(CallWindowProcW)
DEBUGWRAP8(CascadeChildWindows)
DEBUGWRAP20(CascadeWindows)
DEBUGWRAP8(ChangeClipboardChain)
DEBUGWRAP8(ChangeDisplaySettingsA)
DEBUGWRAP8(ChangeDisplaySettingsW)
DEBUGWRAP20(ChangeMenuA)
DEBUGWRAP20(ChangeMenuW)
DEBUGWRAP4(CharLowerA)
DEBUGWRAP8(CharLowerBuffA)
DEBUGWRAP8(CharLowerBuffW)
DEBUGWRAP4(CharLowerW)
DEBUGWRAP4(CharNextA)
DEBUGWRAP12(CharNextExA)
DEBUGWRAP12(CharNextExW)
DEBUGWRAP4(CharNextW)
DEBUGWRAP8(CharPrevA)
DEBUGWRAP16(CharPrevExA)
DEBUGWRAP16(CharPrevExW)
DEBUGWRAP8(CharPrevW)
DEBUGWRAP8(CharToOemA)
DEBUGWRAP12(CharToOemBuffA)
DEBUGWRAP12(CharToOemBuffW)
DEBUGWRAP8(CharToOemW)
DEBUGWRAP4(CharUpperA)
DEBUGWRAP8(CharUpperBuffA)
DEBUGWRAP8(CharUpperBuffW)
DEBUGWRAP4(CharUpperW)
DEBUGWRAP12(CheckDlgButton)
DEBUGWRAP12(CheckMenuItem)
DEBUGWRAP20(CheckMenuRadioItem)
DEBUGWRAP16(CheckRadioButton)
DEBUGWRAP12(ChildWindowFromPoint)
DEBUGWRAP16(ChildWindowFromPointEx)
DEBUGWRAP8(ClientToScreen)
DEBUGWRAP4(ClipCursor)
DEBUGWRAP0(CloseClipboard)
DEBUGWRAP4(CloseDesktop)
DEBUGWRAP4(CloseWindow)
DEBUGWRAP4(CloseWindowStation)
DEBUGWRAP12(CopyAcceleratorTableA)
DEBUGWRAP12(CopyAcceleratorTableW)
DEBUGWRAP4(CopyIcon)
DEBUGWRAP20(CopyImage)
DEBUGWRAP8(CopyRect)
DEBUGWRAP0(CountClipboardFormats)
DEBUGWRAP8(CreateAcceleratorTableA)
DEBUGWRAP8(CreateAcceleratorTableW)
DEBUGWRAP16(CreateCaret)
DEBUGWRAP28(CreateCursor)
DEBUGWRAP24(CreateDesktopA)
DEBUGWRAP24(CreateDesktopW)
DEBUGWRAP20(CreateDialogIndirectParamA)
DEBUGWRAP20(CreateDialogIndirectParamW)
DEBUGWRAP20(CreateDialogParamA)
DEBUGWRAP20(CreateDialogParamW)
DEBUGWRAP28(CreateIcon)
DEBUGWRAP16(CreateIconFromResource)
DEBUGWRAP28(CreateIconFromResourceEx)
DEBUGWRAP4(CreateIconIndirect)
DEBUGWRAP40(CreateMDIWindowA)
DEBUGWRAP40(CreateMDIWindowW)
DEBUGWRAP0(CreateMenu)
DEBUGWRAP0(CreatePopupMenu)
DEBUGWRAP48(CreateWindowExA)
DEBUGWRAP48(CreateWindowExW)
DEBUGWRAP16(CreateWindowStationA)
DEBUGWRAP16(CreateWindowStationW)
DEBUGWRAP12(DdeAbandonTransaction)
DEBUGWRAP8(DdeAccessData)
DEBUGWRAP16(DdeAddData)
DEBUGWRAP32(DdeClientTransaction)
DEBUGWRAP8(DdeCmpStringHandles)
DEBUGWRAP16(DdeConnect)
DEBUGWRAP20(DdeConnectList)
DEBUGWRAP28(DdeCreateDataHandle)
DEBUGWRAP12(DdeCreateStringHandleA)
DEBUGWRAP12(DdeCreateStringHandleW)
DEBUGWRAP4(DdeDisconnect)
DEBUGWRAP4(DdeDisconnectList)
DEBUGWRAP12(DdeEnableCallback)
DEBUGWRAP4(DdeFreeDataHandle)
DEBUGWRAP8(DdeFreeStringHandle)
DEBUGWRAP16(DdeGetData)
DEBUGWRAP4(DdeGetLastError)
DEBUGWRAP4(DdeImpersonateClient)
DEBUGWRAP16(DdeInitializeA)
DEBUGWRAP16(DdeInitializeW)
DEBUGWRAP8(DdeKeepStringHandle)
DEBUGWRAP16(DdeNameService)
DEBUGWRAP12(DdePostAdvise)
DEBUGWRAP12(DdeQueryConvInfo)
DEBUGWRAP8(DdeQueryNextServer)
DEBUGWRAP20(DdeQueryStringA)
DEBUGWRAP20(DdeQueryStringW)
DEBUGWRAP4(DdeReconnect)
DEBUGWRAP12(DdeSetQualityOfService)
DEBUGWRAP12(DdeSetUserHandle)
DEBUGWRAP4(DdeUnaccessData)
DEBUGWRAP4(DdeUninitialize)
DEBUGWRAP16(DefDlgProcA)
DEBUGWRAP16(DefDlgProcW)
DEBUGWRAP20(DefFrameProcA)
DEBUGWRAP20(DefFrameProcW)
DEBUGWRAP16(DefMDIChildProcA)
DEBUGWRAP16(DefMDIChildProcW)
DEBUGWRAP16(DefWindowProcA)
DEBUGWRAP16(DefWindowProcW)
DEBUGWRAP32(DeferWindowPos)
DEBUGWRAP12(DeleteMenu)
DEBUGWRAP4(DestroyAcceleratorTable)
DEBUGWRAP0(DestroyCaret)
DEBUGWRAP4(DestroyCursor)
DEBUGWRAP4(DestroyIcon)
DEBUGWRAP4(DestroyMenu)
DEBUGWRAP4(DestroyWindow)
DEBUGWRAP20(DialogBoxIndirectParamA)
DEBUGWRAP20(DialogBoxIndirectParamW)
DEBUGWRAP20(DialogBoxParamA)
DEBUGWRAP20(DialogBoxParamW)
DEBUGWRAP4(DispatchMessageA)
DEBUGWRAP4(DispatchMessageW)
DEBUGWRAP20(DlgDirListA)
DEBUGWRAP20(DlgDirListComboBoxA)
DEBUGWRAP20(DlgDirListComboBoxW)
DEBUGWRAP20(DlgDirListW)
DEBUGWRAP16(DlgDirSelectComboBoxExA)
DEBUGWRAP16(DlgDirSelectComboBoxExW)
DEBUGWRAP16(DlgDirSelectExA)
DEBUGWRAP16(DlgDirSelectExW)
DEBUGWRAP12(DragDetect)
DEBUGWRAP20(DragObject)
DEBUGWRAP16(DrawAnimatedRects)
DEBUGWRAP16(DrawCaption)
DEBUGWRAP28(DrawCaptionTempA)
DEBUGWRAP28(DrawCaptionTempW)
DEBUGWRAP16(DrawEdge)
DEBUGWRAP8(DrawFocusRect)
DEBUGWRAP16(DrawFrameControl)
DEBUGWRAP16(DrawIcon)
DEBUGWRAP36(DrawIconEx)
DEBUGWRAP4(DrawMenuBar)
DEBUGWRAP40(DrawStateA)
DEBUGWRAP40(DrawStateW)
DEBUGWRAP20(DrawTextA)
DEBUGWRAP24(DrawTextExA)
DEBUGWRAP24(DrawTextExW)
DEBUGWRAP20(DrawTextW)
DEBUGWRAP16(EditWndProc)
DEBUGWRAP0(EmptyClipboard)
DEBUGWRAP12(EnableMenuItem)
DEBUGWRAP12(EnableScrollBar)
DEBUGWRAP8(EnableWindow)
DEBUGWRAP4(EndDeferWindowPos)
DEBUGWRAP8(EndDialog)
DEBUGWRAP8(EndPaint)
DEBUGWRAP12(EndTask)
DEBUGWRAP12(EnumChildWindows)
DEBUGWRAP4(EnumClipboardFormats)
DEBUGWRAP12(EnumDesktopWindows)
DEBUGWRAP12(EnumDesktopsA)
DEBUGWRAP12(EnumDesktopsW)
DEBUGWRAP12(EnumDisplaySettingsA)
DEBUGWRAP12(EnumDisplaySettingsW)
DEBUGWRAP8(EnumPropsA)
DEBUGWRAP12(EnumPropsExA)
DEBUGWRAP12(EnumPropsExW)
DEBUGWRAP8(EnumPropsW)
DEBUGWRAP12(EnumThreadWindows)
DEBUGWRAP8(EnumWindowStationsA)
DEBUGWRAP8(EnumWindowStationsW)
DEBUGWRAP8(EnumWindows)
DEBUGWRAP8(EqualRect)
DEBUGWRAP8(ExcludeUpdateRgn)
DEBUGWRAP8(ExitWindowsEx)
DEBUGWRAP12(FillRect)
DEBUGWRAP8(FindWindowA)
DEBUGWRAP16(FindWindowExA)
DEBUGWRAP16(FindWindowExW)
DEBUGWRAP8(FindWindowW)
DEBUGWRAP8(FlashWindow)
DEBUGWRAP12(FrameRect)
DEBUGWRAP8(FreeDDElParam)
DEBUGWRAP0(GetActiveWindow)
DEBUGWRAP4(GetAsyncKeyState)
DEBUGWRAP0(GetCapture)
DEBUGWRAP0(GetCaretBlinkTime)
DEBUGWRAP4(GetCaretPos)
DEBUGWRAP12(GetClassInfoA)
DEBUGWRAP12(GetClassInfoExA)
DEBUGWRAP12(GetClassInfoExW)
DEBUGWRAP12(GetClassInfoW)
DEBUGWRAP8(GetClassLongA)
DEBUGWRAP8(GetClassLongW)
DEBUGWRAP12(GetClassNameA)
DEBUGWRAP12(GetClassNameW)
DEBUGWRAP8(GetClassWord)
DEBUGWRAP8(GetClientRect)
DEBUGWRAP4(GetClipCursor)
DEBUGWRAP4(GetClipboardData)
DEBUGWRAP12(GetClipboardFormatNameA)
DEBUGWRAP12(GetClipboardFormatNameW)
DEBUGWRAP0(GetClipboardOwner)
DEBUGWRAP0(GetClipboardViewer)
DEBUGWRAP0(GetCursor)
DEBUGWRAP4(GetCursorPos)
DEBUGWRAP4(GetDC)
DEBUGWRAP12(GetDCEx)
DEBUGWRAP0(GetDesktopWindow)
DEBUGWRAP0(GetDialogBaseUnits)
DEBUGWRAP4(GetDlgCtrlID)
DEBUGWRAP8(GetDlgItem)
DEBUGWRAP16(GetDlgItemInt)
DEBUGWRAP16(GetDlgItemTextA)
DEBUGWRAP16(GetDlgItemTextW)
DEBUGWRAP0(GetDoubleClickTime)
DEBUGWRAP0(GetFocus)
DEBUGWRAP0(GetForegroundWindow)
DEBUGWRAP8(GetIconInfo)
DEBUGWRAP0(GetInputState)
DEBUGWRAP12(GetInternalWindowPos)
DEBUGWRAP0(GetKBCodePage)
DEBUGWRAP12(GetKeyNameTextA)
DEBUGWRAP12(GetKeyNameTextW)
DEBUGWRAP4(GetKeyState)
DEBUGWRAP4(GetKeyboardLayout)
DEBUGWRAP8(GetKeyboardLayoutList)
DEBUGWRAP4(GetKeyboardLayoutNameA)
DEBUGWRAP4(GetKeyboardLayoutNameW)
DEBUGWRAP4(GetKeyboardState)
DEBUGWRAP4(GetKeyboardType)
DEBUGWRAP4(GetLastActivePopup)
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
DEBUGWRAP16(GetMessageA)
DEBUGWRAP0(GetMessageExtraInfo)
DEBUGWRAP0(GetMessagePos)
DEBUGWRAP0(GetMessageTime)
DEBUGWRAP16(GetMessageW)
DEBUGWRAP12(GetNextDlgGroupItem)
DEBUGWRAP12(GetNextDlgTabItem)
DEBUGWRAP8(GetNextQueueWindow)
DEBUGWRAP0(GetOpenClipboardWindow)
DEBUGWRAP4(GetParent)
DEBUGWRAP8(GetPriorityClipboardFormat)
DEBUGWRAP0(GetProcessWindowStation)
DEBUGWRAP0(GetProgmanWindow)
DEBUGWRAP8(GetPropA)
DEBUGWRAP8(GetPropW)
DEBUGWRAP4(GetQueueStatus)
DEBUGWRAP12(GetScrollBarInfo)
DEBUGWRAP12(GetScrollInfo)
DEBUGWRAP8(GetScrollPos)
DEBUGWRAP16(GetScrollRange)
DEBUGWRAP0(GetShellWindow)
DEBUGWRAP8(GetSubMenu)
DEBUGWRAP4(GetSysColor)
DEBUGWRAP4(GetSysColorBrush)
DEBUGWRAP8(GetSystemMenu)
DEBUGWRAP4(GetSystemMetrics)
DEBUGWRAP20(GetTabbedTextExtentA)
DEBUGWRAP20(GetTabbedTextExtentW)
DEBUGWRAP0(GetTaskmanWindow)
DEBUGWRAP4(GetThreadDesktop)
DEBUGWRAP4(GetTopWindow)
DEBUGWRAP12(GetUpdateRect)
DEBUGWRAP12(GetUpdateRgn)
DEBUGWRAP20(GetUserObjectInformationA)
DEBUGWRAP20(GetUserObjectInformationW)
DEBUGWRAP20(GetUserObjectSecurity)
DEBUGWRAP8(GetWindow)
DEBUGWRAP4(GetWindowContextHelpId)
DEBUGWRAP4(GetWindowDC)
DEBUGWRAP8(GetWindowLongA)
DEBUGWRAP8(GetWindowLongW)
DEBUGWRAP8(GetWindowPlacement)
DEBUGWRAP8(GetWindowRect)
DEBUGWRAP8(GetWindowRgn)
DEBUGWRAP12(GetWindowTextA)
DEBUGWRAP4(GetWindowTextLengthA)
DEBUGWRAP4(GetWindowTextLengthW)
DEBUGWRAP12(GetWindowTextW)
DEBUGWRAP8(GetWindowThreadProcessId)
DEBUGWRAP8(GetWindowWord)
DEBUGWRAP36(GrayStringA)
DEBUGWRAP36(GrayStringW)
DEBUGWRAP4(HideCaret)
DEBUGWRAP16(HiliteMenuItem)
DEBUGWRAP8(ImpersonateDdeClientWindow)
DEBUGWRAP0(InSendMessage)
DEBUGWRAP12(InflateRect)
DEBUGWRAP20(InsertMenuA)
DEBUGWRAP16(InsertMenuItemA)
DEBUGWRAP16(InsertMenuItemW)
DEBUGWRAP20(InsertMenuW)
DEBUGWRAP12(InternalGetWindowText)
DEBUGWRAP12(IntersectRect)
DEBUGWRAP12(InvalidateRect)
DEBUGWRAP12(InvalidateRgn)
DEBUGWRAP8(InvertRect)
DEBUGWRAP4(IsCharAlphaA)
DEBUGWRAP4(IsCharAlphaNumericA)
DEBUGWRAP4(IsCharAlphaNumericW)
DEBUGWRAP4(IsCharAlphaW)
DEBUGWRAP4(IsCharLowerA)
DEBUGWRAP4(IsCharLowerW)
DEBUGWRAP4(IsCharUpperA)
DEBUGWRAP4(IsCharUpperW)
DEBUGWRAP8(IsChild)
DEBUGWRAP4(IsClipboardFormatAvailable)
DEBUGWRAP8(IsDialogMessageA)
DEBUGWRAP8(IsDialogMessageW)
DEBUGWRAP8(IsDlgButtonChecked)
DEBUGWRAP4(IsHungThread)
DEBUGWRAP8(IsHungAppWindow)
DEBUGWRAP4(IsIconic)
DEBUGWRAP4(IsMenu)
DEBUGWRAP4(IsRectEmpty)
DEBUGWRAP4(IsWindow)
DEBUGWRAP4(IsWindowEnabled)
DEBUGWRAP4(IsWindowUnicode)
DEBUGWRAP4(IsWindowVisible)
DEBUGWRAP4(IsZoomed)
DEBUGWRAP8(KillTimer)
DEBUGWRAP8(LoadAcceleratorsA)
DEBUGWRAP8(LoadAcceleratorsW)
DEBUGWRAP8(LoadBitmapA)
DEBUGWRAP8(LoadBitmapW)
DEBUGWRAP8(LoadCursorA)
DEBUGWRAP4(LoadCursorFromFileA)
DEBUGWRAP4(LoadCursorFromFileW)
DEBUGWRAP8(LoadCursorW)
DEBUGWRAP8(LoadIconA)
DEBUGWRAP8(LoadIconW)
DEBUGWRAP24(LoadImageA)
DEBUGWRAP24(LoadImageW)
DEBUGWRAP8(LoadKeyboardLayoutA)
DEBUGWRAP8(LoadKeyboardLayoutW)
DEBUGWRAP8(LoadMenuA)
DEBUGWRAP4(LoadMenuIndirectW)
DEBUGWRAP8(LoadMenuW)
DEBUGWRAP16(LoadStringA)
DEBUGWRAP16(LoadStringW)
DEBUGWRAP4(LockWindowUpdate)
DEBUGWRAP8(LookupIconIdFromDirectory)
DEBUGWRAP20(LookupIconIdFromDirectoryEx)
DEBUGWRAP8(MapDialogRect)
DEBUGWRAP8(MapVirtualKeyA)
DEBUGWRAP12(MapVirtualKeyExA)
DEBUGWRAP12(MapVirtualKeyExW)
DEBUGWRAP8(MapVirtualKeyW)
DEBUGWRAP16(MapWindowPoints)
DEBUGWRAP16(MenuItemFromPoint)
DEBUGWRAP4(MessageBeep)
DEBUGWRAP16(MessageBoxA)
DEBUGWRAP20(MessageBoxExA)
DEBUGWRAP20(MessageBoxExW)
DEBUGWRAP4(MessageBoxIndirectA)
DEBUGWRAP4(MessageBoxIndirectW)
DEBUGWRAP16(MessageBoxW)
DEBUGWRAP20(ModifyMenuA)
DEBUGWRAP20(ModifyMenuW)
DEBUGWRAP24(MoveWindow)
DEBUGWRAP20(MsgWaitForMultipleObjects)
DEBUGWRAP4(OemKeyScan)
DEBUGWRAP8(OemToCharA)
DEBUGWRAP12(OemToCharBuffA)
DEBUGWRAP12(OemToCharBuffW)
DEBUGWRAP8(OemToCharW)
DEBUGWRAP12(OffsetRect)
DEBUGWRAP4(OpenClipboard)
DEBUGWRAP16(OpenDesktopA)
DEBUGWRAP16(OpenDesktopW)
DEBUGWRAP4(OpenIcon)
DEBUGWRAP12(OpenInputDesktop)
DEBUGWRAP12(OpenWindowStationA)
DEBUGWRAP12(OpenWindowStationW)
DEBUGWRAP12(PackDDElParam)
DEBUGWRAP4(PaintDesktop)
DEBUGWRAP20(PeekMessageA)
DEBUGWRAP20(PeekMessageW)
DEBUGWRAP4(PlaySoundEvent)
DEBUGWRAP16(PostMessageA)
DEBUGWRAP16(PostMessageW)
DEBUGWRAP4(PostQuitMessage)
DEBUGWRAP16(PostThreadMessageA)
DEBUGWRAP16(PostThreadMessageW)
DEBUGWRAP12(PtInRect)
DEBUGWRAP16(RedrawWindow)
DEBUGWRAP4(RegisterClassA)
DEBUGWRAP4(RegisterClassExA)
DEBUGWRAP4(RegisterClassExW)
DEBUGWRAP4(RegisterClassW)
DEBUGWRAP4(RegisterClipboardFormatA)
DEBUGWRAP4(RegisterClipboardFormatW)
DEBUGWRAP16(RegisterHotKey)
DEBUGWRAP8(RegisterNetworkCapabilities)
DEBUGWRAP8(RegisterSystemThread)
DEBUGWRAP4(RegisterTasklist)
DEBUGWRAP4(RegisterWindowMessageA)
DEBUGWRAP4(RegisterWindowMessageW)
DEBUGWRAP0(ReleaseCapture)
DEBUGWRAP8(ReleaseDC)
DEBUGWRAP12(RemoveMenu)
DEBUGWRAP8(RemovePropA)
DEBUGWRAP8(RemovePropW)
DEBUGWRAP4(ReplyMessage)
DEBUGWRAP20(ReuseDDElParam)
DEBUGWRAP8(ScreenToClient)
DEBUGWRAP28(ScrollDC)
DEBUGWRAP20(ScrollWindow)
DEBUGWRAP32(ScrollWindowEx)
DEBUGWRAP20(SendDlgItemMessageA)
DEBUGWRAP20(SendDlgItemMessageW)
DEBUGWRAP16(SendMessageA)
DEBUGWRAP24(SendMessageCallbackA)
DEBUGWRAP24(SendMessageCallbackW)
DEBUGWRAP28(SendMessageTimeoutA)
DEBUGWRAP28(SendMessageTimeoutW)
DEBUGWRAP16(SendMessageW)
DEBUGWRAP16(SendNotifyMessageA)
DEBUGWRAP16(SendNotifyMessageW)
DEBUGWRAP4(SetActiveWindow)
DEBUGWRAP4(SetCapture)
DEBUGWRAP4(SetCaretBlinkTime)
DEBUGWRAP8(SetCaretPos)
DEBUGWRAP12(SetClassLongA)
DEBUGWRAP12(SetClassLongW)
DEBUGWRAP12(SetClassWord)
DEBUGWRAP8(SetClipboardData)
DEBUGWRAP4(SetClipboardViewer)
DEBUGWRAP4(SetCursor)
DEBUGWRAP8(SetCursorPos)
DEBUGWRAP4(SetDebugErrorLevel)
DEBUGWRAP4(SetDeskWallPaper)
DEBUGWRAP16(SetDlgItemInt)
DEBUGWRAP12(SetDlgItemTextA)
DEBUGWRAP12(SetDlgItemTextW)
DEBUGWRAP4(SetDoubleClickTime)
DEBUGWRAP4(SetFocus)
DEBUGWRAP4(SetForegroundWindow)
DEBUGWRAP16(SetInternalWindowPos)
DEBUGWRAP4(SetKeyboardState)
DEBUGWRAP8(SetLastErrorEx)
DEBUGWRAP8(SetLogonNotifyWindow)
DEBUGWRAP8(SetMenu)
DEBUGWRAP8(SetMenuContextHelpId)
DEBUGWRAP12(SetMenuDefaultItem)
DEBUGWRAP20(SetMenuItemBitmaps)
DEBUGWRAP16(SetMenuItemInfoA)
DEBUGWRAP16(SetMenuItemInfoW)
DEBUGWRAP4(SetMessageExtraInfo)
DEBUGWRAP4(SetMessageQueue)
DEBUGWRAP8(SetParent)
DEBUGWRAP4(SetProcessWindowStation)
DEBUGWRAP4(SetProgmanWindow)
DEBUGWRAP12(SetPropA)
DEBUGWRAP12(SetPropW)
DEBUGWRAP20(SetRect)
DEBUGWRAP4(SetRectEmpty)
DEBUGWRAP16(SetScrollInfo)
DEBUGWRAP16(SetScrollPos)
DEBUGWRAP20(SetScrollRange)
DEBUGWRAP4(SetShellWindow)
DEBUGWRAP8(SetShellWindowEx)
DEBUGWRAP12(SetSysColors)
DEBUGWRAP0(SetSysColorsTemp)
DEBUGWRAP8(SetSystemCursor)
DEBUGWRAP4(SetTaskmanWindow)
DEBUGWRAP4(SetThreadDesktop)
DEBUGWRAP16(SetTimer)
DEBUGWRAP16(SetUserObjectInformationA)
DEBUGWRAP16(SetUserObjectInformationW)
DEBUGWRAP12(SetUserObjectSecurity)
DEBUGWRAP8(SetWindowContextHelpId)
DEBUGWRAP12(SetWindowLongA)
DEBUGWRAP12(SetWindowLongW)
DEBUGWRAP8(SetWindowPlacement)
DEBUGWRAP28(SetWindowPos)
DEBUGWRAP12(SetWindowRgn)
DEBUGWRAP8(SetWindowTextA)
DEBUGWRAP8(SetWindowTextW)
DEBUGWRAP12(SetWindowWord)
DEBUGWRAP8(SetWindowsHookA)
DEBUGWRAP16(SetWindowsHookExA)
DEBUGWRAP16(SetWindowsHookExW)
DEBUGWRAP8(SetWindowsHookW)
DEBUGWRAP4(ShowCaret)
DEBUGWRAP4(ShowCursor)
DEBUGWRAP8(ShowOwnedPopups)
DEBUGWRAP12(ShowScrollBar)
DEBUGWRAP8(ShowWindow)
DEBUGWRAP8(ShowWindowAsync)
DEBUGWRAP12(SubtractRect)
DEBUGWRAP4(SwapMouseButton)
DEBUGWRAP4(SwitchDesktop)
DEBUGWRAP8(SwitchToThisWindow)
DEBUGWRAP12(SysErrorBox)
DEBUGWRAP16(SystemParametersInfoA)
DEBUGWRAP16(SystemParametersInfoW)
DEBUGWRAP32(TabbedTextOutA)
DEBUGWRAP32(TabbedTextOutW)
DEBUGWRAP8(TileChildWindows)
DEBUGWRAP20(TileWindows)
DEBUGWRAP20(ToAscii)
DEBUGWRAP24(ToAsciiEx)
DEBUGWRAP24(ToUnicode)
DEBUGWRAP28(TrackPopupMenu)
DEBUGWRAP24(TrackPopupMenuEx)
DEBUGWRAP12(TranslateAcceleratorA)
DEBUGWRAP8(TranslateMDISysAccel)
DEBUGWRAP4(TranslateMessage)
DEBUGWRAP8(UnhookWindowsHook)
DEBUGWRAP4(UnhookWindowsHookEx)
DEBUGWRAP12(UnionRect)
DEBUGWRAP4(UnloadKeyboardLayout)
DEBUGWRAP16(UnpackDDElParam)
DEBUGWRAP8(UnregisterClassA)
DEBUGWRAP8(UnregisterClassW)
DEBUGWRAP8(UnregisterHotKey)
DEBUGWRAP4(UpdateWindow)
DEBUGWRAP16(UserSignalProc)
DEBUGWRAP8(ValidateRect)
DEBUGWRAP8(ValidateRgn)
DEBUGWRAP4(VkKeyScanA)
DEBUGWRAP8(VkKeyScanExA)
DEBUGWRAP8(VkKeyScanExW)
DEBUGWRAP4(VkKeyScanW)
DEBUGWRAP8(WaitForInputIdle)
DEBUGWRAP0(WaitMessage)
DEBUGWRAP16(WinHelpA)
DEBUGWRAP16(WinHelpW)
DEBUGWRAP4(WinOldAppHackoMatic)
DEBUGWRAP4(WindowFromDC)
DEBUGWRAP8(WindowFromPoint)
DEBUGWRAP0(YieldTask)
DEBUGWRAP16(keybd_event)
DEBUGWRAP20(mouse_event)
DEBUGWRAP12(wvsprintfA)
DEBUGWRAP12(wvsprintfW)
DEBUGWRAP4(TrackMouseEvent)
DEBUGWRAP12(SendInput)
DEBUGWRAP8(GetGUIThreadInfo)
DEBUGWRAP20(ChangeDisplaySettingsExA)
DEBUGWRAP20(ChangeDisplaySettingsExW)
DEBUGWRAP16(NotifyWinEvent)
DEBUGWRAP4(UnhookWinEvent)
DEBUGWRAP28(SetWinEventHook)
DEBUGWRAP8(GetMenuInfo)
DEBUGWRAP8(SetMenuInfo)
DEBUGWRAP8(GetMonitorInfoW)
DEBUGWRAP8(GetMonitorInfoA)
DEBUGWRAP8(MonitorFromWindow)
DEBUGWRAP8(MonitorFromRect)
DEBUGWRAP12(MonitorFromPoint)
DEBUGWRAP16(EnumDisplayMonitors)
DEBUGWRAP16(EnumDisplayDevicesA)
DEBUGWRAP16(EnumDisplayDevicesW)
DEBUGWRAP8(SendIMEMessageExA)
DEBUGWRAP8(SendIMEMessageExW)
DEBUGWRAP8(WINNLSEnableIME)
DEBUGWRAP4(WINNLSGetEnableStatus)
DEBUGWRAP4(WINNLSGetIMEHotkey)


