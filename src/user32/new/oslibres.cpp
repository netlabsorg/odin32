/* $Id: oslibres.cpp,v 1.2 1999-07-20 17:50:39 sandervl Exp $ */
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
#include <oslibwin.h>
#include "oslibstyle.h"
#include "oslibutil.h"
#include "oslibmsg.h"
#include "oslibgdi.h"
#include "pmwindow.h"


//******************************************************************************
//******************************************************************************
HWND OSLibWinCreateMenu(HWND hwndParent, PVOID menutemplate)
{
  return WinCreateMenu(hwndParent, menutemplate);
}
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
//todo: save mask handle somewhere
//******************************************************************************
HANDLE OSLibWinSetIcon(HWND hwnd, HANDLE hIcon, PVOID iconbitmap)
{
 POINTERINFO pointerInfo = {0};
 HBITMAP     hbmColor, hbmMask;
 BITMAPARRAYFILEHEADER2 *bafh = (BITMAPARRAYFILEHEADER2 *)iconbitmap;
 BITMAPFILEHEADER2 *bfh;
 HPS         hps;

    if(hIcon == 0) {
	    //skip xor/and mask
	    bfh = (BITMAPFILEHEADER2 *)((char *)&bafh->bfh2 + sizeof(RGB2)*2 + sizeof(BITMAPFILEHEADER2));
	    hps = WinGetPS(hwnd);
	    hbmColor = GpiCreateBitmap(hps, &bfh->bmp2, CBM_INIT, 
		     	               (char *)bafh + bfh->offBits,
	                               (BITMAPINFO2 *)&bfh->bmp2);
	    if(hbmColor == GPI_ERROR) {
	        dprintf(("OSLibWinSetIcon: GpiCreateBitmap failed!"));
	        WinReleasePS(hps);
	        return 0;
	    }
	    hbmMask = GpiCreateBitmap(hps, &bafh->bfh2.bmp2, CBM_INIT, 
		     	              (char *)bafh + bafh->bfh2.offBits,
	                              (BITMAPINFO2 *)&bafh->bfh2.bmp2);
	    if(hbmMask == GPI_ERROR) {
	        dprintf(("OSLibWinSetIcon: GpiCreateBitmap hbmMask failed!"));
	        WinReleasePS(hps);
	        return 0;
	    }
	
	    pointerInfo.fPointer   = FALSE; //icon
	    pointerInfo.xHotspot   = bfh->xHotspot;
	    pointerInfo.yHotspot   = bfh->yHotspot;
	    pointerInfo.hbmColor   = hbmColor;
	    pointerInfo.hbmPointer = hbmMask;
	    hIcon = WinCreatePointerIndirect(HWND_DESKTOP, &pointerInfo);
	    if(hIcon == NULL) {
	        dprintf(("WinSetIcon: WinCreatePointerIndirect failed!"));
		GpiDeleteBitmap(hbmMask);
		GpiDeleteBitmap(hbmColor);
		WinReleasePS(hps);
	    }
    }
    WinSendMsg(hwnd, WM_SETICON, (MPARAM)hIcon, 0);
    WinReleasePS(hps);
    return hIcon;
}
//******************************************************************************
//******************************************************************************

