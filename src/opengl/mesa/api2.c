/* $Id: api2.c,v 1.2 2000-03-01 18:49:23 jeroen Exp $ */

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
#include "types.h"
#include "context.h"
#include "varray.h"
#include "image.h"
#include "macros.h"
#include "matrix.h"
#include "teximage.h"
#include "vb.h"
#endif


/*
 * Part 2 of API functions
 */

void GLAPIENTRY glOrtho(CTX_ARG GLdouble left, GLdouble right,
                         GLdouble bottom, GLdouble top,
                         GLdouble nearval, GLdouble farval )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Ortho)(CC, left, right, bottom, top, nearval, farval);
}


void GLAPIENTRY glPassThrough(CTX_ARG GLfloat token )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PassThrough)(CC, token);
}


void GLAPIENTRY glPixelMapfv(CTX_ARG GLenum map, GLint mapsize, const GLfloat *values )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelMapfv)( CC, map, mapsize, values );
}


void GLAPIENTRY glPixelMapuiv(CTX_ARG GLenum map, GLint mapsize, const GLuint *values )
{
   GLfloat fvalues[MAX_PIXEL_MAP_TABLE];
   GLint i;
   GET_CONTEXT;
   CHECK_CONTEXT;

   if (map==GL_PIXEL_MAP_I_TO_I || map==GL_PIXEL_MAP_S_TO_S) {
      for (i=0;i<mapsize;i++) {
         fvalues[i] = (GLfloat) values[i];
      }
   }
   else {
      for (i=0;i<mapsize;i++) {
         fvalues[i] = UINT_TO_FLOAT( values[i] );
      }
   }
   (*CC->API.PixelMapfv)( CC, map, mapsize, fvalues );
}



void GLAPIENTRY glPixelMapusv(CTX_ARG GLenum map, GLint mapsize, const GLushort *values )
{
   GLfloat fvalues[MAX_PIXEL_MAP_TABLE];
   GLint i;
   GET_CONTEXT;
   CHECK_CONTEXT;

   if (map==GL_PIXEL_MAP_I_TO_I || map==GL_PIXEL_MAP_S_TO_S) {
      for (i=0;i<mapsize;i++) {
         fvalues[i] = (GLfloat) values[i];
      }
   }
   else {
      for (i=0;i<mapsize;i++) {
         fvalues[i] = USHORT_TO_FLOAT( values[i] );
      }
   }
   (*CC->API.PixelMapfv)( CC, map, mapsize, fvalues );
}


void GLAPIENTRY glPixelStoref(CTX_ARG GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelStorei)( CC, pname, (GLint) param );
}


void GLAPIENTRY glPixelStorei(CTX_ARG GLenum pname, GLint param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelStorei)( CC, pname, param );
}


void GLAPIENTRY glPixelTransferf(CTX_ARG GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelTransferf)(CC, pname, param);
}


void GLAPIENTRY glPixelTransferi(CTX_ARG GLenum pname, GLint param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelTransferf)(CC, pname, (GLfloat) param);
}


void GLAPIENTRY glPixelZoom(CTX_ARG GLfloat xfactor, GLfloat yfactor )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelZoom)(CC, xfactor, yfactor);
}


void GLAPIENTRY glPointSize(CTX_ARG GLfloat size )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PointSize)(CC, size);
}


void GLAPIENTRY glPolygonMode(CTX_ARG GLenum face, GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PolygonMode)(CC, face, mode);
}


void GLAPIENTRY glPolygonOffset(CTX_ARG GLfloat factor, GLfloat units )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PolygonOffset)( CC, factor, units );
}


/* GL_EXT_polygon_offset */
void GLAPIENTRY glPolygonOffsetEXT(CTX_ARG GLfloat factor, GLfloat bias )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PolygonOffset)( CC, factor, bias * DEPTH_SCALE );
}


void GLAPIENTRY glPolygonStipple(CTX_ARG const GLubyte *pattern )
{
   GLuint unpackedPattern[32];
   GET_CONTEXT;
   CHECK_CONTEXT;
   gl_unpack_polygon_stipple( CC, pattern, unpackedPattern );
   (*CC->API.PolygonStipple)(CC, unpackedPattern);
}


