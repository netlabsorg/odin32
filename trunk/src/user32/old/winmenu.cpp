/* $Id: winmenu.cpp,v 1.1 2001-05-11 08:36:31 sandervl Exp $ */

/*
 * Win32 menu API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * Parts ported from Wine:
 * Copyright 1993 Martin Ayotte
 * Copyright 1994 Alexandre Julliard
 * Copyright 1997 Morten Welinder
 *
 *
 * TODO: Set/GetMenu(Item)Info only partially implemented
 * TODO: Memory leak when deleting submenus
 * TODO: Check error nrs
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <odin.h>
#include <odinwrap.h>
#include <stdlib.h>
#include <string.h>
#include <win32wbase.h>
#include "oslibmenu.h"
#include "oslibwin.h"
#include "winmenudef.h"

ODINDEBUGCHANNEL(USER32)

BOOL  ODIN_INTERNAL ODIN_InsertMenuA(HMENU,UINT,UINT,UINT,LPCSTR);
BOOL  ODIN_INTERNAL ODIN_InsertMenuW(HMENU,UINT,UINT,UINT,LPCWSTR);
BOOL  ODIN_INTERNAL ODIN_InsertMenuItemA(HMENU,UINT,BOOL,const MENUITEMINFOA*);
BOOL  ODIN_INTERNAL ODIN_InsertMenuItemW(HMENU,UINT,BOOL,const MENUITEMINFOW*);
BOOL  ODIN_INTERNAL ODIN_AppendMenuA(HMENU,UINT,UINT,LPCSTR);
BOOL  ODIN_INTERNAL ODIN_AppendMenuW(HMENU,UINT,UINT,LPCWSTR);
HMENU ODIN_INTERNAL ODIN_CreateMenu(void);
HMENU ODIN_INTERNAL ODIN_CreatePopupMenu(void);
BOOL  ODIN_INTERNAL ODIN_DestroyMenu(HMENU);
BOOL  ODIN_INTERNAL ODIN_DeleteMenu(HMENU, UINT, UINT);

//@@@PH: experiment with WINE LoadMenuIndirect code
#include <heapstring.h>
#define MENU_ITEM_TYPE(flags) \
  ((flags) & (MF_STRING | MF_BITMAP | MF_OWNERDRAW | MF_SEPARATOR))

#define IS_STRING_ITEM(flags) (MENU_ITEM_TYPE ((flags)) == MF_STRING)
#define IS_BITMAP_ITEM(flags) (MENU_ITEM_TYPE ((flags)) == MF_BITMAP)

/**********************************************************************
 *         MENU_ParseResource
 *
 * Parse a standard menu resource and add items to the menu.
 * Return a pointer to the end of the resource.
 */
static LPCSTR MENU_ParseResource( LPCSTR res, HMENU hMenu)
{
    WORD flags, id = 0;
    LPCSTR str;

    do
    {
        flags = GET_WORD(res);
        res += sizeof(WORD);
        if (!(flags & MF_POPUP))
        {
            id = GET_WORD(res);
            res += sizeof(WORD);
        }
        if (!IS_STRING_ITEM(flags))
            dprintf(("USER32: WinMenu: MENU_ParseResource: not a string item %04x\n", flags ));
        str = res;

        res += (lstrlenW((LPCWSTR)str) + 1) * sizeof(WCHAR);
        if (flags & MF_POPUP)
        {
            HMENU hSubMenu = ODIN_CreatePopupMenu();
            if (!hSubMenu) return NULL;
            if (!(res = MENU_ParseResource( res, hSubMenu)))
                return NULL;
            ODIN_AppendMenuW( hMenu, flags, (UINT)hSubMenu, (LPCWSTR)str );
        }
        else  /* Not a popup */
        {
            ODIN_AppendMenuW( hMenu, flags, id, *(LPCWSTR)str ? (LPCWSTR)str : NULL );
        }
    } while (!(flags & MF_END));
    return res;
}


/**********************************************************************
 *         MENUEX_ParseResource
 *
 * Parse an extended menu resource and add items to the menu.
 * Return a pointer to the end of the resource.
 */
