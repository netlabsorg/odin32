#ifndef __gl__h_
#ifndef __GL__H__

#define __gl__h_
#define __GL__H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** Copyright 1996 Silicon Graphics, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
*/

#ifdef __WIN32OS2__
#include <os2win.h>
#endif

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef long GLint;
typedef long GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

/*************************************************************/

/* Mesa-stuff */
#define MESA
#define MESA_MAJOR_VERSION                3
#define MESA_MINOR_VERSION                1


/* Version */
#define GL_VERSION_1_1                    1
#define GL_VERSION_1_2                    1

/* AccumOp */
#define GL_ACCUM                          0x0100
#define GL_LOAD                           0x0101
#define GL_RETURN                         0x0102
#define GL_MULT                           0x0103
#define GL_ADD                            0x0104

/* AlphaFunction */
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207

/* AttribMask */
#define GL_CURRENT_BIT                    0x00000001
#define GL_POINT_BIT                      0x00000002
#define GL_LINE_BIT                       0x00000004
#define GL_POLYGON_BIT                    0x00000008
#define GL_POLYGON_STIPPLE_BIT            0x00000010
#define GL_PIXEL_MODE_BIT                 0x00000020
#define GL_LIGHTING_BIT                   0x00000040
#define GL_FOG_BIT                        0x00000080
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_ACCUM_BUFFER_BIT               0x00000200
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_VIEWPORT_BIT                   0x00000800
#define GL_TRANSFORM_BIT                  0x00001000
#define GL_ENABLE_BIT                     0x00002000
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_HINT_BIT                       0x00008000
#define GL_EVAL_BIT                       0x00010000
#define GL_LIST_BIT                       0x00020000
#define GL_TEXTURE_BIT                    0x00040000
#define GL_SCISSOR_BIT                    0x00080000
#define GL_ALL_ATTRIB_BITS                0x000fffff

/* BeginMode */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_QUAD_STRIP                     0x0008
#define GL_POLYGON                        0x0009

/* BlendingFactorDest */
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305

/* BlendingFactorSrc */
/*      GL_ZERO */
/*      GL_ONE */
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
/*      GL_SRC_ALPHA */
/*      GL_ONE_MINUS_SRC_ALPHA */
/*      GL_DST_ALPHA */
/*      GL_ONE_MINUS_DST_ALPHA */

/* Boolean */
#define GL_TRUE                           1
#define GL_FALSE                          0

/* ClearBufferMask */
/*      GL_COLOR_BUFFER_BIT */
/*      GL_ACCUM_BUFFER_BIT */
/*      GL_STENCIL_BUFFER_BIT */
/*      GL_DEPTH_BUFFER_BIT */

/* ClientArrayType */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */

/* ClipPlaneName */
#define GL_CLIP_PLANE0                    0x3000
#define GL_CLIP_PLANE1                    0x3001
#define GL_CLIP_PLANE2                    0x3002
#define GL_CLIP_PLANE3                    0x3003
#define GL_CLIP_PLANE4                    0x3004
#define GL_CLIP_PLANE5                    0x3005

/* ColorMaterialFace */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* ColorMaterialParameter */
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */
/*      GL_EMISSION */
/*      GL_AMBIENT_AND_DIFFUSE */

/* ColorPointerType */
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* CullFaceMode */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* DataType */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_2_BYTES                        0x1407
#define GL_3_BYTES                        0x1408
#define GL_4_BYTES                        0x1409
#define GL_DOUBLE                         0x140A

/* DepthFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* DrawBufferMode */
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
#define GL_AUX0                           0x0409
#define GL_AUX1                           0x040A
#define GL_AUX2                           0x040B
#define GL_AUX3                           0x040C

/* Enable */
/*      GL_FOG */
/*      GL_LIGHTING */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_LINE_STIPPLE */
/*      GL_POLYGON_STIPPLE */
/*      GL_CULL_FACE */
/*      GL_ALPHA_TEST */
/*      GL_BLEND */
/*      GL_INDEX_LOGIC_OP */
/*      GL_COLOR_LOGIC_OP */
/*      GL_DITHER */
/*      GL_STENCIL_TEST */
/*      GL_DEPTH_TEST */
/*      GL_CLIP_PLANE0 */
/*      GL_CLIP_PLANE1 */
/*      GL_CLIP_PLANE2 */
/*      GL_CLIP_PLANE3 */
/*      GL_CLIP_PLANE4 */
/*      GL_CLIP_PLANE5 */
/*      GL_LIGHT0 */
/*      GL_LIGHT1 */
/*      GL_LIGHT2 */
/*      GL_LIGHT3 */
/*      GL_LIGHT4 */
/*      GL_LIGHT5 */
/*      GL_LIGHT6 */
/*      GL_LIGHT7 */
/*      GL_TEXTURE_GEN_S */
/*      GL_TEXTURE_GEN_T */
/*      GL_TEXTURE_GEN_R */
/*      GL_TEXTURE_GEN_Q */
/*      GL_MAP1_VERTEX_3 */
/*      GL_MAP1_VERTEX_4 */
/*      GL_MAP1_COLOR_4 */
/*      GL_MAP1_INDEX */
/*      GL_MAP1_NORMAL */
/*      GL_MAP1_TEXTURE_COORD_1 */
/*      GL_MAP1_TEXTURE_COORD_2 */
/*      GL_MAP1_TEXTURE_COORD_3 */
/*      GL_MAP1_TEXTURE_COORD_4 */
/*      GL_MAP2_VERTEX_3 */
/*      GL_MAP2_VERTEX_4 */
/*      GL_MAP2_COLOR_4 */
/*      GL_MAP2_INDEX */
/*      GL_MAP2_NORMAL */
/*      GL_MAP2_TEXTURE_COORD_1 */
/*      GL_MAP2_TEXTURE_COORD_2 */
/*      GL_MAP2_TEXTURE_COORD_3 */
/*      GL_MAP2_TEXTURE_COORD_4 */
/*      GL_POINT_SMOOTH */
/*      GL_LINE_SMOOTH */
/*      GL_POLYGON_SMOOTH */
/*      GL_SCISSOR_TEST */
/*      GL_COLOR_MATERIAL */
/*      GL_NORMALIZE */
/*      GL_AUTO_NORMAL */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */
/*      GL_POLYGON_OFFSET_POINT */
/*      GL_POLYGON_OFFSET_LINE */
/*      GL_POLYGON_OFFSET_FILL */

/* ErrorCode */
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY                  0x0505

/* FeedBackMode */
#define GL_2D                             0x0600
#define GL_3D                             0x0601
#define GL_3D_COLOR                       0x0602
#define GL_3D_COLOR_TEXTURE               0x0603
#define GL_4D_COLOR_TEXTURE               0x0604

/* FeedBackToken */
#define GL_PASS_THROUGH_TOKEN             0x0700
#define GL_POINT_TOKEN                    0x0701
#define GL_LINE_TOKEN                     0x0702
#define GL_POLYGON_TOKEN                  0x0703
#define GL_BITMAP_TOKEN                   0x0704
#define GL_DRAW_PIXEL_TOKEN               0x0705
#define GL_COPY_PIXEL_TOKEN               0x0706
#define GL_LINE_RESET_TOKEN               0x0707

/* FogMode */
/*      GL_LINEAR */
#define GL_EXP                            0x0800
#define GL_EXP2                           0x0801

/* FogParameter */
/*      GL_FOG_COLOR */
/*      GL_FOG_DENSITY */
/*      GL_FOG_END */
/*      GL_FOG_INDEX */
/*      GL_FOG_MODE */
/*      GL_FOG_START */

/* FrontFaceDirection */
#define GL_CW                             0x0900
#define GL_CCW                            0x0901

/* GetMapTarget */
#define GL_COEFF                          0x0A00
#define GL_ORDER                          0x0A01
#define GL_DOMAIN                         0x0A02

/* GetPixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* GetPointerTarget */
/*      GL_VERTEX_ARRAY_POINTER */
/*      GL_NORMAL_ARRAY_POINTER */
/*      GL_COLOR_ARRAY_POINTER */
/*      GL_INDEX_ARRAY_POINTER */
/*      GL_TEXTURE_COORD_ARRAY_POINTER */
/*      GL_EDGE_FLAG_ARRAY_POINTER */

