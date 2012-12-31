#ifndef __glut_h__
#define __glut_h__

/* Copyright (c) Mark J. Kilgard, 1994, 1995, 1996, 1998. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)

/* GLUT 3.7 now tries to avoid including <windows.h>
   to avoid name space pollution, but Win32's <GL/gl.h>
   needs APIENTRY and WINGDIAPI defined properly.

   tjump@spgs.com contributes:
   If users are building glut code on MS Windows, then they should
   make sure they include windows.h early, let's not get into a
   header definitions war since MS has proven it's capability to
   change header dependencies w/o publishing they have done so.

   So, let's not include windows.h here, as it's not really required and
   MS own gl/gl.h *should* include it if the dependency is there. */

/* To disable automatic library usage for GLUT, define GLUT_NO_LIB_PRAGMA
   in your compile preprocessor options. */
# if !defined(GLUT_BUILDING_LIB) && !defined(GLUT_NO_LIB_PRAGMA)
#  pragma comment (lib, "winmm.lib")      /* link with Windows MultiMedia lib */
/* To enable automatic SGI OpenGL for Windows library usage for GLUT,
   define GLUT_USE_SGI_OPENGL in your compile preprocessor options.  */
#  ifdef GLUT_USE_SGI_OPENGL
#   pragma comment (lib, "opengl.lib")    /* link with SGI OpenGL for Windows lib */
#   pragma comment (lib, "glu.lib")       /* link with SGI OpenGL Utility lib */
#   pragma comment (lib, "glut.lib")      /* link with Win32 GLUT for SGI OpenGL lib */
#  else
#   pragma comment (lib, "opengl32.lib")  /* link with Microsoft OpenGL lib */
#   pragma comment (lib, "glu32.lib")     /* link with Microsoft OpenGL Utility lib */
#   pragma comment (lib, "glut32.lib")    /* link with Win32 GLUT lib */
#  endif
# endif

/* To disable supression of annoying warnings about floats being promoted
   to doubles, define GLUT_NO_WARNING_DISABLE in your compile preprocessor
   options. */
# ifndef GLUT_NO_WARNING_DISABLE
#  pragma warning (disable:4244)  /* Disable bogus VC++ 4.2 conversion warnings. */
#  pragma warning (disable:4305)  /* VC++ 5.0 version of above warning. */
# endif

/* Win32 has an annoying issue where there are multiple C run-time
   libraries (CRTs).  If the executable is linked with a different CRT
   from the GLUT DLL, the GLUT DLL will not share the same CRT static
   data seen by the executable.  In particular, atexit callbacks registered
   in the executable will not be called if GLUT calls its (different)
   exit routine).  GLUT is typically built with the
   "/MD" option (the CRT with multithreading DLL support), but the Visual
   C++ linker default is "/ML" (the single threaded CRT).

   One workaround to this issue is requiring users to always link with
   the same CRT as GLUT is compiled with.  That requires users supply a
   non-standard option.  GLUT 3.7 has its own built-in workaround where
   the executable's "exit" function pointer is covertly passed to GLUT.
   GLUT then calls the executable's exit function pointer to ensure that
   any "atexit" calls registered by the application are called if GLUT
   needs to exit.

   Note that the __glut*WithExit routines should NEVER be called directly.
   To avoid the atexit workaround, #define GLUT_DISABLE_ATEXIT_HACK. */

/* XXX This is from Win32's <process.h> */
# if !defined(_MSC_VER) && !defined(__cdecl)
   /* Define __cdecl for non-Microsoft compilers. */
#  define __cdecl
#  define GLUT_DEFINED___CDECL
# endif
# ifndef _CRTIMP
#  ifdef _NTSDK
    /* Definition compatible with NT SDK */
#   define _CRTIMP
#  else
    /* Current definition */
#   ifdef _DLL
#    define _CRTIMP __declspec(dllimport)
#   else
#    define _CRTIMP
#   endif
#  endif
#  define GLUT_DEFINED__CRTIMP
# endif
# ifndef GLUT_BUILDING_LIB
extern _CRTIMP void __cdecl exit(int);
# endif

