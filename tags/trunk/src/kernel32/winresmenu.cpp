/* $Id: winresmenu.cpp,v 1.2 1999-08-31 17:15:30 sandervl Exp $ */

/*
 * Win32 Menu resource class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_WIN
#define INCL_GPIBITMAPS
#define INCL_BITMAPFILEFORMAT
#define INCL_DOSMODULEMGR
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdarg.h>
#include <string.h>
#define INCL_WINRES
#include <win32type.h>
#include <winres.h>
#include <winresmenu.h>
#include <misc.h>
#include <nameid.h>
#include <winexe.h>
#include "cvtresource.h"

//******************************************************************************
//******************************************************************************
Win32MenuRes::Win32MenuRes(Win32Image *module, HRSRC hRes, ULONG id, ULONG type)
               : Win32Resource(module, hRes, id, type)
{
}
//******************************************************************************
//******************************************************************************
Win32MenuRes::Win32MenuRes(Win32Image *module, ULONG id, ULONG type,
	                   ULONG size, char *resdata)
               : Win32Resource(module, id, type, size, resdata)
{
}
//******************************************************************************
//******************************************************************************
Win32MenuRes::Win32MenuRes(HWND hwndObjectMenu)
      : Win32Resource()
{
  OS2ResHandle = hwndObjectMenu;
  resType      = RSRC_CUSTOMNODATA;
}
//******************************************************************************
//Template always contains Unicode strings
//******************************************************************************
Win32MenuRes::Win32MenuRes(LPVOID menutemplate)
      : Win32Resource()
{
  resType         = RSRC_CUSTOMINDIRECT;

  os2resdata      = ConvertMenu((MenuHeader *)menutemplate, 0);
  OS2ResHandle    = 0;
}
//******************************************************************************
//******************************************************************************
Win32MenuRes::~Win32MenuRes()
{
}
//******************************************************************************
//******************************************************************************
