/* $Id: msvfw32.cpp,v 1.7 2001-04-28 09:11:36 hugh Exp $ */
/*
 * Copyright 1998 Marcus Meissner
 * Copyright 1999 Jens Wiessner
 */

#include <stdlib.h>
#include <string.h>
#include <odin.h>

#define ICOM_CINTERFACE 1
////#define strcasecmp               strcmp

#include <os2win.h>
#include "winerror.h"
#include "heap.h"
#include "resource.h"
#include "win.h"
#include "commdlg.h"
#include "spy.h"

#include "wine/obj_base.h"
#include "wingdi.h"
#include "vfw.h"
//#include "driver.h"
// #include "msvfw32.h"

#include "debugtools.h"
#include <debugstr.h>
#include <heapstring.h>

DEFAULT_DEBUG_CHANNEL(msvideo)

/****************************************************************************
 *    VideoForWindowsVersion    [MSVFW.2][MSVIDEO.2]
 * Returns the version in major.minor form.
 * In Windows95 this returns 0x040003b6 (4.950)
 */
DWORD WINAPI
VideoForWindowsVersion(void) {
  return 0x040003B6; /* 4.950 */
}

/* system.ini: [drivers] */

/**************************************************************************
 *    ICInfo        [MSVFW.33]
 * Get information about an installable compressor. Return TRUE if there
 * is one.
 */
BOOL WINAPI
ICInfo(
  DWORD fccType,    /* [in] type of compressor ('vidc') */
  DWORD fccHandler, /* [in] <n>th compressor */
  ICINFO *lpicinfo  /* [out] information about compressor */
) {
  char  type[5],buf[2000];

  memcpy(type,&fccType,4);type[4]=0;
  TRACE("(%s,%ld,%p).\n",type,fccHandler,lpicinfo);
  /* does OpenDriver/CloseDriver */
  lpicinfo->dwSize = sizeof(ICINFO);
  lpicinfo->fccType = fccType;
  lpicinfo->dwFlags = 0;
  if (GetPrivateProfileStringA("drivers32",NULL,NULL,buf,2000,"system.ini")) {
    char *s = buf;
    while (*s) {
      if (!lstrncmpiA(type,s,4)) {
        if(!fccHandler--) {
          lpicinfo->fccHandler = mmioStringToFOURCCA(s+5,0);
          return TRUE;
        }
      }
      s=s+lstrlenA(s)+1; /* either next char or \0 */
    }
  }
  return FALSE;
}

/**************************************************************************
 *    ICOpen        [MSVFW.37]
 * Opens an installable compressor. Return special handle.
 */
HIC WINAPI
ICOpen(DWORD fccType,DWORD fccHandler,UINT wMode) {
  char    type[5],handler[5],codecname[20];
  ICOPEN    icopen;
  HDRVR   hdrv;
  WINE_HIC  *whic;

  memcpy(type,&fccType,4);type[4]=0;
  memcpy(handler,&fccHandler,4);handler[4]=0;
  TRACE("(%s,%s,0x%08lx)\n",type,handler,(DWORD)wMode);
  sprintf(codecname,"%s.%s",type,handler);

  /* Well, lParam2 is in fact a LPVIDEO_OPEN_PARMS, but it has the
   * same layout as ICOPEN
   */
  memset(&icopen,0,sizeof(ICOPEN));
  icopen.fccType    = fccType==0x43444956?0x63646976:fccType; //fccType use vidc if VIDC is passed in.
  icopen.fccHandler = fccHandler;
  icopen.dwSize     = sizeof(ICOPEN);
  icopen.dwVersion  = 0x03320000; // version 3,50,00 ???
  icopen.dwFlags    = wMode;
  TRACE("(0x%08lx,0x%08X=%d,%d)\n",icopen.fccType,icopen.fccHandler,icopen.fccHandler,icopen.dwSize);

  /* FIXME: do we need to fill out the rest too? */
  hdrv=OpenDriverA(codecname,"drivers32",(LPARAM)&icopen);
  if (!hdrv)
  {
    TRACE("Driver returned Error %08x Version set to %08x\n",icopen.dwError,icopen.dwVersion);
    if (!strcasecmp(type,"vids"))
    {
      sprintf(codecname,"vidc.%s",handler);
      fccType = mmioFOURCC('v','i','d','c');
    }
    hdrv=OpenDriverA(codecname,"drivers32",(LPARAM)&icopen);
    if (!hdrv)
    {
      TRACE("Driver returned Error %08x Version set to %08x\n",icopen.dwError,icopen.dwVersion);
      return 0;
    }
  }
  whic = (WINE_HIC *) HeapAlloc(GetProcessHeap(),0,sizeof(WINE_HIC));
  whic->hdrv  = hdrv;
  whic->driverproc= NULL;
  whic->privatevfw= ICSendMessage((HIC)whic,DRV_OPEN,0,(LPARAM)&icopen);
  return (HIC)whic;
}
HIC VFWAPI ICOpenFunction(DWORD fccType, DWORD fccHandler, UINT wMode,
FARPROC lpfnHandler) {
  char    type[5],handler[5];
  HIC   hic;
  WINE_HIC  *whic;

  memcpy(type,&fccType,4);type[4]=0;
  memcpy(handler,&fccHandler,4);handler[4]=0;
  dprintf(("(%s,%s,%d,%p), stub!\n",type,handler,wMode,lpfnHandler));
  hic = ICOpen(fccType,fccHandler,wMode);
  if (!hic)
    return hic;
  whic = (WINE_HIC*)hic;
  whic->driverproc = lpfnHandler;
  return hic;
}


