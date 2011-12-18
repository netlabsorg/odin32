/* $Id: lang.cpp,v 1.44 2004-01-29 14:21:50 bird Exp $ */
/*
 * Win32 language API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1999 Przemyslaw Dobrowolski
 * Copyright 2004 InnoTek Systemberatung GmbH
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <winnls.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odinwrap.h>
#include <heapstring.h>
#include <win\winreg.h>
#include <winos2def.h>
#include "unicode.h"
#include "oslibmisc.h"
#include <wprocess.h>
#include <uniconv.h>

#define DBG_LOCALLOG	DBG_lang
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-LANG)

static ULONG gulDefaultLanguageId = 0;

//******************************************************************************
//******************************************************************************
void WIN32API SetDefaultLanguage(DWORD deflang)
{
  gulDefaultLanguageId = deflang;
}
//******************************************************************************
//******************************************************************************
static ULONG DeterminDefaultLanguageId(void)
{
    const char *pszLang = getenv("LANG");
    if (pszLang && pszLang[0] && pszLang[1])   /* (Ignore one character values (like 'C').) */
    {
        /** Conversion table from LANG to LanguageId based upon vac365 locales,
         * Sun JDK Internationalization Docs (1.2), good friends and google.
         * LANG value format: <language id>_<region>.[stuff]
         *
         * If no region is specified in LANG, we'll search exact match, and if not
         * found <val>_<val>. So for values where language id doesn't match region
         * then add a 2nd line with only language id (if there is such a default).
         */
        const static struct
        {
            const char *pszLang;    /* LANG value. (case insensitive) */
            ULONG       ulLangId;   /* Win32 Language ID. */
        }   aLangToLangId[] =
        {
            /* most frequent */
            { "EN_US",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) },
            { "DE_DE",      MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN) },
            /* now sorted */
            { "AR_AE",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_UAE) },
            { "AR_BH",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_BAHRAIN) },
            { "AR_DZ",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_ALGERIA) },
            { "AR",         MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_EGYPT) },
            { "AR_EG",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_EGYPT) },
            { "AR_IQ",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_IRAQ        ) },
            { "AR_JO",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_JORDAN      ) },
            { "AR_KW",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_KUWAIT      ) },
            { "AR_LB",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_LEBANON     ) },
            { "AR_LY",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_LIBYA       ) },
            { "AR_MA",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_MOROCCO     ) },
            { "AR_OM",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_OMAN        ) },
            { "AR_QA",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_QATAR       ) },
            { "AR_SA",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA) },
            { "AR_SY",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SYRIA       ) },
            { "AR_TN",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_TUNISIA     ) },
            { "AR_YE",      MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_YEMEN       ) },
            { "AR_SD",      MAKELANGID(LANG_ARABIC, SUBLANG_DEFAULT) }, /* no win32 constant. */
            { "BE",         MAKELANGID(LANG_BELARUSIAN, SUBLANG_DEFAULT) },
            { "BE_BY",      MAKELANGID(LANG_BELARUSIAN, SUBLANG_DEFAULT) },
            { "BG_BG",      MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT) },
            { "CA",         MAKELANGID(LANG_CATALAN, SUBLANG_DEFAULT) },
            { "CA_ES",      MAKELANGID(LANG_CATALAN, SUBLANG_DEFAULT) },
            { "CS",         MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT) },
            { "CS_CZ",      MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT) },
            { "DA",         MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT) },
            { "DA_DK",      MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT) },
            { "DE_AT",      MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_AUSTRIAN) },
            { "DE_CH",      MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_SWISS) },
            { "DE_LU",      MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LUXEMBOURG) },
            { "EL",         MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT) },
            { "EL_GR",      MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT) },
            { "EN_AU",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_AUS) },
            { "EN_CA",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN) },
            { "EN_GB",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK) },
            { "EN_IE",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_EIRE) },
            { "EN_NZ",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_NZ) },
            { "EN_ZA",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_SOUTH_AFRICA) },
            { "EN_DK",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) }, /* odd ibm vac365 */
            { "EN_JP",      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) }, /* odd ibm vac365 */
            { "ES_AR",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ARGENTINA         ) },
            { "ES_BO",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_BOLIVIA           ) },
            { "ES_CL",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_CHILE             ) },
            { "ES_CO",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COLOMBIA          ) },
            { "ES_CR",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COSTA_RICA        ) },
            { "ES_DO",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_DOMINICAN_REPUBLIC) },
            { "ES_EC",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ECUADOR           ) },
            { "ES_ES",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH                   ) }, /* SUBLANG_SPANISH_MODERN? */
            { "ES_GT",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_GUATEMALA         ) },
            { "ES_HN",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_HONDURAS          ) },
            { "ES_MX",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MEXICAN           ) },
            { "ES_NI",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_NICARAGUA         ) },
            { "ES_PA",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PANAMA            ) },
            { "ES_PE",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PERU              ) },
            { "ES_PR",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PUERTO_RICO       ) },
            { "ES_PY",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PARAGUAY          ) },
            { "ES_SV",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_EL_SALVADOR       ) },
            { "ES_UY",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_URUGUAY           ) },
            { "ES_VE",      MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_VENEZUELA         ) },
            { "ET",         MAKELANGID(LANG_ESTONIAN, SUBLANG_DEFAULT) },
            { "ET_EE",      MAKELANGID(LANG_ESTONIAN, SUBLANG_DEFAULT) },
            { "FI_FI",      MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT) },
            { "FO",         MAKELANGID(LANG_FAEROESE, SUBLANG_DEFAULT) },
            { "FO_FO",      MAKELANGID(LANG_FAEROESE, SUBLANG_DEFAULT) },
            { "FR_BE",      MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_BELGIAN) },
            { "FR_CA",      MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_CANADIAN) },
            { "FR_CH",      MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_SWISS) },
            { "FR_FR",      MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH) },
            { "FR_LU",      MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_LUXEMBOURG) },
            { "HR_HR",      MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT) },
            { "HU_HU",      MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT) },
            { "IS_IS",      MAKELANGID(LANG_ICELANDIC, SUBLANG_DEFAULT) },
            { "IT_CH",      MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN_SWISS) },
            { "IT_IT",      MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN) },
            { "IW",         MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT) },
            { "IW_IL",      MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT) },
            { "JA_JP",      MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT) },
            { "KO",         MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT) },
            { "KO_KR",      MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT) },
            { "LT_LT",      MAKELANGID(LANG_LITHUANIAN, SUBLANG_DEFAULT) },
            { "LV_LV",      MAKELANGID(LANG_LATVIAN, SUBLANG_DEFAULT) },
            /*{ "MK_MK",      MAKELANGID(LANG_MACEDONIAN, SUBLANG_DEFAULT) }, - no win32 eq */
            { "NL_BE",      MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH_BELGIAN) },
            { "NL_NL",      MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH) },
            { "NO_NO",      MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_NYNORSK) }, /* yeah sure! Sun is medling in norwegian politics here... */
            { "NO_NO_B",    MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL) },  /* yeah sure! Sun is medling in norwegian politics here... */
            { "NY",         MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_NYNORSK) },
            { "NY_NO",      MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_NYNORSK) },
            { "NB",         MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL) },
            { "NB_NO",      MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL) },
            { "PL_PL",      MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT) },
            { "PT_BR",      MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN) },
            { "PT_PT",      MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE) },
            { "RO_RO",      MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT) },
            { "RU_RU",      MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT) },
            { "SH",         MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_CYRILLIC) },
            { "SH_YU",      MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_CYRILLIC) },
            { "SK_SK",      MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT) },
            { "SL_SL",      MAKELANGID(LANG_SLOVENIAN, SUBLANG_DEFAULT) },
            { "SQ",         MAKELANGID(LANG_ALBANIAN, SUBLANG_DEFAULT) },
            { "SQ_AL",      MAKELANGID(LANG_ALBANIAN, SUBLANG_DEFAULT) },
            { "SR",         MAKELANGID(LANG_SERBIAN, SUBLANG_DEFAULT) },
            { "SR_YU",      MAKELANGID(LANG_SERBIAN, SUBLANG_DEFAULT) },
            { "SV",         MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH) },
            { "SV_FI",      MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH_FINLAND) },
            { "SV_SE",      MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH) },
            { "TH_TH",      MAKELANGID(LANG_THAI, SUBLANG_DEFAULT) },
            { "TR_TR",      MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT) },
            { "TR_DE",      MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT) }, /* phun ;-) */
            { "UK",         MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT) },
            { "UK_UA",      MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT) },
            { "ZH",         MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) },
            { "ZH_CN",      MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED) },
            { "ZH_TW",      MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL) },
            { "ZH_HK",      MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_HONGKONG) },
            /*{ "ZH_??",      MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SINGAPORE) },*/
        };

        /*
         * Strip off anything past the '.' and any '_EURO'.
         */
        char szLang[16];
        strncpy(szLang, pszLang, sizeof(szLang));
        szLang[sizeof(szLang) - 1] = '\0';
        char *psz = strrchr(szLang, '.');
        if (psz)
            *psz = '\0';
        strupr(szLang);
        psz = strstr(szLang, "_EURO");
        if (psz)
            *psz = '\0';

        /*
         * Loop thru the table looking for 'exact' match.
         */
        int i;
        for (i = 0; i < sizeof(aLangToLangId) / sizeof(aLangToLangId[0]); i++)
            if (!strcmp(aLangToLangId[i].pszLang, szLang))
                return aLangToLangId[i].ulLangId;

        /*
         * No exact match. We'll now try look for <langid> and then for <langid>_<langid>.
         * ASSUMES: no user sets "LANG=e" or any other on character letter.
         */
        szLang[2] = '\0';
        for (i = 0; i < sizeof(aLangToLangId) / sizeof(aLangToLangId[0]); i++)
            if (!strcmp(aLangToLangId[i].pszLang, szLang))
                return aLangToLangId[i].ulLangId;

        szLang[2] = '_';
        szLang[3] = szLang[0];
        szLang[4] = szLang[1];
        szLang[5] = '\0';
        for (i = 0; i < sizeof(aLangToLangId) / sizeof(aLangToLangId[0]); i++)
            if (!strcmp(aLangToLangId[i].pszLang, szLang))
                return aLangToLangId[i].ulLangId;

        /*
         * No match, let's try find the language if nothing else.
         */
        for (i = 0; i < sizeof(aLangToLangId) / sizeof(aLangToLangId[0]); i++)
            if (!strncmp(aLangToLangId[i].pszLang, szLang, 2))
                return MAKELANGID(PRIMARYLANGID(aLangToLangId[i].ulLangId), SUBLANG_NEUTRAL);

        /* we give up! */
        dprintf(("DeterminDefaultLanguageId: WARNING: LANG value '%s' isn't known to us!!!\n", pszLang));
        DebugInt3();
    }


    /*
     * If LANG parsing failes, we'll go for the country info.
     */
    switch(OSLibQueryCountry())
    {
        case CTRY_AUSTRALIA:        return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_AUS);
        case CTRY_AUSTRIA:          return MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_AUSTRIAN);
        case CTRY_BELGIUM:          return MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH_BELGIAN);    //TODO: french-belgian
        case CTRY_BRAZIL:           return MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN);
        case CTRY_BULGARIA:         return MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT);
        case CTRY_CANADA:           return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN);       //TODO: french-canadian
        case CTRY_CHINA:            return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL);
        case CTRY_CROATIA:          return MAKELANGID(LANG_CROATIAN, SUBLANG_CROATIAN);
        case CTRY_CZECH_REPUBLIC:   return MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT);
        case CTRY_DENMARK:          return MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT);
        case CTRY_FINLAND:          return MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT);
        case CTRY_FRANCE:           return MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
        case CTRY_GERMANY:          return MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN);
        case CTRY_GREECE:           return MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT);
        case CTRY_HONG_KONG:        return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_HONGKONG);
        case CTRY_ICELAND:          return MAKELANGID(LANG_ICELANDIC, SUBLANG_DEFAULT);
        case CTRY_IRELAND:          return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_EIRE);
        case CTRY_ITALY:            return MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN);
        case CTRY_JAPAN:            return MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
        case CTRY_KOREA:            return MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT);
        case CTRY_LATIN_AMERICA:    return MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH);
        case CTRY_LUXEMBOURG:       return MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LUXEMBOURG);
        case CTRY_MEXICO:           return MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MEXICAN);
        case CTRY_NETHERLANDS:      return MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH);
        case CTRY_NEW_ZEALAND:      return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_NZ);
        case CTRY_NORWAY:           return MAKELANGID(LANG_NORWEGIAN, SUBLANG_DEFAULT);
        case CTRY_POLAND:           return MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT);
        case CTRY_PORTUGAL:         return MAKELANGID(LANG_PORTUGUESE, SUBLANG_DEFAULT);
        case CTRY_RUSSIA:           return MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN);
        case CTRY_SINGAPORE:        return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SINGAPORE);
        case CTRY_SLOVAK_REPUBLIC:  return MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT);
        case CTRY_SLOVENIA:         return MAKELANGID(LANG_SLOVENIAN, SUBLANG_DEFAULT);
        case CTRY_SPAIN:            return MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH);
        case CTRY_SWEDEN:           return MAKELANGID(LANG_SWEDISH, SUBLANG_DEFAULT);
        case CTRY_SWITZERLAND:      return MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_SWISS);
        case CTRY_TAIWAN:           return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL);
        case CTRY_TURKEY:           return MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT);
        case CTRY_UKRAINE:          return MAKELANGID(LANG_UKRAINIAN, SUBLANG_RUSSIAN);
        case CTRY_UNITED_KINGDOM:   return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK);
        default:
            return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK);
    }
    /* will never reach this point! */
}

