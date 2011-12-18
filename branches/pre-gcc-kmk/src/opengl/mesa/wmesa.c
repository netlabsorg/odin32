/* $Id: wmesa.c,v 1.3 2000-05-23 20:41:05 jeroen Exp $ */

/*
*   File name   :   wmesa.c
*  Version      :   2.3
*
*  Display driver for Mesa 2.3  under
*   Windows95 and WindowsNT
*
*   Copyright (C) 1996-  Li Wei
*  Address      :       Institute of Artificial Intelligence
*               :           & Robotics
*               :       Xi'an Jiaotong University
*  Email        :       liwei@aiar.xjtu.edu.cn
*  Web page :       http://sun.aiar.xjtu.edu.cn
*
*  This file and its associations are partially borrowed from the
*  Windows NT driver for Mesa 1.8 , written by Mark Leaming
*  (mark@rsinc.com).
*/


/*
 * $Log: wmesa.c,v $
 * Revision 1.3  2000-05-23 20:41:05  jeroen
 * *** empty log message ***
 *
 * Revision 1.2  2000/03/01 18:49:40  jeroen
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/08/19 00:55:42  jtg
 * Imported sources
 *
 * Revision 3.10  1999/06/15 01:35:06  brianp
 * small change to wmSetPixel() from TWILMOT@cpr.fr
 *
 * Revision 3.9  1999/05/11 19:06:01  brianp
 * fixed a few VB->Index bugs (mikec@ensoniq.com)
 *
 * Revision 3.8  1999/05/08 15:15:23  brianp
 * various updates from mikec@ensoniq.com
 *
 * Revision 3.7  1999/04/01 01:27:34  brianp
 * always flip Y coord in read_rgba_span()
 *
 * Revision 3.6  1999/03/28 21:17:27  brianp
 * updated SetBuffer driver function
 *
 * Revision 3.5  1999/03/16 01:36:42  brianp
 * patched dither() to check if Current is NULL, per xzhou@nyx.net
 *
 * Revision 3.4  1999/02/25 14:12:33  keithw
 * Merged in kw3 patch
 *
 * Revision 3.3  1999/01/03 03:08:57  brianp
 * Ted Jump's changes
 *
 * Revision 3.2  1998/08/29 00:26:01
 * updated for Mesa 3.0 to accomodate EGCS-Mingw32 build
 *
 * Revision 3.1  1998/06/11 01:42:08  brianp
 * updated for Mesa 3.0 device driver interface (but not tested)
 *
 * Revision 3.0  1998/06/11 01:18:25  brianp
 * initial revision
 *
 */


#define WMESA_STEREO_C

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "wmesa.h"
#include "mesa_extend.h"
#include "colors.h"
#include "macros.h"
                                       /* #include "types.h"               */
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "vb.h"
#include "matrix.h"
#include "depth.h"
#include "wmesadef.h"

#ifndef __WIN32OS2__
#pragma warning ( disable : 4133 4761 )
#else
#include <misc.h>
#ifdef DIVE
#include <mesadive.h>
extern HWND Win32ToOS2Handle(HWND);
#endif
#endif

#ifdef PROFILE
                                       /* #include "profile.h"             */
#endif

#ifdef DITHER
#include <wing.h>
#endif

#if defined(__CYGWIN32__) || defined(__WIN32OS2__)
#include "macros.h"
#include <string.h>
#undef CopyMemory
#define CopyMemory memcpy
#endif

#if !defined(NO_STEREO)

#include "glu.h"
#include "stereo.h"

#endif
#if !defined(NO_PARALLEL)
                                       /* #include "parallel.h"            */
#endif

struct DISPLAY_OPTIONS displayOptions;

GLenum stereoCompile = GL_FALSE ;
GLenum stereoShowing  = GL_FALSE ;
GLenum stereoBuffer = GL_FALSE;
#if !defined(NO_STEREO)
GLint displayList = MAXIMUM_DISPLAY_LIST ;
#endif
GLint stereo_flag = 0 ;

/* end of added code*/

PWMC Current = NULL;
WMesaContext WC = NULL;

//#define DD_GETDC (Current->hDC )
#define DD_GETDC ((Current->db_flag) ? Current->dib.hDC : Current->hDC )
//#define DD_GETDC ((Current->db_flag) ? Current->hDCPrimary : Current->hDCBack )
#define DD_RELEASEDC

//#define BEGINGDICALL  if(Current->rgb_flag)wmFlushBits(Current);
#define BEGINGDICALL
//#define ENDGDICALL        if(Current->rgb_flag)wmGetBits(Current);
#define ENDGDICALL

//#define FLIP(Y)  (Current->dither_flag? Y : Current->height-(Y)-1)
//#define FLIP(Y)  (Current->height-(Y)-1)
//#define FLIP(Y) Y
/*
 * XXX Why only flip Y coord if single buffered???
 */
#define FLIP(Y)  (Current->db_flag? Y: Current->height-(Y)-1)
#define STARTPROFILE
#define ENDPROFILE(PARA)

#define DITHER_RGB_TO_8BIT_SETUP            \
GLubyte pixelDithered;

#define DITHER_RGB_TO_8BIT(red, green, blue, pixel, scanline)               \
{                                                                           \
    char unsigned redtemp, greentemp, bluetemp, paletteindex;               \
    redtemp = aDividedBy51[red]                                             \
    + (aModulo51[red] > aHalftone8x8[(pixel%8)*8                        \
    + scanline%8]);                                                 \
    greentemp = aDividedBy51[(char unsigned)green]                          \
    + (aModulo51[green] > aHalftone8x8[                             \
    (pixel%8)*8 + scanline%8]);                                     \
    bluetemp = aDividedBy51[(char unsigned)blue]                            \
    + (aModulo51[blue] > aHalftone8x8[                              \
    (pixel%8)*8 +scanline%8]);                                      \
    paletteindex = redtemp + aTimes6[greentemp] + aTimes36[bluetemp];       \
    pixelDithered = aWinGHalftoneTranslation[paletteindex];                 \
}


#ifdef DDRAW
static BOOL DDInit( WMesaContext wc, HWND hwnd);
static void DDFree( WMesaContext wc);
static HRESULT DDRestoreAll( WMesaContext wc );
static void DDDeleteOffScreen(WMesaContext wc);
static BOOL DDCreateOffScreen(WMesaContext wc);
#endif

static void FlushToFile(PWMC pwc, PSTR  szFile);

BOOL wmCreateBackingStore(PWMC pwc, long lxSize, long lySize);
BOOL wmDeleteBackingStore(PWMC pwc);
void wmCreatePalette( PWMC pwdc );
BOOL wmSetDibColors(PWMC pwc);
void wmSetPixel(PWMC pwc, int iScanLine, int iPixel, BYTE r, BYTE g, BYTE b);

void wmCreateDIBSection(
                        HDC  hDC,
                        PWMC pwc,      /* handle of device context         */
                        CONST BITMAPINFO *pbmi,/* address of structure containing bitmap size, format, and color data*/
                        UINT iUsage/* color data type indicator: RGB values or palette indices*/
                        );


void WMesaViewport( GLcontext *ctx,
                    GLint x, GLint y, GLsizei width, GLsizei height );


static triangle_func choose_triangle_function( GLcontext *ctx );


static void wmSetPixelFormat( PWMC wc, HDC hDC)
{
#ifdef DIVE
  wc->cColorBits=DiveQueryDepth();

  wc->pixelformat=DiveQueryNativePixelFormat();
#else
    if(wc->rgb_flag)
      wc->cColorBits = GetDeviceCaps(hDC, BITSPIXEL);
    else
      wc->cColorBits = 8;

    switch(wc->cColorBits){
    case 8:
        if(wc->dither_flag != GL_TRUE)
            wc->pixelformat = PF_INDEX8;
        else
            wc->pixelformat = PF_DITHER8;
        break;

    case 16:
        wc->pixelformat = PF_5R6G5B;
        break;

    case 32:
        wc->pixelformat = PF_8R8G8B;
        break;

    default:                                    /* This also handles 24 bpp*/
        wc->pixelformat = PF_BADFORMAT;
      break;
    }
#endif
}

//
                     /* This function sets the color table of a DIB section*/
                                     /* to match that of the destination DC*/
//
BOOL /*WINAPI*/ wmSetDibColors(PWMC pwc)
{
    RGBQUAD         *pColTab, *pRGB;
    PALETTEENTRY    *pPal, *pPE;
    int             i, nColors;
    BOOL            bRet=TRUE;
    DWORD           dwErr=0;

    /* Build a color table in the DIB that maps to the
    selected palette in the DC.
    */
    nColors = 1 << pwc->cColorBits;
    pPal = (PALETTEENTRY *)malloc( nColors * sizeof(PALETTEENTRY));
    memset( pPal, 0, nColors * sizeof(PALETTEENTRY) );
    GetPaletteEntries( pwc->hGLPalette, 0, nColors, pPal );
    pColTab = (RGBQUAD *)malloc( nColors * sizeof(RGBQUAD));
    for (i = 0, pRGB = pColTab, pPE = pPal; i < nColors; i++, pRGB++, pPE++) {
        pRGB->rgbRed = pPE->peRed;
        pRGB->rgbGreen = pPE->peGreen;
        pRGB->rgbBlue = pPE->peBlue;
    }
    if(pwc->db_flag)
        bRet = SetDIBColorTable(pwc->dib.hDC, 0, nColors, pColTab );

    if(!bRet)
        dwErr = GetLastError();

    free( pColTab );
    free( pPal );

    return(bRet);
}


//
// Free up the dib section that was created
//
BOOL wmDeleteBackingStore(PWMC pwc)
{
    SelectObject(pwc->dib.hDC, pwc->hOldBitmap);
    DeleteDC(pwc->dib.hDC);
    DeleteObject(pwc->hbmDIB);
    UnmapViewOfFile(pwc->dib.base);
    CloseHandle(pwc->dib.hFileMap);
    return TRUE;
}


//
// This function creates the DIB section that is used for combined
// GL and GDI calls
//
BOOL /*WINAPI*/ wmCreateBackingStore(PWMC pwc, long lxSize, long lySize)
{
    HDC hdc = pwc->hDC;
    LPBITMAPINFO pbmi = &(pwc->bmi);
    int     iUsage;

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = lxSize;
    pbmi->bmiHeader.biHeight= -lySize;
    pbmi->bmiHeader.biPlanes = 1;
    if(pwc->rgb_flag)
        pbmi->bmiHeader.biBitCount = GetDeviceCaps(pwc->hDC, BITSPIXEL);
    else
        pbmi->bmiHeader.biBitCount = 8;
    pbmi->bmiHeader.biCompression = BI_RGB;
    pbmi->bmiHeader.biSizeImage = 0;
    pbmi->bmiHeader.biXPelsPerMeter = 0;
    pbmi->bmiHeader.biYPelsPerMeter = 0;
    pbmi->bmiHeader.biClrUsed = 0;
    pbmi->bmiHeader.biClrImportant = 0;

    iUsage = (pbmi->bmiHeader.biBitCount <= 8) ? DIB_PAL_COLORS : DIB_RGB_COLORS;

    pwc->cColorBits = pbmi->bmiHeader.biBitCount;
    pwc->ScanWidth = pwc->pitch = lxSize;

    wmCreateDIBSection(hdc, pwc, pbmi, iUsage);

    if ((iUsage == DIB_PAL_COLORS) && !(pwc->hGLPalette)) {
        wmCreatePalette( pwc );
        wmSetDibColors( pwc );
    }
    wmSetPixelFormat(pwc, pwc->hDC);
    return(TRUE);

}