static LPCSTR MENUEX_ParseResource( LPCSTR res, HMENU hMenu)
{
   WORD resinfo;

    do {
        MENUITEMINFOW mii;

        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STATE | MIIM_ID | MIIM_TYPE;
        mii.fType = GET_DWORD(res);
        res += sizeof(DWORD);
        mii.fState = GET_DWORD(res);
        res += sizeof(DWORD);
        mii.wID = GET_DWORD(res);
        res += sizeof(DWORD);
        resinfo = GET_WORD(res); /* FIXME: for 16-bit apps this is a byte.  */
        res += sizeof(WORD);

        /* Align the text on a word boundary.  */
        res += (~((int)res - 1)) & 1;
        mii.dwTypeData = (LPWSTR) res;
        res += (1 + lstrlenW(mii.dwTypeData)) * sizeof(WCHAR);
        /* Align the following fields on a dword boundary.  */
        res += (~((int)res - 1)) & 3;

        /* FIXME: This is inefficient and cannot be optimised away by gcc.  */
        {
            LPSTR newstr = HEAP_strdupWtoA(GetProcessHeap(), 0, mii.dwTypeData);

            dprintf(("USER32:WinMenu:MENUEX_ParseResource Menu item: [%08x,%08x,%04x,%04x,%s]\n",
                     mii.fType, mii.fState, mii.wID, resinfo, newstr));
            HeapFree( GetProcessHeap(), 0, newstr );
        }

        if (resinfo & 1) {                    /* Pop-up? */
            /* DWORD helpid = GET_DWORD(res); FIXME: use this.  */
            res += sizeof(DWORD);
            mii.hSubMenu = ODIN_CreatePopupMenu();
            if (!mii.hSubMenu)
                return NULL;
            if (!(res = MENUEX_ParseResource(res, mii.hSubMenu))) {
                DestroyMenu(mii.hSubMenu);
                return NULL;
            }
            mii.fMask |= MIIM_SUBMENU;
            mii.fType |= MF_POPUP;
        }
        ODIN_InsertMenuItemW(hMenu, -1, MF_BYPOSITION, &mii);
    }
    while (!(resinfo & MF_END));

    return res;
}
/**********************************************************************
 *    myLoadMenuIndirect
 */
