/* $Id: animate.cpp,v 1.3 2000-05-22 17:25:06 cbratschi Exp $ */
/*
 * Animation control
 *
 * Copyright 1998, 1999 Eric Kohl
 *                 1999 Eric Pouech
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * TODO:
 *   - check for the 'rec ' list in some AVI files
 *   - implement some missing flags (ACS_TRANSPARENT and ACS_CENTER)
 *   - protection between service thread and wndproc messages handling
 *     concurrent access to infoPtr
 *
 * Status: complete (read above)
 * Version: 5.00
 */

/*
 - Corel WINE 20000513 level
*/

#include "winbase.h"
#include "commctrl.h"
#include "comctl32.h"
#include "driver.h"
#include "mmsystem.h"
#include "ccbase.h"
#include "animate.h"

HINSTANCE hMSVFW32 = 0;

HINSTANCE hWINMM = 0;
static LONG     (*WINAPI WINMM_mmioRead)(HMMIO,HPSTR,LONG) = 0;
static LONG     (*WINAPI WINMM_mmioSeek)(HMMIO,LONG,INT) = 0;
static MMRESULT (*WINAPI WINMM_mmioClose)(HMMIO,UINT) = 0;
static HMMIO    (*WINAPI WINMM_mmioOpenA)(LPSTR ,MMIOINFO*,DWORD) = 0;
static HMMIO    (*WINAPI WINMM_mmioOpenW)(LPWSTR,MMIOINFO*,DWORD) = 0;
static UINT     (*WINAPI WINMM_mmioDescend)(HMMIO,MMCKINFO*,const MMCKINFO*,UINT) = 0;
static UINT     (*WINAPI WINMM_mmioAscend)(HMMIO,MMCKINFO*,UINT) = 0;

#define ANIMATE_GetInfoPtr(hwnd) ((ANIMATE_INFO*)getInfoPtr(hwnd))

static BOOL ANIMATE_LoadMSVFW32(VOID)
{
  if (hMSVFW32) return TRUE;

  hMSVFW32 = LoadLibraryA("msvfw32.dll");

  return hMSVFW32;
}

static VOID ANIMATE_UnloadMSVFW32(VOID)
{
  if (hMSVFW32)
  {
    FreeLibrary(hMSVFW32);
    hMSVFW32 = 0;
  }
}

static BOOL ANIMATE_LoadWINMM(VOID)
{
  if (hWINMM) return TRUE;

  hWINMM = LoadLibraryA("WINMM");
  if (!hWINMM) return FALSE;

  *(VOID**)&WINMM_mmioRead    = (VOID*)GetProcAddress(hWINMM,"mmioRead");
  *(VOID**)&WINMM_mmioSeek    = (VOID*)GetProcAddress(hWINMM,"mmioSeek");
  *(VOID**)&WINMM_mmioClose   = (VOID*)GetProcAddress(hWINMM,"mmioClose");
  *(VOID**)&WINMM_mmioOpenA   = (VOID*)GetProcAddress(hWINMM,"mmioOpenA");
  *(VOID**)&WINMM_mmioOpenW   = (VOID*)GetProcAddress(hWINMM,"mmioOpenW");
  *(VOID**)&WINMM_mmioDescend = (VOID*)GetProcAddress(hWINMM,"mmioDescend");
  *(VOID**)&WINMM_mmioAscend  = (VOID*)GetProcAddress(hWINMM,"mmioAscend");

  return TRUE;
}

static VOID ANIMATE_UnloadWINMM(VOID)
{
  if (hWINMM)
  {
    FreeLibrary(hWINMM);
    hWINMM = 0;
    *(VOID**)&WINMM_mmioRead    = NULL;
    *(VOID**)&WINMM_mmioSeek    = NULL;
    *(VOID**)&WINMM_mmioClose   = NULL;
    *(VOID**)&WINMM_mmioOpenA   = NULL;
    *(VOID**)&WINMM_mmioOpenW   = NULL;
    *(VOID**)&WINMM_mmioDescend = NULL;
    *(VOID**)&WINMM_mmioAscend  = NULL;
  }
}

