/* $Id: win32wndchild.cpp,v 1.2 1999-10-15 13:52:56 sandervl Exp $ */
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
#include <win32wndchild.h>
#include <misc.h>

//******************************************************************************
//******************************************************************************
ChildWindow::ChildWindow()
{
  parent           = 0;
  nextchild        = 0;
  children         = 0;
}
//******************************************************************************
//******************************************************************************
ChildWindow::~ChildWindow()
{
  if(parent) {
	parent->RemoveChild(this);
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
BOOL ChildWindow::AddChild(ChildWindow *child)
{
 ChildWindow *curchild;

   mutex.enter();

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

   mutex.leave();
   return TRUE;
}
//******************************************************************************
//Remove child from linked list. Doesn't delete it!
//******************************************************************************
BOOL ChildWindow::RemoveChild(ChildWindow *child)
{
 ChildWindow *curchild = children;

   mutex.enter();

   if(curchild == child) {
	children = child->getNextChild();	
   }
   else {
	if(curchild == NULL) {
		dprintf(("ChildWindow::RemoveChild, children == NULL"));
		DebugInt3();
		mutex.leave();
		return FALSE;
	}
	while(curchild->getNextChild() != child) {
		curchild = curchild->getNextChild();
		if(curchild == NULL) {
			dprintf(("ChildWindow::RemoveChild, curchild == NULL"));
			DebugInt3();
			mutex.leave();
			return FALSE;
		}	
	}
	curchild->setNextChild(child->getNextChild());
   }
   mutex.leave();
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL ChildWindow::DestroyChildren()
{
   while(children) {
	delete children;	//child dtor removes itself from the linked list
   }
   return TRUE;
}
//******************************************************************************
//******************************************************************************
