/* $Id: loadres.cpp,v 1.29 2000-05-28 16:43:45 sandervl Exp $ */

/*
 * Win32 resource API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 * Parts based on Wine code (objects\bitmap.c, loader\resource.c, objects\cursoricon.c):
 *
 * Copyright 1993 Alexandre Julliard
 *           1993 Robert J. Amstadt
 *           1996 Martin Von Loewis
 *           1997 Alex Korobka
 *           1998 Turchanov Sergey
 *           1998 Huw D M Davies
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <user32.h>
#include <heapstring.h>
#include <oslibres.h>
#include <win\virtual.h>
#include "dib.h"
#include "initterm.h"
#include <win\cursoricon.h>
#include <winres.h>

#define DBG_LOCALLOG    DBG_loadres
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
INT WIN32API LoadStringA(HINSTANCE instance, UINT resource_id,
                         LPSTR buffer, INT buflen )
{
  INT retval;
  LPWSTR buffer2 = NULL;

    if (buffer && buflen)
        buffer2 = (LPWSTR)HeapAlloc( GetProcessHeap(), 0, buflen * 2 );

    retval = LoadStringW(instance,resource_id,buffer2,buflen);

    if (buffer2)
    {
        if (retval) {
            lstrcpynWtoA( buffer, buffer2, buflen );
            retval = lstrlenA( buffer );
        }
        else
            *buffer = 0;
        HeapFree( GetProcessHeap(), 0, buffer2 );
    }
    return retval;
}
//******************************************************************************
//******************************************************************************
int WIN32API LoadStringW(HINSTANCE hinst, UINT wID, LPWSTR lpBuffer, int cchBuffer)
{
 WCHAR *p;
 int string_num;
 int i = 0;
 HRSRC hRes;

    /* Use bits 4 - 19 (incremented by 1) as resourceid, mask out
     * 20 - 31. */
    hRes = FindResourceW(hinst, (LPWSTR)(((wID>>4)&0xffff)+1), RT_STRINGW);
    if(hRes == NULL) {
        dprintf(("LoadStringW NOT FOUND from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
        *lpBuffer = 0;
        return 0;
    }

    p = (LPWSTR)LockResource(LoadResource(hinst, hRes));
    if(p) {
        string_num = wID & 0x000f;
        for (i = 0; i < string_num; i++)
                p += *p + 1;

        if (lpBuffer == NULL) return *p;
        i = min(cchBuffer - 1, *p);
        if (i > 0) {
                memcpy(lpBuffer, p + 1, i * sizeof (WCHAR));
                lpBuffer[i] = (WCHAR) 0;
        }
        else {
                if (cchBuffer > 1) {
                        lpBuffer[0] = (WCHAR) 0;
                        return 0;
                }
        }
    }

#ifdef DEBUG_ENABLELOG_LEVEL2
    if(i) {
        char *astring = (char *)HEAP_strdupWtoA(GetProcessHeap(), 0, lpBuffer);
        dprintf(("LoadStringW from %X, id %d %s\n", hinst, wID, astring));
        HEAP_free(astring);
    }
#else
    dprintf(("LoadStringW from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
#endif
    return(i);
}
//******************************************************************************
//******************************************************************************
HICON LoadIconA(HINSTANCE hinst, LPCSTR lpszIcon, DWORD cxDesired,
                DWORD cyDesired, DWORD fuLoad)
{
 HICON          hIcon;
 HANDLE 	hMapping = 0;
 char 	       *ptr = NULL;
 HRSRC          hRes;
 LPSTR     	restype = RT_ICONA;

    if(fuLoad & LR_LOADFROMFILE) 
    {
        hMapping = VIRTUAL_MapFileA( lpszIcon, (LPVOID *)&ptr, TRUE);
        if(hMapping == INVALID_HANDLE_VALUE) 
		return 0;
       	hIcon = OSLibWinCreatePointer(ptr);
	CloseHandle(hMapping);    
    }
    else
    {
    	if(!hinst)
    	{
	      	hRes = FindResourceA(hInstanceUser32,lpszIcon,RT_ICONA);
	      	if(!hRes)  {
			hRes = FindResourceA(hInstanceUser32,lpszIcon,RT_GROUP_ICONA);
			restype = RT_GROUP_ICONA;
		}
	      	if(hRes)
	      	{
	                hIcon = OSLibWinCreateIcon(ConvertResourceToOS2(hInstanceUser32, restype, hRes));
	      	} 
		else 	hIcon = OSLibWinQuerySysIcon((ULONG)lpszIcon,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON));
    	} 
    	else
    	{ //not a system icon
	      	hRes = FindResourceA(hinst,lpszIcon,RT_ICONA);
	      	if(!hRes)  {
			hRes = FindResourceA(hinst,lpszIcon,RT_GROUP_ICONA);
			restype = RT_GROUP_ICONA;
		}
	        if(hRes) {
	                hIcon = OSLibWinCreateIcon(ConvertResourceToOS2(hinst, restype, hRes));
	        } 
		else 	hIcon = 0;
	}
    }
    dprintf(("LoadIconA (%X) returned %x\n", hinst, hIcon));

    return(hIcon);
}
//******************************************************************************
//******************************************************************************
HICON LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon, DWORD cxDesired,
                DWORD cyDesired, DWORD fuLoad)
{
 HICON          hIcon;
 HANDLE 	hMapping = 0;
 char 	       *ptr = NULL;
 HRSRC          hRes;
 LPSTR     	restype = RT_ICONA;

    if(fuLoad & LR_LOADFROMFILE) 
    {
        hMapping = VIRTUAL_MapFileW( lpszIcon, (LPVOID *)&ptr, TRUE);
        if(hMapping == INVALID_HANDLE_VALUE) 
		return 0;
       	hIcon = OSLibWinCreatePointer(ptr);
	CloseHandle(hMapping);    
    }
    else
    {
    	if (!hinst)
    	{
	      	hRes = FindResourceW(hInstanceUser32,lpszIcon,RT_ICONW);
	      	if(!hRes)  {
			hRes = FindResourceW(hInstanceUser32,lpszIcon,RT_GROUP_ICONW);
			restype = RT_GROUP_ICONA;
		}
	      	if(hRes)
	      	{
	                hIcon = OSLibWinCreateIcon(ConvertResourceToOS2(hInstanceUser32, restype, hRes));
	      	} 
		else 	hIcon = OSLibWinQuerySysIcon((ULONG)lpszIcon,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON));
	} 
	else
    	{//not a system icon
	      	hRes = FindResourceW(hinst,lpszIcon,RT_ICONW);
	      	if(!hRes)  {
			hRes = FindResourceW(hinst,lpszIcon,RT_GROUP_ICONW);
			restype = RT_GROUP_ICONA;
		}
	        if(hRes) {
	                hIcon = OSLibWinCreateIcon(ConvertResourceToOS2(hinst, restype, hRes));
	        } 
		else 	hIcon = 0;
	}
    }
    dprintf(("LoadIconW (%X) returned %x\n", hinst, hIcon));

    return(hIcon);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconA(HINSTANCE hinst, LPCSTR lpszIcon)
{
    return LoadIconA(hinst, lpszIcon, 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon)
{
    return LoadIconW(hinst, lpszIcon, 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
HCURSOR LoadCursorA(HINSTANCE hinst, LPCSTR lpszCursor, DWORD cxDesired, 
                    DWORD cyDesired, DWORD fuLoad)
{
 HCURSOR        hCursor;
 HANDLE 	hMapping = 0;
 char 	       *ptr = NULL;
 HRSRC          hRes;
 LPSTR     	restype = RT_CURSORA;

    if(fuLoad & LR_LOADFROMFILE) 
    {
        hMapping = VIRTUAL_MapFileA( lpszCursor, (LPVOID *)&ptr, TRUE);
        if(hMapping == INVALID_HANDLE_VALUE) 
		return 0;
       	hCursor = OSLibWinCreatePointer(ptr);
	CloseHandle(hMapping);    
    }
    else
    {
    	if(!hinst)
    	{
	      	hRes = FindResourceA(hInstanceUser32,lpszCursor,RT_CURSORA);
	      	if(!hRes)  {
			hRes = FindResourceA(hInstanceUser32,lpszCursor,RT_GROUP_CURSORA);
			restype = RT_GROUP_CURSORA;
		}
	      	if(hRes)
	      	{
	                hCursor = OSLibWinCreatePointer(ConvertResourceToOS2(hInstanceUser32, restype, hRes));
	      	} 
		else 	hCursor = OSLibWinQuerySysPointer((ULONG)lpszCursor,GetSystemMetrics(SM_CXCURSOR),GetSystemMetrics(SM_CYCURSOR));
    	} 
    	else
    	{ //not a system icon
	      	hRes = FindResourceA(hinst,lpszCursor,RT_CURSORA);
	      	if(!hRes)  {
			hRes = FindResourceA(hinst,lpszCursor,RT_GROUP_CURSORA);
			restype = RT_GROUP_CURSORA;
		}
	        if(hRes) {
	                hCursor = OSLibWinCreatePointer(ConvertResourceToOS2(hinst, restype, hRes));
	        } 
		else 	hCursor = 0;
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
HCURSOR LoadCursorW(HINSTANCE hinst, LPCWSTR lpszCursor, DWORD cxDesired, 
                    DWORD cyDesired, DWORD fuLoad)
{
 HCURSOR        hCursor;
 HANDLE 	hMapping = 0;
 char 	       *ptr = NULL;
 HRSRC          hRes;
 LPSTR     	restype = RT_CURSORA;

    if(fuLoad & LR_LOADFROMFILE) 
    {
        hMapping = VIRTUAL_MapFileW( lpszCursor, (LPVOID *)&ptr, TRUE);
        if(hMapping == INVALID_HANDLE_VALUE) 
		return 0;
       	hCursor = OSLibWinCreatePointer(ptr);
	CloseHandle(hMapping);    
    }
    else
    {
    	if(!hinst)
    	{
	      	hRes = FindResourceW(hInstanceUser32,lpszCursor,RT_CURSORW);
	      	if(!hRes)  {
			hRes = FindResourceW(hInstanceUser32,lpszCursor,RT_GROUP_CURSORW);
			restype = RT_GROUP_CURSORA;
		}
	      	if(hRes)
	      	{
	                hCursor = OSLibWinCreatePointer(ConvertResourceToOS2(hInstanceUser32, restype, hRes));
	      	} 
		else 	hCursor = OSLibWinQuerySysPointer((ULONG)lpszCursor,GetSystemMetrics(SM_CXCURSOR),GetSystemMetrics(SM_CYCURSOR));
    	} 
    	else
    	{ //not a system icon
	      	hRes = FindResourceW(hinst,lpszCursor,RT_CURSORW);
	      	if(!hRes)  {
			hRes = FindResourceW(hinst,lpszCursor,RT_GROUP_CURSORW);
			restype = RT_GROUP_CURSORA;
		}
	        if(hRes) {
	                hCursor = OSLibWinCreatePointer(ConvertResourceToOS2(hinst, restype, hRes));
	        } 
		else 	hCursor = 0;
	}
    }
    dprintf(("LoadCursorW (%X) returned %x\n", hinst, hCursor));

    return(hCursor);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorA(HINSTANCE hinst, LPCSTR lpszCursor)
{
    return LoadCursorA(hinst, lpszCursor, 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorW(HINSTANCE hinst, LPCWSTR lpszCursor)
{
    return LoadCursorW(hinst, lpszCursor, 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
BOOL IsSystemBitmap(ULONG id)
{
   switch(id)
   {
        case OBM_UPARROW:
        case OBM_DNARROW:
        case OBM_RGARROW:
        case OBM_LFARROW:
        case OBM_RESTORE:
        case OBM_RESTORED:
        case OBM_UPARROWD:
        case OBM_DNARROWD:
        case OBM_RGARROWD:
        case OBM_LFARROWD:
        case OBM_OLD_UPARROW:
        case OBM_OLD_DNARROW:
        case OBM_OLD_RGARROW:
        case OBM_OLD_LFARROW:
        case OBM_CHECK:
        case OBM_RADIOCHECK:
        case OBM_CHECKBOXES:
        case OBM_BTNCORNERS:
        case OBM_COMBO:
        case OBM_REDUCE:
        case OBM_REDUCED:
        case OBM_ZOOM:
        case OBM_ZOOMD:
        case OBM_SIZE:
        case OBM_CLOSE:
        case OBM_MNARROW:
        case OBM_UPARROWI:
        case OBM_DNARROWI:
        case OBM_RGARROWI:
        case OBM_LFARROWI:
        case OBM_CLOSED:
        case OBM_OLD_CLOSE:
        case OBM_BTSIZE:
        case OBM_OLD_REDUCE:
        case OBM_OLD_ZOOM:
        case OBM_OLD_RESTORE:
        case OBM_CONTEXTHELP:
        case OBM_CONTEXTHELPD:
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
        hMapping = VIRTUAL_MapFileA( lpszName, (LPVOID *)&ptr, TRUE);
        if (hMapping == INVALID_HANDLE_VALUE) return 0;
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
//        if(fix_info->bmiHeader.biBitCount == 1) {
//              hbitmap = CreateBitmap(fix_info->bmiHeader.biWidth,
//                                       fix_info->bmiHeader.biHeight,
//                                       fix_info->bmiHeader.biPlanes,
//                                       fix_info->bmiHeader.biBitCount,
//                                       (PVOID)bits);
//        }
//        else {
                hbitmap = CreateDIBitmap(hdc, &fix_info->bmiHeader, CBM_INIT,
                                         bits, fix_info, DIB_RGB_COLORS );
		if(hbitmap == 0) {
			dprintf(("LoadBitmapA: CreateDIBitmap failed!!"));
		}
//        }
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

  if (!hinst)
  {
    if(IsSystemBitmap((ULONG)lpszBitmap))
    {
         hBitmap =  LoadBitmapA(hInstanceUser32, lpszBitmap, 0, 0, 0);
    }
  }
  if(!hBitmap)
        hBitmap = LoadBitmapA(hinst, lpszBitmap, 0, 0, 0);

  if(HIWORD(lpszBitmap)) {
  	dprintf(("LoadBitmapA %x %s returned %08xh\n", hinst, lpszBitmap, hBitmap));
  }
  else  dprintf(("LoadBitmapA %x %x returned %08xh\n", hinst, lpszBitmap, hBitmap));

  return(hBitmap);
}
//******************************************************************************
//TODO: No support for RT_NEWBITMAP
//******************************************************************************
HBITMAP WIN32API LoadBitmapW(HINSTANCE hinst, LPCWSTR lpszBitmap)
{
 HBITMAP hBitmap = 0;

  if(HIWORD(lpszBitmap) != 0)
    lpszBitmap = (LPWSTR)UnicodeToAsciiString((LPWSTR)lpszBitmap);

  hBitmap = LoadBitmapA((hinst == 0) ? hInstanceUser32:hinst, (LPSTR)lpszBitmap, 0, 0, 0);

  if(HIWORD(lpszBitmap) != 0)
    FreeAsciiString((LPSTR)lpszBitmap);

  if(HIWORD(lpszBitmap)) {
  	dprintf(("LoadBitmapW %x %s returned %08xh\n", hinst, lpszBitmap, hBitmap));
  }
  else  dprintf(("LoadBitmapW %x %x returned %08xh\n", hinst, lpszBitmap, hBitmap));

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
  else  dprintf(("LoadImageA NOT COMPLETE %x %x %d (%d,%d)\n", hinst, lpszName, uType, cxDesired, cyDesired));

  if(fuLoad & LR_DEFAULTSIZE) {
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
                hRet = (HANDLE)LoadCursorA(hinst, lpszName, cxDesired, cyDesired, fuLoad);
                break;
        case IMAGE_ICON:
                hRet = (HANDLE)LoadIconA(hinst, lpszName, cxDesired, cyDesired, fuLoad);
                break;
        default:
                dprintf(("LoadImageA: unsupported type %d!!", uType));
                return 0;
  }
  dprintf(("LoadImageA returned %x\n", (int)hRet));

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
                hRet = (HANDLE)LoadCursorW(hinst, lpszName, cxDesired, cyDesired, fuLoad);
                break;
        case IMAGE_ICON:
                hRet = (HANDLE)LoadIconW(hinst, lpszName, cxDesired, cyDesired, fuLoad);
                break;
        default:
                dprintf(("LoadImageW: unsupported type %d!!", uType));
                return 0;
  }
  dprintf(("LoadImageW returned %x\n", (int)hRet));

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
//      case IMAGE_BITMAP:
//              return BITMAP_CopyBitmap(hnd);
        case IMAGE_ICON:
                return CopyIcon(hnd);
        case IMAGE_CURSOR:
                return CopyCursor(hnd);
//              return CopyCursorIcon(hnd,type, desiredx, desiredy, flags);
        default:
                dprintf(("CopyImage: Unsupported type"));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