static BOOL ANIMATE_LoadRes(ANIMATE_INFO *infoPtr,HINSTANCE hInst,LPWSTR lpName,BOOL unicode)
{
    HRSRC hrsrc;
    MMIOINFO    mminfo;
    LPVOID lpAvi;

    if (unicode)
      hrsrc = FindResourceW(hInst,lpName,(LPWSTR)L"AVI");
    else
      hrsrc = FindResourceA(hInst,(LPCSTR)lpName,"AVI");
    if (!hrsrc) return FALSE;

    infoPtr->hRes = LoadResource(hInst, hrsrc);
    if (!infoPtr->hRes)
        return FALSE;

    lpAvi = LockResource (infoPtr->hRes);
    if (!lpAvi) return FALSE;

    memset(&mminfo, 0, sizeof(mminfo));
    mminfo.fccIOProc = FOURCC_MEM;
    mminfo.pchBuffer = (LPSTR)lpAvi;
    mminfo.cchBuffer = SizeofResource(hInst, hrsrc);
    infoPtr->hMMio = WINMM_mmioOpenA(NULL, &mminfo, MMIO_READ);

    if (!infoPtr->hMMio) {
        GlobalFree((HGLOBAL)lpAvi);
        return FALSE;
    }

    return TRUE;
}

static BOOL ANIMATE_LoadFile(ANIMATE_INFO *infoPtr,LPWSTR lpName,BOOL unicode)
{
    if (unicode)
      infoPtr->hMMio = WINMM_mmioOpenW(lpName,NULL,MMIO_ALLOCBUF | MMIO_READ | MMIO_DENYWRITE);
    else
      infoPtr->hMMio = WINMM_mmioOpenA((LPSTR)lpName,NULL,MMIO_ALLOCBUF | MMIO_READ | MMIO_DENYWRITE);

    if (!infoPtr->hMMio)
        return FALSE;

    return TRUE;
}

static LRESULT ANIMATE_DoStop(ANIMATE_INFO *infoPtr)
{
    EnterCriticalSection(&infoPtr->cs);

    /* should stop playing */
    if (infoPtr->hThread)
    {
      infoPtr->stopThread = TRUE;
      infoPtr->hThread = 0;
    }
    if (infoPtr->uTimer) {
        KillTimer(infoPtr->hWnd, infoPtr->uTimer);
        infoPtr->uTimer = 0;
    }

    LeaveCriticalSection(&infoPtr->cs);

    sendCommand(infoPtr->hWnd,ACN_STOP);

    return TRUE;
}


static void ANIMATE_Free(ANIMATE_INFO *infoPtr)
{
    if (infoPtr->hMMio) {
        ANIMATE_DoStop(infoPtr);
        WINMM_mmioClose(infoPtr->hMMio, 0);
        if (infoPtr->hRes) {
            FreeResource(infoPtr->hRes);
            infoPtr->hRes = 0;
        }
        if (infoPtr->lpIndex) {
            HeapFree(GetProcessHeap(), 0, infoPtr->lpIndex);
            infoPtr->lpIndex = NULL;
        }
        if (infoPtr->hic) {
            (infoPtr->fnICClose)(infoPtr->hic);
            infoPtr->hic = 0;
        }
        if (infoPtr->inbih) {
            HeapFree(GetProcessHeap(), 0, infoPtr->inbih);
            infoPtr->inbih = NULL;
        }
        if (infoPtr->outbih) {
            HeapFree(GetProcessHeap(), 0, infoPtr->outbih);
            infoPtr->outbih = NULL;
        }
        HeapFree(GetProcessHeap(), 0, infoPtr->indata);
        HeapFree(GetProcessHeap(), 0, infoPtr->outdata);
        infoPtr->indata = infoPtr->outdata = NULL;
        infoPtr->hWnd = 0;
        infoPtr->hMMio = 0;
        memset(&infoPtr->mah, 0, sizeof(infoPtr->mah));
        memset(&infoPtr->ash, 0, sizeof(infoPtr->ash));
        infoPtr->nFromFrame = infoPtr->nToFrame = infoPtr->nLoop = infoPtr->currFrame = 0;
    }
}


