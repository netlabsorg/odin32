/* $Id: glut.h,v 1.3 2000-02-29 13:56:51 sandervl Exp $ */
#ifndef __glut_h__
#define __glut_h__

/* Copyright (c) Mark J. Kilgard, 1994, 1995, 1996, 1998. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

#if defined(_WIN32)

/* GLUT 3.7 now tries to avoid including <windows.h>
   to avoid name space pollution, but Win32's <GL/gl.h>
   needs APIENTRY and WINGDIAPI defined properly. */
# if 0
#  define  WIN32_LEAN_AND_MEAN
#  include <windows.h>
# else
   /* XXX This is from Win32's <windef.h> */
#  ifndef APIENTRY
#   define GLUT_APIENTRY_DEFINED
#   if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#    define APIENTRY    __stdcall
#   else
#    define APIENTRY
#   endif
#  endif
   /* XXX This is from Win32's <winnt.h> */
#  ifndef CALLBACK
#   if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#    define CALLBACK __stdcall
#   else
#    define CALLBACK
#   endif
#  endif
   /* XXX This is from Win32's <wingdi.h> and <winnt.h> */
#  ifndef WINGDIAPI
#   define GLUT_WINGDIAPI_DEFINED
#   define WINGDIAPI __declspec(dllimport)
#  endif
   /* XXX This is from Win32's <ctype.h> */
#  ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#   define _WCHAR_T_DEFINED
#  endif
# endif

#pragma comment (lib, "winmm.lib")     /* link with Windows MultiMedia lib */
#pragma comment (lib, "opengl32.lib")  /* link with Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib")     /* link with OpenGL Utility lib     */
#pragma comment (lib, "glut32.lib")    /* link with Win32 GLUT lib         */

#pragma warning (disable:4244)        /* Disable bogus conversion warnings.*/
#pragma warning (disable:4305)        /* VC++ 5.0 version of above warning.*/

#endif

#include "gl.h"
#include "glu.h"

#include "glutstroke.h"
#include "glutbitmap.h"

/* define APIENTRY and CALLBACK to null string if we aren't on Win32 */
#if !defined(_WIN32) && !defined(__WIN32OS2__)
#define APIENTRY
#define GLUT_APIENTRY_DEFINED
#define CALLBACK
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 GLUT API revision history:

 GLUT_API_VERSION is updated to reflect incompatible GLUT
 API changes (interface changes, semantic changes, deletions,
 or additions).

 GLUT_API_VERSION=1  First public release of GLUT.  11/29/94

 GLUT_API_VERSION=2  Added support for OpenGL/GLX multisampling,
 extension.  Supports new input devices like tablet, dial and button
 box, and Spaceball.  Easy to query OpenGL extensions.

 GLUT_API_VERSION=3  glutMenuStatus added.

 GLUT_API_VERSION=4  glutInitDisplayString, glutWarpPointer,
 glutBitmapLength, glutStrokeLength, glutWindowStatusFunc, dynamic
 video resize subAPI, glutPostWindowRedisplay, glutKeyboardUpFunc,
 glutSpecialUpFunc, glutIgnoreKeyRepeat, glutSetKeyRepeat,
 glutJoystickFunc, glutForceJoystickFunc (NOT FINALIZED!).
**/
#ifndef GLUT_API_VERSION  /* allow this to be overriden */
#define GLUT_API_VERSION                3
#endif

