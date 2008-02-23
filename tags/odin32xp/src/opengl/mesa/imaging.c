/* $Id: imaging.c,v 1.1 2000-05-23 20:40:38 jeroen Exp $ */

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


/* Stub functions for GL_ARB_imaging subset */


#ifdef PC_HEADER
#include "all.h"
#else
#include "glheader.h"
#include "imaging.h"
#endif



#define WARNING(MSG)  fprintf(stderr,"Mesa warning: GL_ARB_imaging functions not implemented\n")



#if 0
void _mesa_BlendColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
   (void) r;
   (void) g;
   (void) b;
   (void) a;
   WARNING("glBlendColor");
}

void _mesa_BlendEquation(GLenum eq)
{
   (void) eq;
   WARNING("glBlendEquation");
}

void _mesa_ColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data)
{
   (void) target;
   (void) start;
   (void) count;
   (void) format;
   (void) type;
   (void) data;
   WARNING("glColorSubTable");
}

void _mesa_ColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table)
{
   (void) target;
   (void) internalformat;
   (void) width;
   (void) format;
   (void) type;
   (void) table;
   WARNING("glColorTable");
}
#endif

void _mesa_ColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glColorTableParameterfv");
}

void _mesa_ColorTableParameteriv(GLenum target, GLenum pname, const GLint *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glColorTableParameteriv");
}


void _mesa_ConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image)
{
   (void) target;
   (void) internalformat;
   (void) width;
   (void) format;
   (void) type;
   (void) image;
   WARNING("glConvolutionFilter1D");
}

void _mesa_ConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image)
{
   (void) target;
   (void) internalformat;
   (void) width;
   (void) height;
   (void) format;
   (void) type;
   (void) image;
   WARNING("glConvolutionFilter2D");
}

void _mesa_ConvolutionParameterf(GLenum target, GLenum pname, GLfloat params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glConvolutionParameterf");
}

void _mesa_ConvolutionParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glConvolutionParameterfv");
}

void _mesa_ConvolutionParameteri(GLenum target, GLenum pname, GLint params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glConvolutionParameteri");
}

void _mesa_ConvolutionParameteriv(GLenum target, GLenum pname, const GLint *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glConvolutionParameteriv");
}

void _mesa_CopyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
   (void) target;
   (void) start;
   (void) x;
   (void) y;
   (void) width;
   WARNING("glCopyColorSubTable");
}

void _mesa_CopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
   (void) target;
   (void) internalformat;
   (void) x;
   (void) y;
   (void) width;
   WARNING("glCopyColorTable");
}

void _mesa_CopyConvolutionFilter1D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
   (void) target;
   (void) internalformat;
   (void) x;
   (void) y;
   (void) width;
   WARNING("glCopyConvolutionFilter1D");
}

void _mesa_CopyConvolutionFilter2D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
   (void) target;
   (void) internalformat;
   (void) x;
   (void) y;
   (void) width;
   (void) height;
   WARNING("glCopyConvolutionFilter2D");
}

#if 0
void _mesa_GetColorTable(GLenum target, GLenum format, GLenum type, GLvoid *table)
{
   (void) target;
   (void) format;
   (void) type;
   (void) table;
   WARNING("glGetColorTable");
}

void _mesa_GetColorTableParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetColorTableParameterfv");
}

void _mesa_GetColorTableParameteriv(GLenum target, GLenum pname, GLint *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetColorTableParameteriv");
}
#endif

void _mesa_GetConvolutionFilter(GLenum target, GLenum format, GLenum type, GLvoid *image)
{
   (void) target;
   (void) format;
   (void) type;
   (void) image;
   WARNING("glGetConvolutionFilter");
}

void _mesa_GetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetConvolutionParameterfv");
}

void _mesa_GetConvolutionParameteriv(GLenum target, GLenum pname, GLint *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetConvolutionParameteriv");
}

void _mesa_GetMinmax(GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values)
{
   (void) target;
   (void) reset;
   (void) format;
   (void) types;
   (void) values;
   WARNING("glGetMinmax");
}

void _mesa_GetHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values)
{
   (void) target;
   (void) reset;
   (void) format;
   (void) type;
   (void) values;
   WARNING("glGetHistogram");
}

void _mesa_GetHistogramParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetHistogramParameterfv");
}

void _mesa_GetHistogramParameteriv(GLenum target, GLenum pname, GLint *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetHistogramParameteriv");
}

void _mesa_GetMinmaxParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetMinmaxParameterfv");
}

void _mesa_GetMinmaxParameteriv(GLenum target, GLenum pname, GLint *params)
{
   (void) target;
   (void) pname;
   (void) params;
   WARNING("glGetMinmaxParameteriv");
}

void _mesa_GetSeparableFilter(GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span)
{
   (void) target;
   (void) format;
   (void) type;
   (void) row;
   (void) column;
   (void) span;
   WARNING("glGetSeperableFilter");
}

void _mesa_Histogram(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
   (void) target;
   (void) width;
   (void) internalformat;
   (void) sink;
   WARNING("glHistogram");
}

void _mesa_Minmax(GLenum target, GLenum internalformat, GLboolean sink)
{
   (void) target;
   (void) internalformat;
   (void) sink;
   WARNING("glMinmax");
}

void _mesa_ResetHistogram(GLenum target)
{
   (void) target;
   WARNING("glResetHistogram");
}

void _mesa_ResetMinmax(GLenum target)
{
   (void) target;
   WARNING("glResetMinmax");
}

void _mesa_SeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column)
{
   (void) target;
   (void) internalformat;
   (void) width;
   (void) height;
   (void) format;
   (void) type;
   (void) row;
   (void) column;
   WARNING("glSeparableFilter2D");
}
