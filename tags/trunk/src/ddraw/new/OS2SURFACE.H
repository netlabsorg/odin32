/* $Id: OS2SURFACE.H,v 1.1 2000-04-07 18:21:06 mike Exp $ */

/*
 * Direct/X Surface class
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __OS2SURFACE_H__
#define __OS2SURFACE_H__
#define FAR
#include "divewrap.h"
#include <commctrl.h>

#define FAR
#undef THIS
#define THIS VOID*

typedef struct _OS2RECTL {
   LONG     xLeft;    /*  X-coordinate of left-hand edge of rectangle. */
   LONG     yBottom;  /*  Y-coordinate of bottom edge of rectangle. */
   LONG     xRight;   /*  X-coordinate of right-hand edge of rectangle. */
   LONG     yTop;     /*  Y-coordinate of top edge of rectangle. */
 } OS2RECTL;

typedef struct _SURFPRIVATEDATA
{
  GUID    guidTag;
  void    *pData;
  DWORD   dwFlags;
  DWORD   dwSize;
  BOOL    isValid;
}SURFPRIVATEDATA , *PSURFPRIVATEDATA;

class OS2IDirectDrawSurface
{
 public:
  //this one has to go first!
  IDirectDrawSurface2Vtbl *lpVtbl;
  IDirectDrawSurface2Vtbl *lpVtbl2;         // 2nd Vtable pointer An other MS assumtion see below
//  IDirectDrawSurface4Vtbl *lpVtbl;
//  IDirectDrawSurface4Vtbl *lpVtbl2;         // 2nd Vtable pointer An other MS assumtion see below
  DWORD                    dwUnknownData;   // Pointer to unknown data set to DEADDEAD to indicate that we can't handle this
  IDirectDrawSurface2Vtbl  Vtbl2;
  IDirectDrawSurface3Vtbl  Vtbl3;
  IDirectDrawSurface4Vtbl  Vtbl; // With Dx6 CreateSurface creates per default one with Interface 4 !

  OS2IDirectDrawSurface( OS2IDirectDraw *lpDirectDraw,
                         LPDDSURFACEDESC2 lpDDSurfaceDesc,
                         BOOL Implicit = FALSE,    // Indicates that this surface was created  as result of a complex surface
                         BOOL Mainchain = FALSE);  // Indicates if an implicit surface belongs to the primary surface
  ~OS2IDirectDrawSurface();
          int           Referenced;
  inline  HRESULT       GetLastError() { return lastError; };
  inline  int           GetDiveBufNr() { return diveBufNr; };
  inline  BOOL          IsImplicitSurface() {return ImplicitSurface; };
  inline  OS2IDirectDrawSurface* GetFrontBuffer(){ return FrontBuffer;};
  inline  void          SetFrontBuffer( OS2IDirectDrawSurface* NewFBuffer);
          void          ColorConversion(LPRECT lpRect);

 private:
        HRESULT         DoColorFill(LPRECT, DWORD);
        void (__cdecl *BltSolid)( char *pDBDst,
                                char *pFBDst,
                                DWORD dwDstTop,
                                DWORD dwDstLeft,
                                DWORD dwPitchDBDst,
                                DWORD dwPitchFBDst,
                                char *pDBSrc,
                                char *pFBSrc,
                                DWORD dwSrcTop,
                                DWORD dwSrcLeft,
                                DWORD dwWidth,
                                DWORD dwHeight,
                                DWORD dwPitchDBSrc,
                                DWORD dwPitchFBSrc
                               );
        void (__cdecl *ColorConv)( char *pDB,
                                   char *pFB,
                                   DWORD dwTop,
                                   DWORD dwLeft,
                                   DWORD dwWidth,
                                   DWORD dwHeight,
                                   DWORD dwPitchDB,
                                   DWORD dwPitchFB,
                                   VOID  *pPalette
                                   );
        void (__cdecl *ColorFill)( char *pDBDst,
                                   char *pFBDst,
                                   DWORD dwDstTop,
                                   DWORD dwDstLeft,
                                   DWORD dwWidth,
                                   DWORD dwHeight,
                                   DWORD dwPitchDBDst,
                                   DWORD dwPitchFBDst,
                                   DWORD dwColor,
                                   VOID  *pPalette
                                 );
 protected:
        HRESULT                lastError;
        HDIVE                  hDive;
        HDIVE                  hDiveCC; // Color conversion DIVE Handle
        ULONG                  height, width;
        ULONG                  screenHeight, screenWidth;
        ULONG                  surfaceType;
        ULONG                  diveBufNr;
        OS2IDirectDraw        *lpDraw;
        OS2IDirectDrawClipper *lpClipper;
        OS2IDirectDrawPalette *lpPalette;
        DDSURFACEDESC2         DDSurfaceDesc;
        BOOL                   fLocked;
        HDC                    hdcImage;
        HBITMAP                hbmImage;
        HGDIOBJ                hgdiOld;
        DWORD                  dwLastDCUnique;
        // The real pointers hold the allocated memory (for the free)
        // The other an pointer addjusted to QWORDs which we use and report
        char                  *pFrameBuffer, *pFBreal;
        char                  *pDiveBuffer, *pDBreal;
        DWORD                 *pTransBufferSrc;
        DWORD                 *pTransBufferDest;
        DWORD                  dwPitchFB;
        DWORD                  dwPitchDB;
        DWORD                  dwBytesPPDive;
        BOOL                   Updated;
        BOOL                   ImplicitSurface;

