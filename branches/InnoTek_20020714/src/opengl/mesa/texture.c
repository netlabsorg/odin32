/* $Id: texture.c,v 1.3 2000-05-23 20:40:57 jeroen Exp $ */

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


#ifdef PC_HEADER
#include "all.h"
#else
#include "glheader.h"
#include "types.h"
#include "context.h"
#include "macros.h"
#include "mmath.h"
#include "pb.h"
#include "texture.h"
#include "xform.h"
#endif

/***********************************************************************
 * Automatic texture coordinate generation (texgen) code.
 */

static GLuint all_bits[5] = {
   0,
   VEC_SIZE_1,
   VEC_SIZE_2,
   VEC_SIZE_3,
   VEC_SIZE_4,
};


static texgen_func texgen_generic_tab[4];
static texgen_func texgen_reflection_map_nv_tab[4];
static texgen_func texgen_normal_map_nv_tab[4];
static texgen_func texgen_sphere_map_tab[4];


typedef void (*build_m_func)(GLfloat f[][3],
                             GLfloat m[],
                             const GLvector3f *normals,
                             const GLvector4f *coord_vec,
                             const GLuint flags[],
                             const GLubyte cullmask[] );


typedef void (*build_f_func)( GLfloat *f,
                              GLuint fstride,
                              const GLvector3f *normal_vec,
                              const GLvector4f *coord_vec,
                              const GLuint flags[],
                              const GLubyte cullmask[] );


/* KW: compacted vs. coindexed normals don't bring any performance
 *     gains to texture generation, but it is still necessary to cope
 *     with the two different formats.
 */
#define TAG(x)           x
#define FIRST_NORMAL     normals->start
#define NEXT_NORMAL      STRIDE_F(normal, normals->stride)
#define LOCAL_VARS
#define CHECK
#define IDX              0
#include "texgen_tmp.h"


#define TAG(x)           x##_compacted
#define FIRST_NORMAL     normals->start
#define NEXT_NORMAL      ((flags[i]&VERT_NORM) ? (normal=first_normal[i]) : (normal))
#define CHECK
#define IDX              2
#define LOCAL_VARS        \
   GLfloat (*first_normal)[3] = (GLfloat (*)[3]) FIRST_NORMAL;

#include "texgen_tmp.h"

#define TAG(x)           x##_masked
#define FIRST_NORMAL     normals->start
#define NEXT_NORMAL      STRIDE_F(normal, normals->stride)
#define LOCAL_VARS
#define CHECK            if (cullmask[i])
#define IDX              1
#include "texgen_tmp.h"

#define TAG(x)           x##_compacted_masked
#define FIRST_NORMAL     normals->start
#define NEXT_NORMAL      ((flags[i]&VERT_NORM) ? normal=first_normal[i] : 0)
#define CHECK            if (cullmask[i])
#define IDX              3
#define LOCAL_VARS       \
   GLfloat (*first_normal)[3] = (GLfloat (*)[3]) FIRST_NORMAL;

#include "texgen_tmp.h"


/*
 * End texgen code
 ***********************************************************************
 */



/*
 * One time inits for texture mapping.
 * Called by one_time_init() in context.c
 */
void gl_init_texture( void )
{
   init_texgen();
   init_texgen_compacted();
   init_texgen_masked();
   init_texgen_compacted_masked();
}


/*
 * After state changes to texturing we call this function to update
 * intermediate and derived state.
 * Called by gl_update_state().
 */
void gl_update_texture_unit( GLcontext *ctx, struct gl_texture_unit *texUnit )
{
   (void) ctx;

   if ((texUnit->Enabled & TEXTURE0_3D) && texUnit->CurrentD[3]->Complete) {
      texUnit->ReallyEnabled = TEXTURE0_3D;
      texUnit->Current = texUnit->CurrentD[3];
      texUnit->CurrentDimension = 3;
      goto utex_cont;
   }
   if ((texUnit->Enabled & TEXTURE0_2D) && texUnit->CurrentD[2]->Complete) {
      texUnit->ReallyEnabled = TEXTURE0_2D;
      texUnit->Current = texUnit->CurrentD[2];
      texUnit->CurrentDimension = 2;
      goto utex_cont;
   }
   if ((texUnit->Enabled & TEXTURE0_1D) && texUnit->CurrentD[1]->Complete) {
      texUnit->ReallyEnabled = TEXTURE0_1D;
      texUnit->Current = texUnit->CurrentD[1];
      texUnit->CurrentDimension = 1;
      goto utex_cont;
   }
   {
/*    if (MESA_VERBOSE & VERBOSE_TEXTURE) {
         switch (texUnit->Enabled) {
         case TEXTURE0_3D:
            fprintf(stderr, "Using incomplete 3d texture %u\n",
                    texUnit->CurrentD[3]->Name);
            break;
         case TEXTURE0_2D:
            fprintf(stderr, "Using incomplete 2d texture %u\n",
                    texUnit->CurrentD[2]->Name);
            break;
         case TEXTURE0_1D:
            fprintf(stderr, "Using incomplete 1d texture %u\n",
                    texUnit->CurrentD[1]->Name);
            break;
         default:
            fprintf(stderr, "Bad value for texUnit->Enabled %x\n",
                    texUnit->Enabled);
            break;
         }
      }      */

      texUnit->ReallyEnabled = 0;
      texUnit->Current = NULL;
      texUnit->CurrentDimension = 0;
      return;
   }

utex_cont:

   texUnit->GenFlags = 0;

   if (texUnit->TexGenEnabled) {
      GLuint sz = 0;

      if (texUnit->TexGenEnabled & S_BIT) {
         sz = 1;
         texUnit->GenFlags |= texUnit->GenBitS;
      }
      if (texUnit->TexGenEnabled & T_BIT) {
         sz = 2;
         texUnit->GenFlags |= texUnit->GenBitT;
      }
      if (texUnit->TexGenEnabled & Q_BIT) {
         sz = 3;
         texUnit->GenFlags |= texUnit->GenBitQ;
      }
      if (texUnit->TexGenEnabled & R_BIT) {
         sz = 4;
         texUnit->GenFlags |= texUnit->GenBitR;
      }

      texUnit->TexgenSize = sz;
      texUnit->Holes = (GLubyte) (all_bits[sz] & ~texUnit->TexGenEnabled);
      texUnit->func = texgen_generic_tab;

      if (texUnit->TexGenEnabled == (S_BIT|T_BIT|R_BIT)) {
         if (texUnit->GenFlags == TEXGEN_REFLECTION_MAP_NV) {
            texUnit->func = texgen_reflection_map_nv_tab;
         }
         else if (texUnit->GenFlags == TEXGEN_NORMAL_MAP_NV) {
            texUnit->func = texgen_normal_map_nv_tab;
         }
      }
      else if (texUnit->TexGenEnabled == (S_BIT|T_BIT) &&
               texUnit->GenFlags == TEXGEN_SPHERE_MAP) {
         texUnit->func = texgen_sphere_map_tab;
      }
   }
}



/*
 * Paletted texture sampling.
 * Input:  tObj - the texture object
 *         index - the palette index (8-bit only)
 * Output:  red, green, blue, alpha - the texel color
 */
