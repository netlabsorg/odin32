/* $Id: mmio.cpp,v 1.1 1999-05-24 20:20:07 ktk Exp $ */

/*
 * MMIO stubs
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>

#include <string.h>
#include <misc.h>
#include <unicode.h>

#include "winmm.h"


MMRESULT WIN32API mmioAdvance(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT fuOperation)
{
  dprintf(("WINMM:mmioAdvance - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API mmioAscend(HMMIO hmmio, LPMMCKINFO lpmmcki, UINT uReserved)
{
  dprintf(("WINMM:mmioAscend - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API mmioClose(HMMIO hmmio, UINT fuOption)
{
  dprintf(("WINMM:mmioClose - stub\n"));
  return 0;
}

/******************************************************************************/
//HMMIO hmmio;  /* identifies file      */
//LPMMCKINFO lpmmcki;   /* address of chunk information structure       */
//UINT fuOptions;       /* chunk creation options       */
/******************************************************************************/
MMRESULT WIN32API mmioCreateChunk(HMMIO hmmio, LPMMCKINFO lpmmcki, UINT fuOptions)
{
    dprintf(("WINMM:mmioCreateChunk - stub\n"));
    return 0;
}

/******************************************************************************/
//HMMIO hmmio;  /* handle of open RIFF file     */
//LPMMCKINFO lpmmcki;   /* address of chunk information structure       */
//LPMMCKINFO lpmmckiParent;     /* address of optional parent structure */
//UINT fuSearch;        /* search-option flags  */
/******************************************************************************/
MMRESULT WIN32API mmioDescend(HMMIO hmmio, LPMMCKINFO lpmmcki,
                              const MMCKINFO *lpmmckiParent, UINT fuSearch)
{
    dprintf(("WINMM:mmioDescend - stub\n"));
    return 0;
}

MMRESULT WIN32API mmioFlush(HMMIO hmmio, UINT fuFlush)
{
  dprintf(("WINMM:mmioFlush - stub\n"));
  return 0;
}

MMRESULT WIN32API mmioGetInfo(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uReserved)
{
  dprintf(("WINMM:mmioGetInfo - stub\n"));
  return(MMSYSERR_NODRIVER);
}

//   mmioInstallIOProc16        = _mmioInstallIOProc16@??     @120

LPMMIOPROC WIN32API mmioInstallIOProcA(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags)
{
  dprintf(("WINMM:mmioInstallIOProcA - stub\n"));
  return 0;
}

LPMMIOPROC WIN32API mmioInstallIOProcW(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags)
{
  dprintf(("WINMM:mmioInstallIOProcW - stub\n"));
  return 0;
}

/******************************************************************************/
//SvL: 24-6-'97 - Added
//TODO: Not implemented
/******************************************************************************/
HMMIO WIN32API mmioOpenA(LPTSTR lpszFilename, LPMMIOINFO lpmmioinfo, DWORD fdwOpen)
{
  dprintf(("WINMM:mmioOpenA %s - stub\n", lpszFilename));
  return 234;
}

HMMIO WIN32API mmioOpenW(LPWSTR lpszFilename, LPMMIOINFO lpmmioinfo, DWORD fdwOpen)
{
  dprintf(("WINMM:mmioOpenW - stub\n"));
  return 234;
}

LONG WIN32API mmioRead(HMMIO hmmio, HPSTR pch, LONG cbRead)
{
  dprintf(("WINMM:mmioRead - stub\n"));
  return 0;
}

MMRESULT WIN32API mmioRenameA(LPCSTR pszFileName, LPCSTR pszNewFileName, LPMMIOINFO pmmioinfo, DWORD fdwRename)
{
  dprintf(("WINMM:mmioRenameA - stub\n"));
  return 0;
}

MMRESULT WIN32API mmioRenameW(LPCWSTR pszFileName, LPCWSTR pszNewFileName, LPMMIOINFO pmmioinfo, DWORD fdwRename)
{
  dprintf(("WINMM:mmioRenameW - stub\n"));
  return 0;
}

