/* $Id: crt_wc.cpp,v 1.5 2001-08-04 15:29:39 sandervl Exp $ */

/*
 * The C RunTime DLL
 * 
 * Implements C run-time functionality as known from UNIX.
 *
 * Partialy based on Wine
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997 Uwe Bonnes
 * Copyright 1999-2000 Jens Wiessner
 * Copyright 2000 Przemyslaw Dobrowolski
 *
 * CRTDLL - widechar functions
 *
 */

#include <odin.h>
#include <os2win.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <heapstring.h>
#include <string.h>
#include "crtimps.h"


/*********************************************************************
 *           _fgetwchar  (CRTDLL.93)
 */
wint_t CDECL CRTDLL__fgetwchar( void *i )
{
  // TODO: Check if really ok.
  dprintf2(("CRTDLL: _fgetwchar -> _getch\n"));
  return CRTDLL__getch();
}


/*********************************************************************
 *           CRTDLL__wcsdup	 (CRTDLL.317)
 */
LPWSTR CDECL CRTDLL__wcsdup( LPCWSTR str )
{
  dprintf2(("CRTDLL: _wcsdup\n"));
  LPWSTR ret = NULL;
  if (str)
  {
      int size = (wcslen((const wchar_t*)str) + 1) * sizeof(WCHAR);
      ret = (WCHAR*)CRTDLL_malloc( size );
      if (ret) memcpy( ret, str, size );
  }
  return ret;
}


/*********************************************************************
 *           CRTDLL__wcsicmp	 (CRTDLL.318)
 */
int CDECL CRTDLL__wcsicmp(LPWSTR str1, LPWSTR str2)
{
  dprintf2(("CRTDLL: _wcsicmp(%08xh,%08xh)\n",
           str1,
           str2));

  return (CRTDLL__wcsnicmp(str1,
                       str2,
                       wcslen((wchar_t*) str1)));
}


/*********************************************************************
 *           CRTDLL__wcsicoll	 (CRTDLL.319)
 */
int CDECL CRTDLL__wcsicoll( LPCWSTR str1, LPCWSTR str2 )
{
  dprintf2(("CRTDLL: _wcsicoll\n"));
  return CRTDLL__wcsicmp( (LPWSTR)str1, (LPWSTR)str2 );
}


/*********************************************************************
 *           CRTDLL__wcslwr	 (CRTDLL.320)
 */
LPWSTR CDECL CRTDLL__wcslwr(LPWSTR str)
{
  DWORD dwIndex;

  dprintf2(("CRTDLL: _wcslwr(%08xh)\n",
           str));

  for (dwIndex = wcslen((const wchar_t*)str);
       dwIndex;
       dwIndex--)
  {
    towlower(str[dwIndex]);
  }

  return (str);
}


/*********************************************************************
 *           CRTDLL__wcsnicmp	 (CRTDLL.321)
 */
int CDECL CRTDLL__wcsnicmp(LPCWSTR str1, LPCWSTR str2, int n)
{
  dprintf2(("CRTDLL: _wcsnicmp(%08xh,%08xh,%08xh)\n",
           str1,
           str2,
           l));

    if (!n) return 0;
    while ((--n > 0) && *str1 && (toupperW(*str1) == toupperW(*str2)))
    {
        str1++;
        str2++;
    }
    return toupperW(*str1) - toupperW(*str2);
}


/*********************************************************************
 *           CRTDLL__wcsnset    (CRTDLL.322)
 */
LPWSTR CDECL CRTDLL__wcsnset( LPWSTR str, WCHAR c, INT n )
{
    dprintf2(("CRTDLL: _wcsnset\n"));
    LPWSTR ret = str;
    while ((n-- > 0) && *str) *str++ = c;
    return ret;
}


/*********************************************************************
 *           CRTDLL__wcsrev	 (CRTDLL.323)
 */
LPWSTR CDECL CRTDLL__wcsrev( LPWSTR str )
{
  dprintf2(("CRTDLL: _wcsrev\n"));
  LPWSTR ret = str;
  LPWSTR end = str + wcslen((const wchar_t*)str) - 1;
  while (end > str)
  {
      WCHAR t = *end;
      *end--  = *str;
      *str++  = t;
  }
  return ret;
}


/*********************************************************************
 *           CRTDLL__wcsset    (CRTDLL.324)
 */
LPWSTR CDECL CRTDLL__wcsset( LPWSTR str, WCHAR c )
{
    dprintf2(("CRTDLL: _wcsset\n"));
    LPWSTR ret = str;
    while (*str) *str++ = c;
    return ret;
}


/*********************************************************************
 *           CRTDLL__wcsupr    (CRTDLL.325)
 */
