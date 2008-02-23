/*
 * msvcrt.dll locale functions
 *
 * Copyright 2000 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __WIN32OS2__
#include "config.h"
#else
#include <emxheader.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#endif

#include "wine/port.h"

#include "winbase.h"
#include "winuser.h"

#include "msvcrt.h"
#include "msvcrt/locale.h"
#include "mtdll.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

/* FIXME: Need to hold locale for each LC_* type and aggregate
 * string to produce lc_all.
 */
#define MAX_ELEM_LEN 64 /* Max length of country/language/CP string */
#define MAX_LOCALE_LENGTH 256
char MSVCRT_current_lc_all[MAX_LOCALE_LENGTH];
LCID MSVCRT_current_lc_all_lcid;
int MSVCRT_current_lc_all_cp;

/* MT */
#define LOCK_LOCALE   _mlock(_SETLOCALE_LOCK);
#define UNLOCK_LOCALE _munlock(_SETLOCALE_LOCK);

/* ctype data modified when the locale changes */
extern WORD MSVCRT__ctype [257];
extern WORD MSVCRT_current_ctype[257];
extern WORD* MSVCRT__pctype;

/* mbctype data modified when the locale changes */
extern int MSVCRT___mb_cur_max;
extern unsigned char MSVCRT_mbctype[257];

#define MSVCRT_LEADBYTE  0x8000

/* Friendly country strings & iso codes for synonym support.
 * Based on MS documentation for setlocale().
 */
static const char* _country_synonyms[] =
{
  "Hong Kong","HK",
  "Hong-Kong","HK",
  "New Zealand","NZ",
  "New-Zealand","NZ",
  "PR China","CN",
  "PR-China","CN",
  "United Kingdom","GB",
  "United-Kingdom","GB",
  "Britain","GB",
  "England","GB",
  "Great Britain","GB",
  "United States","US",
  "United-States","US",
  "America","US"
};

/* INTERNAL: Map a synonym to an ISO code */
static void remap_synonym(char *name)
{
  size_t i;
  for (i = 0; i < sizeof(_country_synonyms)/sizeof(char*); i += 2 )
  {
    if (!_stricmp(_country_synonyms[i],name))
    {
      TRACE(":Mapping synonym %s to %s\n",name,_country_synonyms[i+1]);
      name[0] = _country_synonyms[i+1][0];
      name[1] = _country_synonyms[i+1][1];
      name[2] = '\0';
      return;
    }
  }
}

/* Note: Flags are weighted in order of matching importance */
#define FOUND_LANGUAGE         0x4
#define FOUND_COUNTRY          0x2
#define FOUND_CODEPAGE         0x1

typedef struct {
  char search_language[MAX_ELEM_LEN];
  char search_country[MAX_ELEM_LEN];
  char search_codepage[MAX_ELEM_LEN];
  char found_language[MAX_ELEM_LEN];
  char found_country[MAX_ELEM_LEN];
  char found_codepage[MAX_ELEM_LEN];
  unsigned int match_flags;
  LANGID found_lang_id;
} locale_search_t;

#define CONTINUE_LOOKING TRUE
#define STOP_LOOKING     FALSE

/* INTERNAL: Get and compare locale info with a given string */
static int compare_info(LCID lcid, DWORD flags, char* buff, const char* cmp)
{
  buff[0] = 0;
  GetLocaleInfoA(lcid, flags|LOCALE_NOUSEROVERRIDE,buff, MAX_ELEM_LEN);
  if (!buff[0] || !cmp[0])
    return 0;
  /* Partial matches are allowed, e.g. "Germ" matches "Germany" */
  return !_strnicmp(cmp, buff, strlen(cmp));
}

