/* $Id: opengl32.cpp,v 1.4 2000-02-09 08:50:54 jeroen Exp $ */
/*****************************************************************************/
/*                                                                           */
/* OpenGL32                                                                  */
/*                                                                           */
/*****************************************************************************/
#define INCL_BASE
#include <os2wrap.h>
#include <misc.h>
#include <malloc.h>
#include <memory.h>
#include <float.h>

#include "pglos2.h"
#include "initterm.h"

HWND    WIN32API WindowFromDC(HDC hdc);
HWND             Win32ToOS2Handle(HWND hwnd);
HWND             OS2ToWin32Handle(HWND hwnd);
LRESULT WIN32API SendMessageA(HWND,ULONG,WPARAM,LPARAM);

#define PFD_TYPE_RGBA       0
#define PFD_TYPE_COLORINDEX 1

#define PFD_MAIN_PLANE      0
#define PFD_OVERLAY_PLANE   1
#define PFD_UNDERLAY_PLANE  (-1)

#define PFD_DOUBLEBUFFER          0x00000001
#define PFD_STEREO                0x00000002
#define PFD_DRAW_TO_WINDOW        0x00000004
#define PFD_DRAW_TO_BITMAP        0x00000008
#define PFD_SUPPORT_GDI           0x00000010
#define PFD_SUPPORT_OPENGL        0x00000020
#define PFD_GENERIC_FORMAT        0x00000040
#define PFD_NEED_PALETTE          0x00000080
#define PFD_NEED_SYSTEM_PALETTE   0x00000100
#define PFD_SWAP_EXCHANGE         0x00000200
#define PFD_SWAP_COPY             0x00000400
#define PFD_SWAP_LAYER_BUFFERS    0x00000800
#define PFD_GENERIC_ACCELERATED   0x00001000

#define PFD_DEPTH_DONTCARE        0x20000000
#define PFD_DOUBLEBUFFER_DONTCARE 0x40000000
#define PFD_STEREO_DONTCARE       0x80000000

typedef DWORD COLORREF;

#pragma pack(1)
typedef struct tagPIXELFORMATDESCRIPTOR {
    WORD  nSize;
    WORD  nVersion;
    DWORD dwFlags;
    BYTE  iPixelType;
    BYTE  cColorBits;
    BYTE  cRedBits;
    BYTE  cRedShift;
    BYTE  cGreenBits;
    BYTE  cGreenShift;
    BYTE  cBlueBits;
    BYTE  cBlueShift;
    BYTE  cAlphaBits;
    BYTE  cAlphaShift;
    BYTE  cAccumBits;
    BYTE  cAccumRedBits;
    BYTE  cAccumGreenBits;
    BYTE  cAccumBlueBits;
    BYTE  cAccumAlphaBits;
    BYTE  cDepthBits;
    BYTE  cStencilBits;
    BYTE  cAuxBuffers;
    BYTE  iLayerType;
    BYTE  bReserved;
    DWORD dwLayerMask;
    DWORD dwVisibleMask;
    DWORD dwDamageMask;
} PIXELFORMATDESCRIPTOR, *LPPIXELFORMATDESCRIPTOR;

typedef struct tagLAYERPLANEDESCRIPTOR {
    WORD     nSize;
    WORD     nVersion;
    DWORD    dwFlags;
    BYTE     iPixelType;
    BYTE     cColorBits;
    BYTE     cRedBits;
    BYTE     cRedShifts;
    BYTE     cGreenBits;
    BYTE     cGreenShifts;
    BYTE     cBlueBits;
    BYTE     cBlueShifts;
    BYTE     cAlphaBits;
    BYTE     cAlphaShifts;
    BYTE     cAccumBits;
    BYTE     cAccumRedBits;
    BYTE     cAccumGreenBits;
    BYTE     cAccumBlueBits;
    BYTE     cAccumAlphaBits;
    BYTE     cDepthBits;
    BYTE     cStencilBits;
    BYTE     cAuxBuffers;
    BYTE     iLayerPlane;
    BYTE     bReserved;
    COLORREF crTransparent;
} LAYERPLANEDESCRIPTOR;

/* This structure is set by ChoosePixelConfig - it ties together the HDC, */
/* PIXELFORMATDESCRIPTOR etc */
typedef struct tagPFDINFO
  {
    HWND                   hwnd;
    HDC                    hdc;
    HGC                    hgc;
    PIXELFORMATDESCRIPTOR  pfd;
    int                    iFormatSelected;
    struct tagPFDINFO     *Next;
  }PFDINFO;
#pragma pack()

/*****************************************************************************/
/*                                                                           */
/* Global variabled used in this DLL                                         */
/*                                                                           */
/*****************************************************************************/

PFDINFO       *pfdis                     = NULL;
PVISUALCONFIG *global_visual_config_list = NULL;
int            global_visual_config_count= 0;
HMTX           hmtxPfdInfo;

/*****************************************************************************/
/*                                                                           */
/* DEBUG stuff - dump structures to the log...                               */
/*                                                                           */
/*****************************************************************************/

#ifdef DEBUG
void PixFmtDump(PIXELFORMATDESCRIPTOR *p)
{
  dprintf(("*** PIXELFORMATDESCRIPTOR DUMP ***\n"));
  dprintf(("PIXFMT.nSize: %d\n",p->nSize));
  dprintf(("PIXFMT.nVersion: %d\n",p->nVersion));
  dprintf(("PIXFMT.dwFlags: %08X\n",p->dwFlags));
  dprintf(("PIXFMT.iPixelType: %d\n",p->iPixelType));
  dprintf(("PIXFMT.cColorBits: %d\n",p->cColorBits));
  dprintf(("PIXFMT.cRedBits: %d\n",p->cRedBits));
  dprintf(("PIXFMT.cRedShift: %d\n",p->cRedShift));
  dprintf(("PIXFMT.cGreenBits: %d\n",p->cGreenBits));
  dprintf(("PIXFMT.cGreenShift: %d\n",p->cGreenShift));
  dprintf(("PIXFMT.cBlueBits: %d\n",p->cBlueBits));
  dprintf(("PIXFMT.cBlueShift: %d\n",p->cBlueShift));
  dprintf(("PIXFMT.cAlphaBits: %d\n",p->cAlphaBits));
  dprintf(("PIXFMT.cAlphaShift: %d\n",p->cAlphaShift));
  dprintf(("PIXFMT.cAccumBits: %d\n",p->cAccumBits));
  dprintf(("PIXFMT.cAccumRedBits: %d\n",p->cAccumRedBits));
  dprintf(("PIXFMT.cAccumGreenBits: %d\n",p->cAccumGreenBits));
  dprintf(("PIXFMT.cAccumAlphaBits: %d\n",p->cAccumAlphaBits));
  dprintf(("PIXFMT.cAccumBlueBits: %d\n",p->cAccumBlueBits));
  dprintf(("PIXFMT.cDepthBits: %d\n",p->cDepthBits));
  dprintf(("PIXFMT.cStencilBits: %d\n",p->cStencilBits));
  dprintf(("PIXFMT.cAuxBuffers: %d\n",p->cAuxBuffers));
  dprintf(("PIXFMT.iLayerType: %d\n",p->iLayerType));
  dprintf(("PIXFMT.dwLayerMask: %d\n",p->dwLayerMask));
  dprintf(("PIXFMT.dwVisibleMask: %d\n",p->dwVisibleMask));
  dprintf(("PIXFMT.dwDamageMask: %d\n",p->dwDamageMask));
}

void VisCfgDump(VISUALCONFIG *p)
{
  dprintf(("*** VISUALCONFIG DUMP ***\n"));
  dprintf(("VISCFG.vid: %d\n",p->vid));
  dprintf(("VISCFG.rgba: %d\n",p->rgba));
  dprintf(("VISCFG.redSize: %d\n",p->redSize));
  dprintf(("VISCFG.greenSize: %d\n",p->greenSize));
  dprintf(("VISCFG.blueSize: %d\n",p->blueSize));
  dprintf(("VISCFG.alphaSize: %d\n",p->alphaSize));
  dprintf(("VISCFG.redMask: %08X\n",p->redMask));
  dprintf(("VISCFG.greenMask: %08X\n",p->greenMask));
  dprintf(("VISCFG.blueMask: %08X\n",p->blueMask));
  dprintf(("VISCFG.accumRedSize: %d\n",p->accumRedSize));
  dprintf(("VISCFG.accumGreenSize: %d\n",p->accumGreenSize));
  dprintf(("VISCFG.accumBlueSize: %d\n",p->accumBlueSize));
  dprintf(("VISCFG.accumAlphaSize: %d\n",p->accumAlphaSize));
  dprintf(("VISCFG.doubleBuffer: %d\n",p->doubleBuffer));
  dprintf(("VISCFG.stereo: %d\n",p->stereo));
  dprintf(("VISCFG.bufferSize: %d\n",p->bufferSize));
  dprintf(("VISCFG.depthSize: %d\n",p->depthSize));
  dprintf(("VISCFG.stencilSize: %d\n",p->stencilSize));
  dprintf(("VISCFG.auxBuffers: %d\n",p->auxBuffers));
  dprintf(("VISCFG.level: %d\n",p->level));
}
#endif

/*****************************************************************************/
/*                                                                           */
/* Internal code to maintain the pdfs structure list                         */
/*                                                                           */
/*****************************************************************************/

void pfdi_destroy_all(void)
{
  /* Destroy all allocated structures - called by DLL_term */
  PFDINFO *pi=pfdis;

  DosRequestMutexSem(hmtxPfdInfo,SEM_INDEFINITE_WAIT);

  while(pi)
    {
      pfdis=pi->Next;

      free(pi);

      pi=pfdis;
    }

  DosReleaseMutexSem(hmtxPfdInfo);
}

