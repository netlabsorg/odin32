/*
 * Copyright 1999 Marcus Meissner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __WINE_VFW16_H
#define __WINE_VFW16_H

#include "vfw.h"
#include "wine/windef16.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef __WIN32OS2__
#include <heapstring.h>

#define GlobalAlloc16 GlobalAlloc
#define GlobalLock16 GlobalLock
#define GlobalUnlock16 GlobalUnlock
#define GlobalFree16 GlobalFree

#define MapLS(a)	(DWORD)a
#define UnMapLS(a)

#define HIC16	DWORD

#endif

typedef HANDLE16 HDRAWDIB16;

#include "pshpack1.h"

typedef struct {
	DWORD dwSize;
	DWORD fccType;
	DWORD fccHandler;
	DWORD dwFlags;
	DWORD dwVersion;
	DWORD dwVersionICM;
	/*
	 * under Win16, normal chars are used
	 */
	CHAR szName[16];
	CHAR szDescription[128];
	CHAR szDriver[128];
} ICINFO16;

typedef struct {
    DWORD		dwFlags;
    LPBITMAPINFOHEADER	lpbiSrc;
    LPVOID		lpSrc;
    LPBITMAPINFOHEADER	lpbiDst;
    LPVOID		lpDst;

    INT16  		xDst;       /* destination rectangle */
    INT16		yDst;
    INT16  		dxDst;
    INT16  		dyDst;

    INT16		xSrc;       /* source rectangle */
    INT16  		ySrc;
    INT16		dxSrc;
    INT16  		dySrc;
} ICDECOMPRESSEX16;

typedef struct {
	DWORD		dwFlags;
	HPALETTE16	hpal;
	HWND16		hwnd;
	HDC16		hdc;
	INT16		xDst;
	INT16		yDst;
	INT16		dxDst;
	INT16		dyDst;
	LPBITMAPINFOHEADER	lpbi;
	INT16		xSrc;
	INT16		ySrc;
	INT16		dxSrc;
	INT16		dySrc;
	DWORD		dwRate;
	DWORD		dwScale;
} ICDRAWBEGIN16;

#include "poppack.h"

typedef struct {
	DWORD dwFlags;
	LPBITMAPINFOHEADER lpbiIn;
	LPBITMAPINFOHEADER lpbiSuggest;
	INT16 dxSrc;
	INT16 dySrc;
	INT16 dxDst;
	INT16 dyDst;
	HIC16 hicDecompressor;
} ICDRAWSUGGEST16;

DWORD   VFWAPIV ICDraw16(HIC16,DWORD,LPVOID,LPVOID,DWORD,LONG);
DWORD   VFWAPIV ICDrawBegin16(HIC16,DWORD,HPALETTE16,HWND16,HDC16,INT16,
                              INT16,INT16,INT16,LPBITMAPINFOHEADER,
                              INT16,INT16,INT16,INT16,DWORD,DWORD);
LRESULT WINAPI  ICClose16(HIC16);
DWORD   VFWAPIV ICCompress16(HIC16,DWORD,LPBITMAPINFOHEADER,LPVOID,
                             LPBITMAPINFOHEADER,LPVOID,LPDWORD,
                             LPDWORD,LONG,DWORD,DWORD,
                             LPBITMAPINFOHEADER,LPVOID);
DWORD   VFWAPIV ICDecompress16(HIC16,DWORD,LPBITMAPINFOHEADER,LPVOID,
                               LPBITMAPINFOHEADER,LPVOID);
HIC16   VFWAPI  ICGetDisplayFormat16(HIC16,LPBITMAPINFOHEADER,
                                     LPBITMAPINFOHEADER,INT16,INT16,
                                     INT16);
LRESULT VFWAPI  ICGetInfo16(HIC16,ICINFO16 *,DWORD);
BOOL16  VFWAPI  ICInfo16(DWORD,DWORD,ICINFO16 *);
HIC16   VFWAPI  ICLocate16(DWORD,DWORD,LPBITMAPINFOHEADER,
                           LPBITMAPINFOHEADER,WORD);
LRESULT VFWAPIV ICMessage16(void);
HIC16   VFWAPI  ICOpen16(DWORD,DWORD,UINT16);
HIC16   VFWAPI  ICOpenFunction16(DWORD,DWORD,UINT16,FARPROC16);
LRESULT VFWAPI  ICSendMessage16(HIC16,UINT16,DWORD,DWORD);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __WINE_VFW16_H */
