/* $Id: glut_term.c,v 1.1 2000-02-09 08:46:18 jeroen Exp $ */
/*
 * GLUT Termination - on exit cleanup any open windows
 *
*/

#include <odinwrap.h>
#include "glu.h"
#include "glut.h"
#include "glutint.h"


ODINDEBUGCHANNEL (GLUT32-MAIN)

ODINFUNCTION3(BOOL, Glut32LibMain, HINSTANCE, hinstDLL,
                                   DWORD,     fdwReason,
                                   LPVOID,    ImpLoad)
{
  if(fdwReason==DLL_PROCESS_DETACH)
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

  return TRUE;
}
