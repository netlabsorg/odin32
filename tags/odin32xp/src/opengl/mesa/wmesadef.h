/* $Id: wmesadef.h,v 1.3 2000-05-23 20:35:00 jeroen Exp $ */
/*      File name       :       wmesadef.h
 *  Version             :       2.3
 *
 *  Header file for display driver for Mesa 2.3  under
 *      Windows95, WindowsNT and Win32
 *
 *      Copyright (C) 1996-  Li Wei
 *  Address             :               Institute of Artificial Intelligence
 *                              :                       & Robotics
 *                              :               Xi'an Jiaotong University
 *  Email               :               liwei@aiar.xjtu.edu.cn
 *  Web page    :               http://sun.aiar.xjtu.edu.cn
 *
 *  This file and its associations are partially based on the
 *  Windows NT driver for Mesa, written by Mark Leaming
 *  (mark@rsinc.com).
 */

/*
 * $Log: wmesadef.h,v $
 * Revision 1.3  2000-05-23 20:35:00  jeroen
 * *** empty log message ***
 *
 * Revision 1.2  2000/03/01 18:49:40  jeroen
 * *** empty log message ***
 *
 * Revision 1.1  2000/02/29 00:48:44  sandervl
 * created
 *
 * Revision 1.1.1.1  1999/08/19 00:55:42  jtg
 * Imported sources
 *
 * Revision 1.3  1999/01/03 03:08:57  brianp
 * Ted Jump's changes
 *
 * Initial version 1997/6/14 CST by Li Wei(liwei@aiar.xjtu.edu.cn)
 */

/*
 * $Log: wmesadef.h,v $
 * Revision 1.3  2000-05-23 20:35:00  jeroen
 * *** empty log message ***
 *
 * Revision 1.2  2000/03/01 18:49:40  jeroen
 * *** empty log message ***
 *
 * Revision 1.1  2000/02/29 00:48:44  sandervl
 * created
 *
 * Revision 1.1.1.1  1999/08/19 00:55:42  jtg
 * Imported sources
 *
 * Revision 1.3  1999/01/03 03:08:57  brianp
 * Ted Jump's changes
 *
 * Revision 2.1  1996/11/15 10:54:00  CST by Li Wei(liwei@aiar.xjtu.edu.cn)
 * a new element added to wmesa_context :
 * dither_flag
 */

/*
 * $Log: wmesadef.h,v $
 * Revision 1.3  2000-05-23 20:35:00  jeroen
 * *** empty log message ***
 *
 * Revision 1.2  2000/03/01 18:49:40  jeroen
 * *** empty log message ***
 *
 * Revision 1.1  2000/02/29 00:48:44  sandervl
 * created
 *
 * Revision 1.1.1.1  1999/08/19 00:55:42  jtg
 * Imported sources
 *
 * Revision 1.3  1999/01/03 03:08:57  brianp
 * Ted Jump's changes
 *
 * Revision 2.0  1996/11/15 10:54:00  CST by Li Wei(liwei@aiar.xjtu.edu.cn)
 * Initial revision
 */



#ifndef DDMESADEF_H
#define DDMESADEF_H

#if defined(DDRAW)
#ifdef __WIN32OS2__
#define __WINE_DD_OBJ_BASE_H/* Don't include dd_obj_base.h, or the file won't*/
#endif                                 /* compile due to redefined symbols.*/
#include <ddraw.h>          /* Is this a bug in the Odin DDRAW implementation??*/
#endif

#define REDBITS         0x03
#define REDSHIFT        0x00
#define GREENBITS       0x03
#define GREENSHIFT      0x03
#define BLUEBITS        0x02
#define BLUESHIFT       0x06

typedef HANDLE ULONG;

typedef struct _dibSection{
        HDC     hDC;
        HANDLE  hFileMap;
        BOOL    fFlushed;
        LPVOID  base;
}WMDIBSECTION, *PWMDIBSECTION;

typedef struct tagMY_RECTL
  {
    LONG xLeft,xRight,yBottom,yTop;
  }MY_RECTL;

