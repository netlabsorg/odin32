/* $Id: cursorgrp.cpp,v 1.3 1999-06-10 17:08:53 phaller Exp $ */

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
#include <iostream.h>
#include <string.h>
#include "pefile.h"
#include "lx.h"
#include "cursor.h"
#include "cursorgrp.h"
#include "misc.h"

//******************************************************************************
//******************************************************************************
#ifdef __WATCOMC__
#pragma off (unreferenced)
#endif
void ShowGroupCursor(int id, CursorHeader *chdr, int size)
#ifdef __WATCOMC__
#pragma on (unreferenced)
#endif
{
 CursorResDir *rdir = (CursorResDir *)(chdr + 1);
 int i, groupsize = 0;
 BITMAPARRAYFILEHEADER *bafh, *orgbafh;
 OS2Cursor *Cursor;

  cout << "Cursor Group type :" << chdr->wType << endl;
  cout << "Cursor Group count:" << chdr->cwCount << endl;
  for(i=0;i<chdr->cwCount;i++) {
        cout << "Cursor    :" << rdir->wNameOrdinal << endl;
        cout << "Width   :" << (int)rdir->wWidth << endl;
        cout << "Height  :" << (int)rdir->wHeight << endl;
        cout << "Bits    :" << rdir->wBitCount << endl;
        cout << "ResBytes:" << rdir->lBytesInRes << endl;
        Cursor     = OS2Cursor::GetCursor(rdir->wNameOrdinal);
        groupsize += Cursor->QueryCursorSize();
        rdir++;
  }
  bafh    = (BITMAPARRAYFILEHEADER *)malloc(groupsize+chdr->cwCount*sizeof(BITMAPARRAYFILEHEADER));
  orgbafh = bafh;
#if 1
  rdir = (CursorResDir *)(chdr + 1);
  for(i=0;i<chdr->cwCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
        Cursor            = OS2Cursor::GetCursor(rdir->wNameOrdinal);
        if(Cursor == NULL) {
                cout << "Can't find Cursor!" << endl;
                rdir++;
                continue;
        }
        if(i != chdr->cwCount -1) {
                bafh->offNext = (int)&bafh->bfh - (int)orgbafh + Cursor->QueryCursorSize();
        }
        else    bafh->offNext = 0;

        Cursor->SetCursorHdrOffset((int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER)-sizeof(BITMAPFILEHEADER));

        memcpy((char *)&bafh->bfh, (char *)Cursor->GetCursorHeader(rdir->wWidth, rdir->wHeight, rdir->wPlanes, rdir->wBitCount),
               Cursor->QueryCursorSize());
        bafh = (BITMAPARRAYFILEHEADER *)((int)&bafh->bfh + Cursor->QueryCursorSize());
        rdir++;
  }
  OS2Exe.StoreResource(id, RT_POINTER, groupsize+chdr->cwCount*sizeof(BITMAPARRAYFILEHEADER), (char *)orgbafh);
#else
  rdir   = (CursorResDir *)(chdr + 1);
  Cursor = OS2Cursor::GetCursor(rdir->wNameOrdinal);
  OS2Exe.StoreResource(id, RT_POINTER, Cursor->QueryCursorSize(),
                       (char *)Cursor->GetCursorHeader(rdir->wWidth,
                       rdir->wHeight, rdir->wPlanes, rdir->wBitCount));
#endif
  free((char *)orgbafh);
}
//******************************************************************************
//******************************************************************************