//
// This function copies one scan line in a DIB section to another
//
BOOL WINAPI wmSetDIBits(PWMC pwc, UINT uiScanWidth, UINT uiNumScans, UINT nBypp, UINT uiNewWidth, LPBYTE pBits)
{
    UINT uiScans = 0;
    LPBYTE  pDest = pwc->pbPixels;
    DWORD   dwNextScan = uiScanWidth;
    DWORD   dwNewScan = uiNewWidth;
    DWORD   dwScanWidth = (uiScanWidth * nBypp);

    //
    // We need to round up to the nearest DWORD
    // and multiply by the number of bytes per
    // pixel
    //
    dwNextScan = (((dwNextScan * nBypp)+ 3) & ~3);
    dwNewScan = (((dwNewScan * nBypp)+ 3) & ~3);

    for(uiScans = 0; uiScans < uiNumScans; uiScans++){
        CopyMemory(pDest, pBits, dwScanWidth);
        pBits += dwNextScan;
        pDest += dwNewScan;
    }

    return(TRUE);

}


BOOL wmFlush(PWMC pwc);

/*
* Useful macros:
Modified from file osmesa.c
*/


#define PIXELADDR(X,Y)  ((GLubyte *)Current->pbPixels + (Current->height-Y-1)* Current->ScanWidth + (X)*nBypp)
#define PIXELADDR1( X, Y )  \
((GLubyte *)wmesa->pbPixels + (wmesa->height-Y-1)* wmesa->ScanWidth + (X))
#define PIXELADDR2( X, Y )  \
((GLubyte *)wmesa->pbPixels + (wmesa->height-Y-1)* wmesa->ScanWidth + (X)*2)
#define PIXELADDR4( X, Y )  \
((GLubyte *)wmesa->pbPixels + (wmesa->height-Y-1)* wmesa->ScanWidth + (X)*4)


BYTE DITHER_RGB_2_8BIT( int r, int g, int b, int x, int y);

/* Finish all pending operations and synchronize. */
static void finish(GLcontext* ctx)
{
    /* No op */
}


//
// We cache all gl draw routines until a flush is made
//
static void flush(GLcontext* ctx)
{
    STARTPROFILE
        if((Current->rgb_flag /*&& !(Current->dib.fFlushed)*/&&!(Current->db_flag))
            ||(!Current->rgb_flag))
        {
            wmFlush(Current);
        }
        ENDPROFILE(flush)
}

/*
* Set the color index used to clear the color buffer.
*/
static void clear_index(GLcontext* ctx, GLuint index)
{
    STARTPROFILE
        Current->clearpixel = index;
    ENDPROFILE(clear_index)
}



/*
* Set the color used to clear the color buffer.
*/
static void clear_color( GLcontext* ctx, GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    STARTPROFILE
        Current->clearpixel=RGB(r, g, b );
    ENDPROFILE(clear_color)
}



/*
* Clear the specified region of the color buffer using the clear color
* or index as specified by one of the two functions above.
*/
                                       /* static void clear(GLcontext* ctx,*/
             /* GLboolean all,GLint x, GLint y, GLint width, GLint height )*/
/* TODO: I modified this function to match the prototype in dd.h. (swansma@geocities.com)*/
/* dd.h does not explain what the return type is so I could not set this to the proper*/
                                       /* value.                           */
static GLbitfield clear(GLcontext* ctx, GLbitfield mask,
                  GLboolean all, GLint x, GLint y, GLint width, GLint height)
{
    DWORD   dwColor;
    WORD    wColor;
    BYTE    bColor;
    LPDWORD lpdw = (LPDWORD)Current->pbPixels;
    LPWORD  lpw = (LPWORD)Current->pbPixels;
    LPBYTE  lpb = Current->pbPixels;
    int     lines;

    STARTPROFILE

        if (all){
            x=y=0;
            width=Current->width;
            height=Current->height;
        }

        if(Current->db_flag==GL_TRUE){
            UINT    nBypp = Current->cColorBits / 8;
            int     i = 0;
            int     iSize = 0;

            if(nBypp ==1 ){
                /* Need rectification */
                iSize = Current->width/4;
                bColor  = BGR8(GetRValue(Current->clearpixel),
                    GetGValue(Current->clearpixel),
                    GetBValue(Current->clearpixel));
                wColor  = MAKEWORD(bColor,bColor);
                dwColor = MAKELONG(wColor, wColor);
            }
            if(nBypp == 2){
                iSize = Current->width / 2;
                wColor = BGR16(GetRValue(Current->clearpixel),
                    GetGValue(Current->clearpixel),
                    GetBValue(Current->clearpixel));
                dwColor = MAKELONG(wColor, wColor);
            }
            else if(nBypp == 4){
                iSize = Current->width;
                dwColor = BGR32(GetRValue(Current->clearpixel),
                    GetGValue(Current->clearpixel),
                    GetBValue(Current->clearpixel));
            }

            while(i < iSize){
                *lpdw = dwColor;
                lpdw++;
                i++;
            }

            //
                                       /* This is the 24bit case           */
            //
            if (nBypp == 3) {
                iSize = Current->width *3/4;
                dwColor = BGR24(GetRValue(Current->clearpixel),
                    GetGValue(Current->clearpixel),
                    GetBValue(Current->clearpixel));
                while(i < iSize){
                    *lpdw = dwColor;
                    lpb += nBypp;
                    lpdw = (LPDWORD)lpb;
                    i++;
                }
            }

            i = 0;
            if (stereo_flag)
               lines = height /2;
            else
               lines = height;
            do {
                memcpy(lpb, Current->pbPixels, iSize*4);
                lpb += Current->ScanWidth;
                i++;
            }
            while (i<lines-1);
        }
        else
          {                                            /* For single buffer*/
            HDC DC=DD_GETDC;

            HPEN Pen=CreatePen(PS_SOLID,1,Current->clearpixel);
            HBRUSH Brush=CreateSolidBrush(Current->clearpixel);
            HPEN Old_Pen=SelectObject(DC,Pen);
            HBRUSH Old_Brush=SelectObject(DC,Brush);

            Rectangle(DC,x,y,x+width,y+height);

            SelectObject(DC,Old_Pen);
            SelectObject(DC,Old_Brush);
            DeleteObject(Pen);
            DeleteObject(Brush);

            DD_RELEASEDC;
          }

        ENDPROFILE(clear)

                return mask;/* TODO: I doubt this is correct. dd.h doesn't explain what this should*/
                                       /* be...                            */
}



/* Set the current color index. */
static void set_index(GLcontext* ctx, GLuint index)
{
    STARTPROFILE
        Current->pixel=index;
    ENDPROFILE(set_index)
}



/* Set the current RGBA color. */
static void set_color( GLcontext* ctx, GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    STARTPROFILE
        Current->pixel = RGB( r, g, b );
    ENDPROFILE(set_color)
}



/* Set the index mode bitplane mask. */
static GLboolean index_mask(GLcontext* ctx, GLuint mask)
{
    /* can't implement */
    return GL_FALSE;
}



/* Set the RGBA drawing mask. */
static GLboolean color_mask( GLcontext* ctx,
                            GLboolean rmask, GLboolean gmask,
                            GLboolean bmask, GLboolean amask)
{
    /* can't implement */
    return GL_FALSE;
}



static void dither( GLcontext* ctx, GLboolean enable )
{
   if (!Current)
      return;

    if(enable == GL_FALSE){
        Current->dither_flag = GL_FALSE;
        if(Current->cColorBits == 8)
            Current->pixelformat = PF_INDEX8;
    }
    else{
        if (Current->rgb_flag && Current->cColorBits == 8){
            Current->pixelformat = PF_DITHER8;
            Current->dither_flag = GL_TRUE;
        }
        else
            Current->dither_flag = GL_FALSE;
    }
}