             // DDraw internal vars

        DWORD                  dwUniqueValue;
        HDPA DPA_SurfacePrivateData;

        BOOL                   fOverlayValid;
        LONG                   lOverlayX;
        LONG                   lOverlayY;

        // surface management

        OS2IDirectDrawSurface*  BackBuffer;  // Backbuffer goes in here
        OS2IDirectDrawSurface*  FrontBuffer; // NULL if the real frontbuffer of a Flipchain otherwise  the prev. Backbuffer
        OS2IDirectDrawSurface*  NextFlip;    // Only valid in a frontbuffer

        HDPA DPA_SurfaceMipMaps;
        HDPA DPA_SurfaceAttached;
        HDPA DPA_LockedRects;

    friend    HRESULT __stdcall SurfQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall SurfAddRef(THIS);
    friend    ULONG   __stdcall SurfRelease(THIS);
    friend    HRESULT __stdcall SurfAddAttachedSurface(THIS, LPDIRECTDRAWSURFACE2);
    friend    HRESULT __stdcall SurfAddAttachedSurface3(THIS, LPDIRECTDRAWSURFACE3);
    friend    HRESULT __stdcall SurfAddAttachedSurface4(THIS, LPDIRECTDRAWSURFACE4);
    friend    HRESULT __stdcall SurfAddOverlayDirtyRect(THIS, LPRECT);
    friend    HRESULT __stdcall SurfBlt(THIS, LPRECT,LPDIRECTDRAWSURFACE2, LPRECT,DWORD, LPDDBLTFX);
    friend    HRESULT __stdcall SurfBlt3(THIS, LPRECT,LPDIRECTDRAWSURFACE3, LPRECT,DWORD, LPDDBLTFX);
    friend    HRESULT __stdcall SurfBlt4(THIS, LPRECT,LPDIRECTDRAWSURFACE4, LPRECT,DWORD, LPDDBLTFX);
    friend    HRESULT __stdcall SurfBltBatch(THIS, LPDDBLTBATCH, DWORD, DWORD );
    friend    HRESULT __stdcall SurfBltFast(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE2, LPRECT,DWORD);
    friend    HRESULT __stdcall SurfBltFast3(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE3, LPRECT,DWORD);
    friend    HRESULT __stdcall SurfBltFast4(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE4, LPRECT,DWORD);
    friend    HRESULT __stdcall SurfDeleteAttachedSurface(THIS, DWORD,LPDIRECTDRAWSURFACE2);
    friend    HRESULT __stdcall SurfDeleteAttachedSurface3(THIS, DWORD,LPDIRECTDRAWSURFACE3);
    friend    HRESULT __stdcall SurfDeleteAttachedSurface4(THIS, DWORD,LPDIRECTDRAWSURFACE4);
    friend    HRESULT __stdcall SurfEnumAttachedSurfaces(THIS, LPVOID,LPDDENUMSURFACESCALLBACK);
    friend    HRESULT __stdcall SurfEnumAttachedSurfaces4(THIS, LPVOID,LPDDENUMSURFACESCALLBACK2);
    friend    HRESULT __stdcall SurfEnumOverlayZOrders(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
    friend    HRESULT __stdcall SurfEnumOverlayZOrders4(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2);
    friend    HRESULT __stdcall SurfFlip(THIS, LPDIRECTDRAWSURFACE2 , DWORD);
    friend    HRESULT __stdcall SurfFlip3(THIS, LPDIRECTDRAWSURFACE3 , DWORD);
    friend    HRESULT __stdcall SurfFlip4(THIS, LPDIRECTDRAWSURFACE4 , DWORD);
    friend    HRESULT __stdcall SurfGetAttachedSurface(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE2 FAR *);
    friend    HRESULT __stdcall SurfGetAttachedSurface3(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE3 FAR *);
    friend    HRESULT __stdcall SurfGetAttachedSurface4(THIS, LPDDSCAPS2, LPDIRECTDRAWSURFACE4 FAR *);
    friend    HRESULT __stdcall SurfGetBltStatus(THIS, DWORD);
    friend    HRESULT __stdcall SurfGetCaps(THIS, LPDDSCAPS);
    friend    HRESULT __stdcall SurfGetCaps4(THIS, LPDDSCAPS2);
    friend    HRESULT __stdcall SurfGetClipper(THIS, LPDIRECTDRAWCLIPPER FAR*);
    friend    HRESULT __stdcall SurfGetColorKey(THIS, DWORD, LPDDCOLORKEY);
    friend    HRESULT __stdcall SurfGetDC(THIS, HDC FAR *);
    friend    HRESULT __stdcall SurfGetFlipStatus(THIS, DWORD);
    friend    HRESULT __stdcall SurfGetOverlayPosition(THIS, LPLONG, LPLONG );
    friend    HRESULT __stdcall SurfGetPalette(THIS, LPDIRECTDRAWPALETTE FAR*);
    friend    HRESULT __stdcall SurfGetPixelFormat(THIS, LPDDPIXELFORMAT);
    friend    HRESULT __stdcall SurfGetSurfaceDesc(THIS, LPDDSURFACEDESC);
    friend    HRESULT __stdcall SurfGetSurfaceDesc4(THIS, LPDDSURFACEDESC2);
    friend    HRESULT __stdcall SurfInitialize(THIS, LPDIRECTDRAW, LPDDSURFACEDESC);
    friend    HRESULT __stdcall SurfInitialize4(THIS, LPDIRECTDRAW, LPDDSURFACEDESC2);
    friend    HRESULT __stdcall SurfIsLost(THIS);
    friend    HRESULT __stdcall SurfLock(THIS, LPRECT,LPDDSURFACEDESC,DWORD,HANDLE);
    friend    HRESULT __stdcall SurfLock4(THIS, LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE);
    friend    HRESULT __stdcall SurfReleaseDC(THIS, HDC);
    friend    HRESULT __stdcall SurfRestore(THIS);
    friend    HRESULT __stdcall SurfSetClipper(THIS, LPDIRECTDRAWCLIPPER);
    friend    HRESULT __stdcall SurfSetColorKey(THIS, DWORD, LPDDCOLORKEY);
    friend    HRESULT __stdcall SurfSetOverlayPosition(THIS, LONG, LONG );
    friend    HRESULT __stdcall SurfSetPalette(THIS, LPDIRECTDRAWPALETTE);
    friend    HRESULT __stdcall SurfUnlock(THIS, LPVOID);
    friend    HRESULT __stdcall SurfUnlock4(THIS, LPRECT);
    friend    HRESULT __stdcall SurfUpdateOverlay(THIS, LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX);
    friend    HRESULT __stdcall SurfUpdateOverlay3(THIS, LPRECT, LPDIRECTDRAWSURFACE3,LPRECT,DWORD, LPDDOVERLAYFX);
    friend    HRESULT __stdcall SurfUpdateOverlay4(THIS, LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX);
    friend    HRESULT __stdcall SurfUpdateOverlayDisplay(THIS, DWORD);
    friend    HRESULT __stdcall SurfUpdateOverlayZOrder(THIS, DWORD, LPDIRECTDRAWSURFACE2);
    friend    HRESULT __stdcall SurfUpdateOverlayZOrder3(THIS, DWORD, LPDIRECTDRAWSURFACE3);
    friend    HRESULT __stdcall SurfUpdateOverlayZOrder4(THIS, DWORD, LPDIRECTDRAWSURFACE4);
    // Added in V2 interface
    friend    HRESULT __stdcall SurfGetDDInterface(THIS, LPVOID FAR *);
    friend    HRESULT __stdcall SurfPageLock(THIS, DWORD);
    friend    HRESULT __stdcall SurfPageUnlock(THIS, DWORD);
    // Added in V3 interface
    friend    HRESULT __stdcall SurfSetSurfaceDesc(THIS, LPDDSURFACEDESC, DWORD);
    friend    HRESULT __stdcall SurfSetSurfaceDesc4(THIS, LPDDSURFACEDESC2, DWORD);
    // Added in V4 interface
    friend    HRESULT __stdcall SurfSetPrivateData(THIS, REFGUID, LPVOID, DWORD, DWORD);
    friend    HRESULT __stdcall SurfGetPrivateData(THIS, REFGUID, LPVOID, LPDWORD);
    friend    HRESULT __stdcall SurfFreePrivateData(THIS, REFGUID);
    friend    HRESULT __stdcall SurfGetUniquenessValue(THIS, LPDWORD);
    friend    HRESULT __stdcall SurfChangeUniquenessValue(THIS);
};


HRESULT __stdcall SurfQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall SurfAddRef(THIS);
ULONG   __stdcall SurfRelease(THIS);
HRESULT __stdcall SurfAddAttachedSurface(THIS, LPDIRECTDRAWSURFACE2);
HRESULT __stdcall SurfAddAttachedSurface3(THIS, LPDIRECTDRAWSURFACE3);
HRESULT __stdcall SurfAddAttachedSurface4(THIS, LPDIRECTDRAWSURFACE4);
HRESULT __stdcall SurfAddOverlayDirtyRect(THIS, LPRECT);
HRESULT __stdcall SurfBlt(THIS, LPRECT,LPDIRECTDRAWSURFACE2, LPRECT,DWORD, LPDDBLTFX);
HRESULT __stdcall SurfBlt3(THIS, LPRECT,LPDIRECTDRAWSURFACE3, LPRECT,DWORD, LPDDBLTFX);
HRESULT __stdcall SurfBlt4(THIS, LPRECT,LPDIRECTDRAWSURFACE4, LPRECT,DWORD, LPDDBLTFX);
HRESULT __stdcall SurfBltBatch(THIS, LPDDBLTBATCH, DWORD, DWORD );
HRESULT __stdcall SurfBltFast(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE2, LPRECT,DWORD);
HRESULT __stdcall SurfBltFast3(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE3, LPRECT,DWORD);
HRESULT __stdcall SurfBltFast4(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE4, LPRECT,DWORD);
HRESULT __stdcall SurfDeleteAttachedSurface(THIS, DWORD,LPDIRECTDRAWSURFACE2);
HRESULT __stdcall SurfDeleteAttachedSurface3(THIS, DWORD,LPDIRECTDRAWSURFACE3);
HRESULT __stdcall SurfDeleteAttachedSurface4(THIS, DWORD,LPDIRECTDRAWSURFACE4);
HRESULT __stdcall SurfEnumAttachedSurfaces(THIS, LPVOID,LPDDENUMSURFACESCALLBACK);
HRESULT __stdcall SurfEnumAttachedSurfaces4(THIS, LPVOID,LPDDENUMSURFACESCALLBACK2);
HRESULT __stdcall SurfEnumOverlayZOrders(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
HRESULT __stdcall SurfEnumOverlayZOrders4(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2);
HRESULT __stdcall SurfFlip(THIS, LPDIRECTDRAWSURFACE2 , DWORD);
HRESULT __stdcall SurfFlip3(THIS, LPDIRECTDRAWSURFACE3 , DWORD);
HRESULT __stdcall SurfFlip4(THIS, LPDIRECTDRAWSURFACE4 , DWORD);
HRESULT __stdcall SurfGetAttachedSurface(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE2 FAR *);
HRESULT __stdcall SurfGetAttachedSurface3(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE3 FAR *);
HRESULT __stdcall SurfGetAttachedSurface4(THIS, LPDDSCAPS2, LPDIRECTDRAWSURFACE4 FAR *);
HRESULT __stdcall SurfGetBltStatus(THIS, DWORD);
HRESULT __stdcall SurfGetCaps4(THIS, LPDDSCAPS2);
HRESULT __stdcall SurfGetCaps(THIS, LPDDSCAPS);
HRESULT __stdcall SurfGetClipper(THIS, LPDIRECTDRAWCLIPPER FAR*);
HRESULT __stdcall SurfGetColorKey(THIS, DWORD, LPDDCOLORKEY);
HRESULT __stdcall SurfGetDC(THIS, HDC FAR *);
HRESULT __stdcall SurfGetFlipStatus(THIS, DWORD);
HRESULT __stdcall SurfGetOverlayPosition(THIS, LPLONG, LPLONG );
HRESULT __stdcall SurfGetPalette(THIS, LPDIRECTDRAWPALETTE FAR*);
HRESULT __stdcall SurfGetPixelFormat(THIS, LPDDPIXELFORMAT);
HRESULT __stdcall SurfGetSurfaceDesc(THIS, LPDDSURFACEDESC);
HRESULT __stdcall SurfGetSurfaceDesc4(THIS, LPDDSURFACEDESC2);
HRESULT __stdcall SurfInitialize(THIS, LPDIRECTDRAW, LPDDSURFACEDESC);
HRESULT __stdcall SurfInitialize4(THIS, LPDIRECTDRAW, LPDDSURFACEDESC2);
HRESULT __stdcall SurfIsLost(THIS);
HRESULT __stdcall SurfLock(THIS, LPRECT,LPDDSURFACEDESC,DWORD,HANDLE);
HRESULT __stdcall SurfLock4(THIS, LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE);
HRESULT __stdcall SurfReleaseDC(THIS, HDC);
HRESULT __stdcall SurfRestore(THIS);
HRESULT __stdcall SurfSetClipper(THIS, LPDIRECTDRAWCLIPPER);
HRESULT __stdcall SurfSetColorKey(THIS, DWORD, LPDDCOLORKEY);
HRESULT __stdcall SurfSetOverlayPosition(THIS, LONG, LONG );
HRESULT __stdcall SurfSetPalette(THIS, LPDIRECTDRAWPALETTE);
HRESULT __stdcall SurfUnlock(THIS, LPVOID);
HRESULT __stdcall SurfUnlock4(THIS, LPRECT);
HRESULT __stdcall SurfUpdateOverlay(THIS, LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX);
HRESULT __stdcall SurfUpdateOverlay3(THIS, LPRECT, LPDIRECTDRAWSURFACE3,LPRECT,DWORD, LPDDOVERLAYFX);
HRESULT __stdcall SurfUpdateOverlay4(THIS, LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX);
HRESULT __stdcall SurfUpdateOverlayDisplay(THIS, DWORD);
HRESULT __stdcall SurfUpdateOverlayZOrder(THIS, DWORD, LPDIRECTDRAWSURFACE2);
HRESULT __stdcall SurfUpdateOverlayZOrder3(THIS, DWORD, LPDIRECTDRAWSURFACE3);
HRESULT __stdcall SurfUpdateOverlayZOrder4(THIS, DWORD, LPDIRECTDRAWSURFACE4);
HRESULT __stdcall SurfGetDDInterface(THIS, LPVOID FAR *);
HRESULT __stdcall SurfPageLock(THIS, DWORD);
HRESULT __stdcall SurfPageUnlock(THIS, DWORD);
HRESULT __stdcall SurfSetSurfaceDesc(THIS, LPDDSURFACEDESC, DWORD);
HRESULT __stdcall SurfSetSurfaceDesc4(THIS, LPDDSURFACEDESC2, DWORD);
HRESULT __stdcall SurfSetPrivateData(THIS, REFGUID, LPVOID, DWORD, DWORD);
HRESULT __stdcall SurfGetPrivateData(THIS, REFGUID, LPVOID, LPDWORD);
HRESULT __stdcall SurfFreePrivateData(THIS, REFGUID);
HRESULT __stdcall SurfGetUniquenessValue(THIS, LPDWORD);
HRESULT __stdcall SurfChangeUniquenessValue(THIS);

void __cdecl MoveRects(char* pBuffer, LPRECT lpDestRect, LPRECT lpSrcRect, int bbp, LONG lPitch);
void __cdecl TransSRCBlit8(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);
void __cdecl TransSRCBlit16(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);
void __cdecl TransSRCBlit24(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);
void __cdecl TransSRCBlit32(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);


#endif