/* GLUT callback calling convention for Win32. */
# define GLUTCALLBACK __cdecl

/* for callback/function pointer defs */
# define GLUTAPIENTRYV __cdecl

/* glut-win32 specific macros, defined to prevent collision with
   and redifinition of Windows system defs, also removes requirement of
   pretty much any standard windows header from this file */

#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#       define GLUTAPIENTRY __stdcall
#else
#       define GLUTAPIENTRY
#endif

/* GLUT API entry point declarations for Win32. */
#if defined(GLUT_BUILDING_LIB) && defined(_DLL)
#       define GLUTAPI __declspec(dllexport)
#elif defined(_DLL)
#   define GLUTAPI __declspec(dllimport)
#else
#       define GLUTAPI extern
#endif

#if defined(_WIN32) && !defined(_WINDEF_) && !defined(MESA)
#       if !defined(MESA_MINWARN)
#               pragma message( "note: WINDOWS.H not included, providing Mesa definition of CALLBACK macro" )
#               pragma message( "----: and PROC typedef. If you receive compiler warnings about either ")
#               pragma message( "----: being multiply defined you should include WINDOWS.H priot to gl/glut.h" )
#       endif
#       define CALLBACK __stdcall
typedef int (GLUTAPIENTRY *PROC)();
typedef void *HGLRC;
typedef void *HDC;
typedef unsigned long COLORREF;
#endif

#if defined(_WIN32) && !defined(_WINGDI_) && !defined(MESA)
#       if !defined(MESA_MINWARN)
#               pragma message( "note: WINDOWS.H not included, providing Mesa definition of wgl functions" )
#               pragma message( "----: and macros. If you receive compiler warnings about any being multiply ")
#               pragma message( "----: defined you should include WINDOWS.H priot to gl/glut.h" )
#       endif
#       define WGL_FONT_LINES      0
#       define WGL_FONT_POLYGONS   1
#       ifdef UNICODE
#               define wglUseFontBitmaps  wglUseFontBitmapsW
#               define wglUseFontOutlines  wglUseFontOutlinesW
#       else
#               define wglUseFontBitmaps  wglUseFontBitmapsA
#               define wglUseFontOutlines  wglUseFontOutlinesA
#       endif /* !UNICODE */
typedef struct tagLAYERPLANEDESCRIPTOR LAYERPLANEDESCRIPTOR, *PLAYERPLANEDESCRIPTOR, *LPLAYERPLANEDESCRIPTOR;
typedef struct _GLYPHMETRICSFLOAT GLYPHMETRICSFLOAT, *PGLYPHMETRICSFLOAT, *LPGLYPHMETRICSFLOAT;
#  pragma warning( push )
#  pragma warning( disable : 4273 ) /* 'function' : inconsistent DLL linkage. dllexport assumed. */
#  define WGLAPI __declspec(dllimport)
WGLAPI int   GLAPIENTRY wglDeleteContext(HGLRC);
WGLAPI int   GLAPIENTRY wglMakeCurrent(HDC,HGLRC);
WGLAPI int   GLAPIENTRY wglSetPixelFormat(HDC, int, const PIXELFORMATDESCRIPTOR *);
WGLAPI int   GLAPIENTRY wglSwapBuffers(HDC hdc);
WGLAPI HDC   GLAPIENTRY wglGetCurrentDC(void);
WGLAPI HGLRC GLAPIENTRY wglCreateContext(HDC);
WGLAPI HGLRC GLAPIENTRY wglCreateLayerContext(HDC,int);
WGLAPI HGLRC GLAPIENTRY wglGetCurrentContext(void);
WGLAPI PROC  GLAPIENTRY wglGetProcAddress(const char*);
WGLAPI int   GLAPIENTRY wglChoosePixelFormat(HDC, const PIXELFORMATDESCRIPTOR *);
WGLAPI int   GLAPIENTRY wglCopyContext(HGLRC, HGLRC, unsigned int);
WGLAPI int   GLAPIENTRY wglDeleteContext(HGLRC);
WGLAPI int   GLAPIENTRY wglDescribeLayerPlane(HDC, int, int, unsigned int,LPLAYERPLANEDESCRIPTOR);
WGLAPI int   GLAPIENTRY wglDescribePixelFormat(HDC,int, unsigned int, LPPIXELFORMATDESCRIPTOR);
WGLAPI int   GLAPIENTRY wglGetLayerPaletteEntries(HDC, int, int, int,COLORREF *);
WGLAPI int   GLAPIENTRY wglGetPixelFormat(HDC hdc);
WGLAPI int   GLAPIENTRY wglMakeCurrent(HDC, HGLRC);
WGLAPI int   GLAPIENTRY wglRealizeLayerPalette(HDC, int, int);
WGLAPI int   GLAPIENTRY wglSetLayerPaletteEntries(HDC, int, int, int,const COLORREF *);
WGLAPI int   GLAPIENTRY wglShareLists(HGLRC, HGLRC);
WGLAPI int   GLAPIENTRY wglSwapLayerBuffers(HDC, unsigned int);
WGLAPI int   GLAPIENTRY wglUseFontBitmapsA(HDC, unsigned long, unsigned long, unsigned long);
WGLAPI int   GLAPIENTRY wglUseFontBitmapsW(HDC, unsigned long, unsigned long, unsigned long);
WGLAPI int   GLAPIENTRY wglUseFontOutlinesA(HDC, unsigned long, unsigned long, unsigned long, float,float, int, LPGLYPHMETRICSFLOAT);
WGLAPI int   GLAPIENTRY wglUseFontOutlinesW(HDC, unsigned long, unsigned long, unsigned long, float,float, int, LPGLYPHMETRICSFLOAT);
WGLAPI int   GLAPIENTRY SwapBuffers(HDC);
WGLAPI int   GLAPIENTRY ChoosePixelFormat(HDC,const PIXELFORMATDESCRIPTOR *);
WGLAPI int   GLAPIENTRY DescribePixelFormat(HDC,int,unsigned int,LPPIXELFORMATDESCRIPTOR);
WGLAPI int   GLAPIENTRY GetPixelFormat(HDC);
WGLAPI int   GLAPIENTRY SetPixelFormat(HDC,int,const PIXELFORMATDESCRIPTOR *);
#  undef WGLAPI
#  pragma warning( pop )
#endif