void GLAPIENTRY glPopAttrib(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopAttrib)(CC);
}


void GLAPIENTRY glPopClientAttrib(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopClientAttrib)(CC);
}


void GLAPIENTRY glPopMatrix(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopMatrix)( CC );
}


void GLAPIENTRY glPopName(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopName)(CC);
}


void GLAPIENTRY glPrioritizeTextures(CTX_ARG GLsizei n, const GLuint *textures,
                                      const GLclampf *priorities )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PrioritizeTextures)(CC, n, textures, priorities);
}


void GLAPIENTRY glPushMatrix(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushMatrix)( CC );
}


void GLAPIENTRY glRasterPos2d(CTX_ARG GLdouble x, GLdouble y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos2f(CTX_ARG GLfloat x, GLfloat y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos2i(CTX_ARG GLint x, GLint y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos2s(CTX_ARG GLshort x, GLshort y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos3d(CTX_ARG GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void GLAPIENTRY glRasterPos3f(CTX_ARG GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void GLAPIENTRY glRasterPos3i(CTX_ARG GLint x, GLint y, GLint z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void GLAPIENTRY glRasterPos3s(CTX_ARG GLshort x, GLshort y, GLshort z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void GLAPIENTRY glRasterPos4d(CTX_ARG GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                           (GLfloat) z, (GLfloat) w );
}


void GLAPIENTRY glRasterPos4f(CTX_ARG GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, x, y, z, w );
}


void GLAPIENTRY glRasterPos4i(CTX_ARG GLint x, GLint y, GLint z, GLint w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                           (GLfloat) z, (GLfloat) w );
}


void GLAPIENTRY glRasterPos4s(CTX_ARG GLshort x, GLshort y, GLshort z, GLshort w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                           (GLfloat) z, (GLfloat) w );
}


void GLAPIENTRY glRasterPos2dv(CTX_ARG const GLdouble *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos2fv(CTX_ARG const GLfloat *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos2iv(CTX_ARG const GLint *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos2sv(CTX_ARG const GLshort *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void GLAPIENTRY glRasterPos3dv(CTX_ARG const GLdouble *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                           (GLfloat) v[2], 1.0F );
}


void GLAPIENTRY glRasterPos3fv(CTX_ARG const GLfloat *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void GLAPIENTRY glRasterPos3iv(CTX_ARG const GLint *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                           (GLfloat) v[2], 1.0F );
}


void GLAPIENTRY glRasterPos3sv(CTX_ARG const GLshort *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                           (GLfloat) v[2], 1.0F );
}


void GLAPIENTRY glRasterPos4dv(CTX_ARG const GLdouble *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                           (GLfloat) v[2], (GLfloat) v[3] );
}


void GLAPIENTRY glRasterPos4fv(CTX_ARG const GLfloat *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, v[0], v[1], v[2], v[3] );
}


void GLAPIENTRY glRasterPos4iv(CTX_ARG const GLint *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                           (GLfloat) v[2], (GLfloat) v[3] );
}


void GLAPIENTRY glRasterPos4sv(CTX_ARG const GLshort *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                           (GLfloat) v[2], (GLfloat) v[3] );
}


void GLAPIENTRY glReadBuffer(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ReadBuffer)( CC, mode );
}


void GLAPIENTRY glReadPixels(CTX_ARG GLint x, GLint y, GLsizei width, GLsizei height,
                              GLenum format, GLenum type, GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ReadPixels)( CC, x, y, width, height, format, type, pixels );
}


void GLAPIENTRY glRectd(CTX_ARG GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                     (GLfloat) x2, (GLfloat) y2 );
}


void GLAPIENTRY glRectf(CTX_ARG GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, x1, y1, x2, y2 );
}


void GLAPIENTRY glRecti(CTX_ARG GLint x1, GLint y1, GLint x2, GLint y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                         (GLfloat) x2, (GLfloat) y2 );
}


void GLAPIENTRY glRects(CTX_ARG GLshort x1, GLshort y1, GLshort x2, GLshort y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                     (GLfloat) x2, (GLfloat) y2 );
}


void GLAPIENTRY glRectdv(CTX_ARG const GLdouble *v1, const GLdouble *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],
                    (GLfloat) v2[0], (GLfloat) v2[1]);
}


