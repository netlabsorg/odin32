/* $Id: glut_warp.c,v 1.2 2000-02-09 08:46:19 jeroen Exp $ */
/* Copyright (c) Mark J. Kilgard, 1996, 1997. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "glutint.h"

/* CENTRY */
void APIENTRY
glutWarpPointer(int x, int y)
{
  XWarpPointer(__glutDisplay, None, __glutCurrentWindow->win,
    0, 0, 0, 0, x, y);
  XFlush(__glutDisplay);
}

/* ENDCENTRY */