/**
 GLUT implementation revision history:

 GLUT_XLIB_IMPLEMENTATION is updated to reflect both GLUT
 API revisions and implementation revisions (ie, bug fixes).

 GLUT_XLIB_IMPLEMENTATION=1  mjk's first public release of
 GLUT Xlib-based implementation.  11/29/94

 GLUT_XLIB_IMPLEMENTATION=2  mjk's second public release of
 GLUT Xlib-based implementation providing GLUT version 2
 interfaces.

 GLUT_XLIB_IMPLEMENTATION=3  mjk's GLUT 2.2 images. 4/17/95

 GLUT_XLIB_IMPLEMENTATION=4  mjk's GLUT 2.3 images. 6/?/95

 GLUT_XLIB_IMPLEMENTATION=5  mjk's GLUT 3.0 images. 10/?/95

 GLUT_XLIB_IMPLEMENTATION=7  mjk's GLUT 3.1+ with glutWarpPoitner.  7/24/96

 GLUT_XLIB_IMPLEMENTATION=8  mjk's GLUT 3.1+ with glutWarpPoitner
 and video resize.  1/3/97

 GLUT_XLIB_IMPLEMENTATION=9 mjk's GLUT 3.4 release with early GLUT 4 routines.

 GLUT_XLIB_IMPLEMENTATION=11 Mesa 2.5's GLUT 3.6 release.

 GLUT_XLIB_IMPLEMENTATION=12 mjk's GLUT 3.6 release with early GLUT 4 routines + signal handling.

 GLUT_XLIB_IMPLEMENTATION=13 mjk's GLUT 3.7 release with GameGLUT support.
**/
#ifndef GLUT_XLIB_IMPLEMENTATION  /* Allow this to be overriden. */
#define GLUT_XLIB_IMPLEMENTATION        13
#endif

/* Display mode bit masks. */
#define GLUT_RGB                        0
#define GLUT_RGBA                       GLUT_RGB
#define GLUT_INDEX                      1
#define GLUT_SINGLE                     0
#define GLUT_DOUBLE                     2
#define GLUT_ACCUM                      4
#define GLUT_ALPHA                      8
#define GLUT_DEPTH                      16
#define GLUT_STENCIL                    32
#if (GLUT_API_VERSION >= 2)
#define GLUT_MULTISAMPLE                128
#define GLUT_STEREO                     256
#endif
#if (GLUT_API_VERSION >= 3)
#define GLUT_LUMINANCE                  512
#endif

/* Mouse buttons. */
#define GLUT_LEFT_BUTTON                0
#define GLUT_MIDDLE_BUTTON              1
#define GLUT_RIGHT_BUTTON               2

/* Mouse button  state. */
#define GLUT_DOWN                       0
#define GLUT_UP                         1

#if (GLUT_API_VERSION >= 2)
/* function keys */
#define GLUT_KEY_F1                     1
#define GLUT_KEY_F2                     2
#define GLUT_KEY_F3                     3
#define GLUT_KEY_F4                     4
#define GLUT_KEY_F5                     5
#define GLUT_KEY_F6                     6
#define GLUT_KEY_F7                     7
#define GLUT_KEY_F8                     8
#define GLUT_KEY_F9                     9
#define GLUT_KEY_F10                    10
#define GLUT_KEY_F11                    11
#define GLUT_KEY_F12                    12
/* directional keys */
#define GLUT_KEY_LEFT                   100
#define GLUT_KEY_UP                     101
#define GLUT_KEY_RIGHT                  102
#define GLUT_KEY_DOWN                   103
#define GLUT_KEY_PAGE_UP                104
#define GLUT_KEY_PAGE_DOWN              105
#define GLUT_KEY_HOME                   106
#define GLUT_KEY_END                    107
#define GLUT_KEY_INSERT                 108
#endif

/* Entry/exit  state. */
#define GLUT_LEFT                       0
#define GLUT_ENTERED                    1

/* Menu usage  state. */
#define GLUT_MENU_NOT_IN_USE            0
#define GLUT_MENU_IN_USE                1

/* Visibility  state. */
#define GLUT_NOT_VISIBLE                0
#define GLUT_VISIBLE                    1

/* Window status  state. */
#define GLUT_HIDDEN                     0
#define GLUT_FULLY_RETAINED             1
#define GLUT_PARTIALLY_RETAINED         2
#define GLUT_FULLY_COVERED              3

/* Color index component selection values. */
#define GLUT_RED                        0
#define GLUT_GREEN                      1
#define GLUT_BLUE                       2

/* Layers for use. */
#define GLUT_NORMAL                     0
#define GLUT_OVERLAY                    1

#if defined(_WIN32) || defined(__WIN32OS2__)
/* Stroke font constants (use these in GLUT program). */
#define GLUT_STROKE_ROMAN               ((void*)0)
#define I_GLUT_STROKE_ROMAN             0
#define GLUT_STROKE_MONO_ROMAN          ((void*)1)
#define I_GLUT_STROKE_MONO_ROMAN        1

