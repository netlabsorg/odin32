/* $Id: winres.cpp,v 1.29 2001-03-13 18:45:34 sandervl Exp $ */

/*
 * Win32 resource class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 */
#define INCL_BASE
#define INCL_WIN
#define INCL_GPIBITMAPS
#define INCL_BITMAPFILEFORMAT
#define INCL_DOSMODULEMGR
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INCL_WINRES
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <winres.h>
#include <misc.h>
#include <winimagebase.h>
#include <winexebase.h>
#include <windllbase.h>
#include "cvtresource.h"

#define DBG_LOCALLOG	DBG_winres
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
PVOID WIN32API ConvertResourceToOS2(HINSTANCE hInstance, LPSTR restype, HRSRC hResource)
{
 PIMAGE_RESOURCE_DATA_ENTRY pData = (PIMAGE_RESOURCE_DATA_ENTRY)hResource;
 Win32ImageBase *module;
 char           *resdata;
 int             ressize, cvtressize;

  if(hInstance == 0 || hInstance == -1 || (WinExe && hInstance == WinExe->getInstanceHandle())) {
        module = (Win32ImageBase *)WinExe;
  }
  else {
        module = (Win32ImageBase *)Win32DllBase::findModule(hInstance);
  }
  resdata = (char *)LockResource(LoadResource(hInstance, hResource));
  ressize = SizeofResource(hInstance, hResource);

  switch((int)restype) {
    case NTRT_CURSOR:
    	return ConvertCursor((CursorComponent *)resdata, ressize, &cvtressize);

    case NTRT_GROUP_CURSOR:
    	return ConvertCursorGroup((CursorHeader *)resdata, ressize, module);

    case NTRT_NEWMENU:
    case NTRT_MENU:
    case NTRT_NEWDIALOG:
    case NTRT_DIALOG:
    case NTRT_FONTDIR:
    case NTRT_FONT:
    case NTRT_MESSAGETABLE:
    case NTRT_RCDATA:
    case NTRT_VERSION:
    case NTRT_STRING:
    	break;

    default:
        break;
  }
  dprintf(("ConvertResourceToOS2: Can't convert resource %x (inst %x)", hResource, hInstance));
  return 0;
}
//******************************************************************************
//******************************************************************************
PVOID WIN32API ConvertCursorToOS2(LPVOID lpWinResData)
{
 int ressize, cvtressize;
 
  ressize = QueryConvertedCursorSize((CursorComponent *)lpWinResData, 0);

  return ConvertCursor((CursorComponent *)lpWinResData, ressize, &cvtressize);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FreeOS2Resource(LPVOID lpResource)
{
  free(lpResource);
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API QueryConvertedResourceSize(HINSTANCE hInstance, LPSTR restype, HRSRC hResource)
{
 PIMAGE_RESOURCE_DATA_ENTRY pData = (PIMAGE_RESOURCE_DATA_ENTRY)hResource;
 Win32ImageBase *module;
 char           *resdata;
 int             ressize, cvtressize;

  if(hInstance == 0 || hInstance == -1 || (WinExe && hInstance == WinExe->getInstanceHandle())) {
        module = (Win32ImageBase *)WinExe;
  }
  else {
        module = (Win32ImageBase *)Win32DllBase::findModule(hInstance);
  }
  resdata = (char *)LockResource(LoadResource(hInstance, hResource));
  ressize = SizeofResource(hInstance, hResource);

  switch((int)restype) {
    case NTRT_CURSOR:
    	return QueryConvertedCursorSize((CursorComponent *)resdata, ressize);

    case NTRT_GROUP_ICON:
    case NTRT_GROUP_CURSOR:
    case NTRT_ACCELERATORS:
    case NTRT_NEWMENU:
    case NTRT_MENU:
    case NTRT_NEWDIALOG:
    case NTRT_DIALOG:
    case NTRT_FONTDIR:
    case NTRT_FONT:
    case NTRT_MESSAGETABLE:
    case NTRT_RCDATA:
    case NTRT_VERSION:
    case NTRT_STRING:
    default:
    	dprintf(("Win32Resource::getOS2Size SHOULDN'T BE CALLED for this resource type (%d) (NOT IMPLEMENTED)!!", restype));
    	break;
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
