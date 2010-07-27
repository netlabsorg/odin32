/*
 * msvcrt.dll mbcs functions
 *
 * Copyright 1999 Alexandre Julliard
 * Copyright 2000 Jon Griffths
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
 *
 * FIXME
 * Not currently binary compatible with win32. MSVCRT_mbctype must be
 * populated correctly and the ismb* functions should reference it.
 */

#include "msvcrt.h"

#include "msvcrt/mbctype.h"
#include "msvcrt/mbstring.h"
#include "msvcrt/stdlib.h"
#include "msvcrt/string.h"
#include "msvcrt/wctype.h"

#include "wine/unicode.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

unsigned char MSVCRT_mbctype[257];
int MSVCRT___mb_cur_max = 1;

static MSVCRT_wchar_t msvcrt_mbc_to_wc(unsigned int ch)
{
  MSVCRT_wchar_t chW;
  char mbch[2];
  int n_chars;

  if (ch <= 0xff) {
    mbch[0] = ch;
    n_chars = 1;
  } else {
    mbch[0] = (ch >> 8) & 0xff;
    mbch[1] = ch & 0xff;
    n_chars = 2;
  }
  if (!MultiByteToWideChar(MSVCRT_current_lc_all_cp, 0, mbch, n_chars, &chW, 1))
  {
    WARN("MultiByteToWideChar failed on %x\n", ch);
    return 0;
  }
  return chW;
}

/*********************************************************************
 *		__p__mbctype (MSVCRT.@)
 */
unsigned char* __p__mbctype(void)
{
  dprintf(("MSVCRT: Query for __p__mbctype"));
  return MSVCRT_mbctype;
}

/*********************************************************************
 *		__p___mb_cur_max(MSVCRT.@)
 */
int* __p___mb_cur_max(void)
{
  dprintf(("MSVCRT: Query for __p__mb_cur_max"));
  return &MSVCRT___mb_cur_max;
}

/*********************************************************************
 *		_mbsnextc(MSVCRT.@)
 */
unsigned int _mbsnextc(const unsigned char* str)
{
  dprintf(("MSVCRT: _mbsnextc"));
  if(MSVCRT___mb_cur_max > 1 && MSVCRT_isleadbyte(*str))
    return *str << 8 | str[1];
  return *str; /* ASCII CP or SB char */
}

/*********************************************************************
 *		_mbctolower(MSVCRT.@)
 */
unsigned int _mbctolower(unsigned int c)
{
    dprintf(("MSVCRT: _mbctolower"));
    if (MSVCRT_isleadbyte(c))
    {
      FIXME("Handle MBC chars\n");
      return c;
    }
    return tolower(c); /* ASCII CP or SB char */
}

/*********************************************************************
 *		_mbctoupper(MSVCRT.@)
 */
unsigned int _mbctoupper(unsigned int c)
{
     dprintf(("MSVCRT: _mbctoupper"));
    if (MSVCRT_isleadbyte(c))
    {
      FIXME("Handle MBC chars\n");
      return c;
    }
    return toupper(c); /* ASCII CP or SB char */
}

/*********************************************************************
 *		_mbsdec(MSVCRT.@)
 */
unsigned char* _mbsdec(const unsigned char* start, const unsigned char* cur)
{
  dprintf(("MSVCRT: _mbsdec"));
  if(MSVCRT___mb_cur_max > 1)
    return (char *)(_ismbstrail(start,cur-1) ? cur - 2 : cur -1);

  return (char *)cur - 1; /* ASCII CP or SB char */
}

/*********************************************************************
 *		_mbsinc(MSVCRT.@)
 */
unsigned char* _mbsinc(const unsigned char* str)
{
  dprintf(("MSVCRT: _mbsinc"));
  if(MSVCRT___mb_cur_max > 1 && MSVCRT_isleadbyte(*str))
    return (unsigned char*)str + 2; /* MB char */

  return (unsigned char*)str + 1; /* ASCII CP or SB char */
}

