/* $Id: winmenudef.h,v 1.1 2001-05-11 08:35:22 sandervl Exp $ */

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
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __WINMENUDEF_H__
#define __WINMENUDEF_H__

/* Menu item structure */
typedef struct {
    /* ----------- MENUITEMINFO Stuff ----------- */
    UINT fType;			/* Item type. */
    UINT fState;		/* Item state.  */
    UINT wID;			/* Item id.  */
    HMENU hSubMenu;		/* Pop-up menu.  */
    HBITMAP hCheckBit;		/* Bitmap when checked.  */
    HBITMAP hUnCheckBit;	/* Bitmap when unchecked.  */
    LPSTR text;			/* Item text or bitmap handle.  */
    DWORD dwItemData;		/* Application defined.  */
    DWORD dwTypeData;		/* depends on fMask */
    HBITMAP hbmpItem;		/* bitmap in win98 style menus */
    /* ----------- Wine stuff ----------- */
    RECT      rect;		/* Item area (relative to menu window) */
    UINT      xTab;		/* X position of text after Tab */
} MENUITEM;

/* Popup menu structure */
typedef struct {
    WORD        wFlags;       /* Menu flags (MF_POPUP, MF_SYSMENU) */
    WORD        wMagic;       /* Magic number */
    HQUEUE16    hTaskQ;       /* Task queue for this menu */
    WORD	Width;        /* Width of the whole menu */
    WORD	Height;       /* Height of the whole menu */
    WORD	nItems;       /* Number of items in the menu */
    HWND        hWnd;         /* Window containing the menu */
    MENUITEM    *items;       /* Array of menu items */
    UINT        FocusedItem;  /* Currently focused item */
    HWND	hwndOwner;    /* window receiving the messages for ownerdraw */
    /* ------------ MENUINFO members ------ */
    DWORD	dwStyle;	/* Extended mennu style */
    UINT	cyMax;		/* max hight of the whole menu, 0 is screen hight */
    HBRUSH	hbrBack;	/* brush for menu background */
    DWORD	dwContextHelpID;
    DWORD	dwMenuData;	/* application defined value */
    HMENU       hSysMenuOwner;  /* Handle to the dummy sys menu holder */
} POPUPMENU, *LPPOPUPMENU;

#endif //__WINMENUDEF_H__