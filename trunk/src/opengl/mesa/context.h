/* $Id: context.h,v 1.2 2000-03-01 18:49:25 jeroen Exp $ */

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





#ifndef CONTEXT_H
#define CONTEXT_H


                                       /* #include "types.h"               */

#ifdef THREADS
   /*
    * A seperate GLcontext for each thread
    */
   extern GLcontext *gl_get_thread_context( void );
#else
   /*
    * All threads use same pointer to current context.
    */
   extern GLcontext *CC;
   extern struct immediate *CURRENT_INPUT;
#endif



/*
 * There are three Mesa datatypes which are meant to be used by device
 * drivers:
 *   GLcontext:  this contains the Mesa rendering state
 *   GLvisual:  this describes the color buffer (rgb vs. ci), whether
 *              or not there's a depth buffer, stencil buffer, etc.
 *   GLframebuffer:  contains pointers to the depth buffer, stencil
 *                   buffer, accum buffer and alpha buffers.
 *
 * These types should be encapsulated by corresponding device driver
 * datatypes.  See xmesa.h and xmesaP.h for an example.
 *
 * In OOP terms, GLcontext, GLvisual, and GLframebuffer are base classes
 * which the device driver must derive from.
 *
 * The following functions create and destroy these datatypes.
 */


/*
 * Create/destroy a GLvisual.  A GLvisual is like a GLX visual.  It describes
 * the colorbuffer, depth buffer, stencil buffer and accum buffer which will
 * be used by the GL context and framebuffer.
 */
extern GLvisual *gl_create_visual( GLboolean rgbFlag,
                                   GLboolean alphaFlag,
                                   GLboolean dbFlag,
                                   GLboolean stereoFlag,
                                   GLint depthBits,
                                   GLint stencilBits,
                                   GLint accumBits,
                                   GLint indexBits,
                                   GLint redBits,
                                   GLint greenBits,
                                   GLint blueBits,
                                   GLint alphaBits );

extern void gl_destroy_visual( GLvisual *vis );


/*
 * Create/destroy a GLcontext.  A GLcontext is like a GLX context.  It
 * contains the rendering state.
 */
extern GLcontext *gl_create_context( GLvisual *visual,
                                     GLcontext *share_list,
                                     void *driver_ctx,
                                     GLboolean direct);

extern void gl_destroy_context( GLcontext *ctx );

/* Called by the driver after both the context and driver are fully
 * initialized.  Currently just reads the config file.
 */
extern void gl_context_initialize( GLcontext *ctx );

/*
 * Create/destroy a GLframebuffer.  A GLframebuffer is like a GLX drawable.
 * It bundles up the depth buffer, stencil buffer and accum buffers into a
 * single entity.
 */
extern GLframebuffer *gl_create_framebuffer( GLvisual *visual );

extern void gl_destroy_framebuffer( GLframebuffer *buffer );



extern void gl_make_current( GLcontext *ctx, GLframebuffer *buffer );

extern GLcontext *gl_get_current_context(void);

extern void gl_copy_context(const GLcontext *src, GLcontext *dst, GLuint mask);

extern void gl_set_api_table( GLcontext *ctx, const struct gl_api_table *api );



/*
 * GL_MESA_resize_buffers extension
 */
extern void gl_ResizeBuffersMESA( GLcontext *ctx );



/*
 * Miscellaneous
 */

extern void gl_problem( const GLcontext *ctx, const char *s );

extern void gl_warning( const GLcontext *ctx, const char *s );

extern void gl_error( GLcontext *ctx, GLenum error, const char *s );
extern void gl_compile_error( GLcontext *ctx, GLenum error, const char *s );

extern GLenum gl_GetError( GLcontext *ctx );


extern void gl_update_state( GLcontext *ctx );


/* for debugging */
extern void gl_print_state( const char *msg, GLuint state );

/* for debugging */
extern void gl_print_enable_flags( const char *msg, GLuint flags );


#ifdef PROFILE
extern GLdouble gl_time( void );
#endif


#endif
