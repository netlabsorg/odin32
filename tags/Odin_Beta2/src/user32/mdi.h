/* $Id: mdi.h,v 1.1 2001-09-19 15:39:48 sandervl Exp $ */

/* MDI.H
 *
 * Copyright 1994, Bob Amstadt
 *           1995  Alex Korobka
 *
 * MDI structure definitions.
 */

#ifndef __WINE_MDI_H
#define __WINE_MDI_H

#include "windef.h"

#define MDICLIENTCLASSNAMEA "MDICLIENT"
#define MDICLIENTCLASSNAMEW L"MDICLIENT"

#define MDI_MAXLISTLENGTH	0x40
#define MDI_MAXTITLELENGTH	0xA1

#define MDI_NOFRAMEREPAINT	0
#define MDI_REPAINTFRAMENOW	1
#define MDI_REPAINTFRAME	2

#define WM_MDICALCCHILDSCROLL   0x10AC /* this is exactly what Windows uses */

/* "More Windows..." definitions */
#define MDI_MOREWINDOWSLIMIT    9       /* after this number of windows, a "More Windows..." 
                                           option will appear under the Windows menu */
#define MDI_IDC_LISTBOX         100
#define MDI_IDS_MOREWINDOWS     13

#define MDIF_NEEDUPDATE		0x0001

LRESULT WINAPI MDIClientWndProcA( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT WINAPI MDIClientWndProcW( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

typedef struct
{
    UINT      nActiveChildren;
    HWND      hwndChildMaximized;
    HWND      hwndActiveChild;
    HMENU     hWindowMenu;
    UINT      idFirstChild;
    LPWSTR    frameTitle;
    UINT      nTotalCreated;
    UINT      mdiFlags;
    UINT      sbRecalc;   /* SB_xxx flags for scrollbar fixup */
} MDICLIENTINFO;


#ifdef __cplusplus
extern "C" {
#endif

INT  SCROLL_SetNCSbState(HWND hwnd, int vMin, int vMax, int vPos,
                         int hMin, int hMax, int hPos);

#ifdef __cplusplus
}
#endif

#endif /* __WINE_MDI_H */

