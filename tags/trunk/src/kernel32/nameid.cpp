/* $Id: nameid.cpp,v 1.8 1999-08-26 12:55:36 sandervl Exp $ */

/*
 * Resource id to name id conversion procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_WIN
#define INCL_WINERRORS
#define INCL_DOSFILEMGR
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include "nameid.h"
#include "win32util.h"
#include <winimage.h>
#include <winexe.h>

/******************************************************************************/
/******************************************************************************/
int SYSTEM EXPORT ConvertNameId(ULONG hmod, char *name)
{
 Win32Dll *module;

  dprintf(("Convert %s in mod %X to id\n", name, hmod ));

  if(hmod == 0 || hmod == -1 || hmod == WinExe->getInstanceHandle() )
  {
    return WinExe->convertNameId(name);
  }

  module = Win32Dll::findModule(hmod);
  if(module == 0)
  {
    dprintf(("ConvertNameId: module %X not found\n", hmod));
    return(0);
  }
  return module->convertNameId(name);
}
//******************************************************************************
//******************************************************************************
