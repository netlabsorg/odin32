/* $Id: opengl.cpp,v 1.3 1999-06-10 17:09:04 phaller Exp $ */

/*
 * GDI32 OpenGl stubs
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <string.h>
#include "misc.h"
#include "unicode.h"

typedef int  (* WIN32API CHOOSEPIXELFMT) (HDC, CONST PIXELFORMATDESCRIPTOR *);
typedef BOOL (* WIN32API SETPIXELFMT) (HDC, int, CONST PIXELFORMATDESCRIPTOR *);
typedef BOOL (* WIN32API SWAPBUFFERS) (HDC hdc);

CHOOSEPIXELFMT glChoosePixelFormat = NULL;
SETPIXELFMT    glSetPixelFormat    = NULL;
SWAPBUFFERS    glSwapBuffers       = NULL;

HINSTANCE hOpenGL = NULL;

//******************************************************************************
//******************************************************************************
int WIN32API ChoosePixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *pformat)
{
 int rc;

  return(1);
  dprintf(("OS2ChoosePixelFormat\n"));
  if(hOpenGL == NULL) {
        hOpenGL = O32_LoadLibrary("OPENGL32.DLL");
        if(hOpenGL == NULL)
                return(0);
  }
  if(glChoosePixelFormat == NULL) {
        glChoosePixelFormat = (CHOOSEPIXELFMT)O32_GetProcAddress(hOpenGL, "OS2wglChoosePixelFormat");
        if(glChoosePixelFormat == NULL)
                return(0);
  }
  rc = glChoosePixelFormat(hdc, pformat);
  dprintf(("wglChoosePixelFormat returned %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPixelFormat(HDC hdc, int whatever, CONST PIXELFORMATDESCRIPTOR * pformat)
{
 BOOL rc;

  dprintf(("OS2SetPixelFormat\n"));
  if(hOpenGL == NULL) {
        hOpenGL = O32_LoadLibrary("OPENGL32.DLL");
        if(hOpenGL == NULL)
                return(FALSE);
  }
  if(glSetPixelFormat == NULL) {
        glSetPixelFormat = (SETPIXELFMT)O32_GetProcAddress(hOpenGL, "OS2wglSetPixelFormat");
        if(glSetPixelFormat == NULL)
                return(FALSE);
  }
  rc = glSetPixelFormat(hdc, whatever, pformat);
  dprintf(("wglSetPixelFormat returned %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SwapBuffers(HDC hdc)
{
 BOOL rc;

  dprintf(("OS2SwapBuffers\n"));
  if(hOpenGL == NULL) {
        hOpenGL = O32_LoadLibrary("OPENGL32.DLL");
        if(hOpenGL == NULL)
                return(FALSE);
  }
  if(glSwapBuffers == NULL) {
        glSwapBuffers = (SWAPBUFFERS)O32_GetProcAddress(hOpenGL, "OS2wglSwapBuffers");
        if(glSwapBuffers == NULL)
                return(FALSE);
  }
  rc = glSwapBuffers(hdc);
  dprintf(("wglSwapBuffers returned %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
