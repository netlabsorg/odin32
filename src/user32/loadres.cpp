/* $Id: loadres.cpp,v 1.11 1999-10-28 19:09:16 sandervl Exp $ */

/*
 * Win32 resource API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 * Parts based on Wine code (objects\bitmap.c):
 *
 * Copyright 1993 Alexandre Julliard
 *           1998 Huw D M Davies
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <user32.h>
#include <winres.h>
#include <heapstring.h>
#include <oslibres.h>
#include <winconst.h>
#include <win\virtual.h>
#include "dib.h"

//******************************************************************************
//******************************************************************************
int WIN32API LoadStringA(HINSTANCE hinst, UINT wID, LPSTR lpBuffer, int cchBuffer)
{
 Win32Resource *winres;
 LPWSTR  resstring;
 int     resstrlen = 0;

    winres = (Win32Resource *)FindResourceA(hinst, (LPSTR)wID, RT_STRINGA);
    if(winres == NULL) {
        dprintf(("LoadStringA NOT FOUND from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
        *lpBuffer = 0;
        return 0;
    }

    resstring = (LPWSTR)winres->lockResource();
    if(resstring) {
        resstrlen = min(lstrlenW(resstring)+1, cchBuffer);
        lstrcpynWtoA(lpBuffer, resstring, resstrlen);
        lpBuffer[resstrlen-1] = 0;
        resstrlen--;
        dprintf(("LoadStringA (%d) %s", resstrlen, lpBuffer));
    }
    delete winres;

    dprintf(("LoadStringA from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
    return(resstrlen);
}
//******************************************************************************
//******************************************************************************
int WIN32API LoadStringW(HINSTANCE hinst, UINT wID, LPWSTR lpBuffer, int cchBuffer)
{
 Win32Resource *winres;
 LPWSTR resstring;
 int    resstrlen = 0;

    winres = (Win32Resource *)FindResourceW(hinst, (LPWSTR)wID, RT_STRINGW);
    if(winres == NULL) {
        dprintf(("LoadStringW NOT FOUND from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
        *lpBuffer = 0;
        return 0;
    }

    resstring = (LPWSTR)winres->lockResource();
    if(resstring) {
        resstrlen = min(lstrlenW(resstring)+1, cchBuffer);
        lstrcpynW(lpBuffer, resstring, resstrlen);
        lpBuffer[resstrlen-1] = 0;
        resstrlen--;
    }
    delete winres;

    dprintf(("LoadStringW from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
    return(resstrlen);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconA(HINSTANCE hinst, LPCSTR lpszIcon)
{
 Win32Resource *winres;
 HICON          hIcon;

    hIcon = OSLibWinQuerySysIcon((ULONG)lpszIcon);
    if(hIcon == 0) {//not a system icon
        winres = (Win32Resource *)FindResourceA(hinst, lpszIcon, RT_ICONA);
        if(winres == 0) {
                winres = (Win32Resource *)FindResourceA(hinst, lpszIcon, RT_GROUP_ICONA);
        }
        if(winres) {
                hIcon = OSLibWinCreateIcon(winres->lockOS2Resource());
                delete winres;
        }
    }
    dprintf(("LoadIconA (%X) returned %x\n", hinst, hIcon));

    return(hIcon);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon)
{
 Win32Resource *winres;
 HICON          hIcon;

    hIcon = OSLibWinQuerySysIcon((ULONG)lpszIcon);
    if(hIcon == 0) {//not a system icon
        winres = (Win32Resource *)FindResourceW(hinst, lpszIcon, RT_ICONW);
        if(winres == 0) {
                winres = (Win32Resource *)FindResourceW(hinst, lpszIcon, RT_GROUP_ICONW);
        }
        if(winres) {
                hIcon = OSLibWinCreateIcon(winres->lockOS2Resource());
                delete winres;
        }
    }
    dprintf(("LoadIconW (%X) returned %x\n", hinst, hIcon));

    return(hIcon);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorA(HINSTANCE hinst, LPCSTR lpszCursor)
{
 Win32Resource *winres;
 HCURSOR        hCursor;

    hCursor = OSLibWinQuerySysPointer((ULONG)lpszCursor);
    if(hCursor == 0) {//not a system pointer
        winres = (Win32Resource *)FindResourceA(hinst, lpszCursor, RT_CURSORA);
        if(winres == 0) {
                winres = (Win32Resource *)FindResourceA(hinst, lpszCursor, RT_GROUP_CURSORA);
        }
        if(winres) {
                hCursor = OSLibWinCreatePointer(winres->lockOS2Resource());
                delete winres;
        }
    }
    if(HIWORD(lpszCursor)) {
         dprintf(("LoadCursorA %s from %x returned %x\n", lpszCursor, hinst, hCursor));
    }
    else dprintf(("LoadCursorA %x from %x returned %x\n", lpszCursor, hinst, hCursor));

    return(hCursor);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorW(HINSTANCE hinst, LPCWSTR lpszCursor)
{
 Win32Resource *winres;
 HCURSOR        hCursor;

    hCursor = OSLibWinQuerySysPointer((ULONG)lpszCursor);
    if(hCursor == 0) {//not a system pointer
        winres = (Win32Resource *)FindResourceW(hinst, lpszCursor, RT_CURSORW);
        if(winres == 0) {
                winres = (Win32Resource *)FindResourceW(hinst, lpszCursor, RT_GROUP_CURSORW);
        }
        if(winres) {
                hCursor = OSLibWinCreatePointer(winres->lockOS2Resource());
                delete winres;
        }
    }
    dprintf(("LoadCursorW (%X) returned %x\n", hinst, hCursor));

    return(hCursor);
}
//******************************************************************************
//******************************************************************************
BOOL IsSystemBitmap(ULONG *id)
{
   switch(*id)
   {
        case OBM_UPARROW_W:
        case OBM_DNARROW_W:
        case OBM_RGARROW_W:
        case OBM_LFARROW_W:
        case OBM_RESTORE_W:
        case OBM_RESTORED_W:
        case OBM_UPARROWD_W:
        case OBM_DNARROWD_W:
        case OBM_RGARROWD_W:
        case OBM_LFARROWD_W:
        case OBM_OLD_UPARROW_W:
        case OBM_OLD_DNARROW_W:
        case OBM_OLD_RGARROW_W:
        case OBM_OLD_LFARROW_W:
        case OBM_CHECK_W:
        case OBM_CHECKBOXES_W:
        case OBM_BTNCORNERS_W:
        case OBM_COMBO_W:
        case OBM_REDUCE_W:
        case OBM_REDUCED_W:
        case OBM_ZOOM_W:
        case OBM_ZOOMD_W:
        case OBM_SIZE_W:
        case OBM_CLOSE_W:
        case OBM_MNARROW_W:
        case OBM_UPARROWI_W:
        case OBM_DNARROWI_W:
        case OBM_RGARROWI_W:
        case OBM_LFARROWI_W:
                return TRUE;

        //TODO: Not supported by Open32. Replacement may not be accurate
        case OBM_OLD_CLOSE_W:
                *id = OBM_CLOSE_W;
                return TRUE;

        case OBM_BTSIZE_W:
                *id = OBM_SIZE_W;
                return TRUE;

        case OBM_OLD_REDUCE_W:
                *id = OBM_REDUCE_W;
                return TRUE;

        case OBM_OLD_ZOOM_W:
                *id = OBM_ZOOM_W;
                return TRUE;

        case OBM_OLD_RESTORE_W:
                *id = OBM_RESTORE_W;
                return TRUE;

        default:
                return FALSE;
   }
}
//******************************************************************************
//NOTE: LR_CREATEDIBSECTION flag doesn't work (crash in GDI32)!
//******************************************************************************
HANDLE LoadBitmapA(HINSTANCE hinst, LPCSTR lpszName, int cxDesired, int cyDesired,
                   UINT fuLoad)
{
    HBITMAP hbitmap = 0;
    HDC hdc;
    HRSRC hRsrc;
    HGLOBAL handle, hMapping = 0;
    char *ptr = NULL;
    BITMAPINFO *info, *fix_info=NULL;
    HGLOBAL hFix;
    int size;

    if (!(fuLoad & LR_LOADFROMFILE)) {
      if (!(hRsrc = FindResourceA( hinst, lpszName, RT_BITMAPA ))) return 0;
      if (!(handle = LoadResource( hinst, hRsrc ))) return 0;

      if ((info = (BITMAPINFO *)LockResource( handle )) == NULL) return 0;
    }
    else
    {
        if (!(hMapping = VIRTUAL_MapFileA( lpszName, (LPVOID *)&ptr ))) return 0;
        info = (BITMAPINFO *)(ptr + sizeof(BITMAPFILEHEADER));
    }

    //TODO: This has to be removed once pe2lx stores win32 resources!!!
    if (info->bmiHeader.biSize != sizeof(BITMAPCOREHEADER) &&
        info->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
    {//assume it contains a file header first
        info = (BITMAPINFO *)((char *)info + sizeof(BITMAPFILEHEADER));
    }

    if (info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER)) {
      size = sizeof(BITMAPINFOHEADER) +
             (sizeof (RGBTRIPLE) << ((BITMAPCOREHEADER *)info)->bcBitCount);
    } else
      size = DIB_BitmapInfoSize(info, DIB_RGB_COLORS);

    if ((hFix = GlobalAlloc(0, size)) != NULL) fix_info = (BITMAPINFO *)GlobalLock(hFix);
    if (fix_info) {
      BYTE pix;

      if (info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER)) {
        ULONG colors;
        ULONG *p, *q;

        memset (fix_info, 0, sizeof (BITMAPINFOHEADER));
        fix_info->bmiHeader.biSize     = sizeof (BITMAPINFOHEADER);
        fix_info->bmiHeader.biWidth    = ((BITMAPCOREHEADER *)info)->bcWidth;
        fix_info->bmiHeader.biHeight   = ((BITMAPCOREHEADER *)info)->bcHeight;
        fix_info->bmiHeader.biPlanes   = ((BITMAPCOREHEADER *)info)->bcPlanes;
        fix_info->bmiHeader.biBitCount = ((BITMAPCOREHEADER *)info)->bcBitCount;

        p = (PULONG)((char *)info + sizeof(BITMAPCOREHEADER));
        q = (PULONG)((char *)fix_info + sizeof(BITMAPINFOHEADER));
        for (colors = 1 << fix_info->bmiHeader.biBitCount; colors > 0; colors--) {
          *q = *p & 0x00FFFFFFUL;
          q++;
          p = (PULONG)((char *)p + sizeof (RGBTRIPLE));
        }
      } else
        memcpy(fix_info, info, size);

      size = DIB_BitmapInfoSize(info, DIB_RGB_COLORS);
      pix = *((LPBYTE)info + size);
      DIB_FixColorsToLoadflags(fix_info, fuLoad, pix);
      if ((hdc = GetDC(0)) != 0) {
        char *bits = (char *)info + size;
        if (fuLoad & LR_CREATEDIBSECTION) {
          DIBSECTION dib;
          hbitmap = CreateDIBSection(hdc, fix_info, DIB_RGB_COLORS, NULL, 0, 0);
          GetObjectA(hbitmap, sizeof(DIBSECTION), &dib);
          SetDIBits(hdc, hbitmap, 0, dib.dsBm.bmHeight, bits, info,
                    DIB_RGB_COLORS);
        }
        else {
          hbitmap = CreateDIBitmap( hdc, &fix_info->bmiHeader, CBM_INIT,
                                      bits, fix_info, DIB_RGB_COLORS );
        }
        ReleaseDC( 0, hdc );
      }
      GlobalUnlock(hFix);
      GlobalFree(hFix);
    }
    if (fuLoad & LR_LOADFROMFILE) CloseHandle( hMapping );
    return hbitmap;
}
//******************************************************************************
//TODO: No support for RT_NEWBITMAP
//******************************************************************************
HBITMAP WIN32API LoadBitmapA(HINSTANCE hinst, LPCSTR lpszBitmap)
{
 HBITMAP hBitmap = 0;

  if(IsSystemBitmap((ULONG *)&lpszBitmap)) {
        hBitmap = O32_LoadBitmap(hinst, lpszBitmap);
  }
  if(!hBitmap) {
        hBitmap = LoadBitmapA(hinst, lpszBitmap, 0, 0, 0);
  }
  dprintf(("LoadBitmapA returned %08xh\n", hBitmap));

  return(hBitmap);
}
//******************************************************************************
//TODO: No support for RT_NEWBITMAP
//******************************************************************************
HBITMAP WIN32API LoadBitmapW(HINSTANCE hinst, LPCWSTR lpszBitmap)
{
 HBITMAP hBitmap = 0;

  if(IsSystemBitmap((ULONG *)&lpszBitmap)) {
        hBitmap = O32_LoadBitmap(hinst, (LPCSTR)lpszBitmap);
  }
  if(!hBitmap) {
        if(HIWORD(lpszBitmap) != 0) {
                 lpszBitmap = (LPWSTR)UnicodeToAsciiString((LPWSTR)lpszBitmap);
        }
        hBitmap = LoadBitmapA(hinst, (LPSTR)lpszBitmap, 0, 0, 0);
  }

  if(HIWORD(lpszBitmap) != 0)
        FreeAsciiString((LPSTR)lpszBitmap);

  dprintf(("LoadBitmapW returned %08xh\n", hBitmap));

  return(hBitmap);
}
//******************************************************************************
//TODO: Far from complete, but works for loading resources from exe
//fuLoad flag ignored
//******************************************************************************
HANDLE WIN32API LoadImageA(HINSTANCE hinst, LPCSTR lpszName, UINT uType,
                           int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE hRet = 0;

  if(HIWORD(lpszName)) {
  	dprintf(("LoadImageA NOT COMPLETE %x %s %d (%d,%d)\n", hinst, lpszName, uType, cxDesired, cyDesired));
  }
  else 	dprintf(("LoadImageA NOT COMPLETE %x %x %d (%d,%d)\n", hinst, lpszName, uType, cxDesired, cyDesired));

  if (fuLoad & LR_DEFAULTSIZE) {
        if (uType == IMAGE_ICON) {
            if (!cxDesired) cxDesired = GetSystemMetrics(SM_CXICON);
            if (!cyDesired) cyDesired = GetSystemMetrics(SM_CYICON);
        }
        else if (uType == IMAGE_CURSOR) {
            if (!cxDesired) cxDesired = GetSystemMetrics(SM_CXCURSOR);
            if (!cyDesired) cyDesired = GetSystemMetrics(SM_CYCURSOR);
        }
  }
  if (fuLoad & LR_LOADFROMFILE) fuLoad &= ~LR_SHARED;

  switch(uType) {
        case IMAGE_BITMAP:
                hRet = (HANDLE)LoadBitmapA(hinst, lpszName, cxDesired, cyDesired, fuLoad);
                break;
        case IMAGE_CURSOR:
                hRet = (HANDLE)LoadCursorA(hinst, lpszName);
                break;
        case IMAGE_ICON:
                hRet = (HANDLE)LoadIconA(hinst, lpszName);
                break;
        default:
                dprintf(("LoadImageA: unsupported type %d!!", uType));
                return 0;
  }
  dprintf(("LoadImageA returned %d\n", (int)hRet));

  return(hRet);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API LoadImageW(HINSTANCE hinst, LPCWSTR lpszName, UINT uType,
                              int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE hRet = 0;

  dprintf(("LoadImageW NOT COMPLETE (%d,%d)\n", cxDesired, cyDesired));

  if (fuLoad & LR_DEFAULTSIZE) {
        if (uType == IMAGE_ICON) {
            if (!cxDesired) cxDesired = GetSystemMetrics(SM_CXICON);
            if (!cyDesired) cyDesired = GetSystemMetrics(SM_CYICON);
        }
        else if (uType == IMAGE_CURSOR) {
            if (!cxDesired) cxDesired = GetSystemMetrics(SM_CXCURSOR);
            if (!cyDesired) cyDesired = GetSystemMetrics(SM_CYCURSOR);
        }
  }
  if (fuLoad & LR_LOADFROMFILE) fuLoad &= ~LR_SHARED;

  switch(uType) {
        case IMAGE_BITMAP:
                hRet = (HANDLE)LoadBitmapW(hinst, lpszName);
                break;
        case IMAGE_CURSOR:
                hRet = (HANDLE)LoadCursorW(hinst, lpszName);
                break;
        case IMAGE_ICON:
                hRet = (HANDLE)LoadIconW(hinst, lpszName);
                break;
        default:
                dprintf(("LoadImageW: unsupported type %d!!", uType));
                return 0;
  }
  dprintf(("LoadImageW returned %d\n", (int)hRet));

  return(hRet);
}
/******************************************************************************
 * CopyImage32 [USER32.61]  Creates new image and copies attributes to it
 *
 * PARAMS
 *    hnd      [I] Handle to image to copy
 *    type     [I] Type of image to copy
 *    desiredx [I] Desired width of new image
 *    desiredy [I] Desired height of new image
 *    flags    [I] Copy flags
 *
 * RETURNS
 *    Success: Handle to newly created image
 *    Failure: NULL
 *
 * FIXME: implementation still lacks nearly all features, see LR_*
 * defines in windows.h
 *
 */
HICON WINAPI CopyImage( HANDLE hnd, UINT type, INT desiredx,
                             INT desiredy, UINT flags )
{
    dprintf(("CopyImage %x %d (%d,%d) %x", hnd, type, desiredx, desiredy, flags));
    switch (type)
    {
//	case IMAGE_BITMAP:
//		return BITMAP_CopyBitmap(hnd);
	case IMAGE_ICON:
		return CopyIcon(hnd);
	case IMAGE_CURSOR:
		return CopyCursor(hnd);
	default:
		dprintf(("CopyImage: Unsupported type"));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
