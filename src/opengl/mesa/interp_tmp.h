/* $Id: interp_tmp.h,v 1.1 2000-02-29 00:48:32 sandervl Exp $ */

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


static void NAME( struct vertex_buffer *VB,
		  GLuint dst, GLfloat t, GLuint in, GLuint out )
{
#if (IND & (CLIP_RGBA0|CLIP_FOG_COORD))
   GLfloat col[3][4];
#endif

   (void) VB;
   (void) dst;
   (void) t;
   (void) in;
   (void) out;

#if (IND & CLIP_RGBA0)
   UBYTE_RGBA_TO_FLOAT_RGBA(col[1], VB->Color[0]->data[in]);
   UBYTE_RGBA_TO_FLOAT_RGBA(col[2], VB->Color[0]->data[out]);
   INTERP_SZ( t, col, 0, 1, 2, 4 );
   FLOAT_RGBA_TO_UBYTE_RGBA(VB->Color[0]->data[dst], col[0]);
#endif

#if (IND & CLIP_RGBA1)
   if (VB->ctx->TriangleCaps & DD_TRI_LIGHT_TWOSIDE) {
      UBYTE_RGBA_TO_FLOAT_RGBA(col[1], VB->Color[1]->data[in]);
      UBYTE_RGBA_TO_FLOAT_RGBA(col[2], VB->Color[1]->data[out]);
      INTERP_SZ( t, col, 0, 1, 2, 4 );
      FLOAT_RGBA_TO_UBYTE_RGBA(VB->Color[1]->data[dst], col[0]);
   }

   if (VB->ctx->TriangleCaps & DD_SEPERATE_SPECULAR)
   {
      UBYTE_RGBA_TO_FLOAT_RGBA(col[1], VB->Spec[0][in]);
      UBYTE_RGBA_TO_FLOAT_RGBA(col[2], VB->Spec[0][out]);
      INTERP_SZ( t, col, 0, 1, 2, 3 );
      FLOAT_RGBA_TO_UBYTE_RGBA(VB->Spec[0][dst], col[0]);

      if (VB->ctx->TriangleCaps & DD_TRI_LIGHT_TWOSIDE) {
	 UBYTE_RGBA_TO_FLOAT_RGBA(col[1], VB->Spec[1][in]);
	 UBYTE_RGBA_TO_FLOAT_RGBA(col[2], VB->Spec[1][out]);
	 INTERP_SZ( t, col, 0, 1, 2, 3 );
	 FLOAT_RGBA_TO_UBYTE_RGBA(VB->Spec[1][dst], col[0]);
      }
   }
#endif

#if (IND & CLIP_FOG_COORD)
   col[0][0] = UBYTE_COLOR_TO_FLOAT_COLOR( VB->Spec[0][in][3]);
   col[0][1] = UBYTE_COLOR_TO_FLOAT_COLOR( VB->Spec[0][out][3]);
   col[0][2] = LINTERP( t, col[0][0], col[0][1] );
   FLOAT_COLOR_TO_UBYTE_COLOR(VB->Spec[0][dst][3], col[0][2]);
#endif


#if (IND & CLIP_INDEX0)
   VB->IndexPtr->data[dst] = (GLuint) (GLint)
      LINTERP( t,
	       (GLfloat) VB->Index[0]->data[in],
	       (GLfloat) VB->Index[0]->data[out] );
#endif

#if (IND & CLIP_INDEX1)
   VB->Index[1]->data[dst] = (GLuint) (GLint)
      LINTERP( t,
	       (GLfloat) VB->Index[1]->data[in],
	       (GLfloat) VB->Index[1]->data[out] );
#endif

#if (IND & CLIP_TEX0)
   INTERP_SZ( t,
	      VB->TexCoordPtr[0]->data,
	      dst, in, out,
	      VB->TexCoordPtr[0]->size );
#endif

#if (IND & CLIP_TEX1)
   INTERP_SZ( t,
	      VB->TexCoordPtr[1]->data,
	      dst, in, out,
	      VB->TexCoordPtr[1]->size );
#endif
}


#undef IND
#undef NAME
