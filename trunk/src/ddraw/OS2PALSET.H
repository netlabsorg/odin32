/* $Id: OS2PALSET.H,v 1.6 2001-03-18 21:44:43 mike Exp $ */

/*
 * Defintions for OS/2 Palette functions
 *
 * Copyright 1998 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2PALSET_H__
  #define __OS2PALSET_H__

  #ifndef __OS2PALETTE_H__
   typedef struct tagPALETTEENTRY
   {
      BYTE        peRed;
      BYTE        peGreen;
      BYTE        peBlue;
      BYTE        peFlags;
   } PALETTEENTRY;
  #endif

  //void OS2SetPhysPalette(PALETTEENTRY *pDirectXPal);
  void OS2SetPhysPalette(void *pal);
  void OS2ResetPhysPalette();

#endif
