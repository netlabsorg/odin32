/* $Id: win32util.c,v 1.1 1999-05-24 20:19:49 ktk Exp $ */

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

//******************************************************************************
//******************************************************************************
ULONG SYSTEM Win32QueryModuleName(ULONG hmod, char *modname, int modsize)
{
  dprintf(("Win32QueryModuleHandle %X, %X, %d\n", hmod, modname, modsize));
  return(GetModuleFileNameA(hmod, modname, modsize));
}
//******************************************************************************
//******************************************************************************

