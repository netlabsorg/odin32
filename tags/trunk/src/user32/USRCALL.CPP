/* $Id: USRCALL.CPP,v 1.1 1999-05-24 20:20:00 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 callback functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include "misc.h"
#include "usrcall.h"

static BOOL EXPENTRY_O32 EnumWndProc(HWND hwnd, LPARAM lParam);

//******************************************************************************
//******************************************************************************
EnumWindowCallback::EnumWindowCallback(WNDENUMPROC pUserCallback, LPARAM lpData)
{
  lpUserData = lpData;
  pCallback  = (WNDENUMPROC)pUserCallback;
}
//******************************************************************************
//******************************************************************************
EnumWindowCallback::~EnumWindowCallback()
{

}
//******************************************************************************
//******************************************************************************
WNDENUMPROC_O32 EnumWindowCallback::GetOS2Callback()
{
  return EnumWndProc;
}
//******************************************************************************
//******************************************************************************
static BOOL EXPENTRY_O32 EnumWndProc(HWND hwnd, LPARAM lpParam)
{
 EnumWindowCallback *me = (EnumWindowCallback *)lpParam;

  dprintf(("EnumWndProc hwnd %X\n", hwnd));
  return(me->pCallback(hwnd, me->lpUserData));
}
//******************************************************************************
//******************************************************************************
