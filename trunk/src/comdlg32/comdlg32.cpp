/* $Id: comdlg32.cpp,v 1.7 1999-06-29 15:54:10 cbratschi Exp $ */

/*
 * COMDLG32 implementation
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <wndproc.h>


#define COMDLG32_CHECKHOOK(a,b,c)           \
  if(a->Flags & b)                          \
  {                                         \
    wndproc = CreateWindowProc((WNDPROC)a->lpfnHook);\
    if(wndproc == NULL)                     \
      return(FALSE);                        \
                                            \
    a->lpfnHook = (c)Win32WindowProc::GetOS2Callback();\
 }

#define COMDLG32_CHECKHOOK2(a,b,c,d)        \
  if(a->Flags & b)                          \
  {                                         \
    wndproc = CreateWindowProc((WNDPROC)a->d);\
    if(wndproc == NULL)                     \
      return(FALSE);                        \
                                            \
    a->d = (c)Win32WindowProc::GetOS2Callback();\
 }


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetSaveFileNameA(LPOPENFILENAMEA lpofn)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: GetSaveFileNameA(%08xh)\n",
           lpofn));

  COMDLG32_CHECKHOOK(lpofn, OFN_ENABLEHOOK, WNDPROC)

  return(O32_GetSaveFileName(lpofn));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetOpenFileNameA(LPOPENFILENAMEA lpofn)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: GetOpenFileNameA(%08xh)\n",
           lpofn));

  COMDLG32_CHECKHOOK(lpofn, OFN_ENABLEHOOK, WNDPROC)

  return(O32_GetOpenFileName(lpofn));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

INT16 WIN32API GetFileTitleA(LPCSTR lpFile,
                             LPSTR  lpTitle,
                             UINT    cbBuf)
{
  dprintf(("COMDLG32: GetFileTitleA(%s,%08xh,%08xh)\n",
           lpFile,
           lpTitle,
           cbBuf));

  return O32_GetFileTitle(lpFile,
                          lpTitle,
                          cbBuf);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ChooseColorA(LPCHOOSECOLORA lpcc)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: ChooseColorA(%08xh)\n",
           lpcc));

  COMDLG32_CHECKHOOK(lpcc, CC_ENABLEHOOK, LPCCHOOKPROC)

  return O32_ChooseColor(lpcc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : casting is a little hot, should work however
 *             assuming lpcc->lpTemplateName is READONLY pointer!
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ChooseColorW(LPCHOOSECOLORW lpcc)
{
  Win32WindowProc *wndproc;
  BOOL            bResult;
  LPCWSTR         lpTemplateNameBackup = lpcc->lpTemplateName;

  dprintf(("COMDLG32: ChooseColorW(%08xh).\n",
           lpcc));

  // if no template is to convert, we can take this shortcut
  if (!(lpcc->Flags & CC_ENABLETEMPLATE))
    return O32_ChooseColor( (LPCHOOSECOLORA)lpcc );


  // convert to ASCII string
  if (lpcc->lpTemplateName != NULL)
    lpcc->lpTemplateName = (LPCWSTR)UnicodeToAsciiString((WCHAR*)lpcc->lpTemplateName);

  COMDLG32_CHECKHOOK(lpcc, CC_ENABLEHOOK, LPCCHOOKPROC)

  bResult = O32_ChooseColor((LPCHOOSECOLORA)lpcc);  // call ASCII version

  // free temporary ASCII string and restore UNICODE string
  if (lpcc->lpTemplateName != NULL)
  {
    FreeAsciiString((char*)lpcc->lpTemplateName);
    lpcc->lpTemplateName = lpTemplateNameBackup;
  }

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ChooseFontA(LPCHOOSEFONTA lpcf)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: ChooseFontA(%08xh).\n",
           lpcf));

  COMDLG32_CHECKHOOK(lpcf, CF_ENABLEHOOK, WNDPROC)

  return O32_ChooseFont(lpcf);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown yet, what is INPUT and what is OUTPUT string
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *             Edgar Buerkle  [Mon, 1999/06/28 19:35]
 *****************************************************************************/

