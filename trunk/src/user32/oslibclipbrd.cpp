/* $Id: oslibclipbrd.cpp,v 1.1 2004-04-13 14:17:01 sandervl Exp $ */
/** @file
 *
 * OSLIB Clipboard
 *
 * InnoTek Systemberatung GmbH confidential
 *
 * Copyright (c) 2004 InnoTek Systemberatung GmbH
 * Author: knut st. osmundsen <bird-srcspam@anduin.net>
 *
 * All Rights Reserved
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define  INCL_WIN
#define  INCL_WINSTDDRAG
#define  INCL_PM
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <dbglog.h>
#include <winconst.h>
#include <win32api.h>
#include <winuser32.h>

#include "oslibutil.h"
#include "oslibclipbrd.h"


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/**
 * Debug assertion macro.
 * @param   expr    Assert that this expression is true.
 * @param   msg     Message to print if expr isn't true. It's given to dprintf,
 *                  and must be inclosed in paratheses.
 * @todo move this to some header in /include.
 */
#ifdef DEBUG
    #define DebugAssert(expr, msg) \
    do { if (expr) break; \
         dprintf(("!!!ASSERTION FAILED!!!\nFILE=%s\nLINE=%d\nFUNCTION=%s\n", __FILE__, __LINE__, __FUNCTION__)); \
         dprintf(msg); DebugInt3(); \
       } while (0)
#else
    #define DebugAssert(expr, msg) do {} while (0)
#endif

/**
 * Debug assertion failed macro.
 * @param   msg     Message to print if expr isn't true. It's given to dprintf,
 *                  and must be inclosed in paratheses.
 * @todo move this to some header in /include.
 */
#ifdef DEBUG
    #define DebugAssertFailed(msg) \
    do { dprintf(("!!!ASSERTION FAILED!!!\nFILE=%s\nLINE=%d\nFUNCTION=%s\n", __FILE__, __LINE__, __FUNCTION__)); \
         dprintf(msg); DebugInt3(); \
       } while (0)
#else
    #define DebugAssertFailed(msg) do {} while (0)
#endif


/**
 * Duplicates a PM bitmap.
 *
 * @returns Handle to duplicated PM bitmap.
 * @returns NULL on failure.
 * @param   hbmp    Handle to the bitmap.
 */
HANDLE  OSLibClipboardPMBitmapDuplicate(HANDLE hbmp)
{
    HAB     hab = GetThreadHAB();
    HBITMAP hbmpRet = NULLHANDLE;

    /*
     * Create a PS.
     */
    HDC hdc = DevOpenDC(hab, OD_MEMORY, (PSZ)"*", 0L, NULL, NULLHANDLE);
    if (!hdc)
        return NULL;
    SIZEL   sizl = { WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN), WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN) };
    HPS hps = GpiCreatePS(hab, hdc, &sizl, GPIA_ASSOC | PU_PELS);

    /*
     * Select the bitmap into the PS.
     */
    if (GpiSetBitmap(hps, hbmp) != HBM_ERROR)
    {
        /*
         * Get the bitmap header.
         */
        BITMAPINFOHEADER2   bih2;
        bih2.cbFix = sizeof(bih2);
        if (GpiQueryBitmapInfoHeader(hbmp, &bih2))
        {
            /*
             * Calc bitmap size.
             */
            unsigned    cbColorTable = 0;
            switch (bih2.cBitCount)
            {
                case 1:
                case 2:
                case 4:
                case 8:
                    cbColorTable = (1 << bih2.cBitCount) * sizeof(RGB2);
                    break;
            }

            unsigned cbBits = bih2.cbImage;
            if (!cbBits)
            {
                cbBits = ((bih2.cx * bih2.cBitCount + 31) >> 5) << 2;
                cbBits *= bih2.cy * bih2.cPlanes;
            }

            unsigned cbTotal = sizeof(bih2) + cbColorTable + cbBits;

            /*
             * Allocate temporary storage.
             */
            void *  pv = malloc(cbTotal);
            if (pv)
            {
                /*
                 * Get the bitmap bits.
                 */
                void *pvBits = (char*)pv + sizeof(bih2) + cbColorTable;
                memcpy(pv, &bih2, sizeof(bih2));
                if (GpiQueryBitmapBits(hps, 0, bih2.cy, (PBYTE)pvBits, (PBITMAPINFO2)pv))
                {
                    if (GpiSetBitmap(hps, NULLHANDLE) != HBM_ERROR)
                    {
                        hbmpRet = GpiCreateBitmap(hps, (PBITMAPINFOHEADER2)pv, CBM_INIT, (PBYTE)pvBits,
                                                  (PBITMAPINFO2)pv);
                        DebugAssert(hbmpRet, ("GpiCreateBitmap failed lasterr=%#x\n", WinGetLastError(GetThreadHAB())));
                    }
                }
                free(pv);
            }
        }

    }

    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    dprintf(("OSLibClipboardPMBitmapDuplicate: returning %#x\n", hbmpRet));
    return hbmpRet;
}


/**
 * Destroys a PM bitmap.
 *
 * @param   hbmp    Handle to the bitmap.
 */
void    OSLibClipboardPMBitmapDelete(HANDLE hbmp)
{
    if (!GpiDeleteBitmap(hbmp))
        DebugAssertFailed(("GpiDeleteBitmap failed lasterr=%#x\n", WinGetLastError(GetThreadHAB())));
}


