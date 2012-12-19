/* $Id: win32wndchild.cpp,v 1.8 2004-04-20 10:11:44 sandervl Exp $ */
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
#include <os2win.h>
#include "win32wndchild.h"
#include <misc.h>

#define DBG_LOCALLOG	DBG_win32wndchild
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
ChildWindow::ChildWindow(VMutex *pLock)
{
  parent     = 0;
  nextchild  = 0;
  children   = 0;
  pLockChild = pLock;
}
//******************************************************************************
//******************************************************************************
ChildWindow::~ChildWindow()
{
  if(parent) {
	parent->removeChild(this);
  }
//SvL: PM sends WM_DESTROY for all the children
#if 0
  if(children != 0) {
	dprintf(("ChildWindow::~ChildWindow children not yet destroyed!!"));
	DestroyChildren();
  }
#endif
}
//******************************************************************************
//FIFO insertion
//******************************************************************************
BOOL ChildWindow::addChild(ChildWindow *child)
{
 ChildWindow *curchild;

   Lock();

   curchild = children;
   if(curchild == NULL) {
   	children = child;
   }
   else {
	while(curchild->getNextChild()) {
		curchild = curchild->getNextChild();
	}
	curchild->setNextChild(child);
   }
   child->setNextChild(NULL);

   Unlock();
   return TRUE;
}
//******************************************************************************
//Remove child from linked list. Doesn't delete it!
//******************************************************************************
BOOL ChildWindow::removeChild(ChildWindow *child)
{
 ChildWindow *curchild = children;

   Lock();

   if(curchild == child) {
	children = child->getNextChild();	
   }
   else {
	if(curchild == NULL) {
		dprintf(("ChildWindow::RemoveChild, children == NULL"));
		DebugInt3();
                Unlock();
		return FALSE;
	}
	while(curchild->getNextChild() != child) {
		curchild = curchild->getNextChild();
		if(curchild == NULL) {
			dprintf(("ChildWindow::RemoveChild, curchild == NULL"));
			DebugInt3();
                        Unlock();
			return FALSE;
		}	
	}
	curchild->setNextChild(child->getNextChild());
   }
   Unlock();
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL ChildWindow::destroyChildren()
{
   while(children) {
	delete children;	//child dtor removes itself from the linked list
   }
   return TRUE;
}
//******************************************************************************
//******************************************************************************