BOOL WIN32API ChooseFontW(LPCHOOSEFONTW lpcf)
{
  Win32WindowProc *wndproc;
  BOOL            bResult;
  CHOOSEFONTA     asciicf;
  LOGFONTA        asciilf;
  char            szAsciiStyle[64];

  // NOTE: LOGFONTW/A is NOT converted !
  dprintf(("COMDLG32: ChooseFontW(%08xh) not correctly implemented.\n",
           lpcf));

  // convert to ASCII string
  memcpy(&asciicf,     // make binary copy of CHOOSEFONTW
         lpcf,         // to save the flags
         sizeof(asciicf));

  memcpy (&asciilf,
          lpcf->lpLogFont,
          sizeof(LOGFONTA));

  asciicf.lpLogFont = &asciilf; // update pointer

  // lpTemplatenName bug in open32 ?  This doesn't work.
  // TODO: CF_ENABLETEMPLATEHANDLE
  if (lpcf->Flags & CF_ENABLETEMPLATE)
    if((int)asciicf.lpTemplateName >> 16 != 0)
      asciicf.lpTemplateName = UnicodeToAsciiString((LPWSTR)lpcf->lpTemplateName);

  UnicodeToAsciiN(lpcf->lpszStyle,
                  szAsciiStyle,
                  sizeof(szAsciiStyle) - 1);
  asciicf.lpszStyle = szAsciiStyle;

  UnicodeToAsciiN(lpcf->lpLogFont->lfFaceName,
                  asciilf.lfFaceName,
                  LF_FACESIZE-1);

  // LPCFHOOKPROC != WNDPROC ?
  COMDLG32_CHECKHOOK(lpcf, CF_ENABLEHOOK, WNDPROC)

  // switch strings
  bResult = O32_ChooseFont((LPCHOOSEFONTA)&asciicf);        // call ASCII version

  // transfer BACK resulting strings !!!
  AsciiToUnicodeN(asciicf.lpLogFont->lfFaceName,
                  lpcf->lpLogFont->lfFaceName,
                  LF_FACESIZE-1);

  AsciiToUnicode(asciicf.lpszStyle,
                 lpcf->lpszStyle);

  if (lpcf->Flags & CF_ENABLETEMPLATE)
    if((int)asciicf.lpTemplateName >> 16 != 0)
      FreeAsciiString((char*)asciicf.lpTemplateName);

  // copy back fields
  lpcf->Flags = asciicf.Flags;

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD WIN32API CommDlgExtendedError(void)
{
  dprintf(("COMDLG32: ComDlgExtendedError()\n"));

  return O32_CommDlgExtendedError();
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

HWND WIN32API FindTextA(LPFINDREPLACEA lpfr)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: FindTextA(%08xh).\n",
           lpfr));

  COMDLG32_CHECKHOOK(lpfr, FR_ENABLEHOOK, WNDPROC)

  return O32_FindText(lpfr);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

