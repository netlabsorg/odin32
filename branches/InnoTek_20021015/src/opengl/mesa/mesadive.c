/* $Id: mesadive.c,v 1.3 2000-12-30 13:55:43 sandervl Exp $ */
/*****************************************************************************/
/*                                                                           */
/* DIVE code for OpenGL                                                      */
/*                                                                           */
/*****************************************************************************/

#ifdef DIVE
#define INCL_BASE
#define INCL_WIN
#define INCL_GPI


#include <os2wrap.h>
#include "..\..\ddraw\divewrap.h"
#include <memory.h>
#include <malloc.h>
#include <fourcc.h>
#include <misc.h>
#include <winuser32.h>

#define mmioFOURCC( ch0, ch1, ch2, ch3 )                         \
                  ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                  ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

/* Some extra typedefs to be able to import some of the underneath headers */
typedef ULONG           HPALETTE;
typedef ULONG           HPEN;
typedef ULONG           COLORREF;

#include "gl.h"
#include "config.h"
#include "context.h"
#include "wmesadef.h"
#include "colors.h"

#include "mesadive.h"

extern BYTE DITHER_RGB_2_8BIT( int red, int green, int blue, int pixel, int scanline);
extern void WMesaUpdateScreenPos(PWMC c,HWND hWnd);
extern INT WINAPI MessageBoxA(HWND,LPCSTR,LPCSTR,UINT);

extern PWMC Current;

DIVE_CAPS DiveCaps;

typedef struct tagFN
  {
    HWND          hwnd;
    PFNWP         f;
    struct tagFN *Next;
  }FN;

FN *WndFuncs;

void StoreFn(HWND hwnd,PFNWP f)
{
  FN *p=WndFuncs;

  while(p)
    {
      if(p->hwnd==hwnd)
        {
          p->f=f;
          return;
        }

      p=p->Next;
    }

  p=(FN *)calloc(sizeof(FN),1);

  p->hwnd=hwnd;
  p->f=f;
  p->Next=WndFuncs;

  WndFuncs=p;
}

PFNWP QueryFn(HWND hwnd)
{
  FN *p=WndFuncs;

  while(p)
    {
      if(p->hwnd==hwnd)
        return p->f;

      p=p->Next;
    }

  return 0;
}

FOURCC GetFOURCC(int pf)
{
  switch(pf)
    {
      case PF_8A8B8G8R:
        return FOURCC_BGR4;

      case PF_8R8G8B:
        return FOURCC_RGB3;

      case PF_5R6G5B:
        return FOURCC_R565;

      case PF_DITHER8:
        break;

      case PF_LOOKUP:
        return FOURCC_LUT8;

      case PF_GRAYSCALE:
        return FOURCC_GREY;

      case PF_BADFORMAT:
        return FOURCC_RGB3;

      case PF_INDEX8:
        return FOURCC_LUT8;

      default:
        break;
    }

  return 0;
}

void DiveWriteBackbuffer( PWMC pwc, int iScanLine, int iPixel, BYTE r, BYTE g, BYTE b)
{
  PBYTE   lpb = pwc->pbPixels;
  PDWORD  lpdw;
  PWORD   lpw;
  ULONG   nBypp = pwc->cColorBits >> 3;
  ULONG   nOffset = iPixel % nBypp;

  lpb += pwc->ScanWidth * iScanLine;

  lpb += iPixel * nBypp;
  lpdw = (LPDWORD)lpb;
  lpw = (LPWORD)lpb;

  if(nBypp == 1)
    {
      if(pwc->dither_flag)
          *lpb = DITHER_RGB_2_8BIT(r,g,b,iScanLine,iPixel);
      else
          *lpb = BGR8(r,g,b);
    }
  else
    if(nBypp == 2)
      *lpw = BGR16(r,g,b);
    else
      if (nBypp == 3)
        *lpdw = BGR24(r,g,b);
      else
        if (nBypp == 4)
          *lpdw = BGR32(r,g,b);
}

