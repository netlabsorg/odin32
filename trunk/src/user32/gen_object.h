/* $Id: gen_object.h,v 1.6 2001-06-09 14:50:17 sandervl Exp $ */
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

class GenericObject
{
public:
	 GenericObject(GenericObject **head, CRITICAL_SECTION *pLock);
virtual ~GenericObject();

GenericObject *GetHead()	{ return *head; };
GenericObject *GetNext()	{ return next;  };

	 void  lock()           { EnterCriticalSection(pLock); };
	 void  unlock()         { LeaveCriticalSection(pLock); };

         void  link();
         void  unlink();

         LONG  addRef()         { return InterlockedIncrement(&refCount); };
         LONG  getRefCount()    { return refCount; };
         LONG  release();

         void  markDeleted()    { fDeletePending = TRUE; };

static	 void  lock(CRITICAL_SECTION *pLock)           { EnterCriticalSection(pLock); };
static	 void  unlock(CRITICAL_SECTION *pLock)         { LeaveCriticalSection(pLock); };

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

         CRITICAL_SECTION *pLock;
         LONG              refCount;
         ULONG             fLinked        : 1,
                           fDeletePending : 1;

         GenericObject   **head;
 	 GenericObject    *next;	 
};

#endif