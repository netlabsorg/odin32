/* $Id: gen_object.cpp,v 1.1 1999-09-15 23:18:50 sandervl Exp $ */
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
#include <os2.h>
#include <os2wrap.h>
#include <stdlib.h>
#include <win32type.h>
#include <gen_object.h>
#include <misc.h>
#include <vmutex.h>

VMutex genMutex[OBJTYPE_MAX];

//******************************************************************************
//******************************************************************************
GenericObject::GenericObject(GenericObject **head, DWORD objType)
{
  this->objType = objType;
  this->head    = head;
  this->next    = NULL;

  genMutex[objType].enter();
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
  genMutex[objType].leave();
}
//******************************************************************************
//******************************************************************************
GenericObject::~GenericObject()
{
  //remove from linked list
  genMutex[objType].enter();
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
			DebugInt3();
		}
	}
	cur->next = next;
  }
  genMutex[objType].leave();
}
//******************************************************************************
//******************************************************************************
void GenericObject::enterMutex()
{
  genMutex[objType].enter();
}
//******************************************************************************
//******************************************************************************
void GenericObject::leaveMutex()
{
  genMutex[objType].leave();
}
//******************************************************************************
//******************************************************************************
void GenericObject::enterMutex(DWORD objType)
{
  genMutex[objType].enter();
}
//******************************************************************************
//******************************************************************************
void GenericObject::leaveMutex(DWORD objType)
{
  genMutex[objType].leave();
}
//******************************************************************************
//******************************************************************************
