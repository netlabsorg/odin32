/* $Id: glutos2.h,v 1.1 2000-01-27 21:49:13 sandervl Exp $ */
#ifndef __glutos2_h__
#define __glutos2_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLUT_API_VERSION
#define GLUT_API_VERSION                2
#endif
#ifndef GLUT_XLIB_IMPLEMENTATION
#define GLUT_XLIB_IMPLEMENTATION        2
#endif

extern void APIENTRY OS2glutInit (int *, char **);
extern void APIENTRY OS2glutInitDisplayMode (unsigned long);
extern void APIENTRY OS2glutInitWindowPosition (int, int);
extern void APIENTRY OS2glutInitWindowSize (int, int);
extern void APIENTRY OS2glutMainLoop (void);
extern int APIENTRY OS2glutCreateWindow (char *);
extern int APIENTRY OS2glutCreateSubWindow (int, int, int, int, int);
extern void APIENTRY OS2glutDestroyWindow (int);
extern void APIENTRY OS2glutPostRedisplay (void);
extern void APIENTRY OS2glutSwapBuffers (void);
extern int APIENTRY OS2glutGetWindow (void);
extern void APIENTRY OS2glutSetWindow (int);
extern void APIENTRY OS2glutSetWindowTitle (char *);
extern void APIENTRY OS2glutSetIconTitle (char *);
extern void APIENTRY OS2glutPositionWindow (int, int);
extern void APIENTRY OS2glutReshapeWindow (int, int);
extern void APIENTRY OS2glutPopWindow (void);
extern void APIENTRY OS2glutPushWindow (void);
extern void APIENTRY OS2glutIconifyWindow (void);
extern void APIENTRY OS2glutShowWindow (void);
extern void APIENTRY OS2glutHideWindow (void);
extern int APIENTRY OS2glutCreateMenu (void (* APIENTRY)(int));
extern void APIENTRY OS2glutDestroyMenu (int);
extern int APIENTRY OS2glutGetMenu (void);
extern void APIENTRY OS2glutSetMenu (int);
extern void APIENTRY OS2glutAddMenuEntry (char *, int);
extern void APIENTRY OS2glutAddSubMenu (char *, int);
extern void APIENTRY OS2glutChangeToMenuEntry (int, char *, int);
extern void APIENTRY OS2glutChangeToSubMenu (int, char *, int);
extern void APIENTRY OS2glutRemoveMenuItem (int);
extern void APIENTRY OS2glutAttachMenu (int);
extern void APIENTRY OS2glutDetachMenu (int);
extern void APIENTRY OS2glutDisplayFunc (void (* APIENTRY)(void));
extern void APIENTRY OS2glutReshapeFunc (void (* APIENTRY)(int, int));
extern void APIENTRY OS2glutKeyboardFunc (void (* APIENTRY)(unsigned char, int, int));
extern void APIENTRY OS2glutMouseFunc (void (* APIENTRY)(int, int, int, int));
extern void APIENTRY OS2glutMotionFunc (void (* APIENTRY)(int, int));
extern void APIENTRY OS2glutPassiveMotionFunc (void (* APIENTRY)(int, int));
extern void APIENTRY OS2glutEntryFunc (void (* APIENTRY)(int));
extern void APIENTRY OS2glutVisibilityFunc (void (* APIENTRY)(int));
extern void APIENTRY OS2glutIdleFunc (void (* APIENTRY)(void));
extern void APIENTRY OS2glutTimerFunc (unsigned long, void (* APIENTRY)(int), int);
extern void APIENTRY OS2glutMenuStateFunc (void (* APIENTRY)(int));
extern void APIENTRY OS2glutSpecialFunc (void (* APIENTRY)(int, int, int));
extern void APIENTRY OS2glutSpaceballMotionFunc (void (* APIENTRY)(int, int, int));
extern void APIENTRY OS2glutSpaceballRotateFunc (void (* APIENTRY)(int, int, int));
extern void APIENTRY OS2glutSpaceballButtonFunc (void (* APIENTRY)(int, int));
extern void APIENTRY OS2glutButtonBoxFunc (void (* APIENTRY)(int, int));
extern void APIENTRY OS2glutDialsFunc (void (* APIENTRY)(int, int));
extern void APIENTRY OS2glutTabletMotionFunc (void (* APIENTRY)(int, int));
extern void APIENTRY OS2glutTabletButtonFunc (void (* APIENTRY)(int, int, int, int));
extern void APIENTRY OS2glutSetColor (int, GLfloat, GLfloat, GLfloat);
extern GLfloat APIENTRY OS2glutGetColor (int, int);
extern void APIENTRY OS2glutCopyColormap (int);
extern int APIENTRY OS2glutGet (GLenum);
extern int APIENTRY OS2glutDeviceGet (GLenum);
extern void APIENTRY OS2glutStrokeCharacter (void *, int);
extern void APIENTRY OS2glutBitmapCharacter (void *, int);
extern void APIENTRY OS2glutWireSphere (GLdouble, GLint, GLint);
extern void APIENTRY OS2glutSolidSphere (GLdouble, GLint, GLint);
extern void APIENTRY OS2glutWireCone (GLdouble, GLdouble, GLint, GLint);
extern void APIENTRY OS2glutSolidCone (GLdouble, GLdouble, GLint, GLint);
extern void APIENTRY OS2glutWireCube (GLdouble);
extern void APIENTRY OS2glutSolidCube (GLdouble);
extern void APIENTRY OS2glutWireTorus (GLdouble, GLdouble, GLint, GLint);
extern void APIENTRY OS2glutSolidTorus (GLdouble, GLdouble, GLint, GLint);
extern void APIENTRY OS2glutWireDodecahedron (void);
extern void APIENTRY OS2glutSolidDodecahedron (void);
extern void APIENTRY OS2glutWireTeapot (GLdouble);
extern void APIENTRY OS2glutSolidTeapot (GLdouble);
extern void APIENTRY OS2glutWireOctahedron (void);
extern void APIENTRY OS2glutSolidOctahedron (void);
extern void APIENTRY OS2glutWireTetrahedron (void);
extern void APIENTRY OS2glutSolidTetrahedron (void);
extern void APIENTRY OS2glutWireIcosahedron (void);
extern void APIENTRY OS2glutSolidIcosahedron (void);
extern int APIENTRY OS2glutExtensionSupported (char *);

#ifdef __cplusplus
}
#endif

#endif /* __glutos2_h__ */
