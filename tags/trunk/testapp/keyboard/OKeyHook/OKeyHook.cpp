	// OKeyHook.cpp : Defines the entry point for the DLL application.
//

#include "OKeyHook.h"




#ifdef __cplusplus
  extern "C" {
#endif

//
// exported functions
//
#define OKEYTEST_IMPORT  __declspec(dllimport)

typedef (* OKEYTEST_IMPORT TpfnLogString)(LPSTR lpstrOrigin, LPSTR lpstrText);
typedef (* OKEYTEST_IMPORT TpfnLogMessage)(LPSTR lpstrOrigin, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// 
// module global functions
//
TpfnLogString pfnLogString = NULL;
TpfnLogMessage pfnLogMessage = NULL;
HHOOK     hhookStandard;                        // standard keyboard hook
HHOOK     hhookLowLevel;                        // lowlevel keyboard hook (NT4)
HOOKPROC  hpStandard;
HOOKPROC  hpLowLevel;
HMODULE   hHOOKDLL;

#ifndef WH_KEYBOARD_LL
#define WH_KEYBOARD_LL     13
#endif


//
// prototypes
//
void OKEYHOOK_API hook_Enable(BOOL fEnable);
LRESULT CALLBACK hook_Standard(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hook_LowLevel(INT nCode, WPARAM wParam, LPARAM lParam);



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			hHOOKDLL = (HMODULE)hModule;
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			hook_Enable(FALSE);
			break;
    }
    return TRUE;
}



//
// enable / disable hooks
//
void OKEYHOOK_API hook_Enable(BOOL fEnable)
{
	if (fEnable == TRUE)
	{
		HINSTANCE hInstExe = LoadLibrary("OKEYTEST.EXE");
		if (NULL == hInstExe)
			return;

		pfnLogString = (TpfnLogString)GetProcAddress(hInstExe, "logString");
		if (NULL == pfnLogString)
			return;

		pfnLogMessage = (TpfnLogMessage)GetProcAddress(hInstExe, "logMessage");
		if (NULL == pfnLogMessage)
			return;

		hpStandard = (HOOKPROC)&hook_Standard;
		hpLowLevel = (HOOKPROC)&hook_LowLevel;
		
		// enable the hooks
		if ( (NULL == hhookStandard) && (NULL != hpStandard) )
		{
			hhookStandard = SetWindowsHookEx(WH_KEYBOARD, hpStandard, hHOOKDLL, 0);
			if (NULL != hhookStandard)
				pfnLogString("OKEYHOOK", "Standard keyboard hook was installed");
		}

		if ( (hhookLowLevel == NULL) && (NULL != hpLowLevel) )
		{
			hhookLowLevel = SetWindowsHookEx(WH_KEYBOARD_LL, hpLowLevel, hHOOKDLL, 0);
			if (NULL != hhookLowLevel)
				pfnLogString("OKEYHOOK", "Low-Level keyboard hook was installed");
		}
	}
	else
	{
			if (NULL != hhookStandard)
			{
				if (FALSE == UnhookWindowsHookEx(hhookStandard))
					pfnLogString("OKEYHOOK", "Standard keyboard hook was not uninstalled");
				else
				{
					pfnLogString("OKEYHOOK", "Standard keyboard hook was uninstalled");
					hhookStandard = NULL;
				}
			}

			if (NULL != hhookLowLevel)
			{
				if (FALSE == UnhookWindowsHookEx(hhookLowLevel))
					pfnLogString("OKEYHOOK", "Low-Level keyboard hook was not uninstalled");
				else
				{
					pfnLogString("OKEYHOOK", "Low-Level keyboard hook was uninstalled");
					hhookLowLevel = NULL;
				}
			}
	}
}


//
// standard windows keyboard hook
//

LRESULT CALLBACK hook_Standard(int nCode, WPARAM wParam, LPARAM lParam)
{
	// get the extra info
	DWORD dwExtra = GetMessageExtraInfo();

	// log the message
	// log the message
	CHAR szBuf[512];
	wsprintf(szBuf, 
		"nCode=%08xh, wParam=%08xh, lParam=%08xh, extra=%08xh",
		nCode,
		wParam,
		lParam,
		dwExtra);

    pfnLogString("KeyHook", szBuf);


	// call next hook
	if (nCode >= 0)
		return CallNextHookEx(hhookStandard, nCode, wParam, lParam);
	else
		return 0;
}



typedef struct tagKBDLLHOOKSTRUCT {
    DWORD   vkCode;
    DWORD   scanCode;
    DWORD   flags;
    DWORD   time;
    DWORD   dwExtraInfo;
} KBDLLHOOKSTRUCT, *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;


// This is for NT only
LRESULT CALLBACK hook_LowLevel(INT nCode, WPARAM wParam, LPARAM lParam)
{
	// look into the structure
	// @@@PH
	LPKBDLLHOOKSTRUCT pkbhs = (LPKBDLLHOOKSTRUCT)lParam;
  
	// log the message
	CHAR szBuf[512];
	wsprintf(szBuf, 
		"nCode=%08xh wParam=%08xh vkCode=%04xh scancode=%04xh, flags=%04xh, extra=%08xh, time=%08xh",
		nCode,
		wParam,
		pkbhs->vkCode,
		pkbhs->scanCode,
		pkbhs->flags,
		pkbhs->dwExtraInfo,
		pkbhs->time);

    pfnLogString("LowLevel", szBuf);

  	// call next hook
	if (nCode >= 0)
		return CallNextHookEx(hhookLowLevel, nCode, wParam, lParam);
	else
		return 0;
}



#ifdef __cplusplus
  }
#endif

