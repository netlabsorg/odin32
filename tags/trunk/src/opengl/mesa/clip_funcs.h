/* $Id: clip_funcs.h,v 1.2 2000-05-21 20:15:38 jeroen Exp $ */

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

static GLuint TAG(userclip_line)( struct vertex_buffer *VB,
                                  GLuint *i, GLuint *j );
static GLuint TAG(userclip_polygon)( struct vertex_buffer *VB,
                                     GLuint n, GLuint vlist[] );



/* This now calls the user clip functions if required.
 */
static GLuint TAG(viewclip_line)( struct vertex_buffer *VB,
                                  GLuint *i, GLuint *j,
                                  GLubyte mask )
{
   GLfloat (*coord)[4] = VB->ClipPtr->data;
   GLfloat t, dx, dy, dz, dw, neww;
   GLuint ii = *i, jj = *j;
   GLuint vlist[2];
   GLuint n;
   clip_interp_func interp = VB->ctx->ClipInterpFunc;
   GLuint vb_free = VB->FirstFree;

/*
 * We use 6 instances of this code to clip agains the 6 planes.
 * For each plane, we define the OUTSIDE and COMPUTE_INTERSECTION
 * macros apprpriately.
 */
#define GENERAL_CLIP                                    \
   if (mask & PLANE) {                                  \
      GLuint flagI = INSIDE( ii );                      \
      GLuint flagJ = INSIDE( jj );                      \
                                                        \
      if (!(flagI|flagJ))                               \
         return 0;                                      \
                                                        \
      if (flagI ^ flagJ) {                              \
         COMPUTE_INTERSECTION( jj, ii, vb_free );       \
         interp( VB, vb_free, t, jj, ii);               \
                                                        \
         if (flagI) {                                   \
            VB->ClipMask[jj] |= PLANE;                  \
            jj = vb_free;                               \
         } else {                                       \
            VB->ClipMask[ii] |= PLANE;                  \
            ii = vb_free;                               \
         }                                              \
                                                        \
         VB->ClipMask[vb_free++] = 0;                   \
      }                                                 \
   }

#include "general_clip.h"

   VB->Free = vb_free;

   if (mask & CLIP_USER_BIT) {
      if ( TAG(userclip_line)( VB, &ii, &jj ) == 0 )
         return 0;
   }

   vlist[0] = ii;
   vlist[1] = jj;
   n = 2;

   /* Should always be at least one new vertex.
    */
   {
      const GLfloat *m = VB->ctx->Viewport.WindowMap.m;
      GLfloat sx = m[MAT_SX];
      GLfloat tx = m[MAT_TX];
      GLfloat sy = m[MAT_SY];
      GLfloat ty = m[MAT_TY];
      GLfloat sz = m[MAT_SZ];
      GLfloat tz = m[MAT_TZ];
      GLuint i, j;
      GLfloat (*win)[4] = VB->Win.data;
      GLuint start = VB->FirstFree;

      for (i = 0; i < n; i++) {
         j = vlist[i];
         if (j >= start) {
            if (W(j) != 0.0F) {
               GLfloat wInv = 1.0F / W(j);
               win[j][0] = X(j) * wInv * sx + tx;
               win[j][1] = Y(j) * wInv * sy + ty;
               win[j][2] = Z(j) * wInv * sz + tz;
               win[j][3] = wInv;
            } else {
               win[j][0] = win[j][1] = win[j][2] = 0.0F;
               win[j][3] = 1.0F;
            }
         }
      }
   }

   if (VB->ctx->Driver.RasterSetup)
      VB->ctx->Driver.RasterSetup(VB, VB->FirstFree, VB->Free);

   *i = ii;
   *j = jj;
   return 1;
}

/* We now clip polygon triangles individually.  This is necessary to
 * avoid artifacts dependent on where the boundary of the VB falls
 * within the polygon.  As a result, there is an upper bound on the
 * number of vertices which may be created, and the test against VB_SIZE
 * is redundant.
 */
