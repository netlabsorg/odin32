/* $Id: winres.cpp,v 1.26 2000-02-16 14:22:12 sandervl Exp $ */

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
#ifdef __IBMCPP__
#include <builtin.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INCL_WINRES
#include <win32type.h>
#include <winres.h>
#include <misc.h>
#include <winexepe2lx.h>
#include <windllpe2lx.h>
#include "cvtresource.h"
#include <vmutex.h>

#define DBG_LOCALLOG	DBG_winres
#include "dbglocal.h"

VMutex resmutex;

char *ResTypes[MAX_RES] =
      {"niks", "CURSOR", "BITMAP", "ICON", "MENU", "DIALOG", "STRING",
       "FONTDIR", "FONT", "ACCELERATOR", "RCDATA",  "MESSAGETABLE",
       "GROUP_CURSOR", "niks", "GROUP_ICON", "niks", "VERSION"};

//******************************************************************************
//******************************************************************************
Win32Resource::Win32Resource() :
        os2resdata(NULL), winresdata(NULL), resType(RSRC_CUSTOMNODATA)
{
  next       = NULL;
  module     = NULL;

  id         = -1;
  type       = -1;
  hres       = 0;
  OS2ResHandle = 0;
  //resources are in Unicode format by default; indirectly created resources
  //can also be in ascii format
  isUnicode  = TRUE;
}
//******************************************************************************
//******************************************************************************
Win32Resource::Win32Resource(Win32ImageBase *module, ULONG id, ULONG type,
                 ULONG size, char *resdata) : hres(NULL),
        os2resdata(NULL), winresdata(NULL), resType(RSRC_PELOADER)
{
  resmutex.enter();
  next           = module->winres;
  module->winres = this;
  resmutex.leave();

  this->module   = module;
  this->id       = id;
  this->type     = type;
  this->ressize  = size;
  winresdata     = (char *)malloc(size+sizeof(WCHAR));
  if(winresdata == NULL) {
    DebugInt3();
    return;
  }
  OS2ResHandle = 0;

  if(type == NTRT_STRING) {
    	memcpy(winresdata, resdata, size);
        ((USHORT *)winresdata)[size/sizeof(WCHAR)] = 0;
//        ((USHORT *)winresdata)[size/sizeof(WCHAR)-1] = 0;
  }
  else  memcpy(winresdata, resdata, size);

  //resources are in Unicode format by default
  isUnicode  = TRUE;
}
//******************************************************************************
//******************************************************************************
Win32Resource::~Win32Resource()
{
 Win32Resource *res = module->winres;

  if(os2resdata && (resType == RSRC_PELOADER || resType == RSRC_CUSTOMINDIRECT))
    free(os2resdata);

  if(winresdata)    free(winresdata);

  resmutex.enter();
  if(res == this) {
    module->winres = res->next;
  }
  else {
    while(res->next != this) {
        res  = res->next;
    }
    if(res)
        res->next = next;
  }
  resmutex.leave();
}
//******************************************************************************
//******************************************************************************
PVOID Win32Resource::lockResource()
{
  dprintf(("Win32Resource::lockResource %d %x\n", id, winresdata));

  if(winresdata)
    return(winresdata);

  dprintf(("Win32Resource::lockResource: NO windows resource!"));
  return NULL;
}
//******************************************************************************
//******************************************************************************
PVOID Win32Resource::lockOS2Resource()
{
 APIRET rc;
 PVOID  resdata;

   dprintf(("Win32Resource::lockOS2Resource %d\n", id));
   if(os2resdata == NULL) {
        os2resdata = convertResource(winresdata);
   }
   return os2resdata;
}
//******************************************************************************
//return size of converted win32 resource
//******************************************************************************
ULONG Win32Resource::getOS2Size()
{
  switch(type) {
    case NTRT_NEWBITMAP:
    case NTRT_BITMAP:
    	return QueryConvertedBitmapSize((WINBITMAPINFOHEADER *)winresdata, ressize);

    case NTRT_CURSOR:
    	return QueryConvertedCursorSize((CursorComponent *)winresdata, ressize);

    case NTRT_ICON:
    	return QueryConvertedIconSize((WINBITMAPINFOHEADER *)winresdata, ressize);

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
    	dprintf(("Win32Resource::getOS2Size SHOULDN'T BE CALLED for this resource type (%d) (NOT IMPLEMENTED)!!", type));
    	break;
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
PVOID Win32Resource::convertResource(void *win32res)
{
 int cvtressize;

  switch(type) {
    case NTRT_NEWBITMAP:
    case NTRT_BITMAP:
    	return ConvertBitmap((WINBITMAPINFOHEADER *)win32res, ressize, &ressize);

    case NTRT_CURSOR:
    	return ConvertCursor((CursorComponent *)win32res, ressize, &cvtressize);

    case NTRT_GROUP_CURSOR:
    	return ConvertCursorGroup((CursorHeader *)win32res, ressize, module);

    case NTRT_GROUP_ICON:
    	return ConvertIconGroup((IconHeader *)win32res, ressize, module);

    case NTRT_ICON:
    	return ConvertIcon((WINBITMAPINFOHEADER *)win32res, ressize, &cvtressize);

    case NTRT_ACCELERATORS:
    	return ConvertAccelerator((WINACCEL *)win32res, ressize);

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
  dprintf(("Win32Resource::convertResource: Can't convert resource %d (type %d)", id, type));
  return 0;
}
//******************************************************************************
//******************************************************************************
void Win32Resource::destroyAll(Win32ImageBase *module)
{
 Win32Resource *res = module->winres, *next;

   while(res) {
    next = res->next;
    delete(res);
    res  = next;
   }
}
//******************************************************************************
//******************************************************************************
