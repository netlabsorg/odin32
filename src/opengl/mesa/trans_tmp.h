/* $Id: trans_tmp.h,v 1.1 2000-02-29 00:48:40 sandervl Exp $ */

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
 * New (3.1) transformation code written by Keith Whitwell.
 */


/* KW: It might be possible to organize to do the translation *after*
 *     culling in an array-optimized gl_transform_vb().
 */


#ifdef DEST_4F
static void DEST_4F( GLfloat (*t)[4],
		     const struct gl_client_array *from,
		     ARGS)
{
   GLuint stride = from->StrideB;
   const GLubyte *f = (GLubyte *) from->Ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;

   (void) first;
   (void) start;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 if (SZ >= 1) t[i][0] = TRX_4F(f, 0);
	 if (SZ >= 2) t[i][1] = TRX_4F(f, 1);
	 if (SZ >= 3) t[i][2] = TRX_4F(f, 2);
	 if (SZ == 4) t[i][3] = TRX_4F(f, 3);
      }
   }
}
#endif


#ifdef DEST_3F
static void DEST_3F( GLfloat (*t)[3],
		     const struct gl_client_array *from,
		     ARGS)
{
   GLuint stride = from->StrideB;
   const GLubyte *f = (GLubyte *) from->Ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) first;
   (void) start;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 t[i][0] = TRX_3F(f, 0);
	 t[i][1] = TRX_3F(f, 1);
	 t[i][2] = TRX_3F(f, 2);
      }
   }
}
#endif

#ifdef DEST_4UB
static void DEST_4UB( GLubyte (*t)[4],
		      const struct gl_client_array *from,
		      ARGS)
{
   GLuint stride = from->StrideB;
   const GLubyte *f = (GLubyte *) from->Ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) start;
   (void) first;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 if (SZ >= 1) TRX_UB(t[i][0], f, 0);
	 if (SZ >= 2) TRX_UB(t[i][1], f, 1);
	 if (SZ >= 3) TRX_UB(t[i][2], f, 2);
	 if (SZ == 4) TRX_UB(t[i][3], f, 3); else t[i][3] = 255;
      }
   }
}
#endif


#ifdef DEST_1UB
static void DEST_1UB( GLubyte *t,
		      const struct gl_client_array *from,
		      ARGS)
{
   GLuint stride = from->StrideB;
   const GLubyte *f = (GLubyte *) from->Ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) start;
   (void) first;
   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	  TRX_UB(t[i], f, 0);
      }
   }
}
#endif


#ifdef DEST_1UI
static void DEST_1UI( GLuint *t,
		      const struct gl_client_array *from,
		      ARGS)
{
   GLuint stride = from->StrideB;
   const GLubyte *f = (GLubyte *) from->Ptr + SRC_START * stride;
   const GLubyte *first = f;
   GLuint i;
   (void) start;
   (void) first;

   for (i = DST_START ; i < n ; i++, NEXT_F) {
      CHECK {
         NEXT_F2;
	 t[i] = TRX_UI(f, 0);
      }
   }
}
#endif


static void INIT(void)
{
#ifdef DEST_1UI
   ASSERT(SZ == 1);
   TAB(1ui)[SRC_IDX] = DEST_1UI;
#endif
#ifdef DEST_1UB
   ASSERT(SZ == 1);
   TAB(1ub)[SRC_IDX] = DEST_1UB;
#endif
#ifdef DEST_3F
   ASSERT(SZ == 3);
   TAB(3f)[SRC_IDX] = DEST_3F;
#endif
#ifdef DEST_4UB
   TAB(4ub)[SZ][SRC_IDX] = DEST_4UB;
#endif
#ifdef DEST_4F
   TAB(4f)[SZ][SRC_IDX] = DEST_4F;
#endif

}


#undef INIT
#undef DEST_1UI
#undef DEST_1UB
#undef DEST_4UB
#undef DEST_3F
#undef DEST_4F
#undef SZ
#undef TAG


