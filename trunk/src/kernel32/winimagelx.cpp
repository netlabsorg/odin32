/* $Id: winimagelx.cpp,v 1.4 1999-11-26 00:05:19 sandervl Exp $ */

/*
 * Win32 LX Image base class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#define INCL_BASE
#include <os2wrap.h>             //Odin32 OS/2 api wrappers

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>
#include <misc.h>
#include <win32type.h>
#include <winimagebase.h>
#include <winimagelx.h>
#include <windllbase.h>
#include <winexebase.h>
#include <winexelx.h>
#include <pefile.h>
#include <unicode.h>
#include <winres.h>
#include "oslibmisc.h"
#include "initterm.h"
#include <win\virtual.h>

//******************************************************************************
//******************************************************************************
Win32LxImage::Win32LxImage(HINSTANCE hInstance, PVOID pResData) 
               : Win32ImageBase(hInstance)
{
 APIRET rc;

  szFileName[0] = 0;

  char *name = OSLibGetDllName(hinstance);
  strcpy(szFileName, name);
  strupr(szFileName);

  setFullPath(szFileName);

  //Pointer to PE resource tree generates by wrc (or NULL for system dlls)
  pResDir = (PIMAGE_RESOURCE_DIRECTORY)pResData;

  //pResourceSectionStart contains the virtual address of the imagebase in the PE header
  //for the resource section (images loaded by the pe.exe)
  //For LX images, this is 0 as OffsetToData contains a relative offset
  pResourceSectionStart = 0;
}
//******************************************************************************
//******************************************************************************
Win32LxImage::~Win32LxImage()
{
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxImage::getApi(char *name)
{
  APIRET      rc;
  ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&apiaddr);
  if(rc)  
  {
	if(rc == ERROR_INVALID_HANDLE) 
        {//handle invalid for some silly reason, so load module again (initterm entrypoint not called twice)
		char szErrName[CCHMAXPATH];

		rc = DosLoadModule(szErrName, sizeof(szErrName), szFileName, &hinstance);
		if(!rc)
			rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&apiaddr);
	}
	if(rc) 	return(0);
  }
  return(apiaddr);
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxImage::getApi(int ordinal)
{
 APIRET      rc;
 ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&apiaddr);
  if(rc) {
	if(rc == ERROR_INVALID_HANDLE) 
        {//handle invalid for some silly reason, so load module again (initterm entrypoint not called twice)
		char szErrName[CCHMAXPATH];

		rc = DosLoadModule(szErrName, sizeof(szErrName), szFileName, &hinstance);
		if(!rc)
			rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&apiaddr);
	}
	if(rc) 	return(0);
  }
  return(apiaddr);
}
//******************************************************************************
//******************************************************************************

