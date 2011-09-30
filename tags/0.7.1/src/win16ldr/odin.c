#ifdef WATCOMC
#include <windows.h>
#else
#include "msc60win.h"
#endif
//#include <ddeml.h>
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

typedef void (FAR * PASCAL FUNC_SetPELdr)(LPCSTR);

#ifdef DDE_COMM
HDDEDATA CALLBACK MyDdeCallback(UINT wType, UINT wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2);

DWORD idInst = 0;
HSZ   hszServer = 0, hszTopic = 0;

#define MSG_DDE_WILDCONNECT	WM_USER+1
#define MSG_DDE_CONNECT		WM_USER+2
#endif

//*****************************************************************************************
//*****************************************************************************************
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
	FUNC_SetPELdr pfnSetPELdr;
	char  szPEPath[260];
	LPSTR tmp;
			               
	hDll = LoadLibrary("odindll.dll");
	if(hDll == 0) {
		return 0;
	}

	pfnSetPELdr = (FUNC_SetPELdr)GetProcAddress(hDll, "SETPELDR");
	if(pfnSetPELdr == NULL) {
		return 0;
	}                 
	//check if the caller specified the path of the PE loader; if so, pass
	//it on to odindll
    tmp = strstr(lpCmdLine, "/PELDR=[");
    if(tmp) {
    	tmp += 8;	//skip /peldr=[
    	strcpy(szPEPath, tmp);
    	tmp = szPEPath;
    	
    	while(*tmp != ']' && *tmp != 0) tmp++;
    	
    	if(*tmp == ']') {
    		*tmp = 0;
    		pfnSetPELdr(szPEPath);
    	}               
    	
    	//now skip this parameter                
    	tmp = strstr(lpCmdLine, "/PELDR=[");
    	while(*tmp != ']' && *tmp != 0) tmp++;     
    	if(*tmp == ']') {
    		tmp++;
    	}
		lpCmdLine = tmp;    	
    }

    if (!hPrevInstance)
        if (!InitApplication(hInstance))
            return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

#ifdef DDE_COMM
    if(DdeInitialize(&idInst, MyDdeCallback, APPCLASS_STANDARD, 0) != DMLERR_NO_ERROR) {
        return FALSE;
    }
    hszServer = DdeCreateStringHandle(idInst, "Win16OdinServer", CP_WINANSI);
    if(hszServer == 0) {
        return FALSE;
    }
    hszTopic = DdeCreateStringHandle(idInst, "Win16Broadcast", CP_WINANSI);
    if(hszTopic == 0) {
        return FALSE;
    }
    if(DdeNameService(idInst, hszServer, 0, DNS_REGISTER) == 0) {
        return FALSE;
    }
#endif

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
		MessageBox(hwnd, errormsg, "Internal Error", MB_ICONHAND);
		DestroyWindow(hwnd);
	}
		
#ifdef DDE_COMM
    DdeNameService(idInst, hszServer, 0, DNS_UNREGISTER);		
    DdeFreeStringHandle(idInst, hszServer);
    DdeFreeStringHandle(idInst, hszTopic);
    DdeUninitialize(idInst);
#endif
	FreeLibrary(hDll);
    return (msg.wParam);
}
//*****************************************************************************************
//*****************************************************************************************
BOOL InitApplication(HANDLE hInstance)
{
	WNDCLASS wc;
	
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
#if 1 
		WS_POPUP,
#else        
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
#endif        
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

#ifdef DDE_COMM
        case MSG_DDE_WILDCONNECT:
            break;
        case MSG_DDE_CONNECT:
            MessageBox(0, "MyDdeCallback", "WILDCONNECT", MB_OK);
            break;
#endif
        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}
#ifdef DDE_COMM
//*****************************************************************************************
//*****************************************************************************************
HDDEDATA CALLBACK MyDdeCallback(UINT wType, UINT wFmt, HCONV hConv, 
                                HSZ hszTopic,  HSZ hszItem, 
                                HDDEDATA hData, DWORD dwData1, DWORD dwData2)
{
//    WORD i, j;
//    WORD cItems, iFmt;
//    HDDEDATA hDataRet;

    char msg[128];

////     MessageBeep(500);
//    sprintf(msg, "type=%x format=%x %x %x %x %x %x %x", wType, wFmt, hConv, hszTopic, hszItem, hData, dwData1, dwData2);
//    MessageBox(0, "MyDdeCallback", "test", MB_OK);

    if(wType == XTYP_EXECUTE && wFmt == CF_TEXT) 
    {
        char szExec[64];

        DdeGetData(hData, (LPBYTE)szExec, 64, 0);
        szExec[63] = 0;
        MessageBox(0, "MyDdeCallback", szExec, MB_OK);
        return (HDDEDATA)TRUE;
    }
    /*
     * Process wild initiates here
     */
    if (wType == XTYP_WILDCONNECT) {
        HSZ ahsz[(1 + 1) * 2];
        /*
         * He wants a hsz list of all our available app/topic pairs
         * that conform to hszTopic and hszItem(App).
         */

//        MessageBox(0, "MyDdeCallback", "WILDCONNECT", MB_OK);

//        if (!ValidateContext((PCONVCONTEXT)lData1)) {
//            return(FALSE);
//        }

        if (hszItem != hszServer && hszItem != 0) {
            // we only support the hszAppName service
            return(0);
        }

        ahsz[0] = hszServer;
        ahsz[1] = hszTopic;
        // cap off the list with 0s
        ahsz[2] = ahsz[3] = 0L;

        // send it back
        return(DdeCreateDataHandle(idInst, (LPBYTE)&ahsz[0], sizeof(HSZ) * 4, 0L, 0, wFmt, 0));
    }
    return 0;
}
//*****************************************************************************************
//*****************************************************************************************
#endif
