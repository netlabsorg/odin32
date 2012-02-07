/*
 * GDI32 FreeType2 Support Class
 *
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *                                            (stauff@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define  INCL_DOS
#define  INCL_GPI
#define  INCL_WIN
#define  INCL_SHLERRORS
#define  INCL_WINERRORS
#include <os2wrap.h>  //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <dbglog.h>
#include <win32api.h>
#include <winconst.h>
#include <winnls.h>
#include <heapstring.h>
#include <winuser32.h>
#include <odinlx.h>
#include "oslibgpi.h"

#include "ft2supp.h"

CFT2Module FT2Module;

static BOOL fFT2LIBIntegration = FALSE;

//******************************************************************************
//******************************************************************************
void WIN32API SetFreeTypeIntegration(BOOL fEnabled)
{
    fFT2LIBIntegration = fEnabled;

    FT2Module.init();
}
//******************************************************************************
// Constructor
//******************************************************************************
CFT2Module::CFT2Module(const char* sModuleName): bEnabled(FALSE), hftModule(0), pfnGetGlyphIndices(NULL),
                pfnFt2GetTextExtentW(NULL), pfnFt2EnableFontEngine(NULL),
                pfnFt2GetGlyphOutline(NULL), pfnFt2CharStringPosAtA(NULL),
                pfnFt2CharStringPosAtW(NULL), pfnFt2GetFontData(NULL),
                pfnFt2RegisterUconv(NULL), pfnFt2QueryStringWidthW(NULL),
                pfnFt2GetVersion(NULL)
{
    pszModuleName = sModuleName; //must be static
}
//******************************************************************************
//******************************************************************************
void CFT2Module::init()
{
    APIRET rc;
    UCHAR Loaderror[ 256 ];
    LONG major = 0, minor = 0, buildnr = 0;

    if(!fFT2LIBIntegration) {
        dprintf(("No FT2LIB integration!!"));
        goto failure;
    }

    rc = DosLoadModule((PSZ)Loaderror, sizeof(Loaderror), pszModuleName, &hftModule);
    if (rc != 0)
        dprintf(("Freetype2 library load error: return code = %u\n", rc));
    else
       bEnabled = TRUE;

    if(bEnabled) {
       pfnFt2GetVersion = (PFN_FT2GETVERSION)QueryProcAddress("Ft2GetVersion");
       if(!pfnFt2GetVersion) {
           dprintf(("Ft2GetVersion not found!!"));
           goto failure;
       }
       pfnFt2GetVersion(&major, &minor, &buildnr);
       if(major != FT2LIB_MAJOR_VERSION) {
           dprintf(("Incorrect major version %d; expected %d!!", major, FT2LIB_MAJOR_VERSION));
           goto failure;
       }
       if(minor < FT2LIB_MINOR_VERSION) {
           dprintf(("Incorrect minor version %d; expected >= %d!!", minor, FT2LIB_MINOR_VERSION));
           goto failure;
       }

       pfnFt2EnableFontEngine = (PFN_FT2ENABLEFONTENGINE)QueryProcAddress("Ft2EnableFontEngine");
       if(!pfnFt2EnableFontEngine) dprintf(("Ft2EnableFontEngine not found!!"));
       else pfnFt2EnableFontEngine(bEnabled);

       pfnGetGlyphIndices = (PFN_FT2GETGLYPHINDICES)QueryProcAddress("Ft2GetGlyphIndices");
       if(!pfnGetGlyphIndices) dprintf(("Ft2GetGlyphIndices not found!!"));

       pfnFt2GetTextExtentW  = (PFN_FT2GETTEXTEXTENTW)QueryProcAddress("Ft2GetTextExtentW");
       if(!pfnFt2GetTextExtentW) dprintf(("Ft2GetTextExtentW not found!!"));

       pfnFt2CharStringPosAtA = (PFN_FT2CHARSTRINGPOSATA)QueryProcAddress("Ft2CharStringPosAtA");
       if(!pfnFt2CharStringPosAtA) dprintf(("Ft2CharStringPosAtA not found!!"));
       pfnFt2CharStringPosAtW = (PFN_FT2CHARSTRINGPOSATW)QueryProcAddress("Ft2CharStringPosAtW");
       if(!pfnFt2CharStringPosAtW) dprintf(("Ft2CharStringPosAtW not found!!"));

       pfnFt2GetGlyphOutline = (PFN_FT2GETGLYPHOUTLINE)QueryProcAddress("Ft2GetGlyphOutline");
       if(!pfnFt2GetGlyphOutline) dprintf(("Ft2GetGlyphOutline not found!!"));

       pfnFt2GetFontData = (PFN_FT2GETFONTDATA)QueryProcAddress("Ft2GetFontData");
       if(!pfnFt2GetFontData) dprintf(("Ft2GetFontData not found!!"));

       pfnFt2QueryFontType = (PFN_FT2QUERYFONTTYPE)QueryProcAddress("Ft2QueryFontType");
       if(!pfnFt2QueryFontType) dprintf(("Ft2QueryFontType not found!!"));

       pfnFt2QueryStringWidthW = (PFN_FT2QUERYSTRINGWIDTHW)QueryProcAddress("Ft2QueryStringWidthW");
       if(!pfnFt2QueryStringWidthW) dprintf(("Ft2QueryStringWidthW not found!!"));

       pfnFt2GetCharacterPlacementW = (PFN_FT2GETCHARACTERPLACEMENTW)QueryProcAddress("Ft2GetCharacterPlacementW");
       if(!pfnFt2GetCharacterPlacementW) dprintf(("pfnFt2GetCharacterPlacementW not found!!"));

       // Do not register functions for Mozilla plugins
       if(IsDummyExeLoaded() == FALSE)
       {
           pfnFt2RegisterUconv = (PFN_FT2REGISTERUCONV)QueryProcAddress("Ft2RegisterUconv");
           if(pfnFt2RegisterUconv)
                pfnFt2RegisterUconv(WideCharToMultiByte, MultiByteToWideChar);
           else dprintf(("Ft2QueryFontType not found!!"));
       }
       dprintf(("Freetype2 library enabled state %d",bEnabled));
    }
    return;

failure:
    if(pfnFt2RegisterUconv)
        pfnFt2RegisterUconv(NULL, NULL);
    if(pfnFt2EnableFontEngine)
        pfnFt2EnableFontEngine(FALSE);

    pfnGetGlyphIndices     = NULL;
    pfnFt2GetTextExtentW   = NULL;
    pfnFt2EnableFontEngine = NULL;
    pfnFt2GetGlyphOutline  = NULL;
    pfnFt2CharStringPosAtA = NULL;
    pfnFt2CharStringPosAtW = NULL;
    pfnFt2GetFontData      = NULL;
    pfnFt2RegisterUconv    = NULL;
    pfnFt2QueryStringWidthW= NULL;
    pfnFt2GetVersion       = NULL;
    bEnabled = FALSE;
    if (hftModule) {
        DosFreeModule(hftModule);
        hftModule = 0;
    }
    return;
}
//******************************************************************************
// Destructor
//******************************************************************************
CFT2Module::~CFT2Module()
{
    if (hftModule)
        DosFreeModule(hftModule);

    bEnabled = FALSE;
}
//******************************************************************************
//******************************************************************************
PFN CFT2Module::QueryProcAddress(int ordinal)
{
    APIRET rc;
    PFN ModuleAddr;

    rc = DosQueryProcAddr(hftModule, ordinal, NULL, &ModuleAddr);
    if (rc != 0) {
        dprintf(("FreeType2 QueryProcAddr error: return code = %u\n", rc));
        return 0;
    }
    return ModuleAddr;
}
//******************************************************************************
//******************************************************************************
PFN CFT2Module::QueryProcAddress(const char * procname)
{
    APIRET rc;
    PFN ModuleAddr;

    rc = DosQueryProcAddr(hftModule, 0, procname, &ModuleAddr);
    if (rc != 0) {
        dprintf(("FreeType2 QueryProcAddr error: return code = %u\n", rc));
        return 0;
    }
    return ModuleAddr;
}
//******************************************************************************
//******************************************************************************
DWORD CFT2Module::Ft2GetGlyphIndices(HPS hps, LPCWSTR str, int c, LPWORD pgi, DWORD fl)
{
    DWORD  ret;
    USHORT sel;

    // All FreeType calls should be wrapped for saving FS
    if(pfnGetGlyphIndices) {
        sel  = RestoreOS2FS();
        ret  = pfnGetGlyphIndices(hps, (WCHAR*)str, c, (ULONG*)pgi, fl);
        SetFS(sel);
        return ret;
    }
    //no fallback
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED_W);
    return GDI_ERROR;
}
//******************************************************************************
//******************************************************************************
DWORD CFT2Module::Ft2GetGlyphOutline(HPS hps, UINT glyph, UINT format, LPGLYPHMETRICS lpgm, DWORD buflen, LPVOID buf, const MAT2* lpmat)
{
    DWORD  ret;
    USHORT sel;

    // All FreeType calls should be wrapped for saving FS
    if (pfnFt2GetGlyphOutline)
    {
        sel  = RestoreOS2FS();
        ret  = pfnFt2GetGlyphOutline (hps, glyph, format, lpgm, buflen, buf, lpmat);
        SetFS(sel);
        return ret;
    }

    //no fallback
    SetLastError(ERROR_INVALID_FUNCTION_W);
    return GDI_ERROR;
}
//******************************************************************************
//
// This is basically the same as Ft2QueryTextBoxW, but it behaves as the Win32
// API GetTextExtent (which ignores character attributes and underhang/overhang)
//
// The fallback case is not accurate!! (but the same as our old code)
//
//******************************************************************************
BOOL CFT2Module::Ft2GetTextExtentW(HPS hps, LONG lCount1,LPCWSTR pchString, PPOINTLOS2 pwidthHeight)
{
    DWORD      ret;
    USHORT     sel;
    POINTLOS2  aptlPoints[TXTBOX_COUNT];

    // All FreeType calls should be wrapped for saving FS
    if(pfnFt2GetTextExtentW)
    {
        sel  = RestoreOS2FS();
        ret  = pfnFt2GetTextExtentW(hps, lCount1, pchString, TXTBOX_COUNT, aptlPoints);
        SetFS(sel);
        if(ret || (ret == FALSE && ERRORIDERROR(WinGetLastError(0)) != PMERR_FUNCTION_NOT_SUPPORTED))
        {
            calcDimensions(aptlPoints, pwidthHeight);
            return ret;
        }
    }
    //else fall back to GPI
    INT lenA;
    LPSTR strA;
    POINTLOS2 start = { 0, 0 };
    PPOINTLOS2 pplos2;
    INT cx;
    INT cy;

    pDCData pHps = (pDCData)OSLibGpiQueryDCData(hps);

    lenA = WideCharToMultiByte( CP_ACP, 0, pchString, lCount1, 0, 0, 0, 0 );
    strA = ( LPSTR )malloc( lenA + 1 );
    lstrcpynWtoA( strA, pchString, lenA + 1 );
    pplos2 = ( PPOINTLOS2 )malloc(( lenA + 1 ) * sizeof( POINTLOS2 ));

    ret = OSLibGpiQueryCharStringPosAt( pHps, &start, 0, lenA, strA, NULL, pplos2 );
    if( ret )
    {
        TEXTMETRICW tmW;

        cx = labs( pplos2[ lenA ].x - pplos2[ 0 ].x );
        cy = labs( pplos2[ lenA ].y - pplos2[ 0 ].y );

        aptlPoints[ TXTBOX_BOTTOMLEFT ].x = 0;
        aptlPoints[ TXTBOX_BOTTOMLEFT ].y = 0;
        aptlPoints[ TXTBOX_BOTTOMRIGHT ].x = cx;
        aptlPoints[ TXTBOX_BOTTOMRIGHT ].y = cy;
        aptlPoints[ TXTBOX_TOPLEFT ].x = 0;
        aptlPoints[ TXTBOX_TOPLEFT ].y = 0;
        aptlPoints[ TXTBOX_TOPRIGHT ].x = cx;
        aptlPoints[ TXTBOX_TOPRIGHT ].y = cy;
        aptlPoints[ TXTBOX_CONCAT ].x = cx;
        aptlPoints[ TXTBOX_CONCAT ].y = cy;

        calcDimensions(aptlPoints, pwidthHeight);

        DecreaseLogCount();
        if(GetTextMetricsW( hps, &tmW ) == TRUE)
        {
            pwidthHeight->y = tmW.tmHeight;    // *Must* use the maximum height of the font
        }
#ifdef DEBUG
        else DebugInt3();
#endif
        IncreaseLogCount();
    }

    free( pplos2 );
    free( strA );

    return ret;
}
//******************************************************************************
//******************************************************************************
struct CTISTATE
{
    pDCData pHps;
    BOOL restoreInversion;
    BOOL restoreMatrix;
    LONG oldYInversion;
    MATRIXLF mlf;
};

static void CompensateTextInversion(pDCData pHps, PPOINTLOS2 ptl, PRECTLOS2 rct,
                                    CTISTATE *state)
{
    state->pHps = pHps;
    state->restoreInversion = FALSE;
    state->restoreMatrix = FALSE;

    // The target HPS is most likely set up for Y inversion, to bring the origin
    // from the bottom-left corner (PM) to the top-left (Win). However, PM text
    // drawing routines make an exception for themselves and produce correct
    // top-left oriented glyphs despite the bottom-left oriented coordinate
    // space. Having the mentioned Y inversion makes them flip and display
    // glyphs top to bottom. We need to cancel this flip to get the correct
    // text orientation.

    GpiQueryDefaultViewMatrix(pHps->hps, 9, &state->mlf);

    if (state->mlf.fxM11 == MAKEFIXED(1, 0) && state->mlf.fxM12 == 0 &&
        state->mlf.lM13 == 0 &&
        state->mlf.fxM21 == 0 && state->mlf.fxM22 == MAKEFIXED(1, 0) &&
        state->mlf.lM23 == 0 &&
        state->mlf.lM31 == 0 && state->mlf.lM32 == 0 && state->mlf.lM33 == 1)
    {
        // the most common case: the identity matrix, the inversion is done...
#ifdef INVERT
        // ...through the special GPI call, cancel it and correct ptl/rct
        state->oldYInversion = GpiQueryYInversion(pHps->hps);
        if (state->oldYInversion != 0)
        {
            ptl->y = state->oldYInversion - ptl->y;
            if (rct)
            {
                LONG temp = state->oldYInversion - rct->yBottom;
                rct->yBottom = state->oldYInversion - rct->yTop;
                rct->yTop = temp;
            }

            GpiEnableYInversion(pHps->hps, 0);

            state->restoreInversion = TRUE;
        }
#else
        // through the raw yInvert value
        if (pHps->yInvert > 0)
        {
            LONG temp = pHps->yInvert - rct->yBottom;
            rct->yBottom = pHps->yInvert - rct->yTop;
            rct->yTop = temp;
        }
#endif
    }
    else
    {
        // the complex case: append a matrix transformation that will flip the
        // text along the ptl's Y coordinate
        MATRIXLF mlf;
        mlf.fxM11 = MAKEFIXED(1, 0);
        mlf.fxM12 = 0;
        mlf.lM13  = 0;
        mlf.fxM21 = 0;
        mlf.fxM22 = MAKEFIXED(-1, 0);
        mlf.lM23  = 0;
        mlf.lM31  = 0;
        mlf.lM32  = ptl->y * 2;
        GpiSetDefaultViewMatrix(pHps->hps, 8, &mlf, TRANSFORM_ADD);

        state->restoreMatrix = TRUE;
    }
}

static void UncompensateTextInversion(CTISTATE *state)
{
    if (state->restoreInversion)
        GpiEnableYInversion(state->pHps->hps, state->oldYInversion);
    if (state->restoreMatrix)
        GpiSetDefaultViewMatrix(state->pHps->hps, 9, &state->mlf,
                                TRANSFORM_REPLACE);
}
//******************************************************************************
//******************************************************************************
BOOL CFT2Module::Ft2CharStringPosAtA(HPS hps,PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCSTR pchString,CONST INT *alAdx, DWORD fuWin32Options)
{
    DWORD  ret;
    USHORT sel;
    pDCData pHps;

    pHps = (pDCData)OSLibGpiQueryDCData(hps);

    CTISTATE ctiState;
    CompensateTextInversion(pHps, ptl, rct, &ctiState);

    BOOL fallback = TRUE;

    if(pfnFt2CharStringPosAtA)
    {
        // All FreeType calls should be wrapped for saving FS
        sel  = RestoreOS2FS();
        ret  = pfnFt2CharStringPosAtA(hps, ptl,rct,flOptions,lCount,pchString,alAdx, fuWin32Options);
        SetFS(sel);
        if(ret || (ret == FALSE && ERRORIDERROR(WinGetLastError(0)) != PMERR_FUNCTION_NOT_SUPPORTED))
            fallback = FALSE;
    }

    if (fallback)
    {
        //else fall back to GPI
        //NOTE: We don't support fuWin32Options in the fallback case
        ret = OSLibGpiCharStringPosAt(pHps,ptl,rct,flOptions,lCount,pchString,alAdx);
    }

    UncompensateTextInversion(&ctiState);

    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL CFT2Module::Ft2CharStringPosAtW(HPS hps, PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCWSTR pchString,CONST INT *alAdx, DWORD fuWin32Options)
{
    DWORD  ret;
    USHORT sel;
    pDCData pHps;

    pHps = (pDCData)OSLibGpiQueryDCData(hps);

    CTISTATE ctiState;
    CompensateTextInversion(pHps, ptl, rct, &ctiState);

    BOOL fallback = TRUE;

    if (pfnFt2CharStringPosAtW)
    {
        // All FreeType calls should be wrapped for saving FS
        sel  = RestoreOS2FS();
        ret  = pfnFt2CharStringPosAtW(hps, ptl,rct,flOptions,lCount,pchString,alAdx, fuWin32Options);
        SetFS(sel);
        if(ret || (ret == FALSE && ERRORIDERROR(WinGetLastError(0)) != PMERR_FUNCTION_NOT_SUPPORTED))
            fallback = FALSE;
    }

    if (fallback)
    {
        // fall back to GPI
        // NOTE: We don't support fuWin32Options in the fallback case
        int   len;
        LPSTR astring;

        LPINT lpDx = NULL;

        len = WideCharToMultiByte( CP_ACP, 0, pchString, lCount, 0, 0, NULL, NULL );
        astring = (char *)malloc( len + 1 );
        lstrcpynWtoA(astring, pchString, len + 1 );

        if( IsDBCSEnv() && alAdx )
        {
            int i, j;

            lpDx = ( LPINT )malloc( len * sizeof( INT ));
            for( i = j = 0; i < len; i++, j++ )
            {
                lpDx[ i ] = alAdx[ j ];
                if( IsDBCSLeadByte( astring[ i ]))
                    lpDx[ ++i ] = 0;
            }

            alAdx = ( CONST INT * )lpDx;
        }

        ret = OSLibGpiCharStringPosAt(pHps,ptl,rct,flOptions,len,astring,alAdx);

        if( lpDx )
            free( lpDx );

        free(astring);
    }

    UncompensateTextInversion(&ctiState);

    return ret;
}
//******************************************************************************
//******************************************************************************
DWORD CFT2Module::Ft2GetFontData(HPS hps, DWORD dwTable, DWORD dwOffset,
                                 LPVOID lpvBuffer, DWORD cbData)
{
    DWORD  ret;
    USHORT sel;

    // All FreeType calls should be wrapped for saving FS
    if(pfnFt2GetFontData) {
        sel  = RestoreOS2FS();
        ret  = pfnFt2GetFontData(hps, dwTable, dwOffset, lpvBuffer, cbData);
        SetFS(sel);
        if(ret || (ret == GDI_ERROR && ERRORIDERROR(WinGetLastError(0)) != PMERR_FUNCTION_NOT_SUPPORTED))
            return ret;
    }
    //no fallback
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED_W);
    return(GDI_ERROR);
}
//******************************************************************************
// Query the font type current selected into the presentation space, or,
// when hps == NULL, query the type of the font with the specified name
//******************************************************************************
DWORD CFT2Module::Ft2QueryFontType(HPS hps, LPCSTR lpszFontName)
{
    DWORD  ret;
    USHORT sel;

    // All FreeType calls should be wrapped for saving FS
    if(pfnFt2QueryFontType) {
        sel  = RestoreOS2FS();
        ret  = pfnFt2QueryFontType(hps, lpszFontName);
        SetFS(sel);
        if(ret || (ret == GDI_ERROR && ERRORIDERROR(WinGetLastError(0)) != PMERR_FUNCTION_NOT_SUPPORTED))
            return ret;
    }
    //no fallback
    return FT2_FONTTYPE_UNKNOWN;
}
//******************************************************************************
//******************************************************************************
BOOL CFT2Module::Ft2GetStringWidthW(HDC hdc, LPWSTR lpszString, UINT cbString, PINT pWidthArray)
{
    DWORD  ret;
    USHORT sel;
    pDCData pHps;

    pHps = (pDCData)OSLibGpiQueryDCData(hdc);
    if(pHps == NULL) {
        DebugInt3();
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }

    // All FreeType calls should be wrapped for saving FS
    if(pfnFt2QueryStringWidthW) {
        sel  = RestoreOS2FS();
        ret  = pfnFt2QueryStringWidthW((HPS)hdc, lpszString, cbString, (LONG *)pWidthArray);
        SetFS(sel);
        if(ret || (ret == GDI_ERROR && ERRORIDERROR(WinGetLastError(0)) != PMERR_FUNCTION_NOT_SUPPORTED))
        {
           if(pHps && pHps->isPrinter && pHps->hdc)
           {//scale for printer dcs
               LONG alArray[2];

               if(OSLibDevQueryCaps(pHps, OSLIB_CAPS_HORIZONTAL_RESOLUTION, 2, &alArray[0]) &&
                  alArray[0] != alArray[1])
               {
                   dprintf(("Different hor/vert resolutions (%d,%d)", alArray[0], alArray[1]));
                   for (int i = 0; i < cbString; i++)
                   {
                       pWidthArray[i] = pWidthArray[i] * alArray[0] / alArray[1];
                   }
                }
            }
            return ret;
        }
    }
    //fallback method
    int c, i;
    for (i = 0; i < cbString; i++)
    {
        if (GetCharWidth32W(hdc, lpszString[i], lpszString[i], (LPINT)&c)) {
            dprintf(("%c pWidthArray[%d] = %d", lpszString[i], i, c));
            pWidthArray[i]= c;
        }
        else {
            dprintf(("WARNING: GetCharWidth32W failed for %c!!!", lpszString[i]));
            pWidthArray[i] = 0;
        }
    }
    return TRUE;
}
/*****************************************************************************
 * Name      : DWORD GetCharacterPlacementW
 * Purpose   : The GetCharacterPlacementW function retrieves information about
 *             a character string, such as character widths, caret positioning,
 *             ordering within the string, and glyph rendering. The type of
 *             information returned depends on the dwFlags parameter and is
 *             based on the currently selected font in the given display context.
 *             The function copies the information to the specified GCP_RESULTSW
 *             structure or to one or more arrays specified by the structure.
 * Parameters: HDC     hdc        handle to device context
 *             LPCSTR lpString   pointer to string
 *             int     nCount     number of characters in string
 *             int     nMaxExtent maximum extent for displayed string
 *             GCP_RESULTSW *lpResults  pointer to buffer for placement result
 *             DWORD   dwFlags    placement flags
 * Variables :
 * Result    :
 * Remark    :
 * Status    : Partly working
 *
 * Author    : Borrowed Rewind Code
 *****************************************************************************/