/*********************************************************************
 *		_mbsninc(MSVCRT.@)
 */
unsigned char* _mbsninc(const unsigned char* str, MSVCRT_size_t num)
{
  dprintf(("MSVCRT: _mbsninc"));
  if(!str || num < 1)
    return NULL;
  if(MSVCRT___mb_cur_max > 1)
  {
    while(num--)
      str = _mbsinc(str);
    return (unsigned char*)str;
  }
  return (unsigned char*)str + num; /* ASCII CP */
}

/*********************************************************************
 *		_mbclen(MSVCRT.@)
 */
unsigned int _mbclen(const unsigned char* str)
{
  dprintf(("MSVCRT: _mbclen"));
  return MSVCRT_isleadbyte(*str) ? 2 : 1;
}

/*********************************************************************
 *		mblen(MSVCRT.@)
 */
int MSVCRT_mblen(const char* str, MSVCRT_size_t size)
{
  dprintf(("MSVCRT: mblen"));
  if (str && *str && size)
  {
    if(MSVCRT___mb_cur_max == 1)
      return 1; /* ASCII CP */

    return !MSVCRT_isleadbyte(*str) ? 1 : (size>1 ? 2 : -1);
  }
  return 0;
}

/*********************************************************************
 *		_mbslen(MSVCRT.@)
 */
MSVCRT_size_t _mbslen(const unsigned char* str)
{
  dprintf(("MSVCRT: _mbslen"));
  if(MSVCRT___mb_cur_max > 1)
  {
    MSVCRT_size_t len = 0;
    while(*str)
    {
      str += MSVCRT_isleadbyte(*str) ? 2 : 1;
      len++;
    }
    return len;
  }
  return strlen(str); /* ASCII CP */
}

/*********************************************************************
 *		_mbstrlen(MSVCRT.@)
 */
MSVCRT_size_t _mbstrlen(const char* str)
{
  dprintf(("MSVCRT: _mbstrlen"));
  if(MSVCRT___mb_cur_max > 1)
  {
    MSVCRT_size_t len = 0;
    while(*str)
    {
      /* FIXME: According to the documentation we are supposed to test for
       * multi-byte character validity. Whatever that means
       */
      str += MSVCRT_isleadbyte(*str) ? 2 : 1;
      len++;
    }
    return len;
  }
  return strlen(str); /* ASCII CP */
}

/*********************************************************************
 *		_mbccpy(MSVCRT.@)
 */
void _mbccpy(unsigned char* dest, const unsigned char* src)
{
  dprintf(("MSVCRT: _mbccpy"));
  *dest++ = *src;
  if(MSVCRT___mb_cur_max > 1 && MSVCRT_isleadbyte(*src))
    *dest = *++src; /* MB char */
  else
    ERR("failure.. is this ok?\n");
}

/*********************************************************************
 *		_mbsncpy(MSVCRT.@)
 */
unsigned char* _mbsncpy(unsigned char* dst, const unsigned char* src, MSVCRT_size_t n)
{
  dprintf(("MSVCRT: _mbsncpy"));
  if(!n)
    return dst;
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned char* ret = dst;
    while (*src && n--)
    {
      *dst++ = *src;
      if (MSVCRT_isleadbyte(*src++))
          *dst++ = *src++;
    }
    while(n--)
      *dst++ = '\0';
    return ret;
  }
  return strncpy(dst, src, n); /* ASCII CP */
}

/*********************************************************************
 *              _mbsnbcpy(MSVCRT.@)
 */
unsigned char* _mbsnbcpy(unsigned char* dst, const unsigned char* src, MSVCRT_size_t n)
{
  dprintf(("MSVCRT: _mbsnbcpy"));
  if(!n)
    return dst;
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned char* ret = dst;
    while (*src && (n-- > 1))
    {
      *dst++ = *src;
      if (MSVCRT_isleadbyte(*src++))
      {
        *dst++ = *src++;
        n--;
      }
    }
    if (*src && n && !MSVCRT_isleadbyte(*src))
    {
      /* If the last character is a multi-byte character then
       * we cannot copy it since we have only one byte left
       */
      *dst++ = *src;
      n--;
    }
    while (n--)
      *dst++ = '\0';
    return ret;
  }
  return strncpy(dst, src, n); /* ASCII CP */
}

