/* $Id: texstate.h,v 1.2 2000-05-23 20:34:58 jeroen Exp $ */

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





#ifndef TEXSTATE_H
#define TEXSTATE_H


#include "types.h"


/*** Called from API ***/

extern void
_mesa_GetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );

extern void
_mesa_GetTexEnviv( GLenum target, GLenum pname, GLint *params );

extern void
_mesa_GetTexGendv( GLenum coord, GLenum pname, GLdouble *params );

extern void
_mesa_GetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );

extern void
_mesa_GetTexGeniv( GLenum coord, GLenum pname, GLint *params );

extern void
_mesa_GetTexLevelParameterfv( GLenum target, GLint level,
                              GLenum pname, GLfloat *params );

extern void
_mesa_GetTexLevelParameteriv( GLenum target, GLint level,
                              GLenum pname, GLint *params );

extern void
_mesa_GetTexParameterfv( GLenum target, GLenum pname, GLfloat *params );

extern void
_mesa_GetTexParameteriv( GLenum target, GLenum pname, GLint *params );


extern void
_mesa_TexEnvf( GLenum target, GLenum pname, GLfloat param );

extern void
_mesa_TexEnvfv( GLenum target, GLenum pname, const GLfloat *param );

extern void
_mesa_TexEnvi( GLenum target, GLenum pname, GLint param );

extern void
_mesa_TexEnviv( GLenum target, GLenum pname, const GLint *param );


extern void
_mesa_TexParameterfv( GLenum target, GLenum pname, const GLfloat *params );

extern void
_mesa_TexParameterf( GLenum target, GLenum pname, GLfloat param );


extern void
_mesa_TexParameteri( GLenum target, GLenum pname, const GLint param );

extern void
_mesa_TexParameteriv( GLenum target, GLenum pname, const GLint *params );


extern void
_mesa_TexGend( GLenum coord, GLenum pname, GLdouble param );

extern void
_mesa_TexGendv( GLenum coord, GLenum pname, const GLdouble *params );

extern void
_mesa_TexGenf( GLenum coord, GLenum pname, GLfloat param );

extern void
_mesa_TexGenfv( GLenum coord, GLenum pname, const GLfloat *params );

extern void
_mesa_TexGeni( GLenum coord, GLenum pname, GLint param );

extern void
_mesa_TexGeniv( GLenum coord, GLenum pname, const GLint *params );



extern void gl_SelectTextureTransform( GLcontext *ctx, GLenum target );


/*
 * GL_ARB_multitexture
 */
extern void
_mesa_ActiveTextureARB( GLenum target );

extern void
_mesa_ClientActiveTextureARB( GLenum target );



/*** Internal functions ***/

extern void
gl_put_texobj_on_dirty_list( GLcontext *ctx, struct gl_texture_object *t );

#ifdef VMS
#define gl_remove_texobj_from_dirty_list gl_remove_texobj_from_dirty_lis
#endif
extern void
gl_remove_texobj_from_dirty_list( struct gl_shared_state *shared,
                                  struct gl_texture_object *tObj );

extern void
gl_update_dirty_texobjs( GLcontext *ctx );


#endif

