/* $Id: OS2SURFACE.H,v 1.16 2001-10-05 12:33:10 sandervl Exp $ */

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

#define RECT_WIDTH(a)	((a)->right - (a)->left)
#define RECT_HEIGHT(a)  ((a)->bottom - (a)->top) 
#define RECT_EQUAL(a,b) (memcmp(a, b, sizeof(RECT)) == 0)

inline BOOL intersects(LPRECT lprcSrc1, LPRECT lprcSrc2) 
{
  if( (lprcSrc1->left >= lprcSrc2->right) || (lprcSrc2->left >= lprcSrc1->right) ||
      (lprcSrc1->top >= lprcSrc2->bottom) || (lprcSrc2->top >= lprcSrc1->bottom) )
  {
      return FALSE;
  }
  return TRUE;
}

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

        SETUP_BLITTER          sBlt;

        // surface management

        OS2IDirectDrawSurface*  BackBuffer;  // Backbuffer goes in here
        OS2IDirectDrawSurface*  FrontBuffer; // NULL if the real frontbuffer of a Flipchain otherwise  the prev. Backbuffer
        OS2IDirectDrawSurface*  NextFlip;    // Only valid in a frontbuffer

        HDPA DPA_SurfaceMipMaps;
        HDPA DPA_SurfaceAttached;
        HDPA DPA_LockedRects;

    friend    HRESULT  WIN32API SurfQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG    WIN32API SurfAddRef(THIS);
    friend    ULONG    WIN32API SurfRelease(THIS);
    friend    HRESULT  WIN32API SurfAddAttachedSurface(THIS, LPDIRECTDRAWSURFACE2);
    friend    HRESULT  WIN32API SurfAddAttachedSurface3(THIS, LPDIRECTDRAWSURFACE3);
    friend    HRESULT  WIN32API SurfAddAttachedSurface4(THIS, LPDIRECTDRAWSURFACE4);
    friend    HRESULT  WIN32API SurfAddOverlayDirtyRect(THIS, LPRECT);
    friend    HRESULT  WIN32API SurfBlt(THIS, LPRECT,LPDIRECTDRAWSURFACE2, LPRECT,DWORD, LPDDBLTFX);
    friend    HRESULT  WIN32API SurfBlt3(THIS, LPRECT,LPDIRECTDRAWSURFACE3, LPRECT,DWORD, LPDDBLTFX);
    friend    HRESULT  WIN32API SurfBlt4(THIS, LPRECT,LPDIRECTDRAWSURFACE4, LPRECT,DWORD, LPDDBLTFX);
    friend    HRESULT  WIN32API SurfDoBlt(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
                                          LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx);
    friend    HRESULT  WIN32API SurfBltBatch(THIS, LPDDBLTBATCH, DWORD, DWORD );
    friend    HRESULT  WIN32API SurfBltFast(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE2, LPRECT,DWORD);
    friend    HRESULT  WIN32API SurfBltFast3(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE3, LPRECT,DWORD);
    friend    HRESULT  WIN32API SurfBltFast4(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE4, LPRECT,DWORD);
    friend    HRESULT  WIN32API SurfDeleteAttachedSurface(THIS, DWORD,LPDIRECTDRAWSURFACE2);
    friend    HRESULT  WIN32API SurfDeleteAttachedSurface3(THIS, DWORD,LPDIRECTDRAWSURFACE3);
    friend    HRESULT  WIN32API SurfDeleteAttachedSurface4(THIS, DWORD,LPDIRECTDRAWSURFACE4);
    friend    HRESULT  WIN32API SurfEnumAttachedSurfaces(THIS, LPVOID,LPDDENUMSURFACESCALLBACK);
    friend    HRESULT  WIN32API SurfEnumAttachedSurfaces4(THIS, LPVOID,LPDDENUMSURFACESCALLBACK2);
    friend    HRESULT  WIN32API SurfEnumOverlayZOrders(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
    friend    HRESULT  WIN32API SurfEnumOverlayZOrders4(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2);
    friend    HRESULT  WIN32API SurfFlip(THIS, LPDIRECTDRAWSURFACE2 , DWORD);
    friend    HRESULT  WIN32API SurfFlip3(THIS, LPDIRECTDRAWSURFACE3 , DWORD);
    friend    HRESULT  WIN32API SurfFlip4(THIS, LPDIRECTDRAWSURFACE4 , DWORD);
    friend    HRESULT  WIN32API SurfGetAttachedSurface(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE2 FAR *);
    friend    HRESULT  WIN32API SurfGetAttachedSurface3(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE3 FAR *);
    friend    HRESULT  WIN32API SurfGetAttachedSurface4(THIS, LPDDSCAPS2, LPDIRECTDRAWSURFACE4 FAR *);
    friend    HRESULT  WIN32API SurfGetBltStatus(THIS, DWORD);
    friend    HRESULT  WIN32API SurfGetCaps(THIS, LPDDSCAPS);
    friend    HRESULT  WIN32API SurfGetCaps4(THIS, LPDDSCAPS2);
    friend    HRESULT  WIN32API SurfGetClipper(THIS, LPDIRECTDRAWCLIPPER FAR*);
    friend    HRESULT  WIN32API SurfGetColorKey(THIS, DWORD, LPDDCOLORKEY);
    friend    HRESULT  WIN32API SurfGetDC(THIS, HDC FAR *);
    friend    HRESULT  WIN32API SurfGetFlipStatus(THIS, DWORD);
    friend    HRESULT  WIN32API SurfGetOverlayPosition(THIS, LPLONG, LPLONG );
    friend    HRESULT  WIN32API SurfGetPalette(THIS, LPDIRECTDRAWPALETTE FAR*);
    friend    HRESULT  WIN32API SurfGetPixelFormat(THIS, LPDDPIXELFORMAT);
    friend    HRESULT  WIN32API SurfGetSurfaceDesc(THIS, LPDDSURFACEDESC);
    friend    HRESULT  WIN32API SurfGetSurfaceDesc4(THIS, LPDDSURFACEDESC2);
    friend    HRESULT  WIN32API SurfInitialize(THIS, LPDIRECTDRAW, LPDDSURFACEDESC);
    friend    HRESULT  WIN32API SurfInitialize4(THIS, LPDIRECTDRAW, LPDDSURFACEDESC2);
    friend    HRESULT  WIN32API SurfIsLost(THIS);
    friend    HRESULT  WIN32API SurfLock(THIS, LPRECT,LPDDSURFACEDESC,DWORD,HANDLE);
    friend    HRESULT  WIN32API SurfLock4(THIS, LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE);
    friend    HRESULT  WIN32API SurfReleaseDC(THIS, HDC);
    friend    HRESULT  WIN32API SurfRestore(THIS);
    friend    HRESULT  WIN32API SurfSetClipper(THIS, LPDIRECTDRAWCLIPPER);
    friend    HRESULT  WIN32API SurfSetColorKey(THIS, DWORD, LPDDCOLORKEY);
    friend    HRESULT  WIN32API SurfSetOverlayPosition(THIS, LONG, LONG );
    friend    HRESULT  WIN32API SurfSetPalette(THIS, LPDIRECTDRAWPALETTE);
    friend    HRESULT  WIN32API SurfUnlock(THIS, LPVOID);
    friend    HRESULT  WIN32API SurfUnlock4(THIS, LPRECT);
    friend    HRESULT  WIN32API SurfUpdateOverlay(THIS, LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX);
    friend    HRESULT  WIN32API SurfUpdateOverlay3(THIS, LPRECT, LPDIRECTDRAWSURFACE3,LPRECT,DWORD, LPDDOVERLAYFX);
    friend    HRESULT  WIN32API SurfUpdateOverlay4(THIS, LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX);
    friend    HRESULT  WIN32API SurfUpdateOverlayDisplay(THIS, DWORD);
    friend    HRESULT  WIN32API SurfUpdateOverlayZOrder(THIS, DWORD, LPDIRECTDRAWSURFACE2);
    friend    HRESULT  WIN32API SurfUpdateOverlayZOrder3(THIS, DWORD, LPDIRECTDRAWSURFACE3);
    friend    HRESULT  WIN32API SurfUpdateOverlayZOrder4(THIS, DWORD, LPDIRECTDRAWSURFACE4);
    // Added in V2 interface
    friend    HRESULT  WIN32API SurfGetDDInterface(THIS, LPVOID FAR *);
    friend    HRESULT  WIN32API SurfPageLock(THIS, DWORD);
    friend    HRESULT  WIN32API SurfPageUnlock(THIS, DWORD);
    // Added in V3 interface
    friend    HRESULT  WIN32API SurfSetSurfaceDesc(THIS, LPDDSURFACEDESC, DWORD);
    friend    HRESULT  WIN32API SurfSetSurfaceDesc4(THIS, LPDDSURFACEDESC2, DWORD);
    // Added in V4 interface
    friend    HRESULT  WIN32API SurfSetPrivateData(THIS, REFGUID, LPVOID, DWORD, DWORD);
    friend    HRESULT  WIN32API SurfGetPrivateData(THIS, REFGUID, LPVOID, LPDWORD);
    friend    HRESULT  WIN32API SurfFreePrivateData(THIS, REFGUID);
    friend    HRESULT  WIN32API SurfGetUniquenessValue(THIS, LPDWORD);
    friend    HRESULT  WIN32API SurfChangeUniquenessValue(THIS);
};


