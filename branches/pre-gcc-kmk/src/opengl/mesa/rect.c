/* $Id: rect.c,v 1.3 2000-05-23 20:40:52 jeroen Exp $ */

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
#include "glheader.h"
#include "types.h"
#include "context.h"
#include "macros.h"
#include "rect.h"
#include "vbfill.h"
#endif



/*
 * Execute a glRect*() function.
 */
void
_mesa_Rectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
   /*
    * TODO: we could examine a bunch of state variables and ultimately
    * call the Driver->RectFunc() function to draw a screen-aligned
    * filled rectangle.  Someday...
    *
    * KW: What happens to cull mode here?
    */
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx, "glRect");
   RESET_IMMEDIATE(ctx);
   gl_Begin( ctx, GL_QUADS );
   gl_Vertex2f( ctx, x1, y1 );
   gl_Vertex2f( ctx, x2, y1 );
   gl_Vertex2f( ctx, x2, y2 );
   gl_Vertex2f( ctx, x1, y2 );
   gl_End( ctx );
}


void
_mesa_Rectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
   _mesa_Rectf(x1, y1, x2, y2);
}

void
_mesa_Rectdv(const GLdouble *v1, const GLdouble *v2)
{
   _mesa_Rectf(v1[0], v1[1], v2[0], v2[1]);
}

void
_mesa_Rectfv(const GLfloat *v1, const GLfloat *v2)
{
   _mesa_Rectf(v1[0], v1[1], v2[0], v2[1]);
}

void
_mesa_Recti(GLint x1, GLint y1, GLint x2, GLint y2)
{
   _mesa_Rectf(x1, y1, x2, y2);
}

void
_mesa_Rectiv(const GLint *v1, const GLint *v2)
{
   _mesa_Rectf(v1[0], v1[1], v2[0], v2[1]);
}

void
_mesa_Rects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
   _mesa_Rectf(x1, y1, x2, y2);
}

void
_mesa_Rectsv(const GLshort *v1, const GLshort *v2)
{
   _mesa_Rectf(v1[0], v1[1], v2[0], v2[1]);
}