LPWSTR CDECL CRTDLL__wcsupr(LPWSTR str)
{
  DWORD dwIndex;

  dprintf2(("CRTDLL: _wcsupr(%08xh)\n",
           str));

  for (dwIndex = wcslen((const wchar_t*)str);
       dwIndex;
       dwIndex--)
  {
    toupperW(str[dwIndex]);
  }

  return (str);
}


/*********************************************************************
 *      _wtoi			 		(CRTDLL.330)
 */
int CDECL CRTDLL__wtoi( const wchar_t *s )
{
  // int -32768-32768 = size 6 + null-terminatedstr
  char tempstr[7];

  WideCharToMultiByte(0, // = CP_ACP
                      0,
                      (LPCWSTR)s,
                      -1,
                      tempstr,
                      7,
                      NULL,
                      NULL);

  return (atoi(tempstr));
}


/*********************************************************************
 *	_wtol			 		(CRTDLL.331)
 */
long int CDECL CRTDLL__wtol( const wchar_t *s )
{
  // long int -2147483647-2147483647 = size 10 + null-terminatedstr
  char tempstr[11];
  WideCharToMultiByte(0, // = CP_ACP
                      0,
                      (LPCWSTR)s,
                      -1,
                      tempstr,
                      11,
                      NULL,
                      NULL);

  return (atol(tempstr));

  return 0;
}


/*********************************************************************
 *                  fgetwc	(CRTDLL.366)
 */
wint_t CDECL CRTDLL_fgetwc( FILE *f )
{
  dprintf2(("CRTDLL: fgetwc\n"));
  return (fgetwc(f));
}


/*********************************************************************
 *                  fputwc  (CRTDLL.373)
 */
wint_t CDECL CRTDLL_fputwc( wint_t wc, FILE *strm )
{
  dprintf2(("CRTDLL: fputwc\n"));
  return (fputwc(wc, strm));
}


/*********************************************************************
 *                  iswalnum    (CRTDLL.402)
 */
int CDECL CRTDLL_iswalnum(wint_t i)
{
  dprintf2(("CRTDLL: iswalnum(%08xh)\n", i));
  //return get_char_typeW(wc) & (C1_ALPHA|C1_DIGIT|C1_LOWER|C1_UPPER);
  return (iswalnum(i));
}


/*********************************************************************
 *                  iswalpha    (CRTDLL.403)
 */
int CDECL CRTDLL_iswalpha(wint_t i)
{
  dprintf2(("CRTDLL: iswalpha(%08xh)\n", i));
  return (iswalpha(i));
  //return get_char_typeW(wc) & (C1_ALPHA|C1_LOWER|C1_UPPER);
}


/*********************************************************************
 *                  iswcntrl    (CRTDLL.405)
 */
int CDECL CRTDLL_iswcntrl(wint_t i)
{
  dprintf2(("CRTDLL: iswcntrl(%08xh)\n", i));
  //return get_char_typeW(wc) & C1_CNTRL;
  return (iswcntrl(i));
}


/*********************************************************************
 *                  iswctype    (CRTDLL.406)
 */
int CDECL CRTDLL_iswctype(wint_t i, wctype_t wct)
{
  dprintf2(("CRTDLL: iswctype(%08xh, %08xh)\n", i, wct));
  return (iswctype(i, wct));
}


/*********************************************************************
 *                  iswdigit    (CRTDLL.407)
 */
int CDECL CRTDLL_iswdigit(wint_t i)
{
  dprintf2(("CRTDLL: iswdigit(%08xh)\n", i));
  //return get_char_typeW(wc) & C1_DIGIT;
  return (iswdigit(i));
}


/*********************************************************************
 *                  iswgraph    (CRTDLL.408)
 */
int CDECL CRTDLL_iswgraph(wint_t i)
{
  dprintf2(("CRTDLL: iswgraph(%08xh)\n", i));
  //return get_char_typeW(wc) & (C1_ALPHA|C1_PUNCT|C1_DIGIT|C1_LOWER|C1_UPPER);
  return (iswgraph(i));
}


/*********************************************************************
 *                  iswlower    (CRTDLL.409)
 */
int CDECL CRTDLL_iswlower(wint_t i)
{
  dprintf2(("CRTDLL: iswlower(%08xh)\n", i));
  //return get_char_typeW(wc) & C1_LOWER;
  return (iswlower(i));
}


/*********************************************************************
 *                  iswprint    (CRTDLL.410)
 */
int CDECL CRTDLL_iswprint(wint_t i)
{
  dprintf2(("CRTDLL: iswprint(%08xh)\n", i));
  //return get_char_typeW(wc) & (C1_ALPHA|C1_BLANK|C1_PUNCT|C1_DIGIT|C1_LOWER|C1_UPPER);
  return (iswprint(i));
}


/*********************************************************************
 *                  iswpunct    (CRTDLL.411)
 */
