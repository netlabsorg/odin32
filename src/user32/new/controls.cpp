/* File: controls.cpp -- Win32 common controls
 *
 * Copyright (c) 1999 Christoph Bratschi
 *
 */

#include <os2win.h>
#include <string.h>
#include "misc.h"

#include "controls.h"
#include "button.h"

/* registration */

void CONTROLS_Register()
{
  dprintf(("Register BUTTON class"));
  BUTTON_Register() ? dprintf(("done")):dprintf(("failed!!!"));
}

void CONTROLS_Unregister()
{
  dprintf(("Unregister BUTTON class"));
  BUTTON_Unregister();
}

/* Win32 <-> internal control name */

char* Win32ToOdinControlName(char* name)
{
  if (!name) return NULL;

  if (stricmp(name,WIN32BUTTONCLASSNAME))
    return ODINBUTTONCLASSNAME;

  return name; //original name
}

char* OdinToWin32ControlName(char* name)
{
  if (!name) return NULL;

  if (stricmp(name,ODINBUTTONCLASSNAME))
    return WIN32BUTTONCLASSNAME;

  return name; //original name
}
