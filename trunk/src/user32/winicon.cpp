/* $Id: winicon.cpp,v 1.25 2001-07-04 09:55:18 sandervl Exp $ */
/*
 * Win32 Icon Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl) (OS/2 Port)
 *
 * Based on Wine code (objects\bitmap.c, loader\resource.c, objects\cursoricon.c):
 *
 * Copyright 1993 Alexandre Julliard
 *           1993 Robert J. Amstadt
 *           1996 Martin Von Loewis
 *           1997 Alex Korobka
 *           1998 Turchanov Sergey
 *           1998 Huw D M Davies
 * Theory:
 *
 * http://www.microsoft.com/win32dev/ui/icons.htm
 *
 * Cursors and icons are stored in a global heap block, with the
 * following layout:
 *
 * CURSORICONINFO info;
 * BYTE[]         ANDbits;
 * BYTE[]         XORbits;
 *
 * The bits structures are in the format of a device-dependent bitmap.
 *
 * This layout is very sub-optimal, as the bitmap bits are stored in
 * the X client instead of in the server like other bitmaps; however,
 * some programs (notably Paint Brush) expect to be able to manipulate
 * the bits directly :-(
 *
 * FIXME: what are we going to do with animation and color (bpp > 1) cursors ?!
 *
 **************************************************************************************************
 *
 * TODO: Scaling of system cursors (store them as resources in user32 instead of using PM pointers)
 * TODO: We use the hColorBmp member of the CURSORICONINFO structure to store the PM cursor handle
 *       Might mess up PaintBrush (see above)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <string.h>
#include <winicon.h>
#include <win\cursoricon.h>
#include "dib.h"
#include <heapstring.h>
#include <win\virtual.h>
#include "initterm.h"
#include "oslibres.h"
#include "oslibwin.h"
#include "dc.h"

#define DBG_LOCALLOG    DBG_winicon
#include "dbglocal.h"


/**********************************************************************
 * ICONCACHE for cursors/icons loaded with LR_SHARED.
 *
 * FIXME: This should not be allocated on the system heap, but on a
 *        subsystem-global heap (i.e. one for all Win16 processes,
 *        and one for each Win32 process).
 */
typedef struct tagICONCACHE
{
    struct tagICONCACHE *next;

    HMODULE              hModule;
    HRSRC                hRsrc;
    HRSRC                hGroupRsrc;
    HANDLE               handle;
    INT                  count;
} ICONCACHE;

static ICONCACHE *IconAnchor = NULL;
static CRITICAL_SECTION IconCrst = CRITICAL_SECTION_INIT;
static WORD    ICON_HOTSPOT  = 0x4242;
static HCURSOR hActiveCursor = 0;
static HCURSOR hActiveCursorPM = 0;


static HGLOBAL CURSORICON_CreateFromResource( HINSTANCE hInstance, DWORD dwResGroupId, HGLOBAL hObj, LPBYTE bits,
                                              UINT cbSize, BOOL bIcon, DWORD dwVersion, INT width, INT height, UINT loadflags );
static HGLOBAL CURSORICON_Copy( HGLOBAL handle );
static CURSORICONDIRENTRY *CURSORICON_FindBestIcon( CURSORICONDIR *dir, int width,
                                                    int height, int colors );
static CURSORICONDIRENTRY *CURSORICON_FindBestCursor( CURSORICONDIR *dir,
                                                  int width, int height, int color);
BOOL CURSORICON_SimulateLoadingFromResourceW( LPWSTR filename, BOOL fCursor,
                                                CURSORICONDIR **res, LPBYTE **ptr);

static INT CURSORICON_DelSharedIcon( HANDLE handle );
static void CURSORICON_AddSharedIcon( HMODULE hModule, HRSRC hRsrc, HRSRC hGroupRsrc, HANDLE handle );
static HANDLE CURSORICON_FindSharedIcon( HMODULE hModule, HRSRC hRsrc );
static ICONCACHE* CURSORICON_FindCache(HANDLE handle);

/***********************************************************************
 *           CreateIcon    (USER32.75)
 */
HICON WIN32API CreateIcon(HINSTANCE hInstance, INT nWidth,
                          INT nHeight, BYTE bPlanes, BYTE bBitsPixel,
                          LPCVOID lpANDbits, LPCVOID lpXORbits )
{
    CURSORICONINFO info;

    dprintf(("USER32: CreateIcon (%d,%d), %d, %x, %x", nWidth, nHeight, bPlanes * bBitsPixel, lpXORbits, lpANDbits));

    info.ptHotSpot.x = ICON_HOTSPOT;
    info.ptHotSpot.y = ICON_HOTSPOT;
    info.nWidth = nWidth;
    info.nHeight = nHeight;
    info.nWidthBytes = 0;
    info.bPlanes = bPlanes;
    info.bBitsPerPixel = bBitsPixel;
    info.hInstance = hInstance;
    info.dwResGroupId = -1;
    info.hColorBmp = 0;
    return CreateCursorIconIndirect(0, &info, lpANDbits, lpXORbits);
}
/**********************************************************************
 *          CreateIconFromResource          (USER32.76)
 */