static void palette_sample(const struct gl_texture_object *tObj,
                           GLubyte index, GLubyte rgba[4] )
{
   GLcontext *ctx = gl_get_current_context();             /* THIS IS A HACK*/
   GLint i = index;
   const GLubyte *palette;
   GLenum format;

   if (ctx->Texture.SharedPalette) {
      palette = ctx->Texture.Palette.Table;
      format = ctx->Texture.Palette.Format;
   }
   else {
      palette = tObj->Palette.Table;
      format = tObj->Palette.Format;
   }

   switch (format) {
      case GL_ALPHA:
         rgba[ACOMP] = palette[index];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = palette[index];
         return;
      case GL_LUMINANCE_ALPHA:
         rgba[RCOMP] = palette[(index << 1) + 0];
         rgba[ACOMP] = palette[(index << 1) + 1];
         return;
      case GL_RGB:
         rgba[RCOMP] = palette[index * 3 + 0];
         rgba[GCOMP] = palette[index * 3 + 1];
         rgba[BCOMP] = palette[index * 3 + 2];
         return;
      case GL_RGBA:
         rgba[RCOMP] = palette[(i << 2) + 0];
         rgba[GCOMP] = palette[(i << 2) + 1];
         rgba[BCOMP] = palette[(i << 2) + 2];
         rgba[ACOMP] = palette[(i << 2) + 3];
         return;
      default:
         gl_problem(NULL, "Bad palette format in palette_sample");
   }
}



/*
 * These values are used in the fixed-point arithmetic used
 * for linear filtering.
 */
#define WEIGHT_SCALE 65536.0F
#define WEIGHT_SHIFT 16


/*
 * Used to compute texel locations for linear sampling.
 */
#define COMPUTE_LINEAR_TEXEL_LOCATIONS(wrapMode, S, U, SIZE, I0, I1)    \
{                                                                       \
   if (wrapMode == GL_REPEAT) {                                         \
      U = S * SIZE - 0.5F;                                              \
      I0 = ((GLint) myFloor(U)) & (SIZE - 1);                           \
      I1 = (I0 + 1) & (SIZE - 1);                                       \
   }                                                                    \
   else {                                                               \
      U = S * SIZE;                                                     \
      if (U < 0.0F)                                                     \
         U = 0.0F;                                                      \
      else if (U >= SIZE)                                               \
         U = SIZE;                                                      \
      U -= 0.5F;                                                        \
      I0 = (GLint) myFloor(U);                                          \
      I1 = I0 + 1;                                                      \
      if (wrapMode == GL_CLAMP_TO_EDGE) {                               \
         if (I0 < 0)                                                    \
            I0 = 0;                                                     \
         if (I1 >= SIZE)                                                \
            I1 = SIZE - 1;                                              \
      }                                                                 \
   }                                                                    \
}


/*
 * Used to compute texel location for nearest sampling.
 */
#define COMPUTE_NEAREST_TEXEL_LOCATION(wrapMode, S, SIZE, I)            \
{                                                                       \
   if (wrapMode == GL_REPEAT) {                                         \
      /* s limited to [0,1) */                                          \
      /* i limited to [0,width-1] */                                    \
      I = (GLint) (S * SIZE);                                           \
      if (S < 0.0F)                                                     \
         I -= 1;                                                        \
      I &= (SIZE - 1);                                                  \
   }                                                                    \
   else if (wrapMode == GL_CLAMP_TO_EDGE) {                             \
      const GLfloat min = 1.0F / (2.0F * SIZE);                         \
      const GLfloat max = 1.0F - min;                                   \
      if (S < min)                                                      \
         I = 0;                                                         \
      else if (S > max)                                                 \
         I = SIZE - 1;                                                  \
      else                                                              \
         I = (GLint) (S * SIZE);                                        \
   }                                                                    \
   else {                                                               \
      ASSERT(wrapMode == GL_CLAMP);                                     \
      /* s limited to [0,1] */                                          \
      /* i limited to [0,width-1] */                                    \
      if (S <= 0.0F)                                                    \
         I = 0;                                                         \
      else if (S >= 1.0F)                                               \
         I = SIZE - 1;                                                  \
      else                                                              \
         I = (GLint) (S * SIZE);                                        \
   }                                                                    \
}


/*
 * Bitflags for texture border color sampling.
 */
#define I0BIT   1
#define I1BIT   2
#define J0BIT   4
#define J1BIT   8
#define K0BIT  16
#define K1BIT  32



/**********************************************************************/
/*                    1-D Texture Sampling Functions                  */
/**********************************************************************/


/*
 * Return floor of x, being careful of negative values.
 */
static GLfloat myFloor(GLfloat x)
{
   if (x < 0.0F)
      return (GLfloat) ((GLint) x - 1);
   else
      return (GLfloat) (GLint) x;
}


/*
 * Return the fractional part of x.
 */
#define myFrac(x)  ( (x) - myFloor(x) )




/*
 * Given 1-D texture image and an (i) texel column coordinate, return the
 * texel color.
 */
static void get_1d_texel( const struct gl_texture_object *tObj,
                          const struct gl_texture_image *img, GLint i,
                          GLubyte rgba[4] )
{
   const GLubyte *texel;

#ifdef DEBUG
   GLint width = img->Width;
   ASSERT(i >= 0);
   ASSERT(i < width);
#endif

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[i];
            palette_sample(tObj, index, rgba);
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + i * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + i * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + i * 4;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in get_1d_texel");
         return;
   }
}



/*
 * Return the texture sample for coordinate (s) using GL_NEAREST filter.
 */
static void sample_1d_nearest( const struct gl_texture_object *tObj,
                               const struct gl_texture_image *img,
                               GLfloat s, GLubyte rgba[4] )
{
   const GLint width = img->Width2;  /* without border, power of two */
   const GLubyte *texel;
   GLint i;

   COMPUTE_NEAREST_TEXEL_LOCATION(tObj->WrapS, s, width, i);

   /* skip over the border, if any */
   i += img->Border;

   /* Get the texel */
   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[i];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[i];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[i];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + i * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + i * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + i * 4;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in sample_1d_nearest");
   }
}



/*
 * Return the texture sample for coordinate (s) using GL_LINEAR filter.
 */
static void sample_1d_linear( const struct gl_texture_object *tObj,
                              const struct gl_texture_image *img,
                              GLfloat s,
                              GLubyte rgba[4] )
{
   const GLint width = img->Width2;
   GLint i0, i1;
   GLfloat u;
   GLuint useBorderColor;

   COMPUTE_LINEAR_TEXEL_LOCATIONS(tObj->WrapS, s, u, width, i0, i1);

   useBorderColor = 0;
   if (img->Border) {
      i0 += img->Border;
      i1 += img->Border;
   }
   else {
      if (i0 < 0 || i0 >= width)   useBorderColor |= I0BIT;
      if (i1 < 0 || i1 >= width)   useBorderColor |= I1BIT;
   }

   {
      GLfloat a = myFrac(u);
      /* compute sample weights in fixed point in [0,WEIGHT_SCALE] */
      GLint w0 = (GLint) ((1.0F-a) * WEIGHT_SCALE + 0.5F);
      GLint w1 = (GLint) (      a  * WEIGHT_SCALE + 0.5F);

      GLubyte t0[4], t1[4];  /* texels */

      if (useBorderColor & I0BIT) {
         COPY_4UBV(t0, tObj->BorderColor);
      }
      else {
         get_1d_texel( tObj, img, i0, t0 );
      }
      if (useBorderColor & I1BIT) {
         COPY_4UBV(t1, tObj->BorderColor);
      }
      else {
         get_1d_texel( tObj, img, i1, t1 );
      }

      rgba[0] = (GLubyte) ((w0 * t0[0] + w1 * t1[0]) >> WEIGHT_SHIFT);
      rgba[1] = (GLubyte) ((w0 * t0[1] + w1 * t1[1]) >> WEIGHT_SHIFT);
      rgba[2] = (GLubyte) ((w0 * t0[2] + w1 * t1[2]) >> WEIGHT_SHIFT);
      rgba[3] = (GLubyte) ((w0 * t0[3] + w1 * t1[3]) >> WEIGHT_SHIFT);
   }
}


