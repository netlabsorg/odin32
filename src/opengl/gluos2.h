/* $Id: gluos2.h,v 1.1 2000-01-27 21:49:12 sandervl Exp $ */
#ifndef __gluos2_h__
#define __gluos2_h__

#ifdef __cplusplus
extern "C" {
#endif

#define GLU_BEGIN                            100100
#define GLU_TESS_BEGIN                       100100
#define GLU_VERTEX                           100101
#define GLU_TESS_VERTEX                      100101
#define GLU_END                              100102
#define GLU_TESS_END                         100102
#define GLU_ERROR                            100103
#define GLU_TESS_ERROR                       100103
#define GLU_EDGE_FLAG                        100104
#define GLU_TESS_EDGE_FLAG                   100104
#define GLU_TESS_COMBINE                     100105
#define GLU_TESS_BEGIN_DATA                  100106
#define GLU_TESS_VERTEX_DATA                 100107
#define GLU_TESS_END_DATA                    100108
#define GLU_TESS_ERROR_DATA                  100109
#define GLU_TESS_EDGE_FLAG_DATA              100110
#define GLU_TESS_COMBINE_DATA                100111

typedef struct GLUnurbs GLUnurbs;
typedef struct GLUquadric GLUquadric;
typedef struct GLUtesselator GLUtesselator;
typedef struct GLUnurbs GLUnurbsObj;
typedef struct GLUquadric GLUquadricObj;
typedef struct GLUtesselator GLUtesselatorObj;
typedef struct GLUtesselator GLUtriangulatorObj;

extern void APIENTRY OS2gluBeginCurve (GLUnurbs* nurb);
extern void APIENTRY OS2gluBeginPolygon (GLUtesselator* tess);
extern void APIENTRY OS2gluBeginSurface (GLUnurbs* nurb);
extern void APIENTRY OS2gluBeginTrim (GLUnurbs* nurb);
extern GLint APIENTRY OS2gluBuild1DMipmaps (GLenum target, GLint component, GLsizei width, GLenum format, GLenum type, const void *data);
extern GLint APIENTRY OS2gluBuild2DMipmaps (GLenum target, GLint component, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
extern void APIENTRY OS2gluCylinder (GLUquadric* quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks);
extern void APIENTRY OS2gluDeleteNurbsRenderer (GLUnurbs* nurb);
extern void APIENTRY OS2gluDeleteQuadric (GLUquadric* quad);
extern void APIENTRY OS2gluDeleteTess (GLUtesselator* tess);
extern void APIENTRY OS2gluDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops);
extern void APIENTRY OS2gluEndCurve (GLUnurbs* nurb);
extern void APIENTRY OS2gluEndPolygon (GLUtesselator* tess);
extern void APIENTRY OS2gluEndSurface (GLUnurbs* nurb);
extern void APIENTRY OS2gluEndTrim (GLUnurbs* nurb);
extern const GLubyte * APIENTRY OS2gluErrorString (GLenum error);
extern void APIENTRY OS2gluGetNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat* data);
extern const GLubyte * APIENTRY OS2gluGetString (GLenum name);
extern void APIENTRY OS2gluGetTessProperty (GLUtesselator* tess, GLenum which, GLdouble* data);
extern void APIENTRY OS2gluLoadSamplingMatrices (GLUnurbs* nurb, const GLfloat *model, const GLfloat *perspective, const GLint *view);
extern void APIENTRY OS2gluLookAt (GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
extern GLUnurbs* APIENTRY OS2gluNewNurbsRenderer (void);
extern GLUquadric* APIENTRY OS2gluNewQuadric (void);
extern GLUtesselator* APIENTRY OS2gluNewTess (void);
extern void APIENTRY OS2gluNextContour (GLUtesselator* tess, GLenum type);
extern void APIENTRY OS2gluNurbsCallback (GLUnurbs* nurb, GLenum which, GLvoid (* APIENTRY CallBackFunc)());
extern void APIENTRY OS2gluNurbsCurve (GLUnurbs* nurb, GLint knotCount, GLfloat *knots, GLint stride, GLfloat *control, GLint order, GLenum type);
extern void APIENTRY OS2gluNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat value);
extern void APIENTRY OS2gluNurbsSurface (GLUnurbs* nurb, GLint sKnotCount, GLfloat* sKnots, GLint tKnotCount, GLfloat* tKnots, GLint sStride, GLint tStride, GLfloat* control, GLint sOrder, GLint tOrder, GLenum type);
extern void APIENTRY OS2gluOrtho2D (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
extern void APIENTRY OS2gluPartialDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops, GLdouble start, GLdouble sweep);
extern void APIENTRY OS2gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
extern void APIENTRY OS2gluPickMatrix (GLdouble x, GLdouble y, GLdouble delX, GLdouble delY, GLint *viewport);
extern GLint APIENTRY OS2gluProject (GLdouble objX, GLdouble objY, GLdouble objZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* winX, GLdouble* winY, GLdouble* winZ);
extern void APIENTRY OS2gluPwlCurve (GLUnurbs* nurb, GLint count, GLfloat* data, GLint stride, GLenum type);
extern void APIENTRY OS2gluQuadricCallback (GLUquadric* quad, GLenum which, GLvoid (* APIENTRY CallBackFunc)());
extern void APIENTRY OS2gluQuadricDrawStyle (GLUquadric* quad, GLenum draw);
extern void APIENTRY OS2gluQuadricNormals (GLUquadric* quad, GLenum normal);
extern void APIENTRY OS2gluQuadricOrientation (GLUquadric* quad, GLenum orientation);
extern void APIENTRY OS2gluQuadricTexture (GLUquadric* quad, GLboolean texture);
extern GLint APIENTRY OS2gluScaleImage (GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut);
extern void APIENTRY OS2gluSphere (GLUquadric* quad, GLdouble radius, GLint slices, GLint stacks);
extern void APIENTRY OS2gluTessBeginContour (GLUtesselator* tess);
extern void APIENTRY OS2gluTessBeginPolygon (GLUtesselator* tess, GLvoid* data);
extern void APIENTRY OS2gluTessCallback (GLUtesselator* tess, GLenum which, GLvoid (* APIENTRY CallBackFunc)());
extern void APIENTRY OS2gluTessEndContour (GLUtesselator* tess);
extern void APIENTRY OS2gluTessEndPolygon (GLUtesselator* tess);
extern void APIENTRY OS2gluTessNormal (GLUtesselator* tess, GLdouble valueX, GLdouble valueY, GLdouble valueZ);
extern void APIENTRY OS2gluTessProperty (GLUtesselator* tess, GLenum which, GLdouble data);
extern void APIENTRY OS2gluTessVertex (GLUtesselator* tess, GLdouble *location, GLvoid* data);
extern GLint APIENTRY OS2gluUnProject (GLdouble winX, GLdouble winY, GLdouble winZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* objX, GLdouble* objY, GLdouble* objZ);

#ifdef __cplusplus
}
#endif

#endif /* __gluos2_h__ */
