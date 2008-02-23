/* $Id: glu.h,v 1.5 2000-03-11 17:06:35 sandervl Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.1
 * Copyright (C) 1995-1999  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef GLU_H
#define GLU_H


#if defined(USE_MGL_NAMESPACE)
#include "glu_mangle.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


#include "GL/gl.h"

        /* to facilitate clean DLL building ... */
#if !defined(OPENSTEP) && (defined(__WIN32__) || defined(__CYGWIN32__))
#       if defined(_MSC_VER) && defined(BUILD_GLU32) /* tag specify we're building mesa as a DLL */
#               define GLUAPI __declspec(dllexport)
#       elif defined(_MSC_VER) && defined(_DLL) /* tag specifying we're building for DLL runtime support */
#               define GLUAPI __declspec(dllimport)
#       else /* for use with static link lib build of Win32 edition only */
#               define GLUAPI extern
#       endif /* _STATIC_MESA support */
#else
#       define GLUAPI extern
#endif /* WIN32 / CYGWIN32 bracket */

#ifdef macintosh
        #pragma enumsalwaysint on
        #if PRAGMA_IMPORT_SUPPORTED
        #pragma import on
        #endif
#endif

#ifndef GLUAPI
#define GLUAPI
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

#ifndef GLCALLBACK
#define GLCALLBACK 
#endif


#define GLU_VERSION_1_1         1
#define GLU_VERSION_1_2         1


#define GLU_TRUE   GL_TRUE
#define GLU_FALSE  GL_FALSE

#define GLU_TESS_MAX_COORD      1.0e150


enum {
        /* Normal vectors */
        GLU_SMOOTH      = 100000,
        GLU_FLAT        = 100001,
        GLU_NONE        = 100002,

        /* Quadric draw styles */
        GLU_POINT       = 100010,
        GLU_LINE        = 100011,
        GLU_FILL        = 100012,
        GLU_SILHOUETTE  = 100013,

        /* Quadric orientation */
        GLU_OUTSIDE     = 100020,
        GLU_INSIDE      = 100021,

        /* Tessellator */
        GLU_TESS_BEGIN          = 100100,
        GLU_TESS_VERTEX         = 100101,
        GLU_TESS_END            = 100102,
        GLU_TESS_ERROR          = 100103,
        GLU_TESS_EDGE_FLAG      = 100104,
        GLU_TESS_COMBINE        = 100105,

        GLU_TESS_BEGIN_DATA     = 100106,
        GLU_TESS_VERTEX_DATA    = 100107,
        GLU_TESS_END_DATA       = 100108,
        GLU_TESS_ERROR_DATA     = 100109,
        GLU_TESS_EDGE_FLAG_DATA = 100110,
        GLU_TESS_COMBINE_DATA   = 100111,

        /* Winding rules */
        GLU_TESS_WINDING_ODD            = 100130,
        GLU_TESS_WINDING_NONZERO        = 100131,
        GLU_TESS_WINDING_POSITIVE       = 100132,
        GLU_TESS_WINDING_NEGATIVE       = 100133,
        GLU_TESS_WINDING_ABS_GEQ_TWO    = 100134,

        /* Tessellation properties */
        GLU_TESS_WINDING_RULE   = 100140,
        GLU_TESS_BOUNDARY_ONLY  = 100141,
        GLU_TESS_TOLERANCE      = 100142,

        /* Tessellation errors */
        GLU_TESS_ERROR1 = 100151,  /* Missing gluBeginPolygon */
        GLU_TESS_ERROR2 = 100152,  /* Missing gluBeginContour */
        GLU_TESS_ERROR3 = 100153,  /* Missing gluEndPolygon */
        GLU_TESS_ERROR4 = 100154,  /* Missing gluEndContour */
        GLU_TESS_ERROR5 = 100155,  /* */
        GLU_TESS_ERROR6 = 100156,  /* */
        GLU_TESS_ERROR7 = 100157,  /* */
        GLU_TESS_ERROR8 = 100158,  /* */

