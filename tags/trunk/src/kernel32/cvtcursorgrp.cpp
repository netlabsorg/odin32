/* $Id: cvtcursorgrp.cpp,v 1.9 2000-03-13 13:10:09 sandervl Exp $ */

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
#include <os2wrap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <wincursor.h>
#include <winres.h>
#include <misc.h>
#include "cvtresource.h"

#define DBG_LOCALLOG	DBG_cvtcursorgrp
#include "dbglocal.h"

HRSRC WIN32API FindResourceA(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType);

//******************************************************************************
//******************************************************************************
void *ConvertCursorGroup(CursorHeader *chdr, int size, Win32ImageBase *module)
{
 CursorResDir *rdir = (CursorResDir *)(chdr + 1);
 int i, groupsize = 0, os2cursorsize;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 CursorComponent       *cursorhdr;
 Win32Resource         *winres;
 void                  *os2cursor;

  dprintf(("ConvertCursorGroup"));
  dprintf(("Cursor Group type : %d", chdr->wType));
  dprintf(("Cursor Group count: %d", chdr->cwCount));
  for(i=0;i<chdr->cwCount;i++) {
        dprintf2(("Cursor  : %d", rdir->wNameOrdinal));
        dprintf2(("Width   : %d", (int)rdir->wWidth));
        dprintf2(("Height  : %d", (int)rdir->wHeight));
        dprintf2(("Bits    : %d", rdir->wBitCount));
        dprintf2(("ResBytes: %d", rdir->lBytesInRes));
	winres     = (Win32Resource *)FindResourceA(module->getInstanceHandle(), 
                                                    (LPCSTR)rdir->wNameOrdinal, 
                                                    (LPSTR)NTRT_CURSOR);
	groupsize += winres->getOS2Size();
        rdir++;
  }
  bafh    = (BITMAPARRAYFILEHEADER2 *)malloc(groupsize+chdr->cwCount*sizeof(BITMAPARRAYFILEHEADER2));
  orgbafh = bafh;

  rdir = (CursorResDir *)(chdr + 1);
  for(i=0;i<chdr->cwCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER2);
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
	os2cursor = ConvertCursor(cursorhdr, winres->getSize(), &os2cursorsize, (int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));

        if(os2cursor == NULL) {
                dprintf(("Can't convert cursor!"));
		delete winres;
                rdir++;
                continue;
        }

        if(i != chdr->cwCount -1) {
                bafh->offNext = (int)&bafh->bfh2 - (int)orgbafh + os2cursorsize;
        }
        else    bafh->offNext = 0;

        memcpy((char *)&bafh->bfh2, os2cursor, os2cursorsize);
	free(os2cursor);

        bafh = (BITMAPARRAYFILEHEADER2 *)((int)&bafh->bfh2 + os2cursorsize);
	delete winres;

        rdir++;
  }
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************
