/* $Id: glut_term.c,v 1.4 2000-03-11 15:07:45 sandervl Exp $ */
/*
 * GLUT Termination - on exit cleanup any open windows
 *
*/

#include "glut.h"
#include "glu.h"
#include "glutint.h"
#ifdef __WIN32OS2__
#include <misc.h>
#endif

void CDECL Glut32Terminate(void)
{
 int i;

      /* Our app has ended - close all the open windows! */
      dprintf(("GLUT internal cleanup - numwins %d\n",__glutWindowListSize));

      for (i = 0; i < __glutWindowListSize; i++) {
        if (__glutWindowList[i])
          {
            dprintf(("GLUT32 internal cleanup: dtr %d\n",i));
            glutDestroyWindow(i+1);
            dprintf(("GLUT32 internal cleanup: dtr %d com\n",i));
          }
      }
}
