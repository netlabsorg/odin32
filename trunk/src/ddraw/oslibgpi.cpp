/* $Id: oslibgpi.cpp,v 1.2 2001-10-01 07:06:10 sandervl Exp $ */

/*
 * GPI interface code
 *
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define  INCL_GPI
#define  INCL_WIN
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "win32type.h"
#include <winconst.h>
#include <winuser32.h>
#include "oslibgpi.h"
#include <misc.h>

//******************************************************************************
//******************************************************************************
LPRGNDATA OSLibQueryVisibleRegion(HWND hwnd, DWORD screenHeight)
{
    HRGN hrgnVis = 0;
    RECTL rcl = {0,0,1,1};
    LPRGNDATA lpRgnData = 0;
    HPS hps = 0;
    LONG temp, i;
    ULONG bufSizeNeeded;
    PRECTL pRectl;

    hps = WinGetPS(Win32ToOS2FrameHandle(hwnd));
    if(hps == NULL) {
        dprintf(("OSLibQueryVisibleRegion: WinGetPS %x failed", hwnd));
        return NULL;
    }
    hrgnVis = GreCreateRectRegion(hps, &rcl, 1);
    GreCopyClipRegion(hps, hrgnVis, 0, COPYCRGN_VISRGN);

    RGNRECT rgnRect;
    rgnRect.ircStart    = 1;
    rgnRect.crc         = 0;
    rgnRect.ulDirection = RECTDIR_LFRT_TOPBOT;
    if(!GpiQueryRegionRects(hps, hrgnVis, NULL, &rgnRect, NULL))
    {
        dprintf(("WARNING: GpiQueryRegionRects failed! (%x)", WinGetLastError(0)));
        goto failure;
    }
    bufSizeNeeded = rgnRect.crcReturned * sizeof(RECT) + sizeof (RGNDATAHEADER);
    lpRgnData = (LPRGNDATA)malloc(bufSizeNeeded);

    pRectl      = (PRECTL)lpRgnData->Buffer;
    rgnRect.crc = rgnRect.crcReturned;
    if(!GpiQueryRegionRects(hps, hrgnVis, NULL, &rgnRect, pRectl))
    {
        dprintf(("WARNING: GpiQueryRegionRects failed! (%x)", WinGetLastError(0)));
        goto failure;
    }
    for(i=0;i<rgnRect.crcReturned;i++) {
        temp = pRectl[i].yTop;
        dprintf(("Region rect %d (%d,%d)(%d,%d)", i, pRectl[i].xLeft, pRectl[i].yBottom, pRectl[i].xRight, pRectl[i].yTop));
        pRectl[i].yTop    = screenHeight - pRectl[i].yBottom;
        pRectl[i].yBottom = screenHeight - temp;
        dprintf(("Region rect %d (%d,%d)(%d,%d)", i, pRectl[i].xLeft, pRectl[i].yBottom, pRectl[i].xRight, pRectl[i].yTop));
    }

    RECTL boundRect;
    GpiQueryRegionBox(hps, hrgnVis, &boundRect);

    lpRgnData->rdh.dwSize   = sizeof(lpRgnData->rdh);
    lpRgnData->rdh.iType    = RDH_RECTANGLES_W;    // one and only possible value
    lpRgnData->rdh.nCount   = rgnRect.crcReturned;
    lpRgnData->rdh.nRgnSize = rgnRect.crcReturned * sizeof(RECT);

    //flip top & bottom for bounding rectangle (not really necessary; but cleaner coding)
    dprintf(("Boundary (%d,%d)(%d,%d)", boundRect.xLeft, boundRect.yBottom, boundRect.xRight, boundRect.yTop));
    lpRgnData->rdh.rcBound.left   = boundRect.xLeft;
    lpRgnData->rdh.rcBound.right  = boundRect.xRight;
    lpRgnData->rdh.rcBound.top    = screenHeight - boundRect.yBottom;
    lpRgnData->rdh.rcBound.bottom = screenHeight - boundRect.yTop;
    dprintf(("Boundary (%d,%d)(%d,%d)", boundRect.xLeft, boundRect.yBottom, boundRect.xRight, boundRect.yTop));

    // Destroy the region now we have finished with it.
    GreDestroyRegion(hps, hrgnVis);
    WinReleasePS(hps);
    return lpRgnData;

failure:
    if(lpRgnData) free(lpRgnData);
    if(hrgnVis)   GreDestroyRegion(hps, hrgnVis);
    if(hps)       WinReleasePS(hps);
    return 0;
}
//******************************************************************************
//******************************************************************************