/**
 * Duplicates a PM palette.
 *
 * @returns Handle to duplicated PM palette.
 * @returns NULL on failure.
 * @param   hpal    Handle to the palette.
 */
HANDLE  OSLibClipboardPMPaletteDuplicate(HANDLE hpal)
{
    HANDLE  hpalRet = NULL;
    HAB     hab = GetThreadHAB();

    /*
     * Query data from hpal.
     */
    LONG cEntries = GpiQueryPaletteInfo(hpal, NULLHANDLE, 0, 0, 0, NULL);
    if (cEntries > 0)
    {
        PULONG  paulRGB = (PULONG)malloc(cEntries * sizeof(ULONG));
        if (paulRGB)
        {
            cEntries = GpiQueryPaletteInfo(hab, NULLHANDLE, 0, 0, cEntries, paulRGB);
            if (cEntries != PAL_ERROR)
            {
                hpalRet = GpiCreatePalette(hab, LCOL_PURECOLOR, LCOLF_CONSECRGB, cEntries, paulRGB);
                DebugAssert(hpalRet != GPI_ERROR, ("GpiQueryPaletteInfo failed returning %d. lasterr=%#x\n", cEntries, WinGetLastError(hab)));
            }
            else
                DebugAssertFailed(("GpiQueryPaletteInfo failed returning %d. lasterr=%#x\n", cEntries, WinGetLastError(hab)));
            free(paulRGB);
        }
    }
    else
        DebugAssertFailed(("GpiQueryPaletteInfo failed returning %d. lasterr=%#x\n", cEntries, WinGetLastError(hab)));

    return hpalRet;
}


/**
 * Destroys a PM palette.
 *
 * @param   hpal    Handle to the palette.
 */
void    OSLibClipboardPMPaletteDelete(HANDLE hpal)
{
    if (!GpiDeletePalette(hpal))
        DebugAssertFailed(("GpiDeletePalette failed lasterr=%#x\n", WinGetLastError(GetThreadHAB())));
}


BOOL    OSLib_OpenClipbrd(HANDLE hab, HWND hwndOpenWindow)
{   return _OpenClipbrd(hab, hwndOpenWindow); }

HWND    OSLibWin32QueryOpenClipbrdWindow(void)
{   return Win32QueryOpenClipbrdWindow(); }

BOOL    OSLibWinEmptyClipbrd(HANDLE hab)
{   return WinEmptyClipbrd(hab); }

BOOL    OSLibWinSetClipbrdOwner(HANDLE hab, HWND hwndOwner)
{   return WinSetClipbrdOwner(hab, hwndOwner); }

HWND    OSLibWinQueryClipbrdOwner(HANDLE hab)
{   return WinQueryClipbrdOwner(hab); }

HWND    OSLibWin32QueryClipbrdViewerChain(void)
{   return Win32QueryClipbrdViewerChain(); }

BOOL    OSLibWin32AddClipbrdViewer(HWND hwndNewViewer)
{   return Win32AddClipbrdViewer(hwndNewViewer); }

BOOL    OSLibWin32RemoveClipbrdViewer(HWND hwndRemove)
{   return Win32RemoveClipbrdViewer(hwndRemove); }

BOOL    OSLibWinQueryClipbrdFmtInfo(HANDLE hab, ULONG ulFormat, PULONG pfl)
{   return WinQueryClipbrdFmtInfo(hab, ulFormat, pfl); }

BOOL    OSLibWinSetClipbrdData(HANDLE hab, ULONG ulData, ULONG ulFormat, ULONG ulFlags)
{   return WinSetClipbrdData(hab, ulData, ulFormat, ulFlags); }

ULONG   OSLibWinQueryClipbrdData(HANDLE hab, ULONG ulFormat)
{   return WinQueryClipbrdData(hab, ulFormat); }

BOOL    OSLibWinCloseClipbrd(HANDLE hab)
{   return WinCloseClipbrd(hab); }

ULONG   OSLibWinEnumClipbrdFmts(HANDLE hab, ULONG ulFmt)
{   return WinEnumClipbrdFmts(hab, ulFmt); }

ULONG   OSLibWinAddAtom(HANDLE hAtomTable, const char *pszAtomName)
{   return WinAddAtom(hAtomTable, pszAtomName); }

ULONG   OSLibWinQueryAtomName(HANDLE hAtomTable, ULONG ulFormat, char *pszNameBuf, ULONG cchNameBuf)
{   return WinQueryAtomName(hAtomTable, (ATOM)ulFormat, pszNameBuf, cchNameBuf); }

HANDLE  OSLibWinQuerySystemAtomTable(void)
{   return WinQuerySystemAtomTable(); }

ULONG   OSLibWinSetErrorInfo(ULONG ulError, ULONG ulFlags, ...)
{   return WinSetErrorInfo(ulError, ulFlags); }

ULONG   OSLibDosAllocSharedMem(void **ppv, const char *psz, ULONG cb, ULONG flFlags)
{   return DosAllocSharedMem(ppv, psz, cb, flFlags); }

ULONG   OSLibDosFreeMem(void *pv)
{   return DosFreeMem(pv); }

ULONG   OSLibDosQueryMem(PVOID pvAddr, PULONG pcb, PULONG pfl)
{   return DosQueryMem(pvAddr, pcb, pfl); }