#else /* _WIN32 not defined */
#if defined(__WIN32OS2__)
/* GLUT callback calling convention for Odin. */
#define GLUTCALLBACK __cdecl

/* for callback/function pointer defs */
#define GLUTAPIENTRYV __cdecl

#define GLUTAPIENTRY __stdcall

#define GLUTAPI extern

#else
/* Define GLUTAPIENTRY and GLUTCALLBACK to nothing if we aren't on Win32. */
#  define GLUTAPIENTRY
#  define GLUTAPIENTRYV
#  define GLUT_APIENTRY_DEFINED
#  define GLUTCALLBACK
#  define GLUTAPI extern
/* Prototype exit for the non-Win32 case (see above). */
extern void exit(int);
#endif

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

 GLUT_XLIB_IMPLEMENTATION=13 mjk's GLUT 3.7 beta with GameGLUT support.

 GLUT_XLIB_IMPLEMENTATION=14 mjk's GLUT 3.7 beta with f90gl friend interface.

 GLUT_XLIB_IMPLEMENTATION=15 mjk's GLUT 3.7 beta sync'ed with Mesa <GL/glut.h>
**/
#ifndef GLUT_XLIB_IMPLEMENTATION  /* Allow this to be overriden. */
#define GLUT_XLIB_IMPLEMENTATION        15
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
GLUTAPI void *glutStrokeRoman;
GLUTAPI void *glutStrokeMonoRoman;

/* Stroke font constants (use these in GLUT program). */
#define GLUT_STROKE_ROMAN               (&glutStrokeRoman)
#define GLUT_STROKE_MONO_ROMAN          (&glutStrokeMonoRoman)

