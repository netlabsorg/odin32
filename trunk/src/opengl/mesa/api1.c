/* $Id: api1.c,v 1.4 2000-03-11 13:54:25 sandervl Exp $ */

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




#ifdef PC_HEADER
#include "all.h"
#else
#ifndef XFree86Server
#include <stdio.h>
#include <stdlib.h>
#else
#include "GL/xf86glx.h"
#endif
#include "api.h"
#include "bitmap.h"
#include "context.h"
#include "drawpix.h"
#include "eval.h"
#include "image.h"
#include "light.h"
#include "macros.h"
#include "matrix.h"
#include "mmath.h"
#include "teximage.h"
#include "types.h"
#include "varray.h"
#include "vb.h"
#include "vbfill.h"
#endif

#include <misc.h>

/*
 * Part 1 of API functions
 */

void GLAPIENTRY glAccum(CTX_ARG GLenum op, GLfloat value )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Accum)(CC, op, value);
}


void GLAPIENTRY glAlphaFunc(CTX_ARG GLenum func, GLclampf ref )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.AlphaFunc)(CC, func, ref);
}


GLboolean GLAPIENTRY glAreTexturesResident(CTX_ARG GLsizei n, const GLuint *textures,
                                            GLboolean *residences )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(GL_FALSE);
   return (*CC->API.AreTexturesResident)(CC, n, textures, residences);
}

/* Enough funny business going on in here it might be quicker to use a
 * function pointer.
 */
#define ARRAY_ELT( IM, i )                                      \
{                                                               \
   GLuint count = IM->Count;                                    \
   IM->Elt[count] = i;                                          \
   IM->Flag[count] = ((IM->Flag[count] & IM->ArrayAndFlags) |   \
                      VERT_ELT);                                \
   IM->FlushElt |= IM->ArrayEltFlush;                           \
   IM->Count = count += IM->ArrayIncr;                          \
   if (count == VB_MAX)                                         \
      IM->maybe_transform_vb( IM );                             \
}


void GLAPIENTRY glArrayElement(CTX_ARG GLint i )
{
   GET_IMMEDIATE;
   ARRAY_ELT( IM, i );
}


void GLAPIENTRY glArrayElementEXT(CTX_ARG GLint i )
{
   GET_IMMEDIATE;
   ARRAY_ELT( IM, i );
}


void gl_ArrayElement( GLcontext *CC, GLint i )
{
   struct immediate *im = CC->input;
   ARRAY_ELT( im, i );
}


void GLAPIENTRY glBegin(CTX_ARG GLenum mode )
{
   GET_CONTEXT;

   if (mode < GL_POINTS || mode > GL_POLYGON) {
      gl_compile_error( CC, GL_INVALID_ENUM, "glBegin" );
      return;
   }

   gl_Begin(CC,mode);
}


void GLAPIENTRY glBindTexture(CTX_ARG GLenum target, GLuint texture )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.BindTexture)(CC, target, texture);
}


void GLAPIENTRY glBitmap(CTX_ARG  GLsizei width, GLsizei height,
                         GLfloat xorig, GLfloat yorig,
                         GLfloat xmove, GLfloat ymove,
                         const GLubyte *bitmap )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Bitmap)( CC, width, height, xorig, yorig, xmove, ymove, bitmap,
                      &CC->Unpack );
}


void GLAPIENTRY glBlendFunc(CTX_ARG GLenum sfactor, GLenum dfactor )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.BlendFunc)(CC, sfactor, dfactor);
}


void GLAPIENTRY glCallList(CTX_ARG GLuint list )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CallList)(CC, list);
}


void GLAPIENTRY glCallLists(CTX_ARG GLsizei n, GLenum type, const GLvoid *lists )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CallLists)(CC, n, type, lists);
}


void GLAPIENTRY glClear(CTX_ARG GLbitfield mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Clear)(CC, mask);
}


void GLAPIENTRY glClearAccum(CTX_ARG GLfloat red, GLfloat green,
                              GLfloat blue, GLfloat alpha )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ClearAccum)(CC, red, green, blue, alpha);
}



void GLAPIENTRY glClearIndex(CTX_ARG GLfloat c )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ClearIndex)(CC, c);
}


void GLAPIENTRY glClearColor(CTX_ARG GLclampf red, GLclampf green,
                              GLclampf blue, GLclampf alpha )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ClearColor)(CC, red, green, blue, alpha);
}


void GLAPIENTRY glClearDepth(CTX_ARG GLclampd depth )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ClearDepth)( CC, depth );
}


void GLAPIENTRY glClearStencil(CTX_ARG GLint s )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ClearStencil)(CC, s);
}


void GLAPIENTRY glClipPlane(CTX_ARG GLenum plane, const GLdouble *equation )
{
   GLfloat eq[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   eq[0] = (GLfloat) equation[0];
   eq[1] = (GLfloat) equation[1];
   eq[2] = (GLfloat) equation[2];
   eq[3] = (GLfloat) equation[3];
   (*CC->API.ClipPlane)(CC, plane, eq );
}


/* KW: Again, a stateless implementation of these functions.  The big
 * news here is the impact on color material.  This was previously
 * handled by swaping the function pointers that these API's used to
 * call.  This is no longer possible, and we have to pick up the
 * pieces later on and make them work with either color-color, or
 * color-material.
 *
 * But in truth, this is not a bad thing, because it was necessary
 * to implement that mechanism to get good performance from
 * color-material and vertex arrays.
 */
#define COLOR( IM, r,g,b,a )                    \
{                                               \
   GLuint count = IM->Count;                    \
   IM->Flag[count] |= VERT_RGBA;                \
   IM->Color[count][0] = r;                     \
   IM->Color[count][1] = g;                     \
   IM->Color[count][2] = b;                     \
   IM->Color[count][3] = a;                     \
}

#if 0
#define COLOR4F( IM, r,g,b,a )                          \
{                                                       \
   GLuint count = IM->Count;                            \
   IM->Flag[count] |= VERT_RGBA | VERT_FLOAT_RGBA;      \
   IM->FloatColor[count][0] = r;                        \
   IM->FloatColor[count][1] = g;                        \
   IM->FloatColor[count][2] = b;                        \
   IM->FloatColor[count][3] = a;                        \
}
#else
#define COLOR4F(IM, r, g, b, a)                 \
{                                               \
   GLubyte col[4];                              \
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], r);       \
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], g);       \
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], b);       \
   FLOAT_COLOR_TO_UBYTE_COLOR(col[3], a);       \
   COLORV( IM, col );                           \
}
#endif



