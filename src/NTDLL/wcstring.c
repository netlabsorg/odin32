/* $Id: wcstring.c,v 1.2 2001-09-05 13:14:08 bird Exp $ */
/*
 * NTDLL wide-char functions
 *
 * Copyright 2000 Alexandre Julliard
 * Copyright 2000 Jon Griffiths
 */

#include "config.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __WIN32OS2__
#include <stdarg.h>
#include <heapstring.h>
#endif

#include "windef.h"
#include "winbase.h"
#include "winnls.h"
#include "wine/unicode.h"
#include "heap.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(ntdll);


/*********************************************************************
 *           _wcsicmp    (NTDLL.@)
 */
INT __cdecl NTDLL__wcsicmp( LPCWSTR str1, LPCWSTR str2 )
{
    return strcmpiW( str1, str2 );
}


/*********************************************************************
 *           _wcslwr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL__wcslwr( LPWSTR str )
{
    return strlwrW( str );
}


/*********************************************************************
 *           _wcsnicmp    (NTDLL.@)
 */
INT __cdecl NTDLL__wcsnicmp( LPCWSTR str1, LPCWSTR str2, INT n )
{
    return strncmpiW( str1, str2, n );
}


/*********************************************************************
 *           _wcsupr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL__wcsupr( LPWSTR str )
{
    return struprW( str );
}


/*********************************************************************
 *           towlower    (NTDLL.@)
 */
WCHAR __cdecl NTDLL_towlower( WCHAR ch )
{
    return tolowerW(ch);
}


/*********************************************************************
 *           towupper    (NTDLL.@)
 */
WCHAR __cdecl NTDLL_towupper( WCHAR ch )
{
    return toupperW(ch);
}


/***********************************************************************
 *           wcscat    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcscat( LPWSTR dst, LPCWSTR src )
{
    return strcatW( dst, src );
}


/*********************************************************************
 *           wcschr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcschr( LPCWSTR str, WCHAR ch )
{
    return strchrW( str, ch );
}


/*********************************************************************
 *           wcscmp    (NTDLL.@)
 */
INT __cdecl NTDLL_wcscmp( LPCWSTR str1, LPCWSTR str2 )
{
    return strcmpW( str1, str2 );
}


/***********************************************************************
 *           wcscpy    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcscpy( LPWSTR dst, LPCWSTR src )
{
    return strcpyW( dst, src );
}


/*********************************************************************
 *           wcscspn    (NTDLL.@)
 */
INT __cdecl NTDLL_wcscspn( LPCWSTR str, LPCWSTR reject )
{
    LPCWSTR start = str;
    while (*str)
    {
        LPCWSTR p = reject;
        while (*p && (*p != *str)) p++;
        if (*p) break;
        str++;
    }
    return str - start;
}


/***********************************************************************
 *           wcslen    (NTDLL.@)
 */
INT __cdecl NTDLL_wcslen( LPCWSTR str )
{
    return strlenW( str );
}


/*********************************************************************
 *           wcsncat    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsncat( LPWSTR s1, LPCWSTR s2, INT n )
{
    LPWSTR ret = s1;
    while (*s1) s1++;
    while (n-- > 0) if (!(*s1++ = *s2++)) return ret;
    *s1 = 0;
    return ret;
}


/*********************************************************************
 *           wcsncmp    (NTDLL.@)
 */
INT __cdecl NTDLL_wcsncmp( LPCWSTR str1, LPCWSTR str2, INT n )
{
    return strncmpW( str1, str2, n );
}


/*********************************************************************
 *           wcsncpy    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsncpy( LPWSTR s1, LPCWSTR s2, INT n )
{
    return strncpyW( s1, s2, n );
}


/*********************************************************************
 *           wcspbrk    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcspbrk( LPCWSTR str, LPCWSTR accept )
{
    LPCWSTR p;
    while (*str)
    {
        for (p = accept; *p; p++) if (*p == *str) return (LPWSTR)str;
        str++;
    }
    return NULL;
}


/*********************************************************************
 *           wcsrchr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsrchr( LPWSTR str, WCHAR ch )
{
    LPWSTR last = NULL;
    while (*str)
    {
        if (*str == ch) last = str;
        str++;
    }
    return last;
}


/*********************************************************************
 *           wcsspn    (NTDLL.@)
 */