static BOOL CALLBACK
find_best_locale_proc(HMODULE hModule WINE_UNUSED, LPCSTR type WINE_UNUSED,
                      LPCSTR name WINE_UNUSED, WORD LangID, LONG lParam)
{
  locale_search_t *res = (locale_search_t *)lParam;
  const LCID lcid = MAKELCID(LangID, SORT_DEFAULT);
  char buff[MAX_ELEM_LEN];
  unsigned int flags = 0;

  if(PRIMARYLANGID(LangID) == LANG_NEUTRAL)
    return CONTINUE_LOOKING;

  /* Check Language */
  if (compare_info(lcid,LOCALE_SISO639LANGNAME,buff,res->search_language) ||
      compare_info(lcid,LOCALE_SABBREVLANGNAME,buff,res->search_language) ||
      compare_info(lcid,LOCALE_SENGLANGUAGE,buff,res->search_language))
  {
    TRACE(":Found language: %s->%s\n", res->search_language, buff);
    flags |= FOUND_LANGUAGE;
    memcpy(res->found_language,res->search_language,MAX_ELEM_LEN);
  }
  else if (res->match_flags & FOUND_LANGUAGE)
  {
    return CONTINUE_LOOKING;
  }

  /* Check Country */
  if (compare_info(lcid,LOCALE_SISO3166CTRYNAME,buff,res->search_country) ||
      compare_info(lcid,LOCALE_SABBREVCTRYNAME,buff,res->search_country) ||
      compare_info(lcid,LOCALE_SENGCOUNTRY,buff,res->search_country))
  {
    TRACE("Found country:%s->%s\n", res->search_country, buff);
    flags |= FOUND_COUNTRY;
    memcpy(res->found_country,res->search_country,MAX_ELEM_LEN);
  }
  else if (res->match_flags & FOUND_COUNTRY)
  {
    return CONTINUE_LOOKING;
  }

  /* Check codepage */
  if (compare_info(lcid,LOCALE_IDEFAULTCODEPAGE,buff,res->search_codepage) ||
      (compare_info(lcid,LOCALE_IDEFAULTANSICODEPAGE,buff,res->search_codepage)))
  {
    TRACE("Found codepage:%s->%s\n", res->search_codepage, buff);
    flags |= FOUND_CODEPAGE;
    memcpy(res->found_codepage,res->search_codepage,MAX_ELEM_LEN);
  }
  else if (res->match_flags & FOUND_CODEPAGE)
  {
    return CONTINUE_LOOKING;
  }

  if (flags > res->match_flags)
  {
    /* Found a better match than previously */
    res->match_flags = flags;
    res->found_lang_id = LangID;
  }
  if (flags & (FOUND_LANGUAGE & FOUND_COUNTRY & FOUND_CODEPAGE))
  {
    TRACE(":found exact locale match\n");
    return STOP_LOOKING;
  }
  return CONTINUE_LOOKING;
}

extern int atoi(const char *);

/* Internal: Find the LCID for a locale specification */
static LCID MSVCRT_locale_to_LCID(locale_search_t* locale)
{
  LCID lcid;
  EnumResourceLanguagesA(GetModuleHandleA("KERNEL32"), RT_STRINGA,
			 (LPCSTR)LOCALE_ILANGUAGE,find_best_locale_proc,
			 (LONG)locale);

  if (!locale->match_flags)
    return 0;

  /* If we were given something that didn't match, fail */
  if (locale->search_country[0] && !(locale->match_flags & FOUND_COUNTRY))
    return 0;

  lcid =  MAKELCID(locale->found_lang_id, SORT_DEFAULT);

  /* Populate partial locale, translating LCID to locale string elements */
  if (!locale->found_codepage[0])
  {
    /* Even if a codepage is not enumerated for a locale
     * it can be set if valid */
    if (locale->search_codepage[0])
    {
      if (IsValidCodePage(atoi(locale->search_codepage)))
        memcpy(locale->found_codepage,locale->search_codepage,MAX_ELEM_LEN);
      else
      {
        /* Special codepage values: OEM & ANSI */
        if (_stricmp(locale->search_codepage,"OCP"))
        {
          GetLocaleInfoA(lcid, LOCALE_IDEFAULTCODEPAGE,
                         locale->found_codepage, MAX_ELEM_LEN);
        }
        if (_stricmp(locale->search_codepage,"ACP"))
        {
          GetLocaleInfoA(lcid, LOCALE_IDEFAULTANSICODEPAGE,
                         locale->found_codepage, MAX_ELEM_LEN);
        }
        else
          return 0;

        if (!atoi(locale->found_codepage))
           return 0;
      }
    }
    else
    {
      /* Prefer ANSI codepages if present */
      GetLocaleInfoA(lcid, LOCALE_IDEFAULTANSICODEPAGE,
                     locale->found_codepage, MAX_ELEM_LEN);
      if (!locale->found_codepage[0] || !atoi(locale->found_codepage))
          GetLocaleInfoA(lcid, LOCALE_IDEFAULTCODEPAGE,
                         locale->found_codepage, MAX_ELEM_LEN);
    }
  }
  GetLocaleInfoA(lcid, LOCALE_SENGLANGUAGE|LOCALE_NOUSEROVERRIDE,
                 locale->found_language, MAX_ELEM_LEN);
  GetLocaleInfoA(lcid, LOCALE_SENGCOUNTRY|LOCALE_NOUSEROVERRIDE,
                 locale->found_country, MAX_ELEM_LEN);
  return lcid;
}


