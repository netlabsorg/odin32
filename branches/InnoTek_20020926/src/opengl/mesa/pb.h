/* $Id: pb.h,v 1.2 2000-05-23 20:34:54 jeroen Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.3
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





#ifndef PB_H
#define PB_H


#include "types.h"


/*
 * Pixel buffer size, must be larger than MAX_WIDTH.
 */
#define PB_SIZE (3*MAX_WIDTH)


struct pixel_buffer {
        GLint x[PB_SIZE];         /* X window coord in [0,MAX_WIDTH) */
        GLint y[PB_SIZE];         /* Y window coord in [0,MAX_HEIGHT) */
        GLdepth z[PB_SIZE];       /* Z window coord in [0,Visual.MaxDepth] */
        GLubyte rgba[PB_SIZE][4]; /* Colors */
        GLubyte spec[PB_SIZE][3]; /* Separate specular colors */
        GLuint i[PB_SIZE];        /* Index */
        GLfloat s[MAX_TEXTURE_UNITS][PB_SIZE];  /* Texture S coordinates */
        GLfloat t[MAX_TEXTURE_UNITS][PB_SIZE];  /* Texture T coordinates */
        GLfloat u[MAX_TEXTURE_UNITS][PB_SIZE];  /* Texture R coordinates */
        GLfloat lambda[MAX_TEXTURE_UNITS][PB_SIZE];/* Texture lambda values */
        GLint color[4];         /* Mono color, integers! */
        GLuint index;           /* Mono index */
        GLuint count;           /* Number of pixels in buffer */
        GLboolean mono;         /* Same color or index for all pixels? */
        GLenum primitive;       /* GL_POINT, GL_LINE, GL_POLYGON or GL_BITMAP*/
};



/*
 * Set the color used for all subsequent pixels in the buffer.
 */
#define PB_SET_COLOR( CTX, PB, R, G, B, A )                     \
        if ((PB)->color[RCOMP]!=(R) || (PB)->color[GCOMP]!=(G)  \
         || (PB)->color[BCOMP]!=(B) || (PB)->color[ACOMP]!=(A)  \
         || !(PB)->mono) {                                      \
                gl_flush_pb( ctx );                             \
        }                                                       \
        (PB)->color[RCOMP] = R;                                 \
        (PB)->color[GCOMP] = G;                                 \
        (PB)->color[BCOMP] = B;                                 \
        (PB)->color[ACOMP] = A;                                 \
        (PB)->mono = GL_TRUE;


/*
 * Set the color index used for all subsequent pixels in the buffer.
 */
#define PB_SET_INDEX( CTX, PB, I )              \
        if ((PB)->index!=(I) || !(PB)->mono) {  \
                gl_flush_pb( CTX );             \
        }                                       \
        (PB)->index = I;                        \
        (PB)->mono = GL_TRUE;


/*
 * "write" a pixel using current color or index
 */
#define PB_WRITE_PIXEL( PB, X, Y, Z )           \
        (PB)->x[(PB)->count] = X;               \
        (PB)->y[(PB)->count] = Y;               \
        (PB)->z[(PB)->count] = Z;               \
        (PB)->count++;


/*
 * "write" an RGBA pixel
 */
#define PB_WRITE_RGBA_PIXEL( PB, X, Y, Z, R, G, B, A )  \
        (PB)->x[(PB)->count] = X;                       \
        (PB)->y[(PB)->count] = Y;                       \
        (PB)->z[(PB)->count] = Z;                       \
        (PB)->rgba[(PB)->count][RCOMP] = R;             \
        (PB)->rgba[(PB)->count][GCOMP] = G;             \
        (PB)->rgba[(PB)->count][BCOMP] = B;             \
        (PB)->rgba[(PB)->count][ACOMP] = A;             \
        (PB)->count++;

/*
 * "write" a color-index pixel
 */
