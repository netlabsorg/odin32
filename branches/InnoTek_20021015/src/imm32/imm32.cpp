/* $Id: imm32.cpp,v 1.4 1999-11-12 11:38:39 sandervl Exp $ */
/*
 *	IMM32 library
 *
 *	Copyright 1998	Patrik Stridvall
 *	Copyright 1999	Jens Wiessner
 */

#include <os2win.h>
#include <odinwrap.h>
#include <imm.h>
#include <winversion.h>
#include "imm32.h"

ODINDEBUGCHANNEL(imm)

/***********************************************************************
 *           ImmAssociateContext (IMM32.1)
 */
HIMC WINAPI ImmAssociateContext(HWND hWnd, HIMC hIMC)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmAssociateContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmAssociateContextEx (IMM32.2)
 */
BOOL WINAPI ImmAssociateContextEx(HWND hWnd, HIMC hIMC, DWORD dword)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmAssociateContextEx not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmConfigureIMEA (IMM32.3)
 */
BOOL WINAPI ImmConfigureIMEA(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmConfigureIMEA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/************** *********************************************************
 *           ImmConfigureIMEW (IMM32.4)
 */
BOOL WINAPI ImmConfigureIMEW(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmConfigureIMEW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmCreateContext (IMM32.5)
 */
HIMC WINAPI ImmCreateContext()
{
#ifdef DEBUG
  dprintf(("IMM32: ImmCreateContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmCreateIMCC (IMM32.6)
 */
HIMCC  WINAPI ImmCreateIMCC(DWORD dword)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmCreateIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmCreateSoftKeyboard (IMM32.7)
 */
HWND WINAPI ImmCreateSoftKeyboard(UINT uint, HWND hwnd, int in1, int in2)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmCreateSoftKeyboard not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}


/***********************************************************************
 *           ImmDestroyContext (IMM32.8)
 */
BOOL WINAPI ImmDestroyContext(HIMC hIMC)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmDestroyContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmDestroyIMCC (IMM32.9)
 */
HIMCC  WINAPI ImmDestroyIMCC(HIMCC himcc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmDestroyIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmDestroySoftKeyboard (IMM32.10)
 */
BOOL WINAPI ImmDestroySoftKeyboard(HWND hwnd)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmDestroySoftKeyboard not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           IMMDisableIME (IMM32.11)
 */
BOOL WINAPI ImmDisableIME(DWORD dword)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmDisableIME not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmEnumRegisterWordA (IMM32.12)
 */
UINT WINAPI ImmEnumRegisterWordA( HKL hKL, REGISTERWORDENUMPROCA lpfnEnumProc,
  LPCSTR lpszReading, DWORD dwStyle,  LPCSTR lpszRegister, LPVOID lpData)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmEnumRegisterWordA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmEnumRegisterWordW (IMM32.13)
 */
UINT WINAPI ImmEnumRegisterWordW(  HKL hKL, REGISTERWORDENUMPROCW lpfnEnumProc,
  LPCWSTR lpszReading, DWORD dwStyle,  LPCWSTR lpszRegister, LPVOID lpData)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmEnumRegisterWordW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmEscapeA (IMM32.14)
 */
LRESULT WINAPI ImmEscapeA(  HKL hKL, HIMC hIMC,   UINT uEscape, LPVOID lpData)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmEscapeA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmEscapeW (IMM32.15)
 */
LRESULT WINAPI ImmEscapeW(  HKL hKL, HIMC hIMC,  UINT uEscape, LPVOID lpData)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmEscapeW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGenerateMessage (IMM32.16)
 */
BOOL WINAPI ImmGenerateMessage(HIMC himc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGenerateMessage not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetCandidateListA (IMM32.17)
 */
DWORD WINAPI ImmGetCandidateListA(  HIMC hIMC, DWORD deIndex,   
		LPCANDIDATELIST lpCandList, DWORD dwBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateListCountA (IMM32.18)
 */
DWORD WINAPI ImmGetCandidateListCountA( HIMC hIMC, LPDWORD lpdwListCount)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListCountA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateListCountW (IMM32.19)
 */
DWORD WINAPI ImmGetCandidateListCountW( HIMC hIMC, LPDWORD lpdwListCount)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListCountW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateListW (IMM32.20)
 */
DWORD WINAPI ImmGetCandidateListW(  HIMC hIMC, DWORD deIndex, 
  LPCANDIDATELIST lpCandList, DWORD dwBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetCandidateWindow (IMM32.21)
 */
BOOL WINAPI ImmGetCandidateWindow(HIMC hIMC, DWORD dwBufLen, LPCANDIDATEFORM lpCandidate)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetCompositionFontA (IMM32.22)
 */
BOOL WINAPI ImmGetCompositionFontA(HIMC hIMC, LPLOGFONTA lplf)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionFontA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetCompositionFontW (IMM32.23)
 */
BOOL WINAPI ImmGetCompositionFontW(HIMC hIMC, LPLOGFONTW lplf)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionFontW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetCompositionStringA (IMM32.24)
 */
LONG WINAPI ImmGetCompositionStringA(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionStringA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  switch(GetVersion())
    {
    default:
        dprintf(("OS not supported"));
    case WIN95:
      return 0xffffffff;
    case NT40:
      return 0;
    }
}

/***********************************************************************
 *           ImmGetCompositionStringW (IMM32.25)
 */
LONG WINAPI ImmGetCompositionStringW( HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionStringW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  switch(GetVersion())
    {
    default:
      dprintf(("OS not supported"));
    case WIN95:
      return 0xffffffff;
    case NT40:
      return 0;
    }
}

/***********************************************************************
 *           ImmGetCompositionWindow (IMM32.26)
 */
BOOL WINAPI ImmGetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetContext (IMM32.27)
 */
HIMC WINAPI ImmGetContext(HWND hWnd)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
}

/***********************************************************************
 *           ImmGetConversionListA (IMM32.28)
 */
DWORD WINAPI ImmGetConversionListA(  HKL hKL, HIMC hIMC,
  LPCSTR pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetConversionListA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetConversionListW (IMM32.29)
 */
DWORD WINAPI ImmGetConversionListW(  HKL hKL, HIMC hIMC,
  LPCWSTR pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetConversionListW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetConversionStatus (IMM32.30)
 */
BOOL WINAPI ImmGetConversionStatus(HIMC hIMC, LPDWORD lpfdwConversion, LPDWORD lpfdwSentence)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetConversionStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetDefaultIMEWnd (IMM32.31)
 */
HWND WINAPI ImmGetDefaultIMEWnd(HWND hWnd)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetDefaultIMEWnd not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HWND)NULL;
}

/***********************************************************************
 *           ImmGetDescriptionA (IMM32.32)
 */
UINT WINAPI ImmGetDescriptionA(HKL hKL, LPSTR lpszDescription, UINT uBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetDescriptionA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetDescriptionW (IMM32.33)
 */
UINT WINAPI ImmGetDescriptionW(HKL hKL, LPWSTR lpszDescription, UINT uBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetDescriptionW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetGuideLineA (IMM32.34)
 */
DWORD WINAPI ImmGetGuideLineA(
  HIMC hIMC, DWORD dwIndex, LPSTR lpBuf, DWORD dwBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetGuideLineA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetGuideLineW (IMM32.35)
 */
DWORD WINAPI ImmGetGuideLineW(HIMC hIMC, DWORD dwIndex, LPWSTR lpBuf, DWORD dwBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetGuideLineW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetHotKey (IMM32.36)
 */
BOOL WINAPI ImmGetHotKey(DWORD dword, LPUINT lpuModifiers, LPUINT lpuVKey, LPHKL lphkl)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetHotKey not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMCCLockCount (IMM32.37)
 */
DWORD  WINAPI ImmGetIMCCLockCount(HIMCC himcc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMCCLockCount not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMCCSize (IMM32.38)
 */
DWORD  WINAPI ImmGetIMCCSize(HIMCC himcc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMCCSize not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMCLockCount (IMM32.39)
 */
DWORD WINAPI ImmGetIMCLockCount(HIMC himc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMCLockCount not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetIMEFileNameA (IMM32.40)
 */
UINT WINAPI ImmGetIMEFileNameA( HKL hKL, LPSTR lpszFileName, UINT uBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMEFileNameA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetIMEFileNameW (IMM32.41)
 */
UINT WINAPI ImmGetIMEFileNameW( HKL hKL, LPWSTR lpszFileName, UINT uBufLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMEFileNameW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImmGetImeMenuItemsA (IMM32.42)
 */
DWORD WINAPI ImmGetImeMenuItemsA(HIMC himc, DWORD dword, DWORD dword2, LPIMEMENUITEMINFOA lpimena, LPIMEMENUITEMINFOA lpimena2, DWORD dword3)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetImeMenuItemsA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmGetImeMenuItemsW (IMM32.43)
 */
DWORD WINAPI ImmGetImeMenuItemsW(HIMC himc, DWORD dword, DWORD dword2, LPIMEMENUITEMINFOW lpimenw, LPIMEMENUITEMINFOW lpimenw2, DWORD dword3)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetImeMenuItemsW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmGetOpenStatus (IMM32.44)
 */
BOOL WINAPI ImmGetOpenStatus(HIMC hIMC)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetOpenStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetProperty (IMM32.45)
 */
DWORD WINAPI ImmGetProperty(HKL hKL, DWORD fdwIndex)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetProperty not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetRegisterWordStyleA (IMM32.46)
 */
UINT WINAPI ImmGetRegisterWordStyleA(HKL hKL, UINT nItem, LPSTYLEBUFA lpStyleBuf)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetRegisterWordStyleA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetRegisterWordStyleW (IMM32.47)
 */
UINT WINAPI ImmGetRegisterWordStyleW(HKL hKL, UINT nItem, LPSTYLEBUFW lpStyleBuf)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetRegisterWordStyleW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImmGetStatusWindowPos (IMM32.48)
 */
BOOL WINAPI ImmGetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetStatusWindowPos not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmGetVirtualKey (IMM32.49)
 */
UINT WINAPI ImmGetVirtualKey(HWND hWnd)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmGetVirtualKey not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  switch(GetVersion())
    {
    default:
      dprintf(("OS not supported"));
    case WIN95:
      return VK_PROCESSKEY;
    case NT40:
      return 0;
    }
}

/***********************************************************************
 *           ImmInstallIMEA (IMM32.50)
 */
HKL WINAPI ImmInstallIMEA(LPCSTR lpszIMEFileName, LPCSTR lpszLayoutText)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmInstallIMEA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HKL)NULL;
}

/***********************************************************************
 *           ImmInstallIMEW (IMM32.51)
 */
HKL WINAPI ImmInstallIMEW(LPCWSTR lpszIMEFileName, LPCWSTR lpszLayoutText)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmInstallIMEW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HKL)NULL;
}

/***********************************************************************
 *           ImmIsIME (IMM32.52)
 */
BOOL WINAPI ImmIsIME(HKL hKL)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmIsIME not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmIsUIMessageA (IMM32.53)
 */
BOOL WINAPI ImmIsUIMessageA(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmIsUIMessageA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmIsUIMessageW (IMM32.54)
 */
BOOL WINAPI ImmIsUIMessageW(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmIsUIMessageW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmLockIMC		 (IMM32.55)
 */
LPINPUTCONTEXT WINAPI ImmLockIMC(HIMC himc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmLockIMC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmLockIMCC	 (IMM32.56)
 */
LPVOID WINAPI ImmLockIMCC(HIMCC himcc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmLockIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmNotifyIME (IMM32.57)
 */
BOOL WINAPI ImmNotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmNotifyIME not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmReSizeIMCC (IMM32.58)
 */
HIMCC  WINAPI ImmReSizeIMCC(HIMCC himcc, DWORD dword)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmReSizeIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmRegisterWordA (IMM32.59)
 */
BOOL WINAPI ImmRegisterWordA( HKL hKL, LPCSTR lpszReading, DWORD dwStyle, LPCSTR lpszRegister)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmRegisterWordA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmRegisterWordW (IMM32.60)
 */
BOOL WINAPI ImmRegisterWordW(HKL hKL, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszRegister)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmRegisterWordW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmReleaseContext (IMM32.61)
 */
BOOL WINAPI ImmReleaseContext(HWND hWnd, HIMC hIMC)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmReleaseContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmRequestMessageA (IMM32.62)
 */
LRESULT WINAPI ImmRequestMessageA(HIMC himc, WPARAM wparam, LPARAM lparam)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmRequestMessageA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmRequestMessageW (IMM32.63)
 */
LRESULT WINAPI ImmRequestMessageW(HIMC himc, WPARAM wparam, LPARAM lparam)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmRequestMessageW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSetCandidateWindow (IMM32.64)
 */
BOOL WINAPI ImmSetCandidateWindow(HIMC hIMC, LPCANDIDATEFORM lpCandidate)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCandidateWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionFontA (IMM32.65)
 */
BOOL WINAPI ImmSetCompositionFontA(HIMC hIMC, LPLOGFONTA lplf)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionFontA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionFontW (IMM32.66)
 */
BOOL WINAPI ImmSetCompositionFontW(HIMC hIMC, LPLOGFONTW lplf)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionFontW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionStringA (IMM32.67)
 */
BOOL WINAPI ImmSetCompositionStringA(  HIMC hIMC, DWORD dwIndex, 
  LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionStringA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionStringW (IMM32.68)
 */
BOOL WINAPI ImmSetCompositionStringW(HIMC hIMC, DWORD dwIndex,
	LPCVOID lpComp, DWORD dwCompLen,LPCVOID lpRead, DWORD dwReadLen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionStringW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetCompositionWindow (IMM32.69)
 */
BOOL WINAPI ImmSetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetConversionStatus (IMM32.70)
 */
BOOL WINAPI ImmSetConversionStatus(HIMC hIMC, DWORD fdwConversion, DWORD fdwSentence)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetConversionStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSetHotKey (IMM32.71)
 */
BOOL WINAPI ImmSetHotKey(DWORD dword, UINT uint, UINT uint2, HKL hkl)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetHotKey not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSetOpenStatus (IMM32.72)
 */
BOOL WINAPI ImmSetOpenStatus(HIMC hIMC, BOOL fOpen)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetOpenStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmSetStatusWindowPos (IMM32.73)
 */
BOOL WINAPI ImmSetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSetStatusWindowPos not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmShowSoftKeyboard (IMM32.74)
 */
BOOL WINAPI ImmShowSoftKeyboard(HWND hwnd, int in1)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmShowSoftKeyboard not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmSimulateHotKey (IMM32.75)
 */
BOOL WINAPI ImmSimulateHotKey(HWND hWnd, DWORD dwHotKeyID)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmSimulateHotKey not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmUnlockIMC (IMM32.76)
 */
BOOL  WINAPI ImmUnlockIMC(HIMC himc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmUnlockIMC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmUnlockIMCC (IMM32.77)
 */
BOOL  WINAPI ImmUnlockIMCC(HIMCC himcc)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmUnlockIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           ImmUnregisterWordA (IMM32.78)
 */
BOOL WINAPI ImmUnregisterWordA(HKL hKL, LPCSTR lpszReading, DWORD dwStyle, LPCSTR lpszUnregister)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmUnregisterWordA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImmUnregisterWordW (IMM32.79)
 */
BOOL WINAPI ImmUnregisterWordW(HKL hKL, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszUnregister)
{
#ifdef DEBUG
  dprintf(("IMM32: ImmUnregisterWordW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