        GLU_TESS_MISSING_BEGIN_POLYGON = GLU_TESS_ERROR1,
        GLU_TESS_MISSING_BEGIN_CONTOUR = GLU_TESS_ERROR2,
        GLU_TESS_MISSING_END_POLYGON   = GLU_TESS_ERROR3,
        GLU_TESS_MISSING_END_CONTOUR   = GLU_TESS_ERROR4,
        GLU_TESS_COORD_TOO_LARGE       = GLU_TESS_ERROR5,
        GLU_TESS_NEED_COMBINE_CALLBACK = GLU_TESS_ERROR6,

        /* NURBS */
        GLU_NURBS_MODE          = 100160,
        GLU_NURBS_TESSELLATOR   = 100161,
        GLU_NURBS_RENDERER      = 100162,
        GLU_NURBS_BEGIN         = 100164,
        GLU_NURBS_VERTEX        = 100165,
        GLU_NURBS_NORMAL        = 100166,
        GLU_NURBS_COLOR         = 100167,
        GLU_NURBS_TEXTURE_COORD = 100168,
        GLU_NURBS_END           = 100169,
        GLU_NURBS_BEGIN_DATA    = 100170,
        GLU_NURBS_VERTEX_DATA   = 100171,
        GLU_NURBS_NORMAL_DATA   = 100172,
        GLU_NURBS_COLOR_DATA    = 100173,
        GLU_NURBS_TEXTURE_COORD_DATA = 100174,
        GLU_NURBS_END_DATA      = 100175,

        GLU_AUTO_LOAD_MATRIX    = 100200,
        GLU_CULLING             = 100201,
        GLU_PARAMETRIC_TOLERANCE= 100202,
        GLU_SAMPLING_TOLERANCE  = 100203,
        GLU_DISPLAY_MODE        = 100204,
        GLU_SAMPLING_METHOD     = 100205,
        GLU_U_STEP              = 100206,
        GLU_V_STEP              = 100207,
        GLU_OBJECT_PARAMETRIC_ERROR = 100208,
        GLU_OBJECT_PATH_LENGTH  = 100209,

        GLU_MAP1_TRIM_2         = 100210,
        GLU_MAP1_TRIM_3         = 100211,

        GLU_PATH_LENGTH         = 100215,
        GLU_PARAMETRIC_ERROR    = 100216,
        GLU_DOMAIN_DISTANCE     = 100217,

        GLU_OUTLINE_POLYGON     = 100240,
        GLU_OUTLINE_PATCH       = 100241,

        GLU_ERROR               = GLU_TESS_ERROR,
        GLU_NURBS_ERROR         = GLU_ERROR, /* FIXME: is this correct?? */