#define COLORV( IM, v )                         \
{                                               \
   GLuint count = IM->Count;                    \
   IM->Flag[count] |= VERT_RGBA;                \
   COPY_4UBV(IM->Color[count], v);              \
}


void GLAPIENTRY glColor3b(CTX_ARG GLbyte red, GLbyte green, GLbyte blue )
{
   GET_IMMEDIATE;
   COLOR( IM,
          BYTE_TO_UBYTE(red),
          BYTE_TO_UBYTE(green),
          BYTE_TO_UBYTE(blue),
          255 );
}


void GLAPIENTRY glColor3d(CTX_ARG GLdouble red, GLdouble green, GLdouble blue )
{
   GLubyte col[4];
   GLfloat r = red;
   GLfloat g = green;
   GLfloat b = blue;
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], r);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], g);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], b);
   col[3] = 255;
   COLORV( IM, col );

/*     COLOR4F( IM, red, green, blue, 1.0 ); */
}


void GLAPIENTRY glColor3f(CTX_ARG GLfloat red, GLfloat green, GLfloat blue )
{
   GLubyte col[4];
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], red);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], green);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], blue);
   col[3] = 255;
   COLORV( IM, col );

/*     COLOR4F( IM, red, green, blue, 1.0 ); */
}


void GLAPIENTRY glColor3i(CTX_ARG GLint red, GLint green, GLint blue )
{
   GET_IMMEDIATE;
   COLOR( IM, INT_TO_UBYTE(red),
          INT_TO_UBYTE(green),
          INT_TO_UBYTE(blue),
          255);
}


void GLAPIENTRY glColor3s(CTX_ARG GLshort red, GLshort green, GLshort blue )
{
   GET_IMMEDIATE;
   COLOR( IM, SHORT_TO_UBYTE(red),
          SHORT_TO_UBYTE(green),
          SHORT_TO_UBYTE(blue),
          255);
}


void GLAPIENTRY glColor3ub(CTX_ARG GLubyte red, GLubyte green, GLubyte blue )
{
   GET_IMMEDIATE;
   COLOR( IM, red, green, blue, 255 );
}


void GLAPIENTRY glColor3ui(CTX_ARG GLuint red, GLuint green, GLuint blue )
{
   GET_IMMEDIATE;
   COLOR( IM, UINT_TO_UBYTE(red),
          UINT_TO_UBYTE(green),
          UINT_TO_UBYTE(blue),
          255 );
}


void GLAPIENTRY glColor3us(CTX_ARG GLushort red, GLushort green, GLushort blue )
{
   GET_IMMEDIATE;
   COLOR( IM, USHORT_TO_UBYTE(red), USHORT_TO_UBYTE(green),
          USHORT_TO_UBYTE(blue),
          255 );
}


void GLAPIENTRY glColor4b(CTX_ARG GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
   GET_IMMEDIATE;
   COLOR( IM, BYTE_TO_UBYTE(red), BYTE_TO_UBYTE(green),
          BYTE_TO_UBYTE(blue), BYTE_TO_UBYTE(alpha) );
}


void GLAPIENTRY glColor4d(CTX_ARG GLdouble red, GLdouble green, GLdouble blue,
                           GLdouble alpha )
{
   GLubyte col[4];
   GLfloat r = red;
   GLfloat g = green;
   GLfloat b = blue;
   GLfloat a = alpha;
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], r);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], g);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], b);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[3], a);
   COLORV( IM, col );

/*     COLOR4F( IM, red, green, blue, alpha ); */
}


void GLAPIENTRY glColor4f(CTX_ARG GLfloat red, GLfloat green,
                           GLfloat blue, GLfloat alpha )
{
   GLubyte col[4];
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], red);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], green);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], blue);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[3], alpha);
   COLORV( IM, col );

/*     COLOR4F( IM, red, green, blue, alpha ); */
}

void GLAPIENTRY glColor4i(CTX_ARG GLint red, GLint green, GLint blue, GLint alpha )
{
   GET_IMMEDIATE;
   COLOR( IM, INT_TO_UBYTE(red), INT_TO_UBYTE(green),
          INT_TO_UBYTE(blue), INT_TO_UBYTE(alpha) );
}


void GLAPIENTRY glColor4s(CTX_ARG GLshort red, GLshort green,
                           GLshort blue, GLshort alpha )
{
   GET_IMMEDIATE;
   COLOR( IM, SHORT_TO_UBYTE(red), SHORT_TO_UBYTE(green),
          SHORT_TO_UBYTE(blue), SHORT_TO_UBYTE(alpha) );
}

void GLAPIENTRY glColor4ub(CTX_ARG GLubyte red, GLubyte green,
                            GLubyte blue, GLubyte alpha )
{
   GET_IMMEDIATE;
   COLOR( IM, red, green, blue, alpha );
}

void GLAPIENTRY glColor4ui(CTX_ARG GLuint red, GLuint green,
                            GLuint blue, GLuint alpha )
{
   GET_IMMEDIATE;
   COLOR( IM, UINT_TO_UBYTE(red), UINT_TO_UBYTE(green),
          UINT_TO_UBYTE(blue), UINT_TO_UBYTE(alpha) );
}

void GLAPIENTRY glColor4us(CTX_ARG GLushort red, GLushort green,
                            GLushort blue, GLushort alpha )
{
   GET_IMMEDIATE;
   COLOR( IM, USHORT_TO_UBYTE(red), USHORT_TO_UBYTE(green),
          USHORT_TO_UBYTE(blue), USHORT_TO_UBYTE(alpha) );
}


void GLAPIENTRY glColor3bv(CTX_ARG const GLbyte *v )
{
   GET_IMMEDIATE;
   COLOR( IM, BYTE_TO_UBYTE(v[0]), BYTE_TO_UBYTE(v[1]),
          BYTE_TO_UBYTE(v[2]), 255 );
}


void GLAPIENTRY glColor3dv(CTX_ARG const GLdouble *v )
{
   GLubyte col[4];
   GLfloat r = v[0];
   GLfloat g = v[1];
   GLfloat b = v[2];
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], r);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], g);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], b);
   col[3]= 255;
   COLORV( IM, col );

/*     COLOR4F( IM, v[0], v[1], v[2], v[3] ); */
}


void GLAPIENTRY glColor3fv(CTX_ARG const GLfloat *v )
{
   GLubyte col[4];
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], v[0]);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], v[1]);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], v[2]);
   col[3] = 255;
   COLORV( IM, col );

