/* $Id: winicon.cpp,v 1.10 2000-03-23 23:06:54 sandervl Exp $ */
/*
 * Win32 Icon Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <winicon.h>
#include <win\cursoricon.h>

#define DBG_LOCALLOG	DBG_winicon
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIcon( HINSTANCE arg1, INT arg2, INT arg3, BYTE arg4, BYTE arg5, LPCVOID arg6, LPCVOID arg7)
{
    dprintf(("USER32:  CreateIcon\n"));
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
    dprintf(("USER32:  CreateIconFromResource returned %X (%X)\n", hicon, GetLastError()));
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
    dprintf(("USER32:  CreateIconFromResourceEx %X %d %d %X %d %d %X, not completely supported!\n", presbits, dwResSize, fIcon, dwVer, cxDesired, cyDesired, Flags));
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
                hdcSrc, 0, 0, SRCCOPY);
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
HICON CreateIconIndirect(LPICONINFO pIcon, BOOL bIsIcon, int desiredX, int desiredY, DWORD flags)
{
 HICON   hIcon;
 HDC     hdcSrc, hdcDst;
 BITMAP  bmp;

    if(pIcon->hbmMask && pIcon->hbmColor)
    {
        ICONINFO iconinfo;
        HBITMAP hbmOldSrc, hbmOldDst;

        iconinfo = *pIcon;
        GetObjectA(pIcon->hbmColor, sizeof(BITMAP), (LPVOID)&bmp);

        //if there's a color bitmap, the mask bitmap contains only the AND bits
        //Open32 calls WinCreatePointerIndirect which expects AND & XOR bits
        //To solve this we create a bitmap that's 2x height of the mask, copy
        //the AND bits and set the XOR bits to 0
        hdcSrc = CreateCompatibleDC(0);
        hdcDst = CreateCompatibleDC(0);

        iconinfo.hbmMask  = CreateCompatibleBitmap (hdcDst, desiredX, desiredY*2);
        hbmOldDst = SelectObject (hdcDst, iconinfo.hbmMask);
        hbmOldSrc = SelectObject (hdcSrc, pIcon->hbmMask);
        if(desiredX != bmp.bmWidth || desiredY != bmp.bmHeight) {
                StretchBlt(hdcDst, 0, 0, desiredX, desiredY, hdcSrc, 0, 0,
                           bmp.bmWidth, bmp.bmHeight, SRCCOPY);
        }
        else {
                BitBlt (hdcDst, 0, 0, bmp.bmWidth, bmp.bmHeight,
                        hdcSrc, 0, 0, SRCCOPY);
        }
        PatBlt (hdcDst, desiredX, desiredY, desiredX, desiredY, BLACKNESS);

        if(desiredX != bmp.bmWidth || desiredY != bmp.bmHeight) {
                iconinfo.hbmColor  = CreateCompatibleBitmap (hdcDst, desiredX, desiredY);
                SelectObject (hdcDst, iconinfo.hbmColor);
                SelectObject (hdcSrc, pIcon->hbmColor);
                StretchBlt(hdcDst, 0, 0, desiredX, desiredY, hdcSrc, 0, 0,
                           bmp.bmWidth, bmp.bmHeight, SRCCOPY);
        }

        hIcon = O32_CreateIconIndirect(&iconinfo);

        DeleteObject(iconinfo.hbmMask);
        if(desiredX != bmp.bmWidth || desiredY != bmp.bmHeight) {
                DeleteObject(iconinfo.hbmColor);
        }
        SelectObject (hdcDst, hbmOldDst);
        SelectObject (hdcSrc, hbmOldSrc);
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
BOOL WIN32API DestroyIcon( HICON hIcon)
{
    dprintf(("USER32: DestroyIcon %x", hIcon));
    return O32_DestroyIcon(hIcon);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CopyIcon( HICON hIcon)
{
    dprintf(("USER32:  CopyIcon %x", hIcon));
    return O32_CopyIcon(hIcon);
}
//******************************************************************************
//WARNING: MEMORY LEAK & DIRTY HACK TO WORK AROUND OPEN32 BUG
//OS/2 icon masks must be twice the height of the color bitmap
//In Windows, the mask only contains the AND data if there's a color bitmap
//--->> We're allocating a bitmap to replace the mask bitmap, but DON'T DELETE it!
//WARNING: DIRTY HACK TO WORK AROUND OPEN32 BUG
//******************************************************************************
BOOL WIN32API GetIconInfo( HICON hIcon, LPICONINFO pIconInfo)
{
 BOOL rc;
 HBITMAP hbmMask, hbmOldSrc, hbmOldDst;
 BITMAP bmp;

    dprintf(("USER32: GetIconInfo %x", hIcon));
    rc = O32_GetIconInfo(hIcon, pIconInfo);
#if 1
    if(rc && pIconInfo->hbmColor)
    {
        HDC  hdcSrc, hdcDst;
        hdcSrc = CreateCompatibleDC(0);
        hdcDst = CreateCompatibleDC(0);

        GetObjectA(pIconInfo->hbmColor, sizeof(BITMAP), (LPVOID)&bmp);

        hbmMask = CreateCompatibleBitmap (hdcDst, bmp.bmWidth, bmp.bmHeight);
        hbmOldDst = SelectObject (hdcDst, hbmMask);
        hbmOldSrc = SelectObject (hdcSrc, pIconInfo->hbmMask);
        BitBlt (hdcDst, 0, 0, bmp.bmWidth, bmp.bmHeight,
                hdcSrc, 0, 0, SRCCOPY);

        SelectObject(hdcDst, hbmOldDst);
        SelectObject(hdcSrc, hbmOldSrc);
        DeleteDC(hdcDst);
        DeleteDC(hdcSrc);
        DeleteObject(pIconInfo->hbmMask);
        pIconInfo->hbmMask = hbmMask;
    }
#endif
    return rc;
}
/**********************************************************************
 *          CURSORICON_FindBestIcon
 *
 * Find the icon closest to the requested size and number of colors.
 */
