/* $Id: OS2PALSET.H,v 1.1 1999-05-24 20:19:37 ktk Exp $ */

/*
 * OS/2 Palette function definitions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2PALSET_H__
#define __OS2PALSET_H__

#ifndef __OS2PALETTE_H__
 typedef struct tagPALETTEENTRY {
    BYTE        peRed;
    BYTE        peGreen;
    BYTE        peBlue;
    BYTE        peFlags;
 } PALETTEENTRY;
#endif

//void OS2SetPhysPalette(PALETTEENTRY *pDirectXPal);
void OS2SetPhysPalette(HWND hwndClient, void *pal);
void OS2ResetPhysPalette();

#endif
