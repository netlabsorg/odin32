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


#if 0

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_windlg
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


#endif
