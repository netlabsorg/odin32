/* $Id: glut_term.c,v 1.2 2000-03-04 19:10:15 jeroen Exp $ */
/*
 * GLUT Termination - on exit cleanup any open windows
 *
*/

#include <odinwrap.h>
#include "glu.h"
#include "glut.h"
#include "glutint.h"


void CDECL Glut32Terminate(void)
{
      /* Our app has ended - close all the open windows! */
      dprintf(("GLUT internal cleanup - numwins %d\n",__glutWindowListSize));

      for (int i = 0; i < __glutWindowListSize; i++) {
        if (__glutWindowList[i])
          {
            dprintf(("GLUT32 internal cleanup: dtr %d\n",i));
            glutDestroyWindow(i+1);
            dprintf(("GLUT32 internal cleanup: dtr %d com\n",i));
          }
      }
}
