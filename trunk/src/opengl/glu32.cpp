/* $Id: glu32.cpp,v 1.1 2000-01-27 21:49:12 sandervl Exp $ */
/*****************************************************************************/
/*                                                                           */
/* GLU32 Toolkit                                                             */
/*                                                                           */
/*****************************************************************************/
#define INCL_BASE
#include <os2.h>
#include <os2wrap.h>
#include <misc.h>
#include <malloc.h>
#include <memory.h>

#include "glos2.h"
#include "gluos2.h"
#include "glucb32.h"

/*****************************************************************************/
/*                                                                           */
/* glu Functions                                                             */
/*                                                                           */
/*****************************************************************************/

void WIN32API gluBeginCurve(GLUnurbs* nurb)
{
  OS2gluBeginCurve(nurb);
}

void WIN32API gluBeginPolygon(GLUtesselator* tess)
{
  OS2gluBeginPolygon(tess);
}

void WIN32API gluBeginSurface(GLUnurbs* nurb)
{
  OS2gluBeginSurface(nurb);
}

void WIN32API gluBeginTrim(GLUnurbs* nurb)
{
  OS2gluBeginTrim(nurb);
}

GLint WIN32API gluBuild1DMipmaps(GLenum target,
                                 GLint component,
                                 GLsizei width,
                                 GLenum format,
                                 GLenum type,
                                 const void *data)
{
  return OS2gluBuild1DMipmaps(target,component,width,format,type,data);
}

GLint WIN32API gluBuild2DMipmaps(GLenum target,
                                 GLint component,
                                 GLsizei width,
                                 GLsizei height,
                                 GLenum format,
                                 GLenum type,
                                 const void *data)
{
  return OS2gluBuild2DMipmaps(target,component,width,height,format,type,data);
}

void WIN32API gluCylinder(GLUquadric* quad,
                          GLdouble base,
                          GLdouble top,
                          GLdouble height,
                          GLint slices,
                          GLint stacks)
{
  OS2gluCylinder(quad,base,top,height,slices,stacks);
}

void WIN32API gluDeleteNurbsRenderer(GLUnurbs* nurb)
{
  OS2gluDeleteNurbsRenderer(nurb);

  OS2glucbDestroyCallbackForObject(GLUCB_NURBS,(void *)nurb);
}

void WIN32API gluDeleteQuadric (GLUquadric* quad)
{
  OS2gluDeleteQuadric(quad);

  OS2glucbDestroyCallbackForObject(GLUCB_QUADRIC,(void *)quad);
}

void WIN32API gluDeleteTess(GLUtesselator* tess)
{
  OS2gluDeleteTess(tess);

  OS2glucbDestroyCallbackForObject(GLUCB_TESS,(void *)tess);
}

void WIN32API gluDisk(GLUquadric* quad,
                      GLdouble inner,
                      GLdouble outer,
                      GLint slices,
                      GLint loops)
{
  OS2gluDisk(quad,inner,outer,slices,loops);
}

void WIN32API gluEndCurve(GLUnurbs* nurb)
{
  OS2gluEndCurve(nurb);
}

void WIN32API gluEndPolygon(GLUtesselator* tess)
{
  OS2gluEndPolygon(tess);
}

void WIN32API gluEndSurface(GLUnurbs* nurb)
{
  OS2gluEndSurface(nurb);
}

void WIN32API gluEndTrim(GLUnurbs* nurb)
{
  OS2gluEndTrim(nurb);
}

const GLubyte * WIN32API gluErrorString(GLenum error)
{
  return OS2gluErrorString(error);
}

void WIN32API gluGetNurbsProperty(GLUnurbs* nurb,
                                  GLenum property,
                                  GLfloat* data)
{
  OS2gluGetNurbsProperty(nurb,property,data);
}

const GLubyte * WIN32API gluGetString(GLenum name)
{
  return OS2gluGetString(name);
}

void WIN32API gluGetTessProperty(GLUtesselator* tess,
                                 GLenum which,
                                 GLdouble* data)
{
  OS2gluGetTessProperty(tess,which,data);
}

