/* $Id: apiext.c,v 1.2 2000-03-01 18:49:23 jeroen Exp $ */

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
#include "cva.h"
#include "image.h"
#include "varray.h"
#endif

#if !defined(CTX_ARG)
  #define CTX_ARG
  #define CTX_VOID
  #define CTX_PRM
#endif

/*
 * Extension API functions
 */



/*
 * GL_EXT_blend_minmax
 */

void GLAPIENTRY glBlendEquationEXT(CTX_ARG GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.BlendEquation)(CC, mode);
}




/*
 * GL_EXT_blend_color
 */

void GLAPIENTRY glBlendColorEXT(CTX_ARG GLclampf red, GLclampf green,
                               GLclampf blue, GLclampf alpha )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.BlendColor)(CC, red, green, blue, alpha);
}




/*
 * GL_EXT_vertex_array
 */

void GLAPIENTRY glVertexPointerEXT(CTX_ARG GLint size, GLenum type, GLsizei stride,
                                    GLsizei count, const GLvoid *ptr )
{
   glVertexPointer(CTX_PRM size, type, stride, ptr );
   (void) count;
}


void GLAPIENTRY glNormalPointerEXT(CTX_ARG GLenum type, GLsizei stride, GLsizei count,
                                    const GLvoid *ptr )
{
   glNormalPointer(CTX_PRM type, stride, ptr);
   (void) count;
}


void GLAPIENTRY glColorPointerEXT(CTX_ARG GLint size, GLenum type, GLsizei stride,
                                   GLsizei count, const GLvoid *ptr )
{
   glColorPointer(CTX_PRM size, type, stride, ptr);
   (void) count;
}


void GLAPIENTRY glIndexPointerEXT(CTX_ARG GLenum type, GLsizei stride,
                                   GLsizei count, const GLvoid *ptr )
{
   glIndexPointer(CTX_PRM type, stride, ptr);
   (void) count;
}


void GLAPIENTRY glTexCoordPointerEXT(CTX_ARG GLint size, GLenum type, GLsizei stride,
                                      GLsizei count, const GLvoid *ptr )
{
   glTexCoordPointer(CTX_PRM size, type, stride, ptr);
   (void) count;
}


void GLAPIENTRY glEdgeFlagPointerEXT(CTX_ARG GLsizei stride, GLsizei count,
                                      const GLboolean *ptr )
{
   glEdgeFlagPointer(CTX_PRM stride, ptr);
   (void) count;
}


void GLAPIENTRY glGetPointervEXT(CTX_ARG GLenum pname, GLvoid **params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetPointerv)(CC, pname, params);
}


/* glArrayElementEXT now hiding in api1.c.
 */


void GLAPIENTRY glDrawArraysEXT(CTX_ARG GLenum mode, GLint first, GLsizei count )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   gl_DrawArrays(CC, mode, first, count);
}




/*
 * GL_EXT_texture_object
 */

GLboolean GLAPIENTRY glAreTexturesResidentEXT(CTX_ARG GLsizei n,
                                               const GLuint *textures,
                                               GLboolean *residences )
{
   return glAreTexturesResident(CTX_PRM n, textures, residences );
}


void GLAPIENTRY glBindTextureEXT(CTX_ARG GLenum target, GLuint texture )
{
   glBindTexture(CTX_PRM target, texture );
}


void GLAPIENTRY glDeleteTexturesEXT(CTX_ARG GLsizei n, const GLuint *textures)
{
   glDeleteTextures(CTX_PRM n, textures );
}


void GLAPIENTRY glGenTexturesEXT(CTX_ARG GLsizei n, GLuint *textures )
{
   glGenTextures(CTX_PRM n, textures );
}


GLboolean GLAPIENTRY glIsTextureEXT(CTX_ARG GLuint texture )
{
   return glIsTexture(CTX_PRM texture );
}