void pfdi_update_or_create(HDC hdc,PIXELFORMATDESCRIPTOR *pfd)
{
  /* ChoosePixelFormat returns an index to the most suitable PFD   */
  /* In OS/2 GL a linked-list of VISUALCONFIGS is returned         */
  /* Link the request to the callee so that we can use it later on */
  /* when SetPixelFormat and CreateContext are called.             */
  PFDINFO *pi=pfdis;
  HWND     hwnd=WindowFromDC(hdc);

  DosRequestMutexSem(hmtxPfdInfo,SEM_INDEFINITE_WAIT);

  while(pi)
    {
      if(pi->hwnd==hwnd)
        {
          pi->hdc=hdc;

          memcpy(&pi->pfd,pfd,sizeof(PIXELFORMATDESCRIPTOR));

          DosReleaseMutexSem(hmtxPfdInfo);

          return;
        }

      pi=pi->Next;
    }

  pi=(PFDINFO *)calloc(sizeof(PFDINFO),1);

  pi->hwnd=hwnd;
  pi->hdc=hdc;

  memcpy(&pi->pfd,pfd,sizeof(PIXELFORMATDESCRIPTOR));

  pi->Next=pfdis;

  pfdis=pi;

  DosReleaseMutexSem(hmtxPfdInfo);
}

PFDINFO *query_pfdi_by_hdc(HDC hdc)
{
  /* Query based upon the HDC - internally map this to the hwnd belonging  */
  /* to the HDC, since GetDC returnes different HDC's for the same window! */
  PFDINFO *pi=pfdis;
  HWND     hwnd=WindowFromDC(hdc);

  DosRequestMutexSem(hmtxPfdInfo,SEM_INDEFINITE_WAIT);

  while(pi)
    {
      if(pi->hwnd==hwnd)
        {
          DosReleaseMutexSem(hmtxPfdInfo);

          return pi;
        }

      pi=pi->Next;
    }

  DosReleaseMutexSem(hmtxPfdInfo);

  return NULL;
}

PFDINFO *query_pfdi_by_hwnd(HWND hwnd)
{
  /* Query an PFDINFO struct member based upon a hwnd */
  PFDINFO *pi=pfdis;

  DosRequestMutexSem(hmtxPfdInfo,SEM_INDEFINITE_WAIT);

  while(pi)
    {
      if(pi->hwnd==hwnd)
        {
          DosReleaseMutexSem(hmtxPfdInfo);

          return pi;
        }

      pi=pi->Next;
    }

  DosReleaseMutexSem(hmtxPfdInfo);

  return NULL;
}

PFDINFO *query_pfdi_by_hgc(HGC hgc)
{
  /* Query an PFDINFO struct member based upon an hgc */
  PFDINFO *pi=pfdis;

  DosRequestMutexSem(hmtxPfdInfo,SEM_INDEFINITE_WAIT);

  while(pi)
    {
      if(pi->hgc==hgc)
        {
          DosReleaseMutexSem(hmtxPfdInfo);

          return pi;
        }

      pi=pi->Next;
    }

  DosReleaseMutexSem(hmtxPfdInfo);

  return NULL;
}

void pfdi_purge(HDC hdc)
{
  /* Remove the PFDINFO for this hdc from the linked list */
  PFDINFO *pi,*pi2=NULL;
  HWND     hwnd=WindowFromDC(hdc);

  DosRequestMutexSem(hmtxPfdInfo,SEM_INDEFINITE_WAIT);

  pi=pfdis;

  while(pi)
    {
      if(pi->hwnd==hwnd)
        {
          if(pi2)
            pi2->Next=pi->Next;
          else
            pfdis=pi->Next;

          free(pi);

          DosReleaseMutexSem(hmtxPfdInfo);

          return;
        }

      pi2=pi; pi=pi->Next;
    }

  DosReleaseMutexSem(hmtxPfdInfo);
}

/***************************************************************************/
/*                                                                         */
/* Internal Support Functions                                              */
/*                                                                         */
/***************************************************************************/

void AttrAdd(int *attrs,int *attridx,int value)
{
  attrs[(*attridx)++]=value;
}

PVISUALCONFIG GL_QueryConfigs(HDC hdc,PIXELFORMATDESCRIPTOR *pfd)
{
  int            attrs[40],attridx=0;
  PVISUALCONFIG  pvc;
  PFDINFO       *pfdi;

  pfdi=query_pfdi_by_hdc(hdc);

  if(pfd->dwFlags & PFD_SUPPORT_OPENGL)
    {
      AttrAdd(attrs,&attridx,PGL_USE_GL);
    }

  if(pfd->dwFlags & PFD_DOUBLEBUFFER && !(pfd->dwFlags & PFD_DOUBLEBUFFER_DONTCARE))
    {
      AttrAdd(attrs,&attridx,PGL_DOUBLEBUFFER);
    }

  if(pfd->dwFlags & PFD_STEREO && !(pfd->dwFlags & PFD_STEREO_DONTCARE))
    {
      AttrAdd(attrs,&attridx,PGL_STEREO);
    }

  switch(pfd->iPixelType)
    {
      case PFD_TYPE_RGBA:
        AttrAdd(attrs,&attridx,PGL_RGBA);
        break;

      case PFD_TYPE_COLORINDEX:
        break;
    }

  switch(pfd->iLayerType)
    {
      case PFD_MAIN_PLANE:
        AttrAdd(attrs,&attridx,PGL_LEVEL);
        AttrAdd(attrs,&attridx,0);
        break;

      case PFD_OVERLAY_PLANE:
        AttrAdd(attrs,&attridx,PGL_LEVEL);
        AttrAdd(attrs,&attridx,1);
        break;

      case PFD_UNDERLAY_PLANE:
        AttrAdd(attrs,&attridx,PGL_LEVEL);
        AttrAdd(attrs,&attridx,-1);
        break;
    }

  if(pfd->cDepthBits)
    {
      AttrAdd(attrs,&attridx,PGL_DEPTH_SIZE);

      /* Added JvdH - is my machine bogus? cDepthBits 32 returns no */
      /* valid configs. Is this Matrox specific??                   */

      if(pfd->cDepthBits<=24)
        AttrAdd(attrs,&attridx,pfd->cDepthBits);
      else
        AttrAdd(attrs,&attridx,24);
    }

  if(pfd->cColorBits)
    {
      AttrAdd(attrs,&attridx,PGL_BUFFER_SIZE);
      AttrAdd(attrs,&attridx,pfd->cColorBits);
    }

  if(pfd->cAuxBuffers)
    {
      AttrAdd(attrs,&attridx,PGL_AUX_BUFFERS);
      AttrAdd(attrs,&attridx,pfd->cAuxBuffers);
    }

  if(pfd->cRedBits)
    {
      AttrAdd(attrs,&attridx,PGL_RED_SIZE);
      AttrAdd(attrs,&attridx,pfd->cRedBits);
    }

  if(pfd->cGreenBits)
    {
      AttrAdd(attrs,&attridx,PGL_GREEN_SIZE);
      AttrAdd(attrs,&attridx,pfd->cGreenBits);
    }

  if(pfd->cBlueBits)
    {
      AttrAdd(attrs,&attridx,PGL_BLUE_SIZE);
      AttrAdd(attrs,&attridx,pfd->cBlueBits);
    }

  if(pfd->cAlphaBits)
    {
      AttrAdd(attrs,&attridx,PGL_ALPHA_SIZE);
      AttrAdd(attrs,&attridx,pfd->cAlphaBits);
    }

  if(pfd->cStencilBits)
    {
      AttrAdd(attrs,&attridx,PGL_STENCIL_SIZE);
      AttrAdd(attrs,&attridx,pfd->cStencilBits);
    }

  AttrAdd(attrs,&attridx,None);

  pvc=OS2pglChooseConfig(hdc,attrs);

  return pvc;
}

int GL_VisualConfigIndex(PVISUALCONFIG pvc)
{
  int i;

  for(i=0; i<global_visual_config_count; i++)
    if(memcmp(global_visual_config_list[i],pvc,sizeof(VISUALCONFIG))==0)
      return i+1;

  dprintf(("OPENGL32 (ERROR): Could not map the VisualConfig %08X\n",pvc));

  return 0;
}

void GL_VisualConfig_to_PixelFormat(HDC hdc,
                                    VISUALCONFIG *pvc,
                                    PIXELFORMATDESCRIPTOR *pfd)
{
  /* Map a VisualConfig structure to an identical PixelFmt structure    */
  /* Win32 uses PIXELFORMATDESCRIPTORs, whereas OS/2 uses VISUALCONFIGs */
  PFDINFO *pfdi;
  BOOL     fDirect=FALSE;

  pfdi=query_pfdi_by_hdc(hdc);

  if(pfdi)
    fDirect=(pfdi->pfd.dwFlags & PFD_DRAW_TO_WINDOW)?TRUE:FALSE;

  memset(pfd,0,sizeof(PIXELFORMATDESCRIPTOR));

  pfd->nSize=sizeof(PIXELFORMATDESCRIPTOR);
  pfd->nVersion=1;

  pfd->dwFlags=PFD_SUPPORT_OPENGL | (fDirect?PFD_DRAW_TO_WINDOW:0);

  if(pvc->stereo)
    pfd->dwFlags+=PFD_STEREO;

  if(pvc->doubleBuffer)
    pfd->dwFlags+=PFD_DOUBLEBUFFER;

  if(pvc->rgba)
    pfd->iPixelType=PFD_TYPE_RGBA;
  else
    pfd->iPixelType=PFD_TYPE_COLORINDEX;

  pfd->cColorBits=pvc->bufferSize;
  pfd->cRedBits=pvc->redSize;
  pfd->cRedShift=0;
  pfd->cGreenBits=pvc->greenSize;
  pfd->cGreenShift=0;
  pfd->cBlueBits=pvc->blueSize;
  pfd->cBlueShift=0;
  pfd->cAlphaBits=pvc->alphaSize;
  pfd->cAlphaShift=0;
  pfd->cAccumBits=0;
  pfd->cAccumRedBits=pvc->accumRedSize;
  pfd->cAccumGreenBits=pvc->accumGreenSize;
  pfd->cAccumBlueBits=pvc->accumBlueSize;
  pfd->cAccumAlphaBits=pvc->accumAlphaSize;
  pfd->cDepthBits=pvc->depthSize;
  pfd->cStencilBits=pvc->stencilSize;
  pfd->cAuxBuffers=pvc->auxBuffers;

  if(pvc->level<0)
    pfd->iLayerType=PFD_UNDERLAY_PLANE;
  else
    if(pvc->level>0)
      pfd->iLayerType=PFD_OVERLAY_PLANE;
    else
      pfd->iLayerType=PFD_MAIN_PLANE;

  pfd->bReserved=0;
  pfd->dwLayerMask=0;
  pfd->dwVisibleMask=0;
  pfd->dwDamageMask=0;
}

