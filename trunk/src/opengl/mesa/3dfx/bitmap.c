/* $Id: bitmap.c,v 1.1 2000-02-29 00:46:12 sandervl Exp $ */

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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#else
#include "GL/xf86glx.h"
#endif
#include "bitmap.h"
#include "context.h"
#include "feedback.h"
#include "image.h"
#include "macros.h"
#include "pb.h"
#include "pixel.h"
#include "types.h"
#include "vbrender.h"
#endif



/*
 * Render bitmap data.
 */
static void render_bitmap( GLcontext *ctx, GLint px, GLint py,
                           GLsizei width, GLsizei height,
                           const struct gl_pixelstore_attrib *unpack,
                           const GLubyte *bitmap )
{
   struct pixel_buffer *PB = ctx->PB;
   GLint row, col;
   GLint pz;

   ASSERT(ctx->RenderMode == GL_RENDER);

   if (!bitmap) {
      return;  /* NULL bitmap is legal, a no-op */
   }

   if (ctx->NewState) {
      gl_update_state(ctx);
      gl_reduced_prim_change( ctx, GL_BITMAP );
   }

   /* Set bitmap drawing color */
   if (ctx->Visual->RGBAflag) {
      GLint r, g, b, a;
      r = (GLint) (ctx->Current.RasterColor[0] * 255.0F);
      g = (GLint) (ctx->Current.RasterColor[1] * 255.0F);
      b = (GLint) (ctx->Current.RasterColor[2] * 255.0F);
      a = (GLint) (ctx->Current.RasterColor[3] * 255.0F);
      PB_SET_COLOR( ctx, PB, r, g, b, a );
   }
   else {
      PB_SET_INDEX( ctx, PB, ctx->Current.RasterIndex );
   }

   pz = (GLint) ( ctx->Current.RasterPos[2] * DEPTH_SCALE );

   for (row=0; row<height; row++) {
      const GLubyte *src = (const GLubyte *) gl_pixel_addr_in_image( unpack,
                 bitmap, width, height, GL_COLOR_INDEX, GL_BITMAP, 0, row, 0 );

      if (unpack->LsbFirst) {
         /* Lsb first */
         GLubyte bitmask = 1;
         for (col=0; col<width; col++) {
            if (*src & bitmask) {
               PB_WRITE_PIXEL( PB, px+col, py+row, pz );
            }
            bitmask = bitmask << 1;
            if (bitmask == 0U) {
               src++;
               bitmask = 1U;
            }
         }

         PB_CHECK_FLUSH( ctx, PB );

         /* get ready for next row */
         if (bitmask != 1)
            src++;
      }
      else {
         /* Msb first */
         GLubyte bitmask = 128;
         for (col=0; col<width; col++) {
            if (*src & bitmask) {
               PB_WRITE_PIXEL( PB, px+col, py+row, pz );
            }
            bitmask = bitmask >> 1;
            if (bitmask == 0U) {
               src++;
               bitmask = 128U;
            }
         }

         PB_CHECK_FLUSH( ctx, PB );

         /* get ready for next row */
         if (bitmask!=128)
            src++;
      }
   }

   gl_flush_pb(ctx);
}



/*
 * Execute a glBitmap command.
 */
void gl_Bitmap( GLcontext *ctx,
                GLsizei width, GLsizei height,
	        GLfloat xorig, GLfloat yorig,
	        GLfloat xmove, GLfloat ymove,
                const GLubyte *bitmap,
                const struct gl_pixelstore_attrib *packing )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glBitmap");

   /* Error checking */
   if (width < 0 || height < 0) {
      gl_error( ctx, GL_INVALID_VALUE, "glBitmap" );
      return;
   }

   if (ctx->Current.RasterPosValid == GL_FALSE) {
      return;    /* do nothing */
   }

   if (ctx->RenderMode==GL_RENDER) {
      GLint x = (GLint) ( (ctx->Current.RasterPos[0] - xorig) + 0.0F );
      GLint y = (GLint) ( (ctx->Current.RasterPos[1] - yorig) + 0.0F );
      GLboolean completed = GL_FALSE;
      if (ctx->Driver.Bitmap) {
         /* let device driver try to render the bitmap */
         completed = (*ctx->Driver.Bitmap)( ctx, x, y, width, height,
                                            packing, bitmap );
      }
      if (!completed) {
         /* use generic function */
         render_bitmap( ctx, x, y, width, height, packing, bitmap );
      }
   }
   else if (ctx->RenderMode==GL_FEEDBACK) {
      GLfloat color[4], texcoord[4], invq;
      color[0] = ctx->Current.RasterColor[0];
      color[1] = ctx->Current.RasterColor[1];
      color[2] = ctx->Current.RasterColor[2];
      color[3] = ctx->Current.RasterColor[3];
      if (ctx->Current.Texcoord[0][3] == 0.0)
         invq = 1.0F;
      else
         invq = 1.0F / ctx->Current.RasterTexCoord[3];
      texcoord[0] = ctx->Current.RasterTexCoord[0] * invq;
      texcoord[1] = ctx->Current.RasterTexCoord[1] * invq;
      texcoord[2] = ctx->Current.RasterTexCoord[2] * invq;
      texcoord[3] = ctx->Current.RasterTexCoord[3];
      FEEDBACK_TOKEN( ctx, (GLfloat) (GLint) GL_BITMAP_TOKEN );
      gl_feedback_vertex( ctx,
                          ctx->Current.RasterPos,
			  color, ctx->Current.RasterIndex, texcoord );
   }
   else if (ctx->RenderMode==GL_SELECT) {
      /* Bitmaps don't generate selection hits.  See appendix B of 1.1 spec. */
   }

   /* update raster position */
   ctx->Current.RasterPos[0] += xmove;
   ctx->Current.RasterPos[1] += ymove;
}
