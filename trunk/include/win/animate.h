/* $Id: animate.h,v 1.3 2000-02-23 17:03:00 cbratschi Exp $ */

/*
 * Animation class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_ANIMATE_H
#define __WINE_ANIMATE_H

#define CINTERFACE

#include "windef.h"
#include "vfw.h"

typedef struct tagANIMATE_INFO
{
   COMCTL32_HEADER      header;

   /* pointer to msvideo functions. it's easier to put them here.
    * to be correct, they should be defined on a per process basis, but
    * this would required a per process storage. We're using a per object
    * storage instead, which is not efficient on memory usage, but
    * will lead to less bugs in the future
    */
   HIC          (WINAPI*  fnICOpen)(DWORD, DWORD, UINT);
   LRESULT      (WINAPI*  fnICClose)(HIC);
   LRESULT      (WINAPI*  fnICSendMessage)(HIC, UINT, DWORD, DWORD);
   DWORD        (WINAPIV* fnICDecompress)(HIC,DWORD,LPBITMAPINFOHEADER,LPVOID,LPBITMAPINFOHEADER,LPVOID);

   /* reference to input stream (file or resource) */
   HGLOBAL              hRes;
   HMMIO                hMMio;  /* handle to mmio stream */
   HWND                 hWnd;
   /* information on the loaded AVI file */
   MainAVIHeader        mah;
   AVIStreamHeader      ash;
   LPBITMAPINFOHEADER   inbih;
   LPDWORD              lpIndex;
   /* data for the decompressor */
   HIC                  hic;
   LPBITMAPINFOHEADER   outbih;
   LPVOID               indata;
   LPVOID               outdata;
   /* data for the background mechanism */
   CRITICAL_SECTION     cs;
   HANDLE               hThread;
   UINT                 delay;
   BOOL                 stopThread;
   UINT                 uTimer;
   /* data for playing the file */
   int                  nFromFrame;
   int                  nToFrame;
   int                  nLoop;
   int                  currFrame;
} ANIMATE_INFO;


extern VOID ANIMATE_Register (VOID);
extern VOID ANIMATE_Unregister (VOID);

#endif  /* __WINE_ANIMATE_H */