/***************************************************************************/
/*                                                                         */
/* API Entry Points - Map calls to OPENGL.DLL                              */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* wgl->pgl Conversion functions                                           */
/*                                                                         */
/***************************************************************************/

BOOL WIN32API wglSwapBuffers(HDC hdc)
{
  HWND hWnd;

  hWnd=Win32ToOS2Handle(WindowFromDC(hdc));

  if(hWnd)
    OS2pglSwapBuffers(hdc,hWnd);

  return hWnd!=NULL;
}

int WIN32API wglChoosePixelFormat(HDC hdc,PIXELFORMATDESCRIPTOR *pfd)
{
  /* The PixelFormatDescriptor hold information about the required context */
  /* It is our task to return the 'most suitable one'. Translate the call  */
  /* to native OS/2 OpenGL and return a suitable value.                    */
  PVISUALCONFIG pvc;

#ifdef DEBUG
  /* DUMP the pfd */
  PixFmtDump(pfd);
#endif

  pfdi_update_or_create(hdc,pfd);

  pvc=GL_QueryConfigs(hdc,pfd);

  /* If no valid visualconfig is found there is really no need to keep the */
  /* PFDINFO structure! */
  if(!pvc)
    {
      pfdi_purge(hdc);
      return 0;
    }

  return GL_VisualConfigIndex(pvc);
}

GLint WIN32API wglGetPixelFormat(HDC hdc)
{
  PFDINFO *pfdi;

  pfdi=query_pfdi_by_hdc(hdc);

  if(pfdi)
    return pfdi->iFormatSelected;

  return 0;
}

BOOL WIN32API wglSetPixelFormat(HDC hdc,
                                GLint iFormat,
                                PIXELFORMATDESCRIPTOR *pfd)
{
  /* Set the pixelformat - the pixelformat was chosen by wglChoosePixelFormat */
  /* and stored in the PFDINFO linked list. We just check if the structure is */
  /* present in the list. If not, return FALSE!                               */
  PFDINFO *pfdi;

  pfdi=query_pfdi_by_hdc(hdc);

  if(pfdi)
    {
      pfdi->iFormatSelected=iFormat;

      return TRUE;
    }

  return FALSE;
}

HGC WIN32API wglCreateContext(HDC hdc)
{
  /* The PFDINFO structure must be present (by a prev call to ChooseConfig !) */
  /* If not chosen we don't know what we should return...                     */
  PFDINFO       *pfdi;
  HGC            hgc;
  HGC            sharelist=NULL;
  PVISUALCONFIG  pvc;

  pfdi=query_pfdi_by_hdc(hdc);

  if(pfdi)
    {
      if(pfdi->iFormatSelected>=1 && pfdi->iFormatSelected<=global_visual_config_count)
        {
          pvc=global_visual_config_list[pfdi->iFormatSelected-1];

          pfdi->hgc=OS2pglCreateContext(hdc,
                                        pvc,
                                        sharelist,
                                        (pfdi->pfd.dwFlags & PFD_DRAW_TO_WINDOW)?TRUE:FALSE);

          return pfdi->hgc;
        }
    }

  return NULL;
}

int WIN32API wglDescribePixelFormat(HDC                    hdc,
                                    GLint                  iFormat,
                                    GLuint                 nBytes,
                                    PIXELFORMATDESCRIPTOR *pfd)
{
  if(iFormat<1 || iFormat>global_visual_config_count)
    iFormat=1;                             /* Or should we return an error?*/

  if(pfd && nBytes)
    {
      PIXELFORMATDESCRIPTOR pfd_use;

      GL_VisualConfig_to_PixelFormat(hdc,global_visual_config_list[iFormat-1],&pfd_use);

#ifdef DEBUG
      VisCfgDump(global_visual_config_list[iFormat-1]);
      PixFmtDump(&pfd_use);
#endif

      memcpy(pfd,&pfd_use,nBytes);
    }

  return global_visual_config_count;
}

BOOL WIN32API wglDeleteContext(HGC hgc)
{
  PFDINFO *pfdi;

  pfdi=query_pfdi_by_hgc(hgc);

  if(pfdi)
    return OS2pglDestroyContext(pfdi->hdc,hgc);

  return FALSE;
}

BOOL WIN32API wglMakeCurrent(HDC hdc,HGC hgc)
{
  BOOL rc;
  HWND hWnd;

  hWnd=Win32ToOS2Handle(WindowFromDC(hdc));

  rc=OS2pglMakeCurrent(hdc,hgc,hWnd);

  return rc;
}

BOOL WIN32API wglCopyContext(HGC hgc_src,HGC hgc_dst,GLuint attrib_mask)
{
  PFDINFO *pfdi;

  pfdi=query_pfdi_by_hgc(hgc_src);

  if(pfdi)
    return OS2pglCopyContext(pfdi->hdc,hgc_src,hgc_dst,attrib_mask);

  return FALSE;
}

BOOL WIN32API wglUseFontBitmapsA(HDC   hdc,
                                 DWORD first,
                                 DWORD count,
                                 DWORD listbase)
{
  /* FIXME: how to map to correct font ?? */
  int id=0;

  return OS2pglUsePMBitmapFont(hdc,id,first,count,listbase);
}

BOOL WIN32API wglUseFontBitmapsW(HDC   hdc,
                                 DWORD first,
                                 DWORD count,
                                 DWORD listbase)
{
  /* FIXME: how to map to correct font ?? */
  int id=0;

  return OS2pglUsePMBitmapFont(hdc,id,first,count,listbase);
}

HGC WIN32API wglGetCurrentContext(void)
{
  HWND     hwnd;
  PFDINFO *pfdi;

  hwnd=OS2ToWin32Handle(OS2pglGetCurrentWindow(WinQueryAnchorBlock(HWND_DESKTOP)));

  pfdi=query_pfdi_by_hwnd(hwnd);

  if(pfdi)
    return pfdi->hgc;

  return 0;
}

HDC WIN32API wglGetCurrentDC(void)
{
  HWND     hwnd;
  PFDINFO *pfdi;

  hwnd=OS2ToWin32Handle(OS2pglGetCurrentWindow(WinQueryAnchorBlock(HWND_DESKTOP)));

  pfdi=query_pfdi_by_hwnd(hwnd);

  if(pfdi)
    return pfdi->hdc;

  return 0;
}

/****************************************************************************/
/*                                                                          */
/* As of yet unsupported wgl functions                                      */
/*                                                                          */
/****************************************************************************/

GLint WIN32API wglGetLayerPaletteEntries(HDC   hdc,
                                         GLint iLayerPlane,
                                         GLint iStart,
                                         GLint cEntries,
                                         const COLORREF *pcr)
{
  dprintf(("OPENGL32: wglGetLayerPaletteEntries - not implemented\n"));
  return 0;
}

BOOL WIN32API wglRealizeLayerPalette(HDC hdc,GLint iLayerPlane,BOOL bRealize)
{
  dprintf(("OPENGL32: wglRealizeLayerPalette - not implemented\n"));
  return FALSE;
}

BOOL WIN32API wglShareLists(HGC hgc1,HGC hgc2)
{
  dprintf(("OPENGL32: wglShareLists - not implemented\n"));
  return FALSE;
}

/*BOOL WIN32API wglGetDefaultProcAddress(char *x)
{
  dprintf(("OPENGL32: wglGetDefaultProcAddress - not implemented\n"));
  return FALSE;
}*/

GLint WIN32API wglSetLayerPaletteEntries(HDC   hdc,
                                         GLint iLayerPlane,
                                         GLint iStart,
                                         GLint cEntries,
                                         const COLORREF *pcr)
{
  dprintf(("OPENGL32: wglSetLayerPaletteEntries - not implemented\n"));
  return 0;
}

PFN WIN32API wglGetProcAddress(LPCSTR *lpszProc)
{
  dprintf(("OPENGL32: wglGetProcAddress - not implemented\n"));
  return NULL;
}

BOOL WIN32API wglUseFontOutlinesA(HDC     hdc,
                                  DWORD   first,
                                  DWORD   count,
                                  DWORD   listbase,
                                  GLfloat deviation,
                                  GLfloat extrusion,
                                  GLint   format)
{
  dprintf(("OPENGL32: wglUseFontOutlinesA - not implemented\n"));
  return FALSE;
}

BOOL WIN32API wglUseFontOutlinesW(HDC     hdc,
                                  DWORD   first,
                                  DWORD   count,
                                  DWORD   listbase,
                                  GLfloat deviation,
                                  GLfloat extrusion,
                                  GLint   format)
{
  dprintf(("OPENGL32: wglUseFontOutlinesW - not implemented\n"));
  return FALSE;
}

BOOL WIN32API wglSwapLayerBuffers(HDC hdc,GLuint fuPlanes)
{
  dprintf(("OPENGL32: wglSwapLayerBuffers - not implemented\n"));
  return FALSE;
}

BOOL WIN32API wglDescribeLayerPlane(HDC                   hdc,
                                    GLint                 iPixelFormat,
                                    GLint                 iLayerPlane,
                                    GLuint                nBytes,
                                    LAYERPLANEDESCRIPTOR *plpd)
{
  dprintf(("OPENGL32: wglDescribeLayerPlane - not implemented\n"));
  return FALSE;
}

BOOL WIN32API wglCreateLayerContext(HDC hdc,GLint iLayer)
{
  dprintf(("OPENGL32: wglCreateLayerContext - not implemented\n"));
  return FALSE;
}

/***************************************************************************/
/*                                                                         */
/* gl Conversion functions                                                 */
/*                                                                         */
/***************************************************************************/

