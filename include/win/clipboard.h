/* $Id: clipboard.h,v 1.2 2002-09-03 12:34:35 sandervl Exp $ */

#ifndef __WINE_CLIPBOARD_H
#define __WINE_CLIPBOARD_H

#include "windef.h"

typedef struct tagWINE_CLIPFORMAT {
    UINT	wFormatID;
    UINT	wRefCount;
    BOOL	wDataPresent;
    LPSTR	Name;
    HANDLE      hData16;
    HANDLE	hDataSrc32;
    HANDLE	hData32;
    ULONG       drvData;
    struct tagWINE_CLIPFORMAT *PrevFormat;
    struct tagWINE_CLIPFORMAT *NextFormat;
} WINE_CLIPFORMAT, *LPWINE_CLIPFORMAT;


#endif /* __WINE_CLIPBOARD_H */
