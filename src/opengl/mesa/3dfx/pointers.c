/* $Id: pointers.c,v 1.1 2000-02-29 00:46:23 sandervl Exp $ */

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include "GL/xf86glx.h"
#endif
#include "accum.h"
#include "alpha.h"
#include "attrib.h"
#include "bitmap.h"
#include "blend.h"
#include "clip.h"
#include "context.h"
#include "colortab.h"
#include "copypix.h"
#include "depth.h"
#include "dlist.h"
#include "drawpix.h"
#include "enable.h"
#include "eval.h"
#include "feedback.h"
#include "fog.h"
#include "get.h"
#include "glmisc.h"
#include "extensions.h"
#include "light.h"
#include "lines.h"
#include "logic.h"
#include "macros.h"
#include "masking.h"
#include "matrix.h"
#include "pixel.h"
#include "points.h"
#include "polygon.h"
#include "pointers.h"
#include "rastpos.h"
#include "readpix.h"
#include "rect.h"
#include "scissor.h"
#include "stencil.h"
#include "teximage.h"
#include "texobj.h"
#include "texstate.h"
#include "types.h"
#include "varray.h"
#include "vbfill.h"
#include "winpos.h"
#endif


#ifdef DEBUG
/*
 * For debugging
 */
static void check_pointers( struct gl_api_table *table )
{
   void **entry;
   int numentries = sizeof( struct gl_api_table ) / sizeof(void*);
   int i;

   entry = (void **) table;

   for (i=0;i<numentries;i++) {
      if (!entry[i]) {
         printf("found uninitialized function pointer at %d\n", i );
         gl_problem(NULL, "Missing pointer in pointers.c");
         /*abort()*/
      }
   }
}
#endif


/*
 * Assign all the pointers in 'table' to point to Mesa's immediate-mode
 * execution functions.
 */
