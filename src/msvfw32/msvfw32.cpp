/* $Id: msvfw32.cpp,v 1.9 2002-08-27 08:31:48 sandervl Exp $ */
/*
 * Copyright 1998 Marcus Meissner
 * Copyright 1999 Jens Wiessner
 */

#include <stdlib.h>
#include <string.h>
#include <odin.h>

#define ICOM_CINTERFACE
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


/* Stubs not included in Wine-Code*/


/****************************************************************************
 *    DrawDibChangePalette    [MSVFW.4]
 */

BOOL VFWAPI DrawDibChangePalette(HANDLE /*HDRAWDIB*/ hdd, int iStart, int iLen, LPPALETTEENTRY lppe)
{
    dprintf(("MSVFW32: DrawDibChangePalette not implemented\n"));
    return TRUE;
}


/****************************************************************************
 *    DrawDibProfileDisplay   [MSVFW.11]
 */
DWORD VFWAPI DrawDibProfileDisplay(LPBITMAPINFOHEADER lpbi)
{
       dprintf(("MSVFW32: DrawDibProfileDisplay not implemented\n"));
       return TRUE;
}

/****************************************************************************
 *    DrawDibTime     [MSVFW.16]
 */
BOOL VFWAPI DrawDibTime(HANDLE /*HDRAWDIB*/ hdd, DWORD lpddtime)
{
    dprintf(("MSVFW32: DrawDibTime not implemented\n"));
    return TRUE;
}

/****************************************************************************
 *    DrawDibGetBuffer    [MSVFW.8]
 */
LPVOID VFWAPI DrawDibGetBuffer(HANDLE /*HDRAWDIB*/ hdd, LPBITMAPINFOHEADER lpbi, DWORD dwSize, DWORD dwFlags)
{
    dprintf(("MSVFW32: DrawDibGetBuffer not implemented\n"));
    return 0;
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