void GLAPIENTRY glPrioritizeTexturesEXT(CTX_ARG GLsizei n, const GLuint *textures,
                                         const GLclampf *priorities )
{
   glPrioritizeTextures(CTX_PRM n, textures, priorities );
}




/*
 * GL_EXT_texture3D
 */

void GLAPIENTRY glCopyTexSubImage3DEXT(CTX_ARG GLenum target, GLint level,
                                        GLint xoffset, GLint yoffset,
                                        GLint zoffset,
                                        GLint x, GLint y, GLsizei width,
                                        GLsizei height )
{
   glCopyTexSubImage3D(CTX_PRM target, level, xoffset, yoffset, zoffset,
                        x, y, width, height);
}



void GLAPIENTRY glTexImage3DEXT(CTX_ARG GLenum target, GLint level,
                                 GLenum internalformat,
                                 GLsizei width, GLsizei height, GLsizei depth,
                                 GLint border, GLenum format, GLenum type,
                                 const GLvoid *pixels )
{
   glTexImage3D(CTX_PRM target, level, internalformat, width, height, depth,
                 border, format, type, pixels );
}


void GLAPIENTRY glTexSubImage3DEXT(CTX_ARG GLenum target, GLint level, GLint xoffset,
                                    GLint yoffset, GLint zoffset,
                                    GLsizei width, GLsizei height,
                                    GLsizei depth, GLenum format,
                                    GLenum type, const GLvoid *pixels )
{
   glTexSubImage3D(CTX_PRM target, level, xoffset, yoffset, zoffset,
                    width, height, depth, format, type, pixels );
}




/*
 * GL_EXT_point_parameters
 */

void GLAPIENTRY glPointParameterfEXT(CTX_ARG GLenum pname, GLfloat param )
{
   GLfloat params[3];
   GET_CONTEXT;
   CHECK_CONTEXT;
   params[0] = param;
   params[1] = 0.0;
   params[2] = 0.0;
   (*CC->API.PointParameterfvEXT)(CC, pname, params);
}


void GLAPIENTRY glPointParameterfvEXT(CTX_ARG GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PointParameterfvEXT)(CC, pname, params);
}




#ifdef GL_MESA_window_pos
/*
 * Mesa implementation of glWindowPos*MESA()
 */
void GLAPIENTRY glWindowPos4fMESA(CTX_ARG GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.WindowPos4fMESA)( CC, x, y, z, w );
}
#else
/* Implementation in winpos.c is used */
#endif