void GLAPIENTRY glRectfv(CTX_ARG const GLfloat *v1, const GLfloat *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)(CC, v1[0], v1[1], v2[0], v2[1]);
}


void GLAPIENTRY glRectiv(CTX_ARG const GLint *v1, const GLint *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) v1[0], (GLfloat) v1[1],
                     (GLfloat) v2[0], (GLfloat) v2[1] );
}


void GLAPIENTRY glRectsv(CTX_ARG const GLshort *v1, const GLshort *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],
        (GLfloat) v2[0], (GLfloat) v2[1]);
}


void GLAPIENTRY glScissor(CTX_ARG GLint x, GLint y, GLsizei width, GLsizei height)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Scissor)(CC, x, y, width, height);
}


GLboolean GLAPIENTRY glIsEnabled(CTX_ARG GLenum cap )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(GL_FALSE);
   return (*CC->API.IsEnabled)( CC, cap );
}



void GLAPIENTRY glPushAttrib(CTX_ARG GLbitfield mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushAttrib)(CC, mask);
}


void GLAPIENTRY glPushClientAttrib(CTX_ARG GLbitfield mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushClientAttrib)(CC, mask);
}


void GLAPIENTRY glPushName(CTX_ARG GLuint name )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushName)(CC, name);
}


GLint GLAPIENTRY glRenderMode(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(0);
   return (*CC->API.RenderMode)(CC, mode);
}


void GLAPIENTRY glRotated(CTX_ARG GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rotatef)( CC, (GLfloat) angle,
                       (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void GLAPIENTRY glRotatef(CTX_ARG GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rotatef)( CC, angle, x, y, z );
}


void GLAPIENTRY glSelectBuffer(CTX_ARG GLsizei size, GLuint *buffer )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.SelectBuffer)(CC, size, buffer);
}


void GLAPIENTRY glScaled(CTX_ARG GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Scalef)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void GLAPIENTRY glScalef(CTX_ARG GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Scalef)( CC, x, y, z );
}


void GLAPIENTRY glShadeModel(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ShadeModel)(CC, mode);
}


void GLAPIENTRY glStencilFunc(CTX_ARG GLenum func, GLint ref, GLuint mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.StencilFunc)(CC, func, ref, mask);
}


void GLAPIENTRY glStencilMask(CTX_ARG GLuint mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.StencilMask)(CC, mask);
}


void GLAPIENTRY glStencilOp(CTX_ARG GLenum fail, GLenum zfail, GLenum zpass )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.StencilOp)(CC, fail, zfail, zpass);
}

#define TEXCOORD1(s)                            \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   GET_IMMEDIATE;                               \
   count = IM->Count;                           \
   IM->Flag[count] |= VERT_TEX0_1;              \
   tc = IM->TexCoord[0][count];                 \
   ASSIGN_4V(tc,s,0,0,1);                       \
}

#define TEXCOORD2(s,t)                          \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   GET_IMMEDIATE;                               \
   count = IM->Count;                           \
   IM->Flag[count] |= VERT_TEX0_12;             \
   tc = IM->TexCoord[0][count];                 \
   ASSIGN_4V(tc, s,t,0,1);                      \
}

#define TEXCOORD3(s,t,u)                        \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   GET_IMMEDIATE;                               \
   count = IM->Count;                           \
   IM->Flag[count] |= VERT_TEX0_123;            \
   tc = IM->TexCoord[0][count];                 \
   ASSIGN_4V(tc, s,t,u,1);                      \
}

#define TEXCOORD4(s,t,u,v)                      \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   GET_IMMEDIATE;                               \
   count = IM->Count;                           \
   IM->Flag[count] |= VERT_TEX0_1234;           \
   tc = IM->TexCoord[0][count];                 \
   ASSIGN_4V(tc, s,t,u,v);                      \
}


void GLAPIENTRY glTexCoord1d(CTX_ARG GLdouble s )
{
   TEXCOORD1(s);
}


void GLAPIENTRY glTexCoord1f(CTX_ARG GLfloat s )
{
   TEXCOORD1(s);
}


