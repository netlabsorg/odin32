/* $Id: glut_term.c,v 1.5 2000-05-20 13:48:23 jeroen Exp $ */
/*
 * GLUT Termination - on exit cleanup any open windows
 *
*/

#include "glut.h"
#include "glu.h"
#include "glutint.h"

void CDECL Glut32Terminate(void)
{
 int i;

      /* Our app has ended - close all the open windows! */
      for (i = 0; i < __glutWindowListSize; i++) {
        if (__glutWindowList[i])
          {
            glutDestroyWindow(i+1);
          }
      }
}
