/* $Id: cvtcursorgrp.cpp,v 1.11 2001-03-27 16:18:26 sandervl Exp $ */

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
#include <win32api.h>
#include <winconst.h>
#include <wincursor.h>
#include <win\cursoricon.h>
#include <misc.h>
#include "cvtresource.h"
#include <winres.h>

#define DBG_LOCALLOG	DBG_cvtcursorgrp
#include "dbglocal.h"

//CURSORICONFILEDIR

//******************************************************************************
//******************************************************************************
void * WIN32API ConvertCursorGroupToOS2(LPVOID ptr)
{
 CURSORICONFILEDIR *chdr = (CURSORICONFILEDIR *)ptr;
 int i, groupsize = 0, os2cursorsize;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 CursorComponent       *cursorhdr;
 void                  *os2cursor;
 HRSRC                  hRes;

  dprintf(("ConvertCursorGroup"));
  dprintf(("Cursor Group type : %d", chdr->idType));
  dprintf(("Cursor Group count: %d", chdr->idCount));
  for(i=0;i<chdr->idCount;i++) {
        dprintf2(("Cursor  : %d", chdr->idEntries[i].dwDIBOffset));
        dprintf2(("Width   : %d", (int)chdr->idEntries[i].bWidth));
        dprintf2(("Height  : %d", (int)chdr->idEntries[i].bHeight));
        dprintf2(("Bits    : %d", chdr->idEntries[i].bColorCount));
        dprintf2(("ResBytes: %d", chdr->idEntries[i].dwDIBSize));

	groupsize += chdr->idEntries[i].dwDIBSize;
  }
  bafh    = (BITMAPARRAYFILEHEADER2 *)malloc(groupsize+chdr->idCount*sizeof(BITMAPARRAYFILEHEADER2));
  orgbafh = bafh;

  rdir = (CursorResDir *)(chdr + 1);
  for(i=0;i<chdr->idCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER2);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
	hRes = FindResourceA(module->getInstanceHandle(), 
                            (LPCSTR)rdir->wNameOrdinal, (LPSTR)NTRT_CURSOR);

        if(hRes == NULL) {
                dprintf(("Can't find cursor!"));
                rdir++;
                continue;
        }

	cursorhdr = (CursorComponent *)LockResource(LoadResource(module->getInstanceHandle(), hRes));
	os2cursor = ConvertCursor(cursorhdr, SizeofResource(module->getInstanceHandle(), hRes), &os2cursorsize, (int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));

        if(os2cursor == NULL) {
                dprintf(("Can't convert cursor!"));
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

        rdir++;
  }
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************
void *ConvertCursorGroup(CursorHeader *chdr, int size, Win32ImageBase *module)
{
 CursorResDir *rdir = (CursorResDir *)(chdr + 1);
 int i, groupsize = 0, os2cursorsize;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 CursorComponent       *cursorhdr;
 void                  *os2cursor;
 HRSRC                  hRes;

  dprintf(("ConvertCursorGroup"));
  dprintf(("Cursor Group type : %d", chdr->wType));
  dprintf(("Cursor Group count: %d", chdr->cwCount));
  for(i=0;i<chdr->cwCount;i++) {
        dprintf2(("Cursor  : %d", rdir->wNameOrdinal));
        dprintf2(("Width   : %d", (int)rdir->wWidth));
        dprintf2(("Height  : %d", (int)rdir->wHeight));
        dprintf2(("Bits    : %d", rdir->wBitCount));
        dprintf2(("ResBytes: %d", rdir->lBytesInRes));
	hRes = FindResourceA(module->getInstanceHandle(), 
                            (LPCSTR)rdir->wNameOrdinal, (LPSTR)NTRT_CURSOR);

	groupsize += QueryConvertedResourceSize(module->getInstanceHandle(), (char *)NTRT_CURSOR, hRes);
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
	hRes = FindResourceA(module->getInstanceHandle(), 
                            (LPCSTR)rdir->wNameOrdinal, (LPSTR)NTRT_CURSOR);

        if(hRes == NULL) {
                dprintf(("Can't find cursor!"));
                rdir++;
                continue;
        }

	cursorhdr = (CursorComponent *)LockResource(LoadResource(module->getInstanceHandle(), hRes));
	os2cursor = ConvertCursor(cursorhdr, SizeofResource(module->getInstanceHandle(), hRes), &os2cursorsize, (int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));

        if(os2cursor == NULL) {
                dprintf(("Can't convert cursor!"));
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

        rdir++;
  }
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************