/*     COLOR4F( IM, v[0], v[1], v[2], v[3] ); */
}


void GLAPIENTRY glColor3iv(CTX_ARG const GLint *v )
{
   GET_IMMEDIATE;
   COLOR( IM, INT_TO_UBYTE(v[0]), INT_TO_UBYTE(v[1]),
          INT_TO_UBYTE(v[2]), 255 );
}


void GLAPIENTRY glColor3sv(CTX_ARG const GLshort *v )
{
   GET_IMMEDIATE;
   COLOR( IM, SHORT_TO_UBYTE(v[0]), SHORT_TO_UBYTE(v[1]),
          SHORT_TO_UBYTE(v[2]), 255 );
}


void GLAPIENTRY glColor3ubv(CTX_ARG const GLubyte *v )
{
   GET_IMMEDIATE;
   COLOR( IM, v[0], v[1], v[2], 255 );
}


void GLAPIENTRY glColor3uiv(CTX_ARG const GLuint *v )
{
   GET_IMMEDIATE;
   COLOR( IM, UINT_TO_UBYTE(v[0]), UINT_TO_UBYTE(v[1]),
          UINT_TO_UBYTE(v[2]), 255 );
}


void GLAPIENTRY glColor3usv(CTX_ARG const GLushort *v )
{
   GET_IMMEDIATE;
   COLOR( IM, USHORT_TO_UBYTE(v[0]), USHORT_TO_UBYTE(v[1]),
          USHORT_TO_UBYTE(v[2]), 255 );

}


void GLAPIENTRY glColor4bv(CTX_ARG const GLbyte *v )
{
   GET_IMMEDIATE;
   COLOR( IM, BYTE_TO_UBYTE(v[0]), BYTE_TO_UBYTE(v[1]),
          BYTE_TO_UBYTE(v[2]), BYTE_TO_UBYTE(v[3]) );
}


void GLAPIENTRY glColor4dv(CTX_ARG const GLdouble *v )
{
   GLubyte col[4];
   GLfloat r = v[0];
   GLfloat g = v[1];
   GLfloat b = v[2];
   GLfloat a = v[3];
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], r);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], g);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], b);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[3], a);
   COLORV( IM, col );

/*     COLOR4F( IM, v[0], v[1], v[2], v[3] ); */
}


void GLAPIENTRY glColor4fv(CTX_ARG const GLfloat *v )
{
   GLubyte col[4];
   GET_IMMEDIATE;
   FLOAT_COLOR_TO_UBYTE_COLOR(col[0], v[0]);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[1], v[1]);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[2], v[2]);
   FLOAT_COLOR_TO_UBYTE_COLOR(col[3], v[3]);
   COLORV( IM, col );

/*     COLOR4F( IM, v[0], v[1], v[2], v[3] ); */
}


void GLAPIENTRY glColor4iv(CTX_ARG const GLint *v )
{
   GET_IMMEDIATE;
   COLOR( IM, INT_TO_UBYTE(v[0]), INT_TO_UBYTE(v[1]),
          INT_TO_UBYTE(v[2]), INT_TO_UBYTE(v[3]) );
}


void GLAPIENTRY glColor4sv(CTX_ARG const GLshort *v )
{
   GET_IMMEDIATE;
   COLOR( IM, SHORT_TO_UBYTE(v[0]), SHORT_TO_UBYTE(v[1]),
          SHORT_TO_UBYTE(v[2]), SHORT_TO_UBYTE(v[3]) );
}


void GLAPIENTRY glColor4ubv(CTX_ARG const GLubyte *v )
{
   GET_IMMEDIATE;
   COLORV( IM, v );
}


void GLAPIENTRY glColor4uiv(CTX_ARG const GLuint *v )
{
   GET_IMMEDIATE;
   COLOR( IM, UINT_TO_UBYTE(v[0]), UINT_TO_UBYTE(v[1]),
          UINT_TO_UBYTE(v[2]), UINT_TO_UBYTE(v[3]) );
}


void GLAPIENTRY glColor4usv(CTX_ARG const GLushort *v )
{
   GET_IMMEDIATE;
   COLOR( IM, USHORT_TO_UBYTE(v[0]), USHORT_TO_UBYTE(v[1]),
          USHORT_TO_UBYTE(v[2]), USHORT_TO_UBYTE(v[3]) );
}


void GLAPIENTRY glColorMask(CTX_ARG GLboolean red, GLboolean green,
                             GLboolean blue, GLboolean alpha )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ColorMask)(CC, red, green, blue, alpha);
}


void GLAPIENTRY glColorMaterial(CTX_ARG GLenum face, GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ColorMaterial)(CC, face, mode);
}


void GLAPIENTRY glCopyPixels(CTX_ARG GLint x, GLint y, GLsizei width, GLsizei height,
                              GLenum type )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CopyPixels)(CC, x, y, width, height, type);
}


void GLAPIENTRY glCopyTexImage1D(CTX_ARG GLenum target, GLint level,
                                  GLenum internalformat,
                                  GLint x, GLint y,
                                  GLsizei width, GLint border )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CopyTexImage1D)( CC, target, level, internalformat,
                              x, y, width, border );
}


void GLAPIENTRY glCopyTexImage2D(CTX_ARG GLenum target, GLint level,
                                  GLenum internalformat,
                                  GLint x, GLint y,
                                  GLsizei width, GLsizei height, GLint border )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CopyTexImage2D)( CC, target, level, internalformat,
                              x, y, width, height, border );
}


void GLAPIENTRY glCopyTexSubImage1D(CTX_ARG GLenum target, GLint level,
                                     GLint xoffset, GLint x, GLint y,
                                     GLsizei width )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CopyTexSubImage1D)( CC, target, level, xoffset, x, y, width );
}


void GLAPIENTRY glCopyTexSubImage2D(CTX_ARG GLenum target, GLint level,
                                     GLint xoffset, GLint yoffset,
                                     GLint x, GLint y,
                                     GLsizei width, GLsizei height )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CopyTexSubImage2D)( CC, target, level, xoffset, yoffset,
                                 x, y, width, height );
}


/* 1.2 */
void GLAPIENTRY glCopyTexSubImage3D(CTX_ARG GLenum target, GLint level, GLint xoffset,
                                     GLint yoffset, GLint zoffset,
                                     GLint x, GLint y, GLsizei width,
                                     GLsizei height )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CopyTexSubImage3D)( CC, target, level, xoffset, yoffset,
                                 zoffset, x, y, width, height );
}