void WIN32API glAccum(GLenum op,GLfloat value)
{
  OS2glAccum(op,value);
}

void WIN32API glAlphaFunc(GLenum func,GLclampf ref)
{
  OS2glAlphaFunc(func,ref);
}

void WIN32API glBegin(GLenum mode)
{
  OS2glBegin(mode);
}

void WIN32API glBitmap(GLsizei width,
                       GLsizei height,
                       GLfloat xorig,
                       GLfloat yorig,
                       GLfloat xmove,
                       GLfloat ymove,
                       const GLubyte *bitmap)
{
  OS2glBitmap(width,height,xorig,yorig,xmove,ymove,bitmap);
}

void WIN32API glBlendFunc(GLenum sfactor,
                          GLenum dfactor)
{
  OS2glBlendFunc(sfactor,dfactor);
}

void WIN32API glCallList(GLuint list)
{
  OS2glCallList(list);
}

void WIN32API glCallLists(GLsizei n,
                          GLenum type,
                          const GLvoid *lists)
{
  OS2glCallLists(n,type,lists);
}

void WIN32API glClear(GLbitfield mask)
{
  OS2glClear(mask);
}

void WIN32API glClearAccum(GLfloat red,
                           GLfloat green,
                           GLfloat blue,
                           GLfloat alpha)
{
  OS2glClearAccum(red,green,blue,alpha);
}

void WIN32API glClearColor(GLclampf red,
                           GLclampf green,
                           GLclampf blue,
                           GLclampf alpha)
{
  OS2glClearColor(red,green,blue,alpha);
}

void WIN32API glClearDepth(GLclampd depth)
{
  OS2glClearDepth(depth);
}

void WIN32API glClearIndex(GLfloat c)
{
  OS2glClearIndex(c);
}

void WIN32API glClearStencil(GLint s)
{
  OS2glClearStencil(s);
}

void WIN32API glClipPlane(GLenum plane,
                          const GLdouble *equation)
{
  OS2glClipPlane(plane,equation);
}

void WIN32API glColor3b(GLbyte red,
                        GLbyte green,
                        GLbyte blue)
{
  OS2glColor3b(red,green,blue);
}

void WIN32API glColor3bv(const GLbyte *v)
{
  OS2glColor3bv(v);
}

void WIN32API glColor3d(GLdouble red,
                        GLdouble green,
                        GLdouble blue)
{
  OS2glColor3d(red,green,blue);
}

void WIN32API glColor3dv(const GLdouble *v)
{
  OS2glColor3dv(v);
}

void WIN32API glColor3f(GLfloat red,
                        GLfloat green,
                        GLfloat blue)
{
  OS2glColor3f(red,green,blue);
}

void WIN32API glColor3fv(const GLfloat *v)
{
  OS2glColor3fv(v);
}

void WIN32API glColor3i(GLint red,
                        GLint green,
                        GLint blue)
{
  OS2glColor3i(red,green,blue);
}

void WIN32API glColor3iv(const GLint *v)
{
  OS2glColor3iv(v);
}

void WIN32API glColor3s(GLshort red,
                        GLshort green,
                        GLshort blue)
{
  OS2glColor3s(red,green,blue);
}

void WIN32API glColor3sv(const GLshort *v)
{
  OS2glColor3sv(v);
}

void WIN32API glColor3ub(GLubyte red,
                         GLubyte green,
                         GLubyte blue)
{
  OS2glColor3ub(red,green,blue);
}

void WIN32API glColor3ubv(const GLubyte *v)
{
  OS2glColor3ubv(v);
}

void WIN32API glColor3ui(GLuint red,
                         GLuint green,
                         GLuint blue)
{
  OS2glColor3ui(red,green,blue);
}

void WIN32API glColor3uiv(const GLuint *v)
{
  OS2glColor3uiv(v);
}

void WIN32API glColor3us(GLushort red,
                         GLushort green,
                         GLushort blue)
{
  OS2glColor3us(red,green,blue);
}

void WIN32API glColor3usv(const GLushort *v)
{
  OS2glColor3usv(v);
}

void WIN32API glColor4b(GLbyte red,
                        GLbyte green,
                        GLbyte blue,
                        GLbyte alpha)
{
  OS2glColor4b(red,green,blue,alpha);
}

void WIN32API glColor4bv(const GLbyte *v)
{
  OS2glColor4bv(v);
}

void WIN32API glColor4d(GLdouble red,
                        GLdouble green,
                        GLdouble blue,
                        GLdouble alpha)
{
  OS2glColor4d(red,green,blue,alpha);
}

void WIN32API glColor4dv(const GLdouble *v)
{
  OS2glColor4dv(v);
}

void WIN32API glColor4f(GLfloat red,
                        GLfloat green,
                        GLfloat blue,
                        GLfloat alpha)
{
  OS2glColor4f(red,green,blue,alpha);
}

void WIN32API glColor4fv(const GLfloat *v)
{
  OS2glColor4fv(v);
}

void WIN32API glColor4i(GLint red,
                        GLint green,
                        GLint blue,
                        GLint alpha)
{
  OS2glColor4i(red,green,blue,alpha);
}

void WIN32API glColor4iv(const GLint *v)
{
  OS2glColor4iv(v);
}

void WIN32API glColor4s(GLshort red,
                        GLshort green,
                        GLshort blue,
                        GLshort alpha)
{
  OS2glColor4s(red,green,blue,alpha);
}

void WIN32API glColor4sv(const GLshort *v)
{
  OS2glColor4sv(v);
}

void WIN32API glColor4ub(GLubyte red,
                         GLubyte green,
                         GLubyte blue,
                         GLubyte alpha)
{
  OS2glColor4ub(red,green,blue,alpha);
}

void WIN32API glColor4ubv(const GLubyte *v)
{
  OS2glColor4ubv(v);
}

void WIN32API glColor4ui(GLuint red,
                         GLuint green,
                         GLuint blue,
                         GLuint alpha)
{
  OS2glColor4ui(red,green,blue,alpha);
}

void WIN32API glColor4uiv(const GLuint *v)
{
  OS2glColor4uiv(v);
}

void WIN32API glColor4us(GLushort red,
                         GLushort green,
                         GLushort blue,
                         GLushort alpha)
{
  OS2glColor4us(red,green,blue,alpha);
}

void WIN32API glColor4usv(const GLushort *v)
{
  OS2glColor4usv(v);
}

void WIN32API glColorMask(GLboolean red,
                          GLboolean green,
                          GLboolean blue,
                          GLboolean alpha)
{
  OS2glColorMask(red,green,blue,alpha);
}

void WIN32API glColorMaterial(GLenum face,
                              GLenum mode)
{
  OS2glColorMaterial(face,mode);
}

void WIN32API glCopyPixels(GLint x,
                           GLint y,
                           GLsizei width,
                           GLsizei height,
                           GLenum type)
{
  OS2glCopyPixels(x,y,width,height,type);
}

void WIN32API glCullFace(GLenum mode)
{
  OS2glCullFace(mode);
}

void WIN32API glDeleteLists(GLuint list,
                            GLsizei range)
{
  OS2glDeleteLists(list,range);
}

void WIN32API glDepthFunc(GLenum func)
{
  OS2glDepthFunc(func);
}

void WIN32API glDepthMask(GLboolean flag)
{
  OS2glDepthMask(flag);
}

void WIN32API glDepthRange(GLclampd near,
                           GLclampd far)
{
  OS2glDepthRange(near,far);
}

void WIN32API glDisable(GLenum cap)
{
  OS2glDisable(cap);
}

void WIN32API glDrawBuffer(GLenum mode)
{
  OS2glDrawBuffer(mode);
}

void WIN32API glDrawPixels(GLsizei width,
                           GLsizei height,
                           GLenum format,
                           GLenum type,
                           const GLvoid *pixels)
{
  OS2glDrawPixels(width,height,format,type,pixels);
}

void WIN32API glEdgeFlag(GLboolean flag)
{
  OS2glEdgeFlag(flag);
}

void WIN32API glEdgeFlagv(const GLboolean *flag)
{
  OS2glEdgeFlagv(flag);
}

void WIN32API glEnable(GLenum cap)
{
  OS2glEnable(cap);
}

void WIN32API glEnd(void)
{
  OS2glEnd();
}

void WIN32API glEndList(void)
{
  OS2glEndList();
}

void WIN32API glEvalCoord1d(GLdouble u)
{
  OS2glEvalCoord1d(u);
}

void WIN32API glEvalCoord1dv(const GLdouble *u)
{
  OS2glEvalCoord1dv(u);
}

void WIN32API glEvalCoord1f(GLfloat u)
{
  OS2glEvalCoord1f(u);
}

void WIN32API glEvalCoord1fv(const GLfloat *u)
{
  OS2glEvalCoord1fv(u);
}

void WIN32API glEvalCoord2d(GLdouble u,
                            GLdouble v)
{
  OS2glEvalCoord2d(u,v);
}

void WIN32API glEvalCoord2dv(const GLdouble *u)
{
  OS2glEvalCoord2dv(u);
}

void WIN32API glEvalCoord2f(GLfloat u,
                            GLfloat v)
{
  OS2glEvalCoord2f(u,v);
}

void WIN32API glEvalCoord2fv(const GLfloat *u)
{
  OS2glEvalCoord2fv(u);
}

void WIN32API glEvalMesh1(GLenum mode,
                          GLint i1,
                          GLint i2)
{
  OS2glEvalMesh1(mode,i1,i2);
}

void WIN32API glEvalMesh2(GLenum mode,
                          GLint i1,
                          GLint i2,
                          GLint j1,
                          GLint j2)
{
  OS2glEvalMesh2(mode,i1,i2,j1,j2);
}

void WIN32API glEvalPoint1(GLint i)
{
  OS2glEvalPoint1(i);
}

void WIN32API glEvalPoint2(GLint i,
                           GLint j)
{
  OS2glEvalPoint2(i,j);
}

void WIN32API glFeedbackBuffer(GLsizei size,
                               GLenum type,
                               GLfloat *buffer)
{
  OS2glFeedbackBuffer(size,type,buffer);
}