/* GetTarget */
#define GL_CURRENT_COLOR                  0x0B00
#define GL_CURRENT_INDEX                  0x0B01
#define GL_CURRENT_NORMAL                 0x0B02
#define GL_CURRENT_TEXTURE_COORDS         0x0B03
#define GL_CURRENT_RASTER_COLOR           0x0B04
#define GL_CURRENT_RASTER_INDEX           0x0B05
#define GL_CURRENT_RASTER_TEXTURE_COORDS  0x0B06
#define GL_CURRENT_RASTER_POSITION        0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID  0x0B08
#define GL_CURRENT_RASTER_DISTANCE        0x0B09
#define GL_POINT_SMOOTH                   0x0B10
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_LINE_STIPPLE                   0x0B24
#define GL_LINE_STIPPLE_PATTERN           0x0B25
#define GL_LINE_STIPPLE_REPEAT            0x0B26
#define GL_LIST_MODE                      0x0B30
#define GL_MAX_LIST_NESTING               0x0B31
#define GL_LIST_BASE                      0x0B32
#define GL_LIST_INDEX                     0x0B33
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_POLYGON_STIPPLE                0x0B42
#define GL_EDGE_FLAG                      0x0B43
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_LIGHTING                       0x0B50
#define GL_LIGHT_MODEL_LOCAL_VIEWER       0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE           0x0B52
#define GL_LIGHT_MODEL_AMBIENT            0x0B53
#define GL_SHADE_MODEL                    0x0B54
#define GL_COLOR_MATERIAL_FACE            0x0B55
#define GL_COLOR_MATERIAL_PARAMETER       0x0B56
#define GL_COLOR_MATERIAL                 0x0B57
#define GL_FOG                            0x0B60
#define GL_FOG_INDEX                      0x0B61
#define GL_FOG_DENSITY                    0x0B62
#define GL_FOG_START                      0x0B63
#define GL_FOG_END                        0x0B64
#define GL_FOG_MODE                       0x0B65
#define GL_FOG_COLOR                      0x0B66
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_ACCUM_CLEAR_VALUE              0x0B80
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_MATRIX_MODE                    0x0BA0
#define GL_NORMALIZE                      0x0BA1
#define GL_VIEWPORT                       0x0BA2
#define GL_MODELVIEW_STACK_DEPTH          0x0BA3
#define GL_PROJECTION_STACK_DEPTH         0x0BA4
#define GL_TEXTURE_STACK_DEPTH            0x0BA5
#define GL_MODELVIEW_MATRIX               0x0BA6
#define GL_PROJECTION_MATRIX              0x0BA7
#define GL_TEXTURE_MATRIX                 0x0BA8
#define GL_ATTRIB_STACK_DEPTH             0x0BB0
#define GL_CLIENT_ATTRIB_STACK_DEPTH      0x0BB1
#define GL_ALPHA_TEST                     0x0BC0
#define GL_ALPHA_TEST_FUNC                0x0BC1
#define GL_ALPHA_TEST_REF                 0x0BC2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_INDEX_LOGIC_OP                 0x0BF1
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_AUX_BUFFERS                    0x0C00
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_INDEX_CLEAR_VALUE              0x0C20
#define GL_INDEX_WRITEMASK                0x0C21
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_INDEX_MODE                     0x0C30
#define GL_RGBA_MODE                      0x0C31
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_RENDER_MODE                    0x0C40
#define GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#define GL_POINT_SMOOTH_HINT              0x0C51
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_FOG_HINT                       0x0C54
#define GL_TEXTURE_GEN_S                  0x0C60
#define GL_TEXTURE_GEN_T                  0x0C61
#define GL_TEXTURE_GEN_R                  0x0C62
#define GL_TEXTURE_GEN_Q                  0x0C63
#define GL_PIXEL_MAP_I_TO_I               0x0C70
#define GL_PIXEL_MAP_S_TO_S               0x0C71
#define GL_PIXEL_MAP_I_TO_R               0x0C72
#define GL_PIXEL_MAP_I_TO_G               0x0C73
#define GL_PIXEL_MAP_I_TO_B               0x0C74
#define GL_PIXEL_MAP_I_TO_A               0x0C75
#define GL_PIXEL_MAP_R_TO_R               0x0C76
#define GL_PIXEL_MAP_G_TO_G               0x0C77
#define GL_PIXEL_MAP_B_TO_B               0x0C78
#define GL_PIXEL_MAP_A_TO_A               0x0C79
#define GL_PIXEL_MAP_I_TO_I_SIZE          0x0CB0
#define GL_PIXEL_MAP_S_TO_S_SIZE          0x0CB1
#define GL_PIXEL_MAP_I_TO_R_SIZE          0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE          0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE          0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE          0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE          0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE          0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE          0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE          0x0CB9
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAP_COLOR                      0x0D10
#define GL_MAP_STENCIL                    0x0D11
#define GL_INDEX_SHIFT                    0x0D12
#define GL_INDEX_OFFSET                   0x0D13
#define GL_RED_SCALE                      0x0D14
#define GL_RED_BIAS                       0x0D15
#define GL_ZOOM_X                         0x0D16
#define GL_ZOOM_Y                         0x0D17
#define GL_GREEN_SCALE                    0x0D18
#define GL_GREEN_BIAS                     0x0D19
#define GL_BLUE_SCALE                     0x0D1A
#define GL_BLUE_BIAS                      0x0D1B
#define GL_ALPHA_SCALE                    0x0D1C
#define GL_ALPHA_BIAS                     0x0D1D
#define GL_DEPTH_SCALE                    0x0D1E
#define GL_DEPTH_BIAS                     0x0D1F
#define GL_MAX_EVAL_ORDER                 0x0D30
#define GL_MAX_LIGHTS                     0x0D31
#define GL_MAX_CLIP_PLANES                0x0D32
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_PIXEL_MAP_TABLE            0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH         0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH      0x0D36
#define GL_MAX_NAME_STACK_DEPTH           0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH     0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH        0x0D39
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH  0x0D3B
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_INDEX_BITS                     0x0D51
#define GL_RED_BITS                       0x0D52
#define GL_GREEN_BITS                     0x0D53
#define GL_BLUE_BITS                      0x0D54
#define GL_ALPHA_BITS                     0x0D55
#define GL_DEPTH_BITS                     0x0D56
#define GL_STENCIL_BITS                   0x0D57
#define GL_ACCUM_RED_BITS                 0x0D58
#define GL_ACCUM_GREEN_BITS               0x0D59
#define GL_ACCUM_BLUE_BITS                0x0D5A
#define GL_ACCUM_ALPHA_BITS               0x0D5B
#define GL_NAME_STACK_DEPTH               0x0D70
#define GL_AUTO_NORMAL                    0x0D80
#define GL_MAP1_COLOR_4                   0x0D90
#define GL_MAP1_INDEX                     0x0D91
#define GL_MAP1_NORMAL                    0x0D92
#define GL_MAP1_TEXTURE_COORD_1           0x0D93
#define GL_MAP1_TEXTURE_COORD_2           0x0D94
#define GL_MAP1_TEXTURE_COORD_3           0x0D95
#define GL_MAP1_TEXTURE_COORD_4           0x0D96
#define GL_MAP1_VERTEX_3                  0x0D97
#define GL_MAP1_VERTEX_4                  0x0D98
#define GL_MAP2_COLOR_4                   0x0DB0
#define GL_MAP2_INDEX                     0x0DB1
#define GL_MAP2_NORMAL                    0x0DB2
#define GL_MAP2_TEXTURE_COORD_1           0x0DB3
#define GL_MAP2_TEXTURE_COORD_2           0x0DB4
#define GL_MAP2_TEXTURE_COORD_3           0x0DB5
#define GL_MAP2_TEXTURE_COORD_4           0x0DB6
#define GL_MAP2_VERTEX_3                  0x0DB7
#define GL_MAP2_VERTEX_4                  0x0DB8
#define GL_MAP1_GRID_DOMAIN               0x0DD0
#define GL_MAP1_GRID_SEGMENTS             0x0DD1
#define GL_MAP2_GRID_DOMAIN               0x0DD2
#define GL_MAP2_GRID_SEGMENTS             0x0DD3
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_FEEDBACK_BUFFER_POINTER        0x0DF0
#define GL_FEEDBACK_BUFFER_SIZE           0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE           0x0DF2
#define GL_SELECTION_BUFFER_POINTER       0x0DF3
#define GL_SELECTION_BUFFER_SIZE          0x0DF4
/*      GL_TEXTURE_BINDING_1D */
/*      GL_TEXTURE_BINDING_2D */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */
/*      GL_VERTEX_ARRAY_SIZE */
/*      GL_VERTEX_ARRAY_TYPE */
/*      GL_VERTEX_ARRAY_STRIDE */
/*      GL_NORMAL_ARRAY_TYPE */
/*      GL_NORMAL_ARRAY_STRIDE */
/*      GL_COLOR_ARRAY_SIZE */
/*      GL_COLOR_ARRAY_TYPE */
/*      GL_COLOR_ARRAY_STRIDE */
/*      GL_INDEX_ARRAY_TYPE */
/*      GL_INDEX_ARRAY_STRIDE */
/*      GL_TEXTURE_COORD_ARRAY_SIZE */
/*      GL_TEXTURE_COORD_ARRAY_TYPE */
/*      GL_TEXTURE_COORD_ARRAY_STRIDE */
/*      GL_EDGE_FLAG_ARRAY_STRIDE */
/*      GL_POLYGON_OFFSET_FACTOR */
/*      GL_POLYGON_OFFSET_UNITS */

/* GetTextureParameter */
/*      GL_TEXTURE_MAG_FILTER */
/*      GL_TEXTURE_MIN_FILTER */
/*      GL_TEXTURE_WRAP_S */
/*      GL_TEXTURE_WRAP_T */
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_TEXTURE_BORDER                 0x1005
/*      GL_TEXTURE_RED_SIZE */
/*      GL_TEXTURE_GREEN_SIZE */
/*      GL_TEXTURE_BLUE_SIZE */
/*      GL_TEXTURE_ALPHA_SIZE */
/*      GL_TEXTURE_LUMINANCE_SIZE */
/*      GL_TEXTURE_INTENSITY_SIZE */
/*      GL_TEXTURE_PRIORITY */
/*      GL_TEXTURE_RESIDENT */

