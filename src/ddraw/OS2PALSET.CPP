/* $Id: OS2PALSET.CPP,v 1.3 1999-06-10 17:10:56 phaller Exp $ */

/*
 * OS/2 Palette functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_GREALL
#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <pmddi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "os2palset.h"

//******************************************************************************
//******************************************************************************
void OS2SetPhysPalette(HWND hwndClient, void *pal)
{
 PALETTEENTRY *pDirectXPal = (PALETTEENTRY *)pal;
 HPS  hps;
 HDC  hdc;
 RGB2 os2rgb[256];
 int  i;
 ULONG cclr;

   hps = WinGetPS(HWND_DESKTOP);
   hdc = GpiQueryDevice(hps);
   for(i=0;i<256;i++) {
    os2rgb[i].bBlue     = pDirectXPal[i].peBlue;
    os2rgb[i].bGreen    = pDirectXPal[i].peGreen;
    os2rgb[i].bRed      = pDirectXPal[i].peRed;
    os2rgb[i].fcOptions = PC_RESERVED;
   }

   GpiCreateLogColorTable(hps, LCOL_PURECOLOR | LCOL_REALIZABLE,
                          LCOLF_CONSECRGB, 0, 256, (PLONG)&os2rgb);
   Gre32Entry3(hdc, 0L, 0x000060C6L);
//   WinInvalidateRect(HWND_DESKTOP, (PRECTL)NULL, TRUE);
   WinRealizePalette(hwndClient, hps, &cclr);
   WinReleasePS(hps);
}
//******************************************************************************
//******************************************************************************
void OS2ResetPhysPalette()
{
 HPS hps;
 HDC hdc;

   hps = WinGetPS(HWND_DESKTOP);
   hdc = GpiQueryDevice(hps);
   Gre32Entry3(hdc, 0L, 0x000060C7L);
   WinInvalidateRect(HWND_DESKTOP, (PRECTL)NULL, TRUE);
   WinReleasePS(hps);
}
//******************************************************************************
//******************************************************************************
