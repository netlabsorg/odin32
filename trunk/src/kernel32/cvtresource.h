/* $Id: cvtresource.h,v 1.1 1999-08-19 14:19:14 sandervl Exp $ */

#ifndef _CVTRESOURCE_H_
#define _CVTRESOURCE_H_

#include <winmenu.h>
#include <winicon.h>
#include <winaccel.h>

void *ConvertAccelerator(WINACCEL *accdata, int size, int cp = 0);
void *ConvertBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize, PULONG pulSize);
void *ConvertMenu(MenuHeader *menu, int size, int cp = 0);
void *ConvertIcon(WINBITMAPINFOHEADER *bmpHdr, int size);


#endif /* _CVTRESOURCE_H_ */