/* Bitmap font constants (use these in GLUT program). */
#define GLUT_BITMAP_9_BY_15             ((void*)2)
#define I_GLUT_BITMAP_9_BY_15           2
#define GLUT_BITMAP_8_BY_13             ((void*)3)
#define I_GLUT_BITMAP_8_BY_13           3
#define GLUT_BITMAP_TIMES_ROMAN_10      ((void*)4)
#define I_GLUT_BITMAP_TIMES_ROMAN_10    4
#define GLUT_BITMAP_TIMES_ROMAN_24      ((void*)5)
#define I_GLUT_BITMAP_TIMES_ROMAN_24    5
#if (GLUT_API_VERSION >= 3)
#define GLUT_BITMAP_HELVETICA_10        ((void*)6)
#define I_GLUT_BITMAP_HELVETICA_10      6
#define GLUT_BITMAP_HELVETICA_12        ((void*)7)
#define I_GLUT_BITMAP_HELVETICA_12      7
#define GLUT_BITMAP_HELVETICA_18        ((void*)8)
#define I_GLUT_BITMAP_HELVETICA_18      8
#endif
#else
/* Stroke font opaque addresses (use constants instead in source code). */
extern const StrokeFontRec glutStrokeRoman;
extern const StrokeFontRec glutStrokeMonoRoman;

/* Stroke font constants (use these in GLUT program). */
#define GLUT_STROKE_ROMAN               (&glutStrokeRoman)
#define GLUT_STROKE_MONO_ROMAN          (&glutStrokeMonoRoman)

/* Bitmap font opaque addresses (use constants instead in source code). */
extern const BitmapFontRec glutBitmap9By15;
extern const BitmapFontRec glutBitmap8By13;
extern const BitmapFontRec glutBitmapTimesRoman10;
extern const BitmapFontRec glutBitmapTimesRoman24;
extern const BitmapFontRec glutBitmapHelvetica10;
extern const BitmapFontRec glutBitmapHelvetica12;
extern const BitmapFontRec glutBitmapHelvetica18;

/* Bitmap font constants (use these in GLUT program). */
#define GLUT_BITMAP_9_BY_15             (&glutBitmap9By15)
#define GLUT_BITMAP_8_BY_13             (&glutBitmap8By13)
#define GLUT_BITMAP_TIMES_ROMAN_10      (&glutBitmapTimesRoman10)
#define GLUT_BITMAP_TIMES_ROMAN_24      (&glutBitmapTimesRoman24)
#if (GLUT_API_VERSION >= 3)
#define GLUT_BITMAP_HELVETICA_10        (&glutBitmapHelvetica10)
#define GLUT_BITMAP_HELVETICA_12        (&glutBitmapHelvetica12)
#define GLUT_BITMAP_HELVETICA_18        (&glutBitmapHelvetica18)
#endif
#endif