void GLAPIENTRY glTexCoord1i(CTX_ARG GLint s )
{
   TEXCOORD1(s);
}


void GLAPIENTRY glTexCoord1s(CTX_ARG GLshort s )
{
   TEXCOORD1(s);
}


void GLAPIENTRY glTexCoord2d(CTX_ARG GLdouble s, GLdouble t )
{
   TEXCOORD2(s,t);
}

void GLAPIENTRY glTexCoord2f(CTX_ARG GLfloat s, GLfloat t )
{
   TEXCOORD2(*(&s),*&t);
}


void GLAPIENTRY glTexCoord2s(CTX_ARG GLshort s, GLshort t )
{
   TEXCOORD2(s,t);
}

void GLAPIENTRY glTexCoord2i(CTX_ARG GLint s, GLint t )
{
   TEXCOORD2(s,t);
}


void GLAPIENTRY glTexCoord3d(CTX_ARG GLdouble s, GLdouble t, GLdouble r )
{
   TEXCOORD3(s,t,r);
}


void GLAPIENTRY glTexCoord3f(CTX_ARG GLfloat s, GLfloat t, GLfloat r )
{
   TEXCOORD3(s,t,r);
}


void GLAPIENTRY glTexCoord3i(CTX_ARG GLint s, GLint t, GLint r )
{
   TEXCOORD3(s,t,r);
}


void GLAPIENTRY glTexCoord3s(CTX_ARG GLshort s, GLshort t, GLshort r )
{
   TEXCOORD3(s,t,r);
}


void GLAPIENTRY glTexCoord4d(CTX_ARG GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
   TEXCOORD4(s,t,r,q)
}


void GLAPIENTRY glTexCoord4f(CTX_ARG GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
   TEXCOORD4(s,t,r,q)
}


void GLAPIENTRY glTexCoord4i(CTX_ARG GLint s, GLint t, GLint r, GLint q )
{
   TEXCOORD4(s,t,r,q)
}


void GLAPIENTRY glTexCoord4s(CTX_ARG GLshort s, GLshort t, GLshort r, GLshort q )
{
   TEXCOORD4(s,t,r,q)
}


void GLAPIENTRY glTexCoord1dv(CTX_ARG const GLdouble *v )
{
   TEXCOORD1(v[0]);
}


void GLAPIENTRY glTexCoord1fv(CTX_ARG const GLfloat *v )
{
   TEXCOORD1(v[0]);
}


void GLAPIENTRY glTexCoord1iv(CTX_ARG const GLint *v )
{
   TEXCOORD1(v[0]);
}


void GLAPIENTRY glTexCoord1sv(CTX_ARG const GLshort *v )
{
   TEXCOORD1(v[0]);
}


void GLAPIENTRY glTexCoord2dv(CTX_ARG const GLdouble *v )
{
   TEXCOORD2(v[0],v[1]);
}


void GLAPIENTRY glTexCoord2fv(CTX_ARG const GLfloat *v )
{
   TEXCOORD2(v[0],v[1]);
}


void GLAPIENTRY glTexCoord2iv(CTX_ARG const GLint *v )
{
   TEXCOORD2(v[0],v[1]);
}


void GLAPIENTRY glTexCoord2sv(CTX_ARG const GLshort *v )
{
   TEXCOORD2(v[0],v[1]);
}


void GLAPIENTRY glTexCoord3dv(CTX_ARG const GLdouble *v )
{
   TEXCOORD2(v[0],v[1]);
}


void GLAPIENTRY glTexCoord3fv(CTX_ARG const GLfloat *v )
{
   TEXCOORD3(v[0],v[1],v[2]);
}


void GLAPIENTRY glTexCoord3iv(CTX_ARG const GLint *v )
{
   TEXCOORD3(v[0],v[1],v[2]);
}


void GLAPIENTRY glTexCoord3sv(CTX_ARG const GLshort *v )
{
   TEXCOORD3(v[0],v[1],v[2]);
}


void GLAPIENTRY glTexCoord4dv(CTX_ARG const GLdouble *v )
{
   TEXCOORD4(v[0],v[1],v[2],v[3]);
}


void GLAPIENTRY glTexCoord4fv(CTX_ARG const GLfloat *v )
{

   TEXCOORD4(v[0],v[1],v[2],v[3]);
}


