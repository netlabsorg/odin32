/* $Id: CALLBACK.CPP,v 1.4 2000-02-16 14:18:09 sandervl Exp $ */

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

  me->pCallback(X, Y, me->lpUserData);
  return;
}
//******************************************************************************
//******************************************************************************