LRESULT WINAPI ICGetInfo(HIC hic,ICINFO *picinfo,DWORD cb) {
  LRESULT   ret;

  TRACE("(0x%08lx,%p,%ld)\n",(DWORD)hic,picinfo,cb);
  ret = ICSendMessage(hic,ICM_GETINFO,(DWORD)picinfo,cb);
  TRACE(" -> 0x%08lx\n",ret);
  return ret;
}

HIC  VFWAPI
ICLocate(
  DWORD fccType, DWORD fccHandler, LPBITMAPINFOHEADER lpbiIn,
  LPBITMAPINFOHEADER lpbiOut, WORD wMode
) {
  char  type[5],handler[5];
  HIC hic;
  DWORD querymsg;

  switch (wMode) {
  case ICMODE_FASTCOMPRESS:
  case ICMODE_COMPRESS:
    querymsg = ICM_COMPRESS_QUERY;
    break;
  case ICMODE_DECOMPRESS:
  case ICMODE_FASTDECOMPRESS:
    querymsg = ICM_DECOMPRESS_QUERY;
    break;
  case ICMODE_DRAW:
    querymsg = ICM_DRAW_QUERY;
    break;
  default:
    dprintf(("Unknown mode (%d)\n",wMode));
    return 0;
  }

  /* Easy case: handler/type match, we just fire a query and return */
  hic = ICOpen(fccType,fccHandler,wMode);
  if (hic) {
    if (!ICSendMessage(hic,querymsg,(DWORD)lpbiIn,(DWORD)lpbiOut))
      return hic;
    ICClose(hic);
  }
  type[4]='\0';memcpy(type,&fccType,4);
  handler[4]='\0';memcpy(handler,&fccHandler,4);
  if (fccType==streamtypeVIDEO) {
    hic = ICLocate(ICTYPE_VIDEO,fccHandler,lpbiIn,lpbiOut,wMode);
    if (hic)
      return hic;
  }
  dprintf(("(%s,%s,%p,%p,0x%04x),unhandled!\n",type,handler,lpbiIn,lpbiOut,wMode));
  return 0;
}

HIC VFWAPI ICGetDisplayFormat(
  HIC hic,LPBITMAPINFOHEADER lpbiIn,LPBITMAPINFOHEADER lpbiOut,
  INT depth,INT dx,INT dy
) {
  HIC tmphic = hic;
  LRESULT lres;

  dprintf(("(0x%08lx,%p,%p,%d,%d,%d),stub!\n",(DWORD)hic,lpbiIn,lpbiOut,depth,dx,dy));
  if (!tmphic) {
    tmphic=ICLocate(ICTYPE_VIDEO,0,lpbiIn,NULL,ICMODE_DECOMPRESS);
    if (!tmphic)
      return tmphic;
  }
  if ((dy == lpbiIn->biHeight) || (dx == lpbiIn->biWidth))
    dy = dx = 0; /* no resize needed */
  /* Can we decompress it ? */
  lres = ICDecompressQuery(tmphic,lpbiIn,NULL);
  if (lres)
    goto errout; /* no, sorry */
  ICDecompressGetFormat(hic,lpbiIn,lpbiOut);
  *lpbiOut=*lpbiIn;
  lpbiOut->biCompression = 0;
  lpbiOut->biSize = sizeof(*lpbiOut);
  if (!depth) {
    HDC hdc;

    hdc = GetDC(0);
    depth = GetDeviceCaps(hdc,12)*GetDeviceCaps(hdc,14);
    ReleaseDC(0,hdc);
    if (depth==15)  depth = 16;
    if (depth<8)  depth =  8;
    /* more constraints and tests */
  }
  if (lpbiIn->biBitCount == 8)
    depth = 8;

  return hic;
errout:
  if (hic!=tmphic)
    ICClose(tmphic);
  return 0;
}

