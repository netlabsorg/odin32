/* $Id: oslibres.cpp,v 1.9 2000-02-16 14:34:28 sandervl Exp $ */
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
#include <os2.h>
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winconst.h>
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibmsg.h"
#include "oslibgdi.h"
#include "pmwindow.h"

#define DBG_LOCALLOG	DBG_oslibres
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
//******************************************************************************
//******************************************************************************
HANDLE OSLibWinCreateIcon(PVOID iconbitmap)
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

    hbmColor = GpiCreateBitmap(hps, &bfhColor->bmp2, CBM_INIT,
                               (char *)bafh + bfhColor->offBits,
                               (BITMAPINFO2 *)&bfhColor->bmp2);
    if(hbmColor == GPI_ERROR) {
        dprintf(("OSLibWinCreateIcon: GpiCreateBitmap failed!"));
        WinReleasePS(hps);
        return 0;
    }
    hbmMask = GpiCreateBitmap(hps, &bfhBW->bmp2, CBM_INIT,
                              (char *)bafh + bfhBW->offBits,
                              (BITMAPINFO2 *)&bfhBW->bmp2);
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
//******************************************************************************
HANDLE OSLibWinCreatePointer(PVOID cursorbitmap)
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