HWND WIN32API FindTextW(LPFINDREPLACEW lpfr)
{
  Win32WindowProc *wndproc;
  BOOL            bResult;
  FINDREPLACEA    fr;

  dprintf(("COMDLG32: FindTextW(%08xh)\n",
           lpfr));

  memcpy(&fr,          // make binary copy first to save all the fields
         lpfr,
         sizeof(fr));

    // convert to ASCII string
  if ((lpfr->Flags & FR_ENABLETEMPLATE) &&
      (lpfr->lpTemplateName != NULL))
    fr.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpfr->lpTemplateName);
  else
    fr.lpTemplateName = NULL;

  if (lpfr->lpstrFindWhat != NULL)
    fr.lpstrFindWhat = UnicodeToAsciiString((WCHAR*)lpfr->lpstrFindWhat);

  if (lpfr->lpstrReplaceWith != NULL)
    fr.lpstrReplaceWith = UnicodeToAsciiString((WCHAR*)lpfr->lpstrReplaceWith);


  COMDLG32_CHECKHOOK(lpfr, FR_ENABLEHOOK, WNDPROC)

  bResult = O32_FindText(&fr);                         // call ASCII version

  // @@@PH: Note -- we might have to transfer BACK resulting strings !!!
  // free temporary ASCII string and restore UNICODE string
  if (fr.lpTemplateName != NULL)
    FreeAsciiString((char*)fr.lpTemplateName);

  // free temporary ASCII string and restore UNICODE string
  if (fr.lpstrFindWhat != NULL)
  {
    AsciiToUnicode(fr.lpstrFindWhat,
                   lpfr->lpstrFindWhat);

    FreeAsciiString((char*)fr.lpstrFindWhat);
  }

  if (fr.lpstrReplaceWith != NULL)
  {
    AsciiToUnicode(fr.lpstrReplaceWith,
                   lpfr->lpstrReplaceWith);

    FreeAsciiString((char*)fr.lpstrReplaceWith);
  }

  // copy back fields
  lpfr->Flags = fr.Flags;

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