        GLU_NURBS_ERROR1  = 100251,   /* spline order un-supported */
        GLU_NURBS_ERROR2  = 100252,   /* too few knots */
        GLU_NURBS_ERROR3  = 100253,   /* valid knot range is empty */
        GLU_NURBS_ERROR4  = 100254,   /* decreasing knot sequence */
        GLU_NURBS_ERROR5  = 100255,   /* knot multiplicity > spline order */
        GLU_NURBS_ERROR6  = 100256,   /* endcurve() must follow bgncurve() */
        GLU_NURBS_ERROR7  = 100257,   /* bgncurve() must precede endcurve() */
        GLU_NURBS_ERROR8  = 100258,   /* ctrlarray or knot vector is NULL */
        GLU_NURBS_ERROR9  = 100259,   /* can't draw pwlcurves */
        GLU_NURBS_ERROR10 = 100260,   /* missing gluNurbsCurve() */
        GLU_NURBS_ERROR11 = 100261,   /* missing gluNurbsSurface() */
        GLU_NURBS_ERROR12 = 100262,   /* endtrim() must precede endsurface() */
        GLU_NURBS_ERROR13 = 100263,   /* bgnsurface() must precede endsurface() */
        GLU_NURBS_ERROR14 = 100264,   /* curve of improper type passed as trim curve */
        GLU_NURBS_ERROR15 = 100265,   /* bgnsurface() must precede bgntrim() */
        GLU_NURBS_ERROR16 = 100266,   /* endtrim() must follow bgntrim() */
        GLU_NURBS_ERROR17 = 100267,   /* bgntrim() must precede endtrim()*/
        GLU_NURBS_ERROR18 = 100268,   /* invalid or missing trim curve*/
        GLU_NURBS_ERROR19 = 100269,   /* bgntrim() must precede pwlcurve() */
        GLU_NURBS_ERROR20 = 100270,   /* pwlcurve referenced twice*/
        GLU_NURBS_ERROR21 = 100271,   /* pwlcurve and nurbscurve mixed */
        GLU_NURBS_ERROR22 = 100272,   /* improper usage of trim data type */
        GLU_NURBS_ERROR23 = 100273,   /* nurbscurve referenced twice */
        GLU_NURBS_ERROR24 = 100274,   /* nurbscurve and pwlcurve mixed */
        GLU_NURBS_ERROR25 = 100275,   /* nurbssurface referenced twice */
        GLU_NURBS_ERROR26 = 100276,   /* invalid property */
        GLU_NURBS_ERROR27 = 100277,   /* endsurface() must follow bgnsurface() */
        GLU_NURBS_ERROR28 = 100278,   /* intersecting or misoriented trim curves */
        GLU_NURBS_ERROR29 = 100279,   /* intersecting trim curves */
        GLU_NURBS_ERROR30 = 100280,   /* UNUSED */
        GLU_NURBS_ERROR31 = 100281,   /* unconnected trim curves */
        GLU_NURBS_ERROR32 = 100282,   /* unknown knot error */
        GLU_NURBS_ERROR33 = 100283,   /* negative vertex count encountered */
        GLU_NURBS_ERROR34 = 100284,   /* negative byte-stride */
        GLU_NURBS_ERROR35 = 100285,   /* unknown type descriptor */
        GLU_NURBS_ERROR36 = 100286,   /* null control point reference */
        GLU_NURBS_ERROR37 = 100287,   /* duplicate point on pwlcurve */

        /* Errors */
        GLU_INVALID_ENUM                = 100900,
        GLU_INVALID_VALUE               = 100901,
        GLU_OUT_OF_MEMORY               = 100902,
        GLU_INCOMPATIBLE_GL_VERSION     = 100903,
        GLU_INVALID_OPERATION           = 100904,

        /* New in GLU 1.1 */
        GLU_VERSION     = 100800,
        GLU_EXTENSIONS  = 100801,

        /*** GLU 1.0 tessellation - obsolete! ***/

        /* Contour types */
        GLU_CW          = 100120,
        GLU_CCW         = 100121,
        GLU_INTERIOR    = 100122,
        GLU_EXTERIOR    = 100123,
        GLU_UNKNOWN     = 100124,

        /* Tessellator */
        GLU_BEGIN       = GLU_TESS_BEGIN,
        GLU_VERTEX      = GLU_TESS_VERTEX,
        GLU_END         = GLU_TESS_END,
        GLU_EDGE_FLAG   = GLU_TESS_EDGE_FLAG
};


/*
 * These are the GLU 1.1 typedefs.  GLU 1.3 has different ones!
 */
#if defined(__BEOS__)
    /* The BeOS does something funky and makes these typedefs in one
     * of its system headers.
     */
#else
    typedef struct GLUquadric GLUquadricObj;
    typedef struct GLUnurbs GLUnurbsObj;

    /* FIXME: We need to implement the other 1.3 typedefs - GH */
    typedef struct GLUtesselator GLUtesselator;
    typedef GLUtesselator GLUtriangulatorObj;
#endif



#if defined(__BEOS__) || defined(__QUICKDRAW__)
#pragma export on
#endif


/*
 *
 * Miscellaneous functions
 *
 */

GLUAPI void GLAPIENTRY gluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
                                GLdouble centerx, GLdouble centery,
                                GLdouble centerz,
                                GLdouble upx, GLdouble upy, GLdouble upz );


GLUAPI void GLAPIENTRY gluOrtho2D( GLdouble left, GLdouble right,
                                 GLdouble bottom, GLdouble top );


GLUAPI void GLAPIENTRY gluPerspective( GLdouble fovy, GLdouble aspect,
                                     GLdouble zNear, GLdouble zFar );