void GLAPIENTRY glTexCoord4iv(CTX_ARG const GLint *v )
{
   TEXCOORD4(v[0],v[1],v[2],v[3]);
}


void GLAPIENTRY glTexCoord4sv(CTX_ARG const GLshort *v )
{
   TEXCOORD4(v[0],v[1],v[2],v[3]);
}



void GLAPIENTRY glTexGend(CTX_ARG GLenum coord, GLenum pname, GLdouble param )
{
   GLfloat p = (GLfloat) param;
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &p );
}


void GLAPIENTRY glTexGenf(CTX_ARG GLenum coord, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &param );
}


void GLAPIENTRY glTexGeni(CTX_ARG GLenum coord, GLenum pname, GLint param )
{
   GLfloat p = (GLfloat) param;
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &p );
}


void GLAPIENTRY glTexGendv(CTX_ARG GLenum coord, GLenum pname, const GLdouble *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   p[0] = params[0];
   p[1] = params[1];
   p[2] = params[2];
   p[3] = params[3];
   (*CC->API.TexGenfv)( CC, coord, pname, p );
}


void GLAPIENTRY glTexGeniv(CTX_ARG GLenum coord, GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   p[0] = params[0];
   p[1] = params[1];
   p[2] = params[2];
   p[3] = params[3];
   (*CC->API.TexGenfv)( CC, coord, pname, p );
}


void GLAPIENTRY glTexGenfv(CTX_ARG GLenum coord, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, params );
}




void GLAPIENTRY glTexEnvf(CTX_ARG GLenum target, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, &param );
}



void GLAPIENTRY glTexEnvi(CTX_ARG GLenum target, GLenum pname, GLint param )
{
   GLfloat p[4];
   GET_CONTEXT;
   p[0] = (GLfloat) param;
   p[1] = p[2] = p[3] = 0.0;
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, p );
}



void GLAPIENTRY glTexEnvfv(CTX_ARG GLenum target, GLenum pname, const GLfloat *param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, param );
}



void GLAPIENTRY glTexEnviv(CTX_ARG GLenum target, GLenum pname, const GLint *param )
{
   GLfloat p[4];
   GET_CONTEXT;
   p[0] = INT_TO_FLOAT( param[0] );
   p[1] = INT_TO_FLOAT( param[1] );
   p[2] = INT_TO_FLOAT( param[2] );
   p[3] = INT_TO_FLOAT( param[3] );
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, p );
}


void GLAPIENTRY glTexImage1D(CTX_ARG GLenum target, GLint level,
                             GLint internalformat,
                             GLsizei width, GLint border,
                             GLenum format, GLenum type, const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexImage1D)( CC, target, level, internalformat,
                          width, border, format, type, pixels );
}


void GLAPIENTRY glTexImage2D(CTX_ARG GLenum target, GLint level,
                             GLint internalformat,
                             GLsizei width, GLsizei height, GLint border,
                             GLenum format, GLenum type, const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexImage2D)( CC, target, level, internalformat,
                          width, height, border, format, type, pixels );
}


void GLAPIENTRY glTexImage3D(CTX_ARG GLenum target, GLint level,
                             GLint internalformat,
                             GLsizei width, GLsizei height, GLsizei depth,
                             GLint border, GLenum format, GLenum type,
                             const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexImage3D)( CC, target, level, internalformat,
                          width, height, depth, border, format, type,
                          pixels );
}


void GLAPIENTRY glTexParameterf(CTX_ARG GLenum target, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, &param );
}


void GLAPIENTRY glTexParameteri(CTX_ARG GLenum target, GLenum pname, GLint param )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   fparam[0] = (GLfloat) param;
   fparam[1] = fparam[2] = fparam[3] = 0.0;
   CHECK_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, fparam );
}


void GLAPIENTRY glTexParameterfv(CTX_ARG GLenum target, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, params );
}


