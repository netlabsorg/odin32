/*
 * GDI object statistics
 *
 *
 * Copyright 2002-2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stats.h>
#include <objhandle.h>

#ifdef DEBUG

#define STATS_MAX_OBJECTS        1024

static DWORD createddc[STATS_MAX_OBJECTS]     = {0};
static DWORD createdfont[STATS_MAX_OBJECTS]   = {0};
static DWORD createdpen[STATS_MAX_OBJECTS]    = {0};
static DWORD createdbrush[STATS_MAX_OBJECTS]  = {0};
static DWORD createdregion[STATS_MAX_OBJECTS] = {0};
static DWORD createdbitmap[STATS_MAX_OBJECTS] = {0};

static DWORD nrdcscreated        = 0;
static DWORD nrfontscreated      = 0;
static DWORD nrpenscreated       = 0;
static DWORD nrbrushescreated    = 0;
static DWORD nrregionscreated    = 0;
static DWORD nrbitmapscreated    = 0;


//******************************************************************************
//******************************************************************************
static void STAT_InsertObject(HANDLE hObject, DWORD *pdwObjects)
{
    for(int i=0;i<STATS_MAX_OBJECTS;i++) {
        if(pdwObjects[i] == 0) {
            pdwObjects[i] = hObject;
            break;
        }
    }
    if(i == STATS_MAX_OBJECTS) {
        dprintf(("!WARNING! STAT_InsertObject: no room left!!"));
    }
}
//******************************************************************************
//******************************************************************************
static void STAT_DeleteObject(HANDLE hObject, DWORD *pdwObjects)
{
    for(int i=0;i<STATS_MAX_OBJECTS;i++) {
        if(LOWORD(pdwObjects[i]) == LOWORD(hObject)) {
            pdwObjects[i] = 0;
            break;
        }
    }
    if(i == STATS_MAX_OBJECTS) {
        dprintf(("!WARNING! STAT_DeleteObject: %x not found!!", hObject));
    }
}
//******************************************************************************
//******************************************************************************
static void STAT_PrintLeakedObjects(char *szMessage, DWORD *pdwObjects)
{
    for(int i=0;i<STATS_MAX_OBJECTS;i++) {
        if(pdwObjects[i] != 0) {
            dprintf(("%s %x", szMessage, pdwObjects[i]));
        }
    }
}
//******************************************************************************
//******************************************************************************
void STATS_CreateFontIndirect(HFONT hFont, LOGFONTA* lplf)
{
    nrfontscreated++;
    STAT_InsertObject(hFont, createdfont);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateCompatibleDC(HDC hdc, HDC newHdc)
{
    nrdcscreated++;
    STAT_InsertObject(newHdc, createddc);
}
//******************************************************************************
//******************************************************************************
void STATS_DeleteDC(HDC hdc)
{
    nrdcscreated--;
    STAT_DeleteObject(hdc, createddc);
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePatternBrush(HBRUSH hBrush, HBITMAP hBitmap)
{
    nrbrushescreated++;
    STAT_InsertObject(hBrush, createdbrush);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDIBPatternBrushPt(HBRUSH hBrush, LPCVOID buffer, DWORD usage)
{
    nrbrushescreated++;
    STAT_InsertObject(hBrush, createdbrush);
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePenIndirect(HPEN hPen, const LOGPEN *lplgpn)
{
    nrpenscreated++;
    STAT_InsertObject(hPen, createdpen);
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePen(HPEN hPen, int fnPenStyle, int nWidth, COLORREF crColor)
{
    nrpenscreated++;
    STAT_InsertObject(hPen, createdpen);
}
//******************************************************************************
//******************************************************************************
void STATS_ExtCreatePen(HPEN hPen, DWORD dwPenStyle, DWORD dwWidth, const LOGBRUSH *lplb, 
                        DWORD dwStyleCount, const DWORD *lpStyle)
{
    nrpenscreated++;
    STAT_InsertObject(hPen, createdpen);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateBrushIndirect(HBRUSH hBrush, LPLOGBRUSH pLogBrush)
{
    nrbrushescreated++;
    STAT_InsertObject(hBrush, createdbrush);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateHatchBrush(HBRUSH hBrush, int fnStyle, COLORREF clrref)
{
    nrbrushescreated++;
    STAT_InsertObject(hBrush, createdbrush);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateSolidBrush(HBRUSH hBrush, COLORREF color)
{
    nrbrushescreated++;
    STAT_InsertObject(hBrush, createdbrush);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateICA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit)
{
    nrdcscreated++;
    STAT_InsertObject(hdc, createddc);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDCA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit)
{
    nrdcscreated++;
    STAT_InsertObject(hdc, createddc);
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePolyPolygonRgn(HRGN hRgn, const POINT *lppt, const int *pPolyCount, int nCount, int fnPolyFillMode)
{
    nrregionscreated++;
    STAT_InsertObject(hRgn, createdregion);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateRectRgn(HRGN hRgn, int left, int top, int right, int bottom)
{
    nrregionscreated++;
    STAT_InsertObject(hRgn, createdregion);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateRoundRectRgn(HRGN hRgn, int left, int top, int right, int bottom, int nWidthEllipse, int nHeightEllipse)
{
    nrregionscreated++;
    STAT_InsertObject(hRgn, createdregion);
}
//******************************************************************************
//******************************************************************************
void STATS_ExtCreateRegion(HRGN hRgn, PVOID pXform, DWORD count, const RGNDATA * pData)
{
    nrregionscreated++;
    STAT_InsertObject(hRgn, createdregion);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateEllipticRgn(HRGN hRgn, int left, int top, int right, int bottom)
{
    nrregionscreated++;
    STAT_InsertObject(hRgn, createdregion);
}
//******************************************************************************
//******************************************************************************
void STATS_CreatePolygonRgn(HRGN hRgn, const POINT * lppt, int cPoints, int fnPolyFillMode)
{
    nrregionscreated++;
    STAT_InsertObject(hRgn, createdregion);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDIBitmap(HBITMAP hBitmap,HDC hdc, const BITMAPINFOHEADER *lpbmih,
                          DWORD fdwInit, const void *lpbInit,
                          const BITMAPINFO *lpbmi, UINT fuUsage)
{
    nrbitmapscreated++;
    STAT_InsertObject(hBitmap, createdbitmap);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateCompatibleBitmap(HBITMAP hBitmap,HDC hdc, int nWidth, int nHeight)
{
    nrbitmapscreated++;
    STAT_InsertObject(hBitmap, createdbitmap);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateBitmap(HBITMAP hBitmap,int nWidth, int nHeight, UINT cPlanes,
                        UINT cBitsPerPel, const void *lpvBits)
{
    nrbitmapscreated++;
    STAT_InsertObject(hBitmap, createdbitmap);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateDIBSection(HBITMAP hBitmap,HDC hdc, BITMAPINFO *pbmi, UINT iUsage,
                            VOID **ppvBits, HANDLE hSection, DWORD dwOffset)
{
    nrbitmapscreated++;
    STAT_InsertObject(hBitmap, createdbitmap);
}
//******************************************************************************
//******************************************************************************
void STATS_CreateBitmapIndirect(HBITMAP hBitmap, const BITMAP *pBitmap)
{
    nrbitmapscreated++;
    STAT_InsertObject(hBitmap, createdbitmap);
}
//******************************************************************************
//******************************************************************************
void STATS_DeleteObject(HANDLE hObj, DWORD objtype)
{
    switch(objtype) {
    case OBJ_PEN:
    case OBJ_EXTPEN:
        nrpenscreated--;
        STAT_DeleteObject(hObj, createdpen);
        break;
    case OBJ_BRUSH:
        nrbrushescreated--;
        STAT_DeleteObject(hObj, createdbrush);
        break;
    case OBJ_FONT:
        nrfontscreated--;
        STAT_DeleteObject(hObj, createdfont);
        break;
    case OBJ_REGION:
        nrregionscreated--;
        STAT_DeleteObject(hObj, createdregion);
        break;
    case OBJ_BITMAP:
        nrbitmapscreated--;
        STAT_DeleteObject(hObj, createdbitmap);
        break;

    case OBJ_MEMDC:
    case OBJ_DC:
        nrdcscreated--;
        STAT_DeleteObject(hObj, createddc);
        break;

    case OBJ_PAL:
    case OBJ_METAFILE:
    case OBJ_ENHMETADC:
    case OBJ_ENHMETAFILE:
    case OBJ_METADC:
        break;
    default:
        dprintf(("!ERROR! Unknown object %x of type %d", hObj, objtype));
        break;
    }
}
//******************************************************************************
//******************************************************************************
void STATS_DumpStatsGDI32()
{
    dprintf(("*************  GDI32 STATISTICS BEGIN *****************"));
    dprintf(("Leaked dcs            %d", nrdcscreated));
    STAT_PrintLeakedObjects("Leaked DC", createddc);
    dprintf(("*************  ********************** *****************"));
    dprintf(("Leaked font objects   %d", nrfontscreated));
    STAT_PrintLeakedObjects("Leaked Font", createdfont);
    dprintf(("*************  ********************** *****************"));
    dprintf(("Leaked pen objects    %d", nrpenscreated));
    STAT_PrintLeakedObjects("Leaked Pen", createdpen);
    dprintf(("*************  ********************** *****************"));
    dprintf(("Leaked brush objects  %d", nrbrushescreated));
    STAT_PrintLeakedObjects("Leaked Brush", createdbrush);
    dprintf(("*************  ********************** *****************"));
    dprintf(("Leaked region objects %d", nrregionscreated));
    STAT_PrintLeakedObjects("Leaked Region", createdregion);
    dprintf(("*************  ********************** *****************"));
    dprintf(("Leaked bitmap objects %d", nrbitmapscreated));
    STAT_PrintLeakedObjects("Leaked Bitmap", createdbitmap);
    ObjDumpObjects();
    dprintf(("*************  GDI32 STATISTICS END   *****************"));
}
//******************************************************************************
//******************************************************************************


#endif //DEBUG
