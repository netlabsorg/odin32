/* $Id: font.cpp,v 1.33 2003-12-01 13:28:10 sandervl Exp $ */

/*
 * GDI32 font apis
 *
 * Copyright 1999 Edgar Buerkle (Edgar.Buerkle@gmx.ne)
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 * TODO: EnumFontsA/W, EnumFontFamiliesExA/W not complete
 *
 * Parts based on Wine code (991031)
 *
 * Copyright 1993 Alexandre Julliard
 *           1997 Alex Korobka
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "misc.h"
#include "unicode.h"
#include <heapstring.h>
#include <win\options.h>
#include <wprocess.h>
#include <odininst.h>
#include <stats.h>
#include "oslibgpi.h"

#define DBG_LOCALLOG    DBG_font
#include "dbglocal.h"

ODINDEBUGCHANNEL(GDI32-FONT)


typedef struct {
  DWORD userProc;
  DWORD userData;
  DWORD dwFlags;
} ENUMUSERDATA;

/*
 *  For TranslateCharsetInfo
 */
#define FS(x) {{0,0,0,0},{0x1<<(x),0}}
#define MAXTCIINDEX 32
static CHARSETINFO FONT_tci[MAXTCIINDEX] = {
  /* ANSI */
  { ANSI_CHARSET, 1252, FS(0)},
  { EASTEUROPE_CHARSET, 1250, FS(1)},
  { RUSSIAN_CHARSET, 1251, FS(2)},
  { GREEK_CHARSET, 1253, FS(3)},
  { TURKISH_CHARSET, 1254, FS(4)},
  { HEBREW_CHARSET, 1255, FS(5)},
  { ARABIC_CHARSET, 1256, FS(6)},
  { BALTIC_CHARSET, 1257, FS(7)},
  /* reserved by ANSI */
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  /* ANSI and OEM */
  { THAI_CHARSET,  874,  FS(16)},
  { SHIFTJIS_CHARSET, 932, FS(17)},
  { GB2312_CHARSET, 936, FS(18)},
  { HANGEUL_CHARSET, 949, FS(19)},
  { CHINESEBIG5_CHARSET, 950, FS(20)},
  { JOHAB_CHARSET, 1361, FS(21)},
  /* reserved for alternate ANSI and OEM */
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
  /* reserved for system */
  { DEFAULT_CHARSET, 0, FS(0)},
  { DEFAULT_CHARSET, 0, FS(0)},
};

HFONT hFntDefaultGui = NULL;

/*****************************************************************************
 * Name      : static void iFontRename
 * Purpose   : font remapping table to map win32 fonts to OS/2 pendants
 * Parameters: LPSTR lpstrFaceOriginal - the win32 face name
 *             LPSTR lpstrFaceBuffer   - [LF_FACESIZE] buffer to new name
 * Variables :
 * Result    :
 * Remark    : remapped name is passed back in the buffer
 *             if no mapping pendant is available, return input parameter
 *             as default.
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

static void iFontRename(LPCSTR lpstrFaceOriginal,
                        LPSTR  lpstrFaceTemp)
{
  int   iRet;

  // NULL is a valid parameter
  if (lpstrFaceOriginal == NULL)
     return;

  strncpy(lpstrFaceTemp, lpstrFaceOriginal, LF_FACESIZE);
  lpstrFaceTemp[LF_FACESIZE-1] = 0;

  {
    char *y = lpstrFaceTemp;
    while(*y) {
      if(IsDBCSLeadByte( *y )) {
        y += 2; // DBCS skip
      } else {
        *y = toupper( *y );
        y++;
      }
    }
  }

  //lookup table
  iRet = PROFILE_GetOdinIniString(ODINFONTSECTION,
                                  lpstrFaceTemp,
                                  lpstrFaceOriginal,
                                  lpstrFaceTemp,
                                  LF_FACESIZE);
}
/***********************************************************************
 *           FONT_mbtowc
 *
 * Returns a '\0' terminated Unicode translation of str using the
 * charset of the currently selected font in hdc.  If count is -1 then
 * str is assumed to be '\0' terminated, otherwise it contains the
 * number of bytes to convert.  If plenW is non-NULL, on return it
 * will point to the number of WCHARs (excluding the '\0') that have
 * been written.  If pCP is non-NULL, on return it will point to the
 * codepage used in the conversion (NB, this may be CP_SYMBOL so watch
 * out).  The caller should free the returned LPWSTR from the process
 * heap itself.
 */
