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

#ifdef RAS

RAS_TRACK_HANDLE rthFont;
RAS_TRACK_HANDLE rthDC;
RAS_TRACK_HANDLE rthPen;
RAS_TRACK_HANDLE rthBrush;
RAS_TRACK_HANDLE rthRgn;
RAS_TRACK_HANDLE rthBmp;

void STATS_CreateFontIndirect(HFONT hFont, LOGFONTA* lplf)
{
    if (!rthFont) STATS_InitializeGDI32 ();
    RasAddObject (rthFont, hFont, lplf, sizeof (LOGFONTA));
}
void STATS_DeleteFont(HFONT hFont)
{
    RasRemoveObject (rthFont, hFont);
}
void STATS_CreateCompatibleDC(HDC hdc, HDC hdcCreated)
{
    if (!rthDC) STATS_InitializeGDI32 ();
    RasAddObject (rthDC, hdcCreated, &hdc, sizeof (hdc));
}
void STATS_CreateICA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit)
{
    if (!rthDC) STATS_InitializeGDI32 ();
    RasAddObject (rthDC, hdc, NULL, 0);
}
void STATS_CreateDCA(HDC hdc, LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                     const DEVMODEA *lpdvmInit)
{
    if (!rthDC) STATS_InitializeGDI32 ();
    RasAddObject (rthDC, hdc, NULL, 0);
}
void STATS_DeleteDC(HDC hdc)
{
    RasRemoveObject (rthDC, hdc);
}
void STATS_CreatePatternBrush(HBRUSH hBrush, HBITMAP hBitmap)
{
    if (!rthBrush) STATS_InitializeGDI32 ();
    RasAddObject (rthBrush, hBrush, &hBitmap, sizeof (hBitmap));
}
void STATS_CreateDIBPatternBrushPt(HBRUSH hBrush, LPCVOID buffer, DWORD usage)
{
    if (!rthBrush) STATS_InitializeGDI32 ();
    RasAddObject (rthBrush, hBrush, &buffer, sizeof (buffer));
}
void STATS_CreateBrushIndirect(HBRUSH hBrush, LPLOGBRUSH pLogBrush)
{
    if (!rthBrush) STATS_InitializeGDI32 ();
    RasAddObject (rthBrush, hBrush, pLogBrush, sizeof (LOGBRUSH));
}
void STATS_CreateHatchBrush(HBRUSH hBrush, int fnStyle, COLORREF clrref)
{
    struct Parms
    {
        int fnStyle; 
        COLORREF clrref;
    } parms = {fnStyle, clrref};
    if (!rthBrush) STATS_InitializeGDI32 ();
    RasAddObject (rthBrush, hBrush, &parms, sizeof (parms));
}
void STATS_CreateSolidBrush(HBRUSH hBrush, COLORREF color)
{
    if (!rthBrush) STATS_InitializeGDI32 ();
    RasAddObject (rthBrush, hBrush, &color, sizeof (COLORREF));
}
void STATS_DeleteBrush(HBRUSH hBrush)
{
    RasRemoveObject (rthBrush, hBrush);
}
void STATS_CreatePenIndirect(HPEN hPen, const LOGPEN *lplgpn)
{
    if (!rthPen) STATS_InitializeGDI32 ();
    RasAddObject (rthPen, hPen, (void *)lplgpn, sizeof (LOGPEN));
}
void STATS_CreatePen(HPEN hPen, int fnPenStyle, int nWidth, COLORREF crColor)
{
    struct Parms
    {
        int fnPenStyle;
        int nWidth; 
        COLORREF crColor;
    } parms = {fnPenStyle, nWidth, crColor};
    
    if (!rthPen) STATS_InitializeGDI32 ();
    RasAddObject (rthPen, hPen, &parms, sizeof (parms));
}
void STATS_ExtCreatePen(HPEN hPen, DWORD dwPenStyle, DWORD dwWidth, const LOGBRUSH *lplb, 
                        DWORD dwStyleCount, const DWORD *lpStyle)
{
    struct Parms
    {
        DWORD dwPenStyle; 
        DWORD dwWidth; 
        const LOGBRUSH *lplb;
        DWORD dwStyleCount; 
        const DWORD *lpStyle;
    } parms = {dwPenStyle, dwWidth, lplb, 
               dwStyleCount, lpStyle};
    
    if (!rthPen) STATS_InitializeGDI32 ();
    RasAddObject (rthPen, hPen, &parms, sizeof (parms));
}
void STATS_DeletePen(HPEN hPen)
{
    RasRemoveObject (rthPen, hPen);
}

void STATS_CreatePolyPolygonRgn(HRGN hRgn, const POINT *lppt, const int *pPolyCount, int nCount, int fnPolyFillMode)
{
    if (!rthRgn) STATS_InitializeGDI32 ();
    RasAddObject (rthRgn, hRgn, NULL, 0);
}
void STATS_CreateRectRgn(HRGN hRgn, int left, int top, int right, int bottom)
{
    if (!rthRgn) STATS_InitializeGDI32 ();
    RasAddObject (rthRgn, hRgn, NULL, 0);
}
void STATS_CreateRoundRectRgn(HRGN hRgn, int left, int top, int right, int bottom, int nWidthEllipse, int nHeightEllipse)
{
    if (!rthRgn) STATS_InitializeGDI32 ();
    RasAddObject (rthRgn, hRgn, NULL, 0);
}
void STATS_ExtCreateRegion(HRGN hRgn, PVOID pXform, DWORD count, const RGNDATA * pData)
{
    if (!rthRgn) STATS_InitializeGDI32 ();
    RasAddObject (rthRgn, hRgn, NULL, 0);
}
void STATS_CreateEllipticRgn(HRGN hRgn, int left, int top, int right, int bottom)
{
    if (!rthRgn) STATS_InitializeGDI32 ();
    RasAddObject (rthRgn, hRgn, NULL, 0);
}
void STATS_CreatePolygonRgn(HRGN hRgn, const POINT * lppt, int cPoints, int fnPolyFillMode)
{
    if (!rthRgn) STATS_InitializeGDI32 ();
    RasAddObject (rthRgn, hRgn, NULL, 0);
}
void STATS_DeleteRgn(HRGN hRgn)
{
    RasRemoveObject (rthRgn, hRgn);
}