DWORD VFWAPIV
ICCompress(
  HIC hic,DWORD dwFlags,LPBITMAPINFOHEADER lpbiOutput,LPVOID lpData,
  LPBITMAPINFOHEADER lpbiInput,LPVOID lpBits,LPDWORD lpckid,
  LPDWORD lpdwFlags,LONG lFrameNum,DWORD dwFrameSize,DWORD dwQuality,
  LPBITMAPINFOHEADER lpbiPrev,LPVOID lpPrev
) {
  ICCOMPRESS  iccmp;

  iccmp.dwFlags   = dwFlags;

  iccmp.lpbiOutput  = lpbiOutput;
  iccmp.lpOutput    = lpData;
  iccmp.lpbiInput   = lpbiInput;
  iccmp.lpInput   = lpBits;

  iccmp.lpckid    = lpckid;
  iccmp.lpdwFlags   = lpdwFlags;
  iccmp.lFrameNum   = lFrameNum;
  iccmp.dwFrameSize = dwFrameSize;
  iccmp.dwQuality   = dwQuality;
  iccmp.lpbiPrev    = lpbiPrev;
  iccmp.lpPrev    = lpPrev;
  return ICSendMessage(hic,ICM_COMPRESS,(LPARAM)&iccmp,sizeof(iccmp));
}

DWORD VFWAPIV
ICDecompress(HIC hic,DWORD dwFlags,LPBITMAPINFOHEADER lpbiFormat,LPVOID lpData,LPBITMAPINFOHEADER  lpbi,LPVOID lpBits) {
  ICDECOMPRESS  icd;

  icd.dwFlags = dwFlags;
  icd.lpbiInput = lpbiFormat;
  icd.lpInput = lpData;

  icd.lpbiOutput  = lpbi;
  icd.lpOutput  = lpBits;
  icd.ckid  = 0;
  return ICSendMessage(hic,ICM_DECOMPRESS,(LPARAM)&icd,sizeof(icd));
}

LRESULT VFWAPI
ICSendMessage(HIC hic,UINT msg,DWORD lParam1,DWORD lParam2) {
  LRESULT   ret;
  WINE_HIC  *whic = (WINE_HIC*)hic;

#define XX(x) case x: TRACE("(0x%08lx,"#x",0x%08lx,0x%08lx)\n",(DWORD)hic,lParam1,lParam2);break;

  switch (msg) {
  XX(ICM_ABOUT)
  XX(ICM_GETINFO)
  XX(ICM_COMPRESS_FRAMES_INFO)
  XX(ICM_COMPRESS_GET_FORMAT)
  XX(ICM_COMPRESS_GET_SIZE)
  XX(ICM_COMPRESS_QUERY)
  XX(ICM_COMPRESS_BEGIN)
  XX(ICM_COMPRESS)
  XX(ICM_COMPRESS_END)
  XX(ICM_DECOMPRESS_GET_FORMAT)
  XX(ICM_DECOMPRESS_QUERY)
  XX(ICM_DECOMPRESS_BEGIN)
  XX(ICM_DECOMPRESS)
  XX(ICM_DECOMPRESS_END)
  XX(ICM_DECOMPRESS_SET_PALETTE)
  XX(ICM_DECOMPRESS_GET_PALETTE)
  XX(ICM_DRAW_QUERY)
  XX(ICM_DRAW_BEGIN)
  XX(ICM_DRAW_GET_PALETTE)
  XX(ICM_DRAW_START)
  XX(ICM_DRAW_STOP)
  XX(ICM_DRAW_END)
  XX(ICM_DRAW_GETTIME)
  XX(ICM_DRAW)
  XX(ICM_DRAW_WINDOW)
  XX(ICM_DRAW_SETTIME)
  XX(ICM_DRAW_REALIZE)
  XX(ICM_DRAW_FLUSH)
  XX(ICM_DRAW_RENDERBUFFER)
  XX(ICM_DRAW_START_PLAY)
  XX(ICM_DRAW_STOP_PLAY)
  XX(ICM_DRAW_SUGGESTFORMAT)
  XX(ICM_DRAW_CHANGEPALETTE)
  XX(ICM_GETBUFFERSWANTED)
  XX(ICM_GETDEFAULTKEYFRAMERATE)
  XX(ICM_DECOMPRESSEX_BEGIN)
  XX(ICM_DECOMPRESSEX_QUERY)
  XX(ICM_DECOMPRESSEX)
  XX(ICM_DECOMPRESSEX_END)
  XX(ICM_SET_STATUS_PROC)
  default:
    dprintf(("(0x%08lx,0x%08lx,0x%08lx,0x%08lx)\n",(DWORD)hic,(DWORD)msg,lParam1,lParam2));
  }
#if 0
  if (whic->driverproc) {
    dprintf(("(0x%08lx,0x%08lx,0x%08lx,0x%08lx), calling %p\n",(DWORD)hic,(DWORD)msg,lParam1,lParam2,whic->driverproc));
    ret = whic->driverproc(whic->hdrv,1,msg,lParam1,lParam2);
  } else
#endif
    ret = SendDriverMessage(whic->hdrv,msg,lParam1,lParam2);
  TRACE(" -> 0x%08lx\n",ret);
  return ret;
}

