#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "peexe.h"

HINSTANCE hinstApp;
HWND hwnd;
HINSTANCE hDll;

long FAR PASCAL __export MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitApplication(HANDLE hInstance);
BOOL InitInstance(HANDLE hInstance, int nCmdShow);
HANDLE hModule = 0;


//*****************************************************************************************            
//*****************************************************************************************            
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    
    if (!hPrevInstance)                       
        if (!InitApplication(hInstance))
            return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while(*lpCmdLine == ' ') lpCmdLine++;

	hModule = WinExec(lpCmdLine, SW_SHOW);

	if(hModule >= 32) {
    	while (GetMessage(&msg, NULL, NULL, NULL)) 
    	{
        	TranslateMessage(&msg);
        	DispatchMessage(&msg);
    	}         
	}
	else {
		char errormsg[256];
		
		sprintf(errormsg, "WinExec %s failed with error %d", lpCmdLine, hModule);
		MessageBox(hwnd, errormsg, "Ïnternal Error", MB_ICONHAND);
		DestroyWindow(hwnd);
	}
		    
	FreeLibrary(hDll);
    return (msg.wParam);
}                 
//*****************************************************************************************            
//*****************************************************************************************            
BOOL InitApplication(HANDLE hInstance)
{                                 
	WNDCLASS wc;
	
	hDll = LoadLibrary("odindll.dll");
	if(hDll == 0) {
		return 0;
	}
	
    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;      
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "Win16OdinClass";

    return (RegisterClass(&wc));
}
//*****************************************************************************************            
//*****************************************************************************************            
BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
    hinstApp = hInstance;

    hwnd = CreateWindow(
        "Win16OdinClass",
        "Odin Win16 Program Launcher",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
        return (FALSE);
        
    return (TRUE);

}
//*****************************************************************************************            
//*****************************************************************************************            
long FAR PASCAL __export MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{           
	char modname[256];
	
    switch (message) {
    	case WM_CREATE:
    		SetTimer(hWnd, 1234, 500, NULL);
			return 0;
			
    	case WM_TIMER:
			if(GetModuleFileName(hModule, modname, sizeof(modname)) == 0) {
				KillTimer(hWnd, 1234);
				DestroyWindow(hWnd);
			}
    		break;
    		
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}
//*****************************************************************************************            
//*****************************************************************************************            