static GLboolean set_buffer( GLcontext* ctx, GLenum mode )
{
   STARTPROFILE
   /* TODO: this could be better */
   if (mode==GL_FRONT_LEFT || mode==GL_BACK_LEFT) {
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
   ENDPROFILE(set_buffer)
}



/* Return characteristics of the output buffer. */
static void buffer_size( GLcontext* ctx, GLuint *width, GLuint *height )
{
   int New_Size;
   RECT CR;

   STARTPROFILE

   GetClientRect(Current->Window,&CR);

   *width=CR.right;
   *height=CR.bottom;

   New_Size=((*width)!=Current->width) || ((*height)!=Current->height);

   if (New_Size){
      Current->width=*width;
      Current->height=*height;
      Current->ScanWidth=Current->width;

      if ((Current->ScanWidth%sizeof(long))!=0)
         Current->ScanWidth+=(sizeof(long)-(Current->ScanWidth%sizeof(long)));

      if (Current->db_flag){
#ifdef DIVE
         if(!DiveDirectAccess())
           if (Current->rgb_flag==GL_TRUE && Current->dither_flag!=GL_TRUE){
              wmDeleteBackingStore(Current);
              wmCreateBackingStore(Current, Current->width, Current->height);
           }
#else
#ifdef DDRAW
         DDDeleteOffScreen(Current);
         DDCreateOffScreen(Current);
#else
         if (Current->rgb_flag==GL_TRUE && Current->dither_flag!=GL_TRUE){
            wmDeleteBackingStore(Current);
            wmCreateBackingStore(Current, Current->width, Current->height);
         }
#endif
#endif
      }

      //  Resize OsmesaBuffer if in Parallel mode
#if !defined(NO_PARALLEL)
      if(parallelFlag)
         PRSizeRenderBuffer(Current->width, Current->height,Current->ScanWidth,
                            Current->rgb_flag == GL_TRUE ? Current->pbPixels: Current->ScreenMem);
#endif
   }
   ENDPROFILE(buffer_size)
}



/**********************************************************************/
/*****           Accelerated point, line, polygon rendering       *****/
/**********************************************************************/


static void fast_rgb_points( GLcontext* ctx, GLuint first, GLuint last )
{
    GLuint i;
    //  HDC DC=DD_GETDC;
    PWMC    pwc = Current;

    STARTPROFILE

        if (0 /*Current->gl_ctx->VB->MonoColor*/) {
            /* all drawn with current color */
            for (i=first;i<=last;i++) {
                if (!Current->gl_ctx->VB->ClipMask[i]) {
                    int x, y;
                    x =       (GLint) Current->gl_ctx->VB->Win.data[i][0];
                    y = FLIP( (GLint) Current->gl_ctx->VB->Win.data[i][1] );
                    wmSetPixel(pwc, y,x,GetRValue(Current->pixel),
                        GetGValue(Current->pixel), GetBValue(Current->pixel));
                }
            }
        }
        else {
            /* draw points of different colors */
            for (i=first;i<=last;i++) {
                if (!Current->gl_ctx->VB->ClipMask[i]) {
                    int x, y;
                    unsigned long pixel=RGB((GLubyte)(Current->gl_ctx->VB->ColorPtr->data[i][0]*255.0),
                        (GLubyte)(Current->gl_ctx->VB->ColorPtr->data[i][1]*255.0),
                        (GLubyte)(Current->gl_ctx->VB->ColorPtr->data[i][2]*255.0));
                    x =       (GLint) Current->gl_ctx->VB->Win.data[i][0];
                    y = FLIP( (GLint) Current->gl_ctx->VB->Win.data[i][1] );
                    wmSetPixel(pwc, y,x,Current->gl_ctx->VB->ColorPtr->data[i][0]*255.0,
                        Current->gl_ctx->VB->ColorPtr->data[i][1]*255.0,
                        Current->gl_ctx->VB->ColorPtr->data[i][2]*255.0);
                }
            }
        }

                                       /* DD_RELEASEDC;                    */
        ENDPROFILE(fast_rgb_points)
}



/* Return pointer to accerated points function */
extern points_func choose_points_function( GLcontext* ctx )
{
    STARTPROFILE
        if (ctx->Point.Size==1.0 && !ctx->Point.SmoothFlag && ctx->RasterMask==0
            && !ctx->Texture.Enabled  && ctx->Visual->RGBAflag) {
            ENDPROFILE(choose_points_function)
                return fast_rgb_points;
        }
        else {
            ENDPROFILE(choose_points_function)
                return NULL;
        }
}



/* Draw a line using the color specified by Current->gl_ctx->VB->ColorPtr->data[pv] */
static void fast_flat_rgb_line( GLcontext* ctx, GLuint v0, GLuint v1, GLuint pv )
{
    STARTPROFILE
        int x0, y0, x1, y1;
    unsigned long pixel;
    HDC DC=DD_GETDC;
    HPEN Pen;
    HPEN Old_Pen;

    if (0 /*Current->gl_ctx->VB->MonoColor*/) {
        pixel = Current->pixel;                        /* use current color*/
    }
    else {
        pixel = RGB((GLubyte)(Current->gl_ctx->VB->ColorPtr->data[pv][0]/* *255.0*/),
                    (GLubyte)(Current->gl_ctx->VB->ColorPtr->data[pv][1]/* *255.0*/),
                    (GLubyte)(Current->gl_ctx->VB->ColorPtr->data[pv][2]/* *255.0*/));
    }

    x0 =       (int) Current->gl_ctx->VB->Win.data[v0][0];
    y0 = FLIP( (int) Current->gl_ctx->VB->Win.data[v0][1] );
    x1 =       (int) Current->gl_ctx->VB->Win.data[v1][0];
    y1 = FLIP( (int) Current->gl_ctx->VB->Win.data[v1][1] );


    BEGINGDICALL

    Pen=CreatePen(PS_SOLID,1,(COLORREF)pixel);
    Old_Pen=SelectObject(DC,Pen);
    MoveToEx(DC,x0,y0,NULL);
    LineTo(DC,x1,y1);
    SelectObject(DC,Old_Pen);
    DeleteObject(Pen);
    DD_RELEASEDC;

    ENDGDICALL

    ENDPROFILE(fast_flat_rgb_line)
}



/* Return pointer to accerated line function */
static line_func choose_line_function( GLcontext* ctx )
{
    STARTPROFILE
    if (ctx->Line.Width==1.0 && !ctx->Line.SmoothFlag && !ctx->Line.StippleFlag
        && ctx->Light.ShadeModel==GL_FLAT && ctx->RasterMask==0
        && !ctx->Texture.Enabled && Current->rgb_flag) {
       ENDPROFILE(choose_line_function)
       return fast_flat_rgb_line;
    }
    else {
       ENDPROFILE(choose_line_function)
//     return fast_flat_rgb_line;
       return NULL;
    }
}


/**********************************************************************/
/*****                 Span-based pixel drawing                   *****/
/**********************************************************************/


/* Write a horizontal span of 32-bit color-index pixels with a boolean mask. */
static void write_ci32_span( const GLcontext* ctx,
                             GLuint n, GLint x, GLint y,
                             const GLuint index[],
                             const GLubyte mask[] )
{
    STARTPROFILE
    GLuint i;
    PBYTE Mem=Current->ScreenMem+FLIP(y)*Current->ScanWidth+x;
    ASSERT(Current->rgb_flag==GL_FALSE);
    for (i=0; i<n; i++)
        if (mask[i])
            Mem[i]=index[i];
    ENDPROFILE(write_ci32_span)
}


/* Write a horizontal span of 8-bit color-index pixels with a boolean mask. */
static void write_ci8_span( const GLcontext* ctx,
                            GLuint n, GLint x, GLint y,
                            const GLubyte index[],
                            const GLubyte mask[] )
{
    STARTPROFILE
    GLuint i;
    PBYTE Mem=Current->ScreenMem+FLIP(y)*Current->ScanWidth+x;
    ASSERT(Current->rgb_flag==GL_FALSE);
    for (i=0; i<n; i++)
        if (mask[i])
            Mem[i]=index[i];
    ENDPROFILE(write_ci8_span)
}



/*
* Write a horizontal span of pixels with a boolean mask.  The current
* color index is used for all pixels.
*/
static void write_mono_ci_span(const GLcontext* ctx,
                               GLuint n,GLint x,GLint y,
                               const GLubyte mask[])
{
   STARTPROFILE
   GLuint i;
   BYTE *Mem=Current->ScreenMem+FLIP(y)*Current->ScanWidth+x;
   ASSERT(Current->rgb_flag==GL_FALSE);
   for (i=0; i<n; i++)
      if (mask[i])
         Mem[i]=Current->pixel;
   ENDPROFILE(write_mono_ci_span)
}

/*
 * To improve the performance of this routine, frob the data into an actual
 * scanline and call bitblt on the complete scan line instead of SetPixel.
 */

/* Write a horizontal span of RGBA color pixels with a boolean mask. */
static void write_rgba_span( const GLcontext* ctx, GLuint n, GLint x, GLint y,
                             const GLubyte rgba[][4], const GLubyte mask[] )
{
    STARTPROFILE
    PWMC    pwc = Current;

    if (pwc->rgb_flag==GL_TRUE)
    {
        GLuint i;
        HDC DC=DD_GETDC;
        y=FLIP(y);
        if (mask) {
            for (i=0; i<n; i++)
                if (mask[i])
                    wmSetPixel(pwc, y, x + i, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
        }
        else {
            for (i=0; i<n; i++)
                wmSetPixel(pwc, y, x + i, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP] );
        }
        DD_RELEASEDC;
    }
    else
    {
        GLuint i;
        BYTE *Mem=Current->ScreenMem+y*Current->ScanWidth+x;
        y = FLIP(y);
        if (mask) {
            for (i=0; i<n; i++)
                if (mask[i])
                    Mem[i] = GetNearestPaletteIndex(Current->hPal,RGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]));
        }
        else {
            for (i=0; i<n; i++)
                Mem[i] = GetNearestPaletteIndex(Current->hPal,RGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]));
        }
    }

    ENDPROFILE(write_rgba_span)

}

/* Write a horizontal span of RGB color pixels with a boolean mask. */
static void write_rgb_span( const GLcontext* ctx,
                            GLuint n, GLint x, GLint y,
                            const GLubyte rgb[][3], const GLubyte mask[] )
{
    STARTPROFILE
    PWMC    pwc = Current;

    if (pwc->rgb_flag==GL_TRUE)
    {
        GLuint i;
        HDC DC=DD_GETDC;
        y=FLIP(y);
        if (mask) {
            for (i=0; i<n; i++)
                if (mask[i])
                    wmSetPixel(pwc, y, x + i, rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]);
        }
        else {
            for (i=0; i<n; i++)
                wmSetPixel(pwc, y, x + i, rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP] );
        }
        DD_RELEASEDC;
    }
    else
    {
        GLuint i;
        BYTE *Mem=Current->ScreenMem+y*Current->ScanWidth+x;
        y = FLIP(y);
        if (mask) {
            for (i=0; i<n; i++)
                if (mask[i])
                    Mem[i] = GetNearestPaletteIndex(Current->hPal,RGB(rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]));
        }
        else {
            for (i=0; i<n; i++)
                Mem[i] = GetNearestPaletteIndex(Current->hPal,RGB(rgb[i][RCOMP], rgb[i][GCOMP], rgb[i][BCOMP]));
        }
    }

    ENDPROFILE(write_rgb_span)

}

/*
* Write a horizontal span of pixels with a boolean mask.  The current color
* is used for all pixels.
*/
static void write_mono_rgba_span( const GLcontext* ctx,
                                  GLuint n, GLint x, GLint y,
                                  const GLubyte mask[])
{
    STARTPROFILE
    GLuint i;
    HDC DC=DD_GETDC;
    PWMC pwc = Current;
    ASSERT(Current->rgb_flag==GL_TRUE);
    y=FLIP(y);

    if(Current->rgb_flag==GL_TRUE){
        for (i=0; i<n; i++)
            if (mask[i])
                                       /* Trying                           */
                wmSetPixel(pwc,y,x+i,GetRValue(Current->pixel), GetGValue(Current->pixel), GetBValue(Current->pixel));
    }
    else {
        for (i=0; i<n; i++)
            if (mask[i])
                SetPixel(DC, y, x+i, Current->pixel);
    }
    DD_RELEASEDC;

    ENDPROFILE(write_mono_rgba_span)
}



/**********************************************************************/
/*****                   Array-based pixel drawing                *****/
/**********************************************************************/


/* Write an array of 32-bit index pixels with a boolean mask. */
static void write_ci32_pixels( const GLcontext* ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLuint index[], const GLubyte mask[] )
{
   STARTPROFILE
   GLuint i;
   ASSERT(Current->rgb_flag==GL_FALSE);
   for (i=0; i<n; i++) {
      if (mask[i]) {
         BYTE *Mem=Current->ScreenMem+FLIP(y[i])*Current->ScanWidth+x[i];
         *Mem = index[i];
      }
   }
   ENDPROFILE(write_ci32_pixels)
}



/*
* Write an array of pixels with a boolean mask.  The current color
* index is used for all pixels.
*/
static void write_mono_ci_pixels( const GLcontext* ctx,
                                  GLuint n,
                                  const GLint x[], const GLint y[],
                                  const GLubyte mask[] )
{
   STARTPROFILE
   GLuint i;
   ASSERT(Current->rgb_flag==GL_FALSE);
   for (i=0; i<n; i++) {
      if (mask[i]) {
         BYTE *Mem=Current->ScreenMem+FLIP(y[i])*Current->ScanWidth+x[i];
         *Mem = Current->pixel;
      }
   }
   ENDPROFILE(write_mono_ci_pixels)
}



/* Write an array of RGBA pixels with a boolean mask. */
static void write_rgba_pixels( const GLcontext* ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLubyte rgba[][4], const GLubyte mask[] )
{
    STARTPROFILE

        GLuint i;
    PWMC    pwc = Current;
    HDC DC=DD_GETDC;
    ASSERT(Current->rgb_flag==GL_TRUE);

    for (i=0; i<n; i++)
       if (mask[i])
          wmSetPixel(pwc, FLIP(y[i]),x[i],rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP]);

    DD_RELEASEDC;
    ENDPROFILE(write_rgba_pixels)
}