static LRESULT ANIMATE_PaintFrame(ANIMATE_INFO* infoPtr, HDC hDC)
{
    if (!hDC || !infoPtr->inbih)
        return TRUE;
    if (infoPtr->hic)
        StretchDIBits(hDC, 0, 0, infoPtr->outbih->biWidth, infoPtr->outbih->biHeight,
                      0, 0, infoPtr->outbih->biWidth, infoPtr->outbih->biHeight,
                      infoPtr->outdata, (LPBITMAPINFO)infoPtr->outbih, DIB_RGB_COLORS,
                      SRCCOPY);
    else
        StretchDIBits(hDC, 0, 0, infoPtr->inbih->biWidth, infoPtr->inbih->biHeight,
                      0, 0, infoPtr->inbih->biWidth, infoPtr->inbih->biHeight,
                      infoPtr->indata, (LPBITMAPINFO)infoPtr->inbih, DIB_RGB_COLORS,
                      SRCCOPY);

    return TRUE;
}

static LRESULT ANIMATE_DrawFrame(ANIMATE_INFO* infoPtr)
{
    HDC         hDC;

    //TRACE("Drawing frame %d (loop %d)\n", infoPtr->currFrame, infoPtr->nLoop);

    EnterCriticalSection(&infoPtr->cs);

    WINMM_mmioSeek(infoPtr->hMMio, infoPtr->lpIndex[infoPtr->currFrame], SEEK_SET);
    WINMM_mmioRead(infoPtr->hMMio, (HPSTR)infoPtr->indata, infoPtr->ash.dwSuggestedBufferSize);

    if (infoPtr->hic &&
        (infoPtr->fnICDecompress)(infoPtr->hic, 0, infoPtr->inbih, infoPtr->indata,
                                  infoPtr->outbih, infoPtr->outdata) != ICERR_OK) {
        LeaveCriticalSection(&infoPtr->cs);
        //WARN("Decompression error\n");
        return FALSE;
    }

    if ((hDC = GetDC(infoPtr->hWnd)) != 0) {
        ANIMATE_PaintFrame(infoPtr, hDC);
        ReleaseDC(infoPtr->hWnd, hDC);
    }

    if (infoPtr->currFrame++ >= infoPtr->nToFrame) {
        infoPtr->currFrame = infoPtr->nFromFrame;
        if (infoPtr->nLoop != -1) {
            if (--infoPtr->nLoop == 0) {
                ANIMATE_DoStop(infoPtr);
            }
        }
    }
    LeaveCriticalSection(&infoPtr->cs);

    return TRUE;
}

static DWORD ANIMATE_ThreadFunc(LPDWORD lpdwParam)
{
    HWND hwnd = (HWND)lpdwParam;
    ANIMATE_INFO* infoPtr = ANIMATE_GetInfoPtr(hwnd);

    while (IsWindow(hwnd) && !infoPtr->stopThread)
    {
      EnterCriticalSection(&infoPtr->cs);
      ANIMATE_DrawFrame(infoPtr);
      LeaveCriticalSection(&infoPtr->cs);
      Sleep(infoPtr->delay);
    }

    return 0;
}

static LRESULT ANIMATE_Play(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hWnd);

    /* nothing opened */
    if (!infoPtr->hMMio)
        return FALSE;

    if (infoPtr->hThread || infoPtr->uTimer) {
        //FIXME("Already playing ? what should I do ??\n");
        ANIMATE_DoStop(infoPtr);
    }

    infoPtr->nFromFrame = (INT)LOWORD(lParam);
    infoPtr->nToFrame   = (INT)HIWORD(lParam);
    infoPtr->nLoop      = (INT)wParam;

    if (infoPtr->nToFrame == 0xFFFF)
        infoPtr->nToFrame = infoPtr->mah.dwTotalFrames - 1;

    //TRACE("(repeat=%d from=%d to=%d);\n",
    //        infoPtr->nLoop, infoPtr->nFromFrame, infoPtr->nToFrame);

    if (infoPtr->nFromFrame >= infoPtr->nToFrame ||
        infoPtr->nToFrame >= infoPtr->mah.dwTotalFrames)
        return FALSE;

    infoPtr->currFrame = infoPtr->nFromFrame;

    if (GetWindowLongA(hWnd, GWL_STYLE) & ACS_TIMER) {
        //TRACE("Using a timer\n");
        /* create a timer to display AVI */
        infoPtr->uTimer = SetTimer(hWnd, 1, infoPtr->mah.dwMicroSecPerFrame / 1000, NULL);
    } else
    {
      DWORD id;

      //TRACE("Using the service thread\n");
      infoPtr->stopThread = FALSE;
      infoPtr->delay = infoPtr->mah.dwMicroSecPerFrame+500;
      infoPtr->hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ANIMATE_ThreadFunc,(LPVOID)hWnd,0,&id);
    }

    sendCommand(infoPtr->hWnd,ACN_START);

    return TRUE;
}