/* HintMode */
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102

/* HintTarget */
/*      GL_PERSPECTIVE_CORRECTION_HINT */
/*      GL_POINT_SMOOTH_HINT */
/*      GL_LINE_SMOOTH_HINT */
/*      GL_POLYGON_SMOOTH_HINT */
/*      GL_FOG_HINT */

/* IndexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* LightModelParameter */
/*      GL_LIGHT_MODEL_AMBIENT */
/*      GL_LIGHT_MODEL_LOCAL_VIEWER */
/*      GL_LIGHT_MODEL_TWO_SIDE */

/* LightName */
#define GL_LIGHT0                         0x4000
#define GL_LIGHT1                         0x4001
#define GL_LIGHT2                         0x4002
#define GL_LIGHT3                         0x4003
#define GL_LIGHT4                         0x4004
#define GL_LIGHT5                         0x4005
#define GL_LIGHT6                         0x4006
#define GL_LIGHT7                         0x4007

/* LightParameter */
#define GL_AMBIENT                        0x1200
#define GL_DIFFUSE                        0x1201
#define GL_SPECULAR                       0x1202
#define GL_POSITION                       0x1203
#define GL_SPOT_DIRECTION                 0x1204
#define GL_SPOT_EXPONENT                  0x1205
#define GL_SPOT_CUTOFF                    0x1206
#define GL_CONSTANT_ATTENUATION           0x1207
#define GL_LINEAR_ATTENUATION             0x1208
#define GL_QUADRATIC_ATTENUATION          0x1209

/* InterleavedArrays */
/*      GL_V2F */
/*      GL_V3F */
/*      GL_C4UB_V2F */
/*      GL_C4UB_V3F */
/*      GL_C3F_V3F */
/*      GL_N3F_V3F */
/*      GL_C4F_N3F_V3F */
/*      GL_T2F_V3F */
/*      GL_T4F_V4F */
/*      GL_T2F_C4UB_V3F */
/*      GL_T2F_C3F_V3F */
/*      GL_T2F_N3F_V3F */
/*      GL_T2F_C4F_N3F_V3F */
/*      GL_T4F_C4F_N3F_V4F */

/* ListMode */
#define GL_COMPILE                        0x1300
#define GL_COMPILE_AND_EXECUTE            0x1301

/* ListNameType */
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */
/*      GL_2_BYTES */
/*      GL_3_BYTES */
/*      GL_4_BYTES */

/* LogicOp */
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F

/* MapTarget */
/*      GL_MAP1_COLOR_4 */
/*      GL_MAP1_INDEX */
/*      GL_MAP1_NORMAL */
/*      GL_MAP1_TEXTURE_COORD_1 */
/*      GL_MAP1_TEXTURE_COORD_2 */
/*      GL_MAP1_TEXTURE_COORD_3 */
/*      GL_MAP1_TEXTURE_COORD_4 */
/*      GL_MAP1_VERTEX_3 */
/*      GL_MAP1_VERTEX_4 */
/*      GL_MAP2_COLOR_4 */
/*      GL_MAP2_INDEX */
/*      GL_MAP2_NORMAL */
/*      GL_MAP2_TEXTURE_COORD_1 */
/*      GL_MAP2_TEXTURE_COORD_2 */
/*      GL_MAP2_TEXTURE_COORD_3 */
/*      GL_MAP2_TEXTURE_COORD_4 */
/*      GL_MAP2_VERTEX_3 */
/*      GL_MAP2_VERTEX_4 */

/* MaterialFace */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* MaterialParameter */
#define GL_EMISSION                       0x1600
#define GL_SHININESS                      0x1601
#define GL_AMBIENT_AND_DIFFUSE            0x1602
#define GL_COLOR_INDEXES                  0x1603
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */

/* MatrixMode */
#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701
#define GL_TEXTURE                        0x1702

/* MeshMode1 */
/*      GL_POINT */
/*      GL_LINE */

/* MeshMode2 */
/*      GL_POINT */
/*      GL_LINE */
/*      GL_FILL */

/* NormalPointerType */
/*      GL_BYTE */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* PixelCopyType */
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802

/* PixelFormat */
#define GL_COLOR_INDEX                    0x1900
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A

/* PixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* PixelStore */
/*      GL_UNPACK_SWAP_BYTES */
/*      GL_UNPACK_LSB_FIRST */
/*      GL_UNPACK_ROW_LENGTH */
/*      GL_UNPACK_SKIP_ROWS */
/*      GL_UNPACK_SKIP_PIXELS */
/*      GL_UNPACK_ALIGNMENT */
/*      GL_PACK_SWAP_BYTES */
/*      GL_PACK_LSB_FIRST */
/*      GL_PACK_ROW_LENGTH */
/*      GL_PACK_SKIP_ROWS */
/*      GL_PACK_SKIP_PIXELS */
/*      GL_PACK_ALIGNMENT */

/* PixelTransfer */
/*      GL_MAP_COLOR */
/*      GL_MAP_STENCIL */
/*      GL_INDEX_SHIFT */
/*      GL_INDEX_OFFSET */
/*      GL_RED_SCALE */
/*      GL_RED_BIAS */
/*      GL_GREEN_SCALE */
/*      GL_GREEN_BIAS */
/*      GL_BLUE_SCALE */
/*      GL_BLUE_BIAS */
/*      GL_ALPHA_SCALE */
/*      GL_ALPHA_BIAS */
/*      GL_DEPTH_SCALE */
/*      GL_DEPTH_BIAS */

/* PixelType */
#define GL_BITMAP                         0x1A00
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */

/* PolygonMode */
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02

/* ReadBufferMode */
/*      GL_FRONT_LEFT */
/*      GL_FRONT_RIGHT */
/*      GL_BACK_LEFT */
/*      GL_BACK_RIGHT */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_LEFT */
/*      GL_RIGHT */
/*      GL_AUX0 */
/*      GL_AUX1 */
/*      GL_AUX2 */
/*      GL_AUX3 */

/* RenderingMode */
#define GL_RENDER                         0x1C00
#define GL_FEEDBACK                       0x1C01
#define GL_SELECT                         0x1C02

/* ShadingModel */
#define GL_FLAT                           0x1D00
#define GL_SMOOTH                         0x1D01

/* StencilFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* StencilOp */
/*      GL_ZERO */
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
/*      GL_INVERT */

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

/* TextureCoordName */
#define GL_S                              0x2000
#define GL_T                              0x2001
#define GL_R                              0x2002
#define GL_Q                              0x2003

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* TextureEnvMode */
#define GL_MODULATE                       0x2100
#define GL_DECAL                          0x2101
/*      GL_BLEND */
/*      GL_REPLACE */

/* TextureEnvParameter */
#define GL_TEXTURE_ENV_MODE               0x2200
#define GL_TEXTURE_ENV_COLOR              0x2201

/* TextureEnvTarget */
#define GL_TEXTURE_ENV                    0x2300

/* TextureGenMode */
#define GL_EYE_LINEAR                     0x2400
#define GL_OBJECT_LINEAR                  0x2401
#define GL_SPHERE_MAP                     0x2402

/* TextureGenParameter */
#define GL_TEXTURE_GEN_MODE               0x2500
#define GL_OBJECT_PLANE                   0x2501
#define GL_EYE_PLANE                      0x2502

/* TextureMagFilter */
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601

/* TextureMinFilter */
/*      GL_NEAREST */
/*      GL_LINEAR */
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703

/* TextureParameterName */
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
/*      GL_TEXTURE_BORDER_COLOR */
/*      GL_TEXTURE_PRIORITY */

/* TextureTarget */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_PROXY_TEXTURE_1D */
/*      GL_PROXY_TEXTURE_2D */

/* TextureWrapMode */
#define GL_CLAMP                          0x2900
#define GL_REPEAT                         0x2901

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* ClientAttribMask */
#define GL_CLIENT_PIXEL_STORE_BIT         0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT        0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS         0xffffffff

/* polygon_offset */
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037

/* texture */
#define GL_ALPHA4                         0x803B
#define GL_ALPHA8                         0x803C
#define GL_ALPHA12                        0x803D
#define GL_ALPHA16                        0x803E
#define GL_LUMINANCE4                     0x803F
#define GL_LUMINANCE8                     0x8040
#define GL_LUMINANCE12                    0x8041
#define GL_LUMINANCE16                    0x8042
#define GL_LUMINANCE4_ALPHA4              0x8043
#define GL_LUMINANCE6_ALPHA2              0x8044
#define GL_LUMINANCE8_ALPHA8              0x8045
#define GL_LUMINANCE12_ALPHA4             0x8046
#define GL_LUMINANCE12_ALPHA12            0x8047
#define GL_LUMINANCE16_ALPHA16            0x8048
#define GL_INTENSITY                      0x8049
#define GL_INTENSITY4                     0x804A
#define GL_INTENSITY8                     0x804B
#define GL_INTENSITY12                    0x804C
#define GL_INTENSITY16                    0x804D
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_TEXTURE_LUMINANCE_SIZE         0x8060
#define GL_TEXTURE_INTENSITY_SIZE         0x8061
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064

