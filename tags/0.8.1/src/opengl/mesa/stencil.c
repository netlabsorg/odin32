/* $Id: stencil.c,v 1.3 2000-05-23 20:40:54 jeroen Exp $ */

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


/* $XFree86: xc/lib/GL/mesa/src/stencil.c,v 1.3 1999/04/04 00:20:32 dawes Exp $ */

#ifdef PC_HEADER
#include "all.h"
#else
#include "glheader.h"
#include "types.h"
#include "context.h"
#include "macros.h"
#include "pb.h"
#include "stencil.h"
#include "enable.h"
#include "mem.h"
#include "depth.h"
#endif



void
_mesa_ClearStencil( GLint s )
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glClearStencil");
   ctx->Stencil.Clear = (GLstencil) s;

   if (ctx->Driver.ClearStencil) {
      (*ctx->Driver.ClearStencil)( ctx, s );
   }
}



void
_mesa_StencilFunc( GLenum func, GLint ref, GLuint mask )
{
   GET_CURRENT_CONTEXT(ctx);
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



void
_mesa_StencilMask( GLuint mask )
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glStencilMask");
   ctx->Stencil.WriteMask = (GLstencil) mask;

   if (ctx->Driver.StencilMask) {
      (*ctx->Driver.StencilMask)( ctx, mask );
   }
}



void
_mesa_StencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
   GET_CURRENT_CONTEXT(ctx);
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
 * Return the address of a stencil buffer value given the window coords:
 */
#define STENCIL_ADDRESS(X,Y)  \
       (ctx->DrawBuffer->Stencil + ctx->DrawBuffer->Width * (Y) + (X))



/*
 * Apply the given stencil operator to the array of stencil values.
 * Don't touch stencil[i] if mask[i] is zero.
 * Input:  n - size of stencil array
 *         oper - the stencil buffer operator
 *         stencil - array of stencil values
 *         mask - array [n] of flag:  1=apply operator, 0=don't apply operator
 * Output:  stencil - modified values
 */
static void apply_stencil_op( const GLcontext *ctx, GLenum oper,
                              GLuint n, GLstencil stencil[],
                              const GLubyte mask[] )
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
                  stencil[i] = (GLstencil) ((invmask & s) | (wrtmask & (s+1)));
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
                  stencil[i] = (GLstencil) ((invmask & s) | (wrtmask & (s-1)));
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
 * Apply stencil test to an array of stencil values (before depth buffering).
 * Input:  n - number of pixels in the array
 *         stencil - array of [n] stencil values
 *         mask - array [n] of flag:  0=skip the pixel, 1=stencil the pixel
 * Output:  mask - pixels which fail the stencil test will have their
 *                 mask flag set to 0.
 *          stencil - updated stencil values (where the test passed)
 * Return:  GL_FALSE = all pixels failed, GL_TRUE = zero or more pixels passed.
 */
static GLboolean
do_stencil_test( GLcontext *ctx, GLuint n, GLstencil stencil[],
                 GLubyte mask[] )
{
   GLubyte fail[PB_SIZE];
   GLboolean allfail = GL_FALSE;
   GLuint i;
   GLstencil r, s;

   ASSERT(n <= PB_SIZE);

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
         allfail = GL_TRUE;
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

   return !allfail;
}




/*
 * Apply stencil and depth testing to an array of pixels.
 * Hardware or software stencil buffer acceptable.
 * Input:  n - number of pixels in the span
 *         z - array [n] of z values
 *         stencil - array [n] of stencil values
 *         mask - array [n] of flags  (1=test this pixel, 0=skip the pixel)
 * Output:  stencil - modified stencil values
 *          mask - array [n] of flags (1=stencil and depth test passed)
 * Return: GL_TRUE - all fragments failed the testing
 *         GL_FALSE - one or more fragments passed the testing
 *
 */
static GLboolean
stencil_and_depth_test_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
                             const GLdepth z[], GLstencil stencil[],
                             GLubyte mask[] )
{
   ASSERT(ctx->Stencil.Enabled);
   ASSERT(n <= PB_SIZE);

   /*
    * Apply the stencil test to the fragments.
    * failMask[i] is 1 if the stencil test failed.
    */
   if (do_stencil_test( ctx, n, stencil, mask ) == GL_FALSE) {
      /* all fragments failed the stencil test, we're done. */
      return GL_FALSE;
   }


   /*
    * Some fragments passed the stencil test, apply depth test to them
    * and apply Zpass and Zfail stencil ops.
    */
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
      _mesa_depth_test_span(ctx, n, x, y, z, mask);

      /* Set the stencil pass/fail flags according to result of depth testing.
       * if oldmask[i] == 0 then
       *    Don't touch the stencil value
       * else if oldmask[i] and newmask[i] then
       *    Depth test passed
       * else
       *    ASSERT(oldmask[i] && !newmask[i])
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

   return GL_TRUE;  /* one or more fragments passed both tests */
}



