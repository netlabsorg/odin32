/* $Id: win32wndchild.h,v 1.3 1999-08-31 10:36:23 sandervl Exp $ */
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

#include <vmutex.h>

#ifdef __cplusplus

class ChildWindow
{
public:
 	 ChildWindow();
virtual ~ChildWindow();

protected:

 ChildWindow *GetParent()			{ return parent; };
 ChildWindow *SetParent(ChildWindow *newParent)
 {
  ChildWindow *oldparent = parent;

	parent = newParent;
	return oldparent;
 }                   

	BOOL   AddChild(ChildWindow *child);
	BOOL   RemoveChild(ChildWindow *child);

ChildWindow   *getFirstChild()                  { return children; };
ChildWindow   *getNextChild()   		{ return nextchild; };
        void   setNextChild(ChildWindow *child)	{ nextchild = child; };

	BOOL   DestroyChildren();

private:
        VMutex mutex;

   ChildWindow *parent;		//GWL_HWNDPARENT
   ChildWindow *children;	//linked list of children of this window
   ChildWindow *nextchild;	//pointer to next child of our parent window
};


#endif //__cplusplus

#endif //__WIN32WNDCHILD_H__
