/* $Id: font.cpp,v 1.2 1999-11-09 19:52:33 phaller Exp $ */

/*
 * GDI32 font apis
 *
 * Copyright 1999 Edgar Buerkle (Edgar.Buerkle@gmx.ne)
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 * TODO: EnumFontsA/W, EnumFontFamiliesExA/W + others
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "misc.h"
#include "unicode.h"
#include <vmutex.h>


ODINDEBUGCHANNEL(GDI32-FONT)


VMutex mutexProcWinA;
VMutex mutexProcWinW;
FONTENUMPROCA FontEnumProcWinA;
FONTENUMPROCW FontEnumProcWinW;

//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontA(int arg1, int arg2, int arg3, int arg4, int arg5,
                               DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9,
                               DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13, LPCSTR arg14)
{
 HFONT hfont;

    hfont = O32_CreateFont(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
    dprintf(("GDI32: CreateFontA '%s' returned %D\n", arg14, hfont));
    return(hfont);
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontW(int arg1, int arg2, int arg3, int arg4, int arg5,
                              DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9,
                              DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13, LPCWSTR arg14)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg14);
 HFONT rc;

   dprintf(("GDI32: OS2CreateFontW\n"));
   rc = O32_CreateFont(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, astring);
   FreeAsciiString(astring);
   return(rc);
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontIndirectA(const LOGFONTA *lplf)
{
 HFONT rc;

    dprintf(("GDI32: CreateFontIndirectA\n"));
    dprintf(("GDI32: lfHeight        = %d\n", lplf->lfHeight));
    dprintf(("GDI32: lfWidth          = %d\n", lplf->lfWidth));
    dprintf(("GDI32: lfEscapement    = %d\n", lplf->lfEscapement));
    dprintf(("GDI32: lfOrientation   = %d\n", lplf->lfOrientation));
    dprintf(("GDI32: lfWeight        = %d\n", lplf->lfWeight));
    dprintf(("GDI32: lfItalic        = %d\n", lplf->lfItalic));
    dprintf(("GDI32: lfUnderline     = %d\n", lplf->lfUnderline));
    dprintf(("GDI32: lfStrikeOut     = %d\n", lplf->lfStrikeOut));
    dprintf(("GDI32: lfCharSet       = %X\n", lplf->lfCharSet));
    dprintf(("GDI32: lfOutPrecision  = %X\n", lplf->lfOutPrecision));
    dprintf(("GDI32: lfClipPrecision = %X\n", lplf->lfClipPrecision));
    dprintf(("GDI32: lfQuality       = %X\n", lplf->lfQuality));
    dprintf(("GDI32: lfPitchAndFamily= %X\n", lplf->lfPitchAndFamily));
    dprintf(("GDI32: lfFaceName      = %s\n", lplf->lfFaceName));
    rc = O32_CreateFontIndirect(lplf);
    dprintf(("GDI32: OS2CreateFontIndirectA returned %X\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HFONT, CreateFontIndirectW,const LOGFONTW *, lplf)
{
  LOGFONTA afont;
  HFONT    hfont;

  //memcpy(&afont, lplf, ((int)&afont.lfFaceName - (int)&afont));
  memcpy(&afont, lplf, sizeof(LOGFONTA));
  UnicodeToAsciiN((WCHAR *)lplf->lfFaceName, afont.lfFaceName, LF_FACESIZE-1);
  hfont = CreateFontIndirectA(&afont);
  return(hfont);
}
//******************************************************************************
//******************************************************************************
int  EXPENTRY_O32 EnumFontProcA(LPENUMLOGFONTA lpLogFont, LPNEWTEXTMETRICA
                                lpTextM, DWORD arg3, LPARAM arg4)
{
  return FontEnumProcWinA(lpLogFont, lpTextM, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int  EXPENTRY_O32 EnumFontProcW(LPENUMLOGFONTA lpLogFont, LPNEWTEXTMETRICA lpTextM,
                                DWORD arg3, LPARAM arg4)
{
  ENUMLOGFONTW LogFont;
  int rc;

  memcpy(&LogFont, lpLogFont, ((int)&LogFont.elfLogFont.lfFaceName -
         (int)&LogFont));
  AsciiToUnicodeN(lpLogFont->elfLogFont.lfFaceName, LogFont.elfLogFont.lfFaceName, LF_FACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfFullName, LogFont.elfFullName, LF_FULLFACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfStyle, LogFont.elfStyle, LF_FACESIZE-1);

  rc = FontEnumProcWinW(&LogFont, (LPNEWTEXTMETRICW) lpTextM, arg3, arg4);
  return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontsA( HDC arg1, LPCSTR arg2, FONTENUMPROCA arg3, LPARAM  arg4)
{
    dprintf(("GDI32: OS2EnumFontsA"));
//    return O32_EnumFonts(arg1, arg2, arg3, arg4);
    return 1;
}
//******************************************************************************
//TODO: Callback
//******************************************************************************
int WIN32API EnumFontsW( HDC arg1, LPCWSTR arg2, FONTENUMPROCW arg3, LPARAM  arg4)
{
    dprintf(("GDI32: OS2EnumFontsW - stub (1)"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_EnumFonts(arg1, arg2, arg3, arg4);
    return 1;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontFamiliesA(HDC          arg1,
                               LPCSTR       arg2,
                               FONTENUMPROCA arg3,
                               LPARAM       arg4)
{
  int rc;

  dprintf(("GDI32: OS2EnumFontFamiliesA "));

  mutexProcWinA.enter();

  FontEnumProcWinA = arg3;

  rc = O32_EnumFontFamilies(arg1, arg2, &EnumFontProcA, arg4);

  mutexProcWinA.leave();
  return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontFamiliesW(HDC          arg1,
                               LPCWSTR       arg2,
                               FONTENUMPROCW arg3,
                               LPARAM       arg4)
{
  int rc;
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);

  dprintf(("GDI32: OS2EnumFontFamiliesW "));

  mutexProcWinW.enter();

  FontEnumProcWinW = arg3;

  rc = O32_EnumFontFamilies(arg1, astring, &EnumFontProcW, arg4);

  mutexProcWinW.leave();

  FreeAsciiString(astring);
  return rc;
}
//******************************************************************************
//******************************************************************************
INT WIN32API EnumFontFamiliesExA( HDC arg1, LPLOGFONTA arg2, FONTENUMPROCEXA arg3, LPARAM  arg4, DWORD dwFlags)
{
    dprintf(("GDI32: OS2EnumFontFamiliesExA, not implemented\n"));
    return 0;
}
//******************************************************************************
//******************************************************************************
INT WIN32API EnumFontFamiliesExW( HDC arg1, LPLOGFONTW arg2, FONTENUMPROCEXW arg3, LPARAM  arg4, DWORD dwFlags)
{
    dprintf(("GDI32: OS2EnumFontFamiliesW, not implemented\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFontData(HDC hdc, DWORD dwTable, DWORD dwOffset, LPVOID lpvBuffer,
                              DWORD dbData)
{
  dprintf(("GDI32: GetFontData, not implemented (GDI_ERROR)\n"));
  return(GDI_ERROR);
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceA( LPCSTR arg1)
{
    dprintf(("GDI32: OS2AddFontResourceA"));
    return O32_AddFontResource(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceW( LPCWSTR arg1)
{
    dprintf(("GDI32: OS2AddFontResourceW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_AddFontResource(arg1);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceA( LPCSTR arg1)
{
    dprintf(("GDI32: OS2RemoveFontResourceA %s\n", arg1));
    return O32_RemoveFontResource(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceW(LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 BOOL  rc;

    dprintf(("GDI32: OS2RemoveFontResourceW\n"));
    rc = O32_RemoveFontResource(astring);
    FreeAsciiString(astring);
    return(rc);
}
/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceA
 * Purpose   : The CreateScalableFontResourceA function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceA(DWORD   fdwHidden,
                                             LPCSTR lpszFontRes,
                                             LPCSTR lpszFontFile,
                                             LPCSTR lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           fdwHidden,
           lpszFontRes,
           lpszFontFile,
           lpszCurrentPath));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceW
 * Purpose   : The CreateScalableFontResourceW function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceW(DWORD   fdwHidden,
                                             LPCWSTR lpszFontRes,
                                             LPCWSTR lpszFontFile,
                                             LPCWSTR lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           fdwHidden,
           lpszFontRes,
           lpszFontFile,
           lpszCurrentPath));

  return (FALSE);
}
/*****************************************************************************
 * Name      : DWORD GetFontLanguageInfo
 * Purpose   : The GetFontLanguageInfo function returns information about the
 *             currently selected font for the specified display context.
 *             Applications typically use this information and the
 *             GetCharacterPlacement function to prepare a character string for display.
 * Parameters: HDC     hdc        handle to device context
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetFontLanguageInfo(HDC hdc)
{
  dprintf(("GDI32: GetFontLanguageInfo(%08xh) not implemented.\n",
           hdc));

  return (0);
}
//******************************************************************************
//******************************************************************************