/*
 * Apply stencil and depth testing to the span of pixels.
 * Both software and hardware stencil buffers are acceptable.
 * Input:  n - number of pixels in the span
 *         x, y - location of leftmost pixel in span
 *         z - array [n] of z values
 *         mask - array [n] of flags  (1=test this pixel, 0=skip the pixel)
 * Output:  mask - array [n] of flags (1=stencil and depth test passed)
 * Return: GL_TRUE - all fragments failed the testing
 *         GL_FALSE - one or more fragments passed the testing
 *
 */
GLboolean
gl_stencil_and_depth_test_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
                                const GLdepth z[], GLubyte mask[] )
{
   GLstencil stencilRow[MAX_WIDTH];
   GLstencil *stencil;
   GLboolean result;

   ASSERT(ctx->Stencil.Enabled);
   ASSERT(n <= MAX_WIDTH);

   /* Get initial stencil values */
   if (ctx->Driver.WriteStencilSpan) {
      ASSERT(ctx->Driver.ReadStencilSpan);
      /* Get stencil values from the hardware stencil buffer */
      (*ctx->Driver.ReadStencilSpan)(ctx, n, x, y, stencilRow);
      stencil = stencilRow;
   }
   else {
      /* software stencil buffer */
      stencil = STENCIL_ADDRESS(x, y);
   }

   /* do all the stencil/depth testing/updating */
   result = stencil_and_depth_test_span( ctx, n, x, y, z, stencil, mask );

   if (ctx->Driver.WriteStencilSpan) {
      /* Write updated stencil values into hardware stencil buffer */
      (ctx->Driver.WriteStencilSpan)(ctx, n, x, y, stencil, mask );
   }

   return result;
}




/*
 * Apply the given stencil operator for each pixel in the array whose
 * mask flag is set.  This is for software stencil buffers only.
 * Input:  n - number of pixels in the span
 *         x, y - array of [n] pixels
 *         operator - the stencil buffer operator
 *         mask - array [n] of flag:  1=apply operator, 0=don't apply operator
 */