INT16 WIN32API GetFileTitleW(LPCWSTR lpFile,
                             LPWSTR  lpTitle,
                             UINT    cbBuf)
{
  LPSTR lpFileBackup;
  char  szTitle[256];
  INT16 iResult;

  dprintf(("COMDLG32: GetFileTitleW(%08xh,%08xh,%08xh).\n",
           lpFile,
           lpTitle,
           cbBuf));

  lpFileBackup = UnicodeToAsciiString((LPWSTR)lpFile);
  iResult      = O32_GetFileTitle(lpFileBackup,
                                  szTitle,
                                  cbBuf);

  FreeAsciiString(lpFileBackup);

  // transform result into Unicode
  AsciiToUnicode(szTitle,
                 lpTitle);

  return iResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetOpenFileNameW(LPOPENFILENAMEW lpofn)
{
  Win32WindowProc *wndproc;
  OPENFILENAMEA   ofn;
  char*           szFile;
  char*           szFileTitle;
  char*           szCustFilter;
  BOOL            bResult;

  dprintf(("COMDLG32: GetOpenFileNameW(%08xh)\n",
           lpofn));

  memcpy(&ofn,          // make binary copy first to save all the fields
         lpofn,
         sizeof(ofn));

    // convert to ASCII string
  if ((lpofn->Flags && OFN_ENABLETEMPLATE) &&
      (lpofn->lpTemplateName != NULL))
    ofn.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpofn->lpTemplateName);
  else
    ofn.lpTemplateName = NULL;

  if (lpofn->lpstrFilter != NULL)
    ofn.lpstrFilter = UnicodeToAsciiString((WCHAR*)lpofn->lpstrFilter);

  if (lpofn->lpstrInitialDir != NULL)
    ofn.lpstrInitialDir = UnicodeToAsciiString((WCHAR*)lpofn->lpstrInitialDir);

  if (lpofn->lpstrTitle != NULL)
    ofn.lpstrTitle = UnicodeToAsciiString((WCHAR*)lpofn->lpstrTitle);

  if (lpofn->lpstrDefExt != NULL)
    ofn.lpstrDefExt = UnicodeToAsciiString((WCHAR*)lpofn->lpstrDefExt);

  szFile             = (char*)malloc(lpofn->nMaxFile);
  szFile[0]          = 0;

  if (*lpofn->lpstrFile != 0)
    UnicodeToAscii(lpofn->lpstrFile,
                   szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    szFileTitle        = (char*)malloc(lpofn->nMaxFileTitle);
    szFileTitle[0]     = 0;

    if (*lpofn->lpstrFileTitle != 0)
      UnicodeToAscii(lpofn->lpstrFileTitle,
                     szFileTitle);
  }
  else
    szFileTitle = NULL;

  if (lpofn->lpstrCustomFilter != NULL)
  {
    szCustFilter       = (char*)malloc(lpofn->nMaxCustFilter);
    szCustFilter[0]    = 0;


    if (*lpofn->lpstrCustomFilter != 0)
       UnicodeToAscii(lpofn->lpstrCustomFilter,
                      szCustFilter);
  }
  else
    szCustFilter = NULL;

  ofn.lpstrFile         = szFile;
  ofn.lpstrFileTitle    = szFileTitle;
  ofn.lpstrCustomFilter = szCustFilter;

  COMDLG32_CHECKHOOK(lpofn, OFN_ENABLEHOOK, WNDPROC)

  bResult =  O32_GetOpenFileName(&ofn);

  if (ofn.lpTemplateName    != NULL) FreeAsciiString((char*)ofn.lpTemplateName);
  if (ofn.lpstrFilter       != NULL) FreeAsciiString((char*)ofn.lpstrFilter);
  if (ofn.lpstrInitialDir   != NULL) FreeAsciiString((char*)ofn.lpstrInitialDir);
  if (ofn.lpstrTitle        != NULL) FreeAsciiString((char*)ofn.lpstrTitle);
  if (ofn.lpstrDefExt       != NULL) FreeAsciiString((char*)ofn.lpstrDefExt);

  // transform back the result
  AsciiToUnicode(ofn.lpstrFile,
                 lpofn->lpstrFile);
  free(szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    AsciiToUnicode(ofn.lpstrFileTitle,
                   lpofn->lpstrFileTitle);
    free(szFileTitle);
  }

  if (lpofn->lpstrCustomFilter != NULL)
  {
    AsciiToUnicode(ofn.lpstrCustomFilter,
                   lpofn->lpstrCustomFilter);
    free(szCustFilter);
  }

  // copy over some altered flags
  lpofn->nFilterIndex   = ofn.nFilterIndex;
  lpofn->Flags          = ofn.Flags;
  lpofn->nFileOffset    = ofn.nFileOffset;
  lpofn->nFileExtension = ofn.nFileExtension;

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetSaveFileNameW(LPOPENFILENAMEW lpofn)
{
  Win32WindowProc *wndproc;
  OPENFILENAMEA   ofn;
  char*           szFile;
  char*           szFileTitle;
  char*           szCustFilter;
  BOOL            bResult;

  dprintf(("COMDLG32: GetSaveFileNameW(%08xh)\n",
           lpofn));

  memcpy(&ofn,          // make binary copy first to save all the fields
         lpofn,
         sizeof(ofn));

    // convert to ASCII string
  if ((lpofn->Flags && OFN_ENABLETEMPLATE) &&
      (lpofn->lpTemplateName != NULL))
    ofn.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpofn->lpTemplateName);
  else
    ofn.lpTemplateName = NULL;

  if (lpofn->lpstrFilter != NULL)
    ofn.lpstrFilter = UnicodeToAsciiString((WCHAR*)lpofn->lpstrFilter);

  if (lpofn->lpstrInitialDir != NULL)
    ofn.lpstrInitialDir = UnicodeToAsciiString((WCHAR*)lpofn->lpstrInitialDir);

  if (lpofn->lpstrTitle != NULL)
    ofn.lpstrTitle = UnicodeToAsciiString((WCHAR*)lpofn->lpstrTitle);

  if (lpofn->lpstrDefExt != NULL)
    ofn.lpstrDefExt = UnicodeToAsciiString((WCHAR*)lpofn->lpstrDefExt);

  szFile             = (char*)malloc(lpofn->nMaxFile);
  szFile[0]          = 0;

  if (*lpofn->lpstrFile != 0)
    UnicodeToAscii(lpofn->lpstrFile,
                   szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    szFileTitle        = (char*)malloc(lpofn->nMaxFileTitle);
    szFileTitle[0]     = 0;

    if (*lpofn->lpstrFileTitle != 0)
      UnicodeToAscii(lpofn->lpstrFileTitle,
                     szFileTitle);
  }
  else
    szFileTitle = NULL;

  if (lpofn->lpstrCustomFilter != NULL)
  {
    szCustFilter       = (char*)malloc(lpofn->nMaxCustFilter);
    szCustFilter[0]    = 0;


    if (*lpofn->lpstrCustomFilter != 0)
       UnicodeToAscii(lpofn->lpstrCustomFilter,
                      szCustFilter);
  }
  else
    szCustFilter = NULL;

  ofn.lpstrFile         = szFile;
  ofn.lpstrFileTitle    = szFileTitle;
  ofn.lpstrCustomFilter = szCustFilter;

  COMDLG32_CHECKHOOK(lpofn, OFN_ENABLEHOOK, WNDPROC)

  bResult =  O32_GetSaveFileName(&ofn);

  if (ofn.lpTemplateName    != NULL) FreeAsciiString((char*)ofn.lpTemplateName);
  if (ofn.lpstrFilter       != NULL) FreeAsciiString((char*)ofn.lpstrFilter);
  if (ofn.lpstrInitialDir   != NULL) FreeAsciiString((char*)ofn.lpstrInitialDir);
  if (ofn.lpstrTitle        != NULL) FreeAsciiString((char*)ofn.lpstrTitle);
  if (ofn.lpstrDefExt       != NULL) FreeAsciiString((char*)ofn.lpstrDefExt);

  // transform back the result
  AsciiToUnicode(ofn.lpstrFile,
                 lpofn->lpstrFile);
  free(szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    AsciiToUnicode(ofn.lpstrFileTitle,
                   lpofn->lpstrFileTitle);
    free(szFileTitle);
  }

  if (lpofn->lpstrCustomFilter != NULL)
  {
    AsciiToUnicode(ofn.lpstrCustomFilter,
                   lpofn->lpstrCustomFilter);
    free(szCustFilter);
  }

  // copy over some altered flags
  lpofn->nFilterIndex   = ofn.nFilterIndex;
  lpofn->Flags          = ofn.Flags;
  lpofn->nFileOffset    = ofn.nFileOffset;
  lpofn->nFileExtension = ofn.nFileExtension;

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API PrintDlgA(LPPRINTDLGA lppd)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: PrintDlgA(%08xh)\n",
           lppd));

  COMDLG32_CHECKHOOK2(lppd, PD_ENABLEPRINTHOOK, LPPRINTHOOKPROC,lpfnPrintHook)
  COMDLG32_CHECKHOOK2(lppd, PD_ENABLESETUPHOOK, LPSETUPHOOKPROC,lpfnSetupHook)

  return O32_PrintDlg(lppd);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API PrintDlgW(LPPRINTDLGW lppd)
{
  Win32WindowProc *wndproc;

  PRINTDLGA pd;
  BOOL      bResult;

  dprintf(("COMDLG32: PrintDlgW(%08xh)\n",
           lppd));

  memcpy(&pd,          // make binary copy first to save all the fields
         lppd,
         sizeof(pd));

  // convert to ASCII string
  if ((lppd->Flags & PD_ENABLEPRINTTEMPLATE) &&
      (lppd->lpPrintTemplateName != NULL))
    pd.lpPrintTemplateName = UnicodeToAsciiString((WCHAR*)lppd->lpPrintTemplateName);
  else
    pd.lpPrintTemplateName = NULL;

  if ((lppd->Flags & PD_ENABLESETUPTEMPLATE) &&
      (lppd->lpSetupTemplateName != NULL))
    pd.lpSetupTemplateName = UnicodeToAsciiString((WCHAR*)lppd->lpSetupTemplateName);
  else
    pd.lpSetupTemplateName = NULL;

  COMDLG32_CHECKHOOK2(lppd, PD_ENABLEPRINTHOOK, LPPRINTHOOKPROC,lpfnPrintHook)
  COMDLG32_CHECKHOOK2(lppd, PD_ENABLESETUPHOOK, LPSETUPHOOKPROC,lpfnSetupHook)

  bResult = O32_PrintDlg(&pd); // call ASCII API

  if (pd.lpPrintTemplateName != NULL) FreeAsciiString((char*)pd.lpPrintTemplateName);
  if (pd.lpSetupTemplateName != NULL) FreeAsciiString((char*)pd.lpSetupTemplateName);

  // copy back result
  lppd->Flags     = pd.Flags;
  lppd->nFromPage = pd.nFromPage;
  lppd->nToPage   = pd.nToPage;
  lppd->nMinPage  = pd.nMinPage;
  lppd->nMaxPage  = pd.nMaxPage;
  lppd->nCopies   = pd.nCopies;
  //@@@PH: all pass-back fields ?

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

HWND WIN32API ReplaceTextA(LPFINDREPLACEA lpfr)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: ReplaceTextA(%08xh).\n",
           lpfr));

  COMDLG32_CHECKHOOK(lpfr, FR_ENABLEHOOK, WNDPROC)

  return O32_ReplaceText(lpfr);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

