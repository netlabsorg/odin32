/* $Id: win32util.cpp,v 1.3 2000-02-16 14:22:11 sandervl Exp $ */

/*
 * Misc functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>

#define DBG_LOCALLOG	DBG_win32util
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
ULONG SYSTEM Win32QueryModuleName(ULONG hmod, char *modname, int modsize)
{
  dprintf(("Win32QueryModuleHandle %X, %X, %d\n", hmod, modname, modsize));
  return(GetModuleFileNameA(hmod, modname, modsize));
}
//******************************************************************************
//******************************************************************************