void GLAPIENTRY glCullFace(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.CullFace)(CC, mode);
}


void GLAPIENTRY glDepthFunc(CTX_ARG GLenum func )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DepthFunc)( CC, func );
}


void GLAPIENTRY glDepthMask(CTX_ARG GLboolean flag )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DepthMask)( CC, flag );
}


void GLAPIENTRY glDepthRange(CTX_ARG GLclampd near_val, GLclampd far_val )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DepthRange)( CC, near_val, far_val );
}


void GLAPIENTRY glDeleteLists(CTX_ARG GLuint list, GLsizei range )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DeleteLists)(CC, list, range);
}


void GLAPIENTRY glDeleteTextures(CTX_ARG GLsizei n, const GLuint *textures )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DeleteTextures)(CC, n, textures);
}


void GLAPIENTRY glDisable(CTX_ARG GLenum cap )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Disable)( CC, cap );
}


void GLAPIENTRY glDisableClientState(CTX_ARG GLenum cap )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DisableClientState)( CC, cap );
}


void GLAPIENTRY glDrawArrays(CTX_ARG GLenum mode, GLint first, GLsizei count )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   gl_DrawArrays(CC, mode, first, count);
}


void GLAPIENTRY glDrawBuffer(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DrawBuffer)(CC, mode);
}



void GLAPIENTRY glDrawPixels(CTX_ARG GLsizei width, GLsizei height, GLenum format,
                              GLenum type, const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   if (CC->CompileFlag || !gl_direct_DrawPixels(CC, &CC->Unpack, width, height,
                                                format, type, pixels)) {
      struct gl_image *image;
      image = gl_unpack_image( CC, width, height, format, type, pixels, &CC->Unpack );
      (*CC->API.DrawPixels)( CC, image );
      if (image->RefCount==0) {
         /* image not in display list */
         gl_free_image( image );
      }
   }
}


void GLAPIENTRY glEnable(CTX_ARG GLenum cap )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Enable)( CC, cap );
}


void GLAPIENTRY glEnableClientState(CTX_ARG GLenum cap )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.EnableClientState)( CC, cap );
}


/* KW: Both streams now go to the outside-begin-end state.  Raise
 *     errors for either stream if it was not in the inside state.
 */
void GLAPIENTRY glEnd(CTX_VOID )
{
   GLuint state;
   GLuint inflags;
   GET_IMMEDIATE;


   state = IM->BeginState;
   inflags = (~state) & (VERT_BEGIN_0|VERT_BEGIN_1);
   state |= inflags << 2;       /* errors */

   if (MESA_VERBOSE&VERBOSE_API) {
      if (MESA_VERBOSE&VERBOSE_IMMEDIATE)
         fprintf(stderr, "glEnd(IM %d), BeginState is %x, errors %x\n",
                 IM->id, state,
                 inflags<<2);
      else
         fprintf(stderr, ">");
   }


   if (inflags != (VERT_BEGIN_0|VERT_BEGIN_1))
   {
      GLuint count = IM->Count;
      GLuint last = IM->LastPrimitive;

      state &= ~(VERT_BEGIN_0|VERT_BEGIN_1); /* update state */
      IM->Flag[count] |= VERT_END;
      IM->NextPrimitive[IM->LastPrimitive] = count;
      IM->LastPrimitive = count;
      IM->Primitive[count] = GL_POLYGON+1;

      if (IM->FlushElt) {
         gl_exec_array_elements( IM->backref, IM, last, count );
         IM->FlushElt = 0;
      }
   }

   if (MESA_VERBOSE&VERBOSE_API)
      fprintf(stderr, "in glEnd final state %x\n", state);

   IM->BeginState = state;

   if ((MESA_DEBUG_FLAGS&DEBUG_ALWAYS_FLUSH))
      IM->maybe_transform_vb( IM );
}


void gl_End( GLcontext *ctx )
{
   struct immediate *IM = ctx->input;
   GLuint state = IM->BeginState;
   GLuint inflags = (~state) & (VERT_BEGIN_0|VERT_BEGIN_1);


   if (MESA_VERBOSE&VERBOSE_API) {
      if (MESA_VERBOSE&VERBOSE_IMMEDIATE)
         fprintf(stderr, "gl_End(IM %d), BeginState is %x, errors %x\n",
                 IM->id, state,
                 inflags<<2);
      else
         fprintf(stderr, ">");
   }

   state |= inflags << 2;       /* errors */

   if (inflags != (VERT_BEGIN_0|VERT_BEGIN_1))
   {
      GLuint count = IM->Count;
      GLuint last = IM->LastPrimitive;

      state &= ~(VERT_BEGIN_0|VERT_BEGIN_1); /* update state */
      IM->Flag[count] |= VERT_END;
      IM->NextPrimitive[IM->LastPrimitive] = count;
      IM->LastPrimitive = count;
      IM->Primitive[count] = GL_POLYGON+1;

      if (IM->FlushElt) {
         gl_exec_array_elements( ctx, IM, last, count );
         IM->FlushElt = 0;
      }
   }

   IM->BeginState = state;

   /* You can set this flag to get the old 'flush vb on glEnd()'
    * behaviour.
    */
   if ((MESA_DEBUG_FLAGS&DEBUG_ALWAYS_FLUSH))
      IM->maybe_transform_vb( IM );
}

void GLAPIENTRY glEndList(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.EndList)(CC);
}

/* KW: If are compiling, we don't know whether eval will produce a
 *     vertex when it is run in the future.  If this is pure immediate
 *     mode, eval is a noop if neither vertex map is enabled.
 *
 *     Thus we need to have a check in the display list code or
 *     elsewhere for eval(1,2) vertices in the case where
 *     map(1,2)_vertex is disabled, and to purge those vertices from
 *     the vb.  This is currently done
 *     via  modifications to the cull_vb and render_vb operations, and
 *     by using the existing cullmask mechanism for all other operations.
 */


/* KW: Because the eval values don't become 'current', fixup will flow
 *     through these vertices, and then evaluation will write on top
 *     of the fixup results.
 *
 *     This is a little inefficient, but at least it is correct.  This
 *     could be short-circuited in the case where all vertices are
 *     eval-vertices, or more generally by a cullmask in fixup.
 *
 *     Note: using Obj to hold eval coord data.  This data is actually
 *     transformed if eval is disabled.  But disabling eval & sending
 *     eval coords is stupid, right?
 */


