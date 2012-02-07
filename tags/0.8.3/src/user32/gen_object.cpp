/* $Id: gen_object.cpp,v 1.13 2004-04-20 10:11:42 sandervl Exp $ */
/*
 * Generic Object Class for OS/2
 *
 * Allocated in shared memory, so other processes can access the objects
 *
 * NOTE: Requires safety precautions to use objects in multiple threads or processes
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define  INCL_DOSSEMAPHORES
#include <os2wrap.h>
#include <stdlib.h>
#include <misc.h>
#include <win32type.h>
#include "gen_object.h"

#define DBG_LOCALLOG	DBG_gen_object
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
GenericObject::GenericObject(GenericObject **head, VMutex *pLock)
{
  this->pLock = pLock;
  this->head  = head;
  this->next  = NULL;
  refCount    = 1;

  fLinked     = FALSE;
  fDeletePending = FALSE;

  link();
}
//******************************************************************************
//******************************************************************************
GenericObject::~GenericObject()
{
  unlink();
}
//******************************************************************************
//******************************************************************************
void GenericObject::link()
{
  lock();
  if(*head == NULL) {
	*head = this;	
  }
  else {
	GenericObject *cur = *head;
	while(cur->next)
	{
		cur = cur->next;
	}
	cur->next = this;
  }
  fLinked = TRUE;
  unlock();
}
//******************************************************************************
//******************************************************************************
void GenericObject::unlink()
{
  if(!fLinked)   return;

  //remove from linked list
  lock();
  if(*head == this) {
	*head = next;	
  }
  else {
	GenericObject *cur = *head;
	while(cur->next != this)
	{
		cur = cur->next;
		if(cur == NULL) {
			dprintf(("GenericObject dtor: cur == NULL!!"));
                        unlock();
			DebugInt3();
                        return;
		}
	}
	cur->next = next;
  }
  unlock();
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
LONG GenericObject::addRef()
{
////  dprintf2(("addRef %x -> refcount %x", this, refCount+1));
  return InterlockedIncrement(&refCount);
}
#endif
//******************************************************************************
//******************************************************************************
LONG GenericObject::release()
{
////  dprintf2(("release %x -> refcount %x", this, refCount-1));
#ifdef DEBUG
  if(refCount-1 < 0) {
      DebugInt3();
  }
#endif
  if(InterlockedDecrement(&refCount) == 0 && fDeletePending) {
      dprintf2(("marked for deletion -> delete now"));
      delete this;
      return 0;
  }
  return refCount;
}
//******************************************************************************
//******************************************************************************
void GenericObject::DestroyAll(GenericObject *head)
{
 GenericObject *cur, *next;

  cur = head;
  while(cur) {
	next = cur->next;
        if(cur->getRefCount() != 0) {
            dprintf(("Refcount %d for object %x", cur->getRefCount(), cur));
        }
	delete cur;
	cur = next;
  }
}
//******************************************************************************
//******************************************************************************