/*********************************************************************
 *		_mbscmp(MSVCRT.@)
 */
int _mbscmp(const unsigned char* str, const unsigned char* cmp)
{
  dprintf(("MSVCRT: _mbscmp"));
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int strc, cmpc;
    do {
      if(!*str)
        return *cmp ? -1 : 0;
      if(!*cmp)
        return 1;
      strc = _mbsnextc(str);
      cmpc = _mbsnextc(cmp);
      if(strc != cmpc)
        return strc < cmpc ? -1 : 1;
      str +=(strc > 255) ? 2 : 1;
      cmp +=(strc > 255) ? 2 : 1; /* equal, use same increment */
    } while(1);
  }
  return strcmp(str, cmp); /* ASCII CP */
}

/*********************************************************************
 *		_mbsicoll(MSVCRT.@)
 * FIXME: handle locales.
 */
int _mbsicoll(const unsigned char* str, const unsigned char* cmp)
{
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int strc, cmpc;
    do {
      if(!*str)
        return *cmp ? -1 : 0;
      if(!*cmp)
        return 1;
      strc = _mbctolower(_mbsnextc(str));
      cmpc = _mbctolower(_mbsnextc(cmp));
      if(strc != cmpc)
        return strc < cmpc ? -1 : 1;
      str +=(strc > 255) ? 2 : 1;
      cmp +=(strc > 255) ? 2 : 1; /* equal, use same increment */
    } while(1);
  }
  return strcasecmp(str, cmp); /* ASCII CP */
}

/*********************************************************************
 *		_mbsicmp(MSVCRT.@)
 */
int _mbsicmp(const unsigned char* str, const unsigned char* cmp)
{
  dprintf(("MSVCRT: _mbsicmp"));
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int strc, cmpc;
    do {
      if(!*str)
        return *cmp ? -1 : 0;
      if(!*cmp)
        return 1;
      strc = _mbctolower(_mbsnextc(str));
      cmpc = _mbctolower(_mbsnextc(cmp));
      if(strc != cmpc)
        return strc < cmpc ? -1 : 1;
      str +=(strc > 255) ? 2 : 1;
      cmp +=(strc > 255) ? 2 : 1; /* equal, use same increment */
    } while(1);
  }
  return _stricmp(str, cmp); /* ASCII CP */
}

/*********************************************************************
 *		_mbsncmp(MSVCRT.@)
 */
int _mbsncmp(const unsigned char* str, const unsigned char* cmp, MSVCRT_size_t len)
{
  dprintf(("MSVCRT: _mbsncmp"));
  if(!len)
    return 0;

  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int strc, cmpc;
    while(len--)
    {
      int inc;
      if(!*str)
        return *cmp ? -1 : 0;
      if(!*cmp)
        return 1;
      strc = _mbsnextc(str);
      cmpc = _mbsnextc(cmp);
      if(strc != cmpc)
        return strc < cmpc ? -1 : 1;
      inc=(strc > 255) ? 2 : 1; /* Equal, use same increment */
      str += inc;
      cmp += inc;
    }
    return 0; /* Matched len chars */
  }
  return strncmp(str, cmp, len); /* ASCII CP */
}

/*********************************************************************
 *              _mbsnbcmp(MSVCRT.@)
 */
