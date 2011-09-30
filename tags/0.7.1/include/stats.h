#ifndef _STATISTICS_H__
#define _STATISTICS_H__

#include <_ras.h>

//Get memory statistics from odincrt
void _LNK_CONV getcrtstat(unsigned long *pnrcalls_malloc,
                          unsigned long *pnrcalls_free,
                          unsigned long *ptotalmemalloc);

#ifdef OS2_INCLUDED
#define LOGPEN     void
#define LOGFONTA   void
#define LPLOGBRUSH void *
#define LOGBRUSH void
#define DEVMODEA void
#define BITMAP void
#define BITMAPINFO void
#define BITMAPINFOHEADER void
#endif

#if defined(DEBUG) || defined(RAS)
void STATS_GetDCEx(HWND hwnd, HDC hdc, HRGN hrgn, ULONG flags);
void STATS_ReleaseDC(HWND hwnd, HDC hdc);
void STATS_InitializeGDI32 (void);
void STATS_UninitializeGDI32(void);
void STATS_DumpStatsGDI32();
void STATS_DumpStatsUSER32();
void STATS_InitializeUSER32 (void);
void STATS_UninitializeUSER32(void);
void STATS_CreateFontIndirect(HFONT hFont, LOGFONTA* lplf);
void STATS_CreateCompatibleDC(HDC hdc, HDC newHdc);
void STATS_DeleteDC(HDC hdc);
void STATS_CreatePatternBrush(HBRUSH hBrush, HBITMAP hBitmap);
void STATS_CreateDIBPatternBrushPt(HBRUSH hBrush, LPCVOID buffer, DWORD usage);
void STATS_CreatePenIndirect(HPEN hPen, const LOGPEN *lplgpn);
void STATS_CreatePen(HPEN hPen, int fnPenStyle, int nWidth, COLORREF crColor);
void STATS_ExtCreatePen(HPEN hPen, DWORD dwPenStyle, DWORD dwWidth, const LOGBRUSH *lplb, 
                        DWORD dwStyleCount, const DWORD *lpStyle);
void STATS_CreateBrushIndirect(HBRUSH hBrush, LPLOGBRUSH pLogBrush);
void STATS_CreateHatchBrush(HBRUSH hBrush, int fnStyle, COLORREF clrref);
void STATS_CreateSolidBrush(HBRUSH hBrush, COLORREF color);
void STATS_CreateICA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit);
void STATS_CreateDCA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit);

void STATS_CreatePolyPolygonRgn(HRGN hRgn, const POINT *lppt, const int *pPolyCount, int nCount, int fnPolyFillMode);
void STATS_CreateRectRgn(HRGN hRgn, int left, int top, int right, int bottom);
void STATS_CreateRoundRectRgn(HRGN hRgn, int left, int top, int right, int bottom, int nWidthEllipse, int nHeightEllipse);
void STATS_ExtCreateRegion(HRGN hRgn, PVOID pXform, DWORD count, const RGNDATA * pData);
void STATS_CreateEllipticRgn(HRGN hRgn, int left, int top, int right, int bottom);
void STATS_CreatePolygonRgn(HRGN hRgn, const POINT * lppt, int cPoints, int fnPolyFillMode);
void STATS_DeleteObject(HANDLE hObj, DWORD objtype);

void STATS_CreateDIBitmap(HBITMAP hBitmap,HDC hdc, const BITMAPINFOHEADER *lpbmih,
                          DWORD fdwInit, const void *lpbInit,
                          const BITMAPINFO *lpbmi, UINT fuUsage);
void STATS_CreateCompatibleBitmap(HBITMAP hBitmap,HDC hdc, int nWidth, int nHeight);
void STATS_CreateBitmap(HBITMAP hBitmap,int nWidth, int nHeight, UINT cPlanes,
                        UINT cBitsPerPel, const void *lpvBits);
void STATS_CreateDIBSection(HBITMAP hBitmap,HDC hdc, BITMAPINFO *pbmi, UINT iUsage,
                            VOID **ppvBits, HANDLE hSection, DWORD dwOffset);
void STATS_CreateBitmapIndirect(HBITMAP hBitmap, const BITMAP *pBitmap);

#else
#define STATS_GetDCEx(a, b, c, d)
#define STATS_ReleaseDC(a,b)
#define STATS_InitializeGDI32()
#define STATS_UninitializeGDI32()
#define STATS_DumpStatsGDI32()
#define STATS_DumpStatsUSER32()
#define STATS_CreateFontIndirect(a,b)
#define STATS_CreateCompatibleDC(a,b)
#define STATS_DeleteDC(a)
#define STATS_CreatePatternBrush(a,b)
#define STATS_CreateDIBPatternBrushPt(a,b,c)
#define STATS_CreatePenIndirect(a,b)
#define STATS_CreatePen(a,b,c,d)
#define STATS_CreateBrushIndirect(a,b)
#define STATS_CreateHatchBrush(a,b,c)
#define STATS_CreateSolidBrush(a,b)
#define STATS_CreateDCA(a,b,c,d,e)
#define STATS_CreateICA(a,b,c,d,e)
#define STATS_ExtCreatePen(a,b,c,d,e,f)
#define STATS_CreatePolyPolygonRgn(a,b,c,d,e)
#define STATS_CreateRectRgn(a,b,c,d,e)
#define STATS_CreateRoundRectRgn(a,b,c,d,e,f,g)
#define STATS_ExtCreateRegion(a,b,c,d)
#define STATS_CreateEllipticRgn(a,b,c,d,e)
#define STATS_CreatePolygonRgn(a,b,c,d)
#define STATS_DeleteObject(a,b)
#define STATS_CreateDIBitmap(a,b,c,d,e,f,g)
#define STATS_CreateCompatibleBitmap(a,b,c,d)
#define STATS_CreateBitmap(a,b,c,d,e,f)
#define STATS_CreateDIBSection(a,b,c,d,e,f,g)
#define STATS_CreateBitmapIndirect(a,b)

#endif

#endif