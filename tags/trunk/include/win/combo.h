/* $Id: combo.h,v 1.6 2001-04-26 16:59:55 sandervl Exp $ */

/*
 * Combo box definitions
 */

#ifndef __WINE_COMBO_H
#define __WINE_COMBO_H

#define LISTBOXCLASSNAME  "LISTBOX"
#define COMBOLBOXCLASSNAME "COMBOLBOX"
#define COMBOBOXCLASSNAME "COMBOBOX"
#define EDITCLASSNAME "EDIT"

#define ID_CB_LISTBOX           1000
#define ID_CB_EDIT              1001

/* Internal flags */

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

/* Combo state struct */

typedef struct
{
   HWND         hwndself;
   HWND         owner;
   UINT         dwStyle;
   HWND         hWndEdit;
   HWND         hWndLBox;
   UINT         wState;
   HFONT        hFont;
   RECT         textRect;
   RECT         buttonRect;
   RECT         droppedRect;
   INT          droppedIndex;
   INT          fixedOwnerDrawHeight;
   INT          droppedWidth;           /* last two are not used unless set */
   INT          editHeight;             /* explicitly */
} HEADCOMBO,*LPHEADCOMBO;

/*
 * Note, that CBS_DROPDOWNLIST style is actually (CBS_SIMPLE | CBS_DROPDOWN)!
 */

#define CB_GETTYPE( lphc )    ((lphc)->dwStyle & (CBS_DROPDOWNLIST))
#define CB_DISABLED( lphc )   (GetWindowLongA((lphc)->hwndself,GWL_STYLE) & WS_DISABLED)
#define CB_OWNERDRAWN( lphc ) ((lphc)->dwStyle & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE))
#define CB_HASSTRINGS( lphc ) ((lphc)->dwStyle & CBS_HASSTRINGS)
#define CB_HWND( lphc )       ((lphc)->hwndself)

BOOL 	COMBO_FlipListbox( LPHEADCOMBO, BOOL, BOOL );
LRESULT COMBO_Directory( LPHEADCOMBO, UINT, LPSTR, BOOL );

#ifdef __cplusplus
extern "C" {
#endif

BOOL LISTBOX_Register();
BOOL LISTBOX_Unregister();
BOOL COMBOLBOX_Register();
BOOL COMBOLBOX_Unregister();

BOOL COMBOBOX_Register();
BOOL COMBOBOX_Unregister();

BOOL EDIT_Register();
BOOL EDIT_Unregister();

#ifdef __cplusplus
}
#endif

#endif /* __WINE_COMBO_H */

