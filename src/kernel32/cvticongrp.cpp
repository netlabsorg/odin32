/* $Id: cvticongrp.cpp,v 1.8 2001-08-07 21:34:16 sandervl Exp $ */

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
#include <win32api.h>
#include <winconst.h>
#include <winicon.h>
#include <misc.h>
#include "cvtresource.h"
#include <winres.h>

#define DBG_LOCALLOG	DBG_cvticongrp
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
void * WIN32API ConvertIconGroup(void *hdr, HINSTANCE hInstance, DWORD *ressize)
{
 IconHeader *ihdr = (IconHeader *)hdr;
 ResourceDirectory *rdir = (ResourceDirectory *)(ihdr + 1);
 int i, groupsize = 0, os2iconsize;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 WINBITMAPINFOHEADER    *iconhdr;
 void                   *os2icon;
 HRSRC                   hRes;

  dprintf(("Icon Group type :%d", ihdr->wType));
  dprintf(("Icon Group count:%d", ihdr->wCount));
  for(i=0;i<ihdr->wCount;i++) {
        dprintf2(("Icon    : %d", rdir->wNameOrdinal));
        dprintf2(("Width   : %d", (int)rdir->bWidth));
        dprintf2(("Height  : %d", (int)rdir->bHeight));
        dprintf2(("Colors  : %d", (int)rdir->bColorCount));
        dprintf2(("Bits    : %d", rdir->wBitCount));
        dprintf2(("ResBytes: %d", rdir->lBytesInRes));
	hRes = FindResourceA(hInstance, 
                             (LPCSTR)rdir->wNameOrdinal, (LPSTR)NTRT_ICON);

	groupsize += QueryConvertedResourceSize(hInstance, (char *)NTRT_ICON, hRes);
        rdir++;
  }
  groupsize = groupsize+ihdr->wCount*sizeof(BITMAPARRAYFILEHEADER2);
  bafh    = (BITMAPARRAYFILEHEADER2 *)malloc(groupsize);
  orgbafh = bafh;

  rdir = (ResourceDirectory *)(ihdr + 1);
  for(i=0;i<ihdr->wCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER2);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
	hRes = FindResourceA(hInstance, 
                             (LPCSTR)rdir->wNameOrdinal, (LPSTR)NTRT_ICON);

        if(hRes == NULL) {
                dprintf(("Can't find icon!"));
                rdir++;
                continue;
        }

	iconhdr = (WINBITMAPINFOHEADER *)LockResource(LoadResource(hInstance, hRes));
	os2icon = ConvertIcon(iconhdr, SizeofResource(hInstance, hRes), &os2iconsize, (int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));

        if(os2icon == NULL) {
                dprintf(("Can't convert icon!"));
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

        rdir++;
  }
  *ressize = groupsize;
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************