void WIN32API gluLoadSamplingMatrices(GLUnurbs* nurb,
                                      const GLfloat *model,
                                      const GLfloat *perspective,
                                      const GLint *view)
{
  OS2gluLoadSamplingMatrices(nurb,model,perspective,view);
}

void WIN32API gluLookAt(GLdouble eyeX,
                        GLdouble eyeY,
                        GLdouble eyeZ,
                        GLdouble centerX,
                        GLdouble centerY,
                        GLdouble centerZ,
                        GLdouble upX,
                        GLdouble upY,
                        GLdouble upZ)
{
  OS2gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,upX,upY,upZ);
}

GLUnurbs * WIN32API gluNewNurbsRenderer(void)
{
  return OS2gluNewNurbsRenderer();
}

GLUquadric * WIN32API gluNewQuadric(void)
{
  return OS2gluNewQuadric();
}

GLUtesselator * WIN32API gluNewTess(void)
{
  return OS2gluNewTess();
}

void WIN32API gluNextContour(GLUtesselator* tess,
                             GLenum type)
{
  OS2gluNextContour(tess,type);
}

void WIN32API gluNurbsCallback(GLUnurbs* nurb,
                               GLenum which,
                               GLvoid (* WIN32API CallBackFunc)())
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUCB_NURBS,nurb,which,CallBackFunc);

  OS2gluNurbsCallback(nurb,which,pfn);
}

void WIN32API gluNurbsCurve(GLUnurbs* nurb,
                            GLint knotCount,
                            GLfloat *knots,
                            GLint stride,
                            GLfloat *control,
                            GLint order,
                            GLenum type)
{
  OS2gluNurbsCurve(nurb,knotCount,knots,stride,control,order,type);
}

void WIN32API gluNurbsProperty(GLUnurbs* nurb,
                               GLenum property,
                               GLfloat value)
{
  OS2gluNurbsProperty(nurb,property,value);
}

void WIN32API gluNurbsSurface(GLUnurbs* nurb,
                              GLint sKnotCount,
                              GLfloat* sKnots,
                              GLint tKnotCount,
                              GLfloat* tKnots,
                              GLint sStride,
                              GLint tStride,
                              GLfloat* control,
                              GLint sOrder,
                              GLint tOrder,
                              GLenum type)
{
  OS2gluNurbsSurface(nurb,sKnotCount,sKnots,tKnotCount,tKnots,sStride,tStride,control,sOrder,tOrder,type);
}

void WIN32API gluOrtho2D(GLdouble left,
                         GLdouble right,
                         GLdouble bottom,
                         GLdouble top)
{
  OS2gluOrtho2D(left,right,bottom,top);
}

void WIN32API gluPartialDisk(GLUquadric* quad,
                             GLdouble inner,
                             GLdouble outer,
                             GLint slices,
                             GLint loops,
                             GLdouble start,
                             GLdouble sweep)
{
  OS2gluPartialDisk(quad,inner,outer,slices,loops,start,sweep);
}

void WIN32API gluPerspective(GLdouble fovy,
                             GLdouble aspect,
                             GLdouble zNear,
                             GLdouble zFar)
{
  OS2gluPerspective(fovy,aspect,zNear,zFar);
}

void WIN32API gluPickMatrix(GLdouble x,
                            GLdouble y,
                            GLdouble delX,
                            GLdouble delY,
                            GLint *viewport)
{
  OS2gluPickMatrix(x,y,delX,delY,viewport);
}

GLint WIN32API gluProject(GLdouble objX,
                          GLdouble objY,
                          GLdouble objZ,
                          const GLdouble *model,
                          const GLdouble *proj,
                          const GLint *view,
                          GLdouble* winX,
                          GLdouble* winY,
                          GLdouble* winZ)
{
  return OS2gluProject(objX,objY,objZ,model,proj,view,winX,winY,winZ);
}

void WIN32API gluPwlCurve(GLUnurbs* nurb,
                          GLint count,
                          GLfloat* data,
                          GLint stride,
                          GLenum type)
{
  OS2gluPwlCurve(nurb,count,data,stride,type);
}

