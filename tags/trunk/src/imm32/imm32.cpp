/* $Id: imm32.cpp,v 1.1 1999-08-29 14:55:23 sandervl Exp $ */
/*
 *	IMM32 library
 *
 *	Copyright 1998	Patrik Stridvall
 *	Copyright 1999	Jens Wiessner
 */

#include <os2win.h>
#include <imm.h>
#include <debugdefs.h>
#include "winversion.h"
#include "imm32.h"
#include "debugtools.h"



DEFAULT_DEBUG_CHANNEL(imm)

/***********************************************************************
 *           ImmAssociateContext32 (IMM32.1)
 */
HIMC WINAPI ImmAssociateContext(HWND hWnd, HIMC hIMC)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmAssociateContextEx32 (IMM32.2)
 */
BOOL WINAPI ImmAssociateContextEx(HWND hWnd, HIMC hIMC, DWORD dword)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmConfigureIME32A (IMM32.3)
 */
BOOL WINAPI ImmConfigureIMEA(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/************** *********************************************************
 *           ImmConfigureIME32W (IMM32.4)
 */
BOOL WINAPI ImmConfigureIMEW(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmCreateContext32 (IMM32.5)
 */
HIMC WINAPI ImmCreateContext()
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmCreateIMCC32 (IMM32.6)
 */
HIMCC  WINAPI ImmCreateIMCC(DWORD dword)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmCreateSoftKeyboard32 (IMM32.7)
 */
HWND WINAPI ImmCreateSoftKeyboard(UINT uint, HWND hwnd, int in1, int in2)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmDestroyContext32 (IMM32.8)
 */
BOOL WINAPI ImmDestroyContext(HIMC hIMC)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmDestroyIMCC32 (IMM32.9)
 */
HIMCC  WINAPI ImmDestroyIMCC(HIMCC himcc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmDestroySoftKeyboard32 (IMM32.10)
 */
BOOL WINAPI ImmDestroySoftKeyboard(HWND hwnd)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           IMMDisableIME32 (IMM32.11)
 */
BOOL WINAPI ImmDisableIME(DWORD dword)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmEnumRegisterWord32A (IMM32.12)
 */
UINT WINAPI ImmEnumRegisterWordA( HKL hKL, REGISTERWORDENUMPROCA lpfnEnumProc,
  LPCSTR lpszReading, DWORD dwStyle,  LPCSTR lpszRegister, LPVOID lpData)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmEnumRegisterWord32W (IMM32.13)
 */
UINT WINAPI ImmEnumRegisterWordW(  HKL hKL, REGISTERWORDENUMPROCW lpfnEnumProc,
  LPCWSTR lpszReading, DWORD dwStyle,  LPCWSTR lpszRegister, LPVOID lpData)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmEscape32A (IMM32.14)
 */
LRESULT WINAPI ImmEscapeA(  HKL hKL, HIMC hIMC,   UINT uEscape, LPVOID lpData)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmEscape32W (IMM32.15)
 */
LRESULT WINAPI ImmEscapeW(  HKL hKL, HIMC hIMC,  UINT uEscape, LPVOID lpData)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGenerateMessage32 (IMM32.16)
 */
BOOL WINAPI ImmGenerateMessage(HIMC himc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetCandidateList32A (IMM32.17)
 */
DWORD WINAPI ImmGetCandidateListA(  HIMC hIMC, DWORD deIndex,   
		LPCANDIDATELIST lpCandList, DWORD dwBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateListCount32A (IMM32.18)
 */
DWORD WINAPI ImmGetCandidateListCountA( HIMC hIMC, LPDWORD lpdwListCount)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateListCount32W (IMM32.19)
 */
DWORD WINAPI ImmGetCandidateListCountW( HIMC hIMC, LPDWORD lpdwListCount)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateList32W (IMM32.20)
 */
DWORD WINAPI ImmGetCandidateListW(  HIMC hIMC, DWORD deIndex, 
  LPCANDIDATELIST lpCandList, DWORD dwBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateWindow32 (IMM32.21)
 */
BOOL WINAPI ImmGetCandidateWindow(HIMC hIMC, DWORD dwBufLen, LPCANDIDATEFORM lpCandidate)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetCompositionFont32A (IMM32.22)
 */
BOOL WINAPI ImmGetCompositionFontA(HIMC hIMC, LPLOGFONTA lplf)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetCompositionFont32W (IMM32.23)
 */
BOOL WINAPI ImmGetCompositionFontW(HIMC hIMC, LPLOGFONTW lplf)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetCompositionString32A (IMM32.24)
 */
LONG WINAPI ImmGetCompositionStringA(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  switch(GetVersion())
    {
    default:
//      FIXME("%s not supported",GetVersionName());
    case WIN95:
      return 0xffffffff;
    case NT40:
      return 0;
    }
}

/***********************************************************************
 *           ImmGetCompositionString32W (IMM32.25)
 */
LONG WINAPI ImmGetCompositionStringW( HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  switch(GetVersion())
    {
    default:
//      FIXME("%s not supported",GetVersionName());
    case WIN95:
      return 0xffffffff;
    case NT40:
      return 0;
    }
}

/***********************************************************************
 *           ImmGetCompositionWindow32 (IMM32.26)
 */
BOOL WINAPI ImmGetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetContext32 (IMM32.27)
 */
HIMC WINAPI ImmGetContext(HWND hWnd)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}

/***********************************************************************
 *           ImmGetConversionList32A (IMM32.28)
 */
DWORD WINAPI ImmGetConversionListA(  HKL hKL, HIMC hIMC,
  LPCSTR pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetConversionList32W (IMM32.29)
 */
DWORD WINAPI ImmGetConversionListW(  HKL hKL, HIMC hIMC,
  LPCWSTR pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetConversionStatus32 (IMM32.30)
 */
BOOL WINAPI ImmGetConversionStatus(HIMC hIMC, LPDWORD lpfdwConversion, LPDWORD lpfdwSentence)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetDefaultIMEWnd32 (IMM32.31)
 */
HWND WINAPI ImmGetDefaultIMEWnd(HWND hWnd)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HWND)NULL;
}

/***********************************************************************
 *           ImmGetDescription32A (IMM32.32)
 */
UINT WINAPI ImmGetDescriptionA(HKL hKL, LPSTR lpszDescription, UINT uBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetDescription32W (IMM32.33)
 */
UINT WINAPI ImmGetDescriptionW(HKL hKL, LPWSTR lpszDescription, UINT uBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetGuideLine32A (IMM32.34)
 */
DWORD WINAPI ImmGetGuideLineA(
  HIMC hIMC, DWORD dwIndex, LPSTR lpBuf, DWORD dwBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetGuideLine32W (IMM32.35)
 */
DWORD WINAPI ImmGetGuideLineW(HIMC hIMC, DWORD dwIndex, LPWSTR lpBuf, DWORD dwBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetHotKey32 (IMM32.36)
 */
BOOL WINAPI ImmGetHotKey(DWORD dword, LPUINT lpuModifiers, LPUINT lpuVKey, LPHKL lphkl)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMCCLockCount32 (IMM32.37)
 */
DWORD  WINAPI ImmGetIMCCLockCount(HIMCC himcc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMCCSize32 (IMM32.38)
 */
DWORD  WINAPI ImmGetIMCCSize(HIMCC himcc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMCLockCount32 (IMM32.39)
 */
DWORD WINAPI ImmGetIMCLockCount(HIMC himc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMEFileName32A (IMM32.40)
 */
UINT WINAPI ImmGetIMEFileNameA( HKL hKL, LPSTR lpszFileName, UINT uBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetIMEFileName32W (IMM32.41)
 */
UINT WINAPI ImmGetIMEFileNameW( HKL hKL, LPWSTR lpszFileName, UINT uBufLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetImeMenuItems32A (IMM32.42)
 */
DWORD WINAPI ImmGetImeMenuItemsA(HIMC himc, DWORD dword, DWORD dword2, LPIMEMENUITEMINFOA lpimena, LPIMEMENUITEMINFOA lpimena2, DWORD dword3)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmGetImeMenuItems32W (IMM32.43)
 */
DWORD WINAPI ImmGetImeMenuItemsW(HIMC himc, DWORD dword, DWORD dword2, LPIMEMENUITEMINFOW lpimenw, LPIMEMENUITEMINFOW lpimenw2, DWORD dword3)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmGetOpenStatus32 (IMM32.44)
 */
BOOL WINAPI ImmGetOpenStatus(HIMC hIMC)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetProperty32 (IMM32.45)
 */
DWORD WINAPI ImmGetProperty(HKL hKL, DWORD fdwIndex)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetRegisterWordStyle32A (IMM32.46)
 */
UINT WINAPI ImmGetRegisterWordStyleA(HKL hKL, UINT nItem, LPSTYLEBUFA lpStyleBuf)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetRegisterWordStyle32W (IMM32.47)
 */
UINT WINAPI ImmGetRegisterWordStyleW(HKL hKL, UINT nItem, LPSTYLEBUFW lpStyleBuf)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetStatusWindowPos32 (IMM32.48)
 */
BOOL WINAPI ImmGetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetVirtualKey32 (IMM32.49)
 */
UINT WINAPI ImmGetVirtualKey(HWND hWnd)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  switch(GetVersion())
    {
    default:
//      FIXME("%s not supported", GetVersionName());
    case WIN95:
      return VK_PROCESSKEY;
    case NT40:
      return 0;
    }
}

/***********************************************************************
 *           ImmInstallIME32A (IMM32.50)
 */
HKL WINAPI ImmInstallIMEA(LPCSTR lpszIMEFileName, LPCSTR lpszLayoutText)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HKL)NULL;
}

/***********************************************************************
 *           ImmInstallIME32W (IMM32.51)
 */
HKL WINAPI ImmInstallIMEW(LPCWSTR lpszIMEFileName, LPCWSTR lpszLayoutText)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HKL)NULL;
}

/***********************************************************************
 *           ImmIsIME32 (IMM32.52)
 */
BOOL WINAPI ImmIsIME(HKL hKL)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmIsUIMessage32A (IMM32.53)
 */
BOOL WINAPI ImmIsUIMessageA(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmIsUIMessage32W (IMM32.54)
 */
BOOL WINAPI ImmIsUIMessageW(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmLockIMC32	 (IMM32.55)
 */
LPINPUTCONTEXT WINAPI ImmLockIMC(HIMC himc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmLockIMCC32	 (IMM32.56)
 */
LPVOID WINAPI ImmLockIMCC(HIMCC himcc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmNotifyIME32 (IMM32.57)
 */
BOOL WINAPI ImmNotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmReSizeIMCC32 (IMM32.58)
 */
HIMCC  WINAPI ImmReSizeIMCC(HIMCC himcc, DWORD dword)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmRegisterWord32A (IMM32.59)
 */
BOOL WINAPI ImmRegisterWordA( HKL hKL, LPCSTR lpszReading, DWORD dwStyle, LPCSTR lpszRegister)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmRegisterWord32W (IMM32.60)
 */
BOOL WINAPI ImmRegisterWordW(HKL hKL, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszRegister)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmReleaseContext32 (IMM32.61)
 */
BOOL WINAPI ImmReleaseContext(HWND hWnd, HIMC hIMC)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmRequestMessage32A (IMM32.62)
 */
LRESULT WINAPI ImmRequestMessageA(HIMC himc, WPARAM wparam, LPARAM lparam)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmRequestMessage32W (IMM32.63)
 */
LRESULT WINAPI ImmRequestMessageW(HIMC himc, WPARAM wparam, LPARAM lparam)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSetCandidateWindow32 (IMM32.64)
 */
BOOL WINAPI ImmSetCandidateWindow(HIMC hIMC, LPCANDIDATEFORM lpCandidate)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionFont32A (IMM32.65)
 */
BOOL WINAPI ImmSetCompositionFontA(HIMC hIMC, LPLOGFONTA lplf)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionFont32W (IMM32.66)
 */
BOOL WINAPI ImmSetCompositionFontW(HIMC hIMC, LPLOGFONTW lplf)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionString32A (IMM32.67)
 */
BOOL WINAPI ImmSetCompositionStringA(  HIMC hIMC, DWORD dwIndex, 
  LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionString32W (IMM32.68)
 */
BOOL WINAPI ImmSetCompositionStringW(HIMC hIMC, DWORD dwIndex,
	LPCVOID lpComp, DWORD dwCompLen,LPCVOID lpRead, DWORD dwReadLen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionWindow32 (IMM32.69)
 */
BOOL WINAPI ImmSetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetConversionStatus32 (IMM32.70)
 */
BOOL WINAPI ImmSetConversionStatus(HIMC hIMC, DWORD fdwConversion, DWORD fdwSentence)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSetHotKey32 (IMM32.71)
 */
BOOL WINAPI ImmSetHotKey(DWORD dword, UINT uint, UINT uint2, HKL hkl)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSetOpenStatus32 (IMM32.72)
 */
BOOL WINAPI ImmSetOpenStatus(HIMC hIMC, BOOL fOpen)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetStatusWindowPos32 (IMM32.73)
 */
BOOL WINAPI ImmSetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmShowSoftKeyboard (IMM32.74)
 */
BOOL WINAPI ImmShowSoftKeyboard(HWND hwnd, int in1)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSimulateHotKey32 (IMM32.75)
 */
BOOL WINAPI ImmSimulateHotKey(HWND hWnd, DWORD dwHotKeyID)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmUnlockIMC32 (IMM32.76)
 */
BOOL  WINAPI ImmUnlockIMC(HIMC himc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmUnlockIMCC32 (IMM32.77)
 */
BOOL  WINAPI ImmUnlockIMCC(HIMCC himcc)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmUnregisterWord32A (IMM32.78)
 */
BOOL WINAPI ImmUnregisterWordA(HKL hKL, LPCSTR lpszReading, DWORD dwStyle, LPCSTR lpszUnregister)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmUnregisterWord32W (IMM32.79)
 */
BOOL WINAPI ImmUnregisterWordW(HKL hKL, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszUnregister)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
