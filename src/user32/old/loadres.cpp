/* $Id: loadres.cpp,v 1.5 2001-05-11 08:38:37 sandervl Exp $ */

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
#include <winres.h>
#include "pmwindow.h"

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

    if(i) {
         dprintf(("LoadStringW from %X, id %d %ls buffersize %d", hinst, wID, lpBuffer, cchBuffer));
    }
    else dprintf(("LoadStringW from %X, id %d buffersize %d", hinst, wID, cchBuffer));
    return(i);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconA(HINSTANCE hinst, LPCSTR lpszIcon)
{
    if(HIWORD(lpszIcon)) {
         dprintf(("LoadIconA %x %s", hinst, lpszIcon));
    }
    else dprintf(("LoadIconA %x %x", hinst, lpszIcon));
    return LoadImageA(hinst, lpszIcon, IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon)
{
    if(HIWORD(lpszIcon)) {
         dprintf(("LoadIconW %x %ls", hinst, lpszIcon));
    }
    else dprintf(("LoadIconW %x %x", hinst, lpszIcon));
    return LoadImageW(hinst, lpszIcon, IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorA(HINSTANCE hinst, LPCSTR lpszCursor)
{
    return LoadImageA(hinst, lpszCursor, IMAGE_CURSOR, 0, 0,
                      LR_SHARED | LR_DEFAULTSIZE );
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorW(HINSTANCE hinst, LPCWSTR lpszCursor)
{
    return LoadImageW(hinst, lpszCursor, IMAGE_CURSOR, 0, 0,
                      LR_SHARED | LR_DEFAULTSIZE );
}
/***********************************************************************
*            LoadCursorFromFileW    (USER32.361)
*/
HCURSOR WIN32API LoadCursorFromFileW (LPCWSTR name)
{
    return LoadImageW(0, name, IMAGE_CURSOR, 0, 0,
                      LR_LOADFROMFILE | LR_DEFAULTSIZE );
}
/***********************************************************************
*            LoadCursorFromFileA    (USER32.360)
*/
HCURSOR WIN32API LoadCursorFromFileA (LPCSTR name)
{
    return LoadImageA(0, name, IMAGE_CURSOR, 0, 0,
                      LR_LOADFROMFILE | LR_DEFAULTSIZE );
}
//******************************************************************************
//NOTE: LR_CREATEDIBSECTION flag doesn't work (crash in GDI32)! (still??)
//******************************************************************************
HANDLE LoadBitmapW(HINSTANCE hinst, LPCWSTR lpszName, int cxDesired, int cyDesired,
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

    if (!(fuLoad & LR_LOADFROMFILE))
    {
        handle = 0;
        if(!hinst)
        {
            hRsrc = FindResourceW( hInstanceUser32, lpszName, RT_BITMAPW );
            if(hRsrc) {
                handle = LoadResource( hInstanceUser32, hRsrc );
            }
        }
        if(handle == 0)
        {
            if (!(hRsrc = FindResourceW( hinst, lpszName, RT_BITMAPW ))) return 0;
            if (!(handle = LoadResource( hinst, hRsrc ))) return 0;
        }

        if ((info = (BITMAPINFO *)LockResource( handle )) == NULL) return 0;
    }
    else
    {
        hMapping = VIRTUAL_MapFileW( lpszName, (LPVOID *)&ptr, TRUE);
        if (hMapping == INVALID_HANDLE_VALUE) {
            //TODO: last err set to ERROR_OPEN_FAILED if file not found; correct??
            dprintf(("LoadBitmapW: failed to load file %x (lasterr=%x)", lpszName, GetLastError()));
            return 0;
        }
        info = (BITMAPINFO *)(ptr + sizeof(BITMAPFILEHEADER));
    }

    //TODO: This has to be removed once pe2lx stores win32 resources!!!
    if (info->bmiHeader.biSize != sizeof(BITMAPCOREHEADER) &&
        info->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
    {//assume it contains a file header first
        info = (BITMAPINFO *)((char *)info + sizeof(BITMAPFILEHEADER));
    }

    if (info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
    {//determine size of converted header
         BITMAPCOREHEADER *core = (BITMAPCOREHEADER *)info;

         int colors = 0;
         if (core->bcBitCount <= 8) {
                 colors = (1 << core->bcBitCount);
         }
         size =  sizeof(BITMAPINFOHEADER) + colors * sizeof(RGBQUAD);
    }
    else size = DIB_BitmapInfoSize(info, DIB_RGB_COLORS);

    if ((hFix = GlobalAlloc(0, size)) != NULL) fix_info = (BITMAPINFO *)GlobalLock(hFix);
    if (fix_info)
    {
      BYTE pix;

      if (info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
      {//convert old bitmap header to new format
            ULONG colors;
            ULONG *p, *q;

            memset (fix_info, 0, sizeof (BITMAPINFOHEADER));
            fix_info->bmiHeader.biSize     = sizeof (BITMAPINFOHEADER);
            fix_info->bmiHeader.biWidth    = ((BITMAPCOREHEADER *)info)->bcWidth;
            fix_info->bmiHeader.biHeight   = ((BITMAPCOREHEADER *)info)->bcHeight;
            fix_info->bmiHeader.biPlanes   = ((BITMAPCOREHEADER *)info)->bcPlanes;
            fix_info->bmiHeader.biBitCount = ((BITMAPCOREHEADER *)info)->bcBitCount;

            if(fix_info->bmiHeader.biBitCount <= 8)
            {
                p = (PULONG)((char *)info + sizeof(BITMAPCOREHEADER));
                q = (PULONG)((char *)fix_info + sizeof(BITMAPINFOHEADER));
                //convert RGBTRIPLE to RGBQUAD
                for (colors = 1 << fix_info->bmiHeader.biBitCount; colors > 0; colors--) {
                    *q = *p & 0x00FFFFFFUL;
                    q++;
                    p = (PULONG)((char *)p + sizeof (RGBTRIPLE));
                }
            }
      }
      else {
            memcpy(fix_info, info, size);
      }

      size = DIB_BitmapInfoSize(info, DIB_RGB_COLORS);

      pix = *((LPBYTE)info + size);
      DIB_FixColorsToLoadflags(fix_info, fuLoad, pix);
      if ((hdc = GetDC(0)) != 0)
      {
        char *bits = (char *)info + size;
        if (fuLoad & LR_CREATEDIBSECTION) {
          DIBSECTION dib;
          hbitmap = CreateDIBSection(hdc, fix_info, DIB_RGB_COLORS, NULL, 0, 0);
          GetObjectA(hbitmap, sizeof(DIBSECTION), &dib);
          SetDIBits(hdc, hbitmap, 0, dib.dsBm.bmHeight, bits, info,
                    DIB_RGB_COLORS);
        }
        else
        {
#if 0
          if(fix_info->bmiHeader.biBitCount == 1) {
              hbitmap = CreateBitmap(fix_info->bmiHeader.biWidth,
                                     fix_info->bmiHeader.biHeight,
                                     fix_info->bmiHeader.biPlanes,
                                     fix_info->bmiHeader.biBitCount,
                                     (PVOID)bits);
          }
          else {
#endif
              hbitmap = CreateDIBitmap(hdc, &fix_info->bmiHeader, CBM_INIT,
                                       bits, fix_info, DIB_RGB_COLORS );
//          }
          if(hbitmap == 0) {
              dprintf(("LoadBitmapW: CreateDIBitmap failed!!"));
          }
        }
        ReleaseDC( 0, hdc );
      }
      GlobalUnlock(hFix);
      GlobalFree(hFix);
    }
    if(fuLoad & LR_LOADFROMFILE) {
        UnmapViewOfFile(ptr);
        CloseHandle(hMapping);
    }
    return hbitmap;
}
//******************************************************************************
//TODO: scale bitmap
//******************************************************************************
HANDLE CopyBitmap(HANDLE hBitmap, DWORD desiredx, DWORD desiredy)
{
    HBITMAP res = 0;
    BITMAP bm;

    if(GetObjectA(hBitmap, sizeof(BITMAP), &bm) == FALSE) {
        dprintf(("CopyBitmap: GetObject failed!!"));
        return 0;
    }

    bm.bmBits = NULL;
    res = CreateBitmapIndirect(&bm);

    if(res)
    {
        char *buf = (char *)HeapAlloc( GetProcessHeap(), 0, bm.bmWidthBytes *
                                       bm.bmHeight );
        GetBitmapBits (hBitmap, bm.bmWidthBytes * bm.bmHeight, buf);
        SetBitmapBits (res, bm.bmWidthBytes * bm.bmHeight, buf);
        HeapFree( GetProcessHeap(), 0, buf );
    }
    return res;
}
//******************************************************************************
//TODO: No support for RT_NEWBITMAP
//******************************************************************************
HBITMAP WIN32API LoadBitmapA(HINSTANCE hinst, LPCSTR lpszBitmap)
{
 HBITMAP hBitmap = 0;

  hBitmap = LoadImageA(hinst, lpszBitmap, IMAGE_BITMAP, 0, 0, 0);

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

  hBitmap = LoadBitmapW((hinst == 0) ? hInstanceUser32:hinst, lpszBitmap, 0, 0, 0);

  if(HIWORD(lpszBitmap)) {
        dprintf(("LoadBitmapW %x %ls returned %08xh\n", hinst, lpszBitmap, hBitmap));
  }
  else  dprintf(("LoadBitmapW %x %x returned %08xh\n", hinst, lpszBitmap, hBitmap));

  return(hBitmap);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API LoadImageA(HINSTANCE hinst, LPCSTR lpszName, UINT uType,
                           int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE res = 0;
 LPCWSTR u_name;

  if(HIWORD(lpszName)) {
        dprintf(("LoadImageA %x %s %d (%d,%d)\n", hinst, lpszName, uType, cxDesired, cyDesired));
  }
  else  dprintf(("LoadImageA %x %x %d (%d,%d)\n", hinst, lpszName, uType, cxDesired, cyDesired));

  if (HIWORD(lpszName)) {
        u_name = HEAP_strdupAtoW(GetProcessHeap(), 0, lpszName);
  }
  else  u_name=(LPWSTR)lpszName;

  res = LoadImageW(hinst, u_name, uType, cxDesired, cyDesired, fuLoad);

  if (HIWORD(lpszName))
        HeapFree(GetProcessHeap(), 0, (LPVOID)u_name);

  return res;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API LoadImageW(HINSTANCE hinst, LPCWSTR lpszName, UINT uType,
                           int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE hRet = 0;

  if(HIWORD(lpszName)) {
        dprintf(("LoadImageW %x %ls %d (%d,%d)\n", hinst, lpszName, uType, cxDesired, cyDesired));
  }
  else  dprintf(("LoadImageW %x %x %d (%d,%d)\n", hinst, lpszName, uType, cxDesired, cyDesired));

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

  switch (uType)
  {
        case IMAGE_BITMAP:
        {
            hRet = (HANDLE)LoadBitmapW(hinst, lpszName, cxDesired, cyDesired, fuLoad);
            break;
        }
        case IMAGE_ICON:
        {
#ifdef __WIN32OS2__
            ULONG palEnts = (1 << ScreenBitsPerPel);
#else
            HDC hdc = GetDC(0);
            UINT palEnts = GetSystemPaletteEntries(hdc, 0, 0, NULL);
            if (palEnts == 0)
                palEnts = 256;
            ReleaseDC(0, hdc);
#endif

            hRet = CURSORICON_Load(hinst, lpszName, cxDesired, cyDesired,  palEnts, FALSE, fuLoad);
            break;
        }

        case IMAGE_CURSOR:
            return CURSORICON_Load(hinst, lpszName, cxDesired, cyDesired, 1, TRUE, fuLoad);

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
HICON WINAPI CopyImage(HANDLE hnd, UINT type, INT desiredx,
                       INT desiredy, UINT flags )
{
    dprintf(("CopyImage %x %d (%d,%d) %x", hnd, type, desiredx, desiredy, flags));
    switch (type)
    {
        case IMAGE_BITMAP:
                return CopyBitmap(hnd, desiredx, desiredy);
        case IMAGE_ICON:
                return (HANDLE)CURSORICON_ExtCopy(hnd, type, desiredx, desiredy, flags);
        case IMAGE_CURSOR:
                /* Should call CURSORICON_ExtCopy but more testing
                 * needs to be done before we change this
                 */
                return CURSORICON_ExtCopy(hnd,type, desiredx, desiredy, flags);
        default:
                dprintf(("CopyImage: Unsupported type"));
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
