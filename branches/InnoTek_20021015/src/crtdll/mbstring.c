/* $Id: mbstring.c,v 1.1 2000-11-21 23:48:52 phaller Exp $ */

/*
 * The C RunTime DLL
 * 
 * Implements C run-time functionality as known from UNIX.
 *
 * Partialy based on Wine
 *
 * Copyright 1999 Alexandre Julliard
 *
 * CRTDLL multi-byte string functions
 *
 */

//#include <odin.h>
//#include <os2win.h>
//#include <ctype.h>
//#include <heapstring.h>
#include <string.h>

#define MB_LEN_MAX	2

#include "crtdll.h"

DEFAULT_DEBUG_CHANNEL(crtdll);


/*********************************************************************
 *                  _mbsicmp      (CRTDLL.204)
 */
int CDECL CRTDLL__mbsicmp(unsigned char *x,unsigned char *y)
{
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
 *           CRTDLL__mbsinc    (CRTDLL.205)
 */
LPSTR CDECL CRTDLL__mbsinc( LPCSTR str )
{
  //  int len = mblen( str, MB_LEN_MAX );
  //  if (len < 1) len = 1;
  //  return (LPSTR)(str + len);
  
  if (IsDBCSLeadByte( *str )) str++;
    return (LPSTR)(str + 1);
}


/*********************************************************************
 *           CRTDLL__mbslen    (CRTDLL.206)
 */
INT CDECL CRTDLL__mbslen( LPCSTR str )
{
  INT len;
  
  //  INT len, total = 0;
  //  while ((len = mblen( str, MB_LEN_MAX )) > 0)
  //  {
  //      str += len;
  //      total++;
  //  }
  //  return total;

  
  for (len = 0; *str; len++, str++) 
    if (IsDBCSLeadByte(str[0]) && str[1]) 
      str++;
  
  return len;
}



/*********************************************************************
 *                  _mbsrchr           (CRTDLL.223)
 */
LPSTR CDECL CRTDLL__mbsrchr(LPSTR s,CHAR x)
{
    /* FIXME: handle multibyte strings */
    return strrchr(s,x);
}


/*********************************************************************
 *           CRTDLL_mbtowc    (CRTDLL.430)
 */
INT CDECL CRTDLL_mbtowc( WCHAR *dst, LPCSTR str, INT n )
{
  //  wchar_t res;
  //  int ret = mbtowc( &res, str, n );
  //  if (dst) *dst = (WCHAR)res;
  //  return ret;
  
    if (n <= 0) return 0;
    if (!str) return 0;
    if (!MultiByteToWideChar( CP_ACP, 0, str, n, dst, 1 )) return 0;
    /* return the number of bytes from src that have been used */
    if (!*str) return 0;
    if (n >= 2 && IsDBCSLeadByte(*str) && str[1]) return 2;
    return 1;
}


/*********************************************************************
 *                  _mbccpy           (CRTDLL.??)
 *
 * Copy one multibyte character to another
 */
VOID CDECL CRTDLL__mbccpy(LPSTR dest, LPSTR src)
{
  // @@@PH looks very good, indeed ... .beg.
  FIXME("MBCS copy treated as ASCII\n");
  *dest = *src;
}


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
