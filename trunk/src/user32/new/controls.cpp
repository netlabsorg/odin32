/* File: controls.cpp -- Win32 common controls
 *
 * Copyright (c) 1999 Christoph Bratschi
 *
 */

#include "controls.h"
#include "button.h"

void CONTROLS_Register()
{
  BUTTON_Register;
}

void CONTROLS_Unregister()
{
  BUTTON_Unregister;
}
