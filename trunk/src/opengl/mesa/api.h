/* $Id: api.h,v 1.1 2000-02-29 00:48:24 sandervl Exp $ */

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
/* $XFree86: xc/lib/GL/mesa/src/api.h,v 1.4 1999/06/27 14:07:26 dawes Exp $ */




/*
 * This header contains stuff only included by api1.c, api2.c and apiext.c
 * and cva.c and varray.c
 */


#ifndef API_H
#define API_H


#if defined(GLX_DIRECT_RENDERING) && !defined(XFree86Server) && !defined(GLX_USE_DLOPEN)
#define NEED_MESA_FUNCS_WRAPPED
#include "mesa_api.h"
#endif

/*
 * Single/multiple thread context selection.
 */
#ifdef THREADS

/* Get the context associated with the calling thread */
#define GET_CONTEXT     GLcontext *CC = gl_get_thread_context()
#define GET_IMMEDIATE   struct immediate *IM = (gl_get_thread_context())->input;

#define SET_IMMEDIATE(ctx, im)                                          \
do {                                                                    \
   ctx->input = im;                                                     \
} while (0)

#else

/* CC is a global pointer for all threads in the address space */
#define GET_CONTEXT

/* And so is VB */
#define GET_IMMEDIATE struct immediate *IM = CURRENT_INPUT

#define SET_IMMEDIATE(ctx, im)                                          \
do {                                                                    \
   ctx->input = im;                                                     \
   CURRENT_INPUT = im;                                                  \
} while (0)

#endif /* THREADS */


/* Make sure there's a rendering context. KW: Only do this if compiled
 * debug.
 */
#ifdef DEBUG
#ifdef __WIN32OS2__
#define CHECK_CONTEXT                                                   \
   if (!CC) {                                                           \
      dprintf(("Mesa user error: no rendering context.\n"));            \
      return;                                                           \
   }
#else
#define CHECK_CONTEXT                                                   \
   if (!CC) {                                                           \
      if (getenv("MESA_DEBUG")) {                                       \
         fprintf(stderr,"Mesa user error: no rendering context.\n");    \
      }                                                                 \
      return;                                                           \
   }
#endif

#ifdef __WIN32OS2__
#define CHECK_CONTEXT_RETURN(R)                                         \
   if (!CC) {                                                           \
        dprintf(("Mesa user error: no rendering context.\n"));          \
      }                                                                 \
      return (R);                                                       
#else
#define CHECK_CONTEXT_RETURN(R)                                         \
   if (!CC) {                                                           \
      if (getenv("MESA_DEBUG")) {                                       \
         fprintf(stderr,"Mesa user error: no rendering context.\n");    \
      }                                                                 \
      return (R);                                                       \
   }
#endif
#else
#define CHECK_CONTEXT
#define CHECK_CONTEXT_RETURN(R)
#endif


#if !defined(CTX_ARG)
#define CTX_ARG
#define CTX_VOID
#define CTX_PRM
#define CTX_VPRM
#endif

/*
 * An optimization in a few performance-critical functions.
 */
#ifndef XFree86Server           /* don't think we want this in DRI??? */
#define SHORTCUT
#endif



#endif
