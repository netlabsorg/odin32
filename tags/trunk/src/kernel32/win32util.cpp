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

//******************************************************************************
//******************************************************************************
ULONG SYSTEM Win32QueryModuleName(ULONG hmod, char *modname, int modsize)
{
  dprintf(("Win32QueryModuleHandle %X, %X, %d\n", hmod, modname, modsize));
  return(GetModuleFileNameA(hmod, modname, modsize));
}
//******************************************************************************
//******************************************************************************