#define PB_WRITE_CI_PIXEL( PB, X, Y, Z, I )     \
        (PB)->x[(PB)->count] = X;               \
        (PB)->y[(PB)->count] = Y;               \
        (PB)->z[(PB)->count] = Z;               \
        (PB)->i[(PB)->count] = I;               \
        (PB)->count++;


/*
 * "write" an RGBA pixel with texture coordinates
 */
#define PB_WRITE_TEX_PIXEL( PB, X, Y, Z, R, G, B, A, S, T, U )  \
        (PB)->x[(PB)->count] = X;                               \
        (PB)->y[(PB)->count] = Y;                               \
        (PB)->z[(PB)->count] = Z;                               \
        (PB)->rgba[(PB)->count][RCOMP] = R;                     \
        (PB)->rgba[(PB)->count][GCOMP] = G;                     \
        (PB)->rgba[(PB)->count][BCOMP] = B;                     \
        (PB)->rgba[(PB)->count][ACOMP] = A;                     \
        (PB)->s[0][(PB)->count] = S;                            \
        (PB)->t[0][(PB)->count] = T;                            \
        (PB)->u[0][(PB)->count] = U;                            \
        (PB)->count++;

/*
 * "write" an RGBA pixel with multiple texture coordinates
 */
#define PB_WRITE_MULTITEX_PIXEL( PB, X, Y, Z, R, G, B, A, S, T, U, S1, T1, U1 ) \
        (PB)->x[(PB)->count] = X;                               \
        (PB)->y[(PB)->count] = Y;                               \
        (PB)->z[(PB)->count] = Z;                               \
        (PB)->rgba[(PB)->count][RCOMP] = R;                     \
        (PB)->rgba[(PB)->count][GCOMP] = G;                     \
        (PB)->rgba[(PB)->count][BCOMP] = B;                     \
        (PB)->rgba[(PB)->count][ACOMP] = A;                     \
        (PB)->s[0][(PB)->count] = S;                            \
        (PB)->t[0][(PB)->count] = T;                            \
        (PB)->u[0][(PB)->count] = U;                            \
        (PB)->s[1][(PB)->count] = S1;                           \
        (PB)->t[1][(PB)->count] = T1;                           \
        (PB)->u[1][(PB)->count] = U1;                           \
        (PB)->count++;

/*
 * "write" an RGBA pixel with multiple texture coordinates and specular color
 */
#define PB_WRITE_MULTITEX_SPEC_PIXEL( PB, X, Y, Z, R, G, B, A,  \
                        SR, SG, SB, S, T, U, S1, T1, U1 )       \
        (PB)->x[(PB)->count] = X;                               \
        (PB)->y[(PB)->count] = Y;                               \
        (PB)->z[(PB)->count] = Z;                               \
        (PB)->rgba[(PB)->count][RCOMP] = R;                     \
        (PB)->rgba[(PB)->count][GCOMP] = G;                     \
        (PB)->rgba[(PB)->count][BCOMP] = B;                     \
        (PB)->rgba[(PB)->count][ACOMP] = A;                     \
        (PB)->spec[(PB)->count][RCOMP] = SR;                    \
        (PB)->spec[(PB)->count][GCOMP] = SG;                    \
        (PB)->spec[(PB)->count][BCOMP] = SB;                    \
        (PB)->s[0][(PB)->count] = S;                            \
        (PB)->t[0][(PB)->count] = T;                            \
        (PB)->u[0][(PB)->count] = U;                            \
        (PB)->s[1][(PB)->count] = S1;                           \
        (PB)->t[1][(PB)->count] = T1;                           \
        (PB)->u[1][(PB)->count] = U1;                           \
        (PB)->count++;



/*
 * Call this function at least every MAX_WIDTH pixels:
 */
#define PB_CHECK_FLUSH( CTX, PB )               \
do {                                            \
        if ((PB)->count>=PB_SIZE-MAX_WIDTH) {   \
           gl_flush_pb( CTX );                  \
        }                                       \
} while(0)

extern struct pixel_buffer *gl_alloc_pb(void);

extern void gl_flush_pb( GLcontext *ctx );

#endif
