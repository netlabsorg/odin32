/* $Id: lang.cpp,v 1.7 1999-06-28 16:15:47 phaller Exp $ */

/*
 * Win32 language API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <winnls.h>
#include <stdlib.h>
#include <string.h>
#include <winos2def.h>
#include "unicode.h"


//******************************************************************************
//******************************************************************************
BOOL WIN32API IsValidCodePage(UINT CodePage)
{
  dprintf(("KERNEL32:  IsValidCodePage not implemented\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
LCID WIN32API GetUserDefaultLCID(void)
{
  dprintf(("KERNEL32:  OS2GetUserDefaultLCID: always retuns US English!\n"));
  return(0x409);        //US English
}
//******************************************************************************
//******************************************************************************
LCID WIN32API GetSystemDefaultLCID(void)
{
  dprintf(("KERNEL32:  OS2GetSystemDefaultLCID: always retuns US English!\n"));
  return(0x409);        //US English
}
//******************************************************************************
//******************************************************************************
LANGID WIN32API GetUserDefaultLangID()
{
  dprintf(("KERNEL32:  OS2GetUserDefaultLangID, always returns US English\n"));
  return(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
}
//******************************************************************************
//******************************************************************************
LANGID WIN32API GetSystemDefaultLangID(void)
{
  dprintf(("KERNEL32:  OS2GetSystemDefaultLangID, always returns US English\n"));
  return(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
}
//******************************************************************************
//******************************************************************************
int WIN32API GetLocaleInfoA(LCID lcid, LCTYPE LCType, LPSTR buf, int len)
{
  COUNTRYCODE  Country    = {0};
  COUNTRYINFO  CtryInfo   = {0};
  ULONG        ulInfoLen  = 0;

  dprintf(("KERNEL32:  OS2GetLocaleInfoA: Not complete!\n"));
  if (len && (! buf) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return 0;
  }

  if(!buf) // @@@PH: which result does make sense without a buffer ?
    return 0; // nevertheless, OPERA.EXE calls it with NULL ?!

  // Only standard. TODO: Use os2.ini PM_National section
  DosQueryCtryInfo(sizeof(CtryInfo), &Country, &CtryInfo, &ulInfoLen);

  LCType &= ~(LOCALE_NOUSEROVERRIDE|LOCALE_USE_CP_ACP);

  switch(LCType)
  {
    case LOCALE_SDECIMAL:
      if(buf) *buf = CtryInfo.szDecimal[0];
      return 1;
    case LOCALE_SDATE:
      if(buf) *buf = CtryInfo.szDateSeparator[0];
      return 1;
    case LOCALE_STIME:
      if(buf) *buf = CtryInfo.szTimeSeparator[0];
      return 1;
    case LOCALE_STHOUSAND:
      if(buf) *buf = CtryInfo.szThousandsSeparator[0];
      return 1;
    case LOCALE_SCURRENCY:
      if(len > strlen(CtryInfo.szCurrency))
      {
        strcpy(buf, CtryInfo.szCurrency);
        return (strlen(buf) + 1);
      }
      else
        break;
    case LOCALE_SSHORTDATE:
      if(CtryInfo.fsDateFmt == 0)
      {
        if(len > 8)
        {
          strcpy(buf, "MMXddXyy");
          buf[2] = buf[5] = CtryInfo.szDateSeparator[0];
          return 9;
        }
        else
          break;
      }
      else if(CtryInfo.fsDateFmt == 1)
      {
        if(len > 8)
        {
          strcpy(buf, "ddXMMXyy");
          buf[2] = buf[5] = CtryInfo.szDateSeparator[0];
          return 9;
        }
        else
          break;
      }
      else /* if(CtryInfo.fsDateFmt == 2) */
      {
        if(len > 8)
        {
          strcpy(buf, "yyXMMXdd");
          buf[2] = buf[5] = CtryInfo.szDateSeparator[0];
          return 9;
        }
        else
          break;
      }
    case LOCALE_STIMEFORMAT:
      if(CtryInfo.fsTimeFmt == 0)
      {
        if(len > 8)
        {
          strcpy(buf, "HHXmmXss");
          buf[2] = buf[5] = CtryInfo.szTimeSeparator[0];
          return 9;
        }
        else
          break;
      }
      else /* if(CtryInfo.fsTimeFmt == 1) */
      {
        if(len > 8)
        {
          strcpy(buf, "HHXmmXss");
          buf[2] = buf[5] = CtryInfo.szTimeSeparator[0];
          return 9;
        }
        else
          break;
      }
    case LOCALE_S1159:
      if(CtryInfo.fsTimeFmt == 0)
      {
        if(len > 2)
        {
          strcpy(buf, "AM");
          return 3;
        }
        else
          break;
      }
      *buf = 0;
      return 1;
    case LOCALE_S2359:
      if(CtryInfo.fsTimeFmt == 0)
      {
        if(len > 2)
        {
          strcpy(buf, "PM");
          return 3;
        }
        else
          break;
      }
      if(buf) *buf = 0;
      return 1;