static BOOL ANIMATE_GetAviInfo(ANIMATE_INFO *infoPtr)
{
    MMCKINFO            ckMainRIFF;
    MMCKINFO            mmckHead;
    MMCKINFO            mmckList;
    MMCKINFO            mmckInfo;
    DWORD               numFrame;
    DWORD               insize;

    if (WINMM_mmioDescend(infoPtr->hMMio, &ckMainRIFF, NULL, 0) != 0) {
        //WARN("Can't find 'RIFF' chunk\n");
        return FALSE;
    }

    if ((ckMainRIFF.ckid != FOURCC_RIFF) ||
        (ckMainRIFF.fccType != mmioFOURCC('A', 'V', 'I', ' '))) {
        //WARN("Can't find 'AVI ' chunk\n");
        return FALSE;
    }

    mmckHead.fccType = mmioFOURCC('h', 'd', 'r', 'l');
    if (WINMM_mmioDescend(infoPtr->hMMio, &mmckHead, &ckMainRIFF, MMIO_FINDLIST) != 0) {
        //WARN("Can't find 'hdrl' list\n");
        return FALSE;
    }

    mmckInfo.ckid = mmioFOURCC('a', 'v', 'i', 'h');
    if (WINMM_mmioDescend(infoPtr->hMMio, &mmckInfo, &mmckHead, MMIO_FINDCHUNK) != 0) {
        //WARN("Can't find 'avih' chunk\n");
        return FALSE;
    }

    WINMM_mmioRead(infoPtr->hMMio, (LPSTR)&infoPtr->mah, sizeof(infoPtr->mah));
    //TRACE("mah.dwMicroSecPerFrame=%ld\n",     infoPtr->mah.dwMicroSecPerFrame);
    //TRACE("mah.dwMaxBytesPerSec=%ld\n",       infoPtr->mah.dwMaxBytesPerSec);
    //TRACE("mah.dwPaddingGranularity=%ld\n",   infoPtr->mah.dwPaddingGranularity);
    //TRACE("mah.dwFlags=%ld\n",                        infoPtr->mah.dwFlags);
    //TRACE("mah.dwTotalFrames=%ld\n",          infoPtr->mah.dwTotalFrames);
    //TRACE("mah.dwInitialFrames=%ld\n",                infoPtr->mah.dwInitialFrames);
    //TRACE("mah.dwStreams=%ld\n",              infoPtr->mah.dwStreams);
    //TRACE("mah.dwSuggestedBufferSize=%ld\n",  infoPtr->mah.dwSuggestedBufferSize);
    //TRACE("mah.dwWidth=%ld\n",                        infoPtr->mah.dwWidth);
    //TRACE("mah.dwHeight=%ld\n",               infoPtr->mah.dwHeight);
    WINMM_mmioAscend(infoPtr->hMMio, &mmckInfo, 0);

    mmckList.fccType = mmioFOURCC('s', 't', 'r', 'l');
    if (WINMM_mmioDescend(infoPtr->hMMio, &mmckList, &mmckHead, MMIO_FINDLIST) != 0) {
        //WARN("Can't find 'strl' list\n");
        return FALSE;
    }

    mmckInfo.ckid = mmioFOURCC('s', 't', 'r', 'h');
    if (WINMM_mmioDescend(infoPtr->hMMio, &mmckInfo, &mmckList, MMIO_FINDCHUNK) != 0) {
        //WARN("Can't find 'strh' chunk\n");
        return FALSE;
    }

    WINMM_mmioRead(infoPtr->hMMio, (LPSTR)&infoPtr->ash, sizeof(infoPtr->ash));
    //TRACE("ash.fccType='%c%c%c%c'\n",                 LOBYTE(LOWORD(infoPtr->ash.fccType)),
    //                                          HIBYTE(LOWORD(infoPtr->ash.fccType)),
    //                                          LOBYTE(HIWORD(infoPtr->ash.fccType)),
    //                                          HIBYTE(HIWORD(infoPtr->ash.fccType)));
    //TRACE("ash.fccHandler='%c%c%c%c'\n",      LOBYTE(LOWORD(infoPtr->ash.fccHandler)),
    //                                          HIBYTE(LOWORD(infoPtr->ash.fccHandler)),
    //                                          LOBYTE(HIWORD(infoPtr->ash.fccHandler)),
    //                                          HIBYTE(HIWORD(infoPtr->ash.fccHandler)));
    //TRACE("ash.dwFlags=%ld\n",                        infoPtr->ash.dwFlags);
    //TRACE("ash.wPriority=%d\n",               infoPtr->ash.wPriority);
    //TRACE("ash.wLanguage=%d\n",               infoPtr->ash.wLanguage);
    //TRACE("ash.dwInitialFrames=%ld\n",                infoPtr->ash.dwInitialFrames);
    //TRACE("ash.dwScale=%ld\n",                        infoPtr->ash.dwScale);
    //TRACE("ash.dwRate=%ld\n",                         infoPtr->ash.dwRate);
    //TRACE("ash.dwStart=%ld\n",                        infoPtr->ash.dwStart);
    //TRACE("ash.dwLength=%ld\n",               infoPtr->ash.dwLength);
    //TRACE("ash.dwSuggestedBufferSize=%ld\n",  infoPtr->ash.dwSuggestedBufferSize);
    //TRACE("ash.dwQuality=%ld\n",              infoPtr->ash.dwQuality);
    //TRACE("ash.dwSampleSize=%ld\n",           infoPtr->ash.dwSampleSize);
    //TRACE("ash.rcFrame=(%d,%d,%d,%d)\n",      infoPtr->ash.rcFrame.top, infoPtr->ash.rcFrame.left,
    //    infoPtr->ash.rcFrame.bottom, infoPtr->ash.rcFrame.right);
    WINMM_mmioAscend(infoPtr->hMMio, &mmckInfo, 0);

    mmckInfo.ckid = mmioFOURCC('s', 't', 'r', 'f');
    if (WINMM_mmioDescend(infoPtr->hMMio, &mmckInfo, &mmckList, MMIO_FINDCHUNK) != 0) {
        //WARN("Can't find 'strh' chunk\n");
        return FALSE;
    }

    infoPtr->inbih = (BITMAPINFOHEADER*)HeapAlloc(GetProcessHeap(), 0, mmckInfo.cksize);
    if (!infoPtr->inbih) {
        //WARN("Can't alloc input BIH\n");
        return FALSE;
    }

    WINMM_mmioRead(infoPtr->hMMio, (LPSTR)infoPtr->inbih, mmckInfo.cksize);
    //TRACE("bih.biSize=%ld\n",                 infoPtr->inbih->biSize);
    //TRACE("bih.biWidth=%ld\n",                infoPtr->inbih->biWidth);
    //TRACE("bih.biHeight=%ld\n",       infoPtr->inbih->biHeight);
    //TRACE("bih.biPlanes=%d\n",                infoPtr->inbih->biPlanes);
    //TRACE("bih.biBitCount=%d\n",      infoPtr->inbih->biBitCount);
    //TRACE("bih.biCompression=%ld\n",  infoPtr->inbih->biCompression);
    //TRACE("bih.biSizeImage=%ld\n",    infoPtr->inbih->biSizeImage);
    //TRACE("bih.biXPelsPerMeter=%ld\n",        infoPtr->inbih->biXPelsPerMeter);
    //TRACE("bih.biYPelsPerMeter=%ld\n",        infoPtr->inbih->biYPelsPerMeter);
    //TRACE("bih.biClrUsed=%ld\n",      infoPtr->inbih->biClrUsed);
    //TRACE("bih.biClrImportant=%ld\n",         infoPtr->inbih->biClrImportant);
    WINMM_mmioAscend(infoPtr->hMMio, &mmckInfo, 0);

    WINMM_mmioAscend(infoPtr->hMMio, &mmckList, 0);

#if 0
    /* an AVI has 0 or 1 video stream, and to be animated should not contain
     * an audio stream, so only one strl is allowed
     */
    mmckList.fccType = mmioFOURCC('s', 't', 'r', 'l');
    if (WINMM_mmioDescend(infoPtr->hMMio, &mmckList, &mmckHead, MMIO_FINDLIST) == 0) {
        //WARN("There should be a single 'strl' list\n");
        return FALSE;
    }
#endif

    WINMM_mmioAscend(infoPtr->hMMio, &mmckHead, 0);

    /* no need to read optional JUNK chunk */

    mmckList.fccType = mmioFOURCC('m', 'o', 'v', 'i');
    if (WINMM_mmioDescend(infoPtr->hMMio, &mmckList, &ckMainRIFF, MMIO_FINDLIST) != 0) {
        //WARN("Can't find 'movi' list\n");
        return FALSE;
    }

    /* FIXME: should handle the 'rec ' LIST when present */

    infoPtr->lpIndex = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                 infoPtr->mah.dwTotalFrames * sizeof(DWORD));
    if (!infoPtr->lpIndex) {
        //WARN("Can't alloc index array\n");
        return FALSE;
    }

    numFrame = insize = 0;
    while (WINMM_mmioDescend(infoPtr->hMMio, &mmckInfo, &mmckList, 0) == 0 &&
           numFrame < infoPtr->mah.dwTotalFrames) {
        infoPtr->lpIndex[numFrame] = mmckInfo.dwDataOffset;
        if (insize < mmckInfo.cksize)
            insize = mmckInfo.cksize;
        numFrame++;
        WINMM_mmioAscend(infoPtr->hMMio, &mmckInfo, 0);
    }
    if (numFrame != infoPtr->mah.dwTotalFrames) {
        //WARN("Found %ld frames (/%ld)\n", numFrame, infoPtr->mah.dwTotalFrames);
        return FALSE;
    }
    if (insize > infoPtr->ash.dwSuggestedBufferSize) {
        //WARN("insize=%ld suggestedSize=%ld\n", insize, infoPtr->ash.dwSuggestedBufferSize);
        infoPtr->ash.dwSuggestedBufferSize = insize;
    }

    infoPtr->indata = HeapAlloc(GetProcessHeap(), 0, infoPtr->ash.dwSuggestedBufferSize);
    if (!infoPtr->indata) {
        //WARN("Can't alloc input buffer\n");
        return FALSE;
    }

    return TRUE;
}


