/* $Id: win32util.c,v 1.2 1999-05-31 22:08:14 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Misc functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include "misc.h"


//******************************************************************************
//******************************************************************************
ULONG SYSTEM Win32QueryModuleName(ULONG hmod, char *modname, int modsize)
{
  dprintf(("Win32QueryModuleHandle %X, %X, %d\n", hmod, modname, modsize));
  return(GetModuleFileNameA(hmod, modname, modsize));
}
//******************************************************************************
//******************************************************************************

