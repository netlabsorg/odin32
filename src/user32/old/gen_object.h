/* $Id: gen_object.h,v 1.1 2001-05-11 08:35:18 sandervl Exp $ */
/*
 * Generic Object Class for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __GEN_OBJECT_H__
#define __GEN_OBJECT_H__

#include <heapshared.h>

#define OBJTYPE_WINDOW 		0
#define OBJTYPE_DIALOG		1
#define OBJTYPE_CLASS		2
#define OBJTYPE_BITMAP		3
#define OBJTYPE_ICON		4
#define OBJTYPE_CURSOR		5
#define OBJTYPE_MENU		6

#define OBJTYPE_MAX             7
//......

class GenericObject
{
public:
	 GenericObject(GenericObject **head, DWORD objType);
virtual ~GenericObject();

	 DWORD getObjectType()			{ return objType; };
	 void  setObjectType(DWORD objType)	{ this->objType = objType; };

GenericObject *GetHead()	{ return *head; };
GenericObject *GetNext()	{ return next;  };

	 void  enterMutex();
	 void  leaveMutex();

static	 void  enterMutex(DWORD objType);
static	 void  leaveMutex(DWORD objType);

static   void  DestroyAll(GenericObject *head);

#ifdef __DEBUG_ALLOC__
	void *operator new(size_t size, const char *filename, size_t lineno)
	{
		return _smalloc(size);
	}
	void operator delete(void *location, const char *filename, size_t lineno)
	{
		free(location);
	}
#else
	void *operator new(size_t size)
	{
		return _smalloc(size);
	}
	void operator delete(void *location)
	{
		free(location);
	}
#endif

private:

         DWORD objType;

protected:

         GenericObject **head;
 	 GenericObject  *next;	 
};

#endif