/* texture_object */
#define GL_TEXTURE_PRIORITY               0x8066
#define GL_TEXTURE_RESIDENT               0x8067
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069

/* vertex_array */
#define GL_VERTEX_ARRAY                   0x8074
#define GL_NORMAL_ARRAY                   0x8075
#define GL_COLOR_ARRAY                    0x8076
#define GL_INDEX_ARRAY                    0x8077
#define GL_TEXTURE_COORD_ARRAY            0x8078
#define GL_EDGE_FLAG_ARRAY                0x8079
#define GL_VERTEX_ARRAY_SIZE              0x807A
#define GL_VERTEX_ARRAY_TYPE              0x807B
#define GL_VERTEX_ARRAY_STRIDE            0x807C
#define GL_NORMAL_ARRAY_TYPE              0x807E
#define GL_NORMAL_ARRAY_STRIDE            0x807F
#define GL_COLOR_ARRAY_SIZE               0x8081
#define GL_COLOR_ARRAY_TYPE               0x8082
#define GL_COLOR_ARRAY_STRIDE             0x8083
#define GL_INDEX_ARRAY_TYPE               0x8085
#define GL_INDEX_ARRAY_STRIDE             0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE       0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE       0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE     0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE         0x808C
#define GL_VERTEX_ARRAY_POINTER           0x808E
#define GL_NORMAL_ARRAY_POINTER           0x808F
#define GL_COLOR_ARRAY_POINTER            0x8090
#define GL_INDEX_ARRAY_POINTER            0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER    0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER        0x8093
#define GL_V2F                            0x2A20
#define GL_V3F                            0x2A21
#define GL_C4UB_V2F                       0x2A22
#define GL_C4UB_V3F                       0x2A23
#define GL_C3F_V3F                        0x2A24
#define GL_N3F_V3F                        0x2A25
#define GL_C4F_N3F_V3F                    0x2A26
#define GL_T2F_V3F                        0x2A27
#define GL_T4F_V4F                        0x2A28
#define GL_T2F_C4UB_V3F                   0x2A29
#define GL_T2F_C3F_V3F                    0x2A2A
#define GL_T2F_N3F_V3F                    0x2A2B
#define GL_T2F_C4F_N3F_V3F                0x2A2C
#define GL_T4F_C4F_N3F_V4F                0x2A2D

/* Extensions */
#define GL_EXT_vertex_array               1
#define GL_WIN_swap_hint                  1
#define GL_EXT_bgra                       1
#define GL_EXT_paletted_texture           1

/* EXT_vertex_array */
#define GL_VERTEX_ARRAY_EXT               0x8074
#define GL_NORMAL_ARRAY_EXT               0x8075
#define GL_COLOR_ARRAY_EXT                0x8076
#define GL_INDEX_ARRAY_EXT                0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT        0x8078
#define GL_EDGE_FLAG_ARRAY_EXT            0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT          0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT          0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT        0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT         0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT          0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT        0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT         0x8080
#define GL_COLOR_ARRAY_SIZE_EXT           0x8081
#define GL_COLOR_ARRAY_TYPE_EXT           0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT         0x8083
#define GL_COLOR_ARRAY_COUNT_EXT          0x8084
#define GL_INDEX_ARRAY_TYPE_EXT           0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT         0x8086
#define GL_INDEX_ARRAY_COUNT_EXT          0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT   0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT   0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT 0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT  0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT     0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT      0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT       0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT       0x808F
#define GL_COLOR_ARRAY_POINTER_EXT        0x8090
#define GL_INDEX_ARRAY_POINTER_EXT        0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT    0x8093
#define GL_DOUBLE_EXT                     GL_DOUBLE

/* EXT_bgra */
#define GL_BGR_EXT                        0x80E0
#define GL_BGRA_EXT                       0x80E1
#define GL_BGR                            GL_BGR_EXT
#define GL_BGRA                           GL_BGRA_EXT

/* Testure3D */
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#define GL_TEXTURE_3D                     0x806F
#define GL_PROXY_TEXTURE_3D               0x8070
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_MAX_3D_TEXTURE_SIZE            0x8073

/* Packed Pixels */
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368

#define GL_LIGHT_MODEL_COLOR_CONTROL      0x81F8
#define GL_SINGLE_COLOR                   0x81F9
#define GL_SEPARATE_SPECULAR_COLOR        0x81FA

/* EXT_paletted_texture */

/* These must match the GL_COLOR_TABLE_*_SGI enumerants */
#define GL_COLOR_TABLE_FORMAT_EXT         0x80D8
#define GL_COLOR_TABLE_WIDTH_EXT          0x80D9
#define GL_COLOR_TABLE_RED_SIZE_EXT       0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_EXT     0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_EXT      0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_EXT     0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_EXT 0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_EXT 0x80DF

#define GL_COLOR_INDEX1_EXT               0x80E2
#define GL_COLOR_INDEX2_EXT               0x80E3
#define GL_COLOR_INDEX4_EXT               0x80E4
#define GL_COLOR_INDEX8_EXT               0x80E5
#define GL_COLOR_INDEX12_EXT              0x80E6
#define GL_COLOR_INDEX16_EXT              0x80E7

/* For compatibility with OpenGL v1.0 */
#define GL_LOGIC_OP GL_INDEX_LOGIC_OP
#define GL_TEXTURE_COMPONENTS GL_TEXTURE_INTERNAL_FORMAT

/* GL_ARB_multitexture */
#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4
#define GL_TEXTURE5_ARB                   0x84C5
#define GL_TEXTURE6_ARB                   0x84C6
#define GL_TEXTURE7_ARB                   0x84C7
#define GL_TEXTURE8_ARB                   0x84C8
#define GL_TEXTURE9_ARB                   0x84C9
#define GL_TEXTURE10_ARB                  0x84CA
#define GL_TEXTURE11_ARB                  0x84CB
#define GL_TEXTURE12_ARB                  0x84CC
#define GL_TEXTURE13_ARB                  0x84CD
#define GL_TEXTURE14_ARB                  0x84CE
#define GL_TEXTURE15_ARB                  0x84CF
#define GL_TEXTURE16_ARB                  0x84D0
#define GL_TEXTURE17_ARB                  0x84D1
#define GL_TEXTURE18_ARB                  0x84D2
#define GL_TEXTURE19_ARB                  0x84D3
#define GL_TEXTURE20_ARB                  0x84D4
#define GL_TEXTURE21_ARB                  0x84D5
#define GL_TEXTURE22_ARB                  0x84D6
#define GL_TEXTURE23_ARB                  0x84D7
#define GL_TEXTURE24_ARB                  0x84D8
#define GL_TEXTURE25_ARB                  0x84D9
#define GL_TEXTURE26_ARB                  0x84DA
#define GL_TEXTURE27_ARB                  0x84DB
#define GL_TEXTURE28_ARB                  0x84DC
#define GL_TEXTURE29_ARB                  0x84DD
#define GL_TEXTURE30_ARB                  0x84DE
#define GL_TEXTURE31_ARB                  0x84DF
#define GL_ACTIVE_TEXTURE_ARB             0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB      0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB          0x84E2

/*************************************************************/

void APIENTRY glAccum (GLenum op, GLfloat value);
void APIENTRY glAlphaFunc (GLenum func, GLclampf ref);
GLboolean APIENTRY glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences);
void APIENTRY glArrayElement (GLint i);
void APIENTRY glBegin (GLenum mode);
void APIENTRY glBindTexture (GLenum target, GLuint texture);
void APIENTRY glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
void APIENTRY glCallList (GLuint list);
void APIENTRY glCallLists (GLsizei n, GLenum type, const GLvoid *lists);
void APIENTRY glClear (GLbitfield mask);
void APIENTRY glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void APIENTRY glClearDepth (GLclampd depth);
void APIENTRY glClearIndex (GLfloat c);
void APIENTRY glClearStencil (GLint s);
void APIENTRY glClipPlane (GLenum plane, const GLdouble *equation);
void APIENTRY glColor3b (GLbyte red, GLbyte green, GLbyte blue);
void APIENTRY glColor3bv (const GLbyte *v);
void APIENTRY glColor3d (GLdouble red, GLdouble green, GLdouble blue);
void APIENTRY glColor3dv (const GLdouble *v);
void APIENTRY glColor3f (GLfloat red, GLfloat green, GLfloat blue);
void APIENTRY glColor3fv (const GLfloat *v);
void APIENTRY glColor3i (GLint red, GLint green, GLint blue);
void APIENTRY glColor3iv (const GLint *v);
void APIENTRY glColor3s (GLshort red, GLshort green, GLshort blue);
void APIENTRY glColor3sv (const GLshort *v);
void APIENTRY glColor3ub (GLubyte red, GLubyte green, GLubyte blue);
void APIENTRY glColor3ubv (const GLubyte *v);
void APIENTRY glColor3ui (GLuint red, GLuint green, GLuint blue);
void APIENTRY glColor3uiv (const GLuint *v);
void APIENTRY glColor3us (GLushort red, GLushort green, GLushort blue);
void APIENTRY glColor3usv (const GLushort *v);
void APIENTRY glColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
void APIENTRY glColor4bv (const GLbyte *v);
void APIENTRY glColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
void APIENTRY glColor4dv (const GLdouble *v);
void APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void APIENTRY glColor4fv (const GLfloat *v);
void APIENTRY glColor4i (GLint red, GLint green, GLint blue, GLint alpha);
void APIENTRY glColor4iv (const GLint *v);
void APIENTRY glColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha);
void APIENTRY glColor4sv (const GLshort *v);
void APIENTRY glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
void APIENTRY glColor4ubv (const GLubyte *v);
void APIENTRY glColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha);
void APIENTRY glColor4uiv (const GLuint *v);
void APIENTRY glColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha);
void APIENTRY glColor4usv (const GLushort *v);
void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void APIENTRY glColorMaterial (GLenum face, GLenum mode);
void APIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void APIENTRY glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
void APIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
void APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void APIENTRY glCopyTexSubImage3D( GLenum target, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint zoffset, GLint x,
                                             GLint y, GLsizei width,
                                             GLsizei height );

void APIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
void APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY glCullFace (GLenum mode);
void APIENTRY glDeleteLists (GLuint list, GLsizei range);
void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
void APIENTRY glDepthFunc (GLenum func);
void APIENTRY glDepthMask (GLboolean flag);
void APIENTRY glDepthRange (GLclampd zNear, GLclampd zFar);
void APIENTRY glDisable (GLenum cap);
void APIENTRY glDisableClientState (GLenum array);
void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
void APIENTRY glDrawBuffer (GLenum mode);
void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void APIENTRY glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void APIENTRY glEdgeFlag (GLboolean flag);
void APIENTRY glEdgeFlagPointer (GLsizei stride, const GLvoid *pointer);
void APIENTRY glEdgeFlagv (const GLboolean *flag);
void APIENTRY glEnable (GLenum cap);
void APIENTRY glEnableClientState (GLenum array);
void APIENTRY glEnd (void);
void APIENTRY glEndList (void);
void APIENTRY glEvalCoord1d (GLdouble u);
void APIENTRY glEvalCoord1dv (const GLdouble *u);
void APIENTRY glEvalCoord1f (GLfloat u);
void APIENTRY glEvalCoord1fv (const GLfloat *u);
void APIENTRY glEvalCoord2d (GLdouble u, GLdouble v);
void APIENTRY glEvalCoord2dv (const GLdouble *u);
void APIENTRY glEvalCoord2f (GLfloat u, GLfloat v);
void APIENTRY glEvalCoord2fv (const GLfloat *u);
void APIENTRY glEvalMesh1 (GLenum mode, GLint i1, GLint i2);
void APIENTRY glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
void APIENTRY glEvalPoint1 (GLint i);
void APIENTRY glEvalPoint2 (GLint i, GLint j);
void APIENTRY glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer);
void APIENTRY glFinish (void);
void APIENTRY glFlush (void);
void APIENTRY glFogf (GLenum pname, GLfloat param);
void APIENTRY glFogfv (GLenum pname, const GLfloat *params);
void APIENTRY glFogi (GLenum pname, GLint param);
void APIENTRY glFogiv (GLenum pname, const GLint *params);
void APIENTRY glFrontFace (GLenum mode);
void APIENTRY glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
GLuint APIENTRY glGenLists (GLsizei range);
void APIENTRY glGenTextures (GLsizei n, GLuint *textures);
void APIENTRY glGetBooleanv (GLenum pname, GLboolean *params);
void APIENTRY glGetClipPlane (GLenum plane, GLdouble *equation);
void APIENTRY glGetDoublev (GLenum pname, GLdouble *params);
GLenum APIENTRY glGetError (void);
void APIENTRY glGetFloatv (GLenum pname, GLfloat *params);
void APIENTRY glGetIntegerv (GLenum pname, GLint *params);
void APIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
void APIENTRY glGetLightiv (GLenum light, GLenum pname, GLint *params);
void APIENTRY glGetMapdv (GLenum target, GLenum query, GLdouble *v);
void APIENTRY glGetMapfv (GLenum target, GLenum query, GLfloat *v);
void APIENTRY glGetMapiv (GLenum target, GLenum query, GLint *v);
void APIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
void APIENTRY glGetMaterialiv (GLenum face, GLenum pname, GLint *params);
void APIENTRY glGetPixelMapfv (GLenum map, GLfloat *values);
void APIENTRY glGetPixelMapuiv (GLenum map, GLuint *values);
void APIENTRY glGetPixelMapusv (GLenum map, GLushort *values);
void APIENTRY glGetPointerv (GLenum pname, GLvoid* *params);
void APIENTRY glGetPolygonStipple (GLubyte *mask);
const GLubyte * APIENTRY glGetString (GLenum name);
void APIENTRY glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params);
void APIENTRY glGetTexEnviv (GLenum target, GLenum pname, GLint *params);
void APIENTRY glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params);
void APIENTRY glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params);
void APIENTRY glGetTexGeniv (GLenum coord, GLenum pname, GLint *params);
void APIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
void APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
void APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
void APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
void APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
void APIENTRY glHint (GLenum target, GLenum mode);
void APIENTRY glIndexMask (GLuint mask);
void APIENTRY glIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
void APIENTRY glIndexd (GLdouble c);
void APIENTRY glIndexdv (const GLdouble *c);
void APIENTRY glIndexf (GLfloat c);
void APIENTRY glIndexfv (const GLfloat *c);
void APIENTRY glIndexi (GLint c);
void APIENTRY glIndexiv (const GLint *c);
void APIENTRY glIndexs (GLshort c);
void APIENTRY glIndexsv (const GLshort *c);
void APIENTRY glIndexub (GLubyte c);
void APIENTRY glIndexubv (const GLubyte *c);
void APIENTRY glInitNames (void);
void APIENTRY glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer);
GLboolean APIENTRY glIsEnabled (GLenum cap);
GLboolean APIENTRY glIsList (GLuint list);
GLboolean APIENTRY glIsTexture (GLuint texture);
void APIENTRY glLightModelf (GLenum pname, GLfloat param);
void APIENTRY glLightModelfv (GLenum pname, const GLfloat *params);
void APIENTRY glLightModeli (GLenum pname, GLint param);
void APIENTRY glLightModeliv (GLenum pname, const GLint *params);
void APIENTRY glLightf (GLenum light, GLenum pname, GLfloat param);
void APIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params);
void APIENTRY glLighti (GLenum light, GLenum pname, GLint param);
void APIENTRY glLightiv (GLenum light, GLenum pname, const GLint *params);
void APIENTRY glLineStipple (GLint factor, GLushort pattern);
void APIENTRY glLineWidth (GLfloat width);
void APIENTRY glListBase (GLuint base);
void APIENTRY glLoadIdentity (void);
void APIENTRY glLoadMatrixd (const GLdouble *m);
void APIENTRY glLoadMatrixf (const GLfloat *m);
void APIENTRY glLoadName (GLuint name);
void APIENTRY glLogicOp (GLenum opcode);
void APIENTRY glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
void APIENTRY glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
void APIENTRY glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
void APIENTRY glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
void APIENTRY glMapGrid1d (GLint un, GLdouble u1, GLdouble u2);
void APIENTRY glMapGrid1f (GLint un, GLfloat u1, GLfloat u2);
void APIENTRY glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
void APIENTRY glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
void APIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param);
void APIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params);
void APIENTRY glMateriali (GLenum face, GLenum pname, GLint param);
void APIENTRY glMaterialiv (GLenum face, GLenum pname, const GLint *params);
void APIENTRY glMatrixMode (GLenum mode);
void APIENTRY glMultMatrixd (const GLdouble *m);
void APIENTRY glMultMatrixf (const GLfloat *m);
void APIENTRY glNewList (GLuint list, GLenum mode);
void APIENTRY glNormal3b (GLbyte nx, GLbyte ny, GLbyte nz);
void APIENTRY glNormal3bv (const GLbyte *v);
void APIENTRY glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz);
void APIENTRY glNormal3dv (const GLdouble *v);
void APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz);
void APIENTRY glNormal3fv (const GLfloat *v);
void APIENTRY glNormal3i (GLint nx, GLint ny, GLint nz);
void APIENTRY glNormal3iv (const GLint *v);
void APIENTRY glNormal3s (GLshort nx, GLshort ny, GLshort nz);
void APIENTRY glNormal3sv (const GLshort *v);
void APIENTRY glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
void APIENTRY glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void APIENTRY glPassThrough (GLfloat token);
void APIENTRY glPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values);
void APIENTRY glPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values);
void APIENTRY glPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values);
void APIENTRY glPixelStoref (GLenum pname, GLfloat param);
void APIENTRY glPixelStorei (GLenum pname, GLint param);
void APIENTRY glPixelTransferf (GLenum pname, GLfloat param);
void APIENTRY glPixelTransferi (GLenum pname, GLint param);
void APIENTRY glPixelZoom (GLfloat xfactor, GLfloat yfactor);
void APIENTRY glPointSize (GLfloat size);
void APIENTRY glPolygonMode (GLenum face, GLenum mode);
void APIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
void APIENTRY glPolygonStipple (const GLubyte *mask);
void APIENTRY glPopAttrib (void);
void APIENTRY glPopClientAttrib (void);
void APIENTRY glPopMatrix (void);
void APIENTRY glPopName (void);
void APIENTRY glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities);
void APIENTRY glPushAttrib (GLbitfield mask);
void APIENTRY glPushClientAttrib (GLbitfield mask);
void APIENTRY glPushMatrix (void);
void APIENTRY glPushName (GLuint name);
void APIENTRY glRasterPos2d (GLdouble x, GLdouble y);
void APIENTRY glRasterPos2dv (const GLdouble *v);
void APIENTRY glRasterPos2f (GLfloat x, GLfloat y);
void APIENTRY glRasterPos2fv (const GLfloat *v);
void APIENTRY glRasterPos2i (GLint x, GLint y);
void APIENTRY glRasterPos2iv (const GLint *v);
void APIENTRY glRasterPos2s (GLshort x, GLshort y);
void APIENTRY glRasterPos2sv (const GLshort *v);
void APIENTRY glRasterPos3d (GLdouble x, GLdouble y, GLdouble z);
void APIENTRY glRasterPos3dv (const GLdouble *v);
void APIENTRY glRasterPos3f (GLfloat x, GLfloat y, GLfloat z);
void APIENTRY glRasterPos3fv (const GLfloat *v);
void APIENTRY glRasterPos3i (GLint x, GLint y, GLint z);
void APIENTRY glRasterPos3iv (const GLint *v);
void APIENTRY glRasterPos3s (GLshort x, GLshort y, GLshort z);
void APIENTRY glRasterPos3sv (const GLshort *v);
void APIENTRY glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void APIENTRY glRasterPos4dv (const GLdouble *v);
void APIENTRY glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void APIENTRY glRasterPos4fv (const GLfloat *v);
void APIENTRY glRasterPos4i (GLint x, GLint y, GLint z, GLint w);
void APIENTRY glRasterPos4iv (const GLint *v);
void APIENTRY glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w);
void APIENTRY glRasterPos4sv (const GLshort *v);
void APIENTRY glReadBuffer (GLenum mode);
void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
void APIENTRY glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
void APIENTRY glRectdv (const GLdouble *v1, const GLdouble *v2);
void APIENTRY glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
void APIENTRY glRectfv (const GLfloat *v1, const GLfloat *v2);
void APIENTRY glRecti (GLint x1, GLint y1, GLint x2, GLint y2);
void APIENTRY glRectiv (const GLint *v1, const GLint *v2);
void APIENTRY glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
void APIENTRY glRectsv (const GLshort *v1, const GLshort *v2);
GLint APIENTRY glRenderMode (GLenum mode);
void APIENTRY glRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
void APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void APIENTRY glScaled (GLdouble x, GLdouble y, GLdouble z);
void APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z);
void APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY glSelectBuffer (GLsizei size, GLuint *buffer);
void APIENTRY glShadeModel (GLenum mode);
void APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
void APIENTRY glStencilMask (GLuint mask);
void APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
void APIENTRY glTexCoord1d (GLdouble s);
void APIENTRY glTexCoord1dv (const GLdouble *v);
void APIENTRY glTexCoord1f (GLfloat s);
void APIENTRY glTexCoord1fv (const GLfloat *v);
void APIENTRY glTexCoord1i (GLint s);
void APIENTRY glTexCoord1iv (const GLint *v);
void APIENTRY glTexCoord1s (GLshort s);
void APIENTRY glTexCoord1sv (const GLshort *v);
void APIENTRY glTexCoord2d (GLdouble s, GLdouble t);
void APIENTRY glTexCoord2dv (const GLdouble *v);
void APIENTRY glTexCoord2f (GLfloat s, GLfloat t);
void APIENTRY glTexCoord2fv (const GLfloat *v);
void APIENTRY glTexCoord2i (GLint s, GLint t);
void APIENTRY glTexCoord2iv (const GLint *v);
void APIENTRY glTexCoord2s (GLshort s, GLshort t);
void APIENTRY glTexCoord2sv (const GLshort *v);
void APIENTRY glTexCoord3d (GLdouble s, GLdouble t, GLdouble r);
void APIENTRY glTexCoord3dv (const GLdouble *v);
void APIENTRY glTexCoord3f (GLfloat s, GLfloat t, GLfloat r);
void APIENTRY glTexCoord3fv (const GLfloat *v);
void APIENTRY glTexCoord3i (GLint s, GLint t, GLint r);
void APIENTRY glTexCoord3iv (const GLint *v);
void APIENTRY glTexCoord3s (GLshort s, GLshort t, GLshort r);
void APIENTRY glTexCoord3sv (const GLshort *v);
void APIENTRY glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
void APIENTRY glTexCoord4dv (const GLdouble *v);
void APIENTRY glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void APIENTRY glTexCoord4fv (const GLfloat *v);
void APIENTRY glTexCoord4i (GLint s, GLint t, GLint r, GLint q);
void APIENTRY glTexCoord4iv (const GLint *v);
void APIENTRY glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q);
void APIENTRY glTexCoord4sv (const GLshort *v);
void APIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param);
void APIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params);
void APIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param);
void APIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params);
void APIENTRY glTexGend (GLenum coord, GLenum pname, GLdouble param);
void APIENTRY glTexGendv (GLenum coord, GLenum pname, const GLdouble *params);
void APIENTRY glTexGenf (GLenum coord, GLenum pname, GLfloat param);
void APIENTRY glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params);
void APIENTRY glTexGeni (GLenum coord, GLenum pname, GLint param);
void APIENTRY glTexGeniv (GLenum coord, GLenum pname, const GLint *params);
void APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void APIENTRY glTexImage3D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
void APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
void APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
void APIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
void APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void APIENTRY glTexSubImage3D( GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLsizei width,
                                         GLsizei height, GLsizei depth,
                                         GLenum format,
                                         GLenum type, const GLvoid *pixels);

