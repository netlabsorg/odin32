/* $Id: config.h,v 1.2 2000-03-01 18:49:24 jeroen Exp $ */

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
 * Tunable configuration parameters.
 */



#ifndef CONFIG_H
#define CONFIG_H

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

/*
 *
 * OpenGL implementation limits
 *
 */


/* Maximum modelview matrix stack depth: */
#define MAX_MODELVIEW_STACK_DEPTH 32

/* Maximum projection matrix stack depth: */
#define MAX_PROJECTION_STACK_DEPTH 32

/* Maximum texture matrix stack depth: */
#define MAX_TEXTURE_STACK_DEPTH 10

/* Maximum attribute stack depth: */
#define MAX_ATTRIB_STACK_DEPTH 16

/* Maximum client attribute stack depth: */
#define MAX_CLIENT_ATTRIB_STACK_DEPTH 16

/* Maximum recursion depth of display list calls: */
#define MAX_LIST_NESTING 64

/* Maximum number of lights: */
#define MAX_LIGHTS 8

/* Maximum user-defined clipping planes: */
#define MAX_CLIP_PLANES 6

/* Maximum pixel map lookup table size: */
#define MAX_PIXEL_MAP_TABLE 256

/* Number of auxillary color buffers: */
#define NUM_AUX_BUFFERS 0

/* Maximum order (degree) of curves: */
#ifdef AMIGA
#   define MAX_EVAL_ORDER 12
#else
#   define MAX_EVAL_ORDER 30
#endif

/* Maximum Name stack depth */
#define MAX_NAME_STACK_DEPTH 64

/* Min and Max point sizes and granularity */
#define MIN_POINT_SIZE 1.0
#define MAX_POINT_SIZE 10.0
#define POINT_SIZE_GRANULARITY 0.1

/* Min and Max line widths and granularity */
#define MIN_LINE_WIDTH 1.0
#define MAX_LINE_WIDTH 10.0
#define LINE_WIDTH_GRANULARITY 1.0

/* Max texture palette size */
#define MAX_TEXTURE_PALETTE_SIZE 256

/* Number of texture levels */
#define MAX_TEXTURE_LEVELS 12

/* Number of texture units - GL_ARB_multitexture */
#define MAX_TEXTURE_UNITS 2

/* Maximum viewport size: */
#define MAX_WIDTH 1600
#define MAX_HEIGHT 1200

/* Maxmimum size for CVA.  May be overridden by the drivers.  */
#define MAX_ARRAY_LOCK_SIZE 3000


/*
 *
 * Mesa-specific parameters
 *
 */


/*
 * Bits per accumulation buffer color component:  8 or 16
 */
#define ACCUM_BITS 16


#ifdef MESAD3D
   /* Mesa / Direct3D driver only */
   extern float g_DepthScale, g_MaxDepth;
#  define DEPTH_BITS    32
#  define DEPTH_SCALE   g_DepthScale
#  define MAX_DEPTH     g_MaxDepth
#else
   /*
    * Bits per depth buffer value:  16 or 32
    */
#  define DEPTH_BITS 16
#  if DEPTH_BITS==16
#     define MAX_DEPTH 0xffff
#     define DEPTH_SCALE 65535.0F
#  elif DEPTH_BITS==32
#     define MAX_DEPTH 0x3fffffff
#     define DEPTH_SCALE ((GLfloat) MAX_DEPTH)
#  else
#     error "illegal number of depth bits"
#  endif
#endif


/*
 * Bits per stencil value:  8
 */
#define STENCIL_BITS 8


/*
 * Bits per color channel (must be 8 at this time!)
 */
#define CHAN_BITS 8



/*
 * Color channel component order
 * (changes will almost certainly cause problems at this time)
 */
#define RCOMP 0
#define GCOMP 1
#define BCOMP 2
#define ACOMP 3



/* Vertex buffer size.  KW: no restrictions on the divisibility of
 * this number, though things may go better for you if you choose a
 * value of 12n + 3.
 */

#define VB_START  3

#if defined(FX) && !defined(MITS)
#  define VB_MAX 72 + VB_START  /* better performance */
#else
#  define VB_MAX 480 + VB_START
#endif

/*
 * Actual vertex buffer size.
 *
 * Arrays must also accomodate new vertices from clipping, and
 * potential overflow from primitives which don't fit into neatly into
 * VB_MAX vertices.  (This only happens when mixed primitives are
 * sharing the vb).
 */
#define VB_MAX_CLIPPED_VERTS (2 * (6 + MAX_CLIP_PLANES))
#define VB_SIZE  (VB_MAX + VB_MAX_CLIPPED_VERTS)


/*
 *
 * For X11 driver only:
 *
 */

/*
 * When defined, use 6x6x6 dithering instead of 5x9x5.
 * 5x9x5 better for general colors, 6x6x6 better for grayscale.
 */
/*#define DITHER666*/



typedef struct gl_context GLcontext;
typedef struct gl_visual  GLvisual;
typedef struct gl_frame_buffer GLframebuffer;

extern void gl_read_config_file( struct gl_context *ctx );

#endif
