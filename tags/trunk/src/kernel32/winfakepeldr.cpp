/* $Id: winfakepeldr.cpp,v 1.4 2000-02-16 14:22:12 sandervl Exp $ */

/*
 * Win32 PE loader fake image class (RSRC_LOAD)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * NOTE: RSRC_LOAD is a special flag to only load the resource directory
 *       of a PE image. Processing imports, sections etc is not done.
 *       Nor is it put into the linked list of dlls (if it's a dll).
 *       This is useful for GetVersionSize/Resource in case it wants to
 *       get version info of an image that is not loaded.
 *       So an instance of this type can't be used for anything but resource lookup!
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#define INCL_BASE
#include <os2wrap.h>             //Odin32 OS/2 api wrappers

#include <misc.h>
#include <win32type.h>
#include <winfakepeldr.h>

#define DBG_LOCALLOG	DBG_winfakepeldr
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
Win32PeLdrRsrcImg::Win32PeLdrRsrcImg(char *szFileName) 
                        : Win32ImageBase(-1), 
                          Win32PeLdrImage(szFileName, FALSE, RSRC_LOAD)
{
}
//******************************************************************************
//******************************************************************************
Win32PeLdrRsrcImg::~Win32PeLdrRsrcImg()
{
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrRsrcImg::isDll()
{
   return FALSE;
}
//******************************************************************************
//******************************************************************************
ULONG Win32PeLdrRsrcImg::getApi(char *name)
{
   DebugInt3();
   return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32PeLdrRsrcImg::getApi(int ordinal)
{
   DebugInt3();
   return 0;
}
//******************************************************************************
//******************************************************************************