int _mbsnbcmp(const unsigned char* str, const unsigned char* cmp, MSVCRT_size_t len)
{
  dprintf(("MSVCRT: _mbsnbcmp"));
  if (!len)
    return 0;
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int strc, cmpc;
    while (len)
    {
      int clen;
      if(!*str)
        return *cmp ? -1 : 0;
      if(!*cmp)
        return 1;
      if (MSVCRT_isleadbyte(*str))
      {
        strc=(len>=2)?_mbsnextc(str):0;
        clen=2;
      }
      else
      {
        strc=*str;
        clen=1;
      }
      if (MSVCRT_isleadbyte(*cmp))
        cmpc=(len>=2)?_mbsnextc(cmp):0;
      else
        cmpc=*str;
      if(strc != cmpc)
        return strc < cmpc ? -1 : 1;
      len -= clen;
      str += clen;
      cmp += clen;
    }
    return 0; /* Matched len chars */
      FIXME("%s %s %d\n",str,cmp,len);
  }
  return strncmp(str,cmp,len);
}

/*********************************************************************
 *		_mbsnicmp(MSVCRT.@)
 *
 * Compare two multibyte strings case insensitively to 'len' characters.
 */
int _mbsnicmp(const unsigned char* str, const unsigned char* cmp, MSVCRT_size_t len)
{
  dprintf(("MSVCRT: _mbsnicmp"));
  /* FIXME: No tolower() for mb strings yet */
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int strc, cmpc;
    while(len--)
    {
      if(!*str)
        return *cmp ? -1 : 0;
      if(!*cmp)
        return 1;
      strc = _mbctolower(_mbsnextc(str));
      cmpc = _mbctolower(_mbsnextc(cmp));
      if(strc != cmpc)
        return strc < cmpc ? -1 : 1;
      str +=(strc > 255) ? 2 : 1;
      cmp +=(strc > 255) ? 2 : 1; /* Equal, use same increment */
    }
    return 0; /* Matched len chars */
  }
  return strncasecmp(str, cmp, len); /* ASCII CP */
}

/*********************************************************************
 *              _mbsnbicmp(MSVCRT.@)
 */
int _mbsnbicmp(const unsigned char* str, const unsigned char* cmp, MSVCRT_size_t len)
{
  dprintf(("MSVCRT: _mbsnbicmp"));
  if (!len)
    return 0;
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int strc, cmpc;
    while (len)
    {
      int clen;
      if(!*str)
        return *cmp ? -1 : 0;
      if(!*cmp)
        return 1;
      if (MSVCRT_isleadbyte(*str))
      {
        strc=(len>=2)?_mbsnextc(str):0;
        clen=2;
      }
      else
      {
        strc=*str;
        clen=1;
      }
      if (MSVCRT_isleadbyte(*cmp))
        cmpc=(len>=2)?_mbsnextc(cmp):0;
      else
        cmpc=*str;
      strc = _mbctolower(strc);
      cmpc = _mbctolower(cmpc);
      if(strc != cmpc)
        return strc < cmpc ? -1 : 1;
      len -= clen;
      str += clen;
      cmp += clen;
    }
    return 0; /* Matched len bytes */
      FIXME("%s %s %d\n",str,cmp,len);
  }
  return strncmp(str,cmp,len);
}

/*********************************************************************
 *		_mbschr(MSVCRT.@)
 *
 * Find a multibyte character in a multibyte string.
 */
unsigned char* _mbschr(const unsigned char* s, unsigned int x)
{
  dprintf(("MSVCRT: _mbschr"));
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int c;
    while (1)
    {
      c = _mbsnextc(s);
      if (c == x)
        return (unsigned char*)s;
      if (!c)
        return NULL;
      s += c > 255 ? 2 : 1;
    }
  }
  return strchr(s, x); /* ASCII CP */
}

/*********************************************************************
 *		_mbsrchr(MSVCRT.@)
 */
unsigned char* _mbsrchr(const unsigned char* s, unsigned int x)
{
  dprintf(("MSVCRT: _mbsrchr"));
  if(MSVCRT___mb_cur_max > 1)
  {
    unsigned int c;
    unsigned char* match=NULL;
    if(!s)
      return NULL;
    while (1) {
      c = _mbsnextc(s);
      if (c == x)
        match=(unsigned char*)s;
      if (!c)
        return match;
      s +=(c > 255) ? 2 : 1;
    }
  }
  return strrchr(s,x);
}