void GLAPIENTRY glWindowPos2iMESA(CTX_ARG GLint x, GLint y )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos2sMESA(CTX_ARG GLshort x, GLshort y )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos2fMESA(CTX_ARG GLfloat x, GLfloat y )
{
   glWindowPos4fMESA(CTX_PRM x, y, 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos2dMESA(CTX_ARG GLdouble x, GLdouble y )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos2ivMESA(CTX_ARG const GLint *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos2svMESA(CTX_ARG const GLshort *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos2fvMESA(CTX_ARG const GLfloat *p )
{
   glWindowPos4fMESA(CTX_PRM p[0], p[1], 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos2dvMESA(CTX_ARG const GLdouble *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void GLAPIENTRY glWindowPos3iMESA(CTX_ARG GLint x, GLint y, GLint z )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void GLAPIENTRY glWindowPos3sMESA(CTX_ARG GLshort x, GLshort y, GLshort z )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void GLAPIENTRY glWindowPos3fMESA(CTX_ARG GLfloat x, GLfloat y, GLfloat z )
{
   glWindowPos4fMESA(CTX_PRM x, y, z, 1.0F );
}

void GLAPIENTRY glWindowPos3dMESA(CTX_ARG GLdouble x, GLdouble y, GLdouble z )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void GLAPIENTRY glWindowPos3ivMESA(CTX_ARG const GLint *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void GLAPIENTRY glWindowPos3svMESA(CTX_ARG const GLshort *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void GLAPIENTRY glWindowPos3fvMESA(CTX_ARG const GLfloat *p )
{
   glWindowPos4fMESA(CTX_PRM p[0], p[1], p[2], 1.0F );
}

void GLAPIENTRY glWindowPos3dvMESA(CTX_ARG const GLdouble *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void GLAPIENTRY glWindowPos4iMESA(CTX_ARG GLint x, GLint y, GLint z, GLint w )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void GLAPIENTRY glWindowPos4sMESA(CTX_ARG GLshort x, GLshort y, GLshort z, GLshort w )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void GLAPIENTRY glWindowPos4dMESA(CTX_ARG GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}


void GLAPIENTRY glWindowPos4ivMESA(CTX_ARG const GLint *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}

void GLAPIENTRY glWindowPos4svMESA(CTX_ARG const GLshort *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}

void GLAPIENTRY glWindowPos4fvMESA(CTX_ARG const GLfloat *p )
{
   glWindowPos4fMESA(CTX_PRM p[0], p[1], p[2], p[3] );
}

void GLAPIENTRY glWindowPos4dvMESA(CTX_ARG const GLdouble *p )
{
   glWindowPos4fMESA(CTX_PRM (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}




/*
 * GL_MESA_resize_buffers
 */

/*
 * Called by user application when window has been resized.
 */
void GLAPIENTRY glResizeBuffersMESA(CTX_VOID )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ResizeBuffersMESA)( CC );
}



/*
 * GL_SGIS_multitexture (obsolete - will be removed in near future)
 */

#define TEXCOORD1(s)                            \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   count = IM->Count;                           \
   IM->Flag[count] |= IM->TF1[texSet];          \
   tc = IM->TexCoordPtr[texSet][count];         \
   ASSIGN_4V(tc, s,0,0,1);                      \
}


#define TEXCOORD2(s,t)                          \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   count = IM->Count;                           \
   IM->Flag[count] |= IM->TF2[texSet];          \
   tc = IM->TexCoordPtr[texSet][count];         \
   ASSIGN_4V(tc, s,t,0,1);                      \
}

#define TEXCOORD3(s,t,u)                        \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   count = IM->Count;                           \
   IM->Flag[count] |= IM->TF3[texSet];          \
   tc = IM->TexCoordPtr[texSet][count];         \
   ASSIGN_4V(tc, s,t,u,1);                      \
}

#define TEXCOORD4(s,t,u,v)                      \
{                                               \
   GLuint count;                                \
   GLfloat *tc;                                 \
   count = IM->Count;                           \
   IM->Flag[count] |= IM->TF4[texSet];          \
   tc = IM->TexCoordPtr[texSet][count];         \
   ASSIGN_4V(tc, s,t,u,v);                      \
}


/* KW: Do the check here so that we only have to do a single range
 *     test.  The possible compliance problem with this is that
 *     we will throw out error-producing calls when compiling
 *     display lists.  There are ways around this if need be.
 */


/*
 * GL_ARB_multitexture
 */

#define CHECK_ARB                                                       \
   if (target >= GL_TEXTURE0_ARB && target <= GL_TEXTURE1_ARB) {        \
      texSet = target - GL_TEXTURE0_ARB;                                \
   }                                                                    \
   else {                                                               \
      gl_error(IM->backref, GL_INVALID_ENUM, "glMultiTexCoord(target)");        \
      return;                                                           \
   }

void GLAPIENTRY glActiveTextureARB(CTX_ARG GLenum texture)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ActiveTexture)(CC, texture);
}

void GLAPIENTRY glClientActiveTextureARB(CTX_ARG GLenum texture)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ClientActiveTexture)(CC, texture);
}

void GLAPIENTRY glMultiTexCoord1dARB(CTX_ARG GLenum target, GLdouble s)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( s );
}

void GLAPIENTRY glMultiTexCoord1dvARB(CTX_ARG GLenum target, const GLdouble *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( v[0] );
}

void GLAPIENTRY glMultiTexCoord1fARB(CTX_ARG GLenum target, GLfloat s)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( s );
}

void GLAPIENTRY glMultiTexCoord1fvARB(CTX_ARG GLenum target, const GLfloat *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( v[0] );
}

void GLAPIENTRY glMultiTexCoord1iARB(CTX_ARG GLenum target, GLint s)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( s );
}