/* glutGet parameters. */
#define GLUT_WINDOW_X                   100
#define GLUT_WINDOW_Y                   101
#define GLUT_WINDOW_WIDTH               102
#define GLUT_WINDOW_HEIGHT              103
#define GLUT_WINDOW_BUFFER_SIZE         104
#define GLUT_WINDOW_STENCIL_SIZE        105
#define GLUT_WINDOW_DEPTH_SIZE          106
#define GLUT_WINDOW_RED_SIZE            107
#define GLUT_WINDOW_GREEN_SIZE          108
#define GLUT_WINDOW_BLUE_SIZE           109
#define GLUT_WINDOW_ALPHA_SIZE          110
#define GLUT_WINDOW_ACCUM_RED_SIZE      111
#define GLUT_WINDOW_ACCUM_GREEN_SIZE    112
#define GLUT_WINDOW_ACCUM_BLUE_SIZE     113
#define GLUT_WINDOW_ACCUM_ALPHA_SIZE    114
#define GLUT_WINDOW_DOUBLEBUFFER        115
#define GLUT_WINDOW_RGBA                116
#define GLUT_WINDOW_PARENT              117
#define GLUT_WINDOW_NUM_CHILDREN        118
#define GLUT_WINDOW_COLORMAP_SIZE       119
#if (GLUT_API_VERSION >= 2)
#define GLUT_WINDOW_NUM_SAMPLES         120
#define GLUT_WINDOW_STEREO              121
#endif
#if (GLUT_API_VERSION >= 3)
#define GLUT_WINDOW_CURSOR              122
#endif
#define GLUT_SCREEN_WIDTH               200
#define GLUT_SCREEN_HEIGHT              201
#define GLUT_SCREEN_WIDTH_MM            202
#define GLUT_SCREEN_HEIGHT_MM           203
#define GLUT_MENU_NUM_ITEMS             300
#define GLUT_DISPLAY_MODE_POSSIBLE      400
#define GLUT_INIT_WINDOW_X              500
#define GLUT_INIT_WINDOW_Y              501
#define GLUT_INIT_WINDOW_WIDTH          502
#define GLUT_INIT_WINDOW_HEIGHT         503
#define GLUT_INIT_DISPLAY_MODE          504
#if (GLUT_API_VERSION >= 2)
#define GLUT_ELAPSED_TIME               700
#endif
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
#define GLUT_WINDOW_FORMAT_ID           123
#endif

#if (GLUT_API_VERSION >= 2)
/* glutDeviceGet parameters. */
#define GLUT_HAS_KEYBOARD               600
#define GLUT_HAS_MOUSE                  601
#define GLUT_HAS_SPACEBALL              602
#define GLUT_HAS_DIAL_AND_BUTTON_BOX    603
#define GLUT_HAS_TABLET                 604
#define GLUT_NUM_MOUSE_BUTTONS          605
#define GLUT_NUM_SPACEBALL_BUTTONS      606
#define GLUT_NUM_BUTTON_BOX_BUTTONS     607
#define GLUT_NUM_DIALS                  608
#define GLUT_NUM_TABLET_BUTTONS         609
#endif
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
#define GLUT_DEVICE_IGNORE_KEY_REPEAT   610
#define GLUT_DEVICE_KEY_REPEAT          611
#define GLUT_HAS_JOYSTICK               612
#define GLUT_OWNS_JOYSTICK              613
#define GLUT_JOYSTICK_BUTTONS           614
#define GLUT_JOYSTICK_AXES              615
#define GLUT_JOYSTICK_POLL_RATE         616
#endif

#if (GLUT_API_VERSION >= 3)
/* glutLayerGet parameters. */
#define GLUT_OVERLAY_POSSIBLE           800
#define GLUT_LAYER_IN_USE               801
#define GLUT_HAS_OVERLAY                802
#define GLUT_TRANSPARENT_INDEX          803
#define GLUT_NORMAL_DAMAGED             804
#define GLUT_OVERLAY_DAMAGED            805

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
/* glutVideoResizeGet parameters. */
#define GLUT_VIDEO_RESIZE_POSSIBLE      900
#define GLUT_VIDEO_RESIZE_IN_USE        901
#define GLUT_VIDEO_RESIZE_X_DELTA       902
#define GLUT_VIDEO_RESIZE_Y_DELTA       903
#define GLUT_VIDEO_RESIZE_WIDTH_DELTA   904
#define GLUT_VIDEO_RESIZE_HEIGHT_DELTA  905
#define GLUT_VIDEO_RESIZE_X             906
#define GLUT_VIDEO_RESIZE_Y             907
#define GLUT_VIDEO_RESIZE_WIDTH         908
#define GLUT_VIDEO_RESIZE_HEIGHT        909
#endif

/* glutUseLayer parameters. */
#define GLUT_NORMAL                     0
#define GLUT_OVERLAY                    1

/* glutGetModifiers return mask. */
#define GLUT_ACTIVE_SHIFT               1
#define GLUT_ACTIVE_CTRL                2
#define GLUT_ACTIVE_ALT                 4