void DiveWriteFrontbuffer( PWMC pwc, int iScanLine, int iPixel, BYTE r, BYTE g, BYTE b)
{
  /* Check whether the point is clipped - if so don't draw it! */
  POINTL pt;
  ULONG  rc;

  pt.x=iPixel; pt.y=iScanLine;

  rc=GpiPtInRegion(pwc->hps,pwc->hrgn,&pt);

  if(rc!=PRGN_INSIDE)
    return;

  if(DiveCaps.fBankSwitched)
    {
      MessageBoxA( NULL, "Bank-Switched DIVE access currently not supported", "", MB_OK );
    }
  else
    {
      PBYTE   lpb = (PBYTE)pwc->ppFrameBuffer;
      PDWORD  lpdw;
      PWORD   lpw;
      ULONG   nBypp = DiveCaps.ulDepth >> 3;
/*      ULONG   nOffset = iPixel % nBypp;*/

      iScanLine+=pwc->WinPos.y;
      iPixel+=pwc->WinPos.x;

      lpb += iScanLine * DiveCaps.ulScanLineBytes;
      lpb += iPixel * nBypp;
      lpdw = (LPDWORD)lpb;
      lpw = (LPWORD)lpb;

      if(nBypp == 1){
          if(pwc->dither_flag)
              *lpb = DITHER_RGB_2_8BIT(r,g,b,iScanLine,iPixel);
          else
              *lpb = BGR8(r,g,b);
      }
      else if(nBypp == 2)
          *lpw = BGR16(r,g,b);
      else if (nBypp == 3){
          *lpdw = BGR24(r,g,b);
      }
      else if (nBypp == 4)
          *lpdw = BGR32(r,g,b);
   }
}

void DiveFlush( PWMC pwc )
{
  ULONG rc;

  if(!pwc->hDiveInstance)
    {
      dprintf(("DIVE ERROR : no instance!"));
      return;
    }

  if(pwc->DiveSoftwareBlit)
    {
      /* Perform software blitting if DiveSetupBlitter failed! */
      /* The source and target format will always be identical */
      /* This is accomplished by wmesa.c's wmSetPixelFormat    */
      /* routine that uses the native output-format...         */

      PBYTE  lpb   = (PBYTE)pwc->ppFrameBuffer;
      PBYTE  lpbb  = pwc->pbPixels;
      ULONG  nBypp = pwc->cColorBits >> 3;
      ULONG  bpl   = nBypp*pwc->awidth;

      lpb += pwc->WinPos.y*DiveCaps.ulScanLineBytes;
      lpb += pwc->WinPos.x*nBypp;

      lpbb += (pwc->height-1) * pwc->ScanWidth;

      for(int y=0; y<pwc->height; y++)
        {
          memcpy(lpb,lpbb,bpl);

          lpb+=DiveCaps.ulScanLineBytes;
          lpbb-=pwc->ScanWidth;
        }
    }
  else
    {
      rc=DiveEndImageBufferAccess(pwc->hDiveInstance,
                                  pwc->BackBufferNumber);

      rc=DiveBlitImage(pwc->hDiveInstance,
                       pwc->BackBufferNumber,
                       DIVE_BUFFER_SCREEN);

      ULONG bsl;

      rc=DiveBeginImageBufferAccess(pwc->hDiveInstance,
                                    pwc->BackBufferNumber,
                                    &pwc->pbPixels,
                                    &pwc->ScanWidth,
                                    &bsl);
    }
}

void DiveDefineRegion(PWMC wc,HWND hWnd)
{
  RGNRECT       rgnCtl;
  HWND          hwnd=Win32ToOS2Handle(hWnd);
  ULONG         rc;


  rgnCtl.ircStart=0;
  rgnCtl.crc=50;
  rgnCtl.ulDirection=RECTDIR_LFRT_TOPBOT;

  if(wc->hrgn)
    GpiDestroyRegion(wc->hps,wc->hrgn);

  if(wc->hps)
    WinReleasePS(wc->hps);

  wc->hps=WinGetPS(hwnd);

  wc->hrgn=(ULONG)GpiCreateRegion(wc->hps,0,NULL);

  rc=WinQueryVisibleRegion(hwnd,wc->hrgn);

  rc=GpiQueryRegionRects(wc->hps,wc->hrgn,NULL,&rgnCtl,(RECTL *)wc->rctls);

  wc->NumClipRects=rgnCtl.crcReturned;
}