static void init_execution_pointers( struct gl_api_table *table )
{
   table->Accum = gl_Accum;
   table->AlphaFunc = gl_AlphaFunc;
   table->AreTexturesResident = gl_AreTexturesResident;
   table->BindTexture = gl_BindTexture;
   table->Bitmap = gl_Bitmap;
   table->BlendColor = gl_BlendColor;
   table->BlendEquation = gl_BlendEquation;
   table->BlendFunc = gl_BlendFunc;
   table->BlendFuncSeparate = gl_BlendFuncSeparate;
   table->CallList = gl_CallList;
   table->CallLists = gl_CallLists;
   table->Clear = gl_Clear;
   table->ClearAccum = gl_ClearAccum;
   table->ClearColor = gl_ClearColor;
   table->ClearDepth = gl_ClearDepth;
   table->ClearIndex = gl_ClearIndex;
   table->ClearStencil = gl_ClearStencil;
   table->ClipPlane = gl_ClipPlane;
   table->ColorMask = gl_ColorMask;
   table->ColorMaterial = gl_ColorMaterial;
   table->ColorTable = gl_ColorTable;
   table->ColorSubTable = gl_ColorSubTable;
   table->CopyPixels = gl_CopyPixels;
   table->CopyTexImage1D = gl_CopyTexImage1D;
   table->CopyTexImage2D = gl_CopyTexImage2D;
   table->CopyTexSubImage1D = gl_CopyTexSubImage1D;
   table->CopyTexSubImage2D = gl_CopyTexSubImage2D;
   table->CopyTexSubImage3D = gl_CopyTexSubImage3D;
   table->CullFace = gl_CullFace;
   table->DeleteLists = gl_DeleteLists;
   table->DeleteTextures = gl_DeleteTextures;
   table->DepthFunc = gl_DepthFunc;
   table->DepthMask = gl_DepthMask;
   table->DepthRange = gl_DepthRange;
   table->Disable = gl_Disable;
   table->DisableClientState = gl_DisableClientState;
   table->DrawBuffer = gl_DrawBuffer;
   table->DrawPixels = gl_DrawPixels;
   table->Enable = gl_Enable;
   table->Error = gl_error;
   table->EnableClientState = gl_EnableClientState;
   table->EndList = gl_EndList;
   table->EvalMesh1 = gl_EvalMesh1;
   table->EvalMesh2 = gl_EvalMesh2;
   table->FeedbackBuffer = gl_FeedbackBuffer;
   table->Finish = gl_Finish;
   table->Flush = gl_Flush;
   table->Fogfv = gl_Fogfv;
   table->FrontFace = gl_FrontFace;
   table->Frustum = gl_Frustum;
   table->GenLists = gl_GenLists;
   table->GenTextures = gl_GenTextures;
   table->GetBooleanv = gl_GetBooleanv;
   table->GetClipPlane = gl_GetClipPlane;
   table->GetColorTable = gl_GetColorTable;
   table->GetColorTableParameteriv = gl_GetColorTableParameteriv;
   table->GetDoublev = gl_GetDoublev;
   table->GetError = gl_GetError;
   table->GetFloatv = gl_GetFloatv;
   table->GetIntegerv = gl_GetIntegerv;
   table->GetPointerv = gl_GetPointerv;
   table->GetLightfv = gl_GetLightfv;
   table->GetLightiv = gl_GetLightiv;
   table->GetMapdv = gl_GetMapdv;
   table->GetMapfv = gl_GetMapfv;
   table->GetMapiv = gl_GetMapiv;
   table->GetMaterialfv = gl_GetMaterialfv;
   table->GetMaterialiv = gl_GetMaterialiv;
   table->GetPixelMapfv = gl_GetPixelMapfv;
   table->GetPixelMapuiv = gl_GetPixelMapuiv;
   table->GetPixelMapusv = gl_GetPixelMapusv;
   table->GetPolygonStipple = gl_GetPolygonStipple;
   table->GetString = gl_GetString;
   table->GetTexEnvfv = gl_GetTexEnvfv;
   table->GetTexEnviv = gl_GetTexEnviv;
   table->GetTexGendv = gl_GetTexGendv;
   table->GetTexGenfv = gl_GetTexGenfv;
   table->GetTexGeniv = gl_GetTexGeniv;
   table->GetTexImage = gl_GetTexImage;
   table->GetTexLevelParameterfv = gl_GetTexLevelParameterfv;
   table->GetTexLevelParameteriv = gl_GetTexLevelParameteriv;
   table->GetTexParameterfv = gl_GetTexParameterfv;
   table->GetTexParameteriv = gl_GetTexParameteriv;
   table->Hint = gl_Hint;
   table->IndexMask = gl_IndexMask;
   table->InitNames = gl_InitNames;
   table->IsEnabled = gl_IsEnabled;
   table->IsList = gl_IsList;
   table->IsTexture = gl_IsTexture;
   table->LightModelfv = gl_LightModelfv;
   table->Lightfv = gl_Lightfv;
   table->LineStipple = gl_LineStipple;
   table->LineWidth = gl_LineWidth;
   table->ListBase = gl_ListBase;
   table->LoadIdentity = gl_LoadIdentity;
   table->LoadMatrixf = gl_LoadMatrixf;
   table->LoadName = gl_LoadName;
   table->LogicOp = gl_LogicOp;
   table->Map1f = gl_Map1f;
   table->Map2f = gl_Map2f;
   table->MapGrid1f = gl_MapGrid1f;
   table->MapGrid2f = gl_MapGrid2f;
   table->MatrixMode = gl_MatrixMode;
   table->MultMatrixf = gl_MultMatrixf;
   table->NewList = gl_NewList;
   table->Ortho = gl_Ortho;
   table->PassThrough = gl_PassThrough;
   table->PixelMapfv = gl_PixelMapfv;
   table->PixelStorei = gl_PixelStorei;
   table->PixelTransferf = gl_PixelTransferf;
   table->PixelZoom = gl_PixelZoom;
   table->PointSize = gl_PointSize;
   table->PolygonMode = gl_PolygonMode;
   table->PolygonOffset = gl_PolygonOffset;
   table->PolygonStipple = gl_PolygonStipple;
   table->PopAttrib = gl_PopAttrib;
   table->PopClientAttrib = gl_PopClientAttrib;
   table->PopMatrix = gl_PopMatrix;
   table->PopName = gl_PopName;
   table->PrioritizeTextures = gl_PrioritizeTextures;
   table->PushAttrib = gl_PushAttrib;
   table->PushClientAttrib = gl_PushClientAttrib;
   table->PushMatrix = gl_PushMatrix;
   table->PushName = gl_PushName;
   table->RasterPos4f = gl_RasterPos4f;
   table->ReadBuffer = gl_ReadBuffer;
   table->ReadPixels = gl_ReadPixels;
   table->Rectf = gl_Rectf;
   table->RenderMode = gl_RenderMode;
   table->Rotatef = gl_Rotatef;
   table->Scalef = gl_Scalef;
   table->Scissor = gl_Scissor;
   table->SelectBuffer = gl_SelectBuffer;
   table->ShadeModel = gl_ShadeModel;
   table->StencilFunc = gl_StencilFunc;
   table->StencilMask = gl_StencilMask;
   table->StencilOp = gl_StencilOp;
   table->TexEnvfv = gl_TexEnvfv;
   table->TexGenfv = gl_TexGenfv;
   table->TexImage1D = gl_TexImage1D;
   table->TexImage2D = gl_TexImage2D;
   table->TexImage3D = gl_TexImage3D;
   table->TexSubImage1D = gl_TexSubImage1D;
   table->TexSubImage2D = gl_TexSubImage2D;
   table->TexSubImage3D = gl_TexSubImage3D;
   table->PointParameterfvEXT = gl_PointParameterfvEXT;
   table->TexParameterfv = gl_TexParameterfv;
   table->Translatef = gl_Translatef;
   table->Viewport = gl_Viewport;

   /* GL_MESA_window_pos extension */
   table->WindowPos4fMESA = gl_WindowPos4fMESA;

   /* GL_MESA_resize_buffers extension */
   table->ResizeBuffersMESA = gl_ResizeBuffersMESA;

   /* GL_ARB_multitexture */
   table->ActiveTexture = gl_ActiveTexture;
   table->ClientActiveTexture = gl_ClientActiveTexture;
}




void gl_init_api_function_pointers( GLcontext *ctx )
{
   init_execution_pointers( &ctx->Exec );

   gl_init_dlist_pointers( &ctx->Save );

#ifdef DEBUG
   /* make sure there's no NULL pointers */
   check_pointers( &ctx->Exec );
   check_pointers( &ctx->Save );
#endif
}