void APIENTRY glTranslated (GLdouble x, GLdouble y, GLdouble z);
void APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z);
void APIENTRY glVertex2d (GLdouble x, GLdouble y);
void APIENTRY glVertex2dv (const GLdouble *v);
void APIENTRY glVertex2f (GLfloat x, GLfloat y);
void APIENTRY glVertex2fv (const GLfloat *v);
void APIENTRY glVertex2i (GLint x, GLint y);
void APIENTRY glVertex2iv (const GLint *v);
void APIENTRY glVertex2s (GLshort x, GLshort y);
void APIENTRY glVertex2sv (const GLshort *v);
void APIENTRY glVertex3d (GLdouble x, GLdouble y, GLdouble z);
void APIENTRY glVertex3dv (const GLdouble *v);
void APIENTRY glVertex3f (GLfloat x, GLfloat y, GLfloat z);
void APIENTRY glVertex3fv (const GLfloat *v);
void APIENTRY glVertex3i (GLint x, GLint y, GLint z);
void APIENTRY glVertex3iv (const GLint *v);
void APIENTRY glVertex3s (GLshort x, GLshort y, GLshort z);
void APIENTRY glVertex3sv (const GLshort *v);
void APIENTRY glVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void APIENTRY glVertex4dv (const GLdouble *v);
void APIENTRY glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void APIENTRY glVertex4fv (const GLfloat *v);
void APIENTRY glVertex4i (GLint x, GLint y, GLint z, GLint w);
void APIENTRY glVertex4iv (const GLint *v);
void APIENTRY glVertex4s (GLshort x, GLshort y, GLshort z, GLshort w);
void APIENTRY glVertex4sv (const GLshort *v);
void APIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);

/* EXT_vertex_array */
typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRY * PFNGLARRAYELEMENTARRAYEXTPROC)(GLenum mode, GLsizei count, const GLvoid* pi);

/* WIN_swap_hint */
typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC)  (GLint x, GLint y, GLsizei width, GLsizei height);

/* EXT_paletted_texture */
typedef void (APIENTRY * PFNGLCOLORTABLEEXTPROC)
    (GLenum target, GLenum internalFormat, GLsizei width, GLenum format,
     GLenum type, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOLORSUBTABLEEXTPROC)
    (GLenum target, GLsizei start, GLsizei count, GLenum format,
     GLenum type, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOLORTABLEEXTPROC)
    (GLenum target, GLenum format, GLenum type, GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVEXTPROC)
    (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVEXTPROC)
    (GLenum target, GLenum pname, GLfloat *params);

/* MESA specific */
/* GL_MESA_window_pos */

void APIENTRY glWindowPos2iMESA( GLint x, GLint y );
void APIENTRY glWindowPos2sMESA( GLshort x, GLshort y );
void APIENTRY glWindowPos2fMESA( GLfloat x, GLfloat y );
void APIENTRY glWindowPos2dMESA( GLdouble x, GLdouble y );

void APIENTRY glWindowPos2ivMESA( const GLint *p );
void APIENTRY glWindowPos2svMESA( const GLshort *p );
void APIENTRY glWindowPos2fvMESA( const GLfloat *p );
void APIENTRY glWindowPos2dvMESA( const GLdouble *p );

void APIENTRY glWindowPos3iMESA( GLint x, GLint y, GLint z );
void APIENTRY glWindowPos3sMESA( GLshort x, GLshort y, GLshort z );
void APIENTRY glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z );
void APIENTRY glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z );

