/* $Id: pmwindow.cpp,v 1.1 1999-05-24 20:20:04 ktk Exp $ */

/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_WIN
#define INCL_GPI

#include <os2.h>                        /* PM header file               */
#include "misc.h"
#include "win32wnd.h"
#include "win32dlg.h"

HMQ  hmq = 0;                             /* Message queue handle         */
HAB  hab = 0;

MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY Win32DialogProc(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);

//******************************************************************************
//Initialize PM; create hab, message queue and register special Win32 window class
//******************************************************************************
BOOL InitPM()
{
  hab = WinInitialize(0);
  hmq = WinCreateMsgQueue(hab, 0);

  if(!hab || !hmq) {
	dprintf(("WinInitialize or WinCreateMsgQueue failed %x %x", hab, hmq));
	return(FALSE);
  }

  if(!WinRegisterClass(                 /* Register window class        */
     hab,                               /* Anchor block handle          */
     (PSZ)"Win32Window",                /* Window class name            */
     (PFNWP)Win32WindowProc,            /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     4)) {
	dprintf(("WinRegisterClass Win32Window failed"));
	return(FALSE);
   }

  if(!WinRegisterClass(                 /* Register window class        */
     hab,                               /* Anchor block handle          */
     (PSZ)"Win32Dialog",                /* Window class name            */
     (PFNWP)Win32DialogProc,            /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     4)) {
	dprintf(("WinRegisterClass Win32Dialog failed"));
	return(FALSE);
   }

   return(TRUE);
} /* End of main */
//******************************************************************************
//Win32 window message handler
//******************************************************************************
MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 Win32Window *win32wnd = (Win32Window *)WinQueryWindowULong(hwnd, 0);
 MRESULT      rc;

  switch( msg )
  {
    case WM_CREATE:
	win32wnd = new Win32Window(hwnd, msg, mp1, mp2);
	if(win32wnd == NULL) {
		dprintf(("new win32wnd failed!"));
	}
	if(WinSetWindowULong(hwnd, 0, (ULONG)win32wnd) == FALSE) {
		dprintf(("WinSetWindowULong %X failed!!", hwnd));
	}
      	break;

    default:
      /*
       * Everything else comes here.  This call MUST exist
       * in your window procedure.
       */
	if(win32wnd == NULL) {
		dprintf(("Win32WindowProc: win32wnd NULL!"));
		return WinDefWindowProc( hwnd, msg, mp1, mp2 );
	}

      	if(win32wnd->ProcessMessage(hwnd, msg, mp1, mp2, &rc) == TRUE) {
		return(rc);
      	}
      	return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT)FALSE;
} /* End of Win32WindowProc */
//******************************************************************************
//Win32 dialog message handler
//******************************************************************************
MRESULT EXPENTRY Win32DialogProc( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
 Win32Dialog *win32dlg = (Win32Dialog *)WinQueryWindowULong(hwndDlg, 0);
 MRESULT      rc;

  switch(msg)
  {
    case WM_INITDLG:
	win32dlg = new Win32Dialog(hwndDlg, msg, mp1, mp2);
	if(win32dlg == NULL) {
		dprintf(("new win32dlg failed!"));
	}
	if(WinSetWindowULong(hwndDlg, 0, (ULONG)win32dlg) == FALSE) {
		dprintf(("WinSetWindowULong %X failed!!", hwndDlg));
	}
        break;

    default:
      /*
       * Any event messages that the dialog procedure has not processed
       * come here and are processed by WinDefDlgProc.
       * This call MUST exist in your dialog procedure.
       */
	if(win32dlg == NULL) {
		dprintf(("Win32DialogProc: win32dlg NULL!"));
		return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
	}
      	if(win32dlg->ProcessDlgMessage(hwndDlg, msg, mp1, mp2, &rc) == TRUE) {
		return(rc);
      	}
      	return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
  }
  return (MRESULT) FALSE;
}
//******************************************************************************
//******************************************************************************
