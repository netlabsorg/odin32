/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 language API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 */
#include <os2win.h>
#include <winnls.h>
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
//TODO: Not complete (far from actually)!!
//******************************************************************************
int WIN32API GetLocaleInfoA(LCID lcid, LCTYPE LCType, LPSTR lpLCData, int cchData)
{
  dprintf(("KERNEL32:  OS2GetLocaleInfoA: Not complete!\n"));
  if(cchData == 0)
        return(1);
  *lpLCData = '1';
  return(1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetLocaleInfoW(LCID lcid, LCTYPE LCType, LPWSTR lpLCData, int cchData)
{
  dprintf(("KERNEL32:  OS2GetLocaleInfoA: Not complete!\n"));
  if(cchData == 0)
        return(1*sizeof(USHORT));
  *lpLCData = (USHORT)'1';
  return(sizeof(USHORT));
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


