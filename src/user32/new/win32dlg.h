/* $Id: win32dlg.h,v 1.1 1999-05-24 20:20:04 ktk Exp $ */

#ifndef __WIN32DLG_H__
#define __WIN32DLG_H__

#include "win32wnd.h"

#ifdef __cplusplus

class Win32Dialog : Win32Window
{
public:
 	 Win32Dialog(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
virtual ~Win32Dialog();

     MRESULT    ProcessDlgMessage(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

virtual	 BOOL   SetWindowLong(int index, ULONG value);
virtual	 ULONG  GetWindowLong(int index);

protected:
	// values normally contained in the standard dialog words
    WINDLGPROC	Win32DlgProc;	//DWL_WNDPROC
	ULONG	msgResult;	//DWL_MSGRESULT
	ULONG	userDlgData;	//DWL_USER

private:


};


#endif //__cplusplus

#endif //__WIN32WND_H__