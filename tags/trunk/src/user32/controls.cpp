/* $Id: controls.cpp,v 1.12 2003-01-03 16:35:51 sandervl Exp $ */
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
#include "win32wmdiclient.h"
#include "win32dlg.h"
#include "win32wdesktop.h"
#include "win32class.h"
#include "winswitch.h"
#include "icontitle.h"
#include "menu.h"
#include "controls.h"
#include "ctrlconf.h"

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
  controlAtoms[SCROLLBAR_CONTROL] = 
#if 0
          InternalRegisterClass((LPSTR)SCROLL_builtin_class.name,
                                SCROLL_builtin_class.style,
                                SCROLL_builtin_class.procA,
                                SCROLL_builtin_class.procW,
                                SCROLL_builtin_class.extra,
                                SCROLL_builtin_class.cursor,
                                SCROLL_builtin_class.brush);
#else
SCROLLBAR_Register();
#endif
  if (!controlAtoms[SCROLLBAR_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register LISTBOX class"));
  controlAtoms[LISTBOX_CONTROL] = 
          InternalRegisterClass((LPSTR)LISTBOX_builtin_class.name,
                                LISTBOX_builtin_class.style,
                                LISTBOX_builtin_class.procA,
                                LISTBOX_builtin_class.procW,
                                LISTBOX_builtin_class.extra,
                                LISTBOX_builtin_class.cursor,
                                LISTBOX_builtin_class.brush);

  if (!controlAtoms[LISTBOX_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register COMBOLBOX class"));
  controlAtoms[COMBOLBOX_CONTROL] = 
          InternalRegisterClass((LPSTR)COMBOLBOX_builtin_class.name,
                                COMBOLBOX_builtin_class.style,
                                COMBOLBOX_builtin_class.procA,
                                COMBOLBOX_builtin_class.procW,
                                COMBOLBOX_builtin_class.extra,
                                COMBOLBOX_builtin_class.cursor,
                                COMBOLBOX_builtin_class.brush);
  if (!controlAtoms[COMBOLBOX_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register COMBOBOX class"));
  controlAtoms[COMBOBOX_CONTROL] = 
          InternalRegisterClass((LPSTR)COMBO_builtin_class.name,
                                COMBO_builtin_class.style,
                                COMBO_builtin_class.procA,
                                COMBO_builtin_class.procW,
                                COMBO_builtin_class.extra,
                                COMBO_builtin_class.cursor,
                                COMBO_builtin_class.brush);
  if (!controlAtoms[COMBOBOX_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register EDIT class"));
  controlAtoms[EDIT_CONTROL] = 
          InternalRegisterClass((LPSTR)EDIT_builtin_class.name,
                                EDIT_builtin_class.style,
                                EDIT_builtin_class.procA,
                                EDIT_builtin_class.procW,
                                EDIT_builtin_class.extra,
                                EDIT_builtin_class.cursor,
                                EDIT_builtin_class.brush);
  if (!controlAtoms[EDIT_CONTROL]) dprintf(("failed!!!"));

  dprintf(("Register MDICLIENT class"));
  controlAtoms[MDICLIENT_CONTROL] = 
          InternalRegisterClass((LPSTR)MDICLIENT_builtin_class.name,
                                MDICLIENT_builtin_class.style,
                                MDICLIENT_builtin_class.procA,
                                MDICLIENT_builtin_class.procW,
                                MDICLIENT_builtin_class.extra,
                                MDICLIENT_builtin_class.cursor,
//Default brush is ugly light gray; use this one instead
                                GetStockObject(GRAY_BRUSH));
//                                MDICLIENT_builtin_class.brush);
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
  controlAtoms[POPUPMENU_CONTROL] = 
          InternalRegisterClass((LPSTR)MENU_builtin_class.name,
                                MENU_builtin_class.style,
                                MENU_builtin_class.procA,
                                MENU_builtin_class.procW,
                                MENU_builtin_class.extra,
                                MENU_builtin_class.cursor,
                                MENU_builtin_class.brush);
  if (!controlAtoms[POPUPMENU_CONTROL]) dprintf(("failed!!!"));
  MENU_Init();
}

void CONTROLS_Unregister()
{
  dprintf(("Unregister BUTTON class"));
  if (!BUTTON_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister STATIC class"));
  if (!STATIC_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister SCROLLBAR class"));
#if 0
  if (!UnregisterClassA((LPSTR)SCROLL_builtin_class.name, NULL)) dprintf(("failed!!!"));
#else
  if (!SCROLLBAR_Unregister()) dprintf(("failed!!!"));
#endif

  dprintf(("Unregister LISTBOX class"));
  if (!UnregisterClassA((LPSTR)LISTBOX_builtin_class.name, NULL)) dprintf(("failed!!!"));

  dprintf(("Unregister COMBOLBOX class"));
  if (!UnregisterClassA((LPSTR)COMBOLBOX_builtin_class.name, NULL)) dprintf(("failed!!!"));

  dprintf(("Unregister COMBOBOX class"));
  if (!UnregisterClassA((LPSTR)COMBO_builtin_class.name, NULL)) dprintf(("failed!!!"));

  dprintf(("Unregister EDIT class"));
  if (!UnregisterClassA((LPSTR)EDIT_builtin_class.name, NULL)) dprintf(("failed!!!"));

  dprintf(("Unregister MDICLIENT class"));
  if (!UnregisterClassA((LPSTR)MDICLIENT_builtin_class.name, NULL)) dprintf(("failed!!!"));

  dprintf(("Unregister DIALOG class"));
  if (!DIALOG_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister DESKTOP class"));
  if (!DESKTOP_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister WINSWITCH class"));
  if (!WINSWITCH_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister ICONTITLE class"));
  if (!ICONTITLE_Unregister()) dprintf(("failed!!!"));

  dprintf(("Unregister POPUPMENU class"));
  if (!UnregisterClassA((LPSTR)MENU_builtin_class.name, NULL)) dprintf(("failed!!!"));
}


BOOL CONTROLS_IsControl(Win32BaseWindow *window, int control)
{
  if(control >= MAX_CONTROLS || window == NULL || window->getClass() == NULL)
        return FALSE;

  return controlAtoms[control] == window->getClass()->getAtom();
}

