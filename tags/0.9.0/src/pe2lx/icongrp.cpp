/* $Id: icongrp.cpp,v 1.4 1999-07-20 17:50:19 sandervl Exp $ */

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
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "pefile.h"
#include "lx.h"
#include "icon.h"
#include "icongrp.h"
#include "misc.h"

//******************************************************************************
//******************************************************************************
#ifdef __WATCOMC__
#pragma off (unreferenced)
#endif
void ShowGroupIcon(int id, IconHeader *ihdr, int size)
#ifdef __WATCOMC__
#pragma on (unreferenced)
#endif
{
 ResourceDirectory *rdir = (ResourceDirectory *)(ihdr + 1);
 int i, groupsize = 0;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 OS2Icon *icon;

  cout << "Icon Group id   :" << id << endl;
  cout << "Icon Group type :" << ihdr->wType << endl;
  cout << "Icon Group count:" << ihdr->wCount << endl;
  for(i=0;i<ihdr->wCount;i++) {
        cout << "Icon    :" << rdir->wNameOrdinal << endl;
        cout << "Width   :" << (int)rdir->bWidth << endl;
        cout << "Height  :" << (int)rdir->bHeight << endl;
        cout << "Colors  :" << (int)rdir->bColorCount << endl;
        cout << "Bits    :" << rdir->wBitCount << endl;
        cout << "ResBytes:" << rdir->lBytesInRes << endl;
        icon       = OS2Icon::GetIcon(rdir->wNameOrdinal);
        if(icon)
                groupsize += icon->QueryIconSize();
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
        icon            = OS2Icon::GetIcon(rdir->wNameOrdinal);
        if(icon == NULL) {
                cout << "Can't find icon!" << endl;
                rdir++;
                continue;
        }
        if(i != ihdr->wCount -1) {
                bafh->offNext = (int)&bafh->bfh2 - (int)orgbafh + icon->QueryIconSize();
        }
        else    bafh->offNext = 0;

        icon->SetIconHdrOffset((int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));

        memcpy((char *)&bafh->bfh2, (char *)icon->GetIconHeader(), icon->QueryIconSize());
        bafh = (BITMAPARRAYFILEHEADER2 *)((int)&bafh->bfh2 + icon->QueryIconSize());
        rdir++;
  }
  OS2Exe.StoreResource(id, RT_POINTER, groupsize+ihdr->wCount*sizeof(BITMAPARRAYFILEHEADER2), (char *)orgbafh);
  free((char *)orgbafh);
}
//******************************************************************************
//******************************************************************************