void GLAPIENTRY glMultiTexCoord1ivARB(CTX_ARG GLenum target, const GLint *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( v[0] );
}

void GLAPIENTRY glMultiTexCoord1sARB(CTX_ARG GLenum target, GLshort s)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( s );
}

void GLAPIENTRY glMultiTexCoord1svARB(CTX_ARG GLenum target, const GLshort *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD1( v[0] );
}

void GLAPIENTRY glMultiTexCoord2dARB(CTX_ARG GLenum target, GLdouble s, GLdouble t)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( s, t );
}

void GLAPIENTRY glMultiTexCoord2dvARB(CTX_ARG GLenum target, const GLdouble *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( v[0], v[1] );
}

void GLAPIENTRY glMultiTexCoord2fARB(CTX_ARG GLenum target, GLfloat s, GLfloat t)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( s, t );
}

void GLAPIENTRY glMultiTexCoord2fvARB(CTX_ARG GLenum target, const GLfloat *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( v[0], v[1] );
}

void GLAPIENTRY glMultiTexCoord2iARB(CTX_ARG GLenum target, GLint s, GLint t)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( s, t );
}

void GLAPIENTRY glMultiTexCoord2ivARB(CTX_ARG GLenum target, const GLint *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( v[0], v[1] );
}

void GLAPIENTRY glMultiTexCoord2sARB(CTX_ARG GLenum target, GLshort s, GLshort t)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( s, t );
}

void GLAPIENTRY glMultiTexCoord2svARB(CTX_ARG GLenum target, const GLshort *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD2( v[0], v[1] );
}

void GLAPIENTRY glMultiTexCoord3dARB(CTX_ARG GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( s, t, r );
}

void GLAPIENTRY glMultiTexCoord3dvARB(CTX_ARG GLenum target, const GLdouble *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( v[0], v[1], v[2] );
}

void GLAPIENTRY glMultiTexCoord3fARB(CTX_ARG GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( s, t, r );
}

void GLAPIENTRY glMultiTexCoord3fvARB(CTX_ARG GLenum target, const GLfloat *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( v[0], v[1], v[2] );
}

void GLAPIENTRY glMultiTexCoord3iARB(CTX_ARG GLenum target, GLint s, GLint t, GLint r)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( s, t, r );
}

void GLAPIENTRY glMultiTexCoord3ivARB(CTX_ARG GLenum target, const GLint *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( v[0], v[1], v[2] );
}

void GLAPIENTRY glMultiTexCoord3sARB(CTX_ARG GLenum target, GLshort s, GLshort t, GLshort r)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( s, t, r );
}

void GLAPIENTRY glMultiTexCoord3svARB(CTX_ARG GLenum target, const GLshort *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD3( v[0], v[1], v[2] );
}

void GLAPIENTRY glMultiTexCoord4dARB(CTX_ARG GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( s, t, r, q );
}

void GLAPIENTRY glMultiTexCoord4dvARB(CTX_ARG GLenum target, const GLdouble *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( v[0], v[1], v[2], v[3] );
}

void GLAPIENTRY glMultiTexCoord4fARB(CTX_ARG GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( s, t, r, q );
}


void GLAPIENTRY glMultiTexCoord4fvARB(CTX_ARG GLenum target, const GLfloat *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( v[0], v[1], v[2], v[3] );
}