INT __cdecl NTDLL_wcsspn( LPCWSTR str, LPCWSTR accept )
{
    LPCWSTR start = str;
    while (*str)
    {
        LPCWSTR p = accept;
        while (*p && (*p != *str)) p++;
        if (!*p) break;
        str++;
    }
    return str - start;
}


/*********************************************************************
 *           wcsstr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsstr( LPCWSTR str, LPCWSTR sub )
{
    return strstrW( str, sub );
}


/*********************************************************************
 *           wcstok    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcstok( LPWSTR str, LPCWSTR delim )
{
    static LPWSTR next = NULL;
    LPWSTR ret;

    if (!str)
        if (!(str = next)) return NULL;

    while (*str && NTDLL_wcschr( delim, *str )) str++;
    if (!*str) return NULL;
    ret = str++;
    while (*str && !NTDLL_wcschr( delim, *str )) str++;
    if (*str) *str++ = 0;
    next = str;
    return ret;
}


/*********************************************************************
 *           wcstombs    (NTDLL.@)
 */
INT __cdecl NTDLL_wcstombs( LPSTR dst, LPCWSTR src, INT n )
{
    INT ret;
    if (n <= 0) return 0;
    ret = WideCharToMultiByte( CP_ACP, 0, src, -1, dst, dst ? n : 0, NULL, NULL );
    if (!ret) return n;  /* overflow */
    return ret - 1;  /* do not count terminating NULL */
}


/*********************************************************************
 *           mbstowcs    (NTDLL.@)
 */
INT __cdecl NTDLL_mbstowcs( LPWSTR dst, LPCSTR src, INT n )
{
    INT ret;
    if (n <= 0) return 0;
    ret = MultiByteToWideChar( CP_ACP, 0, src, -1, dst, dst ? n : 0 );
    if (!ret) return n;  /* overflow */
    return ret - 1;  /* do not count terminating NULL */
}


/*********************************************************************
 *                  wcstol  (NTDLL.@)
 * Like strtol, but for wide character strings.
 */
INT __cdecl NTDLL_wcstol(LPWSTR s,LPWSTR *end,INT base)
{
    LPSTR sA = HEAP_strdupWtoA(GetProcessHeap(),0,s),endA;
    INT ret = strtol(sA,&endA,base);

    HeapFree(GetProcessHeap(),0,sA);
    if (end) *end = s+(endA-sA); /* pointer magic checked. */
    return ret;
}


/*********************************************************************
 *           iswctype    (NTDLL.@)
 */
INT __cdecl NTDLL_iswctype( WCHAR wc, WCHAR wct )
{
    return (get_char_typeW(wc) & 0xfff) & wct;
}


/*********************************************************************
 *           iswalpha    (NTDLL.@)
 */
INT __cdecl NTDLL_iswalpha( WCHAR wc )
{
    return get_char_typeW(wc) & C1_ALPHA;
}


/*********************************************************************
 *           _ultow    (NTDLL.@)
 * Like _ultoa, but for wide character strings.
 */
LPWSTR __cdecl _ultow(ULONG value, LPWSTR string, INT radix)
{
    WCHAR tmp[33];
    LPWSTR tp = tmp;
    LPWSTR sp;
    LONG i;
    ULONG v = value;

    if (radix > 36 || radix <= 1)
    return 0;

    while (v || tp == tmp)
    {
    i = v % radix;
    v = v / radix;
    if (i < 10)
        *tp++ = i + '0';
    else
        *tp++ = i + 'a' - 10;
    }

    sp = string;
    while (tp > tmp)
    *sp++ = *--tp;
    *sp = 0;
    return string;
}

/*********************************************************************
 *           _wtol    (NTDLL.@)
 * Like atol, but for wide character strings.
 */
LONG __cdecl _wtol(LPWSTR string)
{
    char buffer[30];
    NTDLL_wcstombs( buffer, string, sizeof(buffer) );
    return atol( buffer );
}

/*********************************************************************
 *           _wtoi    (NTDLL.@)
 */
INT __cdecl _wtoi(LPWSTR string)
{
    return _wtol(string);
}

