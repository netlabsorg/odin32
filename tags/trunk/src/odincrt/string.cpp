/* $Id: string.cpp,v 1.1 1999-12-09 10:58:15 sandervl Exp $ */
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

  length--; // pre-decrement to avoid exceeding buffer length
            // results in better code than (dest > 1)

  for (;*p2 && length; length--)
    *p1++ = *p2++;

  *p1 = 0; //CB: copy arg-1, set end 0

  return dest;
}
