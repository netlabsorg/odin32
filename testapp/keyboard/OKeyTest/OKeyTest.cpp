// OKeyTest.cpp : Defines the entry point for the application.
//


/**

To Do:
- enable standard keyboard hook
- enable WIN NT low-level hook

  */

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "okeytest.h"

#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100


// definitions
#define OKEYTEST_EXPORT __declspec(dllexport)
#define OKEYHOOK_API    __declspec(dllimport)


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR     szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR     szWindowClass[MAX_LOADSTRING];		// The title bar text
HWND      hwndListbox;                          // the listbox for all the logged messages
HINSTANCE hHOOKDLL;                             // instance for the hooking DLL

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#ifdef __cplusplus
extern "C" {
#endif


OKEYTEST_EXPORT void logString(LPSTR lpstrOrigin, LPSTR lpstrText);
OKEYTEST_EXPORT void logMessage(LPSTR lpstrOrigin, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef (* OKEYHOOK_API THOOK_ENABLE)(BOOL fEnable);
THOOK_ENABLE phook_Enable;
BOOL fHookEnabled = FALSE;


#ifdef __cplusplus
}
#endif




void i_hookEnable(BOOL fEnable)
{
	if (NULL == hHOOKDLL)
	{
		hHOOKDLL = LoadLibrary("OKEYHOOK.DLL");

		// Note: loading the DLL automatically installs the hooks
		if (NULL == hHOOKDLL)
		{
			logString("*", "OKEYHOOK.DLL not loaded");
			return;
		}
	}

	// get proc addr
	phook_Enable = (THOOK_ENABLE)GetProcAddress(hHOOKDLL, "hook_Enable");
	if (NULL == phook_Enable)
	{
		logString("*", "Can't get hook_Enable export");
		return;
	}

	phook_Enable(fEnable);
}



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OKEYTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_OKEYTEST);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_OKEYTEST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_OKEYTEST;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



//
// cheesy logging functions
// 

#define LOG_MAX_ENTRIES 100


OKEYTEST_EXPORT void logString(LPSTR lpstrOrigin, LPSTR lpstrText)
{
	CHAR szBuf[512];
	static int nMessage = 0;

	nMessage++;

	wsprintf(szBuf, "%06d|%5s|", 
		nMessage,
		lpstrOrigin);

	if (NULL != lpstrText)
		strcat(szBuf, lpstrText);

	SendMessage(hwndListbox, LB_ADDSTRING, (WPARAM)0, (LPARAM)szBuf);

	// check if we've got to delete the top entry?
	DWORD dwItems = SendMessage(hwndListbox, LB_GETCOUNT, 0, 0);
	if (dwItems > LOG_MAX_ENTRIES)
	{
		// delete the top entry
		SendMessage(hwndListbox, LB_DELETESTRING, 0, 0);
	}

	// scroll to the last visible entry
//	int rc = SendMessage(hwndListbox, LB_SETTOPINDEX, (WPARAM)dwItems - 1, (LPARAM)0);
//	if (rc == LB_ERR)
//		MessageBox(0, "err", "err", MB_OK);

	// simulate "END" key
	SendMessage(hwndListbox, WM_KEYDOWN, 0x23, 0x014f0001);
}


OKEYTEST_EXPORT void logMessage(LPSTR lpstrOrigin, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CHAR szBuf[512];
	LPSTR pszMsg;

	switch(msg)
	{
		case WM_CHAR:		pszMsg = "WM_CHAR"; break;
		case WM_KEYUP:		pszMsg = "WM_KEYUP"; break;
		case WM_KEYDOWN:	pszMsg = "WM_KEYDOWN"; break;
		case WM_SYSKEYUP:	pszMsg = "WM_SYSKEYUP"; break;
		case WM_SYSKEYDOWN:	pszMsg = "WM_SYSKEYDOWN"; break;
		default: pszMsg = "unknown"; break;
	}
	
	wsprintf(szBuf, "%08xh %16s (%08xh, %08xh)",
		hwnd,
		pszMsg,
		wParam,
		lParam);

	logString(lpstrOrigin, szBuf);
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_CREATE:
		{
			RECT rcl;				
			GetClientRect(hWnd, &rcl);

			// create a listbox that fills the main client window
			hwndListbox = CreateWindow("LISTBOX",
				"log",
				WS_CHILD | WS_VSCROLL | WS_VISIBLE | LBS_DISABLENOSCROLL | LBS_NOINTEGRALHEIGHT,
				0, 0,
				rcl.right - rcl.left,
				rcl.bottom - rcl.top,
				hWnd,
				0,
				hInst,
				NULL);

			break;
		}

		case WM_KILLFOCUS:
		{
			// prevent the listbox from gaining the focus
			HWND hwndNew = (HWND)wParam;
			if (hwndNew == hwndListbox)
				SetFocus(hWnd);	// skip this message
			break;
		}

		case WM_SIZE:
		{
			// resize the listbox also!
			RECT rcl;				
			GetClientRect(hWnd, &rcl);
			SetWindowPos(hwndListbox, 0, 0, 0,
				rcl.right - rcl.left,
				rcl.bottom - rcl.top,
				SWP_NOZORDER);
			break;
		}

		case WM_CHAR:
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			logMessage("Queue", hWnd, message, wParam, lParam);
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;

				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;

				case IDC_EDIT_HOOK:
				{
					if (fHookEnabled == TRUE)
					{
						// disable hooks
						logString("*", "Uninstalling hooks");
						i_hookEnable(FALSE);
						fHookEnabled = FALSE;
					}
					else
					{
						// enable hooks
						logString("*", "Installing hooks");
						i_hookEnable(TRUE);
						fHookEnabled = TRUE;
					}

					HMENU hMenu = GetMenu(hWnd);
					CheckMenuItem(hMenu,
						IDC_EDIT_HOOK,
						MF_BYCOMMAND |
						fHookEnabled ? MF_CHECKED : MF_UNCHECKED);

					break;
				}

				case IDC_EDIT_CLEAR:
					SendMessage(hwndListbox,
						LB_RESETCONTENT,
						0,
						0);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		
		case WM_DESTROY:
			if (NULL != hHOOKDLL)
			{
				i_hookEnable(FALSE);
				FreeLibrary(hHOOKDLL);
			}

			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
