/* $Id: gen_object.h,v 1.8 2004-04-20 10:11:42 sandervl Exp $ */
/*
 * Generic Object Class for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __GEN_OBJECT_H__
#define __GEN_OBJECT_H__

#include <heapshared.h>
#ifdef OS2_INCLUDED
#include <win32api.h>
#endif
#include <VMutex.h>

class GenericObject
{
public:
	 GenericObject(GenericObject **head, VMutex *pLock);
virtual ~GenericObject();

GenericObject *GetHead()	{ return *head; };
GenericObject *GetNext()	{ return next;  };

	 void  lock()           { pLock->enter(); };
	 void  unlock()         { pLock->leave(); };

         void  link();
         void  unlink();

#ifdef DEBUG
         LONG  addRef();
#else
         LONG  addRef()         { return InterlockedIncrement(&refCount); };
#endif
         LONG  getRefCount()    { return refCount; };
         LONG  release();

         void  markDeleted()    { fDeletePending = TRUE; };

static	 void  lock(VMutex *pLock)           { pLock->enter(); };
static	 void  unlock(VMutex *pLock)         { pLock->leave(); };

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

protected:

         VMutex           *pLock;
         LONG              refCount;
         ULONG             fLinked        : 1,
                           fDeletePending : 1;

         GenericObject   **head;
 	 GenericObject    *next;	
};

#endif