/* $Id: vmutex.cpp,v 1.10 2002-04-07 15:44:11 sandervl Exp $ */

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
#include <win32type.h>
#include <vmutex.h>
#include <odincrt.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_vmutex
#include "dbglocal.h"

/******************************************************************************/
/******************************************************************************/
VMutex::VMutex()
{
  DosInitializeCriticalSection(&critsect, NULL);
}
/******************************************************************************/
/******************************************************************************/
VMutex::~VMutex()
{
  DosDeleteCriticalSection(&critsect);
}
/******************************************************************************/
/******************************************************************************/