static void
apply_stencil_op_to_pixels( const GLcontext *ctx,
                            GLuint n, const GLint x[], const GLint y[],
                            GLenum oper, const GLubyte mask[] )
{
   const GLstencil ref = ctx->Stencil.Ref;
   const GLstencil wrtmask = ctx->Stencil.WriteMask;
   const GLstencil invmask = (GLstencil) (~ctx->Stencil.WriteMask);
   GLuint i;

   ASSERT(!ctx->Driver.WriteStencilSpan);  /* software stencil buffer only! */

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
 * Used for software stencil buffer only.
 * Input:  n - number of pixels in the span
 *         x, y - array of [n] pixels to stencil
 *         mask - array [n] of flag:  0=skip the pixel, 1=stencil the pixel
 * Output:  mask - pixels which fail the stencil test will have their
 *                 mask flag set to 0.
 * Return:  0 = all pixels failed, 1 = zero or more pixels passed.
 */
static GLboolean
stencil_test_pixels( GLcontext *ctx, GLuint n,
                     const GLint x[], const GLint y[], GLubyte mask[] )
{
   GLubyte fail[PB_SIZE];
   GLstencil r, s;
   GLuint i;
   GLboolean allfail = GL_FALSE;

   ASSERT(!ctx->Driver.WriteStencilSpan);  /* software stencil buffer only! */

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
         allfail = GL_TRUE;
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

   if (ctx->Stencil.FailFunc != GL_KEEP) {
      apply_stencil_op_to_pixels( ctx, n, x, y, ctx->Stencil.FailFunc, fail );
   }

   return !allfail;
}




/*
 * Apply stencil and depth testing to an array of pixels.
 * This is used both for software and hardware stencil buffers.
 *
 * The comments in this function are a bit sparse but the code is
 * almost identical to stencil_and_depth_test_span(), which is well
 * commented.
 *
 * Input:  n - number of pixels in the array
 *         x, y - array of [n] pixel positions
 *         z - array [n] of z values
 *         mask - array [n] of flags  (1=test this pixel, 0=skip the pixel)
 * Output: mask - array [n] of flags (1=stencil and depth test passed)
 * Return: GL_TRUE - all fragments failed the testing
 *         GL_FALSE - one or more fragments passed the testing
 */
GLboolean
gl_stencil_and_depth_test_pixels( GLcontext *ctx,
                                  GLuint n, const GLint x[], const GLint y[],
                                  const GLdepth z[], GLubyte mask[] )
{
   ASSERT(ctx->Stencil.Enabled);
   ASSERT(n <= PB_SIZE);

   if (ctx->Driver.WriteStencilPixels) {
      /*** Hardware stencil buffer ***/
      GLstencil stencil[PB_SIZE];
      GLubyte mask[PB_SIZE];

      ASSERT(ctx->Driver.ReadStencilPixels);
      (*ctx->Driver.ReadStencilPixels)(ctx, n, x, y, stencil);


      if (do_stencil_test( ctx, n, stencil, mask ) == GL_FALSE) {
         /* all fragments failed the stencil test, we're done. */
         return GL_FALSE;
      }

      if (ctx->Depth.Test == GL_FALSE) {
         apply_stencil_op( ctx, ctx->Stencil.ZPassFunc, n, stencil, mask );
      }
      else {
         GLubyte passmask[PB_SIZE], failmask[PB_SIZE], oldmask[PB_SIZE];
         GLuint i;

         MEMCPY(oldmask, mask, n * sizeof(GLubyte));

         _mesa_depth_test_pixels(ctx, n, x, y, z, mask);

         for (i=0;i<n;i++) {
            ASSERT(mask[i] == 0 || mask[i] == 1);
            passmask[i] = oldmask[i] & mask[i];
            failmask[i] = oldmask[i] & (mask[i] ^ 1);
         }

         if (ctx->Stencil.ZFailFunc != GL_KEEP) {
            apply_stencil_op( ctx, ctx->Stencil.ZFailFunc, n, stencil, failmask );
         }
         if (ctx->Stencil.ZPassFunc != GL_KEEP) {
            apply_stencil_op( ctx, ctx->Stencil.ZPassFunc, n, stencil, passmask );
         }
      }

      /* Write updated stencil values into hardware stencil buffer */
      (ctx->Driver.WriteStencilPixels)(ctx, n, x, y, stencil, mask );

      return GL_TRUE;

   }
   else {
      /*** Software stencil buffer ***/

      if (stencil_test_pixels(ctx, n, x, y, mask) == GL_FALSE) {
         /* all fragments failed the stencil test, we're done. */
         return GL_FALSE;
      }


      if (ctx->Depth.Test==GL_FALSE) {
         apply_stencil_op_to_pixels( ctx, n, x, y, ctx->Stencil.ZPassFunc, mask );
      }
      else {
         GLubyte passmask[PB_SIZE], failmask[PB_SIZE], oldmask[PB_SIZE];
         GLuint i;

         MEMCPY(oldmask, mask, n * sizeof(GLubyte));

         _mesa_depth_test_pixels(ctx, n, x, y, z, mask);

         for (i=0;i<n;i++) {
            ASSERT(mask[i] == 0 || mask[i] == 1);
            passmask[i] = oldmask[i] & mask[i];
            failmask[i] = oldmask[i] & (mask[i] ^ 1);
         }

         if (ctx->Stencil.ZFailFunc != GL_KEEP) {
            apply_stencil_op_to_pixels( ctx, n, x, y,
                                        ctx->Stencil.ZFailFunc, failmask );
         }
         if (ctx->Stencil.ZPassFunc != GL_KEEP) {
            apply_stencil_op_to_pixels( ctx, n, x, y,
                                        ctx->Stencil.ZPassFunc, passmask );
         }
      }

      return GL_TRUE;  /* one or more fragments passed both tests */
   }
}



/*
 * Return a span of stencil values from the stencil buffer.
 * Used for glRead/CopyPixels
 * Input:  n - how many pixels
 *         x,y - location of first pixel
 * Output:  stencil - the array of stencil values
 */
void gl_read_stencil_span( GLcontext *ctx,
                           GLint n, GLint x, GLint y, GLstencil stencil[] )
{
   ASSERT(n >= 0);
   if (ctx->DrawBuffer->Stencil) {
      if (ctx->Driver.ReadStencilSpan) {
         (*ctx->Driver.ReadStencilSpan)( ctx, (GLuint) n, x, y, stencil );
      }
      else {
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
}



/*
 * Write a span of stencil values to the stencil buffer.
 * Used for glDraw/CopyPixels
 * Input:  n - how many pixels
 *         x, y - location of first pixel
 *         stencil - the array of stencil values
 */
void gl_write_stencil_span( GLcontext *ctx,
                            GLint n, GLint x, GLint y,
                            const GLstencil stencil[] )
{
   ASSERT(n >= 0);
   if (ctx->DrawBuffer->Stencil) {
      /* do clipping */
      if (y < ctx->DrawBuffer->Ymin || y > ctx->DrawBuffer->Ymax)
         return;
      if (x < ctx->DrawBuffer->Xmin) {
         GLint diff = ctx->DrawBuffer->Xmin - x;
         n -= diff;
         stencil += diff;
         x = ctx->DrawBuffer->Xmin;
      }
      if (x + n > ctx->DrawBuffer->Xmax) {
         GLint diff = x + n - ctx->DrawBuffer->Xmax;
         n -= diff;
      }

      ASSERT( n >= 0);

      if (ctx->Driver.WriteStencilSpan) {
         (*ctx->Driver.WriteStencilSpan)( ctx, n, x, y, stencil, NULL );
      }
      else {
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
}



/*
 * Allocate a new stencil buffer.  If there's an old one it will be
 * deallocated first.  The new stencil buffer will be uninitialized.
 */
void gl_alloc_stencil_buffer( GLcontext *ctx )
{
   GLuint buffersize = ctx->DrawBuffer->Width * ctx->DrawBuffer->Height;

   /* deallocate current stencil buffer if present */
   if (ctx->DrawBuffer->Stencil) {
      FREE(ctx->DrawBuffer->Stencil);
      ctx->DrawBuffer->Stencil = NULL;
   }

   /* allocate new stencil buffer */
   ctx->DrawBuffer->Stencil = (GLstencil *) MALLOC(buffersize * sizeof(GLstencil));
   if (!ctx->DrawBuffer->Stencil) {
      /* out of memory */
      _mesa_set_enable( ctx, GL_STENCIL_TEST, GL_FALSE );
      gl_error( ctx, GL_OUT_OF_MEMORY, "gl_alloc_stencil_buffer" );
   }
}



/*
 * Clear the software (malloc'd) stencil buffer.
 */
static void
clear_software_stencil_buffer( GLcontext *ctx )
{
   if (ctx->Visual->StencilBits==0 || !ctx->DrawBuffer->Stencil) {
      /* no stencil buffer */
      return;
   }

   if (ctx->Scissor.Enabled) {
      /* clear scissor region only */
      const GLint width = ctx->DrawBuffer->Xmax - ctx->DrawBuffer->Xmin + 1;
      if (ctx->Stencil.WriteMask != STENCIL_MAX) {
         /* must apply mask to the clear */
         GLint y;
         for (y = ctx->DrawBuffer->Ymin; y <= ctx->DrawBuffer->Ymax; y++) {
            const GLstencil mask = ctx->Stencil.WriteMask;
            const GLstencil invMask = ~mask;
            const GLstencil clearVal = (ctx->Stencil.Clear & mask);
            GLstencil *stencil = STENCIL_ADDRESS( ctx->DrawBuffer->Xmin, y );
            GLint i;
            for (i = 0; i < width; i++) {
               stencil[i] = (stencil[i] & invMask) | clearVal;
            }
         }
      }
      else {
         /* no masking */
         GLint y;
         for (y = ctx->DrawBuffer->Ymin; y <= ctx->DrawBuffer->Ymax; y++) {
            GLstencil *stencil = STENCIL_ADDRESS( ctx->DrawBuffer->Xmin, y );
#if STENCIL_BITS==8
            MEMSET( stencil, ctx->Stencil.Clear, width * sizeof(GLstencil) );
#else
            GLint i;
            for (i = 0; i < width; i++)
               stencil[x] = ctx->Stencil.Clear;
#endif
         }
      }
   }
   else {
      /* clear whole stencil buffer */
      if (ctx->Stencil.WriteMask != STENCIL_MAX) {
         /* must apply mask to the clear */
         const GLuint n = ctx->DrawBuffer->Width * ctx->DrawBuffer->Height;
         GLstencil *stencil = ctx->DrawBuffer->Stencil;
         const GLstencil mask = ctx->Stencil.WriteMask;
         const GLstencil invMask = ~mask;
         const GLstencil clearVal = (ctx->Stencil.Clear & mask);
         GLuint i;
         for (i = 0; i < n; i++) {
            stencil[i] = (stencil[i] & invMask) | clearVal;
         }
      }
      else {
         /* clear whole buffer without masking */
         const GLuint n = ctx->DrawBuffer->Width * ctx->DrawBuffer->Height;
         GLstencil *stencil = ctx->DrawBuffer->Stencil;

#if STENCIL_BITS==8
         MEMSET(stencil, ctx->Stencil.Clear, n * sizeof(GLstencil) );
#else
         GLuint i;
         for (i = 0; i < n; i++) {
            stencil[i] = ctx->Stencil.Clear;
         }
#endif
      }
   }
}



/*
 * Clear the hardware (in graphics card) stencil buffer.
 * This is done with the Driver.WriteStencilSpan() and Driver.ReadStencilSpan()
 * functions.
 * Actually, if there is a hardware stencil buffer it really should have
 * been cleared in Driver.Clear()!  However, if the hardware does not
 * support scissored clears or masked clears (i.e. glStencilMask) then
 * we have to use the span-based functions.
 */
static void
clear_hardware_stencil_buffer( GLcontext *ctx )
{
   ASSERT(ctx->Driver.WriteStencilSpan);
   ASSERT(ctx->Driver.ReadStencilSpan);

   if (ctx->Scissor.Enabled) {
      /* clear scissor region only */
      const GLint x = ctx->DrawBuffer->Xmin;
      const GLint width = ctx->DrawBuffer->Xmax - ctx->DrawBuffer->Xmin + 1;
      if (ctx->Stencil.WriteMask != STENCIL_MAX) {
         /* must apply mask to the clear */
         GLint y;
         for (y = ctx->DrawBuffer->Ymin; y <= ctx->DrawBuffer->Ymax; y++) {
            const GLstencil mask = ctx->Stencil.WriteMask;
            const GLstencil invMask = ~mask;
            const GLstencil clearVal = (ctx->Stencil.Clear & mask);
            GLstencil stencil[MAX_WIDTH];
            GLint i;
            (*ctx->Driver.ReadStencilSpan)(ctx, x, y, width, stencil);
            for (i = 0; i < width; i++) {
               stencil[i] = (stencil[i] & invMask) | clearVal;
            }
            (*ctx->Driver.WriteStencilSpan)(ctx, x, y, width, stencil, NULL);
         }
      }
      else {
         /* no masking */
         GLstencil stencil[MAX_WIDTH];
         GLint y, i;
         for (i = 0; i < width; i++) {
            stencil[i] = ctx->Stencil.Clear;
         }
         for (y = ctx->DrawBuffer->Ymin; y <= ctx->DrawBuffer->Ymax; y++) {
            (*ctx->Driver.WriteStencilSpan)(ctx, x, y, width, stencil, NULL);
         }
      }
   }
   else {
      /* clear whole stencil buffer */
      if (ctx->Stencil.WriteMask != STENCIL_MAX) {
         /* must apply mask to the clear */
         const GLstencil mask = ctx->Stencil.WriteMask;
         const GLstencil invMask = ~mask;
         const GLstencil clearVal = (ctx->Stencil.Clear & mask);
         const GLint width = ctx->DrawBuffer->Width;
         const GLint height = ctx->DrawBuffer->Height;
         const GLint x = ctx->DrawBuffer->Xmin;
         GLint y;
         for (y = 0; y < height; y++) {
            GLstencil stencil[MAX_WIDTH];
            GLuint i;
            (*ctx->Driver.ReadStencilSpan)(ctx, x, y, width, stencil);
            for (i = 0; i < width; i++) {
               stencil[i] = (stencil[i] & invMask) | clearVal;
            }
            (*ctx->Driver.WriteStencilSpan)(ctx, x, y, width, stencil, NULL);
         }
      }
      else {
         /* clear whole buffer without masking */
         const GLint width = ctx->DrawBuffer->Width;
         const GLint height = ctx->DrawBuffer->Width;
         const GLint x = ctx->DrawBuffer->Xmin;
         GLstencil stencil[MAX_WIDTH];
         GLint y, i;
         for (i = 0; i < width; i++) {
            stencil[i] = ctx->Stencil.Clear;
         }
         for (y = 0; y < height; y++) {
            (*ctx->Driver.WriteStencilSpan)(ctx, x, y, width, stencil, NULL);
         }
      }
   }
}



/*
 * Clear the stencil buffer.
 */
void gl_clear_stencil_buffer( GLcontext *ctx )
{
   if (ctx->Driver.WriteStencilSpan) {
      ASSERT(ctx->Driver.ReadStencilSpan);
      clear_hardware_stencil_buffer(ctx);
   }
   else {
      clear_software_stencil_buffer(ctx);
   }
}