static void
sample_1d_nearest_mipmap_nearest( const struct gl_texture_object *tObj,
                                  GLfloat s, GLfloat lambda,
                                  GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_1d_nearest( tObj, tObj->Image[level], s, rgba );
}


static void
sample_1d_linear_mipmap_nearest( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_1d_linear( tObj, tObj->Image[level], s, rgba );
}



static void
sample_1d_nearest_mipmap_linear( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_1d_nearest( tObj, tObj->Image[tObj->P], s, rgba );
   }
   else {
      GLubyte t0[4], t1[4];
      GLfloat f = myFrac(lambda);
      sample_1d_nearest( tObj, tObj->Image[level  ], s, t0 );
      sample_1d_nearest( tObj, tObj->Image[level+1], s, t1 );
      rgba[RCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void
sample_1d_linear_mipmap_linear( const struct gl_texture_object *tObj,
                                GLfloat s, GLfloat lambda,
                                GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_1d_linear( tObj, tObj->Image[tObj->P], s, rgba );
   }
   else {
      GLubyte t0[4], t1[4];
      GLfloat f = myFrac(lambda);
      sample_1d_linear( tObj, tObj->Image[level  ], s, t0 );
      sample_1d_linear( tObj, tObj->Image[level+1], s, t1 );
      rgba[RCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void sample_nearest_1d( const struct gl_texture_object *tObj, GLuint n,
                               const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) t;
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_1d_nearest( tObj, image, s[i], rgba[i] );
   }
}



static void sample_linear_1d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) t;
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_1d_linear( tObj, image, s[i], rgba[i] );
   }
}


/*
 * Given an (s) texture coordinate and lambda (level of detail) value,
 * return a texture sample.
 *
 */
static void sample_lambda_1d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;

   (void) t;
   (void) u;

   for (i=0;i<n;i++) {
      if (lambda[i] > tObj->MinMagThresh) {
         /* minification */
         switch (tObj->MinFilter) {
            case GL_NEAREST:
               sample_1d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
               break;
            case GL_LINEAR:
               sample_1d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
               break;
            case GL_NEAREST_MIPMAP_NEAREST:
               sample_1d_nearest_mipmap_nearest( tObj, lambda[i], s[i], rgba[i] );
               break;
            case GL_LINEAR_MIPMAP_NEAREST:
               sample_1d_linear_mipmap_nearest( tObj, s[i], lambda[i], rgba[i] );
               break;
            case GL_NEAREST_MIPMAP_LINEAR:
               sample_1d_nearest_mipmap_linear( tObj, s[i], lambda[i], rgba[i] );
               break;
            case GL_LINEAR_MIPMAP_LINEAR:
               sample_1d_linear_mipmap_linear( tObj, s[i], lambda[i], rgba[i] );
               break;
            default:
               gl_problem(NULL, "Bad min filter in sample_1d_texture");
               return;
         }
      }
      else {
         /* magnification */
         switch (tObj->MagFilter) {
            case GL_NEAREST:
               sample_1d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
               break;
            case GL_LINEAR:
               sample_1d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
               break;
            default:
               gl_problem(NULL, "Bad mag filter in sample_1d_texture");
               return;
         }
      }
   }
}




/**********************************************************************/
/*                    2-D Texture Sampling Functions                  */
/**********************************************************************/


/*
 * Given a texture image and an (i,j) integer texel coordinate, return the
 * texel color.
 */
static void get_2d_texel( const struct gl_texture_object *tObj,
                          const struct gl_texture_image *img, GLint i, GLint j,
                          GLubyte rgba[4] )
{
   const GLint width = img->Width;    /* includes border */
   const GLubyte *texel;

#ifdef DEBUG
   const GLint height = img->Height;  /* includes border */
   ASSERT(i >= 0);
   ASSERT(i < width);
   ASSERT(j >= 0);
   ASSERT(j < height);
#endif

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ width *j + i ];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ width * j + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ width * j + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + (width * j + i) * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (width * j + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + (width * j + i) * 4;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in get_2d_texel");
   }
}



/*
 * Return the texture sample for coordinate (s,t) using GL_NEAREST filter.
 */
static void sample_2d_nearest( const struct gl_texture_object *tObj,
                               const struct gl_texture_image *img,
                               GLfloat s, GLfloat t,
                               GLubyte rgba[] )
{
   const GLint imgWidth = img->Width;  /* includes border */
   const GLint width = img->Width2;    /* without border, power of two */
   const GLint height = img->Height2;  /* without border, power of two */
   const GLubyte *texel;
   GLint i, j;

   COMPUTE_NEAREST_TEXEL_LOCATION(tObj->WrapS, s, width,  i);
   COMPUTE_NEAREST_TEXEL_LOCATION(tObj->WrapT, t, height, j);

   /* skip over the border, if any */
   i += img->Border;
   j += img->Border;

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ j * imgWidth + i ];
            palette_sample(tObj, index, rgba);
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ j * imgWidth + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ j * imgWidth + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + ((j * imgWidth + i) << 1);
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (j * imgWidth + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + ((j * imgWidth + i) << 2);
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in sample_2d_nearest");
   }
}



/*
 * Return the texture sample for coordinate (s,t) using GL_LINEAR filter.
 * New sampling code contributed by Lynn Quam <quam@ai.sri.com>.
 */
