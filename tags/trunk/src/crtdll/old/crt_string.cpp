/* $Id: crt_string.cpp,v 1.1 2000-11-21 23:48:54 phaller Exp $ */

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
 * CRTDLL - string functions
 *
 */

#include <odin.h>
#include <os2win.h>
#include <ctype.h>
#include <heapstring.h>
#include <string.h>


/*********************************************************************
 *           _strcmpi   	 (CRTDLL.280)
 */
void CDECL CRTDLL__strcmpi( LPCSTR s1, LPCSTR s2 )
{
  dprintf2(("CRTDLL: _strcmpi(%08xh, %08xh)\n",
           s1,
           s2));

  lstrcmpiA( s1, s2 );
}


/*********************************************************************
 *           CRTDLL__strdate   	 (CRTDLL.281)
 */
char * CDECL CRTDLL__strdate( char *buf )
{
  dprintf2(("CRTDLL: _strdate\n"));
  return(_strdate(buf));
}


/*********************************************************************
 *           CRTDLL__strdec    	 (CRTDLL.282)
 */
char * CDECL CRTDLL__strdec( const char *, const char *p ) 
{
  dprintf2(("CRTDLL: _strdec\n"));
  return( (char *)(p-1) );
}


/*********************************************************************
 *           CRTDLL__strdup    	 (CRTDLL.283)
 */
LPSTR CDECL CRTDLL__strdup(LPCSTR ptr)
{
  dprintf2(("CRTDLL: _strdup\n"));
  return HEAP_strdupA(GetProcessHeap(),0,ptr);
}


/*********************************************************************
 *           _strerror  	 (CRTDLL.284)
 */
char * CDECL CRTDLL__strerror(const char *s)
{
  dprintf(("CRTDLL: _strerror\n"));
  return (_strerror((char*)s));
}


/*********************************************************************
 *           _stricmp  	 (CRTDLL.285)
 */
int CDECL CRTDLL__stricmp(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: _stricmp(%s,%s)\n",
           str1,
           str2));

  return (stricmp(str1, str2));
}



/*********************************************************************
 *           CRTDLL__stricoll  	 (CRTDLL.286)
 */
int CDECL CRTDLL__stricoll( const char *s1, const char *s2 )
{
  dprintf2(("CRTDLL: _stricoll\n"));
  return stricmp(s1,s2);
}


/*********************************************************************
 *           CRTDLL__strinc  	 (CRTDLL.287)
 */
char * CDECL CRTDLL__strinc( const char *p )
{
    dprintf2(("CRTDLL: _strinc\n"));
    return( (char *)(p+1) );
}


/*********************************************************************
 *           _strlwr  	 (CRTDLL.288)
 */
CHAR * CDECL CRTDLL__strlwr(char *x)
{
  char *y =x;

  dprintf2(("CRTDLL: _strlwr got %s\n", x));
  while (*y) {
    if ((*y > 0x40) && (*y< 0x5b))
      *y = *y + 0x20;
    y++;
  }
  dprintf2(("   returned %s\n", x));
		
  return x;
}


/*********************************************************************
 *           CRTDLL__strncnt	 (CRTDLL.289)
 */
size_t CDECL CRTDLL__strncnt( const char *p, size_t l ) 
{
    dprintf2(("CRTDLL: _strncnt\n"));
    size_t i;
    i = strlen(p);
    return( (i>l) ? l : i );
}


/*********************************************************************
 *           CRTDLL__strnextc	 (CRTDLL.290)
 */
unsigned int CDECL CRTDLL__strnextc( const char *p )
{
    dprintf2(("CRTDLL: _strnextc\n"));
    return( (unsigned int)*p );
}


/*********************************************************************
 *           CRTDLL__strnicmp	 (CRTDLL.291)
 */
int  CDECL CRTDLL__strnicmp( LPCSTR s1, LPCSTR s2, INT n )
{
  dprintf2(("CRTDLL: _strnicmp (%s,%s,%d)\n",
           s1,
           s2,
           n));

  // @@@PH: sure it's not a UNICODE API?
  return (lstrncmpiA(s1,s2,n));
}


/*********************************************************************
 *           CRTDLL__strninc	 (CRTDLL.292)
 */
