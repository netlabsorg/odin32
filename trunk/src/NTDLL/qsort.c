/* qsort.c (emx+gcc) -- Copyright (c) 1990-1994 by Eberhard Mattes */

#include <odin.h>
#include <memory.h>
#include <string.h>

 void _memswap (void *s1, void *s2, size_t n)
 {
   char *c1, *c2, c;
   int *i1, *i2, i;
 
   i1 = s1; i2 = s2;
   while (n >= sizeof (int))
     {
       i = *i1; *i1++ = *i2; *i2++ = i;
       n -= sizeof (int);
       }
   c1 = (char *)i1; c2 = (char *)i2;
   while (n >= 1)
     {
       c = *c1; *c1++ = *c2; *c2++ = c;
       --n;
     }
 }

static void qsort1 (char *l, char *r, size_t width,
                    int (* CDECL compare)(const void *x1, const void *x2));

void CDECL qsort (void *base, size_t num, size_t width,
                  int (*CDECL compare)(const void *x1, const void *x2))
{
  if (width > 0 && num > 1 && base != 0)
    qsort1 ((char *)base, (char *)base+(num-1)*width, width, compare);
}


static void qsort1 (char *l, char *r, size_t width,
                    int (* CDECL compare)(const void *x1, const void *x2))
{
  char *i, *j, *x;

redo:
  i = l; j = r;
  x = l + width * (((r-l) / width) >> 1);
  do
    {
      while (i != x && compare ((void *)i, (void *)x) < 0)
        i += width;
      while (j != x && compare ((void *)j, (void *)x) > 0)
        j -= width;
      if (i < j)
        {
          _memswap (i, j, width);
          if (x == i)
            x = j;
          else if (x == j)
            x = i;
        }
      if (i <= j)
        {
          i += width;           /* (almost) no danger */
          if (j > l)
            j -= width;         /* avoid wrap-around */
        }
    } while (i <= j);
  if (j-l < r-i)
    {
      if (l < j)
        qsort1 (l, j, width, compare);
      if (i < r)
        {
          l = i;
          goto redo;            /* qsort1 (i, r, width, compare) */
        }
    }
  else
    {
      if (i < r)
        qsort1 (i, r, width, compare);
      if (l < j)
        {
          r = j;                /* qsort1 (l, j, width, compare) */
          goto redo;
        }
    }
}