DWORD VFWAPIV ICDrawBegin(
        HIC     hic,
        DWORD     dwFlags,/* flags */
        HPALETTE    hpal, /* palette to draw with */
        HWND      hwnd, /* window to draw to */
        HDC     hdc,  /* HDC to draw to */
        INT     xDst, /* destination rectangle */
        INT     yDst,
        INT     dxDst,
        INT     dyDst,
        LPBITMAPINFOHEADER  lpbi, /* format of frame to draw */
        INT     xSrc, /* source rectangle */
        INT     ySrc,
        INT     dxSrc,
        INT     dySrc,
        DWORD     dwRate, /* frames/second = (dwRate/dwScale) */
        DWORD     dwScale) {
  ICDRAWBEGIN icdb;

  icdb.dwFlags = dwFlags;
  icdb.hpal = hpal;
  icdb.hwnd = hwnd;
  icdb.hdc = hdc;
  icdb.xDst = xDst;
  icdb.yDst = yDst;
  icdb.dxDst = dxDst;
  icdb.dyDst = dyDst;
  icdb.lpbi = lpbi;
  icdb.xSrc = xSrc;
  icdb.ySrc = ySrc;
  icdb.dxSrc = dxSrc;
  icdb.dySrc = dySrc;
  icdb.dwRate = dwRate;
  icdb.dwScale = dwScale;
  return ICSendMessage(hic,ICM_DRAW_BEGIN,(LPARAM)&icdb,sizeof(icdb));
}

DWORD VFWAPIV ICDraw(
  HIC hic,DWORD dwFlags,LPVOID lpFormat,LPVOID lpData,DWORD cbData,
  LONG lTime
) {
  ICDRAW  icd;

  icd.dwFlags = dwFlags;
  icd.lpFormat = lpFormat;
  icd.lpData = lpData;
  icd.cbData = cbData;
  icd.lTime = lTime;
  return ICSendMessage(hic,ICM_DRAW,(LPARAM)&icd,sizeof(icd));
}

LRESULT WINAPI ICClose(HIC hic) {
  WINE_HIC  *whic = (WINE_HIC*)hic;
  TRACE("(%d).\n",hic);
  /* FIXME: correct? */
  CloseDriver(whic->hdrv,0,0);
  HeapFree(GetProcessHeap(),0,whic);
  return 0;
}

HANDLE /* HDRAWDIB */ WINAPI
DrawDibOpen( void ) {
  dprintf(("stub!\n"));
  return 0xdead;
}

BOOL WINAPI
DrawDibClose( HANDLE /*HDRAWDIB*/ hDib ) {
       dprintf(("stub!\n"));
       return TRUE;
}

BOOL VFWAPI DrawDibBegin(HANDLE /*HDRAWDIB*/ hdd,
                                    HDC      hdc,
                                    INT      dxDst,
                                    INT      dyDst,
                                    LPBITMAPINFOHEADER lpbi,
                                    INT      dxSrc,
                                    INT      dySrc,
                                    UINT     wFlags) {
  dprintf(("(%d,0x%lx,%d,%d,%p,%d,%d,0x%08lx), stub!\n",
    hdd,(DWORD)hdc,dxDst,dyDst,lpbi,dxSrc,dySrc,(DWORD)wFlags
  ));
  return TRUE;
}