LPWSTR FONT_mbtowc(HDC hdc, LPCSTR str, INT count, INT *plenW, UINT *pCP)
{
    UINT cp = CP_ACP;
    INT lenW, i;
    LPWSTR strW;
    CHARSETINFO csi;
    int charset = GetTextCharset(hdc);

    /* Hmm, nicely designed api this one! */
    if(TranslateCharsetInfo((DWORD*)charset, &csi, TCI_SRCCHARSET))
        cp = csi.ciACP;
    else {
        switch(charset) {
	case OEM_CHARSET:
	    cp = GetOEMCP();
	    break;
	case DEFAULT_CHARSET:
	    cp = GetACP();
	    break;

	case VISCII_CHARSET:
	case TCVN_CHARSET:
	case KOI8_CHARSET:
	case ISO3_CHARSET:
	case ISO4_CHARSET:
	  /* FIXME: These have no place here, but because x11drv
	     enumerates fonts with these (made up) charsets some apps
	     might use them and then the FIXME below would become
	     annoying.  Now we could pick the intended codepage for
	     each of these, but since it's broken anyway we'll just
	     use CP_ACP and hope it'll go away...
	  */
	    cp = CP_ACP;
	    break;


	default:
	    dprintf(("Can't find codepage for charset %d\n", charset));
	    break;
	}
    }

    dprintf(("cp == %d\n", cp));

    if(count == -1) count = strlen(str);
    if(cp != CP_SYMBOL) {
        lenW = MultiByteToWideChar(cp, 0, str, count, NULL, 0);
	strW = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, (lenW + 1) * sizeof(WCHAR));
	MultiByteToWideChar(cp, 0, str, count, strW, lenW);
    } else {
        lenW = count;
	strW = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, (lenW + 1) * sizeof(WCHAR));
	for(i = 0; i < count; i++) strW[i] = (BYTE)str[i];
    }
    strW[lenW] = '\0';
    dprintf(("mapped %s -> %ls\n", str, strW));
    if(plenW) *plenW = lenW;
    if(pCP) *pCP = cp;
    return strW;
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontA(int  nHeight,
                           int    nWidth,
                           int    nEscapement,
                           int    nOrientation,
                           int    fnWeight,
                           DWORD  fdwItalic,
                           DWORD  fdwUnderline,
                           DWORD  fdwStrikeOut,
                           DWORD  fdwCharSet,
                           DWORD  fdwOutputPrecision,
                           DWORD  fdwClipPrecision,
                           DWORD  fdwQuality,
                           DWORD  fdwPitchAndFamily,
                           LPCSTR lpszFace)
{
  CHAR  lpstrFaceNew[LF_FACESIZE];
  HFONT hFont;

  if(lpszFace == NULL)
      lpszFace = "";

  if(strlen(lpszFace) >= LF_FACESIZE)
  {
      dprintf(("ERROR: Invalid string length for font name!!"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
  }

  iFontRename(lpszFace, lpstrFaceNew);

  dprintf(("lpszFace = %s -> %s\n", lpszFace, lpstrFaceNew));

  LOGFONTA logFont =
  {
      nHeight,
      nWidth,
      nEscapement,
      nOrientation,
      fnWeight,
      (BYTE)fdwItalic,
      (BYTE)fdwUnderline,
      (BYTE)fdwStrikeOut,
      (BYTE)fdwCharSet,
      (BYTE)fdwOutputPrecision,
      (BYTE)fdwClipPrecision,
      (BYTE)fdwQuality,
      (BYTE)fdwPitchAndFamily
  };
  strcpy(logFont.lfFaceName, lpszFace);

  return CreateFontIndirectA(&logFont);
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontW(int     nHeight,
                           int     nWidth,
                           int     nEscapement,
                           int     nOrientation,
                           int     fnWeight,
                           DWORD   fdwItalic,
                           DWORD   fdwUnderline,
                           DWORD   fdwStrikeOut,
                           DWORD   fdwCharSet,
                           DWORD   fdwOutputPrecision,
                           DWORD   fdwClipPrecision,
                           DWORD   fdwQuality,
                           DWORD   fdwPitchAndFamily,
                           LPCWSTR lpszFace)
{
  char *astring;
  HFONT hFont;

  // NULL is valid for lpszFace
  if(lpszFace != NULL)
    astring = UnicodeToAsciiString((LPWSTR)lpszFace);
  else
    astring = NULL;

  // @@@PH switch to ODIN_ later
  hFont =    CreateFontA(nHeight,
                         nWidth,
                         nEscapement,
                         nOrientation,
                         fnWeight,
                         fdwItalic,
                         fdwUnderline,
                         fdwStrikeOut,
                         fdwCharSet,
                         fdwOutputPrecision,
                         fdwClipPrecision,
                         fdwQuality,
                         fdwPitchAndFamily,
                         astring);
  if (astring != NULL)
    FreeAsciiString(astring);

  return(hFont);
}

//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontIndirectA(const LOGFONTA* lplf)
{
  HFONT    hFont;
  LOGFONTA afont;

  // don't touch user buffer!
  memcpy(&afont, lplf, sizeof(LOGFONTA));
  iFontRename(lplf->lfFaceName, afont.lfFaceName);

  dprintf(("lpszFace = (%x) %s -> %s\n", lplf->lfFaceName, lplf->lfFaceName, afont.lfFaceName));

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

  if( IsDBCSEnv())
  {
    if( !strcmp( afont.lfFaceName, "WarpSans" ))
    {
        dprintf(("DBCS : WarpSans -> WarpSans Combined"));

        strcpy( afont.lfFaceName, "WarpSans Combined" );
    }
  }

  hFont = O32_CreateFontIndirect(&afont);
  if(hFont) {
      STATS_CreateFontIndirect(hFont, &afont);
  }
  return(hFont);
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontIndirectW(const LOGFONTW * lplf)
{
  LOGFONTA afont;
  HFONT    hfont;

  //memcpy(&afont, lplf, ((ULONG)&afont.lfFaceName - (ULONG)&afont));
  memcpy(&afont, lplf, sizeof(LOGFONTA));
  memset(afont.lfFaceName, 0, LF_FACESIZE);
  dprintf(("lpszFace = (%x)", lplf->lfFaceName));

  UnicodeToAsciiN((WCHAR *)lplf->lfFaceName, afont.lfFaceName, LF_FACESIZE-1);
  hfont = CreateFontIndirectA(&afont);
  return(hfont);
}
//******************************************************************************
//******************************************************************************
int  EXPENTRY_O32 EnumFontProcA(LPENUMLOGFONTA lpLogFont, LPNEWTEXTMETRICA
                                   lpTextM, DWORD arg3, LPARAM arg4)
{
 ENUMUSERDATA *lpEnumData = (ENUMUSERDATA *)arg4;
 FONTENUMPROCA proc = (FONTENUMPROCA)lpEnumData->userProc;
 USHORT selTIB = SetWin32TIB(); // save current FS selector and set win32 sel

  int rc = proc(lpLogFont, lpTextM, arg3, lpEnumData->userData);
  SetFS(selTIB);           // switch back to the saved FS selector
  return rc;
}
//******************************************************************************
//******************************************************************************
int  EXPENTRY_O32 EnumFontProcW(LPENUMLOGFONTA lpLogFont, LPNEWTEXTMETRICA lpTextM,
                                   DWORD arg3, LPARAM arg4)
{
 ENUMUSERDATA *lpEnumData = (ENUMUSERDATA *)arg4;
 FONTENUMPROCW proc = (FONTENUMPROCW)lpEnumData->userProc;
 ENUMLOGFONTW LogFont;
 NEWTEXTMETRICW textM;
 USHORT selTIB = SetWin32TIB(); // save current FS selector and set win32 sel
 int rc;

  memcpy(&LogFont, lpLogFont, ((ULONG)&LogFont.elfLogFont.lfFaceName -
         (ULONG)&LogFont));
  AsciiToUnicodeN(lpLogFont->elfLogFont.lfFaceName, LogFont.elfLogFont.lfFaceName, LF_FACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfFullName, LogFont.elfFullName, LF_FULLFACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfStyle, LogFont.elfStyle, LF_FACESIZE-1);

  textM.tmHeight = lpTextM->tmHeight;
  textM.tmAscent = lpTextM->tmAscent;
  textM.tmDescent = lpTextM->tmDescent;
  textM.tmInternalLeading = lpTextM->tmInternalLeading;
  textM.tmExternalLeading = lpTextM->tmExternalLeading;
  textM.tmAveCharWidth = lpTextM->tmAveCharWidth;
  textM.tmMaxCharWidth = lpTextM->tmMaxCharWidth;
  textM.tmWeight = lpTextM->tmWeight;
  textM.tmOverhang = lpTextM->tmOverhang;
  textM.tmDigitizedAspectX = lpTextM->tmDigitizedAspectX;
  textM.tmDigitizedAspectY = lpTextM->tmDigitizedAspectY;
  textM.tmFirstChar = lpTextM->tmFirstChar;
  textM.tmLastChar = lpTextM->tmLastChar;
  textM.tmDefaultChar = lpTextM->tmDefaultChar;
  textM.tmBreakChar = lpTextM->tmBreakChar;
  textM.tmItalic = lpTextM->tmItalic;
  textM.tmUnderlined = lpTextM->tmUnderlined;
  textM.tmStruckOut = lpTextM->tmStruckOut;
  textM.tmPitchAndFamily = lpTextM->tmPitchAndFamily;
  textM.tmCharSet = lpTextM->tmCharSet;
  textM.ntmFlags = 0;
  textM.ntmSizeEM = 0;
  textM.ntmCellHeight = 0;
  textM.ntmAvgWidth = 0;

  rc = proc(&LogFont, &textM, arg3, lpEnumData->userData);
  SetFS(selTIB);           // switch back to the saved FS selector
  return rc;
}
//******************************************************************************
//TODO: FontEnumdwFlagsEx, script, font signature & NEWTEXTMETRICEX (last part)
//******************************************************************************
int  EXPENTRY_O32 EnumFontProcExA(LPENUMLOGFONTA lpLogFont, LPNEWTEXTMETRICA
                                     lpTextM, DWORD arg3, LPARAM arg4)
{
 ENUMUSERDATA *lpEnumData = (ENUMUSERDATA *)arg4;
 FONTENUMPROCEXA proc = (FONTENUMPROCEXA)lpEnumData->userProc;
 ENUMLOGFONTEXA logFont;
 NEWTEXTMETRICEXA textM;
 USHORT selTIB = SetWin32TIB(); // save current FS selector and set win32 sel

  memcpy(&logFont, lpLogFont, sizeof(ENUMLOGFONTA));
  memset(logFont.elfScript, 0, sizeof(logFont.elfScript));
  memcpy(&textM.ntmTm, lpTextM, sizeof(textM.ntmTm));
  memset(&textM.ntmFontSig, 0, sizeof(textM.ntmFontSig));

  dprintf(("EnumFontProcExA %s height %d", logFont.elfLogFont.lfFaceName, textM.ntmTm.tmHeight));

  int rc = proc(&logFont, &textM, arg3, lpEnumData->userData);
  SetFS(selTIB);           // switch back to the saved FS selector
  return rc;
}
//******************************************************************************
//TODO: FontEnumdwFlagsEx, script, font signature & NEWTEXTMETRICEX (last part)
//******************************************************************************
int EXPENTRY_O32 EnumFontProcExW(LPENUMLOGFONTA lpLogFont, LPNEWTEXTMETRICA lpTextM,
                                    DWORD arg3, LPARAM arg4)
{
 ENUMUSERDATA *lpEnumData = (ENUMUSERDATA *)arg4;
 FONTENUMPROCEXW proc = (FONTENUMPROCEXW)lpEnumData->userProc;
 ENUMLOGFONTEXW LogFont;
 NEWTEXTMETRICEXW textM;
 USHORT selTIB = SetWin32TIB(); // save current FS selector and set win32 sel
 int rc;

  memcpy(&LogFont, lpLogFont, ((ULONG)&LogFont.elfLogFont.lfFaceName - (ULONG)&LogFont));
  memset(LogFont.elfScript, 0, sizeof(LogFont.elfScript));
  AsciiToUnicodeN(lpLogFont->elfLogFont.lfFaceName, LogFont.elfLogFont.lfFaceName, LF_FACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfFullName, LogFont.elfFullName, LF_FULLFACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfStyle, LogFont.elfStyle, LF_FACESIZE-1);

  textM.ntmTm.tmHeight = lpTextM->tmHeight;
  textM.ntmTm.tmAscent = lpTextM->tmAscent;
  textM.ntmTm.tmDescent = lpTextM->tmDescent;
  textM.ntmTm.tmInternalLeading = lpTextM->tmInternalLeading;
  textM.ntmTm.tmExternalLeading = lpTextM->tmExternalLeading;
  textM.ntmTm.tmAveCharWidth = lpTextM->tmAveCharWidth;
  textM.ntmTm.tmMaxCharWidth = lpTextM->tmMaxCharWidth;
  textM.ntmTm.tmWeight = lpTextM->tmWeight;
  textM.ntmTm.tmOverhang = lpTextM->tmOverhang;
  textM.ntmTm.tmDigitizedAspectX = lpTextM->tmDigitizedAspectX;
  textM.ntmTm.tmDigitizedAspectY = lpTextM->tmDigitizedAspectY;
  textM.ntmTm.tmFirstChar = lpTextM->tmFirstChar;
  textM.ntmTm.tmLastChar = lpTextM->tmLastChar;
  textM.ntmTm.tmDefaultChar = lpTextM->tmDefaultChar;
  textM.ntmTm.tmBreakChar = lpTextM->tmBreakChar;
  textM.ntmTm.tmItalic = lpTextM->tmItalic;
  textM.ntmTm.tmUnderlined = lpTextM->tmUnderlined;
  textM.ntmTm.tmStruckOut = lpTextM->tmStruckOut;
  textM.ntmTm.tmPitchAndFamily = lpTextM->tmPitchAndFamily;
  textM.ntmTm.tmCharSet = lpTextM->tmCharSet;
  textM.ntmTm.ntmFlags = 0;
  textM.ntmTm.ntmSizeEM = 0;
  textM.ntmTm.ntmCellHeight = 0;
  textM.ntmTm.ntmAvgWidth = 0;
  memset(&textM.ntmFontSig, 0, sizeof(textM.ntmFontSig));

  dprintf(("EnumFontProcExW %s height %d", lpLogFont->elfLogFont.lfFaceName, textM.ntmTm.tmHeight));
  rc = proc(&LogFont, &textM, arg3, lpEnumData->userData);
  SetFS(selTIB);           // switch back to the saved FS selector
  return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontsA(HDC hdc,
                        LPCSTR arg2,
                        FONTENUMPROCA arg3,
                        LPARAM  arg4)
{
  //@@@PH shouldn't this rather be O32_EnumFonts ?
  return EnumFontFamiliesA(hdc, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontsW(HDC hdc,
                        LPCWSTR arg2,
                        FONTENUMPROCW arg3,
                        LPARAM  arg4)
{
  //@@@PH shouldn't this rather be O32_EnumFonts ?
  return EnumFontFamiliesW(hdc, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontFamiliesA(HDC hdc,
                               LPCSTR lpszFontFamily,
                               FONTENUMPROCA arg3,
                               LPARAM arg4)
{
  ENUMUSERDATA enumData;
  CHAR         lpstrFamilyNew[LF_FACESIZE] = "";
  int rc;

  dprintf(("GDI32: EnumFontFamiliesA %s", lpszFontFamily));

  iFontRename(lpszFontFamily, lpstrFamilyNew);

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;

  rc = O32_EnumFontFamilies(hdc, lpstrFamilyNew, &EnumFontProcA, (LPARAM)&enumData);

  return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontFamiliesW(HDC hdc,
                               LPCWSTR lpszFontFamilyW,
                               FONTENUMPROCW arg3,
                               LPARAM arg4)
{
  CHAR         lpstrFamilyNew[LF_FACESIZE] = "";
  ENUMUSERDATA enumData;
  int          rc;
  char        *lpszFontFamilyA = UnicodeToAsciiString((LPWSTR)lpszFontFamilyW);

  dprintf(("GDI32: EnumFontFamiliesW %s", lpszFontFamilyA));

  iFontRename(lpszFontFamilyA, lpstrFamilyNew);

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;

  rc = O32_EnumFontFamilies(hdc, lpstrFamilyNew, &EnumFontProcW, (LPARAM)&enumData);

  if(lpszFontFamilyA) FreeAsciiString(lpszFontFamilyA);
  return rc;
}
//******************************************************************************
//******************************************************************************
INT WIN32API EnumFontFamiliesExA(HDC hdc,
                                 LPLOGFONTA arg2,
                                 FONTENUMPROCEXA arg3,
                                 LPARAM arg4,
                                 DWORD dwFlags)
{
  ENUMUSERDATA enumData;
  int rc;

  dprintf(("GDI32: EnumFontFamiliesExA not complete %s", arg2->lfFaceName));

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;
  enumData.dwFlags  = dwFlags;

  rc = O32_EnumFontFamilies(hdc, arg2->lfFaceName, &EnumFontProcExA, (LPARAM)&enumData);

  return rc;
}
//******************************************************************************
//******************************************************************************
INT WIN32API EnumFontFamiliesExW(HDC hdc,
                                 LPLOGFONTW arg2,
                                 FONTENUMPROCEXW arg3,
                                 LPARAM arg4,
                                 DWORD dwFlags)
{
  ENUMUSERDATA enumData;
  int rc;
  char *astring = UnicodeToAsciiString((LPWSTR)arg2->lfFaceName);

  dprintf(("GDI32: EnumFontFamiliesExW not complete %s", astring));

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;
  enumData.dwFlags  = dwFlags;

  rc = O32_EnumFontFamilies(hdc, astring, &EnumFontProcExW, (LPARAM)&enumData);

  FreeAsciiString(astring);
  return rc;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFontData(HDC hdc, DWORD dwTable,
                           DWORD dwOffset,
                           LPVOID lpvBuffer,
                           DWORD dbData)
{
  dprintf(("GDI32: GetFontData, not implemented (GDI_ERROR)\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  return(GDI_ERROR);
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceA(LPCSTR szFont)
{
    HINSTANCE hInstance;

    dprintf(("GDI32: AddFontResourceA %s", szFont));
    hInstance = LoadLibraryA(szFont);
    if(hInstance) {
        dprintf(("AddFontResourceA: executable file; NOT IMPLEMENTED"));
        FreeLibrary(hInstance);
        return 1;
    }
    return 1;
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceW(LPCWSTR szFont)
{
 char *astring = UnicodeToAsciiString((LPWSTR)szFont);
 BOOL  rc;

    dprintf(("GDI32: AddFontResourceW"));
    // NOTE: This will not work as is (needs UNICODE support)
    rc = AddFontResourceA(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceA(LPCSTR lpszFont)
{
    dprintf(("GDI32: RemoveFontResourceA %s", lpszFont));
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceW(LPCWSTR szFont)
{
 char *astring = UnicodeToAsciiString((LPWSTR)szFont);
 BOOL  rc;

    dprintf(("GDI32: RemoveFontResourceW"));
    rc = RemoveFontResourceA(astring);
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

BOOL WIN32API CreateScalableFontResourceA(DWORD fdwHidden,
                                          LPCSTR lpszFontRes,
                                          LPCSTR lpszFontFile,
                                          LPCSTR lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceA %x %s %s %s not implemented", fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath));

//  return OSLibGpiLoadFonts((LPSTR)lpszFontFile);
  return FALSE;
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

BOOL WIN32API CreateScalableFontResourceW(DWORD fdwHidden,
                                          LPCWSTR lpszFontRes,
                                          LPCWSTR lpszFontFile,
                                          LPCWSTR lpszCurrentPath)
{
  LPSTR lpszFontFileA = NULL, lpszFontResA = NULL, lpszCurrentPathA = NULL;

  dprintf(("GDI32: CreateScalableFontResourceW %x %ls %ls %ls not implemented", fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath));

  STACK_strdupWtoA(lpszFontFile, lpszFontFileA);
  STACK_strdupWtoA(lpszFontRes, lpszFontResA);
  STACK_strdupWtoA(lpszCurrentPath, lpszCurrentPathA);
  return CreateScalableFontResourceA(fdwHidden, lpszFontResA, lpszFontFileA, lpszCurrentPathA);
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


/*************************************************************************
 * TranslateCharsetInfo [GDI32.382]
 *
 * Fills a CHARSETINFO structure for a character set, code page, or
 * font. This allows making the correspondance between different labelings
 * (character set, Windows, ANSI, and OEM codepages, and Unicode ranges)
 * of the same encoding.
 *
 * Only one codepage will be set in lpCs->fs. If TCI_SRCFONTSIG is used,
 * only one codepage should be set in *lpSrc.
 *
 * RETURNS
 *   TRUE on success, FALSE on failure.
 *
 *
 * LPDWORD lpSrc, if flags == TCI_SRCFONTSIG: pointer to fsCsb of a FONTSIGNATURE
 *                if flags == TCI_SRCCHARSET: a character set value
 *                if flags == TCI_SRCCODEPAGE: a code page value
 * LPCHARSETINFO lpCs, structure to receive charset information
 * DWORD flags  determines interpretation of lpSrc
 */
BOOL WIN32API TranslateCharsetInfo(LPDWORD lpSrc, LPCHARSETINFO lpCs,
                                   DWORD flags)
{
    int index = 0;
    switch (flags) {
    case TCI_SRCFONTSIG:
      while (!(*lpSrc>>index & 0x0001) && index<MAXTCIINDEX) index++;
      break;
    case TCI_SRCCODEPAGE:
      while ((UINT) (lpSrc) != FONT_tci[index].ciACP && index < MAXTCIINDEX) index++;
      break;
    case TCI_SRCCHARSET:
      while ((UINT) (lpSrc) != FONT_tci[index].ciCharset && index < MAXTCIINDEX) index++;
      break;
    default:
      return FALSE;
    }
    if (index >= MAXTCIINDEX || FONT_tci[index].ciCharset == DEFAULT_CHARSET) return FALSE;
    memcpy(lpCs, &FONT_tci[index], sizeof(CHARSETINFO));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextMetricsA( HDC hdc, LPTEXTMETRICA  pwtm)
{
 BOOL rc;

    rc = O32_GetTextMetrics(hdc, pwtm);
    dprintf(("GDI32: GetTextMetricsA %x %x returned %d", hdc, pwtm, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextMetricsW( HDC hdc, LPTEXTMETRICW pwtm)
{
 BOOL rc;
 TEXTMETRICA atm;

    dprintf(("GDI32: GetTextMetricsW"));

    rc = O32_GetTextMetrics(hdc, &atm);
    pwtm->tmHeight = atm.tmHeight;
    pwtm->tmAscent = atm.tmAscent;
    pwtm->tmDescent = atm.tmDescent;
    pwtm->tmInternalLeading = atm.tmInternalLeading;
    pwtm->tmExternalLeading = atm.tmExternalLeading;
    pwtm->tmAveCharWidth = atm.tmAveCharWidth;
    pwtm->tmMaxCharWidth = atm.tmMaxCharWidth;
    pwtm->tmWeight = atm.tmWeight;
    pwtm->tmOverhang = atm.tmOverhang;
    pwtm->tmDigitizedAspectX = atm.tmDigitizedAspectX;
    pwtm->tmDigitizedAspectY = atm.tmDigitizedAspectY;
    pwtm->tmFirstChar = atm.tmFirstChar;
    pwtm->tmLastChar = atm.tmLastChar;
    pwtm->tmDefaultChar = atm.tmDefaultChar;
    pwtm->tmBreakChar = atm.tmBreakChar;
    pwtm->tmItalic = atm.tmItalic;
    pwtm->tmUnderlined = atm.tmUnderlined;
    pwtm->tmStruckOut = atm.tmStruckOut;
    pwtm->tmPitchAndFamily = atm.tmPitchAndFamily;
    pwtm->tmCharSet = atm.tmCharSet;

    dprintf(("GDI32: GetTextMetricsW %x %x returned %d", hdc, pwtm, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextFaceA( HDC hdc, int arg2, LPSTR  arg3)
{
    dprintf(("GDI32: GetTextFaceA %x %d %x", hdc, arg2, arg3));
    return O32_GetTextFace(hdc, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextFaceW( HDC hdc, int arg2, LPWSTR  arg3)
{
 char *astring = NULL;
 int   lenA = GetTextFaceA( hdc, 0, NULL );
 int   rc;

    dprintf(("GDI32: GetTextFaceW"));
    astring = ( char * )malloc( lenA );
    if( astring )
        return 0;

    rc = GetTextFaceA(hdc, lenA, astring);

    if( rc )
    {
        if( arg3 )
        {
            AsciiToUnicodeN(astring, arg3, arg2);
            rc = lstrlenW( arg3 );
        }
        else
            rc = lstrlenAtoW( astring, -1 );

        rc++; // including null-terminator
    }

    free(astring);

    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOutlineTextMetricsA( HDC hdc, UINT arg2, LPOUTLINETEXTMETRICA  arg3)
{
    dprintf(("GDI32: GetOutlineTextMetricsA %x %x %x", hdc, arg2, arg3));
    return O32_GetOutlineTextMetrics(hdc, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOutlineTextMetricsW( HDC hdc, UINT arg2, LPOUTLINETEXTMETRICW  arg3)
{
    dprintf(("!ERROR!: GDI32: GetOutlineTextMetricsW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_GetOutlineTextMetrics(hdc, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
