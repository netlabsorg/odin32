/* $Id: stencil.c,v 1.1 2000-02-29 00:46:25 sandervl Exp $ */

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


/* $XFree86: xc/lib/GL/mesa/src/stencil.c,v 1.3 1999/04/04 00:20:32 dawes Exp $ */

#ifdef PC_HEADER
#include "all.h"
#else
#ifndef XFree86Server
#include <stdlib.h>
#include <string.h>
#else
#include "GL/xf86glx.h"
#endif
#include "context.h"
#include "macros.h"
#include "pb.h"
#include "stencil.h"
#include "types.h"
#include "enable.h"
#endif



/*
 * Return the address of a stencil buffer value given the window coords:
 */
#define STENCIL_ADDRESS(X,Y)  (ctx->Buffer->Stencil + ctx->Buffer->Width * (Y) + (X))


void gl_ClearStencil( GLcontext *ctx, GLint s )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glClearStencil");
   ctx->Stencil.Clear = (GLstencil) s;

   if (ctx->Driver.ClearStencil) {
      (*ctx->Driver.ClearStencil)( ctx, s );
   }
}



void gl_StencilFunc( GLcontext *ctx, GLenum func, GLint ref, GLuint mask )
{
   GLint maxref;

   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glStencilFunc");

   switch (func) {
      case GL_NEVER:
      case GL_LESS:
      case GL_LEQUAL:
      case GL_GREATER:
      case GL_GEQUAL:
      case GL_EQUAL:
      case GL_NOTEQUAL:
      case GL_ALWAYS:
         ctx->Stencil.Function = func;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glStencilFunc" );
         return;
   }

   maxref = (1 << STENCIL_BITS) - 1;
   ctx->Stencil.Ref = (GLstencil) CLAMP( ref, 0, maxref );
   ctx->Stencil.ValueMask = (GLstencil) mask;

   if (ctx->Driver.StencilFunc) {
      (*ctx->Driver.StencilFunc)( ctx, func, ctx->Stencil.Ref, mask );
   }
}



void gl_StencilMask( GLcontext *ctx, GLuint mask )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glStencilMask");
   ctx->Stencil.WriteMask = (GLstencil) mask;

   if (ctx->Driver.StencilMask) {
      (*ctx->Driver.StencilMask)( ctx, mask );
   }
}



void gl_StencilOp( GLcontext *ctx, GLenum fail, GLenum zfail, GLenum zpass )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glStencilOp");
   switch (fail) {
      case GL_KEEP:
      case GL_ZERO:
      case GL_REPLACE:
      case GL_INCR:
      case GL_DECR:
      case GL_INVERT:
      case GL_INCR_WRAP_EXT:
      case GL_DECR_WRAP_EXT:
         ctx->Stencil.FailFunc = fail;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glStencilOp" );
         return;
   }
   switch (zfail) {
      case GL_KEEP:
      case GL_ZERO:
      case GL_REPLACE:
      case GL_INCR:
      case GL_DECR:
      case GL_INVERT:
      case GL_INCR_WRAP_EXT:
      case GL_DECR_WRAP_EXT:
         ctx->Stencil.ZFailFunc = zfail;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glStencilOp" );
         return;
   }
   switch (zpass) {
      case GL_KEEP:
      case GL_ZERO:
      case GL_REPLACE:
      case GL_INCR:
      case GL_DECR:
      case GL_INVERT:
      case GL_INCR_WRAP_EXT:
      case GL_DECR_WRAP_EXT:
         ctx->Stencil.ZPassFunc = zpass;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glStencilOp" );
         return;
   }

   if (ctx->Driver.StencilOp) {
      (*ctx->Driver.StencilOp)( ctx, fail, zfail, zpass );
   }
}



/* Stencil Logic:

IF stencil test fails THEN
   Apply fail-op to stencil value
   Don't write the pixel (RGBA,Z)
ELSE
   IF doing depth test && depth test fails THEN
      Apply zfail-op to stencil value
      Write RGBA and Z to appropriate buffers
   ELSE
      Apply zpass-op to stencil value
ENDIF

*/




/*
 * Apply the given stencil operator to the array of stencil values.
 * Don't touch stencil[i] if mask[i] is zero.
 * Input:  n - number of pixels in the span
 *         oper - the stencil buffer operator
 *         stencil - array of stencil values
 *         mask - array [n] of flag:  1=apply operator, 0=don't apply operator
 */