void GLAPIENTRY glTexParameteriv(CTX_ARG GLenum target, GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   if (pname==GL_TEXTURE_BORDER_COLOR) {
      p[0] = INT_TO_FLOAT( params[0] );
      p[1] = INT_TO_FLOAT( params[1] );
      p[2] = INT_TO_FLOAT( params[2] );
      p[3] = INT_TO_FLOAT( params[3] );
   }
   else {
      p[0] = (GLfloat) params[0];
      p[1] = (GLfloat) params[1];
      p[2] = (GLfloat) params[2];
      p[3] = (GLfloat) params[3];
   }
   (*CC->API.TexParameterfv)( CC, target, pname, p );
}


void GLAPIENTRY glTexSubImage1D(CTX_ARG GLenum target, GLint level,
                                GLint xoffset, GLsizei width, GLenum format,
                                GLenum type, const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexSubImage1D)( CC, target, level, xoffset, width,
                             format, type, pixels );
}


void GLAPIENTRY glTexSubImage2D(CTX_ARG GLenum target, GLint level,
                                 GLint xoffset, GLint yoffset,
                                 GLsizei width, GLsizei height,
                                 GLenum format, GLenum type,
                                 const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexSubImage2D)( CC, target, level, xoffset, yoffset,
                             width, height, format, type, pixels );
}


void GLAPIENTRY glTexSubImage3D(CTX_ARG GLenum target, GLint level,
                                GLint xoffset,
                                GLint yoffset, GLint zoffset, GLsizei width,
                                GLsizei height, GLsizei depth, GLenum format,
                                GLenum type, const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexSubImage3D)( CC, target, level, xoffset, yoffset, zoffset,
                             width, height, depth, format, type, pixels );
}


void GLAPIENTRY glTranslated(CTX_ARG GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Translatef)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void GLAPIENTRY glTranslatef(CTX_ARG GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Translatef)( CC, x, y, z );
}

/* KW: Run into bad problems in reset_vb/fixup_input if we don't fully pad
 *     the incoming vertices.
 */
#define VERTEX2(IM, x,y)                        \
{                                               \
   GLuint count = IM->Count++;                  \
   GLfloat *dest = IM->Obj[count];              \
   IM->Flag[count] |= VERT_OBJ_2;               \
   ASSIGN_4V(dest, x, y, 0, 1);                 \
   if (dest == IM->Obj[VB_MAX-1])               \
      IM->maybe_transform_vb( IM );             \
}

#define VERTEX3(IM,x,y,z)                       \
{                                               \
   GLuint count = IM->Count++;                  \
   GLfloat *dest = IM->Obj[count];              \
   IM->Flag[count] |= VERT_OBJ_23;              \
   ASSIGN_4V(dest, x, y, z, 1);                 \
   if (dest == IM->Obj[VB_MAX-1])               \
      IM->maybe_transform_vb( IM );             \
}

#define VERTEX4(IM, x,y,z,w)                    \
{                                               \
   GLuint count = IM->Count++;                  \
   GLfloat *dest = IM->Obj[count];              \
   IM->Flag[count] |= VERT_OBJ_234;             \
   ASSIGN_4V(dest, x, y, z, w);                 \
   if (dest == IM->Obj[VB_MAX-1])               \
      IM->maybe_transform_vb( IM );             \
}


void GLAPIENTRY glVertex2d(CTX_ARG GLdouble x, GLdouble y )
{
   GET_IMMEDIATE;
   VERTEX2( IM, (GLfloat) x, (GLfloat) y );
}


void GLAPIENTRY glVertex2f(CTX_ARG GLfloat x, GLfloat y )
{
   GET_IMMEDIATE;
   VERTEX2( IM, *(&x), *(&y) );
}

/* Internal use:
 */
void gl_Vertex2f( GLcontext *ctx, GLfloat x, GLfloat y )
{
   struct immediate *im = ctx->input;
   VERTEX2( im, x, y );
}

void GLAPIENTRY glVertex2i(CTX_ARG GLint x, GLint y )
{
   GET_IMMEDIATE;
   VERTEX2( IM, (GLfloat) x, (GLfloat) y );
}

void GLAPIENTRY glVertex2s(CTX_ARG GLshort x, GLshort y )
{
   GET_IMMEDIATE;
   VERTEX2( IM, (GLfloat) x, (GLfloat) y );
}

