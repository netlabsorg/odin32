/* $Id: button.h,v 1.7 2003-10-06 10:00:33 sandervl Exp $ */

/*
 * Button-class extra info
 *
 * Copyright 1994 Alexandre Julliard
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_BUTTON_H
#define __WINE_BUTTON_H

#include "wingdi.h"

#define BUTTONCLASSNAME "Button"

  /* Extra info for BUTTON windows */
  /* Note: under MS-Windows, state is a BYTE and this structure is */
  /* only 3 bytes long. I don't think there are programs out there */
  /* broken enough to rely on this :-) */
typedef struct
{
    WORD     state;   /* Current state */
    HFONT    hFont;   /* Button font (or 0 for system font) */
    HANDLE   hImage;  /* Handle to the image or the icon */
} BUTTONINFO;

  /* Button state values */
#define BUTTON_UNCHECKED       0x00
#define BUTTON_CHECKED         0x01
#define BUTTON_3STATE          0x02
#define BUTTON_HIGHLIGHTED     0x04
#define BUTTON_HASFOCUS        0x08
#define BUTTON_NSTATES         0x0F
  /* undocumented flags */
#define BUTTON_BTNPRESSED      0x40
#define BUTTON_UNKNOWN2        0x20
#define BUTTON_UNKNOWN3        0x10

//#define BUTTON_STATE(hwnd)     ((WIN_FindWndPtr(hwnd))->wExtra[0])

//extern LRESULT WINAPI ButtonWndProc( HWND hWnd, UINT uMsg,
//                                     WPARAM wParam, LPARAM lParam );

BOOL BUTTON_Register();
BOOL BUTTON_Unregister();

#endif  /* __WINE_BUTTON_H */