void GLAPIENTRY glMultiTexCoord4iARB(CTX_ARG GLenum target, GLint s, GLint t, GLint r, GLint q)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( s, t, r, q );
}

void GLAPIENTRY glMultiTexCoord4ivARB(CTX_ARG GLenum target, const GLint *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( v[0], v[1], v[2], v[3] );
}

void GLAPIENTRY glMultiTexCoord4sARB(CTX_ARG GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( s, t, r, q );
}

void GLAPIENTRY glMultiTexCoord4svARB(CTX_ARG GLenum target, const GLshort *v)
{
   GLint texSet;
   GET_IMMEDIATE;
   CHECK_ARB
   TEXCOORD4( v[0], v[1], v[2], v[3] );
}



/*
 * GL_INGR_blend_func_separate
 */

void GLAPIENTRY glBlendFuncSeparateINGR(CTX_ARG GLenum sfactorRGB,
                                         GLenum dfactorRGB,
                                         GLenum sfactorAlpha,
                                         GLenum dfactorAlpha )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.BlendFuncSeparate)( CC, sfactorRGB, dfactorRGB,
                                 sfactorAlpha, dfactorAlpha );
}



/*
 * GL_PGI_misc_hints - I think this is a bit of a dead extension, but
 * it fits well with what I want the config. file to do.
 */
/*** XXX there is no such entry point in gl.h
void GLAPIENTRY glHintPGI(CTX_ARG GLenum target, GLint mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Hint)( CC, target, mode );
}
***/


/*
 * GL_EXT_compiled_vertex_array
 */

/* functions are in cva.c */


/*
 * GL_EXT_color_table
 */

void GLAPIENTRY glColorTableEXT(CTX_ARG GLenum target, GLenum internalFormat,
                                 GLsizei width, GLenum format, GLenum type,
                                 const GLvoid *table )
{
   struct gl_image *image;
   GET_CONTEXT;
   CHECK_CONTEXT;
   image = gl_unpack_image( CC, width, 1, format, type, table, &CC->Unpack );
   (*CC->API.ColorTable)( CC, target, internalFormat, image );
   if (image->RefCount == 0)
      gl_free_image(image);
}


void GLAPIENTRY glColorSubTableEXT(CTX_ARG GLenum target, GLsizei start,
                                    GLsizei count, GLenum format, GLenum type,
                                    const GLvoid *data )
{
   struct gl_image *image;
   GET_CONTEXT;
   CHECK_CONTEXT;
   image = gl_unpack_image( CC, count, 1, format, type, data, &CC->Unpack );
   (*CC->API.ColorSubTable)( CC, target, start, image );
   if (image->RefCount == 0)
      gl_free_image(image);
}


void GLAPIENTRY glGetColorTableEXT(CTX_ARG GLenum target, GLenum format,
                                    GLenum type, GLvoid *table )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetColorTable)(CC, target, format, type, table);
}


void GLAPIENTRY glGetColorTableParameterivEXT(CTX_ARG GLenum target, GLenum pname,
                                               GLint *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetColorTableParameteriv)(CC, target, pname, params);
}


void GLAPIENTRY glGetColorTableParameterfvEXT(CTX_ARG GLenum target, GLenum pname,
                                               GLfloat *params )
{
   GLint iparams;
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetColorTableParameteriv)(CC, target, pname, &iparams);
   *params = (GLfloat) iparams;
}




/*
 * OpenGL 1.2 imaging subset (most not implemented, just stubs)
 */

void GLAPIENTRY
glBlendEquation(CTX_ARG GLenum mode )
{
   glBlendEquationEXT(CTX_PRM mode );
}


void GLAPIENTRY
glBlendColor(CTX_ARG GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
   glBlendColorEXT(CTX_PRM red, green, blue, alpha );
}


void GLAPIENTRY
glHistogram(CTX_ARG GLenum target, GLsizei width, GLenum internalFormat,
             GLboolean sink )
{
   GET_CONTEXT;
   (void) target;
   (void) width;
   (void) internalFormat;
   (void) sink;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glHistogram" );
}