static void sample_2d_linear( const struct gl_texture_object *tObj,
                              const struct gl_texture_image *img,
                              GLfloat s, GLfloat t,
                              GLubyte rgba[] )
{
   const GLint width = img->Width2;
   const GLint height = img->Height2;
   GLint i0, j0, i1, j1;
   GLuint useBorderColor;
   GLfloat u, v;

   COMPUTE_LINEAR_TEXEL_LOCATIONS(tObj->WrapS, s, u, width,  i0, i1);
   COMPUTE_LINEAR_TEXEL_LOCATIONS(tObj->WrapT, t, v, height, j0, j1);

   useBorderColor = 0;
   if (img->Border) {
      i0 += img->Border;
      i1 += img->Border;
      j0 += img->Border;
      j1 += img->Border;
   }
   else {
      if (i0 < 0 || i0 >= width)   useBorderColor |= I0BIT;
      if (i1 < 0 || i1 >= width)   useBorderColor |= I1BIT;
      if (j0 < 0 || j0 >= height)  useBorderColor |= J0BIT;
      if (j1 < 0 || j1 >= height)  useBorderColor |= J1BIT;
   }

   {
      GLfloat a = myFrac(u);
      GLfloat b = myFrac(v);
      /* compute sample weights in fixed point in [0,WEIGHT_SCALE] */
      GLint w00 = (GLint) ((1.0F-a)*(1.0F-b) * WEIGHT_SCALE + 0.5F);
      GLint w10 = (GLint) (      a *(1.0F-b) * WEIGHT_SCALE + 0.5F);
      GLint w01 = (GLint) ((1.0F-a)*      b  * WEIGHT_SCALE + 0.5F);
      GLint w11 = (GLint) (      a *      b  * WEIGHT_SCALE + 0.5F);
      GLubyte t00[4];
      GLubyte t10[4];
      GLubyte t01[4];
      GLubyte t11[4];

      if (useBorderColor & (I0BIT | J0BIT)) {
         COPY_4UBV(t00, tObj->BorderColor);
      }
      else {
         get_2d_texel( tObj, img, i0, j0, t00 );
      }
      if (useBorderColor & (I1BIT | J0BIT)) {
         COPY_4UBV(t10, tObj->BorderColor);
      }
      else {
         get_2d_texel( tObj, img, i1, j0, t10 );
      }
      if (useBorderColor & (I0BIT | J1BIT)) {
         COPY_4UBV(t01, tObj->BorderColor);
      }
      else {
         get_2d_texel( tObj, img, i0, j1, t01 );
      }
      if (useBorderColor & (I1BIT | J1BIT)) {
         COPY_4UBV(t11, tObj->BorderColor);
      }
      else {
         get_2d_texel( tObj, img, i1, j1, t11 );
      }

      rgba[0] = (GLubyte) ((w00 * t00[0] + w10 * t10[0] + w01 * t01[0] + w11 * t11[0]) >> WEIGHT_SHIFT);
      rgba[1] = (GLubyte) ((w00 * t00[1] + w10 * t10[1] + w01 * t01[1] + w11 * t11[1]) >> WEIGHT_SHIFT);
      rgba[2] = (GLubyte) ((w00 * t00[2] + w10 * t10[2] + w01 * t01[2] + w11 * t11[2]) >> WEIGHT_SHIFT);
      rgba[3] = (GLubyte) ((w00 * t00[3] + w10 * t10[3] + w01 * t01[3] + w11 * t11[3]) >> WEIGHT_SHIFT);
   }

}



static void
sample_2d_nearest_mipmap_nearest( const struct gl_texture_object *tObj,
                                  GLfloat s, GLfloat t, GLfloat lambda,
                                  GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_2d_nearest( tObj, tObj->Image[level], s, t, rgba );
}



static void
sample_2d_linear_mipmap_nearest( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_2d_linear( tObj, tObj->Image[level], s, t, rgba );
}



