/* $Id: msvfw32.cpp,v 1.3 1999-09-23 09:38:05 sandervl Exp $ */
/*
 * Copyright 1998 Marcus Meissner
 * Copyright 1999 Jens Wiessner
 */

#include <stdlib.h>
#include <string.h>
#include <odin.h>

#define ICOM_CINTERFACE 1
#define strcasecmp               strcmp 

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
#include "driver.h"
#include "msvfw32.h"

#include "debugtools.h"
#include <debugstr.h>


DEFAULT_DEBUG_CHANNEL(msvideo)


/****************************************************************************
 *		VideoForWindowsVersion		[MSVFW.2]
 * Returns the version in major.minor form.
 * In Windows95 this returns 0x040003b6 (4.950)
 */
DWORD WINAPI VideoForWindowsVersion(void)
{
#ifdef DEBUG
    dprintf(("MSVFW32: VideoForWindowsVersion\n"));
#endif
	return 0x040003B6; /* 4.950 */
}


/****************************************************************************
 *		DrawDibBegin			[MSVFW.3]
 */
BOOL VFWAPI DrawDibBegin(HANDLE /*HDRAWDIB*/ hdd,
                                    HDC      hdc,
                                    INT      dxDst,
                                    INT      dyDst,
                                    LPBITMAPINFOHEADER lpbi,
                                    INT      dxSrc,
                                    INT      dySrc,
                                    UINT     wFlags)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibBegin not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		DrawDibChangePalette		[MSVFW.4]
 */

BOOL VFWAPI DrawDibChangePalette(HANDLE /*HDRAWDIB*/ hdd, int iStart, int iLen, LPPALETTEENTRY lppe)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibChangePalette not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		DrawDibClose			[MSVFW.5]
 */
BOOL WINAPI DrawDibClose( HANDLE /*HDRAWDIB*/ hDib )
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibClose not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *		DrawDibDraw			[MSVFW.6]
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
 *		DrawDibEnd			[MSVFW.7]
 */
BOOL VFWAPI DrawDibEnd(HANDLE /*HDRAWDIB*/ hdd)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibEnd not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *		DrawDibGetBuffer		[MSVFW.8]
 */
LPVOID VFWAPI DrawDibGetBuffer(HANDLE /*HDRAWDIB*/ hdd, LPBITMAPINFOHEADER lpbi, DWORD dwSize, DWORD dwFlags)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibGetBuffer not implemented\n"));
#endif
       return 0;
}


/****************************************************************************
 *		DrawDibGetPalette		[MSVFW.9]
 */
HPALETTE VFWAPI DrawDibGetPalette(HANDLE /*HDRAWDIB*/ hdd)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibGetPalette not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *		DrawDibOpen			[MSVFW.10]
 */
HANDLE /* HDRAWDIB */ WINAPI DrawDibOpen( void ) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibOpen not implemented\n"));
#endif
	return 0xdead;
}


/****************************************************************************
 *		DrawDibProfileDisplay		[MSVFW.11]
 */
DWORD VFWAPI DrawDibProfileDisplay(LPBITMAPINFOHEADER lpbi)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibProfileDisplay not implemented\n"));
#endif
       return TRUE;
}


/****************************************************************************
 *		DrawDibRealize			[MSVFW.12]
 */
UINT VFWAPI DrawDibRealize(HANDLE /*HDRAWDIB*/ hdd, HDC hdc, BOOL fBackground)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibRealize not implemented\n"));
#endif
	return 0;
}


/****************************************************************************
 *		DrawDibSetPalette		[MSVFW.13]
 */
BOOL VFWAPI DrawDibSetPalette(HANDLE /*HDRAWDIB*/ hdd, HPALETTE hpal) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibSetPalette not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		DrawDibStart			[MSVFW.14]
 */
BOOL VFWAPI DrawDibStart(HANDLE /*HDRAWDIB*/ hdd, DWORD rate)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibStart not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		DrawDibStop			[MSVFW.15]
 */
BOOL VFWAPI DrawDibStop(HANDLE /*HDRAWDIB*/ hdd)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibStop not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		DrawDibTime			[MSVFW.16]
 */
