/* $Id: glmisc.h,v 1.1 2000-02-29 00:48:31 sandervl Exp $ */

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


/* $XFree86: xc/lib/GL/mesa/src/glmisc.h,v 1.1 1999/03/14 03:20:45 dawes Exp $ */

#ifndef GLMISC_H
#define GLMISC_H


#include "types.h"


extern void gl_ClearIndex( GLcontext *ctx, GLfloat c );

extern void gl_ClearColor( GLcontext *ctx, GLclampf red, GLclampf green,
                           GLclampf blue, GLclampf alpha );

extern void gl_Clear( GLcontext *ctx, GLbitfield mask );

extern void gl_Finish( GLcontext *ctx );

extern void gl_Flush( GLcontext *ctx );

extern GLboolean gl_Hint( GLcontext *ctx, GLenum target, GLenum mode );

extern void gl_DrawBuffer( GLcontext *ctx, GLenum mode );

extern void gl_ReadBuffer( GLcontext *ctx, GLenum mode );


#endif
