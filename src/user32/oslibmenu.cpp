/* $Id: oslibmenu.cpp,v 1.1 1999-09-15 23:18:53 sandervl Exp $ */
/*
 * Window Menu wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#include <os2.h>
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winconst.h>
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibmenu.h"

//******************************************************************************
//******************************************************************************
HWND OSLibWinCreateMenu(HWND hwndParent, PVOID menutemplate)
{
  return WinCreateMenu(hwndParent, menutemplate);
}
//******************************************************************************
//******************************************************************************
int OSLibGetMenuItemCount(HWND hMenu)
{
   return (int)SHORT1FROMMR(WinSendMsg(hMenu, MM_QUERYITEMCOUNT, NULL, NULL));
}
//******************************************************************************
//******************************************************************************