/* Bitmap font opaque addresses (use constants instead in source code). */
GLUTAPI void *glutBitmap9By15;
GLUTAPI void *glutBitmap8By13;
GLUTAPI void *glutBitmapTimesRoman10;
GLUTAPI void *glutBitmapTimesRoman24;
GLUTAPI void *glutBitmapHelvetica10;
GLUTAPI void *glutBitmapHelvetica12;
GLUTAPI void *glutBitmapHelvetica18;

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
GLUTAPI void GLUTAPIENTRY glutInit(int *argcp, char **argv);
#if defined(_WIN32) && !defined(GLUT_DISABLE_ATEXIT_HACK)
GLUTAPI void GLUTAPIENTRY __glutInitWithExit(int *argcp, char **argv, void (__cdecl *exitfunc)(int));
#ifndef GLUT_BUILDING_LIB
static void GLUTAPIENTRY glutInit_ATEXIT_HACK(int *argcp, char **argv) { __glutInitWithExit(argcp, argv, exit); }
#define glutInit glutInit_ATEXIT_HACK
#endif
#endif
GLUTAPI void GLUTAPIENTRY glutInitDisplayMode(unsigned int mode);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
GLUTAPI void GLUTAPIENTRY glutInitDisplayString(const char *string);
#endif
GLUTAPI void GLUTAPIENTRY glutInitWindowPosition(int x, int y);
GLUTAPI void GLUTAPIENTRY glutInitWindowSize(int width, int height);
GLUTAPI void GLUTAPIENTRY glutMainLoop(void);

/* GLUT window sub-API. */
GLUTAPI int GLUTAPIENTRY glutCreateWindow(const char *title);
#if defined(_WIN32) && !defined(GLUT_DISABLE_ATEXIT_HACK)
GLUTAPI int GLUTAPIENTRY __glutCreateWindowWithExit(const char *title, void (__cdecl *exitfunc)(int));
#ifndef GLUT_BUILDING_LIB
static int GLUTAPIENTRY glutCreateWindow_ATEXIT_HACK(const char *title) { return __glutCreateWindowWithExit(title, exit); }
#define glutCreateWindow glutCreateWindow_ATEXIT_HACK
#endif
#endif
GLUTAPI int GLUTAPIENTRY glutCreateSubWindow(int win, int x, int y, int width, int height);
GLUTAPI void GLUTAPIENTRY glutDestroyWindow(int win);
GLUTAPI void GLUTAPIENTRY glutPostRedisplay(void);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 11)
GLUTAPI void GLUTAPIENTRY glutPostWindowRedisplay(int win);
#endif
GLUTAPI void GLUTAPIENTRY glutSwapBuffers(void);
GLUTAPI int GLUTAPIENTRY glutGetWindow(void);
GLUTAPI void GLUTAPIENTRY glutSetWindow(int win);
GLUTAPI void GLUTAPIENTRY glutSetWindowTitle(const char *title);
GLUTAPI void GLUTAPIENTRY glutSetIconTitle(const char *title);
GLUTAPI void GLUTAPIENTRY glutPositionWindow(int x, int y);
GLUTAPI void GLUTAPIENTRY glutReshapeWindow(int width, int height);
GLUTAPI void GLUTAPIENTRY glutPopWindow(void);
GLUTAPI void GLUTAPIENTRY glutPushWindow(void);
GLUTAPI void GLUTAPIENTRY glutIconifyWindow(void);
GLUTAPI void GLUTAPIENTRY glutShowWindow(void);
GLUTAPI void GLUTAPIENTRY glutHideWindow(void);
#if (GLUT_API_VERSION >= 3)
GLUTAPI void GLUTAPIENTRY glutFullScreen(void);
GLUTAPI void GLUTAPIENTRY glutSetCursor(int cursor);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
GLUTAPI void GLUTAPIENTRY glutWarpPointer(int x, int y);
#endif

