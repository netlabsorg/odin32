/* $Id: logic.h,v 1.1 2000-02-29 00:48:33 sandervl Exp $ */

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





#ifndef LOGIC_H
#define LOGIC_H


#include "types.h"


extern void gl_LogicOp( GLcontext *ctx, GLenum opcode );


extern void gl_logicop_ci_span( GLcontext *ctx,
                                GLuint n, GLint x, GLint y, GLuint index[],
                                const GLubyte mask[] );


extern void gl_logicop_ci_pixels( GLcontext *ctx,
                                  GLuint n, const GLint x[], const GLint y[],
                                  GLuint index[], const GLubyte mask[] );


extern void gl_logicop_rgba_span( GLcontext *ctx,
                                  GLuint n, GLint x, GLint y,
                                  GLubyte rgba[][4], const GLubyte mask[] );


extern void gl_logicop_rgba_pixels( GLcontext *ctx,
                                    GLuint n, const GLint x[], const GLint y[],
                                    GLubyte rgba[][4], const GLubyte mask[] );


#endif