static CURSORICONDIRENTRY *CURSORICON_FindBestIcon( CURSORICONDIR *dir, int width,
                                                    int height, int colors )
{
    int i;
    CURSORICONDIRENTRY *entry, *bestEntry = NULL;
    UINT iTotalDiff, iXDiff=0, iYDiff=0, iColorDiff;
    UINT iTempXDiff, iTempYDiff, iTempColorDiff;

    if (dir->idCount < 1)
    {
        dprintf(("Empty directory!\n" ));
        return NULL;
    }
    if (dir->idCount == 1) return &dir->idEntries[0];  /* No choice... */

    /* Find Best Fit */
    iTotalDiff = 0xFFFFFFFF;
    iColorDiff = 0xFFFFFFFF;
    for (i = 0, entry = &dir->idEntries[0]; i < dir->idCount; i++,entry++)
        {
        iTempXDiff = abs(width - entry->ResInfo.icon.bWidth);
        iTempYDiff = abs(height - entry->ResInfo.icon.bHeight);

        if(iTotalDiff > (iTempXDiff + iTempYDiff))
        {
            iXDiff = iTempXDiff;
            iYDiff = iTempYDiff;
            iTotalDiff = iXDiff + iYDiff;
        }
        }

    /* Find Best Colors for Best Fit */
    for (i = 0, entry = &dir->idEntries[0]; i < dir->idCount; i++,entry++)
        {
        if(abs(width - entry->ResInfo.icon.bWidth) == iXDiff &&
            abs(height - entry->ResInfo.icon.bHeight) == iYDiff)
        {
            iTempColorDiff = abs(colors - entry->ResInfo.icon.bColorCount);
            if(iColorDiff > iTempColorDiff)
        {
            bestEntry = entry;
                iColorDiff = iTempColorDiff;
        }
        }
    }

    return bestEntry;
}


/**********************************************************************
 *          CURSORICON_FindBestCursor
 *
 * Find the cursor closest to the requested size.
 * FIXME: parameter 'color' ignored and entries with more than 1 bpp
 *        ignored too
 */
static CURSORICONDIRENTRY *CURSORICON_FindBestCursor( CURSORICONDIR *dir,
                                                  int width, int height, int color)
{
    int i, maxwidth, maxheight;
    CURSORICONDIRENTRY *entry, *bestEntry = NULL;

    if (dir->idCount < 1)
    {
        dprintf(("Empty directory!\n" ));
        return NULL;
    }
    if (dir->idCount == 1) return &dir->idEntries[0]; /* No choice... */

    /* Double height to account for AND and XOR masks */

    height *= 2;

    /* First find the largest one smaller than or equal to the requested size*/

    maxwidth = maxheight = 0;
    for(i = 0,entry = &dir->idEntries[0]; i < dir->idCount; i++,entry++)
        if ((entry->ResInfo.cursor.wWidth <= width) && (entry->ResInfo.cursor.wHeight <= height) &&
            (entry->ResInfo.cursor.wWidth > maxwidth) && (entry->ResInfo.cursor.wHeight > maxheight) &&
            (entry->wBitCount == 1))
        {
            bestEntry = entry;
            maxwidth  = entry->ResInfo.cursor.wWidth;
            maxheight = entry->ResInfo.cursor.wHeight;
        }
    if (bestEntry) return bestEntry;

    /* Now find the smallest one larger than the requested size */

    maxwidth = maxheight = 255;
    for(i = 0,entry = &dir->idEntries[0]; i < dir->idCount; i++,entry++)
        if ((entry->ResInfo.cursor.wWidth < maxwidth) && (entry->ResInfo.cursor.wHeight < maxheight) &&
            (entry->wBitCount == 1))
        {
            bestEntry = entry;
            maxwidth  = entry->ResInfo.cursor.wWidth;
            maxheight = entry->ResInfo.cursor.wHeight;
        }

    return bestEntry;
}
/**********************************************************************
 *          LookupIconIdFromDirectoryEx16       (USER.364)
 *
 * FIXME: exact parameter sizes
 */