ULONG DiveBlitSetup(PWMC wc,BOOL fActivate)
{
  SETUP_BLITTER setup;
  POINTL        pointl;
  SWP           swp;
  ULONG         rc;

  if(!wc)
    return 0;

  if(!fActivate)
    return DiveSetupBlitter(wc->hDiveInstance,0);

  DiveDefineRegion(wc,wc->hwnd);

  /* If not double-buffered we don't actually need to setup the blitter! */
  /* Also, if setting up the blitter failed once, it will probably fail  */
  /* again, so no need to pursue this piece of logic...                  */
  if(!wc->db_flag || wc->DiveSoftwareBlit)
    return 0;

  memset(&setup,0,sizeof(SETUP_BLITTER));

  setup.ulStructLen=sizeof(SETUP_BLITTER);

  setup.fInvert=1;

  setup.fccSrcColorFormat=GetFOURCC(wc->pixelformat);
  setup.fccDstColorFormat=FOURCC_SCRN;

  setup.ulSrcWidth=wc->awidth;
  setup.ulDstWidth=wc->awidth;
  setup.ulSrcHeight=wc->aheight;
  setup.ulDstHeight=wc->aheight;

  WinQueryWindowPos(Win32ToOS2Handle(wc->hwnd),&swp);

  pointl.x=swp.x; pointl.y=swp.y;

  WinMapWindowPoints(WinQueryWindow(Win32ToOS2Handle(wc->hwnd),QW_PARENT),
                                    HWND_DESKTOP,
                                    &pointl,
                                    1);

  setup.lScreenPosX=pointl.x;
  setup.lScreenPosY=pointl.y;

  setup.ulNumDstRects=wc->NumClipRects;
  setup.pVisDstRects=(RECTL *)wc->rctls;

  rc=DiveSetupBlitter(wc->hDiveInstance,&setup);

  if(rc==DIVE_ERR_INVALID_CONVERSION)
    {
      /* Hmmm, my Matrox does this all the time in 24-bit depth mode   */
      /* It works in lower color-depths, but in order to use Dive even */
      /* if the conversion-code in DIVE is somehow malfunctioning use  */
      /* a software blitter to blit the image. Do however change the   */
      /* backbuffer's pixelformat to that of the screen.               */
      wc->DiveSoftwareBlit=TRUE;

      rc=0;
    }

  return rc;
}

MRESULT EXPENTRY os2DiveWndProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
  PFNWP f=QueryFn(hwnd);
  ULONG rc;

  switch(msg)
    {
      case WM_VRNENABLED:
        if(Current)
          {
            DiveBlitSetup(Current,TRUE);
            WMesaUpdateScreenPos(Current,Current->hwnd);
          }
        break;

      case WM_VRNDISABLED:
        if(Current)
          DiveBlitSetup(Current,FALSE);
        break;

      case WM_SETFOCUS:
        if(Current)
          DiveDefineRegion(Current,Current->hwnd);
        break;

      case WM_SIZE:
        if(Current)
          {
            WMesaUpdateScreenPos(Current,Current->hwnd);
                      /* DiveResizeBuffers(Current->width,Current->Height);*/
          }
        break;

      case WM_DESTROY:
        WinSetVisibleRegionNotify(hwnd,FALSE);
        break;
    }

  return (*f)(hwnd,msg,mp1,mp2);
}

PBYTE AllocateBuffer(int width,int height,int pf,ULONG *s)
{
  *s=width*4;

  return (PBYTE)malloc(width*height*4);
}

