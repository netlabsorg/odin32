/* $Id: wndclass.cpp,v 1.3 1999-09-15 23:20:41 sandervl Exp $ */

/*
 * Win32 Window Class Managment Code for OS/2
 *
 * 18-07-1998 SvL: Merged all class code into this file + bug fixes 
 *                 Register all system classes at dll init
 *                 Rewrote incorrect callback code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <misc.h>
#include <wndproc.h>
#include <wndclass.h>
#include <nameid.h>
#include <spy.h>
#include <wprocess.h>
#include "hooks.h"
#include "wndmsg.h"

//default window handlers that are registered by RegisterClass are called
//in this callback handler
LRESULT EXPENTRY_O32 OS2ToWinCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

DWORD MapOEMToRealKey(DWORD wParam, DWORD lParam);

WNDPROC_O32 ButtonHandler = 0;
WNDPROC_O32 ListboxHandler = 0;
WNDPROC_O32 ComboboxHandler = 0;
WNDPROC_O32 EditHandler = 0;
WNDPROC_O32 MdiClientHandler = 0;
WNDPROC_O32 ScrollbarHandler = 0;
WNDPROC_O32 StaticHandler = 0;

HWND hwndButton = 0;
HWND hwndListbox = 0;
HWND hwndEdit = 0;
HWND hwndCombobox = 0;
HWND hwndScrollbar = 0;
HWND hwndMdiClient = 0;
HWND hwndStatic = 0;

Win32WindowClass *ButtonClass = 0;
Win32WindowClass *ListboxClass = 0;
Win32WindowClass *EditClass = 0;
Win32WindowClass *ComboboxClass = 0;
Win32WindowClass *MdiClientClass = 0;
Win32WindowClass *ScrollbarClass = 0;
Win32WindowClass *StaticClass = 0;
//******************************************************************************
//******************************************************************************
LRESULT WIN32API ButtonCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return ButtonHandler(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API ListboxCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return ListboxHandler(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API ComboboxCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return ComboboxHandler(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API MdiClientCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return MdiClientHandler(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API EditCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return EditHandler(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API ScrollbarCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return ScrollbarHandler(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API StaticCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return StaticHandler(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//SvL: 18-7-'98, Registers system window classes (button, listbox etc etc)
//******************************************************************************
void RegisterSystemClasses(ULONG hModule)
{
 WNDCLASSA wndclass;
 HWND hwnd;

   if(O32_GetClassInfo(NULL, "BUTTON", &wndclass)) {
	dprintf(("Create BUTTON System class"));
	ButtonClass = new Win32WindowClass(ButtonCallback, "BUTTON", hModule);
	ButtonHandler = (WNDPROC_O32)wndclass.lpfnWndProc;
	hwndButton = O32_CreateWindow("BUTTON", "BUTTON", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if(hwndButton == 0) {
		dprintf(("RegisterSystemClasses failed!!"));
	}
	O32_SetClassLong(hwndButton, GCL_WNDPROC, (LONG)Win32WindowClass::GetOS2ClassCallback());
   }
   if(O32_GetClassInfo(NULL, "LISTBOX", &wndclass)) {
	dprintf(("Create LISTBOX System class"));
	ListboxClass = new Win32WindowClass(ListboxCallback, "LISTBOX", hModule);
	ListboxHandler = (WNDPROC_O32)wndclass.lpfnWndProc;
	hwndListbox = O32_CreateWindow("LISTBOX", "LISTBOX", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if(hwndListbox == 0) {
		dprintf(("RegisterSystemClasses failed!!"));
	}
	O32_SetClassLong(hwndListbox, GCL_WNDPROC, (LONG)Win32WindowClass::GetOS2ClassCallback());
   }
   if(O32_GetClassInfo(NULL, "COMBOBOX", &wndclass)) {
	dprintf(("Create COMBOBOX System class"));
	ComboboxClass = new Win32WindowClass(ComboboxCallback, "COMBOBOX", hModule);
	ComboboxHandler = (WNDPROC_O32)wndclass.lpfnWndProc;
	hwndCombobox = O32_CreateWindow("COMBOBOX", "COMBOBOX", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if(hwndCombobox == 0) {
		dprintf(("RegisterSystemClasses failed!!"));
	}
	O32_SetClassLong(hwndCombobox, GCL_WNDPROC, (LONG)Win32WindowClass::GetOS2ClassCallback());
   }
   if(O32_GetClassInfo(NULL, "EDIT", &wndclass)) {
	dprintf(("Create EDIT System class"));
	EditClass = new Win32WindowClass(EditCallback, "EDIT", hModule);
	EditHandler = (WNDPROC_O32)wndclass.lpfnWndProc;
	hwndEdit = O32_CreateWindow("EDIT", "EDIT", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if(hwndEdit == 0) {
		dprintf(("RegisterSystemClasses failed!!"));
	}
	O32_SetClassLong(hwndEdit, GCL_WNDPROC, (LONG)Win32WindowClass::GetOS2ClassCallback());
   }
   //TODO: This doens't work yet!! (need to create a normal window as parent)
   if(O32_GetClassInfo(NULL, "MDICLIENT", &wndclass)) {
	dprintf(("Create MDICLIENT System class"));
	MdiClientClass = new Win32WindowClass(MdiClientCallback, "MDICLIENT", hModule);
	MdiClientHandler = (WNDPROC_O32)wndclass.lpfnWndProc;
	hwndMdiClient = O32_CreateWindow("MDICLIENT", "MDICLIENT", WS_CHILD, 0, 0, 0, 0, hwndListbox, 0, 0, 0);
	if(hwndMdiClient == 0) {
		dprintf(("RegisterSystemClasses failed!!"));
	}
	O32_SetClassLong(hwndMdiClient, GCL_WNDPROC, (LONG)Win32WindowClass::GetOS2ClassCallback());
   }
   if(O32_GetClassInfo(NULL, "SCROLLBAR", &wndclass)) {
	dprintf(("Create SCROLLBAR System class"));
	ScrollbarClass = new Win32WindowClass(ScrollbarCallback, "SCROLLBAR", hModule);
	ScrollbarHandler = (WNDPROC_O32)wndclass.lpfnWndProc;
	hwndScrollbar = O32_CreateWindow("SCROLLBAR", "SCROLLBAR", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if(hwndScrollbar == 0) {
		dprintf(("RegisterSystemClasses failed!!"));
	}
	O32_SetClassLong(hwndScrollbar, GCL_WNDPROC, (LONG)Win32WindowClass::GetOS2ClassCallback());
   }
   if(O32_GetClassInfo(NULL, "STATIC", &wndclass)) {
	dprintf(("Create STATIC System class"));
	StaticClass = new Win32WindowClass(StaticCallback, "STATIC", hModule);
	StaticHandler = (WNDPROC_O32)wndclass.lpfnWndProc;
	hwndStatic = O32_CreateWindow("STATIC", "STATIC", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if(hwndStatic == 0) {
		dprintf(("RegisterSystemClasses failed!!"));
	}
	O32_SetClassLong(hwndStatic, GCL_WNDPROC, (LONG)Win32WindowClass::GetOS2ClassCallback());
   }
//TODO: More standard classes in win95/NT4?
}
//******************************************************************************
//******************************************************************************
void UnregisterSystemClasses()
{
   dprintf(("KERNEL32: UnregisterSystemClasses"));
   if(hwndButton)     O32_DestroyWindow(hwndButton);
   if(hwndListbox)    O32_DestroyWindow(hwndListbox);
   if(hwndCombobox)   O32_DestroyWindow(hwndCombobox);
   if(hwndMdiClient)  O32_DestroyWindow(hwndMdiClient);
   if(hwndEdit)       O32_DestroyWindow(hwndEdit);
   if(hwndScrollbar)  O32_DestroyWindow(hwndScrollbar);
   if(hwndStatic)     O32_DestroyWindow(hwndStatic);
   if(ButtonClass)    delete ButtonClass;
   if(ListboxClass)   delete ListboxClass;
   if(EditClass)      delete EditClass;
   if(ComboboxClass)  delete ComboboxClass;
   if(MdiClientClass) delete MdiClientClass;
   if(ScrollbarClass) delete ScrollbarClass;
   if(StaticClass)    delete StaticClass;
   ButtonClass    = NULL;
   EditClass      = NULL;
   ListboxClass   = NULL;
   ComboboxClass  = NULL;
   MdiClientClass = NULL;
   ScrollbarClass = NULL;
   StaticClass    = NULL;
}
//******************************************************************************
//SvL: 18-7-'98: Moved here from user32.cpp
//******************************************************************************
ATOM WIN32API RegisterClassA(const WNDCLASSA *lpWndClass)
{
 ATOM rc;
 WNDCLASSA wc;

#ifdef DEBUG
    WriteLog("USER32:  RegisterClassA\n");
    WriteLog("USER32:  lpWndClass->style %X\n", lpWndClass->style);
    WriteLog("USER32:  lpWndClass->lpfnWndProc %X\n", lpWndClass->lpfnWndProc);
    WriteLog("USER32:  lpWndClass->cbClsExtra %X\n", lpWndClass->cbClsExtra);
    WriteLog("USER32:  lpWndClass->cbWndExtra %X\n", lpWndClass->cbWndExtra);
    WriteLog("USER32:  lpWndClass->hInstance %X\n", lpWndClass->hInstance);
    WriteLog("USER32:  lpWndClass->hIcon %X\n", lpWndClass->hIcon);
    WriteLog("USER32:  lpWndClass->hCursor %X\n", lpWndClass->hCursor);
    WriteLog("USER32:  lpWndClass->hbrBackground %X\n", lpWndClass->hbrBackground);
    WriteLog("USER32:  lpWndClass->lpszMenuName %X\n", lpWndClass->lpszMenuName);
    if((int)lpWndClass->lpszClassName >> 16 == 0)
	 WriteLog("USER32:  lpWndClass->lpszClassName %X\n", lpWndClass->lpszClassName);
    else WriteLog("USER32:  lpWndClass->lpszClassName %s\n", lpWndClass->lpszClassName);
#endif
    memcpy(&wc, lpWndClass, sizeof(WNDCLASSA));

    if((int)wc.lpszMenuName >> 16 != 0) {//convert string name identifier to numeric id
        dprintf(("USER32:  lpszMenuName %s\n", wc.lpszMenuName));
	*(int *)&wc.lpszMenuName = ConvertNameId(0, (char *)wc.lpszMenuName);
 	dprintf(("USER32:  Menu id = %d\n", (int)wc.lpszMenuName));
    }
#ifdef DEBUG
    if(wc.style & (CS_PARENTDC | CS_CLASSDC)) {
	dprintf(("USER32: Class uses an unsupported style!!!\n"));
    }
#endif
    //These are not supported by Open32
    wc.style &= ~(CS_PARENTDC | CS_CLASSDC);

    wc.lpfnWndProc = (WNDPROC)Win32WindowClass::GetOS2ClassCallback();
    rc = O32_RegisterClass(&wc);
    if(rc) {
	Win32WindowClass *wndclass = new Win32WindowClass((WNDPROC)lpWndClass->lpfnWndProc, (LPSTR)wc.lpszClassName, wc.hInstance);
    }
    dprintf(("USER32:  RegisterClass returned %d (%d)\n", rc, GetLastError()));
    return(rc);
}
//******************************************************************************
//SvL: 18-7-'98: Moved here from user32.cpp
//******************************************************************************
ATOM WIN32API RegisterClassExA(const WNDCLASSEXA *lpWndClass)
{
 ATOM rc;
 WNDCLASSEXA wc;

#ifdef DEBUG
    WriteLog("USER32:  lpWndClass->cbSize %X\n", lpWndClass->cbSize);
    WriteLog("USER32:  lpWndClass->style %X\n", lpWndClass->style);
    WriteLog("USER32:  lpWndClass->lpfnWndProc %X\n", lpWndClass->lpfnWndProc);
    WriteLog("USER32:  lpWndClass->cbClsExtra %X\n", lpWndClass->cbClsExtra);
    WriteLog("USER32:  lpWndClass->cbWndExtra %X\n", lpWndClass->cbWndExtra);
    WriteLog("USER32:  lpWndClass->hInstance %X\n", lpWndClass->hInstance);
    WriteLog("USER32:  lpWndClass->hIcon %X\n", lpWndClass->hIcon);
    WriteLog("USER32:  lpWndClass->hCursor %X\n", lpWndClass->hCursor);
    WriteLog("USER32:  lpWndClass->hbrBackground %X\n", lpWndClass->hbrBackground);
    WriteLog("USER32:  lpWndClass->lpszMenuName %X\n", lpWndClass->lpszMenuName);
    if((int)lpWndClass->lpszClassName >> 16 == 0)
	 WriteLog("USER32:  lpWndClass->lpszClassName %X\n", lpWndClass->lpszClassName);
    else WriteLog("USER32:  lpWndClass->lpszClassName %s\n", lpWndClass->lpszClassName);
#endif
    memcpy(&wc, lpWndClass, sizeof(WNDCLASSEXA));

    if((int)wc.lpszMenuName >> 16 != 0) {//convert string name identifier to numeric id
        dprintf(("USER32:  lpszMenuName %s\n", wc.lpszMenuName));
	*(int *)&wc.lpszMenuName = ConvertNameId(0, (char *)wc.lpszMenuName);
	dprintf(("USER32:  Menu id = %d\n", (int)wc.lpszMenuName));
    }
    if(wc.cbSize != sizeof(WNDCLASSEXA))
	return(0);

#ifdef DEBUG
    if(wc.style & (CS_PARENTDC | CS_CLASSDC)) {
	dprintf(("USER32: Class uses an unsupported style!!!\n"));
    }
#endif
    //These are not supported by Open32
    wc.style &= ~(CS_PARENTDC | CS_CLASSDC);
   
    wc.lpfnWndProc = (WNDPROC)Win32WindowClass::GetOS2ClassCallback();

    rc = O32_RegisterClass((WNDCLASSA *)&wc.style);
    if(rc) {
	Win32WindowClass *wndclass = new Win32WindowClass((WNDPROC)lpWndClass->lpfnWndProc, (LPSTR)wc.lpszClassName, wc.hInstance);
    }
#ifdef DEBUG
    WriteLog("USER32:  RegisterClassExA, not completely supported, returned %d\n", rc);
    if(rc == 0)
	WriteLog("USER32:  GetLastError returned %X\n", GetLastError());
	
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API RegisterClassW(const WNDCLASSW *lpwc)
{
 ATOM rc;
 WNDCLASSA wclass;

    dprintf(("RegisterClassW\n"));
    memcpy(&wclass, lpwc, sizeof(WNDCLASSA));
    if(wclass.lpszMenuName && ((int)wclass.lpszMenuName >> 16 != 0)) {
	wclass.lpszMenuName = UnicodeToAsciiString((LPWSTR)lpwc->lpszMenuName);
    }
    if(wclass.lpszClassName && ((int)wclass.lpszClassName >> 16 != 0)) {
	wclass.lpszClassName = UnicodeToAsciiString((LPWSTR)lpwc->lpszClassName);
    }

    rc = RegisterClassA(&wclass);

    //TODO: Assuming RegisterClass doesn't mess up our structure
    if(lpwc->lpszMenuName && ((int)lpwc->lpszMenuName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszMenuName);
    }
    if(lpwc->lpszClassName && ((int)lpwc->lpszClassName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszClassName);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API RegisterClassExW(CONST WNDCLASSEXW *lpwc)
{
 ATOM rc;
 WNDCLASSEXA wclass;

    dprintf(("RegisterClassExW\n"));
    memcpy(&wclass, lpwc, sizeof(WNDCLASSEXA));
    if(wclass.lpszMenuName && ((int)wclass.lpszMenuName >> 16 != 0)) {
	wclass.lpszMenuName = UnicodeToAsciiString((LPWSTR)lpwc->lpszMenuName);
    }
    if(wclass.lpszClassName && ((int)wclass.lpszClassName >> 16 != 0)) {
	wclass.lpszClassName = UnicodeToAsciiString((LPWSTR)lpwc->lpszClassName);
    }

    rc = RegisterClassExA(&wclass);

    //TODO: Assuming RegisterClassEx doesn't mess up our structure
    if(lpwc->lpszMenuName && ((int)lpwc->lpszMenuName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszMenuName);
    }
    if(lpwc->lpszClassName && ((int)lpwc->lpszClassName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszClassName);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterClassA(LPCSTR lpszClassName, HINSTANCE hinst)
{
 BOOL rc;

    Win32WindowClass::UnregisterClass(hinst, (LPSTR)lpszClassName);
    rc = O32_UnregisterClass(lpszClassName, hinst);
    dprintf(("USER32:  OS2UnregisterClassA returned %d\n", rc));

    //Spintest returns FALSE in dll termination, so pretend it succeeded
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterClassW(LPCWSTR lpszClassName, HINSTANCE hinst)
{
 char *astring = NULL;
 BOOL  rc;

    dprintf(("USER32:  OS2UnregisterClassW\n"));
    if((int)lpszClassName >> 16 != 0) {
	  astring = UnicodeToAsciiString((LPWSTR)lpszClassName);
    }
    else  astring = (char *)lpszClassName;

    Win32WindowClass::UnregisterClass(hinst, (LPSTR)astring);
    rc = O32_UnregisterClass(astring, hinst);
    if((int)astring >> 16 != 0) {
	FreeAsciiString(astring);
    }
    //Spintest returns FALSE in dll termination, so pretend it succeeded
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClassInfoA(HINSTANCE  arg1, LPCSTR arg2, WNDCLASSA * arg3)
{
 BOOL rc;

    dprintf(("USER32:  OS2GetClassInfoA\n"));
    rc = O32_GetClassInfo(arg1, arg2, (WNDCLASSA *)arg3);
    if(rc == TRUE) {
	arg3->lpfnWndProc = (WNDPROC)Win32WindowClass::GetClassCallback((LPSTR)arg2);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClassInfoW(HINSTANCE  hinst,
                            LPCWSTR    lpszClass,
                            WNDCLASSW *lpwc)
{
 WNDCLASSA wclass;
 BOOL      rc;
 char     *szClass = NULL;

    dprintf(("USER32: OS2GetClassInfoW\n"));
    if((int)lpszClass >> 16 != 0) {
	szClass = UnicodeToAsciiString((LPWSTR)lpszClass);
	dprintf(("USER32: OS2GetClassInfoW %s\n", szClass));
    }
    else szClass = (char *)lpszClass;

    rc = GetClassInfoA(hinst, szClass, &wclass);
    if(rc == TRUE) {
	memcpy(lpwc, &wclass, sizeof(WNDCLASSA));
	//TODO: Memory leak (create class object at RegisterClass and delete it at UnregisterClass)
	if(lpwc->lpszMenuName && ((int)lpwc->lpszMenuName >> 16 != 0)) {
		lpwc->lpszMenuName = (LPCWSTR)malloc(strlen(wclass.lpszMenuName)*sizeof(WCHAR)+2);
		AsciiToUnicode((char *)wclass.lpszMenuName, (LPWSTR)lpwc->lpszMenuName);
	}
	if(lpwc->lpszClassName && ((int)lpwc->lpszClassName >> 16 != 0)) {
		lpwc->lpszClassName = (LPCWSTR)malloc(strlen(wclass.lpszClassName)*sizeof(WCHAR)+2);
		AsciiToUnicode((char *)wclass.lpszClassName, (LPWSTR)lpwc->lpszClassName);
	}
    }
    dprintf(("USER32:  OS2GetClassInfoW returned %d\n", rc));
    return(rc);
}
/****************************************************************************
 * Name      : BOOL WIN32API GetClassInfoExA
 * Purpose   : The GetClassInfoEx function retrieves information about a window
 *             class, including the handle of the small icon associated with the
 *             window class. GetClassInfo does not retrieve the handle of the small icon.
 * Parameters: HINSTANCE    hinst     handle of application instance
 *             LPCTSTR      lpszClass address of class name string
 *             LPWNDCLASSEX lpwcx     address of structure for class data
 * Variables :
 * Result    : If the function finds a matching class and successfully copies
 *               the data, the return value is TRUE;
 *             otherwise, it is FALSE.
 *             To get extended error information, call GetLastError.
 * Remark    : PH: does not obtain handle of the small icon
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API GetClassInfoExA(HINSTANCE     hInstance,
                                 LPCTSTR       lpszClass,
                                 LPWNDCLASSEXA lpwcx)
{
  WNDCLASSA WndClass;                   /* structure for data transformation */
  BOOL      bResult;                  /* result of subsequent function calls */
  
  dprintf(("USER32:GetClassInfoExA (%08xh,%s,%08x).\n",
         hInstance,
         lpszClass,
         lpwcx));
  
  /* convert the structures */
  memcpy((PVOID)&WndClass,
         (PVOID)&lpwcx->style,
         sizeof(WndClass) );
  
  bResult = GetClassInfoA(hInstance,
                          lpszClass,
                          (WNDCLASSA *)&WndClass);
  if (bResult == TRUE)
  {
    /* convert data back to original structure */
    memcpy((PVOID)&lpwcx->style,
           (PVOID)&WndClass,
           sizeof(WndClass) );
    lpwcx->cbSize  = sizeof(WNDCLASSEXA);
    lpwcx->hIconSm = 0;
  }
  
  return (bResult);
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetClassInfoExW
 * Purpose   : The GetClassInfoEx function retrieves information about a window
 *             class, including the handle of the small icon associated with the
 *             window class. GetClassInfo does not retrieve the handle of the small icon.
 * Parameters: HINSTANCE    hinst     handle of application instance
 *             LPCWSTR      lpszClass address of class name string
 *             LPWNDCLASSEX lpwcx     address of structure for class data
 * Variables :
 * Result    : If the function finds a matching class and successfully copies
 *               the data, the return value is TRUE;
 *             otherwise, it is FALSE.
 *             To get extended error information, call GetLastError.
 * Remark    : does not obtain handle of the small icon
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API GetClassInfoExW(HINSTANCE     hInstance,
                                 LPCWSTR       lpszClass,
                                 LPWNDCLASSEXW lpwcx)
{
  WNDCLASSW WndClass;                   /* structure for data transformation */
  BOOL      bResult;                  /* result of subsequent function calls */
  
  dprintf(("USER32:GetClassInfoExW (%08xh,%s,%08x).\n",
         hInstance,
         lpszClass,
         lpwcx));
  
  /* convert the structures */
  memcpy((PVOID)&WndClass,
         (PVOID)&lpwcx->style,
         sizeof(WndClass) );
  
  bResult = GetClassInfoW(hInstance,
                          (LPWSTR)lpszClass,
                          (WNDCLASSW *)&WndClass);
  if (bResult == TRUE)
  {
    /* convert data back to original structure */
    memcpy((PVOID)&lpwcx->style,
           (PVOID)&WndClass,
           sizeof(WndClass) );
    lpwcx->cbSize  = sizeof(WNDCLASSEXW);
    lpwcx->hIconSm = 0;
  }
  
  return (bResult);
}
//******************************************************************************
//TODO: Not complete (unsupported extension in WNDCLASSEXA/W, unsupported styles etc)
//******************************************************************************
LONG WIN32API GetClassLongA(HWND hwnd, int nIndex)
{
 DWORD rc;

    dprintf(("USER32: OS2GetClassLongA\n"));
    rc = O32_GetClassLong(hwnd, nIndex);
    if(nIndex == GCL_WNDPROC) 
    {
      char                  szClass[128];
      Win32WindowClass     *wclass;

	if(GetClassNameA(hwnd, szClass, sizeof(szClass))) {
	        wclass = Win32WindowClass::FindClass(szClass);
	        if(wclass) {
			return (LONG)wclass->GetWinCallback();
		}
	}
	dprintf(("GetClassLongA, class of window %X not found\n", hwnd));
	return 0;	 //TODO: set last error
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetClassLongW(HWND hwnd, int nIndex)
{
    dprintf(("USER32:  OS2GetClassLongW\n"));
    if(nIndex == GCL_MENUNAME) { //TODO
	dprintf(("USER32: Class Menu name not implemented yet\n"));
    }
    // NOTE: This will not work as is (needs UNICODE support)
    return GetClassLongA(hwnd, nIndex);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClassNameA( HWND arg1, LPSTR arg2, int  arg3)
{
    dprintf(("USER32:  OS2GetClassNameA\n"));
    return O32_GetClassName(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClassNameW(HWND hwnd, LPWSTR lpClassName, int nMaxCount)
{
 char *astring = (char *)malloc(nMaxCount);
 int   rc;

    dprintf(("USER32:  OS2GetClassNameW\n"));
    rc = GetClassNameA(hwnd, astring, nMaxCount);
    AsciiToUnicode(astring, lpClassName);
    free(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API GetClassWord( HWND arg1, int  arg2)
{
    dprintf(("USER32: OS2GetClassWord\n"));
    return O32_GetClassWord(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetClassLongA(HWND hwnd, int nIndex, LONG lNewVal)
{
 DWORD rc;

    dprintf(("USER32: OS2SetClassLongA\n"));
    if(nIndex == GCL_WNDPROC) {
      char                  szClass[128];
      Win32WindowClass     *wclass;

	if(GetClassNameA(hwnd, szClass, sizeof(szClass))) {
	        wclass = Win32WindowClass::FindClass(szClass);
	        if(wclass) {
			rc = (DWORD)wclass->GetWinCallback();
			wclass->SetWinCallback((WNDPROC)lNewVal);
			return rc;
		}
	}
	dprintf(("SetClassLongA, class of window %X not found\n", hwnd));
	return 0;	 //TODO: set last error
    }
    return O32_SetClassLong(hwnd, nIndex, lNewVal);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetClassLongW( HWND arg1, int arg2, LONG  arg3)
{
    dprintf(("USER32: OS2SetClassLongW\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_SetClassLong(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API SetClassWord( HWND arg1, int arg2, WORD  arg3)
{
    dprintf(("USER32: OS2SetClassWord\n"));
    return O32_SetClassWord(arg1, arg2, arg3);
}
//******************************************************************************
//Win32WindowClass methods:
//******************************************************************************
Win32WindowClass::Win32WindowClass(WNDPROC pUserCallback, LPSTR id, HINSTANCE hinst)
{
  //Insert it in front of the rest
  next        = wndclasses;
  wndclasses  = this;
  if((int)id >> 16 != 0) {
	strcpy(className, id);
	classAtom    = 0;
  }
  else {
	className[0] = 0;
	classAtom    = (DWORD)id;
  }
  this->hinst   = hinst;

  pWinCallback  = pUserCallback;
  dprintf(("Win32WindowClass::Win32WindowClass %d\n", id));
}
//******************************************************************************
//******************************************************************************
Win32WindowClass::~Win32WindowClass()
{
  Win32WindowClass *wndclass = Win32WindowClass::wndclasses;

  if(wndclass == this) {
	wndclasses = next;
  }
  else {
	while(wndclass->next != NULL) {
		if(wndclass->next == this) {
			wndclass->next = next;
			break;
		}
		wndclass = wndclass->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
Win32WindowClass *Win32WindowClass::FindClass(LPSTR id)
{
  Win32WindowClass *wndclass = wndclasses;

  if(wndclass == NULL)	return(NULL);

  if((int)id >> 16 != 0) {
	if(stricmp(wndclass->className, id) == 0) {
		return(wndclass);
	}
	else {
		wndclass = wndclass->next;
		while(wndclass != NULL) {
			if(stricmp(wndclass->className, id) == 0) {
				return(wndclass);
			}
			wndclass = wndclass->next;
	        }
	}
  }
  else {
	if(wndclass->classAtom == (DWORD)id) {
		return(wndclass);
	}
	else {
		wndclass = wndclass->next;
		while(wndclass != NULL) {
			if(wndclass->classAtom == (DWORD)id) {
				return(wndclass);
			}
			wndclass = wndclass->next;
	        }
	}
  }
  return(NULL);
}
//******************************************************************************
//******************************************************************************
void Win32WindowClass::UnregisterClass(HINSTANCE hinst, LPSTR id)
{
  Win32WindowClass *wndclass;

  dprintf(("::UnregisterClass, destroy class %X!!\n", id));
  wndclass = FindClass(id);
  if(wndclass && wndclass->hinst == hinst) {
	delete wndclass;
	return;
  }
  dprintf(("::UnregisterClass, couldn't find class %X!!\n", id));
}
//******************************************************************************
//******************************************************************************
WNDPROC Win32WindowClass::GetClassCallback(HINSTANCE hinst, LPSTR id)
{
  Win32WindowClass *wndclass;

  wndclass = FindClass(id);
  if(wndclass && wndclass->hinst == hinst) {
	return wndclass->pWinCallback;
  }
  dprintf(("::GetClassCallback, couldn't find class %X!!\n", id));
  return(NULL);
}
//******************************************************************************
//******************************************************************************
WNDPROC Win32WindowClass::GetClassCallback(LPSTR id)
{
  Win32WindowClass *wndclass;

  wndclass = FindClass(id);
  if(wndclass) {
	return wndclass->pWinCallback;
  }
  dprintf(("::GetClassCallback2, couldn't find class %X!!\n", id));
  return(NULL);
}
//******************************************************************************
//******************************************************************************
WNDPROC_O32 Win32WindowClass::GetOS2ClassCallback()
{
  return OS2ToWinCallback;
}
//******************************************************************************
//******************************************************************************
LRESULT EXPENTRY_O32 OS2ToWinCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
 char                  szClass[128];
 Win32WindowClass     *wclass;
 LRESULT               rc;
 DWORD                 dwStyle, dwExStyle;
 HWND                  parentHwnd;
 Win32WindowProc      *window;

  //Restore our FS selector
  SetWin32TIB();

  if(Msg == WM_MOUSEACTIVATE) 
  {
	//Open32 sends an OS/2 window message for a button click
	if(HIWORD(lParam) == 0x71)  //WM_BUTTONCLICKFIRST
	{
		lParam = (WM_LBUTTONDOWN << 16) | LOWORD(lParam);
	}
  }
  if(PostSpyMessage(hwnd, Msg, wParam, lParam) == FALSE)
	dprintf(("OS2ToWinCallback %s for %x %x %x", GetMsgText(Msg), hwnd, wParam, lParam));

  if(HkCBT::OS2HkCBTProc(hwnd, Msg, wParam, lParam) == TRUE) {//hook swallowed msg
	RestoreOS2TIB();
        return(0);
  }

  if(O32_GetClassName(hwnd, szClass, sizeof(szClass))) {
        wclass = Win32WindowClass::FindClass(szClass);
        if(wclass) {
		switch(Msg) 
		{
		case WM_CREATE://Open32 isn't sending WM_NCCREATE messages!!
		      	window = Win32WindowProc::FindProc(hwnd, GetCurrentThreadId());
			if(window) {
				dprintf(("OS2ToWinCallback (class): New window object!"));
				window->SetWindowHandle(hwnd);
    			}
		
                        if(wclass->GetWinCallback()(hwnd, WM_NCCREATE, 0, lParam) == 0) {
                                dprintf(("WM_NCCREATE returned FALSE\n"));
                                return(-1); //don't create window
                        }
			//Send WM_CREATE message before notifying parent
			rc = wclass->GetWinCallback()(hwnd, Msg, wParam, lParam);

			NotifyParent(hwnd, WM_CREATE, wParam, lParam);
			RestoreOS2TIB();
			return(rc);

		case WM_DESTROY: //nofity parent
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			NotifyParent(hwnd, Msg, wParam, lParam);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:	//SvL: Correct Open32 key mapping bug
			//TODO: Not good enough, look at Wine
                        lParam = MapOEMToRealKey(wParam, lParam);
			break;

		case WM_ACTIVATE:
                  	if(LOWORD(wParam) != WA_INACTIVE)
                  	{
                   	  //EB: I think the problem is not a missing wm_erasebkgnd.
                   	  //Maybe some wrong flags in open32 during async repainting.
                   	  RECT rect;
                   	  HRGN hrgn;
                   	  HDC hdc = GetDC(hwnd);

                   		// erase the dirty rect
                   		GetUpdateRect(hwnd, &rect, TRUE);
                   		hrgn = CreateRectRgnIndirect(&rect);
                   		SelectClipRgn (hdc, hrgn);
                   		DeleteObject (hrgn);
                   		wclass->GetWinCallback()(hwnd, WM_ERASEBKGND, hdc, (LPARAM)&rect);
                   		SelectClipRgn (hdc, NULL);
                   		ReleaseDC(hwnd, hdc);
                  	}
			break;
		}
		rc = wclass->GetWinCallback()(hwnd, Msg, wParam, lParam);
		RestoreOS2TIB();
		return rc;
	}
  }
  dprintf(("OS2ToWinCallback: couldn't find class procedure of window %X\n", hwnd));
  RestoreOS2TIB();
  return(0);
}
//******************************************************************************
//******************************************************************************
Win32WindowClass *Win32WindowClass::wndclasses = NULL;
