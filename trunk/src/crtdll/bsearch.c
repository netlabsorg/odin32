/* $Id: bsearch.c,v 1.1 1999-12-20 11:09:41 sandervl Exp $ */
/* bsearch.c (emx+gcc) -- Copyright (c) 1990-1995 by Eberhard Mattes */

#include <odin.h>
#include <builtin.h>

void *CDECL bsearch (const void *key, const void *base, size_t num, size_t width,
                     int (* CDECL compare)(const void *key, const void *element))
{
  int left, right, median, sign;
  const void *element;
  
  if (width == 0)
    return 0;
  left = 1; right = num;
  while (left <= right)
    {
      median = (left + right) / 2;
      element = (void *)((char *)base + (median-1)*width);
      sign = compare (key, element);
      if (sign == 0)
        return (void *)element;
      if (sign > 0)
        left = median + 1;
      else
        right = median - 1;
    }
  return 0;
}