static void
sample_2d_nearest_mipmap_linear( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_2d_nearest( tObj, tObj->Image[tObj->P], s, t, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = myFrac(lambda);
      sample_2d_nearest( tObj, tObj->Image[level  ], s, t, t0 );
      sample_2d_nearest( tObj, tObj->Image[level+1], s, t, t1 );
      rgba[RCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void
sample_2d_linear_mipmap_linear( const struct gl_texture_object *tObj,
                                GLfloat s, GLfloat t, GLfloat lambda,
                                GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_2d_linear( tObj, tObj->Image[tObj->P], s, t, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = myFrac(lambda);
      sample_2d_linear( tObj, tObj->Image[level  ], s, t, t0 );
      sample_2d_linear( tObj, tObj->Image[level+1], s, t, t1 );
      rgba[RCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void sample_nearest_2d( const struct gl_texture_object *tObj, GLuint n,
                               const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_2d_nearest( tObj, image, s[i], t[i], rgba[i] );
   }
}



static void sample_linear_2d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_2d_linear( tObj, image, s[i], t[i], rgba[i] );
   }
}


/*
 * Given an (s,t) texture coordinate and lambda (level of detail) value,
 * return a texture sample.
 */
static void sample_lambda_2d( const struct gl_texture_object *tObj,
                              GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   (void) u;
   for (i=0;i<n;i++) {
      if (lambda[i] > tObj->MinMagThresh) {
         /* minification */
         switch (tObj->MinFilter) {
            case GL_NEAREST:
               sample_2d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
               break;
            case GL_LINEAR:
               sample_2d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
               break;
            case GL_NEAREST_MIPMAP_NEAREST:
               sample_2d_nearest_mipmap_nearest( tObj, s[i], t[i], lambda[i], rgba[i] );
               break;
            case GL_LINEAR_MIPMAP_NEAREST:
               sample_2d_linear_mipmap_nearest( tObj, s[i], t[i], lambda[i], rgba[i] );
               break;
            case GL_NEAREST_MIPMAP_LINEAR:
               sample_2d_nearest_mipmap_linear( tObj, s[i], t[i], lambda[i], rgba[i] );
               break;
            case GL_LINEAR_MIPMAP_LINEAR:
               sample_2d_linear_mipmap_linear( tObj, s[i], t[i], lambda[i], rgba[i] );
               break;
            default:
               gl_problem(NULL, "Bad min filter in sample_2d_texture");
               return;
         }
      }
      else {
         /* magnification */
         switch (tObj->MagFilter) {
            case GL_NEAREST:
               sample_2d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
               break;
            case GL_LINEAR:
               sample_2d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
               break;
            default:
               gl_problem(NULL, "Bad mag filter in sample_2d_texture");
         }
      }
   }
}


/*
 * Optimized 2-D texture sampling:
 *    S and T wrap mode == GL_REPEAT
 *    GL_NEAREST min/mag filter
 *    No border
 *    Format = GL_RGB
 */
static void opt_sample_rgb_2d( const struct gl_texture_object *tObj,
                               GLuint n, const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   const struct gl_texture_image *img = tObj->Image[tObj->BaseLevel];
   const GLfloat width = (GLfloat) img->Width;
   const GLfloat height = (GLfloat) img->Height;
   const GLint colMask = img->Width - 1;
   const GLint rowMask = img->Height - 1;
   const GLint shift = img->WidthLog2;
   GLuint k;
   (void) u;
   (void) lambda;
   ASSERT(tObj->WrapS==GL_REPEAT);
   ASSERT(tObj->WrapT==GL_REPEAT);
   ASSERT(tObj->MinFilter==GL_NEAREST);
   ASSERT(tObj->MagFilter==GL_NEAREST);
   ASSERT(img->Border==0);
   ASSERT(img->Format==GL_RGB);

   /* NOTE: negative float->int doesn't floor, add 10000 as to work-around */
   for (k=0;k<n;k++) {
      GLint i = (GLint) ((s[k] + 10000.0) * width) & colMask;
      GLint j = (GLint) ((t[k] + 10000.0) * height) & rowMask;
      GLint pos = (j << shift) | i;
      GLubyte *texel = img->Data + pos + pos + pos;  /* pos*3 */
      rgba[k][RCOMP] = texel[0];
      rgba[k][GCOMP] = texel[1];
      rgba[k][BCOMP] = texel[2];
   }
}


/*
 * Optimized 2-D texture sampling:
 *    S and T wrap mode == GL_REPEAT
 *    GL_NEAREST min/mag filter
 *    No border
 *    Format = GL_RGBA
 */
static void opt_sample_rgba_2d( const struct gl_texture_object *tObj,
                                GLuint n, const GLfloat s[], const GLfloat t[],
                                const GLfloat u[], const GLfloat lambda[],
                                GLubyte rgba[][4] )
{
   const struct gl_texture_image *img = tObj->Image[tObj->BaseLevel];
   const GLfloat width = (GLfloat) img->Width;
   const GLfloat height = (GLfloat) img->Height;
   const GLint colMask = img->Width - 1;
   const GLint rowMask = img->Height - 1;
   const GLint shift = img->WidthLog2;
   GLuint k;
   (void) u;
   (void) lambda;
   ASSERT(tObj->WrapS==GL_REPEAT);
   ASSERT(tObj->WrapT==GL_REPEAT);
   ASSERT(tObj->MinFilter==GL_NEAREST);
   ASSERT(tObj->MagFilter==GL_NEAREST);
   ASSERT(img->Border==0);
   ASSERT(img->Format==GL_RGBA);

   /* NOTE: negative float->int doesn't floor, add 10000 as to work-around */
   for (k=0;k<n;k++) {
      GLint i = (GLint) ((s[k] + 10000.0) * width) & colMask;
      GLint j = (GLint) ((t[k] + 10000.0) * height) & rowMask;
      GLint pos = (j << shift) | i;
      GLubyte *texel = img->Data + (pos << 2);    /* pos*4 */
      rgba[k][RCOMP] = texel[0];
      rgba[k][GCOMP] = texel[1];
      rgba[k][BCOMP] = texel[2];
      rgba[k][ACOMP] = texel[3];
   }
}



/**********************************************************************/
/*                    3-D Texture Sampling Functions                  */
/**********************************************************************/

/*
 * Given a texture image and an (i,j,k) integer texel coordinate, return the
 * texel color.
 */
static void get_3d_texel( const struct gl_texture_object *tObj,
                          const struct gl_texture_image *img,
                          GLint i, GLint j, GLint k,
                          GLubyte rgba[4] )
{
   const GLint width = img->Width;    /* includes border */
   const GLint height = img->Height;  /* includes border */
   const GLint rectarea = width * height;
   const GLubyte *texel;

#ifdef DEBUG
   const GLint depth = img->Depth;    /* includes border */
   ASSERT(i >= 0);
   ASSERT(i < width);
   ASSERT(j >= 0);
   ASSERT(j < height);
   ASSERT(k >= 0);
   ASSERT(k < depth);
#endif

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ rectarea * k +  width * j + i ];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ rectarea * k +  width * j + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ rectarea * k +  width * j + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + ( rectarea * k + width * j + i) * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (rectarea * k + width * j + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + (rectarea * k + width * j + i) * 4;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in get_3d_texel");
   }
}


/*
 * Return the texture sample for coordinate (s,t,r) using GL_NEAREST filter.
 */
static void sample_3d_nearest( const struct gl_texture_object *tObj,
                               const struct gl_texture_image *img,
                               GLfloat s, GLfloat t, GLfloat r,
                               GLubyte rgba[4] )
{
   const GLint imgWidth = img->Width;   /* includes border, if any */
   const GLint imgHeight = img->Height; /* includes border, if any */
   const GLint width = img->Width2;     /* without border, power of two */
   const GLint height = img->Height2;   /* without border, power of two */
   const GLint depth = img->Depth2;     /* without border, power of two */
   const GLint rectarea = imgWidth * imgHeight;
   const GLubyte *texel;
   GLint i, j, k;

   COMPUTE_NEAREST_TEXEL_LOCATION(tObj->WrapS, s, width,  i);
   COMPUTE_NEAREST_TEXEL_LOCATION(tObj->WrapT, t, height, j);
   COMPUTE_NEAREST_TEXEL_LOCATION(tObj->WrapR, r, depth,  k);

   switch (tObj->Image[0]->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ rectarea * k + j * imgWidth + i ];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ rectarea * k + j * imgWidth + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ rectarea * k + j * imgWidth + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + ((rectarea * k + j * imgWidth + i) << 1);
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + ( rectarea * k + j * imgWidth + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + ((rectarea * k + j * imgWidth + i) << 2);
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in sample_3d_nearest");
   }
}



/*
 * Return the texture sample for coordinate (s,t,r) using GL_LINEAR filter.
 */
static void sample_3d_linear( const struct gl_texture_object *tObj,
                              const struct gl_texture_image *img,
                              GLfloat s, GLfloat t, GLfloat r,
                              GLubyte rgba[4] )
{
   const GLint width = img->Width2;
   const GLint height = img->Height2;
   const GLint depth = img->Depth2;
   GLint i0, j0, k0, i1, j1, k1;
   GLuint useBorderColor;
   GLfloat u, v, w;

   COMPUTE_LINEAR_TEXEL_LOCATIONS(tObj->WrapS, s, u, width,  i0, i1);
   COMPUTE_LINEAR_TEXEL_LOCATIONS(tObj->WrapT, t, v, height, j0, j1);
   COMPUTE_LINEAR_TEXEL_LOCATIONS(tObj->WrapR, r, w, depth,  k0, k1);

   useBorderColor = 0;
   if (img->Border) {
      i0 += img->Border;
      i1 += img->Border;
      j0 += img->Border;
      j1 += img->Border;
      k0 += img->Border;
      k1 += img->Border;
   }
   else {
      /* check if sampling texture border color */
      if (i0 < 0 || i0 >= width)   useBorderColor |= I0BIT;
      if (i1 < 0 || i1 >= width)   useBorderColor |= I1BIT;
      if (j0 < 0 || j0 >= height)  useBorderColor |= J0BIT;
      if (j1 < 0 || j1 >= height)  useBorderColor |= J1BIT;
      if (k0 < 0 || k0 >= depth)   useBorderColor |= K0BIT;
      if (k1 < 0 || k1 >= depth)   useBorderColor |= K1BIT;
   }

   {
      GLfloat a = myFrac(u);
      GLfloat b = myFrac(v);
      GLfloat c = myFrac(w);
      /* compute sample weights in fixed point in [0,WEIGHT_SCALE] */
      GLint w000 = (GLint) ((1.0F-a)*(1.0F-b)*(1.0F-c) * WEIGHT_SCALE + 0.5F);
      GLint w100 = (GLint) (      a *(1.0F-b)*(1.0F-c) * WEIGHT_SCALE + 0.5F);
      GLint w010 = (GLint) ((1.0F-a)*      b *(1.0F-c) * WEIGHT_SCALE + 0.5F);
      GLint w110 = (GLint) (      a *      b *(1.0F-c) * WEIGHT_SCALE + 0.5F);
      GLint w001 = (GLint) ((1.0F-a)*(1.0F-b)*      c  * WEIGHT_SCALE + 0.5F);
      GLint w101 = (GLint) (      a *(1.0F-b)*      c  * WEIGHT_SCALE + 0.5F);
      GLint w011 = (GLint) ((1.0F-a)*      b *      c  * WEIGHT_SCALE + 0.5F);
      GLint w111 = (GLint) (      a *      b *      c  * WEIGHT_SCALE + 0.5F);

      GLubyte t000[4], t010[4], t001[4], t011[4];
      GLubyte t100[4], t110[4], t101[4], t111[4];

      if (useBorderColor & (I0BIT | J0BIT | K0BIT)) {
         COPY_4UBV(t000, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i0, j0, k0, t000 );
      }
      if (useBorderColor & (I1BIT | J0BIT | K0BIT)) {
         COPY_4UBV(t100, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i1, j0, k0, t100 );
      }
      if (useBorderColor & (I0BIT | J1BIT | K0BIT)) {
         COPY_4UBV(t010, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i0, j1, k0, t010 );
      }
      if (useBorderColor & (I1BIT | J1BIT | K0BIT)) {
         COPY_4UBV(t110, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i1, j1, k0, t110 );
      }

      if (useBorderColor & (I0BIT | J0BIT | K1BIT)) {
         COPY_4UBV(t001, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i0, j0, k1, t001 );
      }
      if (useBorderColor & (I1BIT | J0BIT | K1BIT)) {
         COPY_4UBV(t101, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i1, j0, k1, t101 );
      }
      if (useBorderColor & (I0BIT | J1BIT | K1BIT)) {
         COPY_4UBV(t011, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i0, j1, k1, t011 );
      }
      if (useBorderColor & (I1BIT | J1BIT | K1BIT)) {
         COPY_4UBV(t111, tObj->BorderColor);
      }
      else {
         get_3d_texel( tObj, img, i1, j1, k1, t111 );
      }

      rgba[0] = (GLubyte) (
                 (w000*t000[0] + w010*t010[0] + w001*t001[0] + w011*t011[0] +
                  w100*t100[0] + w110*t110[0] + w101*t101[0] + w111*t111[0]  )
                 >> WEIGHT_SHIFT);
      rgba[1] = (GLubyte) (
                 (w000*t000[1] + w010*t010[1] + w001*t001[1] + w011*t011[1] +
                  w100*t100[1] + w110*t110[1] + w101*t101[1] + w111*t111[1] )
                 >> WEIGHT_SHIFT);
      rgba[2] = (GLubyte) (
                 (w000*t000[2] + w010*t010[2] + w001*t001[2] + w011*t011[2] +
                  w100*t100[2] + w110*t110[2] + w101*t101[2] + w111*t111[2] )
                 >> WEIGHT_SHIFT);
      rgba[3] = (GLubyte) (
                 (w000*t000[3] + w010*t010[3] + w001*t001[3] + w011*t011[3] +
                  w100*t100[3] + w110*t110[3] + w101*t101[3] + w111*t111[3] )
                 >> WEIGHT_SHIFT);
   }
}



static void
sample_3d_nearest_mipmap_nearest( const struct gl_texture_object *tObj,
                                  GLfloat s, GLfloat t, GLfloat r,
                                  GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_3d_nearest( tObj, tObj->Image[level], s, t, r, rgba );
}


static void
sample_3d_linear_mipmap_nearest( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat r,
                                 GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_3d_linear( tObj, tObj->Image[level], s, t, r, rgba );
}


static void
sample_3d_nearest_mipmap_linear( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat r,
                                 GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_3d_nearest( tObj, tObj->Image[tObj->P], s, t, r, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = myFrac(lambda);
      sample_3d_nearest( tObj, tObj->Image[level  ], s, t, r, t0 );
      sample_3d_nearest( tObj, tObj->Image[level+1], s, t, r, t1 );
      rgba[RCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}


static void
sample_3d_linear_mipmap_linear( const struct gl_texture_object *tObj,
                                GLfloat s, GLfloat t, GLfloat r,
                                GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_3d_linear( tObj, tObj->Image[tObj->P], s, t, r, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = myFrac(lambda);
      sample_3d_linear( tObj, tObj->Image[level  ], s, t, r, t0 );
      sample_3d_linear( tObj, tObj->Image[level+1], s, t, r, t1 );
      rgba[RCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLubyte) (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}


static void sample_nearest_3d( const struct gl_texture_object *tObj, GLuint n,
                               const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_3d_nearest( tObj, image, s[i], t[i], u[i], rgba[i] );
   }
}



static void sample_linear_3d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_3d_linear( tObj, image, s[i], t[i], u[i], rgba[i] );
   }
}


/*
 * Given an (s,t,r) texture coordinate and lambda (level of detail) value,
 * return a texture sample.
 */
static void sample_lambda_3d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;

   for (i=0;i<n;i++) {

      if (lambda[i] > tObj->MinMagThresh) {
         /* minification */
         switch (tObj->MinFilter) {
            case GL_NEAREST:
               sample_3d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
               break;
            case GL_LINEAR:
               sample_3d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
               break;
            case GL_NEAREST_MIPMAP_NEAREST:
               sample_3d_nearest_mipmap_nearest( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
               break;
            case GL_LINEAR_MIPMAP_NEAREST:
               sample_3d_linear_mipmap_nearest( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
               break;
            case GL_NEAREST_MIPMAP_LINEAR:
               sample_3d_nearest_mipmap_linear( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
               break;
            case GL_LINEAR_MIPMAP_LINEAR:
               sample_3d_linear_mipmap_linear( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
               break;
            default:
               gl_problem(NULL, "Bad min filterin sample_3d_texture");
         }
      }
      else {
         /* magnification */
         switch (tObj->MagFilter) {
            case GL_NEAREST:
               sample_3d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
               break;
            case GL_LINEAR:
               sample_3d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
               break;
            default:
               gl_problem(NULL, "Bad mag filter in sample_3d_texture");
         }
      }
   }
}



/**********************************************************************/
/*                       Texture Sampling Setup                       */
/**********************************************************************/


/*
 * Setup the texture sampling function for this texture object.
 */
void gl_set_texture_sampler( struct gl_texture_object *t )
{
   if (!t->Complete) {
      t->SampleFunc = NULL;
   }
   else {
      GLboolean needLambda = (GLboolean) (t->MinFilter != t->MagFilter);

      if (needLambda) {
         /* Compute min/mag filter threshold */
         if (t->MagFilter==GL_LINEAR
             && (t->MinFilter==GL_NEAREST_MIPMAP_NEAREST ||
                 t->MinFilter==GL_LINEAR_MIPMAP_NEAREST)) {
            t->MinMagThresh = 0.5F;
         }
         else {
            t->MinMagThresh = 0.0F;
         }
      }

      switch (t->Dimensions) {
         case 1:
            if (needLambda) {
               t->SampleFunc = sample_lambda_1d;
            }
            else if (t->MinFilter==GL_LINEAR) {
               t->SampleFunc = sample_linear_1d;
            }
            else {
               ASSERT(t->MinFilter==GL_NEAREST);
               t->SampleFunc = sample_nearest_1d;
            }
            break;
         case 2:
            if (needLambda) {
               t->SampleFunc = sample_lambda_2d;
            }
            else if (t->MinFilter==GL_LINEAR) {
               t->SampleFunc = sample_linear_2d;
            }
            else {
               ASSERT(t->MinFilter==GL_NEAREST);
               if (t->WrapS==GL_REPEAT && t->WrapT==GL_REPEAT
                   && t->Image[0]->Border==0 && t->Image[0]->Format==GL_RGB) {
                  t->SampleFunc = opt_sample_rgb_2d;
               }
               else if (t->WrapS==GL_REPEAT && t->WrapT==GL_REPEAT
                   && t->Image[0]->Border==0 && t->Image[0]->Format==GL_RGBA) {
                  t->SampleFunc = opt_sample_rgba_2d;
               }
               else
                  t->SampleFunc = sample_nearest_2d;
            }
            break;
         case 3:
            if (needLambda) {
               t->SampleFunc = sample_lambda_3d;
            }
            else if (t->MinFilter==GL_LINEAR) {
               t->SampleFunc = sample_linear_3d;
            }
            else {
               ASSERT(t->MinFilter==GL_NEAREST);
               t->SampleFunc = sample_nearest_3d;
            }
            break;
         default:
            gl_problem(NULL, "invalid dimensions in gl_set_texture_sampler");
      }
   }
}



/**********************************************************************/
/*                      Texture Application                           */
/**********************************************************************/


/*
 * Combine incoming fragment color with texel color to produce output color.
 * Input:  textureUnit - pointer to texture unit to apply
 *         format - base internal texture format
 *         n - number of fragments
 *         texels - array of texel colors
 * InOut:  rgba - incoming fragment colors modified by texel colors
 *                according to the texture environment mode.
 */
static void apply_texture( const GLcontext *ctx,
                           const struct gl_texture_unit *texUnit,
                           GLuint n,
                           GLubyte rgba[][4], CONST GLubyte texel[][4] )
{
   GLuint i;
   GLint Rc, Gc, Bc, Ac;
   GLenum format;

   ASSERT(texUnit);
   ASSERT(texUnit->Current);
   ASSERT(texUnit->Current->Image[0]);

   format = texUnit->Current->Image[0]->Format;

/*
 * Use (A*(B+1)) >> 8 as a fast approximation of (A*B)/255 for A
 * and B in [0,255]
 */
#define PROD(A,B)   ( (GLubyte) (((GLint)(A) * ((GLint)(B)+1)) >> 8) )

   if (format==GL_COLOR_INDEX) {
      format = GL_RGBA;  /* XXXX a hack! */
   }

   switch (texUnit->EnvMode) {
      case GL_REPLACE:
         switch (format) {
            case GL_ALPHA:
               for (i=0;i<n;i++) {
                  /* Cv = Cf */
                  /* Av = At */
                  rgba[i][ACOMP] = texel[i][ACOMP];
               }
               break;
            case GL_LUMINANCE:
               for (i=0;i<n;i++) {
                  /* Cv = Lt */
                  GLubyte Lt = texel[i][RCOMP];
                  rgba[i][RCOMP] = rgba[i][GCOMP] = rgba[i][BCOMP] = Lt;
                  /* Av = Af */
               }
               break;
            case GL_LUMINANCE_ALPHA:
               for (i=0;i<n;i++) {
                  GLubyte Lt = texel[i][RCOMP];
                  /* Cv = Lt */
                  rgba[i][RCOMP] = rgba[i][GCOMP] = rgba[i][BCOMP] = Lt;
                  /* Av = At */
                  rgba[i][ACOMP] = texel[i][ACOMP];
               }
               break;
            case GL_INTENSITY:
               for (i=0;i<n;i++) {
                  /* Cv = It */
                  GLubyte It = texel[i][RCOMP];
                  rgba[i][RCOMP] = rgba[i][GCOMP] = rgba[i][BCOMP] = It;
                  /* Av = It */
                  rgba[i][ACOMP] = It;
               }
               break;
            case GL_RGB:
               for (i=0;i<n;i++) {
                  /* Cv = Ct */
                  rgba[i][RCOMP] = texel[i][RCOMP];
                  rgba[i][GCOMP] = texel[i][GCOMP];
                  rgba[i][BCOMP] = texel[i][BCOMP];
                  /* Av = Af */
               }
               break;
            case GL_RGBA:
               for (i=0;i<n;i++) {
                  /* Cv = Ct */
                  rgba[i][RCOMP] = texel[i][RCOMP];
                  rgba[i][GCOMP] = texel[i][GCOMP];
                  rgba[i][BCOMP] = texel[i][BCOMP];
                  /* Av = At */
                  rgba[i][ACOMP] = texel[i][ACOMP];
               }
               break;
            default:
               gl_problem(ctx, "Bad format (GL_REPLACE) in apply_texture");
               return;
         }
         break;

      case GL_MODULATE:
         switch (format) {
            case GL_ALPHA:
               for (i=0;i<n;i++) {
                  /* Cv = Cf */
                  /* Av = AfAt */
                  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], texel[i][ACOMP] );
               }
               break;
            case GL_LUMINANCE:
               for (i=0;i<n;i++) {
                  /* Cv = LtCf */
                  GLubyte Lt = texel[i][RCOMP];
                  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], Lt );
                  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], Lt );
                  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], Lt );
                  /* Av = Af */
               }
               break;
            case GL_LUMINANCE_ALPHA:
               for (i=0;i<n;i++) {
                  /* Cv = CfLt */
                  GLubyte Lt = texel[i][RCOMP];
                  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], Lt );
                  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], Lt );
                  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], Lt );
                  /* Av = AfAt */
                  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], texel[i][ACOMP] );
               }
               break;
            case GL_INTENSITY:
               for (i=0;i<n;i++) {
                  /* Cv = CfIt */
                  GLubyte It = texel[i][RCOMP];
                  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], It );
                  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], It );
                  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], It );
                  /* Av = AfIt */
                  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], It );
               }
               break;
            case GL_RGB:
               for (i=0;i<n;i++) {
                  /* Cv = CfCt */
                  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], texel[i][RCOMP] );
                  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], texel[i][GCOMP] );
                  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], texel[i][BCOMP] );
                  /* Av = Af */
               }
               break;
            case GL_RGBA:
               for (i=0;i<n;i++) {
                  /* Cv = CfCt */
                  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], texel[i][RCOMP] );
                  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], texel[i][GCOMP] );
                  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], texel[i][BCOMP] );
                  /* Av = AfAt */
                  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], texel[i][ACOMP] );
               }
               break;
            default:
               gl_problem(ctx, "Bad format (GL_MODULATE) in apply_texture");
               return;
         }
         break;

      case GL_DECAL:
         switch (format) {
            case GL_ALPHA:
            case GL_LUMINANCE:
            case GL_LUMINANCE_ALPHA:
            case GL_INTENSITY:
               /* undefined */
               break;
            case GL_RGB:
               for (i=0;i<n;i++) {
                  /* Cv = Ct */
                  rgba[i][RCOMP] = texel[i][RCOMP];
                  rgba[i][GCOMP] = texel[i][GCOMP];
                  rgba[i][BCOMP] = texel[i][BCOMP];
                  /* Av = Af */
               }
               break;
            case GL_RGBA:
               for (i=0;i<n;i++) {
                  /* Cv = Cf(1-At) + CtAt */
                  GLint t = texel[i][ACOMP], s = 255 - t;
                  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(texel[i][RCOMP],t);
                  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(texel[i][GCOMP],t);
                  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(texel[i][BCOMP],t);
                  /* Av = Af */
               }
               break;
            default:
               gl_problem(ctx, "Bad format (GL_DECAL) in apply_texture");
               return;
         }
         break;

      case GL_BLEND:
         Rc = (GLint) (texUnit->EnvColor[0] * 255.0F);
         Gc = (GLint) (texUnit->EnvColor[1] * 255.0F);
         Bc = (GLint) (texUnit->EnvColor[2] * 255.0F);
         Ac = (GLint) (texUnit->EnvColor[3] * 255.0F);
         switch (format) {
            case GL_ALPHA:
               for (i=0;i<n;i++) {
                  /* Cv = Cf */
                  /* Av = AfAt */
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP], texel[i][ACOMP]);
               }
               break;
            case GL_LUMINANCE:
               for (i=0;i<n;i++) {
                  /* Cv = Cf(1-Lt) + CcLt */
                  GLubyte Lt = texel[i][RCOMP], s = 255 - Lt;
                  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(Rc, Lt);
                  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(Gc, Lt);
                  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(Bc, Lt);
                  /* Av = Af */
               }
               break;
            case GL_LUMINANCE_ALPHA:
               for (i=0;i<n;i++) {
                  /* Cv = Cf(1-Lt) + CcLt */
                  GLubyte Lt = texel[i][RCOMP], s = 255 - Lt;
                  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(Rc, Lt);
                  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(Gc, Lt);
                  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(Bc, Lt);
                  /* Av = AfAt */
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP],texel[i][ACOMP]);
               }
               break;
            case GL_INTENSITY:
               for (i=0;i<n;i++) {
                  /* Cv = Cf(1-It) + CcLt */
                  GLubyte It = texel[i][RCOMP], s = 255 - It;
                  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(Rc, It);
                  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(Gc, It);
                  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(Bc, It);
                  /* Av = Af(1-It) + Ac*It */
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP], s) + PROD(Ac, It);
               }
               break;
            case GL_RGB:
               for (i=0;i<n;i++) {
                  /* Cv = Cf(1-Ct) + CcCt */
                  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], (255-texel[i][RCOMP])) + PROD(Rc,texel[i][RCOMP]);
                  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], (255-texel[i][GCOMP])) + PROD(Gc,texel[i][GCOMP]);
                  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], (255-texel[i][BCOMP])) + PROD(Bc,texel[i][BCOMP]);
                  /* Av = Af */
               }
               break;
            case GL_RGBA:
               for (i=0;i<n;i++) {
                  /* Cv = Cf(1-Ct) + CcCt */
                  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], (255-texel[i][RCOMP])) + PROD(Rc,texel[i][RCOMP]);
                  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], (255-texel[i][GCOMP])) + PROD(Gc,texel[i][GCOMP]);
                  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], (255-texel[i][BCOMP])) + PROD(Bc,texel[i][BCOMP]);
                  /* Av = AfAt */
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP],texel[i][ACOMP]);
               }
               break;
            default:
               gl_problem(ctx, "Bad format (GL_BLEND) in apply_texture");
               return;
         }
         break;

      case GL_ADD:  /* GL_EXT_texture_add_env */
         switch (format) {
            case GL_ALPHA:
               for (i=0;i<n;i++) {
                  /* Rv = Rf */
                  /* Gv = Gf */
                  /* Bv = Bf */
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP], texel[i][ACOMP]);
               }
               break;
            case GL_LUMINANCE:
               for (i=0;i<n;i++) {
                  GLuint Lt = texel[i][RCOMP];
                  GLuint r = rgba[i][RCOMP] + Lt;
                  GLuint g = rgba[i][GCOMP] + Lt;
                  GLuint b = rgba[i][BCOMP] + Lt;
                  rgba[i][RCOMP] = r < 256 ? (GLubyte) r : 255;
                  rgba[i][GCOMP] = g < 256 ? (GLubyte) g : 255;
                  rgba[i][BCOMP] = b < 256 ? (GLubyte) b : 255;
                  /* Av = Af */
               }
               break;
            case GL_LUMINANCE_ALPHA:
               for (i=0;i<n;i++) {
                  GLuint Lt = texel[i][RCOMP];
                  GLuint r = rgba[i][RCOMP] + Lt;
                  GLuint g = rgba[i][GCOMP] + Lt;
                  GLuint b = rgba[i][BCOMP] + Lt;
                  rgba[i][RCOMP] = r < 256 ? (GLubyte) r : 255;
                  rgba[i][GCOMP] = g < 256 ? (GLubyte) g : 255;
                  rgba[i][BCOMP] = b < 256 ? (GLubyte) b : 255;
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP], texel[i][ACOMP]);
               }
               break;
            case GL_INTENSITY:
               for (i=0;i<n;i++) {
                  GLubyte It = texel[i][RCOMP];
                  GLuint r = rgba[i][RCOMP] + It;
                  GLuint g = rgba[i][GCOMP] + It;
                  GLuint b = rgba[i][BCOMP] + It;
                  GLuint a = rgba[i][ACOMP] + It;
                  rgba[i][RCOMP] = r < 256 ? (GLubyte) r : 255;
                  rgba[i][GCOMP] = g < 256 ? (GLubyte) g : 255;
                  rgba[i][BCOMP] = b < 256 ? (GLubyte) b : 255;
                  rgba[i][ACOMP] = a < 256 ? (GLubyte) a : 255;
               }
               break;
            case GL_RGB:
               for (i=0;i<n;i++) {
                  GLuint r = rgba[i][RCOMP] + texel[i][RCOMP];
                  GLuint g = rgba[i][GCOMP] + texel[i][GCOMP];
                  GLuint b = rgba[i][BCOMP] + texel[i][BCOMP];
                  rgba[i][RCOMP] = r < 256 ? (GLubyte) r : 255;
                  rgba[i][GCOMP] = g < 256 ? (GLubyte) g : 255;
                  rgba[i][BCOMP] = b < 256 ? (GLubyte) b : 255;
                  /* Av = Af */
               }
               break;
            case GL_RGBA:
               for (i=0;i<n;i++) {
                  GLuint r = rgba[i][RCOMP] + texel[i][RCOMP];
                  GLuint g = rgba[i][GCOMP] + texel[i][GCOMP];
                  GLuint b = rgba[i][BCOMP] + texel[i][BCOMP];
                  rgba[i][RCOMP] = r < 256 ? (GLubyte) r : 255;
                  rgba[i][GCOMP] = g < 256 ? (GLubyte) g : 255;
                  rgba[i][BCOMP] = b < 256 ? (GLubyte) b : 255;
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP], texel[i][ACOMP]);
               }
               break;
            default:
               gl_problem(ctx, "Bad format (GL_ADD) in apply_texture");
               return;
         }
         break;

      default:
         gl_problem(ctx, "Bad env mode in apply_texture");
         return;
   }