void GLAPIENTRY
glResetHistogram(CTX_ARG GLenum target )
{
   GET_CONTEXT;
   (void) target;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glResetHistogram" );
}


void GLAPIENTRY
glGetHistogram(CTX_ARG GLenum target, GLboolean reset, GLenum format,
                GLenum type, GLvoid *values )
{
   GET_CONTEXT;
   (void) target;
   (void) reset;
   (void) format;
   (void) type;
   (void) values;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetHistogram" );
}


void GLAPIENTRY
glGetHistogramParameterfv(CTX_ARG GLenum target, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetHistogramParameterfv" );
}


void GLAPIENTRY
glGetHistogramParameteriv(CTX_ARG GLenum target, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetHistogramParameteriv" );
}


void GLAPIENTRY
glMinmax(CTX_ARG GLenum target, GLenum internalFormat, GLboolean sink )
{
   GET_CONTEXT;
   (void) target;
   (void) internalFormat;
   (void) sink;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glMinmax" );
}


void GLAPIENTRY
glResetMinmax(CTX_ARG GLenum target )
{
   GET_CONTEXT;
   (void) target;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glResetMinmax" );
}


void GLAPIENTRY
glGetMinmax(CTX_ARG GLenum target, GLboolean reset, GLenum format, GLenum types,
             GLvoid *values )
{
   GET_CONTEXT;
   (void) target;
   (void) reset;
   (void) format;
   (void) types;
   (void) values;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetMinmax" );
}


void GLAPIENTRY
glGetMinmaxParameterfv(CTX_ARG GLenum target, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetMinmaxParameterfv" );
}


void GLAPIENTRY
glGetMinmaxParameteriv(CTX_ARG GLenum target, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetMinmaxParameteriv" );
}


void GLAPIENTRY
glConvolutionFilter1D(CTX_ARG GLenum target, GLenum internalFormat, GLsizei width,
                       GLenum format, GLenum type, const GLvoid *image )
{
   GET_CONTEXT;
   (void) target;
   (void) internalFormat;
   (void) width;
   (void) format;
   (void) type;
   (void) image;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glConvolutionFilter1D" );
}


void GLAPIENTRY
glConvolutionFilter2D(CTX_ARG GLenum target, GLenum internalFormat, GLsizei width,
                       GLsizei height, GLenum format, GLenum type,
                       const GLvoid *image )
{
   GET_CONTEXT;
   (void) target;
   (void) internalFormat;
   (void) width;
   (void) height;
   (void) format;
   (void) type;
   (void) image;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glConvolutionFilter2D" );
}


void GLAPIENTRY
glConvolutionParameterf(CTX_ARG GLenum target, GLenum pname, GLfloat params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glConvolutionParameterf" );
}


void GLAPIENTRY
glConvolutionParameterfv(CTX_ARG GLenum target, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glConvolutionParameterfv" );
}


void GLAPIENTRY
glConvolutionParameteri(CTX_ARG GLenum target, GLenum pname, GLint params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glConvolutionParameteri" );
}


void GLAPIENTRY
glConvolutionParameteriv(CTX_ARG GLenum target, GLenum pname, const GLint *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glConvolutionParameteriv" );
}


void GLAPIENTRY
glCopyConvolutionFilter1D(CTX_ARG GLenum target, GLenum internalFormat,
                           GLint x, GLint y, GLsizei width )
{
   GET_CONTEXT;
   (void) target;
   (void) internalFormat;
   (void) x;
   (void) y;
   (void) width;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glCopyConvolutionFilter1D" );
}


void GLAPIENTRY
glCopyConvolutionFilter2D(CTX_ARG GLenum target, GLenum internalFormat,
                           GLint x, GLint y, GLsizei width, GLsizei height)
{
   GET_CONTEXT;
   (void) target;
   (void) internalFormat;
   (void) x;
   (void) y;
   (void) width;
   (void) height;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glCopyConvolutionFilter2D" );
}


