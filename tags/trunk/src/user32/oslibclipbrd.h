/* $Id: oslibclipbrd.h,v 1.1 2004-04-13 14:17:01 sandervl Exp $ */
/** @file
 *
 * OSLIB for Clipboard
 *
 * InnoTek Systemberatung GmbH
 *
 * Copyright (c) 2004 InnoTek Systemberatung GmbH
 * Author: knut st. osmundsen <bird-srcspam@anduin.net>
 *
 * All Rights Reserved
 *
 */

#ifndef __oslibclipbrd_h__
#define __oslibclipbrd_h__

/*
 * Error codes.
 */
#define OSLIB_PMERR_CALL_FROM_WRONG_THREAD              0x1009


/*
 * Formats
 */
#define OSLIB_CF_TEXT                   1
#define OSLIB_CF_BITMAP                 2
#define OSLIB_CF_DSPTEXT                3
#define OSLIB_CF_DSPBITMAP              4
#define OSLIB_CF_METAFILE               5
#define OSLIB_CF_DSPMETAFILE            6
#define OSLIB_CF_PALETTE                9
#define OSLIB_CF_MMPMFIRST              10
#define OSLIB_CF_MMPMLAST               19

#define OSLIB_SZFMT_TEXT                "#1"
#define OSLIB_SZFMT_BITMAP              "#2"
#define OSLIB_SZFMT_DSPTEXT             "#3"
#define OSLIB_SZFMT_DSPBITMAP           "#4"
#define OSLIB_SZFMT_METAFILE            "#5"
#define OSLIB_SZFMT_DSPMETAFILE         "#6"
#define OSLIB_SZFMT_PALETTE             "#9"
#define OSLIB_SZFMT_SYLK                "Sylk"
#define OSLIB_SZFMT_DIF                 "Dif"
#define OSLIB_SZFMT_TIFF                "Tiff"
#define OSLIB_SZFMT_OEMTEXT             "OemText"
#define OSLIB_SZFMT_DIB                 "Dib"
#define OSLIB_SZFMT_OWNERDISPLAY        "OwnerDisplay"
#define OSLIB_SZFMT_LINK                "Link"
#define OSLIB_SZFMT_METAFILEPICT        "MetaFilePict"
#define OSLIB_SZFMT_DSPMETAFILEPICT     "DspMetaFilePict"
#define OSLIB_SZFMT_CPTEXT              "Codepage Text"
#define OSLIB_SZFMT_WINMETAFILEPICT     "Win_MetaFilePict"
#define OSLIB_SZFMT_WINDSPMETAFILEPICT  "Win_DspMetaFilePict"
#define OSLIB_SZFMT_WINENHMETAFILE      "Win_EnhMetaFile"
#define OSLIB_SZFMT_WINDSPENHMETAFILE   "Win_DspEnhMetaFile"

#pragma pack(1)
typedef struct _OSLIB_BITMAPINFOHEADER2
{
  DWORD	    cbFix;
  DWORD	    cx;
  DWORD	    cy;
  WORD      cPlanes;
  WORD      cBitCount;
  DWORD	    ulCompression;
  DWORD	    cbImage;
  DWORD	    cxResolution;
  DWORD	    cyResolution;
  DWORD	    cclrUsed;
  DWORD	    cclrImportant;
  WORD      usUnits;
  WORD      usReserved;
  WORD      usRecording;
  WORD      usRendering;
  DWORD	    cSize1;
  DWORD	    cSize2;
  DWORD	    ulColorEncoding;
  DWORD	    ulIdentifier;
} OSLIB_BITMAPINFOHEADER2, *POSLIB_BITMAPINFOHEADER2;
#pragma pack()

#define OSLIB_CFI_OWNERFREE             0x0001
#define OSLIB_CFI_OWNERDISPLAY          0x0002
#define OSLIB_CFI_HANDLE                0x0200
#define OSLIB_CFI_POINTER               0x0400

#define OSLIB_PAG_READ                  0x0001
#define OSLIB_PAG_WRITE                 0x0002
#define OSLIB_PAG_EXECUTE               0x0004
#define OSLIB_PAG_GUARD                 0x0008
#define OSLIB_PAG_COMMIT                0x0010
#define OSLIB_PAG_DECOMMIT              0x0020
#define OSLIB_OBJ_TILE                  0x0040
#define OSLIB_OBJ_PROTECTED             0x0080
#define OSLIB_OBJ_GETTABLE              0x0100
#define OSLIB_OBJ_GIVEABLE              0x0200
#define OSLIB_PAG_DEFAULT               0x0400
#define OSLIB_PAG_SHARED                0x2000
#define OSLIB_PAG_FREE                  0x4000
#define OSLIB_PAG_BASE              0x00010000


HANDLE  OSLibClipboardPMBitmapDuplicate(HANDLE hbmp);
void    OSLibClipboardPMBitmapDelete(HANDLE hbmp);
HANDLE  OSLibClipboardPMPaletteDuplicate(HANDLE hpal);
void    OSLibClipboardPMPaletteDelete(HANDLE hpal);

BOOL    OSLib_OpenClipbrd(HANDLE hab, HWND hwndOpenWindow);
HWND    OSLibWin32QueryOpenClipbrdWindow(void);
BOOL    OSLibWinEmptyClipbrd(HANDLE hab);
BOOL    OSLibWinSetClipbrdOwner(HANDLE hab, HWND hwndOwner);
HWND    OSLibWinQueryClipbrdOwner(HANDLE hab);
HWND    OSLibWin32QueryClipbrdViewerChain(void);
BOOL    OSLibWin32AddClipbrdViewer(HWND hwndNewViewer);
BOOL    OSLibWin32RemoveClipbrdViewer(HWND hwndRemove);
BOOL    OSLibWinQueryClipbrdFmtInfo(HANDLE hab, ULONG ulFormat, PULONG pfl);
BOOL    OSLibWinSetClipbrdData(HANDLE hab, ULONG ulData, ULONG ulFormat, ULONG ulFlags);
ULONG   OSLibWinQueryClipbrdData(HANDLE hab, ULONG ulFormat);
BOOL    OSLibWinCloseClipbrd(HANDLE hab);

ULONG   OSLibWinEnumClipbrdFmts(HANDLE hab, ULONG b);
ULONG   OSLibWinAddAtom(HANDLE hAtomTable, const char *pszAtomName);
ULONG   OSLibWinQueryAtomName(HANDLE hAtomTable, ULONG ulFormat, char *pszNameBuf, ULONG cchNameBuf);
HANDLE  OSLibWinQuerySystemAtomTable(void);

ULONG   OSLibWinSetErrorInfo(ULONG ulError, ULONG ulFlags, ...);

ULONG   OSLibDosAllocSharedMem(void **pv, const char *psz, ULONG cb, ULONG flFlags);
ULONG   OSLibDosFreeMem(void *);
ULONG   OSLibDosQueryMem(PVOID pvAddr, PULONG pcb, PULONG pfl);

#endif