/*********************************************************************
 *		_mbstok(MSVCRT.@)
 *
 * Find and extract tokens from strings
 */
unsigned char* _mbstok(unsigned char *str, const unsigned char *delim)
{
    MSVCRT_thread_data *data = msvcrt_get_thread_data();
    char *ret;

    if(MSVCRT___mb_cur_max > 1)
    {
	unsigned int c;

	if (!str)
    	    if (!(str = data->mbstok_next)) return NULL;

	while ((c = _mbsnextc(str)) && _mbschr(delim, c)) {
	    str += c > 255 ? 2 : 1;
	}
	if (!*str) return NULL;
	ret = str++;
	while ((c = _mbsnextc(str)) && !_mbschr(delim, c)) {
	    str += c > 255 ? 2 : 1;
	}
	if (*str) {
	    *str++ = 0;
	    if (c > 255) *str++ = 0;
	}
	data->mbstok_next = str;
	return ret;
    }
    return strtok(str, delim);	/* ASCII CP */
}

/*********************************************************************
 *		mbtowc(MSVCRT.@)
 */
int MSVCRT_mbtowc(MSVCRT_wchar_t *dst, const char* str, MSVCRT_size_t n)
{
  dprintf(("MSVCRT: _mbtowc"));
  if(n <= 0 || !str)
    return 0;
  if(!MultiByteToWideChar(CP_ACP, 0, str, n, dst, 1))
    return 0;
  /* return the number of bytes from src that have been used */
  if(!*str)
    return 0;
  if(n >= 2 && MSVCRT_isleadbyte(*str) && str[1])
    return 2;
  return 1;
}

/*********************************************************************
 *		_mbbtombc(MSVCRT.@)
 */
unsigned int _mbbtombc(unsigned int c)
{
  dprintf(("MSVCRT: _mbbtombc"));
  if(MSVCRT___mb_cur_max > 1 &&
     ((c >= 0x20 && c <=0x7e) ||(c >= 0xa1 && c <= 0xdf)))
  {
    /* FIXME: I can't get this function to return anything
     * different to what I pass it...
     */
  }
  return c;  /* ASCII CP or no MB char */
}

/*********************************************************************
 *		_ismbbkana(MSVCRT.@)
 */
int _ismbbkana(unsigned int c)
{
  dprintf(("MSVCRT: _ismbbkana"));
  /* FIXME: use lc_ctype when supported, not lc_all */
  if(MSVCRT_current_lc_all_cp == 932)
  {
    /* Japanese/Katakana, CP 932 */
    return (c >= 0xa1 && c <= 0xdf);
  }
  return 0;
}

/*********************************************************************
 *              _ismbcdigit(MSVCRT.@)
 */
int _ismbcdigit(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    dprintf(("MSVCRT: _ismbcdigit"));
    return (get_char_typeW( wch ) & C1_DIGIT);
}

/*********************************************************************
 *              _ismbcgraph(MSVCRT.@)
 */
int _ismbcgraph(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & (C1_UPPER | C1_LOWER | C1_DIGIT | C1_PUNCT | C1_ALPHA));
}

/*********************************************************************
 *              _ismbcalpha (MSVCRT.@)
 */
int _ismbcalpha(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & C1_ALPHA);
}

/*********************************************************************
 *              _ismbclower (MSVCRT.@)
 */
int _ismbclower(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & C1_UPPER);
}

/*********************************************************************
 *              _ismbcupper (MSVCRT.@)
 */
int _ismbcupper(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & C1_LOWER);
}

/*********************************************************************
 *              _ismbcsymbol(MSVCRT.@)
 */