#define EVALCOORD1(IM, x)                               \
{                                                       \
   GLuint count = IM->Count++;                          \
   IM->Flag[count] |= VERT_EVAL_C1;                     \
   ASSIGN_4V(IM->Obj[count], x, 0, 0, 1);               \
   if (count == VB_MAX-1)                               \
      IM->maybe_transform_vb( IM );                     \
}

#define EVALCOORD2(IM, x, y)                            \
{                                                       \
   GLuint count = IM->Count++;                          \
   IM->Flag[count] |= VERT_EVAL_C2;                     \
   ASSIGN_4V(IM->Obj[count], x, y, 0, 1);               \
   if (count == VB_MAX-1)                               \
      IM->maybe_transform_vb( IM );                     \
}

#define EVALPOINT1(IM, x)                               \
{                                                       \
   GLuint count = IM->Count++;                          \
   IM->Flag[count] |= VERT_EVAL_P1;                     \
   ASSIGN_4V(IM->Obj[count], x, 0, 0, 1);               \
   if (count == VB_MAX-1)                               \
      IM->maybe_transform_vb( IM );                     \
}

#define EVALPOINT2(IM, x, y)                            \
{                                                       \
   GLuint count = IM->Count++;                          \
   IM->Flag[count] |= VERT_EVAL_P2;                     \
   ASSIGN_4V(IM->Obj[count], x, y, 0, 1);               \
   if (count == VB_MAX-1)                               \
      IM->maybe_transform_vb( IM );                     \
}


void GLAPIENTRY glEvalCoord1d(CTX_ARG GLdouble u )
{
   GET_IMMEDIATE;
   EVALCOORD1( IM, (GLfloat) u );
}


void GLAPIENTRY glEvalCoord1f(CTX_ARG GLfloat u )
{
   GET_IMMEDIATE;
   EVALCOORD1( IM, u );
}


/* Lame internal function:
 */
void gl_EvalCoord1f( GLcontext *CC, GLfloat u )
{
   struct immediate *i = CC->input;
   EVALCOORD1( i, u );
}


void GLAPIENTRY glEvalCoord1dv(CTX_ARG const GLdouble *u )
{
   GET_IMMEDIATE;
   EVALCOORD1( IM, (GLfloat) *u );
}


void GLAPIENTRY glEvalCoord1fv(CTX_ARG const GLfloat *u )
{
   GET_IMMEDIATE;
   EVALCOORD1( IM, (GLfloat) *u );
}


void GLAPIENTRY glEvalCoord2d(CTX_ARG GLdouble u, GLdouble v )
{
   GET_IMMEDIATE;
   EVALCOORD2( IM, (GLfloat) u, (GLfloat) v );
}


void GLAPIENTRY glEvalCoord2f(CTX_ARG GLfloat u, GLfloat v )
{
   GET_IMMEDIATE;
   EVALCOORD2( IM, u, v );
}


/* Lame internal function:
 */
void gl_EvalCoord2f( GLcontext *CC, GLfloat u, GLfloat v )
{
   struct immediate *i = CC->input;
   EVALCOORD2( i, u, v );
}


void GLAPIENTRY glEvalCoord2dv(CTX_ARG const GLdouble *u )
{
   GET_IMMEDIATE;
   EVALCOORD2( IM, (GLfloat) u[0], (GLfloat) u[1] );
}


void GLAPIENTRY glEvalCoord2fv(CTX_ARG const GLfloat *u )
{
   GET_IMMEDIATE;
   EVALCOORD2( IM, u[0], u[1] );
}


void GLAPIENTRY glEvalPoint1(CTX_ARG GLint i )
{
   GET_IMMEDIATE;
   EVALPOINT1( IM, i );
}


void GLAPIENTRY glEvalPoint2(CTX_ARG GLint i, GLint j )
{
   GET_IMMEDIATE;
   EVALPOINT2( IM, i, j );
}


void GLAPIENTRY glEvalMesh1(CTX_ARG GLenum mode, GLint i1, GLint i2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.EvalMesh1)( CC, mode, i1, i2 );
}


void GLAPIENTRY glEdgeFlag(CTX_ARG GLboolean flag )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->EdgeFlag[count] = flag;
   IM->Flag[count] |= VERT_EDGE;
}


void GLAPIENTRY glEdgeFlagv(CTX_ARG const GLboolean *flag )
{
   GLuint count;
   GET_IMMEDIATE;
   count = IM->Count;
   IM->EdgeFlag[count] = *flag;
   IM->Flag[count] |= VERT_EDGE;
}



void GLAPIENTRY glEvalMesh2(CTX_ARG GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.EvalMesh2)( CC, mode, i1, i2, j1, j2 );
}


void GLAPIENTRY glFeedbackBuffer(CTX_ARG GLsizei size, GLenum type, GLfloat *buffer )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.FeedbackBuffer)(CC, size, type, buffer);
}


void GLAPIENTRY glFinish(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Finish)(CC);
}


void GLAPIENTRY glFlush(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Flush)(CC);
}


void GLAPIENTRY glFogf(CTX_ARG GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Fogfv)(CC, pname, &param);
}


void GLAPIENTRY glFogi(CTX_ARG GLenum pname, GLint param )
{
   GLfloat fparam = (GLfloat) param;
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Fogfv)(CC, pname, &fparam);
}


void GLAPIENTRY glFogfv(CTX_ARG GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Fogfv)(CC, pname, params);
}


void GLAPIENTRY glFogiv(CTX_ARG GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   CHECK_CONTEXT;

   switch (pname) {
      case GL_FOG_MODE:
      case GL_FOG_DENSITY:
      case GL_FOG_START:
      case GL_FOG_END:
      case GL_FOG_INDEX:
         p[0] = (GLfloat) *params;
         break;
      case GL_FOG_COLOR:
         p[0] = INT_TO_FLOAT( params[0] );
         p[1] = INT_TO_FLOAT( params[1] );
         p[2] = INT_TO_FLOAT( params[2] );
         p[3] = INT_TO_FLOAT( params[3] );
         break;
      default:
         /* Error will be caught later in gl_Fogfv */
         ;
   }
   (*CC->API.Fogfv)( CC, pname, p );
}


void GLAPIENTRY glFrontFace(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.FrontFace)(CC, mode);
}


void GLAPIENTRY glFrustum(CTX_ARG GLdouble left, GLdouble right,
                         GLdouble bottom, GLdouble top,
                         GLdouble nearval, GLdouble farval )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Frustum)(CC, left, right, bottom, top, nearval, farval);
}