void APIENTRY glWindowPos3ivMESA( const GLint *p );
void APIENTRY glWindowPos3svMESA( const GLshort *p );
void APIENTRY glWindowPos3fvMESA( const GLfloat *p );
void APIENTRY glWindowPos3dvMESA( const GLdouble *p );

void APIENTRY glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w );
void APIENTRY glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w );
void APIENTRY glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
void APIENTRY glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w);

void APIENTRY glWindowPos4ivMESA( const GLint *p );
void APIENTRY glWindowPos4svMESA( const GLshort *p );
void APIENTRY glWindowPos4fvMESA( const GLfloat *p );
void APIENTRY glWindowPos4dvMESA( const GLdouble *p );

void APIENTRY glDrawRangeElements( GLenum mode, GLuint start,
        GLuint end, GLsizei count, GLenum type, const GLvoid *indices );

/* GL_EXT_rescale_normal */
#define GL_RESCALE_NORMAL_EXT       0x803A

#define GL_ABGR_EXT                 0x8000
#define GL_CONSTANT_COLOR           0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA           0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004

/* GL_EXT_blend_minmax and GL_EXT_blend_color */
#define GL_CONSTANT_COLOR_EXT                    0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT          0x8002
#define GL_CONSTANT_ALPHA_EXT                    0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT          0x8004
#define GL_BLEND_EQUATION_EXT                    0x8009
#define GL_MIN_EXT                               0x8007
#define GL_MAX_EXT                               0x8008
#define GL_FUNC_ADD_EXT                          0x8006
#define GL_FUNC_SUBTRACT_EXT                     0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT             0x800B
#define GL_BLEND_COLOR_EXT                       0x8005

/* GL_EXT_texture3D */
#define GL_PACK_SKIP_IMAGES_EXT                  0x806B
#define GL_PACK_IMAGE_HEIGHT_EXT                 0x806C
#define GL_UNPACK_SKIP_IMAGES_EXT                0x806D
#define GL_UNPACK_IMAGE_HEIGHT_EXT               0x806E
#define GL_TEXTURE_3D_EXT                        0x806F
#define GL_PROXY_TEXTURE_3D_EXT                  0x8070
#define GL_TEXTURE_DEPTH_EXT                     0x8071
#define GL_TEXTURE_WRAP_R_EXT                    0x8072
#define GL_MAX_3D_TEXTURE_SIZE_EXT               0x8073
#define GL_TEXTURE_3D_BINDING_EXT                0x806A

/* GL_EXT_shared_texture_palette */
#define GL_SHARED_TEXTURE_PALETTE_EXT            0x81FB

/* GL_ARB_multitexture */
void APIENTRY glActiveTextureARB(GLenum texture);
void APIENTRY glClientActiveTextureARB(GLenum texture);
void APIENTRY glMultiTexCoord1dARB(GLenum target, GLdouble s);
void APIENTRY glMultiTexCoord1dvARB(GLenum target, const GLdouble *v);
void APIENTRY glMultiTexCoord1fARB(GLenum target, GLfloat s);
void APIENTRY glMultiTexCoord1fvARB(GLenum target, const GLfloat *v);
void APIENTRY glMultiTexCoord1iARB(GLenum target, GLint s);
void APIENTRY glMultiTexCoord1ivARB(GLenum target, const GLint *v);
void APIENTRY glMultiTexCoord1sARB(GLenum target, GLshort s);
void APIENTRY glMultiTexCoord1svARB(GLenum target, const GLshort *v);
void APIENTRY glMultiTexCoord2dARB(GLenum target, GLdouble s, GLdouble t);
void APIENTRY glMultiTexCoord2dvARB(GLenum target, const GLdouble *v);
void APIENTRY glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);
void APIENTRY glMultiTexCoord2fvARB(GLenum target, const GLfloat *v);
void APIENTRY glMultiTexCoord2iARB(GLenum target, GLint s, GLint t);
void APIENTRY glMultiTexCoord2ivARB(GLenum target, const GLint *v);
void APIENTRY glMultiTexCoord2sARB(GLenum target, GLshort s, GLshort t);
void APIENTRY glMultiTexCoord2svARB(GLenum target, const GLshort *v);
void APIENTRY glMultiTexCoord3dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r);
void APIENTRY glMultiTexCoord3dvARB(GLenum target, const GLdouble *v);
void APIENTRY glMultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r);
void APIENTRY glMultiTexCoord3fvARB(GLenum target, const GLfloat *v);
void APIENTRY glMultiTexCoord3iARB(GLenum target, GLint s, GLint t, GLint r);
void APIENTRY glMultiTexCoord3ivARB(GLenum target, const GLint *v);
void APIENTRY glMultiTexCoord3sARB(GLenum target, GLshort s, GLshort t, GLshort r);
void APIENTRY glMultiTexCoord3svARB(GLenum target, const GLshort *v);
void APIENTRY glMultiTexCoord4dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
void APIENTRY glMultiTexCoord4dvARB(GLenum target, const GLdouble *v);
void APIENTRY glMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void APIENTRY glMultiTexCoord4fvARB(GLenum target, const GLfloat *v);
void APIENTRY glMultiTexCoord4iARB(GLenum target, GLint s, GLint t, GLint r, GLint q);
void APIENTRY glMultiTexCoord4ivARB(GLenum target, const GLint *v);
void APIENTRY glMultiTexCoord4sARB(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
void APIENTRY glMultiTexCoord4svARB(GLenum target, const GLshort *v);

/* 1.2 imaging extension functions */
void APIENTRY glColorTable( GLenum target, GLenum internalformat,
                                    GLsizei width, GLenum format,
                                    GLenum type, const GLvoid *table );

void APIENTRY glColorSubTable( GLenum target,
                                       GLsizei start, GLsizei count,
                                       GLenum format, GLenum type,
                                       const GLvoid *data );

void APIENTRY glColorTableParameteriv(GLenum target, GLenum pname,
                                              const GLint *params);

void APIENTRY glColorTableParameterfv(GLenum target, GLenum pname,
                                              const GLfloat *params);

void APIENTRY glCopyColorSubTable( GLenum target, GLsizei start,
                                           GLint x, GLint y, GLsizei width );

void APIENTRY glCopyColorTable( GLenum target, GLenum internalformat,
                                        GLint x, GLint y, GLsizei width );

void APIENTRY glGetColorTable( GLenum target, GLenum format,
                                       GLenum type, GLvoid *table );

void APIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname,
                                                  GLfloat *params );

void APIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname,
                                                  GLint *params );

void APIENTRY glBlendEquation( GLenum mode );

void APIENTRY glBlendColor( GLclampf red, GLclampf green,
                                    GLclampf blue, GLclampf alpha );

void APIENTRY glHistogram( GLenum target, GLsizei width,
                                   GLenum internalformat, GLboolean sink );

void APIENTRY glResetHistogram( GLenum target );

void APIENTRY glGetHistogram( GLenum target, GLboolean reset,
                                      GLenum format, GLenum type,
                                      GLvoid *values );

void APIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname,
                                                 GLfloat *params );

void APIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname,
                                                 GLint *params );

void APIENTRY glMinmax( GLenum target, GLenum internalformat,
                                GLboolean sink );

void APIENTRY glResetMinmax( GLenum target );

void APIENTRY glGetMinmax( GLenum target, GLboolean reset,
                                   GLenum format, GLenum types,
                                   GLvoid *values );

void APIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname,
                                              GLfloat *params );

void APIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname,
                                              GLint *params );

void APIENTRY glConvolutionFilter1D( GLenum target,
  GLenum internalformat, GLsizei width, GLenum format, GLenum type,
  const GLvoid *image );

void APIENTRY glConvolutionFilter2D( GLenum target,
  GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
  GLenum type, const GLvoid *image );

void APIENTRY glConvolutionParameterf( GLenum target, GLenum pname,
  GLfloat params );

void APIENTRY glConvolutionParameterfv( GLenum target, GLenum pname,
  const GLfloat *params );

void APIENTRY glConvolutionParameteri( GLenum target, GLenum pname,
  GLint params );

void APIENTRY glConvolutionParameteriv( GLenum target, GLenum pname,
  const GLint *params );

void APIENTRY glCopyConvolutionFilter1D( GLenum target,
  GLenum internalformat, GLint x, GLint y, GLsizei width );

void APIENTRY glCopyConvolutionFilter2D( GLenum target,
  GLenum internalformat, GLint x, GLint y, GLsizei width,
  GLsizei height);

void APIENTRY glGetConvolutionFilter( GLenum target, GLenum format,
  GLenum type, GLvoid *image );

void APIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname,
  GLfloat *params );

void APIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname,
  GLint *params );

void APIENTRY glSeparableFilter2D( GLenum target,
        GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
        GLenum type, const GLvoid *row, const GLvoid *column );

void APIENTRY glGetSeparableFilter( GLenum target, GLenum format,
        GLenum type, GLvoid *row, GLvoid *column, GLvoid *span );

/* GL_EXT_blend_minmax */
void APIENTRY glBlendEquationEXT( GLenum mode );
/* GL_EXT_blend_color */
void APIENTRY glBlendColorEXT( GLclampf red, GLclampf green,
                                       GLclampf blue, GLclampf alpha );