void GLAPIENTRY
glGetConvolutionFilter(CTX_ARG GLenum target, GLenum format,
                        GLenum type, GLvoid *image )
{
   GET_CONTEXT;
   (void) target;
   (void) format;
   (void) type;
   (void) image;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetConvolutionFilter" );
}


void GLAPIENTRY
glGetConvolutionParameterfv(CTX_ARG GLenum target, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetConvolutionParameterfv" );
}


void GLAPIENTRY
glGetConvolutionParameteriv(CTX_ARG GLenum target, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetConvolutionParameteriv" );
}


void GLAPIENTRY
glSeparableFilter2D(CTX_ARG GLenum target, GLenum internalFormat,
                     GLsizei width, GLsizei height, GLenum format,
                     GLenum type, const GLvoid *row, const GLvoid *column )
{
   GET_CONTEXT;
   (void) target;
   (void) internalFormat;
   (void) width;
   (void) height;
   (void) format;
   (void) row;
   (void) type;
   (void) row;
   (void) column;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glSeperableFilter2D" );
}


void GLAPIENTRY
glGetSeparableFilter(CTX_ARG GLenum target, GLenum format, GLenum type,
                      GLvoid *row, GLvoid *column, GLvoid *span )
{
   GET_CONTEXT;
   (void) target;
   (void) format;
   (void) type;
   (void) row;
   (void) column;
   (void) span;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glGetSeparableFilter" );
}


void GLAPIENTRY
glCopyColorSubTable(CTX_ARG GLenum target, GLsizei start, GLint x, GLint y,
                     GLsizei width )
{
   GET_CONTEXT;
   (void) target;
   (void) start;
   (void) x;
   (void) y;
   (void) width;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glCopyColorSubTable" );
}


void GLAPIENTRY
glCopyColorTable(CTX_ARG GLenum target, GLenum internalFormat,
                  GLint x, GLint y, GLsizei width )
{
   GET_CONTEXT;
   (void) target;
   (void) internalFormat;
   (void) x;
   (void) y;
   (void) width;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glCopyColorTable" );
}


void GLAPIENTRY
glColorTable(CTX_ARG GLenum target, GLenum internalFormat,
             GLsizei width, GLenum format, GLenum type,
             const GLvoid *table )
{
   glColorTableEXT(CTX_PRM target, internalFormat, width, format, type, table);
}


void GLAPIENTRY
glColorSubTable(CTX_ARG GLenum target, GLsizei start,
                GLsizei count, GLenum format, GLenum type,
                const GLvoid *data )
{
   GET_CONTEXT;
   (void) target;
   (void) start;
   (void) count;
   (void) format;
   (void) type;
   (void) data;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glColorSubTable" );
}


void GLAPIENTRY
glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params)
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glColorTableParameteriv" );
}


void GLAPIENTRY
glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
   GET_CONTEXT;
   (void) target;
   (void) pname;
   (void) params;
   CHECK_CONTEXT;
   gl_error( CC, GL_INVALID_OPERATION, "glColorTableParameterfv" );
}


void GLAPIENTRY
glGetColorTable(CTX_ARG GLenum target, GLenum format,
                GLenum type, GLvoid *table )
{
   glGetColorTableEXT(CTX_PRM target, format, type, table);
}


void GLAPIENTRY
glGetColorTableParameteriv(CTX_ARG GLenum target, GLenum pname,
                           GLint *params )
{
   glGetColorTableParameterivEXT(CTX_PRM target, pname, params);
}


void GLAPIENTRY
glGetColorTableParameterfv(CTX_ARG GLenum target, GLenum pname,
                           GLfloat *params )
{
   glGetColorTableParameterfvEXT(CTX_PRM target, pname, params);
}


