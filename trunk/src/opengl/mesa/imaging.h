/* $Id: imaging.h,v 1.1 2000-05-23 20:34:51 jeroen Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.3
 *
 * Copyright (C) 1999-2000  Brian Paul   All Rights Reserved.
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


#ifndef IMAGING_H
#define IMAGING_H


#ifdef PC_HEADER
#include "all.h"
#else
#include "glheader.h"
#endif



#if 0
extern void _mesa_BlendColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);

extern void _mesa_BlendEquation(GLenum eq);

extern void _mesa_ColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);

extern void _mesa_ColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
#endif

extern void _mesa_ColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params);

extern void _mesa_ColorTableParameteriv(GLenum target, GLenum pname, const GLint *params);


extern void _mesa_ConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);

extern void _mesa_ConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);

extern void _mesa_ConvolutionParameterf(GLenum target, GLenum pname, GLfloat params);

extern void _mesa_ConvolutionParameterfv(GLenum target, GLenum pname, const GLfloat *params);

extern void _mesa_ConvolutionParameteri(GLenum target, GLenum pname, GLint params);

extern void _mesa_ConvolutionParameteriv(GLenum target, GLenum pname, const GLint *params);

extern void _mesa_CopyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width);

extern void _mesa_CopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);

extern void _mesa_CopyConvolutionFilter1D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);

extern void _mesa_CopyConvolutionFilter2D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);

#if 0
extern void _mesa_GetColorTable(GLenum target, GLenum format, GLenum type, GLvoid *table);

extern void _mesa_GetColorTableParameterfv(GLenum target, GLenum pname, GLfloat *params);

extern void _mesa_GetColorTableParameteriv(GLenum target, GLenum pname, GLint *params);
#endif

extern void _mesa_GetConvolutionFilter(GLenum target, GLenum format, GLenum type, GLvoid *image);

extern void _mesa_GetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat *params);

extern void _mesa_GetConvolutionParameteriv(GLenum target, GLenum pname, GLint *params);

extern void _mesa_GetMinmax(GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values);

extern void _mesa_GetHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);

extern void _mesa_GetHistogramParameterfv(GLenum target, GLenum pname, GLfloat *params);

extern void _mesa_GetHistogramParameteriv(GLenum target, GLenum pname, GLint *params);

extern void _mesa_GetMinmaxParameterfv(GLenum target, GLenum pname, GLfloat *params);

extern void _mesa_GetMinmaxParameteriv(GLenum target, GLenum pname, GLint *params);

extern void _mesa_GetSeparableFilter(GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);

extern void _mesa_Histogram(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);

extern void _mesa_Minmax(GLenum target, GLenum internalformat, GLboolean sink);

extern void _mesa_ResetHistogram(GLenum target);

extern void _mesa_ResetMinmax(GLenum target);

extern void _mesa_SeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);

#endif
