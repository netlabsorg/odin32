/* $Id: comdlg32.cpp,v 1.2 1999-05-31 22:08:08 phaller Exp $ */

/*
 * COMDLG32 implementation
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <stdarg.h>

#include <os2win.h>
#include <misc.h>
#include "wndproc.h"

//******************************************************************************
//TODO:template names in structures!!!!
//******************************************************************************
BOOL WIN32API GetSaveFileNameA(LPOPENFILENAMEA lpofn)
{
 Win32WindowProc *wndproc;

  if(lpofn->Flags & OFN_ENABLEHOOK) {
    wndproc = CreateWindowProc(lpofn->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpofn->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("GetSaveFileName\n");
#endif
  return(O32_GetSaveFileName(lpofn));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetOpenFileNameA(LPOPENFILENAMEA lpofn)
{
 Win32WindowProc *wndproc;

  if(lpofn->Flags & OFN_ENABLEHOOK) {
    wndproc = CreateWindowProc(lpofn->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpofn->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("GetOpenFileNameA\n");
#endif
  return O32_GetOpenFileName(lpofn);
}
//******************************************************************************
//******************************************************************************
INT16 WIN32API GetFileTitleA(LPCSTR lpFile, LPSTR lpTitle, UINT cbBuf)
{
#ifdef DEBUG
    WriteLog("GetFileTitle\n");
#endif
    return O32_GetFileTitle(lpFile, lpTitle, cbBuf);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ChooseColorA(LPCHOOSECOLORA lpcc)
{
 Win32WindowProc *wndproc;

  if(lpcc->Flags & CC_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpcc->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpcc->lpfnHook = (LPCCHOOKPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
    WriteLog("OS2ChooseColorA");
#endif
    return O32_ChooseColor(lpcc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ChooseColorW(LPCHOOSECOLORW lpcc)
{
 Win32WindowProc *wndproc;

  if(lpcc->Flags & CC_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpcc->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpcc->lpfnHook = (LPCCHOOKPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2ChooseColorW");
#endif
  // NOTE: This will not work as is (needs UNICODE support)
//  return O32_ChooseColor(lpcc);
  return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ChooseFontA(LPCHOOSEFONTA lpcf)
{
 Win32WindowProc *wndproc;

  if(lpcf->Flags & CF_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpcf->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpcf->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2ChooseFontA");
#endif
  return O32_ChooseFont(lpcf);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ChooseFontW(LPCHOOSEFONTW lpcf)
{
 Win32WindowProc *wndproc;

  if(lpcf->Flags & CF_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpcf->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpcf->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2ChooseFontW");
#endif
  // NOTE: This will not work as is (needs UNICODE support)
//  return O32_ChooseFont(lpcf);
  return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API CommDlgExtendedError(void)
{
#ifdef DEBUG
    WriteLog("OS2CommDlgExtendedError");
#endif
    return O32_CommDlgExtendedError();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindTextA(LPFINDREPLACEA lpfr)
{
 Win32WindowProc *wndproc;

  if(lpfr->Flags & FR_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpfr->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpfr->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2FindTextA");
#endif
  return O32_FindText(lpfr);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindTextW(LPFINDREPLACEW lpfr)
{
 Win32WindowProc *wndproc;

  if(lpfr->Flags & FR_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpfr->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpfr->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2FindTextW");
#endif
  // NOTE: This will not work as is (needs UNICODE support)
//  return O32_FindText(lpfr);
  return 0;
}
//******************************************************************************
//******************************************************************************
INT16 WIN32API GetFileTitleW(LPCWSTR lpFile, LPWSTR lpTitle, UINT cbBuf)
{
#ifdef DEBUG
    WriteLog("OS2GetFileTitleW");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_GetFileTitle(arg1, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetOpenFileNameW(LPOPENFILENAMEW lpofn)
{
 Win32WindowProc *wndproc;

  if(lpofn->Flags & OFN_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpofn->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpofn->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2GetOpenFileNameW");
#endif
  // NOTE: This will not work as is (needs UNICODE support)
//  return O32_GetOpenFileName(lpofn);
  return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetSaveFileNameW(LPOPENFILENAMEW lpofn)
{
 Win32WindowProc *wndproc;

  if(lpofn->Flags & OFN_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpofn->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpofn->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2GetSaveFileNameW");
#endif
  // NOTE: This will not work as is (needs UNICODE support)
//  return O32_GetSaveFileName((LPOPENFILENAMEA)lpofn);
  return 0;
}
//******************************************************************************
//todo: hooks
//******************************************************************************
BOOL WIN32API PrintDlgA(LPPRINTDLGA lppd)
{
#ifdef DEBUG
  WriteLog("OS2PrintDlgA");
#endif
  return O32_PrintDlg(lppd);
}
//******************************************************************************
//todo: hooks
//******************************************************************************
BOOL WIN32API PrintDlgW( LPPRINTDLGW arg1)
{
#ifdef DEBUG
    WriteLog("OS2PrintDlgW");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
//    return PrintDlg(arg1);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API ReplaceTextA(LPFINDREPLACEA lpfr)
{
 Win32WindowProc *wndproc;

  if(lpfr->Flags & FR_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpfr->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpfr->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2ReplaceTextA");
#endif
  return O32_ReplaceText(lpfr);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API ReplaceTextW(LPFINDREPLACEW lpfr)
{
 Win32WindowProc *wndproc;

  if(lpfr->Flags & FR_ENABLEHOOK) {
    wndproc = CreateWindowProc((WNDPROC)lpfr->lpfnHook);
    if(wndproc == NULL)
        return(FALSE);

    lpfr->lpfnHook = (WNDPROC)Win32WindowProc::GetOS2Callback();
  }
#ifdef DEBUG
  WriteLog("OS2ReplaceTextW");
#endif
  // NOTE: This will not work as is (needs UNICODE support)
//  return O32_ReplaceText(lpfr);
  return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PageSetupDlgA(LPPAGESETUPDLGA lppsd)
{
#ifdef DEBUG
  WriteLog("OS2PageSetupDlgA not implemented");
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PageSetupDlgW(LPPAGESETUPDLGW lppsd)
{
#ifdef DEBUG
  WriteLog("OS2PageSetupDlgW not implemented");
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