int _ismbcsymbol(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    WORD ctype;
    if (!GetStringTypeW(CT_CTYPE3, &wch, 1, &ctype))
    {
        WARN("GetStringTypeW failed on %x\n", ch);
        return 0;
    }
    return ((ctype & C3_SYMBOL) != 0);
}

/*********************************************************************
 *              _ismbcalnum (MSVCRT.@)
 */
int _ismbcalnum(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & (C1_ALPHA | C1_DIGIT));
}

/*********************************************************************
 *              _ismbcspace (MSVCRT.@)
 */
int _ismbcspace(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & C1_SPACE);
}

/*********************************************************************
 *              _ismbcprint (MSVCRT.@)
 */
int _ismbcprint(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & (C1_UPPER | C1_LOWER | C1_DIGIT | C1_PUNCT | C1_ALPHA | C1_SPACE));
}

/*********************************************************************
 *              _ismbcpunct(MSVCRT.@)
 */
int _ismbcpunct(unsigned int ch)
{
    MSVCRT_wchar_t wch = msvcrt_mbc_to_wc( ch );
    return (get_char_typeW( wch ) & C1_PUNCT);
}

/*********************************************************************
 *		_ismbchira(MSVCRT.@)
 */
int _ismbchira(unsigned int c)
{
  /* FIXME: use lc_ctype when supported, not lc_all */
  if(MSVCRT_current_lc_all_cp == 932)
  {
    /* Japanese/Hiragana, CP 932 */
    return (c >= 0x829f && c <= 0x82f1);
  }
  return 0;
}

/*********************************************************************
 *		_ismbckata(MSVCRT.@)
 */
int _ismbckata(unsigned int c)
{
  /* FIXME: use lc_ctype when supported, not lc_all */
  if(MSVCRT_current_lc_all_cp == 932)
  {
    if(c < 256)
      return _ismbbkana(c);
    /* Japanese/Katakana, CP 932 */
    return (c >= 0x8340 && c <= 0x8396 && c != 0x837f);
  }
  return 0;
}

/*********************************************************************
 *		_ismbblead(MSVCRT.@)
 */
int _ismbblead(unsigned int c)
{
  /* FIXME: should reference MSVCRT_mbctype */
  return MSVCRT___mb_cur_max > 1 && MSVCRT_isleadbyte(c);
}


/*********************************************************************
 *		_ismbbtrail(MSVCRT.@)
 */
int _ismbbtrail(unsigned int c)
{
  /* FIXME: should reference MSVCRT_mbctype */
  return !_ismbblead(c);
}

/*********************************************************************
 *		_ismbslead(MSVCRT.@)
 */
int _ismbslead(const unsigned char* start, const unsigned char* str)
{
  /* Lead bytes can also be trail bytes if caller messed up
   * iterating through the string...
   */
  if(MSVCRT___mb_cur_max > 1)
  {
    while(start < str)
      start += MSVCRT_isleadbyte(*str) ? 2 : 1;

    if(start == str)
      return MSVCRT_isleadbyte(*str);
  }
  return 0; /* Must have been a trail, we skipped it */
}

/*********************************************************************
 *		_ismbstrail(MSVCRT.@)
 */
int _ismbstrail(const unsigned char* start, const unsigned char* str)
{
  /* Must not be a lead, and must be preceeded by one */
  return !_ismbslead(start, str) && MSVCRT_isleadbyte(str[-1]);
}

/*********************************************************************
 *		_mbsset(MSVCRT.@)
 */
unsigned char* _mbsset(unsigned char* str, unsigned int c)
{
  unsigned char* ret = str;

  if(MSVCRT___mb_cur_max == 1 || c < 256)
    return MSVCRT__strset(str, c); /* ASCII CP or SB char */

  c &= 0xffff; /* Strip high bits */

  while(str[0] && str[1])
  {
    *str++ = c >> 8;
    *str++ = c & 0xff;
  }
  if(str[0])
    str[0] = '\0'; /* FIXME: OK to shorten? */

  return ret;
}

