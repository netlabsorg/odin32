
/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "glutint.h"

/* strdup is actually not a standard ANSI C or POSIX routine
   so implement a private one for GLUT.  OpenVMS does not have a
   strdup; Linux's standard libc doesn't declare strdup by default
   (unless BSD or SVID interfaces are requested). */
char *
__glutStrdup(const char *string)
{
  char *copy;

  copy = (char*) malloc(strlen(string) + 1);
  if (copy == NULL)
    return NULL;
  strcpy(copy, string);
  return copy;
}

void
__glutWarning(char *format,...)
{
  va_list args;

  va_start(args, format);

#if defined(__WIN32OS2__)
  dprintf(("GLUT32: Warning %s: \n",

    __glutProgramName ? __glutProgramName : "(unamed)"));
  CHAR errmsg[1024];
  sprintf(errmsg, format, args);
  dprintf(("GLUT32: %s\n",errmsg));
#endif

  fprintf(stderr, "GLUT: Warning in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
}

/* CENTRY */
void APIENTRY
glutReportErrors(void)
{
  GLenum error;

  while ((error = glGetError()) != GL_NO_ERROR)
    __glutWarning("GL error: %s", gluErrorString(error));
}
/* ENDCENTRY */

void
__glutFatalError(char *format,...)
{
  va_list args;

  va_start(args, format);
#if defined(__WIN32OS2__)
  CHAR errmsg[1024];

  dprintf(("GLUT32: Fatal Error in %s:\n",
           __glutProgramName ? __glutProgramName : "(unamed)"));

  sprintf(errmsg, format, args);

  dprintf(("GLUT32: %s\n",errmsg));
#endif

  fprintf(stderr, "GLUT: Fatal Error in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);

  exit(4);
}

void
__glutFatalUsage(char *format,...)
{
  va_list args;

  va_start(args, format);

#if defined(__WIN32OS2__)
  dprintf(("GLUT: Fatal API Usage in %s:\n",
    __glutProgramName ? __glutProgramName : "(unamed)"));
  char errmsg[1024];
  sprintf(errmsg, format, args);
  dprintf(("GLUT32: %s",errmsg));
  putc('\n', stderr);
#endif

  fprintf(stderr, "GLUT: Fatal API Usage in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);

  exit(8);
}
