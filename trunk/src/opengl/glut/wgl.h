/* $Id: wgl.h,v 1.2 2000-02-09 08:46:22 jeroen Exp $ */

typedef ULONG HGC;

BOOL APIENTRY wglDeleteContext(HGC hgc);
HDC  APIENTRY wglGetCurrentDC(void);
HGC  APIENTRY wglGetCurrentContext(void);
BOOL APIENTRY wglMakeCurrent(HDC hdc,HGC hgc);
HGC  APIENTRY wglCreateContext(HDC hdc);
