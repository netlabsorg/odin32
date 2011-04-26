/* $Id: devcontext.cpp,v 1.3 2004-02-18 14:05:48 sandervl Exp $ */

/*
 * GDI32 device context apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>
#include "callback.h"
#include "unicode.h"
#include "dibsect.h"
#include <codepage.h>
#include "oslibgpi.h"
#include "oslibgdi.h"
#include <dcdata.h>
#include <winuser32.h>
#include "font.h"
#include <stats.h>
#include <objhandle.h>
#include <winspool.h>
#include "region.h"
#include <wingdi32.h>

#define DBG_LOCALLOG    DBG_devcontext
#include "dbglocal.h"

typedef BOOL (* WIN32API PFN_SPLQUERYPMQUEUENAME)(LPSTR pDeviceName, LPSTR lpszPMQueue, INT cbPMQueue);

#ifdef DEBUG
void dprintfDEVMODE(DEVMODEA *lpInitData);
#else
#define dprintfDEVMODE(a)
#endif

static const char *szDisplay = "DISPLAY";

//******************************************************************************
// GetPMQueueName
//
// Get the PM printer queue name associated with the printer name
//
// NOTE: We can't have a hardcoded dependency on WINSPOOL in GDI32, so get the
//       function address dynamically
//
//******************************************************************************
BOOL WIN32API GetPMQueueName(LPSTR pDeviceName, LPSTR lpszPMQueue, INT cbPMQueue)
{
    static HINSTANCE                hInstance = 0;
    static PFN_SPLQUERYPMQUEUENAME  pfnSplQueryPMQueueName = NULL;

    if(hInstance == 0) hInstance = LoadLibraryA("WINSPOOL.DRV");

    if(hInstance) {
        pfnSplQueryPMQueueName = (PFN_SPLQUERYPMQUEUENAME)GetProcAddress(hInstance, "SplQueryPMQueueName");
        if(pfnSplQueryPMQueueName) {
            return pfnSplQueryPMQueueName(pDeviceName, lpszPMQueue, cbPMQueue);
        }
    }
    DebugInt3();
    return FALSE;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateDCA(LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput, const DEVMODEA *lpInitData)
{
    HDC  hdc;
    char szDevice[256];

    // 2001-05-28 PH
    // Ziff Davis Benchmarks come in here with "display".
    // Obviously, Windows does accept case-insensitive driver names,
    // whereas Open32 doesn't.
    if(lpszDriver && !stricmp(lpszDriver, szDisplay)) {
        lpszDriver = szDisplay;
    }
    else
    { //Check if it's a printer device
        if(GetPMQueueName((LPSTR)lpszDevice, szDevice, sizeof(szDevice)) == TRUE)
        {
            dprintf(("Rename printer %s to PM Queue %s", lpszDevice, szDevice));
            lpszDevice = szDevice;

            //Must ignore port name here or else the wrong queue might be used
            //(unless we are told to print to file)
            if (lpszOutput && strcmp(lpszOutput, "FILE:") != 0 && strcmp(lpszOutput, "FILE") != 0)
                lpszOutput = NULL;
        }
    }

    //SvL: Open32 tests lpszDriver for NULL even though it's ignored
    if(lpszDriver == NULL) {
        lpszDriver = lpszDevice;
    }

    if(lpInitData) {
        dprintfDEVMODE((DEVMODEA *)lpInitData);
    }

    hdc = O32_CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData);
    if(hdc) {
        OSLibGpiSetCp(hdc, GetDisplayCodepage());
        STATS_CreateDCA(hdc, lpszDriver, lpszDevice, lpszOutput, lpInitData);
    }

    dprintf(("GDI32: CreateDCA %s %s %s %x returned %x", lpszDriver, lpszDevice, lpszOutput, lpInitData, hdc));
    return hdc;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateDCW( LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3, const DEVMODEW * arg4)
{
    char *astring4, *astring5;

    char *astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    char *astring2 = UnicodeToAsciiString((LPWSTR)arg2);
    char *astring3 = UnicodeToAsciiString((LPWSTR)arg3);

    if(arg4)
    {
      astring4 = UnicodeToAsciiString((LPWSTR)(arg4->dmDeviceName));
      astring5 = UnicodeToAsciiString((LPWSTR)(arg4->dmFormName));
    }

    HDC   rc;
    DEVMODEA devmode;

    if(arg4)
    {
      strcpy((char*)devmode.dmDeviceName, astring4);
      strcpy((char*)devmode.dmFormName, astring5);

      devmode.dmSpecVersion      = arg4->dmSpecVersion;
      devmode.dmDriverVersion    = arg4->dmDriverVersion;
      devmode.dmSize             = arg4->dmSize;
      devmode.dmDriverExtra      = arg4->dmDriverExtra;
      devmode.dmFields           = arg4->dmFields;
#if (__IBMCPP__ == 360)
      devmode.dmOrientation   = arg4->dmOrientation;
      devmode.dmPaperSize     = arg4->dmPaperSize;
      devmode.dmPaperLength   = arg4->dmPaperLength;
      devmode.dmPaperWidth    = arg4->dmPaperWidth;
#else
      devmode.s1.dmOrientation   = arg4->s1.dmOrientation;
      devmode.s1.dmPaperSize     = arg4->s1.dmPaperSize;
      devmode.s1.dmPaperLength   = arg4->s1.dmPaperLength;
      devmode.s1.dmPaperWidth    = arg4->s1.dmPaperWidth;
#endif
      devmode.dmScale            = arg4->dmScale;
      devmode.dmCopies           = arg4->dmCopies;
      devmode.dmDefaultSource    = arg4->dmDefaultSource;
      devmode.dmPrintQuality     = arg4->dmPrintQuality;
      devmode.dmColor            = arg4->dmColor;
      devmode.dmDuplex           = arg4->dmDuplex;
      devmode.dmYResolution      = arg4->dmYResolution;
      devmode.dmTTOption         = arg4->dmTTOption;
      devmode.dmCollate          = arg4->dmCollate;
      devmode.dmLogPixels        = arg4->dmLogPixels;
      devmode.dmBitsPerPel       = arg4->dmBitsPerPel;
      devmode.dmPelsWidth        = arg4->dmPelsWidth;
      devmode.dmPelsHeight       = arg4->dmPelsHeight;
      devmode.dmDisplayFlags     = arg4->dmDisplayFlags;
      devmode.dmDisplayFrequency = arg4->dmDisplayFrequency;
      devmode.dmICMMethod        = arg4->dmICMMethod;
      devmode.dmICMIntent        = arg4->dmICMIntent;
      devmode.dmMediaType        = arg4->dmMediaType;
      devmode.dmDitherType       = arg4->dmDitherType;
      devmode.dmReserved1        = arg4->dmReserved1;
      devmode.dmReserved2        = arg4->dmReserved2;
      rc = CreateDCA(astring1,astring2,astring3,&devmode);
    }
    else
      rc = CreateDCA(astring1,astring2,astring3, NULL);

    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    FreeAsciiString(astring3);

    if(arg4)
    {
      FreeAsciiString(astring4);
      FreeAsciiString(astring5);
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateICA(LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                       const DEVMODEA *lpdvmInit)
{
    HDC  hdc;
    char szDevice[256];

    //SvL: Open32 tests for "DISPLAY"
    if(lpszDriver && !stricmp(lpszDriver, szDisplay)) {
        lpszDriver = szDisplay;
    }
    else
    { //Check if it's a printer device
        if(GetPMQueueName((LPSTR)lpszDevice, szDevice, sizeof(szDevice)) == TRUE)
        {
            dprintf(("Rename printer %s to PM Queue %s", lpszDevice, szDevice));
            lpszDevice = szDevice;
            //Must ignore port name here or else the wrong queue might be used
            //(unless we are told to print to file)
            if (lpszOutput && strcmp(lpszOutput, "FILE:") != 0 && strcmp(lpszOutput, "FILE") != 0)
                lpszOutput = NULL;
        }
    }

    //SvL: Open32 tests lpszDriver for NULL even though it's ignored
    if(lpszDriver == NULL) {
        lpszDriver = lpszDevice;
    }
    hdc = O32_CreateIC(lpszDriver, lpszDevice, lpszOutput, lpdvmInit);

    dprintf(("GDI32: CreateICA %s %s %s %x returned %x", lpszDriver, lpszDevice, lpszOutput, lpdvmInit, hdc));

    if(hdc) STATS_CreateICA(hdc, lpszDriver, lpszDevice, lpszOutput, lpdvmInit);
    return hdc;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateICW( LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3, const DEVMODEW * arg4)
{
    char *astring4, *astring5;

    char *astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    char *astring2 = UnicodeToAsciiString((LPWSTR)arg2);
    char *astring3 = UnicodeToAsciiString((LPWSTR)arg3);
    if(arg4)
    {
      astring4 = UnicodeToAsciiString((LPWSTR)(arg4->dmDeviceName));
      astring5 = UnicodeToAsciiString((LPWSTR)(arg4->dmFormName));
    }

    HDC   rc;
    DEVMODEA devmode;

    if(arg4)
    {
      strcpy((char*)devmode.dmDeviceName, astring4);
      strcpy((char*)devmode.dmFormName, astring5);

      devmode.dmSpecVersion      = arg4->dmSpecVersion;
      devmode.dmDriverVersion    = arg4->dmDriverVersion;
      devmode.dmSize             = arg4->dmSize;
      devmode.dmDriverExtra      = arg4->dmDriverExtra;
      devmode.dmFields           = arg4->dmFields;
#if (__IBMCPP__ == 360)
      devmode.dmOrientation      = arg4->dmOrientation;
      devmode.dmPaperSize        = arg4->dmPaperSize;
      devmode.dmPaperLength      = arg4->dmPaperLength;
      devmode.dmPaperWidth       = arg4->dmPaperWidth;
#else
      devmode.s1.dmOrientation   = arg4->s1.dmOrientation;
      devmode.s1.dmPaperSize     = arg4->s1.dmPaperSize;
      devmode.s1.dmPaperLength   = arg4->s1.dmPaperLength;
      devmode.s1.dmPaperWidth    = arg4->s1.dmPaperWidth;
#endif
      devmode.dmScale            = arg4->dmScale;
      devmode.dmCopies           = arg4->dmCopies;
      devmode.dmDefaultSource    = arg4->dmDefaultSource;
      devmode.dmPrintQuality     = arg4->dmPrintQuality;
      devmode.dmColor            = arg4->dmColor;
      devmode.dmDuplex           = arg4->dmDuplex;
      devmode.dmYResolution      = arg4->dmYResolution;
      devmode.dmTTOption         = arg4->dmTTOption;
      devmode.dmCollate          = arg4->dmCollate;
      devmode.dmLogPixels        = arg4->dmLogPixels;
      devmode.dmBitsPerPel       = arg4->dmBitsPerPel;
      devmode.dmPelsWidth        = arg4->dmPelsWidth;
      devmode.dmPelsHeight       = arg4->dmPelsHeight;
      devmode.dmDisplayFlags     = arg4->dmDisplayFlags;
      devmode.dmDisplayFrequency = arg4->dmDisplayFrequency;
      devmode.dmICMMethod        = arg4->dmICMMethod;
      devmode.dmICMIntent        = arg4->dmICMIntent;
      devmode.dmMediaType        = arg4->dmMediaType;
      devmode.dmDitherType       = arg4->dmDitherType;
      devmode.dmReserved1        = arg4->dmReserved1;
      devmode.dmReserved2        = arg4->dmReserved2;

      rc = CreateICA(astring1,astring2,astring3,&devmode);
    }
    else
      rc = CreateICA(astring1,astring2,astring3, NULL);

    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    FreeAsciiString(astring3);
    if(arg4)
    {
      FreeAsciiString(astring4);
      FreeAsciiString(astring5);
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateCompatibleDC( HDC hdc)
{
 HDC newHdc;

    newHdc = O32_CreateCompatibleDC(hdc);
    ULONG oldcp = OSLibGpiQueryCp(hdc);
    if (!oldcp) /* If new DC is to be created */
        oldcp = GetDisplayCodepage();

    if(newHdc) STATS_CreateCompatibleDC(hdc, newHdc);
    OSLibGpiSetCp(newHdc, oldcp);
    //PF Open32 seems not to move coordinates to 0,0 in newHdc
    MoveToEx(newHdc, 0, 0 , NULL);

    return newHdc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteDC(HDC hdc)
{
    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: DeleteDC %x; invalid hdc!", hdc));
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }
    SetLastError(ERROR_SUCCESS);

    DIBSection *dsect = DIBSection::findHDC(hdc);
    if(dsect)
    {
        //remove previously selected dibsection
        dprintf(("DeleteDC %x, unselect DIB section %x", hdc, dsect->GetBitmapHandle()));
        dsect->UnSelectDIBObject();
    }

    //Must call ReleaseDC for window dcs
    if(pHps->hdcType == TYPE_1) {
        return ReleaseDC(OS2ToWin32Handle(pHps->hwnd), hdc);
    }

    STATS_DeleteDC(hdc);
    return O32_DeleteDC(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API SaveDC( HDC hdc)
{
    int      id;
    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: SaveDC %x; invalid hdc!", hdc));
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }

    HRGN hClipRgn = 0;
    if(pHps->hrgnWin32Clip) {
        // Make a copy of our current clip region
        // (the visible region remains untouched!)
        hClipRgn = GdiCopyClipRgn(pHps);
    }

    id = O32_SaveDC(hdc);
    if(id == 0) {
         dprintf(("ERROR: GDI32: SaveDC %x FAILED", hdc));
         if(hClipRgn) GdiDestroyRgn(pHps, hClipRgn);
         return 0;
    }
    //overwrite the current clip region with the copy
    dprintf2(("New win32 clip region %x", hClipRgn));
    pHps->hrgnWin32Clip = hClipRgn;
    return id;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RestoreDC(HDC hdc, int id)
{
    BOOL ret;
    HRGN hrgnOldClip = 0;

    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: RestoreDC %x; invalid hdc!", hdc));
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }

    hrgnOldClip = pHps->hrgnWin32Clip;
    ret = O32_RestoreDC(hdc, id);
    if(ret == FALSE) {
        dprintf(("ERROR: GDI32: RestoreDC %x %d FAILED", hdc, id));
    }
    else {
        //Destroy copy of the clip region that we made in SaveDC
        if(hrgnOldClip) GdiDestroyRgn(pHps, hrgnOldClip);

        // Activate previous clip region
        GdiCombineVisRgnClipRgn(pHps, pHps->hrgnWin32Clip, RGN_AND);

        dprintf2(("New win32 clip region %x", pHps->hrgnWin32Clip));
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCA(HDC hdc, const DEVMODEA *lpInitData)
{
    if(lpInitData)
    {
        dprintfDEVMODE((DEVMODEA *)lpInitData);
   }
   HDC ret =O32_ResetDC(hdc, lpInitData);
   return ret;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCW( HDC arg1, const DEVMODEW *  arg2)
{
    dprintf(("GDI32: ResetDCW: not properly implemented"));
    DebugInt3();
    // NOTE: This will not work as is (needs UNICODE support)
    return (HDC)O32_ResetDC(arg1, (const DEVMODEA *)arg2);
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
void dprintfDEVMODE(DEVMODEA *lpInitData)
{
        dprintf(("devmode.dmSpecVersion %x", lpInitData->dmSpecVersion));
        dprintf(("devmode.dmDriverVersion %x",lpInitData->dmDriverVersion));
        dprintf(("devmode.dmSize %x", lpInitData->dmSize));
        dprintf(("devmode.dmDriverExtra %x", lpInitData->dmDriverExtra));
        dprintf(("devmode.dmFields %x", lpInitData->dmFields));
#if (__IBMCPP__ == 360)
        dprintf(("devmode.dmOrientation %x", lpInitData->dmOrientation));
        dprintf(("devmode.dmPaperSize %x", lpInitData->dmPaperSize));
        dprintf(("devmode.dmPaperLength %x", lpInitData->dmPaperLength));
        dprintf(("devmode.dmPaperWidth %x", lpInitData->dmPaperWidth));
#else
        dprintf(("devmode.dmOrientation %x", lpInitData->s1.dmOrientation));
        dprintf(("devmode.dmPaperSize %x", lpInitData->s1.dmPaperSize));
        dprintf(("devmode.dmPaperLength %x", lpInitData->s1.dmPaperLength));
        dprintf(("devmode.dmPaperWidth %x", lpInitData->s1.dmPaperWidth));
#endif
        dprintf(("devmode.dmScale %x", lpInitData->dmScale));
        dprintf(("devmode.dmCopies %x", lpInitData->dmCopies));
        dprintf(("devmode.dmDefaultSource %x", lpInitData->dmDefaultSource));
        dprintf(("devmode.dmPrintQuality %x", lpInitData->dmPrintQuality));
        dprintf(("devmode.dmColor %x", lpInitData->dmColor));
        dprintf(("devmode.dmDuplex %x", lpInitData->dmDuplex));
        dprintf(("devmode.dmYResolution %x", lpInitData->dmYResolution));
        dprintf(("devmode.dmTTOption %x", lpInitData->dmTTOption));
        dprintf(("devmode.dmCollate %x", lpInitData->dmCollate));
        dprintf(("devmode.dmLogPixels %x", lpInitData->dmLogPixels));
        dprintf(("devmode.dmBitsPerPel %x", lpInitData->dmBitsPerPel));
        dprintf(("devmode.dmPelsWidth %x", lpInitData->dmPelsWidth));
        dprintf(("devmode.dmPelsHeight %x", lpInitData->dmPelsHeight));
        dprintf(("devmode.dmDisplayFlags %x", lpInitData->dmDisplayFlags));
        dprintf(("devmode.dmDisplayFrequency %x", lpInitData->dmDisplayFrequency));
        dprintf(("devmode.dmICMMethod %x", lpInitData->dmICMMethod));
        dprintf(("devmode.dmICMIntent %x", lpInitData->dmICMIntent));
        dprintf(("devmode.dmMediaType %x", lpInitData->dmMediaType));
        dprintf(("devmode.dmDitherType %x", lpInitData->dmDitherType));
        dprintf(("devmode.dmReserved1 %x", lpInitData->dmReserved1));
        dprintf(("devmode.dmReserved2 %x", lpInitData->dmReserved2));
}
#endif
//******************************************************************************
//******************************************************************************