int CDECL CRTDLL_iswpunct(wint_t i)
{
  dprintf2(("CRTDLL: iswpunct(%08xh)\n", i));
  //return get_char_typeW(wc) & C1_PUNCT;
  return (iswpunct(i));
}


/*********************************************************************
 *                  iswspace    (CRTDLL.412)
 */
int CDECL CRTDLL_iswspace(wint_t i)
{
  dprintf2(("CRTDLL: iswspace(%08xh)\n", i));
  //return get_char_typeW(wc) & C1_SPACE;
  return (iswspace(i));
}


/*********************************************************************
 *                  iswupper    (CRTDLL.413)
 */
int CDECL CRTDLL_iswupper(wint_t i)
{
  dprintf2(("CRTDLL: iswupper(%08xh)\n", i));
  //return get_char_typeW(wc) & C1_UPPER;
  return (iswupper(i));
}


/*********************************************************************
 *                  iswxdigit    (CRTDLL.414)
 */
int CDECL CRTDLL_iswxdigit(wint_t i)
{
  dprintf2(("CRTDLL: iswxdigit(%08xh)\n", i));
  //return get_char_typeW(wc) & C1_XDIGIT;
  return (iswxdigit(i));
}


/*********************************************************************
 *                  swprintf         (CRTDLL.480)
 */
int CDECL CRTDLL_swprintf(const LPWSTR str,
                      int   i,
                      const LPWSTR format,
                      ...)
{
  va_list valist;
  int     rc;

  dprintf2(("CRTDLL: swprintf(%s,%d,%s)\n",
           str,
           i,
           format));

  va_start( valist, format );
  rc = vswprintf( (wchar_t*)str,
                  i,
                  (wchar_t*)format,
                  valist );
  va_end( valist );
  return rc;
}


/*********************************************************************
 *	swscanf				   	(CRTDLL.481)
 */
int CDECL CRTDLL_swscanf( const wchar_t *s1, const wchar_t *s2, va_list arg )
{
  dprintf(("CRTDLL: swscanf\n"));
  return (swscanf(s1, s2, arg));
}


/*********************************************************************
 *                  towlower       (CRTDLL.490)
 */
WCHAR CDECL CRTDLL_towlower(WCHAR c)
{
  dprintf2(("CRTDLL: towlower(%c)\n",
           c));

  return (towlower(c));
}


/*********************************************************************
 *                  toupperW       (CRTDLL.491)
 */
WCHAR CDECL CRTDLL_towupper(WCHAR c)
{
  dprintf2(("CRTDLL: towupper(%c)\n",
           c));

  return (toupperW(c));
}


/*********************************************************************
 *                  ungetwc           (CRTDLL.493)
 */
wint_t CDECL CRTDLL_ungetwc( wint_t wc, FILE *strm )
{
  dprintf2(("CRTDLL: ungetwc\n"));
  return (ungetwc(wc, strm));
}


/*********************************************************************
 *                  wcscat            (CRTDLL.500)
 */
wchar_t* CDECL CRTDLL_wcscat(      wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscat(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscat(str1, str2));
}


/*********************************************************************
 *                  wcschr            (CRTDLL.501)
 */
wchar_t* CDECL CRTDLL_wcschr(const wchar_t* str,
                       int          i)
{
  dprintf2(("CRTDLL: wcschr(%08xh,%08xh)\n",
           str,
           i));

  return (wcschr(str, i));
}


/*********************************************************************
 *                  wcscmp            (CRTDLL.502)
 */
int CDECL CRTDLL_wcscmp(const wchar_t* str1,
                    const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscmp(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscmp(str1, str2));
}


/*********************************************************************
 *                  wcscoll    (CRTDLL.503)
 */
DWORD CDECL CRTDLL_wcscoll(LPCWSTR str1, LPCWSTR str2)
{
  dprintf2(("CRTDLL: wcscoll\n"));
  return (wcscoll((const wchar_t*)str1, (const wchar_t*)str2));
}


/*********************************************************************
 *                  wcscpy            (CRTDLL.504)
 */
wchar_t* CDECL CRTDLL_wcscpy(      wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscpy(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscpy(str1, str2));
}


/*********************************************************************
 *                  wcscspn            (CRTDLL.505)
 */
size_t CDECL CRTDLL_wcscspn(const wchar_t* str1,
                              wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscspn(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscspn(str1, str2));
}


/*********************************************************************
 *                  wcsftime   (CRTDLL.506)
 */
size_t CDECL CRTDLL_wcsftime( wchar_t *s, size_t maxsize, 
		const wchar_t *format, const struct tm *timeptr )
{
  dprintf2(("CRTDLL: wcsftime\n"));
  return (wcsftime(s, maxsize, format, timeptr));
}


/*********************************************************************
 *                  wcslen            (CRTDLL.507)
 */