void WIN32API glFinish(void)
{
  OS2glFinish();
}

void WIN32API glFlush(void)
{
  OS2glFlush();

  dprintf(("OPENGL32: glFlush() complete\n"));
}

void WIN32API glFogf(GLenum pname,
                     GLfloat param)
{
  OS2glFogf(pname,param);
}

void WIN32API glFogfv(GLenum pname,
                      const GLfloat *params)
{
  OS2glFogfv(pname,params);
}

void WIN32API glFogi(GLenum pname,
                     GLint param)
{
  OS2glFogi(pname,param);
}

void WIN32API glFogiv(GLenum pname,
                      const GLint *params)
{
  OS2glFogiv(pname,params);
}

void WIN32API glFrontFace(GLenum mode)
{
  OS2glFrontFace(mode);
}

void WIN32API glFrustum(GLdouble left,
                        GLdouble right,
                        GLdouble bottom,
                        GLdouble top,
                        GLdouble near,
                        GLdouble far)
{
  OS2glFrustum(left,right,bottom,top,near,far);
}

GLuint WIN32API glGenLists(GLsizei range)
{
  return OS2glGenLists(range);
}

void WIN32API glGetBooleanv(GLenum pname,
                            GLboolean *params)
{
  OS2glGetBooleanv(pname,params);
}

void WIN32API glGetClipPlane(GLenum plane,
                             GLdouble *equation)
{
  OS2glGetClipPlane(plane,equation);
}

void WIN32API glGetDoublev(GLenum pname,
                           GLdouble *params)
{
  OS2glGetDoublev(pname,params);
}

GLenum WIN32API glGetError(void)
{
  return OS2glGetError();
}

void WIN32API glGetFloatv(GLenum pname,
                          GLfloat *params)
{
  OS2glGetFloatv(pname,params);
}

void WIN32API glGetIntegerv(GLenum pname,
                            GLint *params)
{
  OS2glGetIntegerv(pname,params);
}

void WIN32API glGetLightfv(GLenum light,
                           GLenum pname,
                           GLfloat *params)
{
  OS2glGetLightfv(light,pname,params);
}

void WIN32API glGetLightiv(GLenum light,
                           GLenum pname,
                           GLint *params)
{
  OS2glGetLightiv(light,pname,params);
}

void WIN32API glGetMapdv(GLenum target,
                         GLenum query,
                         GLdouble *v)
{
  OS2glGetMapdv(target,query,v);
}

void WIN32API glGetMapfv(GLenum target,
                         GLenum query,
                         GLfloat *v)
{
  OS2glGetMapfv(target,query,v);
}

void WIN32API glGetMapiv(GLenum target,
                         GLenum query,
                         GLint *v)
{
  OS2glGetMapiv(target,query,v);
}

void WIN32API glGetMaterialfv(GLenum face,
                              GLenum pname,
                              GLfloat *params)
{
  OS2glGetMaterialfv(face,pname,params);
}

void WIN32API glGetMaterialiv(GLenum face,
                              GLenum pname,
                              GLint *params)
{
  OS2glGetMaterialiv(face,pname,params);
}

void WIN32API glGetPixelMapfv(GLenum map,
                              GLfloat *values)
{
  OS2glGetPixelMapfv(map,values);
}

void WIN32API glGetPixelMapuiv(GLenum map,
                               GLuint *values)
{
  OS2glGetPixelMapuiv(map,values);
}

void WIN32API glGetPixelMapusv(GLenum map,
                               GLushort *values)
{
  OS2glGetPixelMapusv(map,values);
}

void WIN32API glGetPolygonStipple(GLubyte *mask)
{
  OS2glGetPolygonStipple(mask);
}

const GLubyte * WIN32API glGetString(GLenum name)
{
  return OS2glGetString(name);
}

void WIN32API glGetTexEnvfv(GLenum target,
                            GLenum pname,
                            GLfloat *params)
{
  OS2glGetTexEnvfv(target,pname,params);
}

void WIN32API glGetTexEnviv(GLenum target,
                            GLenum pname,
                            GLint *params)
{
  OS2glGetTexEnviv(target,pname,params);
}

void WIN32API glGetTexGendv(GLenum coord,
                            GLenum pname,
                            GLdouble *params)
{
  OS2glGetTexGendv(coord,pname,params);
}

void WIN32API glGetTexGenfv(GLenum coord,
                            GLenum pname,
                            GLfloat *params)
{
  OS2glGetTexGenfv(coord,pname,params);
}

void WIN32API glGetTexGeniv(GLenum coord,
                            GLenum pname,
                            GLint *params)
{
  OS2glGetTexGeniv(coord,pname,params);
}

void WIN32API glGetTexImage(GLenum target,
                            GLint level,
                            GLenum format,
                            GLenum type,
                            GLvoid *pixels)
{
  OS2glGetTexImage(target,level,format,type,pixels);
}

void WIN32API glGetTexLevelParameterfv(GLenum target,
                                       GLint level,
                                       GLenum pname,
                                       GLfloat *params)
{
  OS2glGetTexLevelParameterfv(target,level,pname,params);
}

void WIN32API glGetTexLevelParameteriv(GLenum target,
                                       GLint level,
                                       GLenum pname,
                                       GLint *params)
{
  OS2glGetTexLevelParameteriv(target,level,pname,params);
}

void WIN32API glGetTexParameterfv(GLenum target,
                                  GLenum pname,
                                  GLfloat *params)
{
  OS2glGetTexParameterfv(target,pname,params);
}

void WIN32API glGetTexParameteriv(GLenum target,
                                  GLenum pname,
                                  GLint *params)
{
  OS2glGetTexParameteriv(target,pname,params);
}

void WIN32API glHint(GLenum target,
                     GLenum mod)
{
  OS2glHint(target,mod);
}

void WIN32API glIndexMask(GLuint mask)
{
  OS2glIndexMask(mask);
}

void WIN32API glIndexd(GLdouble c)
{
  OS2glIndexd(c);
}

void WIN32API glIndexdv(const GLdouble *c)
{
  OS2glIndexdv(c);
}

void WIN32API glIndexf(GLfloat c)
{
  OS2glIndexf(c);
}

void WIN32API glIndexfv(const GLfloat *c)
{
  OS2glIndexfv(c);
}

void WIN32API glIndexi(GLint c)
{
  OS2glIndexi(c);
}

void WIN32API glIndexiv(const GLint *c)
{
  OS2glIndexiv(c);
}

void WIN32API glIndexs(GLshort c)
{
  OS2glIndexs(c);
}

void WIN32API glIndexsv(const GLshort *c)
{
  OS2glIndexsv(c);
}

void WIN32API glInitNames(void)
{
  OS2glInitNames();
}

GLboolean WIN32API glIsEnabled(GLenum cap)
{
  return OS2glIsEnabled(cap);
}

GLboolean WIN32API glIsList(GLuint list)
{
  return OS2glIsList(list);
}

void WIN32API glLightModelf(GLenum pname,
                            GLfloat param)
{
  OS2glLightModelf(pname,param);
}

void WIN32API glLightModelfv(GLenum pname,
                            const GLfloat *params)
{
  OS2glLightModelfv(pname,params);
}

void WIN32API glLightModeli(GLenum pname,
                            GLint param)
{
  OS2glLightModeli(pname,param);
}

void WIN32API glLightModeliv(GLenum pname,
                            const GLint *params)
{
  OS2glLightModeliv(pname,params);
}

void WIN32API glLightf(GLenum light,
                       GLenum pname,
                       GLfloat param)
{
  OS2glLightf(light,pname,param);
}

void WIN32API glLightfv(GLenum light,
                       GLenum pname,
                       const GLfloat *params)
{
  OS2glLightfv(light,pname,params);
}

void WIN32API glLighti(GLenum light,
                       GLenum pname,
                       GLint param)
{
  OS2glLighti(light,pname,param);
}

void WIN32API glLightiv(GLenum light,
                       GLenum pname,
                       const GLint *params)
{
  OS2glLightiv(light,pname,params);
}

void WIN32API glLineStipple(GLint factor,
                            GLushort pattern)
{
  OS2glLineStipple(factor,pattern);
}

void WIN32API glLineWidth(GLfloat width)
{
  OS2glLineWidth(width);
}

void WIN32API glListBase(GLuint base)
{
  OS2glListBase(base);
}

void WIN32API glLoadIdentity(void)
{
  OS2glLoadIdentity();
}

void WIN32API glLoadMatrixd(GLdouble *m)
{
  OS2glLoadMatrixd(m);
}

void WIN32API glLoadMatrixf(GLfloat *m)
{
  OS2glLoadMatrixf(m);
}

void WIN32API glLoadName(GLuint name)
{
  OS2glLoadName(name);
}

void WIN32API glLogicOp(GLenum opcode)
{
  OS2glLogicOp(opcode);
}

void WIN32API glMap1d(GLenum target,
                      GLdouble u1,
                      GLdouble u2,
                      GLint stride,
                      GLint order,
                      const GLdouble *points)
{
  OS2glMap1d(target,u1,u2,stride,order,points);
}

void WIN32API glMap1f(GLenum target,
                      GLfloat u1,
                      GLfloat u2,
                      GLint stride,
                      GLint order,
                      const GLfloat *points)
{
  OS2glMap1f(target,u1,u2,stride,order,points);
}

void WIN32API glMap2d(GLenum target,
                      GLdouble u1,
                      GLdouble u2,
                      GLint ustride,
                      GLint uorder,
                      GLdouble v1,
                      GLdouble v2,
                      GLint vstride,
                      GLint vorder,
                      const GLdouble *points)
{
  OS2glMap2d(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points);
}

void WIN32API glMap2f(GLenum target,
                      GLfloat u1,
                      GLfloat u2,
                      GLint ustride,
                      GLint uorder,
                      GLfloat v1,
                      GLfloat v2,
                      GLint vstride,
                      GLint vorder,
                      const GLfloat *points)
{
  OS2glMap2f(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points);
}