INT WIN32API LookupIconIdFromDirectoryEx(LPBYTE xdir, BOOL bIcon,
                                         INT width, INT height, UINT cFlag )
{
    CURSORICONDIR       *dir = (CURSORICONDIR*)xdir;
    UINT retVal = 0;

    dprintf(("LookupIconIdFromDirectoryEx %x %d (%d,%d)", xdir, bIcon, width, height));
    if( dir && !dir->idReserved && (dir->idType & 3) )
    {
        CURSORICONDIRENTRY* entry;
        HDC hdc;
        UINT palEnts;
        int colors;
        hdc = GetDC(0);
        palEnts = GetSystemPaletteEntries(hdc, 0, 0, NULL);
        if (palEnts == 0)
            palEnts = 256;
        colors = (cFlag & LR_MONOCHROME) ? 2 : palEnts;

        ReleaseDC(0, hdc);

        if( bIcon )
            entry = CURSORICON_FindBestIcon( dir, width, height, colors );
        else
            entry = CURSORICON_FindBestCursor( dir, width, height, 1);

        if( entry ) retVal = entry->wResId;
    }
    else dprintf(("invalid resource directory\n"));
    return retVal;
}
/**********************************************************************
 *          LookupIconIdFromDirectory           (USER32.379)
 */
INT WIN32API LookupIconIdFromDirectory( LPBYTE dir, BOOL bIcon )
{
    return LookupIconIdFromDirectoryEx( dir, bIcon,
           bIcon ? GetSystemMetrics(SM_CXICON) : GetSystemMetrics(SM_CXCURSOR),
           bIcon ? GetSystemMetrics(SM_CYICON) : GetSystemMetrics(SM_CYCURSOR), bIcon ? 0 : LR_MONOCHROME );
}
/*************************************************************************
 * CURSORICON_ExtCopy
 *
 * Copies an Image from the Cache if LR_COPYFROMRESOURCE is specified
 *
 * PARAMS
 *      Handle     [I] handle to an Image
 *      nType      [I] Type of Handle (IMAGE_CURSOR | IMAGE_ICON)
 *      iDesiredCX [I] The Desired width of the Image
 *      iDesiredCY [I] The desired height of the Image
 *      nFlags     [I] The flags from CopyImage
 *
 * RETURNS
 *     Success: The new handle of the Image
 *
 * NOTES
 *     LR_COPYDELETEORG and LR_MONOCHROME are currently not implemented.
 *     LR_MONOCHROME should be implemented by CURSORICON_CreateFromResource.
 *     LR_COPYFROMRESOURCE will only work if the Image is in the Cache.
 *
 *
 * TODO: LR_COPYFROMRESOURCE doesn't work. Uses supplied icon instead
 *
 */
HGLOBAL CopyCursorIcon(HGLOBAL Handle, UINT nType,
                       INT iDesiredCX, INT iDesiredCY,
                       UINT nFlags)
{
    HGLOBAL hNew=0;
    BOOL bIsIcon = (nType == IMAGE_ICON);

    if(Handle == 0)
    {
        return 0;
    }

    /* Best Fit or Monochrome */
    if(!bIsIcon || (nFlags & LR_COPYFROMRESOURCE
        && (iDesiredCX > 0 || iDesiredCY > 0))
        || nFlags & LR_MONOCHROME)
    {
            LPBYTE pBits;
            HANDLE hMem;
            HRSRC hRsrc;
            DWORD dwBytesInRes;
            WORD wResId;
            CURSORICONDIR *pDir;
            CURSORICONDIRENTRY *pDirEntry;

            /* Completing iDesiredCX CY for Monochrome Bitmaps if needed
            */
            if(((nFlags & LR_MONOCHROME) && !(nFlags & LR_COPYFROMRESOURCE))
                || (iDesiredCX == 0 && iDesiredCY == 0))
            {
                iDesiredCY = GetSystemMetrics(bIsIcon ?
                    SM_CYICON : SM_CYCURSOR);
                iDesiredCX = GetSystemMetrics(bIsIcon ?
                    SM_CXICON : SM_CXCURSOR);
            }

            /* Create a New Icon with the proper dimension
            */
            ICONINFO iconinfo;

            GetIconInfo(Handle, &iconinfo);
            hNew = CreateIconIndirect(&iconinfo, bIsIcon, iDesiredCX, iDesiredCY, nFlags);
    }
    else
    {
        if(bIsIcon) {
                return CopyIcon(Handle);
        }
        else    return CopyCursor(Handle);
    }
    return hNew;
}
//******************************************************************************
//******************************************************************************
