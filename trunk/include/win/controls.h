/* $Id: controls.h,v 1.7 1999-10-19 19:25:12 sandervl Exp $ */
/*
 * Win32 common controls
 *
 * Copyright (c) 1999 Christoph Bratschi
 *
 */

#ifndef __CONTROLS_H
#define __CONTROLS_H

#include "wingdi.h"

#define GetInfoPtr(hwnd) (GetWindowLongA(hwnd,0))
#define SetInfoPtr(hwnd,data) \
  SetWindowLongA(hwnd,0,data)

void CONTROLS_Register();
void CONTROLS_Unregister();

#define BUTTON_CONTROL  	0
#define STATIC_CONTROL  	1
#define SCROLLBAR_CONTROL	2
#define LISTBOX_CONTROL		3
#define COMBOLBOX_CONTROL	4
#define COMBOBOX_CONTROL	5
#define EDIT_CONTROL		6
#define MDICLIENT_CONTROL	7
#define DIALOG_CONTROL		8
#define DESKTOP_CONTROL 	9
#define MAX_CONTROLS 		10

BOOL WIDGETS_IsControl(Win32BaseWindow *window, int control);


#endif // __CONTROLS_H