void WIN32API glMapGrid1d(GLint un,
                          GLdouble u1,
                          GLdouble u2)
{
  OS2glMapGrid1d(un,u1,u2);
}

void WIN32API glMapGrid1f(GLint un,
                          GLfloat u1,
                          GLfloat u2)
{
  OS2glMapGrid1f(un,u1,u2);
}

void WIN32API glMapGrid2d(GLint un,
                          GLdouble u1,
                          GLdouble u2,
                          GLint uv,
                          GLdouble v1,
                          GLdouble v2)
{
  OS2glMapGrid2d(un,u1,u2,uv,v1,v2);
}

void WIN32API glMapGrid2f(GLint un,
                          GLfloat u1,
                          GLfloat u2,
                          GLint uv,
                          GLfloat v1,
                          GLfloat v2)
{
  OS2glMapGrid2f(un,u1,u2,uv,v1,v2);
}

void WIN32API glMaterialf(GLenum face,
                          GLenum pname,
                          GLfloat param)
{
  OS2glMaterialf(face,pname,param);
}

void WIN32API glMaterialfv(GLenum face,
                           GLenum pname,
                           const GLfloat *params)
{
  OS2glMaterialfv(face,pname,params);
}

void WIN32API glMateriali(GLenum face,
                          GLenum pname,
                          GLint param)
{
  OS2glMateriali(face,pname,param);
}

void WIN32API glMaterialiv(GLenum face,
                           GLenum pname,
                           const GLint *params)
{
  OS2glMaterialiv(face,pname,params);
}

void WIN32API glMatrixMode(GLenum mode)
{
  OS2glMatrixMode(mode);
}

void WIN32API glMultMatrixd(const GLdouble *m)
{
  OS2glMultMatrixd(m);
}

void WIN32API glMultMatrixf(const GLfloat *m)
{
  OS2glMultMatrixf(m);
}

void WIN32API glNewList(GLuint list,
                        GLenum mode)
{
  OS2glNewList(list,mode);
}

void WIN32API glNormal3b(GLbyte nx,
                         GLbyte ny,
                         GLbyte nz)
{
  OS2glNormal3b(nx,ny,nz);
}

void WIN32API glNormal3bv(const GLbyte *v)
{
  OS2glNormal3bv(v);
}

void WIN32API glNormal3d(GLdouble nx,
                         GLdouble ny,
                         GLdouble nz)
{
  OS2glNormal3d(nx,ny,nz);
}

void WIN32API glNormal3dv(const GLdouble *v)
{
  OS2glNormal3dv(v);
}

void WIN32API glNormal3f(GLfloat nx,
                         GLfloat ny,
                         GLfloat nz)
{
  OS2glNormal3f(nx,ny,nz);
}

void WIN32API glNormal3fv(const GLfloat *v)
{
  OS2glNormal3fv(v);
}

void WIN32API glNormal3i(GLint nx,
                         GLint ny,
                         GLint nz)
{
  OS2glNormal3i(nx,ny,nz);
}

void WIN32API glNormal3iv(const GLint *v)
{
  OS2glNormal3iv(v);
}

void WIN32API glNormal3s(GLshort nx,
                         GLshort ny,
                         GLshort nz)
{
  OS2glNormal3s(nx,ny,nz);
}

void WIN32API glNormal3sv(const GLshort *v)
{
  OS2glNormal3sv(v);
}

void WIN32API glOrtho(GLdouble left,
                      GLdouble right,
                      GLdouble bottom,
                      GLdouble top,
                      GLdouble near,
                      GLdouble far)
{
  OS2glOrtho(left,right,bottom,top,near,far);
}

void WIN32API glPassThrough(GLfloat token)
{
  OS2glPassThrough(token);
}

void WIN32API glPixelMapfv(GLenum map,
                           GLint mapsize,
                           const GLfloat *values)
{
  OS2glPixelMapfv(map,mapsize,values);
}

void WIN32API glPixelMapuiv(GLenum map,
                           GLint mapsize,
                           const GLuint *values)
{
  OS2glPixelMapuiv(map,mapsize,values);
}

void WIN32API glPixelMapusv(GLenum map,
                           GLint mapsize,
                           const GLushort *values)
{
  OS2glPixelMapusv(map,mapsize,values);
}

void WIN32API glPixelStoref(GLenum pname,
                            GLfloat param)
{
  OS2glPixelStoref(pname,param);
}

void WIN32API glPixelStorei(GLenum pname,
                            GLint param)
{
  OS2glPixelStorei(pname,param);
}

void WIN32API glPixelTransferf(GLenum pname,
                               GLfloat param)
{
  OS2glPixelTransferf(pname,param);
}

void WIN32API glPixelTransferi(GLenum pname,
                               GLint param)
{
  OS2glPixelTransferi(pname,param);
}

void WIN32API glPixelZoom(GLfloat xfactor,
                          GLfloat yfactor)
{
  OS2glPixelZoom(xfactor,yfactor);
}

void WIN32API glPointSize(GLfloat size)
{
  OS2glPointSize(size);
}

void WIN32API glPolygonMode(GLenum face,
                            GLenum mode)
{
  OS2glPolygonMode(face,mode);
}

void WIN32API glPolygonStipple(const GLubyte *mask)
{
  OS2glPolygonStipple(mask);
}

void WIN32API glPopAttrib(void)
{
  OS2glPopAttrib();
}

void WIN32API glPopMatrix (void)
{
  OS2glPopMatrix();
}

void WIN32API glPopName(void)
{
  OS2glPopName();
}

void WIN32API glPushAttrib(GLbitfield mask)
{
  OS2glPushAttrib(mask);
}

void WIN32API glPushMatrix(void)
{
  OS2glPushMatrix();
}

void WIN32API glPushName(GLuint name)
{
  OS2glPushName(name);
}

void WIN32API glRasterPos2d(GLdouble x,
                            GLdouble y)
{
  OS2glRasterPos2d(x,y);
}

void WIN32API glRasterPos2dv(const GLdouble *v)
{
  OS2glRasterPos2dv(v);
}
void WIN32API glRasterPos2f(GLfloat x,
                            GLfloat y)
{
  OS2glRasterPos2f(x,y);
}

void WIN32API glRasterPos2fv(const GLfloat *v)
{
  OS2glRasterPos2fv(v);
}

void WIN32API glRasterPos2i(GLint x,
                            GLint y)
{
  OS2glRasterPos2i(x,y);
}

void WIN32API glRasterPos2iv(const GLint *v)
{
  OS2glRasterPos2iv(v);
}

void WIN32API glRasterPos2s(GLshort x,
                            GLshort y)
{
  OS2glRasterPos2s(x,y);
}

void WIN32API glRasterPos2sv(const GLshort *v)
{
  OS2glRasterPos2sv(v);
}

void WIN32API glRasterPos3d(GLdouble x,
                            GLdouble y,
                            GLdouble z)
{
  OS2glRasterPos3d(x,y,z);
}

void WIN32API glRasterPos3dv(const GLdouble *v)
{
  OS2glRasterPos3dv(v);
}

void WIN32API glRasterPos3f(GLfloat x,
                            GLfloat y,
                            GLfloat z)
{
  OS2glRasterPos3f(x,y,z);
}

void WIN32API glRasterPos3fv(const GLfloat *v)
{
  OS2glRasterPos3fv(v);
}

void WIN32API glRasterPos3i(GLint x,
                            GLint y,
                            GLint z)
{
  OS2glRasterPos3i(x,y,z);
}

void WIN32API glRasterPos3iv(const GLint *v)
{
  OS2glRasterPos3iv(v);
}

void WIN32API glRasterPos3s(GLshort x,
                            GLshort y,
                            GLshort z)
{
  OS2glRasterPos3s(x,y,z);
}

void WIN32API glRasterPos3sv(const GLshort *v)
{
  OS2glRasterPos3sv(v);
}

void WIN32API glRasterPos4d(GLdouble x,
                            GLdouble y,
                            GLdouble z,
                            GLdouble w)
{
  OS2glRasterPos4d(x,y,z,w);
}

void WIN32API glRasterPos4dv(const GLdouble *v)
{
  OS2glRasterPos4dv(v);
}

void WIN32API glRasterPos4f(GLfloat x,
                            GLfloat y,
                            GLfloat z,
                            GLfloat w)
{
  OS2glRasterPos4f(x,y,z,w);
}

void WIN32API glRasterPos4fv(const GLfloat *v)
{
  OS2glRasterPos4fv(v);
}

void WIN32API glRasterPos4i(GLint x,
                            GLint y,
                            GLint z,
                            GLint w)
{
  OS2glRasterPos4i(x,y,z,w);
}

void WIN32API glRasterPos4iv(const GLint *v)
{
  OS2glRasterPos4iv(v);
}

void WIN32API glRasterPos4s(GLshort x,
                            GLshort y,
                            GLshort z,
                            GLshort w)
{
  OS2glRasterPos4s(x,y,z,w);
}

void WIN32API glRasterPos4sv(const GLshort *v)
{
  OS2glRasterPos4sv(v);
}

void WIN32API glReadBuffer(GLenum mode)
{
  OS2glReadBuffer(mode);
}

void WIN32API glReadPixels(GLint x,
                           GLint y,
                           GLsizei width,
                           GLsizei height,
                           GLenum format,
                           GLenum type,
                           GLvoid *pixels)
{
  OS2glReadPixels(x,y,width,height,format,type,pixels);
}

void WIN32API glRectd(GLdouble x1,
                      GLdouble y1,
                      GLdouble x2,
                      GLdouble y2)
{
  OS2glRectd(x1,y1,x2,y2);
}

void WIN32API glRectdv(const GLdouble *v1,
                       const GLdouble *v2)
{
  OS2glRectdv(v1,v2);
}

void WIN32API glRectf(GLfloat x1,
                      GLfloat y1,
                      GLfloat x2,
                      GLfloat y2)
{
  OS2glRectf(x1,y1,x2,y2);
}

void WIN32API glRectfv(const GLfloat *v1,
                       const GLfloat *v2)
{
  OS2glRectfv(v1,v2);
}

