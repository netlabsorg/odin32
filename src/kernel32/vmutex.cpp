/* $Id: vmutex.cpp,v 1.9 2000-03-23 19:23:47 sandervl Exp $ */

/*
 * Mutex class
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * NOTE: When creating a shared mutex object, you must call the ctor, enter &
 *       leave with a 2nd parameter (HMTX *). This mutex handle must be located
 *       in the local data segment of the dll.
 *       This enables other processes than the one that created the mutex object
 *       to access it 
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <vmutex.h>
#include <win32type.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_vmutex
#include "dbglocal.h"

/******************************************************************************/
/******************************************************************************/
VMutex::VMutex(int fShared, HMTX *phMutex) : waiting(0)
{
 APIRET rc;

  this->fShared = fShared;
  rc = DosCreateMutexSem(NULL, &sem_handle, (fShared == VMUTEX_SHARED) ? DC_SEM_SHARED : 0, FALSE);
  if(rc != 0) {
    dprintf(("Error creating mutex %X\n", rc));
    sem_handle = 0;
  }
  if(fShared) {
	*phMutex = sem_handle;
  }
}
/******************************************************************************/
/******************************************************************************/
VMutex::~VMutex()
{
 int i;

  if(sem_handle) {
	if(fShared != VMUTEX_SHARED) {
    		for(i=0;i<waiting;i++) {
        		DosReleaseMutexSem(sem_handle);
    		}
	}
    	DosCloseMutexSem(sem_handle);
  }
}
/******************************************************************************/
/******************************************************************************/
void VMutex::enter(ULONG timeout, HMTX *phMutex)
{
 APIRET rc;

  if(fShared == VMUTEX_SHARED && phMutex == NULL) {
	DebugInt3();
	return;
  }
  if(sem_handle) {
	if(fShared == VMUTEX_SHARED && *phMutex == 0) {
		//must open the shared semaphore first (other process created it)
		*phMutex = sem_handle;
		rc = DosOpenMutexSem(NULL, phMutex);
		if(rc) {
			DebugInt3();
		}
	}
    	waiting++;
    	rc = DosRequestMutexSem((fShared == VMUTEX_SHARED) ? *phMutex : sem_handle, timeout);
    	waiting--;
  }
}
/******************************************************************************/
/******************************************************************************/
void VMutex::leave(HMTX *phMutex)
{
  if((fShared == VMUTEX_SHARED && phMutex == NULL) ||
     (fShared == VMUTEX_SHARED && *phMutex == 0)) {
	DebugInt3();
	//should always call enter first...
	return;
  }
  DosReleaseMutexSem((fShared == VMUTEX_SHARED) ? *phMutex : sem_handle);
}
/******************************************************************************/
/******************************************************************************/

