/* $Id: rect.c,v 1.2 2000-03-01 22:45:25 jeroen Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.1
 *
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* $XFree86: xc/lib/GL/mesa/src/rect.c,v 1.2 1999/04/04 00:20:30 dawes Exp $ */





#ifdef PC_HEADER
#include "all.h"
#else
#ifndef XFree86Server
#else
#include "GL/xf86glx.h"
#endif
#include "types.h"
#include "context.h"
#include "macros.h"
#include "rect.h"
#include "vbfill.h"
#endif



/*
 * Execute a glRect*() function.
 */
void gl_Rectf( GLcontext *ctx, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
   /*
    * TODO: we could examine a bunch of state variables and ultimately
    * call the Driver->RectFunc() function to draw a screen-aligned
    * filled rectangle.  Someday...
    *
    * KW: What happens to cull mode here?
    */
   ASSERT_OUTSIDE_BEGIN_END(ctx, "glRect");
   RESET_IMMEDIATE(ctx);
   gl_Begin( ctx, GL_QUADS );
   gl_Vertex2f( ctx, x1, y1 );
   gl_Vertex2f( ctx, x2, y1 );
   gl_Vertex2f( ctx, x2, y2 );
   gl_Vertex2f( ctx, x1, y2 );
   gl_End( ctx );
}
