/* $Id: string.cpp,v 1.2 2000-07-06 21:17:02 sandervl Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 * strncpy replacement (one in RTL lib is buggy; doesn't stop at 0 terminator)
 *
 * Copyright 1999 Sander van Leeuwen
 * Copyright 1999 Patrick Haller
 *
 */
#include <stdio.h>

char * _LNK_CONV __strncpy( char *dest, const char *src, size_t length)
{
  register char *p1 = dest;
  register char *p2 = (char *)src;

  for (;*p2 && length; length--)
    *p1++ = *p2++;

  if ( length > 0 ) // above loop ended due to nul in src
    *p1 = 0;

  return dest;
}
