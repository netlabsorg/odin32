#include <stdlib.h>
#include <string.h>
#include <memory.h>
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                         \
                  ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                  ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#include <fourcc.h>
#define INITGUID
#include "os2ddraw.h"
#include "os2clipper.h"
#include "os2palette.h"
#include "os2surface.h"
#include "rectangle.h"
#define _OS2WIN_H
#define FAR
#include "misc.h"
#include "asmutil.h"
#include <winerror.h>
#include <os2win.h>
#ifndef __WATCOMC__
  #include <builtin.h>
#endif

extern FOURCC  SupportedFourCCs[];
// ToDo: Move the following 2 defines in the right WINE headers.
BYTE DefaultPalette[] = { 0x00,0x00,0x00,   // 0
                          0x80,0x00,0x00,   // 1
                          0x00,0x80,0x00,   // 2
                          0x80,0x80,0x00,   // 3
                          0x00,0x00,0x80,   // 4
                          0x80,0x00,0x80,   // 5
                          0x00,0x80,0x80,   // 6
                          0xC0,0xC0,0xC0,   // 7
                          0xC0,0xDC,0xC0,   // 8
                          0xA6,0xCA,0xF0,   // 9
                          0x04,0x04,0x04,   // 10
                          0x08,0x08,0x08,   // 11
                          0x0C,0x0C,0x0C,   // 12
                          0x11,0x11,0x11,   // 13
                          0x16,0x16,0x16,   // 14
                          0x1C,0x1C,0x1C,   // 15
                          0x22,0x22,0x22,   // 16
                          0x29,0x29,0x29,   // 17
                          0x55,0x55,0x55,   // 18
                          0x4D,0x4D,0x4D,
                          0x42,0x42,0x42,
                          0x39,0x39,0x39,
                          0x81,0x81,0x81,
                          0x81,0x00,0x00,
                          0x00,0x81,0x00,
                          0x81,0x81,0x00,
                          0x00,0x00,0x81,
                          0x81,0x00,0x81,
                          0x00,0x81,0x81,
                          0x33,0x00,0x00,
                          0x66,0x00,0x00,
                          0x99,0x00,0x00,
                          0xCC,0x00,0x00,
                          0x00,0x33,0x00,
                          0x33,0x33,0x00,
                          0x66,0x33,0x00,
                          0x99,0x33,0x00,
                          0xCC,0x33,0x00,
                          0xFF,0x33,0x00,
                          0x00,0x66,0x00,
                          0x33,0x66,0x00,
                          0x66,0x66,0x00,
                          0x99,0x66,0x00,
                          0xCC,0x66,0x00,
                          0xFF,0x66,0x00,
                          0x00,0x99,0x00,
                          0x33,0x99,0x00,
                          0x66,0x99,0x00,
                          0x99,0x99,0x00,
                          0xCC,0x99,0x00,
                          0xFF,0x99,0x00,
                          0x00,0xCC,0x00,
                          0x33,0xCC,0x00,
                          0x66,0xCC,0x00,
                          0x99,0xCC,0x00,
                          0xCC,0xCC,0x00,
                          0xFF,0xCC,0x00,
                          0x66,0xFF,0x00,
                          0x99,0xFF,0x00,
                          0xCC,0xFF,0x00,
                          0x00,0x00,0x33,
                          0x33,0x00,0x33,
                          0x66,0x00,0x33,
                          0x99,0x00,0x33,
                          0xCC,0x00,0x33,
                          0xFF,0x00,0x33,
                          0x00,0x33,0x33,
                          0x33,0x33,0x33,
                          0x66,0x33,0x33,
                          0x99,0x33,0x33,
                          0xCC,0x33,0x33,
                          0xFF,0x33,0x33,
                          0x00,0x66,0x33,
                          0x33,0x66,0x33,
                          0x66,0x66,0x33,
                          0x99,0x66,0x33,
                          0xCC,0x66,0x33,
                          0xFF,0x66,0x33,
                          0x00,0x99,0x33,
                          0x33,0x99,0x33,
                          0x66,0x99,0x33,
                          0x99,0x99,0x33,
                          0xCC,0x99,0x33,
                          0xFF,0x99,0x33,
                          0x00,0xCC,0x33,
                          0x33,0xCC,0x33,
                          0x66,0xCC,0x33,
                          0x99,0xCC,0x33,
                          0xCC,0xCC,0x33,
                          0xFF,0xCC,0x33,
                          0x33,0xFF,0x33,
                          0x66,0xFF,0x33,
                          0x99,0xFF,0x33,
                          0xCC,0xFF,0x33,
                          0xFF,0xFF,0x33,
                          0x00,0x00,0x66,
                          0x33,0x00,0x66,
                          0x66,0x00,0x66,
                          0x99,0x00,0x66,
                          0xCC,0x00,0x66,
                          0xFF,0x00,0x66,
                          0x00,0x33,0x66,
                          0x33,0x33,0x66,
                          0x66,0x33,0x66,
                          0x99,0x33,0x66,
                          0xCC,0x33,0x66,
                          0xFF,0x33,0x66,
                          0x00,0x66,0x66,
                          0x33,0x66,0x66,
                          0x66,0x66,0x66,
                          0x99,0x66,0x66,
                          0xCC,0x66,0x66,
                          0x00,0x99,0x66,
                          0x33,0x99,0x66,
                          0x66,0x99,0x66,
                          0x99,0x99,0x66,
                          0xCC,0x99,0x66,
                          0xFF,0x99,0x66,
                          0x00,0xCC,0x66,
                          0x33,0xCC,0x66,
                          0x99,0xCC,0x66,
                          0xCC,0xCC,0x66,
                          0xFF,0xCC,0x66,
                          0x00,0xFF,0x66,
                          0x33,0xFF,0x66,
                          0x99,0xFF,0x66,
                          0xCC,0xFF,0x66,
                          0xFF,0x00,0xCC,
                          0xCC,0x00,0xFF,
                          0x00,0x99,0x99,
                          0x99,0x33,0x99,
                          0x99,0x00,0x99,
                          0xCC,0x00,0x99,
                          0x00,0x00,0x99,
                          0x33,0x33,0x99,
                          0x66,0x00,0x99,
                          0xCC,0x33,0x99,
                          0xFF,0x00,0x99,
                          0x00,0x66,0x99,
                          0x33,0x33,0x99,
                          0x33,0x66,0x99,
                          0x66,0x33,0x99,
                          0x99,0x33,0x99,
                          0xCC,0x66,0x99,
                          0xFF,0x33,0x99,
                          0x33,0x99,0x99,
                          0x66,0x99,0x99,
                          0x99,0x99,0x99,
                          0xCC,0x99,0x99,
                          0xFF,0x99,0x99,
                          0x00,0xCC,0x99,
                          0x33,0xCC,0x99,
                          0x66,0xCC,0x66,
                          0x99,0xCC,0x99,
                          0xCC,0xCC,0x99,
                          0xFF,0xCC,0x99,
                          0x00,0xFF,0x99,
                          0x33,0xFF,0x99,
                          0x66,0xCC,0x99,
                          0x99,0xFF,0x99,
                          0xCC,0xFF,0x99,
                          0xFF,0xFF,0x99,
                          0x00,0x00,0xCC,
                          0x33,0x00,0x99,
                          0x66,0x00,0xCC,
                          0x99,0x00,0xCC,
                          0xCC,0x00,0xCC,
                          0x00,0x33,0x99,
                          0x33,0x33,0xCC,
                          0x66,0x33,0xCC,
                          0x99,0x33,0xCC,
                          0xCC,0x33,0xCC,
                          0xFF,0x33,0xCC,
                          0x00,0x66,0xCC,
                          0x33,0x66,0xCC,
                          0x66,0x66,0x99,
                          0x99,0x66,0xCC,
                          0xCC,0x66,0xCC,
                          0xFF,0x66,0x99,
                          0x00,0x99,0xCC,
                          0x33,0x99,0xCC,
                          0x66,0x99,0xCC,
                          0x99,0x99,0xCC,
                          0xCC,0x99,0xCC,
                          0xFF,0x99,0xCC,
                          0x00,0xCC,0xCC,
                          0x33,0xCC,0xCC,
                          0x66,0xCC,0xCC,
                          0x99,0xCC,0xCC,
                          0xCC,0xCC,0xCC,
                          0xFF,0xCC,0xCC,
                          0x00,0xFF,0xCC,
                          0x33,0xFF,0xCC,
                          0x66,0xFF,0x99,
                          0x99,0xFF,0xCC,
                          0xCC,0xFF,0xCC,
                          0xFF,0xFF,0xCC,
                          0x33,0x00,0xCC,
                          0x66,0x00,0xFF,
                          0x99,0x00,0xFF,
                          0x00,0x33,0xCC,
                          0x33,0x33,0xFF,
                          0x66,0x33,0xFF,
                          0x99,0x33,0xFF,
                          0xCC,0x33,0xFF,
                          0xFF,0x33,0xFF,
                          0x00,0x66,0xFF,
                          0x33,0x66,0xFF,
                          0x66,0x66,0xCC,
                          0x99,0x66,0xFF,
                          0xCC,0x66,0xFF,
                          0xFF,0x66,0xCC,
                          0x00,0x99,0xFF,
                          0x33,0x99,0xFF,
                          0x66,0x99,0xFF,
                          0x99,0x99,0xFF,
                          0xCC,0x99,0xFF,
                          0xFF,0x99,0xFF,
                          0x00,0xCC,0xFF,
                          0x33,0xCC,0xFF,
                          0x66,0xCC,0xFF,
                          0x99,0xCC,0xFF,
                          0xCC,0xCC,0xFF,
                          0xFF,0xCC,0xFF,
                          0x33,0xFF,0xFF,
                          0x66,0xFF,0xCC,
                          0x99,0xFF,0xFF,
                          0xCC,0xFF,0xFF,
                          0xFF,0x66,0x66,
                          0x66,0xFF,0x66,
                          0xFF,0xFF,0x66,
                          0x66,0x66,0xFF,
                          0xFF,0x66,0xFF,
                          0x66,0xFF,0xFF,
                          0xC1,0xC1,0xC1,
                          0x5F,0x5F,0x5F,
                          0x77,0x77,0x77,
                          0x86,0x86,0x86,
                          0x96,0x96,0x96,
                          0xCB,0xCB,0xCB,
                          0xB2,0xB2,0xB2,
                          0xD7,0xD7,0xD7,
                          0xDD,0xDD,0xDD,
                          0xE3,0xE3,0xE3,
                          0xEA,0xEA,0xEA,
                          0xF1,0xF1,0xF1,
                          0xF8,0xF8,0xF8,
                          0xFF,0xFB,0xF0,
                          0xA0,0xA0,0xA4,
                          0x80,0x80,0x80,
                          0xFF,0x00,0x00,
                          0x00,0xFF,0x00,
                          0xFF,0xFF,0x00,
                          0x00,0x00,0xFF,
                          0xFF,0x00,0xFF,
//                          0x00,0xFF,0xFF,
                          0xFF,0xFF,0xFF};

WORD wDefaultPalete16[256];
BOOL fPalInit=FALSE;

#define CBM_CREATEDIB   0x02L   /* create DIB bitmap */
#define BI_BITFIELDS  0x03L
#ifdef DEBUG

// ******************************************************************************
// *    internal helper functions from WINE
// *

static void _dump_DDBLTFX(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    char  *name;
  } flags[] = {
#define FE(x) { x, #x},
    FE(DDBLTFX_ARITHSTRETCHY)
    FE(DDBLTFX_MIRRORLEFTRIGHT)
    FE(DDBLTFX_MIRRORUPDOWN)
    FE(DDBLTFX_NOTEARING)
    FE(DDBLTFX_ROTATE180)
    FE(DDBLTFX_ROTATE270)
    FE(DDBLTFX_ROTATE90)
    FE(DDBLTFX_ZBUFFERRANGE)
    FE(DDBLTFX_ZBUFFERBASEDEST)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
     if (flags[i].mask & flagmask) {
        WriteLog("%s ",flags[i].name);
     };
  WriteLog("\n");

}

static void _dump_DDBLTFAST(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    char  *name;
  } flags[] = {
#define FE(x) { x, #x},
    FE(DDBLTFAST_NOCOLORKEY)
    FE(DDBLTFAST_SRCCOLORKEY)
    FE(DDBLTFAST_DESTCOLORKEY)
    FE(DDBLTFAST_WAIT)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      WriteLog("%s ",flags[i].name);
  WriteLog("\n");
}

static void _dump_DDBLT(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    char  *name;
  } flags[] = {
#define FE(x) { x, #x},
    FE(DDBLT_ALPHADEST)
    FE(DDBLT_ALPHADESTCONSTOVERRIDE)
    FE(DDBLT_ALPHADESTNEG)
    FE(DDBLT_ALPHADESTSURFACEOVERRIDE)
    FE(DDBLT_ALPHAEDGEBLEND)
    FE(DDBLT_ALPHASRC)
    FE(DDBLT_ALPHASRCCONSTOVERRIDE)
    FE(DDBLT_ALPHASRCNEG)
    FE(DDBLT_ALPHASRCSURFACEOVERRIDE)
    FE(DDBLT_ASYNC)
    FE(DDBLT_COLORFILL)
    FE(DDBLT_DDFX)
    FE(DDBLT_DDROPS)
    FE(DDBLT_KEYDEST)
    FE(DDBLT_KEYDESTOVERRIDE)
    FE(DDBLT_KEYSRC)
    FE(DDBLT_KEYSRCOVERRIDE)
    FE(DDBLT_ROP)
    FE(DDBLT_ROTATIONANGLE)
    FE(DDBLT_ZBUFFER)
    FE(DDBLT_ZBUFFERDESTCONSTOVERRIDE)
    FE(DDBLT_ZBUFFERDESTOVERRIDE)
    FE(DDBLT_ZBUFFERSRCCONSTOVERRIDE)
    FE(DDBLT_ZBUFFERSRCOVERRIDE)
    FE(DDBLT_WAIT)
    FE(DDBLT_DEPTHFILL)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      WriteLog("%s ",flags[i].name);
  WriteLog("\n");
}

static void _dump_DDSCAPS(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    char  *name;
  } flags[] = {
#define FE(x) { x, #x},
    FE(DDSCAPS_RESERVED1)
    FE(DDSCAPS_ALPHA)
    FE(DDSCAPS_BACKBUFFER)
    FE(DDSCAPS_COMPLEX)
    FE(DDSCAPS_FLIP)
    FE(DDSCAPS_FRONTBUFFER)
    FE(DDSCAPS_OFFSCREENPLAIN)
    FE(DDSCAPS_OVERLAY)
    FE(DDSCAPS_PALETTE)
    FE(DDSCAPS_PRIMARYSURFACE)
    FE(DDSCAPS_PRIMARYSURFACELEFT)
    FE(DDSCAPS_SYSTEMMEMORY)
    FE(DDSCAPS_TEXTURE)
    FE(DDSCAPS_3DDEVICE)
    FE(DDSCAPS_VIDEOMEMORY)
    FE(DDSCAPS_VISIBLE)
    FE(DDSCAPS_WRITEONLY)
    FE(DDSCAPS_ZBUFFER)
    FE(DDSCAPS_OWNDC)
    FE(DDSCAPS_LIVEVIDEO)
    FE(DDSCAPS_HWCODEC)
    FE(DDSCAPS_MODEX)
    FE(DDSCAPS_MIPMAP)
    FE(DDSCAPS_RESERVED2)
    FE(DDSCAPS_ALLOCONLOAD)
    FE(DDSCAPS_VIDEOPORT)
    FE(DDSCAPS_LOCALVIDMEM)
    FE(DDSCAPS_NONLOCALVIDMEM)
    FE(DDSCAPS_STANDARDVGAMODE)
    FE(DDSCAPS_OPTIMIZED)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      WriteLog("%s ",flags[i].name);
  WriteLog("\n");
}

static void _dump_DDSCAPS2(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    char  *name;
  } flags[] = {
#define FE(x) { x, #x},
    FE(DDSCAPS2_HARDWAREDEINTERLACE)
    FE(DDSCAPS2_HINTANTIALIASING)
    FE(DDSCAPS2_HINTDYNAMIC)
    FE(DDSCAPS2_HINTSTATIC)
    FE(DDSCAPS2_OPAQUE)
    FE(DDSCAPS2_TEXTUREMANAGE)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      WriteLog("%s ",flags[i].name);
  WriteLog("\n");
}


static void _dump_DDSD(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    char  *name;
  } flags[] = {
    FE(DDSD_CAPS)
    FE(DDSD_HEIGHT)
    FE(DDSD_WIDTH)
    FE(DDSD_PITCH)
    FE(DDSD_BACKBUFFERCOUNT)
    FE(DDSD_ZBUFFERBITDEPTH)
    FE(DDSD_ALPHABITDEPTH)
    FE(DDSD_PIXELFORMAT)
    FE(DDSD_CKDESTOVERLAY)
    FE(DDSD_CKDESTBLT)
    FE(DDSD_CKSRCOVERLAY)
    FE(DDSD_CKSRCBLT)
    FE(DDSD_MIPMAPCOUNT)
    FE(DDSD_REFRESHRATE)
    FE(DDSD_LINEARSIZE)
    FE(DDSD_LPSURFACE)
    FE(DDSD_TEXTURESTAGE)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      WriteLog("%s ",flags[i].name);
  WriteLog("\n");
}

static void _dump_DDCOLORKEY(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    char  *name;
  } flags[] = {
#define FE(x) { x, #x},
          FE(DDPF_ALPHAPIXELS)
    FE(DDPF_ALPHA)
    FE(DDPF_FOURCC)
    FE(DDPF_PALETTEINDEXED4)
    FE(DDPF_PALETTEINDEXEDTO8)
    FE(DDPF_PALETTEINDEXED8)
    FE(DDPF_RGB)
    FE(DDPF_COMPRESSED)
    FE(DDPF_RGBTOYUV)
    FE(DDPF_YUV)
    FE(DDPF_ZBUFFER)
    FE(DDPF_PALETTEINDEXED1)
    FE(DDPF_PALETTEINDEXED2)
    FE(DDPF_ZPIXELS)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      WriteLog("%s ",flags[i].name);
  WriteLog("\n");
}

static void _dump_pixelformat(LPDDPIXELFORMAT pf) {
  _dump_DDCOLORKEY(pf->dwFlags);
  WriteLog("dwFourCC : %ld\n", pf->dwFourCC);
  WriteLog("RBG bit cbout : %ld\n", pf->dwRGBBitCount);
  WriteLog("Masks : R %08lx  G %08lx  B %08lx  A %08lx\n",
       pf->dwRBitMask, pf->dwGBitMask, pf->dwBBitMask, pf->dwRGBAlphaBitMask);
}

// End of Internal Helpers
#endif

//******************************************************************************
//******************************************************************************
OS2IDirectDrawSurface::OS2IDirectDrawSurface(OS2IDirectDraw *lpDirectDraw,
               LPDDSURFACEDESC2 lpDDSurfaceDesc, BOOL Implicit, BOOL Mainchain) :
                  Referenced(0), lastError(DD_OK),
                 diveBufNr(-1), lpClipper(NULL),
                 lpPalette(NULL), lpDraw(NULL),
                 fLocked(FALSE), hdcImage(NULL),
                 hbmImage(NULL),
                 pFrameBuffer(NULL),Updated(FALSE),
                 fOverlayValid(FALSE),
                 BackBuffer(NULL),FrontBuffer(NULL)

