/* $Id: controls.cpp,v 1.9 1999-08-30 14:21:24 sandervl Exp $ */
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
#include "combo.H" //listbox,combo,edit
#include "win32wmdiclient.h"

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
  atom = GlobalFindAtomA(MDICLIENTCLASSNAME);
  if(atom) GlobalDeleteAtom(atom);
  //END to be removed code

  dprintf(("Register BUTTON class"));
  if (!BUTTON_Register()) dprintf(("failed!!!"));

  dprintf(("Register STATIC class"));
  if (!STATIC_Register()) dprintf(("failed!!!"));

  dprintf(("Register SCROLLBAR class"));
  if (!SCROLLBAR_Register()) dprintf(("failed!!!"));

  dprintf(("Register LISTBOX class"));
  if (!LISTBOX_Register()) dprintf(("failed!!!"));

  dprintf(("Register COMBOLBOX class"));
  if (!COMBOLBOX_Register()) dprintf(("failed!!!"));

  dprintf(("Register COMBOBOX class"));
  if (!COMBOBOX_Register()) dprintf(("failed!!!"));

  dprintf(("Register EDIT class"));
  if (!EDIT_Register()) dprintf(("failed!!!"));

  dprintf(("Register MDICLIENT class"));
  if (!MDICLIENT_Register()) dprintf(("failed!!!"));
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
}


