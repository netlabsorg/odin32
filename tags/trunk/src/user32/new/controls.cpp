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
#include "static.h"
#include "scroll.h"

/* registration */

void CONTROLS_Register()
{
  dprintf(("Register BUTTON class"));
  if (!BUTTON_Register()) dprintf(("failed!!!"));

  dprintf(("Register STATIC class"));
  if (!STATIC_Register()) dprintf(("failed!!!"));

  dprintf(("Register SCROLLBAR class"));
  if (!SCROLLBAR_Register()) dprintf(("failed!!!"));
}

void CONTROLS_Unregister()
{
  dprintf(("Unregister BUTTON class"));
  BUTTON_Unregister();

  dprintf(("Unregister STATIC class"));
  STATIC_Unregister();

  dprintf(("Unregister SCROLLBAR class"));
  SCROLLBAR_Unregister();
}

/* Win32 <-> internal control name */

char* Win32ToOdinControlName(char* name)
{
  if (!name) return NULL;

  if (stricmp(name,WIN32BUTTONCLASSNAME))
    return ODINBUTTONCLASSNAME;

  if (stricmp(name,WIN32STATICCLASSNAME))
    return ODINSTATICCLASSNAME;

  return name; //original name
}

char* OdinToWin32ControlName(char* name)
{
  if (!name) return NULL;

  if (stricmp(name,ODINBUTTONCLASSNAME))
    return WIN32BUTTONCLASSNAME;

  if (stricmp(name,ODINSTATICCLASSNAME))
    return WIN32STATICCLASSNAME;

  return name; //original name
}
