/* $Id: stencil.h,v 1.2 2000-05-23 20:34:57 jeroen Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.3
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





#ifndef STENCIL_H
#define STENCIL_H


#include "types.h"


extern void
_mesa_ClearStencil( GLint s );


extern void
_mesa_StencilFunc( GLenum func, GLint ref, GLuint mask );


extern void
_mesa_StencilMask( GLuint mask );


extern void
_mesa_StencilOp( GLenum fail, GLenum zfail, GLenum zpass );



extern GLboolean
gl_stencil_and_depth_test_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
                                const GLdepth z[], GLubyte mask[] );

#ifdef VMS /* VMS allows externals of 31 characters maximum */
#define gl_stencil_and_depth_test_pixels gl_stencil_and_depth_test_pixel
#endif
extern GLboolean
gl_stencil_and_depth_test_pixels( GLcontext *ctx, GLuint n,
                                  const GLint x[], const GLint y[],
                                  const GLdepth z[], GLubyte mask[] );



extern void
gl_read_stencil_span( GLcontext *ctx, GLint n, GLint x, GLint y,
                      GLstencil stencil[] );


extern void
gl_write_stencil_span( GLcontext *ctx, GLint n, GLint x, GLint y,
                       const GLstencil stencil[] );


extern void gl_alloc_stencil_buffer( GLcontext *ctx );


extern void gl_clear_stencil_buffer( GLcontext *ctx );


#endif
