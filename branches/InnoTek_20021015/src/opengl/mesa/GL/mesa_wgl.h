/* $Id: mesa_wgl.h,v 1.1 2000-02-29 00:43:32 sandervl Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.1
 *
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/* prototypes for the Mesa WGL functions */
/* relocated here so that I could make GLUT get them properly */

#ifndef _mesa_wgl_h_
#define _mesa_wgl_h_


#include <gl/gl.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _MSC_VER
#  pragma warning( disable : 4615 ) /* pragma warning : unknown user warning type*/
#  pragma warning( push )
#  pragma warning( disable : 4273 ) /* 'function' : inconsistent DLL linkage. dllexport assumed. */
#endif

WGLAPI int   GLAPIENTRY wglDeleteContext(HGLRC);
WGLAPI int   GLAPIENTRY wglMakeCurrent(HDC,HGLRC);
WGLAPI int   GLAPIENTRY wglSetPixelFormat(HDC, int, const PIXELFORMATDESCRIPTOR *);
WGLAPI int   GLAPIENTRY wglSwapBuffers(HDC hdc);
WGLAPI HDC   GLAPIENTRY wglGetCurrentDC(void);
WGLAPI HGLRC GLAPIENTRY wglCreateContext(HDC);
WGLAPI HGLRC GLAPIENTRY wglCreateLayerContext(HDC,int);
WGLAPI HGLRC GLAPIENTRY wglGetCurrentContext(void);
WGLAPI PROC  GLAPIENTRY wglGetProcAddress(const char*);
WGLAPI int   GLAPIENTRY wglChoosePixelFormat(HDC, const PIXELFORMATDESCRIPTOR *);
WGLAPI int   GLAPIENTRY wglCopyContext(HGLRC, HGLRC, unsigned int);
WGLAPI int   GLAPIENTRY wglDeleteContext(HGLRC);
WGLAPI int   GLAPIENTRY wglDescribeLayerPlane(HDC, int, int, unsigned int,LPLAYERPLANEDESCRIPTOR);
WGLAPI int   GLAPIENTRY wglDescribePixelFormat(HDC,int, unsigned int, LPPIXELFORMATDESCRIPTOR);
WGLAPI int   GLAPIENTRY wglGetLayerPaletteEntries(HDC, int, int, int,COLORREF *);
WGLAPI int   GLAPIENTRY wglGetPixelFormat(HDC hdc);
WGLAPI int   GLAPIENTRY wglMakeCurrent(HDC, HGLRC);
WGLAPI int   GLAPIENTRY wglRealizeLayerPalette(HDC, int, int);
WGLAPI int   GLAPIENTRY wglSetLayerPaletteEntries(HDC, int, int, int,const COLORREF *);
WGLAPI int   GLAPIENTRY wglShareLists(HGLRC, HGLRC);
WGLAPI int   GLAPIENTRY wglSwapLayerBuffers(HDC, unsigned int);
WGLAPI int   GLAPIENTRY wglUseFontBitmapsA(HDC, unsigned long, unsigned long, unsigned long);
WGLAPI int   GLAPIENTRY wglUseFontBitmapsW(HDC, unsigned long, unsigned long, unsigned long);
WGLAPI int   GLAPIENTRY wglUseFontOutlinesA(HDC, unsigned long, unsigned long, unsigned long, float,float, int, LPGLYPHMETRICSFLOAT);
WGLAPI int   GLAPIENTRY wglUseFontOutlinesW(HDC, unsigned long, unsigned long, unsigned long, float,float, int, LPGLYPHMETRICSFLOAT);
WGLAPI int   GLAPIENTRY SwapBuffers(HDC);
WGLAPI int   GLAPIENTRY ChoosePixelFormat(HDC,const PIXELFORMATDESCRIPTOR *);
WGLAPI int   GLAPIENTRY DescribePixelFormat(HDC,int,unsigned int,LPPIXELFORMATDESCRIPTOR);
WGLAPI int   GLAPIENTRY GetPixelFormat(HDC);
WGLAPI int   GLAPIENTRY SetPixelFormat(HDC,int,const PIXELFORMATDESCRIPTOR *);

#ifdef _MSC_VER
#  pragma warning( pop )
#endif

#ifdef __cplusplus
}
#endif


#endif /* _mesa_wgl_h_ */