BOOL VFWAPI
DrawDibSetPalette(HANDLE /*HDRAWDIB*/ hdd, HPALETTE hpal) {
  dprintf(("(%d,%d),stub!\n",hdd,hpal));
  return TRUE;
}

UINT VFWAPI DrawDibRealize(HANDLE /*HDRAWDIB*/ hdd, HDC hdc, BOOL fBackground) {
  dprintf(("(0x%08lx,0x%08lx,%d),stub!\n",(DWORD)hdd,(DWORD)hdc,fBackground));
  return 0;
}


HWND VFWAPIV MCIWndCreate (HWND hwndParent, HINSTANCE hInstance,
                      DWORD dwStyle,LPVOID szFile)
{ dprintf(("%x %x %lx %p\n",hwndParent, hInstance, dwStyle, szFile));
  return 0;
}
HWND VFWAPIV MCIWndCreateA(HWND hwndParent, HINSTANCE hInstance,
                      DWORD dwStyle,LPCSTR szFile)
{ dprintf(("%x %x %lx %s\n",hwndParent, hInstance, dwStyle, szFile));
  return 0;
}
HWND VFWAPIV MCIWndCreateW(HWND hwndParent, HINSTANCE hInstance,
                      DWORD dwStyle,LPCWSTR szFile)
{ dprintf(("%x %x %lx %s\n",hwndParent, hInstance, dwStyle, debugstr_w(szFile)));
  return 0;
}

/* Stubs not included in Wine-Code*/


/****************************************************************************
 *    DrawDibChangePalette    [MSVFW.4]
 */

BOOL VFWAPI DrawDibChangePalette(HANDLE /*HDRAWDIB*/ hdd, int iStart, int iLen, LPPALETTEENTRY lppe)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibChangePalette not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    DrawDibDraw     [MSVFW.6]
 */
BOOL VFWAPI DrawDibDraw(HANDLE /*HDRAWDIB*/ hdd,
                                   HDC      hdc,
                                   int      xDst,
                                   int      yDst,
                                   int      dxDst,
                                   int      dyDst,
                                   LPBITMAPINFOHEADER lpbi,
                                   LPVOID   lpBits,
                                   int      xSrc,
                                   int      ySrc,
                                   int      dxSrc,
                                   int      dySrc,
                                   UINT     wFlags)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibDraw not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *    DrawDibEnd      [MSVFW.7]
 */
BOOL VFWAPI DrawDibEnd(HANDLE /*HDRAWDIB*/ hdd)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibEnd not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *    DrawDibGetBuffer    [MSVFW.8]
 */
LPVOID VFWAPI DrawDibGetBuffer(HANDLE /*HDRAWDIB*/ hdd, LPBITMAPINFOHEADER lpbi, DWORD dwSize, DWORD dwFlags)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibGetBuffer not implemented\n"));
#endif
       return 0;
}


/****************************************************************************
 *    DrawDibGetPalette   [MSVFW.9]
 */
HPALETTE VFWAPI DrawDibGetPalette(HANDLE /*HDRAWDIB*/ hdd)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibGetPalette not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *    DrawDibProfileDisplay   [MSVFW.11]
 */
DWORD VFWAPI DrawDibProfileDisplay(LPBITMAPINFOHEADER lpbi)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibProfileDisplay not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *    DrawDibStart      [MSVFW.14]
 */
BOOL VFWAPI DrawDibStart(HANDLE /*HDRAWDIB*/ hdd, DWORD rate)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibStart not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    DrawDibStop     [MSVFW.15]
 */
BOOL VFWAPI DrawDibStop(HANDLE /*HDRAWDIB*/ hdd)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibStop not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    DrawDibTime     [MSVFW.16]
 */
BOOL VFWAPI DrawDibTime(HANDLE /*HDRAWDIB*/ hdd, DWORD lpddtime)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibTime not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    GetOpenFileNamePreview    [MSVFW.17]
 */

/* NO */


/****************************************************************************
 *    GetOpenFileNamePreviewA   [MSVFW.18]
 */