static void apply_stencil_op( GLcontext *ctx, GLenum oper,
                              GLuint n, GLstencil stencil[], GLubyte mask[] )
{
   const GLstencil ref = ctx->Stencil.Ref;
   const GLstencil wrtmask = ctx->Stencil.WriteMask;
   const GLstencil invmask = (GLstencil) (~ctx->Stencil.WriteMask);
   GLuint i;

   switch (oper) {
      case GL_KEEP:
         /* do nothing */
         break;
      case GL_ZERO:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  stencil[i] = 0;
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  stencil[i] = (GLstencil) (stencil[i] & invmask);
	       }
	    }
	 }
	 break;
      case GL_REPLACE:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  stencil[i] = ref;
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  GLstencil s = stencil[i];
		  stencil[i] = (GLstencil) ((invmask & s ) | (wrtmask & ref));
	       }
	    }
	 }
	 break;
      case GL_INCR:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  GLstencil s = stencil[i];
		  if (s < STENCIL_MAX) {
		     stencil[i] = (GLstencil) (s+1);
		  }
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  /* VERIFY logic of adding 1 to a write-masked value */
		  GLstencil s = stencil[i];
		  if (s < STENCIL_MAX) {
		     stencil[i] = (GLstencil) ((invmask & s) | (wrtmask & (s+1)));
		  }
	       }
	    }
	 }
	 break;
      case GL_DECR:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  GLstencil s = stencil[i];
		  if (s>0) {
		     stencil[i] = (GLstencil) (s-1);
		  }
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  /* VERIFY logic of subtracting 1 to a write-masked value */
		  GLstencil s = stencil[i];
		  if (s>0) {
		     stencil[i] = (GLstencil) ((invmask & s) | (wrtmask & (s-1)));
		  }
	       }
	    }
	 }
	 break;
      case GL_INCR_WRAP_EXT:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  stencil[i]++;
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil s = stencil[i];
                  stencil[i] = (GLstencil) ((invmask & s) | (wrtmask & (stencil[i]+1)));
	       }
	    }
	 }
	 break;
      case GL_DECR_WRAP_EXT:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  stencil[i]--;
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil s = stencil[i];
                  stencil[i] = (GLstencil) ((invmask & s) | (wrtmask & (stencil[i]-1)));
	       }
	    }
	 }
	 break;
      case GL_INVERT:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  GLstencil s = stencil[i];
		  stencil[i] = (GLstencil) ~s;
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
		  GLstencil s = stencil[i];
		  stencil[i] = (GLstencil) ((invmask & s) | (wrtmask & ~s));
	       }
	    }
	 }
	 break;
      default:
         gl_problem(ctx, "Bad stencil op in apply_stencil_op");
   }
}




/*
 * Apply stencil test to a span of pixels before depth buffering.
 * Input:  n - number of pixels in the span
 *         x, y - coordinate of left-most pixel in the span
 *         mask - array [n] of flag:  0=skip the pixel, 1=stencil the pixel
 * Output:  mask - pixels which fail the stencil test will have their
 *                 mask flag set to 0.
 * Return:  0 = all pixels failed, 1 = zero or more pixels passed.
 */
