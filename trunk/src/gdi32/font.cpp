/* $Id: font.cpp,v 1.18 2001-04-25 20:53:02 sandervl Exp $ */

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


//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG14(HFONT,  CreateFontA, int,    nHeight,
                                    int,    nWidth,
                                    int,    nEscapement,
                                    int,    nOrientation,
                                    int,    fnWeight,
                                    DWORD,  fdwItalic,
                                    DWORD,  fdwUnderline,
                                    DWORD,  fdwStrikeOut,
                                    DWORD,  fdwCharSet,
                                    DWORD,  fdwOutputPrecision,
                                    DWORD,  fdwClipPrecision,
                                    DWORD,  fdwQuality,
                                    DWORD,  fdwPitchAndFamily,
                                    LPCSTR, lpszFace)
{
  CHAR  lpstrFaceNew[LF_FACESIZE];
  HFONT hFont;

  iFontRename(lpszFace, lpstrFaceNew);

  dprintf(("lpszFace = %s -> %s\n", lpszFace, lpstrFaceNew));

  hFont = O32_CreateFont(nHeight,
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
                         lpszFace != NULL ? lpstrFaceNew : NULL);
  return hFont;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTIONNODBG14(HFONT,  CreateFontW,
               int,    nHeight,
               int,    nWidth,
               int,    nEscapement,
               int,    nOrientation,
               int,    fnWeight,
               DWORD,  fdwItalic,
               DWORD,  fdwUnderline,
               DWORD,  fdwStrikeOut,
               DWORD,  fdwCharSet,
               DWORD,  fdwOutputPrecision,
               DWORD,  fdwClipPrecision,
               DWORD,  fdwQuality,
               DWORD,  fdwPitchAndFamily,
               LPCWSTR,lpszFace)
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
ODINFUNCTION1(HFONT,CreateFontIndirectA,const LOGFONTA*, lplf)
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

  hFont = O32_CreateFontIndirect(&afont);

  return(hFont);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HFONT, CreateFontIndirectW,const LOGFONTW *, lplf)
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
  memcpy(&textM.ntmetm, lpTextM, sizeof(textM.ntmetm));
  memset(&textM.ntmeFontSignature, 0, sizeof(textM.ntmeFontSignature));

  dprintf(("EnumFontProcExA %s", logFont.elfLogFont.lfFaceName));

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
 int rc;

  memcpy(&LogFont, lpLogFont, ((ULONG)&LogFont.elfLogFont.lfFaceName - (ULONG)&LogFont));
  memset(LogFont.elfScript, 0, sizeof(LogFont.elfScript));
  AsciiToUnicodeN(lpLogFont->elfLogFont.lfFaceName, LogFont.elfLogFont.lfFaceName, LF_FACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfFullName, LogFont.elfFullName, LF_FULLFACESIZE-1);
  AsciiToUnicodeN((char *) lpLogFont->elfStyle, LogFont.elfStyle, LF_FACESIZE-1);

  textM.ntmetm.tmHeight = lpTextM->tmHeight;
  textM.ntmetm.tmAscent = lpTextM->tmAscent;
  textM.ntmetm.tmDescent = lpTextM->tmDescent;
  textM.ntmetm.tmInternalLeading = lpTextM->tmInternalLeading;
  textM.ntmetm.tmExternalLeading = lpTextM->tmExternalLeading;
  textM.ntmetm.tmAveCharWidth = lpTextM->tmAveCharWidth;
  textM.ntmetm.tmMaxCharWidth = lpTextM->tmMaxCharWidth;
  textM.ntmetm.tmWeight = lpTextM->tmWeight;
  textM.ntmetm.tmOverhang = lpTextM->tmOverhang;
  textM.ntmetm.tmDigitizedAspectX = lpTextM->tmDigitizedAspectX;
  textM.ntmetm.tmDigitizedAspectY = lpTextM->tmDigitizedAspectY;
  textM.ntmetm.tmFirstChar = lpTextM->tmFirstChar;
  textM.ntmetm.tmLastChar = lpTextM->tmLastChar;
  textM.ntmetm.tmDefaultChar = lpTextM->tmDefaultChar;
  textM.ntmetm.tmBreakChar = lpTextM->tmBreakChar;
  textM.ntmetm.tmItalic = lpTextM->tmItalic;
  textM.ntmetm.tmUnderlined = lpTextM->tmUnderlined;
  textM.ntmetm.tmStruckOut = lpTextM->tmStruckOut;
  textM.ntmetm.tmPitchAndFamily = lpTextM->tmPitchAndFamily;
  textM.ntmetm.tmCharSet = lpTextM->tmCharSet;
  textM.ntmetm.ntmFlags = 0;
  textM.ntmetm.ntmSizeEM = 0;
  textM.ntmetm.ntmCellHeight = 0;
  textM.ntmetm.ntmAvgWidth = 0;
  memset(&textM.ntmeFontSignature, 0, sizeof(textM.ntmeFontSignature));

  dprintf(("EnumFontProcExW %s", lpLogFont->elfLogFont.lfFaceName));
  return proc(&LogFont, &textM, arg3, lpEnumData->userData);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(int, EnumFontsA,
              HDC, arg1, 
              LPCSTR, arg2, 
              FONTENUMPROCA, arg3, 
              LPARAM,  arg4)
{
  //@@@PH shouldn't this rather be O32_EnumFonts ?
  return EnumFontFamiliesA(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(int, EnumFontsW,
              HDC, arg1, 
              LPCWSTR, arg2,
              FONTENUMPROCW, arg3,
              LPARAM,  arg4)
{
  //@@@PH shouldn't this rather be O32_EnumFonts ?
  return EnumFontFamiliesW(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(int, EnumFontFamiliesA,
              HDC, arg1,
              LPCSTR, arg2,
              FONTENUMPROCA, arg3,
              LPARAM, arg4)
{
  ENUMUSERDATA enumData;
  int rc;

  dprintf(("GDI32: EnumFontFamiliesA %s", arg2));

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;

  rc = O32_EnumFontFamilies(arg1, arg2, &EnumFontProcA, (LPARAM)&enumData);

  return rc;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(int, EnumFontFamiliesW,
              HDC, arg1,
              LPCWSTR, arg2,
              FONTENUMPROCW, arg3,
              LPARAM, arg4)
{
  ENUMUSERDATA enumData;
  int rc;
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);

  dprintf(("GDI32: EnumFontFamiliesW %s", astring));

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;

  rc = O32_EnumFontFamilies(arg1, astring, &EnumFontProcW, (LPARAM)&enumData);

  FreeAsciiString(astring);
  return rc;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(INT, EnumFontFamiliesExA,
              HDC, arg1, 
              LPLOGFONTA, arg2, 
              FONTENUMPROCEXA, arg3, 
              LPARAM, arg4, 
              DWORD, dwFlags)
{
  ENUMUSERDATA enumData;
  int rc;

  dprintf(("GDI32: EnumFontFamiliesExA not complete %s", arg2->lfFaceName));

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;
  enumData.dwFlags  = dwFlags;

  rc = O32_EnumFontFamilies(arg1, arg2->lfFaceName, &EnumFontProcExA, (LPARAM)&enumData);

  return rc;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(INT, EnumFontFamiliesExW,
              HDC, arg1, 
              LPLOGFONTW, arg2,
              FONTENUMPROCEXW, arg3,
              LPARAM, arg4, 
              DWORD, dwFlags)
{
  ENUMUSERDATA enumData;
  int rc;
  char *astring = UnicodeToAsciiString((LPWSTR)arg2->lfFaceName);

  dprintf(("GDI32: EnumFontFamiliesExW not complete %s", astring));

  enumData.userProc = (DWORD)arg3;
  enumData.userData = arg4;
  enumData.dwFlags  = dwFlags;

  rc = O32_EnumFontFamilies(arg1, astring, &EnumFontProcExW, (LPARAM)&enumData);

  FreeAsciiString(astring);
  return rc;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, GetFontData,
              HDC, hdc,
              DWORD, dwTable,
              DWORD, dwOffset,
              LPVOID, lpvBuffer,
              DWORD, dbData)
{
  dprintf(("GDI32: GetFontData, not implemented (GDI_ERROR)\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  return(GDI_ERROR);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(int, AddFontResourceA,
              LPCSTR, szFont)
{
    dprintf(("GDI32: AddFontResourceA %s", szFont));
    return O32_AddFontResource(szFont);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(int, AddFontResourceW,
              LPCWSTR, szFont)
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
ODINFUNCTION1(BOOL, RemoveFontResourceA,
              LPCSTR, arg1)
{
    dprintf(("GDI32: RemoveFontResourceA %s\n", arg1));
    return O32_RemoveFontResource(arg1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, RemoveFontResourceW,
              LPCWSTR, szFont)
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

ODINFUNCTION4(BOOL, CreateScalableFontResourceA,
              DWORD, fdwHidden,
              LPCSTR, lpszFontRes,
              LPCSTR, lpszFontFile,
              LPCSTR, lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceA not implemented.\n"));

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

ODINFUNCTION4(BOOL, CreateScalableFontResourceW,
              DWORD, fdwHidden,
              LPCWSTR, lpszFontRes,
              LPCWSTR, lpszFontFile,
              LPCWSTR, lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceW not implemented.\n"));

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

ODINFUNCTION1(DWORD, GetFontLanguageInfo,
              HDC, hdc)
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
ODINFUNCTION3(BOOL, TranslateCharsetInfo,
              LPDWORD, lpSrc,
              LPCHARSETINFO, lpCs,
              DWORD, flags)
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