/*
* Write an array of pixels with a boolean mask.  The current color
* is used for all pixels.
*/
static void write_mono_rgba_pixels( const GLcontext* ctx,
                                    GLuint n,
                                    const GLint x[], const GLint y[],
                                    const GLubyte mask[] )
{
    STARTPROFILE
    GLuint i;
    PWMC    pwc = Current;
    HDC DC=DD_GETDC;
    ASSERT(Current->rgb_flag==GL_TRUE);

    for (i=0; i<n; i++)
        if (mask[i])
            wmSetPixel(pwc, FLIP(y[i]),x[i],GetRValue(Current->pixel),
                       GetGValue(Current->pixel), GetBValue(Current->pixel));

    DD_RELEASEDC;
    ENDPROFILE(write_mono_rgba_pixels)
}



/**********************************************************************/
/*****            Read spans/arrays of pixels                     *****/
/**********************************************************************/


/* Read a horizontal span of color-index pixels. */
static void read_ci32_span( const GLcontext* ctx, GLuint n, GLint x, GLint y,
                            GLuint index[])
{
   STARTPROFILE
   GLuint i;
   BYTE *Mem=Current->ScreenMem+FLIP(y)*Current->ScanWidth+x;
   ASSERT(Current->rgb_flag==GL_FALSE);
   for (i=0; i<n; i++)
      index[i]=Mem[i];
   ENDPROFILE(read_ci32_span)
}




/* Read an array of color index pixels. */
static void read_ci32_pixels( const GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLuint indx[], const GLubyte mask[] )
{
   STARTPROFILE
   GLuint i;
   ASSERT(Current->rgb_flag==GL_FALSE);
   for (i=0; i<n; i++) {
      if (mask[i]) {
         indx[i]=*(Current->ScreenMem+FLIP(y[i])*Current->ScanWidth+x[i]);
      }
   }
   ENDPROFILE(read_ci32_pixels)
}



/* Read a horizontal span of color pixels. */
static void read_rgba_span( const GLcontext* ctx,
                            GLuint n, GLint x, GLint y,
                            GLubyte rgba[][4] )
{
   STARTPROFILE
   UINT i;
   COLORREF Color;
   HDC DC=DD_GETDC;
   ASSERT(Current->rgb_flag==GL_TRUE);
   /*   y=FLIP(y);*/
   y = Current->height - y - 1;
   for (i=0; i<n; i++) {
      Color=GetPixel(DC,x+i,y);
      rgba[i][RCOMP] = GetRValue(Color);
      rgba[i][GCOMP] = GetGValue(Color);
      rgba[i][BCOMP] = GetBValue(Color);
      rgba[i][ACOMP] = 255;
   }
   DD_RELEASEDC;
// Brian P. Has mentioned to comment this out.
//   memset(alpha,0,n*sizeof(GLubyte));
   ENDPROFILE(read_rgba_span)
}


/* Read an array of color pixels. */
static void read_rgba_pixels( const GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLubyte rgba[][4], const GLubyte mask[] )
{
   STARTPROFILE
   GLuint i;
   COLORREF Color;
   HDC DC=DD_GETDC;
   ASSERT(Current->rgb_flag==GL_TRUE);
   for (i=0; i<n; i++) {
      if (mask[i]) {
         Color=GetPixel(DC,x[i],FLIP(y[i]));
         rgba[i][RCOMP] = GetRValue(Color);
         rgba[i][GCOMP] = GetGValue(Color);
         rgba[i][BCOMP] = GetBValue(Color);
         rgba[i][ACOMP] = 255;
      }
   }
   DD_RELEASEDC;
// Brian P. has mentioned to comment this out.
//   memset(alpha,0,n*sizeof(GLint));
   ENDPROFILE(read_rgba_pixels)
}



/**********************************************************************/
/**********************************************************************/


static const GLubyte *get_string(GLcontext *ctx, GLenum name)
{
   if(name == GL_RENDERER) {
     return (GLubyte *)"Mesa Windows";
   }
   else {
      return NULL;
   }
}



void setup_DD_pointers( GLcontext* ctx )
{
    ctx->Driver.GetString = get_string;
    ctx->Driver.UpdateState = setup_DD_pointers;
    ctx->Driver.GetBufferSize = buffer_size;
    ctx->Driver.Finish = finish;
    ctx->Driver.Flush = flush;

    ctx->Driver.ClearIndex = clear_index;
    ctx->Driver.ClearColor = clear_color;
    ctx->Driver.Clear = clear;

    ctx->Driver.Index = set_index;
    ctx->Driver.Color = set_color;
    ctx->Driver.IndexMask = index_mask;
    ctx->Driver.ColorMask = color_mask;

    ctx->Driver.Dither = dither;

    ctx->Driver.SetDrawBuffer = set_buffer;
    ctx->Driver.GetBufferSize = buffer_size;

    ctx->Driver.PointsFunc = choose_points_function(ctx);
    ctx->Driver.LineFunc = choose_line_function(ctx);
    ctx->Driver.TriangleFunc = choose_triangle_function( ctx );

    /* Pixel/span writing functions: */
    ctx->Driver.WriteRGBASpan        = write_rgba_span;
    ctx->Driver.WriteRGBSpan         = write_rgb_span;
    ctx->Driver.WriteMonoRGBASpan    = write_mono_rgba_span;
    ctx->Driver.WriteRGBAPixels      = write_rgba_pixels;
    ctx->Driver.WriteMonoRGBAPixels  = write_mono_rgba_pixels;
    ctx->Driver.WriteCI32Span        = write_ci32_span;
    ctx->Driver.WriteCI8Span         = write_ci8_span;
    ctx->Driver.WriteMonoCISpan      = write_mono_ci_span;
    ctx->Driver.WriteCI32Pixels      = write_ci32_pixels;
    ctx->Driver.WriteMonoCIPixels    = write_mono_ci_pixels;

    ctx->Driver.ReadCI32Span        = read_ci32_span;
    ctx->Driver.ReadRGBASpan        = read_rgba_span;
    ctx->Driver.ReadCI32Pixels      = read_ci32_pixels;
    ctx->Driver.ReadRGBAPixels      = read_rgba_pixels;
}


/**********************************************************************/
/*****                  WMesa API Functions                       *****/
/**********************************************************************/



#define PAL_SIZE 256
static void GetPalette(HPALETTE Pal,RGBQUAD *aRGB)
{
    STARTPROFILE
        int i;
    HDC hdc;
    struct
    {
        WORD Version;
        WORD NumberOfEntries;
        PALETTEENTRY aEntries[PAL_SIZE];
    } Palette =
    {
        0x300,
            PAL_SIZE
    };
    hdc=GetDC(NULL);
    if (Pal!=NULL)
        GetPaletteEntries(Pal,0,PAL_SIZE,Palette.aEntries);
    else
        GetSystemPaletteEntries(hdc,0,PAL_SIZE,Palette.aEntries);
    if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC)
    {
        for(i = 0; i <PAL_SIZE; i++)
            Palette.aEntries[i].peFlags = PC_RESERVED;
        Palette.aEntries[255].peRed = 255;
        Palette.aEntries[255].peGreen = 255;
        Palette.aEntries[255].peBlue = 255;
        Palette.aEntries[255].peFlags = 0;
        Palette.aEntries[0].peRed = 0;
        Palette.aEntries[0].peGreen = 0;
        Palette.aEntries[0].peBlue = 0;
        Palette.aEntries[0].peFlags = 0;
    }
    else
    {
        int nStaticColors;
        int nUsableColors;
        nStaticColors = GetDeviceCaps(hdc, NUMCOLORS)/2;
        for (i=0; i<nStaticColors; i++)
            Palette.aEntries[i].peFlags = 0;
        nUsableColors = PAL_SIZE-nStaticColors;
        for (; i<nUsableColors; i++)
            Palette.aEntries[i].peFlags = PC_RESERVED;
        for (; i<PAL_SIZE-nStaticColors; i++)
            Palette.aEntries[i].peFlags = PC_RESERVED;
        for (i=PAL_SIZE-nStaticColors; i<PAL_SIZE; i++)
            Palette.aEntries[i].peFlags = 0;
    }
    ReleaseDC(NULL,hdc);
    for (i=0; i<PAL_SIZE; i++)
    {
        aRGB[i].rgbRed=Palette.aEntries[i].peRed;
        aRGB[i].rgbGreen=Palette.aEntries[i].peGreen;
        aRGB[i].rgbBlue=Palette.aEntries[i].peBlue;
        aRGB[i].rgbReserved=Palette.aEntries[i].peFlags;
    }
    ENDPROFILE(GetPalette)
}

#ifdef DIVE
void WMesaUpdateScreenPos(PWMC c,HWND hWnd)
{
  RECT CR;

  GetWindowRect(hWnd,&CR);

  c->WinPos.x=CR.left+GetSystemMetrics(SM_CXFRAME);
  c->WinPos.y=CR.top+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYFRAME);

  c->width=CR.right-CR.left-2*GetSystemMetrics(SM_CXFRAME);
  c->height=CR.bottom-CR.top-GetSystemMetrics(SM_CYCAPTION)-2*GetSystemMetrics(SM_CYFRAME);
}
#endif

WMesaContext GLAPIENTRY WMesaCreateContext( HWND hWnd, HPALETTE* Pal,
                                GLboolean rgb_flag,
                                GLboolean db_flag )
{
    RECT CR;
    WMesaContext c;
    GLboolean true_color_flag;
    c = (struct wmesa_context * ) calloc(1,sizeof(struct wmesa_context));

    if (!c)
        return NULL;

    c->Window=hWnd;
    c->hDC = GetDC(hWnd);
    true_color_flag = GetDeviceCaps(c->hDC, BITSPIXEL) > 8;

#if defined(DDRAW) || defined(DIVE)
    if(true_color_flag) c->rgb_flag = rgb_flag = GL_TRUE;
#endif


#ifdef DITHER
    if ((true_color_flag==GL_FALSE) && (rgb_flag == GL_TRUE)){
        c->dither_flag = GL_TRUE;
        c->hPalHalfTone = WinGCreateHalftonePalette();
    }
    else
        c->dither_flag = GL_FALSE;
#else
    c->dither_flag = GL_FALSE;
#endif

    if (rgb_flag==GL_FALSE)
    {
        c->rgb_flag = GL_FALSE;
#ifndef DIVE
        c->db_flag = db_flag =GL_TRUE;    /* WinG requires double buffering*/
#endif
    }
    else
    {
        c->rgb_flag = GL_TRUE;
                                       /* c->pixel = 0;                    */
    }

    GetClientRect(c->Window,&CR);

    c->width=CR.right;
    c->height=CR.bottom;

    if (db_flag)
    {
        c->db_flag = 1;
        /* Double buffered */
#if !defined(DDRAW) && !defined(DIVE)
                 /* if (c->rgb_flag==GL_TRUE && c->dither_flag != GL_TRUE )*/
        {
            wmCreateBackingStore(c, c->width, c->height);

        }
#else
#ifdef DIVE
            /* Dive direct access unavailable - switch to DIB mode */
            if(!DiveDirectAccess())
              wmCreateBackingStore(c, c->width, c->height);
#endif
#endif
    }
    else
    {
        /* Single Buffered */
        if (c->rgb_flag)
            c->db_flag = 0;
    }

    /* Also set the correct PixelFormat now! */
    wmSetPixelFormat(c, c->hDC);

#ifdef DIVE
    /* Store window position for DIVE draws - all x/y's are relative to this */
    /* base position of the screen...                                        */
    WMesaUpdateScreenPos(c,hWnd);

    if(DiveDirectAccess())
      if(!DiveInit(c,hWnd))
        {
          free( (void *) c );
          Current=0;
          MessageBoxA( hWnd, "Dive Init FAILED", "", MB_OK );
          EXIT(1);
        }
#else
#ifdef DDRAW
    if (DDInit(c,hWnd) == GL_FALSE) {
        free( (void *) c );
        EXIT(1);
    }
#endif
#endif

/* JvdH - Enable software Alpha! (was FALSE!!) */

    c->gl_visual = gl_create_visual(rgb_flag,
                                    GL_TRUE,              /* software alpha*/
                                    db_flag,                     /* db_flag*/
                                    GL_FALSE,                     /* stereo*/
                                    16,                       /* depth_bits*/
                                    8,                      /* stencil_bits*/
                                    8,                        /* accum_bits*/
                                    0,                        /* index bits*/
                                    8,8,8,8 );           /* r, g, b, a bits*/

    if (!c->gl_visual) {
        return NULL;
    }

    /* allocate a new Mesa context */
    c->gl_ctx = gl_create_context( c->gl_visual, NULL, c, GL_TRUE);

    if (!c->gl_ctx) {
        gl_destroy_visual( c->gl_visual );
        free(c);
        return NULL;
    }

    c->gl_buffer = gl_create_framebuffer( c->gl_visual,
                                          c->gl_visual->DepthBits > 0,
                                          c->gl_visual->StencilBits > 0,
                                          c->gl_visual->AccumBits > 0,
                                          c->gl_visual->AlphaBits > 0);

    if (!c->gl_buffer) {
        gl_destroy_visual( c->gl_visual );
        gl_destroy_context( c->gl_ctx );
        free(c);
        return NULL;
    }

    c->gl_ctx->Driver.UpdateState = setup_DD_pointers;

                                       /* setup_DD_pointers(c->gl_ctx);    */

    return c;
}