GLint gl_stencil_span( GLcontext *ctx,
                       GLuint n, GLint x, GLint y, GLubyte mask[] )
{
   GLubyte fail[MAX_WIDTH];
   GLint allfail = 0;
   GLuint i;
   GLstencil r, s;
   GLstencil *stencil;

   stencil = STENCIL_ADDRESS( x, y );

   /*
    * Perform stencil test.  The results of this operation are stored
    * in the fail[] array:
    *   IF fail[i] is non-zero THEN
    *       the stencil fail operator is to be applied
    *   ELSE
    *       the stencil fail operator is not to be applied
    *   ENDIF
    */
   switch (ctx->Stencil.Function) {
      case GL_NEVER:
         /* always fail */
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       mask[i] = 0;
	       fail[i] = 1;
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 allfail = 1;
	 break;
      case GL_LESS:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
	       s = (GLstencil) (stencil[i] & ctx->Stencil.ValueMask);
	       if (r < s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_LEQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
	       s = (GLstencil) (stencil[i] & ctx->Stencil.ValueMask);
	       if (r <= s) {
		  /* pass */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_GREATER:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
	       s = (GLstencil) (stencil[i] & ctx->Stencil.ValueMask);
	       if (r > s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_GEQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
	       s = (GLstencil) (stencil[i] & ctx->Stencil.ValueMask);
	       if (r >= s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_EQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
	       s = (GLstencil) (stencil[i] & ctx->Stencil.ValueMask);
	       if (r == s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_NOTEQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
	       s = (GLstencil) (stencil[i] & ctx->Stencil.ValueMask);
	       if (r != s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_ALWAYS:
	 /* always pass */
	 for (i=0;i<n;i++) {
	    fail[i] = 0;
	 }
	 break;
      default:
         gl_problem(ctx, "Bad stencil func in gl_stencil_span");
         return 0;
   }

   if (ctx->Stencil.FailFunc != GL_KEEP) {
      apply_stencil_op( ctx, ctx->Stencil.FailFunc, n, stencil, fail );
   }

   return (allfail) ? 0 : 1;
}




/*
 * Apply the combination depth-buffer/stencil operator to a span of pixels.
 * Input:  n - number of pixels in the span
 *         x, y - location of leftmost pixel in span
 *         z - array [n] of z values
 * Input:  mask - array [n] of flags  (1=test this pixel, 0=skip the pixel)
 * Output:  mask - array [n] of flags (1=depth test passed, 0=failed)
 */
void gl_depth_stencil_span( GLcontext *ctx,
                            GLuint n, GLint x, GLint y, const GLdepth z[],
			    GLubyte mask[] )
{
   GLstencil *stencil = STENCIL_ADDRESS(x, y);

   if (ctx->Depth.Test==GL_FALSE) {
      /*
       * No depth buffer, just apply zpass stencil function to active pixels.
       */
      apply_stencil_op( ctx, ctx->Stencil.ZPassFunc, n, stencil, mask );
   }
   else {
      /*
       * Perform depth buffering, then apply zpass or zfail stencil function.
       */
      GLubyte passmask[MAX_WIDTH], failmask[MAX_WIDTH], oldmask[MAX_WIDTH];
      GLuint i;

      /* save the current mask bits */
      MEMCPY(oldmask, mask, n * sizeof(GLubyte));

      /* apply the depth test */
      if (ctx->Driver.DepthTestSpan)
         (*ctx->Driver.DepthTestSpan)( ctx, n, x, y, z, mask );

      /* Set the stencil pass/fail flags according to result of depth testing.
       * if oldmask[i] == 0 then
       *    Don't touch the stencil value
       * else if oldmask[i] and newmask[i] then
       *    Depth test passed
       * else
       *    assert(oldmask[i] && !newmask[i])
       *    Depth test failed
       * endif
       */
      for (i=0;i<n;i++) {
         ASSERT(mask[i] == 0 || mask[i] == 1);
         passmask[i] = oldmask[i] & mask[i];
         failmask[i] = oldmask[i] & (mask[i] ^ 1);
      }

      /* apply the pass and fail operations */
      if (ctx->Stencil.ZFailFunc != GL_KEEP) {
         apply_stencil_op( ctx, ctx->Stencil.ZFailFunc, n, stencil, failmask );
      }
      if (ctx->Stencil.ZPassFunc != GL_KEEP) {
         apply_stencil_op( ctx, ctx->Stencil.ZPassFunc, n, stencil, passmask );
      }
   }
}




/*
 * Apply the given stencil operator for each pixel in the array whose
 * mask flag is set.
 * Input:  n - number of pixels in the span
 *         x, y - array of [n] pixels
 *         operator - the stencil buffer operator
 *         mask - array [n] of flag:  1=apply operator, 0=don't apply operator
 */
static void apply_stencil_op_to_pixels( GLcontext *ctx,
                                        GLuint n, const GLint x[],
				        const GLint y[],
				        GLenum oper, GLubyte mask[] )
{
   GLuint i;
   GLstencil ref;
   GLstencil wrtmask, invmask;

   wrtmask = ctx->Stencil.WriteMask;
   invmask = (GLstencil) (~ctx->Stencil.WriteMask);

   ref = ctx->Stencil.Ref;

   switch (oper) {
      case GL_KEEP:
         /* do nothing */
         break;
      case GL_ZERO:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = 0;
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
		  *sptr = (GLstencil) (invmask & *sptr);
	       }
	    }
	 }
	 break;
      case GL_REPLACE:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = ref;
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
		  *sptr = (GLstencil) ((invmask & *sptr ) | (wrtmask & ref));
	       }
	    }
	 }
	 break;
      case GL_INCR:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
		  if (*sptr < STENCIL_MAX) {
		     *sptr = (GLstencil) (*sptr + 1);
		  }
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
		  if (*sptr < STENCIL_MAX) {
		     *sptr = (GLstencil) ((invmask & *sptr) | (wrtmask & (*sptr+1)));
		  }
	       }
	    }
	 }
	 break;
      case GL_DECR:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
		  if (*sptr>0) {
		     *sptr = (GLstencil) (*sptr - 1);
		  }
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
		  if (*sptr>0) {
		     *sptr = (GLstencil) ((invmask & *sptr) | (wrtmask & (*sptr-1)));
		  }
	       }
	    }
	 }
	 break;
      case GL_INCR_WRAP_EXT:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = (GLstencil) (*sptr + 1);
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = (GLstencil) ((invmask & *sptr) | (wrtmask & (*sptr+1)));
	       }
	    }
	 }
	 break;
      case GL_DECR_WRAP_EXT:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = (GLstencil) (*sptr - 1);
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = (GLstencil) ((invmask & *sptr) | (wrtmask & (*sptr-1)));
	       }
	    }
	 }
	 break;
      case GL_INVERT:
	 if (invmask==0) {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = (GLstencil) (~*sptr);
	       }
	    }
	 }
	 else {
	    for (i=0;i<n;i++) {
	       if (mask[i]) {
                  GLstencil *sptr = STENCIL_ADDRESS( x[i], y[i] );
                  *sptr = (GLstencil) ((invmask & *sptr) | (wrtmask & ~*sptr));
	       }
	    }
	 }
	 break;
      default:
         gl_problem(ctx, "Bad stencilop in apply_stencil_op_to_pixels");
   }
}



