/* $Id: controls.h,v 1.1 2002-06-02 10:09:28 sandervl Exp $ */
/*
 * Win32 common controls
 *
 * Copyright (c) 1999 Christoph Bratschi
 *
 */

#ifndef __CONTROLS_H
#define __CONTROLS_H

#include <win\win.h>
#include <heapstring.h>

#define GetInfoPtr(hwnd) (GetWindowLongA(hwnd,0))
#define SetInfoPtr(hwnd,data) \
  SetWindowLongA(hwnd,0,data)

void CONTROLS_Register();
void CONTROLS_Unregister();

BOOL BUTTON_Register();
BOOL BUTTON_Unregister();
BOOL COMBO_Register();
BOOL COMBO_Unregister();
BOOL EDIT_Register();
BOOL EDIT_Unregister();
BOOL LISTBOX_Register();
BOOL LISTBOX_Unregister();
BOOL COMBOLBOX_Register();
BOOL COMBOLBOX_Unregister();
BOOL COMBOBOX_Register();
BOOL COMBOBOX_Unregister();
BOOL SCROLLBAR_Register();
BOOL SCROLLBAR_Unregister();
BOOL STATIC_Register();
BOOL STATIC_Unregister();

#define BUTTON_CONTROL          0
#define STATIC_CONTROL          1
#define SCROLLBAR_CONTROL       2
#define LISTBOX_CONTROL         3
#define COMBOLBOX_CONTROL       4
#define COMBOBOX_CONTROL        5
#define EDIT_CONTROL            6
#define MDICLIENT_CONTROL       7
#define DIALOG_CONTROL          8
#define DESKTOP_CONTROL         9
#define WINSWITCH_CONTROL       10
#define ICONTITLE_CONTROL       11
#define POPUPMENU_CONTROL       12
#define MAX_CONTROLS            13

/* desktop */
extern BOOL DESKTOP_SetPattern( LPCSTR pattern );

/* icon title */
extern HWND ICONTITLE_Create( HWND hwnd );

/* menu controls */
extern BOOL MENU_Init(void);
extern BOOL MENU_IsMenuActive(void);
extern HMENU MENU_GetSysMenu(HWND hWndOwner, HMENU hSysPopup);
extern UINT MENU_GetMenuBarHeight( HWND hwnd, UINT menubarWidth,
                                     INT orgX, INT orgY );
extern void MENU_TrackMouseMenuBar( HWND hwnd, INT ht, POINT pt );
extern void MENU_TrackKbdMenuBar( HWND hwnd, UINT wParam, INT vkey );
extern UINT MENU_DrawMenuBar( HDC hDC, LPRECT lprect,
                                HWND hwnd, BOOL suppress_draw );
extern UINT MENU_FindSubMenu( HMENU *hmenu, HMENU hSubTarget );

/* scrollbar */
extern void SCROLL_DrawScrollBar( HWND hwnd, HDC hdc, INT nBar, BOOL arrows, BOOL interior );
extern void SCROLL_TrackScrollBar( HWND hwnd, INT scrollbar, POINT pt );
extern INT SCROLL_SetNCSbState( HWND hwnd, int vMin, int vMax, int vPos,
                                int hMin, int hMax, int hPos );

/* combo box */

#define ID_CB_LISTBOX           1000
#define ID_CB_EDIT              1001

/* internal flags */
#define CBF_DROPPED             0x0001
#define CBF_BUTTONDOWN          0x0002
#define CBF_NOROLLUP            0x0004
#define CBF_MEASUREITEM         0x0008
#define CBF_FOCUSED             0x0010
#define CBF_CAPTURE             0x0020
#define CBF_EDIT                0x0040
#define CBF_NORESIZE            0x0080
#define CBF_NOTIFY              0x0100
#define CBF_NOREDRAW            0x0200
#define CBF_SELCHANGE           0x0400
#define CBF_NOEDITNOTIFY        0x1000
#define CBF_NOLBSELECT          0x2000  /* do not change current selection */
#define CBF_EUI                 0x8000

/* combo state struct */
typedef struct
{
   HWND           self;
   HWND           owner;
   UINT           dwStyle;
   HWND           hWndEdit;
   HWND           hWndLBox;
   UINT           wState;
   HFONT          hFont;
   RECT           textRect;
   RECT           buttonRect;
   RECT           droppedRect;
   INT            droppedIndex;
   INT            fixedOwnerDrawHeight;
   INT            droppedWidth;   /* last two are not used unless set */
   INT            editHeight;     /* explicitly */
} HEADCOMBO,*LPHEADCOMBO;

/* Note, that CBS_DROPDOWNLIST style is actually (CBS_SIMPLE | CBS_DROPDOWN) */
#define CB_GETTYPE( lphc )    ((lphc)->dwStyle & (CBS_DROPDOWNLIST))

extern BOOL COMBO_FlipListbox( LPHEADCOMBO, BOOL, BOOL );

/* Dialog info structure */
typedef struct
{
    HWND      hwndFocus;   /* Current control with focus */
    HFONT     hUserFont;   /* Dialog font */
    HMENU     hMenu;       /* Dialog menu */
    UINT      xBaseUnit;   /* Dialog units (depends on the font) */
    UINT      yBaseUnit;
    INT       idResult;    /* EndDialog() result / default pushbutton ID */
    UINT      flags;       /* EndDialog() called for this dialog */
    HGLOBAL16 hDialogHeap;
} DIALOGINFO;

#define DF_END  0x0001
#define DF_OWNERENABLED 0x0002

/* offset of DIALOGINFO ptr in dialog extra bytes */
#define DWL_WINE_DIALOGINFO (DWL_USER+sizeof(ULONG_PTR))

#define DIALOG_get_info( hwnd ) \
    (DIALOGINFO *)GetWindowLongW( hwnd, DWL_WINE_DIALOGINFO )

extern BOOL DIALOG_Init(void);

/* Wine look */

typedef enum
{
    WIN31_LOOK,
    WIN95_LOOK,
    WIN98_LOOK
} WINE_LOOK;

extern WINE_LOOK TWEAK_WineLook;

#define WND_OTHER_PROCESS 	0

HWND WIN_GetFullHandle( HWND hwnd );
int  WIN_SuspendWndsLock( void );
void WIN_RestoreWndsLock(int ipreviousLock);
HWND *WIN_ListParents( HWND hwnd );
HWND *WIN_ListChildren( HWND hwnd );
WND  *WIN_GetPtr( HWND hwnd );
WND*  WIN_FindWndPtr( HWND hwnd );
void  WIN_ReleaseWndPtr(WND *wndPtr);
void  WIN_ReleasePtr( WND *ptr );
LONG  WIN_SetStyle( HWND hwnd, LONG style );
LONG  WIN_SetExStyle( HWND hwnd, LONG style );
void  WIN_SetOwner( HWND hwnd, HWND owner );


/* syscolor.c */
extern HPEN SYSCOLOR_GetPen( INT index );

extern const char *SPY_GetMsgName( UINT msg, HWND hWnd );

/* GetAncestor() constants */
#define GA_PARENT       1
#define GA_ROOT         2
#define GA_ROOTOWNER    3

HWND        WINAPI GetAncestor(HWND,UINT);


HMENU WIN32API getSysMenu(HWND hwnd);
void GetWindowRectParent(HWND hwnd, RECT *pRect);
VOID setSysMenu(HWND hwnd,HMENU hMenu);


extern LONG NC_HandleNCHitTest( HWND hwnd, POINT pt );


#endif // __CONTROLS_H
