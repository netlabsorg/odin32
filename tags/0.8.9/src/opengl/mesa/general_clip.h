/* $Id: general_clip.h,v 1.1 2000-02-29 00:48:31 sandervl Exp $ */

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



/*
 * Clip against +X
 *
 * The if conditions are known at compile time.
 */
#define PLANE          (CLIP_RIGHT_BIT)
#define INSIDE(K)      (X(K) <= W(K))
#define COMPUTE_INTERSECTION( in, out, new )		\
        dx = X(out)-X(in);				\
        dw = W(out)-W(in);                      	\
        t = (X(in)-W(in)) / (dw-dx);			\
	neww = W(in) + t * dw;				\
	X(new) = neww;					\
	Y(new) = Y(in) + t * (Y(out) - Y(in));		\
	if (SIZE>=3) coord[new][2] = Z(in) + t * (Z(out) - Z(in)); 		\
	if (SIZE==4) coord[new][3] = neww;

   GENERAL_CLIP

#undef INSIDE
#undef PLANE
#undef COMPUTE_INTERSECTION


/*
 * Clip against -X
 */
#define PLANE          (CLIP_LEFT_BIT)
#define INSIDE(K)       (X(K) >= -W(K))
#define COMPUTE_INTERSECTION( in, out, new )		\
        dx = X(out)-X(in);                      	\
        dw = W(out)-W(in);                      	\
        t = -(X(in)+W(in)) / (dw+dx);           	\
	neww = W(in) + t * dw;				\
        X(new) = -neww;					\
        Y(new) = Y(in) + t * (Y(out) - Y(in));		\
        if (SIZE>=3) coord[new][2] = Z(in) + t * (Z(out) - Z(in));		\
        if (SIZE==4) coord[new][3] = neww;

   GENERAL_CLIP

#undef INSIDE
#undef PLANE
#undef COMPUTE_INTERSECTION


/*
 * Clip against +Y
 */
#define PLANE          (CLIP_TOP_BIT)
#define INSIDE(K)       (Y(K) <= W(K))
#define COMPUTE_INTERSECTION( in, out, new )		\
        dy = Y(out)-Y(in);                      	\
        dw = W(out)-W(in);                      	\
        t = (Y(in)-W(in)) / (dw-dy);            	\
	neww = W(in) + t * dw; 				\
        X(new) = X(in) + t * (X(out) - X(in));		\
        Y(new) = neww;					\
        if (SIZE>=3) coord[new][2] = Z(in) + t * (Z(out) - Z(in));		\
        if (SIZE==4) coord[new][3] = neww;

   GENERAL_CLIP

#undef INSIDE
#undef PLANE
#undef COMPUTE_INTERSECTION


/*
 * Clip against -Y
 */
#define PLANE          (CLIP_BOTTOM_BIT)
#define INSIDE(K)       (Y(K) >= -W(K))
#define COMPUTE_INTERSECTION( in, out, new )		\
        dy = Y(out)-Y(in);                      	\
        dw = W(out)-W(in);                      	\
        t = -(Y(in)+W(in)) / (dw+dy);           	\
	neww = W(in) + t * dw;				\
        X(new) = X(in) + t * (X(out) - X(in));		\
        Y(new) = -neww;					\
        if (SIZE>=3) coord[new][2] = Z(in) + t * (Z(out) - Z(in));		\
        if (SIZE==4) coord[new][3] = neww;

   GENERAL_CLIP

#undef INSIDE
#undef PLANE
#undef COMPUTE_INTERSECTION



/*
 * Clip against +Z
 */
#define PLANE          (CLIP_FAR_BIT)
#define INSIDE(K)       (Z(K) <= W(K))
#define COMPUTE_INTERSECTION( in, out, new )		\
	   dz = Z(out)-Z(in);				\
	   dw = W(out)-W(in);				\
	   t = (Z(in)-W(in)) / (dw-dz);			\
	   neww = W(in) + t * dw;			\
	   X(new) = X(in) + t * (X(out) - X(in));	\
	   Y(new) = Y(in) + t * (Y(out) - Y(in));	\
	   coord[new][2] = neww;				\
	   if (SIZE==4) coord[new][3] = neww;

  if (SIZE>=3) {
   GENERAL_CLIP
  }

#undef INSIDE
#undef PLANE
#undef COMPUTE_INTERSECTION


/*
 * Clip against -Z
 */
#define PLANE          (CLIP_NEAR_BIT)
#define INSIDE(K)       (Z(K) >= -W(K))
#define COMPUTE_INTERSECTION( in, out, new )	\
        dz = Z(out)-Z(in);			\
        dw = W(out)-W(in);			\
        t = -(Z(in)+W(in)) / (dw+dz);		\
	neww = W(in) + t * dw;			\
        X(new) = X(in) + t * (X(out) - X(in));	\
        Y(new) = Y(in) + t * (Y(out) - Y(in));	\
        coord[new][2] = -neww;				\
        if (SIZE==4) coord[new][3] = neww;

  if (SIZE>=3) {
   GENERAL_CLIP
  }

#undef INSIDE
#undef PLANE
#undef COMPUTE_INTERSECTION
#undef GENERAL_CLIP