typedef struct wmesa_context{
  /* This defs *MUST* be at the TOP, since some structure definitions below  */
  /* are NOT correct!! This is because of the fact that wmesa is WIN based,  */
  /* whereas this file is OS/2 based. If we put this stuff at the end of the */
  /* structure the offsets to for example the hDiveInstance will be          */
  /* interpreted differently in this file and in wmesa.c.                    */
#ifdef DIVE
    ULONG               hDiveInstance; /* This actually is a DIVE Handle   */
    ULONG               BackBufferNumber;
    PVOID               ppFrameBuffer; /* FrameBuffer address - used for DIRECT access to screen only */
    ULONG               ScanLineBytes,ScanLines;
    POINT               WinPos;
    WNDPROC             hWndProc;
    BOOL                DiveSoftwareBlit;/* If BlitSetup failed do it in SW*/
    ULONG               NumClipRects;  /* Clip-info is maintained here     */
    MY_RECTL            rctls[50];
    HRGN                hrgn;
    HDC                 hps;
    BOOL                BackBufferOwnAllocation;
    GLint               awidth,aheight;/* Allocated w/h for ImageBuffer    */
#endif
    PBYTE               pbPixels;
    int                 nColors;
    BYTE                cColorBits;
    int                 pixelformat;
    GLboolean           db_flag;       /* double buffered?                 */
    GLboolean           rgb_flag;      /* RGB mode?                        */
    GLboolean           dither_flag;/* use dither when 256 color mode for RGB?*/
    GLuint              depth;         /* bits per pixel (1, 8, 24, etc)   */
    ULONG               pixel;   /* current color index or RGBA pixel value*/
    ULONG               clearpixel; //* pixel for clearing the color buffers*/
    GLuint              width;
    GLuint              height;
    unsigned long       ScanWidth;

    GLcontext          *gl_ctx;                 /* The core GL/Mesa context*/
    GLvisual           *gl_visual;                 /* Describes the buffers*/
    GLframebuffer      *gl_buffer;    /* Depth, stencil, accum, etc buffers*/

    /* 3D projection stuff */
    RECT                drawRect;
    UINT                uiDIBoffset;
                                       /* OpenGL stuff                     */
    HPALETTE            hGLPalette;
    PBYTE                           ScreenMem; /* WinG memory              */
    BITMAPINFO                      *IndexFormat;
    HPALETTE                        hPal;      /* Current Palette          */
    HPALETTE                        hPalHalfTone;


    WMDIBSECTION            dib;
    BITMAPINFO          bmi;
    HBITMAP             hbmDIB;
    HBITMAP             hOldBitmap;
    HBITMAP                         Old_Compat_BM;
    HBITMAP                         Compat_BM;/* Bitmap for double buffering*/

    HWND                Window;
    HDC                 hDC;
    HPALETTE            hPalette;
    HPALETTE            hOldPalette;
    HPEN                hPen;
    HPEN                hOldPen;
    HCURSOR             hOldCursor;
    COLORREF            crColor;
#ifdef DDRAW
        LPDIRECTDRAW            lpDD;  /* DirectDraw object                */
//      LPDIRECTDRAW2            lpDD2;                /* DirectDraw object*/
        LPDIRECTDRAWSURFACE     lpDDSPrimary; /* DirectDraw primary surface*/
        LPDIRECTDRAWSURFACE     lpDDSOffScreen; // DirectDraw off screen surface
        LPDIRECTDRAWPALETTE     lpDDPal;        // DirectDraw palette
        BOOL                    bActive;       /* is application active?   */
        DDSURFACEDESC           ddsd;
        int                                     fullScreen;
        int                                 gMode ;
#endif
        RECT                                    rectOffScreen;
        RECT                                    rectSurface;
        HWND                                    hwnd;
        DWORD                                   pitch;
        PBYTE                                   addrOffScreen;
                                       /* #ifdef PROFILE                   */
                                       /* MESAPROF        profile;         */
                                       /* #endif                           */
}  *PWMC;


#define PAGE_FILE               0xffffffff



#endif