BOOL VFWAPI DrawDibTime(HANDLE /*HDRAWDIB*/ hdd, LPDRAWDIBTIME lpddtime)
{
#ifdef DEBUG
    dprintf(("MSVFW32: DrawDibTime not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		GetOpenFileNamePreview		[MSVFW.17]
 */

/* NO */


/****************************************************************************
 *		GetOpenFileNamePreviewA		[MSVFW.18]
 */
BOOL VFWAPI GetOpenFileNamePreviewA(LPOPENFILENAMEA lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetOpenFileNamePreviewA not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		GetOpenFileNamePreviewW		[MSVFW.19]
 */
BOOL VFWAPI GetOpenFileNamePreviewW(LPOPENFILENAMEW lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetOpenFileNamePreviewW not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		GetSaveFileNamePreviewA		[MSVFW.20]
 */
BOOL VFWAPI GetSaveFileNamePreviewA(LPOPENFILENAMEA lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetSaveFileNamePreviewA not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		GetSaveFileNamePreviewW		[MSVFW.21]
 */
BOOL VFWAPI GetSaveFileNamePreviewW(LPOPENFILENAMEW lpofn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: GetSaveFileNamePreviewW not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		ICClose				[MSVFW.22]
 */
LRESULT WINAPI ICClose(HIC hic) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICClose\n"));
#endif
	WINE_HIC	*whic = (WINE_HIC*)hic;
	/* FIXME: correct? */
	CloseDriver(whic->hdrv,0,0);
	HeapFree(GetProcessHeap(),0,whic);
	return 0;
}


/****************************************************************************
 *		ICCompress			[MSVFW.23]
 */
DWORD VFWAPIV ICCompress(
	HIC hic,DWORD dwFlags,LPBITMAPINFOHEADER lpbiOutput,LPVOID lpData,
	LPBITMAPINFOHEADER lpbiInput,LPVOID lpBits,LPDWORD lpckid,
	LPDWORD lpdwFlags,LONG lFrameNum,DWORD dwFrameSize,DWORD dwQuality,
	LPBITMAPINFOHEADER lpbiPrev,LPVOID lpPrev) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICCompress\n"));
#endif
	ICCOMPRESS	iccmp;

	iccmp.dwFlags		= dwFlags;

	iccmp.lpbiOutput	= lpbiOutput;
	iccmp.lpOutput		= lpData;
	iccmp.lpbiInput		= lpbiInput;
	iccmp.lpInput		= lpBits;

	iccmp.lpckid		= lpckid;
	iccmp.lpdwFlags		= lpdwFlags;
	iccmp.lFrameNum		= lFrameNum;
	iccmp.dwFrameSize	= dwFrameSize;
	iccmp.dwQuality		= dwQuality;
	iccmp.lpbiPrev		= lpbiPrev;
	iccmp.lpPrev		= lpPrev;
	return ICSendMessage(hic,ICM_COMPRESS,(LPARAM)&iccmp,sizeof(iccmp));
}


/****************************************************************************
 *		ICCompressorChoose		[MSVFW.24]
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
 *		ICCompressorFree		[MSVFW.25]
 */
void VFWAPI ICCompressorFree(PCOMPVARS pc)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICCompressorFree not implemented\n"));
#endif
	return;
}


/****************************************************************************
 *		ICDecompress			[MSVFW.26]
 */
DWORD VFWAPIV ICDecompress(HIC hic,DWORD dwFlags,LPBITMAPINFOHEADER lpbiFormat,
		LPVOID lpData,LPBITMAPINFOHEADER  lpbi,LPVOID lpBits) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICDecompress\n"));
#endif
	ICDECOMPRESS	icd;

	icd.dwFlags	= dwFlags;
	icd.lpbiInput	= lpbiFormat;
	icd.lpInput	= lpData;

	icd.lpbiOutput	= lpbi;
	icd.lpOutput	= lpBits;
	icd.ckid	= 0;
	return ICSendMessage(hic,ICM_DECOMPRESS,(LPARAM)&icd,sizeof(icd));
}


/****************************************************************************
 *		ICDraw				[MSVFW.27]
 */
DWORD VFWAPIV ICDraw(HIC hic,DWORD dwFlags,LPVOID lpFormat,LPVOID lpData,
	DWORD cbData, LONG lTime) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICDraw\n"));
#endif
	ICDRAW	icd;

	icd.dwFlags = dwFlags;
	icd.lpFormat = lpFormat;
	icd.lpData = lpData;
	icd.cbData = cbData;
	icd.lTime = lTime;
	return ICSendMessage(hic,ICM_DRAW,(LPARAM)&icd,sizeof(icd));
}


/****************************************************************************
 *		ICDrawBegin			[MSVFW.28]
 */
DWORD	VFWAPIV	ICDrawBegin(
        HIC			hic,
        DWORD			dwFlags,/* flags */
        HPALETTE		hpal,	/* palette to draw with */
        HWND			hwnd,	/* window to draw to */
        HDC			hdc,	/* HDC to draw to */
        INT			xDst,	/* destination rectangle */
        INT			yDst,
        INT			dxDst,
        INT			dyDst,
        LPBITMAPINFOHEADER	lpbi,	/* format of frame to draw */
        INT			xSrc,	/* source rectangle */
        INT			ySrc,
        INT			dxSrc,
        INT			dySrc,
        DWORD			dwRate,	/* frames/second = (dwRate/dwScale) */
        DWORD			dwScale) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICDrawBegin\n"));
#endif
	ICDRAWBEGIN	icdb;

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


/****************************************************************************
 *		ICGetDisplayFormat		[MSVFW.29]
 */
HIC VFWAPI ICGetDisplayFormat(HIC hic,LPBITMAPINFOHEADER lpbiIn,
	LPBITMAPINFOHEADER lpbiOut, INT depth,INT dx,INT dy) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICGetDisplayFormat\n"));
#endif
	HIC	tmphic = hic; 
	LRESULT	lres;

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
		HDC	hdc;

		hdc = GetDC(0);
		depth = GetDeviceCaps(hdc,12)*GetDeviceCaps(hdc,14);
		ReleaseDC(0,hdc);
		if (depth==15)	depth = 16;
		if (depth<8)	depth =  8;
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


/****************************************************************************
 *		ICGetInfo			[MSVFW.30]
 */
LRESULT WINAPI ICGetInfo(HIC hic,ICINFO *picinfo,DWORD cb) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICGetInfo\n"));
#endif
	LRESULT		ret;

	ret = ICSendMessage(hic,ICM_GETINFO,(DWORD)picinfo,cb);
	return ret;
}


/****************************************************************************
 *		ICImageCompress			[MSVFW.31]
 */
HANDLE VFWAPI ICImageCompress(
        HIC                 hic,        // compressor to use
        UINT                uiFlags,    // flags (none yet)
        LPBITMAPINFO	    lpbiIn,     // format to compress from
        LPVOID              lpBits,     // data to compress
        LPBITMAPINFO        lpbiOut,    // compress to this (NULL ==> default)
        LONG                lQuality,   // quality to use
        LONG                plSize)     // compress to this size (0=whatever)
{	
#ifdef DEBUG
    dprintf(("MSVFW32: ICImageCompress not implemented\n"));
#endif
	return 0;
}


/****************************************************************************
 *		ICImageDecompress		[MSVFW.32]
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


/**************************************************************************
 *		ICInfo				[MSVFW.33]
 * Get information about an installable compressor. Return TRUE if there
 * is one.
 */
BOOL WINAPI ICInfo(DWORD fccType,DWORD fccHandler,ICINFO *lpicinfo)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICInfo\n"));
#endif

	char	type[5],buf[2000];

	memcpy(type,&fccType,4);type[4]=0;
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


/****************************************************************************
 *		ICInstall			[MSVFW.34]
 */
BOOL    VFWAPI ICInstall(DWORD fccType, DWORD fccHandler, LPARAM lParam, LPSTR szDesc, UINT wFlags)
{	
#ifdef DEBUG
    dprintf(("MSVFW32: ICInstall not implemented\n"));
#endif
	return TRUE;
}


/****************************************************************************
 *		ICLocate			[MSVFW.35]
 */
HIC  VFWAPI ICLocate(DWORD fccType, DWORD fccHandler, LPBITMAPINFOHEADER lpbiIn,
	LPBITMAPINFOHEADER lpbiOut, WORD wMode) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICLocate\n"));
#endif
	char	type[5],handler[5];
	HIC	hic;
	DWORD	querymsg;

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
	return 0;
}