void GLAPIENTRY WMesaDestroyContext( void )
{
    WMesaContext c = Current;

    ReleaseDC(c->Window,c->hDC);

    WC = c;

    if(c->hPalHalfTone != NULL)
        DeleteObject(c->hPalHalfTone);

    gl_destroy_visual( c->gl_visual );
    gl_destroy_framebuffer( c->gl_buffer );
    gl_destroy_context( c->gl_ctx );

    if (c->db_flag)
#ifdef DIVE
    if(DiveDirectAccess())
      {
        DiveFree(c);
                                       /* Clean up the screen on exit      */
        RedrawWindow( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE |
            RDW_ALLCHILDREN );
      }
    else
      wmDeleteBackingStore(c);
#else
#ifdef DDRAW
      {
        DDFree(c);
      }
#else
    wmDeleteBackingStore(c);
#endif
#endif
    free( (void *) c );

    /* Set current context to 0, otherwise exitlist-routine will trap */
    Current=0;
                       /* Following code is added to enable parallel render*/
                         /* Parallel render only work in double buffer mode*/
#if !defined(NO_PARALLEL)
    if(parallelMachine)
        PRDestroyRenderBuffer();
#endif
                                       /* End modification                 */
}

void GLAPIENTRY WMesaMakeCurrent( WMesaContext c )
{
    if(!c){
        Current = c;
        return;
    }

    //
    // A little optimization
    // If it already is current,
    // don't set it again
    //
    if(Current == c)
        return;

    //gl_set_context( c->gl_ctx );
    gl_make_current(c->gl_ctx, c->gl_buffer);
    setup_DD_pointers(c->gl_ctx);
    Current = c;
    if (Current->gl_ctx->Viewport.Width==0) {
        /* initialize viewport to window size */
        gl_Viewport( Current->gl_ctx,
            0, 0, Current->width, Current->height );
    }
    if ((c->cColorBits <= 8 ) && (c->rgb_flag == GL_TRUE)){
        WMesaPaletteChange(c->hPalHalfTone);
    }
}



void GLAPIENTRY WMesaSwapBuffers( void )
{

    HDC DC = Current->hDC;

    if (Current->db_flag)
        wmFlush(Current);
}



void GLAPIENTRY WMesaPaletteChange(HPALETTE Pal)
{
    int vRet;
    LPPALETTEENTRY pPal;
    ULONG rc;

    if (Current && (Current->rgb_flag==GL_FALSE || Current->dither_flag == GL_TRUE))
    {
        pPal = (PALETTEENTRY *)malloc( 256 * sizeof(PALETTEENTRY));
        Current->hPal=Pal;
                                 /* GetPaletteEntries( Pal, 0, 256, pPal );*/
        GetPalette( Pal, (RGBQUAD *)pPal );
#ifdef DIVE
/*        rc=DiveSetSourcePalette(Current->hDiveInstance,
                                0,
                                256,
                                (PBYTE)pPal);
*/
#else
#ifdef DDRAW
        Current->lpDD->lpVtbl->CreatePalette(Current->lpDD,DDPCAPS_8BIT,
            pPal, &(Current->lpDDPal), NULL);
        if (Current->lpDDPal)
            Current->lpDDSPrimary->lpVtbl->SetPalette(Current->lpDDSPrimary,Current->lpDDPal);
#else
        vRet = SetDIBColorTable(Current->dib.hDC,0,256, (RGBQUAD *)pPal);
#endif
#endif
        free( pPal );
    }

}