/* GL_EXT_polygon_offset */
void APIENTRY glPolygonOffsetEXT( GLfloat factor, GLfloat bias );



/* GL_EXT_vertex_array */
void APIENTRY glVertexPointerEXT( GLint size, GLenum type,
                                          GLsizei stride,
                                          GLsizei count, const GLvoid *ptr );

void APIENTRY glNormalPointerEXT( GLenum type, GLsizei stride,
                                          GLsizei count, const GLvoid *ptr );

void APIENTRY glColorPointerEXT( GLint size, GLenum type,
                                         GLsizei stride,
                                         GLsizei count, const GLvoid *ptr );

void APIENTRY glIndexPointerEXT( GLenum type, GLsizei stride,
                                         GLsizei count, const GLvoid *ptr );

void APIENTRY glTexCoordPointerEXT( GLint size, GLenum type,
                                            GLsizei stride, GLsizei count,
                                            const GLvoid *ptr );

void APIENTRY glEdgeFlagPointerEXT( GLsizei stride, GLsizei count,
                                            const GLboolean *ptr );

void APIENTRY glGetPointervEXT( GLenum pname, void **params );

void APIENTRY glArrayElementEXT( GLint i );

void APIENTRY glDrawArraysEXT( GLenum mode, GLint first,
                                       GLsizei count );



/* GL_EXT_texture_object */
void APIENTRY glGenTexturesEXT( GLsizei n, GLuint *textures );

void APIENTRY glDeleteTexturesEXT( GLsizei n, const GLuint *textures);

void APIENTRY glBindTextureEXT( GLenum target, GLuint texture );

void APIENTRY glPrioritizeTexturesEXT( GLsizei n,
                                               const GLuint *textures,
                                               const GLclampf *priorities );

GLboolean APIENTRY glAreTexturesResidentEXT( GLsizei n,
                                                     const GLuint *textures,
                                                     GLboolean *residences );

GLboolean APIENTRY glIsTextureEXT( GLuint texture );



/* GL_EXT_texture3D */
void APIENTRY glTexImage3DEXT( GLenum target, GLint level,
                                       GLenum internalFormat,
                                       GLsizei width, GLsizei height,
                                       GLsizei depth, GLint border,
                                       GLenum format, GLenum type,
                                       const GLvoid *pixels );

void APIENTRY glTexSubImage3DEXT( GLenum target, GLint level,
                                          GLint xoffset, GLint yoffset,
                                          GLint zoffset, GLsizei width,
                                          GLsizei height, GLsizei depth,
                                          GLenum format,
                                          GLenum type, const GLvoid *pixels);

void APIENTRY glCopyTexSubImage3DEXT( GLenum target, GLint level,
                                              GLint xoffset, GLint yoffset,
                                              GLint zoffset, GLint x,
                                              GLint y, GLsizei width,
                                              GLsizei height );



/* GL_EXT_color_table */
void APIENTRY glColorTableEXT( GLenum target, GLenum internalformat,
                                       GLsizei width, GLenum format,
                                       GLenum type, const GLvoid *table );

void APIENTRY glColorSubTableEXT( GLenum target,
                                          GLsizei start, GLsizei count,
                                          GLenum format, GLenum type,
                                          const GLvoid *data );

void APIENTRY glGetColorTableEXT( GLenum target, GLenum format,
                                          GLenum type, GLvoid *table );

void APIENTRY glGetColorTableParameterfvEXT( GLenum target,
                                                     GLenum pname,
                                                     GLfloat *params );

void APIENTRY glGetColorTableParameterivEXT( GLenum target,
                                                     GLenum pname,
                                                     GLint *params );

/* GL_EXT_compiled_vertex_array */
void APIENTRY glLockArraysEXT( GLint first, GLsizei count );
void APIENTRY glUnlockArraysEXT( void );

/* GL_EXT_point_parameters */
void APIENTRY glPointParameterfEXT( GLenum pname, GLfloat param );
void APIENTRY glPointParameterfvEXT( GLenum pname,
                                               const GLfloat *params );



/* GL_INGR_blend_func_separate */
void APIENTRY glBlendFuncSeparateINGR( GLenum sfactorRGB,
                                               GLenum dfactorRGB,
                                               GLenum sfactorAlpha,
                                               GLenum dfactorAlpha );

void APIENTRY glResizeBuffersMESA( void );

/* GL_INGR_blend_func_separate */
#define GL_BLEND_DST_RGB_INGR           0x80C8
#define GL_BLEND_SRC_RGB_INGR           0x80C9
#define GL_BLEND_DST_ALPHA_INGR         0x80CA
#define GL_BLEND_SRC_ALPHA_INGR         0x80CB

/* OpenGL 1.2 */
#define GL_RESCALE_NORMAL               0x803A
#define GL_CLAMP_TO_EDGE                0x812F
#define GL_MAX_ELEMENTS_VERTICES        0x80E8
#define GL_MAX_ELEMENTS_INDICES         0x80E9
#define GL_TEXTURE_MIN_LOD              0x813A
#define GL_TEXTURE_MAX_LOD              0x813B
#define GL_TEXTURE_BASE_LEVEL           0x813C
#define GL_TEXTURE_MAX_LEVEL            0x813D

/* GL_EXT_point_parameters */
#define GL_POINT_SIZE_MIN_EXT             0x8126
#define GL_POINT_SIZE_MAX_EXT             0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT  0x8128
#define GL_DISTANCE_ATTENUATION_EXT       0x8129

/* GL_PGI_misc_hints */
#define GL_PREFER_DOUBLEBUFFER_HINT_PGI        107000
#define GL_STRICT_DEPTHFUNC_HINT_PGI           107030
#define GL_STRICT_LIGHTING_HINT_PGI            107031
#define GL_STRICT_SCISSOR_HINT_PGI             107032
#define GL_FULL_STIPPLE_HINT_PGI               107033
#define GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI      107011
#define GL_NATIVE_GRAPHICS_END_HINT_PGI        107012
#define GL_CONSERVE_MEMORY_HINT_PGI            107005
#define GL_RECLAIM_MEMORY_HINT_PGI             107006
#define GL_ALWAYS_FAST_HINT_PGI                107020
#define GL_ALWAYS_SOFT_HINT_PGI                107021
#define GL_ALLOW_DRAW_OBJ_HINT_PGI             107022
#define GL_ALLOW_DRAW_WIN_HINT_PGI             107023
#define GL_ALLOW_DRAW_FRG_HINT_PGI             107024
#define GL_ALLOW_DRAW_SPN_HINT_PGI             107024
#define GL_ALLOW_DRAW_MEM_HINT_PGI             107025
#define GL_CLIP_NEAR_HINT_PGI                  107040
#define GL_CLIP_FAR_HINT_PGI                   107041
#define GL_WIDE_LINE_HINT_PGI                  107042
#define GL_BACK_NORMALS_HINT_PGI               107043
#define GL_NATIVE_GRAPHICS_HANDLE_PGI          107010

/* GL_EXT_compiled_vertex_arr*/
#define GL_ARRAY_ELEMENT_LOCK_FIRST_SGI        0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_SGI        0x81A9

/* GL_EXT_clip_volume_hi*/
#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT       0x80F

/* GL 1.2 texturing */
#define GL_PACK_SKIP_IMAGES             0x806B
#define GL_PACK_IMAGE_HEIGHT            0x806C
#define GL_UNPACK_SKIP_IMAGES           0x806D
#define GL_UNPACK_IMAGE_HEIGHT          0x806E
#define GL_TEXTURE_3D                   0x806F
#define GL_PROXY_TEXTURE_3D             0x8070
#define GL_TEXTURE_DEPTH                0x8071
#define GL_TEXTURE_WRAP_R               0x8072
#define GL_MAX_3D_TEXTURE_SIZE          0x8073
#define GL_TEXTURE_BINDING_3D           0x806A

/* GL_EXT_stencil_wrap */
#define GL_INCR_WRAP_EXT                0x8507
#define GL_DECR_WRAP_EXT                0x8508

/* GL_NV_texgen_reflection (nVidia) */
#define GL_NORMAL_MAP_NV                0x8511
#define GL_REFLECTION_MAP_NV            0x8512

/* GL_EXT_paletted_texture */
#define GL_TABLE_TOO_LARGE_EXT                  0x8031
#define GL_COLOR_TABLE_FORMAT_EXT               0x80D8
#define GL_COLOR_TABLE_WIDTH_EXT                0x80D9
#define GL_COLOR_TABLE_RED_SIZE_EXT             0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_EXT           0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_EXT            0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_EXT           0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_EXT       0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_EXT       0x80DF
#define GL_TEXTURE_INDEX_SIZE_EXT               0x80ED
#define GL_COLOR_INDEX1_EXT                     0x80E2
#define GL_COLOR_INDEX2_EXT                     0x80E3
#define GL_COLOR_INDEX4_EXT                     0x80E4
#define GL_COLOR_INDEX8_EXT                     0x80E5
#define GL_COLOR_INDEX12_EXT                    0x80E6
#define GL_COLOR_INDEX16_EXT                    0x80E7

#ifdef __cplusplus
}
#endif

#endif                                 /* __GL__H__                        */
#endif                                 /* __gl__h_                         */
