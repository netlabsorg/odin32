/* $Id: glmisc.c,v 1.1 2000-02-29 00:46:17 sandervl Exp $ */

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


/* $XFree86: xc/lib/GL/mesa/src/glmisc.c,v 1.2 1999/04/04 00:20:25 dawes Exp $ */

#ifdef PC_HEADER
#include "all.h"
#else
#ifndef XFree86Server
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#else
#include "GL/xf86glx.h"
#endif
#include "accum.h"
#include "alphabuf.h"
#include "context.h"
#include "depth.h"
#include "enums.h"
#include "extensions.h"
#include "glmisc.h"
#include "macros.h"
#include "masking.h"
#include "stencil.h"
#include "types.h"
#endif



void gl_ClearIndex( GLcontext *ctx, GLfloat c )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glClearIndex");
   ctx->Color.ClearIndex = (GLuint) c;
   if (!ctx->Visual->RGBAflag) {
      /* it's OK to call glClearIndex in RGBA mode but it should be a NOP */
      (*ctx->Driver.ClearIndex)( ctx, ctx->Color.ClearIndex );
   }
}



void gl_ClearColor( GLcontext *ctx, GLclampf red, GLclampf green,
                    GLclampf blue, GLclampf alpha )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glClearColor");

   ctx->Color.ClearColor[0] = CLAMP( red,   0.0F, 1.0F );
   ctx->Color.ClearColor[1] = CLAMP( green, 0.0F, 1.0F );
   ctx->Color.ClearColor[2] = CLAMP( blue,  0.0F, 1.0F );
   ctx->Color.ClearColor[3] = CLAMP( alpha, 0.0F, 1.0F );

   if (ctx->Visual->RGBAflag) {
      GLubyte r = (GLint) (ctx->Color.ClearColor[0] * 255.0F);
      GLubyte g = (GLint) (ctx->Color.ClearColor[1] * 255.0F);
      GLubyte b = (GLint) (ctx->Color.ClearColor[2] * 255.0F);
      GLubyte a = (GLint) (ctx->Color.ClearColor[3] * 255.0F);
      (*ctx->Driver.ClearColor)( ctx, r, g, b, a );
   }
}




/*
 * Clear the color buffer when glColorMask or glIndexMask is in effect.
 */
static void clear_color_buffer_with_masking( GLcontext *ctx )
{
   GLint x, y, height, width;

   /* Compute region to clear */
   if (ctx->Scissor.Enabled) {
      x = ctx->Buffer->Xmin;
      y = ctx->Buffer->Ymin;
      height = ctx->Buffer->Ymax - ctx->Buffer->Ymin + 1;
      width  = ctx->Buffer->Xmax - ctx->Buffer->Xmin + 1;
   }
   else {
      x = 0;
      y = 0;
      height = ctx->Buffer->Height;
      width  = ctx->Buffer->Width;
   }

   if (ctx->Visual->RGBAflag) {
      /* RGBA mode */
      GLubyte r = (GLint) (ctx->Color.ClearColor[0] * 255.0F);
      GLubyte g = (GLint) (ctx->Color.ClearColor[1] * 255.0F);
      GLubyte b = (GLint) (ctx->Color.ClearColor[2] * 255.0F);
      GLubyte a = (GLint) (ctx->Color.ClearColor[3] * 255.0F);
      GLint i;
      for (i=0;i<height;i++,y++) {
         GLubyte rgba[MAX_WIDTH][4];
         GLint j;
         for (j=0; j<width; j++) {
            rgba[j][RCOMP] = r;
            rgba[j][GCOMP] = g;
            rgba[j][BCOMP] = b;
            rgba[j][ACOMP] = a;
         }
         gl_mask_rgba_span( ctx, width, x, y, rgba );
         (*ctx->Driver.WriteRGBASpan)( ctx, width, x, y,
				       (const GLubyte (*)[4])rgba, NULL );
      }
   }
   else {
      /* Color index mode */
      GLuint indx[MAX_WIDTH];
      GLubyte mask[MAX_WIDTH];
      GLint i, j;
      MEMSET( mask, 1, width );
      for (i=0;i<height;i++,y++) {
         for (j=0;j<width;j++) {
            indx[j] = ctx->Color.ClearIndex;
         }
         gl_mask_index_span( ctx, width, x, y, indx );
         (*ctx->Driver.WriteCI32Span)( ctx, width, x, y, indx, mask );
      }
   }
}