/**************************************************************************
 *		ICOpen				[MSVFW.37]
 * Opens an installable compressor. Return special handle.
 */
HIC WINAPI ICOpen(DWORD fccType,DWORD fccHandler,UINT wMode) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICOpen\n"));
#endif
	char		type[5],handler[5],codecname[20];
	ICOPEN		icopen;
	HDRVR		hdrv;
	WINE_HIC	*whic;

	memcpy(type,&fccType,4);type[4]=0;
	memcpy(handler,&fccHandler,4);handler[4]=0;
	sprintf(codecname,"%s.%s",type,handler);

	/* Well, lParam2 is in fact a LPVIDEO_OPEN_PARMS, but it has the 
	 * same layout as ICOPEN
	 */
	icopen.fccType		= fccType;
	icopen.fccHandler	= fccHandler;
	icopen.dwSize		= sizeof(ICOPEN);
	icopen.dwFlags		= wMode;
	/* FIXME: do we need to fill out the rest too? */
	hdrv=OpenDriverA(codecname,"drivers32",(LPARAM)&icopen);
	if (!hdrv) {
	    if (!strcasecmp(type,"vids")) {
		sprintf(codecname,"vidc.%s",handler);
		fccType = mmioFOURCC('v','i','d','c');
	    }
	    hdrv=OpenDriverA(codecname,"drivers32",(LPARAM)&icopen);
	    if (!hdrv)
		    return 0;
	}
	whic = (PWINE_HIC)HeapAlloc(GetProcessHeap(),0,sizeof(WINE_HIC));
	whic->hdrv	= hdrv;
	whic->driverproc= NULL;
	whic->privatevfw= ICSendMessage((HIC)whic,DRV_OPEN,0,(LPARAM)&icopen);
	return (HIC)whic;
}


