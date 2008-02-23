#ifdef __WIN32OS2__
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#endif

#include "wine/unicode.h"
#include "msvcrt.h"

#include "msvcrt/stdlib.h"


#include "wine/debug.h"

char *dbgemx__gcvt (double a, int b, char * c)
{ 
 char *ret;
 dprintf(("MSVCRT: _gcvt %f, %d %s", a, b, c));
 ret = emx__gcvt(a, b, c);
 dprintf(("MSVCRT: dbgemx_gcvt returned %s", ret));
 return ret; 
}

double _emx__hypot (double, double);

double _dbgemx__hypot (double a, double b)
{
 double ret;
 dprintf(("MSVCRT: _hypot %f, %f", a, b));
 ret = _emx__hypot(a, b);
 dprintf(("MSVCRT: _hypot returned %f", ret));
 return ret; 
}

char *dbgemx__itoa(int a,char *b, int c)
{
 char *ret;
 dprintf(("MSVCRT: _itoa %d, %s %d", a, b, c));
 ret =emx__itoa(a, b, c);
 dprintf(("MSVCRT: _itoa returned %s", ret));
 return ret; 
}

char *dbgemx__ltoa(long a,char *b, int c)
{
 char *ret;
 dprintf(("MSVCRT: _ltoa %d, %s %d", a, b, c));
 ret =emx__ltoa(a, b, c);
 dprintf(("MSVCRT: _ltoa returned %s", ret));
 return ret; 
}

char *dbgemx_strcpy (char * a, const char * b)
{
 char *ret;
 dprintf(("MSVCRT: strcpy %s, %s", a, b));
 ret =emx_strcpy(a, b);
 dprintf(("MSVCRT: strcpy returned %s", ret));
 return ret; 
}

char *dbgemx_strncpy (char * a, const char * b, size_t c)
{
 char *ret;
 dprintf(("MSVCRT: strncpy %p, %s, %d", a, b, c));
 ret =emx_strncpy(a, b, c);
 dprintf(("MSVCRT: strncpy returned %s", ret));
 return ret; 
}

char *dbgemx_strcat (char * a, const char * b)
{
 char *ret;
 dprintf(("MSVCRT: strcat %s, %s", a, b));
 ret =emx_strcat(a, b);
 dprintf(("MSVCRT: strcat returned %s", ret));
 return ret; 
}

char *dbgemx_strncat (char * a, const char * b, size_t c)
{
 char *ret;
 dprintf(("MSVCRT: strncat %s, %s, %d", a, b, c));
 ret =emx_strncat(a, b, c);
 dprintf(("MSVCRT: strncat returned %s", ret));
 return ret; 
}

char *dbgemx_strstr (const char * a, const char * b)
{
 char *ret;
 dprintf(("MSVCRT: strstr %s, %s", a, b));
 ret =emx_strstr(a, b);
 dprintf(("MSVCRT: strstr returned %s", ret));
 return ret; 
}

char *dbgemx__strupr (char * a)
{
 char *ret;
 dprintf(("MSVCRT: strupr %s", a));
 ret =emx__strupr(a);
 dprintf(("MSVCRT: strupr returned %s", ret));
 return ret; 
}

char *dbgemx__strlwr (char * a)
{
 char *ret;
 dprintf(("MSVCRT: strlwr %s", a));
 ret =emx__strlwr(a);
 dprintf(("MSVCRT: strlwr returned %s", ret));
 return ret; 
}

void *dbgemx__memccpy (void * a, const void *b, int c, size_t d)
{
 void *ret;
 dprintf(("MSVCRT: _memccpy %p %p %d %d", a, b, c, d));
 ret =emx__memccpy(a, b, c, d);
 dprintf(("MSVCRT: _memccpy returned %p", ret));
 return ret; 
}

void *dbgemx_bsearch (const void * a, const void *b, size_t c, size_t d,
    int (*e)(const void *, const void *))
{
 void *ret;
 dprintf(("MSVCRT: bsearch %p %p %d %d %p", a, b, c, d, e));
 ret = bsearch(a, b, c, d, e);
 dprintf(("MSVCRT: bsearch returned %p", ret));
 return ret; 
}

void dbgemx_qsort (void *a, size_t b, size_t c,
    int (*d)(__const__ void *, __const__ void *))
{
 dprintf(("MSVCRT: qsort %p %d %d %p", a, b, c, d));
 qsort(a, b, c, d);
}

int dbgemx_vsprintf (char * a, const char * b, va_list c)
{
 int ret;
 dprintf(("MSVCRT: vsprintf %s %s %p", a, b, c));
 ret = vsprintf(a, b, c);
 dprintf(("MSVCRT: vsprintf returned %d", ret));
 return ret; 
}

int dbgemx__vsnprintf (char * a, size_t b, const char * c, va_list d)
{
 int ret;
 dprintf(("MSVCRT: _vsnprintf %p %d %s %p", a, b, c, d));
 ret = _vsnprintf(a, b, c, d);
 dprintf(("MSVCRT: _vsnprintf returned %d", ret));
 return ret; 
}

char *dbgemx__ultoa (unsigned long a, char * b, int c)
{
 char *ret;
 dprintf(("MSVCRT: _ultoa %ld %p %d", a, b, c));
 ret =emx__ultoa(a, b, c);
 dprintf(("MSVCRT: _ultoa returned %s", ret));
 return ret; 
}

double dbgemx_floor (double a)
{
 double ret;
 dprintf(("MSVCRT: floor %f", a));
 ret = floor(a);
 dprintf(("MSVCRT: floor returned %f", ret));
 return ret;
}

int dbgemx_atoi (const char * a)
{
 int ret;
 dprintf(("MSVCRT: atoi %s", a));
 ret = atoi(a);
 dprintf(("MSVCRT: atoi returned %d", ret));
 return ret; 
}

int dbgemx_atol (const char * a)
{
 long ret;
 dprintf(("MSVCRT: atol %s", a));
 ret = atol(a);
 dprintf(("MSVCRT: atol returned %d", ret));
 return ret; 
}

double dbgemx_fabs (double a)
{
 double ret;
 dprintf(("MSVCRT: fabs %f", a));
 ret = fabs(a);
 dprintf(("MSVCRT: fabs returned %f", ret));
 return ret;
}

int dbgemx__stricmp (const char *a, const char *b)
{
 int ret;
 dprintf(("MSVCRT: _stricmp %s, %s", a, b));
 ret =emx__stricmp(a, b);
 dprintf(("MSVCRT: stricmp returned %d", ret));
 return ret; 
}

int dbgemx__strnicmp (const char *a, const char *b, size_t c)
{
 int ret;
 dprintf(("MSVCRT: _strnicmp %s, %s, %d", a, b, c));
 ret =emx__strnicmp(a, b, c);
 dprintf(("MSVCRT: _strnicmp returned %d", ret));
 return ret; 
}

char *dbgemx_strchr (const char *a, int b)
{
 char *ret;
 dprintf(("MSVCRT: strchr %a, %d", a, b));
 ret =emx_strchr(a, b);
 dprintf(("MSVCRT: strchr returned %s", ret));
 return ret; 
}


int dbgemx_strcmp (const char *a, const char *b)
{
 int ret;
 dprintf(("MSVCRT: strcmp %s, %s", a, b));
 ret =emx_strcmp(a, b);
 dprintf(("MSVCRT: strcmp returned %d", ret));
 return ret; 
}
