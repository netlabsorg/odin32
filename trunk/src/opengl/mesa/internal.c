/* $Id: internal.c,v 1.1 2000-03-01 18:49:30 jeroen Exp $ */
/*****************************************************************************/
/*                                                                           */
/* OpenGL - Internal functions for GLIDE support                             */
/*                                                                           */
/*****************************************************************************/

#include <os2win.h>
#include "types.h"
#include "wmesadef.h"
#include "internal.h"

extern PWMC Current;

void * _System _InternalOGLQueryBB(void)
{
  /* Return ptr to the current backbuffer */
  if(Current)
    return Current->pbPixels;

  return 0;
}

void * _System _InternalOGLQueryFB(void)
{
  /* Return ptr to the current frontbuffer */
  if(Current)
    if(Current->ppFrameBuffer)
      return Current->ppFrameBuffer;
    else
      return Current->pbPixels;

  return 0;
}
