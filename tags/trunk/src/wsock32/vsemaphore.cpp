/* $Id: vsemaphore.cpp,v 1.2 2002-07-30 15:34:52 achimha Exp $ */

/*
 * Semaphore class
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * NOTE: When creating a shared semaphore object, you must call the ctor, wait &
 *       post with a 2nd parameter (HMTX *). This semaphore handle must be located
 *       in the local data segment of the dll.
 *       This enables other processes than the one that created the semaphore object
 *       to access it 
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include "vsemaphore.h"
#include <win32type.h>
#include <misc.h>

/******************************************************************************/
/******************************************************************************/
VSemaphore::VSemaphore(int fShared, HEV *phSem)
{
 APIRET rc;

  this->fShared = fShared;
  rc = DosCreateEventSem(NULL, &sem_handle, (fShared == VSEMAPHORE_SHARED) ? DC_SEM_SHARED : 0, FALSE);
  if(rc != 0) {
    dprintf(("Error creating mutex %X\n", rc));
    sem_handle = 0;
  }
  if(fShared) {
	*phSem = sem_handle;
  }
}
/******************************************************************************/
/******************************************************************************/
VSemaphore::~VSemaphore()
{
  if(sem_handle) {
    	DosCloseEventSem(sem_handle);
  }
}
/******************************************************************************/
/******************************************************************************/
void VSemaphore::wait(ULONG timeout, HEV *phSem)
{
 APIRET rc;

  if(fShared == VSEMAPHORE_SHARED && phSem == NULL) {
	DebugInt3();
	return;
  }
  if(sem_handle) {
	if(fShared == VSEMAPHORE_SHARED && *phSem == 0) {
		//must open the shared semaphore first (other process created it)
		*phSem = sem_handle;
		rc = DosOpenEventSem(NULL, phSem);
		if(rc) {
			DebugInt3();
		}
	}
    	rc = DosWaitEventSem((fShared == VSEMAPHORE_SHARED) ? *phSem : sem_handle, timeout);
  	if(rc) {
		DebugInt3();
  	}
  }
}
/******************************************************************************/
/******************************************************************************/
void VSemaphore::reset(HMTX *phSem)
{
 APIRET rc;
 ULONG  count;

  if(fShared == VSEMAPHORE_SHARED && phSem == NULL) {
	DebugInt3();
	return;
  }
  if(fShared == VSEMAPHORE_SHARED && *phSem == 0) {
	//must open the shared semaphore first (other process created it)
	*phSem = sem_handle;
	rc = DosOpenEventSem(NULL, phSem);
	if(rc) {
		DebugInt3();
	}
  }
  rc = DosResetEventSem((fShared == VSEMAPHORE_SHARED) ? *phSem : sem_handle, &count);
  if(rc != NO_ERROR && rc != ERROR_ALREADY_RESET) {
	DebugInt3();
  }
}
/******************************************************************************/
/******************************************************************************/
void VSemaphore::post(HMTX *phSem)
{
 APIRET rc;

  if(fShared == VSEMAPHORE_SHARED && phSem == NULL) {
	DebugInt3();
	return;
  }
  if(fShared == VSEMAPHORE_SHARED && *phSem == 0) {
	//must open the shared semaphore first (other process created it)
	*phSem = sem_handle;
	rc = DosOpenEventSem(NULL, phSem);
	if(rc) {
		DebugInt3();
	}
  }
  rc = DosPostEventSem((fShared == VSEMAPHORE_SHARED) ? *phSem : sem_handle);
  if(rc != NO_ERROR && rc != ERROR_ALREADY_POSTED) {
	DebugInt3();
  }
}
/******************************************************************************/
/******************************************************************************/

