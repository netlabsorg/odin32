/* $Id: vgdrvr.c,v 1.2 2001-09-05 14:31:01 bird Exp $ */
/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com).
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.
**
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.
**
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
**
** $Revision: 1.2 $
** $Date: 2001-09-05 14:31:01 $
**
*/

#include <3dfx.h>
#include <sst1init.h>
#include "init.h"
#include "fxinit.h"
#include <sst1init.h>
#include <sst.h>

static FxBool FX_CALL setVideo( FxU32                   hWnd,
                        GrScreenResolution_t    sRes,
                        GrScreenRefresh_t       vRefresh,
                        InitColorFormat_t       cFormat,
                        InitOriginLocation_t    yOrigin,
                        int                     nColBuffers,
                        int                     nAuxBuffers,
                        int                     *xres,
                        int                     *yres,
                        int                     *fbStride,
                        sst1VideoTimingStruct   *vidTimings) {
    FxBool rv;
    static int _w[] = {320,320,400,512,640,640,640,640,800,960,856,512};
    static int _h[] = {200,240,256,384,200,350,400,480,600,720,480,256};

    rv = sst1InitVideo( (FxU32*)context->info.hwDep.vgInfo.vgBaseAddr,
                        sRes,
                        vRefresh,
                        vidTimings );

    if ( !rv ) goto BAIL;

    if ( context->info.hwDep.vgInfo.sliDetect ) {
        rv = sst1InitRegisters((FxU32*)context->info.hwDep.vgInfo.slaveBaseAddr);
        if ( !rv ) goto BAIL;
        rv = sst1InitVideo( (FxU32*)context->info.hwDep.vgInfo.slaveBaseAddr,
                            sRes,
                            vRefresh,
                            vidTimings );
        if ( !rv ) goto BAIL;
        rv = sst1InitSli( (FxU32*)context->info.hwDep.vgInfo.vgBaseAddr,
                          (FxU32*)context->info.hwDep.vgInfo.slaveBaseAddr );
        if ( !rv ) goto BAIL;
    }

    if(vidTimings) {
        *xres = vidTimings->xDimension;
        *yres = vidTimings->yDimension;
    } else {
        *xres = _w[sRes];
        *yres = _h[sRes];
    }
    *fbStride = 1024; /* 1024 always */

BAIL:
    return rv;
}

static void FX_CALL restoreVideo( void ) {
   sst1InitShutdown( (FxU32*)context->info.hwDep.vgInfo.vgBaseAddr );
}

static FxBool FX_CALL enableTransport( InitFIFOData *info ) {
    FxBool rv = FXFALSE;
    sst1DeviceInfoStruct sstInfo;

    if ( info ) {
        rv = sst1InitGetDeviceInfo( (FxU32*)context->info.hwDep.vgInfo.vgBaseAddr,
                                    &sstInfo );
        info->hwDep.vgFIFOData.memFifoStatusLwm = sstInfo.memFifoStatusLwm;
    }

    sst1InitCachingOn();

    return rv;
}

static void FX_CALL disableTransport( void ) {
}

static InitSwapType_t FX_CALL swapBuffers( FxU32 code ) {
    return INIT_SWAP_FLIP;
}

static FxU32 FX_CALL status( void ) {
  return sst1InitReturnStatus((FxU32 *) context->info.hwDep.vgInfo.vgBaseAddr);
}

static FxBool FX_CALL busy(void) {
  FxU32 stat = status();
  FxBool ret = (stat & SST_BUSY) ? FXTRUE : FXFALSE;

  return ret;
}

static void FX_CALL idle( void ) {
  sst1InitIdle((FxU32 *)context->info.hwDep.vgInfo.vgBaseAddr);
}

static void * FX_CALL getBufferPtr( InitBuffer_t buffer, int *strideBytes ) {
    return 0;
}

static void FX_CALL renderBuffer( InitBuffer_t buffer ) {
    return;
}

static void FX_CALL origin( InitOriginLocation_t origin ) {
    return;
}

static void FX_CALL ioCtl( FxU32 token, void *argument ) {
    return;
}

static FxBool FX_CALL control( FxU32 code ) {
    switch ( code ) {
    case INIT_CONTROL_DEACTIVATE:
        sst1InitVgaPassCtrl( (FxU32*)context->info.hwDep.vgInfo.vgBaseAddr,
                             FXTRUE );
        break;
    case INIT_CONTROL_ACTIVATE:
        sst1InitVgaPassCtrl( (FxU32*)context->info.hwDep.vgInfo.vgBaseAddr,
                             FXFALSE );
        break;
    }
    return FXTRUE;
}

static FxBool FX_CALL wrapFIFO(InitFIFOData *fd) {
    return FXTRUE;
}

static void FX_CALL gamma( double gamma ) {
  sst1InitGamma((FxU32 *) context->info.hwDep.vgInfo.vgBaseAddr, gamma);
}

static void FX_CALL sliPciOwner( FxU32 *regbase, FxU32 owner ) {
  sst1InitSliPciOwner(regbase, owner);
}

static FxBool FX_CALL gammargb( double r, double g, double b ) {
  return sst1InitGammaRGB((FxU32 *) context->info.hwDep.vgInfo.vgBaseAddr, r, g, b);
}

static FxBool FX_CALL gammatable( FxU32 nentries, FxU32 *r, FxU32 *g, FxU32 *b ) {
  return sst1InitGammaTable((FxU32 *) context->info.hwDep.vgInfo.vgBaseAddr, nentries, r, g, b);
}

static sst1VideoTimingStruct * FX_CALL findvidtiming( GrScreenResolution_t sRes, GrScreenRefresh_t vRefresh) {
  return sst1InitFindVideoTimingStruct(sRes, vRefresh);
}

void vgDriverInit( InitContext *context ) {
    context->setVideo         = setVideo;
    context->restoreVideo     = restoreVideo;
    context->enableTransport  = enableTransport;
    context->disableTransport = disableTransport;
    context->swapBuffers      = swapBuffers;
    context->status           = status;
    context->busy             = busy;
    context->idle             = idle;
    context->getBufferPtr     = getBufferPtr;
    context->renderBuffer     = renderBuffer;
    context->origin           = origin;
    context->ioCtl            = ioCtl;
    context->control          = control;
    context->wrapFIFO         = wrapFIFO;


    context->gamma            = gamma;
    context->sliPciOwner      = sliPciOwner;

    context->gammaRGB         = gammargb;
    context->initGammaTable   = gammatable;
    context->findVidTimingStruct = findvidtiming;

}

