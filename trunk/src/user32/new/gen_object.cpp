/* $Id: gen_object.cpp,v 1.3 1999-08-24 09:20:30 sandervl Exp $ */
/*
 * Generic Object Class for OS/2
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
