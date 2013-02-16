
#define _OS2WIN_H
#define FAR

#include <odin.h>
#include <winbase.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                         \
                  ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                  ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#include <fourcc.h>

#define CINTERFACE
#include "ddraw2d.h"
#include "clipper.h"
#include "palette.h"
#include "surface.h"
#include "surfacehlp.h"
#include "os2util.h"
#include "rectangle.h"

#include <misc.h>
#include "asmutil.h"
#include "bltFunc.h"
#include "colorconv.h"
#include "fillfunc.h"
#include <winerror.h>
#include <os2win.h>
#include <cpuhlp.h>
#include "asmutil.h"

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
DWORD dwDefaultPalete24[256];
BOOL fPalInit=FALSE;

#define CBM_CREATEDIB   0x02L   /* create DIB bitmap */

//#define PERFTEST
#ifdef PERFTEST
LARGE_INTEGER liStart;
LARGE_INTEGER liEnd;
ULONG average = 0;
#endif

#ifdef DEBUG

// ******************************************************************************
// *    internal helper functions from WINE
// *

void _dump_DDBLTFX(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
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
        dprintf(("DDRAW: %s ",flags[i].name));
     };
  dprintf(("DDRAW: \n"));

}

void _dump_DDBLTFAST(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
  } flags[] = {
#define FE(x) { x, #x},
    FE(DDBLTFAST_NOCOLORKEY)
    FE(DDBLTFAST_SRCCOLORKEY)
    FE(DDBLTFAST_DESTCOLORKEY)
    FE(DDBLTFAST_WAIT)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      dprintf(("DDRAW: %s ",flags[i].name));
  dprintf(("DDRAW: \n"));
}

void _dump_DDBLT(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
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
      dprintf(("DDRAW: %s ",flags[i].name));
  dprintf(("DDRAW: \n"));
}

void _dump_DDSCAPS(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
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
      dprintf(("DDRAW: %s ",flags[i].name));
  dprintf(("DDRAW: \n"));
}

void _dump_DDSCAPS2(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
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
      dprintf(("DDRAW: %s ",flags[i].name));
  dprintf(("DDRAW: \n"));
}


void _dump_DDSD(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
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
      dprintf(("DDRAW: %s ",flags[i].name));
  dprintf(("DDRAW: \n"));
}

void _dump_DDCOLORKEY(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
  } flags[] = {
#define FE(x) { x, #x},
          FE(DDCKEY_COLORSPACE)
    FE(DDCKEY_DESTBLT)
    FE(DDCKEY_DESTOVERLAY)
    FE(DDCKEY_SRCBLT)
    FE(DDCKEY_SRCOVERLAY)
  };
  for (i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
    if (flags[i].mask & flagmask)
      dprintf(("DDRAW: %s ",flags[i].name));
  dprintf(("DDRAW: \n"));
}

void _dump_DDPIXELFORMAT(DWORD flagmask) {
  int  i;
  const struct {
    DWORD  mask;
    const char  *name;
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
      dprintf(("DDRAW: %s ",flags[i].name));
  dprintf(("DDRAW: \n"));
}

void _dump_pixelformat(LPDDPIXELFORMAT pf) {
  _dump_DDPIXELFORMAT(pf->dwFlags);
  dprintf(("DDRAW: dwFourCC : %4s", &pf->dwFourCC));
  dprintf(("DDRAW: RBG bit count : %ld", pf->dwRGBBitCount));
  dprintf(("DDRAW: Masks : R %08lx  G %08lx  B %08lx  A %08lx\n",
       pf->dwRBitMask, pf->dwGBitMask, pf->dwBBitMask, pf->dwRGBAlphaBitMask));
}

// End of Internal Helpers
#endif

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

  if(sizeof(Scanline) < lPitch) {
      DebugInt3(); 	//oh, oh
      return;
  }
  // Bridge, we may got a problem ;)
  // Check for Overlapping Rects

  pBltPos = pBuffer;
  pSrcPos = pBuffer;

  if(lpDestRect->top > lpSrcRect->top)
  {
    //  +-------+     +-------+      +-------+
    //  |S      |     |S      |      |S      |
    //  |   +---|---+ +-------+  +---|---+   |
    //  |   | D |   | | D     |  | D |   |   |
    //  +-------+   | +-------+  |   +-------+
    //      |       | |       |  |       |
    //      +-------+ +-------+  +-------+
    //
    // We got one of the above cases (or no overlapping) so copy from bottom up

    pBltPos += (lpDestRect->left * bpp) + lPitch * (lpDestRect->bottom-1);
    pSrcPos += (lpSrcRect->left * bpp)  + lPitch * (lpSrcRect->bottom-1);
    BlitHeight = lpDestRect->bottom - lpDestRect->top;
    BlitWidth  = (lpDestRect->right - lpDestRect->left) * bpp;

    while(1)
    {
      memcpy(Scanline,pSrcPos,BlitWidth);
      memcpy(pBltPos,Scanline,BlitWidth);
      pBltPos -= lPitch;
      pSrcPos -= lPitch;
      if(! (--BlitHeight))
        break;
    }
  }
  else
  {
    //  +-------+     +-------+      +-------+
    //  | D     |     | D     |      | D     |
    //  |   +---|---+ +-------+  +---|---+   |  +---+---+---+
    //  |   |S  |   | |S      |  |S  |   |   |  |S  |S/D| D |
    //  +-------+   | +-------+  |   +-------+  |   |   |   |
    //      |       | |       |  |       |      |   |   |   |
    //      +-------+ +-------+  +-------+      +---+---+---+
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

  dwTLineLen = 1 + ((pSrcDesc->dwWidth + 31) & ~31);
  pdwTLine = (DWORD*)pAlpha + (dwTLineLen* lpSrcRect->top);
  dwTLineStart = 1+(lpSrcRect->left/32);
  dwTDWStart   = (lpSrcRect->left+8)/8;
}