/*********************************************************************
 *		_mbsnbset(MSVCRT.@)
 */
unsigned char* _mbsnbset(unsigned char *str, unsigned int c, MSVCRT_size_t len)
{
    unsigned char *ret = str;

    if(!len)
	return ret;

    if(MSVCRT___mb_cur_max == 1 || c < 256)
	return _strnset(str, c, len); /* ASCII CP or SB char */

    c &= 0xffff; /* Strip high bits */

    while(str[0] && str[1] && (len > 1))
    {
	*str++ = c >> 8;
	len--;
	*str++ = c & 0xff;
	len--;
    }
    if(len && str[0]) {
	/* as per msdn pad with a blank character */
	str[0] = ' ';
    }

    return ret;
}

/*********************************************************************
 *		_mbsnset(MSVCRT.@)
 */
unsigned char* _mbsnset(unsigned char* str, unsigned int c, MSVCRT_size_t len)
{
  unsigned char *ret = str;

  if(!len)
    return ret;

  if(MSVCRT___mb_cur_max == 1 || c < 256)
    return MSVCRT__strnset(str, c, len); /* ASCII CP or SB char */

  c &= 0xffff; /* Strip high bits */

  while(str[0] && str[1] && len--)
  {
    *str++ = c >> 8;
    *str++ = c & 0xff;
  }
  if(len && str[0])
    str[0] = '\0'; /* FIXME: OK to shorten? */

  return ret;
}

/*********************************************************************
 *		_mbsnccnt(MSVCRT.@)
 * 'c' is for 'character'.
 */
MSVCRT_size_t _mbsnccnt(const unsigned char* str, MSVCRT_size_t len)
{
  MSVCRT_size_t ret;
  if(MSVCRT___mb_cur_max > 1)
  {
    ret=0;
    while(*str && len-- > 0)
    {
      if(MSVCRT_isleadbyte(*str))
      {
        if (!len)
          break;
        len--;
        str++;
      }
      str++;
      ret++;
    }
    return ret;
  }
  ret=strlen(str);
  return min(ret, len); /* ASCII CP */
}

/*********************************************************************
 *		_mbsnbcnt(MSVCRT.@)
 * 'b' is for byte count.
 */
MSVCRT_size_t _mbsnbcnt(const unsigned char* str, MSVCRT_size_t len)
{
  MSVCRT_size_t ret;
  if(MSVCRT___mb_cur_max > 1)
  {
    const unsigned char* xstr = str;
    while(*xstr && len-- > 0)
    {
      if (MSVCRT_isleadbyte(*xstr++))
        xstr++;
    }
    return xstr-str;
  }
  ret=strlen(str);
  return min(ret, len); /* ASCII CP */
}


/*********************************************************************
 *		_mbsnbcat(MSVCRT.@)
 */
unsigned char* _mbsnbcat(unsigned char* dst, const unsigned char* src, MSVCRT_size_t len)
{
    if(MSVCRT___mb_cur_max > 1)
    {
        char *res = dst;
        while (*dst) {
	    if (MSVCRT_isleadbyte(*dst++)) {
		if (*dst) {
		    dst++;
		} else {
		    /* as per msdn overwrite the lead byte in front of '\0' */
		    dst--;
		    break;
		}
	    }
	}
        while (*src && len--) *dst++ = *src++;
        *dst = '\0';
        return res;
    }
    return strncat(dst, src, len); /* ASCII CP */
}

/*********************************************************************
 *		_mbsncat(MSVCRT.@)
 */
unsigned char* _mbsncat(unsigned char* dst, const unsigned char* src, MSVCRT_size_t len)
{
  if(MSVCRT___mb_cur_max > 1)
  {
    char *res = dst;
    while (*dst)
    {
      if (MSVCRT_isleadbyte(*dst++))
        dst++;
    }
    while (*src && len--)
    {
      *dst++ = *src;
      if(MSVCRT_isleadbyte(*src++))
        *dst++ = *src++;
    }
    *dst = '\0';
    return res;
  }
  return strncat(dst, src, len); /* ASCII CP */
}


