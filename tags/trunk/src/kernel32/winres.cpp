/* $Id: winres.cpp,v 1.2 1999-06-06 12:25:49 cbratschi Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 resource class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_BASE
#define INCL_WIN
#define INCL_GPIBITMAPS
#define INCL_BITMAPFILEFORMAT
#define INCL_DOSMODULEMGR
#include <os2.h>
#include <stdarg.h>
#ifdef __IBMCPP__
#include <builtin.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INCL_WINRES
#include "win32type.h"
#include "winres.h"
#include "misc.h"
#include "nameid.h"
#include "winexe.h"

static ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy)
{
        ULONG alignment;
        ULONG factor;
        BOOL flag = TRUE;       //true: '*'     false: '/'

        cy = cy < 0 ? -cy : cy;

        switch(cBits)
        {
                case 1:
                        factor = 8;
                        flag = FALSE;
                        break;

                case 4:
                        factor = 2;
                        flag = FALSE;
                        break;

                case 8:
                        factor = 1;
                        break;

                case 16:
                        factor = 2;
                        break;

                case 24:
                        factor = 3;
                        break;

                case 32:
                        return cx*cy;

                default:
                        return 0;
        }

        if (flag)
                alignment = (cx = (cx*factor)) % 4;
        else
                alignment = (cx = ((cx+factor-1)/factor)) % 4;

        if (alignment != 0)
                cx += 4 - alignment;

        return cx*cy;
}

//******************************************************************************
//******************************************************************************
Win32Resource::Win32Resource(Win32Image *module, HRSRC hRes, ULONG id, ULONG type) :
        os2resdata(NULL), winresdata(NULL)
{
 APIRET rc;

  next           = module->winres;
  module->winres = this;

  this->module   = module;
  this->id       = id;
  this->type     = type;
  this->hres     = hRes;

  rc = DosQueryResourceSize(module->hinstance, type, id, &ressize);
  if(rc) {
    dprintf(("Win32Resource ctor: DosQueryResourceSize returned %X\n", rc));
    ressize = 0;
  }
}
//******************************************************************************
//******************************************************************************
Win32Resource::Win32Resource(Win32Image *module, ULONG id, ULONG type,
                 ULONG size, char *resdata) : hres(NULL),
        os2resdata(NULL), winresdata(NULL)
{
  next           = module->winres;
  module->winres = this;

  this->module   = module;
  this->id       = id;
  this->type     = type;
  this->ressize  = size;
  winresdata     = (char *)malloc(size);
  if(winresdata == NULL) {
    DebugInt3();
    return;
  }
  memcpy(winresdata, resdata, size);
}
//******************************************************************************
//******************************************************************************
Win32Resource::~Win32Resource()
{
 Win32Resource *res = module->winres;

  if(os2resdata)    free(os2resdata);
  if(winresdata)    free(winresdata);

  if(res == this) {
    module->winres = res->next;
  }
  else {
    while(res->next != this) {
        res  = res->next;
    }
    if(res)
        res->next = next;
  }
}
//******************************************************************************
//******************************************************************************
PVOID Win32Resource::lockResource()
{
 int            restype = 0, newid;
 void          *resdata = NULL;
 APIRET         rc;
 ULONG          os2type = RT_RCDATA;

  dprintf(("Win32Resource::lockResource %d\n", id));
  if(winresdata)
    return(winresdata);

  switch(type) {
    case NTRT_BITMAP:
        rc = DosGetResource((HMODULE)module->hinstance, RT_BITMAP, id, (PPVOID)&resdata);
        if(rc)  return(NULL);
        winresdata = ConvertBitmap((BITMAPFILEHEADER2 *)resdata);
        break;

    case NTRT_ACCELERATORS:
    case NTRT_MENU:
    case NTRT_DIALOG:
        newid = module->getWin32ResourceId(id);

        rc = DosGetResource((HMODULE)module->hinstance, RT_RCDATA, (int)newid, (PPVOID)&resdata);
        if(rc) {
            dprintf(("Can't find original resource!!!\n"));
            return(NULL);
        }
        winresdata = (char *)malloc(ressize);
        memcpy(winresdata, resdata, ressize);
        break;

    //TODO:not yet implemented
    case NTRT_FONTDIR:
    case NTRT_FONT:
    case NTRT_MESSAGETABLE:
    case NTRT_NEWBITMAP:
    case NTRT_NEWMENU:
    case NTRT_NEWDIALOG:
        os2type = RT_RCDATA;
        break;

    //Can't do this right now (all group icons stored into a single one)
    case NTRT_CURSOR:
    case NTRT_GROUP_CURSOR:
    case NTRT_GROUP_ICON:
    case NTRT_ICON:
        dprintf(("Can't convert this resource!!!!!\n"));
        os2type = RT_POINTER;
        break;

    case NTRT_STRING:
        rc = DosGetResource((HMODULE)module->hinstance, RT_RCDATA, id, (PPVOID)&resdata);
        if(rc) {
            dprintf(("Can't find original string!!!\n"));
            return(NULL);
        }
        winresdata = (char *)malloc(ressize);
        memcpy(winresdata, resdata, ressize);
        DosFreeResource(resdata);
        return((PVOID)((ULONG)winresdata+2));   //skip length word

    //no conversion necessary
    case NTRT_RCDATA:
    case NTRT_VERSION:
    default:
        os2type = RT_RCDATA;
        break;
  }

  if(winresdata == NULL) {
    rc = DosGetResource((HMODULE)module->hinstance, os2type, id, (PPVOID)&resdata);
    if(rc) {
        dprintf(("Can't find original string!!!\n"));
        return(NULL);
    }
    winresdata = (char *)malloc(ressize);
    memcpy(winresdata, resdata, ressize);
  }
  if(resdata)
    DosFreeResource(resdata);
  return winresdata;
}
//******************************************************************************
//******************************************************************************
PVOID Win32Resource::ConvertBitmap(void *bmpdata)
{
 BITMAPFILEHEADER2   *bmphdr = (BITMAPFILEHEADER2 *)bmpdata;
 WINBITMAPINFOHEADER *winbmphdr;
 RGBQUAD             *rgb;
 RGB2                *os2rgb;
 int                  palsize = 0;
 int                  imgsize;

  if(bmphdr->cbSize != sizeof(BITMAPFILEHEADER2))
    return(bmpdata);    //don't convert OS/2 1.x bitmap

  if(bmphdr->bmp2.cBitCount < 16) {
    palsize = (1 << bmphdr->bmp2.cBitCount) * sizeof(RGBQUAD);
  }

  // EB: ->>> added imgsize
  if(bmphdr->bmp2.cbImage == 0)
    imgsize =  CalcBitmapSize(bmphdr->bmp2.cBitCount,
                              bmphdr->bmp2.cx,
                              bmphdr->bmp2.cy);
  else
    imgsize = bmphdr->bmp2.cbImage;

  winbmphdr = (WINBITMAPINFOHEADER *)malloc(sizeof(WINBITMAPINFOHEADER) +
                        imgsize + palsize);
  memset((char *)winbmphdr, 0, sizeof(WINBITMAPINFOHEADER) + imgsize + palsize);

  winbmphdr->biSize          = sizeof(WINBITMAPINFOHEADER);
  winbmphdr->biWidth         = bmphdr->bmp2.cx;
  winbmphdr->biHeight        = bmphdr->bmp2.cy;
  winbmphdr->biPlanes        = bmphdr->bmp2.cPlanes;
  winbmphdr->biBitCount      = bmphdr->bmp2.cBitCount;
  //TODO: Identical except for BI_BITFIELDS (3L) type!
  winbmphdr->biCompression   = bmphdr->bmp2.ulCompression;
  winbmphdr->biSizeImage     = bmphdr->bmp2.cbImage; //imgsize;
  //TODO: Doesn't seem to be completely identical..
  winbmphdr->biClrUsed       = bmphdr->bmp2.cclrUsed;
  winbmphdr->biClrImportant  = bmphdr->bmp2.cclrImportant;
  winbmphdr->biXPelsPerMeter = bmphdr->bmp2.cxResolution;
  winbmphdr->biYPelsPerMeter = bmphdr->bmp2.cyResolution;

  os2rgb                     = (RGB2 *)(bmphdr+1);
  rgb                    = (RGBQUAD *)(winbmphdr+1);

  if(palsize) {
    memcpy((char *)rgb, (char *)os2rgb, palsize);
    os2rgb = (RGB2    *)((int)os2rgb + palsize);
    rgb    = (RGBQUAD *)((int)rgb    + palsize);
  }
  memcpy((char *)rgb, (char *)os2rgb, imgsize);
  return((PVOID)winbmphdr);
}
//******************************************************************************
//******************************************************************************
void Win32Resource::destroyAll(Win32Image *module)
{
 Win32Resource *res = module->winres, *next;

   while(res) {
    next = res->next;
    delete(res);
    res  = next;
   }
}
//******************************************************************************
//******************************************************************************
