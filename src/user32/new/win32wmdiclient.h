/* $Id: win32wmdiclient.h,v 1.3 1999-08-31 17:14:52 sandervl Exp $ */
/*
 * Win32 MDI Client Window Class for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WMDICLIENT_H__
#define __WIN32WMDICLIENT_H__

#include <win32wbase.h>
#include <win32wmdichild.h>
#include <mdi.h>

#define MDICLIENTCLASSNAMEA "MDICLIENT"
#define MDICLIENTCLASSNAMEW L"MDICLIENT"

//******************************************************************************
//******************************************************************************
class Win32MDIClientWindow : public Win32BaseWindow
{
public:
                Win32MDIClientWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
virtual        ~Win32MDIClientWindow();

	   int  incTotalCreated()                { return ++nTotalCreated; };
	   int  getFirstChildId()                { return idFirstChild;    };

	   HWND getMDIMenu()                     { return hWindowMenu;     };

Win32MDIChildWindow *getMaximizedChild()         { return maximizedChild;  };
               void  setMaximizedChild(Win32MDIChildWindow *newchild) { maximizedChild = newchild; };

Win32MDIChildWindow *getActiveChild()            { return activeChild;     };
               void  setActiveChild(Win32MDIChildWindow *newchild) { activeChild = newchild; };

Win32MDIChildWindow *getChildByID(INT id);

	   int  getNrOfChildren()                { return nActiveChildren; };
	   int  incNrActiveChildren()            { return ++nActiveChildren; };
	   int  decNrActiveChildren()            { return --nActiveChildren; };

	   LONG childActivate(Win32MDIChildWindow *child);
           void switchActiveChild(Win32MDIChildWindow *nextActiveChild, BOOL bNextWindow );
        LRESULT destroyChild(Win32MDIChildWindow *child, BOOL flagDestroy );

       LRESULT  MDIClientWndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
  Win32MDIChildWindow *maximizedChild;
  Win32MDIChildWindow *activeChild;

 	HWND    hWindowMenu;
        int     idFirstChild;

        int 	nActiveChildren;
        int 	nTotalCreated;
        LPSTR 	frameTitle;
        DWORD 	mdiFlags;

};
//******************************************************************************
//******************************************************************************
BOOL MDICLIENT_Register();
BOOL MDICLIENT_Unregister();

#endif //__WIN32WMDICLIENT_H__