GLuint GLAPIENTRY glGenLists(CTX_ARG GLsizei range )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(0);
   return (*CC->API.GenLists)(CC, range);
}


void GLAPIENTRY glGenTextures(CTX_ARG GLsizei n, GLuint *textures )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GenTextures)(CC, n, textures);
}


void GLAPIENTRY glGetBooleanv(CTX_ARG GLenum pname, GLboolean *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetBooleanv)(CC, pname, params);
}


void GLAPIENTRY glGetClipPlane(CTX_ARG GLenum plane, GLdouble *equation )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetClipPlane)(CC, plane, equation);
}


void GLAPIENTRY glGetDoublev(CTX_ARG GLenum pname, GLdouble *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetDoublev)(CC, pname, params);
}


GLenum GLAPIENTRY glGetError(CTX_VOID )
{
   GET_CONTEXT;
   if (!CC) {
      /* No current context */
      return (GLenum) GL_NO_ERROR;
   }
   return (*CC->API.GetError)(CC);
}


void GLAPIENTRY glGetFloatv(CTX_ARG GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetFloatv)(CC, pname, params);
}


void GLAPIENTRY glGetIntegerv(CTX_ARG GLenum pname, GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetIntegerv)(CC, pname, params);
}


void GLAPIENTRY glGetLightfv(CTX_ARG GLenum light, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetLightfv)(CC, light, pname, params);
}


void GLAPIENTRY glGetLightiv(CTX_ARG GLenum light, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetLightiv)(CC, light, pname, params);
}


void GLAPIENTRY glGetMapdv(CTX_ARG GLenum target, GLenum query, GLdouble *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetMapdv)( CC, target, query, v );
}


void GLAPIENTRY glGetMapfv(CTX_ARG GLenum target, GLenum query, GLfloat *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetMapfv)( CC, target, query, v );
}


void GLAPIENTRY glGetMapiv(CTX_ARG GLenum target, GLenum query, GLint *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetMapiv)( CC, target, query, v );
}


void GLAPIENTRY glGetMaterialfv(CTX_ARG GLenum face, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetMaterialfv)(CC, face, pname, params);
}


void GLAPIENTRY glGetMaterialiv(CTX_ARG GLenum face, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetMaterialiv)(CC, face, pname, params);
}


void GLAPIENTRY glGetPixelMapfv(CTX_ARG GLenum map, GLfloat *values )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetPixelMapfv)(CC, map, values);
}


void GLAPIENTRY glGetPixelMapuiv(CTX_ARG GLenum map, GLuint *values )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetPixelMapuiv)(CC, map, values);
}


void GLAPIENTRY glGetPixelMapusv(CTX_ARG GLenum map, GLushort *values )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetPixelMapusv)(CC, map, values);
}


void GLAPIENTRY glGetPointerv(CTX_ARG GLenum pname, GLvoid **params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetPointerv)(CC, pname, params);
}


void GLAPIENTRY glGetPolygonStipple(CTX_ARG GLubyte *mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetPolygonStipple)(CC, mask);
}


const GLubyte * GLAPIENTRY glGetString(CTX_ARG GLenum name )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(NULL);
   /* even check for NULL context when not compiled with -DDEBUG */
   if (!CC)
      return NULL;
   return (*CC->API.GetString)(CC, name);
}


void GLAPIENTRY glGetTexEnvfv(CTX_ARG GLenum target, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexEnvfv)(CC, target, pname, params);
}


void GLAPIENTRY glGetTexEnviv(CTX_ARG GLenum target, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexEnviv)(CC, target, pname, params);
}


void GLAPIENTRY glGetTexGeniv(CTX_ARG GLenum coord, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexGeniv)(CC, coord, pname, params);
}


void GLAPIENTRY glGetTexGendv(CTX_ARG GLenum coord, GLenum pname, GLdouble *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexGendv)(CC, coord, pname, params);
}


void GLAPIENTRY glGetTexGenfv(CTX_ARG GLenum coord, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexGenfv)(CC, coord, pname, params);
}


void GLAPIENTRY glGetTexImage(CTX_ARG GLenum target, GLint level, GLenum format,
                               GLenum type, GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexImage)(CC, target, level, format, type, pixels);
}


void GLAPIENTRY glGetTexLevelParameterfv(CTX_ARG GLenum target, GLint level,
                                          GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexLevelParameterfv)(CC, target, level, pname, params);
}


void GLAPIENTRY glGetTexLevelParameteriv(CTX_ARG GLenum target, GLint level,
                                          GLenum pname, GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexLevelParameteriv)(CC, target, level, pname, params);
}


void GLAPIENTRY glGetTexParameterfv(CTX_ARG GLenum target, GLenum pname,
                                     GLfloat *params)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexParameterfv)(CC, target, pname, params);
}


void GLAPIENTRY glGetTexParameteriv(CTX_ARG GLenum target, GLenum pname,
                                     GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetTexParameteriv)(CC, target, pname, params);
}


void GLAPIENTRY glHint(CTX_ARG GLenum target, GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   if (mode!=GL_DONT_CARE && mode!=GL_FASTEST && mode!=GL_NICEST)
      (*CC->API.Error)( CC, GL_INVALID_ENUM, "glHint(mode)" );
   else
      (*CC->API.Hint)(CC, target, mode);
}


#define INDEX( c )                              \
{                                               \
   GLuint count;                                \
   GET_IMMEDIATE;                               \
   count = IM->Count;                           \
   IM->Index[count] = c;                        \
   IM->Flag[count] |= VERT_INDEX;               \
}


void GLAPIENTRY glIndexd(CTX_ARG GLdouble c )
{
   INDEX( (GLuint) (GLint) c );
}


void GLAPIENTRY glIndexf(CTX_ARG GLfloat c )
{
   INDEX( (GLuint) (GLint) c );
}


void GLAPIENTRY glIndexi(CTX_ARG GLint c )
{
   INDEX( (GLuint) c );
}


void GLAPIENTRY glIndexs(CTX_ARG GLshort c )
{
   INDEX( (GLuint) c );
}


/* GL_VERSION_1_1 */
void GLAPIENTRY glIndexub(CTX_ARG GLubyte c )
{
   INDEX( (GLuint) c );
}


void GLAPIENTRY glIndexdv(CTX_ARG const GLdouble *c )
{
   INDEX( (GLuint) (GLint) *c );
}


void GLAPIENTRY glIndexfv(CTX_ARG const GLfloat *c )
{
   INDEX( (GLuint) (GLint) *c );
}


