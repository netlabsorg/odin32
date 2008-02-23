/* $Id: CALLBACK.CPP,v 1.7 2001-02-01 18:01:52 sandervl Exp $ */

/*
 * GDI32 support code for Line DDA callbacks
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include "misc.h"
#include "callback.h"
#include <wprocess.h>

#define DBG_LOCALLOG	DBG_callback
#include "dbglocal.h"

static VOID OPEN32API Callback(int X, int Y, LPARAM lpData);

//******************************************************************************
//******************************************************************************
LineDDAProcCallback::LineDDAProcCallback(LINEDDAPROC pUserCallback, LPARAM lpData)
{
  lpUserData = lpData;
  pCallback  = (LINEDDAPROC)pUserCallback;
}
//******************************************************************************
//******************************************************************************
LineDDAProcCallback::~LineDDAProcCallback()
{

}
//******************************************************************************
//******************************************************************************
LINEDDAPROC_O32 LineDDAProcCallback::GetOS2Callback()
{
  return Callback;
}
//******************************************************************************
//******************************************************************************
static VOID OPEN32API Callback(int X, int Y, LPARAM lpData)
{
 LineDDAProcCallback *me = (LineDDAProcCallback *)lpData;
 USHORT selTIB = SetWin32TIB(); // save current FS selector and set win32 sel

  me->pCallback(X, Y, me->lpUserData);
  SetFS(selTIB);           // switch back to the saved FS selector
  return;
}
//******************************************************************************
//******************************************************************************