/* GLUT overlay sub-API. */
GLUTAPI void GLUTAPIENTRY glutEstablishOverlay(void);
GLUTAPI void GLUTAPIENTRY glutRemoveOverlay(void);
GLUTAPI void GLUTAPIENTRY glutUseLayer(GLenum layer);
GLUTAPI void GLUTAPIENTRY glutPostOverlayRedisplay(void);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 11)
GLUTAPI void GLUTAPIENTRY glutPostWindowOverlayRedisplay(int win);
#endif
GLUTAPI void GLUTAPIENTRY glutShowOverlay(void);
GLUTAPI void GLUTAPIENTRY glutHideOverlay(void);
#endif

/* GLUT menu sub-API. */
GLUTAPI int GLUTAPIENTRY glutCreateMenu(void (* GLUTCALLBACK func)(int));
#if defined(_WIN32) && !defined(GLUT_DISABLE_ATEXIT_HACK)
GLUTAPI int GLUTAPIENTRY __glutCreateMenuWithExit(void (* GLUTCALLBACK func)(int), void (__cdecl *exitfunc)(int));
#ifndef GLUT_BUILDING_LIB
static int GLUTAPIENTRY glutCreateMenu_ATEXIT_HACK(void (* GLUTCALLBACK func)(int)) { return __glutCreateMenuWithExit(func, exit); }
#define glutCreateMenu glutCreateMenu_ATEXIT_HACK
#endif
#endif
GLUTAPI void GLUTAPIENTRY glutDestroyMenu(int menu);
GLUTAPI int GLUTAPIENTRY glutGetMenu(void);
GLUTAPI void GLUTAPIENTRY glutSetMenu(int menu);
GLUTAPI void GLUTAPIENTRY glutAddMenuEntry(const char *label, int value);
GLUTAPI void GLUTAPIENTRY glutAddSubMenu(const char *label, int submenu);
GLUTAPI void GLUTAPIENTRY glutChangeToMenuEntry(int item, const char *label, int value);
GLUTAPI void GLUTAPIENTRY glutChangeToSubMenu(int item, const char *label, int submenu);
GLUTAPI void GLUTAPIENTRY glutRemoveMenuItem(int item);
GLUTAPI void GLUTAPIENTRY glutAttachMenu(int button);
GLUTAPI void GLUTAPIENTRY glutDetachMenu(int button);