/*
 * Apply stencil test to an array of pixels before depth buffering.
 * Input:  n - number of pixels in the span
 *         x, y - array of [n] pixels to stencil
 *         mask - array [n] of flag:  0=skip the pixel, 1=stencil the pixel
 * Output:  mask - pixels which fail the stencil test will have their
 *                 mask flag set to 0.
 * Return:  0 = all pixels failed, 1 = zero or more pixels passed.
 */
GLint gl_stencil_pixels( GLcontext *ctx,
                         GLuint n, const GLint x[], const GLint y[],
			 GLubyte mask[] )
{
   GLubyte fail[PB_SIZE];
   GLstencil r, s;
   GLuint i;
   GLint allfail = 0;

   /*
    * Perform stencil test.  The results of this operation are stored
    * in the fail[] array:
    *   IF fail[i] is non-zero THEN
    *       the stencil fail operator is to be applied
    *   ELSE
    *       the stencil fail operator is not to be applied
    *   ENDIF
    */

   switch (ctx->Stencil.Function) {
      case GL_NEVER:
         /* always fail */
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       mask[i] = 0;
	       fail[i] = 1;
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 allfail = 1;
	 break;
      case GL_LESS:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
               GLstencil *sptr = STENCIL_ADDRESS(x[i],y[i]);
	       s = (GLstencil) (*sptr & ctx->Stencil.ValueMask);
	       if (r < s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_LEQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
               GLstencil *sptr = STENCIL_ADDRESS(x[i],y[i]);
	       s = (GLstencil) (*sptr & ctx->Stencil.ValueMask);
	       if (r <= s) {
		  /* pass */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_GREATER:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
               GLstencil *sptr = STENCIL_ADDRESS(x[i],y[i]);
	       s = (GLstencil) (*sptr & ctx->Stencil.ValueMask);
	       if (r > s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_GEQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
               GLstencil *sptr = STENCIL_ADDRESS(x[i],y[i]);
	       s = (GLstencil) (*sptr & ctx->Stencil.ValueMask);
	       if (r >= s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_EQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
               GLstencil *sptr = STENCIL_ADDRESS(x[i],y[i]);
	       s = (GLstencil) (*sptr & ctx->Stencil.ValueMask);
	       if (r == s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_NOTEQUAL:
	 r = (GLstencil) (ctx->Stencil.Ref & ctx->Stencil.ValueMask);
	 for (i=0;i<n;i++) {
	    if (mask[i]) {
               GLstencil *sptr = STENCIL_ADDRESS(x[i],y[i]);
	       s = (GLstencil) (*sptr & ctx->Stencil.ValueMask);
	       if (r != s) {
		  /* passed */
		  fail[i] = 0;
	       }
	       else {
		  fail[i] = 1;
		  mask[i] = 0;
	       }
	    }
	    else {
	       fail[i] = 0;
	    }
	 }
	 break;
      case GL_ALWAYS:
	 /* always pass */
	 for (i=0;i<n;i++) {
	    fail[i] = 0;
	 }
	 break;
      default:
         gl_problem(ctx, "Bad stencil func in gl_stencil_pixels");
         return 0;
   }

   apply_stencil_op_to_pixels( ctx, n, x, y, ctx->Stencil.FailFunc, fail );

   return (allfail) ? 0 : 1;
}




/*
 * Apply the combination depth-buffer/stencil operator to a span of pixels.
 * Input:  n - number of pixels in the span
 *         x, y - array of [n] pixels to stencil
 *         z - array [n] of z values
 * Input:  mask - array [n] of flags  (1=test this pixel, 0=skip the pixel)
 * Output:  mask - array [n] of flags (1=depth test passed, 0=failed)
 */
void gl_depth_stencil_pixels( GLcontext *ctx,
                              GLuint n, const GLint x[], const GLint y[],
			      const GLdepth z[], GLubyte mask[] )
{
   if (ctx->Depth.Test==GL_FALSE) {
      /*
       * No depth buffer, just apply zpass stencil function to active pixels.
       */
      apply_stencil_op_to_pixels( ctx, n, x, y, ctx->Stencil.ZPassFunc, mask );
   }
   else {
      /*
       * Perform depth buffering, then apply zpass or zfail stencil function.
       */
      GLubyte passmask[PB_SIZE], failmask[PB_SIZE], oldmask[PB_SIZE];
      GLuint i;

      /* save the current mask bits */
      MEMCPY(oldmask, mask, n * sizeof(GLubyte));

      /* apply the depth test */
      if (ctx->Driver.DepthTestPixels)
         (*ctx->Driver.DepthTestPixels)( ctx, n, x, y, z, mask );

      /* Set the stencil pass/fail flags according to result of depth testing.
       * if oldmask[i] == 0 then
       *    Don't touch the stencil value
       * else if oldmask[i] and newmask[i] then
       *    Depth test passed
       * else
       *    assert(oldmask[i] && !newmask[i])
       *    Depth test failed
       * endif
       */
      for (i=0;i<n;i++) {
         ASSERT(mask[i] == 0 || mask[i] == 1);
         passmask[i] = oldmask[i] & mask[i];
         failmask[i] = oldmask[i] & (mask[i] ^ 1);
      }

      /* apply the pass and fail operations */
      apply_stencil_op_to_pixels( ctx, n, x, y,
                                  ctx->Stencil.ZFailFunc, failmask );
      apply_stencil_op_to_pixels( ctx, n, x, y,
                                  ctx->Stencil.ZPassFunc, passmask );
   }

}



/*
 * Return a span of stencil values from the stencil buffer.
 * Input:  n - how many pixels
 *         x,y - location of first pixel
 * Output:  stencil - the array of stencil values
 */
void gl_read_stencil_span( GLcontext *ctx,
                           GLuint n, GLint x, GLint y, GLstencil stencil[] )
{
   if (ctx->Buffer->Stencil) {
      const GLstencil *s = STENCIL_ADDRESS( x, y );
#if STENCIL_BITS == 8
      MEMCPY( stencil, s, n * sizeof(GLstencil) );
#else
      GLuint i;
      for (i=0;i<n;i++)
         stencil[i] = s[i];
#endif
   }
}



/*
 * Write a span of stencil values to the stencil buffer.
 * Input:  n - how many pixels
 *         x,y - location of first pixel
 *         stencil - the array of stencil values
 */
void gl_write_stencil_span( GLcontext *ctx,
                            GLuint n, GLint x, GLint y,
			    const GLstencil stencil[] )
{
   if (ctx->Buffer->Stencil) {
      GLstencil *s = STENCIL_ADDRESS( x, y );
#if STENCIL_BITS == 8
      MEMCPY( s, stencil, n * sizeof(GLstencil) );
#else
      GLuint i;
      for (i=0;i<n;i++)
         s[i] = stencil[i];
#endif
   }
}



/*
 * Allocate a new stencil buffer.  If there's an old one it will be
 * deallocated first.  The new stencil buffer will be uninitialized.
 */
void gl_alloc_stencil_buffer( GLcontext *ctx )
{
   GLuint buffersize = ctx->Buffer->Width * ctx->Buffer->Height;

   /* deallocate current stencil buffer if present */
   if (ctx->Buffer->Stencil) {
      FREE(ctx->Buffer->Stencil);
      ctx->Buffer->Stencil = NULL;
   }

   /* allocate new stencil buffer */
   ctx->Buffer->Stencil = (GLstencil *) MALLOC(buffersize * sizeof(GLstencil));
   if (!ctx->Buffer->Stencil) {
      /* out of memory */
      gl_set_enable( ctx, GL_STENCIL_TEST, GL_FALSE );
      gl_error( ctx, GL_OUT_OF_MEMORY, "gl_alloc_stencil_buffer" );
   }
}




/*
 * Clear the stencil buffer.  If the stencil buffer doesn't exist yet we'll
 * allocate it now.
 */
void gl_clear_stencil_buffer( GLcontext *ctx )
{
   if (ctx->Visual->StencilBits==0 || !ctx->Buffer->Stencil) {
      /* no stencil buffer */
      return;
   }

   if (ctx->Scissor.Enabled) {
      /* clear scissor region only */
      const GLint width = ctx->Buffer->Xmax - ctx->Buffer->Xmin + 1;
      if (ctx->Stencil.WriteMask != STENCIL_MAX) {
         /* must apply mask to the clear */
         GLint y;
         for (y=ctx->Buffer->Ymin; y<=ctx->Buffer->Ymax; y++) {
            GLstencil *ptr = STENCIL_ADDRESS( ctx->Buffer->Xmin, y );
            GLint x;
            const GLstencil mask = ctx->Stencil.WriteMask;
            const GLstencil invMask = ~mask;
            const GLstencil clearVal = (ctx->Stencil.Clear & mask);
            for (x = 0; x < width; x++) {
               ptr[x] = (ptr[x] & invMask) | clearVal;
            }
         }
      }
      else {
         /* no masking */
         GLint y;
         for (y=ctx->Buffer->Ymin; y<=ctx->Buffer->Ymax; y++) {
            GLstencil *ptr = STENCIL_ADDRESS( ctx->Buffer->Xmin, y );
#if STENCIL_BITS==8
            MEMSET( ptr, ctx->Stencil.Clear, width * sizeof(GLstencil) );
#else
            GLint x;
            for (x = 0; x < width; x++)
               ptr[x] = ctx->Stencil.Clear;
#endif
         }
      }
   }
   else {
      /* clear whole stencil buffer */
      if (ctx->Stencil.WriteMask != STENCIL_MAX) {
         /* must apply mask to the clear */
         const GLuint n = ctx->Buffer->Width * ctx->Buffer->Height;
         GLstencil *buffer = ctx->Buffer->Stencil;
         const GLstencil mask = ctx->Stencil.WriteMask;
         const GLstencil invMask = ~mask;
         const GLstencil clearVal = (ctx->Stencil.Clear & mask);
         GLuint i;
         for (i = 0; i < n; i++) {
            buffer[i] = (buffer[i] & invMask) | clearVal;
         }
      }
      else {
         /* clear whole buffer without masking */
         const GLuint n = ctx->Buffer->Width * ctx->Buffer->Height;
         GLstencil *buffer = ctx->Buffer->Stencil;

#if STENCIL_BITS==8
         MEMSET(buffer, ctx->Stencil.Clear, n * sizeof(GLstencil) );
#else
         GLuint i;
         for (i = 0; i < n; i++) {
            buffer[i] = ctx->Stencil.Clear;
         }
#endif
      }
   }
}