static GLuint TAG(viewclip_polygon)( struct vertex_buffer *VB,
                                     GLuint n, GLuint vlist[],
                                     GLubyte mask )
{
   GLfloat (*coord)[4] = VB->ClipPtr->data;
   GLuint vlist2[VB_SIZE-VB_MAX];
   GLuint *inlist = vlist, *outlist = vlist2;
   GLdouble dx, dy, dz, dw, t = 0, neww;
   GLuint i;
   GLuint vb_free = VB->FirstFree;
   clip_interp_func interp = VB->ctx->ClipInterpFunc;

   if (mask & CLIP_ALL_BITS) {

#define GENERAL_CLIP                                                    \
   if (mask & PLANE) {                                                  \
      GLuint prevj = inlist[0];                                         \
      GLuint prevflag = INSIDE(prevj);                                  \
      GLuint outcount = 0;                                              \
      GLuint i;                                                         \
                                                                        \
      inlist[n] = inlist[0];                                            \
                                                                        \
      for (i = 1; i < n+1; i++) {                                       \
         GLuint j = inlist[i];                                          \
         GLuint flag = INSIDE(j);                                       \
                                                                        \
         if (prevflag) {                                                \
            outlist[outcount++] = prevj;                                \
         } else {                                                       \
            VB->ClipMask[prevj] |= (PLANE&CLIP_ALL_BITS);               \
         }                                                              \
                                                                        \
         if (flag ^ prevflag) {                                         \
            if (flag) {                                                 \
               /* Coming back in                                        \
                */                                                      \
               COMPUTE_INTERSECTION( j, prevj, vb_free );               \
               interp( VB, vb_free, t, j, prevj );                      \
                                                                        \
               if (IND&CLIP_TAB_EDGEFLAG)                               \
                  VB->EdgeFlagPtr->data[vb_free] =                      \
                     VB->EdgeFlagPtr->data[prevj];                      \
                                                                        \
                                                                        \
            } else {                                                    \
               /* Going out of bounds                                   \
                */                                                      \
               COMPUTE_INTERSECTION( prevj, j, vb_free );               \
               interp( VB, vb_free, t, prevj, j );                      \
                                                                        \
               if (IND&CLIP_TAB_EDGEFLAG) {                             \
                  VB->EdgeFlagPtr->data[vb_free] = 3;                   \
               }                                                        \
            }                                                           \
                                                                        \
            if (IND&CLIP_TAB_EDGEFLAG) {                                \
               /* Demote trailing edge to internal edge.                \
                */                                                      \
               if (outcount &&                                          \
                   (VB->EdgeFlagPtr->data[outlist[outcount-1]] & 0x2))  \
                  VB->EdgeFlagPtr->data[outlist[outcount-1]] = 1;       \
            }                                                           \
                                                                        \
            outlist[outcount++] = vb_free;                              \
            VB->ClipMask[vb_free++] = 0;                                \
         }                                                              \
                                                                        \
         prevj = j;                                                     \
         prevflag = flag;                                               \
      }                                                                 \
                                                                        \
      if (outcount < 3)                                                 \
         return 0;                                                      \
      else {                                                            \
         GLuint *tmp = inlist;                                          \
         inlist = outlist;                                              \
         outlist = tmp;                                                 \
         n = outcount;                                                  \
      }                                                                 \
   }


#include "general_clip.h"

      if (inlist != vlist)
         for (i = 0 ; i < n ; i++)
            vlist[i] = inlist[i];
   }

   VB->Free = vb_free;

   /* Clip against user clipping planes in clip space.
    */
   if (mask & CLIP_USER_BIT) {
      n = TAG(userclip_polygon)( VB, n, vlist );
      if (n < 3) return 0;
   }


   {
      const GLfloat *m = VB->ctx->Viewport.WindowMap.m;
      GLfloat sx = m[MAT_SX];
      GLfloat tx = m[MAT_TX];
      GLfloat sy = m[MAT_SY];
      GLfloat ty = m[MAT_TY];
      GLfloat sz = m[MAT_SZ];
      GLfloat tz = m[MAT_TZ];
      GLuint i;
      GLfloat (*win)[4] = VB->Win.data;
      GLuint first = VB->FirstFree;

      for (i = 0; i < n; i++) {
         GLuint j = vlist[i];
         if (j >= first) {
            if (W(j) != 0.0F) {
               GLfloat wInv = 1.0F / W(j);
               win[j][0] = X(j) * wInv * sx + tx;
               win[j][1] = Y(j) * wInv * sy + ty;
               win[j][2] = Z(j) * wInv * sz + tz;
               win[j][3] = wInv;
            } else {
               win[j][0] = win[j][1] = win[j][2] = 0.0;
               win[j][3] = 1.0F;
            }
         }
      }
   }

   if (VB->ctx->Driver.RasterSetup)
      VB->ctx->Driver.RasterSetup(VB, VB->FirstFree, VB->Free);

   return n;
}


#define INSIDE( J ) !NEGATIVE(J)
#define OUTSIDE( J ) NEGATIVE(J)




