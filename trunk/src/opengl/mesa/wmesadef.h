/* $Id: wmesadef.h,v 1.1 2000-02-29 00:48:44 sandervl Exp $ */
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
 * Revision 1.1  2000-02-29 00:48:44  sandervl
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
 * Revision 1.1  2000-02-29 00:48:44  sandervl
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
 * Revision 1.1  2000-02-29 00:48:44  sandervl
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

#ifdef __WIN32OS2__
#include <os2win.h>
#else
#include <windows.h>
#endif

#if defined(DDRAW)
#include <ddraw.h>
#endif

#include "gl.h"
#include "context.h"
//#include "profile.h"

#define REDBITS         0x03
#define REDSHIFT        0x00
#define GREENBITS       0x03
#define GREENSHIFT      0x03
#define BLUEBITS        0x02
#define BLUESHIFT       0x06

typedef struct _dibSection{
        HDC             hDC;
        HANDLE  hFileMap;
        BOOL    fFlushed;
        LPVOID  base;
}WMDIBSECTION, *PWMDIBSECTION;


typedef struct wmesa_context{
    GLcontext *gl_ctx;          /* The core GL/Mesa context */
        GLvisual *gl_visual;            /* Describes the buffers */
    GLframebuffer *gl_buffer;   /* Depth, stencil, accum, etc buffers */


        HWND                            Window;
    HDC                 hDC;
    HPALETTE            hPalette;
    HPALETTE            hOldPalette;
    HPEN                hPen;
    HPEN                hOldPen;
    HCURSOR             hOldCursor;
    COLORREF            crColor;
    // 3D projection stuff
    RECT                drawRect;
    UINT                uiDIBoffset;
    // OpenGL stuff
    HPALETTE            hGLPalette;
        GLuint                          width;
        GLuint                          height;
        GLuint                          ScanWidth;
        GLboolean                       db_flag;        //* double buffered?
        GLboolean                       rgb_flag;       //* RGB mode?
        GLboolean                       dither_flag;    //* use dither when 256 color mode for RGB?
        GLuint                          depth;          //* bits per pixel (1, 8, 24, etc)
        ULONG                           pixel;  // current color index or RGBA pixel value
        ULONG                           clearpixel; //* pixel for clearing the color buffers
        PBYTE                           ScreenMem; // WinG memory
        BITMAPINFO                      *IndexFormat;
        HPALETTE                        hPal; // Current Palette
        HPALETTE                        hPalHalfTone;


        WMDIBSECTION            dib;
    BITMAPINFO          bmi;
    HBITMAP             hbmDIB;
    HBITMAP             hOldBitmap;
        HBITMAP                         Old_Compat_BM;
        HBITMAP                         Compat_BM;            // Bitmap for double buffering
    PBYTE               pbPixels;
    int                 nColors;
        BYTE                            cColorBits;
        int                                     pixelformat;

#ifdef DDRAW
        LPDIRECTDRAW            lpDD;           // DirectDraw object
//      LPDIRECTDRAW2            lpDD2;           // DirectDraw object
        LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
        LPDIRECTDRAWSURFACE     lpDDSOffScreen; // DirectDraw off screen surface
        LPDIRECTDRAWPALETTE     lpDDPal;        // DirectDraw palette
        BOOL                    bActive;        // is application active?
        DDSURFACEDESC           ddsd;
        int                                     fullScreen;
        int                                 gMode ;
#endif
        RECT                                    rectOffScreen;
        RECT                                    rectSurface;
        HWND                                    hwnd;
        DWORD                                   pitch;
        PBYTE                                   addrOffScreen;
//#ifdef PROFILE
//      MESAPROF        profile;
//#endif
}  *PWMC;


#define PAGE_FILE               0xffffffff



#endif
