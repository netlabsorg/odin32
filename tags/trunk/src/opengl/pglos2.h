/* $Id: pglos2.h,v 1.2 2000-02-09 08:54:25 jeroen Exp $ */
#ifndef _pglos2_h_
#define _pglos2_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "glos2.h"

#pragma pack(1)
typedef struct tagVISUALCONFIG {
    unsigned long  vid;                /* Visual ID                        */
    BOOL   rgba;
    int redSize, greenSize, blueSize, alphaSize;
    ULONG redMask, greenMask, blueMask;
    int accumRedSize, accumGreenSize, accumBlueSize, accumAlphaSize;
    BOOL doubleBuffer;
    BOOL  stereo;
    int bufferSize;
    int depthSize;
    int stencilSize;
    int auxBuffers;
    int level;
    PVOID reserved;
    struct visualconfig *next;
    } VISUALCONFIG, *PVISUALCONFIG;
#pragma pack()

typedef LHANDLE HGC;

extern PVISUALCONFIG APIENTRY OS2pglChooseConfig(HAB hab, int *attriblist);
extern PVISUALCONFIG* APIENTRY OS2pglQueryConfigs(HAB hab);
extern HGC  APIENTRY OS2pglCreateContext(HAB hab, PVISUALCONFIG pVisualConfig, HGC Sharelist, BOOL IsDirect);
extern BOOL APIENTRY OS2pglMakeCurrent(HAB hab,HGC hgc, HWND hwnd);
extern BOOL APIENTRY OS2pglDestroyContext(HAB hab, HGC hgc);
extern BOOL APIENTRY OS2pglCopyContext(HAB hab, HGC hgc_src, HGC hgc_dst, GLuint attrib_mask);
extern int  APIENTRY OS2pglSelectColorIndexPalette(HAB hab, HPAL hpal, HGC hgc);
extern LONG APIENTRY OS2pglIsIndirect(HAB hab, HGC hgc);
extern HGC  APIENTRY OS2pglGetCurrentContext(HAB hab);
extern HWND APIENTRY OS2pglGetCurrentWindow(HAB hab);
extern HPS  APIENTRY OS2pglWaitGL(HAB hab);
extern void APIENTRY OS2pglWaitPM(HAB hab);
extern void APIENTRY OS2pglSwapBuffers(HAB hab, HWND hwnd);
extern LONG APIENTRY OS2pglQueryCapability(HAB hab);
extern void APIENTRY OS2pglQueryVersion(HAB hab, int *major, int *minor);
extern BOOL APIENTRY OS2pglUsePMBitmapFont(HAB hab, LONG id, int first, int count, int listbase);
extern BOOL APIENTRY OS2pglGrabFrontBitmap(HAB hab,HPS *phps,HBITMAP *phbitmap);
extern BOOL APIENTRY OS2pglReleaseFrontBitmap(HAB hab);
extern BOOL APIENTRY OS2pglUseFont(HAB,HPS,FATTRS *,long,int,int,int);

#define  None                   0x0

#define  PGL_USE_GL             1
#define  PGL_BUFFER_SIZE        2
#define  PGL_LEVEL              3
#define  PGL_RGBA               4
#define  PGL_DOUBLEBUFFER       5
#define  PGL_STEREO             6
#define  PGL_AUX_BUFFERS        7
#define  PGL_RED_SIZE           8
#define  PGL_GREEN_SIZE         9
#define  PGL_BLUE_SIZE          10
#define  PGL_ALPHA_SIZE         11
#define  PGL_DEPTH_SIZE         12
#define  PGL_STENCIL_SIZE       13
#define  PGL_ACCUM_RED_SIZE     14
#define  PGL_ACCUM_GREEN_SIZE   15
#define  PGL_ACCUM_BLUE_SIZE    16
#define  PGL_ACCUM_ALPHA_SIZE   17
#define  PGL_SINGLEBUFFER       18

#ifdef __cplusplus
}
#endif

#endif  /* _pglos2_h_ */
