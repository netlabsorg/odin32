/* $Id: OS2SURFACE.H,v 1.3 1999-06-10 17:10:56 phaller Exp $ */

/*
 * DirectDraw Surface class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2SURFACE_H__
#define __OS2SURFACE_H__

#include "os2ddraw.h"
#include "os2clipper.h"
#include "os2palette.h"


 typedef struct _OS2RECTL {
   LONG     xLeft;    /*  X-coordinate of left-hand edge of rectangle. */
   LONG     yBottom;  /*  Y-coordinate of bottom edge of rectangle. */
   LONG     xRight;   /*  X-coordinate of right-hand edge of rectangle. */
   LONG     yTop;     /*  Y-coordinate of top edge of rectangle. */
 } OS2RECTL;

#define MAX_COLORKEYTYPE    4
#define COLORKEY_DEST       0
#define COLORKEY_DESTOVERLAY    1
#define COLORKEY_SRC        2
#define COLORKEY_SRCOVERLAY 3



 /* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDrawSurface2


class OS2IDirectDrawSurface
{
 public:
    //this one has to go first!
    IDirectDrawSurface2Vtbl *lpVtbl;
    IDirectDrawSurface2Vtbl  Vtbl;

    OS2IDirectDrawSurface(OS2IDirectDraw *lpDirectDraw, LPDDSURFACEDESC lpDDSurfaceDesc);
    ~OS2IDirectDrawSurface();

            int           Referenced;
    inline  HRESULT       GetLastError() { return lastError; };
    inline  int           GetDiveBufNr() { return diveBufNr; };
 private:

 protected:
          HRESULT                lastError;
          HDIVE                  hDive;
          ULONG                  height, width;
          ULONG                  surfaceType;
          ULONG                  diveBufNr;
          double                 screenXFact, screenYFact;
          ULONG                  fccColorFormat, bpp;
          OS2IDirectDraw        *lpDraw;
          OS2IDirectDrawClipper *lpClipper;
          OS2IDirectDrawPalette *lpPalette;
          DDSURFACEDESC          DDSurfaceDesc;
          DDCAPS                 SurfaceCaps;
          BOOL                   fLocked;
          HDC                    hdcImage;
          HBITMAP                hbmImage;
          HGDIOBJ                hgdiOld;
          char                  *bitmapData;
          char                  *pFrameBuffer;
          PBYTE                  lpBuffer;  //non-primary buffers

          ULONG                  ColorKeyFlags;
          ULONG                  ColorSpaceLowValue[MAX_COLORKEYTYPE];
          ULONG                  ColorSpaceHighValue[MAX_COLORKEYTYPE];

          OS2IDirectDrawSurface* attached;           // attached surfaces
          OS2IDirectDrawSurface* flip;           // surface used for next flip

              // Linked list management
              OS2IDirectDrawSurface* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDrawSurface* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT __stdcall SurfQueryInterface(THIS_ REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall SurfAddRef(THIS);
    friend    ULONG   __stdcall SurfRelease(THIS);
    friend    HRESULT __stdcall SurfAddAttachedSurface(THIS_ LPDIRECTDRAWSURFACE2);
    friend    HRESULT __stdcall SurfAddOverlayDirtyRect(THIS_ W32_LPRECT);
    friend    HRESULT __stdcall SurfBlt(THIS_ W32_LPRECT,LPDIRECTDRAWSURFACE2,W32_LPRECT,DWORD, LPDDBLTFX);
    friend    HRESULT __stdcall SurfBltBatch(THIS_ LPDDBLTBATCH, DWORD, DWORD );
    friend    HRESULT __stdcall SurfBltFast(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE2, W32_LPRECT,DWORD);
    friend    HRESULT __stdcall SurfDeleteAttachedSurface(THIS_ DWORD,LPDIRECTDRAWSURFACE2);
    friend    HRESULT __stdcall SurfEnumAttachedSurfaces(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK);
    friend    HRESULT __stdcall SurfEnumOverlayZOrders(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
    friend    HRESULT __stdcall SurfFlip(THIS_ LPDIRECTDRAWSURFACE2, DWORD);
    friend    HRESULT __stdcall SurfGetAttachedSurface(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE2 FAR *);
    friend    HRESULT __stdcall SurfGetBltStatus(THIS_ DWORD);
    friend    HRESULT __stdcall SurfGetCaps(THIS_ LPDDSCAPS);
    friend    HRESULT __stdcall SurfGetClipper(THIS_ LPDIRECTDRAWCLIPPER FAR*);
    friend    HRESULT __stdcall SurfGetColorKey(THIS_ DWORD, LPDDCOLORKEY);
    friend    HRESULT __stdcall SurfGetDC(THIS_ W32_HDC FAR *);
    friend    HRESULT __stdcall SurfGetFlipStatus(THIS_ DWORD);
    friend    HRESULT __stdcall SurfGetOverlayPosition(THIS_ LPLONG, LPLONG );
    friend    HRESULT __stdcall SurfGetPalette(THIS_ LPDIRECTDRAWPALETTE FAR*);
    friend    HRESULT __stdcall SurfGetPixelFormat(THIS_ LPDDPIXELFORMAT);
    friend    HRESULT __stdcall SurfGetSurfaceDesc(THIS_ LPDDSURFACEDESC);
    friend    HRESULT __stdcall SurfInitialize(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC);
    friend    HRESULT __stdcall SurfIsLost(THIS);
    friend    HRESULT __stdcall SurfLock(THIS_ W32_LPRECT,LPDDSURFACEDESC,DWORD,W32_HANDLE);
    friend    HRESULT __stdcall SurfReleaseDC(THIS_ W32_HDC);
    friend    HRESULT __stdcall SurfRestore(THIS);
    friend    HRESULT __stdcall SurfSetClipper(THIS_ LPDIRECTDRAWCLIPPER);
    friend    HRESULT __stdcall SurfSetColorKey(THIS_ DWORD, LPDDCOLORKEY);
    friend    HRESULT __stdcall SurfSetOverlayPosition(THIS_ LONG, LONG );
    friend    HRESULT __stdcall SurfSetPalette(THIS_ LPDIRECTDRAWPALETTE);
    friend    HRESULT __stdcall SurfUnlock(THIS_ LPVOID);
    friend    HRESULT __stdcall SurfUpdateOverlay(THIS_ W32_LPRECT, LPDIRECTDRAWSURFACE2,W32_LPRECT,DWORD, LPDDOVERLAYFX);
    friend    HRESULT __stdcall SurfUpdateOverlayDisplay(THIS_ DWORD);
    friend    HRESULT __stdcall SurfUpdateOverlayZOrder(THIS_ DWORD, LPDIRECTDRAWSURFACE2);
    friend    HRESULT __stdcall SurfGetDDInterface(THIS_ LPVOID FAR *);
    friend    HRESULT __stdcall SurfPageLock(THIS_ DWORD);
    friend    HRESULT __stdcall SurfPageUnlock(THIS_ DWORD);
};


HRESULT __stdcall SurfQueryInterface(THIS_ REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall SurfAddRef(THIS);
ULONG   __stdcall SurfRelease(THIS);
HRESULT __stdcall SurfAddAttachedSurface(THIS_ LPDIRECTDRAWSURFACE2);
HRESULT __stdcall SurfAddOverlayDirtyRect(THIS_ W32_LPRECT);
HRESULT __stdcall SurfBlt(THIS_ W32_LPRECT,LPDIRECTDRAWSURFACE2, W32_LPRECT,DWORD, LPDDBLTFX);
HRESULT __stdcall SurfBltBatch(THIS_ LPDDBLTBATCH, DWORD, DWORD );
HRESULT __stdcall SurfBltFast(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE2, W32_LPRECT,DWORD);
HRESULT __stdcall SurfDeleteAttachedSurface(THIS_ DWORD,LPDIRECTDRAWSURFACE2);
HRESULT __stdcall SurfEnumAttachedSurfaces(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK);
HRESULT __stdcall SurfEnumOverlayZOrders(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
HRESULT __stdcall SurfFlip(THIS_ LPDIRECTDRAWSURFACE2, DWORD);
HRESULT __stdcall SurfGetAttachedSurface(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE2 FAR *);
HRESULT __stdcall SurfGetBltStatus(THIS_ DWORD);
HRESULT __stdcall SurfGetCaps(THIS_ LPDDSCAPS);
HRESULT __stdcall SurfGetClipper(THIS_ LPDIRECTDRAWCLIPPER FAR*);
HRESULT __stdcall SurfGetColorKey(THIS_ DWORD, LPDDCOLORKEY);
HRESULT __stdcall SurfGetDC(THIS_ W32_HDC FAR *);
HRESULT __stdcall SurfGetFlipStatus(THIS_ DWORD);
HRESULT __stdcall SurfGetOverlayPosition(THIS_ LPLONG, LPLONG );
HRESULT __stdcall SurfGetPalette(THIS_ LPDIRECTDRAWPALETTE FAR*);
HRESULT __stdcall SurfGetPixelFormat(THIS_ LPDDPIXELFORMAT);
HRESULT __stdcall SurfGetSurfaceDesc(THIS_ LPDDSURFACEDESC);
HRESULT __stdcall SurfInitialize(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC);
HRESULT __stdcall SurfIsLost(THIS);
HRESULT __stdcall SurfLock(THIS_ W32_LPRECT,LPDDSURFACEDESC,DWORD,W32_HANDLE);
HRESULT __stdcall SurfReleaseDC(THIS_ W32_HDC);
HRESULT __stdcall SurfRestore(THIS);
HRESULT __stdcall SurfSetClipper(THIS_ LPDIRECTDRAWCLIPPER);
HRESULT __stdcall SurfSetColorKey(THIS_ DWORD, LPDDCOLORKEY);
HRESULT __stdcall SurfSetOverlayPosition(THIS_ LONG, LONG );
HRESULT __stdcall SurfSetPalette(THIS_ LPDIRECTDRAWPALETTE);
HRESULT __stdcall SurfUnlock(THIS_ LPVOID);
HRESULT __stdcall SurfUpdateOverlay(THIS_ W32_LPRECT, LPDIRECTDRAWSURFACE2,W32_LPRECT,DWORD, LPDDOVERLAYFX);
HRESULT __stdcall SurfUpdateOverlayDisplay(THIS_ DWORD);
HRESULT __stdcall SurfUpdateOverlayZOrder(THIS_ DWORD, LPDIRECTDRAWSURFACE2);
HRESULT __stdcall SurfGetDDInterface(THIS_ LPVOID FAR *);
HRESULT __stdcall SurfPageLock(THIS_ DWORD);
HRESULT __stdcall SurfPageUnlock(THIS_ DWORD);

#endif