static BOOL    ANIMATE_GetAviCodec(ANIMATE_INFO *infoPtr)
{
    DWORD       outSize;

    /* check uncompressed AVI */
    if (infoPtr->ash.fccHandler == mmioFOURCC('D', 'I', 'B', ' ')) {
        infoPtr->hic = 0;
        return TRUE;
    }

    /* try to get a decompressor for that type */
    infoPtr->hic = (infoPtr->fnICOpen)(ICTYPE_VIDEO,
                                       infoPtr->ash.fccHandler,
                                       ICMODE_DECOMPRESS);
    if (!infoPtr->hic) {
        //WARN("Can't load codec for the file\n");
        return FALSE;
    }

    outSize = (infoPtr->fnICSendMessage)(infoPtr->hic,
                                         ICM_DECOMPRESS_GET_FORMAT,
                                         (DWORD)infoPtr->inbih, 0L);
    infoPtr->outbih = (BITMAPINFOHEADER*)HeapAlloc(GetProcessHeap(), 0, outSize);
    if (!infoPtr->outbih) {
        //WARN("Can't alloc output BIH\n");
        return FALSE;
    }

    if ((infoPtr->fnICSendMessage)(infoPtr->hic, ICM_DECOMPRESS_GET_FORMAT,
                                   (DWORD)infoPtr->inbih,
                                   (DWORD)infoPtr->outbih) != ICERR_OK) {
        //WARN("Can't get output BIH\n");
        return FALSE;
    }

    infoPtr->outdata = HeapAlloc(GetProcessHeap(), 0, infoPtr->outbih->biSizeImage);
    if (!infoPtr->outdata) {
        //WARN("Can't alloc output buffer\n");
        return FALSE;
    }

    if ((infoPtr->fnICSendMessage)(infoPtr->hic, ICM_DECOMPRESS_BEGIN,
                                   (DWORD)infoPtr->inbih,
                                   (DWORD)infoPtr->outbih) != ICERR_OK) {
        //WARN("Can't begin decompression\n");
        return FALSE;
    }

    return TRUE;
}

