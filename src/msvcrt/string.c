/*
 * MSVCRT string functions
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997,2000 Uwe Bonnes
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

#include "msvcrt.h"
#include "msvcrt/stdlib.h"
#include "msvcrt/string.h"
#include <string.h>
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

/* INTERNAL: MSVCRT_malloc() based strndup */
char* msvcrt_strndup(const char* buf, unsigned int size)
{
  char* ret;
  unsigned int len = strlen(buf), max_len;

  max_len = size <= len? size : len + 1;

  ret = MSVCRT_malloc(max_len);
  if (ret)
  {
    memcpy(ret,buf,max_len);
    ret[max_len] = 0;
  }
  return ret;
}

/*********************************************************************
 *		_mbsdup (MSVCRT.@)
 *		_strdup (MSVCRT.@)
 */
char* MSVCRT__strdup(const char* str)
{
    char * ret = MSVCRT_malloc(strlen(str)+1);
    if (ret) strcpy( ret, str );
    return ret;
}

/*********************************************************************
 *		_strnset (MSVCRT.@)
 */
char* MSVCRT__strnset(char* str, int value, unsigned int len)
{
  dprintf(("MSVCRT: _strnset %s %d %d",str, value, len));
  if (len > 0 && str)
    while (*str && len--)
      *str++ = value;
  return str;
}

/*********************************************************************
 *		_strrev (MSVCRT.@)
 */
char* MSVCRT__strrev(char* str)
{
  char * p1;
  char * p2;

  dprintf(("MSVCRT: _strrev %s",str));

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
 *		_strset (MSVCRT.@)
 */
char* MSVCRT__strset(char* str, int value)
{
  char *ptr = str;

  dprintf(("MSVCRT: _strset %s %d",str, value));

  while (*ptr)
    *ptr++ = value;

  return str;
}

/*********************************************************************
 *		_swab (MSVCRT.@)
 */
void MSVCRT__swab(char* src, char* dst, int len)
{
  dprintf(("MSVCRT: _swab %s %s %d",src, dst, len));

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

int MSVCRT_strncmp (const char* a,const char* b,MSVCRT(size_t) c)
{
  dprintf(("MSVCRT: strncmp %s,%s (%d)",a,b,c));
  return strncmp(a,b,c);
}

