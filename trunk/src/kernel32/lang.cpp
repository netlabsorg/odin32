/* $Id: lang.cpp,v 1.36 2001-05-20 08:17:41 sandervl Exp $ */
/*
 * Win32 language API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1999 Przemyslaw Dobrowolski
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

static ULONG defaultLanguage = 0;
//******************************************************************************
//******************************************************************************
ULONG GetLanguageId()
{
  if(defaultLanguage == 0) {
	switch(OSLibQueryCountry()) {
	case CC_USA:
		defaultLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		break;
        case CC_CANADA:
		defaultLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		break;
        case CC_LATIN_AMERICA:
		defaultLanguage = MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH);
		break;
        case CC_GREECE:
		defaultLanguage = MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT);
		break;
        case CC_NETHERLANDS:
		defaultLanguage = MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH);
		break;
        case CC_BELGIUM:
		defaultLanguage = MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH_BELGIAN);
		break;
        case CC_FRANCE:
		defaultLanguage = MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
		break;
        case CC_SPAIN:
		defaultLanguage = MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH);
		break;
        case CC_ITALY:
		defaultLanguage = MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN);
		break;
        case CC_SWITZERLAND:
		defaultLanguage = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_SWISS);
		break;
        case CC_AUSTRIA:
		defaultLanguage = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_AUSTRIAN);
		break;
        case CC_UNITED_KINGDOM:
		defaultLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK);
		break;
        case CC_DENMARK:
		defaultLanguage = MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT);
		break;
        case CC_SWEDEN:
		defaultLanguage = MAKELANGID(LANG_SWEDISH, SUBLANG_DEFAULT);
		break;
        case CC_NORWAY:
		defaultLanguage = MAKELANGID(LANG_NORWEGIAN, SUBLANG_DEFAULT);
		break;
        case CC_WEST_GERMANY:
		defaultLanguage = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN);
		break;
        case CC_MEXICO:
		defaultLanguage = MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MEXICAN);
		break;
        case CC_BRAZIL:
		defaultLanguage = MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN);
		break;
        case CC_AUSTRALIA:
		defaultLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_AUS);
		break;
        case CC_NEW_ZEALAND:
		defaultLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_NZ);
		break;
        case CC_JAPAN:
		defaultLanguage = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
		break;
        case CC_KOREA:
		defaultLanguage = MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT);
		break;
        case CC_CHINA:
		defaultLanguage = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL);
		break;
        case CC_TAIWAN:
		defaultLanguage = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL);
		break;
        case CC_TURKEY:
		defaultLanguage = MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT);
		break;
        case CC_PORTUGAL:
		defaultLanguage = MAKELANGID(LANG_PORTUGUESE, SUBLANG_DEFAULT);
		break;
        case CC_LUXEMBOURG:
		defaultLanguage = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LUXEMBOURG);
		break;
        case CC_ICELAND:
		defaultLanguage = MAKELANGID(LANG_ICELANDIC, SUBLANG_DEFAULT);
		break;
        case CC_FINLAND:
		defaultLanguage = MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT);
		break;
        case CC_CZECH_REPUBLIC:
		defaultLanguage = MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT);
		break;
        case CC_SLOVAK_REPUBLIC:
		defaultLanguage = MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT);
		break;
	case CC_POLAND:
                defaultLanguage = MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT);
                break;
	default:
		defaultLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK);
		break;
	}
  }
  return defaultLanguage;
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
static BOOL LocaleFromUniStr(LPWSTR lpUniStr, LPWSTR wbuf, ULONG *pLen)
{
  if (wbuf)
  {
    if (*pLen > UniStrlen(lpUniStr))
      UniStrcpy(wbuf,lpUniStr);
    else
    {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      *pLen=0;
      return FALSE;
    }
  }

  *pLen=UniStrlen(lpUniStr)+1; // + null terminator

  return TRUE;
}
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
ODINFUNCTION4(int, GetLocaleInfoW, LCID, lcid, LCTYPE, LCType, LPWSTR, wbuf, 
              int, len)
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
        if(defaultLanguage == CC_CZECH_REPUBLIC) {
           ((BYTE*)wbuf)[0] = ((BYTE*)wbuf)[0] & 0x7F;
        }
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
ODINFUNCTION4(int, GetLocaleInfoA, LCID, lcid, LCTYPE, LCType, LPSTR, buf, 
              int, len)
{
  LPWSTR lpWStr;
  int    ret_len;

  if (len && (! buf) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return 0;
  }

  dprintf(("KERNEL32:  OS2GetLocaleInfoA lcID=%d,lcType=%x,buf=%X,len=%d\n",lcid,LCType,buf,len));

  if (buf)
    lpWStr=(LPWSTR)malloc(len*(sizeof(WCHAR)));
  else
    lpWStr=NULL; // Query for buffer size

  ret_len = CALL_ODINFUNC(GetLocaleInfoW)(lcid, LCType, lpWStr, len);

  if (ret_len && buf)
    UnicodeToAscii(lpWStr,buf);
  
  if (lpWStr) free(lpWStr); // free prevooisly allocated memory

  dprintf(("KERNEL32:  OS2GetLocaleInfoA returned %d\n",ret_len));

  return (ret_len);
}
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