HWND WIN32API ReplaceTextW(LPFINDREPLACEW lpfr)
{
  Win32WindowProc *wndproc;
  BOOL            bResult;
  FINDREPLACEA    fr;

  dprintf(("COMDLG32: ReplaceTextW(%08xh)\n",
           lpfr));

  memcpy(&fr,          // make binary copy first to save all the fields
         lpfr,
         sizeof(fr));

    // convert to ASCII string
  if ((lpfr->Flags & FR_ENABLETEMPLATE) &&
      (lpfr->lpTemplateName != NULL))
    fr.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpfr->lpTemplateName);
  else
    fr.lpTemplateName = NULL;

  if (lpfr->lpstrFindWhat != NULL)
    fr.lpstrFindWhat = UnicodeToAsciiString((WCHAR*)lpfr->lpstrFindWhat);

  if (lpfr->lpstrReplaceWith != NULL)
    fr.lpstrReplaceWith = UnicodeToAsciiString((WCHAR*)lpfr->lpstrReplaceWith);


  COMDLG32_CHECKHOOK(lpfr, FR_ENABLEHOOK, WNDPROC)

  bResult = O32_ReplaceText(&fr);                         // call ASCII version

  // @@@PH: Note -- we might have to transfer BACK resulting strings !!!
  // free temporary ASCII string and restore UNICODE string
  if (fr.lpTemplateName != NULL)
    FreeAsciiString((char*)fr.lpTemplateName);

  // free temporary ASCII string and restore UNICODE string
  if (fr.lpstrFindWhat != NULL)
  {
    AsciiToUnicode(fr.lpstrFindWhat,
                   lpfr->lpstrFindWhat);

    FreeAsciiString((char*)fr.lpstrFindWhat);
  }

  if (fr.lpstrReplaceWith != NULL)
  {
    AsciiToUnicode(fr.lpstrReplaceWith,
                   lpfr->lpstrReplaceWith);

    FreeAsciiString((char*)fr.lpstrReplaceWith);
  }

  // copy back fields
  lpfr->Flags = fr.Flags;

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API PageSetupDlgA(LPPAGESETUPDLGA lppsd)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: PageSetupDlgA(%08xh) not implemented.\n",
           lppsd));

  //COMDLG32_CHECKHOOK2(lppsd, PSD_ENABLESETUPHOOK, LPPAGESETUPHOOK, lpfnPageSetupHook)

  return(FALSE);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API PageSetupDlgW(LPPAGESETUPDLGW lppsd)
{
  Win32WindowProc *wndproc;

  dprintf(("COMDLG32: PageSetupDlgW(%08xh) not implemented.\n",
           lppsd));

  //COMDLG32_CHECKHOOK2(lppsd, PSD_ENABLESETUPHOOK, LPPAGESETUPHOOK, lpfnPageSetupHook)

  return(FALSE);
}
