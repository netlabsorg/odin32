/* $Id: glut_term.c,v 1.3 2000-03-11 09:05:05 jeroen Exp $ */
/*
 * GLUT Termination - on exit cleanup any open windows
 *
*/

#include <odinwrap.h>
#include "glut.h"
#include "glu.h"
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
