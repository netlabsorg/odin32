/* $Id: gluos.h,v 1.1 2000-02-09 08:50:55 jeroen Exp $ */
/*
** gluos.h - operating system dependencies for GLU
**
** $Header: /home/ktk/tmp/odin/odin32xp/src/opengl/glu/include/gluos.h,v 1.1 2000-02-09 08:50:55 jeroen Exp $*/

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOIME
#include <windows.h>

/* Disable warnings */
#pragma warning(disable : 4101)
#pragma warning(disable : 4244)
#pragma warning(disable : 4761)

#else
#if defined(__WIN32OS2__)
#include <os2win.h>
#define GLAPI WIN32API
#else
/* Disable Microsoft-specific keywords */
#define GLAPI
#define WINGDIAPI

#endif
#endif
