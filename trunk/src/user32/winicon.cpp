/* $Id: winicon.cpp,v 1.2 1999-11-03 19:51:44 sandervl Exp $ */
/*
 * Win32 Icon Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <winicon.h>

//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIcon( HINSTANCE arg1, INT arg2, INT arg3, BYTE arg4, BYTE arg5, LPCVOID arg6, LPCVOID arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIcon\n");
#endif
    return O32_CreateIcon(arg1, arg2, arg3, arg4, arg5, (const BYTE *)arg6, (const BYTE *)arg7);
}
//******************************************************************************
//ASSERT dwVer == win31 (ok according to SDK docs)
//******************************************************************************
HICON WIN32API CreateIconFromResource(PBYTE presbits,  UINT dwResSize,
                                      BOOL  fIcon,     DWORD dwVer)
{
 HICON hicon;
 DWORD OS2ResSize = 0;
 PBYTE OS2Icon    = ConvertWin32Icon(presbits, dwResSize, &OS2ResSize);

    hicon = O32_CreateIconFromResource(OS2Icon, OS2ResSize, fIcon, dwVer);
#ifdef DEBUG
    WriteLog("USER32:  CreateIconFromResource returned %X (%X)\n", hicon, GetLastError());
#endif
    if(OS2Icon)
        FreeIcon(OS2Icon);

    return(hicon);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIconFromResourceEx(PBYTE presbits,  UINT dwResSize,
                                        BOOL  fIcon,     DWORD dwVer,
                                        int   cxDesired, int cyDesired,
                                        UINT  Flags)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIconFromResourceEx %X %d %d %X %d %d %X, not completely supported!\n", presbits, dwResSize, fIcon, dwVer, cxDesired, cyDesired, Flags);
#endif
    return CreateIconFromResource(presbits, dwResSize, fIcon, dwVer);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIconIndirect(LPICONINFO pIcon)
{
 HICON   hIcon;
 HDC     hdcSrc, hdcDst;

    dprintf(("USER32:  CreateIconIndirect\n"));
    if(pIcon->hbmMask && pIcon->hbmColor) 
    {
	ICONINFO iconinfo;
	SIZE bmpsize;

	iconinfo = *pIcon;
	if(GetBitmapDimensionEx(pIcon->hbmColor, &bmpsize) == FALSE) {
		return 0;
	}
	//if there's a color bitmap, the mask bitmap contains only the AND bits
        //Open32 calls WinCreatePointerIndirect which expects AND & XOR bits
        //To solve this we create a bitmap that's 2x height of the mask, copy
        //the AND bits and set the XOR bits to 0
    	hdcSrc = CreateCompatibleDC(0);
    	hdcDst = CreateCompatibleDC(0);

	iconinfo.hbmMask  = CreateCompatibleBitmap (hdcDst, bmpsize.cx, bmpsize.cy*2);
    	SelectObject (hdcDst, iconinfo.hbmMask);
	SelectObject (hdcSrc, pIcon->hbmMask);
	BitBlt (hdcDst, 0, 0, bmpsize.cx, bmpsize.cy,
                hdcSrc, bmpsize.cx, 0, SRCCOPY);
	PatBlt (hdcDst, bmpsize.cx, bmpsize.cy, bmpsize.cx, bmpsize.cy, BLACKNESS);
    	
	hIcon = O32_CreateIconIndirect(&iconinfo);

    	DeleteObject(iconinfo.hbmMask);
    	DeleteDC(hdcSrc);
    	DeleteDC(hdcDst);

	return hIcon;
    }
    hIcon = O32_CreateIconIndirect(pIcon);
    if(hIcon == 0) {
	dprintf(("CreateIconIndirect %d (%d,%d) %x %x failed with %x", pIcon->fIcon, pIcon->xHotspot, pIcon->yHotspot, pIcon->hbmMask, pIcon->hbmColor, GetLastError()));
    }
    return hIcon;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyIcon( HICON arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyIcon\n");
#endif
    return O32_DestroyIcon(arg1);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CopyIcon( HICON arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  CopyIcon\n");
#endif
    return O32_CopyIcon(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetIconInfo( HICON arg1, LPICONINFO  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetIconInfo\n");
#endif
    return O32_GetIconInfo(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
