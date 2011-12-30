/* $Id: imm32.cpp,v 1.5 2004-05-24 08:51:00 sandervl Exp $ */
/*
 *  IMM32 library
 *
 *  Copyright 1998  Patrik Stridvall
 *  Copyright 1999  Jens Wiessner
 */

#include <os2win.h>
#include <odinwrap.h>
#include <imm.h>
#include <winversion.h>
#include "imm32.h"
#include "im32.h"

ODINDEBUGCHANNEL(imm)

/***********************************************************************
 *           ImmAssociateContext (IMM32.1)
 */
HIMC WINAPI ImmAssociateContext(HWND hWnd, HIMC hIMC)
{
#ifdef __WIN32OS2__
    return IM32AssociateContext( hWnd, hIMC );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmAssociateContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
#endif
}


/***********************************************************************
 *           ImmAssociateContextEx (IMM32.2)
 */
BOOL WINAPI ImmAssociateContextEx(HWND hWnd, HIMC hIMC, DWORD dword)
{
#ifdef __WIN32OS2__
    return IM32AssociateContextEx( hWnd, hIMC, dword );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmAssociateContextEx not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
#endif
}


/***********************************************************************
 *           ImmConfigureIMEA (IMM32.3)
 */
BOOL WINAPI ImmConfigureIMEA(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
#ifdef __WIN32OS2__
    return IM32ConfigureIME( hKL, hWnd, dwMode, lpData, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmConfigureIMEA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/************** *********************************************************
 *           ImmConfigureIMEW (IMM32.4)
 */
BOOL WINAPI ImmConfigureIMEW(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
#ifdef __WIN32OS2__
    return IM32ConfigureIME( hKL, hWnd, dwMode, lpData, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmConfigureIMEW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmCreateContext (IMM32.5)
 */
HIMC WINAPI ImmCreateContext()
{
#ifdef __WIN32OS2__
    return IM32CreateContext();
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmCreateContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
#endif
}


/***********************************************************************
 *           ImmCreateIMCC (IMM32.6)
 */
HIMCC  WINAPI ImmCreateIMCC(DWORD dword)
{
#ifdef __WIN32OS2__
    return IM32CreateIMCC( dword );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmCreateIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
#endif
}


/***********************************************************************
 *           ImmCreateSoftKeyboard (IMM32.7)
 */
HWND WINAPI ImmCreateSoftKeyboard(UINT uint, HWND hwnd, int in1, int in2)
{
#ifdef __WIN32OS2__
    return IM32CreateSoftKeyboard( uint, hwnd, in1, in2 );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmCreateSoftKeyboard not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
#endif
}


/***********************************************************************
 *           ImmDestroyContext (IMM32.8)
 */
BOOL WINAPI ImmDestroyContext(HIMC hIMC)
{
#ifdef __WIN32OS2__
    return IM32DestroyContext( hIMC );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmDestroyContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmDestroyIMCC (IMM32.9)
 */
HIMCC  WINAPI ImmDestroyIMCC(HIMCC himcc)
{
#ifdef __WIN32OS2__
    return IM32DestroyIMCC( himcc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmDestroyIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmDestroySoftKeyboard (IMM32.10)
 */
BOOL WINAPI ImmDestroySoftKeyboard(HWND hwnd)
{
#ifdef __WIN32OS2__
    return IM32DestroySoftKeyboard( hwnd );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmDestroySoftKeyboard not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           IMMDisableIME (IMM32.11)
 */
BOOL WINAPI ImmDisableIME(DWORD dword)
{
#ifdef __WIN32OS2__
    return IM32DisableIME( dword );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmDisableIME not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmEnumRegisterWordA (IMM32.12)
 */
UINT WINAPI ImmEnumRegisterWordA( HKL hKL, REGISTERWORDENUMPROCA lpfnEnumProc,
  LPCSTR lpszReading, DWORD dwStyle,  LPCSTR lpszRegister, LPVOID lpData)
{
#ifdef __WIN32OS2__
    return IM32EnumRegisterWord( hKL, ( LPVOID )lpfnEnumProc, ( LPVOID )lpszReading, dwStyle, ( LPVOID )lpszRegister, lpData, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmEnumRegisterWordA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmEnumRegisterWordW (IMM32.13)
 */
UINT WINAPI ImmEnumRegisterWordW(  HKL hKL, REGISTERWORDENUMPROCW lpfnEnumProc,
  LPCWSTR lpszReading, DWORD dwStyle,  LPCWSTR lpszRegister, LPVOID lpData)
{
#ifdef __WIN32OS2__
    return IM32EnumRegisterWord( hKL, ( LPVOID )lpfnEnumProc, ( LPVOID )lpszReading, dwStyle, ( LPVOID )lpszRegister, lpData, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmEnumRegisterWordW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmEscapeA (IMM32.14)
 */
LRESULT WINAPI ImmEscapeA(  HKL hKL, HIMC hIMC,   UINT uEscape, LPVOID lpData)
{
#ifdef __WIN32OS2__
    return IM32Escape( hKL, hIMC, uEscape, lpData, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmEscapeA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmEscapeW (IMM32.15)
 */
LRESULT WINAPI ImmEscapeW(  HKL hKL, HIMC hIMC,  UINT uEscape, LPVOID lpData)
{
#ifdef __WIN32OS2__
    return IM32Escape( hKL, hIMC, uEscape, lpData, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmEscapeW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGenerateMessage (IMM32.16)
 */
BOOL WINAPI ImmGenerateMessage(HIMC himc)
{
#ifdef __WIN32OS2__
    return IM32GenerateMessage( himc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGenerateMessage not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGetCandidateListA (IMM32.17)
 */
DWORD WINAPI ImmGetCandidateListA(  HIMC hIMC, DWORD deIndex,
        LPCANDIDATELIST lpCandList, DWORD dwBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetCandidateList( hIMC, deIndex, lpCandList, dwBufLen, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetCandidateListCountA (IMM32.18)
 */
DWORD WINAPI ImmGetCandidateListCountA( HIMC hIMC, LPDWORD lpdwListCount)
{
#ifdef __WIN32OS2__
    return IM32GetCandidateListCount( hIMC, lpdwListCount, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListCountA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetCandidateListCountW (IMM32.19)
 */
DWORD WINAPI ImmGetCandidateListCountW( HIMC hIMC, LPDWORD lpdwListCount)
{
#ifdef __WIN32OS2__
    return IM32GetCandidateListCount( hIMC, lpdwListCount, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListCountW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetCandidateListW (IMM32.20)
 */
DWORD WINAPI ImmGetCandidateListW(  HIMC hIMC, DWORD deIndex,
  LPCANDIDATELIST lpCandList, DWORD dwBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetCandidateList( hIMC, deIndex, lpCandList, dwBufLen, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateListW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetCandidateWindow (IMM32.21)
 */
BOOL WINAPI ImmGetCandidateWindow(HIMC hIMC, DWORD dwBufLen, LPCANDIDATEFORM lpCandidate)
{
#ifdef __WIN32OS2__
    return IM32GetCandidateWindow( hIMC, dwBufLen, lpCandidate );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCandidateWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmGetCompositionFontA (IMM32.22)
 */
BOOL WINAPI ImmGetCompositionFontA(HIMC hIMC, LPLOGFONTA lplf)
{
#ifdef __WIN32OS2__
    return IM32GetCompositionFont( hIMC, ( LPVOID )lplf, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionFontA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmGetCompositionFontW (IMM32.23)
 */
BOOL WINAPI ImmGetCompositionFontW(HIMC hIMC, LPLOGFONTW lplf)
{
#ifdef __WIN32OS2__
    return IM32GetCompositionFont( hIMC, ( LPVOID )lplf, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionFontW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmGetCompositionStringA (IMM32.24)
 */
LONG WINAPI ImmGetCompositionStringA(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetCompositionString( hIMC, dwIndex, lpBuf, dwBufLen, FALSE );
#else
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
#endif
}

/***********************************************************************
 *           ImmGetCompositionStringW (IMM32.25)
 */
LONG WINAPI ImmGetCompositionStringW( HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetCompositionString( hIMC, dwIndex, lpBuf, dwBufLen, TRUE );
#else
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
#endif
}

/***********************************************************************
 *           ImmGetCompositionWindow (IMM32.26)
 */
BOOL WINAPI ImmGetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
#ifdef __WIN32OS2__
    return IM32GetCompositionWindow( hIMC, lpCompForm );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetCompositionWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetContext (IMM32.27)
 */
HIMC WINAPI ImmGetContext(HWND hWnd)
{
#ifdef __WIN32OS2__
    return IM32GetContext( hWnd );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HIMC)NULL;
#endif
}

/***********************************************************************
 *           ImmGetConversionListA (IMM32.28)
 */
DWORD WINAPI ImmGetConversionListA(  HKL hKL, HIMC hIMC,
  LPCSTR pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag)
{
#ifdef __WIN32OS2__
    return IM32GetConversionList( hKL, hIMC, ( LPVOID )pSrc, lpDst, dwBufLen, uFlag, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetConversionListA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetConversionListW (IMM32.29)
 */
DWORD WINAPI ImmGetConversionListW(  HKL hKL, HIMC hIMC,
  LPCWSTR pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag)
{
#ifdef __WIN32OS2__
    return IM32GetConversionList( hKL, hIMC, ( LPVOID )pSrc, lpDst, dwBufLen, uFlag, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetConversionListW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetConversionStatus (IMM32.30)
 */
BOOL WINAPI ImmGetConversionStatus(HIMC hIMC, LPDWORD lpfdwConversion, LPDWORD lpfdwSentence)
{
#ifdef __WIN32OS2__
    return IM32GetConversionStatus( hIMC, lpfdwConversion, lpfdwSentence );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetConversionStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmGetDefaultIMEWnd (IMM32.31)
 */
HWND WINAPI ImmGetDefaultIMEWnd(HWND hWnd)
{
#ifdef __WIN32OS2__
    return IM32GetDefaultIMEWnd( hWnd );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetDefaultIMEWnd not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HWND)NULL;
#endif
}

/***********************************************************************
 *           ImmGetDescriptionA (IMM32.32)
 */
UINT WINAPI ImmGetDescriptionA(HKL hKL, LPSTR lpszDescription, UINT uBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetDescription( hKL, lpszDescription, uBufLen, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetDescriptionA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetDescriptionW (IMM32.33)
 */
UINT WINAPI ImmGetDescriptionW(HKL hKL, LPWSTR lpszDescription, UINT uBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetDescription( hKL, lpszDescription, uBufLen, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetDescriptionW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetGuideLineA (IMM32.34)
 */
DWORD WINAPI ImmGetGuideLineA(
  HIMC hIMC, DWORD dwIndex, LPSTR lpBuf, DWORD dwBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetGuideLine( hIMC, dwIndex, lpBuf, dwBufLen, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetGuideLineA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetGuideLineW (IMM32.35)
 */
DWORD WINAPI ImmGetGuideLineW(HIMC hIMC, DWORD dwIndex, LPWSTR lpBuf, DWORD dwBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetGuideLine( hIMC, dwIndex, lpBuf, dwBufLen, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetGuideLineW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGetHotKey (IMM32.36)
 */
BOOL WINAPI ImmGetHotKey(DWORD dword, LPUINT lpuModifiers, LPUINT lpuVKey, LPHKL lphkl)
{
#ifdef __WIN32OS2__
    return IM32GetHotKey( dword, lpuModifiers, lpuVKey, lphkl );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetHotKey not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGetIMCCLockCount (IMM32.37)
 */
DWORD  WINAPI ImmGetIMCCLockCount(HIMCC himcc)
{
#ifdef __WIN32OS2__
    return IM32GetIMCCLockCount( himcc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMCCLockCount not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGetIMCCSize (IMM32.38)
 */
DWORD  WINAPI ImmGetIMCCSize(HIMCC himcc)
{
#ifdef __WIN32OS2__
    return IM32GetIMCCSize( himcc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMCCSize not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGetIMCLockCount (IMM32.39)
 */
DWORD WINAPI ImmGetIMCLockCount(HIMC himc)
{
#ifdef __WIN32OS2__
    return IM32GetIMCLockCount( himc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMCLockCount not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGetIMEFileNameA (IMM32.40)
 */
UINT WINAPI ImmGetIMEFileNameA( HKL hKL, LPSTR lpszFileName, UINT uBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetIMEFileName( hKL, lpszFileName, uBufLen, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMEFileNameA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetIMEFileNameW (IMM32.41)
 */
UINT WINAPI ImmGetIMEFileNameW( HKL hKL, LPWSTR lpszFileName, UINT uBufLen)
{
#ifdef __WIN32OS2__
    return IM32GetIMEFileName( hKL, lpszFileName, uBufLen, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetIMEFileNameW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}


/***********************************************************************
 *           ImmGetImeMenuItemsA (IMM32.42)
 */
DWORD WINAPI ImmGetImeMenuItemsA(HIMC himc, DWORD dword, DWORD dword2, LPIMEMENUITEMINFOA lpimena, LPIMEMENUITEMINFOA lpimena2, DWORD dword3)
{
#ifdef __WIN32OS2__
    return IM32GetImeMenuItems( himc, dword, dword2, lpimena, lpimena2, dword3, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetImeMenuItemsA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmGetImeMenuItemsW (IMM32.43)
 */
DWORD WINAPI ImmGetImeMenuItemsW(HIMC himc, DWORD dword, DWORD dword2, LPIMEMENUITEMINFOW lpimenw, LPIMEMENUITEMINFOW lpimenw2, DWORD dword3)
{
#ifdef __WIN32OS2__
    return IM32GetImeMenuItems( himc, dword, dword2, ( LPVOID )lpimenw, ( LPVOID )lpimenw2, dword3, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetImeMenuItemsW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmGetOpenStatus (IMM32.44)
 */
BOOL WINAPI ImmGetOpenStatus(HIMC hIMC)
{
#ifdef __WIN32OS2__
    return IM32GetOpenStatus( hIMC );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetOpenStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmGetProperty (IMM32.45)
 */
DWORD WINAPI ImmGetProperty(HKL hKL, DWORD fdwIndex)
{
#ifdef __WIN32OS2__
    return IM32GetProperty( hKL, fdwIndex );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetProperty not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetRegisterWordStyleA (IMM32.46)
 */
UINT WINAPI ImmGetRegisterWordStyleA(HKL hKL, UINT nItem, LPSTYLEBUFA lpStyleBuf)
{
#ifdef __WIN32OS2__
    return IM32GetRegisterWordStyle( hKL, nItem, lpStyleBuf, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetRegisterWordStyleA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetRegisterWordStyleW (IMM32.47)
 */
UINT WINAPI ImmGetRegisterWordStyleW(HKL hKL, UINT nItem, LPSTYLEBUFW lpStyleBuf)
{
#ifdef __WIN32OS2__
    return IM32GetRegisterWordStyle( hKL, nItem, lpStyleBuf, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetRegisterWordStyleW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
#endif
}

/***********************************************************************
 *           ImmGetStatusWindowPos (IMM32.48)
 */
BOOL WINAPI ImmGetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
#ifdef __WIN32OS2__
    return IM32GetStatusWindowPos( hIMC, lpptPos );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmGetStatusWindowPos not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmGetVirtualKey (IMM32.49)
 */
UINT WINAPI ImmGetVirtualKey(HWND hWnd)
{
#ifdef __WIN32OS2__
    return IM32GetVirtualKey( hWnd );
#else
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
#endif
}

/***********************************************************************
 *           ImmInstallIMEA (IMM32.50)
 */
HKL WINAPI ImmInstallIMEA(LPCSTR lpszIMEFileName, LPCSTR lpszLayoutText)
{
#ifdef __WIN32OS2__
    return IM32InstallIME(( LPVOID )lpszIMEFileName, ( LPVOID )lpszLayoutText, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmInstallIMEA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HKL)NULL;
#endif
}

/***********************************************************************
 *           ImmInstallIMEW (IMM32.51)
 */
HKL WINAPI ImmInstallIMEW(LPCWSTR lpszIMEFileName, LPCWSTR lpszLayoutText)
{
#ifdef __WIN32OS2__
    return IM32InstallIME( ( LPVOID )lpszIMEFileName, ( LPVOID )lpszLayoutText, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmInstallIMEW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HKL)NULL;
#endif
}

/***********************************************************************
 *           ImmIsIME (IMM32.52)
 */
BOOL WINAPI ImmIsIME(HKL hKL)
{
#ifdef __WIN32OS2__
    return IM32IsIME( hKL );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmIsIME not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmIsUIMessageA (IMM32.53)
 */
BOOL WINAPI ImmIsUIMessageA(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef __WIN32OS2__
    return IM32IsUIMessage( hWndIME, msg, wParam, lParam, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmIsUIMessageA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmIsUIMessageW (IMM32.54)
 */
BOOL WINAPI ImmIsUIMessageW(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef __WIN32OS2__
    return IM32IsUIMessage( hWndIME, msg, wParam, lParam, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmIsUIMessageW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmLockIMC      (IMM32.55)
 */
LPINPUTCONTEXT WINAPI ImmLockIMC(HIMC himc)
{
#ifdef __WIN32OS2__
    return IM32LockIMC( himc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmLockIMC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmLockIMCC     (IMM32.56)
 */
LPVOID WINAPI ImmLockIMCC(HIMCC himcc)
{
#ifdef __WIN32OS2__
    return IM32LockIMCC( himcc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmLockIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmNotifyIME (IMM32.57)
 */
BOOL WINAPI ImmNotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
#ifdef __WIN32OS2__
    return IM32NotifyIME( hIMC, dwAction, dwIndex, dwValue );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmNotifyIME not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmReSizeIMCC (IMM32.58)
 */
HIMCC  WINAPI ImmReSizeIMCC(HIMCC himcc, DWORD dword)
{
#ifdef __WIN32OS2__
    return IM32ReSizeIMCC( himcc, dword );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmReSizeIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmRegisterWordA (IMM32.59)
 */
BOOL WINAPI ImmRegisterWordA( HKL hKL, LPCSTR lpszReading, DWORD dwStyle, LPCSTR lpszRegister)
{
#ifdef __WIN32OS2__
    return IM32RegisterWord( hKL, ( LPVOID )lpszReading, dwStyle, ( LPVOID )lpszRegister, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmRegisterWordA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmRegisterWordW (IMM32.60)
 */
BOOL WINAPI ImmRegisterWordW(HKL hKL, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszRegister)
{
#ifdef __WIN32OS2__
    return IM32RegisterWord( hKL, ( LPVOID )lpszReading, dwStyle, ( LPVOID )lpszRegister, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmRegisterWordW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmReleaseContext (IMM32.61)
 */
BOOL WINAPI ImmReleaseContext(HWND hWnd, HIMC hIMC)
{
#ifdef __WIN32OS2__
    return IM32ReleaseContext( hWnd, hIMC );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmReleaseContext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmRequestMessageA (IMM32.62)
 */
LRESULT WINAPI ImmRequestMessageA(HIMC himc, WPARAM wparam, LPARAM lparam)
{
#ifdef __WIN32OS2__
    return IM32RequestMessage( himc, wparam, lparam, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmRequestMessageA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmRequestMessageW (IMM32.63)
 */
LRESULT WINAPI ImmRequestMessageW(HIMC himc, WPARAM wparam, LPARAM lparam)
{
#ifdef __WIN32OS2__
    return IM32RequestMessage( himc, wparam, lparam, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmRequestMessageW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmSetCandidateWindow (IMM32.64)
 */
BOOL WINAPI ImmSetCandidateWindow(HIMC hIMC, LPCANDIDATEFORM lpCandidate)
{
#ifdef __WIN32OS2__
    return IM32SetCandidateWindow( hIMC, lpCandidate );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCandidateWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmSetCompositionFontA (IMM32.65)
 */
BOOL WINAPI ImmSetCompositionFontA(HIMC hIMC, LPLOGFONTA lplf)
{
#ifdef __WIN32OS2__
    return IM32SetCompositionFont( hIMC, lplf, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionFontA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmSetCompositionFontW (IMM32.66)
 */
BOOL WINAPI ImmSetCompositionFontW(HIMC hIMC, LPLOGFONTW lplf)
{
#ifdef __WIN32OS2__
    return IM32SetCompositionFont( hIMC, lplf, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionFontW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmSetCompositionStringA (IMM32.67)
 */
BOOL WINAPI ImmSetCompositionStringA(  HIMC hIMC, DWORD dwIndex,
  LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen)
{
#ifdef __WIN32OS2__
    return IM32SetCompositionString( hIMC, dwIndex, lpComp, dwCompLen, lpRead, dwReadLen, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionStringA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmSetCompositionStringW (IMM32.68)
 */
BOOL WINAPI ImmSetCompositionStringW(HIMC hIMC, DWORD dwIndex,
    LPCVOID lpComp, DWORD dwCompLen,LPCVOID lpRead, DWORD dwReadLen)
{
#ifdef __WIN32OS2__
    return IM32SetCompositionString( hIMC, dwIndex, lpComp, dwCompLen, lpRead, dwReadLen, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionStringW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmSetCompositionWindow (IMM32.69)
 */
BOOL WINAPI ImmSetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
#ifdef __WIN32OS2__
    return IM32SetCompositionWindow( hIMC, lpCompForm );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetCompositionWindow not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmSetConversionStatus (IMM32.70)
 */
BOOL WINAPI ImmSetConversionStatus(HIMC hIMC, DWORD fdwConversion, DWORD fdwSentence)
{
#ifdef __WIN32OS2__
    return IM32SetConversionStatus( hIMC, fdwConversion, fdwSentence );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetConversionStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmSetHotKey (IMM32.71)
 */
BOOL WINAPI ImmSetHotKey(DWORD dword, UINT uint, UINT uint2, HKL hkl)
{
#ifdef __WIN32OS2__
    return IM32SetHotKey( dword, uint, uint2, hkl );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetHotKey not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmSetOpenStatus (IMM32.72)
 */
BOOL WINAPI ImmSetOpenStatus(HIMC hIMC, BOOL fOpen)
{
#ifdef __WIN32OS2__
    return IM32SetOpenStatus( hIMC, fOpen );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetOpenStatus not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmSetStatusWindowPos (IMM32.73)
 */
BOOL WINAPI ImmSetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
#ifdef __WIN32OS2__
    return IM32SetStatusWindowPos( hIMC, lpptPos );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSetStatusWindowPos not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmShowSoftKeyboard (IMM32.74)
 */
BOOL WINAPI ImmShowSoftKeyboard(HWND hwnd, int in1)
{
#ifdef __WIN32OS2__
    return IM32ShowSoftKeyboard( hwnd, in1 );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmShowSoftKeyboard not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmSimulateHotKey (IMM32.75)
 */
BOOL WINAPI ImmSimulateHotKey(HWND hWnd, DWORD dwHotKeyID)
{
#ifdef __WIN32OS2__
    return IM32SimulateHotKey( hWnd, dwHotKeyID );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmSimulateHotKey not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmUnlockIMC (IMM32.76)
 */
BOOL  WINAPI ImmUnlockIMC(HIMC himc)
{
#ifdef __WIN32OS2__
    return IM32UnlockIMC( himc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmUnlockIMC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmUnlockIMCC (IMM32.77)
 */
BOOL  WINAPI ImmUnlockIMCC(HIMCC himcc)
{
#ifdef __WIN32OS2__
    return IM32UnlockIMCC( himcc );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmUnlockIMCC not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}


/***********************************************************************
 *           ImmUnregisterWordA (IMM32.78)
 */
BOOL WINAPI ImmUnregisterWordA(HKL hKL, LPCSTR lpszReading, DWORD dwStyle, LPCSTR lpszUnregister)
{
#ifdef __WIN32OS2__
    return IM32UnregisterWord( hKL, ( LPVOID )lpszReading, dwStyle, ( LPVOID )lpszUnregister, FALSE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmUnregisterWordA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}

/***********************************************************************
 *           ImmUnregisterWordW (IMM32.79)
 */
BOOL WINAPI ImmUnregisterWordW(HKL hKL, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszUnregister)
{
#ifdef __WIN32OS2__
    return IM32UnregisterWord( hKL, ( LPVOID )lpszReading, dwStyle, ( LPVOID )lpszUnregister, TRUE );
#else
#ifdef DEBUG
  dprintf(("IMM32: ImmUnregisterWordW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
#endif
}
