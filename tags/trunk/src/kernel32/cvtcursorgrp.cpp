/* $Id: cvtcursorgrp.cpp,v 1.4 1999-09-21 08:24:53 sandervl Exp $ */

/*
 * PE2LX cursor group code
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
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <wincursor.h>
#include <winres.h>
#include <misc.h>
#include "cvtresource.h"

HRSRC WIN32API FindResourceA(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType);

//******************************************************************************
//******************************************************************************
void *ConvertCursorGroup(CursorHeader *chdr, int size, Win32ImageBase *module)
{
 CursorResDir *rdir = (CursorResDir *)(chdr + 1);
 int i, groupsize = 0, os2cursorsize;
 BITMAPARRAYFILEHEADER *bafh, *orgbafh;
 CursorComponent       *cursorhdr;
 Win32Resource         *winres;
 void                  *os2cursor;

  dprintf(("ConvertCursorGroup"));
  dprintf(("Cursor Group type : %d", chdr->wType));
  dprintf(("Cursor Group count: %d", chdr->cwCount));
  for(i=0;i<chdr->cwCount;i++) {
        dprintf(("Cursor  : %d", rdir->wNameOrdinal));
        dprintf(("Width   : %d", (int)rdir->wWidth));
        dprintf(("Height  : %d", (int)rdir->wHeight));
        dprintf(("Bits    : %d", rdir->wBitCount));
        dprintf(("ResBytes: %d", rdir->lBytesInRes));
	winres     = (Win32Resource *)FindResourceA(module->getInstanceHandle(), 
                                                    (LPCSTR)rdir->wNameOrdinal, 
                                                    (LPSTR)NTRT_CURSOR);
	groupsize += winres->getOS2Size();
        rdir++;
  }
  bafh    = (BITMAPARRAYFILEHEADER *)malloc(groupsize+chdr->cwCount*sizeof(BITMAPARRAYFILEHEADER));
  orgbafh = bafh;

  rdir = (CursorResDir *)(chdr + 1);
  for(i=0;i<chdr->cwCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
	winres          = (Win32Resource *)FindResourceA(module->getInstanceHandle(), 
                                                         (LPCSTR)rdir->wNameOrdinal, 
                                                         (LPSTR)NTRT_CURSOR);
        if(winres == NULL) {
                dprintf(("Can't find cursor!"));
                rdir++;
                continue;
        }

	cursorhdr = (CursorComponent *)winres->lockResource();
	os2cursor = ConvertCursor(cursorhdr, winres->getSize(), &os2cursorsize, (int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER)-sizeof(BITMAPFILEHEADER));

        if(os2cursor == NULL) {
                dprintf(("Can't convert cursor!"));
		delete winres;
                rdir++;
                continue;
        }

        if(i != chdr->cwCount -1) {
                bafh->offNext = (int)&bafh->bfh - (int)orgbafh + os2cursorsize;
        }
        else    bafh->offNext = 0;

        memcpy((char *)&bafh->bfh, os2cursor, os2cursorsize);
	free(os2cursor);

        bafh = (BITMAPARRAYFILEHEADER *)((int)&bafh->bfh + os2cursorsize);
	delete winres;

        rdir++;
  }
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************