#undef PROD
}



/*
 * Apply a unit of texture mapping to the incoming fragments.
 */
void gl_texture_pixels( GLcontext *ctx, GLuint texUnit, GLuint n,
                        const GLfloat s[], const GLfloat t[],
                        const GLfloat r[], GLfloat lambda[],
                        GLubyte rgba[][4] )
{
   GLuint mask = (TEXTURE0_1D | TEXTURE0_2D | TEXTURE0_3D) << (texUnit * 4);
   if (ctx->Texture.Enabled & mask) {
      const struct gl_texture_unit *textureUnit = &ctx->Texture.Unit[texUnit];
      if (textureUnit->Current && textureUnit->Current->SampleFunc) {
         GLubyte texel[PB_SIZE][4];

         if (textureUnit->LodBias != 0.0F) {
            /* apply LOD bias, but don't clamp yet */
            GLuint i;
            for (i=0;i<n;i++) {
               lambda[i] += textureUnit->LodBias;
            }
         }

         if (textureUnit->Current->MinLod != -1000.0
             || textureUnit->Current->MaxLod != 1000.0) {
            /* apply LOD clamping to lambda */
            GLfloat min = textureUnit->Current->MinLod;
            GLfloat max = textureUnit->Current->MaxLod;
            GLuint i;
            for (i=0;i<n;i++) {
               GLfloat l = lambda[i];
               lambda[i] = CLAMP(l, min, max);
            }
         }

         /* Sample the texture. */
         (*textureUnit->Current->SampleFunc)( textureUnit->Current, n,
                                             s, t, r, lambda, texel );

         apply_texture( ctx, textureUnit, n,
                        rgba, (const GLubyte (*)[4])texel );
      }
   }
}
