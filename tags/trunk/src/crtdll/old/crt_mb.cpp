/* $Id: crt_mb.cpp,v 1.1 2000-11-21 23:48:54 phaller Exp $ */

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
 *
 * CRTDLL - multibyte functions
 *
 */

#include <odin.h>
#include <os2win.h>
#include <ctype.h>
#include <heapstring.h>
#include <string.h>

#define MB_LEN_MAX	2


/*********************************************************************
 *                  _mbscat       (CRTDLL.195)
 */
unsigned char * CDECL CRTDLL__mbscat( unsigned char *dst, const unsigned char *src )
{
  dprintf2(("CRTDLL: _mbscat\n"));
  return (unsigned char*)strcat((char*)dst,(char*)src);
}


/*********************************************************************
 *                  _mbschr       (CRTDLL.196)
 */
unsigned char * CDECL CRTDLL__mbschr( const unsigned char *str, unsigned int c )
{
  dprintf2(("CRTDLL: _mbschr\n"));
  return (unsigned char*)strchr((char*)str,c);
}


/*********************************************************************
 *                  _mbscmp       (CRTDLL.197)
 */
int CDECL CRTDLL__mbscmp( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbscmp\n"));
  return strcmp((char*)s1,(char*)s2);
}


/*********************************************************************
 *                  _mbscpy       (CRTDLL.198)
 */
unsigned char * CDECL CRTDLL__mbscpy( unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbscpy\n"));
  return (unsigned char*)strcpy((char*)s1,(char*)s2);
}


/*********************************************************************
 *           _mbsdup    (CRTDLL.201)
 */
unsigned char * CDECL CRTDLL__mbsdup( unsigned char *s1 )
{
  dprintf2(("CRTDLL: _mbsdup\n"));
  return (unsigned char*)strdup((const char*)s1);
}


/*********************************************************************
 *           CRTDLL__mbsicmp   (CRTDLL.202)
 */
int CDECL CRTDLL__mbsicmp( const unsigned char *x, const unsigned char *y )
{
  dprintf2(("CRTDLL: _mbsicmp\n"));
    do {
	if (!*x)
	    return !!*y;
	if (!*y)
	    return !!*x;
	/* FIXME: MBCS handling... */
	if (*x!=*y)
	    return 1;
        x++;
        y++;
    } while (1);
}


/*********************************************************************
 *           _mbsinc    (CRTDLL.203)
 */
LPSTR CDECL CRTDLL__mbsinc( LPCSTR str )
{
    dprintf2(("CRTDLL: _mbsinc\n"));
    int len = mblen( str, MB_LEN_MAX );
    if (len < 1) len = 1;
    return (LPSTR)(str + len);
}


/*********************************************************************
 *           _mbslen    (CRTDLL.204)
 */
INT CDECL CRTDLL__mbslen( LPCSTR str )
{
    dprintf2(("CRTDLL: _mbslen\n"));
    INT len, total = 0;
    while ((len = mblen( str, MB_LEN_MAX )) > 0)
    {
        str += len;
        total++;
    }
    return total;
}


/*********************************************************************
 *           _mbsrchr   (CRTDLL.221)
 */
LPSTR CDECL CRTDLL__mbsrchr(LPSTR s,CHAR x)
{
  dprintf2(("CRTDLL: _mbsrchr\n"));
	/* FIXME: handle multibyte strings */
	return strrchr(s,x);
}


/*********************************************************************
 *           _mbsstr    (CRTDLL.226)
 */
unsigned char * CDECL CRTDLL__mbsstr( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbsstr\n"));
  return (unsigned char*)strstr((const char*)s1,(const char*)s2);
}


/*********************************************************************
 *	mblen					(CRTDLL.425)
 */
INT CDECL CRTDLL_mblen( const char *s, size_t n )
{
      dprintf2(("CRTDLL: mblen\n"));
      return (mblen(s, n));
}


/*********************************************************************
 *	mbstowcs				(CRTDLL.426)
 */
size_t CDECL CRTDLL_mbstowcs( wchar_t *pwcs, const char *s, size_t n )
{
      dprintf2(("CRTDLL: mbstowcs(%08xh, %08xh, %08xh)\n", pwcs, s, n));
      return (mbstowcs(pwcs, s, n));
}


/*********************************************************************
 *	mbtowc					(CRTDLL.427)
 */
INT CDECL CRTDLL_mbtowc( WCHAR *dst, LPCSTR str, INT n )
{
    dprintf2(("CRTDLL: mbtowc\n"));
    wchar_t res;
    int ret = mbtowc( &res, str, n );
    if (dst) *dst = (WCHAR)res;
    return ret;
}



