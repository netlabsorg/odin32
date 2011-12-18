/* $Id: glutos2.h,v 1.6 2000-05-20 13:48:23 jeroen Exp $ */
#ifndef __glutos2_h__
#define __glutos2_h__

/* Copyright (c) Nate Robins, 1997. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include "os2_x11.h"
#include "os2_glx.h"
/* KSO: Dirty trick to get around problems with gettimeofday being
 * defined in the sys\time.h in the toolkit. (at least in CSD2)
 */
#define gettimeofday gettimeofday_os2
#include <sys/time.h>
#undef gettimeofday

/* We have to undef some things because Microsoft likes to pollute the
   global namespace. */
#undef TRANSPARENT

/* Win32 "equivalent" cursors - eventually, the X glyphs should be
   converted to Win32 cursors -- then they will look the same */
#define XC_arrow               IDC_ARROW
#define XC_top_left_arrow      IDC_ARROW
#define XC_hand1               IDC_SIZEALL
#define XC_pirate              IDC_NO
#define XC_question_arrow      IDC_HELP
#define XC_exchange            IDC_NO
#define XC_spraycan            IDC_SIZEALL
#define XC_watch               IDC_WAIT
#define XC_xterm               IDC_IBEAM
#define XC_crosshair           IDC_CROSS
#define XC_sb_v_double_arrow   IDC_SIZENS
#define XC_sb_h_double_arrow   IDC_SIZEWE
#define XC_top_side            IDC_UPARROW
#define XC_bottom_side         IDC_SIZENS
#define XC_left_side           IDC_SIZEWE
#define XC_right_side          IDC_SIZEWE
#define XC_top_left_corner     IDC_SIZENWSE
#define XC_top_right_corner    IDC_SIZENESW
#define XC_bottom_right_corner IDC_SIZENWSE
#define XC_bottom_left_corner  IDC_SIZENESW
#define XA_STRING 0

/* Private routines from win32_util.c */
extern int gettimeofday(struct timeval* tp, void* tzp);
extern void *__glutFont(void *font);
extern int __glutGetTransparentPixel(Display *dpy, XVisualInfo *vinfo);
extern void __glutAdjustCoords(Window parent, int *x, int *y, int *width, int *height);

#endif                                 /* __glutos2_h__                    */