void STATS_CreateDIBitmap(HBITMAP hBitmap,HDC hdc, const BITMAPINFOHEADER *lpbmih,
                          DWORD fdwInit, const void *lpbInit,
                          const BITMAPINFO *lpbmi, UINT fuUsage)
{
    if (!rthBmp) STATS_InitializeGDI32 ();
    RasAddObject (rthBmp, hBitmap, NULL, 0);
}
void STATS_CreateCompatibleBitmap(HBITMAP hBitmap,HDC hdc, int nWidth, int nHeight)
{
    if (!rthBmp) STATS_InitializeGDI32 ();
    RasAddObject (rthBmp, hBitmap, NULL, 0);
}
void STATS_CreateBitmap(HBITMAP hBitmap,int nWidth, int nHeight, UINT cPlanes,
                        UINT cBitsPerPel, const void *lpvBits)
{
    if (!rthBmp) STATS_InitializeGDI32 ();
    RasAddObject (rthBmp, hBitmap, NULL, 0);
}
void STATS_CreateDIBSection(HBITMAP hBitmap,HDC hdc, BITMAPINFO *pbmi, UINT iUsage,
                            VOID **ppvBits, HANDLE hSection, DWORD dwOffset)
{
    if (!rthBmp) STATS_InitializeGDI32 ();
    RasAddObject (rthBmp, hBitmap, NULL, 0);
}
void STATS_CreateBitmapIndirect(HBITMAP hBitmap, const BITMAP *pBitmap)
{
    if (!rthBmp) STATS_InitializeGDI32 ();
    RasAddObject (rthBmp, hBitmap, NULL, 0);
}
void STATS_DeleteBitmap(HBITMAP hBitmap)
{
    RasRemoveObject (rthBmp, hBitmap);
}

void STATS_DeleteObject(HANDLE hObj, DWORD objtype)
{
    switch(objtype) {
    case OBJ_PEN:
    case OBJ_EXTPEN:
        STATS_DeletePen(hObj);
        break;
    case OBJ_BRUSH:
        STATS_DeleteBrush(hObj);
        break;
    case OBJ_FONT:
        STATS_DeleteFont(hObj);
        break;
    case OBJ_REGION:
        STATS_DeleteRgn(hObj);
        break;
    case OBJ_BITMAP:
        STATS_DeleteBitmap(hObj);
        break;

    case OBJ_MEMDC:
    case OBJ_DC:
        STATS_DeleteDC(hObj);
        break;

    case OBJ_PAL:
    case OBJ_METAFILE:
    case OBJ_ENHMETADC:
    case OBJ_ENHMETAFILE:
    case OBJ_METADC:
        break;
    default:
        RasLog ("!ERROR! Unknown object %x of type %d", hObj, objtype);
        break;
    }
}

void STATS_DumpStatsGDI32()
{
    RasLogObjects(rthDC, RAS_FLAG_LOG_OBJECTS);
    RasLogObjects(rthPen, RAS_FLAG_LOG_OBJECTS);
    RasLogObjects(rthBrush, RAS_FLAG_LOG_OBJECTS);
    RasLogObjects(rthRgn, RAS_FLAG_LOG_OBJECTS);
    RasLogObjects(rthBmp, RAS_FLAG_LOG_OBJECTS);
}

void STATS_InitializeGDI32(void)
{
    RasEnterSerialize ();
    if (!rthDC)
    {
        RasRegisterObjectTracking(&rthDC, "DC", 0, 0, NULL, NULL);
    }
    if (!rthPen)
    {
        RasRegisterObjectTracking(&rthPen, "Pen", 0, 0, NULL, NULL);
    }
    if (!rthBrush)
    {
        RasRegisterObjectTracking(&rthBrush, "Brush", 0, 0, NULL, NULL);
    }
    if (!rthRgn)
    {
        RasRegisterObjectTracking(&rthRgn, "Region", 0, 0, NULL, NULL);
    }
    if (!rthBmp)
    {
        RasRegisterObjectTracking(&rthBmp, "Bitmap", 0, 0, NULL, NULL);
    }
    RasExitSerialize ();
}

void STATS_UninitializeGDI32(void)
{
// done in RAS kernel    
//    RasDeregisterObjectTracking(rthDC);
//    RasDeregisterObjectTracking(rthPen);
//    RasDeregisterObjectTracking(rthBrush);
//    RasDeregisterObjectTracking(rthRgn);
//    RasDeregisterObjectTracking(rthBmp);
}

#else
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
void STATS_InitializeGDI32(void)
{
    return;
}

void STATS_UninitializeGDI32(void)
{
    return;
}


#endif //DEBUG
#endif //RAS