/*
* doInit - do work required for every instance of the application:
*                create the window, initialize data
*/
BOOL DiveInit( PWMC wc, HWND hwnd)
{
    ULONG rc;
    RECT  rect;

/*  wc->fullScreen = displayOptions.fullScreen;
    wc->gMode = displayOptions.mode;*/
    wc->hwnd = hwnd;

    /* Register Region change Notifications for our window                  */
    /* This allows us to set the correct clipping rects and window position */
    /* We must subclass the OS/2 hwnd in order to accomplish this, since the*/
    /* WM_VRNxxxxx messages are not relayed to the WIN hwnd.                */
    PFNWP f=WinSubclassWindow(Win32ToOS2Handle(hwnd),(PFNWP)os2DiveWndProc);
    StoreFn(Win32ToOS2Handle(hwnd),f);

    WinSetVisibleRegionNotify(Win32ToOS2Handle(hwnd),TRUE);

/*  stereo_flag = displayOptions.stereo;

    if(wc->db_flag!= TRUE)
        stereo_flag = FALSE;
*/
    rc=DiveOpen(&wc->hDiveInstance,
                FALSE,
                &wc->ppFrameBuffer);

    dprintf(("OPENGL32: Created a new dive-instance (%08X) - pfb %08X - rc was %d",
             wc->hDiveInstance,
             wc->ppFrameBuffer,
             rc));

    if(rc)
      return FALSE;
/* For later? Switch to FullScreen mode ...
    switch( wc->gMode )
    {
    case 1:  ddrval = DiveSetDisplayMode( wc->lpDD, 640, 480, displayOptions.bpp); break;
    case 2:  ddrval = DiveSetDisplayMode( wc->lpDD, 800, 600, displayOptions.bpp); break;
    case 3:  ddrval = DiveSetDisplayMode( wc->lpDD, 1024, 768, displayOptions.bpp); break;
    case 4:  ddrval = DiveSetDisplayMode( wc->lpDD, 1152, 864, displayOptions.bpp); break;
    case 5:  ddrval = DiveSetDisplayMode( wc->lpDD, 1280, 1024, displayOptions.bpp); break;
    }
*/

   /* If in double-buffer mode allocate an offscreen rendering buffer! */
   if(wc->db_flag)
     {
       wc->BackBufferNumber=0;

       wc->awidth=wc->width;
       wc->aheight=wc->height;

       rc=DiveAllocImageBuffer(wc->hDiveInstance,
                               &wc->BackBufferNumber,
                               GetFOURCC(wc->pixelformat),
                               wc->awidth,
                               wc->aheight,
                               0,
                               0);

       dprintf(("Allocated imagebuffer %d (double-buffered screen). rc is %d",wc->BackBufferNumber,rc));

       if(rc==DIVE_ERR_SOURCE_FORMAT)
         {
           /* Darn - the driver doesn't support the format we want! */
           /* Well, just allocate it ourselves then!!               */
           wc->BackBufferOwnAllocation=TRUE;

           wc->pbPixels=AllocateBuffer(wc->awidth,
                                       wc->aheight,
                                       wc->pixelformat,
                                       &wc->ScanWidth);

           wc->BackBufferNumber=0;

           rc=DiveAllocImageBuffer(wc->hDiveInstance,
                                   &wc->BackBufferNumber,
                                   GetFOURCC(wc->pixelformat),
                                   wc->awidth,
                                   wc->aheight,
                                   wc->ScanWidth,
                                   wc->pbPixels);

           dprintf(("Allocated imagebuffer @%08X. rc is %d",wc->pbPixels,rc));
         }
       else
         {
           if(rc)
             {
               DiveClose(wc->hDiveInstance);
               wc->hDiveInstance=0;

               return FALSE;
             }
         }

       ULONG bsl;

       rc=DiveBeginImageBufferAccess(wc->hDiveInstance,
                                     wc->BackBufferNumber,
                                     &wc->pbPixels,
                                     &wc->ScanWidth,
                                     &bsl);

       if(rc)
         {
           DiveFree(wc);

           return FALSE;
         }

       rc=DiveBlitSetup(wc,TRUE);

       if(rc)
         {
           DiveFree(wc);

           return FALSE;
         }
     }

   return TRUE;
}

void DiveFree(PWMC wc)
{
  ULONG rc;

  if(wc->hDiveInstance!=0)
    {
      if(wc->BackBufferNumber)                  /* A backbuffer allocated??*/
        {
          rc=DiveEndImageBufferAccess(wc->hDiveInstance,
                                      wc->BackBufferNumber);

          rc=DiveFreeImageBuffer(wc->hDiveInstance,
                                 wc->BackBufferNumber);

        }

        wc->BackBufferNumber=0;

        rc=DiveClose(wc->hDiveInstance);

        wc->hDiveInstance=0;
    }
}