HRESULT  WIN32API SurfQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG    WIN32API SurfAddRef(THIS);
ULONG    WIN32API SurfRelease(THIS);
HRESULT  WIN32API SurfAddAttachedSurface(THIS, LPDIRECTDRAWSURFACE2);
HRESULT  WIN32API SurfAddAttachedSurface3(THIS, LPDIRECTDRAWSURFACE3);
HRESULT  WIN32API SurfAddAttachedSurface4(THIS, LPDIRECTDRAWSURFACE4);
HRESULT  WIN32API SurfAddOverlayDirtyRect(THIS, LPRECT);
HRESULT  WIN32API SurfBlt(THIS, LPRECT,LPDIRECTDRAWSURFACE2, LPRECT,DWORD, LPDDBLTFX);
HRESULT  WIN32API SurfBlt3(THIS, LPRECT,LPDIRECTDRAWSURFACE3, LPRECT,DWORD, LPDDBLTFX);
HRESULT  WIN32API SurfBlt4(THIS, LPRECT,LPDIRECTDRAWSURFACE4, LPRECT,DWORD, LPDDBLTFX);
HRESULT  WIN32API SurfDoBlt(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
                            LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx);
HRESULT  WIN32API SurfBltBatch(THIS, LPDDBLTBATCH, DWORD, DWORD );
HRESULT  WIN32API SurfBltFast(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE2, LPRECT,DWORD);
HRESULT  WIN32API SurfBltFast3(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE3, LPRECT,DWORD);
HRESULT  WIN32API SurfBltFast4(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE4, LPRECT,DWORD);
HRESULT  WIN32API SurfDeleteAttachedSurface(THIS, DWORD,LPDIRECTDRAWSURFACE2);
HRESULT  WIN32API SurfDeleteAttachedSurface3(THIS, DWORD,LPDIRECTDRAWSURFACE3);
HRESULT  WIN32API SurfDeleteAttachedSurface4(THIS, DWORD,LPDIRECTDRAWSURFACE4);
HRESULT  WIN32API SurfEnumAttachedSurfaces(THIS, LPVOID,LPDDENUMSURFACESCALLBACK);
HRESULT  WIN32API SurfEnumAttachedSurfaces4(THIS, LPVOID,LPDDENUMSURFACESCALLBACK2);
HRESULT  WIN32API SurfEnumOverlayZOrders(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
HRESULT  WIN32API SurfEnumOverlayZOrders4(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2);
HRESULT  WIN32API SurfFlip(THIS, LPDIRECTDRAWSURFACE2 , DWORD);
HRESULT  WIN32API SurfFlip3(THIS, LPDIRECTDRAWSURFACE3 , DWORD);
HRESULT  WIN32API SurfFlip4(THIS, LPDIRECTDRAWSURFACE4 , DWORD);
HRESULT  WIN32API SurfGetAttachedSurface(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE2 FAR *);
HRESULT  WIN32API SurfGetAttachedSurface3(THIS, LPDDSCAPS, LPDIRECTDRAWSURFACE3 FAR *);
HRESULT  WIN32API SurfGetAttachedSurface4(THIS, LPDDSCAPS2, LPDIRECTDRAWSURFACE4 FAR *);
HRESULT  WIN32API SurfGetBltStatus(THIS, DWORD);
HRESULT  WIN32API SurfGetCaps4(THIS, LPDDSCAPS2);
HRESULT  WIN32API SurfGetCaps(THIS, LPDDSCAPS);
HRESULT  WIN32API SurfGetClipper(THIS, LPDIRECTDRAWCLIPPER FAR*);
HRESULT  WIN32API SurfGetColorKey(THIS, DWORD, LPDDCOLORKEY);
HRESULT  WIN32API SurfGetDC(THIS, HDC FAR *);
HRESULT  WIN32API SurfGetFlipStatus(THIS, DWORD);
HRESULT  WIN32API SurfGetOverlayPosition(THIS, LPLONG, LPLONG );
HRESULT  WIN32API SurfGetPalette(THIS, LPDIRECTDRAWPALETTE FAR*);
HRESULT  WIN32API SurfGetPixelFormat(THIS, LPDDPIXELFORMAT);
HRESULT  WIN32API SurfGetSurfaceDesc(THIS, LPDDSURFACEDESC);
HRESULT  WIN32API SurfGetSurfaceDesc4(THIS, LPDDSURFACEDESC2);
HRESULT  WIN32API SurfInitialize(THIS, LPDIRECTDRAW, LPDDSURFACEDESC);
HRESULT  WIN32API SurfInitialize4(THIS, LPDIRECTDRAW, LPDDSURFACEDESC2);
HRESULT  WIN32API SurfIsLost(THIS);
HRESULT  WIN32API SurfLock(THIS, LPRECT,LPDDSURFACEDESC,DWORD,HANDLE);
HRESULT  WIN32API SurfLock4(THIS, LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE);
HRESULT  WIN32API SurfReleaseDC(THIS, HDC);
HRESULT  WIN32API SurfRestore(THIS);
HRESULT  WIN32API SurfSetClipper(THIS, LPDIRECTDRAWCLIPPER);
HRESULT  WIN32API SurfSetColorKey(THIS, DWORD, LPDDCOLORKEY);
HRESULT  WIN32API SurfSetOverlayPosition(THIS, LONG, LONG );
HRESULT  WIN32API SurfSetPalette(THIS, LPDIRECTDRAWPALETTE);
HRESULT  WIN32API SurfUnlock(THIS, LPVOID);
HRESULT  WIN32API SurfUnlock4(THIS, LPRECT);
HRESULT  WIN32API SurfUpdateOverlay(THIS, LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX);
HRESULT  WIN32API SurfUpdateOverlay3(THIS, LPRECT, LPDIRECTDRAWSURFACE3,LPRECT,DWORD, LPDDOVERLAYFX);
HRESULT  WIN32API SurfUpdateOverlay4(THIS, LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX);
HRESULT  WIN32API SurfUpdateOverlayDisplay(THIS, DWORD);
HRESULT  WIN32API SurfUpdateOverlayZOrder(THIS, DWORD, LPDIRECTDRAWSURFACE2);
HRESULT  WIN32API SurfUpdateOverlayZOrder3(THIS, DWORD, LPDIRECTDRAWSURFACE3);
HRESULT  WIN32API SurfUpdateOverlayZOrder4(THIS, DWORD, LPDIRECTDRAWSURFACE4);
HRESULT  WIN32API SurfGetDDInterface(THIS, LPVOID FAR *);
HRESULT  WIN32API SurfPageLock(THIS, DWORD);
HRESULT  WIN32API SurfPageUnlock(THIS, DWORD);
HRESULT  WIN32API SurfSetSurfaceDesc(THIS, LPDDSURFACEDESC, DWORD);
HRESULT  WIN32API SurfSetSurfaceDesc4(THIS, LPDDSURFACEDESC2, DWORD);
HRESULT  WIN32API SurfSetPrivateData(THIS, REFGUID, LPVOID, DWORD, DWORD);
HRESULT  WIN32API SurfGetPrivateData(THIS, REFGUID, LPVOID, LPDWORD);
HRESULT  WIN32API SurfFreePrivateData(THIS, REFGUID);
HRESULT  WIN32API SurfGetUniquenessValue(THIS, LPDWORD);
HRESULT  WIN32API SurfChangeUniquenessValue(THIS);

void __cdecl MoveRects(char* pBuffer, LPRECT lpDestRect, LPRECT lpSrcRect, int bbp, LONG lPitch);
void __cdecl TransSRCBlit8(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);
void __cdecl TransSRCBlit16(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);
void __cdecl TransSRCBlit24(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);
void __cdecl TransSRCBlit32(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect);


#endif