void GLAPIENTRY glIndexiv(CTX_ARG const GLint *c )
{
   INDEX( *c );
}


void GLAPIENTRY glIndexsv(CTX_ARG const GLshort *c )
{
   INDEX( (GLuint) (GLint) *c );
}


/* GL_VERSION_1_1 */
void GLAPIENTRY glIndexubv(CTX_ARG const GLubyte *c )
{
   INDEX( (GLuint) *c );
}


void GLAPIENTRY glIndexMask(CTX_ARG GLuint mask )
{
   GET_CONTEXT;
   (*CC->API.IndexMask)(CC, mask);
}




void GLAPIENTRY glInitNames(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.InitNames)(CC);
}


GLboolean GLAPIENTRY glIsList(CTX_ARG GLuint list )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(GL_FALSE);
   return (*CC->API.IsList)(CC, list);
}


GLboolean GLAPIENTRY glIsTexture(CTX_ARG GLuint texture )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(GL_FALSE);
   return (*CC->API.IsTexture)(CC, texture);
}


void GLAPIENTRY glLightf(CTX_ARG GLenum light, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Lightfv)( CC, light, pname, &param, 1 );
}


void GLAPIENTRY glLighti(CTX_ARG GLenum light, GLenum pname, GLint param )
{
   GLfloat fparam = (GLfloat) param;
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Lightfv)( CC, light, pname, &fparam, 1 );
}


void GLAPIENTRY glLightfv(CTX_ARG GLenum light, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Lightfv)( CC, light, pname, params, 4 );
}


void GLAPIENTRY glLightiv(CTX_ARG GLenum light, GLenum pname, const GLint *params )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   CHECK_CONTEXT;

   switch (pname) {
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_SPECULAR:
         fparam[0] = INT_TO_FLOAT( params[0] );
         fparam[1] = INT_TO_FLOAT( params[1] );
         fparam[2] = INT_TO_FLOAT( params[2] );
         fparam[3] = INT_TO_FLOAT( params[3] );
         break;
      case GL_POSITION:
         fparam[0] = (GLfloat) params[0];
         fparam[1] = (GLfloat) params[1];
         fparam[2] = (GLfloat) params[2];
         fparam[3] = (GLfloat) params[3];
         break;
      case GL_SPOT_DIRECTION:
         fparam[0] = (GLfloat) params[0];
         fparam[1] = (GLfloat) params[1];
         fparam[2] = (GLfloat) params[2];
         break;
      case GL_SPOT_EXPONENT:
      case GL_SPOT_CUTOFF:
      case GL_CONSTANT_ATTENUATION:
      case GL_LINEAR_ATTENUATION:
      case GL_QUADRATIC_ATTENUATION:
         fparam[0] = (GLfloat) params[0];
         break;
      default:
         /* error will be caught later in gl_Lightfv */
         ;
   }
   (*CC->API.Lightfv)( CC, light, pname, fparam, 4 );
}


void GLAPIENTRY glLightModelf(CTX_ARG GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LightModelfv)( CC, pname, &param );
}


void GLAPIENTRY glLightModeli(CTX_ARG GLenum pname, GLint param )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   fparam[0] = (GLfloat) param;
   (*CC->API.LightModelfv)( CC, pname, fparam );
}


void GLAPIENTRY glLightModelfv(CTX_ARG GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LightModelfv)( CC, pname, params );
}


void GLAPIENTRY glLightModeliv(CTX_ARG GLenum pname, const GLint *params )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   CHECK_CONTEXT;

   switch (pname) {
      case GL_LIGHT_MODEL_AMBIENT:
         fparam[0] = INT_TO_FLOAT( params[0] );
         fparam[1] = INT_TO_FLOAT( params[1] );
         fparam[2] = INT_TO_FLOAT( params[2] );
         fparam[3] = INT_TO_FLOAT( params[3] );
         break;
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
      case GL_LIGHT_MODEL_COLOR_CONTROL:
         fparam[0] = (GLfloat) params[0];
         break;
      default:
         /* Error will be caught later in gl_LightModelfv */
         ;
   }
   (*CC->API.LightModelfv)( CC, pname, fparam );
}


void GLAPIENTRY glLineWidth(CTX_ARG GLfloat width )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LineWidth)(CC, width);
}


void GLAPIENTRY glLineStipple(CTX_ARG GLint factor, GLushort pattern )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LineStipple)(CC, factor, pattern);
}


void GLAPIENTRY glListBase(CTX_ARG GLuint base )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ListBase)(CC, base);
}


void GLAPIENTRY glLoadIdentity(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LoadIdentity)( CC );
}


void GLAPIENTRY glLoadMatrixd(CTX_ARG const GLdouble *m )
{
   GLfloat fm[16];
   GLuint i;
   GET_CONTEXT;
   CHECK_CONTEXT;

   for (i=0;i<16;i++) {
          fm[i] = (GLfloat) m[i];
   }

   (*CC->API.LoadMatrixf)( CC, fm );
}


void GLAPIENTRY glLoadMatrixf(CTX_ARG const GLfloat *m )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LoadMatrixf)( CC, m );
}


void GLAPIENTRY glLoadName(CTX_ARG GLuint name )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LoadName)(CC, name);
}


void GLAPIENTRY glLogicOp(CTX_ARG GLenum opcode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.LogicOp)(CC, opcode);
}



void GLAPIENTRY glMap1d(CTX_ARG GLenum target, GLdouble u1, GLdouble u2, GLint stride,
                         GLint order, const GLdouble *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;
   CHECK_CONTEXT;

   pnts = gl_copy_map_points1d( target, stride, order, points );
   retain = CC->CompileFlag;
   (*CC->API.Map1f)( CC, target, u1, u2, stride, order, pnts, retain );
}


void GLAPIENTRY glMap1f(CTX_ARG GLenum target, GLfloat u1, GLfloat u2, GLint stride,
                         GLint order, const GLfloat *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;
   CHECK_CONTEXT;

   pnts = gl_copy_map_points1f( target, stride, order, points );
   retain = CC->CompileFlag;
   (*CC->API.Map1f)( CC, target, u1, u2, stride, order, pnts, retain );
}


void GLAPIENTRY glMap2d(CTX_ARG GLenum target,
                         GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
                         GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
                         const GLdouble *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;
   CHECK_CONTEXT;

   pnts = gl_copy_map_points2d( target, ustride, uorder,
                                vstride, vorder, points );
   retain = CC->CompileFlag;
   (*CC->API.Map2f)( CC, target, u1, u2, ustride, uorder,
                     v1, v2, vstride, vorder, pnts, retain );
}


