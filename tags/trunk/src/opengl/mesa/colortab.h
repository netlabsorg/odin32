/* $Id: colortab.h,v 1.1 2000-02-29 00:48:26 sandervl Exp $ */

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





#ifndef COLORTAB_H
#define COLORTAB_H


#include "types.h"


extern void gl_ColorTable( GLcontext *ctx, GLenum target,
                           GLenum internalformat,
                           struct gl_image *table );

extern void gl_ColorSubTable( GLcontext *ctx, GLenum target,
                              GLsizei start, struct gl_image *data );

extern void gl_GetColorTable( GLcontext *ctx, GLenum target, GLenum format,
                              GLenum type, GLvoid *table );

extern void gl_GetColorTableParameterfv( GLcontext *ctx, GLenum target,
                                         GLenum pname, GLfloat *params );

extern void gl_GetColorTableParameteriv( GLcontext *ctx, GLenum target,
                                         GLenum pname, GLint *params );


#endif