size_t CDECL CRTDLL_wcslen(const wchar_t* str)
{
  dprintf2(("CRTDLL: wcslen(%08xh)\n",
           str));

  return (wcslen(str));
}


/*********************************************************************
 *                  wcsncat            (CRTDLL.508)
 */
wchar_t* CDECL CRTDLL_wcsncat(      wchar_t* str1,
                        const wchar_t* str2,
                        size_t      i)
{
  dprintf2(("CRTDLL: wcsncat(%08xh,%08xh,%08xh)\n",
           str1,
           str2,
           i));

  return (wcsncat(str1, str2, i));
}



/*********************************************************************
 *                  wcsncmp            (CRTDLL.509)
 */
int CDECL CRTDLL_wcsncmp(const wchar_t* str1,
                     const wchar_t* str2,
                     size_t      i)
{
  dprintf2(("CRTDLL: wcsncmp(%08xh,%08xh,%08xh)\n",
           str1,
           str2,
           i));

  return (wcsncmp(str1, str2, i));
}


/*********************************************************************
 *                  wcsncmp            (CRTDLL.510)
 */
wchar_t* CDECL CRTDLL_wcsncpy(      wchar_t* str1,
                          const wchar_t* str2,
                          size_t       i)
{
  dprintf2(("CRTDLL: wcsncpy(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (wcsncpy(str1, str2, i));
}


/*********************************************************************
 *                  wcspbrk            (CRTDLL.511)
 */
wchar_t* CDECL CRTDLL_wcspbrk(const wchar_t* str1,
                        const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcspbrk(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcspbrk(str1, str2));
}


/*********************************************************************
 *                  wcsrchr            (CRTDLL.512)
 */
wchar_t* CDECL CRTDLL_wcsrchr(const wchar_t* str,
                        size_t       i)
{
  dprintf2(("CRTDLL: wcsrchr(%08xh,%08xh)\n",
           str,
           i));

  return (wcsrchr(str, i));
}


/*********************************************************************
 *                  wcsspn             (CRTDLL.513)
 */
size_t CDECL CRTDLL_wcsspn(const wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcsspn(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcsspn(str1, str2));
}


/*********************************************************************
 *                  wcsstr   (CRTDLL.514)
 */
wchar_t* CDECL CRTDLL_wcsstr(const wchar_t* str1,
                         const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcsstr(%s,%s)\n",
           str1,
           str2));

  return (wcsstr(str1, str2));
}


/*********************************************************************
 *                  wcstod   (CRTDLL.515)
 */
double CDECL CRTDLL_wcstod( const wchar_t *nptr, wchar_t **endptr )
{
  dprintf2(("CRTDLL: wcstod\n"));
  return (wcstod(nptr, endptr));
}


/*********************************************************************
 *                  wcstok   (CRTDLL.516)
 */
wchar_t * CDECL CRTDLL_wcstok( wchar_t *s1, const wchar_t *s2, wchar_t **ptr )
{
  dprintf2(("CRTDLL: wcstok(%08xh, %08xh, %08xh)\n",s1,s2,ptr));
  return (wcstok(s1, s2, ptr));
}


/*********************************************************************
 *                  wcstol   (CRTDLL.517)
 */
long int CDECL CRTDLL_wcstol( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf2(("CRTDLL: wcstol(%08xh, %08xh, %08xh)\n",s1,s2,i));
  return (wcstol(s1, s2, i));
}


/*********************************************************************
 *                  wcstombs   (CRTDLL.518)
 */
size_t CDECL CRTDLL_wcstombs( char *s, const wchar_t *pwcs, size_t n )
{
  dprintf2(("CRTDLL: wcstombs(%08xh, %08xh, %08xh)\n",s,pwcs,n));
  return (wcstombs(s, pwcs, n));
}


/*********************************************************************
 *                  wcstoul   (CRTDLL.519)
 */
unsigned long int CDECL CRTDLL_wcstoul( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf2(("CRTDLL: wcstoul(%08xh, %08xh, %08xh)\n",s1,s2,i));
  return (wcstoul(s1, s2, i));
}


/*********************************************************************
 *                  wcsxfrm   (CRTDLL.520)
 */
size_t CDECL CRTDLL_wcsxfrm( wchar_t *s1, const wchar_t *s2, size_t n )
{
  dprintf2(("CRTDLL: wcsxfrm\n"));
  return (wcsxfrm(s1, s2, n));
}


/*********************************************************************
 *                  wctomb   (CRTDLL.521)
 */
int CDECL CRTDLL_wctomb( LPSTR dst, WCHAR ch )
{
  dprintf2(("CRTDLL: wctomb\n"));
  //return WideCharToMultiByte( CP_ACP, 0, &ch, 1, dst, 6, NULL, NULL );
  return (wctomb((char*)dst,ch));
}
