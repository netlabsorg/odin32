/*
 * Menu definitions
 */

#ifndef __WINE_MENU_H
#define __WINE_MENU_H

#include "win.h"

#define POPUPMENUCLASSNAME "#32768"

LRESULT WINAPI PopupMenuWndProc( HWND hwnd, UINT message, WPARAM wParam,
                                 LPARAM lParam );

extern BOOL MENU_Init(void);
extern HMENU MENU_GetSysMenu(HWND hWndOwner, HMENU hSysPopup);
extern UINT MENU_GetMenuBarHeight( HWND hwnd, UINT menubarWidth,
                                     INT orgX, INT orgY );
extern BOOL MENU_PatchResidentPopup( HQUEUE, HWND );
extern void MENU_TrackMouseMenuBar( HWND hwnd, INT ht, POINT pt );
extern void MENU_TrackKbdMenuBar( HWND hwnd, UINT wParam, INT vkey );
extern UINT MENU_DrawMenuBar( HDC hDC, LPRECT lprect,
                                HWND hwnd, BOOL suppress_draw );
extern UINT MENU_FindSubMenu( HMENU *hmenu, HMENU hSubTarget );

BOOL POPUPMENU_Register();
BOOL POPUPMENU_Unregister();

#endif /* __WINE_MENU_H */
