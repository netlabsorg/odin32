/* $Id: os2thread.c,v 1.1 2000-05-23 20:40:42 jeroen Exp $ */
/*****************************************************************************/
/*                                                                           */
/* Thread/Mutex support for OS/2 port of Mesa                                */
/*                                                                           */
/*****************************************************************************/

#define INCL_DOSSEMAPHORES
#include <os2wrap.h>
#include "os2thread.h"

ULONG _System OS2CreateMutexSem(CHAR *pszName,HMTX *hmtx,ULONG a,ULONG b)
{
  return DosCreateMutexSem(pszName,hmtx,a,b);
}

ULONG _System OS2RequestMutexSem(HMTX hmtx,ULONG t)
{
  return DosRequestMutexSem(hmtx,t);
}

ULONG _System OS2ReleaseMutexSem(HMTX hmtx)
{
  return DosReleaseMutexSem(hmtx);
}