/*
 * Clear the front and/or back color buffers.  Also clear the alpha
 * buffer(s) if present.
 */
static void clear_color_buffers( GLcontext *ctx )
{

   if (ctx->RasterMask & MULTI_DRAW_BIT) {
      GLuint bufferBit;

      if (ctx->Color.DrawBuffer == GL_NONE)
         return;

      /* loop over four possible dest color buffers */
      for (bufferBit = 1; bufferBit <= 8; bufferBit = bufferBit << 1) {
         if (bufferBit & ctx->Color.DrawDestMask) {
            if (bufferBit == FRONT_LEFT_BIT) {
               (*ctx->Driver.SetBuffer)( ctx, GL_FRONT_LEFT);
               ctx->Buffer->Alpha = ctx->Buffer->FrontLeftAlpha;
            }
            else if (bufferBit == FRONT_RIGHT_BIT) {
               (*ctx->Driver.SetBuffer)( ctx, GL_FRONT_RIGHT);
               ctx->Buffer->Alpha = ctx->Buffer->FrontRightAlpha;
            }
            else if (bufferBit == BACK_LEFT_BIT) {
               (*ctx->Driver.SetBuffer)( ctx, GL_BACK_LEFT);
               ctx->Buffer->Alpha = ctx->Buffer->BackLeftAlpha;
            }
            else {
               (*ctx->Driver.SetBuffer)( ctx, GL_BACK_RIGHT);
               ctx->Buffer->Alpha = ctx->Buffer->BackRightAlpha;
            }

            if (ctx->Color.SWmasking) {
               clear_color_buffer_with_masking( ctx );
            }
            else {
               GLint x = ctx->Buffer->Xmin;
               GLint y = ctx->Buffer->Ymin;
               GLint height = ctx->Buffer->Ymax - ctx->Buffer->Ymin + 1;
               GLint width  = ctx->Buffer->Xmax - ctx->Buffer->Xmin + 1;
               (void) (*ctx->Driver.Clear)( ctx, GL_COLOR_BUFFER_BIT,
                                            !ctx->Scissor.Enabled,
                                            x, y, width, height );
            }
         }
      }

      /* restore default dest buffer */
      (void) (*ctx->Driver.SetBuffer)( ctx, ctx->Color.DriverDrawBuffer );
   }
   else {
      /* normal case:  clear exactly one color buffer */

      if (ctx->Color.SWmasking) {
         clear_color_buffer_with_masking( ctx );
      }
      else {
         GLint x = ctx->Buffer->Xmin;
         GLint y = ctx->Buffer->Ymin;
         GLint height = ctx->Buffer->Ymax - ctx->Buffer->Ymin + 1;
         GLint width  = ctx->Buffer->Xmax - ctx->Buffer->Xmin + 1;
         (void) (*ctx->Driver.Clear)( ctx, GL_COLOR_BUFFER_BIT,
                                      !ctx->Scissor.Enabled,
                                      x, y, width, height );
      }
   }
}



