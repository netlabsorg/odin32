/* $Id: bitmap.h,v 1.1 1999-10-06 18:50:50 bird Exp $ */

/*
 * PE2LX bitmaps
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __BITMAP_H__
#define __BITMAP_H__

BOOL ShowBitmap(LXHeaderSuper &OS2Exe, int id, WINBITMAPINFOHEADER *bhdr, int size);
void *ConvertBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize, PULONG pulSize);
ULONG QuerySizeBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize);

#endif