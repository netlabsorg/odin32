/* $Id: win32wmdiclient.h,v 1.1 2001-05-11 08:35:21 sandervl Exp $ */
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

#define MDIF_NEEDUPDATE         0x0001

//******************************************************************************
//******************************************************************************
class Win32MDIClientWindow : public Win32BaseWindow
{
public:
                Win32MDIClientWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
virtual        ~Win32MDIClientWindow();

virtual  BOOL   isMDIClient();

           int  incTotalCreated()                { return ++nTotalCreated; };
           int  getFirstChildId()                { return idFirstChild;    };

           HWND getMDIMenu()                     { return hWindowMenu;     };

Win32MDIChildWindow *getMaximizedChild()         { return maximizedChild;  };
               void  setMaximizedChild(Win32MDIChildWindow *newchild) { maximizedChild = newchild; };

Win32MDIChildWindow *getActiveChild()            { return activeChild;     };
               void  setActiveChild(Win32MDIChildWindow *newchild) { activeChild = newchild; };

Win32MDIChildWindow *getWindow(Win32MDIChildWindow *actchild, BOOL bNext, DWORD dwStyleMask);

Win32MDIChildWindow *getChildByID(INT id);
               void postUpdate(WORD recalc);

           int  getNrOfChildren()                { return nActiveChildren; };
           int  incNrActiveChildren()            { return ++nActiveChildren; };
           int  decNrActiveChildren()            { return --nActiveChildren; };

           int  getMdiFlags()                    { return mdiFlags; };
           void setMdiFlags(int newflags)        { mdiFlags = newflags; };

           void updateFrameText(BOOL repaint, LPCSTR lpTitle );

           LONG childActivate(Win32MDIChildWindow *child);
           void switchActiveChild(Win32MDIChildWindow *nextActiveChild, BOOL bNextWindow );
        LRESULT destroyChild(Win32MDIChildWindow *child, BOOL flagDestroy );

        LRESULT setMDIMenu(HMENU hmenuFrame, HMENU hmenuWindow);
        LRESULT refreshMDIMenu(HMENU hmenuFrame, HMENU hmenuWindow);
        BOOL    restoreFrameMenu(Win32BaseWindow *child);

       LRESULT  MDIClientWndProc(UINT message, WPARAM wParam, LPARAM lParam);

Win32BaseWindow** buildWindowArray(UINT bwaFlags,PUINT total);
        void    releaseWindowArray(Win32BaseWindow **wndArray);
        void    calcDefaultChildPos(WORD n,LPPOINT lpPos,INT delta);
        BOOL    cascade(UINT fuCascade);
        BOOL    tile(UINT fuTile);
        BOOL    augmentFrameMenu(Win32MDIChildWindow *child);
        BOOL    restoreFrameMenu(Win32MDIChildWindow *child);

private:
  Win32MDIChildWindow *maximizedChild;
  Win32MDIChildWindow *activeChild;

        HWND    hWindowMenu;
        int     idFirstChild;

        int     nActiveChildren;
        int     nTotalCreated;
        int     sbRecalc;
        LPSTR   frameTitle;
        DWORD   mdiFlags;

};
//******************************************************************************
//******************************************************************************
BOOL MDICLIENT_Register();
BOOL MDICLIENT_Unregister();

#endif //__WIN32WMDICLIENT_H__
