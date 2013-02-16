/*
 * Menu definitions
 */

#ifndef __WINE_MENU_H
#define __WINE_MENU_H

#define POPUPMENUCLASSNAME "#32768"

#ifdef __cplusplus
extern "C" {
#endif

HMENU MENU_GetSysMenu(HWND hWnd,HMENU hPopupMenu);
UINT MENU_GetMenuBarHeight(HWND hWnd,UINT menubarWidth,INT orgX,INT orgY);
void MENU_TrackMouseMenuBar(HWND hWnd,INT ht,POINT pt);
void MENU_TrackMouseMenuBar_MouseMove(HWND hwnd,POINT pt,BOOL OnMenu);
void MENU_TrackKbdMenuBar(HWND hWnd,UINT wParam,INT vkey);
UINT MENU_DrawMenuBar(HDC hDC,LPRECT lprect,HWND hWnd,BOOL suppress_draw);

BOOL MENU_Init();
BOOL POPUPMENU_Register();
BOOL POPUPMENU_Unregister();
BOOL IsMenuActive(void);

#ifdef __cplusplus
}
#endif

#endif /* __WINE_MENU_H */
