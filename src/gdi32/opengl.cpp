/* $Id: opengl.cpp,v 1.12 2004-04-14 09:44:36 sandervl Exp $ */

/*
 * GDI32 OpenGl stubs
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include "misc.h"
#include "unicode.h"

#define DBG_LOCALLOG	DBG_opengl
#include "dbglocal.h"


ODINDEBUGCHANNEL(GDI32-OPENGL)


/****************************************************************************
 * Definitions                                                              *
 ****************************************************************************/

typedef int  (* WIN32API CHOOSEPIXELFMT) (HDC, CONST PIXELFORMATDESCRIPTOR *);
typedef BOOL (* WIN32API SETPIXELFMT) (HDC, int, CONST PIXELFORMATDESCRIPTOR *);
typedef BOOL (* WIN32API SWAPBUFFERS) (HDC hdc);
typedef int  (* WIN32API DESCRIBEPIXELFMT) (HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);
typedef int  (* WIN32API GETPIXELFMT) (HDC);


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

static CHOOSEPIXELFMT    glChoosePixelFormat   = NULL;
static SETPIXELFMT       glSetPixelFormat      = NULL;
static SWAPBUFFERS       glSwapBuffers         = NULL;
static DESCRIBEPIXELFMT  glDescribePixelFormat = NULL;
static GETPIXELFMT       glGetPixelFormat      = NULL;
static HINSTANCE         hOpenGL               = NULL;


/*****************************************************************************
 * Name      : internalOpenGLEnable
 * Purpose   : check if loaded, load OpenGL.DLL on demand
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

static BOOL internalOpenGLEnable(void)
{
  if(hOpenGL == NULL)
  {
     hOpenGL = LoadLibraryA("OPENGL32.DLL");
     if(hOpenGL == NULL)
       return(FALSE);
  }

  if(glChoosePixelFormat == NULL) {
        glChoosePixelFormat = (CHOOSEPIXELFMT)GetProcAddress(hOpenGL, "wglChoosePixelFormat");
        if(glChoosePixelFormat == NULL)
                return(0);
  }

  if(glSetPixelFormat == NULL) {
        glSetPixelFormat = (SETPIXELFMT)GetProcAddress(hOpenGL, "wglSetPixelFormat");
        if(glSetPixelFormat == NULL)
                return(FALSE);
  }

  if(glSwapBuffers == NULL) {
        glSwapBuffers = (SWAPBUFFERS)GetProcAddress(hOpenGL, "wglSwapBuffers");
        if(glSwapBuffers == NULL)
                return(FALSE);
  }

  if(glDescribePixelFormat == NULL) {
        glDescribePixelFormat = (DESCRIBEPIXELFMT)GetProcAddress(hOpenGL, "wglDescribePixelFormat");
        if(glDescribePixelFormat == NULL)
                return(FALSE);
  }

  if(glGetPixelFormat == NULL) {
        glGetPixelFormat = (GETPIXELFMT)GetProcAddress(hOpenGL, "wglGetPixelFormat");
        if(glGetPixelFormat == NULL)
                return(FALSE);
  }

  return(TRUE);                        /* OpenGL is initialized and enabled*/
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

int WIN32API ChoosePixelFormat(HDC hdc, const LPPIXELFORMATDESCRIPTOR pformat)
{
  if (glChoosePixelFormat == NULL)
    if (internalOpenGLEnable() == FALSE)
      return(0);

  return(glChoosePixelFormat(hdc, pformat));
}

int WIN32API DescribePixelFormat(HDC hdc, int iFormat, UINT nBytes,
                                      LPPIXELFORMATDESCRIPTOR pformat)
{
  if (glDescribePixelFormat == NULL)
    if (internalOpenGLEnable() == FALSE)
      return(0);

  return(glDescribePixelFormat(hdc, iFormat, nBytes, pformat));
}

int WIN32API GetPixelFormat(HDC hdc)
{
  if (glGetPixelFormat == NULL)
    if (internalOpenGLEnable() == FALSE)
      return(0);

  return(glGetPixelFormat(hdc));
}

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

BOOL WIN32API SetPixelFormat(HDC hdc, int whatever,
                             CONST PIXELFORMATDESCRIPTOR *pformat)
{
  if (glSetPixelFormat == NULL)
    if (internalOpenGLEnable() == FALSE)
      return(0);

  return(glSetPixelFormat(hdc, whatever, pformat));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

BOOL WIN32API SwapBuffers(HDC hdc)
{
  if (glSwapBuffers == NULL)
    if (internalOpenGLEnable() == FALSE)
      return(0);


  return(glSwapBuffers(hdc));
}

