/* $Id: vmutex.cpp,v 1.8 2000-02-16 14:22:47 sandervl Exp $ */

/*
 * Mutex class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_DOSSEMAPHORES
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <vmutex.h>
#include <win32type.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_vmutex
#include "dbglocal.h"

/******************************************************************************/
/******************************************************************************/
VMutex::VMutex(int fShared) : waiting(0)
{
 APIRET rc;

  rc = DosCreateMutexSem(NULL, &sem_handle, (fShared == VMUTEX_SHARED) ? DC_SEM_SHARED : 0, FALSE);
  if(rc != 0) {
    dprintf(("Error creating mutex %X\n", rc));
    sem_handle = 0;
  }
}
/******************************************************************************/
/******************************************************************************/
VMutex::~VMutex()
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
void VMutex::enter(ULONG timeout)
{
  if(sem_handle) {
    waiting++;
    DosRequestMutexSem(sem_handle, timeout);
    waiting--;
  }
}
/******************************************************************************/
/******************************************************************************/
void VMutex::leave()
{
  DosReleaseMutexSem(sem_handle);
}
/******************************************************************************/
/******************************************************************************/