static LRESULT ANIMATE_Open(HWND hWnd, WPARAM wParam, LPARAM lParam,BOOL unicode)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hWnd);
    HINSTANCE hInstance = (HINSTANCE)wParam;

    ANIMATE_Free(infoPtr);

    if (!lParam) {
        //TRACE("Closing avi!\n");
        return TRUE;
    }

    if (!hInstance)
       hInstance = GetWindowLongA(hWnd, GWL_HINSTANCE);

    if (HIWORD(lParam)) {
        //TRACE("(\"%s\");\n", (LPSTR)lParam);

        if (!ANIMATE_LoadRes(infoPtr, hInstance, (LPWSTR)lParam,unicode)) {
            //TRACE("No AVI resource found!\n");
            if (!ANIMATE_LoadFile(infoPtr, (LPWSTR)lParam,unicode)) {
                //WARN("No AVI file found!\n");
                return FALSE;
            }
        }
    } else {
        //TRACE("(%u);\n", (WORD)LOWORD(lParam));

        if (!ANIMATE_LoadRes(infoPtr,hInstance,unicode ? MAKEINTRESOURCEW((INT)lParam):(LPWSTR)MAKEINTRESOURCEA((INT)lParam),unicode)) {
            //WARN("No AVI resource found!\n");
            return FALSE;
        }
    }

    if (!ANIMATE_GetAviInfo(infoPtr)) {
        //WARN("Can't get AVI information\n");
        ANIMATE_Free(infoPtr);
        return FALSE;
    }

    if (!ANIMATE_GetAviCodec(infoPtr)) {
        //WARN("Can't get AVI Codec\n");
        ANIMATE_Free(infoPtr);
        return FALSE;
    }

    if (GetWindowLongA(hWnd, GWL_STYLE) & ACS_CENTER) {
        //FIXME("ACS_CENTER: NIY\n");
    } else {
        /*      MoveWindow(hWnd, 0, 0, infoPtr->mah.dwWidth, infoPtr->mah.dwHeight, FALSE);*/
        SetWindowPos(hWnd, 0, 0, 0, infoPtr->mah.dwWidth, infoPtr->mah.dwHeight,
                     SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    }

    if (GetWindowLongA(hWnd, GWL_STYLE) & ACS_TRANSPARENT) {
        //FIXME("ACS_TRANSPARENT: NIY\n");
    }

    if (GetWindowLongA(hWnd, GWL_STYLE) & ACS_AUTOPLAY) {
        return ANIMATE_Play(hWnd, -1, (LPARAM)MAKELONG(0, infoPtr->mah.dwTotalFrames-1));
    }

    return TRUE;
}

