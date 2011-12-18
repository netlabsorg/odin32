/* $Id: cvtresource.h,v 1.5 1999-10-26 11:15:33 sandervl Exp $ */

#ifndef _CVTRESOURCE_H_
#define _CVTRESOURCE_H_

#include <winicon.h>
#include <winaccel.h>
#include <wincursor.h>
#include "winimagebase.h"

void *ConvertAccelerator(WINACCEL *accdata, int size, int cp = 0);
void *ConvertBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize, PULONG pulSize);
void *ConvertCursor(CursorComponent *curHdr, int size, int *os2size, int offsetBits = 0);
void *ConvertCursorGroup(CursorHeader *chdr, int size, Win32ImageBase *module);
void *ConvertIcon(WINBITMAPINFOHEADER *bmpHdr, int size, int *os2size, int offsetBits = 0);
void *ConvertIconGroup(IconHeader *ihdr, int size, Win32ImageBase *module);

ULONG QueryConvertedBitmapSize(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize);
ULONG QueryConvertedCursorSize(CursorComponent *curHdr, int size);
ULONG QueryConvertedIconSize(WINBITMAPINFOHEADER *bmpHdr, int size);

#endif /* _CVTRESOURCE_H_ */

