/* $Id: OS2PALSET.CPP,v 1.9 2001-03-09 22:48:47 mike Exp $ */

/*
 * Functions used to Set the Physical Palette in OS/2 when in 8 Bit mode
 *
 * Copyright 1998 Sander va Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_GREALL
#define INCL_GPI
#include <os2wrap.h>
#include <pmddi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os2palset.h"

//******************************************************************************
//******************************************************************************
void OS2SetPhysPalette(void *pal)
{
 PALETTEENTRY *pDirectXPal = (PALETTEENTRY *)pal;
 HPS  hps;
 HDC  hdc;
 RGB2 os2rgb[256];
 int  i;

 USHORT sel = RestoreOS2FS();




  hps = WinGetPS(HWND_DESKTOP);
  hdc = GpiQueryDevice(hps);

  for(i=0;i<256;i++)
  {
    os2rgb[i].bBlue     = pDirectXPal[i].peBlue;
    os2rgb[i].bGreen    = pDirectXPal[i].peGreen;
    os2rgb[i].bRed      = pDirectXPal[i].peRed;
    os2rgb[i].fcOptions = 0;
  }

  GpiCreateLogColorTable( hps, LCOL_PURECOLOR | LCOL_REALIZABLE,
                          LCOLF_CONSECRGB,
                          0,
                          256,
                          (PLONG)&os2rgb[0]);
  Gre32Entry3( hdc,
               0L,
               0x000060C6L);

  WinInvalidateRect( HWND_DESKTOP,
                     (PRECTL)NULL,
                     TRUE);
  WinReleasePS(hps);

  SetFS(sel);

}

//******************************************************************************
//******************************************************************************
void OS2ResetPhysPalette()
{
 HPS hps;
 HDC hdc;

 USHORT sel = RestoreOS2FS();



  hps = WinGetPS( HWND_DESKTOP);

  hdc = GpiQueryDevice( hps);

  Gre32Entry3( hdc,
               0L,
               0x000060C7L);

  WinInvalidateRect( HWND_DESKTOP,
                    (PRECTL)NULL,
                    TRUE);

  WinReleasePS(hps);

  SetFS(sel);

}
//******************************************************************************
//******************************************************************************
