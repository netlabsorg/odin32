/* $Id: alphabuf.c,v 1.2 2000-03-01 18:49:22 jeroen Exp $ */

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





/*
 * Software alpha planes.  Many frame buffers don't have alpha bits so
 * we simulate them in software.
 */

#include <stdlib.h>

#ifdef PC_HEADER
#include "all.h"
#else
#include "types.h"
#include "alphabuf.h"
#include "context.h"
#include "macros.h"
#endif



#define ALPHA_ADDR(X,Y)  (ctx->Buffer->Alpha + (Y) * ctx->Buffer->Width + (X))



/*
 * Allocate a new front and back alpha buffer.
 */
void gl_alloc_alpha_buffers( GLcontext *ctx )
{
   GLint bytes = ctx->Buffer->Width * ctx->Buffer->Height * sizeof(GLubyte);

   ASSERT(ctx->Visual->SoftwareAlpha);

   if (ctx->Buffer->FrontLeftAlpha) {
      FREE( ctx->Buffer->FrontLeftAlpha );
   }
   ctx->Buffer->FrontLeftAlpha = (GLubyte *) MALLOC( bytes );
   if (!ctx->Buffer->FrontLeftAlpha) {
      /* out of memory */
      gl_error( ctx, GL_OUT_OF_MEMORY,
                "Couldn't allocate front-left alpha buffer" );
   }

   if (ctx->Visual->DBflag) {
      if (ctx->Buffer->BackLeftAlpha) {
         FREE( ctx->Buffer->BackLeftAlpha );
      }
      ctx->Buffer->BackLeftAlpha = (GLubyte *) MALLOC( bytes );
      if (!ctx->Buffer->BackLeftAlpha) {
         /* out of memory */
         gl_error( ctx, GL_OUT_OF_MEMORY,
                      "Couldn't allocate back-left alpha buffer" );
      }
   }

   if (ctx->Visual->StereoFlag) {
      if (ctx->Buffer->FrontRightAlpha) {
         FREE( ctx->Buffer->FrontRightAlpha );
      }
      ctx->Buffer->FrontRightAlpha = (GLubyte *) MALLOC( bytes );
      if (!ctx->Buffer->FrontRightAlpha) {
         /* out of memory */
         gl_error( ctx, GL_OUT_OF_MEMORY,
                   "Couldn't allocate front-right alpha buffer" );
      }

      if (ctx->Visual->DBflag) {
         if (ctx->Buffer->BackRightAlpha) {
            FREE( ctx->Buffer->BackRightAlpha );
         }
         ctx->Buffer->BackRightAlpha = (GLubyte *) MALLOC( bytes );
         if (!ctx->Buffer->BackRightAlpha) {
            /* out of memory */
            gl_error( ctx, GL_OUT_OF_MEMORY,
                      "Couldn't allocate back-right alpha buffer" );
         }
      }
   }

   if (ctx->Color.DriverDrawBuffer == GL_FRONT_LEFT)
      ctx->Buffer->Alpha = ctx->Buffer->FrontLeftAlpha;
   else if (ctx->Color.DriverDrawBuffer == GL_BACK_LEFT)
      ctx->Buffer->Alpha = ctx->Buffer->BackLeftAlpha;
   else if (ctx->Color.DriverDrawBuffer == GL_FRONT_RIGHT)
      ctx->Buffer->Alpha = ctx->Buffer->FrontRightAlpha;
   else if (ctx->Color.DriverDrawBuffer == GL_BACK_RIGHT)
      ctx->Buffer->Alpha = ctx->Buffer->BackRightAlpha;
}


/*
 * Clear all the alpha buffers
 */
void gl_clear_alpha_buffers( GLcontext *ctx )
{
   const GLubyte aclear = (GLint) (ctx->Color.ClearColor[3] * 255.0F);
   GLuint bufferBit;

   ASSERT(ctx->Visual->SoftwareAlpha);
   ASSERT(ctx->Color.ColorMask[ACOMP]);

   /* loop over four possible alpha buffers */
   for (bufferBit = 1; bufferBit <= 8; bufferBit = bufferBit << 1) {
      if (bufferBit & ctx->Color.DrawDestMask) {
         GLubyte *buffer;
         if (bufferBit == FRONT_LEFT_BIT) {
            buffer = ctx->Buffer->FrontLeftAlpha;
         }
         else if (bufferBit == FRONT_RIGHT_BIT) {
            buffer = ctx->Buffer->FrontRightAlpha;
         }
         else if (bufferBit == BACK_LEFT_BIT) {
            buffer = ctx->Buffer->BackLeftAlpha;
         }
         else {
            buffer = ctx->Buffer->BackRightAlpha;
         }

         if (ctx->Scissor.Enabled) {
            /* clear scissor region */
            GLint j;
            GLint rowLen = ctx->Buffer->Xmax - ctx->Buffer->Xmin + 1;
            GLint rows = ctx->Buffer->Ymax - ctx->Buffer->Ymin + 1;
            GLubyte *aptr = buffer + ctx->Buffer->Ymin * ctx->Buffer->Width
                          + ctx->Buffer->Xmin;
            for (j = 0; j < rows; j++) {
               MEMSET( aptr, aclear, rowLen );
               aptr += rowLen;
            }
         }
         else {
            /* clear whole buffer */
            MEMSET( buffer, aclear, ctx->Buffer->Width * ctx->Buffer->Height );
         }
      }
   }
}



void gl_write_alpha_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
                          CONST GLubyte rgba[][4], const GLubyte mask[] )
{
   GLubyte *aptr = ALPHA_ADDR( x, y );
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            *aptr = rgba[i][ACOMP];
         }
         aptr++;
      }
   }
   else {
      for (i=0;i<n;i++) {
         *aptr++ = rgba[i][ACOMP];
      }
   }
}


void gl_write_mono_alpha_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
                               GLubyte alpha, const GLubyte mask[] )
{
   GLubyte *aptr = ALPHA_ADDR( x, y );
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            *aptr = alpha;
         }
         aptr++;
      }
   }
   else {
      for (i=0;i<n;i++) {
         *aptr++ = alpha;
      }
   }
}


void gl_write_alpha_pixels( GLcontext *ctx,
                            GLuint n, const GLint x[], const GLint y[],
                            CONST GLubyte rgba[][4], const GLubyte mask[] )
{
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
            *aptr = rgba[i][ACOMP];
         }
      }
   }
   else {
      for (i=0;i<n;i++) {
         GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
         *aptr = rgba[i][ACOMP];
      }
   }
}


void gl_write_mono_alpha_pixels( GLcontext *ctx,
                                 GLuint n, const GLint x[], const GLint y[],
                                 GLubyte alpha, const GLubyte mask[] )
{
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
            *aptr = alpha;
         }
      }
   }
   else {
      for (i=0;i<n;i++) {
         GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
         *aptr = alpha;
      }
   }
}



void gl_read_alpha_span( GLcontext *ctx,
                         GLuint n, GLint x, GLint y, GLubyte rgba[][4] )
{
   GLubyte *aptr = ALPHA_ADDR( x, y );
   GLuint i;
   for (i=0;i<n;i++) {
      rgba[i][ACOMP] = *aptr++;
   }
}


void gl_read_alpha_pixels( GLcontext *ctx,
                           GLuint n, const GLint x[], const GLint y[],
                           GLubyte rgba[][4], const GLubyte mask[] )
{
   GLuint i;
   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
         rgba[i][ACOMP] = *aptr;
      }
   }
}