void GLAPIENTRY glVertex3d(CTX_ARG GLdouble x, GLdouble y, GLdouble z )
{
   GET_IMMEDIATE;
   VERTEX3( IM, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void GLAPIENTRY glVertex3f(CTX_ARG GLfloat x, GLfloat y, GLfloat z )
{
   GET_IMMEDIATE;
   VERTEX3( IM, *(&x), *(&y), *(&z) );
}

void GLAPIENTRY glVertex3i(CTX_ARG GLint x, GLint y, GLint z )
{
   GET_IMMEDIATE;
   VERTEX3( IM, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void GLAPIENTRY glVertex3s(CTX_ARG GLshort x, GLshort y, GLshort z )
{
   GET_IMMEDIATE;
   VERTEX3( IM, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void GLAPIENTRY glVertex4d(CTX_ARG GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   GET_IMMEDIATE;
   VERTEX4( IM, (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}


void GLAPIENTRY glVertex4f(CTX_ARG GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_IMMEDIATE;
   VERTEX4( IM, *(&x), *(&y), *(&z), *(&w) );
}


void GLAPIENTRY glVertex4i(CTX_ARG GLint x, GLint y, GLint z, GLint w )
{
   GET_IMMEDIATE;
   VERTEX4( IM, (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}


void GLAPIENTRY glVertex4s(CTX_ARG GLshort x, GLshort y, GLshort z, GLshort w )
{
   GET_IMMEDIATE;
   VERTEX4( IM, (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}


void GLAPIENTRY glVertex2dv(CTX_ARG const GLdouble *v )
{
   GET_IMMEDIATE;
   VERTEX2( IM, (GLfloat) v[0], (GLfloat) v[1] );
}


void GLAPIENTRY glVertex2fv(CTX_ARG const GLfloat *v )
{
   GET_IMMEDIATE;
   VERTEX2( IM, v[0], v[1] );
}


void GLAPIENTRY glVertex2iv(CTX_ARG const GLint *v )
{
   GET_IMMEDIATE;
   VERTEX2( IM, (GLfloat) v[0], (GLfloat) v[1] );
}


void GLAPIENTRY glVertex2sv(CTX_ARG const GLshort *v )
{
   GET_IMMEDIATE;
   VERTEX2( IM, (GLfloat) v[0], (GLfloat) v[1] );
}


void GLAPIENTRY glVertex3dv(CTX_ARG const GLdouble *v )
{
   GET_IMMEDIATE;
   VERTEX3( IM, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );
}


void GLAPIENTRY glVertex3fv(CTX_ARG const GLfloat *v )
{
   GET_IMMEDIATE;
   VERTEX3( IM, v[0], v[1], v[2] );
}

void GLAPIENTRY glVertex3iv(CTX_ARG const GLint *v )
{
   GET_IMMEDIATE;
   VERTEX3( IM, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );
}


void GLAPIENTRY glVertex3sv(CTX_ARG const GLshort *v )
{
   GET_IMMEDIATE;
   VERTEX3( IM, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );
}


void GLAPIENTRY glVertex4dv(CTX_ARG const GLdouble *v )
{
   GET_IMMEDIATE;
   VERTEX4( IM,
            (GLfloat) v[0], (GLfloat) v[1],
            (GLfloat) v[2], (GLfloat) v[3] );
}


void GLAPIENTRY glVertex4fv(CTX_ARG const GLfloat *v )
{
   GET_IMMEDIATE;
   VERTEX4( IM, v[0], v[1], v[2], v[3] );
}


void GLAPIENTRY glVertex4iv(CTX_ARG const GLint *v )
{
   GET_IMMEDIATE;
   VERTEX4( IM,
            (GLfloat) v[0], (GLfloat) v[1],
            (GLfloat) v[2], (GLfloat) v[3] );
}


void GLAPIENTRY glVertex4sv(CTX_ARG const GLshort *v )
{
   GET_IMMEDIATE;
   VERTEX4( IM,
            (GLfloat) v[0], (GLfloat) v[1],
            (GLfloat) v[2], (GLfloat) v[3] );
}



void GLAPIENTRY glViewport(CTX_ARG GLint x, GLint y, GLsizei width, GLsizei height )
{
   GET_CONTEXT;
   (*CC->API.Viewport)( CC, x, y, width, height );
}

