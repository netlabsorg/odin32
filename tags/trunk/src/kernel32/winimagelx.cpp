/* $Id: winimagelx.cpp,v 1.1 1999-09-15 23:39:07 sandervl Exp $ */

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
Win32LxImage::Win32LxImage(HINSTANCE hInstance) 
               : Win32ImageBase(hInstance)
{
 APIRET rc;

  szFileName[0] = 0;

  char *name = OSLibGetDllName(hinstance);
  strcpy(szFileName, name);
  strupr(szFileName);
}
//******************************************************************************
//******************************************************************************
Win32LxImage::~Win32LxImage()
{
}
//******************************************************************************
//******************************************************************************

