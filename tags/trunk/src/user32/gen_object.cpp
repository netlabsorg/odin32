/* $Id: gen_object.cpp,v 1.8 2001-06-09 14:50:17 sandervl Exp $ */
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
#include <gen_object.h>

#define DBG_LOCALLOG	DBG_gen_object
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
GenericObject::GenericObject(GenericObject **head, CRITICAL_SECTION *pLock)
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
LONG GenericObject::release()
{
////  dprintf(("release -> refcount %x", refCount));
#ifdef DEBUG
  if(refCount-1 < 0) {
      DebugInt3();
  }
#endif
  if(InterlockedDecrement(&refCount) == 0 && fDeletePending) {
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
