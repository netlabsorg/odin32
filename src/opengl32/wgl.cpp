/*
 * opengl32 - WGL layer for Odin32 on top of Mesa 7.11 (MesaGL7.dll, OSMesa).
 *
 * Every GL context is an OSMesa context rendering into a 32 bpp BGRA buffer
 * that is sized after the window client area.  wglSwapBuffers() copies the
 * buffer to the window with StretchDIBits.  The gl* entry points are generated
 * stdcall forwarders (glwrap.c) to the cdecl exports of MesaGL7.dll.
 *
 * Odin32 compatibility DLL; see PLAN notes in the Java 21 / Sweet Home 3D work.
 */
#include <os2win.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "glwrap.h"

extern "C" unsigned long glw_loadmesa(char *failname, unsigned long cb, unsigned long *hmod);
extern "C" void *glw_proc(unsigned long hmod, const char *name);

static void glog(const char *fmt, ...)
{
    static const char *path = (const char *)-1;
    if(path == (const char *)-1) path = getenv("OGL32_LOG");
    if(!path) return;
    FILE *f = fopen(path, "a");
    if(!f) return;
    va_list ap; va_start(ap, fmt); vfprintf(f, fmt, ap); va_end(ap);
    fputc(10, f); fclose(f);
}

#define GL_UNSIGNED_BYTE   0x1401
#define OSMESA_BGRA        0x1
#define OSMESA_Y_UP        0x11
#define OSMESA_ROW_LENGTH  0x10

typedef void *OSMESAcontext;
typedef OSMESAcontext (*PFN_OSMesaCreateContextExt)(unsigned int format, int depth, int stencil, int accum, OSMESAcontext share);
typedef void          (*PFN_OSMesaDestroyContext)(OSMESAcontext ctx);
typedef unsigned char (*PFN_OSMesaMakeCurrent)(OSMESAcontext ctx, void *buffer, unsigned int type, int w, int h);
typedef void          (*PFN_OSMesaPixelStore)(int pname, int value);

static ULONG hMesa;
static PFN_OSMesaCreateContextExt pCreate;
static PFN_OSMesaDestroyContext   pDestroy;
static PFN_OSMesaMakeCurrent      pMakeCurrent;
static PFN_OSMesaPixelStore       pPixelStore;
static CRITICAL_SECTION           csWgl;
static int                        fInit;
static DWORD                      tlsIndex = TLS_OUT_OF_INDEXES;

struct GLRC
{
    OSMESAcontext osctx;
    HDC           hdc;        /* last HDC the context was made current on */
    BYTE         *buf;
    int           w, h;
    struct GLRC  *next;
};
static GLRC *ctxList;

static int  curPixelFormat = 0;
#define NUM_PIXELFORMATS 2

static int glwInit(void)
{
    if(fInit) return fInit > 0;
    InitializeCriticalSection(&csWgl);
    EnterCriticalSection(&csWgl);
    if(!fInit)
    {
        fInit = -1;
        char   szFail[260]; szFail[0] = 0;
        unsigned long rc = glw_loadmesa(szFail, sizeof(szFail), &hMesa);
        glog("glwInit: load MesaGL7 rc=%u failing='%s'", (unsigned)rc, szFail);
        if(rc == 0)
        {
            pCreate      = (PFN_OSMesaCreateContextExt)glw_proc(hMesa, "OSMesaCreateContextExt");
            pDestroy     = (PFN_OSMesaDestroyContext)  glw_proc(hMesa, "OSMesaDestroyContext");
            pMakeCurrent = (PFN_OSMesaMakeCurrent)     glw_proc(hMesa, "OSMesaMakeCurrent");
            pPixelStore  = (PFN_OSMesaPixelStore)      glw_proc(hMesa, "OSMesaPixelStore");
            glog("glwInit: create %p destroy %p makecur %p pixelstore %p", pCreate, pDestroy, pMakeCurrent, pPixelStore);
            if(pCreate && pDestroy && pMakeCurrent)
            {
                for(int i = 0; i < glw_count; i++)
                {
                    glw_fn[i] = glw_proc(hMesa, glw_names[i]);
                }
                tlsIndex = TlsAlloc();
                fInit = 1;
            }
        }
    }
    LeaveCriticalSection(&csWgl);
    return fInit > 0;
}