/***
    LOCALE_SABBREVMONTHNAME11:
    LOCALE_SABBREVMONTHNAME12:
    LOCALE_SABBREVMONTHNAME13:
    LOCALE_SPOSITIVESIGN:
    LOCALE_SNEGATIVESIGN:
    LOCALE_IPOSSIGNPOSN:
    LOCALE_INEGSIGNPOSN:
    LOCALE_IPOSSYMPRECEDES:
    LOCALE_IPOSSEPBYSPACE:
    LOCALE_INEGSYMPRECEDES:
    LOCALE_INEGSEPBYSPACE:
    LOCALE_FONTSIGNATURE:
    LOCALE_SISO639LANGNAME:
    LOCALE_SISO3166CTRYNAME:
***/
    default:
      dprintf(("KERNEL32:  OS2GetLocaleInfoA: LCType %d not yet supported\n", LCType));
      if(buf) *buf = '1';
      return(1);
  }

  // a 'break' in 'switch(LCType)': buffer too small
  SetLastError(ERROR_INSUFFICIENT_BUFFER);
  return 0;

}
//******************************************************************************
//******************************************************************************
int WIN32API GetLocaleInfoW(LCID lcid, LCTYPE LCType, LPWSTR wbuf, int len)
{
  WORD wlen;
  char *abuf;

  dprintf(("KERNEL32:  OS2GetLocaleInfoW\n"));
  if (len && (! wbuf) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return 0;
  }
  abuf = (char *) malloc(len);
  wlen = GetLocaleInfoA(lcid, LCType, abuf, len);

  if (wlen && len)  // no check of wbuf length !!
    AsciiToUnicodeN(abuf, wbuf, wlen);

  free(abuf);
  return wlen;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsValidLocale(LCID Locale, DWORD dwFlags)
{
  dprintf(("KERNEL32:  OS2IsValidLocale, always returns TRUE\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
LCID WIN32API GetThreadLocale()
{
  dprintf(("KERNEL32:  OS2GetThreadLocale always return US English!\n"));
  return(0x409);        //US English
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetThreadLocale( LCID locale )
{
  dprintf(("KERNEL32:  OS2SetThreadLocale not implemented!\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumSystemLocalesA(LOCALE_ENUMPROCA lpLocaleEnumProc,
                                    DWORD dwFlags)
{
  dprintf(("EnumSystemLocalesA %X %X\n", lpLocaleEnumProc, dwFlags));
  if(lpLocaleEnumProc == NULL || ((dwFlags & LCID_INSTALLED) && (dwFlags & LCID_SUPPORTED))) {
        dprintf(("Invalid parameter\n"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
  }
  lpLocaleEnumProc("OS/2 US English");
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumSystemLocalesW(LOCALE_ENUMPROCW lpLocaleEnumProc,
                                    DWORD            dwFlags)
{
 WCHAR locStr[32];

  dprintf(("EnumSystemLocalesW %X %X\n", lpLocaleEnumProc, dwFlags));
  if(lpLocaleEnumProc == NULL || ((dwFlags & LCID_INSTALLED) && (dwFlags & LCID_SUPPORTED))) {
        dprintf(("Invalid parameter\n"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
  }
  AsciiToUnicode("OS/2 US English", locStr);
  lpLocaleEnumProc(locStr);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************


/*****************************************************************************
 * Name      : BOOL SetLocaleInfoA
 * Purpose   : The SetLocaleInfoA function sets an item of locale information.
 *             It does so by making an entry in the process portion of the
 *             locale table. This setting only affects the user override portion
 *             of the locale settings; it does not set the system defaults.
 *             Only certain types of locale information, or LCTYPE values, can
 *             be set by this function. See the following Remarks section for a
 *             list of valid LCTYPE values.
 *             The locale information is always passed in as a null-terminated
 *             Unicode string in the Unicode (W) version of the function, and as
 *             a null-terminated ANSI string in the ANSI (A) version. No integers
 *             are allowed by this function; any numeric values must be specified
 *             as Unicode or ANSI text. Each LCTYPE has a particular format, as
 *             noted in Locale Identifiers.
 * Parameters: LCID    Locale   locale identifier
 *             LCTYPE  LCType   type of information to set
 *             LPCTSTR lpLCData pointer to information to set
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetLocaleInfoA(LCID   Locale,
                                LCTYPE LCType,
                                LPCSTR lpLCData)
{
  dprintf(("KERNEL32: SetLocaleInfoA(%08xh,%08xh,%08xh) not implemented.\n",
           Locale,
           LCType,
           lpLCData));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetLocaleInfoW
 * Purpose   : The SetLocaleInfoW function sets an item of locale information.
 *             It does so by making an entry in the process portion of the
 *             locale table. This setting only affects the user override portion
 *             of the locale settings; it does not set the system defaults.
 *             Only certain types of locale information, or LCTYPE values, can
 *             be set by this function. See the following Remarks section for a
 *             list of valid LCTYPE values.
 *             The locale information is always passed in as a null-terminated
 *             Unicode string in the Unicode (W) version of the function, and as
 *             a null-terminated ANSI string in the ANSI (A) version. No integers
 *             are allowed by this function; any numeric values must be specified
 *             as Unicode or ANSI text. Each LCTYPE has a particular format, as
 *             noted in Locale Identifiers.
 * Parameters: LCID    Locale   locale identifier
 *             LCTYPE  LCType   type of information to set
 *             LPCTSTR lpLCData pointer to information to set
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetLocaleInfoW(LCID    Locale,
                                LCTYPE  LCType,
                                LPCWSTR lpLCData)
{
  dprintf(("KERNEL32: SetLocaleInfoW(%08xh,%08xh,%08xh) not implemented.\n",
           Locale,
           LCType,
           lpLCData));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD VerLanguageNameA
 * Purpose   : The VerLanguageNameA function converts the specified binary
 *             Microsoft language identifier into a text representation of the language.
 * Parameters: DWORD  idLang   Microsoft language identifier
 *             LPTSTR lpszLang address of buffer for language string
 *             DWORD  cbLang   size of buffer
 * Variables :
 * Result    : size of target buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API VerLanguageNameA(UINT  idLang,
                                LPSTR lpszLang,
                                UINT  cbLang)
{
  dprintf(("KERNEL32: VerLanguageNameA(%08x,%08x,%08x) not implemented.\n",
           idLang,
           lpszLang,
           cbLang));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD VerLanguageNameW
 * Purpose   : The VerLanguageNameW function converts the specified binary
 *             Microsoft language identifier into a text representation of the language.
 * Parameters: DWORD  idLang   Microsoft language identifier
 *             LPTSTR lpszLang address of buffer for language string
 *             DWORD  cbLang   size of buffer
 * Variables :
 * Result    : size of target buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API VerLanguageNameW(UINT  idLang,
                                LPWSTR lpszLang,
                                UINT  cbLang)
{
  dprintf(("KERNEL32: VerLanguageNameW(%08x,%08x,%08x) not implemented.\n",
           idLang,
           lpszLang,
           cbLang));

  return (0);
}


