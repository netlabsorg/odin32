/* $Id: pmtitlebar.cpp,v 1.1 2000-01-08 14:15:38 sandervl Exp $ */
/*
 * Win32 Titlebar Managment Code for OS/2
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2000 by Christoph Bratschi (cbratschi@datacomm.ch)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_WIN
#define INCL_GPI

#include <os2.h>                        /* PM header file               */
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include <win32wbase.h>
#include <wprocess.h>
#include "pmframe.h"
#include "oslibutil.h"
#include "oslibwin.h"
#include "oslibmsg.h"
#include "pmtitlebar.h"

//******************************************************************************
//******************************************************************************
MRESULT EXPENTRY Win32TitleBarProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
 Win32BaseWindow *win32wnd;
 PFNWP            OldTitleBarProc;
 APIRET           rc = 0;
 THDB            *thdb;
 MSG              winMsg, *pWinMsg;

  SetWin32TIB();

  thdb = GetThreadTHDB();
  win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(WinQueryWindow(hwnd, QW_PARENT));

  if(!thdb || win32wnd == NULL || !win32wnd->getOldTitleBarProc())
  {
        dprintf(("Win32TitleBarProc: Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
    	goto RunDefWndProc;
  }
  OldTitleBarProc = (PFNWP)win32wnd->getOldTitleBarProc();

  if(msg == WM_MOUSEMOVE) {
	rc = 0;
  }
  if((thdb->msgstate & 1) == 0)
  {//message that was sent directly to our window proc handler; translate it here
        QMSG qmsg;

        qmsg.msg  = msg;
        qmsg.hwnd = hwnd;
        qmsg.mp1  = mp1;
        qmsg.mp2  = mp2;
        qmsg.time = WinQueryMsgTime(thdb->hab);
        WinQueryMsgPos(thdb->hab, &qmsg.ptl);
        qmsg.reserved = 0;

        if(OS2ToWinMsgTranslate((PVOID)thdb, &qmsg, &winMsg, FALSE, MSG_REMOVE) == FALSE)
        {//message was not translated
            memset(&winMsg, 0, sizeof(MSG));
        }
        pWinMsg = &winMsg;
  }
  else {
        pWinMsg = &thdb->msg;
        thdb->msgstate++;
  }
  win32wnd->clearDefWndProcCalled();

  switch(pWinMsg->message)
  {
  case WINWM_NCHITTEST:
  {
      DWORD res;

      // Only send this message if the window is enabled
      if (!WinIsWindowEnabled(hwnd))
        res = HT_ERROR;
      else if (win32wnd->getIgnoreHitTest())
        res = HT_NORMAL;
      else
      {
        dprintf(("USER32: WM_HITTEST %x (%d,%d)",hwnd,(*(POINTS *)&mp1).x,(*(POINTS *)&mp1).y));

        //CB: WinWindowFromPoint: PM sends WM_HITTEST -> loop -> stack overflow
        win32wnd->setIgnoreHitTest(TRUE);
        res = win32wnd->MsgHitTest(pWinMsg);
        win32wnd->setIgnoreHitTest(FALSE);
      }
      RestoreOS2TIB();
      return (MRESULT)res;
  }
  case WINWM_NCPAINT:
        win32wnd->DispatchMsgA(pWinMsg);
	if(win32wnd->getDefWndProcCalled())
        	goto RunDefTitleBarProc;
	goto RunDefWndProc;

  case WINWM_NCLBUTTONDOWN:
  case WINWM_NCLBUTTONUP:
  case WINWM_NCLBUTTONDBLCLK:
  case WINWM_NCMBUTTONDOWN:
  case WINWM_NCMBUTTONUP:
  case WINWM_NCMBUTTONDBLCLK:
  case WINWM_NCRBUTTONDOWN:
  case WINWM_NCRBUTTONUP:
  case WINWM_NCRBUTTONDBLCLK:
        win32wnd->MsgButton(pWinMsg);
	if(win32wnd->getDefWndProcCalled())
        	goto RunDefTitleBarProc;

        rc = TRUE;
        break;

  case WINWM_NCMOUSEMOVE:
  {
        //OS/2 Window coordinates -> Win32 Window coordinates
        win32wnd->MsgMouseMove(pWinMsg);
	if(win32wnd->getDefWndProcCalled())
        	goto RunDefTitleBarProc;
        break;
  }

  default:
      	goto RunDefTitleBarProc;
  }

  RestoreOS2TIB();
  return (MRESULT)rc;

RunDefTitleBarProc:
  RestoreOS2TIB();
  return OldTitleBarProc(hwnd,msg,mp1,mp2);

RunDefWndProc:
  RestoreOS2TIB();
  return WinDefWindowProc(hwnd,msg,mp1,mp2);
}
//******************************************************************************
//******************************************************************************
BOOL FrameSubclassTitleBar(Win32BaseWindow *window)
{
 HWND hwndTitleBar;

   hwndTitleBar = WinWindowFromID(window->getOS2FrameWindowHandle(), FID_TITLEBAR);
   if(hwndTitleBar) {  	
	window->setOldTitleBarProc(WinSubclassWindow(hwndTitleBar, PFNWP(Win32TitleBarProc)));
   }
   return FALSE;
}
//******************************************************************************
//******************************************************************************