/* GLUT window callback sub-API. */
GLUTAPI void GLUTAPIENTRY glutDisplayFunc(void (* GLUTCALLBACK func)(void));
GLUTAPI void GLUTAPIENTRY glutReshapeFunc(void (* GLUTCALLBACK func)(int width, int height));
GLUTAPI void GLUTAPIENTRY glutKeyboardFunc(void (* GLUTCALLBACK func)(unsigned char key, int x, int y));
GLUTAPI void GLUTAPIENTRY glutMouseFunc(void (* GLUTCALLBACK func)(int button, int state, int x, int y));
GLUTAPI void GLUTAPIENTRY glutMotionFunc(void (* GLUTCALLBACK func)(int x, int y));
GLUTAPI void GLUTAPIENTRY glutPassiveMotionFunc(void (* GLUTCALLBACK func)(int x, int y));
GLUTAPI void GLUTAPIENTRY glutEntryFunc(void (* GLUTCALLBACK func)(int state));
GLUTAPI void GLUTAPIENTRY glutVisibilityFunc(void (* GLUTCALLBACK func)(int state));
GLUTAPI void GLUTAPIENTRY glutIdleFunc(void (* GLUTCALLBACK func)(void));
GLUTAPI void GLUTAPIENTRY glutTimerFunc(unsigned int millis, void (* GLUTCALLBACK func)(int value), int value);
GLUTAPI void GLUTAPIENTRY glutMenuStateFunc(void (* GLUTCALLBACK func)(int state));
#if (GLUT_API_VERSION >= 2)
GLUTAPI void GLUTAPIENTRY glutSpecialFunc(void (* GLUTCALLBACK func)(int key, int x, int y));
GLUTAPI void GLUTAPIENTRY glutSpaceballMotionFunc(void (* GLUTCALLBACK func)(int x, int y, int z));
GLUTAPI void GLUTAPIENTRY glutSpaceballRotateFunc(void (* GLUTCALLBACK func)(int x, int y, int z));
GLUTAPI void GLUTAPIENTRY glutSpaceballButtonFunc(void (* GLUTCALLBACK func)(int button, int state));
GLUTAPI void GLUTAPIENTRY glutButtonBoxFunc(void (* GLUTCALLBACK func)(int button, int state));
GLUTAPI void GLUTAPIENTRY glutDialsFunc(void (* GLUTCALLBACK func)(int dial, int value));
GLUTAPI void GLUTAPIENTRY glutTabletMotionFunc(void (* GLUTCALLBACK func)(int x, int y));
GLUTAPI void GLUTAPIENTRY glutTabletButtonFunc(void (* GLUTCALLBACK func)(int button, int state, int x, int y));
#if (GLUT_API_VERSION >= 3)
GLUTAPI void GLUTAPIENTRY glutMenuStatusFunc(void (* GLUTCALLBACK func)(int status, int x, int y));
GLUTAPI void GLUTAPIENTRY glutOverlayDisplayFunc(void (* GLUTCALLBACK func)(void));
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
GLUTAPI void GLUTAPIENTRY glutWindowStatusFunc(void (* GLUTCALLBACK func)(int state));
#endif
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
GLUTAPI void GLUTAPIENTRY glutKeyboardUpFunc(void (* GLUTCALLBACK func)(unsigned char key, int x, int y));
GLUTAPI void GLUTAPIENTRY glutSpecialUpFunc(void (* GLUTCALLBACK func)(int key, int x, int y));
GLUTAPI void GLUTAPIENTRY glutJoystickFunc(void (* GLUTCALLBACK func)(unsigned int buttonMask, int x, int y, int z), int pollInterval);
#endif
#endif
#endif

/* GLUT color index sub-API. */
GLUTAPI void GLUTAPIENTRY glutSetColor(int, GLfloat red, GLfloat green, GLfloat blue);
GLUTAPI GLfloat GLUTAPIENTRY glutGetColor(int ndx, int component);
GLUTAPI void GLUTAPIENTRY glutCopyColormap(int win);

/* GLUT state retrieval sub-API. */
GLUTAPI int GLUTAPIENTRY glutGet(GLenum type);
GLUTAPI int GLUTAPIENTRY glutDeviceGet(GLenum type);
#if (GLUT_API_VERSION >= 2)
/* GLUT extension support sub-API */
GLUTAPI int GLUTAPIENTRY glutExtensionSupported(const char *name);
#endif
#if (GLUT_API_VERSION >= 3)
GLUTAPI int GLUTAPIENTRY glutGetModifiers(void);
GLUTAPI int GLUTAPIENTRY glutLayerGet(GLenum type);
#endif

/* GLUT font sub-API */
GLUTAPI void GLUTAPIENTRY glutBitmapCharacter(void *font, int character);
GLUTAPI int GLUTAPIENTRY glutBitmapWidth(void *font, int character);
GLUTAPI void GLUTAPIENTRY glutStrokeCharacter(void *font, int character);
GLUTAPI int GLUTAPIENTRY glutStrokeWidth(void *font, int character);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
GLUTAPI int GLUTAPIENTRY glutBitmapLength(void *font, const unsigned char *string);
GLUTAPI int GLUTAPIENTRY glutStrokeLength(void *font, const unsigned char *string);
#endif

