/* $Id: cvtresource.h,v 1.1 1999-09-15 23:32:53 sandervl Exp $ */

#ifndef _CVTRESOURCE_H_
#define _CVTRESOURCE_H_

#include <winmenu.h>
#include <winicon.h>
#include <winaccel.h>
#include <wincursor.h>
#include <winimage.h>

void *ConvertAccelerator(WINACCEL *accdata, int size, int cp = 0);
void *ConvertBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize, PULONG pulSize);
void *ConvertCursor(CursorComponent *curHdr, int size, int *os2size, int offsetBits = 0);
void *ConvertCursorGroup(CursorHeader *chdr, int size, Win32Image *module);
void *ConvertIcon(WINBITMAPINFOHEADER *bmpHdr, int size, int *os2size, int offsetBits = 0);
void *ConvertIconGroup(IconHeader *ihdr, int size, Win32Image *module);
void *ConvertMenu(MenuHeader *menu, int size, int cp = 0);

ULONG QueryConvertedBitmapSize(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize);
ULONG QueryConvertedCursorSize(CursorComponent *curHdr, int size);
ULONG QueryConvertedIconSize(WINBITMAPINFOHEADER *bmpHdr, int size);

#endif /* _CVTRESOURCE_H_ */

