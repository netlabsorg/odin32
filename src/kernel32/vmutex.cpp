/* $Id: vmutex.cpp,v 1.4 1999-06-19 10:54:43 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Mutex class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define  INCL_DOSSEMAPHORES
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <vmutex.h>
#include <win32type.h>
#include <misc.h>

/******************************************************************************/
/******************************************************************************/
_Export VMutex::VMutex() : waiting(0)
{
 APIRET rc;

  rc = DosCreateMutexSem(NULL, &sem_handle, 0, FALSE);
  if(rc != 0) {
    dprintf(("Error creating mutex %X\n", rc));
    sem_handle = 0;
  }
}
/******************************************************************************/
/******************************************************************************/
_Export VMutex::~VMutex()
{
 int i;

  if(sem_handle) {
    for(i=0;i<waiting;i++) {
        DosReleaseMutexSem(sem_handle);
    }
    DosCloseMutexSem(sem_handle);
  }
}
/******************************************************************************/
/******************************************************************************/
void _Export VMutex::enter(ULONG timeout)
{
  if(sem_handle) {
    waiting++;
    DosRequestMutexSem(sem_handle, timeout);
    waiting--;
  }
}
/******************************************************************************/
/******************************************************************************/
void _Export VMutex::leave()
{
  DosReleaseMutexSem(sem_handle);
}
/******************************************************************************/
/******************************************************************************/