/* glutSetCursor parameters. */
/* Basic arrows. */
#define GLUT_CURSOR_RIGHT_ARROW         0
#define GLUT_CURSOR_LEFT_ARROW          1
/* Symbolic cursor shapes. */
#define GLUT_CURSOR_INFO                2
#define GLUT_CURSOR_DESTROY             3
#define GLUT_CURSOR_HELP                4
#define GLUT_CURSOR_CYCLE               5
#define GLUT_CURSOR_SPRAY               6
#define GLUT_CURSOR_WAIT                7
#define GLUT_CURSOR_TEXT                8
#define GLUT_CURSOR_CROSSHAIR           9
/* Directional cursors. */
#define GLUT_CURSOR_UP_DOWN             10
#define GLUT_CURSOR_LEFT_RIGHT          11
/* Sizing cursors. */
#define GLUT_CURSOR_TOP_SIDE            12
#define GLUT_CURSOR_BOTTOM_SIDE         13
#define GLUT_CURSOR_LEFT_SIDE           14
#define GLUT_CURSOR_RIGHT_SIDE          15
#define GLUT_CURSOR_TOP_LEFT_CORNER     16
#define GLUT_CURSOR_TOP_RIGHT_CORNER    17
#define GLUT_CURSOR_BOTTOM_RIGHT_CORNER 18
#define GLUT_CURSOR_BOTTOM_LEFT_CORNER  19
/* Inherit from parent window. */
#define GLUT_CURSOR_INHERIT             100
/* Blank cursor. */
#define GLUT_CURSOR_NONE                101
/* Fullscreen crosshair (if available). */
#define GLUT_CURSOR_FULL_CROSSHAIR      102
#endif

/* GLUT initialization sub-API. */
extern void GLAPIENTRY glutInit(int *argcp, char **argv);
extern void GLAPIENTRY glutInitDisplayMode(unsigned int mode);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
extern void GLAPIENTRY glutInitDisplayString(const char *string);
#endif
extern void GLAPIENTRY glutInitWindowPosition(int x, int y);
extern void GLAPIENTRY glutInitWindowSize(int width, int height);
extern void GLAPIENTRY glutMainLoop(void);

/* GLUT window sub-API. */
extern int GLAPIENTRY glutCreateWindow(const char *title);
extern int GLAPIENTRY glutCreateSubWindow(int win, int x, int y, int width, int height);
extern void GLAPIENTRY glutDestroyWindow(int win);
extern void GLAPIENTRY glutPostRedisplay(void);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 11)
extern void GLAPIENTRY glutPostWindowRedisplay(int win);
#endif
extern void GLAPIENTRY glutSwapBuffers(void);
extern int GLAPIENTRY glutGetWindow(void);
extern void GLAPIENTRY glutSetWindow(int win);
extern void GLAPIENTRY glutSetWindowTitle(const char *title);
extern void GLAPIENTRY glutSetIconTitle(const char *title);
extern void GLAPIENTRY glutPositionWindow(int x, int y);
extern void GLAPIENTRY glutReshapeWindow(int width, int height);
extern void GLAPIENTRY glutPopWindow(void);
extern void GLAPIENTRY glutPushWindow(void);
extern void GLAPIENTRY glutIconifyWindow(void);
extern void GLAPIENTRY glutShowWindow(void);
extern void GLAPIENTRY glutHideWindow(void);
#if (GLUT_API_VERSION >= 3)
extern void GLAPIENTRY glutFullScreen(void);
extern void GLAPIENTRY glutSetCursor(int cursor);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
extern void GLAPIENTRY glutWarpPointer(int x, int y);
#endif

/* GLUT overlay sub-API. */
extern void GLAPIENTRY glutEstablishOverlay(void);
extern void GLAPIENTRY glutRemoveOverlay(void);
extern void GLAPIENTRY glutUseLayer(GLenum layer);
extern void GLAPIENTRY glutPostOverlayRedisplay(void);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 11)
extern void GLAPIENTRY glutPostWindowOverlayRedisplay(int win);
#endif
extern void GLAPIENTRY glutShowOverlay(void);
extern void GLAPIENTRY glutHideOverlay(void);
#endif