/**************************************************************************
 *		ICOpenFunction			[MSVFW.38]
 */
HIC VFWAPI ICOpenFunction(DWORD fccType, DWORD fccHandler, UINT wMode,
FARPROC lpfnHandler) 
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICOpenFunction\n"));
#endif
	char		type[5],handler[5];
	HIC		hic;
	WINE_HIC	*whic;

	memcpy(type,&fccType,4);type[4]=0;
	memcpy(handler,&fccHandler,4);handler[4]=0;
	hic = ICOpen(fccType,fccHandler,wMode);
	if (!hic)
		return hic;
	whic = (WINE_HIC*)hic;
	whic->driverproc = lpfnHandler;
	return hic;
}


/**************************************************************************
 *		ICRemove			[MSVFW.39]
 */
BOOL    VFWAPI ICRemove(DWORD fccType, DWORD fccHandler, UINT wFlags)
{	
#ifdef DEBUG
    dprintf(("MSVFW32: ICRemove not implemented\n"));
#endif
	return TRUE;
}


/**************************************************************************
 *		ICSendMessage			[MSVFW.40]
 */
LRESULT VFWAPI ICSendMessage(HIC hic,UINT msg,DWORD lParam1,DWORD lParam2) 

{
#ifdef DEBUG
    dprintf(("MSVFW32: ICSendMessage\n"));
#endif
	LRESULT		ret;
	WINE_HIC	*whic = (WINE_HIC*)hic;
	INT		tvfw;

#define XX(x) case x:

	switch (msg)
        {

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
	  tvfw = 0; 
	}
#if 0
	if (whic->driverproc) {
		ret = whic->driverproc(whic->hdrv,1,msg,lParam1,lParam2);
	} else
#endif
	ret = SendDriverMessage(whic->hdrv,msg,lParam1,lParam2);
	return ret;
}


/**************************************************************************
 *		ICSeqCompressFrame		[MSVFW.41]
 */
LPVOID VFWAPI ICSeqCompressFrame(
    PCOMPVARS               pc,         // set by ICCompressorChoose
    UINT                    uiFlags,    // flags
    LPVOID                  lpBits,     // input DIB bits
    BOOL 		    *pfKey,	// did it end up being a key frame?
    LONG		    *plSize) 	// size to compress to/of returned image
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICSeqCompressFrame not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *		ICSeqCompressFrameEnd		[MSVFW.42]
 */
BOOL VFWAPI ICSeqCompressFrameStart(PCOMPVARS pc, LPBITMAPINFO lpbiIn)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICSeqCompressFrameEnd not implemented\n"));
#endif
	return TRUE;
}


/**************************************************************************
 *		ICSeqCompressFrameStart		[MSVFW.43]
 */
void VFWAPI ICSeqCompressFrameEnd(PCOMPVARS pc)
{
#ifdef DEBUG
    dprintf(("MSVFW32: ICSeqCompressFrameStart not implemented\n"));
#endif
	return;
}


/**************************************************************************
 *		MCIWndCreate			[MSVFW.44]
 */
HWND VFWAPIV MCIWndCreate (HWND hwndParent, HINSTANCE hInstance,
                      DWORD dwStyle,LPVOID szFile)
{	
#ifdef DEBUG
    dprintf(("MSVFW32: MCIWndCreate not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *		MCIWndCreateA			[MSVFW.45]
 */
HWND VFWAPIV MCIWndCreateA(HWND hwndParent, HINSTANCE hInstance,
                      DWORD dwStyle,LPCSTR szFile)
{	
#ifdef DEBUG
    dprintf(("MSVFW32: MCIWndCreateA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *		MCIWndCreateW			[MSVFW.46]
 */
HWND VFWAPIV MCIWndCreateW(HWND hwndParent, HINSTANCE hInstance,
                      DWORD dwStyle,LPCWSTR szFile)
{	
#ifdef DEBUG
    dprintf(("MSVFW32: MCIWndCreateW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *		MCIWndRegisterClass		[MSVFW.47]
 */
BOOL VFWAPIV MCIWndRegisterClass()
{
#ifdef DEBUG
    dprintf(("MSVFW32: MCIWndRegisterClass not implemented\n"));
#endif
	return TRUE;
}


/**************************************************************************
 *		StretchDIB			[MSVFW.48]
 */

/* NO */