GLUAPI void GLAPIENTRY gluPickMatrix( GLdouble x, GLdouble y,
                                    GLdouble width, GLdouble height,
                                    const GLint viewport[4] );

GLUAPI GLint GLAPIENTRY gluProject( GLdouble objx, GLdouble objy, GLdouble objz,
                                  const GLdouble modelMatrix[16],
                                  const GLdouble projMatrix[16],
                                  const GLint viewport[4],
                                  GLdouble *winx, GLdouble *winy,
                                  GLdouble *winz );

GLUAPI GLint GLAPIENTRY gluUnProject( GLdouble winx, GLdouble winy,
                                    GLdouble winz,
                                    const GLdouble modelMatrix[16],
                                    const GLdouble projMatrix[16],
                                    const GLint viewport[4],
                                    GLdouble *objx, GLdouble *objy,
                                    GLdouble *objz );

GLUAPI const GLubyte* GLAPIENTRY gluErrorString( GLenum errorCode );



/*
 *
 * Mipmapping and image scaling
 *
 */

GLUAPI GLint GLAPIENTRY gluScaleImage( GLenum format,
                                     GLsizei widthin, GLsizei heightin,
                                     GLenum typein, const void *datain,
                                     GLsizei widthout, GLsizei heightout,
                                     GLenum typeout, void *dataout );

GLUAPI GLint GLAPIENTRY gluBuild1DMipmaps( GLenum target, GLint components,
                                         GLsizei width, GLenum format,
                                         GLenum type, const void *data );

GLUAPI GLint GLAPIENTRY gluBuild2DMipmaps( GLenum target, GLint components,
                                         GLsizei width, GLsizei height,
                                         GLenum format,
                                         GLenum type, const void *data );



/*
 *
 * Quadrics
 *
 */

GLUAPI GLUquadricObj* GLAPIENTRY gluNewQuadric( void );

GLUAPI void GLAPIENTRY gluDeleteQuadric( GLUquadricObj *state );

GLUAPI void GLAPIENTRY gluQuadricDrawStyle( GLUquadricObj *quadObject,
                                          GLenum drawStyle );

GLUAPI void GLAPIENTRY gluQuadricOrientation( GLUquadricObj *quadObject,
                                            GLenum orientation );

GLUAPI void GLAPIENTRY gluQuadricNormals( GLUquadricObj *quadObject,
                                        GLenum normals );

GLUAPI void GLAPIENTRY gluQuadricTexture( GLUquadricObj *quadObject,
                                        GLboolean textureCoords );

GLUAPI void GLAPIENTRY gluQuadricCallback( GLUquadricObj *qobj,
                                           GLenum which,
                                           void (* GLCALLBACK fn)() );

GLUAPI void GLAPIENTRY gluCylinder( GLUquadricObj *qobj,
                                  GLdouble baseRadius,
                                  GLdouble topRadius,
                                  GLdouble height,
                                  GLint slices, GLint stacks );

GLUAPI void GLAPIENTRY gluSphere( GLUquadricObj *qobj,
                                GLdouble radius, GLint slices, GLint stacks );

GLUAPI void GLAPIENTRY gluDisk( GLUquadricObj *qobj,
                              GLdouble innerRadius, GLdouble outerRadius,
                              GLint slices, GLint loops );

GLUAPI void GLAPIENTRY gluPartialDisk( GLUquadricObj *qobj, GLdouble innerRadius,
                                     GLdouble outerRadius, GLint slices,
                                     GLint loops, GLdouble startAngle,
                                     GLdouble sweepAngle );



/*
 *
 * Nurbs
 *
 */

GLUAPI GLUnurbsObj* GLAPIENTRY gluNewNurbsRenderer( void );

GLUAPI void GLAPIENTRY gluDeleteNurbsRenderer( GLUnurbsObj *nobj );

GLUAPI void GLAPIENTRY gluLoadSamplingMatrices( GLUnurbsObj *nobj,
                                              const GLfloat modelMatrix[16],
                                              const GLfloat projMatrix[16],
                                              const GLint viewport[4] );

GLUAPI void GLAPIENTRY gluNurbsProperty( GLUnurbsObj *nobj, GLenum property,
                                       GLfloat value );

