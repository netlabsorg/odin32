/* $Id: drag.cpp,v 1.1 1999-10-09 11:11:30 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 * Copyright 1989 Patrick Haller
 * Project Odin Software License can be found in LICENSE.TXT
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <shellapi.h>
#include <winreg.h>
#include "shell32.h"

#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <unicode.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/


/*****************************************************************************
 * Name      : VOID DragAcceptFiles
 * Purpose   :
 * Parameters: HWND hwnd
 *             BOOL fAccept
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:00]
 *****************************************************************************/

VOID WIN32API DragAcceptFiles(HWND hwnd,
                              BOOL fAccept)
{
  dprintf(("SHELL32: DragAcceptFiles (%08uxh, %08xh) not implemented.\n",
           hwnd,
           fAccept));
}


/*****************************************************************************
 * Name      : VOID DragFinish
 * Purpose   :
 * Parameters: HDROP hDrop
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:00]
 *****************************************************************************/

VOID WIN32API DragFinish(HDROP hDrop)
{
  dprintf(("SHELL32: DragFinish(%08xh) not implemented.\n",
           hDrop));
}


/*****************************************************************************
 * Name      : UINT DragQueryFileA
 * Purpose   :
 * Parameters: HDROP  hDrop    - drop structure handle
 *             UINT   iFile    - index of file to query
 *             LPTSTR lpszFile - target buffer
 *             UINT   cch      - target buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:00]
 *****************************************************************************/

UINT WIN32API DragQueryFileA(HDROP  hDrop,
                             UINT   iFile,
                             LPTSTR lpszFile,
                             UINT   cch)
{
  dprintf(("SHELL32: DragQueryFileA(%08xh,%08xh,%s,%08xh) not implemented.\n",
           hDrop,
           iFile,
           lpszFile,
           cch));

  return(0);
}


/*****************************************************************************
 * Name      : UINT DragQueryFileAorW
 * Purpose   :
 * Parameters: HDROP  hDrop    - drop structure handle
 *             UINT   iFile    - index of file to query
 *             LPTSTR lpszFile - target buffer
 *             UINT   cch      - target buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:00]
 *****************************************************************************/

UINT WIN32API DragQueryFileAorW(HDROP  hDrop,
                                UINT   iFile,
                                LPTSTR lpszFile,
                                UINT   cch)
{
  dprintf(("SHELL32: DragQueryFileAorW(%08xh,%08xh,%s,%08xh) not implemented.\n",
           hDrop,
           iFile,
           lpszFile,
           cch));

  return(0);
}


/*****************************************************************************
 * Name      : UINT DragQueryFileW
 * Purpose   :
 * Parameters: HDROP  hDrop    - drop structure handle
 *             UINT   iFile    - index of file to query
 *             LPWSTR lpszFile - target buffer
 *             UINT   cch      - target buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:00]
 *****************************************************************************/

UINT WIN32API DragQueryFileW(HDROP  hDrop,
                                UINT   iFile,
                                LPWSTR lpszFile,
                                UINT   cch)
{
  dprintf(("SHELL32: DragQueryFileW(%08xh,%08xh,%s,%08xh) not implemented.\n",
           hDrop,
           iFile,
           lpszFile,
           cch));

  return(0);
}


/*****************************************************************************
 * Name      : BOOL DrawQueryPoint
 * Purpose   :
 * Parameters: HDROP   hDrop
 *             LPPOINT lppt
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:00]
 *****************************************************************************/

BOOL WIN32API DragQueryPoint(HDROP   hDrop,
                             LPPOINT lppt)
{
  dprintf(("SHELL32: DragQueryPoint (%08xh,%08xh) not implemented.\n",
           hDrop,
           lppt));

  return(FALSE);
}

