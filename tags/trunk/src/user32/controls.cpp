/* $Id: controls.cpp,v 1.4 1999-10-19 19:26:07 sandervl Exp $ */
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
#include "controls.h"

ATOM controlAtoms[MAX_CONTROLS] = {0};

/* registration */

void CONTROLS_Register()
{
  //TODO: Remove this once we replace the atom code in kernel32
  //Delete all Open32 global class atoms and recreate them
  ATOM atom;

  atom = GlobalFindAtomA(BUTTONCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  atom = GlobalFindAtomA(STATICCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  atom = GlobalFindAtomA(SCROLLBARCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  atom = GlobalFindAtomA(LISTBOXCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  atom = GlobalFindAtomA(COMBOLBOXCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  atom = GlobalFindAtomA(COMBOBOXCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  atom = GlobalFindAtomA(EDITCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  atom = GlobalFindAtomA(MDICLIENTCLASSNAMEA);
  if(atom) GlobalDeleteAtom(atom);
  //END to be removed code

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

  dprintf(("Register DESKTOP class"));
  controlAtoms[DESKTOP_CONTROL] = DESKTOP_Register();
  if (!controlAtoms[DESKTOP_CONTROL]) dprintf(("failed!!!")); 
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
}


BOOL WIDGETS_IsControl(Win32BaseWindow *window, int control)
{
  if(control >= MAX_CONTROLS || window == NULL || window->getClass() == NULL)
	return FALSE;

  return controlAtoms[control] == window->getClass()->getAtom();
}