void gl_Clear( GLcontext *ctx, GLbitfield mask )
{
#ifdef PROFILE
   GLdouble t0 = gl_time();
#endif

   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glClear");

   if (MESA_VERBOSE & VERBOSE_API)
      fprintf(stderr, "glClear 0x%x\n", mask);

   if (ctx->RenderMode==GL_RENDER) {
      GLint x, y, width, height;
      GLbitfield newMask;

      if (ctx->NewState) {
         gl_update_state( ctx );
      }

      x = ctx->Buffer->Xmin;
      y = ctx->Buffer->Ymin;
      height = ctx->Buffer->Ymax - ctx->Buffer->Ymin + 1;
      width  = ctx->Buffer->Xmax - ctx->Buffer->Xmin + 1;

      /* clear software alpha buffer(s) */
      if ( (mask & GL_COLOR_BUFFER_BIT) && (ctx->RasterMask & ALPHABUF_BIT) ) {
         gl_clear_alpha_buffers( ctx );
      }

      /* let device driver try to clear the buffers */
      if ((mask & GL_COLOR_BUFFER_BIT) != 0
          && (ctx->Color.SWmasking || (ctx->RasterMask & MULTI_DRAW_BIT))) {
         /* Driver can't clear color buffer for some reason, let it try
          * to clear the other ancillary buffers.
          */
         GLbitfield mask2 = mask & (~GL_COLOR_BUFFER_BIT);
         newMask = (*ctx->Driver.Clear)( ctx, mask2, !ctx->Scissor.Enabled,
                                         x, y, width, height );
         newMask |= GL_COLOR_BUFFER_BIT;
      }
      else {
         newMask = (*ctx->Driver.Clear)( ctx, mask, !ctx->Scissor.Enabled,
                                         x, y, width, height );
      }

      if (newMask & GL_COLOR_BUFFER_BIT)    clear_color_buffers( ctx );
      if (newMask & GL_DEPTH_BUFFER_BIT)    gl_clear_depth_buffer( ctx );
      if (newMask & GL_ACCUM_BUFFER_BIT)    gl_clear_accum_buffer( ctx );
      if (newMask & GL_STENCIL_BUFFER_BIT)  gl_clear_stencil_buffer( ctx );

#ifdef PROFILE
      ctx->ClearTime += gl_time() - t0;
      ctx->ClearCount++;
#endif
   }
}



void gl_Finish( GLcontext *ctx )
{
   /* Don't compile into display list */
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glFinish");
   if (ctx->Driver.Finish) {
      (*ctx->Driver.Finish)( ctx );
   }
}



void gl_Flush( GLcontext *ctx )
{
   /* Don't compile into display list */
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glFlush");
   if (ctx->Driver.Flush) {
      (*ctx->Driver.Flush)( ctx );
   }
}



GLboolean gl_Hint( GLcontext *ctx, GLenum target, GLenum mode )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH_WITH_RETVAL(ctx, "glHint", GL_FALSE);

   if (MESA_VERBOSE & VERBOSE_API)
      fprintf(stderr, "glHint %s %d\n", gl_lookup_enum_by_nr(target), mode);

   switch (target) {
   case GL_FOG_HINT:
      ctx->Hint.Fog = mode;
      break;
   case GL_LINE_SMOOTH_HINT:
      ctx->Hint.LineSmooth = mode;
      break;
   case GL_PERSPECTIVE_CORRECTION_HINT:
      ctx->Hint.PerspectiveCorrection = mode;
      break;
   case GL_POINT_SMOOTH_HINT:
      ctx->Hint.PointSmooth = mode;
      break;
   case GL_POLYGON_SMOOTH_HINT:
      ctx->Hint.PolygonSmooth = mode;
      break;
   case GL_PREFER_DOUBLEBUFFER_HINT_PGI:
   case GL_STRICT_DEPTHFUNC_HINT_PGI:
      break;
   case GL_STRICT_LIGHTING_HINT_PGI:
      ctx->Hint.StrictLighting = mode;
      break;
   case GL_STRICT_SCISSOR_HINT_PGI:
   case GL_FULL_STIPPLE_HINT_PGI:
   case GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI:
   case GL_NATIVE_GRAPHICS_END_HINT_PGI:
   case GL_CONSERVE_MEMORY_HINT_PGI:
   case GL_RECLAIM_MEMORY_HINT_PGI:
      break;
   case GL_ALWAYS_FAST_HINT_PGI:
      if (mode) {
	 ctx->Hint.AllowDrawWin = GL_TRUE;
	 ctx->Hint.AllowDrawSpn = GL_FALSE;
	 ctx->Hint.AllowDrawMem = GL_FALSE;
      } else {
	 ctx->Hint.AllowDrawWin = GL_TRUE;
	 ctx->Hint.AllowDrawSpn = GL_TRUE;
	 ctx->Hint.AllowDrawMem = GL_TRUE;
      }
      break;
   case GL_ALWAYS_SOFT_HINT_PGI:
      ctx->Hint.AllowDrawWin = GL_TRUE;
      ctx->Hint.AllowDrawSpn = GL_TRUE;
      ctx->Hint.AllowDrawMem = GL_TRUE;
      break;
   case GL_ALLOW_DRAW_OBJ_HINT_PGI:
      break;
   case GL_ALLOW_DRAW_WIN_HINT_PGI:
      ctx->Hint.AllowDrawWin = mode;
      break;
   case GL_ALLOW_DRAW_SPN_HINT_PGI:
      ctx->Hint.AllowDrawSpn = mode;
      break;
   case GL_ALLOW_DRAW_MEM_HINT_PGI:
      ctx->Hint.AllowDrawMem = mode;
      break;
   case GL_CLIP_NEAR_HINT_PGI:
   case GL_CLIP_FAR_HINT_PGI:
   case GL_WIDE_LINE_HINT_PGI:
   case GL_BACK_NORMALS_HINT_PGI:
   case GL_NATIVE_GRAPHICS_HANDLE_PGI:
      break;

      /* GL_EXT_clip_volume_hint */
   case GL_CLIP_VOLUME_CLIPPING_HINT_EXT:
      ctx->Hint.ClipVolumeClipping = mode;
      break;

   default:
      gl_error( ctx, GL_INVALID_ENUM, "glHint(target)" );
      return GL_FALSE;
   }
   ctx->NewState |= NEW_ALL;   /* just to be safe */

   if (ctx->Driver.Hint) {
      (*ctx->Driver.Hint)( ctx, target, mode );
   }

   return GL_TRUE;
}



