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
  if (!BUTTON_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister STATIC class"));
  if (!STATIC_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister SCROLLBAR class"));
  if (!SCROLLBAR_Unregister()) dprintf(("failed!!!"));
}

