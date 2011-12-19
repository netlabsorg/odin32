/* $Id: internal.c,v 1.2 2000-05-23 20:40:38 jeroen Exp $ */
/*****************************************************************************/
/*                                                                           */
/* OpenGL - Internal functions for GLIDE support                             */
/*                                                                           */
/*****************************************************************************/

#include <windows.h>
#include "types.h"
#include "wmesadef.h"
#include "internal.h"

extern PWMC Current;

void * _System _InternalOGLQueryBB(void)
{
#ifdef DIVE
  /* Return ptr to the current backbuffer */
  if(Current)
    return Current->pbPixels;
#endif

  return 0;
}

void * _System _InternalOGLQueryFB(void)
{
  /* Return ptr to the current frontbuffer */
#ifdef DIVE
  if(Current)
    if(Current->ppFrameBuffer)
      return Current->ppFrameBuffer;
    else
      return Current->pbPixels;
#endif

  return 0;
}
