/* -*- mode: C; tab-width:8; c-basic-offset:2 -*- */

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
 *
 *
 * Original Mesa / 3Dfx device driver (C) 1999 David Bucciarelli, by the
 * terms stated above.
 *
 * Thank you for your contribution, David!
 *
 * Please make note of the above copyright/license statement.  If you
 * contributed code or bug fixes to this code under the previous (GNU
 * Library) license and object to the new license, your code will be
 * removed at your request.  Please see the Mesa docs/COPYRIGHT file
 * for more information.
 *
 * Additional Mesa/3Dfx driver developers:
 *   Daryll Strauss <daryll@precisioninsight.com>
 *   Keith Whitwell <keith@precisioninsight.com>
 *
 * See fxapi.h for more revision/author details.
 */


static void TAG(fx_tri)(GLcontext *ctx, GLuint e1, GLuint e2, GLuint e3, GLuint pv)
{
   fxMesaContext fxMesa=(fxMesaContext)ctx->DriverCtx;
   struct vertex_buffer *VB=ctx->VB;
   fxVertex *gWin = FX_DRIVER_DATA(VB)->verts;
   GrVertex *v1 = (GrVertex *)gWin[e1].f; 
   GrVertex *v2 = (GrVertex *)gWin[e2].f;
   GrVertex *v3 = (GrVertex *)gWin[e3].f;

   (void) fxMesa;

   if (IND & (FX_TWOSIDE|FX_OFFSET)) 
   {
      GLfloat ex = v1->x - v3->x;
      GLfloat ey = v1->y - v3->y;
      GLfloat fx = v2->x - v3->x;
      GLfloat fy = v2->y - v3->y;
      GLfloat c = ex*fy-ey*fx;
	 
      if (IND & FX_TWOSIDE) {
	 GLuint facing = (c<0.0) ^ ctx->Polygon.FrontBit;
	 GLubyte (*color)[4] = VB->Color[facing]->data;
	 if (IND & FX_FLAT) {
	    FX_VB_COLOR(fxMesa, color[pv]);
	 } else {
	    GOURAUD2(v1,color[e1]); 
	    GOURAUD2(v2,color[e2]); 
	    GOURAUD2(v3,color[e3]); 
	 }
      }
      
      /* Should apply a factor to ac to compensate for different x/y
       * scaling introduced in the Viewport matrix.
       *
       * The driver should supply scaling factors for 'factor' and 'units'.
       */
      if (IND & FX_OFFSET) {
	 GLfloat offset = ctx->Polygon.OffsetUnits;

	 if (c * c > 1e-16) {
	    GLfloat factor = ctx->Polygon.OffsetFactor;
	    GLfloat ez = v1->ooz - v3->ooz;
	    GLfloat fz = v2->ooz - v3->ooz;
	    GLfloat a = ey*fz-ez*fy;
	    GLfloat b = ez*fx-ex*fz;
	    GLfloat ic = 1.0 / c;
	    GLfloat ac = a * ic;
	    GLfloat bc = b * ic;
	    if (ac<0.0F)  ac = -ac;
	    if (bc<0.0F)  bc = -bc;
	    offset += MAX2( ac, bc ) * factor;
	 }
	 /* Probably a lot quicker just to nudge the z values and put
	  * them back afterwards.
	  */
	 FX_grDepthBiasLevel((int)offset);
      }
   }
   else if (IND & FX_FLAT) {
      GLubyte (*color)[4] = VB->Color[0]->data;
      FX_VB_COLOR(fxMesa, color[pv]);
   }

   if (IND & FX_FRONT_BACK) {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  FXFALSE);

      FX_grDepthMask(FXFALSE);
      FX_grRenderBuffer(GR_BUFFER_BACKBUFFER);
   }

   if (IND & FX_ANTIALIAS) 
      FX_grAADrawTriangle(v1, v2, v3, FXTRUE, FXTRUE, FXTRUE);
   else 
      FX_grDrawTriangle(v1, v2, v3);

   /* Might be quicker to do two passes, one for each buffer?
    */
   if (IND & FX_FRONT_BACK) {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  ctx->Color.ColorMask[ACOMP] && fxMesa->haveAlphaBuffer); 

      if(ctx->Depth.Mask) FX_grDepthMask(FXTRUE);

      FX_grRenderBuffer(GR_BUFFER_FRONTBUFFER);

      if (IND & FX_ANTIALIAS)
	 FX_grAADrawTriangle(v1,v2,v3, FXTRUE,FXTRUE,FXTRUE);
      else
	 FX_grDrawTriangle(v1, v2, v3);
   }   
}


/* Not worth the space?
 */
