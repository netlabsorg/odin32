#ifndef __ODIN_WING32
 #define __ODIN_WING32

#ifdef __cplusplus
      extern "C" {
#endif
HDC WINAPI WinGCreateDC();
BOOL WINAPI WinGRecommendDIBFormat(BITMAPINFO *bmpi);
HBITMAP WINAPI WinGCreateBitmap( HDC hdc,
                  BITMAPINFO *bmpi,
                  LPVOID *bits);
LPVOID WINAPI WinGGetDIBPointer(HBITMAP hWinGBitmap, BITMAPINFO* bmpi);

UINT WINAPI WinGGetDIBColorTable(HDC hdc, UINT start, UINT num, RGBQUAD *colors);
UINT WINAPI WinGSetDIBColorTable(HDC hdc, UINT start, UINT num, RGBQUAD *colors);
BOOL WINAPI WinGBitBlt( HDC destDC, UINT xDest, UINT yDest,
                 UINT widDest, UINT heiDest, HDC srcDC,
                 UINT xSrc, UINT ySrc);
BOOL WINAPI WinGStretchBlt( HDC destDC, UINT xDest, UINT yDest,
                     UINT widDest, UINT heiDest,
                     HDC srcDC, UINT xSrc, UINT ySrc,
                     UINT widSrc, UINT heiSrc);
HBRUSH WINAPI WinGCreateHalftoneBrush( HDC winDC,
                                       COLORREF col,
                                       WING_DITHER_TYPE type);
HPALETTE WINAPI WinGCreateHalftonePalette()


#ifdef __cplusplus
  }
#endif

#endif