/*********************************************************************
 *              _mbslwr(MSVCRT.@)
 */
unsigned char* _mbslwr(unsigned char* s)
{
  if (!s)
    return NULL;
  if (MSVCRT___mb_cur_max > 1)
  {
    unsigned int c;
    unsigned char* p=s;
    while (*s)
    {
      c = _mbctolower(_mbsnextc(s));
      /* Note that I assume that the size of the character is unchanged */
      if (c > 255)
      {
          *s++=(c>>8);
          c=c & 0xff;
      }
      *s++=c;
    }
    return p;
  }
  return _strlwr(s);
}


/*********************************************************************
 *              _mbsupr(MSVCRT.@)
 */
unsigned char* _mbsupr(unsigned char* s)
{
  if (!s)
    return NULL;
  if (MSVCRT___mb_cur_max > 1)
  {
    unsigned int c;
    unsigned char* p=s;
    while (*s)
    {
      c = _mbctoupper(_mbsnextc(s));
      /* Note that I assume that the size of the character is unchanged */
      if (c > 255)
      {
          *s++=(c>>8);
          c=c & 0xff;
      }
      *s++=c;
    }
    return p;
  }
  return _strupr(s);
}


/*********************************************************************
 *              _mbsspn (MSVCRT.@)
 */
MSVCRT_size_t _mbsspn(const unsigned char* string, const unsigned char* set)
{
  const unsigned char *p, *q;

  for (p = string; *p; p++)
    {
      if (MSVCRT_isleadbyte(*p))
	{
	  for (q = set; *q; q++)
	    {
	      if (!q[1])
		break;
	      if ((*p == *q) &&  (p[1] == q[1]))
		break;
	      q++;
	    }
	  if (*++p == '\0')
	    break;
	}
      else
	for (q = set; *q; q++)
	  if (*p == *q)
	    break;
    }
  return p - string;
}

/*********************************************************************
 *		_mbscspn(MSVCRT.@)
 */
MSVCRT_size_t _mbscspn(const unsigned char* str, const unsigned char* cmp)
{
  if (MSVCRT___mb_cur_max > 1)
    FIXME("don't handle double character case\n");
  return strcspn(str, cmp);
}

/*********************************************************************
 *              _mbsrev (MSVCRT.@)
 */
unsigned char* _mbsrev(unsigned char* str)
{
    int i, len = _mbslen(str);
    unsigned char *p, *temp=MSVCRT_malloc(len*2);

    if(!temp)
        return str;

    /* unpack multibyte string to temp buffer */
    p=str;
    for(i=0; i<len; i++)
    {
        if (MSVCRT_isleadbyte(*p))
        {
            temp[i*2]=*p++;
            temp[i*2+1]=*p++;
        }
        else
        {
            temp[i*2]=*p++;
            temp[i*2+1]=0;
        }
    }

    /* repack it in the reverse order */
    p=str;
    for(i=len-1; i>=0; i--)
    {
        if(MSVCRT_isleadbyte(temp[i*2]))
        {
            *p++=temp[i*2];
            *p++=temp[i*2+1];
        }
        else
        {
            *p++=temp[i*2];
        }
    }

    MSVCRT_free(temp);

    return str;
}

/*********************************************************************
 *		_mbspbrk (MSVCRT.@)
 */
unsigned char* _mbspbrk(const unsigned char* str, const unsigned char* accept)
{
    const unsigned char* p;

    while(*str)
    {
        for(p = accept; *p; p += (MSVCRT_isleadbyte(*p)?2:1) )
        {
            if (*p == *str)
                if( !MSVCRT_isleadbyte(*p) || ( *(p+1) == *(str+1) ) )
                     return (unsigned char*)str;
        }
        str += (MSVCRT_isleadbyte(*str)?2:1);
    }
    return NULL;
}