void GLAPIENTRY glMap2f(CTX_ARG GLenum target,
                         GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
                         GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
                         const GLfloat *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;
   CHECK_CONTEXT;

   pnts = gl_copy_map_points2f( target, ustride, uorder,
                                vstride, vorder, points );
   retain = CC->CompileFlag;
   (*CC->API.Map2f)( CC, target, u1, u2, ustride, uorder,
                     v1, v2, vstride, vorder, pnts, retain );
}


void GLAPIENTRY glMapGrid1d(CTX_ARG GLint un, GLdouble u1, GLdouble u2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.MapGrid1f)( CC, un, (GLfloat) u1, (GLfloat) u2 );
}


void GLAPIENTRY glMapGrid1f(CTX_ARG GLint un, GLfloat u1, GLfloat u2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.MapGrid1f)( CC, un, u1, u2 );
}


void GLAPIENTRY glMapGrid2d(CTX_ARG GLint un, GLdouble u1, GLdouble u2,
                             GLint vn, GLdouble v1, GLdouble v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.MapGrid2f)( CC, un, (GLfloat) u1, (GLfloat) u2,
                         vn, (GLfloat) v1, (GLfloat) v2 );
}


void GLAPIENTRY glMapGrid2f(CTX_ARG GLint un, GLfloat u1, GLfloat u2,
                             GLint vn, GLfloat v1, GLfloat v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.MapGrid2f)( CC, un, u1, u2, vn, v1, v2 );
}


void GLAPIENTRY glMaterialf(CTX_ARG GLenum face, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   gl_Materialfv( CC, face, pname, &param );
}



void GLAPIENTRY glMateriali(CTX_ARG GLenum face, GLenum pname, GLint param )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   fparam[0] = (GLfloat) param;
   gl_Materialfv( CC, face, pname, fparam );
}


void GLAPIENTRY glMaterialfv(CTX_ARG GLenum face, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   gl_Materialfv( CC, face, pname, params );
}


void GLAPIENTRY glMaterialiv(CTX_ARG GLenum face, GLenum pname, const GLint *params )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   switch (pname) {
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_SPECULAR:
      case GL_EMISSION:
      case GL_AMBIENT_AND_DIFFUSE:
         fparam[0] = INT_TO_FLOAT( params[0] );
         fparam[1] = INT_TO_FLOAT( params[1] );
         fparam[2] = INT_TO_FLOAT( params[2] );
         fparam[3] = INT_TO_FLOAT( params[3] );
         break;
      case GL_SHININESS:
         fparam[0] = (GLfloat) params[0];
         break;
      case GL_COLOR_INDEXES:
         fparam[0] = (GLfloat) params[0];
         fparam[1] = (GLfloat) params[1];
         fparam[2] = (GLfloat) params[2];
         break;
      default:
         /* Error will be caught later in gl_Materialfv */
         ;
   }
   gl_Materialfv( CC, face, pname, fparam );
}


void GLAPIENTRY glMatrixMode(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.MatrixMode)( CC, mode );
}


void GLAPIENTRY glMultMatrixd(CTX_ARG const GLdouble *m )
{
   GLfloat fm[16];
   GLuint i;
   GET_CONTEXT;
   CHECK_CONTEXT;

   for (i=0;i<16;i++) {
          fm[i] = (GLfloat) m[i];
   }

   (*CC->API.MultMatrixf)( CC, fm );
}


void GLAPIENTRY glMultMatrixf(CTX_ARG const GLfloat *m )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.MultMatrixf)( CC, m );
}


void GLAPIENTRY glNewList(CTX_ARG GLuint list, GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.NewList)(CC, list, mode);
}


/* KW: Now that we build buffers for display lists the same way we
 *     fill the vb, we can do the work here without a second function
 *     call.  The Flag member allows the identification of missing
 *     (ie shared) normals.
 */
#define NORMAL( x,y,z )                         \
{                                               \
   GLuint count;                                \
   GLfloat *normal;                             \
   GET_IMMEDIATE;                               \
   count = IM->Count;                           \
   IM->Flag[count] |= VERT_NORM;                \
   normal = IM->Normal[count];                  \
   ASSIGN_3V(normal, x,y,z);                    \
}


void GLAPIENTRY glNormal3b(CTX_ARG GLbyte nx, GLbyte ny, GLbyte nz )
{
   NORMAL( BYTE_TO_FLOAT(nx),
           BYTE_TO_FLOAT(ny),
           BYTE_TO_FLOAT(nz) );
}


void GLAPIENTRY glNormal3d(CTX_ARG GLdouble nx, GLdouble ny, GLdouble nz )
{
   NORMAL(nx,ny,nz);
}


void GLAPIENTRY glNormal3f(CTX_ARG GLfloat nx, GLfloat ny, GLfloat nz )
{
   NORMAL(nx,ny,nz);
}


void GLAPIENTRY glNormal3i(CTX_ARG GLint nx, GLint ny, GLint nz )
{
   NORMAL( INT_TO_FLOAT(nx),
           INT_TO_FLOAT(ny),
           INT_TO_FLOAT(nz) );
}


void GLAPIENTRY glNormal3s(CTX_ARG GLshort nx, GLshort ny, GLshort nz )
{
   NORMAL( SHORT_TO_FLOAT(nx),
           SHORT_TO_FLOAT(ny),
           SHORT_TO_FLOAT(nz) );
}


void GLAPIENTRY glNormal3bv(CTX_ARG const GLbyte *v )
{
   NORMAL( BYTE_TO_FLOAT(v[0]),
           BYTE_TO_FLOAT(v[1]),
           BYTE_TO_FLOAT(v[2]) );
}


void GLAPIENTRY glNormal3dv(CTX_ARG const GLdouble *v )
{
   NORMAL( v[0], v[1], v[2] );
}


void GLAPIENTRY glNormal3fv(CTX_ARG const GLfloat *v )
{
   NORMAL( v[0], v[1], v[2] );
}


void GLAPIENTRY glNormal3iv(CTX_ARG const GLint *v )
{
   NORMAL( INT_TO_FLOAT(v[0]),
           INT_TO_FLOAT(v[1]),
           INT_TO_FLOAT(v[2]) );
}


void GLAPIENTRY glNormal3sv(CTX_ARG const GLshort *v )
{
   NORMAL( SHORT_TO_FLOAT(v[0]),
           SHORT_TO_FLOAT(v[1]),
           SHORT_TO_FLOAT(v[2]) );
}