/* GLUT menu sub-API. */
extern int GLAPIENTRY glutCreateMenu(void (*)(int));
extern void GLAPIENTRY glutDestroyMenu(int menu);
extern int GLAPIENTRY glutGetMenu(void);
extern void GLAPIENTRY glutSetMenu(int menu);
extern void GLAPIENTRY glutAddMenuEntry(const char *label, int value);
extern void GLAPIENTRY glutAddSubMenu(const char *label, int submenu);
extern void GLAPIENTRY glutChangeToMenuEntry(int item, const char *label, int value);
extern void GLAPIENTRY glutChangeToSubMenu(int item, const char *label, int submenu);
extern void GLAPIENTRY glutRemoveMenuItem(int item);
extern void GLAPIENTRY glutAttachMenu(int button);
extern void GLAPIENTRY glutDetachMenu(int button);

/* GLUT window callback sub-API. */
extern void GLAPIENTRY glutDisplayFunc(void (*func)(void));
extern void GLAPIENTRY glutReshapeFunc(void (*func)(int width, int height));
extern void GLAPIENTRY glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
extern void GLAPIENTRY glutMouseFunc(void (*func)(int button, int state, int x, int y));
extern void GLAPIENTRY glutMotionFunc(void (*func)(int x, int y));
extern void GLAPIENTRY glutPassiveMotionFunc(void (*func)(int x, int y));
extern void GLAPIENTRY glutEntryFunc(void (*func)(int state));
extern void GLAPIENTRY glutVisibilityFunc(void (*func)(int state));
extern void GLAPIENTRY glutIdleFunc(void (*func)(void));
extern void GLAPIENTRY glutTimerFunc(unsigned int millis, void (*func)(int value), int value);
extern void GLAPIENTRY glutMenuStateFunc(void (*func)(int state));
#if (GLUT_API_VERSION >= 2)
extern void GLAPIENTRY glutSpecialFunc(void (*func)(int key, int x, int y));
extern void GLAPIENTRY glutSpaceballMotionFunc(void (*func)(int x, int y, int z));
extern void GLAPIENTRY glutSpaceballRotateFunc(void (*func)(int x, int y, int z));
extern void GLAPIENTRY glutSpaceballButtonFunc(void (*func)(int button, int state));
extern void GLAPIENTRY glutButtonBoxFunc(void (*func)(int button, int state));
extern void GLAPIENTRY glutDialsFunc(void (*func)(int dial, int value));
extern void GLAPIENTRY glutTabletMotionFunc(void (*func)(int x, int y));
extern void GLAPIENTRY glutTabletButtonFunc(void (*func)(int button, int state, int x, int y));
#if (GLUT_API_VERSION >= 3)
extern void GLAPIENTRY glutMenuStatusFunc(void (*func)(int status, int x, int y));
extern void GLAPIENTRY glutOverlayDisplayFunc(void (*func)(void));
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
extern void GLAPIENTRY glutWindowStatusFunc(void (*func)(int state));
#endif
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
extern void GLAPIENTRY glutKeyboardUpFunc(void (*func)(unsigned char key, int x, int y));
extern void GLAPIENTRY glutSpecialUpFunc(void (*func)(int key, int x, int y));
extern void GLAPIENTRY glutJoystickFunc(void (*func)(unsigned int buttonMask, int x, int y, int z), int pollInterval);
#endif
#endif
#endif

/* GLUT color index sub-API. */
extern void GLAPIENTRY glutSetColor(int, GLfloat red, GLfloat green, GLfloat blue);
extern GLfloat GLAPIENTRY glutGetColor(int ndx, int component);
extern void GLAPIENTRY glutCopyColormap(int win);

/* GLUT state retrieval sub-API. */
extern int GLAPIENTRY glutGet(GLenum type);
extern int GLAPIENTRY glutDeviceGet(GLenum type);
#if (GLUT_API_VERSION >= 2)
/* GLUT extension support sub-API */
extern int GLAPIENTRY glutExtensionSupported(const char *name);
#endif
#if (GLUT_API_VERSION >= 3)
extern int GLAPIENTRY glutGetModifiers(void);
extern int GLAPIENTRY glutLayerGet(GLenum type);
#endif