DWORD CFT2Module::Ft2GetCharacterPlacementW(HDC           hdc,
                                            LPCWSTR       lpString,
                                            int           uCount,
                                            int           nMaxExtent,
                                            GCP_RESULTSW *lpResults,
                                            DWORD         dwFlags)
{
    DWORD  ret;
    USHORT sel;
    SIZE   size;
    UINT   i, nSet;
    pDCData pHps;

    pHps = (pDCData)OSLibGpiQueryDCData(hdc);
    if(pHps == NULL) {
        DebugInt3();
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }

    dprintf(("%ls, %d, %d, 0x%08lx\n", lpString, uCount, nMaxExtent, dwFlags));

    dprintf(("lStructSize=%ld, lpOutString=%p, lpOrder=%p, lpDx=%p, lpCaretPos=%p, lpClass=%p, lpGlyphs=%p, nGlyphs=%u, nMaxFit=%d\n",
        lpResults->lStructSize, lpResults->lpOutString, lpResults->lpOrder,
        lpResults->lpDx, lpResults->lpCaretPos, lpResults->lpClass,
        lpResults->lpGlyphs, lpResults->nGlyphs, lpResults->nMaxFit));

    if(dwFlags&(~0))            dprintf(("unsupported flags 0x%08lx\n", dwFlags));
    if(lpResults->lpCaretPos)   dprintf(("caret positions not implemented\n"));
    if(lpResults->lpClass)  dprintf(("classes not implemented\n"));

    nSet = (UINT)uCount;
    if(nSet > lpResults->nGlyphs)
    nSet = lpResults->nGlyphs;

    /* return number of initialized fields */
    lpResults->nGlyphs = nSet;
    lpResults->nMaxFit = nSet;

    /* Treat the case where no special handling was requested in a fastpath way */
    /* copy will do if the GCP_REORDER flag is not set */
    if(lpResults->lpOutString)
        strncpyW( lpResults->lpOutString, lpString, nSet );

    if(lpResults->lpOrder)
        for(i = 0; i < nSet; i++)
            lpResults->lpOrder[i] = i;

    // All FreeType calls should be wrapped for saving FS
    if(pfnFt2GetCharacterPlacementW) {
        sel  = RestoreOS2FS();
        ret  = pfnFt2GetCharacterPlacementW((HPS)hdc, lpString, nSet, nMaxExtent, lpResults, dwFlags);
        SetFS(sel);
        if(ret || (ret == 0 && ERRORIDERROR(WinGetLastError(0)) != PMERR_FUNCTION_NOT_SUPPORTED))
        {
#ifdef DEBUG
           if(ret && lpResults->lpDx) {
               for (i = 0; i < nSet; i++)
               {
                   dprintf(("%c pWidthArray[%d] = %d", lpString[i], i, lpResults->lpDx[i]));
               }
           }
#endif
           if(pHps && pHps->isPrinter && pHps->hdc)
           {//scale for printer dcs
               LONG alArray[2];

               if(OSLibDevQueryCaps(pHps, OSLIB_CAPS_HORIZONTAL_RESOLUTION, 2, &alArray[0]) &&
                  alArray[0] != alArray[1])
               {
                   dprintf(("Different hor/vert resolutions (%d,%d)", alArray[0], alArray[1]));
                   if(lpResults->lpDx) {
                       for (i = 0; i < nSet; i++)
                       {
                           lpResults->lpDx[i] = lpResults->lpDx[i] * alArray[0] / alArray[1];
                       }
                   }
                   ULONG x = (ret & 0xffff);
                   x = x * alArray[0] / alArray[1];
                   ret &= ~0xffff;
                   ret |= (x & 0xffff);
               }
           }
           return ret;
        }
    }
    //fallback method

    /* FIXME: Will use the placement chars */
    if (lpResults->lpDx)
    {
        Ft2GetStringWidthW(hdc, (LPWSTR)lpString, nSet, lpResults->lpDx);
    }

    if(lpResults->lpGlyphs)
    GetGlyphIndicesW(hdc, lpString, nSet, lpResults->lpGlyphs, 0);

    if (GetTextExtentPoint32W(hdc, lpString, uCount, &size))
         ret = MAKELONG(size.cx, size.cy);
    else ret = 0;

    return ret;
}
//******************************************************************************
//******************************************************************************
