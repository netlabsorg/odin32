/* $Id: vbfill.c,v 1.1 2000-02-29 00:50:14 sandervl Exp $ */

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
#include <math.h>
#include <stdio.h>
#else
#include "GL/xf86glx.h"
#endif
#include "context.h"
#include "enums.h"
#include "light.h"
#include "macros.h"
#include "matrix.h"
#include "mmath.h"
#include "types.h"
#include "varray.h"
#include "vb.h"
#include "vbcull.h"
#include "vbfill.h"
#include "vbrender.h"
#include "vbxform.h"
#include "xform.h"
#endif



/* KW: The work of most of the functions which were in this file are
 *     now done directly by the GLVertex, GLTexCoord, GLIndex, ...,
 *     functions in api{1,2}.c.  This is made possible by the fact
 *     that display lists and vertex buffers are now built the same
 *     way, so there is no need for the indirection and overhead of a
 *     function pointer.
 */


void gl_Begin( GLcontext *ctx, GLenum p )
{
   struct immediate *IM = ctx->input;
   GLuint inflags, state;

   if (MESA_VERBOSE&VERBOSE_API) {
      if (MESA_VERBOSE&VERBOSE_IMMEDIATE)
	 fprintf(stderr, "glBegin(IM %d) %s\n", IM->id, gl_lookup_enum_by_nr(p));
      else
	 fprintf(stderr, "<");
   }

   if (ctx->NewState)
      gl_update_state( ctx );	/* should already be flushed */

   /* if only a very few slots left, might as well flush now
    */
   if (IM->Count > VB_MAX-4) {	
      IM->maybe_transform_vb( IM );
      IM = ctx->input;
   }

   state = IM->BeginState;
   inflags = state & (VERT_BEGIN_0|VERT_BEGIN_1);
   state |= inflags << 2;	/* set error conditions */

   if (MESA_VERBOSE&VERBOSE_API)
      fprintf(stderr, "in gl_Begin(IM %d), BeginState is %x, errors %x",
	      IM->id,
	      state,
	      inflags<<2);

   if (inflags != (VERT_BEGIN_0|VERT_BEGIN_1))
   {
      GLuint count = IM->Count;
      GLuint last = IM->LastPrimitive;

      state |= (VERT_BEGIN_0|VERT_BEGIN_1);
      IM->Flag[count] |= VERT_BEGIN;
      IM->Primitive[count] = p;

      IM->NextPrimitive[IM->LastPrimitive] = count;
      IM->LastPrimitive = count;

      if (IM->FlushElt) {
	 gl_exec_array_elements( ctx, IM, last, count );
	 IM->FlushElt = 0;
      }
   }

   if (MESA_VERBOSE&VERBOSE_API)
      fprintf(stderr, "in gl_Begin final state %x\n", state);

   IM->BeginState = state;
}