static void TAG(fx_quad)(GLcontext *ctx, GLuint e1, GLuint e2, GLuint e3,
			 GLuint e4, GLuint pv)
{
   fxMesaContext fxMesa=(fxMesaContext)ctx->DriverCtx;
   struct vertex_buffer *VB=ctx->VB;
   fxVertex *gWin = FX_DRIVER_DATA(VB)->verts;
   GrVertex *v1 = (GrVertex *)gWin[e1].f; 
   GrVertex *v2 = (GrVertex *)gWin[e2].f;
   GrVertex *v3 = (GrVertex *)gWin[e3].f;
   GrVertex *v4 = (GrVertex *)gWin[e4].f;

   (void) fxMesa;

   if (IND & (FX_TWOSIDE|FX_OFFSET)) 
   {
      GLfloat ex = v3->x - v1->x;
      GLfloat ey = v3->y - v1->y;
      GLfloat fx = v4->x - v2->x;
      GLfloat fy = v4->y - v2->y;
      GLfloat c = ex*fy-ey*fx;
	 
      if (IND & FX_TWOSIDE) {
	 GLuint facing = (c<0.0) ^ ctx->Polygon.FrontBit;
	 GLubyte (*color)[4] = VB->Color[facing]->data;
	 if (IND & FX_FLAT) {
	    FX_VB_COLOR(fxMesa, color[pv]);
	 } else {
	    GOURAUD2(v1,color[e1]); 
	    GOURAUD2(v2,color[e2]); 
	    GOURAUD2(v3,color[e3]); 
	    GOURAUD2(v4,color[e4]); 
	 }
      }
      
      /* Should apply a factor to ac to compensate for different x/y
       * scaling introduced in the Viewport matrix.
       *
       * The driver should supply scaling factors for 'factor' and 'units'.
       */
      if (IND & FX_OFFSET) {
	 GLfloat offset = ctx->Polygon.OffsetUnits;

	 if (c * c > 1e-16) {
	    GLfloat factor = ctx->Polygon.OffsetFactor;
	    GLfloat ez = v3->ooz - v1->ooz;
	    GLfloat fz = v4->ooz - v2->ooz;
	    GLfloat a = ey*fz-ez*fy;
	    GLfloat b = ez*fx-ex*fz;
	    GLfloat ic = 1.0 / c;
	    GLfloat ac = a * ic;
	    GLfloat bc = b * ic;
	    if (ac<0.0F)  ac = -ac;
	    if (bc<0.0F)  bc = -bc;
	    offset += MAX2( ac, bc ) * factor;
	 }
	 /* Probably a lot quicker just to nudge the z values and put
	  * them back afterwards.
	  */
	 FX_grDepthBiasLevel((int)offset);
      }
   }
   else if (IND & FX_FLAT) {
      GLubyte (*color)[4] = VB->Color[0]->data;
      FX_VB_COLOR(fxMesa, color[pv]);
   }

   if (IND & FX_FRONT_BACK) {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  FXFALSE);

      FX_grDepthMask(FXFALSE);
      FX_grRenderBuffer(GR_BUFFER_BACKBUFFER);
   }

   if (IND & FX_ANTIALIAS) {
      FX_grAADrawTriangle(v1, v2, v4, FXTRUE, FXTRUE, FXTRUE);
      FX_grAADrawTriangle(v2, v3, v4, FXTRUE, FXTRUE, FXTRUE);
   } else {
      FX_grDrawTriangle(v1, v2, v4);
      FX_grDrawTriangle(v2, v3, v4);
   }

   /* Might be quicker to do two passes, one for each buffer?
    */
   if (IND & FX_FRONT_BACK) {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  ctx->Color.ColorMask[ACOMP] && fxMesa->haveAlphaBuffer); 

      if(ctx->Depth.Mask) FX_grDepthMask(FXTRUE);

      FX_grRenderBuffer(GR_BUFFER_FRONTBUFFER);

      if (IND & FX_ANTIALIAS) {
	 FX_grAADrawTriangle(v1, v2, v4, FXTRUE, FXTRUE, FXTRUE);
	 FX_grAADrawTriangle(v2, v3, v4, FXTRUE, FXTRUE, FXTRUE);
      } else {
	 FX_grDrawTriangle(v1, v2, v4);
	 FX_grDrawTriangle(v2, v3, v4);
      }
   }   
}