void WIN32API glRecti(GLint x1,
                      GLint y1,
                      GLint x2,
                      GLint y2)
{
  OS2glRecti(x1,y1,x2,y2);
}

void WIN32API glRectiv(const GLint *v1,
                       const GLint *v2)
{
  OS2glRectiv(v1,v2);
}

void WIN32API glRects(GLshort x1,
                      GLshort y1,
                      GLshort x2,
                      GLshort y2)
{
  OS2glRects(x1,y1,x2,y2);
}

void WIN32API glRectsv(const GLshort *v1,
                       const GLshort *v2)
{
  OS2glRectsv(v1,v2);
}

GLint WIN32API glRenderMode(GLenum mode)
{
  return OS2glRenderMode(mode);
}

void WIN32API glRotated(GLdouble angle,
                        GLdouble x,
                        GLdouble y,
                        GLdouble z)
{
  OS2glRotated(angle,x,y,z);
}

void WIN32API glRotatef(GLfloat angle,
                        GLfloat x,
                        GLfloat y,
                        GLfloat z)
{
  OS2glRotatef(angle,x,y,z);
}

void WIN32API glScaled(GLdouble x,
                       GLdouble y,
                       GLdouble z)
{
  OS2glScaled(x,y,z);
}

void WIN32API glScalef(GLfloat x,
                       GLfloat y,
                       GLfloat z)
{
  OS2glScalef(x,y,z);
}

void WIN32API glScissor(GLint x,
                        GLint y,
                        GLsizei width,
                        GLsizei height)
{
  OS2glScissor(x,y,width,height);
}

void WIN32API glSelectBuffer(GLsizei size,
                             GLuint *buffer)
{
  OS2glSelectBuffer(size,buffer);
}

void WIN32API glShadeModel(GLenum mode)
{
  OS2glShadeModel(mode);
}

void WIN32API glStencilFunc(GLenum func,
                            GLint ref,
                            GLuint mask)
{
  OS2glStencilFunc(func,ref,mask);
}

void WIN32API glStencilMask(GLuint mask)
{
  OS2glStencilMask(mask);
}

void WIN32API glStencilOp(GLenum fail,
                          GLenum zfail,
                          GLenum zpass)
{
  OS2glStencilOp(fail,zfail,zpass);
}

void WIN32API glTexCoord1d(GLdouble s)
{
  OS2glTexCoord1d(s);
}

void WIN32API glTexCoord1dv(const GLdouble *v)
{
  OS2glTexCoord1dv(v);
}

void WIN32API glTexCoord1f(GLfloat s)
{
  OS2glTexCoord1f(s);
}

void WIN32API glTexCoord1fv(const GLfloat *v)
{
  OS2glTexCoord1fv(v);
}

void WIN32API glTexCoord1i(GLint s)
{
  OS2glTexCoord1i(s);
}

void WIN32API glTexCoord1iv(const GLint *v)
{
  OS2glTexCoord1iv(v);
}

void WIN32API glTexCoord1s(GLshort s)
{
  OS2glTexCoord1s(s);
}

void WIN32API glTexCoord1sv(const GLshort *v)
{
  OS2glTexCoord1sv(v);
}

void WIN32API glTexCoord2d(GLdouble s,
                           GLdouble t)
{
  OS2glTexCoord2d(s,t);
}

void WIN32API glTexCoord2dv(const GLdouble *v)
{
  OS2glTexCoord2dv(v);
}

void WIN32API glTexCoord2f(GLfloat s,
                           GLfloat t)
{
  OS2glTexCoord2f(s,t);
}

void WIN32API glTexCoord2fv(const GLfloat *v)
{
  OS2glTexCoord2fv(v);
}

void WIN32API glTexCoord2i(GLint s,
                           GLint t)
{
  OS2glTexCoord2i(s,t);
}

void WIN32API glTexCoord2iv(const GLint *v)
{
  OS2glTexCoord2iv(v);
}

void WIN32API glTexCoord2s(GLshort s,
                           GLshort t)
{
  OS2glTexCoord2s(s,t);
}

void WIN32API glTexCoord2sv(const GLshort *v)
{
  OS2glTexCoord2sv(v);
}

void WIN32API glTexCoord3d(GLdouble s,
                           GLdouble t,
                           GLdouble r)
{
  OS2glTexCoord3d(s,t,r);
}

void WIN32API glTexCoord3dv(const GLdouble *v)
{
  OS2glTexCoord3dv(v);
}

void WIN32API glTexCoord3f(GLfloat s,
                           GLfloat t,
                           GLfloat r)
{
  OS2glTexCoord3f(s,t,r);
}

void WIN32API glTexCoord3fv(const GLfloat *v)
{
  OS2glTexCoord3fv(v);
}

void WIN32API glTexCoord3i(GLint s,
                           GLint t,
                           GLint r)
{
  OS2glTexCoord3i(s,t,r);
}

void WIN32API glTexCoord3iv(const GLint *v)
{
  OS2glTexCoord3iv(v);
}

void WIN32API glTexCoord3s(GLshort s,
                           GLshort t,
                           GLshort r)
{
  OS2glTexCoord3s(s,t,r);
}

void WIN32API glTexCoord3sv(const GLshort *v)
{
  OS2glTexCoord3sv(v);
}

void WIN32API glTexCoord4d(GLdouble s,
                           GLdouble t,
                           GLdouble r,
                           GLdouble q)
{
  OS2glTexCoord4d(s,t,r,q);
}

void WIN32API glTexCoord4dv(const GLdouble *v)
{
  OS2glTexCoord4dv(v);
}

void WIN32API glTexCoord4f(GLfloat s,
                           GLfloat t,
                           GLfloat r,
                           GLfloat q)
{
  OS2glTexCoord4f(s,t,r,q);
}

void WIN32API glTexCoord4fv(const GLfloat *v)
{
  OS2glTexCoord4fv(v);
}

void WIN32API glTexCoord4i(GLint s,
                           GLint t,
                           GLint r,
                           GLint q)
{
  OS2glTexCoord4i(s,t,r,q);
}

void WIN32API glTexCoord4iv(const GLint *v)
{
  OS2glTexCoord4iv(v);
}

void WIN32API glTexCoord4s(GLshort s,
                           GLshort t,
                           GLshort r,
                           GLshort q)
{
  OS2glTexCoord4s(s,t,r,q);
}

void WIN32API glTexCoord4sv(const GLshort *v)
{
  OS2glTexCoord4sv(v);
}

void WIN32API glTexEnvf(GLenum target,
                        GLenum pname,
                        GLfloat param)
{
  OS2glTexEnvf(target,pname,param);
}

void WIN32API glTexEnvfv(GLenum target,
                         GLenum pname,
                         const GLfloat *params)
{
  OS2glTexEnvfv(target,pname,params);
}

void WIN32API glTexEnvi(GLenum target,
                        GLenum pname,
                        GLint param)
{
  OS2glTexEnvi(target,pname,param);
}

void WIN32API glTexEnviv(GLenum target,
                         GLenum pname,
                         const GLint *params)
{
  OS2glTexEnviv(target,pname,params);
}

void WIN32API glTexGend(GLenum coord,
                        GLenum pname,
                        GLdouble param)
{
  OS2glTexGend(coord,pname,param);
}

void WIN32API glTexGendv(GLenum coord,
                         GLenum pname,
                         const GLdouble *params)
{
  OS2glTexGendv(coord,pname,params);
}

void WIN32API glTexGenf(GLenum coord,
                        GLenum pname,
                        GLfloat param)
{
  OS2glTexGenf(coord,pname,param);
}

void WIN32API glTexGenfv(GLenum coord,
                         GLenum pname,
                         const GLfloat *params)
{
  OS2glTexGenfv(coord,pname,params);
}

void WIN32API glTexGeni(GLenum coord,
                        GLenum pname,
                        GLint param)
{
  OS2glTexGeni(coord,pname,param);
}

void WIN32API glTexGeniv(GLenum coord,
                         GLenum pname,
                         const GLint *params)
{
  OS2glTexGeniv(coord,pname,params);
}


void WIN32API glTexImage1D(GLenum target,
                           GLint level,
                           GLint component,
                           GLsizei width,
                           GLint border,
                           GLenum format,
                           GLenum type,
                           const GLvoid *pixels)
{
  OS2glTexImage1D(target,level,component,width,border,format,type,pixels);
}

void WIN32API glTexImage2D(GLenum target,
                           GLint level,
                           GLint component,
                           GLsizei width,
                           GLsizei height,
                           GLint border,
                           GLenum format,
                           GLenum type,
                           const GLvoid *pixels)
{
  OS2glTexImage2D(target,level,component,width,height,border,format,type,pixels);
}

void WIN32API glTexParameterf(GLenum target,
                              GLenum pname,
                              GLfloat param)
{
  OS2glTexParameterf(target,pname,param);
}

void WIN32API glTexParameterfv(GLenum target,
                               GLenum pname,
                               const GLfloat *params)
{
  OS2glTexParameterfv(target,pname,params);
}

void WIN32API glTexParameteri(GLenum target,
                              GLenum pname,
                              GLint param)
{
  OS2glTexParameteri(target,pname,param);
}

void WIN32API glTexParameteriv(GLenum target,
                               GLenum pname,
                               const GLint *params)
{
  OS2glTexParameteriv(target,pname,params);
}

void WIN32API glTranslated(GLdouble x,
                           GLdouble y,
                           GLdouble z)
{
  OS2glTranslated(x,y,z);
}

void WIN32API glTranslatef(GLfloat x,
                           GLfloat y,
                           GLfloat z)
{
  OS2glTranslatef(x,y,z);
}

void WIN32API glVertex2d(GLdouble x,
                         GLdouble y)
{
  OS2glVertex2d(x,y);
}

void WIN32API glVertex2dv(const GLdouble *v)
{
  OS2glVertex2dv(v);
}

void WIN32API glVertex2f(GLfloat x,
                         GLfloat y)
{
  OS2glVertex2f(x,y);
}

void WIN32API glVertex2fv(const GLfloat *v)
{
  OS2glVertex2fv(v);
}

