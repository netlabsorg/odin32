/* $Id: win32wndchild.cpp,v 1.2 1999-08-28 14:09:30 sandervl Exp $ */
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
  if(children != 0) {
	dprintf(("ChildWindow::~ChildWindow children not yet destroyed!!"));
	DestroyChildren();
  }
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
	while(curchild->GetNextChild()) {
		curchild = curchild->GetNextChild();
	}
	curchild->SetNextChild(this);
   }
   child->SetNextChild(NULL);

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
	children = child->GetNextChild();	
   }
   else {
	if(curchild == NULL) {
		dprintf(("ChildWindow::RemoveChild, children == NULL"));
		DebugInt3();
		mutex.leave();
		return FALSE;
	}
	while(curchild->GetNextChild() != child) {
		curchild = curchild->GetNextChild();
		if(curchild == NULL) {
			dprintf(("ChildWindow::RemoveChild, curchild == NULL"));
			DebugInt3();
			mutex.leave();
			return FALSE;
		}	
	}
	curchild->SetNextChild(child->GetNextChild());
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