/* INTERNAL: Wide char snprintf
 * If you fix a bug in this function, fix it in msvcrt/wcs.c also!
 */
static int __cdecl NTDLL_vsnwprintf(WCHAR *str, unsigned int len,
                                    const WCHAR *format, va_list valist)
{
  unsigned int written = 0;
  const WCHAR *iter = format;
  char bufa[256], fmtbufa[64], *fmta;

  TRACE("(%d,%s)\n",len,debugstr_w(format));

  while (*iter)
  {
    while (*iter && *iter != (WCHAR)L'%')
    {
     if (written++ >= len)
       return -1;
     *str++ = *iter++;
    }
    if (*iter == (WCHAR)L'%')
    {
      fmta = fmtbufa;
      *fmta++ = *iter++;
      while (*iter == (WCHAR)L'0' ||
             *iter == (WCHAR)L'+' ||
             *iter == (WCHAR)L'-' ||
             *iter == (WCHAR)L' ' ||
             *iter == (WCHAR)L'0' ||
             *iter == (WCHAR)L'*' ||
             *iter == (WCHAR)L'#')
      {
        if (*iter == (WCHAR)L'*')
        {
          char *buffiter = bufa;
          int fieldlen = va_arg(valist, int);
          sprintf(buffiter, "%d", fieldlen);
          while (*buffiter)
            *fmta++ = *buffiter++;
        }
        else
          *fmta++ = *iter;
        iter++;
      }

      while (isdigit(*iter))
        *fmta++ = *iter++;

      if (*iter == (WCHAR)L'.')
      {
        *fmta++ = *iter++;
        if (*iter == (WCHAR)L'*')
        {
          char *buffiter = bufa;
          int fieldlen = va_arg(valist, int);
          sprintf(buffiter, "%d", fieldlen);
          while (*buffiter)
            *fmta++ = *buffiter++;
        }
        else
          while (isdigit(*iter))
            *fmta++ = *iter++;
      }
      if (*iter == (WCHAR)L'h' ||
          *iter == (WCHAR)L'l')
      {
          *fmta++ = *iter++;
          *fmta++ = *iter++;
      }

      switch (*iter)
      {
      case (WCHAR)L's':
        {
          static const WCHAR none[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };
          const WCHAR *wstr = va_arg(valist, const WCHAR *);
          const WCHAR *striter = wstr ? wstr : none;
          while (*striter)
          {
            if (written++ >= len)
              return -1;
            *str++ = *striter++;
          }
          iter++;
          break;
        }

      case (WCHAR)L'c':
        if (written++ >= len)
          return -1;
        *str++ = (WCHAR)va_arg(valist, int);
        iter++;
        break;

      default:
        {
          /* For non wc types, use system sprintf and append to wide char output */
          /* FIXME: for unrecognised types, should ignore % when printing */
          char *bufaiter = bufa;
          if (*iter == (WCHAR)L'p')
            sprintf(bufaiter, "%08lX", va_arg(valist, long));
          else
          {
            *fmta++ = *iter;
            *fmta = '\0';
            if (*iter == (WCHAR)L'f')
              sprintf(bufaiter, fmtbufa, va_arg(valist, double));
            else
              sprintf(bufaiter, fmtbufa, va_arg(valist, void *));
          }
          while (*bufaiter)
          {
            if (written++ >= len)
              return -1;
            *str++ = *bufaiter++;
          }
          iter++;
          break;
        }
      }
    }
  }
  if (written >= len)
    return -1;
  *str++ = (WCHAR)L'\0';
  return (int)written;
}


/***********************************************************************
 *        _snwprintf (NTDLL.@)
 */
int __cdecl _snwprintf(WCHAR *str, unsigned int len, const WCHAR *format, ...)
{
  int retval;
  va_list valist;
  va_start(valist, format);
  retval = NTDLL_vsnwprintf(str, len, format, valist);
  va_end(valist);
  return retval;
}


/***********************************************************************
 *        swprintf (NTDLL.@)
 */
int __cdecl NTDLL_swprintf(WCHAR *str, const WCHAR *format, ...)
{
  int retval;
  va_list valist;
  va_start(valist, format);
  retval = NTDLL_vsnwprintf(str, INT_MAX, format, valist);
  va_end(valist);
  return retval;
}
