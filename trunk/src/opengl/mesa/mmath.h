/* $Id: mmath.h,v 1.1 2000-02-29 00:48:34 sandervl Exp $ */

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
/* $XFree86: xc/lib/GL/mesa/src/mmath.h,v 1.2 1999/04/04 00:20:28 dawes Exp $ */





/*
 * Faster arithmetic functions.  If the FAST_MATH preprocessor symbol is
 * defined on the command line (-DFAST_MATH) then we'll use some (hopefully)
 * faster functions for sqrt(), etc.
 */


#ifndef MMATH_H
#define MMATH_H

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

#ifndef XFree86Server
#include <math.h>
#else
#include "GL/xf86glx.h"
#endif

/*
 * Set the x86 FPU control word to guarentee only 32 bits of presision
 * are stored in registers.  Allowing the FPU to store more introduces
 * differences between situations where numbers are pulled out of memory
 * vs. situations where the compiler is able to optimize register usage.
 *
 * In the worst case, we force the compiler to use a memory access to
 * truncate the float, by specifying the 'volatile' keyword.
 */
#if defined(__linux__) && defined(__i386__)
#include <fpu_control.h>

#if !defined(_FPU_SETCW)
#define _FPU_SETCW __setfpucw
typedef unsigned short fpu_control_t;
#endif

#if !defined(_FPU_GETCW)
#define _FPU_GETCW(a) (a) = __fpu_control;
#endif

/* Set it up how we want it.
 */
#if !defined(NO_FAST_MATH)
#define START_FAST_MATH(x)                                      \
   {                                                            \
      static fpu_control_t mask = _FPU_SINGLE | _FPU_MASK_IM    \
            | _FPU_MASK_DM | _FPU_MASK_ZM | _FPU_MASK_OM        \
            | _FPU_MASK_UM | _FPU_MASK_PM;                      \
      _FPU_GETCW( x );                                          \
      _FPU_SETCW( mask );                                       \
   }
#else
#define START_FAST_MATH(x)                      \
   {                                            \
      static fpu_control_t mask = _FPU_DEFAULT; \
      _FPU_GETCW( x );                          \
      _FPU_SETCW( mask );                       \
   }
#endif

/* Put it back how the application had it.
 */
#define END_FAST_MATH(x)                        \
   {                                            \
      _FPU_SETCW( x );                          \
   }

#define HAVE_FAST_MATH

#else
#define START_FAST_MATH(x) (void)(x)
#define END_FAST_MATH(x)   (void)(x)

/* The mac float really is a float, with the same precision as a
 * single precision 387 float.
 */
#if defined(macintosh)
#define HAVE_FAST_MATH
#endif

#endif



/*
 * Float -> Int conversion
 */

#if defined(USE_X86_ASM)
#if defined(__GNUC__) && defined(__i386__)
static __inline__ int FloatToInt(float f)
{
   int r;
   __asm__ ("fistpl %0" : "=m" (r) : "t" (f) : "st");
   return r;
}
#elif  defined(__MSC__) && defined(__WIN32__)
static __inline int FloatToInt(float f)
{
   int r;
   _asm {
     fld f
     fistp r
    }
   return r;
}
#elif defined(__WIN32OS2__)
#define FloatToInt(F) ((int) (F))
#endif
#else
#define FloatToInt(F) ((int) (F))
#endif





/*
 * Square root
 */

extern float gl_sqrt(float x);

#ifdef FAST_MATH
#  define GL_SQRT(X)  gl_sqrt(X)
#else
#  define GL_SQRT(X)  sqrt(X)
#endif



/*
 * Normalize a 3-element vector to unit length.
 */
#define NORMALIZE_3FV( V )			\
do {						\
   GLdouble len = LEN_SQUARED_3FV(V);		\
   if (len > 1e-50) {				\
      len = 1.0 / GL_SQRT(len);			\
      V[0] = (GLfloat) (V[0] * len);		\
      V[1] = (GLfloat) (V[1] * len);		\
      V[2] = (GLfloat) (V[2] * len);		\
   }						\
} while(0)

#define LEN_3FV( V ) (GL_SQRT(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]))

#define LEN_SQUARED_3FV( V ) (V[0]*V[0]+V[1]*V[1]+V[2]*V[2])

/*
 * Optimization for:
 * GLfloat f;
 * GLubyte b = FloatToInt(CLAMP(f, 0, 1) * 255)
 */

#if defined(__i386__) || defined(__sparc__)
#define USE_IEEE
#endif

#if defined(USE_IEEE) && !defined(DEBUG)