/* per-thread current context */
static GLRC *getCurrent(void)
{
    return (tlsIndex == TLS_OUT_OF_INDEXES) ? NULL : (GLRC *)TlsGetValue(tlsIndex);
}
static void setCurrent(GLRC *c)
{
    if(tlsIndex != TLS_OUT_OF_INDEXES) TlsSetValue(tlsIndex, c);
}

/* size of the drawable behind a DC */
static void getDrawableSize(HDC hdc, int *w, int *h)
{
    RECT rc;
    HWND hwnd = WindowFromDC(hdc);
    *w = *h = 0;
    if(hwnd && GetClientRect(hwnd, &rc))
    {
        *w = rc.right - rc.left;
        *h = rc.bottom - rc.top;
    }
    if(*w <= 0) *w = 64;
    if(*h <= 0) *h = 64;
}

static BOOL attachBuffer(GLRC *c, HDC hdc)
{
    int w, h;
    getDrawableSize(hdc, &w, &h);
    if(!c->buf || w != c->w || h != c->h)
    {
        BYTE *nb = (BYTE *)calloc((size_t)w * h, 4);
        if(!nb) return FALSE;
        if(c->buf) free(c->buf);
        c->buf = nb; c->w = w; c->h = h;
    }
    /* OSMESA_Y_UP is the default (bottom row first), which matches a bottom-up DIB */
    return pMakeCurrent(c->osctx, c->buf, GL_UNSIGNED_BYTE, c->w, c->h) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------
 * pixel formats: 1 = double buffered, 2 = single buffered; both 32 bpp RGBA,
 * 24 bit depth, 8 bit stencil.
 *--------------------------------------------------------------------------*/
static void fillPfd(int idx, PIXELFORMATDESCRIPTOR *p)
{
    memset(p, 0, sizeof(*p));
    p->nSize        = sizeof(*p);
    p->nVersion     = 1;
    p->dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_FORMAT |
                      (idx == 1 ? (PFD_DOUBLEBUFFER | PFD_SWAP_COPY) : 0);
    p->iPixelType   = PFD_TYPE_RGBA;
    p->cColorBits   = 32;
    p->cRedBits     = 8; p->cRedShift   = 16;
    p->cGreenBits   = 8; p->cGreenShift = 8;
    p->cBlueBits    = 8; p->cBlueShift  = 0;
    p->cAlphaBits   = 8; p->cAlphaShift = 24;
    p->cDepthBits   = 24;
    p->cStencilBits = 8;
    p->iLayerType   = PFD_MAIN_PLANE;
}

extern "C" {

int WIN32API wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd)
{
    glog("wglChoosePixelFormat");
    if(ppfd && !(ppfd->dwFlags & PFD_DOUBLEBUFFER) && (ppfd->dwFlags & PFD_DOUBLEBUFFER_DONTCARE) == 0
       && !(ppfd->dwFlags & PFD_DRAW_TO_BITMAP))
        return 2;
    return 1;
}

int WIN32API wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR *ppfd)
{
    if(!ppfd) return NUM_PIXELFORMATS;
    if(iPixelFormat < 1 || iPixelFormat > NUM_PIXELFORMATS || nBytes < sizeof(PIXELFORMATDESCRIPTOR))
        return 0;
    fillPfd(iPixelFormat, ppfd);
    return NUM_PIXELFORMATS;
}

BOOL WIN32API wglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR *ppfd)
{
    if(iPixelFormat < 1 || iPixelFormat > NUM_PIXELFORMATS) return FALSE;
    curPixelFormat = iPixelFormat;
    return TRUE;
}

int WIN32API wglGetPixelFormat(HDC hdc)
{
    return curPixelFormat;
}