{
  ULONG rc;
  DWORD i;
  DIVE_CAPS dCaps;
  DDSURFACEDESC2 ComplexSurfaceDesc;
  OS2IDirectDrawSurface *AttachedSurface;
  OS2IDirectDrawSurface *MipMapSurface;

  lpVtbl                     = &Vtbl;
  Vtbl.AddRef                = SurfAddRef;
  Vtbl.Release               = SurfRelease;
  Vtbl.QueryInterface        = SurfQueryInterface;
  Vtbl.AddAttachedSurface    = SurfAddAttachedSurface4;
  Vtbl.AddOverlayDirtyRect   = SurfAddOverlayDirtyRect;
  Vtbl.Blt                   = SurfBlt4;
  Vtbl.BltBatch              = SurfBltBatch;
  Vtbl.BltFast               = SurfBltFast4;
  Vtbl.DeleteAttachedSurface = SurfDeleteAttachedSurface4;
  Vtbl.EnumAttachedSurfaces  = SurfEnumAttachedSurfaces4;
  Vtbl.EnumOverlayZOrders    = SurfEnumOverlayZOrders4;
  Vtbl.Flip                  = SurfFlip4;
  Vtbl.GetAttachedSurface    = SurfGetAttachedSurface4;
  Vtbl.GetBltStatus          = SurfGetBltStatus;
  Vtbl.GetCaps               = SurfGetCaps4;
  Vtbl.GetClipper            = SurfGetClipper;
  Vtbl.GetColorKey           = SurfGetColorKey;
  Vtbl.GetDC                 = SurfGetDC;
  Vtbl.GetFlipStatus         = SurfGetFlipStatus;
  Vtbl.GetOverlayPosition    = SurfGetOverlayPosition;
  Vtbl.GetPalette            = SurfGetPalette;
  Vtbl.GetPixelFormat        = SurfGetPixelFormat;
  Vtbl.GetSurfaceDesc        = SurfGetSurfaceDesc4;
  Vtbl.Initialize            = SurfInitialize4;
  Vtbl.IsLost                = SurfIsLost;
  Vtbl.Lock                  = SurfLock4;
  Vtbl.ReleaseDC             = SurfReleaseDC;
  Vtbl.Restore               = SurfRestore;
  Vtbl.SetClipper            = SurfSetClipper;
  Vtbl.SetColorKey           = SurfSetColorKey;
  Vtbl.SetOverlayPosition    = SurfSetOverlayPosition;
  Vtbl.SetPalette            = SurfSetPalette;
  Vtbl.Unlock                = SurfUnlock4;
  Vtbl.UpdateOverlay         = SurfUpdateOverlay4;
  Vtbl.UpdateOverlayDisplay  = SurfUpdateOverlayDisplay;
  Vtbl.UpdateOverlayZOrder   = SurfUpdateOverlayZOrder4;
  Vtbl.GetDDInterface        = SurfGetDDInterface;
  Vtbl.PageLock              = SurfPageLock;
  Vtbl.PageUnlock            = SurfPageUnlock;
  Vtbl.SetSurfaceDesc        = SurfSetSurfaceDesc4;
  Vtbl.SetPrivateData        = SurfSetPrivateData;
  Vtbl.GetPrivateData        = SurfGetPrivateData;
  Vtbl.FreePrivateData       = SurfFreePrivateData;
  Vtbl.ChangeUniquenessValue = SurfChangeUniquenessValue;
  Vtbl.GetUniquenessValue    = SurfGetUniquenessValue;
  lpDraw                     = lpDirectDraw;
  lpDraw->Vtbl.AddRef(lpDraw);

  ImplicitSurface = Implicit;

  hDive                      = lpDirectDraw->GetDiveInstance();
  hDiveCC                    = lpDirectDraw->GetCCDiveInstance();
  surfaceType                = DDSCAPS_OFFSCREENPLAIN;
  memcpy((char *)&DDSurfaceDesc, (char *)lpDDSurfaceDesc, sizeof(DDSURFACEDESC2));

  if(lpDraw->dCaps.ulDepth != 15)
  {
    if(lpDraw->dCaps.ulDepth >= 8)
      dwBytesPPDive = lpDraw->dCaps.ulDepth >> 3;
    else
      dwBytesPPDive = 1; // not sure if this is the case
  }
  else
    dwBytesPPDive = 2; // 2 bytes for 15Bit

  // Setting up Cursors for handling attached Surfaces

  DPA_SurfaceMipMaps = DPA_Create(8);
  if(NULL==DPA_SurfaceMipMaps)
  {
    #ifdef DEBUG
      WriteLog("Internal : Error creating DPA for MipMaps\n");
    #endif
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  DPA_SurfaceAttached = DPA_Create(8);
  if(NULL==DPA_SurfaceAttached)
  {
    #ifdef DEBUG
      WriteLog("Internal : Error creating DPA for attached surfaces\n");
    #endif
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  DPA_LockedRects = DPA_Create(8);
  if(NULL==DPA_LockedRects)
  {
    #ifdef DEBUG
      WriteLog("Internal : Error creating DPA for Locked Rectangles\n");
    #endif
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  DPA_SurfacePrivateData = DPA_Create(8);
  if(NULL==DPA_SurfacePrivateData)
  {
    #ifdef DEBUG
      WriteLog("Internal : Error creating DPA for priva surface Data\n");
    #endif
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  if( lpDDSurfaceDesc->dwFlags & DDSD_CAPS )
  {
    // First check if we want to create a primary surface while the ddraw object already has one
    surfaceType = lpDDSurfaceDesc->ddsCaps.dwCaps;

    if( surfaceType & DDSCAPS_PRIMARYSURFACE)
    {
      #ifdef DEBUG
        WriteLog("Primary surface!\n");
      #endif

      if( lpDraw->HasPrimarySurface())
      {
        #ifdef DEBUG
          WriteLog("Primary surface already exits!\n");
        #endif
        lastError = DDERR_PRIMARYSURFACEALREADYEXISTS;
        return;
      }

      if( (lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT) ||
          (lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)  ||
          (lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT)
        )
      {
        // Dx doc says passing width,height etc. for primary surface in not permitted!!
        WriteLog("Invalid parameters\n\n");
        lastError = DDERR_INVALIDPARAMS;
        return;
      }

      // Check if OS/2 is running in the requested colormode

      diveBufNr    = DIVE_BUFFER_SCREEN;
      if( lpDraw->dCaps.ulDepth == lpDraw->GetScreenBpp() )
      {
        WriteLog("DirectScreenAccess possible\n");

        // Yes so direct access to framebuffer is possible

        pFrameBuffer = lpDraw->GetFrameBuffer();
        pDiveBuffer  = pFrameBuffer;
        dwPitchDB    = lpDraw->dCaps.ulScanLineBytes;
        dwPitchFB    = dwPitchDB;
        DDSurfaceDesc.lPitch = dwPitchDB;
      }
      else
      {
        // No so we create a virtual framebuffer which the program can access
        // and blit to the real framebuffer on Unlock to do color conversion

        WriteLog( "Need Color conversation %d => %d Bit\n",
                  lpDraw->GetScreenBpp(),
                  lpDraw->dCaps.ulDepth
                );

        dwPitchFB = (lpDraw->GetScreenWidth() * lpDraw->GetScreenBpp() +7) & ~7;
        DDSurfaceDesc.lPitch = dwPitchFB;

        // 24 byte more to enable alignment and speed up blitting

        pFBreal = (char*)malloc( lpDraw->GetScreenHeight() * dwPitchFB + 24);
        pFrameBuffer = (char*)(((int)pFBreal + 7) & ~7); // align to QWORD

        // DiveBuffer points to real framebuffer
        dwPitchDB    = lpDraw->dCaps.ulScanLineBytes;
        pDiveBuffer = lpDraw->GetFrameBuffer();

      }

      // Update passed in and local Surface description

      #ifdef DEBUG
        WriteLog("Setting up Surface\n");
      #endif
      DDSurfaceDesc.dwFlags     |= DDSD_WIDTH | DDSD_HEIGHT |
                                   DDSD_PITCH | DDSD_LPSURFACE |
                                   DDSD_PIXELFORMAT;
      DDSurfaceDesc.dwHeight     = lpDraw->GetScreenHeight();
      DDSurfaceDesc.dwWidth      = lpDraw->GetScreenWidth();
      DDSurfaceDesc.lpSurface    = pFrameBuffer;
      lpDDSurfaceDesc->dwFlags   = DDSurfaceDesc.dwFlags;
      lpDDSurfaceDesc->dwHeight  = DDSurfaceDesc.dwHeight;
      lpDDSurfaceDesc->dwWidth   = DDSurfaceDesc.dwWidth;
      lpDDSurfaceDesc->lpSurface = pFrameBuffer;
      lpDDSurfaceDesc->lPitch    = DDSurfaceDesc.lPitch;

      lpDDSurfaceDesc->ddsCaps.dwCaps |= DDSCAPS_VISIBLE;
      DDSurfaceDesc.ddsCaps.dwCaps    |= DDSCAPS_VISIBLE;
      lpDraw->SetPrimarySurface(TRUE);
      lpDDSurfaceDesc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
      lpDDSurfaceDesc->ddpfPixelFormat.dwFourCC = (DWORD) lpDraw->GetScreenFourCC();
      lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = lpDraw->GetScreenBpp();
      DDSurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
      DDSurfaceDesc.ddpfPixelFormat.dwFourCC = (DWORD) lpDraw->GetScreenFourCC();
      DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = lpDraw->GetScreenBpp();

      switch(DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
      {
        case 4:
          lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED4 | DDPF_RGB;
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED4 | DDPF_RGB;
          break;
        case 8:
          lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_FOURCC | DDPF_RGB;
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_FOURCC | DDPF_RGB;
          break;
        case 16:
          lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
          lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x0000F800;
          lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000007E0;
          lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000001F;
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
          DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x0000F800;
          DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x000007E0;
          DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x0000001F;
          break;
        case 24:
        case 32:
          lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
          lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x00FF0000;
          lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
          lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x000000FF;
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
          DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
          DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
          DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
          break;
        default:
          // Remove the Pixelformat flag
          lpDDSurfaceDesc->dwFlags &= ~DDSD_PIXELFORMAT;
          DDSurfaceDesc.dwFlags    &= ~DDSD_PIXELFORMAT;
          #ifdef DEBUG
            WriteLog("Unexpected BitDepth : %d\n",lpDraw->GetScreenBpp());
          #endif
          break;
      } // end switch

      #ifdef DEBUG
        WriteLog("Surface set up, checking other Caps\n");
      #endif

      if( DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_COMPLEX)
      {
        #ifdef DEBUG
          WriteLog("Complex Surface\n");
        #endif

        if(lpDDSurfaceDesc->dwFlags & DDSD_BACKBUFFERCOUNT)
        {
          #ifdef DEBUG
            WriteLog("Backbuffer # = %d\n",lpDDSurfaceDesc->dwBackBufferCount);
          #endif
          memset( &ComplexSurfaceDesc,
                  0,
                  sizeof(DDSURFACEDESC2));

          ComplexSurfaceDesc.dwSize  = sizeof(DDSURFACEDESC2);
          ComplexSurfaceDesc.dwFlags = DDSD_CAPS |
                                       DDSD_WIDTH |
                                       DDSD_HEIGHT |
                                       DDSD_PIXELFORMAT;
          ComplexSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;
          ComplexSurfaceDesc.dwHeight = DDSurfaceDesc.dwHeight;
          ComplexSurfaceDesc.dwWidth = DDSurfaceDesc.dwWidth;
          ComplexSurfaceDesc.ddpfPixelFormat.dwFlags = DDSurfaceDesc.ddpfPixelFormat.dwFlags;
          ComplexSurfaceDesc.ddpfPixelFormat.dwRBitMask = DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwGBitMask = DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwBBitMask = DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

          if(lpDDSurfaceDesc->dwBackBufferCount>1)
          {
            ComplexSurfaceDesc.dwFlags |=DDSD_BACKBUFFERCOUNT;
            ComplexSurfaceDesc.dwBackBufferCount = lpDDSurfaceDesc->dwBackBufferCount -1;
            ComplexSurfaceDesc.ddsCaps.dwCaps|= DDSCAPS_COMPLEX;
          }

          BackBuffer = new OS2IDirectDrawSurface(lpDraw, &ComplexSurfaceDesc, TRUE, TRUE);

          if (BackBuffer->GetLastError()==DD_OK)
          {
            WriteLog("created backbuffer");
            // Our Primary Buffer is also the frontbuffer of a flipchain
            DDSurfaceDesc.ddsCaps.dwCaps    |= DDSCAPS_FRONTBUFFER | DDSCAPS_FLIP;
            lpDDSurfaceDesc->ddsCaps.dwCaps |= DDSCAPS_FRONTBUFFER | DDSCAPS_FLIP;
            BackBuffer->SetFrontBuffer(this);
          }
          else
          {
            WriteLog("Error creating backbuffer");
          }
        }
        else
        {
          #ifdef DEBUG
            WriteLog("Unsupported Complex Surface\n");
            _dump_DDSCAPS(lpDDSurfaceDesc->dwFlags);
          #endif
          lastError = DDERR_OUTOFMEMORY;
          return;
        } //endif Backbuffer
      } // endif DDSCAPS_COMPLEX

      width  = DDSurfaceDesc.dwWidth;
      height = DDSurfaceDesc.dwHeight;

      lpDraw->pPrimSurf = this;

      lastError = DD_OK;
      return;
    } // endif  DDSCAPS_PRIMARYSURFACE

    //
    // ToDo : Do better calulation of Bitmap Size to support the compressed Bitmaps in Dx6
    //

    if( (DDSurfaceDesc.dwFlags & DDSD_HEIGHT) &&
        (DDSurfaceDesc.dwFlags & DDSD_WIDTH)
      )

    {
      DWORD dwBpp;
      DWORD dwCaps;
      WriteLog( " Requested Size %dx%d\n",
                DDSurfaceDesc.dwWidth,
                DDSurfaceDesc.dwHeight);

      if(DDSurfaceDesc.dwFlags & DDSD_PIXELFORMAT)         // Pixelformat passed in ?
      {
        WriteLog(" Pixelformat requested :");
        // YES use it
        if(DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_RGB)
        {
          dwBpp = DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
        }
        else
        {
          if(DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
            dwBpp = 8;
          if(DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
            dwBpp = 4;
        }
      }
      else
      {
        WriteLog(" Use Screen Format :");
        dwBpp = lpDraw->GetScreenBpp(); // No use Screenformat
        lpDDSurfaceDesc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        lpDDSurfaceDesc->ddpfPixelFormat.dwFourCC = (DWORD) lpDraw->GetScreenFourCC();
        lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = dwBpp;
        DDSurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        DDSurfaceDesc.ddpfPixelFormat.dwFourCC = (DWORD) lpDraw->GetScreenFourCC();
        DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = dwBpp;
        switch(DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
        {
          case 4:
            lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED4 | DDPF_RGB;
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED4 | DDPF_RGB;
            break;
          case 8:
            lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_FOURCC | DDPF_RGB;
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_FOURCC | DDPF_RGB;
            break;
          case 16:
            lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
            lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x0000F800;
            lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000007E0;
            lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000001F;
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
            DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x0000F800;
            DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x000007E0;
            DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x0000001F;
            break;
          case 24:
          case 32:
            lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
            lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x00FF0000;
            lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
            lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x000000FF;
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
            DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
            DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
            DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
            break;
          default:
            // Remove the Pixelformat flag
            lpDDSurfaceDesc->dwFlags &= ~DDSD_PIXELFORMAT;
            DDSurfaceDesc.dwFlags    &= ~DDSD_PIXELFORMAT;
            #ifdef DEBUG
              WriteLog("Unexpected BitDepth : %d\n",lpDraw->GetScreenBpp());
            #endif
            break;
        } // end switch

      }

      WriteLog("%d Bits\n",dwBpp);

      // three possible situaltions
      // 1. User supplied pointer to surface -> use it
      // 2. Delayed allocation of a texture -> don't alloc
      // 3. Normal allocation
      // After this check for complex flag.

      dwCaps = DDSurfaceDesc.ddsCaps.dwCaps;

      if(DDSurfaceDesc.dwFlags & DDSD_LPSURFACE)
      {
        // 1.

        WriteLog(" Usersupplied Surface\n");

        if(NULL==DDSurfaceDesc.lpSurface)
        {
          // pointer is NULL! Stupid user ;)
          lastError = DDERR_INVALIDPARAMS;
          return;
        }

        // User allocated the Buffer for the surface so we don't have to

        Updated = TRUE; // Set Flag to indicate User supplied buffer so we don't free it

        // As we allready copied the surface description we are done if the # of colors
        // of the surface is the same then the screen

        pFrameBuffer = (char*)DDSurfaceDesc.lpSurface;
        diveBufNr    = -1;
        dwPitchFB    = DDSurfaceDesc.lPitch;

        if( lpDraw->dCaps.ulDepth  == dwBpp )
        {
          // Yes No Colorconversion is needed so point to the same buffer
          dwPitchDB    = dwPitchFB;
          pDiveBuffer  = pFrameBuffer;
        }
        else
        {
          // No so we must create the Divebuffer to do the color conversion
          // and blit to the real framebuffer on Unlock to do color conversion

          dwPitchDB = (lpDDSurfaceDesc->dwWidth * dwBytesPPDive +7) & ~7;

          // 24 byte more to enable alignment and speed up blitting

          pDBreal = (char*)malloc( lpDDSurfaceDesc->dwHeight * dwPitchDB + 24);
          pDiveBuffer = (char*)(((int)pDBreal + 7) & ~7); // align to QWORD

          // Not sure if that is ok but we need this for translation
          // I hope no game uses YUV or such a crap

          DDSurfaceDesc.ddpfPixelFormat.dwFourCC = SupportedFourCCs[dwBpp>>3];
        }

      }
      else
      {

        if(dwCaps & DDSCAPS_ALLOCONLOAD)
        {
          // 2.

          WriteLog(" Alloc on Load Texture?!\n");

          dwCaps &= ~DDSCAPS_ALLOCONLOAD; // remove flag

          // only allow this flag for textures
          if(!dwCaps & DDSCAPS_TEXTURE)
          {
            lastError = DDERR_INVALIDPARAMS;
            return;
          }

          dwCaps &= ~DDSCAPS_TEXTURE; // remove flag
          pFrameBuffer = NULL;
          pDiveBuffer  = NULL;

          // This surface isn't allocated yet, but when the texture is loaded
          #ifdef DEBUG
            WriteLog("Warning : Delayed memory allocation on request\n");
          #endif
          DDSurfaceDesc.lpSurface = NULL;
          lpDDSurfaceDesc->lpSurface = NULL;
        }
        else
        {
          // 3.

          WriteLog(" Alloc now!\n");

          lpDDSurfaceDesc->dwFlags  |= DDSD_PITCH|DDSD_LPSURFACE;
          DDSurfaceDesc.dwFlags      = lpDDSurfaceDesc->dwFlags;

          dwPitchFB = lpDDSurfaceDesc->dwWidth * (dwBpp<8?1:dwBpp/8);
          dwPitchFB = (dwPitchFB +7) & ~7;  // Align on QWords
          DDSurfaceDesc.lPitch    = dwPitchFB;
          lpDDSurfaceDesc->lPitch = dwPitchFB;

          #ifdef DEBUG
            if(dwBpp<8)
            {
              WriteLog("1 or 4 Bit Surface encountered may not work !");
            }
          #endif


          // 24 byte more to enable alignment and speed up blitting

          pFBreal = (char*)malloc( lpDDSurfaceDesc->dwHeight * dwPitchFB + 24);

          if(NULL==pFBreal)
          {
            lastError = DDERR_OUTOFMEMORY;
            return;
          }

          pFrameBuffer = (char*)(((int)pFBreal + 7) & ~7); // align to QWORD

          WriteLog( " Framebuffer @ %08X QWAligned @ %08X with a Pitch of %d\n",
                    pFBreal, pFrameBuffer, dwPitchFB);

          lpDDSurfaceDesc->lpSurface = pFrameBuffer;
          DDSurfaceDesc.lpSurface    = pFrameBuffer;


          if( (lpDraw->dCaps.ulDepth ) == dwBpp )
          {
            WriteLog(" No CC_Buffer needed\n");
            // Yes => No Colorconversion is needed so point to the same buffer
            pDiveBuffer  = pFrameBuffer;
          }
          else
          {
            WriteLog(" Alloc CCBuf ");
            dwPitchDB = (lpDDSurfaceDesc->dwWidth * (lpDraw->dCaps.ulDepth/8) +7) & ~7;
            if(Mainchain)
            {
              WriteLog("with DIVE\n");
              // This surface is part of flipchain with the primary surface use dive to assoc memory
              pDiveBuffer = (char*)malloc( lpDDSurfaceDesc->dwHeight *
                                           dwPitchDB);
              diveBufNr = 0;
              rc = DiveAllocImageBuffer( hDive,
                                         &diveBufNr,
                                         lpDraw->dCaps.fccColorEncoding,
                                         lpDDSurfaceDesc->dwWidth,
                                         lpDDSurfaceDesc->dwHeight,
                                         dwPitchDB,
                                         (PBYTE)pDiveBuffer);
               WriteLog("rc = 0x%08X\n",rc);
            }
            else
            {
              WriteLog( "with malloc (%dx%d) Pitch %d ",
                        lpDDSurfaceDesc->dwHeight,
                        lpDDSurfaceDesc->dwWidth,
                        dwPitchDB);
              // No so we must create the Divebuffer to do the colortranslation
              // and blit to the real framebuffer on Unlock to do color conversion
              pDiveBuffer = (char*)malloc( lpDDSurfaceDesc->dwHeight *
                                           dwPitchDB);
            }
            WriteLog( "  @ %08X\n", pDiveBuffer);
          }

        }  // end of 3rd case
      } // End of alloc surfaces

      width  = DDSurfaceDesc.dwWidth;
      height = DDSurfaceDesc.dwHeight;

      if( dwCaps & DDSCAPS_COMPLEX)
      {
        // remove the flag
        dwCaps &= ~DDSCAPS_COMPLEX;
        #ifdef DEBUG
          WriteLog("Complex Surface\n");
        #endif

        if(lpDDSurfaceDesc->dwFlags & DDSD_BACKBUFFERCOUNT)
        {
          lpDDSurfaceDesc->dwFlags &= ~DDSD_BACKBUFFERCOUNT;
          #ifdef DEBUG
            WriteLog("Backbuffer # = %d\n",lpDDSurfaceDesc->dwBackBufferCount);
          #endif
          memcpy(&ComplexSurfaceDesc,lpDDSurfaceDesc,sizeof(DDSURFACEDESC2));
          ComplexSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FLIP;        // set flip
          ComplexSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_BACKBUFFER; // remove backbuffer

          if(ComplexSurfaceDesc.dwBackBufferCount>1)
          {
            ComplexSurfaceDesc.dwBackBufferCount--;
          }
          else
          {
            ComplexSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
            ComplexSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_COMPLEX;
          }

          BackBuffer = new OS2IDirectDrawSurface(lpDraw, &ComplexSurfaceDesc, TRUE, Mainchain);
          if (BackBuffer->GetLastError()==DD_OK)
          {
            DDSurfaceDesc.dwFlags |= DDSCAPS_FLIP;
            BackBuffer->SetFrontBuffer(this);
          }

        }

        // MipMap Surfaces are handled here
        if( (lpDDSurfaceDesc->dwFlags & DDSD_MIPMAPCOUNT) &&
            (dwCaps & DDSCAPS_TEXTURE) &&
            (dwCaps & DDSCAPS_MIPMAP) )
        {
          dwCaps &= ~ (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP);

          lpDDSurfaceDesc->dwFlags &= ~DDSD_MIPMAPCOUNT;
          #ifdef DEBUG
            WriteLog("Mipmpa # = %d\n",lpDDSurfaceDesc->dwMipMapCount);
          #endif
          memcpy(&ComplexSurfaceDesc,lpDDSurfaceDesc,sizeof(DDSURFACEDESC2));
          ComplexSurfaceDesc.dwMipMapCount = 0;

          for(int i =0; i < lpDDSurfaceDesc->dwMipMapCount; i++)
          {
            #ifdef DEBUG
              WriteLog("Creating MipMap %d\n",i);
            #endif
            // Mpmaps shirnk by 2
            ComplexSurfaceDesc.dwWidth  /= 2;
            ComplexSurfaceDesc.dwHeight /= 2;

            MipMapSurface = new OS2IDirectDrawSurface(lpDraw, &ComplexSurfaceDesc, TRUE);

            DPA_InsertPtr( DPA_SurfaceMipMaps,
                           DPA_GetPtrCount(DPA_SurfaceMipMaps),
                           MipMapSurface);

            if(MipMapSurface->GetLastError() != DD_OK)
            {
              lastError = MipMapSurface->GetLastError();
              #ifdef DEBUG
                WriteLog("Attached surface creation returned error %d\n",lastError);
              #endif
              return;
            } // Endif Errorcheck
          } //End for(i =0; i < lpDDSurfaceDesc->dwMipMapCount; i++)
        } // End of MipMaps

        #ifdef DEBUG
          if(lpDDSurfaceDesc->dwFlags)
            WriteLog("Unsupported Complex Surface\n");
        #endif
      } // Endif DDSCAPS_COMPLEX
    }
    else
    {
      lastError = DDERR_INVALIDPARAMS;
    }
  } // Endif DDCAPS is valid
  else
  {
    WriteLog("CAPS not valid\n");
    lastError = DDERR_INVALIDPARAMS;
  }
  #ifdef DEBUG
    WriteLog("Buf %X Screen Caps (%d,%d), bitcount %d\n\n", this, lpDraw->GetScreenHeight(), lpDraw->GetScreenWidth(),
    lpDraw->dCaps.ulDepth);
    if(DD_OK!=lastError)
    {
      WriteLog("Some Error Check Flags\n");
      _dump_DDSCAPS(lpDDSurfaceDesc->dwFlags);
    }
  #endif
}
//******************************************************************************
//******************************************************************************
HRESULT OS2IDirectDrawSurface::ColorFill(LPRECT lpDestRect,DWORD dwFillColor)
{

  int i,j, FillWidth, FillHeight;
  char *pLine, *pFillPos;

  DWORD *pColor, dwColor,y;
  DWORD *pPal24;
  WORD  *pPal16;
  #ifdef DEBUG
    WriteLog("ColorFill with %08X\n", dwFillColor);
  #endif

  if(NULL!=lpDestRect)
  {
    #ifdef DEBUG
      WriteLog("Fill only Rect(%d,%d)(%d,%d)\n", lpDestRect->left, lpDestRect->top,
               lpDestRect->right, lpDestRect->bottom);
    #endif
    FillWidth  = lpDestRect->right - lpDestRect->left;
    FillHeight = lpDestRect->bottom - lpDestRect->top -1;
    pLine = pDiveBuffer +
            (lpDestRect->top*dwPitchDB) +
            (lpDestRect->left*dwBytesPPDive);
  }
  else
  {
    WriteLog("Fill all at addr ");
    FillWidth  = width;
    FillHeight = height -1;
    pLine = pDiveBuffer;
  }
  WriteLog("0x%08X (%d/%d) at\n", pLine,FillWidth,FillHeight);

  if(pDiveBuffer!=pFrameBuffer)
  {
    if( (NULL== lpDraw->pPrimSurf) ||
        (NULL== ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette) )
    {
      if(!fPalInit)
      {
        for(DWORD i=0;i<256;i++)
        {
          wDefaultPalete16[i] = (DefaultPalette[i*3+2]>>3) +
                                ((DefaultPalette[i*3+1]>>2) <<5) +
                                ((DefaultPalette[i*3]>>3) << 11);
          WriteLog( " # %d : RGB=%02X/%02X/%02X => %04X\n",
                    i,
                    DefaultPalette[i*3],
                    DefaultPalette[i*3+1],
                    DefaultPalette[i*3+2],
                    wDefaultPalete16[i]);
         // aPal24[i] = (lpColorTable[i].peBlue <<8) +
         //             (lpColorTable[i].peGreen<<16) +
         //             (lpColorTable[i].peRed<<24);
        }
        fPalInit = TRUE;
      }
      pPal16 = &wDefaultPalete16[0];
    }
    else
    {
      pPal16 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal16;
      pPal24 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal24;
    }
  }

  switch(dwBytesPPDive)
  {
    case 1:
      WriteLog("8 Bit\n");
      dwColor = (dwFillColor<<24) + (dwFillColor<<16) +
                (dwFillColor<<8)  + (dwFillColor);
      for(i=0,pColor = (DWORD*)pLine;i<(FillWidth/4);i++)
        pColor[i] = dwColor;
      if(FillWidth % 4)
      {
         pFillPos = (char*) (&pColor[i-1]);
        for(i=0;i<FillWidth % 4;i++)
           pFillPos[i] = (UCHAR) dwColor;
      }
      break;
    case 2:
      WriteLog("16 Bit\n");
      if(pDiveBuffer!=pFrameBuffer)
      {
        if(8==lpDraw->GetScreenBpp())
          dwFillColor = pPal16[dwFillColor];
      }
      dwColor = (dwFillColor<<16) + (dwFillColor);
      WriteLog("Fill with 0x%08X\n",dwColor);
      for(i=0,pColor = (DWORD*)pLine;i<(FillWidth/2);i++)
        pColor[i] = dwColor;
      if(FillWidth % 2)
      {
         pFillPos = (char*)(&pColor[i-1]);
        *((USHORT*)pFillPos) = (USHORT)dwColor;
      }
      break;
    case 3:
      WriteLog("24 Bit\n");

      dwColor = (dwFillColor<<8);
      for(i=0 ; i<FillWidth ; i++)
      {
         char* pColor = (char*)pLine+(i*3);
        *pColor = dwColor;
      }
      break;
    case 4:
      WriteLog("32 Bit\n");
      dwColor = dwFillColor;
      for(i=0,pColor = (DWORD*)pLine;i<FillWidth;i++)
        pColor[i] = dwColor;
      break;
    default:
      #ifdef DEBUG
        WriteLog("Unexpected Bitdepth\n");
      #endif
      return DDERR_GENERIC;
  } // end switch(dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)


  pFillPos = pLine + dwPitchDB;
  FillWidth = FillWidth*dwBytesPPDive;;
  for( y=0;y<FillHeight;y++,pFillPos+=dwPitchDB)
  {
    #ifdef USE_ASM
      // ToDo get the loop into an asm function as well to speed up filling
      // maybe remove the creation of the first fill line in an all asm
      // function and use MMX regs to set 8 bytes
      MemFlip(pFillPos,pLine,FillWidth);
    #else
      memcpy(pFillPos,pLine,FillWidth);
    #endif
  }

  return(DD_OK);

#if 0 // rest of old code for resuse/move to a function to get closest syscolor
      // for colorconversion modes color fills
  else
  {
    // Yeah! Color conversion needed ;)
    #ifdef DEBUG
       WriteLog("Color converion ! Urks, may not work \n");
    #endif

    switch(dest->lpDraw->dCaps.ulDepth)
    {
      case 8:  // 256 Mode bad thing as programm wants to run in a higher mode might look ugly
        GetSystemPaletteEntries(GetDC(HWND_DESKTOP),0,255,&SysPal[1]);
        pLogPal->palVersion = 0;
        pLogPal->palNumEntries = 256;

        hPal = CreatePalette(pLogPal);
        if(hPal!=NULL)
        {
          if(dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount==16)
          {
            ulColor = ((lpDDBltFx->dwFillColor & 0x0000001F) <<3) +
                      ((lpDDBltFx->dwFillColor & 0x000007E0) <<5) +
                      ((lpDDBltFx->dwFillColor & 0x0000F800) <<8);
          }
          else
            ulColor = lpDDBltFx->dwFillColor;

          ulColor = GetNearestPaletteIndex(hPal,ulColor);
          ulColor = (ulColor & 0x000000FF) + ((ulColor & 0x000000FF) << 8) +
                    ((ulColor & 0x000000FF) << 16) + ((ulColor & 0x000000FF) << 24);
        }
        else
        {
          #ifdef DEBUG
            WriteLog("Error creating Palette default to 0");
          #endif
          ulColor = 0;
        }
        for(i=0,(char*)pColor = ScanLine;i<(FillWidth/4)+1;i++)
           *(pColor+i) = ulColor;
        break;
      case 16:
        if(dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount==8)
        {
          if(lpDDBltFx->dwFillColor > 255)
            return DDERR_INVALIDPARAMS;

          if(dest->lpPalette!=NULL)
            dest->lpPalette->lpVtbl->GetEntries(dest->lpPalette,0,0,256,(LPPALETTEENTRY)&SysPal);
          else
            GetSystemPaletteEntries(GetDC(HWND_DESKTOP),0,255,(PPALETTEENTRY)&SysPal);

          SysPal[lpDDBltFx->dwFillColor].peBlue  >>3;
          SysPal[lpDDBltFx->dwFillColor].peGreen >>2;
          SysPal[lpDDBltFx->dwFillColor].peRed   >>3;
          ulColor = ULONG(SysPal[lpDDBltFx->dwFillColor].peRed) << 11 +
                    ULONG(SysPal[lpDDBltFx->dwFillColor].peGreen) << 5 +
                    ULONG(SysPal[lpDDBltFx->dwFillColor].peBlue);
        }
        else
        {
          ulColor = (lpDDBltFx->dwFillColor & 0x000000FF)>>3 +
                    (((lpDDBltFx->dwFillColor & 0x0000FF00)>>5) & 0x000007E0) +
                    (((lpDDBltFx->dwFillColor & 0x00FF0000)>>8) & 0x0000F800);
        }
        ulColor += ulColor << 16;
        for(i=0,(char*)pColor = ScanLine;i<(FillWidth/2)+1;i++)
          *(pColor+i) = ulColor;
        break;
      case 24:
        if(dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount==8)
        {
          if(lpDDBltFx->dwFillColor > 255)
            return DDERR_INVALIDPARAMS;

          if(dest->lpPalette!=NULL)
            dest->lpPalette->lpVtbl->GetEntries(dest->lpPalette,0,0,256,(LPPALETTEENTRY)&SysPal);
          else
            GetSystemPaletteEntries(GetDC(HWND_DESKTOP),0,255,(PPALETTEENTRY)&SysPal);

          ulColor = ULONG(SysPal[lpDDBltFx->dwFillColor].peRed) <<24 +
                    ULONG(SysPal[lpDDBltFx->dwFillColor].peGreen) <<16 +
                    ULONG(SysPal[lpDDBltFx->dwFillColor].peBlue) <<8;
        }
        else
        {
          if(dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount==16)
          {
            ulColor = ((lpDDBltFx->dwFillColor & 0x0000001F) <<11) +
                       ((lpDDBltFx->dwFillColor & 0x000007E0) <<13) +
                       ((lpDDBltFx->dwFillColor & 0x0000F800) <<18);
          }
          else
            ulColor = lpDDBltFx->dwFillColor << 8 ;
        }

        for(i=0;i<FillWidth;i++)
        {
          char* pColor = (char*)Scanline+(i*3);
          *pColor = ulColor;
        }
        break;
      case 32:
        if(dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount==8)
        {
          if(lpDDBltFx->dwFillColor > 255)
            return DDERR_INVALIDPARAMS;

          if(dest->lpPalette!=NULL)
            dest->lpPalette->lpVtbl->GetEntries(dest->lpPalette,0,0,256,(LPPALETTEENTRY)&SysPal);
          else
            GetSystemPaletteEntries(GetDC(HWND_DESKTOP),0,255,(PPALETTEENTRY)&SysPal);

          ulColor = ULONG(SysPal[lpDDBltFx->dwFillColor].peRed) <<16 +
                    ULONG(SysPal[lpDDBltFx->dwFillColor].peGreen) <<8 +
                    ULONG(SysPal[lpDDBltFx->dwFillColor].peBlue);
        }
        else
        {
          if(dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount==16)
          {
            ulColor = ((lpDDBltFx->dwFillColor & 0x0000001F) <<3) +
                      ((lpDDBltFx->dwFillColor & 0x000007E0) <<5) +
                      ((lpDDBltFx->dwFillColor & 0x0000F800) <<8);
          }
          else
            ulColor = lpDDBltFx->dwFillColor & 0x00FFFFFF;
        }

        for(i=0,(char*)pColor = ScanLine;i<(FillWidth/4);i++)
          *(pColor+i) = ulColor;
        break;
      default:
        #ifdef DEBUG
          WriteLog("Unexpected Bitdepth\n");
        #endif
        return DDERR_GENERIC;
    } // end switch (dest->lpDraw->dCaps.ulDepth)
  }// end of Scanline setup
#endif

}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawSurface::ColorConversion(LPRECT lpRect)
{
  char *pSrc,*pDst,*pSLine,*pDLine;
  DWORD CCwidth,CCheight,x,y;
  DWORD *pPal24;
  WORD  *pPal16;

  if( (NULL== lpDraw->pPrimSurf) ||
      (NULL== ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette) )
  {
    if(!fPalInit)
    {
      for(DWORD i=0;i<256;i++)
      {
          wDefaultPalete16[i] = (DefaultPalette[i*3+2]>>3) +
                                ((DefaultPalette[i*3+1]>>2) <<5) +
                                ((DefaultPalette[i*3]>>3) << 11);
        WriteLog( " # %d : RGB=%02X/%02X/%02X => %04X\n",
                  i,
                  DefaultPalette[i*3],
                  DefaultPalette[i*3+1],
                  DefaultPalette[i*3+2],
                  wDefaultPalete16[i]);
       // aPal24[i] = (lpColorTable[i].peBlue <<8) +
       //             (lpColorTable[i].peGreen<<16) +
       //             (lpColorTable[i].peRed<<24);
      }
      fPalInit = TRUE;
    }
    pPal16 = &wDefaultPalete16[0];
  }
  else
  {
    pPal16 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal16;
    pPal24 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal24;
  }

  if(NULL==lpRect)
  {
    pSrc = pFrameBuffer;
    pDst = pDiveBuffer;
    CCwidth  = width;
    CCheight = height;
  }
  else
  {
    // ToDo: Check why the top/bottom values come in swaped here
    // for now simply reverse them with the following 3 lines
    y = lpRect->top;
    lpRect->top = lpRect->bottom;
    lpRect->bottom = y;
    // end of hack
    pSrc = pFrameBuffer +
           (lpRect->top * dwPitchFB) +
           (lpRect->left* (lpDraw->dCaps.ulDepth/8));
    pDst = pDiveBuffer +
           (lpRect->top * dwPitchDB) +
           (lpRect->left* dwBytesPPDive);
    CCwidth  = lpRect->right - lpRect->left;
    CCheight = lpRect->bottom - lpRect->top;

  }

  WriteLog( "H: %d W: %d\n SRC @ %08X\n DST @ %08X\n",
            CCheight, CCwidth, pSrc,pDst);

  pSLine = pSrc;
  pDLine = pDst;

  switch(lpDraw->dCaps.ulDepth)
  {
    case 8:
      WriteLog(" 8Bit target CC not implemented\n");
      break;
    case 15:
      WriteLog(" 15 Bit not implemented using 16bit might look ugly\n");
    case 16:
      if(8==lpDraw->GetScreenBpp())
      {
        WriteLog("8->16Bit CC\n");
        for(y=0;CCheight;CCheight--,y++)
        {
          for(x=0;x<width;x++)
          {
            *((WORD*)pDLine) = pPal16[pSLine[x]];
          }
          pSLine += dwPitchFB;
          pDLine += dwPitchDB;
        }
      }
      else
      {
        WriteLog("%d ->16Bit Not implemented",lpDraw->GetScreenBpp());
      }
      break;
    case 24:
      if(8==lpDraw->GetScreenBpp())
      {
        WriteLog("8->24Bit CC");
      }
      else
      {
        WriteLog("%d ->24Bit Not implemented",lpDraw->GetScreenBpp());
      }
      break;
    case 32:
      if(8==lpDraw->GetScreenBpp())
      {
        WriteLog("8->32Bit CC");
      }
      else
      {
        WriteLog("%d ->32Bit Not implemented",lpDraw->GetScreenBpp());
      }
      break;
    default:
      #ifdef DEBUG
        WriteLog( "Unexpected Screen Bitdepth %d\n",
                  lpDraw->dCaps.ulDepth);
      #endif
      break;
  }

/*
  SETUP_BLITTER sBlt;
  ULONG ulDN1, ulDN2;
  ULONG rc;
  ulDN1 = ulDN2 = 0;

  memset(&sBlt,0,sizeof(sBlt));
  sBlt.ulStructLen = sizeof(sBlt);
  sBlt.fccSrcColorFormat = (FOURCC) DDSurfaceDesc.ddpfPixelFormat.dwFourCC;
  if (NULL!=lpRect)
  {
    sBlt.ulSrcWidth        = lpRect->right - lpRect->left;
    sBlt.ulSrcHeight       = lpRect->top   - lpRect->bottom;
    sBlt.ulSrcPosX         = lpRect->left;
    sBlt.ulSrcPosY         = height - lpRect->top;
  }
  else
  {
    sBlt.ulSrcWidth        = width;
    sBlt.ulSrcHeight       = height;
    sBlt.ulSrcPosX         = 0;
    sBlt.ulSrcPosY         = 0;
  }
  sBlt.fccDstColorFormat = FOURCC_SCRN;
  sBlt.ulDstWidth        = sBlt.ulSrcWidth;
  sBlt.ulDstHeight       = sBlt.ulSrcHeight;
  sBlt.lDstPosX          = sBlt.ulSrcPosX;
  sBlt.lDstPosY          = sBlt.ulSrcPosY;
  sBlt.ulNumDstRects     = DIVE_FULLY_VISIBLE;

  WriteLog( "Colorconversion:\n FCC SRC %08X\n FCC DST %08X\n",
            sBlt.fccSrcColorFormat,
            sBlt.fccDstColorFormat );

  rc = DiveAllocImageBuffer( hDiveCC,
                             &ulDN1,
                             sBlt.fccSrcColorFormat,
                             width,
                             height,
                             dwPitchFB,
                             (PBYTE)pFrameBuffer);
  WriteLog("AllocDiveSrc Buffer rc= 0x%08X\n",rc);

  rc = DiveAllocImageBuffer( hDiveCC,
                             &ulDN2,
                             sBlt.fccDstColorFormat,
                             width,
                             height,
                             dwPitchDB,
                             (PBYTE)pDiveBuffer);
  WriteLog("AllocDiveDst Buffer rc= 0x%08X\n",rc);

  rc = DiveSetupBlitter( hDiveCC,
                         &sBlt);
  WriteLog("SetupBlitter rc= %X\n",rc);

  rc = DiveBlitImage( hDiveCC,
                      ulDN1,
                      ulDN2);

  WriteLog("Blit rc= %X\n",rc);

  rc = DiveFreeImageBuffer( hDiveCC,
                            ulDN1);
  WriteLog("Free Src rc= %X\n",rc);

  rc = DiveFreeImageBuffer( hDiveCC,
                            ulDN2);

  WriteLog("Free dst rc= %X\n",rc);
*/
}
//******************************************************************************
//******************************************************************************
// Internal callbacks uses by destructor
int  DestroyRects(LPVOID lpItem, DWORD dwRes)
{
  DDRectangle *pItem = (DDRectangle*) lpItem;
  delete(pItem);
  return 1;
}

int ReleaseSurfaces(LPVOID lpItem, DWORD dwRes)
{
  OS2IDirectDrawSurface *pSurf;
  pSurf = (OS2IDirectDrawSurface *)lpItem;
  pSurf->Vtbl.Release(pSurf);
  return 1;
}
//******************************************************************************
//******************************************************************************

OS2IDirectDrawSurface::~OS2IDirectDrawSurface()
{
  OS2IDirectDrawSurface  *AttachedSurface;


  if(DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
  {
    lpDraw->SetPrimarySurface(FALSE);
    if(lpPalette)
      lpPalette->RestorePhysPalette();
  }
  else
  {
    if( diveBufNr != -1)
    {
      if(fLocked)
        DiveEndImageBufferAccess(hDive, diveBufNr);
      DiveFreeImageBuffer(hDive, diveBufNr);
    }
    else
    {
      // Memory Surfaces
      if (!Updated)  // check for user handled buffer
        free(DDSurfaceDesc.lpSurface);
    }


    fLocked = FALSE;
    diveBufNr = -1;
  }

  // Free the translation buffer
  if(pFrameBuffer != pDiveBuffer)
    free(pDiveBuffer);

  // Clear the list of locked rectangles
  if (DPA_GetPtrCount(DPA_LockedRects)>0)
  {
    DPA_DestroyCallback( DPA_LockedRects,
                         (DPAENUMPROC)DestroyRects,
                         0);
  }

  if(lpClipper)
  {
    lpClipper->Vtbl.Release((IDirectDrawClipper*)lpClipper);
    lpClipper = NULL;
  }

  if(lpPalette)
  {
    lpPalette->Vtbl.Release((IDirectDrawPalette*)lpPalette);
    lpPalette = NULL;
  }

  if(hbmImage)
    DeleteObject(hbmImage);

  if(hdcImage)
    DeleteDC(hdcImage);

  if (NULL!=BackBuffer)
    BackBuffer->Vtbl.Release(AttachedSurface);

  if (DPA_GetPtrCount(DPA_SurfaceMipMaps)>0)
  {
    DPA_DestroyCallback( DPA_SurfaceMipMaps,
                         (DPAENUMPROC)ReleaseSurfaces,
                         0);
  }

  if (DPA_GetPtrCount(DPA_SurfaceAttached)>0)
  {
    DPA_DestroyCallback( DPA_SurfaceAttached,
                         (DPAENUMPROC)ReleaseSurfaces,
                         0);
  }

  lpDraw->Vtbl.Release(lpDraw);

}
//******************************************************************************
//******************************************************************************
inline  void OS2IDirectDrawSurface::SetFrontBuffer( OS2IDirectDrawSurface* NewFBuffer)
{
  FrontBuffer = NewFBuffer;
  if (NULL==NewFBuffer)
  {

    WriteLog("Remove Frontbuffer\n");

    // The real Frontbuffer was removed check if I'm now the one
    if(NULL!=BackBuffer)
    {
      WriteLog("We have the a backbuffer so we're the Frontbuffer\n");
      NextFlip = BackBuffer;
      DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_BACKBUFFER;
      DDSurfaceDesc.ddsCaps.dwCaps |=  DDSCAPS_FRONTBUFFER;
      BackBuffer->SetFrontBuffer(this);
    }
    else
    {
      // Flipchain is destroyed
      WriteLog("No longer part of a flipchain\n");
      DDSurfaceDesc.ddsCaps.dwCaps &= ~(DDSCAPS_BACKBUFFER | DDSCAPS_FLIP);
    }
  }
  else
  {
    WriteLog("Set new Frontbuffer to 0x%08X\n",NewFBuffer);
    if(NULL==NewFBuffer->GetFrontBuffer())
    {
      WriteLog("We're the 1st backbuffer\n");
      DDSurfaceDesc.ddsCaps.dwCaps |=  DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;
      FrontBuffer->NextFlip = this;
    }
    else
    {
      WriteLog("We are oe of many buffers\n");
      DDSurfaceDesc.ddsCaps.dwCaps &=  ~DDSCAPS_BACKBUFFER ;
      DDSurfaceDesc.ddsCaps.dwCaps |=  DDSCAPS_FLIP;
    }

    if(NULL!=BackBuffer)
      BackBuffer->SetFrontBuffer(this);

    DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FRONTBUFFER;
  }
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
  // ToDo: Add Interface handling for D3D Textures

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawSurface::SurfQueryInterface\n");
  #endif

  *ppvObj = NULL;

  if(!IsEqualGUID(riid, IID_IDirectDrawSurface) &&
     !IsEqualGUID(riid, IID_IDirectDrawSurface2))
//&& !IsEqualGUID(riid, IID_IUnknown))
    return E_NOINTERFACE;

  *ppvObj = This;

  SurfAddRef(This);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall SurfAddRef(THIS This)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawSurface::SurfAddRef %d\n", me->Referenced+1);
  #endif

  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall SurfRelease(THIS This)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawSurface::SurfRelease %d\n", me->Referenced-1);
    WriteLog("OS2IDirectDrawSurface::Surface %X\n", me);
  #endif
  if(me->Referenced)
  {
    me->Referenced--;
    if(me->Referenced == 0)
    {
      delete( me);
      #ifndef __WATCOMC__
        //_interrupt(3);
      #endif
      return(0);
    }
    else
      return me->Referenced;
  }
  else
    return(0);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfAddAttachedSurface(THIS This, LPDIRECTDRAWSURFACE2 lpDDSurface)
{

  #ifdef DEBUG
    WriteLog("SurfAddAttachedSurface\n");
  #endif
  return SurfAddAttachedSurface4(This, (LPDIRECTDRAWSURFACE4)lpDDSurface);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfAddAttachedSurface4(THIS This, LPDIRECTDRAWSURFACE4 lpDDSurface)
{
  OS2IDirectDrawSurface *AttachedSurface;
  OS2IDirectDrawSurface *BBCursor;
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int rc;

  #ifdef DEBUG
    WriteLog("SurfAddAttachedSurface4\n");
  #endif

  if (NULL==lpDDSurface)
    return DDERR_INVALIDPARAMS;

  AttachedSurface = (OS2IDirectDrawSurface*) lpDDSurface;

  if(AttachedSurface->IsImplicitSurface())
  {
    #ifdef DEBUG
     WriteLog("Internal : Can't attach an implicit created surface to an other surface\n");
    #endif
    return(DDERR_CANNOTATTACHSURFACE);
  }

  if(This == AttachedSurface)
  {
    #ifdef DEBUG
     WriteLog("Can't attach an surface to itself\n");
    #endif
    return(DDERR_CANNOTATTACHSURFACE);
  }

  if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_TEXTURE)
  {
    if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
    {
      if(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
      {
          rc = DPA_InsertPtr( me->DPA_SurfaceMipMaps,
                              DPA_GetPtrCount(me->DPA_SurfaceMipMaps),
                              AttachedSurface);

        if( rc>=0)
        {
          me->DDSurfaceDesc.dwFlags |= DDSD_MIPMAPCOUNT;
          me->DDSurfaceDesc.dwMipMapCount++;

          AttachedSurface->Vtbl.AddRef(AttachedSurface);
        }
        else
        {
          #ifdef DEBUG
            WriteLog("Internal : Error attaching to MipMap\n");
          #endif
          return(DDERR_CANNOTATTACHSURFACE);
        }
      }
      else
      {
        #ifdef DEBUG
          WriteLog("Target Surface isn't a MipMap\n");
        #endif
        return(DDERR_CANNOTATTACHSURFACE);
      }
    }
    else
    {
        rc = DPA_InsertPtr( me->DPA_SurfaceAttached,
                            DPA_GetPtrCount(me->DPA_SurfaceAttached),
                            AttachedSurface);

      if(rc>=0)
      {
        AttachedSurface->Vtbl.AddRef(AttachedSurface);
      }
      else
      {
        #ifdef DEBUG
          WriteLog("Internal : Error attaching to general Set\n");
        #endif
        return(DDERR_CANNOTATTACHSURFACE);
      }
    }
  } // endif DDSCAPS_TEXTURE
  else
  {
    if( (AttachedSurface->DDSurfaceDesc.dwWidth != me->DDSurfaceDesc.dwWidth)
        || (AttachedSurface->DDSurfaceDesc.dwHeight != me->DDSurfaceDesc.dwHeight)
    //    || (AttachedSurface->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount !=
    //        me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
       )
    {
      #ifdef DEBUG
        WriteLog("Surfaces don't have same dimensions\n");
      #endif
      return(DDERR_CANNOTATTACHSURFACE);
    }
    else
    {
      if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
      {
        if( (AttachedSurface->GetFrontBuffer()!=NULL) || (AttachedSurface->BackBuffer!= NULL))
        {
          #ifdef DEBUG
            WriteLog("Surfaces already has a front/backbuffer\n");
          #endif
          return(DDERR_SURFACEALREADYATTACHED);
        }

        if(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER)
        {
          if(NULL!=me->BackBuffer)
          {
            BBCursor = me->BackBuffer;
            while(NULL!=BBCursor)
            {
              BBCursor->DDSurfaceDesc.dwFlags |= DDSD_BACKBUFFERCOUNT;
              BBCursor->DDSurfaceDesc.dwBackBufferCount++;
              BBCursor->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FLIP;
              BBCursor = BBCursor->BackBuffer;
            }
            BBCursor->BackBuffer = AttachedSurface;
            AttachedSurface->SetFrontBuffer(BBCursor);
          }
          else
          {
            me->BackBuffer = AttachedSurface;
            AttachedSurface->SetFrontBuffer(me);
          }
          me->DDSurfaceDesc.dwFlags |= DDSD_BACKBUFFERCOUNT;
          me->DDSurfaceDesc.dwBackBufferCount++;
          me->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FLIP;

          AttachedSurface->Vtbl.AddRef(AttachedSurface);
          return (DD_OK);
        }
        else
        {
          #ifdef DEBUG
            WriteLog("Can't attach backbuffer to anything but a frontbuffer\n");
          #endif
         return(DDERR_CANNOTATTACHSURFACE);
        }
      }
      else
      {
        if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
        {
          if(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
          {
            if( DPA_InsertPtr( me->DPA_SurfaceMipMaps,
                               DPA_GetPtrCount(me->DPA_SurfaceMipMaps),
                               AttachedSurface) >=0)
            {
              me->DDSurfaceDesc.dwFlags |= DDSD_MIPMAPCOUNT;
              me->DDSurfaceDesc.dwMipMapCount++;
              me->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FRONTBUFFER;

              AttachedSurface->Vtbl.AddRef(AttachedSurface);
            }
            else
            {
              #ifdef DEBUG
                WriteLog("Internal : Error attaching to MipMap\n");
              #endif
              return(DDERR_CANNOTATTACHSURFACE);
            }
          }
          else
          {
            #ifdef DEBUG
              WriteLog("Tagget Surface isn't a MipMap\n");
            #endif
           return(DDERR_CANNOTATTACHSURFACE);
          }
        }
        else
        {
          if( DPA_InsertPtr( me->DPA_SurfaceAttached,
                             DPA_GetPtrCount(me->DPA_SurfaceAttached),
                             AttachedSurface) >=0)
          {
            AttachedSurface->Vtbl.AddRef(AttachedSurface);
          }
          else
          {
            #ifdef DEBUG
              WriteLog("Internal : Error attaching to general Set\n");
            #endif
            return(DDERR_CANNOTATTACHSURFACE);
          }
        }
      }// End if not DDSCAPS_BACKBUFFER
    }
  }
  WriteLog("Surface attached\n");
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfAddOverlayDirtyRect(THIS, LPRECT)
{
  #ifdef DEBUG
    WriteLog("SurfAddOverlayDirtyRect Not implemented by M$ in V 6.0! \n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBlt(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE2 lpDDSrcSurface,
        LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
 return SurfBlt4( This,
                  lpDestRect,
                  (LPDIRECTDRAWSURFACE4)lpDDSrcSurface,
                  lpSrcRect,
                  dwFlags,
                  lpDDBltFx);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBlt4(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
        LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
   // We have determine between 3 different blit senarios.
   // 1. Blitting between Divebuffers (Front/Backbuffer and primary surface)
   // 2. Blitting between memory and Divebuffers (Front/Backbuffer and primary surface).
   // 3. Blitting between memory buffers.
   // 1 and 3 are easy. DiveBlitImage or memcpy will do the job for non transparent blits
   // 2 is now also easy as we do colorconverion via Dive after each unlocking of a surface
   // The advantage is that we don't have to call DiveSetupBlitter each time. The Blitter will be
   // setup only when the screen resolution is changed by ddraw. I guess we should see a big performance
   // increase by doing it this way, unless the software blits directly from memory to the primary surface)
   // But even then this could be faster as the SetupBlitter call is timeconsumeing and DIVE does emulate
   // the blit in SW anyway as there is no interface in the driver to blit with HW support from the sysmem.

 OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
 OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)lpDDSrcSurface;

 HRESULT                rc;
 ULONG                  ulColor, *pColor;
 RECTL                  cliprect;

 int                    x, y, i, j, BlitWidth, BlitHeight;
 PALETTEENTRY           SysPal[257];
 PLOGPALETTE            pLogPal = (PLOGPALETTE) SysPal;
 char                   *pBltPos, *pSrcPos;
 DDSURFACEDESC2         DestSurfaceDesc, SrcSurfaceDesc;
 DDRectangle             *pIRectDest,*pIRectSrc,*pIRectTest;
 RECTL                  DestRect, SrcRect;
 BOOL Found;
 DWORD dwSrcColor, dwDestColor;

  #ifdef DEBUG
    if ( (NULL!=lpDestRect)&& (NULL!=lpSrcRect))
      WriteLog("SurfBlt4 to (%d,%d)(%d,%d) at %08X from (%d,%d)(%d,%d) at %08X\n", lpDestRect->left, lpDestRect->top,
               lpDestRect->right, lpDestRect->bottom, dest, lpSrcRect->left, lpSrcRect->top,
               lpSrcRect->right, lpSrcRect->bottom, src);

    _dump_DDBLT(dwFlags);
  #endif


  if (NULL!=lpDestRect)
  {
    pIRectDest = new DDRectangle( lpDestRect->top,
                                  lpDestRect->left,
                                  lpDestRect->bottom,
                                  lpDestRect->right);
    memcpy(&DestRect,lpDestRect,sizeof(RECTL) );
  }
  else
  {
    pIRectDest = new DDRectangle( 0, 0, dest->height, dest->width);
    DestRect.top    = 0;
    DestRect.left   = 0;
    DestRect.bottom = dest->height;
    DestRect.right  = dest->width;
  }

  if(dest->fLocked)
  {
    if (NULL==lpDestRect)
    {
      // If anything is locked we can't blit to the complete surface as
      // a part is locked
      Found = TRUE;
    }
    else
    {
      // If the dest Rectangle intersects with any of the locked rectangles
      // we can't blit to it

      Found = FALSE;
      i=0;
      while( (i<DPA_GetPtrCount(dest->DPA_LockedRects))  && !Found)
      {
        pIRectTest = (DDRectangle*) DPA_FastGetPtr(dest->DPA_LockedRects,i);
        Found = pIRectDest->intersects(*pIRectTest);
        i++;
      }

    }

    if (Found)
    {
      delete pIRectDest;
      #ifdef DEBUG
        WriteLog("Blt: Dest Surface partly locked\n");
      #endif
      return(DDERR_SURFACEBUSY);
    }
  }

  DestSurfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
  SrcSurfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

  // First check the simple first

  dwFlags &= ~(DDBLT_WAIT|DDBLT_ASYNC); // FIXME: can't handle right now

  if(dwFlags & DDBLT_COLORFILL)
  {
    WriteLog("ColorFill\n");
    if((NULL==lpDDBltFx)||(lpDDBltFx->dwSize!=sizeof(DDBLTFX)) )
      return DDERR_INVALIDPARAMS;

    // ToDo : as we fill the DiveBuffer check if we need to convert the
    // specified color

    dest->ColorFill(lpDestRect,lpDDBltFx->dwFillColor);

    return(DD_OK); // according to the M$ DDK only one flag shall/can be set.
  } // end of colorfill

  if (dwFlags & DDBLT_DEPTHFILL)
  {
    WriteLog("DepthFill\n");
  #ifdef USE_OPENGL
    GLboolean ztest;
    // Todo support more than one Z-Buffer
    // Clears the screen
    WriteLog("Filling depth buffer with %ld\n", lpbltfx->b.dwFillDepth);
    glClearDepth(lpDDBltFx->b.dwFillDepth / 65535.0); // We suppose a 16 bit Z Buffer
    glGetBooleanv(GL_DEPTH_TEST, &ztest);
    glDepthMask(GL_TRUE); // Enables Z writing to be sure to delete also the Z buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(ztest);

    return (DD_OK);
  #endif // USE_OPENGL
  }

  if(dwFlags & DDBLT_ROP)
  {
    // HEL and we only support the following ROPS
    // SRC_COPY
    // BLACKNESS
    // WHITENESS
    //
    if(lpDDBltFx->dwROP & SRCCOPY)
      dwFlags = 0;  // srccopy is a normal fast blt
    else
    {
      if(lpDDBltFx->dwROP & BLACKNESS)
      {
        if(1==dest->dwBytesPPDive)
        {
          // ToDo: Realy implement code to get the correct index for black in 8 Bitmode
          dest->ColorFill(lpDestRect, 0 );
        }
        else
          dest->ColorFill(lpDestRect, 0);
        return DD_OK;
      }

      if(lpDDBltFx->dwROP & WHITENESS)
      {
        if(1==dest->dwBytesPPDive)
        {
          // ToDo: Realy implement code to get the correct index for black in 8 Bitmode
          dest->ColorFill(lpDestRect, 0xFFFFFFFF );
        }
        else
          dest->ColorFill(lpDestRect, 0xFFFFFFFF);
        return DD_OK;
      }

      return DDERR_NORASTEROPHW;
    }
  }

  if(NULL==src)
  {
    #ifdef DEBUG
      WriteLog("Unsupported sourceless FX operation. Flags = 0x%04X\n",dwFlags);
    #endif
    return DD_OK;
  }

  if (NULL!=lpSrcRect)
  {
    pIRectSrc = new DDRectangle( lpSrcRect->top,
                                 lpSrcRect->left,
                                 lpSrcRect->bottom,
                                 lpSrcRect->right);
    memcpy(&SrcRect,lpSrcRect,sizeof(RECTL) );
  }
  else
  {
    pIRectSrc = new DDRectangle( 0, 0, src->height, src->width);
    SrcRect.top    = 0;
    SrcRect.left   = 0;
    SrcRect.bottom = src->height;
    SrcRect.right  = src->width;
  }

  if(src->fLocked)
  {
    if (NULL==lpSrcRect)
    {
      // If anything is locked we can't blit from the complete surface as
      // a part is locked
      Found = TRUE;
    }
    else
    {
      // If the src Rectangle intersects with any of the locked rectangles of the
      // source surface we can't blit from it

      Found = FALSE;
      i=0;

      while((i<DPA_GetPtrCount(src->DPA_LockedRects) ) && !Found)
      {
        pIRectTest = (DDRectangle*) DPA_FastGetPtr(src->DPA_LockedRects,i);
        Found = pIRectDest->intersects(*pIRectTest);
        i++;
      }

    }

    if (Found)
    {
      delete pIRectSrc;
      #ifdef DEBUG
        WriteLog("Blt: Src Surface partly locked\n");
      #endif
      return(DDERR_SURFACEBUSY);
    }
  }

  if( ( (NULL==lpDestRect) && (NULL!=lpSrcRect) ) ||
      ( (NULL==lpSrcRect) && (NULL!=lpDestRect) ) )
  {
    #ifdef DEBUG
      WriteLog("Blitting with scaleing\n Not supported.\n");
    #endif
    return DDERR_NOSTRETCHHW;
  }

  if( ( pIRectDest->width()  != pIRectSrc->width() ) ||
      ( pIRectDest->height() != pIRectSrc->height() )
    )
  {
    // Stretching not supported
    WriteLog("No stretched blits\n");

    return DDERR_NOSTRETCHHW;
  }

  if (dest->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
  {
    WriteLog("Dest is Primary Surface\n");
    if(src->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
    {
      // special Type 1 : Bliting between parts of the screen

      WriteLog("Src is Primary Surface\n");

      if( *pIRectDest == *pIRectSrc)
        return DD_OK; // rects are the same => no blit needed

      // Todo: might add transparent blits but I don't think they are used here, so later!

      MoveRects( dest->pDiveBuffer,
                 (LPRECT)&DestRect,
                 (LPRECT)&SrcRect,
                 dest->dwBytesPPDive,
                 dest->dwPitchDB);

      // End of Special Type 1 blitting on the screen
    }
    else
    {
      if( src->diveBufNr>0)
      {

        if( (NULL==lpSrcRect)&&( NULL== lpDestRect))
        {
          // No Rectangles so use Dive to blit everything
          // ToDo : Implement transparent blitting but that seams more
          //        inportant for partial blits.
          //        If we do this later we could skip this check and don't
          //        use Dive .This keeps our simpler and smaler
          //
          DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);

        }
      }

      // Everything else we do yourselfs
      // Type 2 Sysmem to Primarysurface ca also be handled by this

      pBltPos = (char*) dest->pDiveBuffer + (DestRect.top * dest->dwPitchDB) +
                (DestRect.left * dest->dwBytesPPDive);

      pSrcPos = (char*) src->pDiveBuffer + (SrcRect.top * src->dwPitchDB) +
                (SrcRect.left * src->dwBytesPPDive);

      BlitHeight = pIRectDest->height();
      BlitWidth = pIRectDest->width();

      if(!dwFlags)
      {
        BlitWidth *= dest->dwBytesPPDive;

        #ifdef USE_ASM
        BltRec(pBltPos, pSrcPos, BlitWidth, BlitHeight,
               dest->dwPitchDB,
               src->dwPitchDB);
        #else
        // Solid Blit
        while(1)
        {
          memcpy(pBltPos,pSrcPos,BlitWidth);
          pBltPos += dest->dwPitchDB;
          pSrcPos += src->dwPitchDB;
          if(! (--BlitHeight))
            break;
        }
        #endif
      }
      else
      {
        // Transparent Blit
        if( (dwFlags &DDBLT_KEYSRC) || (dwFlags & DDBLT_KEYSRCOVERRIDE) )
        {
          WriteLog("Transparent src blit not done yet for primary!!");
        }
        else
        {
          WriteLog("Unhandled Flags Destination colorkey ? 0x%04X",dwFlags);
        }
      }
    } // end of handling blitting to primary
  }  // end of target primary surface
  else
  {
    if(0==src->diveBufNr)
    {
      // copy from the screen to a buffer

      if( (NULL==lpDestRect) &&
          (NULL==lpSrcRect) &&
          (dest->diveBufNr>0) )
      {
        // Blitting everything from frontbuffer to a Divebuffer
        // ToDo: Might should add checking for flags here
        DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);
      }
      else
      {
        // DIVE => DIVE  or Mem => Dive
        // or a rectangle from streen to a buffer can be handelt in the same way
        pBltPos = (char*) dest->pDiveBuffer + (DestRect.top * dest->dwPitchDB) +
                  (DestRect.left * dest->dwBytesPPDive);

        pSrcPos = (char*) src->pDiveBuffer + (SrcRect.top * src->dwPitchDB) +
                  (SrcRect.left * src->dwBytesPPDive);

        BlitHeight = pIRectDest->height();
        BlitWidth = pIRectDest->width();

        // Check for transparent blit
        if(!dwFlags)
        {
          BlitWidth *= dest->dwBytesPPDive;

          #ifdef USE_ASM
          BltRec(pBltPos, pSrcPos, BlitWidth, BlitHeight,
                 dest->dwPitchDB,
                 src->dwPitchDB);
          #else
          // Solid Blit
          while(1)
          {
            memcpy(pBltPos,pSrcPos,BlitWidth);
            pBltPos += dest->dwPitchDB;
            pSrcPos += src->dwPitchDB;
            if(! (--BlitHeight))
              break;
          }
          #endif
        }
        else
        {
          WriteLog("Transblt not done yet");
          if(dwFlags & DDBLT_KEYSRC)
          {
            if(!(src->DDSurfaceDesc.dwFlags & DDCKEY_SRCBLT))
            {
            }
          }
          else
          {
            if(dwFlags & DDBLT_KEYSRCOVERRIDE)
            {
            }
            else
            {
            }
          }
        }
      }
    } // end handling source screen
    else
    {
      // DIVE => DIVE  or Mem => Dive can be handelt in the same way

      if( (src->pDiveBuffer == dest->pDiveBuffer) &&
          (pIRectDest->intersects(*pIRectSrc) ) )
      {
        // Overlapping rects  on the same surface ?

        // ToDo : Maybe implement all the fancy blit flags here too ? ;)

        MoveRects( dest->pDiveBuffer,
                   (LPRECT)&DestRect,
                   (LPRECT)&SrcRect,
                   dest->dwBytesPPDive,
                   dest->dwPitchDB);

        return DD_OK;
      }

      pBltPos = (char*) dest->pDiveBuffer + (DestRect.top * dest->dwPitchDB) +
                (DestRect.left * dest->dwBytesPPDive);

      pSrcPos = (char*) src->pDiveBuffer + (SrcRect.top * src->dwPitchDB) +
                (SrcRect.left * src->dwBytesPPDive);

      BlitHeight = pIRectDest->height();
      BlitWidth = pIRectDest->width();

      // Check for transparent blit
      if(!dwFlags)
      {
        BlitWidth *= dest->dwBytesPPDive;

        #ifdef USE_ASM
        BltRec(pBltPos, pSrcPos, BlitWidth, BlitHeight,
               dest->dwPitchDB,
               src->dwPitchDB);
        #else
        // Solid Blit
        while(1)
        {
          memcpy(pBltPos,pSrcPos,BlitWidth);
          pBltPos += dest->dwPitchDB;
          pSrcPos += src->dwPitchDB;
          if(! (--BlitHeight))
            break;
        }
        #endif
      }
      else
      {
        DWORD dwPitch = dest->dwPitchDB;

        if(dwFlags &DDBLT_ROTATIONANGLE)
        {
          return DDERR_NOROTATIONHW;
        }

        if(dwFlags & DDBLT_DDFX)
        {
          DWORD dwFx;
          DWORD dwSrcColor, dwDestColor;

          dwFlags &= ~DDBLT_DDFX; // remove the handled flag

          if( NULL==lpDDBltFx)
            return DDERR_INVALIDPARAMS;

          dwFx = lpDDBltFx->dwDDFX;

          // Remove unsupported Flags
          dwFx &= ~(DDBLTFX_ARITHSTRETCHY |    // Not streach support
                    DDBLTFX_ZBUFFERBASEDEST |  // All ZBuffer flags are not
                    DDBLTFX_ZBUFFERRANGE |     // implementet in M$ Dx 6
                    DDBLTFX_NOTEARING );       // No sync with VRetrace yet

          if(dwFx & DDBLTFX_ROTATE180)
          {
            // 180 degree turn is a mix of a flip up/down and one left/right
            dwFx |= (DDBLTFX_MIRRORUPDOWN | DDBLTFX_MIRRORLEFTRIGHT);
            dwFx &= ~DDBLTFX_ROTATE180; // remove handled flag
          }
          if(dwFx & DDBLTFX_MIRRORUPDOWN)
          {
            // switching the the direction can be integrated with other flags
            dwPitch = -dwPitch;
            pBltPos = (char*) dest->pDiveBuffer +
                      ((DestRect.top +BlitHeight)* dest->dwPitchDB) +
                      (DestRect.left * dest->dwBytesPPDive);

            dwFx &= ~DDBLTFX_MIRRORUPDOWN;  // remove handled flag
          }

          if(dwFx & DDBLTFX_MIRRORLEFTRIGHT)
          {
            // 180 degree turn or a LR Mirroring
            // don't support any other dwFlags like transparent at the moment

            switch(dest->dwBytesPPDive)
            {
              case 1:
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    pBltPos[BlitWidth-x] = pSrcPos[x];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 2:
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    ((USHORT*)pBltPos)[BlitWidth-x] = ((USHORT*)pSrcPos)[x];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 3:
                BlitWidth *= 3;
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    pBltPos[BlitWidth-x] = pSrcPos[x-2];
                    x--;
                    pBltPos[BlitWidth-x] = pSrcPos[x];
                    x--;
                    pBltPos[BlitWidth-x] = pSrcPos[x+2];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 4:
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    ((DWORD*)pBltPos)[BlitWidth-x] = ((DWORD*)pSrcPos)[x];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
            }  // end switch
            dest->lpVtbl->ChangeUniquenessValue(dest);
            return DD_OK;
          }

          #ifdef DEBUG
            if(dwFx)
              _dump_DDBLTFX(dwFx);
          #endif
          // We ignore unhandled flags at the moment
        }

        if( (dwFlags & DDBLT_KEYSRC) |
            (dwFlags & DDBLT_KEYSRCOVERRIDE) )
        {
          if(dwFlags & DDBLT_KEYSRCOVERRIDE)
          {

            if( NULL==lpDDBltFx)
              return DDERR_INVALIDPARAMS;

            dwFlags &= ~DDBLT_KEYSRCOVERRIDE;

            // We work like the HEL and test only the low value
            dwSrcColor = lpDDBltFx->ddckSrcColorkey.dwColorSpaceLowValue;

          }
          else
          {

            dwFlags &= ~DDBLT_KEYSRC;

            // Not sure if that is OK maybe check if one is set ?
            // if(!(src->DDSurfaceDesc.dwFlags & DDCKEY_SRCBLT)) return DDERR_WRONGPARAM;?

            dwSrcColor = src->DDSurfaceDesc.ddckCKSrcBlt.dwColorSpaceLowValue;
          }

          // ToDo : We currently indicate that we don't support
          // DDBLT_KEYDEST but HEL does change that!
          // also add this key in the get/setColorKey functions

          if( (dwFlags & DDBLT_KEYDEST) |
              (dwFlags & DDBLT_KEYDESTOVERRIDE) )
          {
            // Source and dest color keying SLOW!!!
            if(dwFlags & DDBLT_KEYDESTOVERRIDE)
            {
              if( NULL==lpDDBltFx)
                return DDERR_INVALIDPARAMS;

              dwFlags &= ~DDBLT_KEYDESTOVERRIDE;

              // We work like the HEL and test only the low value
              dwDestColor = lpDDBltFx->ddckDestColorkey.dwColorSpaceLowValue;

            }
            else
            {

              dwFlags &= ~DDBLT_KEYDEST;

              // Not sure if that is OK maybe check if one is set ?
              // if(!(Dest->DDSurfaceDesc.dwFlags & DDCKEY_DESTBLT)) return DDERR_WRONGPARAM;?

              dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
            }

            // This will be be slow maybe move to ASM ?
            // using MMX should be much faster
            switch(dest->dwBytesPPDive)
            {
              case 1:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(pSrcPos[x] != (char) dwSrcColor)
                    {
                      if(pBltPos[x] != (char) dwDestColor)
                        pBltPos[x] = pSrcPos[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 2:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(((USHORT*)pSrcPos)[x] != (USHORT) dwSrcColor)
                    {
                      if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                        ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 3:
              {
                char *pSC, *pDC;
                pSC = (char*)&dwSrcColor;
                pDC = (char*)&dwDestColor;
                BlitWidth *=3;

                while(BlitHeight--)
                {
                  x = 0;

                  while(x<BlitWidth)
                  {
                    if( (pSrcPos[x]   != pSC[1]) &&
                        (pSrcPos[x+1] != pSC[2]) &&
                        (pSrcPos[x+2] != pSC[3])
                      )
                    {
                      if( (pBltPos[x]   != pDC[1]) &&
                          (pBltPos[x+1] != pDC[2]) &&
                          (pBltPos[x+2] != pDC[3])
                        )
                        {
                          pBltPos[x] = pSrcPos[x];
                          pBltPos[x+1] = pSrcPos[x+2];
                          pBltPos[x+1] = pSrcPos[x+2];
                        }
                    }
                    x +=3;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              }
              case 4:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(((DWORD*)pSrcPos)[x] != dwSrcColor)
                    {
                      if(((DWORD*)pBltPos)[x] != dwDestColor)
                        ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
            }  // End switch
          }
          else
          {
            // Only Source colorkey
          }
          dest->lpVtbl->ChangeUniquenessValue(dest);
          return DD_OK;
        }

        if( (dwFlags & DDBLT_KEYDEST) |
            (dwFlags & DDBLT_KEYDESTOVERRIDE) )
        {
          // Dest color keying SLOW!!!
          if(dwFlags & DDBLT_KEYSRCOVERRIDE)
          {
            if( NULL==lpDDBltFx)
              return DDERR_INVALIDPARAMS;

            dwFlags &= ~DDBLT_KEYSRCOVERRIDE;

            // We work like the HEL and test only the low value
            dwDestColor = lpDDBltFx->ddckDestColorkey.dwColorSpaceLowValue;

          }
          else
          {

            dwFlags &= ~DDBLT_KEYDEST;

            // Not sure if that is OK maybe check if one is set ?
            // if(!(src->DDSurfaceDesc.dwFlags & DDCKEY_DESTBLT)) return DDERR_WRONGPARAM;?

            dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
          }

          // This will be be slow maybe move to ASM ?
          // using MMX should be much faster
          switch(dest->dwBytesPPDive)
          {
            case 1:
              while(BlitHeight--)
              {
                x = 0;
                while(x<BlitWidth)
                {
                  if(pBltPos[x] != (char) dwDestColor)
                    pBltPos[x] = pSrcPos[x];
                  x++;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
            case 2:
              while(BlitHeight--)
              {
                x = 0;
                while(x<BlitWidth)
                {
                  if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                    ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                  x++;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
            case 3:
            {
              char *pSC, *pDC;
              pSC = (char*)&dwSrcColor;
              pDC = (char*)&dwDestColor;
              BlitWidth *=3;

              while(BlitHeight--)
              {
                x = 0;

                while(x<BlitWidth)
                {
                  if( (pBltPos[x]   != pDC[1]) &&
                      (pBltPos[x+1] != pDC[2]) &&
                      (pBltPos[x+2] != pDC[3])
                    )
                  {
                    pBltPos[x] = pSrcPos[x];
                    pBltPos[x+1] = pSrcPos[x+2];
                    pBltPos[x+1] = pSrcPos[x+2];
                  }
                  x +=3;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
            }
            case 4:
              while(BlitHeight--)
              {
                x = 0;
                while(x<BlitWidth)
                {
                  if( ((DWORD*)pBltPos)[x] != dwDestColor)
                    ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                  x++;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
          }  // End switch
        } // End of Dest ColorKey


      }// end  handling dwFlags
    } // End handling source not Framebuffer

  }// end handling destination not framebuffer

  dest->lpVtbl->ChangeUniquenessValue(dest);
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBltBatch(THIS, LPDDBLTBATCH, DWORD, DWORD )
{
  #ifdef DEBUG
    WriteLog("SurfBltBatch Not implemented by M$\n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBltFast(THIS, DWORD,DWORD,LPDIRECTDRAWSURFACE2, LPRECT,DWORD)
{
  #ifdef DEBUG
    WriteLog("SurfBltFast NIY\n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBltFast4( THIS This,
                                DWORD dwX,
                                DWORD dwY,
                                LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
                                LPRECT lpSrcRect,
                                DWORD dwTrans)
{
  OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)lpDDSrcSurface;
  RECTL SrcRect;
  char *pBltPos, *pSrcPos;
  DWORD dwDestColor, dwSrcColor, BlitWidth, BlitHeight,x,y;

  #ifdef DEBUG
    WriteLog("SurfBltFast4 %08X at(%d/%d) onto %08X with flags %08X\n",src, dwX,dwY, dest, dwTrans);
  #endif

  if( (NULL==lpDDSrcSurface) ||
      (dwX<0) || (dwY<0) ||
      (dwX>dest->width) ||
      (dwY>dest->height))
  {
    WriteLog("Invalid Parameters %08X, %d %d", lpDDSrcSurface ,dest->width , dest->height);
    return DDERR_INVALIDPARAMS;
  }

  if (NULL!=lpSrcRect)
  {
    memcpy(&SrcRect,lpSrcRect,sizeof(RECTL) );
  }
  else
  {
    SrcRect.top    = 0;
    SrcRect.left   = 0;
    SrcRect.bottom = src->height;
    SrcRect.right  = src->width;
  }

  // Todo: Test for locked src/dest

  pBltPos = (char*) dest->pDiveBuffer + (dwY * dest->dwPitchDB) +
            (dwX * dest->dwBytesPPDive);

  pSrcPos = (char*) src->pDiveBuffer + (SrcRect.top * src->dwPitchDB) +
            (SrcRect.left * src->dwBytesPPDive);

  BlitHeight = SrcRect.bottom - SrcRect.top;
  BlitWidth  = (SrcRect.right - SrcRect.left) * src->dwBytesPPDive;

  // Remove unsupported wait flag
  dwTrans &= ~DDBLTFAST_WAIT;

  if(DDBLTFAST_NOCOLORKEY == dwTrans )
  {
    WriteLog("Solid Blit");
    #ifdef USE_ASM
      BltRec(pBltPos, pSrcPos, BlitWidth, BlitHeight,
             dest->dwPitchDB,
             src->dwPitchDB);
    #else
      // Solid Blit
      while(1)
      {
        memcpy(pBltPos,pSrcPos,BlitWidth);
        pBltPos += dest->dwPitchDB;
        pSrcPos += src->dwPitchDB;
        if(! (--BlitHeight))
          break;
      }
    #endif

  }
  else
  {
    WriteLog("TransBlit\n");

    if(dwTrans & DDBLTFAST_SRCCOLORKEY)
    {
      WriteLog("Trans SRC\n");
      // transparent source
      dwSrcColor = src->DDSurfaceDesc.ddckCKSrcBlt.dwColorSpaceLowValue;
      if(dwTrans & DDBLTFAST_DESTCOLORKEY)
      {
        WriteLog("And Dest Colorkey");
        dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
        // Source and dest colorkeying
        switch(dest->dwBytesPPDive)
        {
          case 1:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(pSrcPos[x] != (char) dwSrcColor)
                {
                  if(pBltPos[x] != (char) dwDestColor)
                    pBltPos[x] = pSrcPos[x];
                }
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 2:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((USHORT*)pSrcPos)[x] != (USHORT) dwSrcColor)
                {
                  if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                    ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                }
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 3:
          {
            char *pSC, *pDC;
            pSC = (char*)&dwSrcColor;
            pDC = (char*)&dwDestColor;
            BlitWidth *=3;

            while(BlitHeight--)
            {
              x = 0;

              while(x<BlitWidth)
              {
                if( (pSrcPos[x]   != pSC[1]) &&
                    (pSrcPos[x+1] != pSC[2]) &&
                    (pSrcPos[x+2] != pSC[3])
                  )
                {
                  if( (pBltPos[x]   != pDC[1]) &&
                      (pBltPos[x+1] != pDC[2]) &&
                      (pBltPos[x+2] != pDC[3])
                    )
                    {
                      pBltPos[x] = pSrcPos[x];
                      pBltPos[x+1] = pSrcPos[x+2];
                      pBltPos[x+1] = pSrcPos[x+2];
                    }
                }
                x +=3;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          }
          case 4:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((DWORD*)pSrcPos)[x] != dwSrcColor)
                {
                  if(((DWORD*)pBltPos)[x] != dwDestColor)
                    ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                }
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
        }  // End switch
      }
      else
      {
        // This MMX detection should be moved into OS2Draw
        // and into the surface constructor a setup for blitting pointers
        WriteLog("Only Src ColorKey");
        switch(dest->dwBytesPPDive)
        {
          case 1:
            if(CPUHasMMX())
              while(BlitHeight--)
              {
                BlitColorKey8MMX((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            else
              while(BlitHeight--)
              {
                BlitColorKey8((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            break;
          case 2:

            if(CPUHasMMX())
              while(BlitHeight--)
              {
                BlitColorKey16MMX((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            else
              while(BlitHeight--)
              {
                BlitColorKey16((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            break;
          case 3:
            char *pSC;
            pSC = (char*)&dwSrcColor;
            BlitWidth *=3;

            while(BlitHeight--)
            {
              x = 0;

              while(x<BlitWidth)
              {
                if( (pSrcPos[x]   != pSC[1]) &&
                    (pSrcPos[x+1] != pSC[2]) &&
                    (pSrcPos[x+2] != pSC[3])
                  )
                {
                  pBltPos[x] = pSrcPos[x];
                  pBltPos[x+1] = pSrcPos[x+1];
                  pBltPos[x+1] = pSrcPos[x+2];
                }
                x +=3;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 4:
            break;
        }
      }
    }
    else
    {
      if(dwTrans & DDBLTFAST_DESTCOLORKEY)
      {
        WriteLog("DestColorKey\n");

        dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
        switch(dest->dwBytesPPDive)
        {
          case 1:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(pBltPos[x] != (char) dwDestColor)
                  pBltPos[x] = pSrcPos[x];
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 2:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                  ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 3:
          {
            char *pSC, *pDC;
            pSC = (char*)&dwSrcColor;
            pDC = (char*)&dwDestColor;
            BlitWidth *=3;

            while(BlitHeight--)
            {
              x = 0;

              while(x<BlitWidth)
              {
                if( (pBltPos[x]   != pDC[1]) &&
                    (pBltPos[x+1] != pDC[2]) &&
                    (pBltPos[x+2] != pDC[3])
                  )
                {
                  pBltPos[x] = pSrcPos[x];
                  pBltPos[x+1] = pSrcPos[x+2];
                  pBltPos[x+1] = pSrcPos[x+2];
                }
                x +=3;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          }
          case 4:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((DWORD*)pBltPos)[x] != dwDestColor)
                  ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
        }  // End switch
      }
      else
      {
        WriteLog("Unexpected Flags");
      }
    }
  }
  dest->lpVtbl->ChangeUniquenessValue(dest);


  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfDeleteAttachedSurface(THIS This, DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSurface)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  #ifdef DEBUG
    WriteLog("SurfDeleteAttachedSurface\n");
  #endif

  return(SurfDeleteAttachedSurface4(me, dwFlags, (LPDIRECTDRAWSURFACE4)lpDDSurface));
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfDeleteAttachedSurface4(THIS This, DWORD dwFlags , LPDIRECTDRAWSURFACE4 lpDDSurface)
{

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *AttachedSurface;
  OS2IDirectDrawSurface *SurfaceCursor;
  int i;

  BOOL Found = FALSE;
  #ifdef DEBUG
    WriteLog("SurfDeleteAttachedSurface\n");
  #endif

  if((0!=dwFlags)||(NULL==lpDDSurface))
    return(DDERR_INVALIDPARAMS);

  AttachedSurface = (OS2IDirectDrawSurface*) lpDDSurface;
  if (AttachedSurface->IsImplicitSurface())
    return (DDERR_CANNOTDETACHSURFACE);

  if ( (AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & ( DDSCAPS_FLIP | DDSCAPS_BACKBUFFER)) &&
       !(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER) )
  {
    // Surface seams to be a backbuffer in a flipchain search it

    // Goto top of list
    if(me->FrontBuffer!=NULL)
    {
      SurfaceCursor = me->FrontBuffer;
      while(SurfaceCursor->GetFrontBuffer()!=NULL)
        SurfaceCursor = SurfaceCursor->GetFrontBuffer();
    }
    else
      SurfaceCursor = me;

    // now iterrate through the list skip first in list as we don't remove the frontbuffer

    SurfaceCursor = SurfaceCursor->BackBuffer;
    while((SurfaceCursor!= AttachedSurface)&&(SurfaceCursor!=NULL))
      SurfaceCursor = SurfaceCursor->BackBuffer;

    if(SurfaceCursor!=NULL)
    {
      Found = TRUE;
      // remove the Surface from the list
      SurfaceCursor->FrontBuffer->BackBuffer = SurfaceCursor->BackBuffer;
      if(SurfaceCursor->BackBuffer!=NULL)
      {
        SurfaceCursor->BackBuffer->SetFrontBuffer(SurfaceCursor->FrontBuffer);

      }
      else
      {
        // we were the last buffer in the list have we been the only backbuffer?
        if(SurfaceCursor->FrontBuffer->FrontBuffer == NULL)
        {
          // Yepp so "destroy" the flipchain
          SurfaceCursor->FrontBuffer->DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FLIP;
          SurfaceCursor->FrontBuffer->DDSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
        }
      }
      // decrement the backbuffer count of all buffers in the chain in front of us
      while(SurfaceCursor->GetFrontBuffer()!=NULL)
      {
        SurfaceCursor = SurfaceCursor->GetFrontBuffer();
        SurfaceCursor->DDSurfaceDesc.dwBackBufferCount-- ;
      }

      AttachedSurface->DDSurfaceDesc.dwBackBufferCount = 0;
      AttachedSurface->DDSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
      AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FLIP;
      AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER; // Set this flag as adding to the chain removed it
      AttachedSurface->lpVtbl->Release(AttachedSurface);

    }
  } //endif delete back/frontbuffers

  if ( (!Found) && (AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & ( DDSCAPS_FLIP & DDSCAPS_FRONTBUFFER)) )
  {
    // seams like someone wants a new frontbuffer

    // Goto top of list

    if(me->FrontBuffer!=NULL)
    {
      SurfaceCursor = me->FrontBuffer;
      while(SurfaceCursor->GetFrontBuffer()!=NULL)
        SurfaceCursor = SurfaceCursor->GetFrontBuffer();
    }
    else
      SurfaceCursor = me;

    if(SurfaceCursor == AttachedSurface)
    {
      Found = TRUE;
      SurfaceCursor->BackBuffer->SetFrontBuffer(NULL);
      AttachedSurface->DDSurfaceDesc.dwBackBufferCount = 0;
      AttachedSurface->DDSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
      AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FLIP;
      AttachedSurface->lpVtbl->Release(AttachedSurface);
    }

  }


  if ( (!Found) && (AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps &  DDSCAPS_MIPMAP ) )
  {
    // Surface seams to be a mipmap
    i = 0;
    while((DPA_GetPtrCount(me->DPA_SurfaceMipMaps)>i ) && !Found)
    {
      if (DPA_FastGetPtr(me->DPA_SurfaceMipMaps,i) == AttachedSurface)
      {
        Found = TRUE;
        DPA_DeletePtr(me->DPA_SurfaceMipMaps,i);
        AttachedSurface->lpVtbl->Release(AttachedSurface);
        // adjust our info
        me->DDSurfaceDesc.dwMipMapCount-- ;
        if (!me->DDSurfaceDesc.dwMipMapCount)
        {
          me->DDSurfaceDesc.dwFlags &= ~DDSD_MIPMAPCOUNT;
        }
      }
      i++;
    }
  }

  if(!Found)
  {
    // Surface seams to be an attached one
    i = 0;
    while((DPA_GetPtrCount(me->DPA_SurfaceAttached)>i ) && !Found)
    {
      if (DPA_FastGetPtr(me->DPA_SurfaceAttached,i) == AttachedSurface)
      {
        Found = TRUE;
        DPA_DeletePtr(me->DPA_SurfaceAttached,i);
        AttachedSurface->lpVtbl->Release(AttachedSurface);
      }
      i++;
    }
  }


  return(Found?DD_OK:DDERR_SURFACENOTATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfEnumAttachedSurfaces(THIS This, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpCallBack)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  #ifdef DEBUG
    WriteLog("SurfEnumAttachedSurfaces\n");
  #endif

  return(SurfEnumAttachedSurfaces4(me,lpContext, (LPDDENUMSURFACESCALLBACK2) lpCallBack));
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfEnumAttachedSurfaces4(THIS This, LPVOID lpContext ,LPDDENUMSURFACESCALLBACK2 lpCallBack)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *EnumSurface;
  DDSURFACEDESC2         EnumDesc;
  int i,count;
  HRESULT rc;

  #ifdef DEBUG
    WriteLog("SurfEnumAttachedSurfaces\n");
  #endif
  if (NULL==lpCallBack)
    return (DDERR_INVALIDPARAMS);

  rc = DDENUMRET_OK;


  if(me->BackBuffer != NULL)
  {
    memcpy(&EnumDesc,&(me->DDSurfaceDesc),sizeof(DDSURFACEDESC2));
    rc = lpCallBack((LPDIRECTDRAWSURFACE4)me->BackBuffer,&EnumDesc,lpContext);
  }

  count = DPA_GetPtrCount(me->DPA_SurfaceMipMaps);

  if(count>0)
  {
    i=0;
    while( (DDENUMRET_OK == rc) && i<count )
    {
        EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceMipMaps,i);
        memcpy( &EnumDesc,
                &(EnumSurface->DDSurfaceDesc),
                sizeof(DDSURFACEDESC2));
      // Calling back into WIN32 app so we had to reset FS
      rc = lpCallBack((LPDIRECTDRAWSURFACE4)EnumSurface,&EnumDesc,lpContext);
      i++;
    }
  }

  count = DPA_GetPtrCount(me->DPA_SurfaceAttached);

  if(count>0)
  {
    i=0;
    while( (DDENUMRET_OK == rc) && i<count )
    {
      EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceAttached,i);
      memcpy( &EnumDesc,
              &(EnumSurface->DDSurfaceDesc),
              sizeof(DDSURFACEDESC2));
      rc = lpCallBack((LPDIRECTDRAWSURFACE4)EnumSurface,&EnumDesc,lpContext);
      i++;
    }
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfEnumOverlayZOrders(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK)
{
  #ifdef DEBUG
    WriteLog("SurfEnumOverlayZOrders\n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfEnumOverlayZOrders4(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2)
{
  #ifdef DEBUG
    WriteLog("SurfEnumOverlayZOrders\n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfFlip(THIS This, LPDIRECTDRAWSURFACE2 lpDDSurf, DWORD dwFlags)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  #ifdef DEBUG
    WriteLog("SurfFlip\n");
  #endif

  return(SurfFlip4(me, (LPDIRECTDRAWSURFACE4) lpDDSurf, dwFlags));
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfFlip4(THIS This, LPDIRECTDRAWSURFACE4 lpDDSurf, DWORD dwFlags)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *FlipSurface;
  OS2IDirectDrawSurface *FlipCursor;
  LPVOID Data;
  char *pcrFB,*pcFB,*pcrDB,*pcDB;
  ULONG rc;

  #ifdef DEBUG
    WriteLog("SurfFlip4\n");
  #endif
  if(!((me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER) &&
       (me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FLIP))
    )
  {
    #ifdef DEBUG
      WriteLog("Flip called on none Frontbuffer/Flip surface\n Flags:\n");
      _dump_DDSCAPS(me->DDSurfaceDesc.ddsCaps.dwCaps);
      WriteLog("\n");
    #endif
    return(DDERR_NOTFLIPPABLE);
  }

  if(NULL!=lpDDSurf)
  {
    #ifdef DEBUG
      WriteLog("Check if Surface is in Flipchain!\n");
    #endif

    // We got an override surface check if it is in the flipchain
    FlipSurface = (OS2IDirectDrawSurface*) lpDDSurf;
    FlipCursor = me->BackBuffer;
    while((NULL!=FlipCursor)&&(FlipCursor!=FlipSurface))
    {
      FlipCursor = FlipCursor->BackBuffer;
    }

    if(FlipCursor!=FlipSurface)
    {
      #ifdef DEBUG
        WriteLog("Surface not in Flipchain!\n");
      #endif
      return (DDERR_INVALIDPARAMS); // Not sure if the returnvalue is right
    }
  }
  else
  {
    FlipSurface = me->NextFlip; // Take the next Surface in the Flipchain
    #ifdef DEBUG
      WriteLog("Next Surface @ 0x%08X\n",FlipSurface);
    #endif
  }

  if((me->fLocked)||(FlipSurface->fLocked))
  {
    #ifdef DEBUG
      WriteLog("Locked surface(s) Dest %d Src %d\n",me->fLocked,FlipSurface->fLocked);
    #endif
    return(DDERR_SURFACEBUSY);
  }

  if(-1 != me->diveBufNr)
  {
    #ifdef DEBUG
      WriteLog("DIVE Flipchain");
    #endif

    // we got some DIVE surfaces
    // On Dive Buffers More then Double buffering won't get any perf. gain
    // as we have to move all the data to the Frontbuffer and can't simply exchange the pointers
    // Doulebuffering should work best.

    rc = DiveBlitImage(me->hDive, FlipSurface->diveBufNr, me->diveBufNr);

    #ifdef DEBUG
      WriteLog("DiveBlitImage rc = 0x%08X\n");
    #endif

    if(NULL==lpDDSurf)
    {
      // advance in the flipchain if no valid override surface was passed in
      me->NextFlip = FlipSurface->BackBuffer!=NULL?FlipSurface->BackBuffer:me->BackBuffer;
    }
  }
  else
  {
    #ifdef DEBUG
      WriteLog("Memory Flipchain");
    #endif
    // Memory Flipchain
    //
    // ToDo : Check what happens to src/dest colorkeys etc do the move also ?
    //
    // We only change the memory pointer to the buffers no need to copy all the data
    // So the NextFlip is here allways the Backbuffer so we won't advance this
    //
    // Sample  (triple buffering) :               Before Flip    After Flip
    //                               Buffer:      FB  BB  TB     FB  BB  TB
    //                               Memory:      11  22  33     22  33  11
    //

    Data  = me->DDSurfaceDesc.lpSurface;
    pcrFB = me->pFBreal;
    pcFB  = me->pFrameBuffer;
    pcrDB = me->pDBreal;
    pcDB  = me->pDiveBuffer;
    me->DDSurfaceDesc.lpSurface = FlipSurface->DDSurfaceDesc.lpSurface;
    me->pFBreal                 = FlipSurface->pFBreal;
    me->pFrameBuffer            = FlipSurface->pFrameBuffer;
    me->pDBreal                 = FlipSurface->pDBreal;
    me->pDiveBuffer             = FlipSurface->pDiveBuffer;

    if(NULL==lpDDSurf)
    {
      while(NULL!=FlipSurface->BackBuffer)
      {
        FlipSurface->DDSurfaceDesc.lpSurface = FlipSurface->BackBuffer->DDSurfaceDesc.lpSurface;
        FlipSurface->pFBreal                 = FlipSurface->BackBuffer->pFBreal;
        FlipSurface->pFrameBuffer            = FlipSurface->BackBuffer->pFrameBuffer;
        FlipSurface->pDBreal                 = FlipSurface->BackBuffer->pDBreal;
        FlipSurface->pDiveBuffer             = FlipSurface->BackBuffer->pDiveBuffer;
        FlipSurface = FlipSurface->BackBuffer;
      }
    }
    FlipSurface->DDSurfaceDesc.lpSurface = Data;
    FlipSurface->pFBreal                 = pcrFB;
    FlipSurface->pFrameBuffer            = pcFB;
    FlipSurface->pDBreal                 = pcrDB;
    FlipSurface->pDiveBuffer             = pcDB;
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetAttachedSurface(THIS This, LPDDSCAPS lpDDCaps,
                                         LPDIRECTDRAWSURFACE2 FAR * lpDDSurf)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  #ifdef DEBUG
    WriteLog("SurfGetAttachedSurface\n");
  #endif

  return(SurfGetAttachedSurface4(me, (LPDDSCAPS2)lpDDCaps , (LPDIRECTDRAWSURFACE4*)lpDDSurf));
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetAttachedSurface4(THIS This, LPDDSCAPS2 lpDDCaps,
                                          LPDIRECTDRAWSURFACE4 FAR * lpDDSurf)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *EnumSurface = NULL;
  OS2IDirectDrawSurface *AttachedSurface = NULL;
  HRESULT rc;
  int i;

  #ifdef DEBUG
    WriteLog("SurfGetAttachedSurface4\n>Requested Caps: ");
    _dump_DDSCAPS(lpDDCaps->dwCaps);
    WriteLog("\n");
  #endif

  if( (NULL==lpDDCaps)||(NULL==lpDDSurf))
  {
    WriteLog("Invalid params\n\n");
    return (DDERR_INVALIDPARAMS);
  }


  rc = DD_OK;

  if( (me->BackBuffer!=NULL) &&
      (me->BackBuffer->DDSurfaceDesc.ddsCaps.dwCaps & lpDDCaps->dwCaps) )
  {
    WriteLog("Return Backbuffer\n");
    AttachedSurface = me->BackBuffer;
  }

  if(DPA_GetPtrCount(me->DPA_SurfaceMipMaps)>0)
  {
    i=0;
    while( i<DPA_GetPtrCount(me->DPA_SurfaceMipMaps) )
    {
      EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceMipMaps,i);
      if(EnumSurface->DDSurfaceDesc.ddsCaps.dwCaps == lpDDCaps->dwCaps)
      {
        if(NULL==AttachedSurface)
          AttachedSurface = EnumSurface;
        else
          rc = DDERR_NOTFOUND; // Not sure if this is the right return value,
                                  // but function must fail if more then one surface fits

      }
      i++;
    }
  }

  if(DPA_GetPtrCount(me->DPA_SurfaceAttached)>0)
  {
    i=0;
    while( i<DPA_GetPtrCount(me->DPA_SurfaceAttached) )
    {
      EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceAttached,i);
      if(EnumSurface->DDSurfaceDesc.ddsCaps.dwCaps == lpDDCaps->dwCaps)
      {
        if(NULL==AttachedSurface)
          AttachedSurface = EnumSurface;
        else
          rc = DDERR_NOTFOUND; // Not sure if this is the right return value,
                                  // but function must fail if more then one surface fits

      }
      i++;
    }
  }

  if( (DD_OK==rc) &&
      (NULL!=AttachedSurface) )
  {
    *lpDDSurf = (IDirectDrawSurface4*)AttachedSurface;
    // not sure but as we returned an reference rains usage count
    AttachedSurface->lpVtbl->AddRef(AttachedSurface);
  }
  else
  {
    *lpDDSurf = NULL;
    rc = DDERR_NOTFOUND;
  }


  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetBltStatus(THIS This, DWORD)
{
  #ifdef DEBUG
    WriteLog("SurfGetBltStatus\n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetCaps(THIS This, LPDDSCAPS lpDDCaps)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfGetCaps\n");
  #endif

  if(NULL==lpDDCaps)
    return(DDERR_INVALIDPARAMS);

  lpDDCaps->dwCaps = me->DDSurfaceDesc.ddsCaps.dwCaps;
  #ifdef DEBUG
    _dump_DDSCAPS(lpDDCaps->dwCaps);
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetCaps4(THIS This, LPDDSCAPS2 lpDDCaps)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  #ifdef DEBUG
    WriteLog("SurfGetCaps4\n");
  #endif

  if(NULL==lpDDCaps)
    return(DDERR_INVALIDPARAMS);

  memcpy(lpDDCaps, &(me->DDSurfaceDesc.ddsCaps), sizeof(DDSCAPS2) );
  #ifdef DEBUG
    _dump_DDSCAPS(lpDDCaps->dwCaps);
    _dump_DDSCAPS2(lpDDCaps->dwCaps2);

  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetClipper(THIS This, LPDIRECTDRAWCLIPPER FAR *lplpClipper)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfGetClipper\n");
  #endif

  if(me->lpClipper)
  {
    *lplpClipper = (LPDIRECTDRAWCLIPPER) me->lpClipper;
    return(DD_OK);
  }
  else
    return(DDERR_NOCLIPPERATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetColorKey(THIS This, DWORD dwFlags, LPDDCOLORKEY lpDDColKey)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

#ifdef DEBUG
  WriteLog("SurfGetColorKey\n");
#endif

  if ((0==dwFlags) || (NULL==lpDDColKey))
    return (DDERR_INVALIDPARAMS);

  // as we report only src colorkey in the caps return error on all others flags
  if( (DDCKEY_DESTBLT|DDCKEY_DESTOVERLAY|DDCKEY_SRCOVERLAY) & dwFlags)
    return(DDERR_UNSUPPORTED);

  if(me->DDSurfaceDesc.dwFlags & dwFlags)
  {
    if(DDCKEY_SRCBLT & dwFlags)
    {
      memcpy(lpDDColKey,&(me->DDSurfaceDesc.ddckCKSrcBlt),sizeof(DDCOLORKEY) );
    }
    else
      return (DDERR_INVALIDPARAMS); // some other flags where set => error
  }
  else
    return(DDERR_NOCOLORKEY); // surface doesn't have a color key set

  return (DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetDC(THIS This, HDC FAR *hdc)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 DDSURFACEDESC2        LockedSurfaceDesc;
 HRESULT rc;
 BITMAP bmpSurface;
 struct
 {
   BITMAPINFOHEADER  bmiHead;
   RGBQUAD           bmiCols[256];
 } BitmapInfo;

  #ifdef DEBUG
    WriteLog("SurfGetDC\n");
  #endif

  if (NULL==hdc)
    return(DDERR_INVALIDPARAMS);

  LockedSurfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

  if(DD_OK != me->Vtbl.Lock(me,NULL,&LockedSurfaceDesc,0,0))
  {
    return(DDERR_DCALREADYCREATED);
  }

  rc = DD_OK;

  if(me->hdcImage == NULL)
  {
    // Create a Device context
    me->hdcImage = CreateCompatibleDC(NULL);
    if(me->hdcImage == NULL)
    {
      #ifdef DEBUG
        WriteLog("Can't create compatible DC!\n");
      #endif
      me->Vtbl.Unlock(me,NULL);
      rc = DDERR_GENERIC;
    }
  }

  if( (DD_OK==rc) &&
      (me->hbmImage == NULL) )
  {
    WriteLog( "Trying to create Bitmap (%d/%d) at %d Bit\n",
              LockedSurfaceDesc.lPitch/ (LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount>>3),
              LockedSurfaceDesc.dwHeight,
              LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount
            );
    #if 0
    memset(&BitmapInfo, 0, sizeof(BitmapInfo));
    BitmapInfo.bmiHead.biSize   = sizeof(BITMAPINFOHEADER);
    BitmapInfo.bmiHead.biWidth  = LockedSurfaceDesc.lPitch/ (LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount>>3);
    BitmapInfo.bmiHead.biHeight = LockedSurfaceDesc.dwHeight;
    BitmapInfo.bmiHead.biPlanes = 1;
    BitmapInfo.bmiHead.biBitCount    = LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
    #else
    bmpSurface.bmType       = 0;
    bmpSurface.bmWidth      = LockedSurfaceDesc.dwWidth;
    bmpSurface.bmHeight     = LockedSurfaceDesc.dwHeight;
    bmpSurface.bmWidthBytes = LockedSurfaceDesc.lPitch;
    bmpSurface.bmPlanes     = 1;
    bmpSurface.bmBits       = LockedSurfaceDesc.lpSurface;
    #endif
    switch(LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
    {
      case 1:
      case 4:
        WriteLog("1/4 Bit Not yet supported\n");
        break;
      case 8:
        #if 0
        BitmapInfo.bmiHead.biCompression = BI_RGB;
        GetSystemPaletteEntries(me->hdcImage,0,255,(PPALETTEENTRY)&BitmapInfo.bmiCols[0]);
        me->hbmImage = CreateDIBitmap( me->hdcImage,
                                       NULL,
                                       CBM_CREATEDIB,
                                       LockedSurfaceDesc.lpSurface,
                                       (PBITMAPINFO)&BitmapInfo,
                                       DIB_RGB_COLORS);
        #else
        bmpSurface.bmBitsPixel  = 8;
        me->hbmImage = CreateBitmapIndirect( &bmpSurface);
        #endif
        break;
      case 16:
      case 32:
        WriteLog("16/32 Bit not supported by OS/2");
        break;
      case 24:
        #if 0
        BitmapInfo.bmiHead.biCompression = BI_RGB;
        me->hbmImage = CreateDIBitmap( me->hdcImage,
                                       NULL,
                                       CBM_CREATEDIB,
                                       LockedSurfaceDesc.lpSurface,
                                       (PBITMAPINFO)&BitmapInfo,
                                       DIB_RGB_COLORS);
        #else
        bmpSurface.bmBitsPixel  = 24;
        me->hbmImage = CreateBitmapIndirect( &bmpSurface);
        #endif
        break;
      default:
        #ifdef DEBUG
          WriteLog( "Unexptected BitCount %d \n",
                    LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount);
        #endif
        me->hbmImage=NULL;
    } // end switch (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)

    if(me->hbmImage == NULL)
    {
      #ifdef DEBUG
        WriteLog("Can't create bitmap!\n");
      #endif
      DeleteDC(me->hdcImage);
      me->hdcImage = NULL;
      me->Vtbl.Unlock(me,NULL);
      rc = DDERR_GENERIC;
    }
  }
  else
  {
    if( (DD_OK==rc) &&
        (me->dwLastDCUnique != me->dwUniqueValue) )
    {
      #ifdef DEBUG
        WriteLog("The Surface was locked/unlocked after the last DC was created =>Update Bitmap!\n");
      #endif

      memset(&BitmapInfo,0, sizeof(BitmapInfo));
      BitmapInfo.bmiHead.biSize     = sizeof(BITMAPINFOHEADER);
      BitmapInfo.bmiHead.biWidth    = LockedSurfaceDesc.lPitch/ (LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount>>3);
      BitmapInfo.bmiHead.biHeight   = LockedSurfaceDesc.dwHeight;
      BitmapInfo.bmiHead.biPlanes   = 1;
      BitmapInfo.bmiHead.biBitCount = LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

      switch(LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
      {
        case 1:
        case 4:
        case 8:
          BitmapInfo.bmiHead.biCompression = BI_RGB;
          GetSystemPaletteEntries(me->hdcImage,0,255,(PPALETTEENTRY)&BitmapInfo.bmiCols[0]);
          SetDIBits(me->hdcImage, me->hbmImage, 0, LockedSurfaceDesc.dwHeight,
                    me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
          break;
        case 16:
        case 32:
          BitmapInfo.bmiHead.biCompression = BI_BITFIELDS;
          BitmapInfo.bmiHead.biClrUsed     = 3;
          *((DWORD *) &(BitmapInfo.bmiCols[0])) = me->DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
          *((DWORD *) &(BitmapInfo.bmiCols[1])) = me->DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
          *((DWORD *) &(BitmapInfo.bmiCols[2])) = me->DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
          SetDIBits(me->hdcImage, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                    me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
          break;
        case 24:
          BitmapInfo.bmiHead.biCompression = BI_RGB;
          SetDIBits(me->hdcImage, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                    me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
          break;
        default:
          #ifdef DEBUG
            WriteLog("Unexptected BitCount %d => Bitmap not updated!\n",LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount);
          #endif
          break;
      } // end switch (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)

    }
  }

  // Allways select the bitmap into the DC! No matter if the old or a new one

  if(DD_OK==rc)
  {
    if((me->hgdiOld = SelectObject(me->hdcImage, me->hbmImage)) == NULL)
    {
      #ifdef DEBUG
        WriteLog("Can't select bitmap into dc!\n");
      #endif
      DeleteDC(me->hdcImage);
      me->hdcImage = NULL;
      DeleteObject(me->hbmImage);
      me->hbmImage = NULL;
      me->Vtbl.Unlock(me,NULL);
      rc = DDERR_GENERIC;
    }
    else
    {
      *hdc = me->hdcImage;
    }
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetFlipStatus(THIS This, DWORD dwFlags)
{
  #ifdef DEBUG
    WriteLog("SurfGetFlipStatus\n");
  #endif

  if( (DDGFS_CANFLIP!=dwFlags) && (DDGFS_ISFLIPDONE!=dwFlags) )
    return DDERR_INVALIDPARAMS;

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetOverlayPosition(THIS This, LPLONG lplX, LPLONG lplY)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  #ifdef DEBUG
    WriteLog("SurfGetOverlayPosition\n");
  #endif

  // Maybe simply return dderr_notsupported as we retun a max overlay value of 0 in the caps ?

  if( (NULL==lplX) || (NULL==lplY))
    return DDERR_INVALIDPARAMS;

  if(!(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_OVERLAY))
    return DDERR_NOTAOVERLAYSURFACE;

  if(!(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_VISIBLE))
    return DDERR_OVERLAYNOTVISIBLE;

  if(!me->fOverlayValid)
    return DDERR_NOOVERLAYDEST;

  *lplX = me->lOverlayX;
  *lplY = me->lOverlayY;

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetPalette(THIS This, LPDIRECTDRAWPALETTE FAR *lplpPalette)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfGetPalette\n");
  #endif

  if(me->lpPalette)
  {
    *lplpPalette = (LPDIRECTDRAWPALETTE)me->lpPalette;
    return(DD_OK);
  }
  else
    return(DDERR_NOPALETTEATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetPixelFormat(THIS This, LPDDPIXELFORMAT lpPixelFormat)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfGetPixelFormat\n");
  #endif

  if(NULL==lpPixelFormat)
    return DDERR_INVALIDPARAMS;

  memcpy( (char*)lpPixelFormat,
          (char*)&(me->DDSurfaceDesc.ddpfPixelFormat),
          sizeof(DDPIXELFORMAT));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetSurfaceDesc(THIS This, LPDDSURFACEDESC lpSurface)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfGetSurfaceDesc\n");
  #endif

  if((lpSurface == NULL)||(lpSurface->dwSize != sizeof(DDSURFACEDESC)) )
    return(DDERR_INVALIDPARAMS);

  memcpy( (char *)lpSurface,
          (char *)&me->DDSurfaceDesc,
          sizeof(DDSURFACEDESC));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetSurfaceDesc4(THIS This, LPDDSURFACEDESC2 lpSurface)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfGetSurfaceDesc4\n");
  #endif

  if((lpSurface == NULL)||(lpSurface->dwSize != sizeof(DDSURFACEDESC2)) )
    return(DDERR_INVALIDPARAMS);

  memcpy( (char *)lpSurface,
          (char *)&me->DDSurfaceDesc,
          sizeof(DDSURFACEDESC2));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfInitialize(THIS, LPDIRECTDRAW, LPDDSURFACEDESC)
{
  #ifdef DEBUG
    WriteLog("SurfInitialize\n");
  #endif

  return(DDERR_ALREADYINITIALIZED); // Init is done during creation see M$ Doc
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfInitialize4(THIS, LPDIRECTDRAW, LPDDSURFACEDESC2)
{
  #ifdef DEBUG
    WriteLog("SurfInitialize\n");
  #endif

  return(DDERR_ALREADYINITIALIZED); // Init is done during creation see M$ Doc
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfIsLost(THIS)
{
  // We don't loose any surface ;)
  // But we might shoud check for primary and/or Dive Buffers as I don't know
  // if  they are preserved if switching to a FS DOS/OS2 session
  //
  #ifdef DEBUG
    WriteLog("SurfIsLost\n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfLock( THIS This,
                            LPRECT lpRect,
                            LPDDSURFACEDESC lpSurfaceDesc,
                            DWORD dwFlags,
                            HANDLE hEvent)
{
  DDSURFACEDESC2 SurfaceDesc4;
  HRESULT rc;

  #ifdef DEBUG
    WriteLog("SurfLock %d %08X %d %d\n", (int)lpRect, (int)lpSurfaceDesc, dwFlags, hEvent);
  #endif

  if((NULL==lpSurfaceDesc)||(NULL!=hEvent))
    return DDERR_INVALIDPARAMS;

  if(lpSurfaceDesc->dwSize != sizeof(DDSURFACEDESC))
    return DDERR_INVALIDPARAMS;

  SurfaceDesc4.dwSize = sizeof(DDSURFACEDESC2);

  rc = SurfLock4( This,
                  lpRect,
                  &SurfaceDesc4,
                  dwFlags,
                  hEvent);
  if (DD_OK==rc)
  {
    memcpy( (char*)lpSurfaceDesc,
            (char*)&SurfaceDesc4,
            sizeof(DDSURFACEDESC) );
  }

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfLock4( THIS This,
                             LPRECT lpRect,
                             LPDDSURFACEDESC2 lpSurfaceDesc,
                             DWORD dwFlags,
                             HANDLE hEvent)
{

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;

  BOOL Found;
  ULONG  nrScanLines;
  char *pBuffer;
  DDRectangle *pIRectCurrent,*pIRectNew;
  static int times = 0;
  HRESULT rc;

  #ifdef DEBUG
    WriteLog("SurfLock4 %08X %08X %d %d\n", (int)lpRect, (int)lpSurfaceDesc, dwFlags, hEvent);
  #endif

  if( (NULL==lpSurfaceDesc) ||
      (NULL!=hEvent)
    )
    return DDERR_INVALIDPARAMS;


  if (NULL!=lpRect)
    pIRectNew = new DDRectangle( lpRect->left, lpRect->top, lpRect->bottom, lpRect->right );
  else
    pIRectNew = new DDRectangle( 0, 0, me->height, me->width);

  // ToDo : the lockchecking should be done in a critcal seq.
  WriteLog( "Lock Rectangle (%d/%d) x (%d/%d)\n",
            pIRectNew->Top(),
            pIRectNew->Left(),
            pIRectNew->Bottom(),
            pIRectNew->Right() );

  rc = DD_OK;

  if(me->fLocked)
  {
    if (NULL==lpRect)
    {
      // If anything is locked we can't locke the complete surface
      WriteLog("Surface has locked Rectangles, and we want to complete lock it\n");
      Found = TRUE;
    }
    else
    {
      // If the new Rectangle intersects with any of the already locked rectangles it can't
      // be locked so check for this

      WriteLog("Surface has locked Rectangles, check if the overlap\n");

      i=0;
      Found = FALSE;

      while(i<DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
      {
        pIRectCurrent = (DDRectangle*) DPA_FastGetPtr(me->DPA_LockedRects,i);
        WriteLog( "Test with Rectangle (%d/%d) x (%d/%d)\n",
                  pIRectCurrent->Top(),
                  pIRectCurrent->Left(),
                  pIRectCurrent->Bottom(),
                  pIRectCurrent->Right() );
        Found = pIRectCurrent->intersects(*pIRectNew);
        i++;
      }

    }

    if (Found)
    {
      delete pIRectNew;
      #ifdef DEBUG
        WriteLog("SurfLock4: Surface already locked\n\n");
      #endif
      rc = DDERR_SURFACEBUSY;
    }

  }

  if(DD_OK == rc)
  {
    memcpy((char *)lpSurfaceDesc, (char *)&me->DDSurfaceDesc, sizeof(DDSURFACEDESC2));

    if(lpRect != NULL)
    {
      lpSurfaceDesc->lpSurface =  (LPVOID)((char*)me->pFrameBuffer +
                                  (lpRect->top * me->dwPitchFB) +
                                  (lpRect->left * (lpSurfaceDesc->ddpfPixelFormat.dwRGBBitCount>>3)));
      #ifdef DEBUG
        WriteLog("SurfLock4 %08X (x,y) = (%d,%d)\n\n", lpSurfaceDesc->lpSurface, lpRect->top, lpRect->left);
      #endif
    }
    else
    {
      #ifdef DEBUG
        WriteLog("SurfLock4 %08X \n\n", lpSurfaceDesc->lpSurface);
      #endif
    }
    // Add the rectangle to the list of locked rectangles

    pIRectNew->SetMemPtr(lpSurfaceDesc->lpSurface);

    DPA_InsertPtr( me->DPA_LockedRects,
                   DPA_GetPtrCount(me->DPA_LockedRects),
                   pIRectNew);

    me->fLocked = TRUE;
  }


  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfReleaseDC(THIS This, HDC hdc)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 struct
 {
   BITMAPINFOHEADER  bmiHead;
   RGBQUAD           bmiCols[256];
 } BitmapInfo;
 BITMAP bmpData;
 char szError[256];
 int i,rc;

  #ifdef DEBUG
    WriteLog("SurfReleaseDC\n");
  #endif

  if(hdc != me->hdcImage)
    return(DDERR_INVALIDOBJECT);

  #if 1
  //unselect our bitmap
  SelectObject(me->hdcImage, me->hgdiOld);
  memset(&BitmapInfo,0, sizeof(BitmapInfo));
  BitmapInfo.bmiHead.biSize     = sizeof(BITMAPINFOHEADER);
  BitmapInfo.bmiHead.biWidth    = me->DDSurfaceDesc.lPitch/ (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount>>3);
  BitmapInfo.bmiHead.biHeight   = me->DDSurfaceDesc.dwHeight;
  BitmapInfo.bmiHead.biPlanes   = 1;
  BitmapInfo.bmiHead.biBitCount = 0; // me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

  switch(me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
  {
    case 1:
    case 4:
    case 8:
      BitmapInfo.bmiHead.biCompression = BI_RGB;
      //GetSystemPaletteEntries(me->hdcImage,0,255,(PPALETTEENTRY)&BitmapInfo.bmiCols[0]);
      rc = GetDIBits(hdc, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                NULL,(PBITMAPINFO)&BitmapInfo,DIB_PAL_COLORS);
      WriteLog( "GetDIBits rc=%d\n Size   :%d\n Width  :%d\n Height :%d\n"
                " Planes :%d\n BitCount :%d\nLastEror = %d\n",
                rc,
                BitmapInfo.bmiHead.biSize,
                BitmapInfo.bmiHead.biWidth,
                BitmapInfo.bmiHead.biHeight,
                BitmapInfo.bmiHead.biPlanes,
                BitmapInfo.bmiHead.biBitCount,
                GetLastError());
      /*
      rc = GetDIBits(hdc, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_PAL_COLORS);
      WriteLog( "GetDIBits rc=%d\n LastEror = %d\n",
                rc,
                GetLastError());
      */
      break;
    case 16:
    case 32:
      BitmapInfo.bmiHead.biCompression = BI_BITFIELDS;
      BitmapInfo.bmiHead.biClrUsed     = 3;
      *((DWORD *) &(BitmapInfo.bmiCols[0])) = me->DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
      *((DWORD *) &(BitmapInfo.bmiCols[1])) = me->DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
      *((DWORD *) &(BitmapInfo.bmiCols[2])) = me->DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
      GetDIBits(hdc, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
      break;
    case 24:
      BitmapInfo.bmiHead.biCompression = BI_RGB;
      GetDIBits(hdc, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
      break;
    default:
      #ifdef DEBUG
        WriteLog("Unexptected BitCount %d => Surface unlocked but no data copied back\n",me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount);
      #endif
      // we might could keep the surface locked and return an error but this is more "safe"
      break;
  } // end switch (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
  #else

  rc = GetObjectA( me->hbmImage,
                  sizeof(BITMAP),
                  &bmpData);
  WriteLog( "GetObject returned rc=%d\n BitmapInfo:\n Size:(%dx%d)\n Pitch: %d\n Bits %d\n @mem %08X",
            rc,
            bmpData.bmWidth, bmpData.bmHeight,
            bmpData.bmWidthBytes,
            bmpData.bmBitsPixel,
            bmpData.bmBits);
  #endif
  me->Vtbl.Unlock(me,NULL);
  me->dwLastDCUnique = me->dwUniqueValue; // Store this to see if the surface was locked after we released the DC

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfRestore(THIS)
{
  #ifdef DEBUG
    WriteLog("SurfRestore\n");
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetClipper(THIS This, LPDIRECTDRAWCLIPPER lpClipper)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfSetClipper\n");
  #endif

  if(lpClipper == NULL)
  {
    //deattach surface
    if(me->lpClipper)
    {
      me->lpClipper->Vtbl.Release((IDirectDrawClipper*)me->lpClipper);
      me->lpClipper = NULL;
      return(DD_OK);
    }
    else
      return(DDERR_NOCLIPPERATTACHED);
  }

  if(lpClipper == (LPDIRECTDRAWCLIPPER)me->lpClipper)
    return(DD_OK);  //already attached

  if(me->lpClipper != NULL)
  {
    me->lpClipper->Vtbl.Release((IDirectDrawClipper*)me->lpClipper);  //attach other surface
    return(DD_OK);
  }

  me->lpClipper = (OS2IDirectDrawClipper *)lpClipper;
  me->lpClipper->Vtbl.AddRef((IDirectDrawClipper*)me->lpClipper);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetColorKey(THIS  This, DWORD dwFlags, LPDDCOLORKEY lpDDColKey)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  HRESULT rc;
  #ifdef DEBUG
    WriteLog("SurfSetColorKey\n");
  #endif

  if (0==dwFlags)
    return (DDERR_INVALIDPARAMS);

  // as we report only src colorkey in the caps return error on all others flags
  if( (DDCKEY_DESTBLT|DDCKEY_DESTOVERLAY|DDCKEY_SRCOVERLAY|DDCKEY_COLORSPACE) & dwFlags)
    return(DDERR_UNSUPPORTED);

  if(DDCKEY_SRCBLT & dwFlags)
  {
    me->lpVtbl->ChangeUniquenessValue(me); // we changed somethin so change this value
    if(NULL!=lpDDColKey)
    {
      memcpy(&(me->DDSurfaceDesc.ddckCKSrcBlt), lpDDColKey, sizeof(DDCOLORKEY) );
      me->DDSurfaceDesc.dwFlags |= DDCKEY_SRCBLT;

      // ToDo: Generate a maskbitmap for transparent blitting here
    }
    else
    {
      memset(&(me->DDSurfaceDesc.ddckCKSrcBlt), 0, sizeof(DDCOLORKEY) );
      me->DDSurfaceDesc.dwFlags &= ~DDCKEY_SRCBLT;
    }
    rc = DD_OK;
  }
  else
    rc = DDERR_INVALIDPARAMS; // some other flags where set => error

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetOverlayPosition(THIS This, LONG lX, LONG lY)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfSetOverlayPosition\n");
  #endif

  if( (me->DDSurfaceDesc.dwFlags & DDSD_CAPS) &&
      (me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_OVERLAY) )
  {
    if(me->fOverlayValid)
      return(DDERR_NOOVERLAYDEST);

    if(!(me->DDSurfaceDesc.dwFlags & DDSCAPS_VISIBLE))
      return(DDERR_OVERLAYNOTVISIBLE);

    // ToDo: If we implement alignment restricions to the Overlay position
    // check if the new values are OK otherwiese return DDERR_INVALIDPOSITION

    me->lOverlayX = lX;
    me->lOverlayY = lY;
    return(DD_OK);
  }

  return(DDERR_NOTAOVERLAYSURFACE);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetPalette(THIS This, LPDIRECTDRAWPALETTE lpPalette)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfSetPalette\n");
  #endif

  if(lpPalette == NULL)
  {
    //deattach palette
    if(me->lpPalette)
    {
      // If removed from a primary surface notify
      // palette that it is no longer attached to the
      // primary surface => doesn't modify physical palette
      if(me->surfaceType & DDSCAPS_PRIMARYSURFACE)
      {
        me->lpPalette->SetIsPrimary(FALSE);
      }
      me->lpPalette->Vtbl.Release((IDirectDrawPalette*)me->lpPalette);
      me->lpPalette = NULL;
      return(DD_OK);
    }
    else
      return(DDERR_NOPALETTEATTACHED);
  }

  if(lpPalette == (LPDIRECTDRAWPALETTE)me->lpPalette)
    return(DD_OK);  //already attached

  if(me->lpPalette != NULL)
  {
    me->lpPalette->Vtbl.Release((IDirectDrawPalette*)me->lpPalette);  //attach other palette
    return(DD_OK);
  }
  me->lpPalette = (OS2IDirectDrawPalette *)lpPalette;
  me->lpPalette->Vtbl.AddRef((IDirectDrawPalette*)me->lpPalette);

  // If Attached to a primary surface notify
  // palette that it is attached to the primary surface
  //  => It does modify physical palette.
  // This is important as an palette can be attached to
  // multiple surfaces. If one is the primary surface
  // changes done to it via any surface must result in
  // changes in the phys pal.

  if(me->surfaceType & DDSCAPS_PRIMARYSURFACE)
  {
    me->lpPalette->SetIsPrimary(TRUE);
  }
  me->lpVtbl->ChangeUniquenessValue(me);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUnlock(THIS This, LPVOID lpSurfaceData)
{

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  DDRectangle *pIRectUnlock, *pIRectEnum;
  BOOL Found = FALSE;
  OS2RECTL CCRect;
  HRESULT rc;

  #ifdef DEBUG
    WriteLog("SurfUnlock at %08X\n",lpSurfaceData);
  #endif

  if(me->fLocked == FALSE)
  {
    #ifdef DEBUG
      WriteLog("Surface not locked!\n");
    #endif
    return(DDERR_NOTLOCKED);
  }

  #ifdef DEBUG
    WriteLog("Start Emuneration of Locked Rects\n");
  #endif

  i=0;
  if(NULL!=lpSurfaceData)
  {
    WriteLog("Buffer Pointer Compare");

    // We got a pinter to the surface memory so we must search for
    // this pointer in the locked rects DPA to unlock the right rect.

    while(i<DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
    {
      pIRectEnum = (DDRectangle*)DPA_FastGetPtr(me->DPA_LockedRects,i);
      WriteLog( "Test with Rectangle %d (%d/%d) x (%d/%d) Mem at %08X\n",
                i,
                pIRectEnum->Top(),
                pIRectEnum->Left(),
                pIRectEnum->Bottom(),
                pIRectEnum->Right(),
                pIRectEnum->GetMemPtr() );

      Found = ( pIRectEnum->GetMemPtr() == lpSurfaceData);
      if(!Found)
      {
        #ifdef DEBUG
          WriteLog("Not Found, try Next rect\n");
        #endif
        i++;
      }
      #ifdef DEBUG
      else
      {
        WriteLog("Found Rect\n");
      }
      #endif
    }
  }
  else
  {
    // If a NULL pointer was passed in the SW tries to unlock the
    // complete surface so we must compare the rects.
    WriteLog("Rectangle compare");

    pIRectUnlock = new DDRectangle( 0, 0, me->height, me->width);

    while(i<DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
    {
      pIRectEnum = (DDRectangle*)DPA_FastGetPtr(me->DPA_LockedRects,i);
      WriteLog( "Test with Rectangle %d (%d/%d) x (%d/%d) Mem at %08X\n",
                i,
                pIRectEnum->Top(),
                pIRectEnum->Left(),
                pIRectEnum->Bottom(),
                pIRectEnum->Right(),
                pIRectEnum->GetMemPtr() );

      Found = (*pIRectEnum == *pIRectUnlock);
      if(!Found)
      {
        #ifdef DEBUG
          WriteLog("Not Found, try Next rect\n");
        #endif
        i++;
      }
      #ifdef DEBUG
      else
      {
        WriteLog("Found Rect\n");
      }
      #endif
    }
    delete pIRectUnlock;
  }

  if(!Found)
  {
    #ifdef DEBUG
      WriteLog("Rectangle not locked, wrong Rect!\n\n");
    #endif
    rc = DDERR_INVALIDRECT;
  }
  else
  {
    #ifdef DEBUG
      WriteLog("Remove Rect %d from Seq.\n",i);
    #endif

    DPA_DeletePtr(me->DPA_LockedRects,i);

    #ifdef DEBUG
      WriteLog("Test if locked Rects main\n");
    #endif

    if(0==DPA_GetPtrCount(me->DPA_LockedRects))  // Do we have unlocked last rectangle
    {
      #ifdef DEBUG
        WriteLog("No Locked Rects left for surface\n");
      #endif
      me->fLocked = FALSE;
    }
    #ifdef DEBUG
    else
      WriteLog( "%d Rects in Seq\n",
                DPA_GetPtrCount(me->DPA_LockedRects));
    #endif

    if(me->pFrameBuffer != me->pDiveBuffer)
    {
      #ifdef DEBUG
        WriteLog( "ColorConversion Needed %08X != %08X\n",
                  me->pFrameBuffer,
                  me->pDiveBuffer);
      #endif
      if(NULL!=lpSurfaceData)
      {
        CCRect.yTop    = pIRectEnum->Top();
        CCRect.xLeft   = pIRectEnum->Left();
        CCRect.xRight  = pIRectEnum->Right();
        CCRect.yBottom = pIRectEnum->Bottom();

        me->ColorConversion( (LPRECT)&CCRect);
      }
      else
      {
        me->ColorConversion(NULL);
      }
    }
    #ifdef DEBUG
    else
      WriteLog( "No ColorConversion Needed");
    #endif

    // delete tne DDRectobject of the found rectangle
    delete pIRectEnum;

    me->lpVtbl->ChangeUniquenessValue(me);

    WriteLog("Unlock OK\n\n");

    rc = DD_OK;
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUnlock4(THIS This, LPRECT lpSurfaceRect)
{
  // MS changed the parameter from LPVOID to LPRECT with DX6
  // as DX-6 allways returns a DDSurface4 on create surface this
  // is a problem i not NULL is passed in.
  // Solution We first test with the pointer ns assuming a rectangle
  // if we don't find a rect Which is very likely if we get a pointer
  // SurfaceMemory we call SurfUnlock and test for the pointer there.


  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  DDRectangle *pIRectUnlock, *pIRectEnum;
  BOOL Found = FALSE;
  HRESULT rc;

  #ifdef DEBUG
    WriteLog("SurfUnlock4\n");
  #endif

  if(me->fLocked == FALSE)
  {
    #ifdef DEBUG
      WriteLog("Surface not locked!\n");
    #endif
    return(DDERR_NOTLOCKED);
  }

  if(NULL!=lpSurfaceRect)
  {
    WriteLog("Unlock rectangle\n");
    pIRectUnlock = new DDRectangle( lpSurfaceRect->top,
                                    lpSurfaceRect->left,
                                    lpSurfaceRect->bottom,
                                    lpSurfaceRect->right);
  }
  else
  {
    WriteLog("Unlock complete surface\n");
    pIRectUnlock = new DDRectangle( 0, 0, me->height, me->width);
  }

  WriteLog( "Try to Unlock Rectangle (%d/%d) x (%d/%d)\n",
            pIRectUnlock->Top(),
            pIRectUnlock->Left(),
            pIRectUnlock->Bottom(),
            pIRectUnlock->Right() );

  #ifdef DEBUG
    WriteLog("Start Emuneration of Locked Rects\n");
  #endif

  i=0;
  while(i<DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
  {
    pIRectEnum = (DDRectangle*)DPA_FastGetPtr(me->DPA_LockedRects,i);
    WriteLog( "Test with Rectangle (%d/%d) x (%d/%d)\n",
              pIRectEnum->Top(),
              pIRectEnum->Left(),
              pIRectEnum->Bottom(),
              pIRectEnum->Right() );

    Found = (*pIRectEnum == *pIRectUnlock);
    if(!Found)
    {
      #ifdef DEBUG
        WriteLog("Not Found, try Next rect\n");
      #endif
      i++;
    }
    #ifdef DEBUG
    else
    {
      WriteLog("Found Rect\n");
    }
    #endif
  }

  if(!Found)
  {

    if(NULL==lpSurfaceRect)
    {
      #ifdef DEBUG
        WriteLog("Rectangle not locked, wrong Rect!\n\n");
      #endif
      return(DDERR_INVALIDRECT);
    }
    else
      rc = SurfUnlock(This, (LPVOID)lpSurfaceRect);
  }
  else
  {
    #ifdef DEBUG
      WriteLog("Remove Rect from Seq.\n");
    #endif

    DPA_DeletePtr(me->DPA_LockedRects,i);

    if(0==DPA_GetPtrCount(me->DPA_LockedRects))  // Do we have unlocked last rectangle
    {
      #ifdef DEBUG
        WriteLog("No Locked Rects left for surface\n");
      #endif
      me->fLocked = FALSE;
    }

    if(me->pFrameBuffer != me->pDiveBuffer)
    {
      #ifdef DEBUG
        WriteLog( "ColorConversion Needed %08X != %08X\n",
                  me->pFrameBuffer,
                  me->pDiveBuffer);
      #endif
      me->ColorConversion(lpSurfaceRect);
    }

    me->lpVtbl->ChangeUniquenessValue(me);

    WriteLog("Unlock OK\n\n");
    rc = DD_OK;
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlay(THIS This, LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX)
{
  #ifdef DEBUG
    WriteLog("SurfUpdateOverlay\n");
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlay4(THIS, LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX)
{
  #ifdef DEBUG
    WriteLog("SurfUpdateOverlay\n");
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlayDisplay(THIS, DWORD)
{
  #ifdef DEBUG
    WriteLog("SurfUpdateOverlayDisplay\n");
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlayZOrder(THIS, DWORD, LPDIRECTDRAWSURFACE2)
{
  #ifdef DEBUG
    WriteLog("SurfUpdateOverlayZOrder\n");
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlayZOrder4(THIS, DWORD, LPDIRECTDRAWSURFACE4)
{
  #ifdef DEBUG
    WriteLog("SurfUpdateOverlayZOrder4\n");
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetDDInterface(THIS This, LPVOID FAR *lplpDirectDraw)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  #ifdef DEBUG
    WriteLog("SurfGetDDInterface\n");
  #endif
  *lplpDirectDraw = (LPVOID FAR *)me->lpDraw;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfPageLock(THIS, DWORD)
{
  // Only used for DMA memory access
  // If we implement this for the None dive buffers with a pdd the we must change
  // from malloc to DosAllocMem and use OBJ_TILE flag
  #ifdef DEBUG
    WriteLog("SurfPageLock\n");
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfPageUnlock(THIS, DWORD)
{
  #ifdef DEBUG
    WriteLog("SurfPageUnlock\n");
  #endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
// V3 Interface Functions

HRESULT __stdcall SurfSetSurfaceDesc(THIS This, LPDDSURFACEDESC lpSurfDesc, DWORD dwFlags)
{
  #ifdef DEBUG
    WriteLog("SurfSetSurfaceDesc\n");
  #endif

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  if ( (NULL==lpSurfDesc) || (dwFlags!=0) )
    return DDERR_INVALIDPARAMS;

  // Is this ok ? Not sure if Front/BackBuffer should be handled like the Primary one.
  //  if ( ( DDSCAPS_PRIMARYSURFACE == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_FRONTBUFFER == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_BACKBUFFER  == me->DDSurfaceDesc.ddsCaps.dwCaps) )
  if(-1==me->diveBufNr)
    return DDERR_INVALIDSURFACETYPE; // only work for system alloced surfaces

  if (!me->Updated)
  {
    me->Updated = TRUE;
    // free our allocated Memory
    if(me->DDSurfaceDesc.dwFlags & DDSD_LPSURFACE)
      free(me->DDSurfaceDesc.lpSurface);
  }
  me->lpVtbl->ChangeUniquenessValue(me);
  memcpy((char *)&(me->DDSurfaceDesc), (char *)lpSurfDesc, sizeof(DDSURFACEDESC));

  return DD_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetSurfaceDesc4(THIS This, LPDDSURFACEDESC2 lpSurfDesc, DWORD dwFlags)
{
  #ifdef DEBUG
    WriteLog("SurfSetSurfaceDesc4\n");
  #endif

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  if ( (NULL==lpSurfDesc) || (dwFlags!=0) )
    return DDERR_INVALIDPARAMS;

  // Is this ok ? Not sure if Front/BackBuffer should be handled like the Primary one.
  //  if ( ( DDSCAPS_PRIMARYSURFACE == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_FRONTBUFFER == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_BACKBUFFER  == me->DDSurfaceDesc.ddsCaps.dwCaps) )
  if(-1==me->diveBufNr)
    return DDERR_INVALIDSURFACETYPE; // only work for system alloced surfaces

  if (!me->Updated)
  {
    me->Updated = TRUE;
    // free our allocated Memory
    if(me->DDSurfaceDesc.dwFlags & DDSD_LPSURFACE)
      free(me->DDSurfaceDesc.lpSurface);
  }
  me->lpVtbl->ChangeUniquenessValue(me);
  memcpy((char *)&(me->DDSurfaceDesc), (char *)lpSurfDesc, sizeof(DDSURFACEDESC2));

  return DD_OK;
}
//******************************************************************************
//******************************************************************************
// V4 Interface Functions

HRESULT __stdcall SurfSetPrivateData(THIS This, REFGUID refGUID, LPVOID lpData,
                                     DWORD dwDataSize, DWORD dwFlags)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;
  void *pBuffer;
  BOOL bFound = FALSE;
  HRESULT rc;

  #ifdef DEBUG
    WriteLog("SurfSetPrivateData\n");
  #endif

  if(NULL==me)
    return(DDERR_INVALIDOBJECT);

  if((NULL==lpData)||(0==dwDataSize)||
     (dwFlags & ~(DDSPD_IUNKNOWNPOINTER|DDSPD_VOLATILE)))
    return(DDERR_INVALIDPARAMS);

  // first check if the refGUID is stored as then the content will be updated
  if( DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0 )
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData) && !bFound)
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);

      if (IsEqualGUID(pSData->guidTag,refGUID))
        bFound = TRUE;

      i++;
    }
  }

  if(bFound)
  {
    // update Private Data

    if (!pSData->isValid)
    {
      // Current data is invalid we need to update/allocate

      if(dwFlags & DDSPD_IUNKNOWNPOINTER)
      {
        pSData->pData = lpData;
        pSData->dwSize = 4;
        pSData->dwFlags = dwFlags;
        pSData->isValid = TRUE;
        ((OS2IDirectDrawSurface *) lpData)->lpVtbl->AddRef(lpData);
      }
      else
      {
        pSData->pData = malloc(dwDataSize);
        if(NULL!=pSData->pData)
        {
          memcpy(pSData->pData,lpData,dwDataSize);
          pSData->dwSize  = dwDataSize;
          pSData->dwFlags = dwFlags;
          pSData->isValid = TRUE;
        }
        else
        {
          delete pSData;
          rc = DDERR_OUTOFMEMORY;
        }
      }
    }
    else
    {
      if(pSData->dwFlags & DDSPD_IUNKNOWNPOINTER)
      {
        if(dwFlags & DDSPD_IUNKNOWNPOINTER)
        {
          if(pSData->pData != lpData)
          {
            // Change of IUNKOWNPOINTER => release old and add ref to new one
            ((OS2IDirectDrawSurface *)pSData->pData)->lpVtbl->Release(pSData->pData);
            ((OS2IDirectDrawSurface *)lpData)->lpVtbl->AddRef(lpData);
            pSData->pData = lpData;
          }
          pSData->dwFlags = dwFlags; // Update the flags, size is the same
        }
        else
        {
          // Replace IUNKOWN through data
          pBuffer = malloc(dwDataSize); // get new buffer first
          if(NULL!=pBuffer)
          {
            // release old ref and copy data
            ((OS2IDirectDrawSurface *)pSData->pData)->lpVtbl->Release(pSData->pData);
            memcpy(pBuffer,lpData,dwDataSize);
            pSData->pData = pBuffer;
            pSData->dwSize = dwDataSize; // Update the size
            pSData->dwFlags = dwFlags;   // Update the flags
          }
          else
            rc = DDERR_OUTOFMEMORY;
        }
      }
      else
      {
        if(dwFlags & DDSPD_IUNKNOWNPOINTER)
        {
          // Change of data to IUNKOWNPOINTER => free old memory and add ref to new one
          free(pSData->pData);
          ((OS2IDirectDrawSurface *)lpData)->lpVtbl->AddRef(lpData);
          pSData->pData = lpData;
          pSData->dwSize = dwDataSize; // Update the size
          pSData->dwFlags = dwFlags;   // Update the flags
        }
        else
        {
          //  Update/Replace data
          if(pSData->dwSize!=dwDataSize)
            pBuffer = realloc(pSData->pData,dwDataSize); // update buffer to new size
          else
            pBuffer = pSData->pData;

          if(NULL!=pBuffer)
          {
            // release old ref and copy data
            memcpy(pBuffer,lpData,dwDataSize);
            pSData->pData = pBuffer;
            pSData->dwSize = dwDataSize; // Update the size
            pSData->dwFlags = dwFlags;   // Update the flags
          }
          else
            rc = DDERR_OUTOFMEMORY;
        }
      }
    }
  }
  else
  {
    // New data

    pSData = new(SURFPRIVATEDATA);
    if (NULL!=pSData)
    {
      if(dwFlags & DDSPD_IUNKNOWNPOINTER)
      {
        memcpy(&(pSData->guidTag),&refGUID,sizeof(GUID));
        pSData->pData = lpData;
        pSData->dwSize = 4;
        pSData->dwFlags = dwFlags;
        pSData->isValid = TRUE;
        ((OS2IDirectDrawSurface *)lpData)->lpVtbl->AddRef(lpData);
      }
      else
      {
        pSData->pData = malloc(dwDataSize);
        if(NULL!=pSData->pData)
        {
          memcpy(&(pSData->guidTag),&refGUID,sizeof(GUID));
          memcpy(pSData->pData,lpData,dwDataSize);
          pSData->dwSize  = dwDataSize;
          pSData->dwFlags = dwFlags;
          pSData->isValid = TRUE;

          if( DPA_InsertPtr( me->DPA_SurfacePrivateData,
                             DPA_GetPtrCount(me->DPA_SurfacePrivateData),
                             pSData) <0)
          {
            delete(pSData);
            rc = DDERR_OUTOFMEMORY;
          }
        }
        else
        {
          delete(pSData);
          rc = DDERR_OUTOFMEMORY;
        }
      }

    }
    else
      rc = DDERR_OUTOFMEMORY;
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetPrivateData(THIS This, REFGUID refGUID, LPVOID lpData, LPDWORD lpDataSize)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;
  HRESULT rc;
  BOOL bFound = FALSE;

  #ifdef DEBUG
    WriteLog("SurfGetPrivateData\n");
  #endif

  if(NULL==me)
    return(DDERR_INVALIDOBJECT);

  if((NULL==lpData)||(NULL==lpDataSize))
    return(DDERR_INVALIDPARAMS);

  if(DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0)
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData) && !bFound)
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);

      if (IsEqualGUID(pSData->guidTag,refGUID))
        bFound = TRUE;

      i++;
    }
  }

  if(bFound)
  {
    if(!pSData->isValid)
    {
      rc =DDERR_EXPIRED;
    }
    else
    {
      if(pSData->dwSize > *lpDataSize)
      {
        // Buffer to small return needed Size
        *lpDataSize = pSData->dwSize;
        rc = DDERR_MOREDATA;
      }
      else
      {
        memcpy(lpData,pSData->pData,pSData->dwSize);
        rc = DD_OK;
      }
    }
  }
  else
   rc = DDERR_NOTFOUND;


  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfFreePrivateData(THIS This, REFGUID refGUID)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;
  BOOL bFound = FALSE;

  #ifdef DEBUG
    WriteLog("SurfFreePrivateData\n");
  #endif

  if(NULL==me)
    return(DDERR_INVALIDOBJECT);

  if(DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0)
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData) && !bFound)
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);

      if (IsEqualGUID(pSData->guidTag,refGUID))
      {
        bFound = TRUE;

        if(pSData->isValid)
        {
          // delete the data if valid
          if (pSData->dwFlags & DDSPD_IUNKNOWNPOINTER)
          {
            // pointer to com stored so calll its release
            ((OS2IDirectDrawSurface *) pSData->pData)->lpVtbl->Release(pSData->pData);
          }
          else
          {
            // Free allocated data
            free( pSData->pData);
          }
        }
        // Now remove the entry from the list
        DPA_DeletePtr(me->DPA_SurfacePrivateData,i);
      }
      i++;
    }
  }

  return (bFound?DD_OK:DDERR_NOTFOUND);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetUniquenessValue(THIS This, LPDWORD lpValue)
{
  #ifdef DEBUG
    WriteLog("SurfGetUniquenessValue\n");
  #endif
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  if (NULL==lpValue)
    return DDERR_INVALIDPARAMS;

  *lpValue = me->dwUniqueValue;
  return DD_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfChangeUniquenessValue(THIS This)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;


  #ifdef DEBUG
    WriteLog("SurfChangeUniquenessValue\n");
  #endif
  me->dwUniqueValue++;

  if(DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0)
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData))
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);
      if (pSData->dwFlags & DDSPD_VOLATILE)
      {
        // Data becomes unvalid after a Surface change
        if (pSData->dwFlags & DDSPD_IUNKNOWNPOINTER)
        {
          // pointer to com stored so call its release
          ((OS2IDirectDrawSurface *) pSData->pData)->lpVtbl->Release(pSData->pData);
        }
        else
        {
          // Free allocated data
          free( pSData->pData);
        }
        pSData->pData = NULL;
        pSData->isValid = FALSE; // set flag to invalid
      }
      i++;
    }
  }

  return (DD_OK);
}

//******************************************************************************
//
//  Purpose function copies one part of the bitmap inside the same bitmap
//
//******************************************************************************

void __cdecl MoveRects(char* pBuffer, LPRECT lpDestRect, LPRECT lpSrcRect, int bpp, LONG lPitch)
{

  char *pBltPos, *pSrcPos;
  int BlitWidth,BlitHeight;
  static char Scanline[6400];  // sufficient for 1600 at 32 bit

  // Bridge, we may got a problem ;)
  // Check for Overlapping Rects

  pBltPos = pBuffer;
  pSrcPos = pBuffer;

  if(lpDestRect->top <= lpSrcRect->top)
  {
    //  +-------+     +-------+      +-------+
    //  |S      |     |S      |      |S      |  +---+---+---+
    //  |   +---|---+ +-------+  +---|---+   |  |S/D|D/S|   |
    //  |   | D |   | | D     |  | D |   |   |  |   |   |   |
    //  +-------+   | +-------+  |   +-------+  |   |   |   |
    //      |       | |       |  |       |      +---+---+---+
    //      +-------+ +-------+  +-------+
    //
    // We got one of the above cases (or no overlapping) so copy from bottom up

    pBltPos += (lpDestRect->left * bpp) + lPitch * lpDestRect->top;
    pSrcPos += (lpSrcRect->left * bpp)  + lPitch * (lpSrcRect->bottom-1);
    BlitHeight = lpDestRect->bottom - lpDestRect->top;
    BlitWidth  = (lpDestRect->right - lpDestRect->left) * bpp;

    while(1)
    {
      memcpy(Scanline,pSrcPos,BlitWidth);
      memcpy(pBltPos,Scanline,BlitWidth);
      pBltPos += lPitch;
      pSrcPos -= lPitch;
      if(! (--BlitHeight))
        break;
    }
  }
  else
  {
    //  +-------+     +-------+      +-------+
    //  | D     |     | D     |      | D     |
    //  |   +---|---+ +-------+  +---|---+   |
    //  |   |S  |   | |S      |  |S  |   |   |
    //  +-------+   | +-------+  |   +-------+
    //      |       | |       |  |       |
    //      +-------+ +-------+  +-------+
    //
    // We got one of the above cases so copy top down

    pBltPos += (lpDestRect->left * bpp) + lPitch * lpDestRect->top;
    pSrcPos += (lpSrcRect->left  * bpp) + lPitch * lpSrcRect->top;
    BlitHeight = lpDestRect->bottom - lpDestRect->top;
    BlitWidth  = (lpDestRect->right - lpDestRect->left) * bpp;

    while(1)
    {
      memcpy(Scanline,pSrcPos,BlitWidth);
      memcpy(pBltPos,Scanline,BlitWidth);
      pBltPos += lPitch;
      pSrcPos += lPitch;
      if(! (--BlitHeight))
        break;
    }
  }

}

//******************************************************************************
//
//  Purpose : Do a blit using the precalced Transbuffer
//            That is the only way to do fast blits if a colorrange is used
//            and we can find totally transparent lines and don't blit them
//            and detect if the part of the line is transparent
//
//            Idea for a kind of mask buffer
//            Format of Transparentbuffer (each line):
//            the first DWORD contains 2 WORDS with Offset of First Non transparent
//            pixel in the low word and the last non transparent pixel in a row in
//            the high word. => 0 = line totally transparent!
//            This limits the max supported width to 2^16 but I thing this is enougth
//            The size per line is 1+((Width+31) & ~31) DWORDS => each Bit represents
//            1 pixel
//
//            TransparentBufferCreate(lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight);
//
//          Layout of a DWORD:
//             UUVVWWXX  dword is processed LS Byte FIRST ...
//             Each bit in a byte stands for one pixel. MS Bit First
//
//          example: Bitmap (16x5) (X= opaque, . = Transparent)
//                   ...XX...XX....XX
//                   ..XXXX....XXXX..
//                   ................
//                   .........XXXXXX.
//                   ...XX...X......X
//
//                   Transparent buffer (2DWORDS) per line
//
//         0x00100003, 0x0000C318
//         0x000E0002, 0x00003C3C
//         0x00000000, 0x00000000
//         0x000F000A, 0x00007E00
//         0x00100003, 0x00008118
//******************************************************************************

void __cdecl TransSRCBlit8(LPDDSURFACEDESC2 pDestDesc, LPDDSURFACEDESC2 pSrcDesc,  char *pAlpha, LPRECT lpSrcRect)
{
  DWORD *pdwTLine; // pointer to the transparent buffer
  DWORD dwTLineLen;      // # of DWORDS in each tBuffer line
  DWORD dwTLineStart;     // # DWORD in which the first transinfo is
  DWORD dwTDWStart;       // byte in which the firs transinfo is
  DWORD dwTrans;          // current transparentvalue
  DWORD BlitWidth;
  dwTLineLen = 1 + ((pSrcDesc->dwWidth + 31) & ~31);
  pdwTLine = (DWORD*)pAlpha + (dwTLineLen* lpSrcRect->top);
  dwTLineStart = 1+(lpSrcRect->left/32);
  dwTDWStart   = (lpSrcRect->left+8)/8;
}