void WIN32API gluQuadricCallback(GLUquadric* quad,
                                 GLenum which,
                                 GLvoid (* WIN32API CallBackFunc)())
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUCB_QUADRIC,(void *)quad,which,CallBackFunc);

  OS2gluQuadricCallback(quad,which,pfn);
}

void WIN32API gluQuadricDrawStyle(GLUquadric* quad,
                                  GLenum draw)
{
  OS2gluQuadricDrawStyle(quad,draw);
}

void WIN32API gluQuadricNormals(GLUquadric* quad,
                                GLenum normal)
{
  OS2gluQuadricNormals(quad,normal);
}

void WIN32API gluQuadricOrientation(GLUquadric* quad,
                                    GLenum orientation)
{
  OS2gluQuadricOrientation(quad,orientation);
}

void WIN32API gluQuadricTexture(GLUquadric* quad,
                                GLboolean texture)
{
  OS2gluQuadricTexture(quad,texture);
}

GLint WIN32API gluScaleImage(GLenum format,
                             GLsizei wIn,
                             GLsizei hIn,
                             GLenum typeIn,
                             const void *dataIn,
                             GLsizei wOut,
                             GLsizei hOut,
                             GLenum typeOut,
                             GLvoid* dataOut)
{
  return OS2gluScaleImage(format,wIn,hIn,typeIn,dataIn,wOut,hOut,typeOut,dataOut);
}

void WIN32API gluSphere(GLUquadric* quad,
                        GLdouble radius,
                        GLint slices,
                        GLint stacks)
{
  OS2gluSphere(quad,radius,slices,stacks);
}

void WIN32API gluTessBeginContour(GLUtesselator* tess)
{
  OS2gluTessBeginContour(tess);
}

void WIN32API gluTessBeginPolygon(GLUtesselator* tess,
                                  GLvoid* data)
{
  OS2gluTessBeginPolygon(tess,data);
}

void WIN32API gluTessCallback(GLUtesselator* tess,
                              GLenum which,
                              GLvoid (* WIN32API CallBackFunc)())
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUCB_TESS,(void *)tess,which,CallBackFunc);

  OS2gluTessCallback(tess,which,pfn);
}

void WIN32API gluTessEndContour(GLUtesselator* tess)
{
  OS2gluTessEndContour(tess);
}

void WIN32API gluTessEndPolygon(GLUtesselator* tess)
{
  OS2gluTessEndPolygon(tess);
}

void WIN32API gluTessNormal(GLUtesselator* tess,
                            GLdouble valueX,
                            GLdouble valueY,
                            GLdouble valueZ)
{
  OS2gluTessNormal(tess,valueX,valueY,valueZ);
}

void WIN32API gluTessProperty(GLUtesselator* tess,
                              GLenum which,
                              GLdouble data)
{
  OS2gluTessProperty(tess,which,data);
}

void WIN32API gluTessVertex(GLUtesselator* tess,
                            GLdouble *location,
                            GLvoid* data)
{
  OS2gluTessVertex(tess,location,data);
}

GLint WIN32API gluUnProject(GLdouble winX,
                            GLdouble winY,
                            GLdouble winZ,
                            const GLdouble *model,
                            const GLdouble *proj,
                            const GLint *view,
                            GLdouble* objX,
                            GLdouble* objY,
                            GLdouble* objZ)
{
  return OS2gluUnProject(winX,winY,winZ,model,proj,view,objX,objY,objZ);
}

/*****************************************************************************/
/*                                                                           */
/* Module Internal init and cleanup - called by initterm                     */
/*                                                                           */
/*****************************************************************************/

void mod_init(void)
{
  dprintf(("GLU32 INIT\n"));

  DosCreateMutexSem(NULL,&hmtxGlucb,0,0);
}

void mod_cleanup(void)
{
  while(Glucbs)
    OS2glucbDestroyGlucb(Glucbs);

  DosCloseMutexSem(hmtxGlucb);

  dprintf(("GLU32 TERM\n"));
}
