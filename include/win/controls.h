/* $Id */
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

#endif // __CONTROLS_H