BOOL VFWAPI GetOpenFileNamePreviewA(LPOPENFILENAMEA lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetOpenFileNamePreviewA not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    GetOpenFileNamePreviewW   [MSVFW.19]
 */
BOOL VFWAPI GetOpenFileNamePreviewW(LPOPENFILENAMEW lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetOpenFileNamePreviewW not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    GetSaveFileNamePreviewA   [MSVFW.20]
 */
BOOL VFWAPI GetSaveFileNamePreviewA(LPOPENFILENAMEA lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetSaveFileNamePreviewA not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    GetSaveFileNamePreviewW   [MSVFW.21]
 */
BOOL VFWAPI GetSaveFileNamePreviewW(LPOPENFILENAMEW lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetSaveFileNamePreviewW not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    ICCompressorChoose    [MSVFW.24]
 */
BOOL VFWAPI ICCompressorChoose(
        HWND        hwnd,               // parent window for dialog
        UINT        uiFlags,            // flags
        LPVOID      pvIn,               // input format (optional)
        LPVOID      lpData,             // input data (optional)
        PCOMPVARS   pc,                 // data about the compressor/dlg
        LPSTR       lpszTitle)          // dialog title (optional)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICCompressorChoose not implemented\n"));
#endif
  return TRUE;
}


/****************************************************************************
 *    ICCompressorFree    [MSVFW.25]
 */
void VFWAPI ICCompressorFree(PCOMPVARS pc)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICCompressorFree not implemented\n"));
#endif
  return;
}


/****************************************************************************
 *    ICImageCompress     [MSVFW.31]
 */
HANDLE VFWAPI ICImageCompress(
        HIC                 hic,        // compressor to use
        UINT                uiFlags,    // flags (none yet)
        LPBITMAPINFO      lpbiIn,     // format to compress from
        LPVOID              lpBits,     // data to compress
        LPBITMAPINFO        lpbiOut,    // compress to this (NULL ==> default)
        LONG                lQuality,   // quality to use
        LONG  *             plSize)     // compress to this size (0=whatever)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICImageCompress not implemented\n"));
#endif
  return 0;
}


/****************************************************************************
 *    ICImageDecompress   [MSVFW.32]
 */
HANDLE VFWAPI ICImageDecompress(
        HIC                 hic,        // compressor to use
        UINT                uiFlags,    // flags (none yet)
        LPBITMAPINFO        lpbiIn,     // format to decompress from
        LPVOID              lpBits,     // data to decompress
        LPBITMAPINFO        lpbiOut)    // decompress to this (NULL ==> default)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICImageDecompress not implemented\n"));
#endif
  return 0;
}


/****************************************************************************
 *    ICInstall     [MSVFW.34]
 */
BOOL    VFWAPI ICInstall(DWORD fccType, DWORD fccHandler, LPARAM lParam, LPSTR szDesc, UINT wFlags)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICInstall not implemented\n"));
#endif
  return TRUE;
}


/**************************************************************************
 *    ICRemove      [MSVFW.39]
 */
BOOL    VFWAPI ICRemove(DWORD fccType, DWORD fccHandler, UINT wFlags)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICRemove not implemented\n"));
#endif
  return TRUE;
}


/**************************************************************************
 *    ICSeqCompressFrame    [MSVFW.41]
 */
LPVOID VFWAPI ICSeqCompressFrame(
    PCOMPVARS               pc,         // set by ICCompressorChoose
    UINT                    uiFlags,    // flags
    LPVOID                  lpBits,     // input DIB bits
    BOOL        *pfKey, // did it end up being a key frame?
    LONG        *plSize)  // size to compress to/of returned image
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICSeqCompressFrame not implemented\n"));
#endif
  return 0;
}


/**************************************************************************
 *    ICSeqCompressFrameEnd   [MSVFW.42]
 */
BOOL VFWAPI ICSeqCompressFrameStart(PCOMPVARS pc, LPBITMAPINFO lpbiIn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICSeqCompressFrameEnd not implemented\n"));
#endif
  return TRUE;
}


/**************************************************************************
 *    ICSeqCompressFrameStart   [MSVFW.43]
 */
void VFWAPI ICSeqCompressFrameEnd(PCOMPVARS pc)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICSeqCompressFrameStart not implemented\n"));
#endif
  return;
}


/**************************************************************************
 *    MCIWndRegisterClass   [MSVFW.47]
 */
BOOL VFWAPIV MCIWndRegisterClass()
{
#ifdef DEBUG
    dprintf(("MSVFW32: MCIWndRegisterClass not implemented\n"));
#endif
  return TRUE;
}