/* INTERNAL: Set ctype behaviour for a codepage */
static void msvcrt_set_ctype(unsigned int codepage, LCID lcid)
{
  CPINFO cp;

  memset(&cp, 0, sizeof(CPINFO));

  if (GetCPInfo(codepage, &cp))
  {
    int i;
    char str[3];
    unsigned char *traverse = (unsigned char *)cp.LeadByte;

    memset(MSVCRT_current_ctype, 0, sizeof(MSVCRT__ctype));
    MSVCRT_current_lc_all_cp = codepage;

    /* Switch ctype macros to MBCS if needed */
    MSVCRT___mb_cur_max = cp.MaxCharSize;

    /* Set remaining ctype flags: FIXME: faster way to do this? */
    str[1] = str[2] = 0;
    for (i = 0; i < 256; i++)
    {
      if (!(MSVCRT__pctype[i] & MSVCRT_LEADBYTE))
      {
        str[0] = i;
        GetStringTypeA(lcid, CT_CTYPE1, str, 1, MSVCRT__pctype + i);
      }
    }

    /* Set leadbyte flags */
    while (traverse[0] || traverse[1])
    {
      for( i = traverse[0]; i <= traverse[1]; i++ )
        MSVCRT_current_ctype[i+1] |= MSVCRT_LEADBYTE;
      traverse += 2;
    };
  }
}


/*********************************************************************
 *		setlocale (MSVCRT.@)
 */
char* MSVCRT_setlocale(int category, const char* locale)
{
  LCID lcid = 0;
  locale_search_t lc;
  int haveLang, haveCountry, haveCP;
  char* next;
  int lc_all = 0;

  TRACE("MSVCRT: setlocale (%d %s)\n",category,locale);

  if (category < MSVCRT_LC_MIN || category > MSVCRT_LC_MAX)
    return NULL;

  if (locale == NULL)
  {
    /* Report the current Locale */
    return MSVCRT_current_lc_all;
  }

  LOCK_LOCALE;

  if (locale[0] == 'L' && locale[1] == 'C' && locale[2] == '_')
  {
    FIXME(":restore previous locale not implemented!\n");
    /* FIXME: Easiest way to do this is parse the string and
     * call this function recursively with its elements,
     * Where they differ for each lc_ type.
     */
    UNLOCK_LOCALE;
    return MSVCRT_current_lc_all;
  }

  /* Default Locale: Special case handling */
  if (!strlen(locale) || ((toupper(locale[0]) == 'C') && !locale[1]))
  {
    MSVCRT_current_lc_all[0] = 'C';
    MSVCRT_current_lc_all[1] = '\0';
    MSVCRT_current_lc_all_cp = GetACP();

    switch (category) {
    case MSVCRT_LC_ALL:
      lc_all = 1; /* Fall through all cases ... */
    case MSVCRT_LC_COLLATE:
      if (!lc_all) break;
    case MSVCRT_LC_CTYPE:
      /* Restore C locale ctype info */
      MSVCRT___mb_cur_max = 1;
      memcpy(MSVCRT_current_ctype, MSVCRT__ctype, sizeof(MSVCRT__ctype));
      memset(MSVCRT_mbctype, 0, sizeof(MSVCRT_mbctype));
      if (!lc_all) break;
    case MSVCRT_LC_MONETARY:
      if (!lc_all) break;
    case MSVCRT_LC_NUMERIC:
      if (!lc_all) break;
    case MSVCRT_LC_TIME:
      break;
    }
    UNLOCK_LOCALE;
    return MSVCRT_current_lc_all;
  }

  /* Get locale elements */
  haveLang = haveCountry = haveCP = 0;
  memset(&lc,0,sizeof(lc));

  next = strchr(locale,'_');
  if (next && next != locale)
  {
    haveLang = 1;
    strncpy(lc.search_language,locale,next-locale);
    locale += next-locale+1;
  }

  next = strchr(locale,'.');
  if (next)
  {
    haveCP = 1;
    if (next == locale)
    {
      locale++;
      strncpy(lc.search_codepage, locale, MAX_ELEM_LEN);
    }
    else
    {
      if (haveLang)
      {
        haveCountry = 1;
        strncpy(lc.search_country,locale,next-locale);
        locale += next-locale+1;
      }
      else
      {
        haveLang = 1;
        strncpy(lc.search_language,locale,next-locale);
        locale += next-locale+1;
      }
      strncpy(lc.search_codepage, locale, MAX_ELEM_LEN);
    }
  }
  else
  {
    if (haveLang)
    {
      haveCountry = 1;
      strncpy(lc.search_country, locale, MAX_ELEM_LEN);
    }
    else
    {
      haveLang = 1;
      strncpy(lc.search_language, locale, MAX_ELEM_LEN);
    }
  }

  if (haveCountry)
    remap_synonym(lc.search_country);

  if (haveCP && !haveCountry && !haveLang)
  {
    FIXME(":Codepage only locale not implemented\n");
    /* FIXME: Use default lang/country and skip locale_to_LCID()
     * call below...
     */
    UNLOCK_LOCALE;
    return NULL;
  }

  lcid = MSVCRT_locale_to_LCID(&lc);

  TRACE(":found LCID %ld\n",lcid);

  if (lcid == 0)
  {
    UNLOCK_LOCALE;
    return NULL;
  }

  MSVCRT_current_lc_all_lcid = lcid;

  _snprintf(MSVCRT_current_lc_all,MAX_LOCALE_LENGTH,"%s_%s.%s",
	   lc.found_language,lc.found_country,lc.found_codepage);

  switch (category) {
  case MSVCRT_LC_ALL:
    lc_all = 1; /* Fall through all cases ... */
  case MSVCRT_LC_COLLATE:
    if (!lc_all) break;
  case MSVCRT_LC_CTYPE:
    msvcrt_set_ctype(atoi(lc.found_codepage),lcid);
    if (!lc_all) break;
  case MSVCRT_LC_MONETARY:
    if (!lc_all) break;
  case MSVCRT_LC_NUMERIC:
    if (!lc_all) break;
  case MSVCRT_LC_TIME:
    break;
  }
  UNLOCK_LOCALE;
  return MSVCRT_current_lc_all;
}