GLUAPI void GLAPIENTRY gluGetNurbsProperty( GLUnurbsObj *nobj, GLenum property,
                                          GLfloat *value );

GLUAPI void GLAPIENTRY gluBeginCurve( GLUnurbsObj *nobj );

GLUAPI void GLAPIENTRY gluEndCurve( GLUnurbsObj * nobj );

GLUAPI void GLAPIENTRY gluNurbsCurve( GLUnurbsObj *nobj, GLint nknots,
                                    GLfloat *knot, GLint stride,
                                    GLfloat *ctlarray, GLint order,
                                    GLenum type );

GLUAPI void GLAPIENTRY gluBeginSurface( GLUnurbsObj *nobj );

GLUAPI void GLAPIENTRY gluEndSurface( GLUnurbsObj * nobj );

GLUAPI void GLAPIENTRY gluNurbsSurface( GLUnurbsObj *nobj,
                                      GLint sknot_count, GLfloat *sknot,
                                      GLint tknot_count, GLfloat *tknot,
                                      GLint s_stride, GLint t_stride,
                                      GLfloat *ctlarray,
                                      GLint sorder, GLint torder,
                                      GLenum type );

GLUAPI void GLAPIENTRY gluBeginTrim( GLUnurbsObj *nobj );

GLUAPI void GLAPIENTRY gluEndTrim( GLUnurbsObj *nobj );

GLUAPI void GLAPIENTRY gluPwlCurve( GLUnurbsObj *nobj, GLint count,
                                  GLfloat *array, GLint stride, GLenum type );

GLUAPI void GLAPIENTRY gluNurbsCallback( GLUnurbsObj *nobj, GLenum which,
                                       void (* GLCALLBACK fn)() );

GLUAPI GLboolean GLAPIENTRY gluCheckExtension(const GLubyte *extName,
                                              const GLubyte *extString);


/*
 *
 * Polygon tessellation
 *
 */

GLUAPI GLUtesselator* GLAPIENTRY gluNewTess( void );

GLUAPI void GLAPIENTRY gluDeleteTess( GLUtesselator *tobj );

GLUAPI void GLAPIENTRY gluTessBeginPolygon( GLUtesselator *tobj,
                                            void *polygon_data );

GLUAPI void GLAPIENTRY gluTessBeginContour( GLUtesselator *tobj );

GLUAPI void GLAPIENTRY gluTessVertex( GLUtesselator *tobj, GLdouble coords[3],
                                      void *vertex_data );

GLUAPI void GLAPIENTRY gluTessEndContour( GLUtesselator *tobj );

GLUAPI void GLAPIENTRY gluTessEndPolygon( GLUtesselator *tobj );

GLUAPI void GLAPIENTRY gluTessProperty( GLUtesselator *tobj, GLenum which,
                                        GLdouble value );

GLUAPI void GLAPIENTRY gluTessNormal( GLUtesselator *tobj, GLdouble x,
                                      GLdouble y, GLdouble z );

GLUAPI void GLAPIENTRY gluTessCallback( GLUtesselator *tobj, GLenum which,
                                        void (* GLCALLBACK fn)() );

GLUAPI void GLAPIENTRY gluGetTessProperty( GLUtesselator *tobj, GLenum which,
                                           GLdouble *value );

/*
 *
 * Obsolete 1.0 tessellation functions
 *
 */

GLUAPI void GLAPIENTRY gluBeginPolygon( GLUtesselator *tobj );

GLUAPI void GLAPIENTRY gluNextContour( GLUtesselator *tobj, GLenum type );

GLUAPI void GLAPIENTRY gluEndPolygon( GLUtesselator *tobj );



/*
 *
 * New functions in GLU 1.1
 *
 */

GLUAPI const GLubyte* GLAPIENTRY gluGetString( GLenum name );



#if defined(__BEOS__) || defined(__QUICKDRAW__)
#pragma export off
#endif


#ifdef macintosh
        #pragma enumsalwaysint reset
        #if PRAGMA_IMPORT_SUPPORTED
        #pragma import off
        #endif
#endif


#ifdef __cplusplus
}
#endif


#endif
