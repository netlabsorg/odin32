#include <os2win.h>
#include <stats.h>

#ifdef DEBUG

static DWORD nrdcscreated     = 0;
static DWORD nrfontscreated   = 0;
static DWORD nrpenscreated    = 0;
static DWORD nrbrushescreated = 0;
static DWORD nrregionscreated = 0;
static DWORD nrbitmapscreated = 0;

//******************************************************************************
//******************************************************************************
void STATS_CreateFontIndirect(HFONT hFont, LOGFONTA* lplf)
{
    nrfontscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateCompatibleDC(HDC hdc, HDC newHdc)
{
    nrdcscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_DeleteDC(HDC hdc)
{
    nrdcscreated--;
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePatternBrush(HBRUSH hBrush, HBITMAP hBitmap)
{
    nrbrushescreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDIBPatternBrushPt(HBRUSH hBrush, LPCVOID buffer, DWORD usage)
{
    nrbrushescreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePenIndirect(HPEN hPen, const LOGPEN *lplgpn)
{
    nrpenscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePen(HPEN hPen, int fnPenStyle, int nWidth, COLORREF crColor)
{
    nrpenscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_ExtCreatePen(HPEN hPen, DWORD dwPenStyle, DWORD dwWidth, const LOGBRUSH *lplb, 
                        DWORD dwStyleCount, const DWORD *lpStyle)
{
    nrpenscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateBrushIndirect(HBRUSH hBrush, LPLOGBRUSH pLogBrush)
{
    nrbrushescreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateHatchBrush(HBRUSH hBrush, int fnStyle, COLORREF clrref)
{
    nrbrushescreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateSolidBrush(HBRUSH hBrush, COLORREF color)
{
    nrbrushescreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateICA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit)
{
    nrdcscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDCA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit)
{
    nrdcscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePolyPolygonRgn(HRGN hRgn, const POINT *lppt, const int *pPolyCount, int nCount, int fnPolyFillMode)
{
    nrregionscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateRectRgn(HRGN hRgn, int left, int top, int right, int bottom)
{
    nrregionscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateRoundRectRgn(HRGN hRgn, int left, int top, int right, int bottom, int nWidthEllipse, int nHeightEllipse)
{
    nrregionscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_ExtCreateRegion(HRGN hRgn, PVOID pXform, DWORD count, const RGNDATA * pData)
{
    nrregionscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateEllipticRgn(HRGN hRgn, int left, int top, int right, int bottom)
{
    nrregionscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePolygonRgn(HRGN hRgn, const POINT * lppt, int cPoints, int fnPolyFillMode)
{
    nrregionscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDIBitmap(HBITMAP hBitmap,HDC hdc, const BITMAPINFOHEADER *lpbmih,
                          DWORD fdwInit, const void *lpbInit,
                          const BITMAPINFO *lpbmi, UINT fuUsage)
{
    nrbitmapscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateCompatibleBitmap(HBITMAP hBitmap,HDC hdc, int nWidth, int nHeight)
{
    nrbitmapscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateBitmap(HBITMAP hBitmap,int nWidth, int nHeight, UINT cPlanes,
                        UINT cBitsPerPel, const void *lpvBits)
{
    nrbitmapscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDIBSection(HBITMAP hBitmap,HDC hdc, BITMAPINFO *pbmi, UINT iUsage,
                            VOID **ppvBits, HANDLE hSection, DWORD dwOffset)
{
    nrbitmapscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_CreateBitmapIndirect(HBITMAP hBitmap, const BITMAP *pBitmap)
{
    nrbitmapscreated++;
}
//******************************************************************************
//******************************************************************************
void STATS_DeleteObject(HANDLE hObj, DWORD objtype)
{
    switch(objtype) {
    case OBJ_PEN:
    case OBJ_EXTPEN:
        nrpenscreated--;
        break;
    case OBJ_BRUSH:
        nrbrushescreated--;
        break;
    case OBJ_FONT:
        nrfontscreated--;
        break;
    case OBJ_REGION:
        nrregionscreated--;
        break;
    case OBJ_BITMAP:
        nrbitmapscreated--;
        break;

    case OBJ_PAL:
    case OBJ_METAFILE:
    case OBJ_MEMDC:
    case OBJ_ENHMETADC:
    case OBJ_ENHMETAFILE:
    case OBJ_DC:
    case OBJ_METADC:
         break;
    }
}
//******************************************************************************
//******************************************************************************
void STATS_DumpStatsGDI32()
{
    dprintf(("*************  GDI32 STATISTICS BEGIN *****************"));
    dprintf(("Leaked dcs            %d", nrdcscreated));
    dprintf(("Leaked font objects   %d", nrfontscreated));
    dprintf(("Leaked pen objects    %d", nrpenscreated));
    dprintf(("Leaked brush objects  %d", nrbrushescreated));
    dprintf(("Leaked region objects %d", nrregionscreated));
    dprintf(("Leaked bitmap objects %d", nrbitmapscreated));
    dprintf(("*************  GDI32 STATISTICS END   *****************"));
}
//******************************************************************************
//******************************************************************************


#endif //DEBUG
