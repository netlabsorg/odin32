/* $Id: gen_object.cpp,v 1.1 1999-07-14 08:35:34 sandervl Exp $ */
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

HMTX genMutexSem = 0;

//******************************************************************************
//******************************************************************************
GenericObject::GenericObject(GenericObject **head, DWORD objType)
{
 APIRET rc;

  this->objType = objType;
  this->head    = head;
  this->next    = NULL;

  if(genMutexSem == 0) {
	rc = DosCreateMutexSem(NULL, &genMutexSem, 0, FALSE);
	if(rc) {
		dprintf(("GenericObject ctor: DosCreateMutexSem returned %d", rc));
		DebugInt3();
		exit(1);
	}
  }
  //insert in linked list
  rc = DosRequestMutexSem(genMutexSem, SEM_INDEFINITE_WAIT);
  if(rc) {
	dprintf(("GenericObject ctor: DosRequestMutexSem returned %d", rc));
	DebugInt3();
  }
  if(*head == NULL) {
	*head = this;	
  }
  else {
	GenericObject *cur = (*head)->next;
	while(cur->next)
	{
		cur = cur->next;
	}
	cur->next = this;
  }
  DosReleaseMutexSem(genMutexSem);
}
//******************************************************************************
//******************************************************************************
GenericObject::~GenericObject()
{
 APIRET rc;

  //remove from linked list
  rc = DosRequestMutexSem(genMutexSem, SEM_INDEFINITE_WAIT);
  if(rc) {
	dprintf(("GenericObject dtor: DosRequestMutexSem returned %d", rc));
	DebugInt3();
  }
  if(*head == this) {
	*head = next;	
  }
  else {
	GenericObject *cur = (*head)->next;
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
  DosReleaseMutexSem(genMutexSem);
}
//******************************************************************************
//******************************************************************************
