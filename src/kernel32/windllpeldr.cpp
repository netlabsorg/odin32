/* $Id: windllpeldr.cpp,v 1.13 2004-01-15 10:39:08 sandervl Exp $ */

/*
 * Win32 PE loader Dll class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2wrap.h>    //Odin32 OS/2 api wrappers

#define __USE_EMX
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef __GNUC__
#include <iostream.h>
#include <fstream.h>
#endif
#include <misc.h>
#include <win32type.h>
#include <pefile.h>
#include "windllpeldr.h"
#include <wprocess.h>

#include "oslibmisc.h"
#include "oslibdos.h"

#define DBG_LOCALLOG    DBG_windllpeldr
#include "dbglocal.h"


//******************************************************************************
//******************************************************************************
Win32PeLdrDll::Win32PeLdrDll(char *szDllName, Win32ImageBase *parentImage)
                : Win32ImageBase(-1),
                  Win32DllBase(-1, 0, parentImage),
                  Win32PeLdrImage(szDllName, FALSE)
{
  dprintf(("Win32PeLdrDll::Win32PeLdrDll %s %s loaded by %s", szFileName, szModule,
          (parentImage) ? parentImage->getModuleName() : "Unknown"));
}
//******************************************************************************
//******************************************************************************
Win32PeLdrDll::~Win32PeLdrDll()
{
  dprintf(("Win32PeLdrDll::~Win32PeLdrDll %s", szModule));
}
//******************************************************************************
//******************************************************************************
DWORD Win32PeLdrDll::init(ULONG reservedMem, ULONG ulPEOffset)
{
 char   modname[CCHMAXPATH];
 char  *syspath;
 HFILE  dllfile;
 APIRET rc;
 DWORD  dwRet;

  strupr(szFileName);
  if(!strchr(szFileName, (int)'.')) {
    strcat(szFileName, DLL_EXTENSION);
  }

  dllfile = OSLibDosOpen(szFileName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
  if(dllfile == NULL) {//search in libpath for dll
    syspath = getenv("WIN32LIBPATH");
    if(syspath) {
        strcpy(modname, syspath);
        if(modname[strlen(modname)-1] != '\\') {
            strcat(modname, "\\");
        }
        strcat(modname, szFileName);
        strcpy(szFileName, modname);
    }
  }
  else  OSLibDosClose(dllfile);

  dwRet = Win32PeLdrImage::init(0);
  dllEntryPoint = (WIN32DLLENTRY)entryPoint;

  if(!(Characteristics & IMAGE_FILE_DLL)) {
    //executable loaded as dll; don't call entrypoint
    dprintf(("WARNING: Exe %s loaded as dll; entrypoint not called", szFileName));
    dllEntryPoint = NULL;
  }
  return dwRet;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrDll::isPe2LxDll() const
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrDll::isLxDll() const
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