ULONG GetLanguageId(void)
{
    /*
     * If no default language is set we'll try determin that by evaluating the
     * LANG env.var. and OS/2 country information.
     */
    if (gulDefaultLanguageId == 0)
    {
        gulDefaultLanguageId = DeterminDefaultLanguageId();
        dprintf(("KERNEL32: GetLanguageId: Sets default language id to %#x\n", gulDefaultLanguageId));
    }
    return gulDefaultLanguageId;
}
//******************************************************************************
//******************************************************************************
LCID WIN32API GetUserDefaultLCID(void)
{
  dprintf2(("KERNEL32: GetUserDefaultLCID: returns %x", MAKELCID(GetLanguageId(), SORT_DEFAULT)));
  return(MAKELCID(GetLanguageId(), SORT_DEFAULT));
}
//******************************************************************************
//******************************************************************************
LCID WIN32API GetSystemDefaultLCID(void)
{
  dprintf2(("KERNEL32: GetSystemDefaultLCID: returns %x", GetUserDefaultLCID()));
  return GetUserDefaultLCID();
}
//******************************************************************************
//******************************************************************************
LANGID WIN32API GetUserDefaultLangID()
{
  dprintf2(("KERNEL32: GetUserDefaultLangID, returns %x", GetLanguageId()));
  return(GetLanguageId());
}
//******************************************************************************
//******************************************************************************
LANGID WIN32API GetSystemDefaultLangID(void)
{
  dprintf2(("KERNEL32: GetSystemDefaultLangID returns %x", GetUserDefaultLangID()));
  return GetUserDefaultLangID();
}

