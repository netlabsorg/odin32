/* $Id: string.c,v 1.1 2000-11-22 01:11:01 phaller Exp $ */

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
 * Implementation Notes:
 * MT Safe.
 */

//#include <odin.h>
//#include <os2win.h>
//#include <ctype.h>
//#include <heapstring.h>
#include <string.h>


#include "crtdll.h"
#include "winuser.h"


DEFAULT_DEBUG_CHANNEL(crtdll);

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
  LPSTR y=x;
  
  dprintf2(("CRTDLL: _strupr(%s)\n",
           x));

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


/* INTERNAL: CRTDLL_malloc() based strndup */
LPSTR __CRTDLL__strndup(LPSTR buf, INT size)
{
  char* ret;
  int len = strlen(buf);
  int max_len;

  max_len = size <= len? size : len + 1;

  ret = (char*)CRTDLL_malloc(max_len);
  if (ret)
  {
    memcpy(ret,buf,max_len);
    ret[max_len] = 0;
  }
  return ret;
}


/*********************************************************************
 *                  _strdec           (CRTDLL.282)
 *
 * Return the byte before str2 while it is >= to str1. 
 *
 * PARAMS
 *   str1 [in]  Terminating string
 *
 *   sre2 [in]  string to start searching from
 *
 * RETURNS
 *   The byte before str2, or str1, whichever is greater
 *
 * NOTES
 * This function is implemented as tested with windows, which means
 * it does not have a terminating condition. It always returns
 * the byte before str2. Use with extreme caution!
 */
LPSTR CDECL CRTDLL__strdec(LPSTR str1, LPSTR str2)
{
  /* Hmm. While the docs suggest that the following should work... */
  /*  return (str2<=str1?0:str2-1); */
  /* ...Version 2.50.4170 (NT) from win98 constantly decrements! */
  return str2-1;
}


/*********************************************************************
 *                  _strdup          (CRTDLL.285)
 *
 * Duplicate a string.
 */
LPSTR CDECL CRTDLL__strdup(LPCSTR ptr)
{
    LPSTR ret = (LPSTR)CRTDLL_malloc(strlen(ptr)+1);
    if (ret) strcpy( ret, ptr );
    return ret;
}


/*********************************************************************
 *                  _strinc           (CRTDLL.287)
 *
 * Return a pointer to the next character in a string
 */
LPSTR CDECL CRTDLL__strinc(LPSTR str)
{
  return str+1;
}


/*********************************************************************
 *                   _strnextc         (CRTDLL.290)
 *
 * Return an unsigned int from a string.
 */
UINT CDECL CRTDLL__strnextc(LPCSTR str)
{
  return (UINT)*str;
}


/*********************************************************************
 *                  _strninc           (CRTDLL.292)
 *
 * Return a pointer to the 'n'th character in a string
 */
LPSTR CDECL CRTDLL__strninc(LPSTR str, INT n)
{
  return str+n;
}


/*********************************************************************
 *                  _strnset           (CRTDLL.293)
 *
 * Fill a string with a character up to a certain length
 */
LPSTR CDECL CRTDLL__strnset(LPSTR str, INT c, INT len)
{
  if (len > 0 && str)
    while (*str && len--)
      *str++ = c;
  return str;
}


/*********************************************************************
 *                  _strrev              (CRTDLL.294)
 *
 * Reverse a string in place
 */
LPSTR CDECL CRTDLL__strrev (LPSTR str)
{
  LPSTR p1;
  LPSTR p2;
 
  if (str && *str)
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
      *p1 ^= *p2;
      *p2 ^= *p1;
      *p1 ^= *p2;
    }

  return str;
}

/*********************************************************************
 *                  _strset          (CRTDLL.295)
 *
 * Fill a string with a value.
 */
LPSTR  CDECL CRTDLL__strset (LPSTR str, INT set)
{
  char *ptr = str;

  while (*ptr)
    *ptr++ = set;

  return str;
}


/*********************************************************************
 *                  _strncnt           (CRTDLL.289)
 *
 * Return the length of a string or the maximum given length.
 */
LONG CDECL CRTDLL__strncnt(LPSTR str, LONG max)
{
  LONG len = strlen(str);
  return (len > max? max : len);
}


/*********************************************************************
 *                  _strspnp           (CRTDLL.296)
 *
 */
LPSTR CDECL CRTDLL__strspnp(LPSTR str1, LPSTR str2)
{
  str1 += strspn(str1,str2);
  return *str1? str1 : 0;
}


/*********************************************************************
 *                  _swab           (CRTDLL.299)
 *
 * Copy from source to dest alternating bytes (i.e 16 bit big-to-little
 * endian or vice versa).
 */
void CDECL CRTDLL__swab(LPSTR src, LPSTR dst, INT len)
{
  //_swab(s1, s2, i);
  
  if (len > 1)
  {
    len = (unsigned)len >> 1;

    while (len--) {
      *dst++ = src[1];
      *dst++ = *src++;
      src++;
    }
  }
}