LONG WIN32API mmioSeek(HMMIO hmmio, LONG lOffset, int nOrigin)
{
  dprintf(("WINMM:mmioSeek - stub\n"));
  return(MMSYSERR_NODRIVER);
}

LRESULT WIN32API mmioSendMessage(HMMIO hmmio, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
  dprintf(("WINMM:mmioSendMessage - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WINAPI mmioSetBuffer(HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer, UINT fuBuffer)
{
  dprintf(("WINMM:mmioSetBuffer - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API mmioSetInfo(HMMIO hmmio, const MMIOINFO *lpmmioinfo, UINT uReserved)
{
  dprintf(("WINMM:mmioSetInfo - stub\n"));
  return(MMSYSERR_NODRIVER);
}


/*****************************************************************************
 * Name      : FOURCC WIN32API OS2mmioStringToFOURCCA
 * Purpose   : Converts a null-terminated string to a four-character code
 * Parameters: LPTSTR sz
 *             UINT    wFlags
 * Variables :
 * Result    :
 * Remark    : 
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

FOURCC WIN32API mmioStringToFOURCCA(LPCSTR sz,
                                    UINT   wFlags)
{
  union
  {
    ULONG ulFourcc; 
    UCHAR ucFourcc[5];
  } unFourcc;

  unFourcc.ucFourcc[0] = sz[0];  
  unFourcc.ucFourcc[1] = sz[1];  
  unFourcc.ucFourcc[2] = sz[2];  
  unFourcc.ucFourcc[3] = sz[3];  
  unFourcc.ucFourcc[4] = 0;     /* string termination */

  if (wFlags & MMIO_TOUPPER) /* upcase the whole thing ? */
    strupr( (LPSTR) unFourcc.ucFourcc);

  dprintf(("WINMM: mmioStringToFOURCCA(%s,%08x).\n",
           sz,
           wFlags));

  return unFourcc.ulFourcc; /* return FOURCC */
}


/*****************************************************************************
 * Name      : FOURCC WIN32API mmioStringToFOURCCW
 * Purpose   : Converts a null-terminated string to a four-character code
 * Parameters: LPWSTR sz
 *             UINT    wFlags
 * Variables :
 * Result    :
 * Remark    : 
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

FOURCC WIN32API mmioStringToFOURCCW(LPCWSTR sz,
                                    UINT   wFlags)
{
  union
  {
    ULONG ulFourcc; 
    UCHAR ucFourcc[5];
  } unFourcc;

  LPSTR pszAscii;                                   /* pointer to ASCII string */
  UCHAR ucBuffer[5];                                    /* buffer for FOURCC */
  
  pszAscii = UnicodeToAsciiString((LPWSTR)sz);
  
  strncpy ( (LPSTR) ucBuffer,
           pszAscii,
           sizeof (ucBuffer) );
  
  FreeAsciiString(pszAscii);
  

  unFourcc.ucFourcc[0] = ucBuffer[0];  
  unFourcc.ucFourcc[1] = ucBuffer[1];  
  unFourcc.ucFourcc[2] = ucBuffer[2];  
  unFourcc.ucFourcc[3] = ucBuffer[3];  
  unFourcc.ucFourcc[4] = 0;                            /* string termination */

  if (wFlags & MMIO_TOUPPER)                     /* upcase the whole thing ? */
    strupr( (LPSTR) unFourcc.ucFourcc);

  dprintf(("WINMM: mmioStringToFOURCCW(%s,%08x).\n",
           sz,
           wFlags));
  
  return unFourcc.ulFourcc; /* return FOURCC */
}

/******************************************************************************/
//SvL: 24-6-'97 - Added
//TODO: Not implemented
/******************************************************************************/
LONG WIN32API mmioWrite(HMMIO hmmio, const char *ch, LONG cbWrite)
{
  dprintf(("WINMM:mmioWrite - stub\n"));
  return 0;
}