HICON WIN32API CreateIconFromResource(LPBYTE bits, UINT cbSize,
                                      BOOL bIcon, DWORD dwVersion)
{
    return CreateIconFromResourceEx( bits, cbSize, bIcon, dwVersion, 0,0,0);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIconFromResourceEx(LPBYTE bits, UINT cbSize,
                                        BOOL bIcon, DWORD dwVersion,
                                        INT width, INT height,
                                        UINT cFlag )
{
    dprintf(("USER32:  CreateIconFromResourceEx %X %d %d %X %d %d %X,", bits, cbSize, bIcon, dwVersion, width, height, cFlag));
    return CURSORICON_CreateFromResource(0, -1, 0, bits, cbSize, bIcon, dwVersion, width, height, cFlag );
}
/**********************************************************************
 *          CreateIconIndirect      (USER32.78)
 */
HICON WINAPI CreateIconIndirect(ICONINFO *iconinfo)
{
    BITMAP bmpXor,bmpAnd;
    HICON hObj;
    int sizeXor,sizeAnd;

    dprintf(("USER32: CreateIconIndirect %x", iconinfo));

    GetObjectA( iconinfo->hbmColor, sizeof(bmpXor), &bmpXor );
    GetObjectA( iconinfo->hbmMask, sizeof(bmpAnd), &bmpAnd );

    sizeXor = bmpXor.bmHeight * bmpXor.bmWidthBytes;
    sizeAnd = bmpAnd.bmHeight * bmpAnd.bmWidthBytes;

    hObj = GlobalAlloc( GMEM_MOVEABLE, sizeof(CURSORICONINFO) + sizeXor + sizeAnd );
    if (hObj)
    {
        CURSORICONINFO *info;

        info = (CURSORICONINFO *)GlobalLock( hObj );

        /* If we are creating an icon, the hotspot is unused */
        if (iconinfo->fIcon)
        {
            info->ptHotSpot.x   = ICON_HOTSPOT;
            info->ptHotSpot.y   = ICON_HOTSPOT;
        }
        else
        {
            info->ptHotSpot.x   = iconinfo->xHotspot;
            info->ptHotSpot.y   = iconinfo->yHotspot;
        }

        info->nWidth        = bmpXor.bmWidth;
        info->nHeight       = bmpXor.bmHeight;
        info->nWidthBytes   = bmpXor.bmWidthBytes;
        info->bPlanes       = bmpXor.bmPlanes;
        info->bBitsPerPixel = bmpXor.bmBitsPixel;
        info->hInstance     = -1;
        info->dwResGroupId  = -1;
        info->hColorBmp     = 0;

        /* Transfer the bitmap bits to the CURSORICONINFO structure */
        GetBitmapBits( iconinfo->hbmMask ,sizeAnd,(char*)(info + 1) );
        GetBitmapBits( iconinfo->hbmColor,sizeXor,(char*)(info + 1) +sizeAnd);
        GlobalUnlock(hObj);
    }
    else {
        dprintf(("ERROR: CreateIconIndirect GlobalAlloc failed!!"));
    }
    return hObj;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyIcon( HICON hIcon)
{
    dprintf(("USER32: DestroyIcon %x", hIcon));
    return CURSORICON_Destroy( hIcon, 0 );
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CopyIcon( HICON hIcon)
{
    dprintf(("USER32:  CopyIcon %x", hIcon));
    return CURSORICON_Copy( hIcon );
}
//******************************************************************************
//******************************************************************************
HICON WIN32API GetOS2Icon(HICON hIcon)
{
    CURSORICONINFO  *ciconinfo;

    ciconinfo = (CURSORICONINFO *)GlobalLock((HGLOBAL)hIcon);
    if (!ciconinfo)
        return 0;
    HICON hOS2Icon = ciconinfo->hColorBmp;
    GlobalUnlock(hIcon);
    return hOS2Icon;
}
/**********************************************************************
 *          GetIconInfo     (USER32.242)
 */
BOOL WINAPI GetIconInfo(HICON hIcon, ICONINFO *iconinfo)
{
    CURSORICONINFO  *ciconinfo;

    dprintf(("GetIconInfo %x %x", hIcon, iconinfo));

    ciconinfo = (CURSORICONINFO *)GlobalLock((HGLOBAL)hIcon);
    if (!ciconinfo)
        return FALSE;

    if((ciconinfo->ptHotSpot.x == ICON_HOTSPOT) &&
       (ciconinfo->ptHotSpot.y == ICON_HOTSPOT))
    {
        iconinfo->fIcon    = TRUE;
        iconinfo->xHotspot = ciconinfo->nWidth / 2;
        iconinfo->yHotspot = ciconinfo->nHeight / 2;
    }
    else
    {
        iconinfo->fIcon    = FALSE;
        iconinfo->xHotspot = ciconinfo->ptHotSpot.x;
        iconinfo->yHotspot = ciconinfo->ptHotSpot.y;
    }

    //Create new bitmaps for the color and mask data; application is responsible
    //for deleteing them (according to docs & verified in NT4)
    if(ciconinfo->bBitsPerPixel > 1)
    {
        BITMAPINFO* pInfo;
        int colorsize = 0;
        int coloroff;

        HDC hdc = CreateCompatibleDC(0);

        if(ciconinfo->bBitsPerPixel <= 8) {
            colorsize = (1<<ciconinfo->bBitsPerPixel)*sizeof(RGBQUAD);
        }
        else {
            colorsize = 3*sizeof(DWORD); //color masks
        }
        pInfo = (BITMAPINFO *)malloc(ciconinfo->nHeight * ciconinfo->nWidthBytes + colorsize + sizeof(BITMAPINFO));
        memset(pInfo, 0, sizeof(BITMAPINFO)+colorsize);

        pInfo->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
        pInfo->bmiHeader.biWidth    = ciconinfo->nWidth;
        pInfo->bmiHeader.biHeight   = ciconinfo->nHeight,
        pInfo->bmiHeader.biPlanes   = ciconinfo->bPlanes;
        pInfo->bmiHeader.biBitCount = ciconinfo->bBitsPerPixel;
        pInfo->bmiHeader.biSizeImage= ciconinfo->nHeight * ciconinfo->nWidthBytes;

        //offset in cursorinfo memory
        coloroff = ciconinfo->nHeight * BITMAP_GetWidthBytes (ciconinfo->nWidth, 1);

        char *src  = (char *)(ciconinfo + 1) + coloroff;
        if(ciconinfo->bBitsPerPixel <= 8) {
            src += colorsize;       //no color masks in cursorinfo data for bpp > 8
        }
        if(ciconinfo->bBitsPerPixel <= 8) {
                memcpy(&pInfo->bmiColors[0], (char *)(ciconinfo + 1) + coloroff, colorsize);
        }
        //else TODO: color masks (currently unused in CreateDIBitmap)

        iconinfo->hbmColor = CreateDIBitmap(hdc, &pInfo->bmiHeader, CBM_INIT, src, pInfo, DIB_RGB_COLORS);

        free(pInfo);
        DeleteDC(hdc);
    }
    else {
        iconinfo->hbmColor = CreateBitmap ( ciconinfo->nWidth, ciconinfo->nHeight,
                                            ciconinfo->bPlanes, ciconinfo->bBitsPerPixel,
                                            (char *)(ciconinfo + 1)
                                            + ciconinfo->nHeight *
                                            BITMAP_GetWidthBytes (ciconinfo->nWidth, 1) );
    }

    iconinfo->hbmMask = CreateBitmap ( ciconinfo->nWidth, ciconinfo->nHeight,
                                1, 1, (char *)(ciconinfo + 1));

    GlobalUnlock(hIcon);

    return TRUE;
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API CreateCursor(HINSTANCE hInst, int xHotSpot, int yHotSpot, int nWidth, int nHeight,
                              const VOID *lpANDbits, const VOID *lpXORbits)
{
    CURSORICONINFO info;

    dprintf(("CreateCursor %dx%d spot=%d,%d xor=%p and=%p\n",
             nWidth, nHeight, xHotSpot, yHotSpot, lpXORbits, lpANDbits));

    info.ptHotSpot.x   = xHotSpot;
    info.ptHotSpot.y   = yHotSpot;
    info.nWidth        = nWidth;
    info.nHeight       = nHeight;
    info.nWidthBytes   = 0;
    info.bPlanes       = 1;
    info.bBitsPerPixel = 1;
    info.hInstance     = hInst;
    info.dwResGroupId  = -1;
    info.hColorBmp     = 0;

    return CreateCursorIconIndirect( 0, &info, lpANDbits, lpXORbits );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyCursor( HCURSOR hCursor)
{
    dprintf(("USER32:  DestroyCursor %x", hCursor));
    return CURSORICON_Destroy( hCursor, CID_WIN32 );
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API GetCursor(void)
{
    dprintf2(("USER32: GetCursor"));
    if(hActiveCursorPM && hActiveCursorPM != OSLibWinQueryPointer()) {
       dprintf(("Another app changed mouse cursor"));
       hActiveCursorPM = hActiveCursor = 0; 
    }
    return hActiveCursor;
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API SetCursor( HCURSOR hCursor)
{
  HCURSOR hOldCursor;

    dprintf(("USER32: SetCursor %x (prev %x)", hCursor, hActiveCursor));
    GetCursor();
    if (hCursor == hActiveCursor) return hActiveCursor;  /* No change */

    hOldCursor = hActiveCursor;
    hActiveCursor = hCursor;

    CURSORICONINFO *iconinfo = (CURSORICONINFO *)GlobalLock((HGLOBAL)hCursor);
    if (!iconinfo) {
        dprintf(("ERROR: Invalid cursor!"));
        return 0;
    }
    if(!iconinfo->hColorBmp) {
        dprintf(("SetCursor: invalid os/2 pointer handle!!"));
    }

    if(OSLibWinSetPointer(iconinfo->hColorBmp) == FALSE) {
        dprintf(("OSLibWinSetPointer %x returned FALSE!!", iconinfo->hColorBmp));
    }
    hActiveCursorPM = iconinfo->hColorBmp;
    GlobalUnlock(hCursor);

    return hOldCursor;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCursorPos( PPOINT lpPoint)
{
    dprintf2(("USER32: GetCursorPos %x", lpPoint));

    if (!lpPoint) return FALSE;

    if (OSLibWinQueryPointerPos(lpPoint)) //POINT == POINTL
    {
        mapScreenPoint((OSLIBPOINT*)lpPoint);
        return TRUE;
    }
    else return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCursorPos( int X, int Y)
{
    dprintf(("USER32: SetCursorPos %d %d", X,Y));
    return OSLibWinSetPointerPos(X, mapScreenY(Y));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ClipCursor(const RECT * lpRect)
{
    if(lpRect) {
         dprintf(("USER32: ClipCursor (%d,%d)(%d,%d)", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom));
    }
    else dprintf(("USER32: ClipCursor NULL"));
    return OSLibWinClipCursor(lpRect);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClipCursor( LPRECT lpRect)
{
    dprintf(("USER32: GetClipCursor %x", lpRect));
    return OSLibWinGetClipCursor(lpRect);
}
//******************************************************************************
//******************************************************************************
/*****************************************************************************
 * Name      : BOOL WIN32API SetSystemCursor
 * Purpose   : The SetSystemCursor function replaces the contents of the system
 *             cursor specified by dwCursorId with the contents of the cursor
 *             specified by hCursor, and then destroys hCursor. This function
 *             lets an application customize the system cursors.
 * Parameters: HCURSOR  hCursor    set specified system cursor to this cursor's
 *                                 contents, then destroy this
 *             DWORD    dwCursorID system cursor specified by its identifier
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API SetSystemCursor(HCURSOR hCursor, DWORD dwCursorId)
{
  dprintf(("USER32:SetSystemCursor (%08xh,%08x) not supported.\n",
         hCursor,
         dwCursorId));

  return DestroyCursor(hCursor);
}
//******************************************************************************
//******************************************************************************
INT OPEN32API __ShowCursor(BOOL bShow);

inline INT _ShowCursor (BOOL bShow)
{
 INT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = __ShowCursor(bShow);
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
static int cursorshowcnt = 0;
//******************************************************************************
void RestoreCursor()
{
    dprintf2(("USER32: RestoreCursor %d", cursorshowcnt));
    while(cursorshowcnt != 0)
    {
        if(cursorshowcnt > 0 )
        {
            ShowCursor(FALSE);
        }
        else
        {
            ShowCursor(TRUE);
        }
    }
}
//******************************************************************************
//******************************************************************************
int WIN32API ShowCursor(BOOL bShow)
{
    dprintf2(("USER32: ShowCursor %d", bShow));
    cursorshowcnt = cursorshowcnt + ((bShow) ? 1 : -1);
    return _ShowCursor(bShow);
}
/***********************************************************************
 *           CreateCursorIconIndirect
 */
HGLOBAL WIN32API CreateCursorIconIndirect( HINSTANCE hInstance,
                                           CURSORICONINFO *info,
                                           LPCVOID lpANDbits,
                                           LPCVOID lpXORbits )
{
    HGLOBAL handle;
    char *ptr;
    int sizeAnd, sizeXor;

    if (!lpXORbits || !lpANDbits || info->bPlanes != 1) return 0;
    info->nWidthBytes = BITMAP_GetWidthBytes(info->nWidth,info->bBitsPerPixel);
    sizeXor = info->nHeight * info->nWidthBytes;
    sizeAnd = info->nHeight * BITMAP_GetWidthBytes( info->nWidth, 1 );
    if (!(handle = GlobalAlloc( GMEM_MOVEABLE,
                                sizeof(CURSORICONINFO) + sizeXor + sizeAnd)))
        return 0;
    ptr = (char *)GlobalLock( handle );
    memcpy( ptr, info, sizeof(*info) );
    memcpy( ptr + sizeof(CURSORICONINFO), lpANDbits, sizeAnd );
    memcpy( ptr + sizeof(CURSORICONINFO) + sizeAnd, lpXORbits, sizeXor );
    GlobalUnlock( handle );
    return handle;
}
/**********************************************************************
 *          CURSORICON_Load
 *
 * Load a cursor or icon from resource or file.
 */
HGLOBAL CURSORICON_Load( HINSTANCE hInstance, LPCWSTR name,
                         INT width, INT height, INT colors,
                         BOOL fCursor, UINT loadflags )
{
    HANDLE handle = 0, h = 0;
    HANDLE hRsrc;
    CURSORICONDIR *dir;
    CURSORICONDIRENTRY *dirEntry;
    LPBYTE bits;

#ifdef __WIN32OS2__
    //TODO: Can system cursors be loaded by name??? (#xxx)
    if (fCursor && hInstance == NULL && !HIWORD(name))
    {
        HCURSOR hCursor = OSLibWinQuerySysPointer((ULONG)name, width, height);
        if(hCursor)
        {
            /* If shared icon, check whether it was already loaded */
            if ((loadflags & LR_SHARED)
                && (h = CURSORICON_FindSharedIcon( -1, hCursor ) ) != 0 )
            {
                dprintf(("Found icon/cursor in cache; returned old handle %x", h));
                return h;
            }

            HANDLE hObj = GlobalAlloc( GMEM_MOVEABLE,  sizeof(CURSORICONINFO));
            if (!hObj)
            {
                DebugInt3();
                return 0;
            }
            CURSORICONINFO *info;

            info = (CURSORICONINFO *)GlobalLock( hObj );
            info->ptHotSpot.x   = 0;
            info->ptHotSpot.y   = 0;
            info->nWidth        = width;
            info->nHeight       = height;
            info->nWidthBytes   = width*height/8;
            info->bPlanes       = 1;
            info->bBitsPerPixel = 1;
            info->hColorBmp     = hCursor;
            info->hInstance     = -1;
            info->dwResGroupId  = -1;

            if (loadflags & LR_SHARED )
                CURSORICON_AddSharedIcon( -1, hCursor, -1, hObj );

            GlobalUnlock( hObj );

            return hObj;
        }
    }
#endif
    if ( loadflags & LR_LOADFROMFILE )    /* Load from file */
    {
        LPBYTE *ptr;

        if (!CURSORICON_SimulateLoadingFromResourceW((LPWSTR)name, fCursor, &dir, &ptr))
            return 0;
        if (fCursor)
            dirEntry = (CURSORICONDIRENTRY *)CURSORICON_FindBestCursor(dir, width, height, 1);
        else
            dirEntry = (CURSORICONDIRENTRY *)CURSORICON_FindBestIcon(dir, width, height, colors);
        bits = ptr[dirEntry->wResId-1];
        h = CURSORICON_CreateFromResource( 0, -1, 0, bits, dirEntry->dwBytesInRes,
                                           !fCursor, 0x00030000, width, height, loadflags);

        HeapFree( GetProcessHeap(), 0, dir );
        HeapFree( GetProcessHeap(), 0, ptr );
    }
    else  /* Load from resource */
    {
        HANDLE hGroupRsrc;
        WORD wResId;
        DWORD dwBytesInRes;
        BOOL  bIsGroup = TRUE;

        /* Get directory resource ID */
        if (!hInstance)
        {
            hRsrc = FindResourceW(hInstanceUser32, name, fCursor ? RT_CURSORW : RT_ICONW);
            if(!hRsrc) {
                hRsrc = FindResourceW(hInstanceUser32, name, fCursor ? RT_GROUP_CURSORW : RT_GROUP_ICONW);
            }
            else bIsGroup = FALSE;

            if(!hRsrc)  return 0;

            hInstance = hInstanceUser32;
        }
        else {
            hRsrc = FindResourceW(hInstance, name, fCursor ? RT_GROUP_CURSORW : RT_GROUP_ICONW);
            if(!hRsrc)  return 0;
        }
        hGroupRsrc = hRsrc;

        if(bIsGroup) {
            /* Find the best entry in the directory */

            if (!(handle = LoadResource( hInstance, hRsrc ))) return 0;
            if (!(dir = (CURSORICONDIR*)LockResource( handle ))) return 0;

            if (fCursor)
                dirEntry = (CURSORICONDIRENTRY *)CURSORICON_FindBestCursor( dir,
                                                                  width, height, 1);
            else
                dirEntry = (CURSORICONDIRENTRY *)CURSORICON_FindBestIcon( dir,
                                                           width, height, colors );
            if (!dirEntry) return 0;
            wResId = dirEntry->wResId;
            dwBytesInRes = dirEntry->dwBytesInRes;
            FreeResource( handle );

            /* Load the resource */
            if (!(hRsrc = FindResourceW(hInstance,MAKEINTRESOURCEW(wResId),
                                      fCursor ? RT_CURSORW : RT_ICONW ))) return 0;
        }

        /* If shared icon, check whether it was already loaded */
        if ((loadflags & LR_SHARED)
            && (h = CURSORICON_FindSharedIcon( hInstance, hRsrc ) ) != 0 )
        {
            dprintf(("Found icon/cursor in cache; returned old handle %x", h));
            return h;
        }

        if (!(handle = LoadResource( hInstance, hRsrc ))) return 0;
        bits = (LPBYTE)LockResource( handle );
        h = CURSORICON_CreateFromResource( hInstance, (DWORD)name, 0, bits, dwBytesInRes,
                                           !fCursor, 0x00030000, width, height, loadflags);
        FreeResource( handle );

        /* If shared icon, add to icon cache */

        if ( h && (loadflags & LR_SHARED) )
            CURSORICON_AddSharedIcon( hInstance, hRsrc, hGroupRsrc, h );

    }

    return h;
}

/*********************************************************************
 * The main purpose of this function is to create fake resource directory
 * and fake resource entries. There are several reasons for this:
 *  -   CURSORICONDIR and CURSORICONFILEDIR differ in sizes and their
 *              fields
 *  There are some "bad" cursor files which do not have
 *      bColorCount initialized but instead one must read this info
 *      directly from corresponding DIB sections
 * Note: wResId is index to array of pointer returned in ptrs (origin is 1)
 */
BOOL CURSORICON_SimulateLoadingFromResourceW( LPWSTR filename, BOOL fCursor,
                                              CURSORICONDIR **res, LPBYTE **ptr)
{
    LPBYTE    _free;
    CURSORICONFILEDIR *bits;
    int        entries, size, i;
    HANDLE     hMapping = 0;

    *res = NULL;
    *ptr = NULL;

    hMapping = VIRTUAL_MapFileW( filename, (LPVOID *)&bits, TRUE);
    if(hMapping == INVALID_HANDLE_VALUE)
            return FALSE;

    /* FIXME: test for inimated icons
     * hack to load the first icon from the *.ani file
     */
    if ( *(LPDWORD)bits==0x46464952 ) /* "RIFF" */
    {
        LPBYTE pos = (LPBYTE) bits;
        dprintf(("Animated icons not correctly implemented! %p \n", bits));

        for (;;)
        {
            if (*(LPDWORD)pos==0x6e6f6369)      /* "icon" */
            {
                dprintf(("icon entry found! %p\n", bits));
                pos+=4;
                if ( !*(LPWORD) pos==0x2fe)       /* iconsize */
                {
                    goto fail;
                }
                bits=(CURSORICONFILEDIR*)(pos+4);
                dprintf(("icon size ok. offset=%p \n", bits));
                break;
            }
            pos+=2;
            if (pos>=(LPBYTE)bits+766) goto fail;
        }
    }
    if (!(entries = bits->idCount)) goto fail;
    size = sizeof(CURSORICONDIR) + sizeof(CURSORICONDIRENTRY) * (entries - 1);
    _free = (LPBYTE) size;

    for (i=0; i < entries; i++)
      size += bits->idEntries[i].dwDIBSize + (fCursor ? sizeof(POINT16): 0);

    if (!(*ptr = (LPBYTE *)HeapAlloc( GetProcessHeap(), 0,
                            entries * sizeof (CURSORICONDIRENTRY*)))) goto fail;
    if (!(*res = (CURSORICONDIR *)HeapAlloc( GetProcessHeap(), 0, size))) goto fail;

    _free = (LPBYTE)(*res) + (int)_free;
    memcpy((*res), bits, 6);
    for (i=0; i<entries; i++)
    {
      ((LPBYTE*)(*ptr))[i] = _free;
      if (fCursor) {
        (*res)->idEntries[i].ResInfo.cursor.wWidth=bits->idEntries[i].bWidth;
        (*res)->idEntries[i].ResInfo.cursor.wHeight=bits->idEntries[i].bHeight;
        ((LPPOINT16)_free)->x=bits->idEntries[i].xHotspot;
        ((LPPOINT16)_free)->y=bits->idEntries[i].yHotspot;
        _free+=sizeof(POINT16);
      }
      else {
        (*res)->idEntries[i].ResInfo.icon.bWidth=bits->idEntries[i].bWidth;
        (*res)->idEntries[i].ResInfo.icon.bHeight=bits->idEntries[i].bHeight;
        (*res)->idEntries[i].ResInfo.icon.bColorCount = bits->idEntries[i].bColorCount;
      }
      (*res)->idEntries[i].wPlanes=1;
      (*res)->idEntries[i].wBitCount = ((LPBITMAPINFOHEADER)((LPBYTE)bits +
                                                   bits->idEntries[i].dwDIBOffset))->biBitCount;
      (*res)->idEntries[i].dwBytesInRes = bits->idEntries[i].dwDIBSize;
      (*res)->idEntries[i].wResId=i+1;

      memcpy(_free,(LPBYTE)bits +bits->idEntries[i].dwDIBOffset,
             (*res)->idEntries[i].dwBytesInRes);
      _free += (*res)->idEntries[i].dwBytesInRes;
    }
    UnmapViewOfFile( bits );
    CloseHandle(hMapping);
    return TRUE;

fail:
    if (*res) HeapFree( GetProcessHeap(), 0, *res );
    if (*ptr) HeapFree( GetProcessHeap(), 0, *ptr );

    UnmapViewOfFile( bits );
    CloseHandle(hMapping);
    return FALSE;
}

/**********************************************************************
 *      CURSORICON_CreateFromResource
 *
 * Create a cursor or icon from in-memory resource template.
 *
 * FIXME: Convert to mono when cFlag is LR_MONOCHROME. Do something
 *        with cbSize parameter as well.
 */
static HGLOBAL CURSORICON_CreateFromResource( HINSTANCE hInstance, DWORD dwResGroupId, HGLOBAL hObj, LPBYTE bits,
                        UINT cbSize, BOOL bIcon, DWORD dwVersion,
                        INT width, INT height, UINT loadflags )
{
    int sizeAnd, sizeXor;
    HBITMAP hAndBits = 0, hXorBits = 0; /* error condition for later */
    BITMAP bmpXor, bmpAnd;
    POINT16 hotspot;
    BITMAPINFO *bmi;
    HDC hdc = 0;
    BOOL DoStretch;
    INT size, colortablesize, bwsize, colorsize;

    hotspot.x = ICON_HOTSPOT;
    hotspot.y = ICON_HOTSPOT;

//testestest
    dprintf(("CURSORICON_CreateFromResource %x %x %x %x %d", hInstance, dwResGroupId, hObj, bits, cbSize));

    if (dwVersion == 0x00020000)
    {
        dprintf(("CURSORICON_CreateFromResource 2.xx resources are not supported"));
        return 0;
    }

    if (bIcon) {
        bmi = (BITMAPINFO *)bits;
    }
    else /* get the hotspot */
    {
        POINT16 *pt = (POINT16 *)bits;
        hotspot = *pt;
        bmi = (BITMAPINFO *)(pt + 1);
    }
    size = DIB_BitmapInfoSize(bmi, DIB_RGB_COLORS);

    if (!width) width = bmi->bmiHeader.biWidth;
    if (!height) height = bmi->bmiHeader.biHeight/2;

    DoStretch = (bmi->bmiHeader.biHeight/2 != height) || (bmi->bmiHeader.biWidth != width);

    colorsize = DIB_GetDIBImageBytes(bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight/2, bmi->bmiHeader.biBitCount);
    bwsize    = (bmi->bmiHeader.biWidth * bmi->bmiHeader.biHeight/2)/8;

    /* Check bitmap header */
    if((bmi->bmiHeader.biSize != sizeof(BITMAPCOREHEADER)) &&
       (bmi->bmiHeader.biSize != sizeof(BITMAPINFOHEADER)  ||
        bmi->bmiHeader.biCompression != BI_RGB) )
    {
        return 0;
    }

#ifdef __WIN32OS2__
    if( (hdc = CreateCompatibleDC( 0 )) )
#else
    if( (hdc = GetDC( 0 )) )
#endif
    {
        BITMAPINFO* pInfo;

        /* Make sure we have room for the monochrome bitmap later on.
         * Note that BITMAPINFOINFO and BITMAPCOREHEADER are the same
         * up to and including the biBitCount. In-memory icon resource
         * format is as follows:
         *
         *   BITMAPINFOHEADER   icHeader  // DIB header
         *   RGBQUAD         icColors[]   // Color table
         *   BYTE            icXOR[]      // DIB bits for XOR mask
         *   BYTE            icAND[]      // DIB bits for AND mask
         */

        if ((pInfo = (BITMAPINFO *)HeapAlloc( GetProcessHeap(), 0,
                                              max(size, sizeof(BITMAPINFOHEADER) + 2*sizeof(RGBQUAD)))))
        {
            memcpy( pInfo, bmi, size );
            pInfo->bmiHeader.biHeight /= 2;

            /* Create the XOR bitmap */
            if (DoStretch)
            {
                if(bIcon)
                {
                    hXorBits = CreateCompatibleBitmap(hdc, width, height);
                }
                else
                {
                    hXorBits = CreateBitmap(width, height, 1, 1, NULL);
                }
                if(hXorBits)
                {
                    HBITMAP hOld;
                    HDC hMem = CreateCompatibleDC(hdc);
                    BOOL res;

                    if (hMem) {
                        hOld = SelectObject(hMem, hXorBits);
                        res = StretchDIBits(hMem, 0, 0, width, height, 0, 0,
                                            bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight/2,
                                            (char*)bmi + size, pInfo, DIB_RGB_COLORS, SRCCOPY);
                        SelectObject(hMem, hOld);
                        DeleteDC(hMem);
                    }
                    else res = FALSE;
                    if (!res) {
                        DeleteObject(hXorBits);
                        hXorBits = 0;
                    }
                }
            }
            else
            {
                hXorBits = CreateDIBitmap(hdc, &pInfo->bmiHeader,
                                          CBM_INIT, (char*)bmi + size, pInfo, DIB_RGB_COLORS );
            }
            if( hXorBits )
            {
                char* xbits = (char *)bmi + size + DIB_GetDIBImageBytes(bmi->bmiHeader.biWidth,
                                                                        bmi->bmiHeader.biHeight,
                                                                        bmi->bmiHeader.biBitCount) / 2;

                pInfo->bmiHeader.biBitCount = 1;
                if (pInfo->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
                {
                    RGBQUAD *rgb = pInfo->bmiColors;

                    pInfo->bmiHeader.biClrUsed = pInfo->bmiHeader.biClrImportant = 2;
                    rgb[0].rgbBlue = rgb[0].rgbGreen = rgb[0].rgbRed = 0x00;
                    rgb[1].rgbBlue = rgb[1].rgbGreen = rgb[1].rgbRed = 0xff;
                    rgb[0].rgbReserved = rgb[1].rgbReserved = 0;
                }
                else
                {
                    RGBTRIPLE *rgb = (RGBTRIPLE *)(((BITMAPCOREHEADER *)pInfo) + 1);

                    rgb[0].rgbtBlue = rgb[0].rgbtGreen = rgb[0].rgbtRed = 0x00;
                    rgb[1].rgbtBlue = rgb[1].rgbtGreen = rgb[1].rgbtRed = 0xff;
                }

                /* Create the AND bitmap */
                if (DoStretch)
                {
                    if ((hAndBits = CreateBitmap(width, height, 1, 1, NULL)))
                    {
                        HBITMAP hOld;
                        HDC hMem = CreateCompatibleDC(hdc);
                        BOOL res;

                        if (hMem) {
                            hOld = SelectObject(hMem, hAndBits);
//SvL: This also doesn't work as StretchDIBits doesn't handle 1bpp bitmaps correctly
//--------->>> hack alert!
#if 1
                            HBITMAP hBmp, hOld1;
                            HDC hMem1;

                            hMem1 = CreateCompatibleDC(hdc);

                            int linewidth = BITMAP_GetWidthBytes(pInfo->bmiHeader.biWidth, 1);

                            char *newpix = (char *)malloc(linewidth*pInfo->bmiHeader.biHeight);

                            newpix += ((pInfo->bmiHeader.biHeight-1)*linewidth);

                            if(cbSize - size - colorsize - bwsize == bwsize)
                            {//this means an AND and XOR mask is present (interleaved; and/xor)
                                for(int i=0;i<pInfo->bmiHeader.biHeight;i++) {
                                    memcpy(newpix, xbits, linewidth);
                                    newpix -= linewidth;
                                    xbits  += linewidth*2;
                                }
                            }
                            else {
                                for(int i=0;i<pInfo->bmiHeader.biHeight;i++) {
                                    memcpy(newpix, xbits, linewidth);
                                    newpix -= linewidth;
                                    xbits  += linewidth;
                                }
                            }
                            newpix += linewidth;
                            hBmp = CreateBitmap(pInfo->bmiHeader.biWidth, pInfo->bmiHeader.biHeight, 1, 1, newpix);
                            free(newpix);

                            hOld1 = SelectObject(hMem1, hBmp);

                            res = StretchBlt(hMem, 0, 0, width, height, hMem1,  0, 0, pInfo->bmiHeader.biWidth, pInfo->bmiHeader.biHeight, SRCCOPY);

                            SelectObject(hMem1, hOld1);
                            DeleteObject(hBmp);
                            DeleteDC(hMem1);


#else
                            res = StretchDIBits(hMem, 0, 0, width, height, 0, 0,
                                                pInfo->bmiHeader.biWidth, pInfo->bmiHeader.biHeight,
                                                xbits, pInfo, DIB_RGB_COLORS, SRCCOPY);
#endif
                            SelectObject(hMem, hOld);
                            DeleteDC(hMem);
                        }
                        else res = FALSE;
                        if (!res) {
                            DeleteObject(hAndBits);
                            hAndBits = 0;
                        }
                    }
                }
                else {
//SvL: Must use CreateBitmap here as CreateDIBitmap converts data to 8bpp (GetObjectA info -> 8 bpp)
#if 1
                    int linewidth = BITMAP_GetWidthBytes(width, 1);

                    char *newpix = (char *)malloc(linewidth*height);

                    newpix += ((height-1)*linewidth);

                    if(cbSize - size - colorsize - bwsize == bwsize)
                    {//this means an AND and XOR mask is present (interleaved; and/xor)
                        for(int i=0;i<height;i++) {
                            memcpy(newpix, xbits, linewidth);
                            newpix -= linewidth;
                            xbits  += linewidth*2;
                        }
                    }
                    else {
                        for(int i=0;i<height;i++) {
                            memcpy(newpix, xbits, linewidth);
                            newpix -= linewidth;
                            xbits  += linewidth;
                        }
                    }
                    newpix += linewidth;
                    hAndBits = CreateBitmap(width, height, 1, 1, newpix);

                    free(newpix);

#else
                    hAndBits = CreateDIBitmap(hdc, &pInfo->bmiHeader,
                                              CBM_INIT, xbits, pInfo, DIB_RGB_COLORS );
#endif
                }
                if( !hAndBits )
                    DeleteObject( hXorBits );
             }
             HeapFree( GetProcessHeap(), 0, pInfo );
        }
#ifdef __WIN32OS2__
        DeleteDC(hdc );
#else
        ReleaseDC( 0, hdc );
#endif
    }

    if( !hXorBits || !hAndBits )
    {
        dprintf(("\tunable to create an icon bitmap."));
        return 0;
    }

    /* Now create the CURSORICONINFO structure */
    GetObjectA( hXorBits, sizeof(bmpXor), &bmpXor );
    GetObjectA( hAndBits, sizeof(bmpAnd), &bmpAnd );
    colortablesize = 0;

    if(bmpXor.bmBitsPixel <= 8) {
         colortablesize = sizeof(RGBQUAD)*(1<<bmpXor.bmBitsPixel);
         sizeXor = bmpXor.bmHeight * bmpXor.bmWidthBytes + colortablesize;
    }
    else sizeXor = bmpXor.bmHeight * bmpXor.bmWidthBytes;

    sizeAnd = bmpAnd.bmHeight * bmpAnd.bmWidthBytes;

    if (hObj) hObj = GlobalReAlloc( hObj,
             sizeof(CURSORICONINFO) + sizeXor + sizeAnd, GMEM_MOVEABLE );
    if (!hObj) hObj = GlobalAlloc( GMEM_MOVEABLE,
             sizeof(CURSORICONINFO) + sizeXor + sizeAnd );
    if (hObj)
    {
        CURSORICONINFO *info;

        info = (CURSORICONINFO *)GlobalLock( hObj );
        info->ptHotSpot.x   = hotspot.x;
        info->ptHotSpot.y   = hotspot.y;
        info->nWidth        = bmpXor.bmWidth;
        info->nHeight       = bmpXor.bmHeight;
        info->nWidthBytes   = bmpXor.bmWidthBytes;
        info->bPlanes       = bmpXor.bmPlanes;
        info->bBitsPerPixel = bmpXor.bmBitsPixel;
        info->hInstance     = hInstance;
        info->dwResGroupId  = dwResGroupId;
        info->hColorBmp     = 0;

        /* Transfer the bitmap bits to the CURSORICONINFO structure */
        GetBitmapBits( hAndBits, sizeAnd, (char *)(info + 1));

        if(bmpXor.bmBitsPixel > 1)
        {
            BITMAPINFO* pInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO)+colortablesize+3*sizeof(DWORD)); //+ extra space for > 8bpp images
            HBITMAP oldbmp;

            hdc = CreateCompatibleDC(0);

            memset(pInfo, 0, sizeof(BITMAPINFO)+colortablesize+3*sizeof(DWORD));
            pInfo->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
            pInfo->bmiHeader.biPlanes   = info->bPlanes;
            pInfo->bmiHeader.biBitCount = info->bBitsPerPixel;

            GetDIBits(hdc, hXorBits, 0, bmpXor.bmHeight, (char *)(info + 1) + sizeAnd + colortablesize, pInfo, DIB_RGB_COLORS);
            if(colortablesize) {
                memcpy((char *)(info + 1) + sizeAnd, (char *)&pInfo->bmiHeader + pInfo->bmiHeader.biSize, colortablesize);
            }

            DeleteDC(hdc);
            free(pInfo);
        }
        else {
            GetBitmapBits( hXorBits, sizeXor, (char *)(info + 1) + sizeAnd);
        }
        info->hColorBmp = OSLibWinCreatePointer(info, (char *)(info + 1), (LPBITMAP_W)&bmpAnd, (char *)(info + 1) + sizeAnd, (LPBITMAP_W)&bmpXor, bIcon == FALSE);
        dprintf(("Create cursor/icon %x with OS/2 pointer handle %x", hObj, info->hColorBmp));
        GlobalUnlock( hObj );
    }

    DeleteObject( hAndBits );
    DeleteObject( hXorBits );
    return hObj;
}

/**********************************************************************
 *      CURSORICON_Destroy   (USER.610)
 *
 * This routine is actually exported from Win95 USER under the name
 * DestroyIcon32 ...  The behaviour implemented here should mimic
 * the Win95 one exactly, especially the return values, which
 * depend on the setting of various flags.
 */
WORD WIN32API CURSORICON_Destroy( HGLOBAL handle, UINT flags )
{
    WORD retv;

    /* Check whether destroying active cursor */

    if ( hActiveCursor == handle )
    {
        dprintf(("WARNING: Destroying active cursor!" ));
        SetCursor( 0 );
    }

    /* Try shared cursor/icon first */
    if ( !(flags & CID_NONSHARED) )
    {
        INT count = CURSORICON_DelSharedIcon( handle );

        if ( count != -1 )
            return (flags & CID_WIN32)? TRUE : (count == 0);

        /* FIXME: OEM cursors/icons should be recognized */
    }
    /* Now assume non-shared cursor/icon */

#ifdef __WIN32OS2__
    CURSORICONINFO *iconinfo = (CURSORICONINFO *)GlobalLock((HGLOBAL)handle);
    if (!iconinfo) {
        dprintf(("ERROR: Invalid cursor!"));
        return 0;
    }

    if(iconinfo->hColorBmp) {
        OSLibWinDestroyPointer(iconinfo->hColorBmp);
    }
    GlobalUnlock(handle);
#endif

    retv = GlobalFree( handle );
    return (flags & CID_RESOURCE)? retv : TRUE;
}

/***********************************************************************
 *           CURSORICON_Copy
 *
 * Make a copy of a cursor or icon.
 */
static HGLOBAL CURSORICON_Copy(HGLOBAL handle)
{
    char *ptrOld, *ptrNew;
    int size;
    HGLOBAL hNew;

    if (!(ptrOld = (char *)GlobalLock( handle ))) return 0;

    size = GlobalSize( handle );
    hNew = GlobalAlloc( GMEM_MOVEABLE, size );
    ptrNew = (char *)GlobalLock( hNew );
    memcpy( ptrNew, ptrOld, size );
    GlobalUnlock( handle );
    GlobalUnlock( hNew );
    return hNew;
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
 */
HGLOBAL CURSORICON_ExtCopy(HGLOBAL Handle, UINT nType,
                           INT iDesiredCX, INT iDesiredCY,
                           UINT nFlags)
{
    HGLOBAL hNew=0;

    if(Handle == 0)
    {
        return 0;
    }
    /* Best Fit or Monochrome */
    if( (nFlags & LR_COPYFROMRESOURCE
        && (iDesiredCX > 0 || iDesiredCY > 0))
        || nFlags & LR_MONOCHROME)
    {
        ICONCACHE* pIconCache = CURSORICON_FindCache(Handle);

        /* Not Found in Cache, then do a straight copy
        */
        if(pIconCache == NULL)
        {
#ifdef __WIN32OS2__
            hNew = CURSORICON_Copy(Handle);
#else
            hNew = CURSORICON_Copy(0, Handle);
#endif
            if(nFlags & LR_COPYFROMRESOURCE)
            {
                dprintf(("WARNING: LR_COPYFROMRESOURCE: Failed to load from cache\n"));
            }
        }
        else
        {
            int iTargetCY = iDesiredCY, iTargetCX = iDesiredCX;
            LPBYTE pBits;
            HANDLE hMem;
            HRSRC hRsrc;
            DWORD dwBytesInRes;
            WORD wResId;
            DWORD dwResGroupId;
            HINSTANCE hInstance;
            CURSORICONINFO *iconinfo;
            CURSORICONDIR *pDir;
            CURSORICONDIRENTRY *pDirEntry;
            BOOL bIsIcon = (nType == IMAGE_ICON);

            iconinfo = (CURSORICONINFO *)GlobalLock( Handle );
            if(iconinfo == NULL) {
                dprintf(("ERROR: CURSORICON_ExtCopy invalid icon!"));
            }
            hInstance    = iconinfo->hInstance;
            dwResGroupId = iconinfo->dwResGroupId;
            GlobalUnlock( Handle );
            if(dwResGroupId == -1) {
                //todo: if scaling is necessary..
                dprintf(("WARNING: no resource associated with icon/cursor -> copy without scaling!"));
                hNew = CURSORICON_Copy(Handle);
                return hNew;
            }

            /* Completing iDesiredCX CY for Monochrome Bitmaps if needed
            */
            if(((nFlags & LR_MONOCHROME) && !(nFlags & LR_COPYFROMRESOURCE))
                || (iDesiredCX == 0 && iDesiredCY == 0))
            {
                iDesiredCY = GetSystemMetrics(bIsIcon ? SM_CYICON : SM_CYCURSOR);
                iDesiredCX = GetSystemMetrics(bIsIcon ? SM_CXICON : SM_CXCURSOR);
            }

            /* Retreive the CURSORICONDIRENTRY
            */
            hRsrc = FindResourceW(hInstance, (LPWSTR)dwResGroupId, bIsIcon ? RT_GROUP_ICONW : RT_GROUP_CURSORW);
            if(!hRsrc)  {
                goto notfound;
            }

            if (!(hMem = LoadResource( hInstance, hRsrc)))
            {
                goto notfound;
            }
            if (!(pDir = (CURSORICONDIR*)LockResource( hMem )))
            {
                goto notfound;
            }

            /* Find Best Fit
            */
            if(bIsIcon)
            {
                pDirEntry = (CURSORICONDIRENTRY *)CURSORICON_FindBestIcon(
                                pDir, iDesiredCX, iDesiredCY, 256);
            }
            else
            {
                pDirEntry = (CURSORICONDIRENTRY *)CURSORICON_FindBestCursor(
                                pDir, iDesiredCX, iDesiredCY, 1);
            }

            wResId = pDirEntry->wResId;
            dwBytesInRes = pDirEntry->dwBytesInRes;
            FreeResource(hMem);

            /* Get the Best Fit
            */
            if (!(hRsrc = FindResourceW(hInstance ,
                MAKEINTRESOURCEW(wResId), bIsIcon ? RT_ICONW : RT_CURSORW)))
            {
                goto notfound;
            }
            if (!(hMem = LoadResource( hInstance, hRsrc )))
            {
                goto notfound;
            }

            pBits = (LPBYTE)LockResource( hMem );

            if(nFlags & LR_DEFAULTSIZE)
            {
                iTargetCY = GetSystemMetrics(SM_CYICON);
                iTargetCX = GetSystemMetrics(SM_CXICON);
            }

            /* Create a New Icon with the proper dimension
            */
            hNew = CURSORICON_CreateFromResource( hInstance, dwResGroupId, 0, pBits, dwBytesInRes,
                                                  bIsIcon, 0x00030000, iTargetCX, iTargetCY, nFlags);
            FreeResource(hMem);
        }
    }
    else
    {
            hNew = CURSORICON_Copy(Handle);
    }
    return hNew;

notfound:
    dprintf(("WARNING: unable to find resource associated with icon/cursor -> copy without scaling!"));
    hNew = CURSORICON_Copy(Handle);
    return hNew;
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
#ifdef __WIN32OS2__
            iTempColorDiff = abs(colors - (1 << entry->wBitCount));
#else
            iTempColorDiff = abs(colors - entry->ResInfo.icon.bColorCount);
#endif
            if(iColorDiff > iTempColorDiff)
            {
                bestEntry = entry;
                iColorDiff = iTempColorDiff;
            }
        }
    }
////testestest
    dprintf(("CURSORICON_FindBestIcon (%d,%d) %d -> %d", width, height, colors, (bestEntry) ? bestEntry->wResId : 0));
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
 *          LookupIconIdFromDirectoryEx       (USER.364)
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
//******************************************************************************
//******************************************************************************

//ICON cache implementation (Wine code)

/**********************************************************************
 *      CURSORICON_FindSharedIcon
 */
static HANDLE CURSORICON_FindSharedIcon( HMODULE hModule, HRSRC hRsrc )
{
    HANDLE handle = 0;
    ICONCACHE *ptr;

    EnterCriticalSection( &IconCrst );

    for ( ptr = IconAnchor; ptr; ptr = ptr->next )
        if ( ptr->hModule == hModule && ptr->hRsrc == hRsrc )
        {
            ptr->count++;
            handle = ptr->handle;
            break;
        }

    LeaveCriticalSection( &IconCrst );

    return handle;
}

/*************************************************************************
 * CURSORICON_FindCache
 *
 * Given a handle, find the corresponding cache element
 *
 * PARAMS
 *      Handle     [I] handle to an Image
 *
 * RETURNS
 *     Success: The cache entry
 *     Failure: NULL
 *
 */
static ICONCACHE* CURSORICON_FindCache(HANDLE handle)
{
    ICONCACHE *ptr;
    ICONCACHE *pRet=NULL;
    BOOL IsFound = FALSE;
    int count;

    EnterCriticalSection( &IconCrst );

    for (count = 0, ptr = IconAnchor; ptr != NULL && !IsFound; ptr = ptr->next, count++ )
    {
        if ( handle == ptr->handle )
        {
            IsFound = TRUE;
            pRet = ptr;
        }
    }

    LeaveCriticalSection( &IconCrst );

    return pRet;
}

/**********************************************************************
 *      CURSORICON_AddSharedIcon
 */
static void CURSORICON_AddSharedIcon( HMODULE hModule, HRSRC hRsrc, HRSRC hGroupRsrc, HANDLE handle )
{
    ICONCACHE *ptr = (ICONCACHE *)HeapAlloc( GetProcessHeap(), 0, sizeof(ICONCACHE) );
    if ( !ptr ) return;

    ptr->hModule = hModule;
    ptr->hRsrc   = hRsrc;
    ptr->handle  = handle;
    ptr->hGroupRsrc = hGroupRsrc;
    ptr->count   = 1;

    EnterCriticalSection( &IconCrst );
    ptr->next    = IconAnchor;
    IconAnchor   = ptr;
    LeaveCriticalSection( &IconCrst );
}

/**********************************************************************
 *      CURSORICON_DelSharedIcon
 */
static INT CURSORICON_DelSharedIcon( HANDLE handle )
{
    INT count = -1;
    ICONCACHE *ptr;

    EnterCriticalSection( &IconCrst );

    for ( ptr = IconAnchor; ptr; ptr = ptr->next )
        if ( ptr->handle == handle )
        {
            if ( ptr->count > 0 ) ptr->count--;
            count = ptr->count;
            break;
        }

    LeaveCriticalSection( &IconCrst );

    return count;
}

/**********************************************************************
 *      CURSORICON_FreeModuleIcons
 */
void CURSORICON_FreeModuleIcons( HMODULE hModule )
{
    ICONCACHE **ptr = &IconAnchor;

    EnterCriticalSection( &IconCrst );

    while ( *ptr )
    {
        if ( (*ptr)->hModule == hModule )
        {
            ICONCACHE *freePtr = *ptr;
            *ptr = freePtr->next;

#ifdef __WIN32OS2__
            CURSORICON_Destroy(freePtr->handle, CID_NONSHARED);
#else
            GlobalFree( freePtr->handle );
#endif
            HeapFree( GetProcessHeap(), 0, freePtr );
            continue;
        }
        ptr = &(*ptr)->next;
    }

    LeaveCriticalSection( &IconCrst );
}

