/* $Id: nameid.cpp,v 1.2 1999-05-31 22:08:12 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Resource id to name id conversion procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_BASE
#define INCL_WIN
#define INCL_WINERRORS
#define INCL_DOSFILEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <odin.h>
#include "win32type.h"
#include "misc.h"
#include "nameid.h"
#include "win32util.h"
#include "winimage.h"
#include "winexe.h"
#include "os2util.h"

void UpCase(char *mixedcase);

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
void SYSTEM EXPORT UpCase(char *mixedcase)
{
 int i;

  for(i=0;i<strlen(mixedcase);i++) {
        if(mixedcase[i] >= 'a' && mixedcase[i] <= 'z') {
                mixedcase[i] += 'A' - 'a';
        }
  }
}
/******************************************************************************/
/******************************************************************************/
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