static LRESULT ANIMATE_Stop(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hWnd);

    /* nothing opened */
    if (!infoPtr->hMMio)
        return FALSE;

    ANIMATE_DoStop(infoPtr);
    return TRUE;
}


static LRESULT ANIMATE_NCCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO*       infoPtr;

    if (!ANIMATE_LoadWINMM()) return FALSE;
    if (!ANIMATE_LoadMSVFW32()) return FALSE;

    /* allocate memory for info structure */
    infoPtr = (ANIMATE_INFO*)initControl(hWnd,sizeof(ANIMATE_INFO));
    if (!infoPtr) {
        //ERR("could not allocate info memory!\n");
        return 0;
    }

    /* Temporary hack until we get dllglue up and running */
    *(VOID**)&infoPtr->fnICOpen        = (VOID*)GetProcAddress(hMSVFW32,"ICOpen");
    *(VOID**)&infoPtr->fnICClose       = (VOID*)GetProcAddress(hMSVFW32,"ICClose");
    *(VOID**)&infoPtr->fnICSendMessage = (VOID*)GetProcAddress(hMSVFW32,"ICSendMessage");
    *(VOID**)&infoPtr->fnICDecompress  = (VOID*)GetProcAddress(hMSVFW32,"ICDecompress");

    //TRACE("Animate style=0x%08lx, parent=%08lx\n", GetWindowLongA(hWnd, GWL_STYLE), (DWORD)GetParent(hWnd));

    /* store crossref hWnd <-> info structure */
    infoPtr->hWnd = hWnd;

    InitializeCriticalSection(&infoPtr->cs);
    //RegisterDebugptr(&infoPtr->cs, "Animate infoPtr->cs");

    return DefWindowProcA(hWnd,WM_NCCREATE,wParam,lParam);
}