void gl_DrawBuffer( GLcontext *ctx, GLenum mode )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glDrawBuffer");

   if (MESA_VERBOSE & VERBOSE_API)
      fprintf(stderr, "glDrawBuffer %s\n", gl_lookup_enum_by_nr(mode));

   switch (mode) {
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3:
         /* AUX buffers not implemented in Mesa at this time */
         gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
         return;
      case GL_RIGHT:
         if (!ctx->Visual->StereoFlag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
            return;
         }
         if (ctx->Visual->DBflag)
            ctx->Color.DrawDestMask = FRONT_RIGHT_BIT | BACK_RIGHT_BIT;
         else
            ctx->Color.DrawDestMask = FRONT_RIGHT_BIT;
         break;
      case GL_FRONT_RIGHT:
         if (!ctx->Visual->StereoFlag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
            return;
         }
         ctx->Color.DrawDestMask = FRONT_RIGHT_BIT;
         break;
      case GL_BACK_RIGHT:
         if (!ctx->Visual->StereoFlag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
            return;
         }
         if (!ctx->Visual->DBflag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
            return;
         }
         ctx->Color.DrawDestMask = BACK_RIGHT_BIT;
         break;
      case GL_BACK_LEFT:
         if (!ctx->Visual->DBflag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
            return;
         }
         ctx->Color.DrawDestMask = BACK_LEFT_BIT;
         break;
      case GL_FRONT_AND_BACK:
         if (!ctx->Visual->DBflag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
            return;
         }
         if (ctx->Visual->StereoFlag)
            ctx->Color.DrawDestMask = FRONT_LEFT_BIT | BACK_LEFT_BIT
                                    | FRONT_RIGHT_BIT | BACK_RIGHT_BIT;
         else
            ctx->Color.DrawDestMask = FRONT_LEFT_BIT | BACK_LEFT_BIT;
         break;
      case GL_BACK:
         if (!ctx->Visual->DBflag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glDrawBuffer" );
            return;
         }
         if (ctx->Visual->StereoFlag)
            ctx->Color.DrawDestMask = BACK_LEFT_BIT | BACK_RIGHT_BIT;
         else
            ctx->Color.DrawDestMask = BACK_LEFT_BIT;
         break;
      case GL_LEFT:
         /* never an error */
         if (ctx->Visual->DBflag)
            ctx->Color.DrawDestMask = FRONT_LEFT_BIT | BACK_LEFT_BIT;
         else
            ctx->Color.DrawDestMask = FRONT_LEFT_BIT;
         break;
      case GL_FRONT_LEFT:
         /* never an error */
         ctx->Color.DrawDestMask = FRONT_LEFT_BIT;
         break;
      case GL_FRONT:
         /* never an error */
         if (ctx->Visual->StereoFlag)
            ctx->Color.DrawDestMask = FRONT_LEFT_BIT | FRONT_RIGHT_BIT;
         else
            ctx->Color.DrawDestMask = FRONT_LEFT_BIT;
         break;
      case GL_NONE:
         /* never an error */
         ctx->Color.DrawDestMask = 0;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glDrawBuffer" );
         return;
   }

   /*
    * Make the dest buffer mode more precise if possible
    */
   if (mode == GL_LEFT && !ctx->Visual->DBflag)
      ctx->Color.DriverDrawBuffer = GL_FRONT_LEFT;
   else if (mode == GL_RIGHT && !ctx->Visual->DBflag)
      ctx->Color.DriverDrawBuffer = GL_FRONT_RIGHT;
   else if (mode == GL_FRONT && !ctx->Visual->StereoFlag)
      ctx->Color.DriverDrawBuffer = GL_FRONT_LEFT;
   else if (mode == GL_BACK && !ctx->Visual->StereoFlag)
      ctx->Color.DriverDrawBuffer = GL_BACK_LEFT;
   else
      ctx->Color.DriverDrawBuffer = mode;

   /*
    * Set current alpha buffer pointer
    */
   if (ctx->Visual->SoftwareAlpha) {
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
    * If we get here there can't have been an error.
    * Now see if device driver can implement the drawing to the target
    * buffer(s).  The driver may not be able to do GL_FRONT_AND_BACK mode
    * for example.  We'll take care of that in the core code by looping
    * over the individual buffers.
    */
   ASSERT(ctx->Driver.SetBuffer);
   if ( (*ctx->Driver.SetBuffer)(ctx, ctx->Color.DriverDrawBuffer) ) {
      /* All OK, the driver will do all buffer writes */
      ctx->Color.MultiDrawBuffer = GL_FALSE;
   }
   else {
      /* We'll have to loop over the multiple draw buffer targets */
      ctx->Color.MultiDrawBuffer = GL_TRUE;
      /* Set drawing buffer to front for now */
      (*ctx->Driver.SetBuffer)(ctx, GL_FRONT_LEFT);
   }

   ctx->Color.DrawBuffer = mode;
   ctx->NewState |= NEW_RASTER_OPS;
}



