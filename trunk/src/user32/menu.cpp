/* $Id: menu.cpp,v 1.1 1999-05-24 20:20:02 ktk Exp $ */

/*
 * Win32 menu API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * Parts ported from Wine: (ChangeMenuA/W)
 * Copyright 1993 Martin Ayotte
 * Copyright 1994 Alexandre Julliard
 * Copyright 1997 Morten Welinder
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include "user32.h"

//******************************************************************************
//******************************************************************************
HMENU WIN32API GetMenu( HWND arg1)
{
#ifdef DEBUG
 HMENU rc;

    rc = O32_GetMenu(arg1);
    WriteLog("USER32:  GetMenu %X returned %d\n", arg1, rc);
    return rc;
#else
    return O32_GetMenu(arg1);
#endif
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMenuCheckMarkDimensions(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetMenuCheckMarkDimensions\n");
#endif
    return O32_GetMenuCheckMarkDimensions();
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMenuItemCount( HMENU arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetMenuItemCount\n");
#endif
    return O32_GetMenuItemCount(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetMenuItemID( HMENU arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetMenuItemID\n");
#endif
    return O32_GetMenuItemID(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetMenuState( HMENU arg1, UINT arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetMenuState\n");
#endif
    return O32_GetMenuState(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMenuStringA( HMENU arg1, UINT arg2, LPSTR arg3, int arg4, UINT  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  GetMenuString\n");
#endif
    return O32_GetMenuString(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMenuStringW(HMENU hmenu, UINT idItem, LPWSTR lpsz, int cchMax, UINT fuFlags)
{
 char *astring = (char *)malloc(cchMax);
 int   rc;

    dprintf(("USER32:  OS2GetMenuStringW\n"));
    rc = O32_GetMenuString(hmenu, idItem, astring, cchMax, fuFlags);
    free(astring);
    if(rc) {
		dprintf(("USER32: OS2GetMenuStringW %s\n", astring));
		AsciiToUnicode(astring, lpsz);
    }
    else	lpsz[0] = 0;
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMenu( HWND arg1, HMENU  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2SetMenu\n");
#endif
    return O32_SetMenu(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMenuItemBitmaps( HMENU arg1, UINT arg2, UINT arg3, HBITMAP arg4, HBITMAP  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2SetMenuItemBitmaps\n");
#endif
    return O32_SetMenuItemBitmaps(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API GetSubMenu(HWND arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetSubMenu\n");
#endif
    return O32_GetSubMenu(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API GetSystemMenu( HWND arg1, BOOL  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetSystemMenu\n");
#endif
    return O32_GetSystemMenu(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsMenu( HMENU arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsMenu\n");
#endif
    return O32_IsMenu(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TrackPopupMenu(HMENU arg1, UINT arg2, int arg3, int arg4, int arg5, HWND arg6, const RECT *  arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  TrackPopupMenu\n");
#endif
    return O32_TrackPopupMenu(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TrackPopupMenuEx(HMENU hmenu, UINT flags, int X, int Y, HWND hwnd, LPTPMPARAMS lpPM)
{
 RECT *rect = NULL;

#ifdef DEBUG
  WriteLog("USER32:  TrackPopupMenuEx, not completely implemented\n");
#endif
  if(lpPM->cbSize != 0)
	rect = &lpPM->rcExclude;

  return O32_TrackPopupMenu(hmenu, flags, X, Y, 0, hwnd, rect);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AppendMenuA(HMENU hMenu, UINT uFlags, UINT ulDNewItem,
			     LPCSTR lpNewItem)
{
#ifdef DEBUG
BOOL rc;

    WriteLog("USER32:  OS2AppendMenuA uFlags = %X\n", uFlags);

    if(uFlags & MF_STRING || uFlags == 0)
	    WriteLog("USER32:  OS2AppendMenuA %s\n", lpNewItem);

    rc = O32_AppendMenu(hMenu, uFlags, ulDNewItem, lpNewItem);
    WriteLog("USER32:  OS2AppendMenuA returned %d\n", rc);
    return rc;
#else
    return O32_AppendMenu(hMenu, uFlags, ulDNewItem, lpNewItem);
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AppendMenuW( HMENU arg1, UINT arg2, UINT arg3, LPCWSTR  arg4)
{
 BOOL  rc;
 char *astring = UnicodeToAsciiString((LPWSTR)arg4);

#ifdef DEBUG
    WriteLog("USER32:  OS2AppendMenuW\n");
#endif
    rc = O32_AppendMenu(arg1, arg2, arg3, astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API CheckMenuItem( HMENU arg1, UINT arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2CheckMenuItem\n");
#endif
    return O32_CheckMenuItem(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API CreateMenu(void)
{
#ifdef DEBUG
 HMENU rc;

    rc = O32_CreateMenu();
    WriteLog("USER32:  OS2CreateMenu returned %d\n", rc);
    return(rc);
#else
    return(O32_CreateMenu());
#endif
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API CreatePopupMenu(void)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2CreatePopupMenu\n");
#endif
    return O32_CreatePopupMenu();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableMenuItem( HMENU arg1, UINT arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2EnableMenuItem\n");
#endif
    return O32_EnableMenuItem(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ModifyMenuA( HMENU arg1, UINT arg2, UINT arg3, UINT arg4, LPCSTR  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2ModifyMenuA\n");
#endif
    return O32_ModifyMenu(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ModifyMenuW( HMENU arg1, UINT arg2, UINT arg3, UINT arg4, LPCWSTR arg5)
{
 BOOL  rc;
 char *astring = UnicodeToAsciiString((LPWSTR)arg5);

#ifdef DEBUG
    WriteLog("USER32: OS2ModifyMenuW %s\n", astring);
#endif
    rc = O32_ModifyMenu(arg1, arg2, arg3, arg4, astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveMenu( HMENU arg1, UINT arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2RemoveMenu\n");
#endif
    return O32_RemoveMenu(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteMenu( HMENU arg1, UINT arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2DeleteMenu\n");
#endif
    return O32_DeleteMenu(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyMenu( HMENU arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2DestroyMenu\n");
#endif
    return O32_DestroyMenu(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HiliteMenuItem( HWND arg1, HMENU arg2, UINT arg3, UINT  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2HiliteMenuItem\n");
#endif
    return O32_HiliteMenuItem(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InsertMenuA( HMENU arg1, UINT arg2, UINT arg3, UINT arg4, LPCSTR  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2InsertMenuA\n");
#endif
    return O32_InsertMenu(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InsertMenuW(HMENU arg1, UINT arg2, UINT arg3, UINT arg4, LPCWSTR arg5)
{
 BOOL  rc;
 char *astring = UnicodeToAsciiString((LPWSTR)arg5);

#ifdef DEBUG
    WriteLog("USER32:  OS2InsertMenuW %s\n", astring);
#endif
    rc = O32_InsertMenu(arg1, arg2, arg3, arg4, astring);
    FreeAsciiString(astring);
    return(rc);
}
BOOL WIN32API SetMenuContextHelpId(HMENU hmenu, DWORD dwContextHelpId)
{
#ifdef DEBUG
  WriteLog("USER32:  OS2SetMenuContextHelpId, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMenuContextHelpId(HMENU hmenu)
{
#ifdef DEBUG
  WriteLog("USER32:  OS2GetMenuContextHelpId, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CheckMenuRadioItem(HMENU hmenu, UINT idFirst, UINT idLast,
				    UINT idCheck, UINT uFlags)
{
#ifdef DEBUG
  WriteLog("USER32:  OS2CheckMenuRadioItem, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//Stolen from Wine (controls\menu.c)
//******************************************************************************
BOOL WIN32API ChangeMenuA(HMENU hMenu, UINT pos, LPCSTR data, UINT id, UINT flags)
{
#ifdef DEBUG
    WriteLog("USER32:  ChangeMenuA flags %X\n", flags);
#endif
    if (flags & MF_APPEND) return AppendMenuA(hMenu, flags & ~MF_APPEND,
                                             id, data );
    if (flags & MF_DELETE) return DeleteMenu(hMenu, pos, flags & ~MF_DELETE);
    if (flags & MF_CHANGE) return ModifyMenuA(hMenu, pos, flags & ~MF_CHANGE,
                                             id, data );
    if (flags & MF_REMOVE) return RemoveMenu(hMenu,
                                             flags & MF_BYPOSITION ? pos : id,
                                             flags & ~MF_REMOVE );
    /* Default: MF_INSERT */
    return InsertMenuA( hMenu, pos, flags, id, data );
}
//******************************************************************************
//Stolen from Wine (controls\menu.c)
//******************************************************************************
BOOL WIN32API ChangeMenuW(HMENU hMenu, UINT pos, LPCWSTR data,
                          UINT id, UINT flags )
{
#ifdef DEBUG
    WriteLog("USER32:  ChangeMenuW flags %X\n", flags);
#endif
    if (flags & MF_APPEND) return AppendMenuW(hMenu, flags & ~MF_APPEND,
                                                 id, data );
    if (flags & MF_DELETE) return DeleteMenu(hMenu, pos, flags & ~MF_DELETE);
    if (flags & MF_CHANGE) return ModifyMenuW(hMenu, pos, flags & ~MF_CHANGE,
                                                 id, data );
    if (flags & MF_REMOVE) return RemoveMenu(hMenu,
                                             flags & MF_BYPOSITION ? pos : id,
                                             flags & ~MF_REMOVE );
    /* Default: MF_INSERT */
    return InsertMenuW(hMenu, pos, flags, id, data);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMenuItemInfoA(HMENU hmenu, UINT par1, BOOL par2,
			       const MENUITEMINFOA * lpMenuItemInfo)
{
#ifdef DEBUG
  WriteLog("USER32:  SetMenuItemInfoA, faked\n");
#endif
  return(TRUE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetMenuItemInfoW
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

BOOL WIN32API SetMenuItemInfoW(HMENU            hMenu,
                               UINT             uItem,
                               BOOL             fByPosition,
                               const MENUITEMINFOW *lpmmi)
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
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMenuDefaultItem(HMENU hmenu, UINT uItem, UINT fByPos )
{
#ifdef DEBUG
  WriteLog("USER32:  SetMenuDefaultItem, faked\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMenuItemInfoA(HMENU hmenu, UINT uItem, BOOL aBool,
                               MENUITEMINFOA *lpMenuItemInfo )

{
#ifdef DEBUG
  WriteLog("USER32:  GetMenuItemInfoA, faked\n");
#endif
  return(TRUE);
}
/*****************************************************************************
 * Name      : UINT WIN32API GetMenuDefaultItem
 * Purpose   : The GetMenuDefaultItem function determines the default menu item
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

UINT WIN32API GetMenuDefaultItem(HMENU hMenu,
                                    UINT  fByPos,
                                    UINT  gmdiFlags)
{
  dprintf(("USER32:GetMenuDefaultItem (%08xh,%u,%08x) not implemented.\n",
         hMenu,
         fByPos,
         gmdiFlags));

  return (-1);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetMenuItemInfoW
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

BOOL WIN32API GetMenuItemInfoW(HMENU           hMenu,
                               UINT            uItem,
                               BOOL            fByPosition,
                               MENUITEMINFOW * lpmii)
{
  dprintf(("USER32:GetMenuItemInfoW (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

  return(GetMenuItemInfoA(hMenu,
                          uItem,
                          fByPosition,
                          (MENUITEMINFOA *)lpmii));
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetMenuItemRect
 * Purpose   : The GetMenuItemRect function retrieves the bounding rectangle
 *             for the specified menu item.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get
 *             extended error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API GetMenuItemRect(HWND   hWnd,
                                 HMENU  hMenu,
                                 UINT   uItem,
                                 LPRECT lprcItem)
{
  dprintf(("USER32:GetMenuItemRect (%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hWnd,
         hMenu,
         uItem,
         lprcItem));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API InsertMenuItemA
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

BOOL WIN32API InsertMenuItemA(HMENU           hMenu,
                              UINT            uItem,
                              BOOL            fByPosition,
                              const MENUITEMINFOA* lpmii)
{
  dprintf(("USER32:InsertMenuItemA (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

   return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API InsertMenuItemW
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

BOOL WIN32API InsertMenuItemW(HMENU           hMenu,
                              UINT            uItem,
                              BOOL            fByPosition,
                              const MENUITEMINFOW * lpmii)
{
  dprintf(("USER32:InsertMenuItemW (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

   return (FALSE);
}
/*****************************************************************************
 * Name      : UINT WIN32API MenuItemFromPoint
 * Purpose   : The MenuItemFromPoint function determines which menu item, if
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

UINT WIN32API MenuItemFromPoint(HWND  hWnd,
                                   HMENU hMenu,
                                   POINT ptScreen)
{
  dprintf(("USER32:MenuItemFromPoint (%08xh,%08xh,%u) not implemented.\n",
         hWnd,
         hMenu,
         ptScreen));

  return (-1);
}
