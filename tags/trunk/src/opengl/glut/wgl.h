/* JvdH - define the wglXXX functions */

typedef ULONG HGC;

BOOL APIENTRY wglDeleteContext(HGC hgc);
HDC  APIENTRY wglGetCurrentDC(void);
HGC  APIENTRY wglGetCurrentContext(void);
BOOL APIENTRY wglMakeCurrent(HDC hdc,HGC hgc);
HGC  APIENTRY wglCreateContext(HDC hdc);
