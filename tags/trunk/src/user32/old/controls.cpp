/* $Id: controls.cpp,v 1.1 2001-05-11 08:38:36 sandervl Exp $ */
/* File: controls.cpp -- Win32 common controls
 *
 * Copyright (c) 1999 Christoph Bratschi
 *
 */

#include <os2win.h>
#include <string.h>
#include "misc.h"

#include "button.h"
#include "static.h"
#include "scroll.h"
#include "combo.h" //listbox,combo,edit
#include "win32wmdiclient.h"
#include "win32dlg.h"
#include "win32wdesktop.h"
#include "winswitch.h"
#include "icontitle.h"
#include "menu.h"
#include "controls.h"

#define DBG_LOCALLOG	DBG_controls
#include "dbglocal.h"

ATOM  controlAtoms[MAX_CONTROLS] = {0};

/* registration */

void CONTROLS_Register()
{
  dprintf(("Register DESKTOP class"));
  controlAtoms[DESKTOP_CONTROL] = DESKTOP_Register();
  if (!controlAtoms[DESKTOP_CONTROL]) dprintf(("failed!!!"));

  //SvL: Create Desktop Window
  CreateWin32Desktop();

  dprintf(("Register BUTTON class"));
  controlAtoms[BUTTON_CONTROL] = BUTTON_Register();
  if (!controlAtoms[BUTTON_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register STATIC class"));
  controlAtoms[STATIC_CONTROL] = STATIC_Register();
  if (!controlAtoms[STATIC_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register SCROLLBAR class"));
  controlAtoms[SCROLLBAR_CONTROL] = SCROLLBAR_Register();
  if (!controlAtoms[SCROLLBAR_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register LISTBOX class"));
  controlAtoms[LISTBOX_CONTROL] = LISTBOX_Register();
  if (!controlAtoms[LISTBOX_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register COMBOLBOX class"));
  controlAtoms[COMBOLBOX_CONTROL] = COMBOLBOX_Register();
  if (!controlAtoms[COMBOLBOX_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register COMBOBOX class"));
  controlAtoms[COMBOBOX_CONTROL] = COMBOBOX_Register();
  if (!controlAtoms[COMBOBOX_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register EDIT class"));
  controlAtoms[EDIT_CONTROL] = EDIT_Register();
  if (!controlAtoms[EDIT_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register MDICLIENT class"));
  controlAtoms[MDICLIENT_CONTROL] = MDICLIENT_Register();
  if (!controlAtoms[MDICLIENT_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register DIALOG class"));
  controlAtoms[DIALOG_CONTROL] = DIALOG_Register();
  if (!controlAtoms[DIALOG_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register WINSWITCH class"));
  controlAtoms[WINSWITCH_CONTROL] = WINSWITCH_Register();
  if (!controlAtoms[WINSWITCH_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register ICONTITLE class"));
  controlAtoms[ICONTITLE_CONTROL] = ICONTITLE_Register();
  if (!controlAtoms[ICONTITLE_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register POPUPMENU class"));
  controlAtoms[POPUPMENU_CONTROL] = POPUPMENU_Register();
  if (!controlAtoms[POPUPMENU_CONTROL]) dprintf(("failed!!!"));
}

void CONTROLS_Unregister()
{
  dprintf(("Unregister BUTTON class"));
  if (!BUTTON_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister STATIC class"));
  if (!STATIC_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister SCROLLBAR class"));
  if (!SCROLLBAR_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister LISTBOX class"));
  if (!LISTBOX_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister COMBOLBOX class"));
  if (!COMBOLBOX_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister COMBOBOX class"));
  if (!COMBOBOX_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister EDIT class"));
  if (!EDIT_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister MDICLIENT class"));
  if (!MDICLIENT_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister DIALOG class"));
  if (!DIALOG_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister DESKTOP class"));
  if (!DESKTOP_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister WINSWITCH class"));
  if (!WINSWITCH_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister ICONTITLE class"));
  if (!ICONTITLE_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister POPUPMENU class"));
  if (!POPUPMENU_Unregister()) dprintf(("failed!!!"));

}


BOOL CONTROLS_IsControl(Win32BaseWindow *window, int control)
{
  if(control >= MAX_CONTROLS || window == NULL || window->getClass() == NULL)
        return FALSE;

  return controlAtoms[control] == window->getClass()->getAtom();
}

