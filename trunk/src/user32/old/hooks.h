/* $Id: hooks.h,v 1.1 1999-09-15 23:04:26 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 hook class for OS/2
 * Copyright 1998 Sander van Leeuwen
 */
#ifndef __HOOKS_H__
#define __HOOKS_H__

class HkWindow
{
public:
	HkWindow(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkWindow();

 static HOOKPROC_O32 GetOS2Hook();
 static         BOOL UnHookWindowsHook(HHOOK hook);
 static        HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC       pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkWindow  *hooks;
  	        HkWindow  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkWindowProc(int code, WPARAM wParam, LPARAM lParam);

};

class HkCBT
{
public:
	HkCBT(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkCBT();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);
 static     HkCBT *FindHook(HHOOK hhook);
 static    LRESULT OS2HkCBTProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	   LRESULT CallNextHook(HHOOK hhook, int nCode, WPARAM wParam, LPARAM lParam);
	   LRESULT CBTCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

 HOOKPROC       pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 //Original parameters of GetMessage hook (TODO: NOT REENTRANT!!)
 HWND 		orghwnd;
 UINT 		orgMsg;
 WPARAM 	orgwParam;
 LPARAM 	orglParam;

 static	        HkCBT  *hooks;
  	        HkCBT  *next;
};

class HkDebug
{
public:
	HkDebug(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkDebug();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkDebug  *hooks;
  	        HkDebug  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkDebugProc(int code, WPARAM wParam, LPARAM lParam);

};

class HkGetMessage
{
public:
	HkGetMessage(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkGetMessage();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkGetMessage  *hooks;
  	        HkGetMessage  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkGetMessageProc(int code, WPARAM wParam, LPARAM lParam);
};

class HkJrnlPlayback
{
public:
	HkJrnlPlayback(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkJrnlPlayback();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkJrnlPlayback  *hooks;
  	        HkJrnlPlayback  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkJrnlPlaybackProc(int code, WPARAM wParam, LPARAM lParam);
};

class HkJrnlRecord
{
public:
	HkJrnlRecord(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkJrnlRecord();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkJrnlRecord  *hooks;
  	        HkJrnlRecord  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkJrnlRecordProc(int code, WPARAM wParam, LPARAM lParam);
};


class HkKeyboard
{
public:
	HkKeyboard(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkKeyboard();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkKeyboard  *hooks;
  	        HkKeyboard  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkKeyboardProc(int code, WPARAM wParam, LPARAM lParam);
};

class HkMouse
{
public:
	HkMouse(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkMouse();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkMouse  *hooks;
  	        HkMouse  *next;
 friend static LRESULT EXPENTRY_O32 OS2HkMouseProc(int code, WPARAM wParam, LPARAM lParam);
};

class HkMsgFilter
{
public:
	HkMsgFilter(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkMsgFilter();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkMsgFilter  *hooks;
  	        HkMsgFilter  *next;
 friend static LRESULT EXPENTRY_O32 OS2HkMsgFilterProc(int code, WPARAM wParam, LPARAM lParam);
};

class HkShell
{
public:
	HkShell(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkShell();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkShell  *hooks;
   	        HkShell  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkShellProc(int code, WPARAM wParam, LPARAM lParam);
};

class HkSysMsgFilter
{
public:
	HkSysMsgFilter(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId);
       ~HkSysMsgFilter();

 static   HOOKPROC_O32 GetOS2Hook();
 static       BOOL UnHookWindowsHook(HHOOK hook);
 static      HHOOK FindHookProc(HOOKPROC hkprc);

private:

 HOOKPROC    pCallback;
 HHOOK          hhook;
 HINSTANCE      hinst;
 DWORD          dwThreadId;
 BOOL           fRecursive;

 static	        HkSysMsgFilter  *hooks;
  	        HkSysMsgFilter  *next;

 friend static LRESULT EXPENTRY_O32 OS2HkSysMsgFilterProc(int code, WPARAM wParam, LPARAM lParam);
};


#endif