void _System DiveGlobalInitialize(void)/* Called by INITTERM               */
{
  DiveQueryCaps(&DiveCaps,DIVE_BUFFER_SCREEN);
/*
  WriteLog("OPENGL32: Dive Capabilities\n");
  WriteLog("OPENGL32: ulPlaneCount %d\n",DiveCaps.ulPlaneCount);
  WriteLog("OPENGL32: fScreenDirect %d\n",DiveCaps.fScreenDirect);
  WriteLog("OPENGL32: fBankSwitched %d\n",DiveCaps.fBankSwitched);
  WriteLog("OPENGL32: ulDepth %d\n",DiveCaps.ulDepth);
  WriteLog("OPENGL32: ulHorizontalResolution %d\n",DiveCaps.ulHorizontalResolution);
  WriteLog("OPENGL32: ulVerticalResolution %d\n",DiveCaps.ulVerticalResolution);
  WriteLog("OPENGL32: ulScanLineBytes %d\n",DiveCaps.ulScanLineBytes);
  WriteLog("OPENGL32: fccColorEncoding %08X\n",DiveCaps.fccColorEncoding);
  WriteLog("OPENGL32: ulApertureSize %d\n",DiveCaps.ulApertureSize);
  WriteLog("OPENGL32: ulInputFormats %d\n",DiveCaps.ulInputFormats);
  WriteLog("OPENGL32: ulOutputFormats %d\n",DiveCaps.ulOutputFormats);
  WriteLog("OPENGL32: ulFormatLength %d\n",DiveCaps.ulFormatLength);
  WriteLog("OPENGL32: pFormatData %08X\n",DiveCaps.pFormatData);
*/
}

void _System DiveGlobalTerminate(void)
{
  if(Current)
    {
      PWMC c=Current;

      Current=0;

      if(c->BackBufferNumber)
        {
          ULONG ulb=c->BackBufferNumber;

          c->BackBufferNumber=0;

          DiveEndImageBufferAccess(Current->hDiveInstance,ulb);
        }
    }
}

BOOL  _System DiveDirectAccess(void)
{
  return DiveCaps.fScreenDirect;
}

ULONG _System DiveQueryDepth(void)
{
  return DiveCaps.ulDepth;
}

ULONG _System DiveQueryNativePixelFormat(void)
{
  switch(DiveCaps.fccColorEncoding)
    {
      case FOURCC_LUT8:
        return PF_INDEX8;

      case FOURCC_BGR4:
        return PF_8A8B8G8R;

      case FOURCC_RGB3:
        return PF_8R8G8B;

      case FOURCC_R565:
        return PF_5R6G5B;

      case FOURCC_GREY:
        return PF_GRAYSCALE;

      default:
        break;
    }

  return 0;
}

void  _System DiveResizeBuffers(GLint width,GLint height)
{
  ULONG rc;

  if(!Current)
    return;

  if(Current->awidth==width && Current->aheight==height)
    return;

  DiveBlitSetup(Current,FALSE);

  Current->awidth=width;
  Current->aheight=height;

  if(Current->db_flag)
    {
      rc=DiveEndImageBufferAccess(Current->hDiveInstance,
                                  Current->BackBufferNumber);

      rc=DiveFreeImageBuffer(Current->hDiveInstance,
                             Current->BackBufferNumber);

      if(Current->BackBufferOwnAllocation)
        {
          free(Current->pbPixels);

          Current->pbPixels=AllocateBuffer(Current->awidth,
                                           Current->aheight,
                                           Current->pixelformat,
                                           &Current->ScanWidth);

          Current->BackBufferNumber=0;

          rc=DiveAllocImageBuffer(Current->hDiveInstance,
                                  &Current->BackBufferNumber,
                                  GetFOURCC(Current->pixelformat),
                                  Current->awidth,
                                  Current->aheight,
                                  Current->ScanWidth,
                                  Current->pbPixels);

          ULONG bsl;

          rc=DiveBeginImageBufferAccess(Current->hDiveInstance,
                                        Current->BackBufferNumber,
                                        &Current->pbPixels,
                                        &Current->ScanWidth,
                                        &bsl);
        }
      else
        {
          Current->BackBufferNumber=0;

          rc=DiveAllocImageBuffer(Current->hDiveInstance,
                                  &Current->BackBufferNumber,
                                  GetFOURCC(Current->pixelformat),
                                  Current->awidth,
                                  Current->aheight,
                                  0,
                                  0);

          ULONG bsl;

          rc=DiveBeginImageBufferAccess(Current->hDiveInstance,
                                        Current->BackBufferNumber,
                                        &Current->pbPixels,
                                        &Current->ScanWidth,
                                        &bsl);
        }
    }

  DiveBlitSetup(Current,TRUE);
}
#endif
