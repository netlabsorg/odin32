/* $Id: nameid.cpp,v 1.6 1999-08-18 17:18:00 sandervl Exp $ */

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
#include "os2util.h"

/******************************************************************************/
/******************************************************************************/
int SYSTEM EXPORT ConvertNameId(ULONG hmod, char *name)
{
 HMODULE   hmodule;
 Win32Dll *module;

  dprintf(("Convert %s in mod %X to id\n", name, hmod ));

  hmodule = GetOS2ModuleHandle(hmod);
  if(  hmodule == 0
    || hmodule == -1
    || hmodule == OS2iGetModuleHandleA( NULL ) )
  {
    return WinExe->convertNameId(name);
  }

  module = Win32Dll::findModule(hmodule);
  if(module == 0)
  {
    dprintf(("ConvertNameId: module %X not found\n", hmod));
    return(0);
  }
  return module->convertNameId(name);
}
//******************************************************************************
//******************************************************************************
char *StripPath(char *path)
{
  /* @@@PH what does this function do ? Strip the path from a FQFN name ? */
  char *pszFilename;
  
  pszFilename = strrchr(path, '\\');                 /* find rightmost slash */
  if (pszFilename != NULL)
    return (pszFilename++);              /* return pointer to next character */
  
  pszFilename = strrchr(path, '/');                  /* find rightmost slash */
  if (pszFilename != NULL)
    return (pszFilename++);              /* return pointer to next character */
  
  return (path);                                     /* default return value */
}
//******************************************************************************
//******************************************************************************
ULONG GetOS2ModuleHandle(ULONG hmod)
{
 APIRET  rc;
 char    modname[128];
 HMODULE hmodule;

  if(hmod == 0 || hmod == -1) {
        return(0);
  }

  if(Win32QueryModuleName(hmod, modname, sizeof(modname)) == 0) {
        dprintf(("Can't determine handle of dll %X\n", hmod));
        return(0);
  }
  rc = DosQueryModuleHandle(modname, &hmodule);
  if(rc) {
        dprintf(("Can't determine handle of dll %s\n", modname));
        return(0);
  }
  return(hmodule);
}
/******************************************************************************/
/******************************************************************************/
