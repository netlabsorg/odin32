/* $Id: win32wmdichild.h,v 1.1 2001-05-11 08:35:21 sandervl Exp $ */
/*
 * Win32 MDI Client Window Class for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WMDICHILD_H__
#define __WIN32WMDICHILD_H__

#include <win32wbase.h>
#include <mdi.h>

class Win32MDIClientWindow;

//******************************************************************************
//******************************************************************************
class Win32MDIChildWindow : public Win32BaseWindow
{
public:
                Win32MDIChildWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL fUnicode);
virtual        ~Win32MDIChildWindow();

   static HWND  createChild(Win32MDIClientWindow *client, LPMDICREATESTRUCTA cs);
           void childGetMinMaxInfo(MINMAXINFO* lpMinMax);

           BOOL menuModifyItem();
	   BOOL menuDeleteItem();

virtual  BOOL   isMDIChild();

        LRESULT DefMDIChildProcA(UINT Msg, WPARAM wParam, LPARAM lParam);
        LRESULT DefMDIChildProcW(UINT Msg, WPARAM wParam, LPARAM lParam);

   static  void calcDefaultChildPos(Win32MDIClientWindow *client, WORD n, LPPOINT lpPos, INT delta);
private:

   friend class Win32MDIClientWindow;
};
//******************************************************************************
//******************************************************************************

#endif //__WIN32WMDICHILD_H__
