/* $Id: oslibres.cpp,v 1.15 2001-07-04 09:55:17 sandervl Exp $ */
/*
 * Window API wrappers for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winconst.h>
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibmsg.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "pmwindow.h"

#define DBG_LOCALLOG    DBG_oslibres
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HANDLE OSLibWinSetAccelTable(HWND hwnd, HANDLE hAccel, PVOID acceltemplate)
{
 HAB    hab = WinQueryAnchorBlock(hwnd);

    if(hAccel == 0) {
        hAccel = WinCreateAccelTable(hab, (PACCELTABLE)acceltemplate);
        if(hAccel == 0) {
            dprintf(("OSLibWinSetAccelTable: WinCreateAccelTable returned 0"));
            return FALSE;
        }
    }
    if(WinSetAccelTable(hab, hAccel, hwnd) == TRUE) {
            return hAccel;
    }
    else    return 0;
}
#if 0
//******************************************************************************
//TODO: change search method for icon array (cxDesired, cyDesired)
//TODO: PM rescales the icon internally!!! ($W(#*&$(*%&)
//******************************************************************************
HANDLE OSLibWinCreateIcon(PVOID iconbitmap, ULONG cxDesired, ULONG cyDesired)
{
 POINTERINFO pointerInfo = {0};
 HBITMAP     hbmColor, hbmMask;
 BITMAPARRAYFILEHEADER2 *bafh = (BITMAPARRAYFILEHEADER2 *)iconbitmap;
 BITMAPFILEHEADER2 *bfhBW;
 BITMAPFILEHEADER2 *bfhColor;
 HPS         hps;
 HANDLE      hIcon;

    if(iconbitmap == NULL) {
        dprintf(("OSLibWinCreateIcon iconbitmap == NULL!!"));
        return 0;
    }
    if(bafh->usType == BFT_BITMAPARRAY && bafh->cbSize == sizeof(BITMAPARRAYFILEHEADER2)) {
        // search best icon for the current screen,
        // TODO: maybe compare icon size with screen size.
        // Some bugs with black/white Icons ?
        BITMAPARRAYFILEHEADER2 *next, *found;
        LONG bitcountScreen, bitcountIcon=-1, cxIcon=-1, cyIcon=-1;

        HPS hps = WinGetPS(HWND_DESKTOP);
        HDC hdc = GpiQueryDevice(hps);
        DevQueryCaps(hdc, CAPS_COLOR_BITCOUNT, 1, &bitcountScreen);
        WinReleasePS(hps);

        next = found = bafh;
        while(TRUE)
        {
            bfhColor = (BITMAPFILEHEADER2 *)((char *)&next->bfh2 + sizeof(RGB2)*2 + sizeof(BITMAPFILEHEADER2));
            if(bfhColor->bmp2.cBitCount <= bitcountScreen &&
               bfhColor->bmp2.cBitCount > bitcountIcon ||
               (bfhColor->bmp2.cBitCount == bitcountIcon &&
               (cxIcon < bfhColor->bmp2.cx || cyIcon < bfhColor->bmp2.cy)))
            {
                found = next;
                bitcountIcon = bfhColor->bmp2.cBitCount;
                cxIcon = bfhColor->bmp2.cx;
                cyIcon = bfhColor->bmp2.cy;
            }
            if(next->offNext != 0)
                next = (BITMAPARRAYFILEHEADER2 *) ((char *)bafh + next->offNext);
            else
                break;
        }
        bfhBW    = &found->bfh2;
        bfhColor = (BITMAPFILEHEADER2 *)((char *)bfhBW + sizeof(RGB2)*2 + sizeof(BITMAPFILEHEADER2));
    }
    else {//single icon
        bfhBW    = (BITMAPFILEHEADER2 *)iconbitmap;
        bfhColor = (BITMAPFILEHEADER2 *)((char *)bfhBW + sizeof(RGB2)*2 + sizeof(BITMAPFILEHEADER2));
        bafh     = (BITMAPARRAYFILEHEADER2 *)bfhBW; //for calculation bitmap offset
    }
    hps = WinGetScreenPS(HWND_DESKTOP);

    //Resize icon bitmap if requested size is different
    if(cxDesired != bfhColor->bmp2.cx|| cyDesired != bfhColor->bmp2.cy)
    {
        BITMAPINFOHEADER2 infohdr = bfhColor->bmp2;

        infohdr.cx = cxDesired;
        infohdr.cy = cyDesired;
        hbmColor = GpiCreateBitmap(hps, &infohdr, CBM_INIT,
                                   (char *)bafh + bfhColor->offBits,
                                   (BITMAPINFO2 *)&bfhColor->bmp2);
    }
    else {
        hbmColor = GpiCreateBitmap(hps, &bfhColor->bmp2, CBM_INIT,
                                   (char *)bafh + bfhColor->offBits,
                                   (BITMAPINFO2 *)&bfhColor->bmp2);
    }
    if(hbmColor == GPI_ERROR) {
        dprintf(("OSLibWinCreateIcon: GpiCreateBitmap failed!"));
        WinReleasePS(hps);
        return 0;
    }
    //Resize icon mask if requested size is different
    if(cxDesired != bfhBW->bmp2.cx|| cyDesired*2 != bfhBW->bmp2.cy)
    {
        BITMAPINFOHEADER2 infohdr = bfhBW->bmp2;

        infohdr.cx = cxDesired;
        infohdr.cy = cyDesired;
        hbmMask = GpiCreateBitmap(hps, &infohdr, CBM_INIT,
                                  (char *)bafh + bfhBW->offBits,
                                  (BITMAPINFO2 *)&bfhBW->bmp2);
    }
    else {
        hbmMask = GpiCreateBitmap(hps, &bfhBW->bmp2, CBM_INIT,
                                  (char *)bafh + bfhBW->offBits,
                                  (BITMAPINFO2 *)&bfhBW->bmp2);
    }
    if(hbmMask == GPI_ERROR) {
        dprintf(("OSLibWinCreateIcon: GpiCreateBitmap hbmMask failed!"));
        GpiDeleteBitmap(hbmColor);
        WinReleasePS(hps);
        return 0;
    }

    pointerInfo.fPointer   = FALSE; //icon
    pointerInfo.xHotspot   = bfhBW->xHotspot;
    pointerInfo.yHotspot   = bfhBW->yHotspot;
    pointerInfo.hbmColor   = hbmColor;
    pointerInfo.hbmPointer = hbmMask;
    hIcon = WinCreatePointerIndirect(HWND_DESKTOP, &pointerInfo);
    if(hIcon == NULL) {
        dprintf(("OSLibWinCreateIcon: WinCreatePointerIndirect failed!"));
    }
    GpiDeleteBitmap(hbmMask);
    GpiDeleteBitmap(hbmColor);
    WinReleasePS(hps);

    return hIcon;
}
//******************************************************************************
//TODO: change cursor size if required!! (cxDesired, cyDesired)
//******************************************************************************
HANDLE OSLibWinCreatePointer(PVOID cursorbitmap, ULONG cxDesired, ULONG cyDesired)
{
 POINTERINFO pointerInfo = {0};
 HBITMAP     hbmColor = 0, hbmMask = 0;
 BITMAPARRAYFILEHEADER2 *bafh = (BITMAPARRAYFILEHEADER2 *)cursorbitmap;
 BITMAPFILEHEADER2 *bfh = (BITMAPFILEHEADER2 *)cursorbitmap, *bfhColor = 0;
 HPS         hps;
 HANDLE      hPointer;

    if(cursorbitmap == NULL) {
        dprintf(("OSLibWinCreatePointer cursorbitmap == NULL!!"));
        return 0;
    }
    if(bafh->usType == BFT_BITMAPARRAY && bafh->cbSize == sizeof(BITMAPARRAYFILEHEADER2)) {
        bfh  = &bafh->bfh2;
        bfhColor = (BITMAPFILEHEADER2 *)((char *)bfh + sizeof(RGB2)*2 + sizeof(BITMAPFILEHEADER2));
    }
    else {//single cursor
        bfh  = (BITMAPFILEHEADER2 *)cursorbitmap;
        bfhColor = (BITMAPFILEHEADER2 *)((char *)bfh + sizeof(RGB2)*2 + sizeof(BITMAPFILEHEADER2));
        bafh = (BITMAPARRAYFILEHEADER2 *)bfh; //for calculation bitmap offset
    }
    //skip xor/and mask
    hps = WinGetScreenPS(HWND_DESKTOP);
    hbmMask = GpiCreateBitmap(hps, &bfh->bmp2, CBM_INIT,
                              (char *)bafh + bfh->offBits,
                              (BITMAPINFO2 *)&bfh->bmp2);
    if(hbmMask == GPI_ERROR) {
        dprintf(("OSLibWinCreatePointer: GpiCreateBitmap failed!"));
        WinReleasePS(hps);
        return 0;
    }

    if((ULONG)((char *)bafh+bfh->offBits) != (ULONG)bfhColor)
    {//color bitmap present
        hbmColor = GpiCreateBitmap(hps, &bfhColor->bmp2, CBM_INIT,
                               (char *)bafh + bfhColor->offBits,
                               (BITMAPINFO2 *)&bfhColor->bmp2);
        if(hbmColor == GPI_ERROR) {
                dprintf(("OSLibWinCreateIcon: GpiCreateBitmap failed!"));
                GpiDeleteBitmap(hbmMask);
                WinReleasePS(hps);
                return 0;
        }
    }

    pointerInfo.fPointer   = TRUE;
    pointerInfo.xHotspot   = bfh->xHotspot;
    pointerInfo.yHotspot   = bfh->yHotspot;
    pointerInfo.hbmColor   = hbmColor;
    pointerInfo.hbmPointer = hbmMask;
    hPointer = WinCreatePointerIndirect(HWND_DESKTOP, &pointerInfo);

    if(hPointer == NULL) {
        dprintf(("OSLibWinCreatePointer: WinCreatePointerIndirect failed!"));
    }
    GpiDeleteBitmap(hbmMask);
    if(hbmColor) GpiDeleteBitmap(hbmColor);
    WinReleasePS(hps);
    return hPointer;
}
#endif
//******************************************************************************
//NOTE: Depends on origin of bitmap data!!!
//      Assumes 1 bpp bitmaps have a top left origin and all others have a bottom left origin
//******************************************************************************
HANDLE OSLibWinCreatePointer(CURSORICONINFO *pInfo, char *pAndBits, BITMAP_W *pAndBmp, char *pXorBits, 
                             BITMAP_W *pXorBmp, BOOL fCursor)
{
 POINTERINFO  pointerInfo = {0};
 HANDLE       hPointer;
 HBITMAP      hbmColor = 0, hbmMask = 0;
 BITMAPINFO2 *pBmpColor, *pBmpMask;
 int          masksize, colorsize, rgbsize, i;
 HPS          hps;
 char        *dest, *src;

    hps = WinGetScreenPS(HWND_DESKTOP);
    masksize = sizeof(BITMAPINFO2) + (pAndBmp->bmHeight * 2 * pAndBmp->bmWidthBytes) + 2*sizeof(RGB2);
    pBmpMask = (BITMAPINFO2 *)malloc(masksize);
    if(pBmpMask == NULL) {
        DebugInt3();
        return 0;
    }
    memset(pBmpMask, 0, masksize);
    pBmpMask->cbFix             = sizeof(BITMAPINFOHEADER2);
    pBmpMask->cx                = (USHORT)pAndBmp->bmWidth;
    pBmpMask->cy                = (USHORT)pAndBmp->bmHeight*2;
    pBmpMask->cPlanes           = pAndBmp->bmPlanes;
    pBmpMask->cBitCount         = 1;
    pBmpMask->ulCompression     = BCA_UNCOMP;
    pBmpMask->ulColorEncoding   = BCE_RGB;
    memset(&pBmpMask->argbColor[0], 0, sizeof(RGB2));
    memset(&pBmpMask->argbColor[1], 0xff, sizeof(RGB)); //not the reserved byte
    //Xor bits are already 0
    //copy And bits (must reverse scanlines because origin is top left instead of bottom left)
    src  = pAndBits;
    dest = ((char *)&pBmpMask->argbColor[2]) + (pAndBmp->bmHeight * 2 - 1) * (pAndBmp->bmWidthBytes);
    for(i=0;i<pAndBmp->bmHeight;i++) {
        memcpy(dest, src, pAndBmp->bmWidthBytes);
        dest -= pAndBmp->bmWidthBytes;
        src  += pAndBmp->bmWidthBytes;
    }
    hbmMask = GpiCreateBitmap(hps, (BITMAPINFOHEADER2 *)pBmpMask, CBM_INIT,
                              (PBYTE)&pBmpMask->argbColor[2], pBmpMask);

    if(hbmMask == GPI_ERROR) {
        dprintf(("OSLibWinCreatePointer: GpiCreateBitmap failed!"));
        WinReleasePS(hps);
        free(pBmpMask);
        return 0;
    }
    if(pXorBits)
    {//color bitmap present
        RGBQUAD *rgb;
        RGB2    *os2rgb;

        if(pXorBmp->bmBitsPixel <= 8)
             rgbsize = (1<<pXorBmp->bmBitsPixel)*sizeof(RGB2);
        else rgbsize = 0;

        colorsize = sizeof(BITMAPINFO2) + (pXorBmp->bmHeight * pXorBmp->bmWidthBytes) + rgbsize;
        pBmpColor = (BITMAPINFO2 *)malloc(colorsize);
        if(pBmpColor == NULL) {
            DebugInt3();
            return 0;
        }
        memset(pBmpColor, 0, colorsize);
        pBmpColor->cbFix            = sizeof(BITMAPINFOHEADER2);
        pBmpColor->cx               = (USHORT)pXorBmp->bmWidth;
        pBmpColor->cy               = (USHORT)pXorBmp->bmHeight;
        pBmpColor->cPlanes          = pXorBmp->bmPlanes;
        pBmpColor->cBitCount        = pXorBmp->bmBitsPixel;
        pBmpColor->ulCompression    = BCA_UNCOMP;
        pBmpColor->ulColorEncoding  = BCE_RGB;

  	    os2rgb                      = &pBmpColor->argbColor[0];
  	    rgb                         = (RGBQUAD *)(pXorBits);

  	    if(pXorBmp->bmBitsPixel <= 8) {
    	        for(i=0;i<(1<<pXorBmp->bmBitsPixel);i++) {
    	                os2rgb->bRed   = rgb->rgbRed;
    	                os2rgb->bBlue  = rgb->rgbBlue;
    	                os2rgb->bGreen = rgb->rgbGreen;
    	                os2rgb++;
    	                rgb++;
    	        }
    	}

        if(pXorBmp->bmBitsPixel == 1) {
                //copy Xor bits (must reverse scanlines because origin is top left instead of bottom left)
                src  = (char *)rgb;
                dest = ((char *)os2rgb) + (pXorBmp->bmHeight - 1) * pXorBmp->bmWidthBytes;
                for(i=0;i<pXorBmp->bmHeight;i++) {
                    memcpy(dest, src, pXorBmp->bmWidthBytes);
                    dest -= pXorBmp->bmWidthBytes;
                    src  += pXorBmp->bmWidthBytes;
                }
        }
        else    memcpy(os2rgb, rgb, pXorBmp->bmHeight * pXorBmp->bmWidthBytes);

        hbmColor = GpiCreateBitmap(hps, (BITMAPINFOHEADER2 *)pBmpColor, CBM_INIT,
                              (PBYTE)os2rgb, pBmpColor);

        if(hbmColor == GPI_ERROR) {
                dprintf(("OSLibWinCreateIcon: GpiCreateBitmap failed!"));
                GpiDeleteBitmap(hbmMask);
                WinReleasePS(hps);
                free(pBmpMask);
                return 0;
        }
    }

    pointerInfo.fPointer   = fCursor; //FALSE = icon
    pointerInfo.xHotspot   = pInfo->ptHotSpot.x;
    pointerInfo.yHotspot   = mapY(pInfo->nHeight, pInfo->ptHotSpot.y);
    pointerInfo.hbmColor   = hbmColor;
    pointerInfo.hbmPointer = hbmMask;
    hPointer = WinCreatePointerIndirect(HWND_DESKTOP, &pointerInfo);

    if(hPointer == NULL) {
        dprintf(("OSLibWinCreateCursor: WinCreatePointerIndirect failed! (lasterr=%x)", WinGetLastError(GetThreadHAB())));
    }
    GpiDeleteBitmap(hbmMask);
    if(hbmColor) GpiDeleteBitmap(hbmColor);
    WinReleasePS(hps);

    free(pBmpMask);
    free(pBmpColor);
    return hPointer;
}
//******************************************************************************
//******************************************************************************
HANDLE OSLibWinQuerySysIcon(ULONG type,INT w,INT h)
{
 ULONG os2type = 0;
 HPOINTER hPointer;

    switch(type) {
    case IDI_APPLICATION_W:
        os2type = SPTR_PROGRAM;
        break;
    case IDI_HAND_W:
        os2type = SPTR_ICONWARNING;
        break;
    case IDI_QUESTION_W:
        os2type = SPTR_ICONQUESTION;
        break;
    case IDI_EXCLAMATION_W:
        os2type = SPTR_ICONWARNING;
        break;
    case IDI_ASTERISK_W:
        os2type = SPTR_ICONINFORMATION;
        break;
    default:
        return 0;
    }

    hPointer = WinQuerySysPointer(HWND_DESKTOP, os2type, TRUE);

    if (hPointer)
    {
      INT sysW = WinQuerySysValue(HWND_DESKTOP,SV_CXICON),sysH = WinQuerySysValue(HWND_DESKTOP,SV_CYICON);

      if (sysW != w || sysH != h)
      {
        POINTERINFO pi;

        WinQueryPointerInfo(hPointer,&pi);
        //CB: todo: change icon size

      }
    }

    return hPointer;
}
//******************************************************************************
//******************************************************************************
HANDLE OSLibWinQuerySysPointer(ULONG type,INT w,INT h)
{
 ULONG os2type = 0;

    switch(type) {
    case IDC_ARROW_W:
        os2type = SPTR_ARROW;
        break;
    case IDC_UPARROW_W:
        os2type = SPTR_ARROW;
        break;
    case IDC_IBEAM_W:
        os2type = SPTR_TEXT;
        break;
    case IDC_ICON_W:
        os2type = SPTR_PROGRAM;
        break;
    case IDC_NO_W:
        os2type = SPTR_ILLEGAL;
        break;
    case IDC_CROSS_W:
        os2type = SPTR_MOVE;
        break;
    case IDC_SIZE_W:
        os2type = SPTR_MOVE;
        break;
    case IDC_SIZEALL_W:
        os2type = SPTR_MOVE;
        break;
    case IDC_SIZENESW_W:
        os2type = SPTR_SIZENESW;
        break;
    case IDC_SIZENS_W:
        os2type = SPTR_SIZENS;
        break;
    case IDC_SIZENWSE_W:
        os2type = SPTR_SIZENWSE;
        break;
    case IDC_SIZEWE_W:
        os2type = SPTR_SIZEWE;
        break;
    case IDC_WAIT_W:
        os2type = SPTR_WAIT;
        break;
    case IDC_APPSTARTING_W:
        os2type = SPTR_WAIT;
        break;
    case IDC_HELP_W: //TODO: Create a cursor for this one
        os2type = SPTR_WAIT;
        break;
    default:
        return 0;
    }
    //Note: Does not create a copy
    return WinQuerySysPointer(HWND_DESKTOP, os2type, FALSE);
}
//******************************************************************************
//******************************************************************************
VOID OSLibWinDestroyPointer(HANDLE hPointer)
{
    WinDestroyPointer(hPointer);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetPointer(HANDLE hPointer)
{
    return WinSetPointer(HWND_DESKTOP, hPointer);
}
//******************************************************************************
//******************************************************************************
HANDLE  OSLibWinQueryPointer()
{
    return WinQueryPointer(HWND_DESKTOP);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinClipCursor(const RECT * pRect)
{
   RECTL  rectl;
   PRECTL ptr = NULL;

   if (pRect != NULL)
   {
      rectl.xLeft = max(pRect->left, 0);
      rectl.xRight = min(pRect->right, ScreenWidth-1);
      rectl.yBottom = max(ScreenHeight - pRect->bottom, 0);
      rectl.yTop = min(ScreenHeight - pRect->top, ScreenHeight-1);
      ptr = &rectl;
   }
   return WinSetPointerClipRect (HWND_DESKTOP, ptr);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetClipCursor(LPRECT pRect)
{
   RECTL  rectl;

   if (WinQueryPointerClipRect(HWND_DESKTOP, &rectl))
   {
      pRect->left   = rectl.xLeft;
      pRect->right  = rectl.xRight;
      pRect->bottom = ScreenHeight - rectl.yBottom;
      pRect->top    = ScreenHeight - rectl.yTop;
      return TRUE;
   }
   return FALSE;
}
//******************************************************************************
//******************************************************************************