#define IEEE_ONE 0x3f7f0000

#define CLAMP_FLOAT_COLOR(f)			\
	do {					\
	   if (*(GLuint *)&f >= IEEE_ONE)	\
	      f = (*(GLint *)&f < 0) ? 0 : 1;	\
	} while(0)

#define CLAMP_FLOAT_COLOR_VALUE(f)              \
    ( (*(GLuint *)&f >= IEEE_ONE)               \
      ? ((*(GLint *)&f < 0) ? 0 : 1)            \
      : f )

/*
 * This function/macro is sensitive to precision.  Test carefully
 * if you change it.
 */
#define FLOAT_COLOR_TO_UBYTE_COLOR(b, f)                        \
        do {                                                    \
           union { GLfloat r; GLuint i; } tmp;                  \
           tmp.r = f;                                           \
           b = ((tmp.i >= IEEE_ONE)                             \
               ? ((GLint)tmp.i < 0) ? (GLubyte)0 : (GLubyte)255 \
               : (tmp.r = tmp.r*(255.0F/256.0F) + 32768.0F,     \
                  (GLubyte)tmp.i));                             \
        } while (0)


#define CLAMPED_FLOAT_COLOR_TO_UBYTE_COLOR(b,f) \
         FLOAT_COLOR_TO_UBYTE_COLOR(b, f)

#else

#define CLAMP_FLOAT_COLOR(f) \
        (void) CLAMP_SELF(f,0,1)

#define CLAMP_FLOAT_COLOR_VALUE(f) \
        CLAMP(f,0,1)

#define FLOAT_COLOR_TO_UBYTE_COLOR(b, f)                        \
        b = ((GLubyte) FloatToInt(CLAMP(f, 0.0F, 1.0F) * 255.0F))

#define CLAMPED_FLOAT_COLOR_TO_UBYTE_COLOR(b,f) \
        b = ((GLubyte) FloatToInt(f * 255.0F))

#endif


extern float gl_ubyte_to_float_color_tab[256];
extern float gl_ubyte_to_float_255_color_tab[256];
#define UBYTE_COLOR_TO_FLOAT_COLOR(c) gl_ubyte_to_float_color_tab[c]

#define UBYTE_COLOR_TO_FLOAT_255_COLOR(c) gl_ubyte_to_float_255_color_tab[c]

#define UBYTE_COLOR_TO_FLOAT_255_COLOR2(f,c) \
    (*(int *)&(f)) = ((int *)gl_ubyte_to_float_255_color_tab)[c]


#define UBYTE_RGBA_TO_FLOAT_RGBA(f,b)           \
do {                                            \
   f[0] = UBYTE_COLOR_TO_FLOAT_COLOR(b[0]);     \
   f[1] = UBYTE_COLOR_TO_FLOAT_COLOR(b[1]);     \
   f[2] = UBYTE_COLOR_TO_FLOAT_COLOR(b[2]);     \
   f[3] = UBYTE_COLOR_TO_FLOAT_COLOR(b[3]);     \
} while(0)


#define UBYTE_RGBA_TO_FLOAT_255_RGBA(f,b)               \
do {                                            \
   f[0] = UBYTE_COLOR_TO_FLOAT_255_COLOR(b[0]); \
   f[1] = UBYTE_COLOR_TO_FLOAT_255_COLOR(b[1]); \
   f[2] = UBYTE_COLOR_TO_FLOAT_255_COLOR(b[2]); \
   f[3] = UBYTE_COLOR_TO_FLOAT_255_COLOR(b[3]); \
} while(0)

#define FLOAT_RGBA_TO_UBYTE_RGBA(b,f)           \
do {                                            \
   FLOAT_COLOR_TO_UBYTE_COLOR((b[0]),(f[0]));   \
   FLOAT_COLOR_TO_UBYTE_COLOR((b[1]),(f[1]));   \
   FLOAT_COLOR_TO_UBYTE_COLOR((b[2]),(f[2]));   \
   FLOAT_COLOR_TO_UBYTE_COLOR((b[3]),(f[3]));   \
} while(0)

#define FLOAT_RGB_TO_UBYTE_RGB(b,f)             \
do {                                            \
   FLOAT_COLOR_TO_UBYTE_COLOR(b[0],f[0]);       \
   FLOAT_COLOR_TO_UBYTE_COLOR(b[1],f[1]);       \
   FLOAT_COLOR_TO_UBYTE_COLOR(b[2],f[2]);       \
} while(0)


extern void gl_init_math(void);


#endif
