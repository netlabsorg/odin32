/* $Id: HOOK.CPP,v 1.5 1999-09-15 23:18:47 sandervl Exp $ */

/*
 * Win32 hook API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include "misc.h"
#include "hooks.h"

//******************************************************************************
//******************************************************************************
HHOOK WIN32API SetWindowsHookExA(int idHook, HOOKPROC hkprc, HINSTANCE hmod, DWORD dwThreadId)
{
 HHOOK    rc;
 HOOKPROC_O32 os2hkprc;

    switch(idHook) {
	case WH_CALLWNDPROC:
		os2hkprc = HkWindow::GetOS2Hook();
		break;
	case WH_CBT:
	{
		HkCBT *hkhook = new HkCBT(0, hkprc, hmod, dwThreadId);;	
#ifdef DEBUG
	        WriteLog("OS2SetWindowsHookExA WH_CBT %X, %X, %X, %X\n", idHook, hkprc, hmod, dwThreadId);
#endif
		return((HHOOK)hkhook);
	}
	case WH_DEBUG:
		os2hkprc = HkDebug::GetOS2Hook();	
		break;
	case WH_JOURNALPLAYBACK:
		os2hkprc = HkJrnlPlayback::GetOS2Hook();	
		break;
	case WH_JOURNALRECORD:
		os2hkprc = HkJrnlRecord::GetOS2Hook();	
		break;
	case WH_GETMESSAGE:
		os2hkprc = HkGetMessage::GetOS2Hook();	
		break;
	case WH_MOUSE:
		os2hkprc = HkMouse::GetOS2Hook();	
		break;
	case WH_KEYBOARD:
		os2hkprc = HkKeyboard::GetOS2Hook();	
		break;
	case WH_SHELL:
		os2hkprc = HkShell::GetOS2Hook();	
		break;
	case WH_SYSMSGFILTER:
		os2hkprc = HkSysMsgFilter::GetOS2Hook();	
		break;
	case WH_MSGFILTER:
		os2hkprc = HkMsgFilter::GetOS2Hook();	
		break;
    }

    rc = O32_SetWindowsHookEx(idHook, os2hkprc, hmod, dwThreadId);

    if(rc) {
      switch(idHook) {
	case WH_CALLWNDPROC:
		new HkWindow(rc, hkprc, hmod, dwThreadId);
		break;
	case WH_CBT:
		break;
	case WH_DEBUG:
		new HkDebug(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_JOURNALPLAYBACK:
		new HkJrnlPlayback(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_JOURNALRECORD:
		new HkJrnlRecord(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_GETMESSAGE:
		new HkGetMessage(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_MOUSE:
		new HkMouse(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_KEYBOARD:
		new HkKeyboard(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_SHELL:
		new HkShell(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_SYSMSGFILTER:
		new HkSysMsgFilter(rc, hkprc, hmod, dwThreadId);;	
		break;
	case WH_MSGFILTER:
		new HkMsgFilter(rc, hkprc, hmod, dwThreadId);;	
		break;
      }	
    }
#ifdef DEBUG
    WriteLog("OS2SetWindowsHookExA %X, %X, %X, %X, returned %X\n", idHook, hkprc, hmod, dwThreadId, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HHOOK WIN32API SetWindowsHookA(int idHook, HOOKPROC hkprc)
{
 HHOOK rc;

    rc = SetWindowsHookExA(idHook, hkprc, NULL, 0);
#ifdef DEBUG
    WriteLog("OS2SetWindowsHookA %X, %X, returned %X\n", idHook, hkprc, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HHOOK WIN32API SetWindowsHookExW(int idHook, HOOKPROC hkprc, HINSTANCE hmod, DWORD dwThreadId)
{
#ifdef DEBUG
    WriteLog("OS2SetWindowsHookExW\n");
#endif
    return(SetWindowsHookExA(idHook, hkprc, hmod, dwThreadId));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnhookWindowsHookEx(HHOOK hook)
{
#ifdef DEBUG
    WriteLog("OS2UnhookWindowsHookEx\n");
#endif
   
    if(HkWindow::UnHookWindowsHook(hook) == FALSE)
     if(HkCBT::UnHookWindowsHook(hook) == TRUE) {
	return(TRUE);
     }
     else
      if(HkGetMessage::UnHookWindowsHook(hook) == FALSE)
       if(HkMsgFilter::UnHookWindowsHook(hook) == FALSE)
        if(HkSysMsgFilter::UnHookWindowsHook(hook) == FALSE)
         if(HkMouse::UnHookWindowsHook(hook) == FALSE)
          if(HkKeyboard::UnHookWindowsHook(hook) == FALSE)
           if(HkShell::UnHookWindowsHook(hook) == FALSE)
            if(HkDebug::UnHookWindowsHook(hook) == FALSE)
             if(HkJrnlPlayback::UnHookWindowsHook(hook) == FALSE)
              if(HkJrnlRecord::UnHookWindowsHook(hook) == FALSE) {
		dprintf(("Hook %X not found!\n", hook));
	      }
    return O32_UnhookWindowsHookEx(hook);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnhookWindowsHook(int nCode, HOOKPROC hkproc)
{
 HHOOK hook = 0;
    
    switch(nCode) {
	case WH_CALLWNDPROC:
		hook = HkWindow::FindHookProc(hkproc);	
		break;
	case WH_CBT:
		hook = HkCBT::FindHookProc(hkproc);
		break;
	case WH_DEBUG:
		hook = HkDebug::FindHookProc(hkproc);	
		break;
	case WH_JOURNALPLAYBACK:
		hook = HkJrnlPlayback::FindHookProc(hkproc);	
		break;
	case WH_JOURNALRECORD:
		hook = HkJrnlRecord::FindHookProc(hkproc);	
		break;
	case WH_GETMESSAGE:
		hook = HkGetMessage::FindHookProc(hkproc);	
		break;
	case WH_MOUSE:
		hook = HkMouse::FindHookProc(hkproc);	
		break;
	case WH_KEYBOARD:
		hook = HkKeyboard::FindHookProc(hkproc);	
		break;
	case WH_SHELL:
		hook = HkShell::FindHookProc(hkproc);
		break;
	case WH_SYSMSGFILTER:
		hook = HkSysMsgFilter::FindHookProc(hkproc);	
		break;
	case WH_MSGFILTER:
		hook = HkMsgFilter::FindHookProc(hkproc);	
		break;
    }
#ifdef DEBUG
    WriteLog("OS2UnhookWindowsHook %X\n", hook);
#endif
    if(hook == 0)
	return(FALSE);

    return O32_UnhookWindowsHookEx(hook);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API CallNextHookEx(HHOOK arg1, int arg2, WPARAM arg3, LPARAM  arg4)
{
 HkCBT *hook;

#ifdef DEBUG
    WriteLog("OS2CallNextHookEx\n");
#endif
    if((hook = HkCBT::FindHook(arg1)) != NULL) {
		return hook->CallNextHook(arg1, arg2, arg3, arg4);
    }
    else	return O32_CallNextHookEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//TODO
//******************************************************************************
BOOL WIN32API CallMsgFilterA( LPMSG msg, int nCode)
{
#ifdef DEBUG
    WriteLog("USER32:  CallMsgFilterA\n");
#endif
#if 0
    if(HkSysMsgFilter::OS2HkCBTProc(msg->hwnd, nCode, 0, (LPARAM)msg))
	return TRUE;

    return HkMsgFilter::OS2HkCBTProc(hwnd, Msg, wParam, lParam);
#else
    return 0;
#endif
}
//******************************************************************************
//TODO
//******************************************************************************
BOOL WIN32API CallMsgFilterW( LPMSG msg, int nCode)
{
#ifdef DEBUG
    WriteLog("USER32:  CallMsgFilterW\n");
#endif
#if 0
    if(HkSysMsgFilter::OS2HkCBTProc(msg->hwnd, nCode, 0, (LPARAM)msg))
	return TRUE;

    return HkMsgFilter::OS2HkCBTProc(hwnd, Msg, wParam, lParam);
#else
    return 0;
#endif
}
//******************************************************************************
//******************************************************************************