void WIN32API glVertex2i(GLint x,
                         GLint y)
{
  OS2glVertex2i(x,y);
}

void WIN32API glVertex2iv(const GLint *v)
{
  OS2glVertex2iv(v);
}

void WIN32API glVertex2s(GLshort x,
                         GLshort y)
{
  OS2glVertex2s(x,y);
}

void WIN32API glVertex2sv(const GLshort *v)
{
  OS2glVertex2sv(v);
}

void WIN32API glVertex3d(GLdouble x,
                         GLdouble y,
                         GLdouble z)
{
  OS2glVertex3d(x,y,z);
}

void WIN32API glVertex3dv(const GLdouble *v)
{
  OS2glVertex3dv(v);
}

void WIN32API glVertex3f(GLfloat x,
                         GLfloat y,
                         GLfloat z)
{
  OS2glVertex3f(x,y,z);
}

void WIN32API glVertex3fv(const GLfloat *v)
{
  OS2glVertex3fv(v);
}

void WIN32API glVertex3i(GLint x,
                         GLint y,
                         GLint z)
{
  OS2glVertex3i(x,y,z);
}

void WIN32API glVertex3iv(const GLint *v)
{
  OS2glVertex3iv(v);
}

void WIN32API glVertex3s(GLshort x,
                         GLshort y,
                         GLshort z)
{
  OS2glVertex3s(x,y,z);
}

void WIN32API glVertex3sv(const GLshort *v)
{
  OS2glVertex3sv(v);
}

void WIN32API glVertex4d(GLdouble x,
                         GLdouble y,
                         GLdouble z,
                         GLdouble w)
{
  OS2glVertex4d(x,y,z,w);
}

void WIN32API glVertex4dv(const GLdouble *v)
{
  OS2glVertex4dv(v);
}

void WIN32API glVertex4f(GLfloat x,
                         GLfloat y,
                         GLfloat z,
                         GLfloat w)
{
  OS2glVertex4f(x,y,z,w);
}

void WIN32API glVertex4fv(const GLfloat *v)
{
  OS2glVertex4fv(v);
}

void WIN32API glVertex4i(GLint x,
                         GLint y,
                         GLint z,
                         GLint w)
{
  OS2glVertex4i(x,y,z,w);
}

void WIN32API glVertex4iv(const GLint *v)
{
  OS2glVertex4iv(v);
}

void WIN32API glVertex4s(GLshort x,
                         GLshort y,
                         GLshort z,
                         GLshort w)
{
  OS2glVertex4s(x,y,z,w);
}

void WIN32API glVertex4sv(const GLshort *v)
{
  OS2glVertex4sv(v);
}

void WIN32API glViewport(GLint x,
                         GLint y,
                         GLsizei width,
                         GLsizei height)
{
  OS2glViewport(x,y,width,height);
}

GLboolean WIN32API glAreTexturesResident(GLsizei in,
                                         const GLuint *textures,
                                         GLboolean *residences)
{
  return OS2glAreTexturesResident(in,textures,residences);
}

void WIN32API glArrayElement(GLint i)
{
  OS2glArrayElement(i);
}

void WIN32API glBindTexture(GLenum target,
                            GLuint texture)
{
  OS2glBindTexture(target,texture);
}

void WIN32API glColorPointer(GLint size,
                             GLenum type,
                             GLsizei stride,
                             const GLvoid *pointer)
{
  OS2glColorPointer(size,type,stride,pointer);
}

void WIN32API glCopyTexImage1D(GLenum target,
                               GLint level,
                               GLenum internalformat,
                               GLint x,
                               GLint y,
                               GLsizei width,
                               GLint border)
{
  OS2glCopyTexImage1D(target,level,internalformat,x,y,width,border);
}

void WIN32API glCopyTexImage2D(GLenum target,
                               GLint level,
                               GLenum internalformat,
                               GLint x,
                               GLint y,
                               GLsizei width,
                               GLsizei height,
                               GLint border)
{
  OS2glCopyTexImage2D(target,level,internalformat,x,y,width,height,border);
}

void WIN32API glCopyTexSubImage1D(GLenum target,
                                  GLint level,
                                  GLint xoffset,
                                  GLint x,
                                  GLint y,
                                  GLsizei width)
{
  OS2glCopyTexSubImage1D(target,level,xoffset,x,y,width);
}

void WIN32API glCopyTexSubImage2D(GLenum target,
                                  GLint level,
                                  GLint xoffset,
                                  GLint yoffset,
                                  GLint x,
                                  GLint y,
                                  GLsizei width,
                                  GLsizei height)
{
  OS2glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height);
}

/*void WIN32API glDebugEntry(void)
{
}
*/
void WIN32API glDeleteTextures(GLsizei n,
                               const GLuint *textures)
{
  OS2glDeleteTextures(n,textures);
}

void WIN32API glDisableClientState(GLenum array)
{
  OS2glDisableClientState(array);
}

void WIN32API glDrawArrays(GLenum mode,
                           GLint first,
                           GLsizei count)
{
  OS2glDrawArrays(mode,first,count);
}

void WIN32API glDrawElements(GLenum mode,
                             GLsizei count,
                             GLenum type,
                             const GLvoid *indices)
{
  OS2glDrawElements(mode,count,type,indices);
}

void WIN32API glEdgeFlagPointer(GLsizei stride,
                                const GLboolean *pointer)
{
  OS2glEdgeFlagPointer(stride,pointer);
}

void WIN32API glEnableClientState(GLenum array)
{
  OS2glEnableClientState(array);
}

void WIN32API glGenTextures(GLsizei n,
                            GLuint *textures)
{
  OS2glGenTextures(n,textures);
}

void WIN32API glGetPointerv(GLenum pname,
                            GLvoid *params)
{
  OS2glGetPointerv(pname,params);
}

void WIN32API glIndexPointer(GLenum type,
                             GLsizei stride,
                             const GLvoid *pointer)
{
  OS2glIndexPointer(type,stride,pointer);
}

void WIN32API glIndexub(GLubyte c)
{
  OS2glIndexub(c);
}

void WIN32API glIndexubv(const GLubyte *v)
{
  OS2glIndexubv(v);
}

void WIN32API glInterleavedArrays(GLenum format,
                                  GLsizei stride,
                                  const GLvoid *pointer)
{
  OS2glInterleavedArrays(format,stride,pointer);
}

GLboolean WIN32API glIsTexture(GLuint texture)
{
  return OS2glIsTexture(texture);
}

void WIN32API glNormalPointer(GLenum type,
                              GLsizei stride,
                              const GLvoid *pointer)
{
  OS2glNormalPointer(type,stride,pointer);
}

void WIN32API glPolygonOffset(GLfloat factor,
                              GLfloat units)
{
  OS2glPolygonOffset(factor,units);
}

void WIN32API glPopClientAttrib(void)
{
  OS2glPopClientAttrib();
}

void WIN32API glPrioritizeTextures(GLsizei n,
                                   const GLuint *textures,
                                   const GLclampf *priorities)
{
  OS2glPrioritizeTextures(n,textures,priorities);
}

void WIN32API glPushClientAttrib(GLbitfield mask)
{
  OS2glPushClientAttrib(mask);
}

void WIN32API glTexCoordPointer(GLint size,
                                GLenum type,
                                GLsizei stride,
                                const GLvoid *pointer)
{
  OS2glTexCoordPointer(size,type,stride,pointer);
}

void WIN32API glTexSubImage1D(GLenum target,
                              GLint level,
                              GLint xoffset,
                              GLsizei width,
                              GLenum format,
                              GLenum type,
                              const GLvoid *pixels)
{
  OS2glTexSubImage1D(target,level,xoffset,width,format,type,pixels);
}

void WIN32API glTexSubImage2D(GLenum target,
                              GLint level,
                              GLint xoffset,
                              GLint yoffset,
                              GLsizei width,
                              GLsizei height,
                              GLenum format,
                              GLenum type,
                              const GLvoid *pixels)
{
  OS2glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void WIN32API glVertexPointer(GLint size,
                              GLenum type,
                              GLsizei stride,
                              GLsizei count,
                              const GLvoid *pointer)
{
  OS2glVertexPointer(size,type,stride,count,pointer);
}

/*****************************************************************************/
/*                                                                           */
/* As of yet unsupported gl functions                                        */
/*                                                                           */
/*****************************************************************************/

void WIN32API glTexImage3D(GLenum target,
                           GLint level,
                           GLint component,
                           GLsizei width,
                           GLsizei height,
                           GLsizei depth,
                           GLint border,
                           GLenum format,
                           GLenum type,
                           const GLvoid *pixels)
{
  dprintf(("OPENGL32: glTexImage3D - not implemented\n"));
}

/*****************************************************************************/
/*                                                                           */
/* Module Internal init and cleanup - called by initterm                     */
/*                                                                           */
/*****************************************************************************/

void mod_init(void)
{
  /* OpenGL32 Initialize - Query all valid configs for this system and */
  /* store them. All indexes returned are based upon this list!        */
  dprintf(("OPENGL32: INIT\n"));

  _control87(EM_UNDERFLOW,EM_UNDERFLOW);
  _control87(EM_OVERFLOW,EM_OVERFLOW);

  DosCreateMutexSem(NULL,&hmtxPfdInfo,0,0);

  global_visual_config_count=0;

  global_visual_config_list=
    OS2pglQueryConfigs(WinQueryAnchorBlock(HWND_DESKTOP));

  if(global_visual_config_list)
    {
      while(global_visual_config_list[global_visual_config_count])
        global_visual_config_count++;
    }

  dprintf(("OPENGL32 (INIT): global_visual_config_list: %08X [%d entries]\n",
           global_visual_config_list,
           global_visual_config_count));
}

void mod_cleanup(void)
{
  /* Cleanup all structures we allocated */
  pfdi_destroy_all();

  DosCloseMutexSem(hmtxPfdInfo);

  _control87(0,EM_UNDERFLOW);
  _control87(0,EM_OVERFLOW);

  dprintf(("OPENGL32 TERM\n"));
}
