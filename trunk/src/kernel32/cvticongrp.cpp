/* $Id: cvticongrp.cpp,v 1.5 1999-12-29 12:39:09 sandervl Exp $ */

/*
 * PE2LX Icon group code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_GPIBITMAPS
#define INCL_BITMAPFILEFORMAT
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2wrap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <winicon.h>
#include <winres.h>
#include <misc.h>
#include "cvtresource.h"

HRSRC WIN32API FindResourceA(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType);

//******************************************************************************
//******************************************************************************
void *ConvertIconGroup(IconHeader *ihdr, int size, Win32ImageBase *module)
{
 ResourceDirectory *rdir = (ResourceDirectory *)(ihdr + 1);
 int i, groupsize = 0, os2iconsize;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 WINBITMAPINFOHEADER    *iconhdr;
 Win32Resource          *winres;
 void                   *os2icon;

  dprintf(("Icon Group type :%d", ihdr->wType));
  dprintf(("Icon Group count:%d", ihdr->wCount));
  for(i=0;i<ihdr->wCount;i++) {
        dprintf2(("Icon    : %d", rdir->wNameOrdinal));
        dprintf2(("Width   : %d", (int)rdir->bWidth));
        dprintf2(("Height  : %d", (int)rdir->bHeight));
        dprintf2(("Colors  : %d", (int)rdir->bColorCount));
        dprintf2(("Bits    : %d", rdir->wBitCount));
        dprintf2(("ResBytes: %d", rdir->lBytesInRes));
	winres     = (Win32Resource *)FindResourceA(module->getInstanceHandle(), 
                                                   (LPCSTR)rdir->wNameOrdinal, 
                                                   (LPSTR)NTRT_ICON);
	groupsize += winres->getOS2Size();
        rdir++;
  }
  bafh    = (BITMAPARRAYFILEHEADER2 *)malloc(groupsize+ihdr->wCount*sizeof(BITMAPARRAYFILEHEADER2));
  orgbafh = bafh;

  rdir = (ResourceDirectory *)(ihdr + 1);
  for(i=0;i<ihdr->wCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER2);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
	winres          = (Win32Resource *)FindResourceA(module->getInstanceHandle(), 
                                                         (LPCSTR)rdir->wNameOrdinal, 
                                                         (LPSTR)NTRT_ICON);
        if(winres == NULL) {
                dprintf(("Can't find icon!"));
                rdir++;
                continue;
        }

	iconhdr = (WINBITMAPINFOHEADER *)winres->lockResource();
	os2icon = ConvertIcon(iconhdr, winres->getSize(), &os2iconsize, (int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));

        if(os2icon == NULL) {
                dprintf(("Can't convert icon!"));
		delete winres;
                rdir++;
                continue;
        }

        if(i != ihdr->wCount -1) {
                bafh->offNext = (int)&bafh->bfh2 - (int)orgbafh + os2iconsize;
        }
        else    bafh->offNext = 0;

        memcpy((char *)&bafh->bfh2, os2icon, os2iconsize);
	free(os2icon);

        bafh = (BITMAPARRAYFILEHEADER2 *)((int)&bafh->bfh2 + os2iconsize);
	delete winres;

        rdir++;
  }
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************