/* GLUT pre-built models sub-API */
GLUTAPI void GLUTAPIENTRY glutWireSphere(GLdouble radius, GLint slices, GLint stacks);
GLUTAPI void GLUTAPIENTRY glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
GLUTAPI void GLUTAPIENTRY glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
GLUTAPI void GLUTAPIENTRY glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
GLUTAPI void GLUTAPIENTRY glutWireCube(GLdouble size);
GLUTAPI void GLUTAPIENTRY glutSolidCube(GLdouble size);
GLUTAPI void GLUTAPIENTRY glutWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
GLUTAPI void GLUTAPIENTRY glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
GLUTAPI void GLUTAPIENTRY glutWireDodecahedron(void);
GLUTAPI void GLUTAPIENTRY glutSolidDodecahedron(void);
GLUTAPI void GLUTAPIENTRY glutWireTeapot(GLdouble size);
GLUTAPI void GLUTAPIENTRY glutSolidTeapot(GLdouble size);
GLUTAPI void GLUTAPIENTRY glutWireOctahedron(void);
GLUTAPI void GLUTAPIENTRY glutSolidOctahedron(void);
GLUTAPI void GLUTAPIENTRY glutWireTetrahedron(void);
GLUTAPI void GLUTAPIENTRY glutSolidTetrahedron(void);
GLUTAPI void GLUTAPIENTRY glutWireIcosahedron(void);
GLUTAPI void GLUTAPIENTRY glutSolidIcosahedron(void);

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
/* GLUT video resize sub-API. */
GLUTAPI int GLUTAPIENTRY glutVideoResizeGet(GLenum param);
GLUTAPI void GLUTAPIENTRY glutSetupVideoResizing(void);
GLUTAPI void GLUTAPIENTRY glutStopVideoResizing(void);
GLUTAPI void GLUTAPIENTRY glutVideoResize(int x, int y, int width, int height);
GLUTAPI void GLUTAPIENTRY glutVideoPan(int x, int y, int width, int height);

/* GLUT debugging sub-API. */
GLUTAPI void GLUTAPIENTRY glutReportErrors(void);
#endif

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
/* GLUT device control sub-API. */
/* glutSetKeyRepeat modes. */
#define GLUT_KEY_REPEAT_OFF		0
#define GLUT_KEY_REPEAT_ON		1
#define GLUT_KEY_REPEAT_DEFAULT		2

/* Joystick button masks. */
#define GLUT_JOYSTICK_BUTTON_A		1
#define GLUT_JOYSTICK_BUTTON_B		2
#define GLUT_JOYSTICK_BUTTON_C		4
#define GLUT_JOYSTICK_BUTTON_D		8

GLUTAPI void GLUTAPIENTRY glutIgnoreKeyRepeat(int ignore);
GLUTAPI void GLUTAPIENTRY glutSetKeyRepeat(int repeatMode);
GLUTAPI void GLUTAPIENTRY glutForceJoystickFunc(void);

/* GLUT game mode sub-API. */
/* glutGameModeGet. */
#define GLUT_GAME_MODE_ACTIVE           0
#define GLUT_GAME_MODE_POSSIBLE         1
#define GLUT_GAME_MODE_WIDTH            2
#define GLUT_GAME_MODE_HEIGHT           3
#define GLUT_GAME_MODE_PIXEL_DEPTH      4
#define GLUT_GAME_MODE_REFRESH_RATE     5
#define GLUT_GAME_MODE_DISPLAY_CHANGED  6

GLUTAPI void GLUTAPIENTRY glutGameModeString(const char *string);
GLUTAPI int GLUTAPIENTRY glutEnterGameMode(void);
GLUTAPI void GLUTAPIENTRY glutLeaveGameMode(void);
GLUTAPI int GLUTAPIENTRY glutGameModeGet(GLenum mode);
#endif

#ifdef __cplusplus
}
#endif

#if 0
#ifdef GLUT_APIENTRY_DEFINED
# undef GLUT_APIENTRY_DEFINED
# undef APIENTRY
#endif

#ifdef GLUT_WINGDIAPI_DEFINED
# undef GLUT_WINGDIAPI_DEFINED
# undef WINGDIAPI
#endif

#ifdef GLUT_DEFINED___CDECL
# undef GLUT_DEFINED___CDECL
# undef __cdecl
#endif

#ifdef GLUT_DEFINED__CRTIMP
# undef GLUT_DEFINED__CRTIMP
# undef _CRTIMP
#endif
#endif

#endif                  /* __glut_h__ */