//******************************************************************************
//******************************************************************************
 LANGID WIN32API GetUserDefaultUILanguage(void)
{
  dprintf2(("KERNEL32: GetSystemDefaultLangID returns %x", GetUserDefaultLangID()));
  return GetUserDefaultLangID();
}
//******************************************************************************
//******************************************************************************
static BOOL LocaleFromUniStr(LPWSTR lpUniStr, LPWSTR wbuf, ULONG *pLen)
{
  if (wbuf)
  {
    if (*pLen > lstrlenW(lpUniStr))
      lstrcpyW(wbuf,lpUniStr);
    else
    {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      *pLen=0;
      return FALSE;
    }
  }

  *pLen=lstrlenW(lpUniStr)+1; // + null terminator

  return TRUE;
}
//replaced by Wine functions in ole2nls.c
#if 0
//******************************************************************************
//******************************************************************************
static BOOL LocaleFromUniChar(WCHAR wcUniChar, LPWSTR wbuf, ULONG *pLen)
{
  if (wbuf)
  {
//    if (*pLen >= sizeof(WCHAR))
    if (*pLen >= 2)
    {
      wbuf[0]=(WCHAR)'0'+wcUniChar;
      wbuf[1]=0;
    }
    else
    {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      *pLen=0;
      return FALSE;
    }
  }

  *pLen=2;

  return TRUE;
}