char * CDECL CRTDLL__strninc( const char *p, size_t l )
{
    dprintf2(("CRTDLL: _strninc\n"));
    return( (char *)(p+l) );
}


/*********************************************************************
 *           CRTDLL__strnset	 (CRTDLL.293)
 */
char * CDECL CRTDLL__strnset(char *string, int c, size_t count)
{
  dprintf2(("CRTDLL: _strnset\n"));
  char *dst;

  dst = string;
  while (count > 0 && *dst != 0)
    {
      *dst++ = (char)c;
      --count;
    }
  return string;
}


/*********************************************************************
 *           CRTDLL__strrev	 (CRTDLL.294)
 */
char * CDECL CRTDLL__strrev( char *string )
{
  dprintf2(("CRTDLL: _strrev\n"));
  char *p, *q, c;

  p = q = string;
  while (*q != 0)
    ++q;
  --q;                                  /* Benign, as string must be != 0 */
  while ((size_t)q > (size_t)p)
    {
      c = *p; *p = *q; *q = c;
      ++p; --q;
    }
  return string;
}


/*********************************************************************
 *           CRTDLL__strset	 (CRTDLL.295)
 */
char * CDECL CRTDLL__strset(char *string, int c)
{
  dprintf2(("CRTDLL: _strset\n"));
  char *dst;

  dst = string;
  while (*dst != 0)
    *dst++ = (char)c;
  return string;
}


/*********************************************************************
 *           CRTDLL__strspnp	 (CRTDLL.296)
 */
char * CDECL CRTDLL__strspnp( const char *p1, const char *p2 ) 
{
    dprintf2(("CRTDLL: _strspnp\n"));
    return( (*(p1 += strspn(p1,p2))!='\0') ? (char*)p1 : NULL );
}


/*********************************************************************
 *           CRTDLL__strtime	 (CRTDLL.297)
 */
char * CDECL CRTDLL__strtime( char *buf )
{
  dprintf2(("CRTDLL: _strtime\n"));
  return (_strtime(buf));
}


/*********************************************************************
 *           _strupr 	 (CRTDLL.298)
 */
LPSTR CDECL CRTDLL__strupr(LPSTR x)
{
  dprintf2(("CRTDLL: _strupr(%s)\n",
           x));

  LPSTR y=x;

  while (*y)
  {
    *y=toupper(*y);
    y++;
  }
  return x;
}


/*********************************************************************
 *                  strcat         (CRTDLL.459)
 */
LPSTR CDECL CRTDLL_strcat(      LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: strcat\n"));

  return (strcat(str1, str2));
}


/*********************************************************************
 *                  strchr         (CRTDLL.460)
 */
LPSTR CDECL CRTDLL_strchr(const LPSTR str,
                       int         i)
{
  dprintf2(("CRTDLL: strchr(%s,%08xh)\n",
           str,
           i));

  return (strchr(str, i));
}


/*********************************************************************
 *                  strcmp         (CRTDLL.461)
 */
int CDECL CRTDLL_strcmp(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: strcmp(%s,%s)\n",
           str1,
           str2));

  return (strcmp(str1, str2));
}


/*********************************************************************
 *                  strcoll        (CRTDLL.462)
 */
int CDECL CRTDLL_strcoll( const char *s1, const char *s2 )
{
  dprintf2(("CRTDLL: strcoll\n"));
  return strcoll(s1, s2);
}


/*********************************************************************
 *                  strcpy         (CRTDLL.463)
 */
LPSTR CDECL CRTDLL_strcpy(      LPSTR str1,
                       const LPSTR str2)
{
  dprintf2(("CRTDLL: strcpy\n"));

  return (strcpy(str1, str2));
}


/*********************************************************************
 *                  strcspn         (CRTDLL.464)
 */
size_t CDECL CRTDLL_strcspn(const LPSTR str1,
                                LPSTR str2)
{
  dprintf2(("CRTDLL: strcspn(%s,%s)\n",
           str1,
           str2));

  return (strcspn(str1, str2));
}


/*********************************************************************
 *                  strerror        (CRTDLL.465)
 */
