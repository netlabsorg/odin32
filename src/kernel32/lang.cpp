/*
 * Win32 language API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1999 Przemyslaw Dobrowolski
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

ODINDEBUGCHANNEL(KERNEL32-LANG)


//******************************************************************************
//******************************************************************************
BOOL WIN32API IsValidCodePage(UINT CodePage)
{
  dprintf(("KERNEL32:  IsValidCodePage %d not implemented", CodePage));
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
//    if (*pLen > sizeof(WCHAR))
    if (*pLen > 2)
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

  ret_len = ODIN_GetLocaleInfoW(lcid, LCType, lpWStr, len);

  if (ret_len && buf)
  {
    UnicodeToAscii(lpWStr,buf);
    free(lpWStr);
  }

  dprintf(("KERNEL32:  OS2GetLocaleInfoA returned %d\n",ret_len));

  return (ret_len);
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
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(DWORD,VerLanguageNameA,DWORD,  wLang,
                                     LPSTR,  szLang,
                                     DWORD,  nSize)
{
  char    buffer[80];
  LPCSTR  name;
  DWORD   result;

  /*
   * First, check \System\CurrentControlSet\control\Nls\Locale\<langid>
   * from the registry.
   */

#if 0
PHS: disabled because if interlinkage with registry
  sprintf( buffer,
           "\\System\\CurrentControlSet\\control\\Nls\\Locale\\%08x",
           wLang );

  result = RegQueryValueA(HKEY_LOCAL_MACHINE,
                          buffer,
                          szLang,
                          (LPLONG)&nSize );
  if (result == ERROR_SUCCESS ||
      result == ERROR_MORE_DATA)
    return nSize;
#endif

  /*
   * If that fails, use the internal table
   * (actually, Windows stores the names in a string table resource ...)
   */

  lstrcpynA(szLang,
            "Language-Neutral",
            nSize);

  return strlen(szLang);
}

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(DWORD,VerLanguageNameW,DWORD,  wLang,
                                     LPWSTR, szLang,
                                     DWORD,  nSize)
{
  LPSTR szLangA;
  DWORD rc;

  if ( (szLang == NULL) ||
       (nSize  == 0) )          // validate parameters
     return 0;

  szLangA = (LPSTR)HEAP_malloc(nSize + 1);
  rc = VerLanguageNameA(wLang,
                        szLangA,
                        nSize);
  if (rc == ERROR_SUCCESS || rc == ERROR_MORE_DATA)
    AsciiToUnicodeN(szLangA,
                    szLang,
                    nSize);
  HEAP_free(szLangA);
  return rc;
}


