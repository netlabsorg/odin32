/* $Id: unknown.cpp,v 1.5 2001-05-11 08:38:40 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 USER32 Subsystem for OS/2
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>

#define DBG_LOCALLOG	DBG_unknown
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

static struct _Locals
{
  HWND hGlobalProgmanWindow;
  HWND hGlobalShellWindow;
  HWND hGlobalTaskmanWindow;
} Locals;


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/


/*****************************************************************************
 * Name      : IsHungAppWindow
 * Purpose   : UNKNOWN
 * Parameters: UNKNOWN
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

BOOL WIN32API IsHungAppWindow(HWND  hwnd,
                              ULONG ulDummy)
{
  dprintf (("USER32: IsHungAppWindow(%08xh,%08xh) not implemented.\n",
            hwnd,
            ulDummy));

  return(FALSE);
}


/*****************************************************************************
 * Name      : SetProgmanWindow
 * Purpose   : UNKNOWN
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : USER32.522
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HRESULT WIN32API SetProgmanWindow ( HWND hwnd )
{
  dprintf(("USER32: SetProgmanWindow(%08xh) not implemented.\n",
           hwnd));

   Locals.hGlobalProgmanWindow = hwnd;
   return Locals.hGlobalProgmanWindow;
}


/*****************************************************************************
 * Name      : GetProgmanWindow
 * Purpose   : UNKNOWN
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : USER32.289
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HRESULT WIN32API GetProgmanWindow ( )
{
  dprintf(("USER32: GetProgmanWindow not implemented.\n"));

  return Locals.hGlobalProgmanWindow;
}


/*****************************************************************************
 * Name      : SetShellWindowEx
 * Purpose   : UNKNOWN
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : USER32.531
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *
 * hwndProgman =  Progman[Program Manager]
 *                |-> SHELLDLL_DefView
 * hwndListView = |   |-> SysListView32
 *                |   |   |-> tooltips_class32
 *                |   |
 *                |   |-> SysHeader32
 *                |
 *                |-> ProxyTarget
 */

HRESULT WIN32API SetShellWindowEx (HWND hwndProgman,
                                   HWND hwndListView)
{
  dprintf(("USER32: SetShellWindowEx(%08xh,%08xh) not implemented.\n",
           hwndProgman,
           hwndListView));

  Locals.hGlobalShellWindow = hwndProgman;
  return Locals.hGlobalShellWindow;
}


/*****************************************************************************
 * Name      : SetTaskmanWindow
 * Purpose   : UNKNOWN
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : USER32.537
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *
 * NOTES
 *   hwnd = MSTaskSwWClass
 *          |-> SysTabControl32
 */

HRESULT WIN32API SetTaskmanWindow ( HWND hwnd )
{
  dprintf(("USER32: SetTaskmanWindow(%08xh) not implemented.\n",
           hwnd));

  Locals.hGlobalTaskmanWindow = hwnd;
  return Locals.hGlobalTaskmanWindow;
}

/*****************************************************************************
 * Name      : GetTaskmanWindow
 * Purpose   : UNKNOWN
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : USER32.304
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HRESULT WIN32API GetTaskmanWindow ( )
{
  dprintf(("USER32: GetTaskmanWindow() not implemented.\n"));

  return Locals.hGlobalTaskmanWindow;
}