static unsigned char threeto8[8] = {
    0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

static unsigned char twoto8[4] = {
    0, 0x55, 0xaa, 0xff
};

static unsigned char oneto8[2] = {
    0, 255
};

static unsigned char componentFromIndex(UCHAR i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = i >> shift;
    switch (nbits) {

    case 1:
        val &= 0x1;
        return oneto8[val];

    case 2:
        val &= 0x3;
        return twoto8[val];

    case 3:
        val &= 0x7;
        return threeto8[val];

    default:
        return 0;
    }
}

void /*WINAPI*/ wmCreatePalette( PWMC pwdc )
{
    /* Create a compressed and re-expanded 3:3:2 palette */
    int            i;
    LOGPALETTE     *pPal;
    BYTE           rb, rs, gb, gs, bb, bs;

    pwdc->nColors = 0x100;

    pPal = (PLOGPALETTE)malloc(sizeof(LOGPALETTE) + pwdc->nColors * sizeof(PALETTEENTRY));
    memset( pPal, 0, sizeof(LOGPALETTE) + pwdc->nColors * sizeof(PALETTEENTRY) );

    pPal->palVersion = 0x300;

    rb = REDBITS;
    rs = REDSHIFT;
    gb = GREENBITS;
    gs = GREENSHIFT;
    bb = BLUEBITS;
    bs = BLUESHIFT;

    if (pwdc->db_flag) {

        /* Need to make two palettes: one for the screen DC and one for the DIB. */
        pPal->palNumEntries = pwdc->nColors;
        for (i = 0; i < pwdc->nColors; i++) {
            pPal->palPalEntry[i].peRed = componentFromIndex( i, rb, rs );
            pPal->palPalEntry[i].peGreen = componentFromIndex( i, gb, gs );
            pPal->palPalEntry[i].peBlue = componentFromIndex( i, bb, bs );
            pPal->palPalEntry[i].peFlags = 0;
        }
        pwdc->hGLPalette = CreatePalette( pPal );
        pwdc->hPalette = CreatePalette( pPal );
    }

    else {
        pPal->palNumEntries = pwdc->nColors;
        for (i = 0; i < pwdc->nColors; i++) {
            pPal->palPalEntry[i].peRed = componentFromIndex( i, rb, rs );
            pPal->palPalEntry[i].peGreen = componentFromIndex( i, gb, gs );
            pPal->palPalEntry[i].peBlue = componentFromIndex( i, bb, bs );
            pPal->palPalEntry[i].peFlags = 0;
        }
        pwdc->hGLPalette = CreatePalette( pPal );
    }

    free(pPal);

}

void DibSetPixel(PWMC pwc, int iScanLine, int iPixel, BYTE r, BYTE g, BYTE b)
{
   if (Current->db_flag)
   {
       LPBYTE  lpb = pwc->pbPixels;
       LPDWORD lpdw;
       LPWORD  lpw;
       UINT    nBypp = pwc->cColorBits >> 3;
       UINT    nOffset = iPixel % nBypp;

       // Move the pixel buffer pointer to the scanline that we
       // want to access

       //      pwc->dib.fFlushed = FALSE;

       lpb += pwc->ScanWidth * iScanLine;
                                      /* Now move to the desired pixel    */
       lpb += iPixel * nBypp;
       lpb = PIXELADDR(iPixel, iScanLine);
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
   else{
       SetPixel(Current->hDC, iPixel, iScanLine, RGB(r,g,b));
       DD_RELEASEDC;
   }
}

void /*WINAPI*/ wmSetPixel(PWMC pwc, int iScanLine, int iPixel, BYTE r, BYTE g, BYTE b)
{
#ifdef DIVE
  if(DiveDirectAccess())
    {
      if (Current->db_flag)
        {
          /* We're using a buffered context - write to the backbuffer */
          DiveWriteBackbuffer(/*pwc*/Current,iScanLine,iPixel,r,g,b);
        }
      else
        {
          /* We're using a non-buffered context - write to the video mem! */
          /* TODO: What about cards that do not support direct access??   */
          DiveWriteFrontbuffer(/*pwc*/Current,iScanLine,iPixel,r,g,b);
        }
    }
  else
    DibSetPixel(pwc, iScanLine, iPixel, r, g, b);
#else
   DibSetPixel(pwc, iScanLine, iPixel, r, g, b);
#endif
}

void /*WINAPI*/ wmCreateDIBSection(
                                   HDC   hDC,
                                   PWMC pwc,   /* handle of device context */
                                   CONST BITMAPINFO *pbmi,/* address of structure containing bitmap size, format, and color data*/
                                   UINT iUsage/* color data type indicator: RGB values or palette indices*/
                                   )
{
    DWORD   dwSize = 0;
    DWORD   dwScanWidth;
    UINT    nBypp = pwc->cColorBits / 8;
    HDC     hic;

    dwScanWidth = (((pwc->ScanWidth * nBypp)+ 3) & ~3);

    pwc->ScanWidth =pwc->pitch = dwScanWidth;

    if (stereo_flag)
        pwc->ScanWidth = 2* pwc->pitch;

    dwSize = sizeof(BITMAPINFO) + (dwScanWidth * pwc->height);

#ifdef __WIN32OS2__
    pwc->dib.hFileMap = CreateFileMappingA((HANDLE)PAGE_FILE,
#else
    pwc->dib.hFileMap = CreateFileMapping((HANDLE)PAGE_FILE,
#endif
        NULL,
        PAGE_READWRITE | SEC_COMMIT,
        0,
        dwSize,
        NULL);

    if (!pwc->dib.hFileMap)
        return;

    pwc->dib.base = MapViewOfFile(pwc->dib.hFileMap,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0);

    if(!pwc->dib.base){
        CloseHandle(pwc->dib.hFileMap);
        return;
    }

    //  pwc->pbPixels = pwc->addrOffScreen = ((LPBYTE)pwc->dib.base) + sizeof(BITMAPINFO);

    //  pwc->dib.hDC = CreateCompatibleDC(hDC);

    memcpy(pwc->dib.base, pbmi, sizeof(BITMAPINFO));

#ifdef __WIN32OS2__
    hic = CreateICA("display", NULL, NULL, NULL);
#else
    hic = CreateIC("display", NULL, NULL, NULL);
#endif
    pwc->dib.hDC = CreateCompatibleDC(hic);


    /*  pwc->hbmDIB = CreateDIBitmap(hic,
    &(pwc->bmi.bmiHeader),
    CBM_INIT,
    pwc->pbPixels,
    &(pwc->bmi),
    DIB_RGB_COLORS);
    */
    pwc->hbmDIB = CreateDIBSection(hic,
        &(pwc->bmi),
        (iUsage ? DIB_PAL_COLORS : DIB_RGB_COLORS),
        (LPVOID*)&(pwc->pbPixels),
        pwc->dib.hFileMap,
        0);
        /*
        pwc->hbmDIB = CreateDIBSection(hic,
        &(pwc->bmi),
        DIB_RGB_COLORS,
        &(pwc->pbPixels),
        pwc->dib.hFileMap,
        0);
    */

    pwc->ScreenMem = pwc->addrOffScreen = pwc->pbPixels;
    pwc->hOldBitmap = SelectObject(pwc->dib.hDC, pwc->hbmDIB);

    DeleteDC(hic);

    return;

}

//
// Blit memory DC to screen DC
//
BOOL /*WINAPI*/ wmFlush(PWMC pwc)
{
    BOOL    bRet = 0;
    DWORD   dwErr = 0;
#ifdef DIVE
    ULONG rc;
#endif
#ifdef DDRAW
    HRESULT             ddrval;
#endif

    if(pwc->db_flag){
#ifdef DIVE
       if(DiveDirectAccess())
         {
           DiveFlush(pwc);
         }
       else
         bRet = BitBlt(pwc->hDC, 0, 0, pwc->width, pwc->height,
                       pwc->dib.hDC, 0, 0, SRCCOPY);
#else
#ifdef DDRAW
        if (pwc->lpDDSOffScreen == NULL)
            if(DDCreateOffScreen(pwc) == GL_FALSE)
                return 0;

            pwc->lpDDSOffScreen->lpVtbl->Unlock(pwc->lpDDSOffScreen, NULL);

            while( 1 )
            {
                ddrval = pwc->lpDDSPrimary->lpVtbl->Blt( pwc->lpDDSPrimary,
                    &(pwc->rectSurface), pwc->lpDDSOffScreen, &(pwc->rectOffScreen), 0, NULL );

                if( ddrval == DD_OK )
                {
                    break;
                }
                if( ddrval == DDERR_SURFACELOST )
                {
                    if(!DDRestoreAll(pwc))
                    {
                        break;
                    }
                }
                if( ddrval != DDERR_WASSTILLDRAWING )
                {
                    break;
                }
            }

            while (pwc->lpDDSOffScreen->lpVtbl->Lock(pwc->lpDDSOffScreen,
                NULL, &(pwc->ddsd), 0, NULL) == DDERR_WASSTILLDRAWING)
                ;

            if(ddrval != DD_OK)
                dwErr = GetLastError();
#else
            bRet = BitBlt(pwc->hDC, 0, 0, pwc->width, pwc->height,
                pwc->dib.hDC, 0, 0, SRCCOPY);
#endif
#endif
    }

    return(TRUE);

}


// The following code is added by Li Wei to enable stereo display

#if !defined(NO_STEREO)

void WMesaShowStereo(GLuint list)
{

    GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    GLfloat cm[16];
    GLint matrix_mode;
    // Must use double Buffer
    if( ! Current-> db_flag )
        return;


    glGetIntegerv(GL_MATRIX_MODE,&matrix_mode);

    //  glPushMatrix();  //****
    WMesaViewport(Current->gl_ctx,0,Current->height/2,Current->width,Current->height/2);
    //  Current->gl_ctx->NewState = 0;

    //  glViewport(0,0,Current->width,Current->height/2);
    if(matrix_mode!=GL_MODELVIEW)
        glMatrixMode(GL_MODELVIEW);

    glGetFloatv(GL_MODELVIEW_MATRIX,cm);
    glLoadIdentity();
    gluLookAt(viewDistance/2,0.0,0.0 ,
        viewDistance/2,0.0,-1.0,
        0.0,1.0,0.0 );
    //  glTranslatef(viewDistance/2.0,0.,0.);
    glMultMatrixf( cm );

    Current->ScreenMem = Current->pbPixels = Current->addrOffScreen;
    //glPushMatrix();
    glCallList( list );
    //glPopMatrix();

    glGetFloatv(GL_MODELVIEW_MATRIX,cm);
    glLoadIdentity();
    gluLookAt(-viewDistance/2,0.0,0.0 ,
        -viewDistance/2,0.0,-1.0,
        0.0,1.0,0.0 );
    //  glTranslatef(-viewDistance/2.0,0.,0.);
    glMultMatrixf(cm);

    Current->ScreenMem = Current->pbPixels = Current->addrOffScreen + Current->pitch;
    glCallList(list);
    if(matrix_mode!=GL_MODELVIEW)
        glMatrixMode(matrix_mode);

    //  glPopMatrix();
    glFlush();

    WMesaViewport(Current->gl_ctx,0,0,Current->width,Current->height);
    //  Current->gl_ctx->NewState = 0;
    WMesaSwapBuffers();

}

void toggleStereoMode()
{
    if(!Current->db_flag)
        return;
    if(!stereo_flag){
        stereo_flag = 1;
        if(stereoBuffer==GL_FALSE)
#if !defined(NO_PARALLEL)
            if(!parallelFlag)
#endif
            {
                Current->ScanWidth = Current->pitch*2;
            }
    }
    else {
        stereo_flag = 0;
#if !defined(NO_PARALLEL)
        if(!parallelFlag)
#endif
            Current->ScanWidth = Current->pitch;
        Current->pbPixels = Current->addrOffScreen;
    }
}

/* if in stereo mode, the following function is called */
void glShowStereo(GLuint list)
{
    WMesaShowStereo(list);
}

#endif // End if NO_STEREO not defined

#if !defined(NO_PARALLEL)

void toggleParallelMode(void)
{
    if(!parallelFlag){
        parallelFlag = GL_TRUE;
        if(parallelMachine==GL_FALSE){
            PRCreateRenderBuffer( Current->rgb_flag? GL_RGBA :GL_COLOR_INDEX,
                Current->cColorBits/8,
                Current->width ,Current->height,
                Current->ScanWidth,
                Current->rgb_flag? Current->pbPixels: Current->ScreenMem);
            parallelMachine = GL_TRUE;
        }
    }
    else {
        parallelFlag = GL_FALSE;
        if(parallelMachine==GL_TRUE){
            PRDestroyRenderBuffer();
            parallelMachine=GL_FALSE;
            ReadyForNextFrame = GL_TRUE;
        }

        /***********************************************
        // Seems something wrong!!!!
        ************************************************/

        WMesaMakeCurrent(Current);
#if !defined(NO_STEREO)
        stereo_flag = GL_FALSE ;
#endif
    }
}

void PRShowRenderResult(void)
{
    int flag = 0;
    if(!glImageRendered())
        return;

    if (parallelFlag)
    {
        WMesaSwapBuffers();
    }

}
#endif //End if NO_PARALLEL not defined

//end modification

BYTE DITHER_RGB_2_8BIT( int red, int green, int blue, int pixel, int scanline)
{
    char unsigned redtemp, greentemp, bluetemp, paletteindex;

    //*** now, look up each value in the halftone matrix
    //*** using an 8x8 ordered dither.
    redtemp = aDividedBy51[red]
        + (aModulo51[red] > aHalftone8x8[(pixel%8)*8
        + scanline%8]);
    greentemp = aDividedBy51[(char unsigned)green]
        + (aModulo51[green] > aHalftone8x8[
        (pixel%8)*8 + scanline%8]);
    bluetemp = aDividedBy51[(char unsigned)blue]
        + (aModulo51[blue] > aHalftone8x8[
        (pixel%8)*8 +scanline%8]);

    //*** recombine the halftoned rgb values into a palette index
    paletteindex =
        redtemp + aTimes6[greentemp] + aTimes36[bluetemp];

    //*** and translate through the wing halftone palette
    //*** translation vector to give the correct value.
    return aWinGHalftoneTranslation[paletteindex];
}

#ifdef DDRAW
/*
* restoreAll
*
* restore all lost objects
*/
HRESULT DDRestoreAll( WMesaContext wc )
{
    HRESULT     ddrval;

    ddrval = wc->lpDDSPrimary->lpVtbl->Restore(wc->lpDDSPrimary);
    if( ddrval == DD_OK )
    {
        ddrval = wc->lpDDSOffScreen->lpVtbl->Restore(wc->lpDDSOffScreen);
    }
    return ddrval;

} /* restoreAll */


  /*
  * This function is called if the initialization function fails
*/
BOOL initFail( HWND hwnd, WMesaContext wc )
{
    DDFree(wc);
#ifdef __WIN32OS2__
    MessageBoxA( hwnd, "DirectDraw Init FAILED", "", MB_OK );
#else
    MessageBox( hwnd, "DirectDraw Init FAILED", "", MB_OK );
#endif
    return FALSE;

} /* initFail */


static void DDDeleteOffScreen(WMesaContext wc)
{
    if( wc->lpDDSOffScreen != NULL )
    {
        wc->lpDDSOffScreen->lpVtbl->Unlock(wc->lpDDSOffScreen,NULL);
        wc->lpDDSOffScreen->lpVtbl->Release(wc->lpDDSOffScreen);
        wc->lpDDSOffScreen = NULL;
    }

}

static void DDFreePrimarySurface(WMesaContext wc)
{
    if( wc->lpDDSPrimary != NULL )
    {
        if(wc->db_flag == GL_FALSE)
            wc->lpDDSPrimary->lpVtbl->ReleaseDC(wc->lpDDSPrimary, wc->hDC);
        wc->lpDDSPrimary->lpVtbl->Release(wc->lpDDSPrimary);
        wc->lpDDSPrimary = NULL;
    }
}

static BOOL DDCreatePrimarySurface(WMesaContext wc)
{
    HRESULT ddrval;
    DDSCAPS             ddscaps;
    wc->ddsd.dwSize = sizeof( wc->ddsd );
    wc->ddsd.dwFlags = DDSD_CAPS;
    wc->ddsd.ddsCaps.dwCaps =   DDSCAPS_PRIMARYSURFACE;

    ddrval = wc->lpDD->lpVtbl->CreateSurface( wc->lpDD,&(wc->ddsd), &(wc->lpDDSPrimary), NULL );
    if( ddrval != DD_OK )
    {
        return initFail(wc->hwnd , wc);
    }
    if(wc->db_flag == GL_FALSE)
        wc->lpDDSPrimary->lpVtbl->GetDC(wc->lpDDSPrimary, &wc->hDC);
    return TRUE;
}

static BOOL DDCreateOffScreen(WMesaContext wc)
{
    POINT   pt;
    HRESULT     ddrval;

    if(wc->lpDD == NULL)
        return FALSE;
    GetClientRect( wc->hwnd, &(wc->rectOffScreen) );
    wc->ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    wc->ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    wc->ddsd.dwHeight = wc->rectOffScreen.bottom - wc->rectOffScreen.top;
    wc->ddsd.dwWidth = wc->rectOffScreen.right - wc->rectOffScreen.left;

    ddrval = wc->lpDD->lpVtbl->CreateSurface( wc->lpDD, &(wc->ddsd), &(wc->lpDDSOffScreen), NULL );
    if( ddrval != DD_OK )
    {
        return FALSE;
    }

    while (wc->lpDDSOffScreen->lpVtbl->Lock(wc->lpDDSOffScreen,NULL, &(wc->ddsd), 0, NULL) == DDERR_WASSTILLDRAWING)
        ;
/* while ((ddrval = wc->lpDDSOffScreen->lpVtbl->Lock(wc->lpDDSOffScreen,NULL, &(wc->ddsd), DDLOCK_SURFACEMEMORYPTR , NULL)) != DD_OK)*/
    ;
    if(wc->ddsd.lpSurface==NULL)
        return initFail(wc->hwnd, wc);

    wc->ScreenMem = wc->pbPixels = wc->addrOffScreen = (PBYTE)(wc->ddsd.lpSurface);
    wc->ScanWidth = wc->pitch = wc->ddsd.lPitch;

    if (stereo_flag)
        wc->ScanWidth = wc->ddsd.lPitch*2;

    GetClientRect( wc->hwnd, &(wc->rectSurface) );
    pt.x = pt.y = 0;
    ClientToScreen( wc->hwnd, &pt );
    OffsetRect(&(wc->rectSurface), pt.x, pt.y);
    wmSetPixelFormat(wc, wc->hDC);
    return TRUE;
}

/*
* doInit - do work required for every instance of the application:
*                create the window, initialize data
*/
static BOOL DDInit( WMesaContext wc, HWND hwnd)
{
    HRESULT             ddrval;
    DWORD dwFrequency;

    LPDIRECTDRAW            lpDD;           // DirectDraw object
    LPDIRECTDRAW2            lpDD2;


    wc->fullScreen = displayOptions.fullScreen;
    wc->gMode = displayOptions.mode;
    wc->hwnd = hwnd;
    stereo_flag = displayOptions.stereo;
    if(wc->db_flag!= GL_TRUE)
        stereo_flag = GL_FALSE;
        /*
        * create the main DirectDraw object
    */
    ddrval = DirectDrawCreate( NULL, &(wc->lpDD), NULL );
    if( ddrval != DD_OK )
    {
        return initFail(hwnd,wc);
    }

    // Get exclusive mode if requested
    if(wc->fullScreen)
    {
        ddrval = wc->lpDD->lpVtbl->SetCooperativeLevel( wc->lpDD, hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
    }
    else
    {
        ddrval = wc->lpDD->lpVtbl->SetCooperativeLevel( wc->lpDD, hwnd, DDSCL_NORMAL );
    }
    if( ddrval != DD_OK )
    {
        return initFail(hwnd , wc);
    }


    /*  ddrval = wc->lpDD->lpVtbl->QueryInterface(wc->lpDD, IID_IDirectDraw2,
    (LPVOID *)((wc->lpDD2)));

    */
    if(ddrval != DD_OK)
        return initFail(hwnd , wc);


    //ddrval = wc->lpDD->lpVtbl->GetDisplayMode( wc->lpDD, &(wc->ddsd));
    //  wc->lpDD2->lpVtbl->GetMonitorFrequency(wc->lpDD, &dwFrequency);
    switch( wc->gMode )
    {
    case 1:  ddrval = wc->lpDD->lpVtbl->SetDisplayMode( wc->lpDD, 640, 480, displayOptions.bpp); break;
    case 2:  ddrval = wc->lpDD->lpVtbl->SetDisplayMode( wc->lpDD, 800, 600, displayOptions.bpp); break;
    case 3:  ddrval = wc->lpDD->lpVtbl->SetDisplayMode( wc->lpDD, 1024, 768, displayOptions.bpp); break;
    case 4:  ddrval = wc->lpDD->lpVtbl->SetDisplayMode( wc->lpDD, 1152, 864, displayOptions.bpp); break;
    case 5:  ddrval = wc->lpDD->lpVtbl->SetDisplayMode( wc->lpDD, 1280, 1024, displayOptions.bpp); break;
    }

    if( ddrval != DD_OK )
    {
        printf("Can't modify display mode, current mode used\n");
        //        return initFail(hwnd , wc);
    }
    //ddrval = wc->lpDD->lpVtbl->GetDisplayMode( wc->lpDD, &(wc->ddsd));
    switch(ddrval){
    case DDERR_INVALIDOBJECT:
        break;
    case DDERR_INVALIDPARAMS:
        break;
    case DDERR_UNSUPPORTEDMODE:
        ;
    }

    if(DDCreatePrimarySurface(wc) == GL_FALSE)
        return initFail(hwnd, wc);

    if(wc->db_flag)
        return DDCreateOffScreen(wc);

   return TRUE;
}                                                                 /* DDInit*/

static void DDFree( WMesaContext wc)
{
    if( wc->lpDD != NULL )
    {
        DDFreePrimarySurface(wc);
        DDDeleteOffScreen(wc);
        wc->lpDD->lpVtbl->Release(wc->lpDD);
        wc->lpDD = NULL;
    }
                                       /* Clean up the screen on exit      */
    RedrawWindow( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE |
        RDW_ALLCHILDREN );

}
#endif

void WMesaMove(void)
{
    WMesaContext wc = Current;
    POINT   pt;
    if (Current != NULL){
        GetClientRect( wc->hwnd, &(wc->rectSurface) );
        pt.x = pt.y = 0;
        ClientToScreen( wc->hwnd, &pt );
        OffsetRect(&(wc->rectSurface), pt.x, pt.y);
    }
}

/*
* Like PACK_8A8B8G8R() but don't use alpha.  This is usually an acceptable
* shortcut.
*/
#define PACK_8B8G8R( R, G, B )   ( ((B) << 16) | ((G) << 8) | (R) )


/**********************************************************************/
/***                   Triangle rendering                            ***/
/**********************************************************************/

/*
 * XImage, smooth, depth-buffered, PF_8A8B8G8R triangle.
 */
static void smooth_8A8B8G8R_z_triangle( GLcontext *ctx,
                                       GLuint v0, GLuint v1, GLuint v2,
                                       GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint i, len = RIGHT-LEFT;                      \
    for (i=0;i<len;i++) {                       \
    GLdepth z = FixedToDepth(ffz);                  \
    if (z < zRow[i]) {                      \
    pRow[i] = PACK_8B8G8R( FixedToInt(ffr), FixedToInt(ffg),    \
    FixedToInt(ffb) );          \
    zRow[i] = z;                            \
    }                                   \
    ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;         \
    ffz += fdzdx;                           \
    }                                   \
    }

#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, smooth, depth-buffered, PF_8R8G8B triangle.
*/
static void smooth_8R8G8B_z_triangle( GLcontext *ctx,
                                     GLuint v0, GLuint v1, GLuint v2,
                                     GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint i, len = RIGHT-LEFT;                      \
    for (i=0;i<len;i++) {                       \
    GLdepth z = FixedToDepth(ffz);                  \
    if (z < zRow[i]) {                      \
    pRow[i] = PACK_8R8G8B( FixedToInt(ffr), FixedToInt(ffg),    \
    FixedToInt(ffb) );          \
    zRow[i] = z;                            \
    }                                   \
    ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;         \
    ffz += fdzdx;                           \
    }                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}



/*
* XImage, smooth, depth-buffered, PF_5R6G5B triangle.
*/
static void smooth_5R6G5B_z_triangle( GLcontext *ctx,
                                     GLuint v0, GLuint v1, GLuint v2,
                                     GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint i, len = RIGHT-LEFT;                      \
    for (i=0;i<len;i++) {                       \
    GLdepth z = FixedToDepth(ffz);                  \
    if (z < zRow[i]) {                      \
    pRow[i] = PACK_5R6G5B( FixedToInt(ffr), FixedToInt(ffg),    \
    FixedToInt(ffb) );          \
    zRow[i] = z;                            \
    }                                   \
    ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;         \
    ffz += fdzdx;                           \
    }                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}

/*
* XImage, flat, depth-buffered, PF_8A8B8G8R triangle.
*/
static void flat_8A8B8G8R_z_triangle( GLcontext *ctx, GLuint v0,
                                     GLuint v1, GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                  \
    unsigned long p = PACK_8B8G8R( VB->ColorPtr->data[pv][0],    \
    VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint i, len = RIGHT-LEFT;                      \
    for (i=0;i<len;i++) {                       \
    GLdepth z = FixedToDepth(ffz);                  \
    if (z < zRow[i]) {                      \
    pRow[i] = p;                            \
    zRow[i] = z;                            \
    }                                   \
    ffz += fdzdx;                           \
    }                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, flat, depth-buffered, PF_8R8G8B triangle.
*/
static void flat_8R8G8B_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                   GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                  \
    unsigned long p = PACK_8R8G8B( VB->ColorPtr->data[pv][0],    \
    VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )            \
    {                           \
    GLint i, len = RIGHT-LEFT;              \
    for (i=0;i<len;i++) {               \
    GLdepth z = FixedToDepth(ffz);          \
    if (z < zRow[i]) {              \
    pRow[i] = p;                    \
    zRow[i] = z;                    \
    }                           \
    ffz += fdzdx;                   \
    }                           \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, flat, depth-buffered, PF_5R6G5B triangle.
*/
static void flat_5R6G5B_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                   GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                  \
    unsigned long p = PACK_5R6G5B( VB->ColorPtr->data[pv][0],    \
    VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )            \
    {                           \
    GLint i, len = RIGHT-LEFT;              \
    for (i=0;i<len;i++) {               \
    GLdepth z = FixedToDepth(ffz);          \
    if (z < zRow[i]) {              \
    pRow[i] = p;                    \
    zRow[i] = z;                    \
    }                           \
    ffz += fdzdx;                   \
    }                           \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, smooth, NON-depth-buffered, PF_8A8B8G8R triangle.
*/
static void smooth_8A8B8G8R_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                     GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint xx;                               \
    PIXEL_TYPE *pixel = pRow;                       \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {               \
    *pixel = PACK_8B8G8R( FixedToInt(ffr), FixedToInt(ffg),     \
                FixedToInt(ffb) );          \
                ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;         \
    }                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, smooth, NON-depth-buffered, PF_8R8G8B triangle.
*/
static void smooth_8R8G8B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                   GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint xx;                               \
    PIXEL_TYPE *pixel = pRow;                       \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {               \
    *pixel = PACK_8R8G8B( FixedToInt(ffr), FixedToInt(ffg),     \
                FixedToInt(ffb) );          \
                ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;         \
    }                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, smooth, NON-depth-buffered, PF_5R6G5B triangle.
*/
static void smooth_5R6G5B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                   GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint xx;                               \
    PIXEL_TYPE *pixel = pRow;                       \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {               \
    *pixel = PACK_5R6G5B( FixedToInt(ffr), FixedToInt(ffg),     \
                FixedToInt(ffb) );          \
                ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;         \
    }                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}



/*
* XImage, flat, NON-depth-buffered, PF_8A8B8G8R triangle.
*/
static void flat_8A8B8G8R_triangle( GLcontext *ctx, GLuint v0,
                                   GLuint v1, GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                  \
    unsigned long p = PACK_8B8G8R( VB->ColorPtr->data[pv][0],    \
    VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )            \
    {                           \
    GLint xx;                       \
    PIXEL_TYPE *pixel = pRow;               \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {       \
    *pixel = p;                 \
    }                           \
    }

#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, flat, NON-depth-buffered, PF_8R8G8B triangle.
*/
static void flat_8R8G8B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                 GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                  \
    unsigned long p = PACK_8R8G8B( VB->ColorPtr->data[pv][0],    \
    VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )            \
    {                           \
    GLint xx;                       \
    PIXEL_TYPE *pixel = pRow;               \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {       \
    *pixel = p;                 \
    }                           \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, flat, NON-depth-buffered, PF_5R6G5B triangle.
*/
static void flat_5R6G5B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                 GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
    //#define BYTES_PER_ROW (wmesa->xm_buffer->backimage->bytes_per_line)
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                  \
    unsigned long p = PACK_5R6G5B( VB->ColorPtr->data[pv][0],    \
    VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )            \
    {                           \
    GLint xx;                       \
    PIXEL_TYPE *pixel = pRow;               \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {       \
    *pixel = p;                 \
    }                           \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, smooth, depth-buffered, 8-bit PF_LOOKUP triangle.
*/

static void smooth_ci_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                 GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_INDEX 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                                \
    {                                                                   \
    GLint i, len = RIGHT-LEFT;                                      \
    for (i=0;i<len;i++) {                                           \
    GLdepth z = FixedToDepth(ffz);                              \
    if (z < zRow[i]) {                                          \
    pRow[i] = FixedToInt(ffi);                                  \
    zRow[i] = z;                                                \
    }                                                               \
    ffi += fdidx;                                                   \
    ffz += fdzdx;                                                   \
    }                                                               \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, flat, depth-buffered, 8-bit PF_LOOKUP triangle.
*/

static void flat_ci_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                               GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                      \
   GLuint index = VB->IndexPtr->data[pv];       \
   (*ctx->Driver.Index)( ctx, index );
#define INNER_LOOP( LEFT, RIGHT, Y )    \
   {                                    \
      GLint i, len = RIGHT-LEFT;        \
      for (i=0;i<len;i++) {             \
         GLdepth z = FixedToDepth(ffz); \
         if (z < zRow[i]) {             \
            pRow[i] = index;            \
            zRow[i] = z;                \
         }                              \
         ffz += fdzdx;                  \
      }                                 \
   }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}



/*
* XImage, smooth, NON-depth-buffered, 8-bit PF_LOOKUP triangle.
*/

static void smooth_ci_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                               GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_INDEX 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                    \
    {                                   \
    GLint xx;                               \
    PIXEL_TYPE *pixel = pRow;                       \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {               \
    *pixel = FixedToInt(ffi);           \
    ffi += fdidx;           \
    }                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}


/*
* XImage, flat, NON-depth-buffered, 8-bit PF_LOOKUP triangle.
*/
static void flat_ci_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                             GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define SETUP_CODE                      \
   GLuint index = VB->IndexPtr->data[pv];       \
   (*ctx->Driver.Index)( ctx, index );
#define INNER_LOOP( LEFT, RIGHT, Y )            \
   {                                            \
      GLint xx;                                 \
      PIXEL_TYPE *pixel = pRow;                 \
      for (xx=LEFT;xx<RIGHT;xx++,pixel++) {     \
         *pixel = index;                        \
      }                                         \
   }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}

/*
* XImage, smooth, depth-buffered, 8-bit, PF_DITHER8 triangle.
*/
static void smooth_DITHER8_z_triangle( GLcontext *ctx,
                                      GLuint v0, GLuint v1, GLuint v2,
                                      GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
    DITHER_RGB_TO_8BIT_SETUP
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                                    \
    {                                                                       \
    GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;                 \
    for (i=0;i<len;i++,xx++) {                                          \
    GLdepth z = FixedToDepth(ffz);                                  \
    if (z < zRow[i]) {                                              \
    DITHER_RGB_TO_8BIT( FixedToInt(ffr), FixedToInt(ffg),           \
    FixedToInt(ffb), xx, yy);               \
    pRow[i] = pixelDithered;                                        \
    zRow[i] = z;                                                    \
    }                                                                   \
    ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;                     \
    ffz += fdzdx;                                                       \
    }                                                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}

/*
* XImage, flat, depth-buffered, 8-bit PF_DITHER triangle.
*/
static void flat_DITHER8_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
    DITHER_RGB_TO_8BIT_SETUP
#define INTERP_Z 1
#define DEPTH_TYPE DEFAULT_SOFTWARE_DEPTH_TYPE
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)

#define INNER_LOOP( LEFT, RIGHT, Y )                                    \
    {                                                                       \
    GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;                 \
    for (i=0;i<len;i++,xx++) {                                          \
    GLdepth z = FixedToDepth(ffz);                                  \
    if (z < zRow[i]) {                                              \
    DITHER_RGB_TO_8BIT( VB->ColorPtr->data[pv][0],                           \
             VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2], xx, yy);               \
             pRow[i] = pixelDithered;                                       \
             zRow[i] = z;                                                   \
    }                                                                   \
    ffz += fdzdx;                                                       \
    }                                                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}

/*
* XImage, smooth, NON-depth-buffered, 8-bit PF_DITHER triangle.
*/
static void smooth_DITHER8_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
    DITHER_RGB_TO_8BIT_SETUP
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)
#define INNER_LOOP( LEFT, RIGHT, Y )                                    \
    {                                                                       \
    GLint xx, yy = FLIP(Y);                                             \
    PIXEL_TYPE *pixel = pRow;                                           \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {                               \
    DITHER_RGB_TO_8BIT( VB->ColorPtr->data[pv][0],   VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2], xx, yy);\
    *pixel = pixelDithered;                                         \
    ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;                     \
    }                                                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}

/*
* XImage, flat, NON-depth-buffered, 8-bit PF_DITHER triangle.
*/

static void flat_DITHER8_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint pv )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
    DITHER_RGB_TO_8BIT_SETUP
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (wmesa->ScanWidth)

#define INNER_LOOP( LEFT, RIGHT, Y )                                    \
    {                                                                       \
    GLint xx, yy = FLIP(Y);                                             \
    PIXEL_TYPE *pixel = pRow;                                           \
    for (xx=LEFT;xx<RIGHT;xx++,pixel++) {                               \
    DITHER_RGB_TO_8BIT( VB->ColorPtr->data[pv][0],                               \
             VB->ColorPtr->data[pv][1], VB->ColorPtr->data[pv][2], xx, yy);               \
             *pixel = pixelDithered;                                            \
    }                                                                   \
    }
#ifdef __MINGW32__
        #include "tritemp.h"
#else

        #ifdef WIN32
                #include "..\tritemp.h"
        #else
                #include "tritemp.h"
        #endif
#endif
}




static triangle_func choose_triangle_function( GLcontext *ctx )
{
    WMesaContext wmesa = (WMesaContext) ctx->DriverCtx;
    int depth = wmesa->cColorBits;

    if (ctx->Polygon.SmoothFlag)     return NULL;
    if (ctx->Texture.Enabled)        return NULL;
    if (!wmesa->db_flag) return NULL;
    /*if (wmesa->xm_buffer->buffer==XIMAGE)*/ {
    if (   ctx->Light.ShadeModel==GL_SMOOTH
        && ctx->RasterMask==DEPTH_BIT
        && ctx->Depth.Func==GL_LESS
        && ctx->Depth.Mask==GL_TRUE
        && ctx->Polygon.StippleFlag==GL_FALSE) {
        switch (wmesa->pixelformat) {
        case PF_8A8B8G8R:
            return smooth_8A8B8G8R_z_triangle;
        case PF_8R8G8B:
            return smooth_8R8G8B_z_triangle;
        case PF_5R6G5B:
            return smooth_5R6G5B_z_triangle;
        case PF_DITHER8:
            return  smooth_DITHER8_z_triangle;
        case PF_INDEX8:
            return smooth_ci_z_triangle;
        default:
            return NULL;
        }
    }
    if (   ctx->Light.ShadeModel==GL_FLAT
        && ctx->RasterMask==DEPTH_BIT
        && ctx->Depth.Func==GL_LESS
        && ctx->Depth.Mask==GL_TRUE
        && ctx->Polygon.StippleFlag==GL_FALSE) {
        switch (wmesa->pixelformat) {
        case PF_8A8B8G8R:
            return flat_8A8B8G8R_z_triangle;
        case PF_8R8G8B:
            return flat_8R8G8B_z_triangle;
        case PF_5R6G5B:
            return flat_5R6G5B_z_triangle;
        case PF_DITHER8:
            return flat_DITHER8_z_triangle;
        case PF_INDEX8:
            return flat_ci_z_triangle;
        default:
            return NULL;
        }
    }
    if (   ctx->RasterMask==0   /* no depth test */
        && ctx->Light.ShadeModel==GL_SMOOTH
        && ctx->Polygon.StippleFlag==GL_FALSE) {
        switch (wmesa->pixelformat) {
        case PF_8A8B8G8R:
            return smooth_8A8B8G8R_triangle;
        case PF_8R8G8B:
            return smooth_8R8G8B_triangle;
        case PF_5R6G5B:
            return smooth_5R6G5B_triangle;
        case PF_DITHER8:
            return smooth_DITHER8_triangle;
        case PF_INDEX8:
            return smooth_ci_triangle;
        default:
            return NULL;
        }
    }

    if (   ctx->RasterMask==0   /* no depth test */
        && ctx->Light.ShadeModel==GL_FLAT
        && ctx->Polygon.StippleFlag==GL_FALSE) {
        switch (wmesa->pixelformat) {
        case PF_8A8B8G8R:
            return flat_8A8B8G8R_triangle;
        case PF_8R8G8B:
            return flat_8R8G8B_triangle;
        case PF_5R6G5B:
            return flat_5R6G5B_triangle;
        case PF_DITHER8:
            return flat_DITHER8_triangle;
        case PF_INDEX8:
            return flat_ci_triangle;
        default:
            return NULL;
        }
    }

    return NULL;
    }
}

/*
* Define a new viewport and reallocate auxillary buffers if the size of
* the window (color buffer) has changed.
*/
void WMesaViewport( GLcontext *ctx,
                   GLint x, GLint y, GLsizei width, GLsizei height )
{
    /* Save viewport */
    ctx->Viewport.X = x;
    ctx->Viewport.Width = width;
    ctx->Viewport.Y = y;
    ctx->Viewport.Height = height;

    /* compute scale and bias values */
/* Pre-Keith 3.1 changes
    ctx->Viewport.Map.m[Sx] = (GLfloat) width / 2.0F;
    ctx->Viewport.Map.m[Tx] = ctx->Viewport.Sx + x;
    ctx->Viewport.Map.m[Sy] = (GLfloat) height / 2.0F;
    ctx->Viewport.Map.m[Ty] = ctx->Viewport.Sy + y;
*/
        ctx->Viewport.WindowMap.m[MAT_SX] = (GLfloat) width / 2.0F;
        ctx->Viewport.WindowMap.m[MAT_TX] = ctx->Viewport.WindowMap.m[MAT_SX] + x;
        ctx->Viewport.WindowMap.m[MAT_SY] = (GLfloat) height / 2.0F;
        ctx->Viewport.WindowMap.m[MAT_TY] = ctx->Viewport.WindowMap.m[MAT_SY] + y;
}