/*********************************************************************
 *		_Getdays (MSVCRT.@)
 */
const char* _Getdays(void)
{
  static const char *MSVCRT_days = ":Sun:Sunday:Mon:Monday:Tue:Tuesday:Wed:"
                            "Wednesday:Thu:Thursday:Fri:Friday:Sat:Saturday";
  /* FIXME: Use locale */
  TRACE("(void) semi-stub\n");
  return MSVCRT_days;
}

/*********************************************************************
 *		_Getmonths (MSVCRT.@)
 */
const char* _Getmonths(void)
{
  static const char *MSVCRT_months = ":Jan:January:Feb:February:Mar:March:Apr:"
                "April:May:May:Jun:June:Jul:July:Aug:August:Sep:September:Oct:"
                "October:Nov:November:Dec:December";
  /* FIXME: Use locale */
  TRACE("(void) semi-stub\n");
  return MSVCRT_months;
}

/*********************************************************************
 *		_Getnames (MSVCRT.@)
 */
const char* _Getnames(void)
{
  /* FIXME: */
  TRACE("(void) stub\n");
  return "";
}

/*********************************************************************
 *		_Strftime (MSVCRT.@)
 */
const char* _Strftime(char *out, unsigned int len, const char *fmt,
                                     const void *tm, void *foo)
{
  /* FIXME: */
  TRACE("(%p %d %s %p %p) stub\n", out, len, fmt, tm, foo);
  return "";
}

/* FIXME: MBCP probably belongs in mbcs.c */

/*********************************************************************
 *		_setmbcp (MSVCRT.@)
 */
void _setmbcp(int cp)
{
  LOCK_LOCALE;
  dprintf(("MSVCRT: _setmbcp %d",cp));
  if (MSVCRT_current_lc_all_cp != cp)
  {
    /* FIXME: set ctype behaviour for this cp */
    MSVCRT_current_lc_all_cp = cp;
  }
  UNLOCK_LOCALE;
}

/*********************************************************************
 *		_getmbcp (MSVCRT.@)
 */
int _getmbcp(void)
{
  dprintf(("MSVCRT: _getmbcp"));
  return MSVCRT_current_lc_all_cp;
}

/*********************************************************************
 *		__crtLCMapStringA (MSVCRT.@)
 */
int __crtLCMapStringA(
  LCID lcid, DWORD mapflags, const char* src, int srclen, char* dst,
  int dstlen, unsigned int codepage, int xflag
) {
  FIXME("(lcid %lx, flags %lx, %s(%d), %p(%d), %x, %d), partial stub!\n",
        lcid,mapflags,src,srclen,dst,dstlen,codepage,xflag);
  /* FIXME: A bit incorrect. But msvcrt itself just converts its
   * arguments to wide strings and then calls LCMapStringW
   */
  return LCMapStringA(lcid,mapflags,src,srclen,dst,dstlen);
}