#if (IND & FX_OFFSET) == 0
static void TAG(fx_line)(GLcontext *ctx, GLuint e1, GLuint e2, GLuint pv)
{
   fxMesaContext fxMesa=(fxMesaContext)ctx->DriverCtx;
   struct vertex_buffer *VB=ctx->VB;
   fxVertex *gWin = FX_DRIVER_DATA(VB)->verts;
   GLubyte (* const color)[4] = VB->Color[0]->data;
   GrVertex *v1 = (GrVertex *)gWin[e1].f; 
   GrVertex *v2 = (GrVertex *)gWin[e2].f;

   if (IND & FX_FLAT) 
   {
      FX_VB_COLOR(fxMesa, color[pv]);
      if (IND & FX_ANTIALIAS)
#if FX_USE_PARGB
      {
        GLuint v1argb = v1->argb;
        GLuint v2argb = v2->argb;
	v1->argb = (color[pv][ACOMP] << 24) | (v1argb & 0x00FFFFFF);
	v2->argb = (color[pv][ACOMP] << 24) | (v2argb & 0x00FFFFFF);
      }
#else
	v1->a = v2->a = UBYTE_COLOR_TO_FLOAT_255_COLOR(color[pv][3]);
#endif 
   } 
   else if (IND & FX_TWOSIDE) 
   {
      GOURAUD2(v1,color[e1]); 
      GOURAUD2(v2,color[e2]); 
   }

   if (IND & FX_FRONT_BACK) {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  FXFALSE);
      FX_grDepthMask(FXFALSE);
      FX_grRenderBuffer(GR_BUFFER_BACKBUFFER);
   }

   if (IND & FX_ANTIALIAS)
      FX_grAADrawLine(v1,v2);
   else
      FX_grDrawLine(v1,v2);

   if (IND & FX_FRONT_BACK) 
   {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  ctx->Color.ColorMask[ACOMP] && fxMesa->haveAlphaBuffer); 

      if(ctx->Depth.Mask)
	 FX_grDepthMask(FXTRUE);

      FX_grRenderBuffer(GR_BUFFER_FRONTBUFFER);

      if (IND & FX_ANTIALIAS)
	 FX_grAADrawLine(v1,v2);
      else
	 FX_grDrawLine(v1,v2);
   }
}
#endif


#if (IND & FX_OFFSET) == 0

#if IND & FX_FLAT
# if IND & FX_ANTIALIAS
#if FX_USE_PARGB
#  define FLAT_COLOR(x,y) GET_PA(gWin[i].f) = y[3];
#else
#  define FLAT_COLOR(x,y) gWin[i].f[ACOORD] = y[3]; \
                          FX_VB_COLOR(x, y)
#endif
# else
#  define FLAT_COLOR(x,y) FX_VB_COLOR(x, y)
# endif
#else
#define FLAT_COLOR(x,y)
#endif

#if IND & FX_ANTIALIAS
#define DRAW_POINT(i) FX_grAADrawPoint((GrVertex *)gWin[i].f);
#else
#define DRAW_POINT(i) FX_grDrawPoint((GrVertex *)gWin[i].f);
#endif


static void TAG(fx_points)(GLcontext *ctx, GLuint first, GLuint last)
{
   fxMesaContext fxMesa=(fxMesaContext)ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   fxVertex *gWin = FX_DRIVER_DATA(VB)->verts;
   GLubyte (*color)[4] = VB->ColorPtr->data;
   GLuint i;

   (void) color; (void) fxMesa;

   if (IND & FX_FRONT_BACK) {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  FXFALSE);

      FX_grDepthMask(FXFALSE);
      FX_grRenderBuffer(GR_BUFFER_BACKBUFFER);
   }

   if(!VB->ClipOrMask) {
      for(i=first;i<=last;i++) {
	 FLAT_COLOR(fxMesa, color[i]);
	 DRAW_POINT(i);
      }
   } else {
      for(i=first;i<=last;i++) {
	 if(VB->ClipMask[i]==0) {
	    FLAT_COLOR(fxMesa, color[i]);
	    DRAW_POINT(i);
	 }
      }
   }

   if (IND & FX_FRONT_BACK) {
      FX_grColorMask(ctx->Color.ColorMask[RCOMP] ||
		  ctx->Color.ColorMask[GCOMP] ||
		  ctx->Color.ColorMask[BCOMP],
		  ctx->Color.ColorMask[ACOMP] && fxMesa->haveAlphaBuffer); 
      if(ctx->Depth.Mask)
	 FX_grDepthMask(FXTRUE);
      FX_grRenderBuffer(GR_BUFFER_FRONTBUFFER);


      if(!VB->ClipOrMask) {
	 for(i=first;i<=last;i++) {
	    FLAT_COLOR(fxMesa, color[i]);
	    DRAW_POINT(i);
	 }
      } else {
	 for(i=first;i<=last;i++) {
	    if(VB->ClipMask[i]==0) {
	       FLAT_COLOR(fxMesa, color[i]);
	       DRAW_POINT(i);
	    }
	 }
      }
   }
}

#endif



static void TAG(init)( void )
{
   tri_tab[IND] = TAG(fx_tri);
   quad_tab[IND] = TAG(fx_quad);

#if ((IND & FX_OFFSET) == 0) 
      line_tab[IND] = TAG(fx_line);
      points_tab[IND] = TAG(fx_points);
#else 
      line_tab[IND] = line_tab[IND & ~FX_OFFSET];
      points_tab[IND] = points_tab[IND & ~FX_OFFSET];
#endif
}

#undef IND
#undef TAG
#undef FLAT_COLOR
#undef DRAW_POINT