/* GLUT font sub-API */
extern void GLAPIENTRY glutBitmapCharacter(void *font, int character);
extern int GLAPIENTRY glutBitmapWidth(void *font, int character);
extern void GLAPIENTRY glutStrokeCharacter(void *font, int character);
extern int GLAPIENTRY glutStrokeWidth(void *font, int character);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
extern int GLAPIENTRY glutBitmapLength(void *font, const unsigned char *string);
extern int GLAPIENTRY glutStrokeLength(void *font, const unsigned char *string);
#endif

/* GLUT pre-built models sub-API */
extern void GLAPIENTRY glutWireSphere(GLdouble radius, GLint slices, GLint stacks);
extern void GLAPIENTRY glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
extern void GLAPIENTRY glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
extern void GLAPIENTRY glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
extern void GLAPIENTRY glutWireCube(GLdouble size);
extern void GLAPIENTRY glutSolidCube(GLdouble size);
extern void GLAPIENTRY glutWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
extern void GLAPIENTRY glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
extern void GLAPIENTRY glutWireDodecahedron(void);
extern void GLAPIENTRY glutSolidDodecahedron(void);
extern void GLAPIENTRY glutWireTeapot(GLdouble size);
extern void GLAPIENTRY glutSolidTeapot(GLdouble size);
extern void GLAPIENTRY glutWireOctahedron(void);
extern void GLAPIENTRY glutSolidOctahedron(void);
extern void GLAPIENTRY glutWireTetrahedron(void);
extern void GLAPIENTRY glutSolidTetrahedron(void);
extern void GLAPIENTRY glutWireIcosahedron(void);
extern void GLAPIENTRY glutSolidIcosahedron(void);

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
/* GLUT video resize sub-API. */
extern int GLAPIENTRY glutVideoResizeGet(GLenum param);
extern void GLAPIENTRY glutSetupVideoResizing(void);
extern void GLAPIENTRY glutStopVideoResizing(void);
extern void GLAPIENTRY glutVideoResize(int x, int y, int width, int height);
extern void GLAPIENTRY glutVideoPan(int x, int y, int width, int height);

/* GLUT debugging sub-API. */
extern void GLAPIENTRY glutReportErrors(void);
#endif

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
/* GLUT device control sub-API. */
/* glutSetKeyRepeat modes. */
#define GLUT_KEY_REPEAT_OFF             0
#define GLUT_KEY_REPEAT_ON              1
#define GLUT_KEY_REPEAT_DEFAULT         2

/* Joystick button masks. */
#define GLUT_JOYSTICK_BUTTON_A          1
#define GLUT_JOYSTICK_BUTTON_B          2
#define GLUT_JOYSTICK_BUTTON_C          4
#define GLUT_JOYSTICK_BUTTON_D          8

extern void GLAPIENTRY glutIgnoreKeyRepeat(int ignore);
extern void GLAPIENTRY glutSetKeyRepeat(int repeatMode);
extern void GLAPIENTRY glutForceJoystickFunc(void);

/* GLUT game mode sub-API. */
/* glutGameModeGet. */
#define GLUT_GAME_MODE_ACTIVE           0
#define GLUT_GAME_MODE_POSSIBLE         1
#define GLUT_GAME_MODE_WIDTH            2
#define GLUT_GAME_MODE_HEIGHT           3
#define GLUT_GAME_MODE_PIXEL_DEPTH      4
#define GLUT_GAME_MODE_REFRESH_RATE     5
#define GLUT_GAME_MODE_DISPLAY_CHANGED  6

extern void GLAPIENTRY glutGameModeString(const char *string);
extern int GLAPIENTRY glutEnterGameMode(void);
extern void GLAPIENTRY glutLeaveGameMode(void);
extern int GLAPIENTRY glutGameModeGet(GLenum mode);
#endif

#ifdef __cplusplus
}

#endif

#ifdef GLUT_APIENTRY_DEFINED
# undef GLUT_APIENTRY_DEFINED
# undef APIENTRY
#endif

#ifdef GLUT_WINGDIAPI_DEFINED
# undef GLUT_WINGDIAPI_DEFINED
# undef WINGDIAPI
#endif

#endif                  /* __glut_h__ */