void gl_ReadBuffer( GLcontext *ctx, GLenum mode )
{
   ASSERT_OUTSIDE_BEGIN_END_AND_FLUSH(ctx, "glReadBuffer");

   if (MESA_VERBOSE & VERBOSE_API)
      fprintf(stderr, "glReadBuffer %s\n", gl_lookup_enum_by_nr(mode));

   switch (mode) {
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3:
         /* AUX buffers not implemented in Mesa at this time */
         gl_error( ctx, GL_INVALID_OPERATION, "glReadBuffer" );
         return;
      case GL_LEFT:
      case GL_FRONT:
      case GL_FRONT_LEFT:
         /* Front-Left buffer, always exists */
         ctx->Pixel.DriverReadBuffer = GL_FRONT_LEFT;
         break;
      case GL_BACK:
      case GL_BACK_LEFT:
         /* Back-Left buffer, requires double buffering */
         if (!ctx->Visual->DBflag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glReadBuffer" );
            return;
         }
         ctx->Pixel.DriverReadBuffer = GL_BACK_LEFT;
         break;
      case GL_FRONT_RIGHT:
      case GL_RIGHT:
         if (!ctx->Visual->StereoFlag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glReadBuffer" );
            return;
         }
         ctx->Pixel.DriverReadBuffer = GL_FRONT_RIGHT;
         break;
      case GL_BACK_RIGHT:
         if (!ctx->Visual->StereoFlag || !ctx->Visual->DBflag) {
            gl_error( ctx, GL_INVALID_OPERATION, "glReadBuffer" );
            return;
         }
         ctx->Pixel.DriverReadBuffer = GL_BACK_RIGHT;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glReadBuffer" );
         return;
   }

   ctx->Pixel.ReadBuffer = mode;
   ctx->NewState |= NEW_RASTER_OPS;
}