static LRESULT ANIMATE_NCHitTest(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return HTTRANSPARENT;
}

static LRESULT ANIMATE_Timer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  return ANIMATE_DrawFrame(ANIMATE_GetInfoPtr(hWnd));
}

static LRESULT ANIMATE_Paint(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  if (wParam)
  {
    ANIMATE_PaintFrame(ANIMATE_GetInfoPtr(hWnd), (HDC)wParam);
  } else
  {
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);

    ANIMATE_PaintFrame(ANIMATE_GetInfoPtr(hWnd), hDC);
    EndPaint(hWnd, &ps);
  }

  return 0;
}

static LRESULT ANIMATE_Destroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hWnd);


    /* free avi data */
    ANIMATE_Free(infoPtr);

    /* free animate info data */
    doneControl(hWnd);

    return DefWindowProcA(hWnd,WM_DESTROY,wParam,lParam);
}


static LRESULT ANIMATE_EraseBackground(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rect;

    GetClientRect(hWnd, &rect);
#if 0
    HBRUSH hBrush = CreateSolidBrush(infoPtr->clrBk);

    FillRect((HDC)wParam, &rect, hBrush);
    DeleteObject(hBrush);
#else
    FillRect((HDC)wParam, &rect, GetSysColorBrush(COLOR_WINDOW));
#endif
    return TRUE;
}

static LRESULT WINAPI ANIMATE_Close(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   ANIMATE_Free(ANIMATE_GetInfoPtr(hWnd));
   return TRUE;
}

static LRESULT WINAPI ANIMATE_Size(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hWnd);

    if (GetWindowLongA(hWnd, GWL_STYLE) & ACS_CENTER) {
        //FIXME("NIY\n");
        if (infoPtr->hMMio) {
            /* centers the animation in the control, invalidates the control
             */
        }
        InvalidateRect(hWnd, NULL, TRUE);
    }
    return DefWindowProcA(hWnd,WM_SIZE,wParam,lParam);
}

static LRESULT ANIMATE_StyleChanged(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  //CB: todo

  return 0;
}

static LRESULT WINAPI ANIMATE_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case ACM_OPENA:
          return ANIMATE_Open(hwnd,wParam,lParam,FALSE);

        case ACM_OPENW:
          return ANIMATE_Open(hwnd,wParam,lParam,TRUE);

        case ACM_PLAY:
          return ANIMATE_Play (hwnd, wParam, lParam);

        case ACM_STOP:
          return ANIMATE_Stop (hwnd, wParam, lParam);

        case WM_CLOSE:
          return ANIMATE_Close(hwnd,wParam,lParam);

        case WM_NCCREATE:
          return ANIMATE_NCCreate(hwnd,wParam,lParam);

        case WM_DESTROY:
          return ANIMATE_Destroy (hwnd, wParam, lParam);

        case WM_ERASEBKGND:
          return ANIMATE_EraseBackground (hwnd, wParam, lParam);

        case WM_NCHITTEST:
          return ANIMATE_NCHitTest(hwnd,wParam,lParam);

        case WM_PAINT:
          return ANIMATE_Paint(hwnd,wParam,lParam);

        case WM_SIZE:
          return ANIMATE_Size(hwnd,wParam,lParam);

      case WM_STYLECHANGED:
          return ANIMATE_StyleChanged(hwnd,wParam,lParam);

      case WM_TIMER:
          return ANIMATE_Timer(hwnd,wParam,lParam);

        default:
//          if (uMsg >= WM_USER)
//              ERR (animate, "unknown msg %04x wp=%08x lp=%08lx\n",
//                   uMsg, wParam, lParam);
            return defComCtl32ProcA (hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


VOID
ANIMATE_Register (VOID)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)ANIMATE_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(ANIMATE_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wndClass.lpszClassName = ANIMATE_CLASSA;

    RegisterClassA (&wndClass);
}


VOID
ANIMATE_Unregister (VOID)
{
    UnregisterClassA (ANIMATE_CLASSA, (HINSTANCE)NULL);
}

