/* $Id: win32dlg.h,v 1.2 1999-07-14 08:35:37 sandervl Exp $ */
/*
 * Win32 Dialog Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32DLG_H__
#define __WIN32DLG_H__

#include <win32wnd.h>

#ifdef __cplusplus

class Win32Dialog : public Win32Window
{
public:
 	 Win32Dialog();
virtual ~Win32Dialog();

virtual  ULONG  MsgOS2Create(HWND hwndOS2, ULONG initParam);

virtual	 LONG   SetWindowLongA(int index, ULONG value);
virtual	 ULONG  GetWindowLongA(int index);

protected:
	// values normally contained in the standard dialog words
      DLGPROC	Win32DlgProc;	//DWL_WNDPROC
	ULONG	msgResult;	//DWL_MSGRESULT
	ULONG	userDlgData;	//DWL_USER

private:
};


#endif //__cplusplus

#endif //__WIN32WND_H__