/*****************************************************************************
 * Name      : DWORD GetLocaleInfoW
 * Purpose   : The GetLocaleInfoW function returns specific locale information
 * Parameters: LCID   lcid    locale identifier
 *             LCTYPE LCType  type of information
 *             LPSTRW wbuf    address of buffer for information
 *             int    len     size of buffer
 * Variables :
 * Result    : size of target buffer
 * Remark    : In this version LOCALE_SYSTEM_DEFAULT == LOCALE_USER_DEFAULT
 *
 * LOCALE_STHOUSAND   - with some languages returns OS/2 separator not Windows!
 * LOCALE_IMEASURE    - return '0' like English US
 * LOCALE_INEGNUMBER  - return '1' like English US
 * LOCALE_ICURRDIGITS - return '2' like English US
 * LOCALE_IDATE are identical as LOCALE_ILDATE (must be fixed?)
 * LOCALE_SGROUPING and
 * LOCALE_SMONGROUPING are tested only with Polish & English languages
 * LOCALE_SMONTHNAME13 and
 * LOCALE_SABBERVMONTHNAME13 - returns empty string like English US
 * LOCALE_FONTSIGNATURE - return empty string like English US
 *
 * Status    : FULLY TESTED
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/07/22 17:07]
 *****************************************************************************/