char * CDECL CRTDLL_strerror( int errnum )
{
  dprintf2(("CRTDLL: strerror\n"));
  return strerror(errnum);
}


/*********************************************************************
 *                  strftime        (CRTDLL.466)
 */
size_t CDECL CRTDLL_strftime( char *s, size_t maxsiz, const char *fmt, const struct tm *tp )
{
  dprintf2(("CRTDLL: strftime\n"));
  return strftime(s, maxsiz, fmt, tp);
}


/*********************************************************************
 *                  strlen          (CRTDLL.467)
 */
size_t CDECL CRTDLL_strlen(const LPSTR str)
{
  dprintf2(("CRTDLL: strlen(%s)\n",
           str));

  return (strlen(str));
}


/*********************************************************************
 *                  strncat        (CRTDLL.468)
 */
LPSTR CDECL CRTDLL_strncat(      LPSTR str1,
                        const LPSTR str2,
                        size_t      i)
{
  dprintf2(("CRTDLL: strncat(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (strncat(str1, str2, i));
}


/*********************************************************************
 *                  strncmp        (CRTDLL.469)
 */
int CDECL CRTDLL_strncmp(const LPSTR str1,
                       const LPSTR str2,
                       size_t      i)
{
  dprintf2(("CRTDLL: strncmp(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (strncmp(str1, str2, i));
}


/*********************************************************************
 *                  strncpy        (CRTDLL.470)
 */
LPSTR CDECL CRTDLL_strncpy(const LPSTR str1,
                        const LPSTR str2,
                        size_t      i)
{
  dprintf2(("CRTDLL: strncpy(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (strncpy(str1, str2, i));
}


/*********************************************************************
 *                  strpbrk        (CRTDLL.471)
 */
LPSTR CDECL CRTDLL_strpbrk(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: strpbrk(%s,%s)\n",
           str1,
           str2));

  return (strpbrk(str1, str2));
}


/*********************************************************************
 *                  strrchr        (CRTDLL.472)
 */
LPSTR CDECL CRTDLL_strrchr(const LPSTR str,
                        size_t      i)
{
  dprintf2(("CRTDLL: strrchr(%s,%08xh)\n",
           str,
           i));

  return (strrchr(str, i));
}


/*********************************************************************
 *                  strspn        (CRTDLL.473)
 */
size_t CDECL CRTDLL_strspn(const LPSTR str1,
                       const LPSTR str2)
{
  dprintf2(("CRTDLL: strspn(%s,%s)\n",
           str1,
           str2));

  return (strspn(str1, str2));
}


/*********************************************************************
 *                  strstr        (CRTDLL.474)
 */
LPSTR CDECL CRTDLL_strstr(const LPSTR str1,
                       const LPSTR str2)
{
  dprintf2(("CRTDLL: strstr(%s,%s)\n",
           str1,
           str2));

  return (strstr(str1, str2));
}
 

/*********************************************************************
 *                  strtod        (CRTDLL.475)
 */
double CDECL CRTDLL_strtod( const char *nptr, char **endptr )
{
  dprintf2(("CRTDLL: strtod\n"));
  return strtod(nptr, endptr);
}


/*********************************************************************
 *                  strtok        (CRTDLL.476)
 */
char * CDECL CRTDLL_strtok( char *s1, const char *s2 )
{
  dprintf2(("CRTDLL: strtok\n"));
  return strtok(s1, s2);
}


/*********************************************************************
 *                  strtol        (CRTDLL.477)
 */
long int CDECL CRTDLL_strtol( const char *nptr, char **endptr, int base )
{
  dprintf2(("CRTDLL: strtol\n"));
  return strtol(nptr, endptr, base);
}


/*********************************************************************
 *                  strtoul        (CRTDLL.478)
 */
unsigned long CDECL CRTDLL_strtoul( const char *nptr, char **endptr, int base )
{
  dprintf2(("CRTDLL: strtoul\n"));
  return strtoul(nptr, endptr, base);
}


/*********************************************************************
 *                  strxfrm        (CRTDLL.479)
 */
size_t CDECL CRTDLL_strxfrm( char *s1, const char *s2, size_t n )
{
  dprintf2(("CRTDLL: strxfrm\n"));
  return strxfrm(s1, s2, n);
}
