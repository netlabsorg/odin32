/* $Id: winevent.cpp,v 1.1 2001-08-31 20:31:03 phaller Exp $ */

/*
 * Win32 windows special event handling
 *
 * Copyright 2001 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

//Attention: many functions belong to other subsystems, move them to their
//           right place!

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <misc.h>
#include <winuser32.h>

#include "user32.h"
#include <winicon.h>
#include "syscolor.h"
#include "pmwindow.h"
#include "oslibgdi.h"
#include "oslibwin.h"
#include "oslibprf.h"

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
//#include <oslibwin.h>
#include <win32wnd.h>
#include <winuser.h>
#include "initterm.h"

#define DBG_LOCALLOG    DBG_user32
#include "dbglocal.h"

ODINDEBUGCHANNEL(USER32-WINEVENT)


/*****************************************************************************
 * Name      : NotifyWinEvent
 * Purpose   : The NotifyWinEvent function signalsthe system that a specific
 *             predefined event occured. If any client applications have
 *             registered a hook function for the event, the system calls
 *             the client's hook function
 * Parameters: DWORD event
 *             HWND  hwnd
 *             LONG  idObjectType
 *             LONG  idObject
 * Variables :
 * Result    : void
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [2001-08-31]
 *****************************************************************************/

ODINPROCEDURE4(NotifyWinEvent,
               DWORD, event,
               HWND,  hwnd,
               LONG,  idObjectType,
               LONG,  idObject)
{
  /*
  dprintf(("USER32::NotifyWinEvent not implemented"));
  
  switch(event)
  {
    // case EVENT_SYSTEM_xxx:
    // @@@PH call the WinEventProc
    // break;
    }
    */
}

DWORD WIN32API UnhookWinEvent(DWORD arg1)
{
  dprintf(("USER32: UnhookWinEvent %x - empty stub!!", arg1));

  return 0;
}

DWORD WIN32API SetWinEventHook(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7)
{
  dprintf(("USER32: SetWinEventHook %x %x %x %x %x %x %x - empty stub!!", arg1, arg2, arg3, arg4, arg5, arg6, arg7));

  return 0;
}