static GLuint TAG(userclip_line)( struct vertex_buffer *VB,
                                  GLuint *i, GLuint *j )
{
   GLcontext *ctx = VB->ctx;
   GLfloat (*coord)[4] = VB->ClipPtr->data;
   clip_interp_func interp = ctx->ClipInterpFunc;
   GLuint ii = *i;
   GLuint jj = *j;
   GLuint p;
   GLuint vb_free = VB->Free;

   for (p=0;p<MAX_CLIP_PLANES;p++) {
      if (ctx->Transform.ClipEnabled[p]) {
         GLfloat a = ctx->Transform.ClipUserPlane[p][0];
         GLfloat b = ctx->Transform.ClipUserPlane[p][1];
         GLfloat c = ctx->Transform.ClipUserPlane[p][2];
         GLfloat d = ctx->Transform.ClipUserPlane[p][3];

         GLfloat dpI = d*W(ii) + c*Z(ii) + b*Y(ii) + a*X(ii);
         GLfloat dpJ = d*W(jj) + c*Z(jj) + b*Y(jj) + a*X(jj);

         GLuint flagI = OUTSIDE( dpI );
         GLuint flagJ = OUTSIDE( dpJ );

         if (flagI & flagJ)
            return 0;

         if (flagI ^ flagJ) {
            GLfloat t = -dpI/(dpJ-dpI);

            INTERP_SZ( t, coord, vb_free, ii, jj, SIZE );
            interp( VB, vb_free, t, ii, jj );

            if (flagI) {
               VB->ClipMask[ii] |= CLIP_USER_BIT;
               ii = vb_free;
            } else {
               VB->ClipMask[jj] |= CLIP_USER_BIT;
               jj = vb_free;
            }

            VB->ClipMask[vb_free++] = 0;
         }
      }
   }

   VB->Free = vb_free;
   *i = ii;
   *j = jj;
   return 1;
}


static GLuint TAG(userclip_polygon)( struct vertex_buffer *VB,
                                     GLuint n, GLuint vlist[] )
{
   GLcontext *ctx = VB->ctx;
   GLfloat (*coord)[4] = VB->ClipPtr->data;
   clip_interp_func interp = ctx->ClipInterpFunc;
   GLuint vlist2[VB_SIZE];
   GLuint *inlist = vlist, *outlist = vlist2;
   GLuint vb_free = VB->Free;
   GLuint p;

   for (p=0;p<MAX_CLIP_PLANES;p++) {
      if (ctx->Transform.ClipEnabled[p]) {
         register float a = ctx->Transform.ClipUserPlane[p][0];
         register float b = ctx->Transform.ClipUserPlane[p][1];
         register float c = ctx->Transform.ClipUserPlane[p][2];
         register float d = ctx->Transform.ClipUserPlane[p][3];

         /* initialize prev to be last in the input list */
         GLuint prevj = inlist[0];
         GLfloat dpJ = d*W(prevj) + c*Z(prevj) + b*Y(prevj) + a*X(prevj);
         GLuint flagJ = INSIDE(dpJ);
         GLuint outcount = 0;
         GLuint curri;

         inlist[n] = inlist[0];


         for (curri=1;curri<n+1;curri++) {
            GLuint currj = inlist[curri];
            GLfloat dpI = d*W(currj) + c*Z(currj) + b*Y(currj) + a*X(currj);
            GLuint flagI = INSIDE(dpI);

            if (flagJ) {
               outlist[outcount++] = prevj;
            } else {
               VB->ClipMask[prevj] |= CLIP_USER_BIT;
            }

            if (flagI ^ flagJ) {
               GLfloat t;
               GLuint in;
               GLuint out;

               if (flagI) {
                  out = prevj;
                  in = currj;
                  t = dpI/(dpI-dpJ);

                  if (IND&CLIP_TAB_EDGEFLAG)
                     VB->EdgeFlagPtr->data[vb_free] =
                        VB->EdgeFlagPtr->data[prevj];
               } else {
                  in = prevj;
                  out = currj;
                  t = dpJ/(dpJ-dpI);

                  if (IND&CLIP_TAB_EDGEFLAG)
                     VB->EdgeFlagPtr->data[vb_free] = 1;
               }

               if (IND&CLIP_TAB_EDGEFLAG) {
                  /* Demote trailing edge to internal edge.
                   */
                  if (outcount &&
                      (VB->EdgeFlagPtr->data[outlist[outcount-1]] & 0x2))
                     VB->EdgeFlagPtr->data[outlist[outcount-1]] = 1;
               }

               INTERP_SZ( t, coord, vb_free, in, out, SIZE );
               interp( VB, vb_free, t, in, out);
               outlist[outcount++] = vb_free;
               VB->ClipMask[vb_free++] = 0;
            }

            prevj = currj;
            flagJ = flagI;
            dpJ = dpI;
         }

         if (outcount < 3)
            return 0;
         else {
            GLuint *tmp;
            tmp = inlist;
            inlist = outlist;
            outlist = tmp;
            n = outcount;
         }

      } /* if */
   } /* for p */

   if (inlist!=vlist) {
      GLuint i;
      for (i = 0 ; i < n ; i++)
         vlist[i] = inlist[i];
   }

   VB->Free = vb_free;
   return n;
}


static void TAG(init_clip_funcs)(void)
{
   gl_poly_clip_tab[IND][SIZE] = TAG(viewclip_polygon);
   gl_line_clip_tab[IND][SIZE] = TAG(viewclip_line);
}

#undef W
#undef Z
#undef Y
#undef X
#undef SIZE
#undef TAG
#undef INSIDE
#undef OUTSIDE
#undef IND
