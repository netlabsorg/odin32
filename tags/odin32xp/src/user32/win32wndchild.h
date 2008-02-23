/* $Id: win32wndchild.h,v 1.7 2004-04-20 10:11:44 sandervl Exp $ */
/*
 * Win32 Child/Parent window class for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WNDCHILD_H__
#define __WIN32WNDCHILD_H__

#ifdef OS2_INCLUDED
#include <win32api.h>
#endif
#include <VMutex.h>

#ifdef __cplusplus

class ChildWindow
{
public:
 	 ChildWindow(VMutex *pLock);
virtual ~ChildWindow();

ChildWindow   *getFirstChild()                  { return children; };
ChildWindow   *getNextChild()   		{ return nextchild; };

protected:

 ChildWindow *getParentOfChild()		{ return parent; };
 ChildWindow *setParentOfChild(ChildWindow *newParent)
 {
  ChildWindow *oldparent = parent;

	parent = newParent;
	return oldparent;
 }

	BOOL   addChild(ChildWindow *child);
	BOOL   removeChild(ChildWindow *child);

        void   setNextChild(ChildWindow *child)	{ nextchild = child; };

	BOOL   destroyChildren();

private:
	 void  Lock()           { pLockChild->enter(); };
	 void  Unlock()         { pLockChild->leave(); };

   VMutex *pLockChild;

   ChildWindow *parent;		//GWL_HWNDPARENT
   ChildWindow *children;	//linked list of children of this window
   ChildWindow *nextchild;	//pointer to next child of our parent window
};


#endif //__cplusplus

#endif //__WIN32WNDCHILD_H__
