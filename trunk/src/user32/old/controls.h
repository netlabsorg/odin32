/* $Id: controls.h,v 1.1 2001-05-11 08:35:17 sandervl Exp $ */
/*
 * Win32 common controls
 *
 * Copyright (c) 1999 Christoph Bratschi
 *
 */

#ifndef __CONTROLS_H
#define __CONTROLS_H

#ifdef __cplusplus
#include "win32wbase.h"
#endif

#define GetInfoPtr(hwnd) (GetWindowLongA(hwnd,0))
#define SetInfoPtr(hwnd,data) \
  SetWindowLongA(hwnd,0,data)

void CONTROLS_Register();
void CONTROLS_Unregister();

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

#ifdef __cplusplus
BOOL CONTROLS_IsControl(Win32BaseWindow *window, int control);
#endif

/* combo box */

#define ID_CB_LISTBOX           1000
#define ID_CB_EDIT              1001

#endif // __CONTROLS_H