int WIN32API GetLocaleInfoW(LCID lcid, LCTYPE LCType, LPWSTR wbuf,
                            int len)
{
  LocaleObject    locale_object = NULL;
  struct UniLconv *puni_lconv = NULL;
  WCHAR           *pInfoItem;
  int             rc;
  COUNTRYCODE     Country    = {0};
  COUNTRYINFO     CtryInfo   = {0};
  ULONG           ulInfoLen  = 0;

  dprintf(("KERNEL32:  OS2GetLocaleInfoW lcID=%d,lcType=%x,wbuf=%X,len=%d\n",lcid,LCType,wbuf,len));

  if (len && (! wbuf) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return 0;
  }

  if (wbuf) *wbuf==0;

  rc=UniCreateLocaleObject(UNI_UCS_STRING_POINTER,(UniChar *)L"", &locale_object);

  if ( rc == ULS_SUCCESS )
  {
    UniQueryLocaleInfo(locale_object, &puni_lconv);

    DosQueryCtryInfo(sizeof(COUNTRYINFO), &Country, &CtryInfo, &ulInfoLen);

    ulInfoLen = len; // Now we use this variable as returned length info

    LCType &= ~(LOCALE_NOUSEROVERRIDE|LOCALE_USE_CP_ACP);

    switch(LCType)
    {
      case LOCALE_SDECIMAL:
        LocaleFromUniStr(puni_lconv->decimal_point,wbuf,&ulInfoLen);
        break;

      case LOCALE_IMEASURE: // Fixme!
        // only like English US
        LocaleFromUniChar(0,wbuf,&ulInfoLen);
        break;

      case LOCALE_INEGNUMBER: // Fixme!
        // only like English US
        LocaleFromUniChar(1,wbuf,&ulInfoLen);
        break;

      case LOCALE_ICURRDIGITS:// Fixme!
        // only like English US
        LocaleFromUniChar(2,wbuf,&ulInfoLen);
        break;

       case LOCALE_STHOUSAND:
        LocaleFromUniStr(puni_lconv->thousands_sep,wbuf,&ulInfoLen);
        //MN: hack for Czech language; weird value returned here (0xA0 instead of 0x20)
        if (gulDefaultLanguageId == CTRY_CZECH_REPUBLIC)
           ((BYTE*)wbuf)[0] = ((BYTE*)wbuf)[0] & 0x7F;
        break;

      case LOCALE_SGROUPING: // tested only with Polish & English
        if (wbuf)
        {
          if (puni_lconv->grouping)
          {
            if (len >= 3*sizeof(WCHAR))
            {
              wbuf[0]='0'+*puni_lconv->grouping;
              wbuf[1]=(WCHAR)';';
              wbuf[2]=(WCHAR)'0';
              wbuf[3]=0;
              ulInfoLen=4;
            }
            else
            {
              SetLastError(ERROR_INSUFFICIENT_BUFFER);
              break;
            }
          }
          else
          {
            wbuf[0]=20; // Windows return space!!
            ulInfoLen=2;
          }
        }
        else
            ulInfoLen=2;
        break;

      case LOCALE_SMONGROUPING: // tested only with Polish & English
        if (wbuf)
        {
          if (puni_lconv->mon_grouping)
          {
            if (len >= 3*sizeof(WCHAR))
            {
              wbuf[0]='0'+*puni_lconv->mon_grouping;
              wbuf[1]=(WCHAR)';';
              wbuf[2]=(WCHAR)'0';
              wbuf[3]=0;
              ulInfoLen=4;
            }
            else
            {
              SetLastError(ERROR_INSUFFICIENT_BUFFER);
              break;
            }
          }
          else
          {
            wbuf[0]=20; // Windows return space!!
            ulInfoLen=2;
          }
        }
        else
            ulInfoLen=2;
        break;


      case LOCALE_SCURRENCY:
        LocaleFromUniStr(puni_lconv->currency_symbol,wbuf,&ulInfoLen);
        break;

      case LOCALE_SMONDECIMALSEP:
        LocaleFromUniStr(puni_lconv->mon_decimal_point,wbuf,&ulInfoLen);
        break;

      case LOCALE_SMONTHOUSANDSEP:
        LocaleFromUniStr(puni_lconv->mon_thousands_sep,wbuf,&ulInfoLen);
        break;

      case LOCALE_SPOSITIVESIGN:
        LocaleFromUniStr(puni_lconv->positive_sign,wbuf,&ulInfoLen);
        break;

      case LOCALE_SNEGATIVESIGN:
        LocaleFromUniStr(puni_lconv->negative_sign,wbuf,&ulInfoLen);
        break;

      case LOCALE_IPOSSIGNPOSN:
        LocaleFromUniChar(puni_lconv->p_sign_posn,wbuf,&ulInfoLen);
        break;

      case LOCALE_INEGSIGNPOSN:
        LocaleFromUniChar(puni_lconv->n_sign_posn,wbuf,&ulInfoLen);
        break;

      case LOCALE_IPOSSYMPRECEDES:
        LocaleFromUniChar(puni_lconv->p_cs_precedes,wbuf,&ulInfoLen);
        break;

      case LOCALE_IPOSSEPBYSPACE:
        LocaleFromUniChar(puni_lconv->p_sep_by_space,wbuf,&ulInfoLen);
        break;

      case LOCALE_INEGSYMPRECEDES:
        LocaleFromUniChar(puni_lconv->n_cs_precedes,wbuf,&ulInfoLen);
        break;

      case LOCALE_INEGSEPBYSPACE:
        LocaleFromUniChar(puni_lconv->n_sep_by_space,wbuf,&ulInfoLen);
        break;

      case LOCALE_S1159:
        UniQueryLocaleItem(locale_object, AM_STR,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_S2359:
        UniQueryLocaleItem(locale_object, PM_STR,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_IDEFAULTLANGUAGE:
        strcpyW(wbuf, (LPWSTR)L"0409");
        ulInfoLen = 5;
        break;
      case LOCALE_ILANGUAGE:
        UniQueryLocaleItem(locale_object, XWINLOCALE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_INEGCURR:
        UniQueryLocaleItem(locale_object, INEGCURR,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_ICURRENCY:
        UniQueryLocaleItem(locale_object, ICURRENCY,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;



      case LOCALE_SNATIVELANGNAME:
      case LOCALE_SLANGUAGE:
        UniQueryLocaleItem(locale_object, SLANGUAGE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVLANGNAME:
        UniQueryLocaleItem(locale_object, SABBREVLANGNAME,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_IDEFAULTCOUNTRY:
      case LOCALE_ICOUNTRY:
        UniQueryLocaleItem(locale_object, ICOUNTRY,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SENGLANGUAGE:
        UniQueryLocaleItem(locale_object, SENGLANGUAGE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SENGCOUNTRY:
      case LOCALE_SCOUNTRY:
        UniQueryLocaleItem(locale_object, SENGCOUNTRYNAME,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVCTRYNAME:
        UniQueryLocaleItem(locale_object, SABBREVCTRYNAME,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SNATIVECTRYNAME:
        UniQueryLocaleItem(locale_object, SCOUNTRY,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SNATIVEDIGITS:
        UniQueryLocaleItem(locale_object, SNATIVEDIGITS,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;


      case LOCALE_IDEFAULTCODEPAGE:
        UniQueryLocaleItem(locale_object, ICODEPAGE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_IDEFAULTANSICODEPAGE:
        UniQueryLocaleItem(locale_object, IANSICODEPAGE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_IDEFAULTMACCODEPAGE:
        UniQueryLocaleItem(locale_object, IMACCODEPAGE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SLIST:
        UniQueryLocaleItem(locale_object, LISTSEP,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SINTLSYMBOL:
        UniQueryLocaleItem(locale_object, SINTLSYMBOL,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_IFIRSTDAYOFWEEK:
        UniQueryLocaleItem(locale_object, IFIRSTDAYOFWEEK, &pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDAYNAME7:
        UniQueryLocaleItem(locale_object, DAY_1,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDAYNAME1:
        UniQueryLocaleItem(locale_object, DAY_2,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDAYNAME2:
        UniQueryLocaleItem(locale_object, DAY_3,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDAYNAME3:
        UniQueryLocaleItem(locale_object, DAY_4,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDAYNAME4:
        UniQueryLocaleItem(locale_object, DAY_5,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDAYNAME5:
        UniQueryLocaleItem(locale_object, DAY_6,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDAYNAME6:
        UniQueryLocaleItem(locale_object, DAY_7,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVDAYNAME7:
        UniQueryLocaleItem(locale_object, ABDAY_1,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVDAYNAME1:
        UniQueryLocaleItem(locale_object, ABDAY_2,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVDAYNAME2:
        UniQueryLocaleItem(locale_object, ABDAY_3,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVDAYNAME3:
        UniQueryLocaleItem(locale_object, ABDAY_4,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVDAYNAME4:
        UniQueryLocaleItem(locale_object, ABDAY_5,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVDAYNAME5:
        UniQueryLocaleItem(locale_object, ABDAY_6,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVDAYNAME6:
        UniQueryLocaleItem(locale_object, ABDAY_7,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME1:
        UniQueryLocaleItem(locale_object, MON_1,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME2:
        UniQueryLocaleItem(locale_object, MON_2,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME3:
        UniQueryLocaleItem(locale_object, MON_3,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME4:
        UniQueryLocaleItem(locale_object, MON_4,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME5:
        UniQueryLocaleItem(locale_object, MON_5,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME6:
        UniQueryLocaleItem(locale_object, MON_6,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME7:
        UniQueryLocaleItem(locale_object, MON_7,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME8:
        UniQueryLocaleItem(locale_object, MON_8,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME9:
        UniQueryLocaleItem(locale_object, MON_9,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME10:
        UniQueryLocaleItem(locale_object, MON_10,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME11:
        UniQueryLocaleItem(locale_object, MON_11,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SMONTHNAME12:
        UniQueryLocaleItem(locale_object, MON_12,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_FONTSIGNATURE: // FixMe!!!
      case LOCALE_SABBREVMONTHNAME13:
      case LOCALE_SMONTHNAME13:
	if(wbuf) {
		*wbuf = 0;
	}
        LocaleFromUniStr((LPWSTR)L"\0",wbuf,&ulInfoLen);
        break;

      case LOCALE_SABBREVMONTHNAME1:
        UniQueryLocaleItem(locale_object, ABMON_1,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME2:
        UniQueryLocaleItem(locale_object, ABMON_2,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME3:
        UniQueryLocaleItem(locale_object, ABMON_3,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME4:
        UniQueryLocaleItem(locale_object, ABMON_4,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME5:
        UniQueryLocaleItem(locale_object, ABMON_5,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME6:
        UniQueryLocaleItem(locale_object, ABMON_6,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME7:
        UniQueryLocaleItem(locale_object, ABMON_7,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME8:
        UniQueryLocaleItem(locale_object, ABMON_8,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME9:
        UniQueryLocaleItem(locale_object, ABMON_9,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME10:
        UniQueryLocaleItem(locale_object, ABMON_10,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME11:
        UniQueryLocaleItem(locale_object, ABMON_11,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SABBREVMONTHNAME12:
        UniQueryLocaleItem(locale_object, ABMON_12,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SDATE:
        UniQueryLocaleItem(locale_object, DATESEP,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

    case LOCALE_STIME:
        UniQueryLocaleItem(locale_object, TIMESEP,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

    case LOCALE_IDATE:
    case LOCALE_ILDATE:
        LocaleFromUniChar(CtryInfo.fsDateFmt,wbuf,&ulInfoLen);
        break;

      case LOCALE_ITIME:
        UniQueryLocaleItem(locale_object, TIMESEP,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SSHORTDATE:
        UniQueryLocaleItem(locale_object, WSHORTDATE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SLONGDATE:
        UniQueryLocaleItem(locale_object, WLONGDATE,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;


      case LOCALE_STIMEFORMAT:
        UniQueryLocaleItem(locale_object, WTIMEFORMAT,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SISO639LANGNAME:
        UniQueryLocaleItem(locale_object, SISO639LANGNAME,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

      case LOCALE_SISO3166CTRYNAME:
        UniQueryLocaleItem(locale_object, SISO3166CTRYNAME,&pInfoItem);
        LocaleFromUniStr(pInfoItem,wbuf,&ulInfoLen);
        UniFreeMem(pInfoItem);
        break;

    default:
      dprintf(("KERNEL32:  OS2GetLocaleInfoW: LCType %X not yet supported\n", LCType));
      if (wbuf)
      {
        wbuf[0] = (WCHAR) '1';
        wbuf[1] = 0;
      }
      ulInfoLen=2;
      break;

    }
    UniFreeLocaleInfo(puni_lconv);
    UniFreeLocaleObject(locale_object);
  }
  else
    return (0);

  dprintf(("KERNEL32:  OS2GetLocaleInfoW returned %d\n",ulInfoLen));

  return (ulInfoLen);
}
/*****************************************************************************
 * Name      : DWORD GetLocaleInfoA
 * Purpose   : The GetLocaleInfoA function returns specific locale information
 * Parameters: LCID   lcid    locale identifier
 *             LCTYPE LCType  type of information
 *             LPSTR  buf     address of buffer for information
 *             int    len     size of buffer
 * Variables :
 * Result    : size of target buffer
 * Remark    : In this version LOCALE_SYSTEM_DEFAULT == LOCALE_USER_DEFAULT
 *             look into GetLocaleInfoW
 *
 * Status    : TESTED
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/07/21 12:18]
 *****************************************************************************/
int WIN32API GetLocaleInfoA(LCID lcid, LCTYPE LCType, LPSTR buf, int len)
{
  LPWSTR lpWStr;
  int    ret_len;

  if (len && (! buf) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return 0;
  }

  dprintf(("KERNEL32: GetLocaleInfoA lcID=%d,lcType=%x,buf=%X,len=%d\n",lcid,LCType,buf,len));

  if (buf)
    lpWStr=(LPWSTR)malloc(len*(sizeof(WCHAR)));
  else
    lpWStr=NULL; // Query for buffer size

  ret_len = CALL_ODINFUNC(GetLocaleInfoW)(lcid, LCType, lpWStr, len);

  if (ret_len && buf)
    UnicodeToAscii(lpWStr,buf);

  if (lpWStr) free(lpWStr); // free prevooisly allocated memory

  dprintf(("KERNEL32: GetLocaleInfoA returned %d -> %s",ret_len, (ret_len) ? buf : NULL));

  return (ret_len);
}
#endif
//******************************************************************************
//******************************************************************************
LCID WIN32API GetThreadLocale()
{
 TEB *teb = GetThreadTEB();

  if(teb == NULL) {
  	dprintf(("KERNEL32: ERROR: GetThreadLocale teb == NULL!"));
	return FALSE;
  }
  dprintf(("KERNEL32: GetThreadLocale returned %x", teb->o.odin.lcid));
  return(teb->o.odin.lcid);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetThreadLocale(LCID locale)
{
 TEB *teb = GetThreadTEB();

  if(teb == NULL) {
  	dprintf(("KERNEL32: ERROR SetThreadLocale teb == NULL!"));
	return FALSE;
  }
  dprintf(("KERNEL32: SetThreadLocale %x", locale));
  teb->o.odin.lcid = locale;
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