HMENU myLoadMenuIndirect(LPCVOID pMenuTemplate)
{
  HMENU hMenu;
  WORD  version,
        offset;
  LPCSTR p = (LPCSTR)pMenuTemplate;

    version = GET_WORD(p);
    p += sizeof(WORD);

    switch (version)
    {
    case 0:
        offset = GET_WORD(p);
        p += sizeof(WORD) + offset;
        if (!(hMenu = ODIN_CreateMenu()))
            return 0;
        if (!MENU_ParseResource( p, hMenu))
        {
            DestroyMenu( hMenu );
            return 0;
        }
        return hMenu;

    case 1:
        offset = GET_WORD(p);
        p += sizeof(WORD) + offset;
        if (!(hMenu = ODIN_CreateMenu()))
            return 0;
        if (!MENUEX_ParseResource( p, hMenu))
        {
            ODIN_DestroyMenu( hMenu );
            return 0;
        }
        return hMenu;

    default:
        dprintf(("USER32: LoadMenuIndirectA: version %d not supported.\n", version));
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
void SetInternalMenuInfo(HMENU hMenu)
{
  LPPOPUPMENU lpMenuInfo;

    lpMenuInfo = (LPPOPUPMENU)malloc(sizeof(*lpMenuInfo));
    memset(lpMenuInfo, 0, sizeof(*lpMenuInfo));
    OSLibWinSetWindowULong(hMenu, OSLIB_QWL_USER, (ULONG)lpMenuInfo);
}
//******************************************************************************
//******************************************************************************
LPPOPUPMENU GetInternalMenuInfo(HMENU hMenu)
{
    return (LPPOPUPMENU)OSLibWinGetWindowULong(hMenu, OSLIB_QWL_USER);
}
//******************************************************************************
//******************************************************************************
void DeleteInternalMenuInfo(HMENU hMenu)
{
  LPPOPUPMENU lpMenuInfo;

    lpMenuInfo = (LPPOPUPMENU)OSLibWinGetWindowULong(hMenu, OSLIB_QWL_USER);
    if(lpMenuInfo) {
        free(lpMenuInfo);
        OSLibWinSetWindowULong(hMenu, OSLIB_QWL_USER, 0);
    }
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HMENU, CreateMenu)
{
  HMENU hMenu;

    dprintf(("USER32: CreateMenu\n"));

    hMenu = OSLibWinCreateEmptyMenu();
    if(hMenu) {
            SetInternalMenuInfo(hMenu);
    }
    else    SetLastError(ERROR_INVALID_PARAMETER); //wrong error

    return hMenu;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HMENU, CreatePopupMenu)
{
  HMENU hMenu;

    dprintf(("USER32: CreatePopupMenu\n"));

    hMenu = OSLibWinCreateEmptyPopupMenu();
    if(hMenu) {
            SetInternalMenuInfo(hMenu);
    }
    else    SetLastError(ERROR_INVALID_PARAMETER); //wrong error

    return hMenu;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU,     LoadMenuA,
              HINSTANCE, hinst,
              LPCSTR,    lpszMenu)
{
  Win32Resource *winres;
  HMENU hMenu;

    winres = (Win32Resource *)FindResourceA(hinst, lpszMenu, RT_MENUA);
    if(winres)
    {
        hMenu = myLoadMenuIndirect((MENUITEMTEMPLATEHEADER *)winres->lockResource());
        if(hMenu) {
                SetInternalMenuInfo(hMenu);
        }
        else    SetLastError(ERROR_INVALID_PARAMETER);

        delete winres;
        return hMenu;
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, LoadMenuW,
              HINSTANCE, hinst,
              LPCWSTR, lpszMenu)
{
  Win32Resource *winres;
  HMENU hMenu;

    winres = (Win32Resource *)FindResourceW(hinst, lpszMenu, RT_MENUW);
    if(winres) {
        hMenu = myLoadMenuIndirect((MENUITEMTEMPLATEHEADER *)winres->lockResource());
        if(hMenu) {
                SetInternalMenuInfo(hMenu);
        }
        else    SetLastError(ERROR_INVALID_PARAMETER);

        delete winres;
        return hMenu;
    }
    return 0;
}
//******************************************************************************
//NOTE: menutemplate strings are always in Unicode format
//******************************************************************************
ODINFUNCTION1(HMENU, LoadMenuIndirectW,
              const MENUITEMTEMPLATEHEADER *, menuTemplate)
{
  HMENU hMenu;

    hMenu = myLoadMenuIndirect(menuTemplate);
    if(hMenu) {
            SetInternalMenuInfo(hMenu);
    }
    else    SetLastError(ERROR_INVALID_PARAMETER);

    return (HMENU)hMenu;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL,  DestroyMenu,
              HMENU, hMenu)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    DeleteInternalMenuInfo(hMenu);
    return O32_DestroyMenu(hMenu);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HMENU, GetMenu,
              HWND,  hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window)
    {
        dprintf(("GetMenu, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }

    return window->GetMenu();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,  SetMenu,
              HWND,  hwnd,
              HMENU, hMenu)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window)
    {
        dprintf(("SetMenu, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }

    window->SetMenu(hMenu);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(DWORD, GetMenuCheckMarkDimensions)
{
  return O32_GetMenuCheckMarkDimensions();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(int,   GetMenuItemCount,
              HMENU, hMenu)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return OSLibGetMenuItemCount(hMenu);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(UINT,  GetMenuItemID,
              HMENU, hMenu,
              int,   nPos)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_GetMenuItemID(hMenu, nPos);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(UINT,  GetMenuState,
              HMENU, hMenu,
              UINT,  arg2,
              UINT,  arg3)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_GetMenuState(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,   GetMenuStringA,
              HMENU, hMenu,
              UINT,  idItem,
              LPSTR, lpsz,
              int,   cchMax,
              UINT,  fuFlags)
{
 int rc, nritems;

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if(!lpsz || !cchMax) {//determine menu string length
        char menustr[256];
        rc = O32_GetMenuString(hMenu, idItem, menustr, sizeof(menustr), fuFlags);
        //SvL: Open32 returns the wrong error
        return (rc == -1) ? 0 : rc;
    }
    rc = O32_GetMenuString(hMenu, idItem, lpsz, cchMax, fuFlags);
    //SvL: Open32 returns the wrong error
    return (rc == -1) ? 0 : rc;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,   GetMenuStringW,
              HMENU, hMenu,
              UINT,  idItem,
              LPWSTR,lpsz,
              int,   cchMax,
              UINT,  fuFlags)
{
  char *astring = (char *)malloc(cchMax);
  int   rc;

    rc = ODIN_GetMenuStringA(hMenu, idItem, astring, cchMax, fuFlags);
    if(rc)
    {
            dprintf(("USER32: GetMenuStringW %s\n", astring));
            AsciiToUnicode(astring, lpsz);
    }
    else    lpsz[0] = 0;
    free(astring);

    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, SetMenuItemBitmaps,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              HBITMAP, arg4,
              HBITMAP, arg5)
{
    dprintf(("USER32:  SetMenuItemBitmaps\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_SetMenuItemBitmaps(hMenu, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, GetSubMenu,
              HWND, hMenu,
              int, arg2)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_GetSubMenu(hMenu, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, GetSystemMenu,
              HWND,  hSystemWindow,
              BOOL,  bRevert)
{
    Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hSystemWindow);
    if(!window)
    {
        dprintf(("GetSystemMenu, window %x not found", hSystemWindow));
        return 0;
    }

    return window->GetSystemMenu(bRevert);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, IsMenu,
              HMENU, hMenu)
{
    dprintf(("USER32:  IsMenu\n"));
    return O32_IsMenu(hMenu);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION7(BOOL, TrackPopupMenu,
              HMENU, hMenu,
              UINT, arg2,
              int, arg3,
              int, arg4,
              int, arg5,
              HWND, arg6,
              const RECT *, arg7)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(arg6);
    if(!window)
    {
        dprintf(("TrackPopupMenu, window %x not found", arg6));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("USER32:  TrackPopupMenu\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_TrackPopupMenu(hMenu, arg2, arg3, arg4, arg5, window->getOS2WindowHandle(),
                              arg7);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(BOOL, TrackPopupMenuEx,
              HMENU, hMenu,
              UINT, flags,
              int, X,
              int, Y,
              HWND, hwnd,
              LPTPMPARAMS, lpPM)
{
  RECT *rect = NULL;
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window)
    {
        dprintf(("TrackPopupMenu, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }

    dprintf(("USER32:  TrackPopupMenuEx, not completely implemented\n"));

    if (lpPM != NULL) // this parameter can be NULL
      if(lpPM->cbSize != 0)
        rect = &lpPM->rcExclude;

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_TrackPopupMenu(hMenu, flags, X, Y, 0, window->getOS2WindowHandle(), rect);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, AppendMenuA,
              HMENU, hMenu,
              UINT, uFlags,
              UINT, id,
              LPCSTR, lpNewItem)
{
     return ODIN_InsertMenuA( hMenu, -1, uFlags | MF_BYPOSITION, id, lpNewItem );
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, AppendMenuW,
              HMENU, hMenu,
              UINT, uFlags,
              UINT, id,
              LPCWSTR, lpNewItem)
{
     return ODIN_InsertMenuW( hMenu, -1, uFlags | MF_BYPOSITION, id, lpNewItem );
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, CheckMenuItem,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_CheckMenuItem(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,EnableMenuItem,HMENU,hMenu,
                                  UINT, uIDEnableItem,
                                  UINT, uEnable)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_EnableMenuItem(hMenu,
                              uIDEnableItem,
                              uEnable);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ModifyMenuA,
              HMENU, hMenu,
              UINT, uItem,
              UINT, fuFlags,
              UINT, idNewItem,
              LPCSTR, lpszNewItem)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if(IS_STRING_ITEM(fuFlags) && HIWORD(lpszNewItem) != 0) {
        dprintf(("ModifyMenuA %s", lpszNewItem));
    }
/* //CB: doesn't compile, but don't need it anyway
    if(((fuFlags & (MF_BYCOMMAND|MF_BYPOSITION|MF_POPUP)) == MF_BYCOMMAND) && uItem != idNewItem) {
        DWORD pos = OSLibGetMenuItemPos(hMenu, uItem);
        ODIN_DeleteMenu(hMenu, uItem, MF_BYCOMMAND);
        return ODIN_InsertMenuA(hMenu, pos, fuFlags | MF_BYPOSITION, idNewItem, lpszNewItem);
    }
*/
    return O32_ModifyMenu(hMenu, uItem, fuFlags, idNewItem, lpszNewItem);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ModifyMenuW,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              UINT, arg4,
              LPCWSTR, arg5)
{
 BOOL  rc;
 char *astring = NULL;

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if(IS_STRING_ITEM(arg3) && HIWORD(arg5) != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg5);
    else
      astring = (char *) arg5;

    rc = ODIN_ModifyMenuA(hMenu, arg2, arg3, arg4, astring);
    if(IS_STRING_ITEM(arg3) && HIWORD(arg5) != 0)
      FreeAsciiString(astring);

    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, RemoveMenu,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_RemoveMenu(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, DeleteMenu,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_DeleteMenu(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, HiliteMenuItem,
              HWND, hMenu,
              HMENU, arg2,
              UINT, arg3,
              UINT, arg4)
{
    dprintf(("USER32:  OS2HiliteMenuItem\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_HiliteMenuItem(hMenu, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, InsertMenuA,
              HMENU, hMenu,
              UINT, pos,
              UINT, flags,
              UINT, id,
              LPCSTR, str)
{
    if(IS_STRING_ITEM(flags) && HIWORD(str)) {
         dprintf(("USER32: InsertMenuA %x %d %x %d %s", hMenu, pos, flags, id, str));
    }
    else dprintf(("USER32: InsertMenuA %x %d %x %d %x", hMenu, pos, flags, id, str));

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if(IS_STRING_ITEM(flags) && (!str || *str == NULL)) {
        flags |= MF_SEPARATOR;
    }
    //SvL: RealPlayer calls InsertMenu with flag 0 & pos -1
    if((flags & (MF_BYCOMMAND|MF_BYPOSITION)) == 0 && (pos == 0xffffffff))
        flags |= MF_BYPOSITION;

    return O32_InsertMenu(hMenu, pos, flags, id, str);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, InsertMenuW,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              UINT, arg4,
              LPCWSTR, arg5)
{
 BOOL  rc;
 char *astring = NULL;

    if(IS_STRING_ITEM(arg3) && HIWORD(arg5) != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg5);
    else
      astring = (char *) arg5;

    rc = ODIN_InsertMenuA(hMenu, arg2, arg3, arg4, astring);

    if(IS_STRING_ITEM(arg3) && HIWORD(arg5) != 0)
      FreeAsciiString(astring);

    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SetMenuContextHelpId,
              HMENU, hMenu,
              DWORD, dwContextHelpId)
{
 POPUPMENU *menu;

    menu = GetInternalMenuInfo(hMenu);
    if(menu == NULL) {
        dprintf(("USER32: SetMenuContextHelpId(%x) No POPUPMENU structure found!", hMenu));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    dprintf(("USER32:  SetMenuContextHelpId %x %d", hMenu, dwContextHelpId));
    menu->dwContextHelpID = dwContextHelpId;
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GetMenuContextHelpId,
              HMENU, hMenu)
{
 POPUPMENU *menu;

    menu = GetInternalMenuInfo(hMenu);
    if(menu == NULL) {
        dprintf(("USER32: GetMenuContextHelpId(%x) No POPUPMENU structure found!", hMenu));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    dprintf(("USER32:  GetMenuContextHelpId %x %d", hMenu, menu->dwContextHelpID));
    return menu->dwContextHelpID;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, CheckMenuRadioItem,
              HMENU, hMenu,
              UINT, idFirst,
              UINT, idLast,
              UINT, idCheck,
              UINT, uFlags)
{
  dprintf(("USER32:  OS2CheckMenuRadioItem, not implemented\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ChangeMenuA,
              HMENU, hMenu,
              UINT, pos,
              LPCSTR, data,
              UINT, id,
              UINT, flags)
{
    dprintf(("USER32:  ChangeMenuA flags %X\n", flags));

    if (flags & MF_APPEND) return ODIN_AppendMenuA(hMenu, flags & ~MF_APPEND,
                                                   id, data );
    if (flags & MF_DELETE) return ODIN_DeleteMenu(hMenu, pos, flags & ~MF_DELETE);
    if (flags & MF_CHANGE) return ODIN_ModifyMenuA(hMenu, pos, flags & ~MF_CHANGE,
                                                   id, data );
    if (flags & MF_REMOVE) return ODIN_RemoveMenu(hMenu,
                                                  flags & MF_BYPOSITION ? pos : id,
                                                  flags & ~MF_REMOVE );
    /* Default: MF_INSERT */
    return InsertMenuA( hMenu, pos, flags, id, data );
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ChangeMenuW,
              HMENU, hMenu,
              UINT, pos,
              LPCWSTR, data,
              UINT, id,
              UINT, flags)
{
    dprintf(("USER32:  ChangeMenuW flags %X\n", flags));

    if (flags & MF_APPEND) return ODIN_AppendMenuW(hMenu, flags & ~MF_APPEND,
                                                   id, data );
    if (flags & MF_DELETE) return ODIN_DeleteMenu(hMenu, pos, flags & ~MF_DELETE);
    if (flags & MF_CHANGE) return ODIN_ModifyMenuW(hMenu, pos, flags & ~MF_CHANGE,
                                                   id, data );
    if (flags & MF_REMOVE) return ODIN_RemoveMenu(hMenu,
                                                  flags & MF_BYPOSITION ? pos : id,
                                                     flags & ~MF_REMOVE );
    /* Default: MF_INSERT */
    return InsertMenuW(hMenu, pos, flags, id, data);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, SetMenuItemInfoA,
              HMENU, hMenu,
              UINT, par1,
              BOOL, par2,
              const MENUITEMINFOA *, lpmii)
{
    dprintf(("USER32:  SetMenuItemInfoA faked %x", hMenu));

    if (!hMenu) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return TRUE;
}
/*****************************************************************************
 * Function  : SetMenuItemInfoW
 * Purpose   : The SetMenuItemInfo function changes information about a menu item.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, SetMenuItemInfoW,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, fByPosition,
              const MENUITEMINFOW *, lpmmi)
{
  dprintf(("USER32:SetMenuItemInfoW (%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmmi));

  return (SetMenuItemInfoA(hMenu,
                           uItem,
                           fByPosition,
                           (const MENUITEMINFOA *)lpmmi));
}
/*****************************************************************************
 * Function  : GetMenuDefaultItem
 * Purpose   : TheGetMenuDefaultItem function determines the default menu item
 *             on the specified menu.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the identifier or
 *             position of the menu item.
 *             If the function fails, the return value is - 1.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION3(UINT, GetMenuDefaultItem,
              HMENU, hMenu,
              UINT, fByPos,
              UINT, gmdiFlags)
{
  dprintf(("USER32:GetMenuDefaultItem (%08xh,%u,%08x) not implemented.\n",
         hMenu,
         fByPos,
         gmdiFlags));

  return (-1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, SetMenuDefaultItem,
              HMENU, hMenu,
              UINT, uItem,
              UINT, fByPos)
{
    dprintf(("USER32:  SetMenuDefaultItem, faked\n"));
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL GetMenuItemInfoAW(HMENU hMenu, UINT uItem, BOOL byPos, MENUITEMINFOA *lpmii, BOOL unicode)
{
    if(byPos) {
        uItem = GetMenuItemID(hMenu, uItem);
    }
    if(ODIN_GetMenuState(hMenu, uItem, MF_BYCOMMAND) == -1) {
        //item doesn't exist
        dprintf(("USER32:  GetMenuItemInfoAW %x item %d doesn't exist", hMenu, uItem));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (lpmii->fMask & MIIM_TYPE)
    {
        lpmii->fType = ODIN_GetMenuState(hMenu, uItem, MF_BYCOMMAND); //not correct
//      lpmii->fType = menu->fType;
        if (unicode) {
            lpmii->cch = ODIN_GetMenuStringW(hMenu, uItem, (LPWSTR)lpmii->dwTypeData, lpmii->cch, MF_BYCOMMAND);
        }
        else {
            lpmii->cch = ODIN_GetMenuStringA(hMenu, uItem, (LPSTR)lpmii->dwTypeData, lpmii->cch, MF_BYCOMMAND);
        }
//TODO:
#if 0
        switch (MENU_ITEM_TYPE(menu->fType)) {
        case MF_STRING:
            if (menu->text && lpmii->dwTypeData && lpmii->cch) {
                if (unicode) {
                    lstrcpynAtoW((LPWSTR) lpmii->dwTypeData, menu->text, lpmii->cch);
                    lpmii->cch = lstrlenW((LPWSTR)menu->text);
                }
                else {
                    lstrcpynA(lpmii->dwTypeData, menu->text, lpmii->cch);
                    lpmii->cch = lstrlenA(menu->text);
                }
            }
            break;
        case MF_OWNERDRAW:
        case MF_BITMAP:
            lpmii->dwTypeData = menu->text;
            /* fall through */
        default:
            lpmii->cch = 0;
        }
#endif
    }

    if (lpmii->fMask & MIIM_STRING) {
        if (unicode) {
            lpmii->cch = ODIN_GetMenuStringW(hMenu, uItem, (LPWSTR)lpmii->dwTypeData, lpmii->cch, MF_BYCOMMAND);
        }
        else {
            lpmii->cch = ODIN_GetMenuStringA(hMenu, uItem, (LPSTR)lpmii->dwTypeData, lpmii->cch, MF_BYCOMMAND);
        }
    }

//TODO:
#if 0
    if (lpmii->fMask & MIIM_FTYPE)
        lpmii->fType = menu->fType;

    if (lpmii->fMask & MIIM_BITMAP)
        lpmii->hbmpItem = menu->hbmpItem;
#endif

    if (lpmii->fMask & MIIM_STATE)
        lpmii->fState = ODIN_GetMenuState(hMenu, uItem, MF_BYCOMMAND);

    if (lpmii->fMask & MIIM_ID)
        lpmii->wID = uItem;

//TODO:
#if 1
    lpmii->hSubMenu = 0;
    lpmii->hbmpChecked = 0;
    lpmii->hbmpUnchecked = 0;
    lpmii->dwItemData = 0;
#else
    if (lpmii->fMask & MIIM_SUBMENU)
        lpmii->hSubMenu = ODIN_GetSubMenu(hMenu, uItem); //need index, not id

    if (lpmii->fMask & MIIM_CHECKMARKS) {
        lpmii->hbmpChecked = menu->hCheckBit;
        lpmii->hbmpUnchecked = menu->hUnCheckBit;
    }
    if (lpmii->fMask & MIIM_DATA)
        lpmii->dwItemData = menu->dwItemData;
#endif

    return(FALSE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, GetMenuItemInfoA,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, byPos,
              MENUITEMINFOA *, lpMenuItemInfo)
{
    return GetMenuItemInfoAW(hMenu, uItem, byPos, lpMenuItemInfo, FALSE);
}
/*****************************************************************************
 * Function  : GetMenuItemInfoW
 * Purpose   : The GetMenuItemInfo function retrieves information about a menu item.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, GetMenuItemInfoW,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, byPos,
              MENUITEMINFOW *, lpMenuItemInfo)
{
    return GetMenuItemInfoAW(hMenu, uItem, byPos, (MENUITEMINFOA*)lpMenuItemInfo, TRUE);
}
/*****************************************************************************
 * Function  : GetMenuItemRect
 * Purpose   : The GetMenuItemRect function retrieves the bounding rectangle
 *             for the specified menu item.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get
 *             extended error information, use the GetLastError function.
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL GetMenuItemRect(HWND hwnd, HMENU hMenu, UINT uItem, LPRECT lprcItem)
{
 BOOL rc;

    rc = OSLibGetMenuItemRect(hMenu, uItem, lprcItem);
    dprintf(("GetMenuItemRect %x %x %x (%d,%d)(%d,%d)", hwnd, hMenu, uItem, lprcItem->top, lprcItem->left, lprcItem->bottom, lprcItem->right));
    return rc;
}
/*****************************************************************************
 * Function  : InsertMenuItemA
 * Purpose   : The InsertMenuItem function inserts a new menu item at the specified
 *             position in a menu bar or pop-up menu.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, InsertMenuItemA,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, fByPosition,
              const MENUITEMINFOA*, lpmii)
{
 DWORD dwType;
 BOOL rc;

    dprintf(("USER32:InsertMenuItemA (%08xh,%08xh,%u,%08x) not correctly implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

    if(fByPosition) {
            dwType = lpmii->fType | MF_BYPOSITION;
    }
    else    dwType = lpmii->fType | MF_BYCOMMAND;

    if(lpmii->fMask & MIIM_SUBMENU && lpmii->hSubMenu) {
        rc &= ODIN_InsertMenuA(hMenu, uItem, dwType | MF_POPUP, lpmii->hSubMenu, lpmii->dwTypeData);
    }
    else
    if(lpmii->fMask & MIIM_ID) {
        rc = ODIN_InsertMenuA(hMenu, uItem, dwType, lpmii->wID, lpmii->dwTypeData);
    }
    if(lpmii->fMask & MIIM_STATE) {
    //TODO
    }
    return rc;
}


/*****************************************************************************
 * Function  : InsertMenuItemW
 * Purpose   : The InsertMenuItem function inserts a new menu item at the specified
 *             position in a menu bar or pop-up menu.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, InsertMenuItemW,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, fByPosition,
              const MENUITEMINFOW *, lpmii)
{
    dprintf(("USER32:InsertMenuItemW (%08xh,%08xh,%u,%08x) not correctly implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

    if(fByPosition) {
            return ODIN_InsertMenuW(hMenu, uItem, lpmii->fType | MF_BYPOSITION, (lpmii->fType & MF_POPUP) ? lpmii->hSubMenu : lpmii->wID, lpmii->dwTypeData);
    }
    else    return ODIN_InsertMenuW(hMenu, uItem, lpmii->fType | MF_BYCOMMAND, (lpmii->fType & MF_POPUP) ? lpmii->hSubMenu : lpmii->wID, lpmii->dwTypeData);
}
/*****************************************************************************
 * Function  : MenuItemFromPoint
 * Purpose   : TheMenuItemFromPoint function determines which menu item, if
 *             any, is at the specified location.
 * Parameters:
 * Variables :
 * Result    : Returns the zero-based position of the menu item at the specified
 *             location or -1 if no menu item is at the specified location.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION3(UINT, MenuItemFromPoint,
              HWND, hWnd,
              HMENU, hMenu,
              POINT, ptScreen)
{
  dprintf(("USER32:MenuItemFromPoint (%08xh,%08xh,%u) not implemented.\n",
         hWnd,
         hMenu,
         ptScreen));

  return (-1);
}


/*****************************************************************************
 * Function  :  GetMenuInfo
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB win98/NT5.0
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION2(BOOL, GetMenuInfo,
              HMENU, hMenu,
              LPMENUINFO, lpmi)
{
 POPUPMENU *menu;

    menu = GetInternalMenuInfo(hMenu);
    if(menu == NULL) {
        dprintf(("USER32: GetMenuInfo(%08xh,%08xh) No POPUPMENU structure found!", hMenu, lpmi));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    dprintf(("USER32: GetMenuInfo(%08xh,%08xh)", hMenu, lpmi));

    if (lpmi)
    {
        if (lpmi->fMask & MIM_BACKGROUND)
            lpmi->hbrBack = menu->hbrBack;

        if (lpmi->fMask & MIM_HELPID)
            lpmi->dwContextHelpID = menu->dwContextHelpID;

        if (lpmi->fMask & MIM_MAXHEIGHT)
            lpmi->cyMax = menu->cyMax;

        if (lpmi->fMask & MIM_MENUDATA)
            lpmi->dwMenuData = menu->dwMenuData;

        if (lpmi->fMask & MIM_STYLE)
            lpmi->dwStyle = menu->dwStyle;

        return TRUE;
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
}
/*****************************************************************************
 * Function  :  SetMenuInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * FIXME
 * MIM_APPLYTOSUBMENUS
 * actually use the items to draw the menu
 * Status    : UNTESTED STUB win98/NT5.0
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION2(BOOL, SetMenuInfo,
              HMENU, hMenu,
              LPCMENUINFO, lpmi)
{
 POPUPMENU *menu;

    menu = GetInternalMenuInfo(hMenu);
    if(menu == NULL) {
        dprintf(("USER32: SetMenuInfo(%08xh,%08xh) No POPUPMENU structure found!", hMenu, lpmi));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    dprintf(("USER32: SetMenuInfo(%08xh,%08xh)", hMenu, lpmi));

    if (lpmi && (lpmi->cbSize==sizeof(MENUINFO)))
    {
        if (lpmi->fMask & MIM_BACKGROUND)
            menu->hbrBack = lpmi->hbrBack;

        if (lpmi->fMask & MIM_HELPID)
            menu->dwContextHelpID = lpmi->dwContextHelpID;

        if (lpmi->fMask & MIM_MAXHEIGHT)
            menu->cyMax = lpmi->cyMax;

        if (lpmi->fMask & MIM_MENUDATA)
            menu->dwMenuData = lpmi->dwMenuData;

        if (lpmi->fMask & MIM_STYLE)
            menu->dwStyle = lpmi->dwStyle;

        return TRUE;
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
}
//******************************************************************************
//******************************************************************************

