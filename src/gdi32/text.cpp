/* $Id: text.cpp,v 1.2 1999-12-03 17:31:51 cbratschi Exp $ */

/*
 * GDI32 text apis
 *
 * Based on Wine code (991031) (objects\text.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <misc.h>
#include <string.h>

//******************************************************************************
//******************************************************************************
UINT WINAPI GetTextCharsetInfo(
    HDC hdc,          /* [in]  Handle to device context */
    LPFONTSIGNATURE fs, /* [out] Pointer to struct to receive data */
    DWORD flags)        /* [in]  Reserved - must be 0 */
{
    HGDIOBJ hFont;
    UINT charSet = DEFAULT_CHARSET;
    LOGFONTW lf;
    CHARSETINFO csinfo;

    dprintf(("GetTextCharsetInfo %x %x %x", hdc, fs, flags));

    hFont = GetCurrentObject(hdc, OBJ_FONT);
    if (hFont == 0)
        return(DEFAULT_CHARSET);
    if ( GetObjectW(hFont, sizeof(LOGFONTW), &lf) != 0 )
        charSet = lf.lfCharSet;

    if (fs != NULL) {
      if (!TranslateCharsetInfo((LPDWORD)charSet, &csinfo, TCI_SRCCHARSET))
           return  (DEFAULT_CHARSET);
      memcpy(fs, &csinfo.fs, sizeof(FONTSIGNATURE));
    }
    return charSet;
}
/***********************************************************************
 * GetTextCharset32 [GDI32.226]  Gets character set for font in DC
 *
 * NOTES
 *    Should it return a UINT32 instead of an INT32?
 *    => YES, as GetTextCharsetInfo returns UINT32
 *
 * RETURNS
 *    Success: Character set identifier
 *    Failure: DEFAULT_CHARSET
 */
UINT WINAPI GetTextCharset(HDC hdc) /* [in] Handle to device context */
{
    /* MSDN docs say this is equivalent */
    return GetTextCharsetInfo(hdc, NULL, 0);
}
//******************************************************************************
//******************************************************************************
static  BOOL InternalTextOutA(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCSTR lpszString,INT cbCount,CONST INT *lpDx,BOOL IsExtTextOut)
{
  if (cbCount < 0 || (lpszString == NULL && cbCount != 0))
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  //CB: todo

  if (IsExtTextOut)
    return O32_ExtTextOut(hdc,X,Y,fuOptions,lprc,lpszString,cbCount,lpDx);
  else
    return O32_TextOut(hdc,X,Y,lpszString,cbCount);
}
//******************************************************************************
//******************************************************************************
static  BOOL InternalTextOutW(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCWSTR lpszString,INT cbCount,CONST INT *lpDx,BOOL IsExtTextOut)
{
  char *astring = UnicodeToAsciiString((LPWSTR)lpszString);
  BOOL  rc;

  rc = InternalTextOutA(hdc,X,Y,fuOptions,lprc,(LPCSTR)astring,cbCount,lpDx,IsExtTextOut);
  FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutA(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCSTR lpszString,UINT cbCount,CONST INT *lpDx)
{
  dprintf(("GDI32: ExtTextOutA\n"));

  return InternalTextOutA(hdc,X,Y,fuOptions,lprc,lpszString,cbCount,lpDx,TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutW(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCWSTR lpszString,UINT cbCount,CONST int *lpDx)
{
  dprintf(("GDI32: ExtTextOutW\n"));

  return InternalTextOutW(hdc,X,Y,fuOptions,lprc,lpszString,cbCount,lpDx,TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutA(HDC hdc,int nXStart,int nYStart,LPCSTR lpszString,int cbString)
{
  dprintf(("GDI32: TextOutA"));

  return InternalTextOutA(hdc,nXStart,nYStart,0,NULL,lpszString,cbString,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutW(HDC hdc,int nXStart,int nYStart,LPCWSTR lpszString,int cbString)
{
  dprintf(("GDI32: TextOutW"));

  return InternalTextOutW(hdc,nXStart,nYStart,0,NULL,lpszString,cbString,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
