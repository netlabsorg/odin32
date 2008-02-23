/* $Id: bitmap.h,v 1.3 1999-06-10 17:08:52 phaller Exp $ */

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

BOOL ShowBitmap(LXHeader &OS2Exe, int id, WINBITMAPINFOHEADER *bhdr, int size);
void *ConvertBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize, PULONG pulSize);
ULONG QuerySizeBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize);

#endif
