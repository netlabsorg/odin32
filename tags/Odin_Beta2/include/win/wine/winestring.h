/* $Id: winestring.h,v 1.6 1999-08-22 22:52:12 sandervl Exp $ */
#ifndef __WINE_WINE_WINESTRING_H
#define __WINE_WINE_WINESTRING_H

#include "windef.h"

INT16       WINAPI WideCharToLocal16(LPSTR,LPWSTR,INT16);
INT         WINAPI WideCharToLocal(LPSTR,LPWSTR,INT);
INT16       WINAPI LocalToWideChar16(LPWSTR,LPSTR,INT16);
INT         WINAPI LocalToWideChar(LPWSTR,LPSTR,INT);
#include <heapstring.h>

#endif /* __WINE_WINE_WINESTRING_H */