HGLRC WIN32API wglCreateContext(HDC hdc)
{
    glog("wglCreateContext called");
    if(!glwInit()) { glog("  glwInit failed"); SetLastError(ERROR_INVALID_FUNCTION); return NULL; }
    GLRC *c = (GLRC *)calloc(1, sizeof(GLRC));
    if(!c) return NULL;
    c->osctx = pCreate(OSMESA_BGRA, 24, 8, 0, NULL);
    glog("wglCreateContext hdc=%p -> osctx %p", hdc, c->osctx);
    if(!c->osctx) { free(c); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    EnterCriticalSection(&csWgl);
    c->next = ctxList; ctxList = c;
    LeaveCriticalSection(&csWgl);
    return (HGLRC)c;
}

BOOL WIN32API wglDeleteContext(HGLRC hglrc)
{
    GLRC *c = (GLRC *)hglrc, **pp;
    if(!c) return FALSE;
    if(getCurrent() == c) { pMakeCurrent(NULL, NULL, 0, 0, 0); setCurrent(NULL); }
    EnterCriticalSection(&csWgl);
    for(pp = &ctxList; *pp; pp = &(*pp)->next)
        if(*pp == c) { *pp = c->next; break; }
    LeaveCriticalSection(&csWgl);
    pDestroy(c->osctx);
    if(c->buf) free(c->buf);
    free(c);
    return TRUE;
}

BOOL WIN32API wglMakeCurrent(HDC hdc, HGLRC hglrc)
{
    GLRC *c = (GLRC *)hglrc;
    glog("wglMakeCurrent hdc=%p ctx=%p", hdc, hglrc);
    if(!glwInit()) return FALSE;
    if(!c)
    {
        pMakeCurrent(NULL, NULL, 0, 0, 0);
        setCurrent(NULL);
        return TRUE;
    }
    c->hdc = hdc;
    if(!attachBuffer(c, hdc)) { glog("  attachBuffer failed (%dx%d)", c->w, c->h); return FALSE; }
    glog("  made current %dx%d", c->w, c->h);
    setCurrent(c);
    return TRUE;
}

HGLRC WIN32API wglGetCurrentContext(void)
{
    return (HGLRC)getCurrent();
}

HDC WIN32API wglGetCurrentDC(void)
{
    GLRC *c = getCurrent();
    return c ? c->hdc : NULL;
}

PROC WIN32API wglGetProcAddress(LPCSTR name)
{
    if(!name || !glwInit()) return NULL;
    /* the extension/GL functions are all exported by this DLL */
    HMODULE self = GetModuleHandleA("OPENGL32.DLL");
    if(!self) self = GetModuleHandleA("opengl32");
    if(!self) return NULL;
    if(strncmp(name, "gl", 2) != 0) return NULL;
    return (PROC)GetProcAddress(self, name);
}

BOOL WIN32API wglShareLists(HGLRC a, HGLRC b)
{
    return FALSE;   /* contexts are created without sharing; callers fall back */
}

BOOL WIN32API wglCopyContext(HGLRC a, HGLRC b, UINT mask)
{
    return FALSE;
}

BOOL WIN32API wglUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listBase)
{
    return FALSE;
}

BOOL WIN32API wglUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD listBase)
{
    return FALSE;
}

BOOL WIN32API wglSwapLayerBuffers(HDC hdc, UINT planes)
{
    extern BOOL WIN32API wglSwapBuffers(HDC);
    return wglSwapBuffers(hdc);
}

BOOL WIN32API wglSwapBuffers(HDC hdc)
{
    GLRC *c = getCurrent();
    glog("wglSwapBuffers hdc=%p cur=%p buf=%p", hdc, c, c ? c->buf : 0);
    if(!c || !c->buf) return FALSE;
    /* the glFlush of the software renderer is implicit: everything is in c->buf */
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = c->w;
    bmi.bmiHeader.biHeight      = c->h;          /* bottom-up == OSMesa Y_UP */
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, 0, 0, c->w, c->h, 0, 0, c->w, c->h, c->buf, &bmi, DIB_RGB_COLORS, SRCCOPY);

    /* follow window resizes: reallocate the buffer for the next frame */
    int w, h;
    getDrawableSize(hdc, &w, &h);
    if(w != c->w || h != c->h)
        attachBuffer(c, hdc);
    return TRUE;
}

} /* extern "C" */
